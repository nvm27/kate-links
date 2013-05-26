#include "linksplugin.h"

#include <ktexteditor/document.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <klocale.h>
#include <kaction.h>
#include <kactioncollection.h>

K_PLUGIN_FACTORY(LinksPluginFactory, registerPlugin<LinksPlugin>();)
K_EXPORT_PLUGIN(LinksPluginFactory("ktexteditor_links", "ktexteditor_plugins"))

LinksPlugin::LinksPlugin(QObject *parent, const QVariantList& args) : KTextEditor::Plugin(parent) {
	Q_UNUSED(args);
}

LinksPlugin::~LinksPlugin() {}

void LinksPlugin::addView(KTextEditor::View *view) {
	LinksPluginView *nview = new LinksPluginView(view);
	m_views.append(nview);
}

void LinksPlugin::removeView(KTextEditor::View *view) {
	for (int z=0; z < m_views.size(); z++) {
		if (m_views.at(z)->parentClient() == view) {
			LinksPluginView *nview = m_views.at(z);
			m_views.removeAll(nview);
			delete nview;
		}
	}
}

LinksPluginView::LinksPluginView(KTextEditor::View *view) : QObject(view), KXMLGUIClient(view), m_view(view), m_rangeAttr(new KTextEditor::Attribute()) {
	setComponentData(LinksPluginFactory::componentData());

	m_docMoving = qobject_cast<KTextEditor::MovingInterface*>(view->document());
	m_docSearch = qobject_cast<KTextEditor::SearchInterface*>(view->document());

	if (!m_docSearch || !m_docMoving) {
		kDebug() << "SearchInterface or MovingInterface not implemented. seriously? don't know what to do now...";

		return;
	}

	KAction* openAction = new KAction("Open URL", view->document());
	connect(openAction, SIGNAL(triggered()), this, SLOT(openURL));

	KTextEditor::Attribute::Ptr mouseInAttr(new KTextEditor::Attribute());
	mouseInAttr->setFontUnderline(true);
	m_rangeAttr->setDynamicAttribute(KTextEditor::Attribute::ActivateMouseIn, mouseInAttr);
	//m_rangeAttr->associateAction(openAction);

	connect(view->document(), SIGNAL(textChanged(KTextEditor::Document*)), this, SLOT(scanDocument(KTextEditor::Document*)));

	once = false;
}

LinksPluginView::~LinksPluginView() {}

// from Konsole source code
QString LinksPluginView::urlPattern("(www\\.(?!\\.)|[a-z][a-z0-9+.-]*://)[^\\s<>'\"]+[^!,\\.\\s<>'\"\\]\\)\\:]");
QString LinksPluginView::emailPattern("\\b(\\w|\\.|-)+@(\\w|\\.|-)+\\.\\w+\\b");
QString LinksPluginView::completePattern('(' + urlPattern + '|' + emailPattern + ')');

void LinksPluginView::scanDocument(KTextEditor::Document* document) {
	if (once)
		return;

	scanRange(document->documentRange());
	once = true;
}

void LinksPluginView::scanRange(KTextEditor::Range range) {
	QVector<KTextEditor::Range> found;
	forever {
		found = m_docSearch->searchText(range, urlPattern, KTextEditor::Search::Regex | KTextEditor::Search::CaseInsensitive);

		if (found.first().isValid()) {
			KTextEditor::Range& f = found.first();

			kDebug() << "found pattern: ";
			kDebug() << "begin: " << f.start().line() << ": " << f.start().column();
			kDebug() << "end: " << f.end().line() << ": " << f.end().column();

			KTextEditor::MovingRange* mr = m_docMoving->newMovingRange(f);
			mr->setView(m_view);
			mr->setAttribute(m_rangeAttr);
			mr->setFeedback(&m_feedback);
			mr->setInsertBehaviors(KTextEditor::MovingRange::ExpandRight);
			//mr->setZDepth(-90000.0); // Set the z-depth to slightly worse than the selection
			mr->setAttributeOnlyForViews(true);

			range.setRange(f.end(), range.end());
		} else
			break;
	}
}

void LinksPluginView:: openURL() {
	kDebug() << "open!";
}

#include "linksplugin.moc"
