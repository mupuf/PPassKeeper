#ifndef PASSWORDLISTMODEL_H
#define PASSWORDLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>

struct PPassKeeper_module_entry_net;
struct PPassKeeper_module_entry_app;
struct PPassKeeper_module_entry_item;

class PasswordListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	PasswordListModel(QObject *parent = 0);
	virtual ~PasswordListModel();
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	void setupModelData(const char *moduleId);

private:
	void freeEntries();

	//PPassKeeper data
	struct PPassKeeper_module_entry_net *net_ent;
	struct PPassKeeper_module_entry_app *app_ent;
	struct PPassKeeper_module_entry_item *item_ent;
	unsigned int net_count, app_count, item_count;
};

#endif
