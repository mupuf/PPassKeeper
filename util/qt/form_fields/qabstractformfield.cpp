#include "qabstractformfield.h"

#include <QWidget>
#include <QMessageBox>

QAbstractFormField::QAbstractFormField()
{

}

void QAbstractFormField::setFieldName(QString name)
{
	this->name=name;
}

void QAbstractFormField::setDefaultValue(QVariant default_value)
{
	this->default_value=default_value;
}

void QAbstractFormField::setHelpText(QString help_text)
{
	this->help_text=help_text;
}

void QAbstractFormField::setUserData(void* data)
{
	user_data=data;
}

QString QAbstractFormField::fieldName()
{
	return name;
}

QVariant QAbstractFormField::defaultValue()
{
	return default_value;
}

QString QAbstractFormField::helpText()
{
	return help_text;
}

void* QAbstractFormField::userData()
{
	return user_data;
}

void QAbstractFormField::reset()
{
	setValue(default_value);
}

void QAbstractFormField::showHelpText()
{
	QMessageBox::information(widget(), tr("Help on the parameter '%1'").arg(fieldName()),
						tr("Help on the parameter '%1' : %2").arg(fieldName()).arg(helpText()));
}

//Virtual functions, do nothing
bool QAbstractFormField::setWidget(QWidget* /*widget*/)
{
	return true;
}

//Slots
void QAbstractFormField::onInputChanged(QVariant /*inputValue*/)
{

}
