/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PASSWORDLISTMODEL_H
#define PASSWORDLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVector>

#include <ppasskeeper.h>

class PasswordListModel : public QAbstractItemModel
{
	Q_OBJECT

private:
	//PPassKeeper data
	ppk_entry **entries;
	size_t entry_count;
	ppk_entry_type currentType;
	QString currentEntry;

	//Filter
	bool usefilter;
	QString filter;
	QVector<QString> v_app, v_net, v_item;
	QVector<const ppk_entry*> e_app, e_net, e_item;

	void freeEntries();
	void updateFilter();
	bool filterAccept(QString entry);

public:
	PasswordListModel(QObject *parent = 0);
	virtual ~PasswordListModel();
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	QModelIndex find(const QString& name) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	void setupModelData(const char* moduleId);

	ppk_entry_type selectedType();
	QString selectedEntry();

private slots:
	void rowSelected(const QModelIndex &current, const QModelIndex &previous);

public slots:
	void useFilter(bool usefilter);
	void setFilter(QString filter);

signals:
	void onPasswordSelected(const char* key);
	void appPasswordSelected(const char *app_name, const char *username);
	void netPasswordSelected(const char *host, const char *login, unsigned short int post);
	void itemPasswordSelected(const char *key);
	void noItemSelected();
};

#endif
