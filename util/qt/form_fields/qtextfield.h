#ifndef QTEXTFIELD_H
#define QTEXTFIELD_H

#include "qabstractformfield.h"
#include <QLineEdit>
#include <QRegExp>

class QTextField : public QAbstractFormField
{
	Q_OBJECT

private:
	QRegExp rExpValidation;
	QString lastGoodValue;

	bool validate(QString val);
	bool validate();

protected:
	QLineEdit* edit;

public:
	QTextField(QWidget* parent=NULL, QRegExp regExp=QRegExp(), int maxLength=1000);

	virtual void setValue(QVariant value);

	virtual QVariant value();

	virtual QWidget* widget();

private slots:
	void onValueUpdated(QString newInput=QString());
	void onValueFinished();
};

#endif // QTEXTFIELD_H
