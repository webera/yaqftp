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

#include <QtGui>

#include "bookmark.h"

#include "quickconnectiondialog.h"

QuickConnectionDialog::QuickConnectionDialog(QWidget *parent, Bookmark *tempBookmark)
 : QDialog(parent)
{
	setModal(true);
	initGui(tempBookmark);

	// tell Qt to delete the window when it is closed
	setAttribute(Qt::WA_DeleteOnClose);
}

void QuickConnectionDialog::initGui(const Bookmark *tempBookmark)
{
	setWindowTitle(tr("Quick Connection"));

	lblHost = new QLabel(tr("Host:"));
	lblPort = new QLabel(tr("Port:"));
	lblUserName = new QLabel(tr("Username:"));
	lblPassword = new QLabel(tr("Password:"));
	txtHost = new QLineEdit(tempBookmark->getHost());
	connect(txtHost, SIGNAL(textChanged(QString)), tempBookmark, SLOT(setHost(QString)));
	txtUserName = new QLineEdit(tempBookmark->getUserName());
	connect(txtUserName, SIGNAL(textChanged(QString)), tempBookmark, SLOT(setUserName(QString)));
	txtPassword = new QLineEdit(tempBookmark->getPassword());
	connect(txtPassword, SIGNAL(textChanged(QString)), tempBookmark, SLOT(setPassword(QString)));
	txtPassword->setProperty("echoMode", QLineEdit::Password);
	connect(txtPassword, SIGNAL(textChanged(QString)), tempBookmark, SLOT(setPassword(QString)));
	port = new QSpinBox();
	connect(port, SIGNAL(valueChanged(QString)), tempBookmark, SLOT(setPort(QString)));
	port->setMaximum(65535);
	port->setMinimum(1);
	port->setValue(21);

	cmdConnect = new QPushButton(tr("Co&nnect"));
	connect(cmdConnect, SIGNAL(clicked()), this, SLOT(accept()));
	cmdCancel = new QPushButton(tr("C&ancel"));
	connect(cmdCancel, SIGNAL(clicked()), this, SLOT(reject()));
	buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(cmdConnect);
	buttonLayout->addWidget(cmdCancel);

	mainLayout = new QGridLayout();
	mainLayout->addWidget(lblHost, 0, 0);
	mainLayout->addWidget(lblPort, 1, 0);
	mainLayout->addWidget(lblUserName, 2, 0);
	mainLayout->addWidget(lblPassword, 3, 0);
	mainLayout->addWidget(txtHost, 0, 1);
	mainLayout->addWidget(port, 1, 1);
	mainLayout->addWidget(txtUserName, 2, 1);
	mainLayout->addWidget(txtPassword, 3, 1);
	mainLayout->addLayout(buttonLayout, 4, 0, 1, 2);

	setLayout(mainLayout);
}
