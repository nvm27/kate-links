#ifndef _LINKS_PLUGIN_H_
#define _LINKS_PLUGIN_H_

#include "linksplugindocument.h"

#include <ktexteditor/plugin.h>
#include <ktexteditor/document.h>

#include <QtCore/QList>
#include <QtCore/QString>

class LinksPlugin : public KTextEditor::Plugin {
	Q_OBJECT

public:
	explicit LinksPlugin(QObject *parent = 0, const QVariantList &args = QVariantList());
	virtual ~LinksPlugin();

	void addDocument(KTextEditor::Document *document);
	void removeDocument(KTextEditor::Document *document);

private:
	QList<LinksPluginDocument*> m_docs;
};

#endif // _LINKS_PLUGIN_H_
