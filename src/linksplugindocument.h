#ifndef _LINKS_PLUGIN_DOCUMENT_H_
#define _LINKS_PLUGIN_DOCUMENT_H_

#include <QMenu>

#include <kaction.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/movingcursor.h>
#include <ktexteditor/searchinterface.h>
#include <ktexteditor/movinginterface.h>
#include <ktexteditor/movingrangefeedback.h>

#include <set>

class LinksPluginDocument : public QObject {
	Q_OBJECT

private:
	class LinksFeedback : public KTextEditor::MovingRangeFeedback {

		friend class LinksPluginDocument;

	public:
		LinksFeedback(LinksPluginDocument* plugin);
		virtual ~LinksFeedback();
		virtual void caretEnteredRange(KTextEditor::MovingRange* range, KTextEditor::View* view);
		virtual void caretExitedRange(KTextEditor::MovingRange* range, KTextEditor::View* view);
		virtual void rangeEmpty(KTextEditor::MovingRange* range);
		virtual void rangeInvalid(KTextEditor::MovingRange* range);

	private:
		LinksPluginDocument* m_plugin;
		QHash<KTextEditor::View*, QSet<KTextEditor::MovingRange*> > m_ranges;
	};

	struct MovingCursorCompare {
		inline bool operator() (const KTextEditor::MovingCursor* const& lhs, const KTextEditor::MovingCursor* const& rhs) {
			//if (!lhs->isValid())
			//	kDebug() << "comaprision between invalid cursors: " << *lhs << " vs. " << *rhs;

			return *lhs < *rhs;
		}
	};

public:
	LinksPluginDocument(KTextEditor::Document* document);
	~LinksPluginDocument();

	inline bool isValid() const { return m_valid; }
	inline KTextEditor::Document* document() const { return m_document; }

public slots:
	void documentFirstChange();
	void documentAboutToCloseOrReload();

	void documentTextInserted(KTextEditor::Document* document, const KTextEditor::Range& range);
	void documentTextRemoved(KTextEditor::Document* document, const KTextEditor::Range& range);

	void handleView(KTextEditor::Document* document, KTextEditor::View* view);
	void modifyContextMenu(KTextEditor::View* view, QMenu* menu) const;

	void openUrl() const;
	void copyUrl() const;

private:
	void rescanLine(int line);
	void scanRange(KTextEditor::Range range);
	void deleteMovingRange(KTextEditor::MovingRange* range);

private:
	KTextEditor::Document* m_document;

	KTextEditor::MovingInterface* m_moving;
	KTextEditor::SearchInterface* m_search;

	KTextEditor::Attribute::Ptr m_rangeAttr;

	QAction* m_openAction;
	QAction* m_copyAction;
	QAction* m_separatorAction;

	LinksFeedback m_feedback;
	bool m_valid;

	// MovingCursors should hold inequality
	std::set<const KTextEditor::MovingCursor*, LinksPluginDocument::MovingCursorCompare> m_cursors;

private:
    static QString emailPattern;
    static QString urlPattern;
    static QString completePattern;
};

#endif // _LINKS_PLUGIN_DOCUMENT_H_
