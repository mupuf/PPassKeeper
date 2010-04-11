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

#ifndef QCHECKFIELD_H
#define QCHECKFIELD_H

#include "qabstractformfield.h"
#include <QCheckBox>

class QCheckField : public QAbstractFormField
{
	Q_OBJECT

protected:
	QCheckBox* checkbox;

public:
	QCheckField(QWidget* parent=NULL, QString text=QString());

	virtual void setValue(QVariant value);

	virtual QVariant value();

	virtual QWidget* widget();
};

#endif
