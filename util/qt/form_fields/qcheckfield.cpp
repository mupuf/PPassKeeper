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
