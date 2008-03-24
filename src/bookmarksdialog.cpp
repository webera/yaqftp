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

// TODO still some memory leaks in there i think - also some stuff is redundant and waste
// TODO can we hold the dialog resistent in memory?
// TODO make sure that bookmarkname is unique
// TODO store start dir of local and remote site
// TODO undo or something like that when a bookmark is deleted so the dialog must not be reopened

#include <QtGui>

#include "bookmarksdialog.h"


BookmarksDialog::BookmarksDialog(QWidget *parent, QList<Bookmark *> *allBookmarks)
 : QDialog(parent), allBookmarks(allBookmarks), tmpList(new QList<Bookmark *>())
{
	initGui();
	loadBookmarks();

	setWindowTitle(tr("Bookmarks"));
	setModal(true);

	// tell Qt to delete the window when it is closed
	setAttribute(Qt::WA_DeleteOnClose);
}

// TODO set splitter position (should not be in the middle)
void BookmarksDialog::initGui()
{
	bookmarksList = new QListWidget();
	connect(bookmarksList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
			this, SLOT(selectedBookmarkChanged(QListWidgetItem *, QListWidgetItem *)));

	propertiesTabbedWidget = new QTabWidget();
	commonProperties = new CommonBookmarkProperties(this);
	connect(commonProperties, SIGNAL(nameChanged(QString)), this, SLOT(bookmarkNameChanged(QString)));

	propertiesTabbedWidget->addTab(commonProperties, tr("Common"));

	buttonLayout = new QHBoxLayout();

	cmdOk = new QPushButton(QIcon(":images/apply.png"), tr("&Ok"));
	connect(cmdOk, SIGNAL(clicked()), this, SLOT(okClicked()));

	cmdCancel = new QPushButton(QIcon(":images/cancel.png"), tr("&Cancel"));
	connect(cmdCancel, SIGNAL(clicked()), this, SLOT(reject()));

	cmdNew = new QPushButton(QIcon(":images/bookmark_add.png"), tr("&New"));
	connect(cmdNew, SIGNAL(clicked()), this, SLOT(newBookmark()));

	cmdRemove = new QPushButton(tr("&Remove"));
	connect(cmdRemove, SIGNAL(clicked()), this, SLOT(removeBookmark()));

	buttonLayout->addWidget(cmdOk);
	buttonLayout->addWidget(cmdCancel);
	buttonLayout->addWidget(cmdNew);
	buttonLayout->addWidget(cmdRemove);

	splitter = new QSplitter(Qt::Horizontal);
	splitter->addWidget(bookmarksList);
	splitter->addWidget(propertiesTabbedWidget);

	mainLayout = new QVBoxLayout();
	mainLayout->addWidget(splitter);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
	resize(500, 400);
}

void BookmarksDialog::bookmarkNameChanged(QString newName)
{
	QListWidgetItem *currentItem = bookmarksList->currentItem();
	Bookmark *aktBookmark = qobject_cast<Bookmark *>(currentItem->data(Qt::UserRole).value<QObject *>());

	currentItem->setText(newName);
	aktBookmark->setName(newName);
}

void BookmarksDialog::okClicked()
{
	// TODO updateing allBookmarks should get nicer

	// delete old listBookmark
	int count = allBookmarks->count();
	while(count-- != 0)
		delete allBookmarks->takeAt(count);

	count = bookmarksList->count();
	for(int i = 0; i < count; i++) {
		Bookmark *aktBookmark = qobject_cast<Bookmark *>
				 (bookmarksList->item(i)->data(Qt::UserRole).value<QObject *>());
		allBookmarks->append(new Bookmark(aktBookmark));
	}
	accept();
}

void BookmarksDialog::loadBookmarks()
{
	int count = allBookmarks->count();
	for(int i = 0; i < count; i++) {
		Bookmark *aktBookmark = new Bookmark(allBookmarks->at(i));
		QObject *aktBookmarkObject = qobject_cast<QObject *>(aktBookmark);
		QListWidgetItem *newItem = new QListWidgetItem(aktBookmark->getName());
		newItem->setData(Qt::UserRole, QVariant::fromValue(aktBookmarkObject));
		bookmarksList->addItem(newItem);
	}
	if(count == 0) {
		cmdRemove->setEnabled(false);
	}
}

