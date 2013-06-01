#ifndef _LINKS_PLUGIN_DOCUMENT_H_
#define _LINKS_PLUGIN_DOCUMENT_H_

#include <QMenu>

#include <kaction.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/searchinterface.h>
#include <ktexteditor/movinginterface.h>
#include <ktexteditor/movingrangefeedback.h>


class LinksPluginDocument : public QObject {
	Q_OBJECT

private:
	class LinksFeedback : public KTextEditor::MovingRangeFeedback {

		friend class LinksPluginDocument;

	public:
		virtual ~LinksFeedback();
		virtual void caretEnteredRange(KTextEditor::MovingRange* range, KTextEditor::View* view);
		virtual void caretExitedRange(KTextEditor::MovingRange* range, KTextEditor::View* view);
		virtual void rangeEmpty(KTextEditor::MovingRange* range);
		virtual void rangeInvalid(KTextEditor::MovingRange* range);

	private:
		void clearRanges(KTextEditor::MovingRange* range);

	private:
		QHash<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > m_ranges;
	};

public:
	LinksPluginDocument(KTextEditor::Document* document);
	~LinksPluginDocument();

	inline bool isValid() { return m_valid; }
	inline KTextEditor::Document* document() { return m_document; }

public slots:
	void handleView(KTextEditor::Document* document, KTextEditor::View* view);
	void scanDocument(KTextEditor::Document* document);
	void modifyContextMenu(KTextEditor::View* view, QMenu* menu);

	void openUrl();
	void copyUrl();

private:
	void scanRange(KTextEditor::Range range);

private:
	KTextEditor::Document* m_document;

	KTextEditor::MovingInterface* m_moving;
	KTextEditor::SearchInterface* m_search;

	KTextEditor::Attribute::Ptr m_rangeAttr;

	QAction* m_openAction;
	QAction* m_copyAction;
	QAction* m_separatorAction;

	LinksFeedback m_feedback;
	bool once, m_valid;

private:
    static QString emailPattern;
    static QString urlPattern;
    static QString completePattern;

};

#endif // _LINKS_PLUGIN_DOCUMENT_H_
