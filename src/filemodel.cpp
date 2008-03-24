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

// TODO test on M$ Windows - can get trouble on this OS
// TODO keep in mind that for sorting the .. - entry should not be affected


#include <QIcon>

#include "filemodel.h"

// TODO can cause recursive dependence (include)
#include "ftpmodel.h"

FileModel::FileModel(QObject *parent, Bookmark *bookmark)
 : QAbstractItemModel(parent), cols(8), rows(0), currentDirectory(NULL), files(NULL)
{
	QDir dir = QDir::home();
	QString localPath = bookmark->getLocalPath();

	if(localPath.size() > 0) {
		QDir testDir(localPath);
		if(testDir.exists()) {
			dir = testDir;
		}
	}
	setCurrentDirectory(dir);
}

void FileModel::setCurrentDirectory(QDir newDir)
{
	currentDirectory = newDir;
	directoryChanged();
	reset();
}

void FileModel::directoryChanged()
{
	// clear old list
	if(files != NULL)
	{
		int count = files->count();
		for(int i = 0; i < count; i++)
		{
			delete files->at(i);
		}
		delete files;
	}

	// TODO perhaps get all entrys but make not readable entrys disabled
	QFileInfoList fileInfos = currentDirectory.entryInfoList(QDir::AllEntries | QDir::Readable);

	int count = fileInfos.count();
	files = new QList<ModelItem *>();
	for(int i = 0; i < count; i++)
	{
		QFileInfo info = fileInfos.at(i);
		if(info.fileName() != QString("."))
		{
			files->append(new FileModelItem(info));
		}
	}
	rows = files->count();
}

QModelIndex FileModel::index(int row, int column, const QModelIndex &parent) const
{
	if(files != NULL) {
		if(row < files->count()) {
			ModelItem *file = files->at(row);
			QModelIndex modelIndex = createIndex(row, column, file);
			return modelIndex;
		}
	}
	return QModelIndex();
}

bool FileModel::dropMimeData(const QMimeData *data, Qt::DropAction action, 
							 int row, int column, const QModelIndex &parent)
{
	qDebug() << "dropMimeData " << data->text() << " / row: " << row << " / column: " << column;
}

