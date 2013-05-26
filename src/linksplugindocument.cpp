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
			//mr->setView(m_view);
			mr->setAttribute(m_rangeAttr);
			mr->setFeedback(&m_feedback);
			mr->setInsertBehaviors(KTextEditor::MovingRange::DoNotExpand);
			//mr->setZDepth(-90000.0); // Set the z-depth to slightly worse than the selection
			mr->setAttributeOnlyForViews(true);

			range.setRange(f.end(), range.end());
		} else
			break;
	}
}

void LinksPluginDocument:: openLink() {
	kDebug() << "open!";
}

#include "linksplugindocument.moc"
