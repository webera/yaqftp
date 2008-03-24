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


#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QObject>

//! Bookmark representation
/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class Bookmark : public QObject
{

	Q_OBJECT

	public:
		//! Default ctor
		Bookmark() : name(QString("New Ftp")), host(QString("ftp.newhost.org")), 
					 port(QString("21")), userName(QString("anonymous")),
					 password(QString("anonymous@gmail.com")), 
					 localPath(QString("")), remotePath(QString("")) { }

		//! Init ctor
		Bookmark(QString name, QString host, QString port, QString userName, 
				 QString password, QString localPath, QString remotePath) : name(name), host(host),
				 port(port), userName(userName), password(password),
				 localPath(localPath), remotePath(remotePath) { }

		Bookmark(Bookmark *copy) : name(copy->getName()), host(copy->getHost()), port(copy->getPort()),
					userName(copy->getUserName()), password(copy->getPassword()), 
					localPath(copy->getLocalPath()), remotePath(copy->getRemotePath()) { }


		//! @return Bookmarks name
		QString getName() const { return name; }
		//! @return Hostname
		QString getHost() const { return host; }
		//! @return Port to connect
		QString getPort() const { return port; }
		//! @return Uername
		QString getUserName() const { return userName; }
		//! @return Password (unencrypted)
		QString getPassword() const { return password; }
		//! @return Local directory which is set at startup of connection
		QString getLocalPath() const { return localPath; }
		//! @return Remote directory which is set directly after connection setup
		QString getRemotePath() const { return remotePath; }

		void setName(QString newName) { name = newName; }

	public slots:
		void setHost(const QString newHost) { host = newHost; }
		void setPort(const QString newPort) { port = newPort; }
		void setUserName(const QString newUserName) { userName = newUserName; }
		void setPassword(const QString newPassword) { password = newPassword; }
		void setLocalPath(const QString newLocalPath) { localPath = newLocalPath; }
		void setRemotePath(const QString newRemotePath) { remotePath = newRemotePath; }

	private:
		QString name;
		QString host;
		QString port;
		QString userName;
		QString password;
		QString localPath;
		QString remotePath;
};

#endif
