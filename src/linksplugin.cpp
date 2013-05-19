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
	m_views.append (nview);
}

void LinksPlugin::removeView(KTextEditor::View *view) {
	for (int z=0; z < m_views.size(); z++) {
		if (m_views.at(z)->parentClient() == view) {
			TimeDatePluginView *nview = m_views.at(z);
			m_views.removeAll(nview);
			delete nview;
		}
	}
}

void readConfig() {}
void writeConfig() {}

LinksPluginView::LinksPluginView(KTextEditor::View *view) : QObject(view), KXMLGUIClient(view), m_view(view) {
	setComponentData(LinksPluginFactory::componentData());
}

LinksPluginView::~LinksPluginView() {}

#include "linksplugin.moc"
