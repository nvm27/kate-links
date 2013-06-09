/*
 * Copyright 2013 Kamil Wojtuch
 *
 * This file is part of kate-links plugin.
 *
 * kate-links is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kate-links is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kate-links.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _LINKS_PLUGIN_H_
#define _LINKS_PLUGIN_H_

#include <ktexteditor/plugin.h>
#include <ktexteditor/document.h>

#include <QtCore/QList>
#include <QtCore/QString>

class LinksPluginDocument;

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
