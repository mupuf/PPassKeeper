/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

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

	void setModule(const char* module);

protected:
	virtual void changeEvent(QEvent *e);

private slots:
	void buttonBoxClicked(QAbstractButton* bbox);

private:
	Ui::EditParams *m_ui;
	QTabWidget* catTab;
	const char* module;

	QList<QAbstractFormField*> fields;
	QMap<QString, QGridLayout*> categories;

	QString createNameString(const ppk_proto_param* pparam);
	QAbstractFormField* abstractFormFieldFromParamProto(QWidget* parent, const ppk_proto_param* pparam);
	void addParam(QWidget* parent, QGridLayout* layout, const ppk_proto_param* pparam);
	QWidget* addCategory(QTabWidget* catTab, const ppk_settings_group* categ);

	void saveParam();
};

#endif // EDITPARAMS_H
