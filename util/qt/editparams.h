#ifndef EDITPARAMS_H
#define EDITPARAMS_H

#include <QtGui/QDialog>
#include <QAbstractButton>
#include <QList>
#include <QMap>
#include <QGridLayout>
#include <QTabWidget>
#include <ppasskeeper.h>

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

	void setModule(ppk_module* module);

protected:
	virtual void changeEvent(QEvent *e);

private slots:
	void buttonBoxClicked(QAbstractButton* bbox);

private:
	Ui::EditParams *m_ui;
	QTabWidget* catTab;
	ppk_module* module;

	QList<QAbstractFormField*> fields;
	QMap<QString, QGridLayout*> categories;

	QString createNameString(const ppk_proto_param* pparam);
	QAbstractFormField* abstractFormFieldFromParamProto(QWidget* parent, const ppk_proto_param* pparam);
	void addParam(QWidget* parent, QGridLayout* layout, const ppk_proto_param* pparam);
	QWidget* addCategory(QTabWidget* catTab, const ppk_settings_group* categ);

	void saveParam();
};

#endif // EDITPARAMS_H
