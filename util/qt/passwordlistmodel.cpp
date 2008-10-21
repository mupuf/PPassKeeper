#include "passwordlistmodel.h"

#include <ppasskeeper.h>

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

void PasswordListModel::rowSelected(const QModelIndex &current, const QModelIndex &previous)
{
	if (current.internalId() == appChildId)
	{
		struct PPassKeeper_module_entry_app &a = app_ent[current.row()];
		emit appPasswordSelected(a.app_name, a.username);
	} else if (current.internalId() == netChildId)
	{
		struct PPassKeeper_module_entry_net &n = net_ent[current.row()];
		emit netPasswordSelected(n.host, n.login, n.port);
	} else if (current.internalId() == itemChildId)
	{
		struct PPassKeeper_module_entry_item &i = item_ent[current.row()];
		emit itemPasswordSelected(i.key);
	}
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

	net_count = ppk_getPasswordListCount(moduleId, ppk_network, 0);
	app_count = ppk_getPasswordListCount(moduleId, ppk_application, 0);
	item_count = ppk_getPasswordListCount(moduleId, ppk_item, 0);
	if (net_count > 0)
	{
		net_ent = new struct PPassKeeper_module_entry_net[net_count];
		ppk_getPasswordList(moduleId, ppk_network, net_ent, net_count, 0);
	}
	if (app_count > 0)
	{
		app_ent = new struct PPassKeeper_module_entry_app[app_count];
		ppk_getPasswordList(moduleId, ppk_application, app_ent, app_count, 0);
	}
	if (item_count > 0)
	{
		item_ent = new struct PPassKeeper_module_entry_item[app_count];
		ppk_getPasswordList(moduleId, ppk_item, item_ent, item_count, 0);
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
			return app_count;
		case netId:
			return net_count;
		case itemId:
			return item_count;
		case appChildId:
		case netChildId:
		case itemChildId:
			return 0;
		}
	}
	Q_ASSERT(false);
	return 0;
}

int PasswordListModel::columnCount(const QModelIndex &parent) const
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
			{
				const PPassKeeper_module_entry_app &a = app_ent[index.row()];
				return QVariant(QString("%1:%2").arg(a.app_name).arg(a.username));
			} else if (index.internalId() == netChildId)
			{
				const PPassKeeper_module_entry_net &n = net_ent[index.row()];
				return QVariant(QString("%1@%2:%3").arg(n.login).arg(n.host).arg(n.port));
			} else if (index.internalId() == itemChildId)
			{
				const PPassKeeper_module_entry_item &i = item_ent[index.row()];
				return QVariant(QString(i.key));
			}
		} else {
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
