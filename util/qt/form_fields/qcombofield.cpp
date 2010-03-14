#include "qcombofield.h"
#include <QVariant>

void QComboField::setList(const char** list)
{
	int i=0;
	while(list!=NULL && list[i]!=NULL)
	{
		combo->addItem(QString::fromUtf8(list[i]));
		i++;
	}
}

QComboField::QComboField(QWidget* parent, const char** list) : QAbstractFormField()
{
	combo=new QComboBox(parent);

	setList(list);

	connect(combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(onValueUpdated(QString)));
}

void QComboField::onValueUpdated(QString newInput)
{
	onInputChanged(newInput);
}

void QComboField::setValue(QVariant value)
{
	combo->setCurrentIndex(combo->findText(value.toString()));
}

QVariant QComboField::value()
{
	return QVariant(combo->currentText());
}

QWidget* QComboField::widget()
{
	return combo;
}
