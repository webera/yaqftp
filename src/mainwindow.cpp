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

// TODO add "show hidden files" etc
// TODO Progress also for commands so the user knows that there is for example the login in progress

#include <QtGui>
#include <QtXml>
#include <QtCore>

#include "mainwindow.h"
#include "bookmarksdialog.h"
#include "ftpsessiondialog.h"
#include "quickconnectiondialog.h"

MainWindow::MainWindow()
 : QMainWindow(), allBookmarkConnections(NULL)
{
	loadBookmarks();
	initGui();
	createMenu();
	// tell Qt to delete windows when it is closed
	setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::initGui()
{
	setWindowTitle(tr("KFtp"));
	workspace = new QWorkspace();
	setCentralWidget(workspace);
	connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(windowChanged()));
}

void MainWindow::windowChanged()
{
	if(workspace->activeWindow() != NULL) {
		closeWindowAction->setVisible(true);
	} else {
		closeWindowAction->setVisible(false);
	}
}

void MainWindow::updateBookmarkConnections()
{
	// TODO perhaps can be more effective (must not delete all - could only insert or remove)
	if(allBookmarkConnections != NULL) {
		QList<QAction *> all = allBookmarkConnections->actions();
		int size = all.size();
		for(int i = 0; i < size; i++) {
			delete all.at(i);
		}
		delete allBookmarkConnections;
	}

	allBookmarkConnections = new QActionGroup(newConnectionMenu);
	int size = allBookmarks->size();
	if(size > 0) {
		newConnectionMenu->setEnabled(true);
		for(int i = 0; i < size; i++) {
			Bookmark *aktBookmark = allBookmarks->at(i);
			QAction *aktAction = new QAction(aktBookmark->getName(), allBookmarkConnections);
			QObject *bookmarkObject = qobject_cast<QObject *>(aktBookmark);
			aktAction->setData(QVariant::fromValue(bookmarkObject));
			allBookmarkConnections->addAction(aktAction);
			newConnectionMenu->addAction(aktAction);
		}
		connect(allBookmarkConnections, SIGNAL(triggered(QAction *)), this, SLOT(newConnection(QAction *)));
	} else {
		newConnectionMenu->setEnabled(false);
	}	
}

