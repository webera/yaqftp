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

// TODO keep track of all actions

#include <QIcon>

#include "ftpmodel.h"

FtpModel::FtpModel(QObject *parent, Bookmark *bookmark)
 : FileModel(parent), ftp(NULL)
{
	FileModel::bookmark = bookmark;
	currentFiles = new QHash<int, QFile *>();
	reconnect();
}

FtpModel::~FtpModel()
{
	if(ftp != NULL) {
		if(ftp->state() != QFtp::Unconnected)
			ftp->close();
		delete ftp;
	}
}

QFtp *FtpModel::ftpClient()
{
	if(ftp == NULL) {
		ftp = new QFtp();
		// object-connect suff
		connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(ftpStateChanged(int)));
		connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStarted(int)));
		connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(ftpCommandFinished(int, bool)));
		// connect(ftp, SIGNAL(done(bool)), this, SLOT(ftpDone(bool)));
		connect(ftp, SIGNAL(rawCommandReply(int ,const QString&)),
				this, SLOT(ftpRawCommandReply(int ,const QString&)));
		connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)),
				this, SLOT(ftpDataTransferProgress(qint64, qint64)));
		connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(ftpListInfo(const QUrlInfo &)));
	}
	return ftp;
}

void FtpModel::reconnect()
{
	ushort port = bookmark->getPort().toUShort();
	ftpClient()->connectToHost(bookmark->getHost(), port);
	ftpClient()->login(bookmark->getUserName(), bookmark->getPassword());
	initPwdCommandId = ftpClient()->rawCommand(QString("pwd"));
}

void FtpModel::setCurrentDirectory(QString& newDir)
{
	currentDir = newDir;
	qDebug() << "seturrentDirectory: " << newDir;
	if(files != NULL) {
		int size = files->size();
		for(int i = 0; i < size; i++) {
			delete files->at(i);
		}
		delete files;
	}
	files = new QList<ModelItem *>();
	ftpClient()->cd(newDir);

	// we need only the name and the information that it is a directory
	QUrlInfo dotDot(QString(".."), 0, "", "", 4096, QDateTime(), QDateTime(), 
					true, false, false, false, true, false);
	files->append(new FtpModelItem(dotDot));
	aktListId = ftpClient()->list();
}

void FtpModel::ftpCommandStarted(int id)
{
	if(ftpClient()->currentCommand() == QFtp::Get || ftpClient()->currentCommand() == QFtp::Put) {
		currentTransferId = id;
		emit transferStarted(id, ftpClient()->currentCommand());
	}
	qDebug() << "commandStarted - id: " << id;
}

void FtpModel::ftpRawCommandReply(int replyCode, const QString &detail)
{
	QString path = detail;
	int currentId = ftpClient()->currentId();
	if(replyCode == 257 && currentId == initPwdCommandId) { // initial pwd
		// parse path from detail.
		// TODO perhaps this can bin done cooler :) - (regex)
		int index = path.indexOf(QString("\""));
		path.remove(0, index + 1);
		index = path.indexOf(QString("\""));
		path.remove(index, detail.size());
		setCurrentDirectory(path);
	}
	qDebug() << "replyCode: " << replyCode << " / detail: " << detail;
}

void FtpModel::ftpCommandFinished(int id, bool error)
{
	qDebug() << "commandFinished id: " << id << " error: " << error;
	if(id == aktListId) {
		reset();
	} else {
		 QFtp::Command command = ftpClient()->currentCommand();
		if(command == QFtp::Get || command == QFtp::Put) {
			if(currentFiles->contains(id)) {
				QFile *file = currentFiles->take(id);
				if(file->isOpen()) {
					qDebug() << "close File: " << file->fileName();
					file->close();
				}
				delete file;
				file = NULL;
				// TODO create a option if this should happen or only after complete done
				if(command == QFtp::Put) { setCurrentDirectory(currentDir); }
			}
			emit transferFinished(id, !error);
		}
	}
	if(error) {
		QString errorMessage = ftpClient()->errorString();
		qDebug() << "Errormessage: " << errorMessage;
		emit ftpErrorOccurred(errorMessage);
	}
}

void FtpModel::ftpDataTransferProgress(qint64 done, qint64 total)
{
	emit transferProgress(currentTransferId, done, total);
}

void FtpModel::ftpListInfo(const QUrlInfo &info)
{
	if(info.isValid() && info.isReadable() && ftpClient()->currentId() == aktListId) {
		files->append(new FtpModelItem(info));
	}
}

void FtpModel::mkdir(const QString& newDirName)
{
	ftpClient()->mkdir(newDirName);
	setCurrentDirectory(currentDir);
}

