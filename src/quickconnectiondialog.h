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

// TODO save history (last hosts etc. used in this dialog)
// TODO quick connected sessions should get the possiblity to add a bookmark
// TODO in a session which is connected through a bookmark we should have the possiblity to change path

#ifndef QUICKCONNECTIONDIALOG_H
#define QUICKCONNECTIONDIALOG_H

#include <QDialog>

class Bookmark;

class QLabel;
class QSpinBox;
class QLineEdit;
class QPushButton;
class QGridLayout;
class QHBoxLayout;

/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class QuickConnectionDialog : public QDialog
{

	Q_OBJECT

	public:
		QuickConnectionDialog(QWidget *parent, Bookmark *tempBookmark);

	private:
		void initGui(const Bookmark *tempBookmark);

		QGridLayout *mainLayout;
		QHBoxLayout *buttonLayout;

		QLabel *lblHost;
		QLabel *lblPort;
		QLabel *lblUserName;
		QLabel *lblPassword;
		QLineEdit *txtHost;
		QSpinBox *port;
		QLineEdit *txtUserName;
		QLineEdit *txtPassword;
		QPushButton *cmdConnect;
		QPushButton *cmdCancel;
};

#endif
