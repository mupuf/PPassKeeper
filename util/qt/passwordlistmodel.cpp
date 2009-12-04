#include "passwordlistmodel.h"

#include <ppasskeeper/key.h>

static const quint32 appId = 0;
static const quint32 netId = 1;
static const quint32 itemId = 2;
static const quint32 appChildId = 3;
static const quint32 netChildId = 4;
static const quint32 itemChildId = 5;

inline void PasswordListModel::freeEntries()
{
	if(entries)
		ppk_module_free_entry_list(entries);

	entries = NULL;
	entry_count = 0;
}

#include <stdio.h>
void PasswordListModel::updateFilter()
{
	v_app.clear();
	e_app.clear();
	v_net.clear();
	e_net.clear();
	v_item.clear();
	e_item.clear();

	char buf[401];

	for(unsigned int i=0;i<entry_count;i++)
	{
		const ppk_entry* entry = entries[i];

		if(ppk_get_entry_type(entry)==ppk_application)
		{
			if(ppk_get_key(entry, buf,sizeof(buf)-1)==PPK_TRUE)
			{
				if(filterAccept(QString::fromUtf8(buf)))
				{
					v_app.push_back(QString::fromUtf8(buf));
					e_app.push_back(entry);
				}
			}
		}
	}

	for(unsigned int i=0;i<entry_count;i++)
	{
		const ppk_entry* entry = entries[i];

		if(ppk_get_entry_type(entry)==ppk_network)
		{
			if(ppk_get_key(entry, buf,sizeof(buf)-1)==PPK_TRUE)
			{
				if(filterAccept(QString::fromUtf8(buf)))
				{
					v_net.push_back(QString::fromUtf8(buf));
					e_net.push_back(entry);
				}
			}
		}
	}

	for(unsigned int i=0;i<entry_count;i++)
	{
		const ppk_entry* entry = entries[i];

		if(ppk_get_entry_type(entry)==ppk_item)
		{
			if(ppk_get_key(entry, buf,sizeof(buf)-1)==PPK_TRUE)
			{
				if(filterAccept(QString::fromUtf8(buf)))
				{
					v_item.push_back(QString::fromUtf8(buf));
					e_item.push_back(entry);
				}
			}
		}
	}

	reset();
	emit noItemSelected();
}

bool PasswordListModel::filterAccept(QString entry)
{
	return !usefilter || (usefilter && (filter==QString() || entry.contains(filter, Qt::CaseInsensitive)));
}



//Public
PasswordListModel::PasswordListModel(QObject *parent)
	: QAbstractItemModel(parent),
	entries(NULL),
	entry_count(0)
{
}

PasswordListModel::~PasswordListModel()
{
	freeEntries();
}

QModelIndex PasswordListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (! hasIndex(row, column, parent))
		return QModelIndex();

	if (! parent.isValid())
	{
		switch (row)
		{
		case 0:
			return createIndex(row, column, appId);
		case 1:
			return createIndex(row, column, netId);
		case 2:
			return createIndex(row, column, itemId);
		}
	} else {
		switch (parent.internalId())
		{
		case appId:
			return createIndex(row, column, appChildId);
		case netId:
			return createIndex(row, column, netChildId);
		case itemId:
			return createIndex(row, column, itemChildId);
		}
	}

	return QModelIndex();
}

QModelIndex PasswordListModel::parent(const QModelIndex &index) const
{
	if (! index.isValid())
		return QModelIndex();

	if (index.internalId() == appId
			|| index.internalId() == netId
			|| index.internalId() == itemId)
	{
		/* it is a top level item */
		return QModelIndex();
	} else {
		switch (index.internalId())
		{
		case appChildId:
			return createIndex(0, 0, appId);
		case netChildId:
			return createIndex(1, 0, netId);
		case itemChildId:
			return createIndex(2, 0, itemId);
		}
	}

	return QModelIndex();
}

