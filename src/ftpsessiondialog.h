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
#ifndef FTPSESSIONDIALOG_H
#define FTPSESSIONDIALOG_H

#include <QDialog>

class QProgressBar;
class QTableWidget;
class QListWidget;
class QSplitter;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QListView;
class QScrollArea;
class QDirModel;
class QModelIndex;
class QTableView;
class QMenu;
class FileModel;
class FtpModel;
class Bookmark;
class QFileSystemWatcher;


/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class FtpSessionDialog : public QDialog
{
	Q_OBJECT

	public:
		FtpSessionDialog(QWidget *parent, Bookmark *bookmark);

	protected:
		bool eventFilter(QObject *watchedObject, QEvent *event);

	private slots:
		void localPathTextChanged();
		void localViewDoubleClicked(const QModelIndex &index);
		void remoteViewDoubleClicked(const QModelIndex &index);
		void localModelReseted();
		void remoteModelReseted();
		void connectionStateChanged(QString& message);
		void leftListMenuRequested(const QPoint &pos);
		void rightListMenuRequested(const QPoint &pos);

		void upload();
		void download();
		void deleteLocal();
		void deleteRemote();
		void mkdirLocal();
		void mkdirRemote();
		void renameLocal();
		void renameRemote();

		void queryFile(int id, QString& name);
		void updateProgress(int id, qint64 done, qint64 total);
		void transferStarted(int id, int type);
		void transferFinished(int id);

		void localDirectoryChanged(const QString& pathName);

	private:
		// METHODS
		void init(Bookmark *bookmark);
		QMenu *getListContextMenu(const bool itemsSelected);

		QHash<int, QProgressBar *> *progressBars;
		QFileSystemWatcher *watcher;

		int aktType; // not used at the moment - do we need this really?
		// MEMBER
		// contextMenu
		QMenu *leftListContextMenu;
		QAction *leftCopyAction;
		QAction *leftDeleteAction;
		QAction *leftMkdirAction;
		QAction *leftRenameAction;

		QMenu *rightListContextMenu;
		QAction *rightCopyAction;
		QAction *rightDeleteAction;
		QAction *rightMkdirAction;
		QAction *rightRenameAction;

		// gui
		QLayout *mainLayout;
		QVBoxLayout *leftLayout;
		QVBoxLayout *rightLayout;
		QHBoxLayout *controlsLayoutLeft;
		QHBoxLayout *controlsLayoutRight;

		QSplitter *widgetSplitter;
		QSplitter *folderSplitter;
		QTableWidget *progressTable;
		QListWidget *logList;

		QPushButton *cmdRefreshLeft;
		QPushButton *cmdRefreshRight;
		QLineEdit *txtPathLeft;
		QLineEdit *txtPathRight;
		QTableView *leftFolderList;
		QTableView *rightFolderList;
		QWidget *left;
		QWidget *right;

		// models
		FileModel *localModel;
		FtpModel *remoteModel;
};

#endif
