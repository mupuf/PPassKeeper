#ifndef QFILEFIELD_H
#define QFILEFIELD_H

#include "qtextfield.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QString>

class QFileField : public QTextField
{
	Q_OBJECT
private:
	QPushButton* button;
	QHBoxLayout* _layout;
	QString filter;

public:
	QFileField(QWidget* parent=NULL, int maxLength=1000, QString filter=QString());

	virtual QLayout* layout();

private slots:
	void onPushButtonPressed();
};

#endif // QFILEFIELD_H
