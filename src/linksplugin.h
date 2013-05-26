#ifndef _LINKS_PLUGIN_H_
#define _LINKS_PLUGIN_H_

#include <ktexteditor/plugin.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/searchinterface.h>
#include <ktexteditor/movinginterface.h>
#include <ktexteditor/movingrangefeedback.h>
#include <kxmlguiclient.h>
#include <klocalizedstring.h>

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QString>

class LinksFeedback : public KTextEditor::MovingRangeFeedback {
public:
	virtual void caretEnteredRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
		Q_UNUSED(view);

		kDebug() << "entered: " << *range;
	}
	virtual void rangeEmpty(KTextEditor::MovingRange* range) {
		kDebug() << "empty: " << *range;

		delete range;
	}
	virtual void rangeInvalid(KTextEditor::MovingRange* range) {
		kDebug() << "invalid: " << *range;

		delete range;
	}

	virtual ~LinksFeedback() {};
};

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
	void openURL();

private:
	void scanRange(KTextEditor::Range range);

private:
	KTextEditor::View *m_view;

	KTextEditor::MovingInterface* m_docMoving;
	KTextEditor::SearchInterface* m_docSearch;

	KTextEditor::Attribute::Ptr m_rangeAttr;

	LinksFeedback m_feedback;
	bool once;

    static QString emailPattern;
    static QString urlPattern;
    static QString completePattern;
};

#endif // _LINKS_PLUGIN_H_
