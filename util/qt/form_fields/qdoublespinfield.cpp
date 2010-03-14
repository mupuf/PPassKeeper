#include "qdoublespinfield.h"

QDoubleSpinField::QDoubleSpinField(QWidget* parent, double min, double max)
{
	spin=new QDoubleSpinBox(parent);
	spin->setRange(min, max);

	connect(spin, SIGNAL(valueChanged(double)), this, SLOT(onValueUpdated(double)));
}

void QDoubleSpinField::setValue(QVariant value)
{
	spin->setValue(value.toDouble());
}

QVariant QDoubleSpinField::value()
{
	return spin->value();
}

double QDoubleSpinField::min()
{
	return spin->minimum();
}

double QDoubleSpinField::max()
{
	return spin->maximum();
}

QWidget* QDoubleSpinField::widget()
{
	return spin;
}

void QDoubleSpinField::onValueUpdated(double newInput)
{
	onInputChanged(newInput);
}
