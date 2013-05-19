#ifndef _LINKS_PLUGIN_H_
#define _LINKS_PLUGIN_H_

#include <ktexteditor/plugin.h>
#include <ktexteditor/view.h>
#include <kxmlguiclient.h>
#include <klocalizedstring.h>

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QList>

class LinksPlugin : public KTextEditor::Plugin {
	Q_OBJECT

public:
	explicit LinksPlugin(QObject *parent = 0, const QVariantList &args = QVariantList());
	virtual ~LinksPlugin();

	void addView(KTextEditor::View *view);
	void removeView(KTextEditor::View *view);

	void readConfig();
	void writeConfig();

private:
	QList<class LinksPluginView*> m_views;
};

class LinksPluginView : public QObject, public KXMLGUIClient {
	Q_OBJECT

public:
	explicit LinksPluginView(KTextEditor::View *view = 0);
	~LinksPluginView();

private:
	KTextEditor::View *m_view;
};

#endif // _LINKS_PLUGIN_H_
