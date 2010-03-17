#include "qtextfield.h"
#include <QVariant>
#include <QStyle>

bool QTextField::validate(QString val)
{
	return rExpValidation==QRegExp() || rExpValidation.exactMatch(val);
}

bool QTextField::validate()
{
	return validate(value().toString());
}

QTextField::QTextField(QWidget* parent, QRegExp regExp, int maxLength) : QAbstractFormField()
{
	rExpValidation=regExp;

	edit=new QLineEdit(parent);
	edit->setMaxLength(maxLength);

	onValueUpdated();

	connect(edit, SIGNAL(textEdited(QString)), this, SLOT(onValueUpdated(QString)));
	connect(edit, SIGNAL(editingFinished()), this, SLOT(onValueFinished()));
}

void QTextField::onValueUpdated(QString /*newInput*/)
{
	QString styleSheet;

	if(!validate())
		styleSheet=QString::fromUtf8("background-color: red;");
	else
		styleSheet=QString();

	widget()->setStyleSheet(styleSheet);
}

void QTextField::onValueFinished()
{
	if(!validate())
		setValue(lastGoodValue);
	else
		setValue(edit->text());
}


void QTextField::setValue(QVariant value)
{
	if(validate(value.toString()))
	{
		edit->setText(value.toString());
		lastGoodValue=value.toString();
		onValueUpdated();
	}
}

QVariant QTextField::value()
{
	return QVariant(edit->text());
}

QWidget* QTextField::widget()
{
	return edit;
}
