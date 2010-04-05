#ifndef QCHECKFIELD_H
#define QCHECKFIELD_H

#include "qabstractformfield.h"
#include <QCheckBox>

class QCheckField : public QAbstractFormField
{
	Q_OBJECT

protected:
	QCheckBox* checkbox;

public:
	QCheckField(QWidget* parent=NULL, QString text=QString());

	virtual void setValue(QVariant value);

	virtual QVariant value();

	virtual QWidget* widget();
};

#endif