QModelIndex PasswordListModel::find(const QString& name) const
{
	//is it an application entry ?
	for(int i=0;i<v_app.size();i++)
		if(v_app[i]==name)
			return index(i, 0, index(0, 0, QModelIndex()));

	//is it a network entry ?
	for(int i=0;i<v_net.size();i++)
		if(v_net[i]==name)
			return index(i, 0, index(1, 0, QModelIndex()));

	//is it an item entry ?
	for(int i=0;i<v_item.size();i++)
		if(v_item[i]==name)
			return index(i, 0, index(2, 0, QModelIndex()));

	return QModelIndex();
}

int PasswordListModel::rowCount(const QModelIndex &parent) const
{
	if (! parent.isValid())
		return 3;
	else
	{
		switch (parent.internalId())
		{
		case appId:
			return v_app.size();
		case netId:
			return v_net.size();
		case itemId:
			return v_item.size();
		case appChildId:
		case netChildId:
		case itemChildId:
			return 0;
		}
	}
	Q_ASSERT(false);
	return 0;
}

int PasswordListModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 1;
}

QVariant PasswordListModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (index.parent().isValid())
		{
			if (index.internalId() == appChildId)
				return QVariant(v_app[index.row()]);
			else if (index.internalId() == netChildId)
				return QVariant(v_net[index.row()]);
			else if (index.internalId() == itemChildId)
				return QVariant(v_item[index.row()]);
		}
		else
		{
			if (index.row() == 0)
				return QVariant(tr("Application"));
			else if (index.row() == 1)
				return QVariant(tr("Network"));
			else if (index.row() == 2)
				return QVariant(tr("Item"));
		}
		return QVariant();
	}
	else
		return QVariant();
}

QVariant PasswordListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && section == 0 && orientation == Qt::Horizontal)
		return QVariant(tr("Key"));
	return QVariant();
}

#include <QMessageBox>
void PasswordListModel::setupModelData(const char* moduleId)
{
	freeEntries();

	ppk_error list_error=ppk_module_get_entry_list(moduleId, ppk_network|ppk_application|ppk_item, &entries, &entry_count, ppk_lf_none);
	if(list_error!=PPK_OK && list_error!=PPK_UNSUPPORTED_METHOD)
	{
		if(moduleId!=NULL && strcmp(moduleId, "")!=0)
			QMessageBox::critical(NULL, tr("PPassKeeper: Listing Error"), QString::fromUtf8("%1:\n\n%2").arg(tr("Error while listing entries")).arg(QString::fromUtf8(ppk_error_get_string(list_error))));
	}

	updateFilter();
}

ppk_entry_type PasswordListModel::selectedType()
{
	return currentType;
}

QString PasswordListModel::selectedEntry()
{
	return currentEntry;
}

void PasswordListModel::rowSelected(const QModelIndex &current, const QModelIndex &/*previous*/)
{
	//Update the current selected entry
	if(current.internalId() != netId &&
	   current.internalId() != appId &&
	   current.internalId() != itemId)
		currentEntry=current.data().toString();
	else
		currentEntry=QString();

	//send the update
	emit onPasswordSelected(qPrintable(selectedEntry()));

	//Send the specialised events
	if (current.internalId() == appChildId)
	{
		const ppk_entry* a = e_app[current.row()];
		currentType = ppk_application;
		emit appPasswordSelected(a->app.app_name, a->app.username);
	} else if (current.internalId() == netChildId)
	{
		const ppk_entry* n = e_net[current.row()];
		currentType = ppk_network;
		emit netPasswordSelected(n->net.host, n->net.login, n->net.port);
	} else if (current.internalId() == itemChildId)
	{
		const ppk_entry* i = e_item[current.row()];
		currentType = ppk_item;
		emit itemPasswordSelected(i->item);
	}
	else if (current.internalId() == netId)
	{
		currentType = ppk_network;
		emit noItemSelected();
	}
	else if (current.internalId() == appId)
	{
		currentType = ppk_application;
		emit noItemSelected();
	}
	else if (current.internalId() == itemId)
	{
		currentType = ppk_item;
		emit noItemSelected();
	}
}

void PasswordListModel::useFilter(bool usefilter)
{
	this->usefilter=usefilter;
	updateFilter();
}

void PasswordListModel::setFilter(QString filter)
{
	this->filter=filter;
	if(usefilter)
		updateFilter();
}
