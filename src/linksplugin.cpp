#include "linksplugin.h"

#include <ktexteditor/document.h>
#include <ktexteditor/searchinterface.h>
#include <ktexteditor/movinginterface.h>

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
	m_views.append (nview);
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

LinksPluginView::LinksPluginView(KTextEditor::View *view) : QObject(view), KXMLGUIClient(view), m_view(view) {
	setComponentData(LinksPluginFactory::componentData());

	connect(view->document(), SIGNAL(textChanged(KTextEditor::Document*)), this, SLOT(scanDocument(KTextEditor::Document*)));
}

LinksPluginView::~LinksPluginView() {}

// from Konsole source code
QString LinksPluginView::urlPattern("(www\\.(?!\\.)|[a-z][a-z0-9+.-]*://)[^\\s<>'\"]+[^!,\\.\\s<>'\"\\]\\)\\:]");
QString LinksPluginView::emailPattern("\\b(\\w|\\.|-)+@(\\w|\\.|-)+\\.\\w+\\b");
QString LinksPluginView::completePattern('(' + urlPattern + '|' + emailPattern + ')');

void LinksPluginView::scanDocument(KTextEditor::Document* document) {
	KTextEditor::SearchInterface* docSearch = qobject_cast<KTextEditor::SearchInterface*>(document);
	KTextEditor::MovingInterface* docMoving = qobject_cast<KTextEditor::MovingInterface*>(document);

	if (!docSearch || !docMoving) {
		kDebug() << "SearchInterface or MovingInterface not implemented. seriously? don't know what to do now...";

		return;
	}

	KTextEditor::Attribute::Ptr mouseInAttr(new KTextEditor::Attribute());
	mouseInAttr->setFontBold(true);

	KTextEditor::Attribute::Ptr attr(new KTextEditor::Attribute());
	attr->setDynamicAttribute(KTextEditor::Attribute::ActivateMouseIn, mouseInAttr);
	//attr->setBackground(Qt::yellow);
	//attr->setEffects(KTextEditor::Attribute::EffectPulse);

	KTextEditor::Cursor start(0, 0);
	KTextEditor::Range searchRange;

	QVector<KTextEditor::Range> found;
	forever {
		searchRange.setRange(start, document->documentEnd());

		found = docSearch->searchText(searchRange, urlPattern, KTextEditor::Search::Regex | KTextEditor::Search::CaseInsensitive);

		// really, why QVector?
		if (found.first().isValid()) {
			KTextEditor::Range& f = found.first();

			kDebug() << "found pattern: ";
			kDebug() << "begin: " << f.start().line() << ": " << f.start().column();
			kDebug() << "end: " << f.end().line() << ": " << f.end().column();

			KTextEditor::MovingRange* mr = docMoving->newMovingRange(f);
			mr->setAttribute(attr);
			mr->setView(m_view);
			//mr->setZDepth(-90000.0); // Set the z-depth to slightly worse than the selection
			mr->setAttributeOnlyForViews(true);

			start = f.end();
		} else
			break;
	}
}

#include "linksplugin.moc"
