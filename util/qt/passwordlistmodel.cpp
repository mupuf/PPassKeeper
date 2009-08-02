#include "passwordlistmodel.h"

#include <ppasskeeper/ppasskeeper-key.h>

static const quint32 appId = 0;
static const quint32 netId = 1;
static const quint32 itemId = 2;
static const quint32 appChildId = 3;
static const quint32 netChildId = 4;
static const quint32 itemChildId = 5;

PasswordListModel::PasswordListModel(QObject *parent)
	: QAbstractItemModel(parent),
	net_ent(NULL), app_ent(NULL), item_ent(NULL),
	net_count(0), app_count(0), item_count(0)
{
}

PasswordListModel::~PasswordListModel()
{
	freeEntries();
}

void PasswordListModel::rowSelected(const QModelIndex &current, const QModelIndex &/*previous*/)
{
	if (current.internalId() == appChildId)
	{
		ppk_entry &a = e_app[current.row()];
		currentType = ppk_application;
		emit appPasswordSelected(a.app.app_name, a.app.username);
	} else if (current.internalId() == netChildId)
	{
		ppk_entry &n = e_net[current.row()];
		currentType = ppk_network;
		emit netPasswordSelected(n.net.host, n.net.login, n.net.port);
	} else if (current.internalId() == itemChildId)
	{
		ppk_entry &i = e_item[current.row()];
		currentType = ppk_item;
		emit itemPasswordSelected(i.item);
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

ppk_entry_type PasswordListModel::currentSelectedType()
{
	return currentType;
}

inline void PasswordListModel::freeEntries()
{
	delete[] net_ent;
	delete[] app_ent;
	delete[] item_ent;
	net_ent = NULL;
	app_ent = NULL;
	item_ent = NULL;
}

void PasswordListModel::setupModelData(const char *moduleId)
{
	freeEntries();

	net_count = ppk_module_get_entry_count(moduleId, ppk_network, ppk_lf_none);
	app_count = ppk_module_get_entry_count(moduleId, ppk_application, ppk_lf_none);
	item_count = ppk_module_get_entry_count(moduleId, ppk_item, ppk_lf_none);
	if (net_count > 0)
	{
		net_ent = new ppk_entry[net_count];
		net_count=ppk_module_get_entry_list(moduleId, ppk_network, net_ent, net_count, ppk_lf_none);
	}
	if (app_count > 0)
	{
		app_ent = new ppk_entry[app_count];
		app_count=ppk_module_get_entry_list(moduleId, ppk_application, app_ent, app_count, ppk_lf_none);
	}
	if (item_count > 0)
	{
		item_ent = new ppk_entry[item_count];
		item_count=ppk_module_get_entry_list(moduleId, ppk_item, item_ent, item_count, ppk_lf_none);
	}

	updateFilter();
}

bool PasswordListModel::filterAccept(QString entry)
{
	return !usefilter || (usefilter && (filter==QString() || entry.contains(filter, Qt::CaseInsensitive)));
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

	for(unsigned int i=0;i<app_count;i++)
	{
		const ppk_entry &a = app_ent[i];

		if(ppk_get_key(&a, buf,sizeof(buf)-1)==PPK_TRUE)
		{
			if(filterAccept(buf))
			{
				v_app.push_back(buf);
				e_app.push_back(a);
			}
		}
	}

	for(unsigned int i=0;i<net_count;i++)
	{
		const ppk_entry &n = net_ent[i];

		QString entry("%1@%2:%3");
		entry=entry.arg(QString::fromUtf8(n.net.login)).arg(QString::fromUtf8(n.net.host)).arg(n.net.port);

		//Bug : protocol is ill initialised
		/*if(ppk_getKey(&n, buf,sizeof(buf)-1)==PPK_TRUE)
		{*/
			if(filterAccept(entry))
			{
				v_net.push_back(entry);
				e_net.push_back(n);
			}
		//}
	}

	for(unsigned int i=0;i<item_count;i++)
	{
		const ppk_entry &it = item_ent[i];

		if(ppk_get_key(&it, buf,sizeof(buf)-1)==PPK_TRUE)
		{
			if(filterAccept(buf))
			{
				v_item.push_back(buf);
				e_item.push_back(it);
			}
		}
	}

	reset();
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

void PasswordListModel::useFilter(bool usefilter)
{
	this->usefilter=usefilter;
	updateFilter();
}

void PasswordListModel::setFilter(QString filter)
{
	this->filter=filter;
	updateFilter();
}