QVariant FileModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole)	{
		return files->at(index.row())->data(index.column());
	} else {
		if(role == Qt::DecorationRole) {
			ModelItem *item = static_cast<ModelItem *>(index.internalPointer());
			if(index.column() == 0) {
				if(item->isDirectory()) {
					return QVariant::fromValue(QImage(":/images/folder.png"));
				} else {
					QString fileName = item->name();
					if(fileName.endsWith(QString(".gz"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/gz.png"));
					}
					if(fileName.endsWith(QString(".zip"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/zip.png"));
					}
					if(fileName.endsWith(QString(".tar.gz"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/tar.png"));
					}
					if(fileName.endsWith(QString(".tgz"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/tgz.png"));
					}
					if(fileName.endsWith(QString(".lha"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/lha.png"));
					}
					if(fileName.endsWith(QString(".lhz"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/lhz.png"));
					}
					if(fileName.endsWith(QString(".rar"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/rar.png"));
					}
					if(fileName.endsWith(QString(".rpm"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/rpm.png"));
					}
					if(fileName.endsWith(QString(".mp3"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/mp3.png"));
					}
					if(fileName.endsWith(QString(".ogg"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/ogg.png"));
					}
					if(fileName.endsWith(QString(".wav"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/wav.png"));
					}
					if(fileName.endsWith(QString(".bmp"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/bmp.png"));
					}
					if(fileName.endsWith(QString(".jpg"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/jpg.png"));
					}
					if(fileName.endsWith(QString(".gif"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/gif.png"));
					}
					if(fileName.endsWith(QString(".tiff"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/tiff.png"));
					}
					if(fileName.endsWith(QString(".jpeg"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/jpeg.png"));
					}
					if(fileName.endsWith(QString(".png"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/png.png"));
					}
					if(fileName.endsWith(QString(".java"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/source_java.png"));
					}
					if(fileName.endsWith(QString(".c"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/source_c.png"));
					}
					if(fileName.endsWith(QString(".cpp"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/source_cpp.png"));
					}
					if(fileName.endsWith(QString(".php"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/source_php.png"));
					}
					if(fileName.endsWith(QString(".py"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/source_py.png"));
					}
					if(fileName.endsWith(QString(".pyc"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/source_pyc.png"));
					}
					if(fileName.endsWith(QString(".sh"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/shellscript.png"));
					}
					if(fileName.endsWith(QString(".xcf"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/xcf.png"));
					}
					if(fileName.endsWith(QString(".odt"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/odt.png"));
					}
					if(fileName.endsWith(QString(".pdf"), Qt::CaseInsensitive)) {
						return QVariant::fromValue(QImage(":/images/pdf.png"));
					}
					return QVariant::fromValue(QImage(":/images/unknown.png"));
				}
			}
		}
	}
	return QVariant();
}

Qt::ItemFlags FileModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	if(index.row() > 0)
		flags = flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	return flags;
}

int FileModel::columnCount(const QModelIndex &parent) const
{
	return cols;
}

int FileModel::rowCount(const QModelIndex &parent) const
{
	return rows;
}

Qt::DropActions FileModel::supportedDragActions() const
{
	return Qt::CopyAction;
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole) { return QVariant(); }

    if (orientation == Qt::Horizontal) {
		switch(section) {
			case NAME:
				return tr("Name");
				break;
			case SIZE:
				return tr("Size");
				break;
			case TYPE:
				return tr("Type");
				break;
			case CREATED:
				return tr("Created");
				break;
			case CHANGED:
				return tr("Last Modified");
				break;
			case OWNER:
				return tr("Owner");
				break;
			case GROUP:
				return tr("Group");
				break;
			case RIGHTS:
				return tr("Rights");
				break;
			default:
				return tr("Undefined");
		}
	}
	else {
		// ignore row header
		return QVariant();
	}
}

// TODO if file then only delete - if directory - delete recusive
// TODO check before call of this, if everything can be deleted
// TODO this sucks - redesign
void FileModel::deleteSelected(const QModelIndexList& toDelete)
{
	int count = toDelete.count();
	for(int i = 0; (i * COL_COUNT) < count; i++) {
		QModelIndex aktIndex = toDelete.at(i);
		FileModelItem *fileModel = static_cast<FileModelItem *>(aktIndex.internalPointer());
		if(fileModel->isDirectory()) {
			qDebug() << "delete directory: " << fileModel->getInfo().absoluteFilePath();
			QDir aktDir;
			if(!removeDirectory(fileModel->getInfo().absoluteFilePath())) {
				qDebug() << "!!!!!!!! cant remove dir !!!!!!!!!!!!!!";
			} else {
				files->removeAll(fileModel);
				delete fileModel;
				fileModel = NULL;
				rows--;
			}
		} else {
			QFile aktFile(fileModel->getInfo().absoluteFilePath());
			qDebug() << fileModel->getInfo().absoluteFilePath();
			if(!aktFile.remove()) {
				qDebug() << "!!!!!!!! cant remove file: " << aktFile.fileName() << " !!!!!!!!!!!!!!";
			} else {
				files->removeAll(fileModel);
				delete fileModel;
				fileModel = NULL;
				rows--;
			}
		}
	}
	reset();
}

bool FileModel::removeDirectory(QString path)
{
	QDir aktDir(path);
	QFileInfoList fileInfos = aktDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	int count = fileInfos.count();
	for(int i = 0; i < count; i++) {
		QFileInfo aktFileInfo = fileInfos.at(i);
		if(aktFileInfo.isDir()) {
			return removeDirectory(aktFileInfo.absoluteFilePath());
		} else {
			QFile aktFile(aktFileInfo.absoluteFilePath());
			if(!aktFile.remove()) return false;
 		}
	}
	return aktDir.rmdir(path);
}

void FileModel::mkdir(const QString& newDirName)
{
	currentDirectory.mkdir(newDirName);
	QFileInfo newInfo(currentDirectory.absolutePath() + "/" + newDirName);
	files->append(new FileModelItem(newInfo));
	rows++;
}

// ---------------------------- FileModelItem -----------------------------------

QVariant FileModelItem::data(int column) const
{
	switch(column)
	{
		case 0:
			return info.fileName();
			break;
		case 1:
			return QString::number(info.size());
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
			return info.created().toString();
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

QString FileModelItem::createRightsString() const
{
	QString permissionString;
	QFile::Permissions permissions = info.permissions();
	permissionString += permissions & QFile::ReadOwner ? 'r' : '-';
	permissionString += permissions & QFile::WriteOwner ? 'w' : '-';
	permissionString += permissions & QFile::ExeOwner ? 'x' : '-';

	permissionString += permissions & QFile::ReadGroup ? 'r' : '-';
	permissionString += permissions & QFile::WriteGroup ? 'w' : '-';
	permissionString += permissions & QFile::ExeGroup ? 'x' : '-';

	permissionString += permissions & QFile::ReadOther ? 'r' : '-';
	permissionString += permissions & QFile::WriteOther ? 'w' : '-';
	permissionString += permissions & QFile::ExeOther ? 'x' : '-';
	return permissionString;
}
