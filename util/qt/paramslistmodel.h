#ifndef PARAMSLISTMODEL_H
#define PARAMSLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVector>

#include <ppasskeeper.h>

class ParamsListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	ParamsListModel(QObject *parent = 0);
	virtual ~ParamsListModel();
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	Qt::ItemFlags flags ( const QModelIndex & index ) const;

	void setupModelData(const char *moduleId);

private slots:
	void rowSelected(const QModelIndex &current, const QModelIndex &previous);

private:
	struct Param
	{
		QString key_name;
		QString help_text;
		QVariant value;
		QVariant default_value;
		QVariant::Type type;
	};

	QVector<Param> params;

	void freeEntries();
};

#endif
