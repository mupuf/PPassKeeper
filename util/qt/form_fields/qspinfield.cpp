#include "qspinfield.h"

QSpinField::QSpinField(QWidget* parent, int min, int max)
{
	spin=new QSpinBox(parent);
	spin->setRange(min, max);

	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(onValueUpdated(int)));
}

void QSpinField::setValue(QVariant value)
{
	spin->setValue(value.toInt());
}


QVariant QSpinField::value()
{
	return spin->value();
}

QWidget* QSpinField::widget()
{
	return spin;
}

void QSpinField::onValueUpdated(int newInput)
{
	onInputChanged(newInput);
}
