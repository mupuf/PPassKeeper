#ifndef QTEXTFIELD_H
#define QTEXTFIELD_H

#include "qabstractformfield.h"
#include <QLineEdit>

class QTextField : public QAbstractFormField
{
	Q_OBJECT
private:
	QLineEdit* edit;

public:
	QTextField(QWidget* parent=NULL, int maxLength=1000);

	void setValue(QVariant value);

	QVariant value();

	QWidget* widget();

private slots:
	void onValueUpdated(QString newInput);
};

#endif // QTEXTFIELD_H
