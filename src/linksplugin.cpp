#include "linksplugin.h"
#include "linksplugindocument.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>

#include <ktexteditor/document.h>

K_PLUGIN_FACTORY(LinksPluginFactory, registerPlugin<LinksPlugin>();)
K_EXPORT_PLUGIN(LinksPluginFactory("ktexteditor_links", "ktexteditor_plugins"))

LinksPlugin::LinksPlugin(QObject *parent, const QVariantList& args) : KTextEditor::Plugin(parent) {
	Q_UNUSED(args);
}

LinksPlugin::~LinksPlugin() {}

void LinksPlugin::addDocument(KTextEditor::Document *document) {
	LinksPluginDocument* documentPlugin = new LinksPluginDocument(document);

	if (documentPlugin->isValid())
		m_docs.append(documentPlugin);
	else {
		kDebug() << "SearchInterface or MovingInterface not implemented, although needed.";

		delete documentPlugin;
	}
}

void LinksPlugin::removeDocument(KTextEditor::Document *document) {
	for (int z=0; z<m_docs.size(); z++) {
		if (m_docs.at(z)->document() == document) {
			LinksPluginDocument *documentPlugin = m_docs.at(z);
			m_docs.removeAll(documentPlugin);

			delete documentPlugin;

			break;
		}
	}
}

#include "linksplugin.moc"
