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

/*
	TODO keep in mind that we want to use only one model - this model is only created
	to be faster - perhaps make only one model sometimes
*/

#ifndef FTPMODEL_H
#define FTPMODEL_H

#include <QFtp>

#include "filemodel.h"

/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class FtpModelItem : public ModelItem
{
	public:
		FtpModelItem(QUrlInfo info) : info(info) {}

		QVariant data(int column) const;
		bool isDirectory() const { return info.isDir(); }
		QString name() const { return info.name(); }

		QUrlInfo getInfo() const { return info; }

	private:
		QUrlInfo info;

		QString createRightsString() const;
};

// TODO new design crystallize out - no more model methods in this class

/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class FtpModel : public FileModel
{

	Q_OBJECT

	public:
		FtpModel(QObject *parent, Bookmark *bookmark);
		~FtpModel();

		// TODO we need a update method?

		void setCurrentDirectory(QString& newDir);
		QString currentDirectory() { return currentDir; }

		void uploadFiles(QModelIndexList& files);
		void downloadFiles(QModelIndexList& selectedModelIndizes, QString path);

		void deleteSelected(QModelIndexList& toDelete);
		void mkdir(const QString& newDirName);

	signals:
		// TODO perhaps we need only one signal to post a message to the ftpsessiondialog
		void connectionStateChanged(QString& message);
		void ftpErrorOccurred(QString& errorMessage);
		void queryUpload(int id, QString& name);
		void queryDownload(int id, QString& name);
		void transferStarted(int id, int type);
		void transferProgress(int id, qint64 done, qint64 total);
		void transferFinished(int id, bool success);

	private slots:
		void ftpStateChanged(int state);
		void ftpCommandStarted(int id);
		// void ftpDone(bool error);	// called after all commands finished
		void ftpCommandFinished(int id, bool error);
		void ftpRawCommandReply(int replyCode, const QString &detail);
		void ftpDataTransferProgress(qint64 done, qint64 total);
		void ftpListInfo(const QUrlInfo &info);

	private:
		QFtp *ftp;
		QString currentDir;
		int aktListId;
		int initPwdCommandId;
		int currentTransferId;

		QHash<int, QFile *> *currentFiles;

		QFtp *ftpClient();
		void reconnect();
};

#endif
