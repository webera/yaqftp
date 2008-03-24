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

// TODO possibility to hide columns - how to find out that the listheader is clicked?
// TODO how implement the menu? addAction / customcontextMenu / "normal" contextMenu
// TODO track last focused side to use shortcuts like F5 for copy or so
// FIXME crazy behavior when dialog is maximized - header are mazy

#include <QtGui>

#include "bookmark.h"
#include "ftpmodel.h"
#include "ftpsessiondialog.h"

FtpSessionDialog::FtpSessionDialog(QWidget *parent, Bookmark *bookmark)
 : QDialog(parent), progressBars(new QHash<int, QProgressBar *>()), watcher(new QFileSystemWatcher())
{
	init(bookmark);
	setWindowTitle(bookmark->getName());

	// tell Qt to delete the window when it is closed
	setAttribute(Qt::WA_DeleteOnClose);
}

void FtpSessionDialog::remoteViewDoubleClicked(const QModelIndex &index)
{
	FtpModelItem *item = static_cast<FtpModelItem *>(index.internalPointer());
	if(item->isDirectory()) {
		QString dirName = item->getInfo().name();
		QString newName = remoteModel->currentDirectory();
		if(dirName == QString("..")) {
			int lastIndex = newName.lastIndexOf(QString("/"));
			if(lastIndex > 0) {
				newName = newName.remove(lastIndex, newName.size());
			} else {
				newName = QString("/");
			}
		} else {
			if(!newName.endsWith('/')) {
				newName += '/';
			}
			newName += dirName;
		}
		remoteModel-> setCurrentDirectory(newName);
	}
}

void FtpSessionDialog::localViewDoubleClicked(const QModelIndex &index)
{
	FileModelItem *item = static_cast<FileModelItem *>(index.internalPointer());
	if(item->getInfo().isDir()) {
		QDir dir = localModel->currentDir();
		QString fileName = item->getInfo().fileName();

		if(dir.cd(fileName)) {
			localModel->setCurrentDirectory(dir);
		}
	}
}

// TODO support relative path changes?
void FtpSessionDialog::localPathTextChanged()
{
	QString newLocal = txtPathLeft->text();
	// check if path exists
	QDir test(newLocal);
	if(test.exists() && test.isReadable()) {
		localModel->setCurrentDirectory(QDir(newLocal));
	} else {
		txtPathLeft->setText(localModel->currentDir().absolutePath());
		QMessageBox::warning(this, tr("Warning"), tr("Direcotry do not exists"));
	}
}

void FtpSessionDialog::localModelReseted()
{
	QString newPath = localModel->currentDir().absolutePath();
	QString prevPath = txtPathLeft->text();
	txtPathLeft->setText(newPath);
	watcher->removePath(prevPath);
	watcher->addPath(newPath);
}

void FtpSessionDialog::remoteModelReseted()
{
	txtPathRight->setText(remoteModel->currentDirectory());
}

void FtpSessionDialog::connectionStateChanged(QString& message)
{
	logList->addItem(message);
}

void FtpSessionDialog::leftListMenuRequested(const QPoint &pos)
{
	leftListContextMenu->popup(leftFolderList->viewport()->mapToGlobal(pos));
}

void FtpSessionDialog::rightListMenuRequested(const QPoint &pos)
{
	rightListContextMenu->popup(rightFolderList->viewport()->mapToGlobal(pos));
}

