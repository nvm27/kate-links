#ifndef _LINKS_PLUGIN_DOCUMENT_H_
#define _LINKS_PLUGIN_DOCUMENT_H_

#include <QMenu>

#include <kaction.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/searchinterface.h>
#include <ktexteditor/movinginterface.h>
#include <ktexteditor/movingrangefeedback.h>

class LinksFeedback : public KTextEditor::MovingRangeFeedback {
public:
	virtual ~LinksFeedback() {};

	virtual void caretEnteredRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
		m_ranges[view].insert(range);

		kDebug() << "entered range " << *range;
	}

	virtual void caretExitedRange(KTextEditor::MovingRange* range, KTextEditor::View* view) {
		m_ranges[view].remove(range);

		kDebug() << "exited range " << *range;
	}

	virtual void rangeEmpty(KTextEditor::MovingRange* range) {
		kDebug() << "deleting empty range " << *range;

		clearRanges(range);
		delete range;
	}

	virtual void rangeInvalid(KTextEditor::MovingRange* range) {
		kDebug() << "deleting invalid range " << *range;

		clearRanges(range);
		delete range;
	}

private:
	inline void clearRanges(KTextEditor::MovingRange* range) {
		QMutableHashIterator<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > it(m_ranges);
		while (it.hasNext()) {
			it.next();
			it.value().remove(range);
		}
	}

public:
	QHash<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > m_ranges;
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
	void modifyMenu(KTextEditor::View *view, QMenu* menu);
	void connectViewMenu(KTextEditor::Document* document, KTextEditor::View* view);

private:
	void scanRange(KTextEditor::Range range);

private:
	KTextEditor::Document* m_document;

	KTextEditor::MovingInterface* m_moving;
	KTextEditor::SearchInterface* m_search;

	KTextEditor::Attribute::Ptr m_rangeAttr;

	KAction* m_action;

	LinksFeedback m_feedback;
	bool once, m_valid;

public:
    static QString emailPattern;
    static QString urlPattern;
    static QString completePattern;
};

#endif // _LINKS_PLUGIN_DOCUMENT_H_
