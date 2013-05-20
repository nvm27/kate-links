#ifndef _LINKS_PLUGIN_H_
#define _LINKS_PLUGIN_H_

#include <ktexteditor/plugin.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <kxmlguiclient.h>
#include <klocalizedstring.h>

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QString>

class LinksPlugin : public KTextEditor::Plugin {
	Q_OBJECT

public:
	explicit LinksPlugin(QObject *parent = 0, const QVariantList &args = QVariantList());
	virtual ~LinksPlugin();

	void addView(KTextEditor::View *view);
	void removeView(KTextEditor::View *view);

private:
	QList<class LinksPluginView*> m_views;
};

class LinksPluginView : public QObject, public KXMLGUIClient {
	Q_OBJECT

public:
	explicit LinksPluginView(KTextEditor::View *view = 0);
	~LinksPluginView();

public slots:
	void scanDocument(KTextEditor::Document* document);

private:
	KTextEditor::View *m_view;

    static QString emailPattern;
    static QString urlPattern;
    static QString completePattern;
};

#endif // _LINKS_PLUGIN_H_
