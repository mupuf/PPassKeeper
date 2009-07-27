#ifndef QDOUBLESPINFIELD_H
#define QDOUBLESPINFIELD_H

#include "qabstractformfield.h"
#include <QDoubleSpinBox>

class QDoubleSpinField : public QAbstractFormField
{
	Q_OBJECT
private:
	QDoubleSpinBox* spin;

public:
	QDoubleSpinField(QWidget* parent=NULL, double min=-2147483648, double max=2147483647);

	void setValue(QVariant value);

	QVariant value();

	QWidget* widget();

private slots:
	void onValueUpdated(double newInput);
};

#endif // QSPINFIELD_H
