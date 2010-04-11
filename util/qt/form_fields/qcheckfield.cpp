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

#include "qcheckfield.h"
#include <QVariant>
#include <QStyle>

QCheckField::QCheckField(QWidget* parent, QString text) : QAbstractFormField()
{
	checkbox=new QCheckBox(parent);
	checkbox->setText(text);
}

void QCheckField::setValue(QVariant value)
{
	if(value.canConvert(QVariant::Bool))
		checkbox->setChecked(value.toBool());
}

QVariant QCheckField::value()
{
	return QVariant(checkbox->isChecked());
}

QWidget* QCheckField::widget()
{
	return checkbox;
}
