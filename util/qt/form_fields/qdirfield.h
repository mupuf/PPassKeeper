#ifndef QDIRFIELD_H
#define QDIRFIELD_H

#include "qfilefield.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QString>

class QDirField : public QFileField
{
	Q_OBJECT

public:
	QDirField(QWidget* parent=NULL, int maxLength=1000);

private slots:
	void onPushButtonPressed();
};

#endif // QFILEFIELD_H