void FtpSessionDialog::init(Bookmark *bookmark)
{ 
	connect(watcher, SIGNAL(directoryChanged(const QString&)), 
			this, SLOT(localDirectoryChanged(const QString&)));

	// GUI STUFF

	// left side
	controlsLayoutLeft = new QHBoxLayout();
	cmdRefreshLeft = new QPushButton(tr("Refresh"));
	cmdRefreshLeft->setIcon(QIcon(":images/reload.png"));
	txtPathLeft = new QLineEdit();
	controlsLayoutLeft->addWidget(txtPathLeft);
	controlsLayoutLeft->addWidget(cmdRefreshLeft);
	
	// right side
	controlsLayoutRight = new QHBoxLayout();
	cmdRefreshRight = new QPushButton(tr("Refresh"));
	cmdRefreshRight->setIcon(QIcon(":images/reload.png"));
	txtPathRight = new QLineEdit();
	controlsLayoutRight->addWidget(txtPathRight);
	controlsLayoutRight->addWidget(cmdRefreshRight);

	// folder splitter
	folderSplitter = new QSplitter(Qt::Horizontal);
	widgetSplitter = new QSplitter(Qt::Vertical);

	// left side with local dir model
	// TODO drop support on both sides
	localModel = new FileModel(this, bookmark);
	watcher->addPath(localModel->currentDir().absolutePath());
	txtPathLeft->setText(localModel->currentDir().absolutePath());
	connect(localModel, SIGNAL(modelReset()), this, SLOT(localModelReseted()));
	connect(txtPathLeft, SIGNAL(returnPressed()), this, SLOT(localPathTextChanged()));

	leftFolderList = new QTableView();
	leftFolderList->setFocusPolicy(Qt::NoFocus);
	leftFolderList->verticalHeader()->hide();
	leftFolderList->setShowGrid(false);
	leftFolderList->setContextMenuPolicy(Qt::CustomContextMenu);
	leftFolderList->setSelectionBehavior(QAbstractItemView::SelectRows);
	leftFolderList->setAcceptDrops(true);
	leftFolderList->setDragDropMode(QAbstractItemView::DragDrop);
	leftFolderList->setDropIndicatorShown(true);
	leftFolderList->setDragEnabled(true);
	leftFolderList->setModel(localModel);
	leftFolderList->horizontalHeader()->installEventFilter(this);

	connect(leftFolderList, SIGNAL(doubleClicked(const QModelIndex &)), 
			this, SLOT(localViewDoubleClicked(const QModelIndex &)));
	connect(leftFolderList, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(leftListMenuRequested(const QPoint &)));

	remoteModel = new FtpModel(this, bookmark);
	txtPathRight->setText(remoteModel->currentDirectory());
	connect(remoteModel, SIGNAL(connectionStateChanged(QString&)),
			this, SLOT(connectionStateChanged(QString&)));
	connect(remoteModel, SIGNAL(ftpErrorOccurred(QString&)), 
			this, SLOT(connectionStateChanged(QString&)));
	connect(remoteModel, SIGNAL(modelReset()), this, SLOT(remoteModelReseted()));



	// FIXME implement
	connect(remoteModel, SIGNAL(queryUpload(int, QString&)), this, SLOT(queryFile(int, QString&)));
	// FIXME implement
	connect(remoteModel, SIGNAL(queryDownload(int, QString&)), this, SLOT(queryFile(int, QString&)));
	// FIXME implement
	connect(remoteModel, SIGNAL(transferStarted(int, int)), 
			this, SLOT(transferStarted(int, int)));
	// FIXME implement
	connect(remoteModel, SIGNAL(transferProgress(int, qint64, qint64)), 
			this, SLOT(updateProgress(int, qint64, qint64)));
	// FIXME implement
	connect(remoteModel, SIGNAL(transferFinished(int, bool)), this, SLOT(transferFinished(int)));


	rightFolderList = new QTableView();
	rightFolderList->setModel(remoteModel);
	rightFolderList->verticalHeader()->hide();
	rightFolderList->setFocusPolicy(Qt::NoFocus);
	rightFolderList->setShowGrid(false);
	rightFolderList->setAcceptDrops(true);
	rightFolderList->setContextMenuPolicy(Qt::CustomContextMenu);
	rightFolderList->setSelectionBehavior(QAbstractItemView::SelectRows);
 	rightFolderList->setDragDropMode(QAbstractItemView::DragDrop);
	rightFolderList->setDropIndicatorShown(true);
	rightFolderList->setDragEnabled(true);
	rightFolderList->horizontalHeader()->installEventFilter(this);
	connect(rightFolderList, SIGNAL(doubleClicked(const QModelIndex &)), 
			this, SLOT(remoteViewDoubleClicked(const QModelIndex &)));
	connect(rightFolderList, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(rightListMenuRequested(const QPoint &)));

	leftLayout = new QVBoxLayout();
	leftLayout->addLayout(controlsLayoutLeft);
	leftLayout->addWidget(leftFolderList);
	left = new QWidget(this);
	left->setLayout(leftLayout);

	rightLayout = new QVBoxLayout();
	rightLayout->addLayout(controlsLayoutRight);
	rightLayout->addWidget(rightFolderList);
	right = new QWidget(this);
	right->setLayout(rightLayout);

	folderSplitter->addWidget(left);
	folderSplitter->addWidget(right);

	progressTable = new QTableWidget();
	QStringList progressHorizontalHeader;
	progressHorizontalHeader.append(tr("Name"));
	progressHorizontalHeader.append(tr("Progress")); // TODO set progress column width to max size
	progressTable->setColumnCount(progressHorizontalHeader.size());
	progressTable->setHorizontalHeaderLabels(progressHorizontalHeader);
	logList = new QListWidget();

	widgetSplitter->addWidget(folderSplitter);
	widgetSplitter->addWidget(progressTable);
	widgetSplitter->addWidget(logList);

	mainLayout = new QVBoxLayout();
	mainLayout->addWidget(widgetSplitter);

	setLayout(mainLayout);

	// CONTEXT MENUS
	leftListContextMenu = new QMenu(this); // give parent control about memory
	leftCopyAction = leftListContextMenu->addAction(tr("Upload selected items"));
	connect(leftCopyAction, SIGNAL(triggered()), this, SLOT(upload()));
	leftDeleteAction = leftListContextMenu->addAction(tr("Delete selected items"));
	connect(leftDeleteAction, SIGNAL(triggered()), this, SLOT(deleteLocal()));
	leftMkdirAction = leftListContextMenu->addAction(tr("Create Directory"));
	connect(leftMkdirAction, SIGNAL(triggered()), this, SLOT(mkdirLocal()));
	leftRenameAction = leftListContextMenu->addAction(tr("Rename"));
	connect(leftRenameAction, SIGNAL(triggered()), this, SLOT(renameLocal()));

	// CONTEXT MENUS
	rightListContextMenu = new QMenu(this); // give parent control about memory
	rightCopyAction = rightListContextMenu->addAction(tr("Download selected items"));
	connect(rightCopyAction, SIGNAL(triggered()), this, SLOT(download()));
	rightDeleteAction = rightListContextMenu->addAction(tr("Delete selected items"));
	connect(rightDeleteAction, SIGNAL(triggered()), this, SLOT(deleteRemote()));
	rightMkdirAction = rightListContextMenu->addAction(tr("Create Directory"));
	connect(rightMkdirAction, SIGNAL(triggered()), this, SLOT(mkdirRemote()));
	rightRenameAction = rightListContextMenu->addAction(tr("Rename"));
	connect(rightRenameAction, SIGNAL(triggered()), this, SLOT(renameRemote()));

	resize(800, 600);
}