void BookmarksDialog::selectedBookmarkChanged(QListWidgetItem *current, QListWidgetItem *prev)
{
	if(current != NULL) {
		Bookmark *bookmark = qobject_cast<Bookmark *>(current->data(Qt::UserRole).value<QObject *>()); // allBookmarks->at(currentRow);
		commonProperties->loadBookmark(bookmark);
		commonProperties->enabled(true);
		cmdRemove->setEnabled(true);
	} else {
		commonProperties->enabled(false);
		cmdRemove->setEnabled(false);
	}
}

void BookmarksDialog::removeBookmark()
{
	int newCount = bookmarksList->count() - 1;
	int currentRow = bookmarksList->row(bookmarksList->currentItem());

	// before taking the item (and the new row is set automatically and 
	// currentItemChanged is called, what causes problems) set the new row manually
	if(newCount > 0) {
		if(currentRow != 0) {
			bookmarksList->setCurrentRow(currentRow - 1);
		}
	} else {
		bookmarksList->setCurrentItem(NULL);
	}

	QListWidgetItem *item = bookmarksList->takeItem(currentRow);
	if(item != NULL) {
		Bookmark *currBookmark = qobject_cast<Bookmark *>(item->data(Qt::UserRole).value<QObject *>());

		delete item;
		delete currBookmark;

		item = NULL;
		currBookmark = NULL;
	}
}

void BookmarksDialog::newBookmark()
{
	Bookmark *bookmark = new Bookmark();
	// TODO generate unique Bookmarkname
	QListWidgetItem *newItem = new QListWidgetItem(bookmark->getName());
	QObject *bookmarkObject = qobject_cast<QObject *>(bookmark);
	newItem->setData(Qt::UserRole, QVariant::fromValue(bookmarkObject));
	tmpList->append(bookmark);
	bookmarksList->addItem(newItem);
	bookmarksList->setCurrentItem(newItem);
}

BookmarksDialog::~BookmarksDialog() {
	// delete tmpList
	/*
	while(!tmpList->isEmpty()) { delete tmpList->takeFirst(); }
	delete tmpList;
	*/

	int count = bookmarksList->count();
	for(int i = 0; i < count; i++) {
		delete bookmarksList->takeItem(i);
	}

/*
	cout << "delete BookmarksDialog" << endl;
	int count = bookmarksList->count();
	for(int i = 0; i < count; i++) {

		// FIXME this code did not work so we need this "hack" with the templist and so on - why??
		Bookmark *aktBookmark = qobject_cast<Bookmark *>
				 (bookmarksList->item(i)->data(Qt::UserRole).value<QObject *>());
		if(!allBookmarks->contains(aktBookmark)) {
			delete aktBookmark;
		}
		delete bookmarksList->takeItem(i);
	}
*/
}


// ----------------- CommonBookmarkProperties -------------------------
CommonBookmarkProperties::CommonBookmarkProperties(QWidget *parent)
 : QWidget(parent), aktBookmark(NULL)
{
	initGui();
}

