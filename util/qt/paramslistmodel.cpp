#include "paramslistmodel.h"

ParamsListModel::ParamsListModel(QObject *parent)
	: QAbstractItemModel(parent)
{
}

ParamsListModel::~ParamsListModel()
{
	freeEntries();
}

void ParamsListModel::rowSelected(const QModelIndex &current, const QModelIndex &/*previous*/)
{
/*	if (current.internalId() == appChildId)
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
	}*/
}

inline void ParamsListModel::freeEntries()
{
	params.clear();
}

void ParamsListModel::setupModelData(const char *moduleId)
{
	freeEntries();

	ppk_proto_param* p_list=ppk_availableParameters(moduleId);

	for(int i=0;p_list!=NULL && p_list[i].expected_type!=cvariant_none;i++)
	{
		Param p;

		p.key_name=p_list[i].name;
		p.help_text=p_list[i].help_text;

		//Get the value of the parameter
		cvariant value=ppk_module_get_param(moduleId, p.key_name.toAscii().data());

		//Depending on the expected type
		if(p_list[i].expected_type==cvariant_string)
		{
			p.type=QVariant::String;

			p.default_value=cvariant_get_string(p_list[i].default_value);
			p.value=cvariant_get_string(value);

			params.push_back(p);
		}
		else if(p_list[i].expected_type==cvariant_int)
		{
			p.type=QVariant::Int;

			p.default_value=cvariant_get_int(p_list[i].default_value);
			p.value=cvariant_get_int(value);

			params.push_back(p);
		}
		else if(p_list[i].expected_type==cvariant_float)
		{
			p.type=QVariant::Double;

			p.default_value=cvariant_get_float(p_list[i].default_value);
			p.value=cvariant_get_float(value);

			params.push_back(p);
		}
	}

	emit layoutChanged();
}

QModelIndex ParamsListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (! hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex ParamsListModel::parent(const QModelIndex &index) const
{
	if (! index.isValid())
		return QModelIndex();

	return createIndex(0, 0);
}

int ParamsListModel::rowCount(const QModelIndex &parent) const
{
	return params.size();
}

int ParamsListModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 2;
}

Qt::ItemFlags ParamsListModel::flags(const QModelIndex& index) const
{
	if(index.isValid())
	{
		if(index.column()==1)
			return Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable;
	}

	return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

bool ParamsListModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (! index.isValid() || index.column()!=1 || index.row() > params.size())
		return false;
	else
	{
		params[index.row()].value=value;

		emit dataChanged(index, index);

		return true;
	}
}

QVariant ParamsListModel::data(const QModelIndex &index, int role) const
{
	Param p=params[index.row()];

	if (role == Qt::DisplayRole)
	{
/*		if (index.parent().isValid())
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
		}*/

		if(index.column()==0)
			return p.key_name;
		else if(index.column()==1)
			return p.value;

		return QVariant();
	}
	else if (role == Qt::ToolTipRole)
	{
		return p.help_text;
	}
	else
		return QVariant();
}

QVariant ParamsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		if(section == 0)
			return QVariant(tr("Key"));
		else if(section == 1)
			return QVariant(tr("Value"));
	}

	return QVariant();
}
