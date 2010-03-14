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
	QSpinField(QWidget* parent=NULL, int min=0, int max=100);

	void setValue(QVariant value);

	QVariant value();
	int min();
	int max();

	QWidget* widget();

private slots:
	void onValueUpdated(int newInput);
};

#endif // QSPINFIELD_H
