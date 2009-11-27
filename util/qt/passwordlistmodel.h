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