void CommonBookmarkProperties::initGui()
{
	lblName = new QLabel(tr("Name:"));
	lblHost = new QLabel(tr("Host:"));
	lblPort = new QLabel(tr("Port:"));
	lblUserName = new QLabel(tr("Username:"));
	lblPassword = new QLabel(tr("Password:"));
	lblLocalDir = new QLabel(tr("Local Path:"));
	lblRemoteDir = new QLabel(tr("Remote Path:"));

	txtName = new QLineEdit();
	connect(txtName, SIGNAL(textChanged(QString)), this, SLOT(emitNameChanged(QString)));
	connect(txtName, SIGNAL(editingFinished()), this, SLOT(checkBookmarkName()));

	txtHost = new QLineEdit();
	txtPort = new QSpinBox();
	txtPort->setMinimum(1);
	txtPort->setMaximum(65535);
	txtPort->setValue(21);
	txtUserName = new QLineEdit();
	txtPassword = new QLineEdit();
	txtPassword->setProperty("echoMode", QLineEdit::Password);
	txtLocalDir = new QLineEdit();
	txtRemoteDir = new QLineEdit();
	cmdSelectLocalDir = new QPushButton(QString("..."));
	cmdSelectLocalDir->setFixedWidth(20);
	connect(cmdSelectLocalDir, SIGNAL(clicked()), this, SLOT(selectLocalPath()));

	mainLayout = new QGridLayout();
	mainLayout->addWidget(lblName, 0, 0);
	mainLayout->addWidget(lblHost, 1, 0);
	mainLayout->addWidget(lblPort, 2, 0);
	mainLayout->addWidget(lblUserName, 3, 0);
	mainLayout->addWidget(lblPassword, 4, 0);
	mainLayout->addWidget(lblLocalDir, 5, 0);
	mainLayout->addWidget(lblRemoteDir, 6, 0);

	mainLayout->addWidget(txtName, 0, 1, 1, 2);
	mainLayout->addWidget(txtHost, 1, 1, 1, 2);
	mainLayout->addWidget(txtPort, 2, 1, 1, 2);
	mainLayout->addWidget(txtUserName, 3, 1, 1, 2);
	mainLayout->addWidget(txtPassword, 4, 1, 1, 2);
	mainLayout->addWidget(txtLocalDir, 5, 1, 1, 1);
	mainLayout->addWidget(cmdSelectLocalDir, 5, 2, 1, 1);
	mainLayout->addWidget(txtRemoteDir, 6, 1, 1, 2);

	setLayout(mainLayout);
	enabled(false);
}

void CommonBookmarkProperties::selectLocalPath()
{
	QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), 
														  QDir::homePath(), QFileDialog::ShowDirsOnly);
	txtLocalDir->setText(directory);
}

void CommonBookmarkProperties::enabled(bool enabled)
{
	txtName->setEnabled(enabled);
	txtHost->setEnabled(enabled);
	txtPort->setEnabled(enabled);
	txtUserName->setEnabled(enabled);
	txtPassword->setEnabled(enabled);
	txtLocalDir->setEnabled(enabled);
	cmdSelectLocalDir->setEnabled(enabled);
	txtRemoteDir->setEnabled(enabled);
}

void CommonBookmarkProperties::emitNameChanged(QString newName)
{
	emit nameChanged(newName);
}

void CommonBookmarkProperties::checkBookmarkName()
{
	if(txtName->text().size() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("Bookmarkname can not be empty"));
		txtName->setFocus(Qt::OtherFocusReason);
	}
}

void CommonBookmarkProperties::loadBookmark(Bookmark *bookmark)
{
	txtHost->disconnect();
	txtPort->disconnect();
	txtUserName->disconnect();
	txtPassword->disconnect();
	txtLocalDir->disconnect();
	txtRemoteDir->disconnect();

	aktBookmark = bookmark;
	txtName->setText(bookmark->getName());
	txtHost->setText(bookmark->getHost());
	txtPort->setValue(bookmark->getPort().toInt());
	txtUserName->setText(bookmark->getUserName());
	txtPassword->setText(bookmark->getPassword());
	txtLocalDir->setText(bookmark->getLocalPath());
	txtRemoteDir->setText(bookmark->getRemotePath());

	connect(txtHost, SIGNAL(textChanged(QString)), bookmark, SLOT(setHost(QString)));
	connect(txtPort, SIGNAL(valueChanged(QString)), bookmark, SLOT(setPort(QString)));
	connect(txtUserName, SIGNAL(textChanged(QString)), bookmark, SLOT(setUserName(QString)));
	connect(txtPassword, SIGNAL(textChanged(QString)), bookmark, SLOT(setPassword(QString)));
	connect(txtLocalDir, SIGNAL(textChanged(QString)), bookmark, SLOT(setLocalPath(QString)));
	connect(txtRemoteDir, SIGNAL(textChanged(QString)), bookmark, SLOT(setRemotePath(QString)));
}