void FtpModel::ftpStateChanged(int state)
{
	QString message;
	switch(state) {
		case QFtp::Unconnected:
			message = tr("No connection to host");
			break;
		case QFtp::HostLookup:
			message = tr("Looking up host");
			break;
		case QFtp::Connecting:
			message = tr("Connection to host");
			break;
		case QFtp::Connected:
			message = tr("Connection established");
			break;
		case QFtp::LoggedIn:
			message = tr("Sucessfully logged in");
			break;
		case QFtp::Closing:
			message = tr("Closing connection");
			break;
	}
	// perhaps we have to emite the integer value state too
	emit connectionStateChanged(message);
}

// TODO refresh view
void FtpModel::uploadFiles(QModelIndexList& toTransfer)
{
	int count = toTransfer.count();
	qDebug() << count;
	for(int i = 0; i < count; i += 8) { // 8 = columnCount very dirty -> perhaps QItemSelectionModel::Rows
		QModelIndex aktModelIndex = toTransfer.at(i);
		FileModelItem *fileModel = static_cast<FileModelItem *>(aktModelIndex.internalPointer());
		QFileInfo aktFileInfo = fileModel->getInfo();
		QFile *aktFile = new QFile(aktFileInfo.absoluteFilePath());
		if(aktFile->exists()) {
			qDebug() << "path: " << aktFileInfo.absoluteFilePath();
			QString fileName = aktFileInfo.fileName();
			int commandId = ftpClient()->put(aktFile, fileName);
			emit queryUpload(commandId, fileName);
			currentFiles->insert(commandId, aktFile);
		}
	}
}

// TODO check local file -> overwrite / append / skip if it exists
void FtpModel::downloadFiles(QModelIndexList& toTransfer, QString path)
{
	qDebug() << "downloadFiles";
	int count = toTransfer.count();
	qDebug() << count;
	for(int i = 0; i < count; i += 8) { // 8 = columnCount very dirty -> perhaps QItemSelectionModel::Rows
		QModelIndex aktModelIndex = toTransfer.at(i);
		FtpModelItem *fileModel = static_cast<FtpModelItem *>(aktModelIndex.internalPointer());
		QUrlInfo aktUrlInfo = fileModel->getInfo();
		QString fileName = aktUrlInfo.name();
		qDebug() << "File: " << fileName;
		QFile *toSave = new QFile(path + "/" + fileName);
		toSave->open(QIODevice::WriteOnly);
		int commandId = ftpClient()->get(fileName, toSave);
		emit queryDownload(commandId, fileName);
		currentFiles->insert(commandId, toSave);
	}
}

// TODO check local file -> overwrite / append / skip if it exists
// TODO only remove the item when remove finished successfully (need a structure to hold information
void FtpModel::deleteSelected(QModelIndexList& toDelete)
{
	int count = toDelete.count();
	qDebug() << "FtpModel - deleteSelected. Count: " << count;
	for(int i = 0; i < count; i += 8) {
		QModelIndex aktIndex = toDelete.at(i);
		FtpModelItem *fileItem = static_cast<FtpModelItem *>(aktIndex.internalPointer());
		QUrlInfo aktUrlInfo = fileItem->getInfo();
		qDebug() << "Filename: " << aktUrlInfo.name();
		ftpClient()->remove(aktUrlInfo.name());
		files->takeAt(aktIndex.row());
	}
	reset();
}

QVariant FtpModelItem::data(int column) const
{
	switch(column) {
		case 0:
			return info.name();
			break;
		case 1:
			return info.size();
			break;
		case 2:
			if(info.isDir()) {
				return tr("Directory");
			}
			else {
				return tr("File");
			}
			break;
		case 3:
			return info.lastModified().toString();
			break;
		case 4:
			return info.lastModified().toString();
			break;
		case 5:
			return info.owner();
			break;
		case 6:
			return info.group();
			break;
		case 7:
			return createRightsString();
			break;
		default:
			return QVariant();
	}
}

QString FtpModelItem::createRightsString() const
{
	QString permissionString;
	int permissions = info.permissions();
	permissionString += permissions & QUrlInfo::ReadOwner ? 'r' : '-';
	permissionString += permissions & QUrlInfo::WriteOwner ? 'w' : '-';
	permissionString += permissions & QUrlInfo::ExeOwner ? 'x' : '-';

	permissionString += permissions & QUrlInfo::ReadGroup ? 'r' : '-';
	permissionString += permissions & QUrlInfo::WriteGroup ? 'w' : '-';
	permissionString += permissions & QUrlInfo::ExeGroup ? 'x' : '-';

	permissionString += permissions & QUrlInfo::ReadOther ? 'r' : '-';
	permissionString += permissions & QUrlInfo::WriteOther ? 'w' : '-';
	permissionString += permissions & QUrlInfo::ExeOther ? 'x' : '-';
	return permissionString;
}