void MainWindow::createMenu()
{
	// File Menu
	fileMenu = menuBar()->addMenu(tr("&File"));
	
	// File -> New Connection
	QAction *quickConnectionAction = fileMenu->addAction(tr("Quick Co&nnection"));
	quickConnectionAction->setShortcut(tr("Ctrl+N"));
	connect(quickConnectionAction, SIGNAL(triggered()), this, SLOT(quickConnection()));

	newConnectionMenu = new QMenu(tr("&Open Connection"));
	updateBookmarkConnections();
	// connect(newConnectionMenu, SIGNAL(triggered()), this, SLOT(newConnection()));
	//newConnectionMenu->addAction(quickConnectionAction);
	fileMenu->addMenu(newConnectionMenu);

	fileMenu->addSeparator();

	// File -> Quit
	closeWindowAction = fileMenu->addAction(tr("C&lose"));
	closeWindowAction->setVisible(false);
	closeWindowAction->setIcon(QIcon(":images/close.png"));
	closeWindowAction->setShortcut(tr("Ctrl+W"));
	connect(closeWindowAction, SIGNAL(triggered()), this, SLOT(closeActiveWindow()));

	// File -> Quit
	QAction *exitAction = fileMenu->addAction(tr("&Quit"));
	exitAction->setIcon(QIcon(":images/exit.png"));
	exitAction->setShortcut(tr("Ctrl+Q"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	// edit Menu
	editMenu = menuBar()->addMenu(tr("&Edit"));

	// bookmarks Menu
	bookmarksMenu = menuBar()->addMenu(tr("&Bookmarks"));

	// Bookmarks -> Manage Bookmarks
	QAction *manageBookmarksAction = bookmarksMenu->addAction(tr("&Manage Bookmarks"));
	manageBookmarksAction->setIcon(QIcon(":images/bookmark.png"));
	connect(manageBookmarksAction, SIGNAL(triggered()), this, SLOT(manageBookmarks()));

	// Help Menu
	helpMenu = menuBar()->addMenu(tr("&Help"));

	QAction *aboutAction = helpMenu->addAction(tr("A&bout"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	// Help -> About Qt TODO is there a fine looking icon for that?
	QAction *aboutQtAction = helpMenu->addAction(tr("About &Qt"));
	connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::newConnection(QAction *action)
{
	QObject *bookmarkObject = action->data().value<QObject *>();
	Bookmark *bookmark = qobject_cast<Bookmark *>(bookmarkObject);
	FtpSessionDialog *newSession = new FtpSessionDialog(this, bookmark);
	workspace->addWindow(newSession);
	newSession->show();
}

void MainWindow::quickConnection()
{
	Bookmark *tempBookmark = new Bookmark();
	QuickConnectionDialog *quickConnection = new QuickConnectionDialog(this, tempBookmark);
	if(quickConnection->exec() == QDialog::Accepted) {
		tempBookmark->setName(tempBookmark->getHost());
		FtpSessionDialog *newSession = new FtpSessionDialog(this, tempBookmark);
		workspace->addWindow(newSession);
		newSession->show();
	}
}

void MainWindow::manageBookmarks()
{
	// we must not take care about memory becase qt deletes it after the dialog get closed
	bookmarks = new BookmarksDialog(this, allBookmarks);
	connect(bookmarks, SIGNAL(accepted()), this, SLOT(saveBookmarks()));
	if(bookmarks->exec() == QDialog::Accepted) {
		updateBookmarkConnections();
	}
}

void MainWindow::loadBookmarks()
{
	QString home = QDir::homePath();
	QDir dir;
	home.append(QString("/") + tr(CONFIG_DIR));
	dir.mkpath(home);
	home.append(QString("/") + tr(BOOKMARK_XML_FILE));
	bookmarksFile = new QFile(home);

	allBookmarks = new QList<Bookmark *>();

	if(bookmarksFile->exists() && bookmarksFile->size() > 0) {
		QDomDocument document;
		QXmlSimpleReader reader;
		QXmlInputSource input(bookmarksFile);
		if(document.setContent(&input, &reader)) {
			QDomNodeList bookmarkNodeList = document.elementsByTagName(QString("Bookmark"));
			int count = bookmarkNodeList.count();
			for(int i = 0; i < count; i++) {
				QString name, host, port, username, password, localPath, remotePath;

				QDomNode bookmarkNode = bookmarkNodeList.item(i);
				QDomNode bookmarkNameNode = bookmarkNode.attributes().namedItem(QString("Name"));
				name = bookmarkNameNode.nodeValue();

				QDomNode hostNode = bookmarkNode.firstChildElement("Host");
				host = hostNode.firstChild().nodeValue();

				QDomNode portNode = bookmarkNode.firstChildElement("Port");
				port = portNode.firstChild().nodeValue();

				QDomNode usernameNode = bookmarkNode.firstChildElement("Username");
				username = usernameNode.firstChild().nodeValue();

				// TODO what about encrypted passwords...
				QDomNode passwordNode = bookmarkNode.firstChildElement("Password");
				password = passwordNode.firstChild().nodeValue();

				QDomNode localPathNode = bookmarkNode.firstChildElement("LocalPath");
				localPath = localPathNode.firstChild().nodeValue();

				QDomNode remotePathNode = bookmarkNode.firstChildElement("RemotePath");
				remotePath = remotePathNode.firstChild().nodeValue();

				Bookmark *aktBookmark = new Bookmark(name, host, port, username,
													 password, localPath, remotePath);

				allBookmarks->append(aktBookmark);
			}
		}
		else {
			QMessageBox::critical(this, tr("Error"), tr("Error while reading bookmarks.xml"));
		}

		// make sure file gets closed after using it
		// in my opinion it would be cleaner if QXmlInputSource can be closed
		if(bookmarksFile->isOpen()) {
			bookmarksFile->close();
		}
	}
}

// TODO try to use gnupg or something else to encrypt password
void MainWindow::saveBookmarks()
{
	QDomDocument doc(QString("Bookmarks"));
	QDomElement rootElement = doc.createElement(QString("Bookmarks"));
	doc.appendChild(rootElement);

	int count = allBookmarks->size();

	for(int i = 0; i < count; i++) {
		Bookmark *aktBookmark = allBookmarks->at(i);
		QDomElement bookmarksElement = doc.createElement(QString("Bookmark"));
		bookmarksElement.setAttribute(QString("Name"), aktBookmark->getName());

		QDomElement hostElement = doc.createElement(QString("Host"));
		QDomElement portElement = doc.createElement(QString("Port"));
		QDomElement usernameElement = doc.createElement(QString("Username"));
		QDomElement passwordElement = doc.createElement(QString("Password"));
		QDomElement localPathElement = doc.createElement(QString("LocalPath"));
		QDomElement remotePathElement = doc.createElement(QString("RemotePath"));

		hostElement.appendChild(doc.createTextNode(aktBookmark->getHost()));
		portElement.appendChild(doc.createTextNode(aktBookmark->getPort()));
		usernameElement.appendChild(doc.createTextNode(aktBookmark->getUserName()));
		passwordElement.appendChild(doc.createTextNode(aktBookmark->getPassword()));
		localPathElement.appendChild(doc.createTextNode(aktBookmark->getLocalPath()));
		remotePathElement.appendChild(doc.createTextNode(aktBookmark->getRemotePath()));

		bookmarksElement.appendChild(hostElement);
		bookmarksElement.appendChild(portElement);
		bookmarksElement.appendChild(usernameElement);
		bookmarksElement.appendChild(passwordElement);
		bookmarksElement.appendChild(localPathElement);
		bookmarksElement.appendChild(remotePathElement);

		rootElement.appendChild(bookmarksElement);
	}

	if(bookmarksFile->open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream out(bookmarksFile);
		doc.save(out, 4);
		bookmarksFile->close();
	}
	else {
		QMessageBox::critical(this, tr("Error"), tr("Error while writing bookmarks.xml"));
		bookmarksFile->close();
	}
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QtFtp"), tr("<h2>QtFtp 0.1</h2>" 
								"<p>Copyright &copy; 2007 by Antonio Weber." 
								"<p>QtFtp is a Ftp - Client application.")); 
}

void MainWindow::closeActiveWindow()
{
	workspace->activeWindow()->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	// TODO sessions schließen etc
	QWidgetList allWindowsList = workspace->windowList();
	int count = allWindowsList.count();
	for(int i = 0; i < count; i++) {
		delete allWindowsList.at(i);
	}
	event->accept();
}
