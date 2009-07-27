#include "qtextfield.h"
#include <QVariant>

QTextField::QTextField(QWidget* parent, int maxLength) : QAbstractFormField()
{
	edit=new QLineEdit(parent);
	edit->setMaxLength(maxLength);

	connect(edit, SIGNAL(textEdited(QString)), this, SLOT(onValueUpdated(QString)));
}

void QTextField::onValueUpdated(QString newInput)
{
	onInputChanged(newInput);
}

void QTextField::setValue(QVariant value)
{
	edit->setText(value.toString());
}

QVariant QTextField::value()
{
	return QVariant(edit->text());
}

QWidget* QTextField::widget()
{
	return edit;
}
