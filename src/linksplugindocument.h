#ifndef _LINKS_PLUGIN_DOCUMENT_H_
#define _LINKS_PLUGIN_DOCUMENT_H_

#include <ktexteditor/document.h>
#include <ktexteditor/searchinterface.h>
#include <ktexteditor/movinginterface.h>
#include <ktexteditor/movingrangefeedback.h>

class LinksFeedback : public KTextEditor::MovingRangeFeedback {
public:
	virtual void caretEnteredRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
		Q_UNUSED(view);

		kDebug() << "entered: " << *range;
	}

	virtual void caretExitedRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
		Q_UNUSED(view);

		kDebug() << "exited: " << *range;
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

class LinksPluginDocument : public QObject {
	Q_OBJECT

public:
	LinksPluginDocument(KTextEditor::Document* document);
	~LinksPluginDocument();

	inline bool isValid() { return m_valid; }
	inline KTextEditor::Document* document() { return m_document; }

public slots:
	void scanDocument(KTextEditor::Document* document);
	void openLink();

private:
	void scanRange(KTextEditor::Range range);

private:
	KTextEditor::Document* m_document;

	KTextEditor::MovingInterface* m_moving;
	KTextEditor::SearchInterface* m_search;

	KTextEditor::Attribute::Ptr m_rangeAttr;

	LinksFeedback m_feedback;
	bool once, m_valid;

public:
    static QString emailPattern;
    static QString urlPattern;
    static QString completePattern;
};

#endif // _LINKS_PLUGIN_DOCUMENT_H_
