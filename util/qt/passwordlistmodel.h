#ifndef PASSWORDLISTMODEL_H
#define PASSWORDLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>

struct ppk_entry_net;
struct ppk_entry_app;

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

signals:
	void appPasswordSelected(const char *app_name, const char *username);
	void netPasswordSelected(const char *host, const char *login, unsigned short int post);
	void itemPasswordSelected(const char *key);

private slots:
	void rowSelected(const QModelIndex &current, const QModelIndex &previous);

private:
	void freeEntries();

	//PPassKeeper data
	struct ppk_entry *net_ent;
	struct ppk_entry *app_ent;
	struct ppk_entry *item_ent;
	unsigned int net_count, app_count, item_count;
};

#endif
