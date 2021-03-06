/*
 * Copyright 2013 Kamil Wojtuch
 *
 * This file is part of kate-links plugin.
 *
 * kate-links is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kate-links is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kate-links.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "linksplugindocument.h"

#include <QClipboard>
#include <QApplication>
#include <QDesktopServices>

#include <klocale.h>

#include <vector>

// from Konsole source code
QString LinksPluginDocument::urlPattern("(www\\.(?!\\.)|[a-z][a-z0-9+.-]*://)[^\\s<>'\"]+[^!,\\.\\s<>'\"\\]\\)\\:]");
QString LinksPluginDocument::emailPattern("\\b(\\w|\\.|-)+@(\\w|\\.|-)+\\.\\w+\\b");
QString LinksPluginDocument::completePattern('(' + urlPattern + '|' + emailPattern + ')');

LinksPluginDocument::LinksPluginDocument(KTextEditor::Document* document) : m_document(document), m_rangeAttr(new KTextEditor::Attribute()), m_feedback(this) {
	m_moving = qobject_cast<KTextEditor::MovingInterface*>(document);
	m_search = qobject_cast<KTextEditor::SearchInterface*>(document);

	m_valid = m_search && m_moving;
	if (!m_valid)
		return;

	KTextEditor::Attribute::Ptr mouseInAttr(new KTextEditor::Attribute());
	mouseInAttr->setFontUnderline(true);
	m_rangeAttr->setDynamicAttribute(KTextEditor::Attribute::ActivateMouseIn, mouseInAttr);

	// locale workaround for now
	KLocale tmpLocale(*KGlobal::locale());
	tmpLocale.insertCatalog("konsole");

	// actions in context menu
	m_separatorAction = new QAction(document);
	m_separatorAction->setSeparator(true);

	m_openAction = new QAction(ki18n("Open Link").toString(&tmpLocale), document);
	connect(m_openAction, SIGNAL(triggered()), this, SLOT(openUrl()));

	m_copyAction = new QAction(ki18n("Copy Link Address").toString(&tmpLocale), document);
	connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copyUrl()));

	connect(document, SIGNAL(textChanged(KTextEditor::Document*)), this, SLOT(documentFirstChange()));

	// handle views creating
	connect(m_document, SIGNAL(viewCreated(KTextEditor::Document*, KTextEditor::View*)), this, SLOT(handleView(KTextEditor::Document*, KTextEditor::View*)));
}

LinksPluginDocument::~LinksPluginDocument() {
	if (!m_valid)
		return;

	delete m_separatorAction;
	delete m_openAction;
	delete m_copyAction;
}

void LinksPluginDocument::handleView(KTextEditor::Document* document, KTextEditor::View* view) {
	Q_UNUSED(document);

	connect(view, SIGNAL(contextMenuAboutToShow(KTextEditor::View*, QMenu*)), this, SLOT(modifyContextMenu(KTextEditor::View*, QMenu*)));
}

void LinksPluginDocument::documentFirstChange() {
	disconnect(m_document, SIGNAL(textChanged(KTextEditor::Document*)), this, SLOT(documentFirstChange()));

	connect(m_document, SIGNAL(textInserted(KTextEditor::Document*, const KTextEditor::Range&)), this, SLOT(documentTextInserted(KTextEditor::Document*, const KTextEditor::Range&)));
	connect(m_document, SIGNAL(textRemoved(KTextEditor::Document*, const KTextEditor::Range&)), this, SLOT(documentTextRemoved(KTextEditor::Document*, const KTextEditor::Range&)));

	connect(m_document, SIGNAL(aboutToClose(KTextEditor::Document*)), this, SLOT(documentAboutToCloseOrReload()));
	connect(m_document, SIGNAL(aboutToReload(KTextEditor::Document*)), this, SLOT(documentAboutToCloseOrReload()));

	scanRange(m_document->documentRange());
}

void LinksPluginDocument::documentAboutToCloseOrReload() {
	m_cursors.clear();
	m_feedback.m_ranges.clear();

	disconnect(m_document, SIGNAL(textInserted(KTextEditor::Document*, const KTextEditor::Range&)), this, SLOT(documentTextInserted(KTextEditor::Document*, const KTextEditor::Range&)));
	disconnect(m_document, SIGNAL(textRemoved(KTextEditor::Document*, const KTextEditor::Range&)), this, SLOT(documentTextRemoved(KTextEditor::Document*, const KTextEditor::Range&)));

	disconnect(m_document, SIGNAL(aboutToClose(KTextEditor::Document*)), this, SLOT(documentAboutToCloseOrReload()));
	disconnect(m_document, SIGNAL(aboutToReload(KTextEditor::Document*)), this, SLOT(documentAboutToCloseOrReload()));

	connect(m_document, SIGNAL(textChanged(KTextEditor::Document*)), this, SLOT(documentFirstChange()));
}

void LinksPluginDocument::documentTextInserted(KTextEditor::Document* document, const KTextEditor::Range& range) {
	Q_UNUSED(document);

	rescanLine(range.start().line());
}

void LinksPluginDocument::documentTextRemoved(KTextEditor::Document* document, const KTextEditor::Range& range) {
	Q_UNUSED(document);

	rescanLine(range.start().line());
}

void LinksPluginDocument::rescanLine(int line) {
	KTextEditor::Cursor begin(line, 0);
	KTextEditor::Cursor end(line, m_document->lineLength(line));

	KTextEditor::MovingCursor* m_begin = m_moving->newMovingCursor(begin);
	KTextEditor::MovingCursor* m_end = m_moving->newMovingCursor(end);

	std::vector<const KTextEditor::MovingCursor*> pointers(m_cursors.lower_bound(m_begin), m_cursors.upper_bound(m_end));

	std::vector<const KTextEditor::MovingCursor*>::iterator it = pointers.begin();
	for (; it != pointers.end(); ++it)
		deleteMovingRange((*it)->range());

	scanRange(KTextEditor::Range(begin, end));

	delete m_begin;
	delete m_end;
}

void LinksPluginDocument::scanRange(KTextEditor::Range range) {
	QVector<KTextEditor::Range> found;
	forever {
		found = m_search->searchText(range, completePattern, KTextEditor::Search::Regex | KTextEditor::Search::CaseInsensitive);

		if (found.first().isValid()) {
			KTextEditor::Range& f = found.first();

			kDebug() << "found pattern: " << f;

			KTextEditor::MovingRange* mr = m_moving->newMovingRange(f);
			mr->setAttribute(m_rangeAttr);
			mr->setFeedback(&m_feedback);
			//mr->setInsertBehaviors(KTextEditor::MovingRange::ExpandLeft | KTextEditor::MovingRange::ExpandRight);
			mr->setEmptyBehavior(KTextEditor::MovingRange::AllowEmpty);
			mr->setAttributeOnlyForViews(true);

			m_cursors.insert(&mr->start());

			range.setRange(f.end(), range.end());
		} else
			break;
	}
}

void LinksPluginDocument::modifyContextMenu(KTextEditor::View *view, QMenu* menu) const {
	QAction* firstAction = menu->actions().first();

	const bool isLink = !m_feedback.m_ranges[view].isEmpty();
	const bool inMenu = (firstAction == m_openAction);

	if (isLink == inMenu)
		return;

	if (isLink && !inMenu) {
		// add action to menu
		menu->insertAction(firstAction, m_separatorAction);
		menu->insertAction(m_separatorAction, m_openAction);
		menu->setDefaultAction(m_openAction);

		if (m_feedback.m_ranges[view].size() == 1)
			menu->insertAction(m_separatorAction, m_copyAction);
	} else {
		// remove action from menu
		menu->removeAction(m_openAction);
		menu->removeAction(m_copyAction);
		menu->removeAction(m_separatorAction);
	}
}

void LinksPluginDocument::openUrl() const {
	QHashIterator<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > it(m_feedback.m_ranges);
	while (it.hasNext()) {
		it.next();

		foreach (KTextEditor::MovingRange* mr, it.value()) {
			QString url = m_document->text(mr->toRange());

			if (url.indexOf(QRegExp(emailPattern, Qt::CaseInsensitive)) == 0)
				url.prepend("mailto:");
			else if (!url.contains("://"))
				url.prepend("http://");

			kDebug() << "opening url: " << url << " " << *mr;
			QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
		}
	}
}

void LinksPluginDocument::copyUrl() const {
	QHashIterator<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > it(m_feedback.m_ranges);
	while (it.hasNext()) {
		it.next();

		// QSet has only one element, but this is simple
		foreach (KTextEditor::MovingRange* mr, it.value()) {
			QString url = m_document->text(mr->toRange());

			kDebug() << "copying url: " << url << " " << *mr;
			QApplication::clipboard()->setText(url, QClipboard::Clipboard);
		}
	}
}

void LinksPluginDocument::deleteMovingRange(KTextEditor::MovingRange* range) {
	kDebug() << "deleting range: " << range;

	QMutableHashIterator<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > it(m_feedback.m_ranges);
	while (it.hasNext()) {
		it.next();
		it.value().remove(range);
	}

	m_cursors.erase(&(range->start()));
	delete range;
}

// LinksFeedback class methods
LinksPluginDocument::LinksFeedback::LinksFeedback(LinksPluginDocument* plugin) : m_plugin(plugin) {}
LinksPluginDocument::LinksFeedback::~LinksFeedback() {}

void LinksPluginDocument::LinksFeedback::caretEnteredRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
	m_ranges[view].insert(range);
}

void LinksPluginDocument::LinksFeedback::caretExitedRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
	m_ranges[view].remove(range);
}

void LinksPluginDocument::LinksFeedback::rangeEmpty(KTextEditor::MovingRange* range) {
	kDebug() << "range becomes empty: " << range;
	m_plugin->deleteMovingRange(range);
}

void LinksPluginDocument::LinksFeedback::rangeInvalid(KTextEditor::MovingRange* range) {
	kDebug() << "range becomes invalid";

	// happens when closing document, but we already have m_cursors and m_rages cleared
	delete range;
}

#include "linksplugindocument.moc"
