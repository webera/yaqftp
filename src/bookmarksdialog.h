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
#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QDialog>
#include "bookmark.h"

class QFile;
class QLabel;
class QSplitter;
class QListWidget;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QGridLayout;
class QLineEdit;
class QSpinBox;
class QListWidgetItem;

//! CommonBookmarkProperties
/**
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class CommonBookmarkProperties : public QWidget {

	Q_OBJECT

	public:
		CommonBookmarkProperties(QWidget *parent);
		// no need of a destructor becase qt deletes all components when 
		// parent (bookmarksdialog) is deleted

		void enabled(bool enabled);
		void loadBookmark(Bookmark *bookmark);

	signals:
		void nameChanged(QString newName);

	private slots:
		void emitNameChanged(QString newName);
		void selectLocalPath();
		void checkBookmarkName();

	private:
		// methods
		void initGui();

		// members
		Bookmark *aktBookmark;

		QGridLayout *mainLayout;

		QLabel *lblName;
		QLabel *lblHost;
		QLabel *lblPort;	// TODO use combobox as input for port
		QLabel *lblUserName;
		QLabel *lblPassword;
		QLabel *lblLocalDir;
		QLabel *lblRemoteDir;

		QLineEdit *txtName;
		QLineEdit *txtHost;
		QSpinBox *txtPort;
		QLineEdit *txtUserName;
		QLineEdit *txtPassword;
		QLineEdit *txtLocalDir;
		QLineEdit *txtRemoteDir;
		QPushButton *cmdSelectLocalDir;
};

//! BookmarksDialog
/**
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class BookmarksDialog : public QDialog {

	Q_OBJECT

	public:
    	BookmarksDialog(QWidget *parent, QList<Bookmark *> *allBookmarks);
		~BookmarksDialog();

	private slots:
		void okClicked();
		void newBookmark();
		void selectedBookmarkChanged(QListWidgetItem *current, QListWidgetItem *prev);
		void removeBookmark();
		void bookmarkNameChanged(QString);

	private:
		// methods
		void initGui();
		void loadBookmarks();

		// members
		//! this list holds newly added bookmarks which are not saved now
		QList<Bookmark *> *tmpList; // FIXME how to avoid this "hack"?

		QList<Bookmark *> *allBookmarks;

		QVBoxLayout *mainLayout;
		QHBoxLayout *buttonLayout;
		QSplitter *splitter;

		QListWidget *bookmarksList;
		QTabWidget *propertiesTabbedWidget;

		QPushButton *cmdOk;
		QPushButton *cmdNew;
		QPushButton *cmdCancel;
		QPushButton *cmdRemove;

		CommonBookmarkProperties *commonProperties;
};

#endif
