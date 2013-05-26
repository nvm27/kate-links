#include "linksplugindocument.h"

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

	m_action = new KAction("Open URL", document);
	connect(m_action, SIGNAL(triggered()), this, SLOT(openLink()));
	connect(m_document, SIGNAL(viewCreated(KTextEditor::Document*, KTextEditor::View*)), this, SLOT(connectViewMenu(KTextEditor::Document*, KTextEditor::View*)));
}

LinksPluginDocument::~LinksPluginDocument() {}

void LinksPluginDocument::scanDocument(KTextEditor::Document* document) {
	if (once)
		return;

	scanRange(document->documentRange());
	once = true;
}

void LinksPluginDocument::scanRange(KTextEditor::Range range) {
	QVector<KTextEditor::Range> found;
	forever {
		found = m_search->searchText(range, urlPattern, KTextEditor::Search::Regex | KTextEditor::Search::CaseInsensitive);

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

void LinksPluginDocument::openLink() {
	QHashIterator<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > it(m_feedback.m_ranges);
	while (it.hasNext()) {
		it.next();

		foreach (KTextEditor::MovingRange* mr, it.value()) {
			QString link = m_document->text(mr->toRange());
			kDebug() << "opening link: " << link << " " << *mr;
		}
	}
}

void LinksPluginDocument::modifyMenu(KTextEditor::View *view, QMenu* menu) {
	if (!m_feedback.m_ranges[view].isEmpty())
		menu->addAction(m_action);
}

void LinksPluginDocument::connectViewMenu(KTextEditor::Document* document, KTextEditor::View* view) {
	Q_UNUSED(document);
	kDebug() << "view created!";

	connect(view, SIGNAL(contextMenuAboutToShow(KTextEditor::View*, QMenu*)), this, SLOT(modifyMenu(KTextEditor::View*, QMenu*)));
}

#include "linksplugindocument.moc"
