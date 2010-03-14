#ifndef QCOMBOFIELD_H
#define QCOMBOFIELD_H

#include "qabstractformfield.h"
#include <QComboBox>

class QComboField : public QAbstractFormField
{
	Q_OBJECT

protected:
	QComboBox* combo;

	void setList(const char** list);

public:
	QComboField(QWidget* parent=NULL, const char** list=NULL);

	virtual void setValue(QVariant value);

	virtual QVariant value();

	virtual QWidget* widget();

private slots:
	void onValueUpdated(QString newInput);
};

#endif // QTEXTFIELD_H
