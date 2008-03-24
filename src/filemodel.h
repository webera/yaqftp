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

// TODO check for local changes outside of the ftp client

#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QtCore>

#include "bookmark.h"

enum Column { NAME, SIZE, TYPE, CREATED, CHANGED, OWNER, GROUP, RIGHTS, COL_COUNT };

/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class ModelItem : public QObject {
	public:
		virtual QVariant data(int column) const = 0;
		virtual bool isDirectory() const = 0;
		virtual QString name() const = 0;
};

/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class FileModelItem : public ModelItem
{
	public:
		FileModelItem(QFileInfo info) : info(info) {}

		QVariant data(int column) const;
		bool isDirectory() const { return info.isDir(); }
		QString name() const { return info.fileName(); }

		QFileInfo getInfo() const { return info; }

	private:
		QFileInfo info;

		QString createRightsString() const;
};

/*!
	@author Antonio Weber <antonio.weber@stud.fh-regensburg.de>
*/
class FileModel : public QAbstractItemModel
{

	Q_OBJECT

	public:
 		// TODO redesign this ctor stuff - with ftpmodel in mind
		FileModel(QObject *parent) : QAbstractItemModel(parent), cols(8), files(NULL) {}
    	FileModel(QObject *parent, Bookmark *bookmark);
		virtual ~FileModel() { }

		void update() {  setCurrentDirectory(currentDirectory); }
		void setCurrentDirectory(QDir newDir);

		// overwriting methods
		// http://doc.trolltech.com/4.2/model-view-model-subclassing.html
		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		int columnCount(const QModelIndex & parent = QModelIndex()) const;
		int rowCount(const QModelIndex & parent = QModelIndex()) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const { return QModelIndex(); }
		//bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
		Qt::DropActions supportedDragActions() const;
		bool dropMimeData(const QMimeData *data, Qt::DropAction action, 
					 int row, int column, const QModelIndex & parent);

		QDir currentDir() const { return currentDirectory; }

		virtual void deleteSelected(const QModelIndexList& toDelete);
		virtual void mkdir(const QString& newDirName);

	protected:
		Bookmark *bookmark;
		QList<ModelItem *> *files;

		int cols;
		int rows;

		virtual bool removeDirectory(QString path);

	private:
		void directoryChanged(); // TODO do we really need this?
		

		QDir currentDirectory;
};

#endif
