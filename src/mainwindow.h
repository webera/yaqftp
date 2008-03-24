/***************************************************************************
 *   Copyright (C) 2007 by Antonio Weber                                   *
 *   antonio.weber@stud.fh-regensburg.de                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// TODO let user set colors for messages (remote message - error message - local message etc)
// TODO a lot of error handling has to be done (path stuff)
// FIXME have to do very much redesign stuff but I want to get a first version to release

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#define CONFIG_DIR ".kftp"
#define BOOKMARK_XML_FILE "bookmarks.xml"


#include <QMainWindow>

// forward declaration
class QWorkspace;
class BookmarksDialog;
class Bookmark;
class QActionGroup;
class QFile;

/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
    	MainWindow();

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:
		void newConnection(QAction *action);
		void manageBookmarks();
		void quickConnection();
		void windowChanged();
		void saveBookmarks();
		void about();

		void closeActiveWindow();

	private:
		// methods
		void createMenu();
		void initGui();
		void loadBookmarks();
		void updateBookmarkConnections();

		// variables
		QList<Bookmark *> *allBookmarks;
		QFile *bookmarksFile;

		QWorkspace *workspace;
		BookmarksDialog *bookmarks;

		QMenu *fileMenu;
		QMenu *bookmarksMenu;
		QMenu *helpMenu;
		QMenu *editMenu;
		QMenu *newConnectionMenu;

		QAction *closeWindowAction;

		QActionGroup *allBookmarkConnections;
};

#endif
