#include "qdirfield.h"
#include <QVariant>
#include <QFileDialog>

QDirField::QDirField(QWidget* parent, int maxLength) : QFileField(parent, maxLength, QString())
{

}

void QDirField::onPushButtonPressed()
{
	QString dir;
	QString res=QFileDialog::getExistingDirectory(widget(), QString::fromUtf8("Please select a directory ..."), dir);
	if(res!=QString())
		setValue(res);
}
