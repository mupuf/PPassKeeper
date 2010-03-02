#ifndef QTEXTFIELD_H
#define QTEXTFIELD_H

#include "qabstractformfield.h"
#include <QLineEdit>

class QTextField : public QAbstractFormField
{
	Q_OBJECT

protected:
	QLineEdit* edit;

public:
	QTextField(QWidget* parent=NULL, int maxLength=1000);

	virtual void setValue(QVariant value);

	virtual QVariant value();

	virtual QWidget* widget();

private slots:
	void onValueUpdated(QString newInput);
};

#endif // QTEXTFIELD_H
