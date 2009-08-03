#ifndef EDITPARAMS_H
#define EDITPARAMS_H

#include <QtGui/QDialog>
#include <QAbstractButton>
#include <QList>
#include <QMap>
#include <QGridLayout>
#include <QTabWidget>
#include "ppasskeeper.h"

#include "form_fields/qabstractformfield.h"

namespace Ui {
    class EditParams;
}

class EditParams : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(EditParams)
public:
	explicit EditParams(QWidget *parent = 0);
	virtual ~EditParams();

	void setModule(QString module_name);

protected:
	virtual void changeEvent(QEvent *e);

private slots:
	void buttonBoxClicked(QAbstractButton* bbox);

private:
	Ui::EditParams *m_ui;
	QTabWidget* catTab;
	QLatin1String module_id;

	QList<QAbstractFormField*> fields;
	QMap<QString, QGridLayout*> categories;

	QString createNameString(ppk_proto_param* pparam);
	QAbstractFormField* abstractFormFieldFromParamProto(QWidget* parent, ppk_proto_param* pparam);
	void addParam(QWidget* parent, QGridLayout* layout, ppk_proto_param* pparam);
	QWidget* addCategory(QTabWidget* catTab, ppk_settings_group* categ);

	void saveParam();
};

#endif // EDITPARAMS_H