bool FtpSessionDialog::eventFilter(QObject *watchedObject, QEvent *event)
{
	if (watchedObject == leftFolderList->horizontalHeader() && event->type() == QEvent::ContextMenu) {
		qDebug() << "------------------------------ leftFolderList rightclicked";
	} else {
		if (watchedObject == rightFolderList->horizontalHeader() && event->type() == QEvent::ContextMenu) {
			qDebug() << "------------------------------ rightFolderList rightclicked";
		}
	}
	return true;
}

void FtpSessionDialog::upload()
{
	qDebug() << "upload";
	// retrive selected
	QModelIndexList selectedModelIndizes = leftFolderList->selectionModel()->selectedIndexes();
	remoteModel->uploadFiles(selectedModelIndizes);
}

void FtpSessionDialog::download()
{
	qDebug() << "download";
	QModelIndexList selectedModelIndizes = rightFolderList->selectionModel()->selectedIndexes();
	remoteModel->downloadFiles(selectedModelIndizes, localModel->currentDir().absolutePath());
}

void FtpSessionDialog::deleteLocal()
{
	qDebug() << "delete Local";
	QModelIndexList selectedModelIndizes = leftFolderList->selectionModel()->selectedIndexes();
	localModel->deleteSelected(selectedModelIndizes);
}

void FtpSessionDialog::deleteRemote()
{
	qDebug() << "deleteRemote";
	QModelIndexList selectedModelIndizes = rightFolderList->selectionModel()->selectedIndexes();
	remoteModel->deleteSelected(selectedModelIndizes);
}

// TODO only create if it not exists - else inform user about the Problem
void FtpSessionDialog::mkdirLocal()
{
	qDebug() << "mkdirLocal";
    bool ok;
	QString newDirName = QInputDialog::getText(this, tr("Create Directory"), tr("Create Direcotry"),
										 QLineEdit::Normal, "", &ok);

	if (ok && !newDirName.isEmpty()) {
		localModel->mkdir(newDirName);
	}
}

void FtpSessionDialog::mkdirRemote()
{
	qDebug() << "mkdirRemote";
    bool ok;
	QString newDirName = QInputDialog::getText(this, tr("Create Directory"), tr("Create Direcotry"),
										 QLineEdit::Normal, "", &ok);

	if (ok && !newDirName.isEmpty()) {
		remoteModel->mkdir(newDirName);
	}
}

void FtpSessionDialog::renameLocal()
{
	qDebug() << "renameLocal()";
    bool ok;
	QString text = QInputDialog::getText(this, tr("Rename"), tr("Rename"),
										 QLineEdit::Normal, "Selected blah", &ok);

	if (ok && !text.isEmpty()) {
		// rename item ... textLabel->setText(text);
	}
}

void FtpSessionDialog::renameRemote()
{
	qDebug() << "renameRemote()";
}

void FtpSessionDialog::queryFile(int id, QString& name)
{
	int rowCount = progressTable->rowCount();
	progressTable->setRowCount(rowCount + 1);
	progressTable->setItem(rowCount, 0, new QTableWidgetItem(name));
	QProgressBar *progressBar = new QProgressBar;
	progressBar->setMinimum(0);
	progressBar->setValue(0);
	progressBars->insert(id, progressBar);
	progressTable->setCellWidget(rowCount, 1, progressBar);
}

void FtpSessionDialog::transferStarted(int id, int type)
{
	aktType = type;
}

void FtpSessionDialog::updateProgress(int id, qint64 done, qint64 total)
{
	QProgressBar *progress = progressBars->value(id);
	progress->setMaximum(total);
	progress->setValue(done);
}

void FtpSessionDialog::transferFinished(int id)
{
	// delete row and progressbar
	progressTable->removeRow(0);
	QProgressBar *progress = progressBars->value(id);
	if(progress != NULL) { delete progress; }
	if(aktType == 8) { // QFtp::Get = 8
		localModel->update();
	}
}

void FtpSessionDialog::localDirectoryChanged(const QString& pathName)
{
	if(pathName == localModel->currentDir().absolutePath()) {
		localModel->update();
	}
}
