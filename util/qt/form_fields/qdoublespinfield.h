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
	QDoubleSpinField(QWidget* parent=NULL, double min=-0.0, double max=100.0);

	void setValue(QVariant value);

	QVariant value();
	double min();
	double max();

	QWidget* widget();

private slots:
	void onValueUpdated(double newInput);
};

#endif // QSPINFIELD_H
