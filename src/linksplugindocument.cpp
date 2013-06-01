#include "linksplugindocument.h"

#include <QClipboard>
#include <QApplication>
#include <QDesktopServices>

#include <klocale.h>

// from Konsole source code
QString LinksPluginDocument::urlPattern("(www\\.(?!\\.)|[a-z][a-z0-9+.-]*://)[^\\s<>'\"]+[^!,\\.\\s<>'\"\\]\\)\\:]");
QString LinksPluginDocument::emailPattern("\\b(\\w|\\.|-)+@(\\w|\\.|-)+\\.\\w+\\b");
QString LinksPluginDocument::completePattern('(' + urlPattern + '|' + emailPattern + ')');

LinksPluginDocument::LinksPluginDocument(KTextEditor::Document* document) : m_document(document), m_rangeAttr(new KTextEditor::Attribute()) {
	m_moving = qobject_cast<KTextEditor::MovingInterface*>(document);
	m_search = qobject_cast<KTextEditor::SearchInterface*>(document);

	m_valid = m_search && m_moving;
	if (!m_valid)
		return;

	KTextEditor::Attribute::Ptr mouseInAttr(new KTextEditor::Attribute());
	mouseInAttr->setFontUnderline(true);
	m_rangeAttr->setDynamicAttribute(KTextEditor::Attribute::ActivateMouseIn, mouseInAttr);

	connect(document, SIGNAL(textChanged(KTextEditor::Document*)), this, SLOT(scanDocument(KTextEditor::Document*)));

	once = false;

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

	// handle views creating
	connect(m_document, SIGNAL(viewCreated(KTextEditor::Document*, KTextEditor::View*)), this, SLOT(handleView(KTextEditor::Document*, KTextEditor::View*)));
}

LinksPluginDocument::~LinksPluginDocument() {}

void LinksPluginDocument::handleView(KTextEditor::Document* document, KTextEditor::View* view) {
	Q_UNUSED(document);

	connect(view, SIGNAL(contextMenuAboutToShow(KTextEditor::View*, QMenu*)), this, SLOT(modifyContextMenu(KTextEditor::View*, QMenu*)));
}


void LinksPluginDocument::scanDocument(KTextEditor::Document* document) {
	if (once)
		return;

	scanRange(document->documentRange());
	once = true;
}

void LinksPluginDocument::scanRange(KTextEditor::Range range) {
	QVector<KTextEditor::Range> found;
	forever {
		found = m_search->searchText(range, completePattern, KTextEditor::Search::Regex | KTextEditor::Search::CaseInsensitive);

		if (found.first().isValid()) {
			KTextEditor::Range& f = found.first();

			kDebug() << "found pattern: ";
			kDebug() << "begin: " << f.start().line() << ": " << f.start().column();
			kDebug() << "end: " << f.end().line() << ": " << f.end().column();

			KTextEditor::MovingRange* mr = m_moving->newMovingRange(f);
			mr->setAttribute(m_rangeAttr);
			mr->setFeedback(&m_feedback);
			mr->setInsertBehaviors(KTextEditor::MovingRange::DoNotExpand);
			mr->setAttributeOnlyForViews(true);

			range.setRange(f.end(), range.end());
		} else
			break;
	}
}

void LinksPluginDocument::modifyContextMenu(KTextEditor::View *view, QMenu* menu) {
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

void LinksPluginDocument::openUrl() {
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

void LinksPluginDocument::copyUrl() {
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

// LinksFeedback class methods
LinksPluginDocument::LinksFeedback::~LinksFeedback() {}

void LinksPluginDocument::LinksFeedback::caretEnteredRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
	m_ranges[view].insert(range);
}

void LinksPluginDocument::LinksFeedback::caretExitedRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
	m_ranges[view].remove(range);
}

void LinksPluginDocument::LinksFeedback::rangeEmpty(KTextEditor::MovingRange* range) {
	clearRanges(range);
	delete range;
}

void LinksPluginDocument::LinksFeedback::rangeInvalid(KTextEditor::MovingRange* range) {
	clearRanges(range);
	delete range;
}

void LinksPluginDocument::LinksFeedback::clearRanges(KTextEditor::MovingRange* range) {
	QMutableHashIterator<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > it(m_ranges);

	while (it.hasNext()) {
		it.next();
		it.value().remove(range);
	}
}

#include "linksplugindocument.moc"
