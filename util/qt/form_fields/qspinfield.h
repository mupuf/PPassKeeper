#ifndef QSPINFIELD_H
#define QSPINFIELD_H

#include "qabstractformfield.h"
#include <QSpinBox>

class QSpinField : public QAbstractFormField
{
	Q_OBJECT
private:
	QSpinBox* spin;

public:
	QSpinField(QWidget* parent=NULL, int min=-2147483648, int max=2147483647);

	void setValue(QVariant value);

	QVariant value();

	QWidget* widget();

private slots:
	void onValueUpdated(int newInput);
};

#endif // QSPINFIELD_H
