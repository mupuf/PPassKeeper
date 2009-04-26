#ifndef PASSWORDLISTMODEL_H
#define PASSWORDLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVector>

#include <ppasskeeper.h>

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

	ppk_entry_type currentSelectedType();

signals:
	void appPasswordSelected(const char *app_name, const char *username);
	void netPasswordSelected(const char *host, const char *login, unsigned short int post);
	void itemPasswordSelected(const char *key);
	void noItemSelected();

public slots:
	void useFilter(bool usefilter);
	void setFilter(QString filter);

private slots:
	void rowSelected(const QModelIndex &current, const QModelIndex &previous);

private:
	void freeEntries();
	void updateFilter();
	bool filterAccept(QString entry);

	//PPassKeeper data
	ppk_entry *net_ent;
	ppk_entry *app_ent;
	ppk_entry *item_ent;
	unsigned int net_count, app_count, item_count;
	ppk_entry_type currentType;

	//Filter
	bool usefilter;
	QString filter;
	QVector<QString> v_app, v_net, v_item;
	QVector<ppk_entry> e_app, e_net, e_item;
};

#endif
