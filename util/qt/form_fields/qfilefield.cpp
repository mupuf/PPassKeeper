#include "qfilefield.h"
#include <QVariant>
#include <QFileDialog>

QFileField::QFileField(QWidget* parent, int maxLength, QString filter) : QTextField(parent, QRegExp(), maxLength)
{
	this->filter=filter;
	_layout=new QHBoxLayout();
	button=new QPushButton(parent);

	button->setText(QString::fromUtf8("..."));
	button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	button->setToolTip(QString::fromUtf8("Browse"));

	_layout->setSpacing(2);

	_layout->addWidget(QTextField::widget());
	_layout->addWidget(button);

	connect(button, SIGNAL(clicked()), this, SLOT(onPushButtonPressed()));
}

QLayout* QFileField::layout()
{
	return _layout;
}

void QFileField::onPushButtonPressed()
{
	QString dir;
	QString res=QFileDialog::getOpenFileName(widget(), QString::fromUtf8("Please select a file ..."), dir, filter);
	if(res!=QString())
		setValue(res);
}
