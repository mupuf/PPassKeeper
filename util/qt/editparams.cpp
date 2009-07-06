#include "editparams.h"
#include "ui_editparams.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QMessageBox>

EditParams::EditParams(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::EditParams)
{
	m_ui->setupUi(this);

	//model.setupModelData("AskForPass_Qt");
	static ppk_proto_param tst_param;
	tst_param.expected_type=cvariant_string;
	tst_param.name="test_param";
	tst_param.help_text="Supposed to be the help string. Sorry ...";
	tst_param.default_value=cvariant_from_string("default_value for 'test_param'");
	//tst_param.group=&ppk_settings_basic;
	addParam(2, tst_param);

	static ppk_proto_param tst_param2;
	tst_param2.expected_type=cvariant_int;
	tst_param2.name="test_param2";
	tst_param2.help_text="Supposed to be the second help string. Sorry ...";
	tst_param2.default_value=cvariant_from_int(42);
	//tst_param.group=&ppk_settings_basic;
	addParam(3, tst_param2);

	connect(m_ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
}

EditParams::~EditParams()
{
    delete m_ui;
}

void EditParams::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
	   m_ui->retranslateUi(this);
	   break;
    default:
	   break;
    }
}

void EditParams::buttonBoxClicked(QAbstractButton* bbox)
{
	if(m_ui->buttonBox->standardButton(bbox)==QDialogButtonBox::RestoreDefaults)
	{
		QList<QObject*> klist=paramAssociated.keys();

		for(int i=0;i<klist.size();i++)
			resetParam(klist[i]);
	}
}

QString createNameString(ppk_proto_param pparam)
{
	QVariant default_value;

	switch(cvariant_get_type(pparam.default_value))
	{
		case cvariant_string:
			default_value=cvariant_get_string(pparam.default_value);
			break;

		case cvariant_int:
			default_value=cvariant_get_int(pparam.default_value);
			break;

		case cvariant_float:
			default_value=cvariant_get_float(pparam.default_value);
			break;

		default:
			return "Unknown";
	}

	return QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Helvetica'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">%1</span> (Default: %2)</p></body></html>").arg(pparam.name).arg(default_value.toString());
}

QWidget* EditParams::paramEditWidget(ppk_proto_param pparam)
{
	QVariant value=10.365;
	switch(cvariant_get_type(pparam.default_value))
	{
		case cvariant_none:
		{
			break;
		}

		case cvariant_string:
		{
			//default_value=cvariant_get_string(pparam.default_value);
			QLineEdit* lineEdit = new QLineEdit(value.toString(), m_ui->tab);
			return (QWidget*)lineEdit;
		}

		case cvariant_int:
		{
			QSpinBox* spinBox = new QSpinBox(m_ui->tab);
			spinBox->setValue(value.toInt());
			return (QWidget*)spinBox;
		}

		case cvariant_float:
		{
			QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(m_ui->tab);
			doubleSpinBox->setValue(value.toDouble());
			return (QWidget*)doubleSpinBox;
		}
	}

	return NULL;
}

void EditParams::addParam(int line, ppk_proto_param pparam)
{
	QWidget* widget=paramEditWidget(pparam);
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
	widget->setSizePolicy(sizePolicy);
	widget->setToolTip(pparam.help_text);
	paramAssociated[widget]=qMakePair(pparam, widget);
	m_ui->gridLayout->addWidget(widget, line, 1, 1, 1);

	QLabel* nameParamLbl = new QLabel(createNameString(pparam), m_ui->tab);
	nameParamLbl->setToolTip(pparam.help_text);
	paramAssociated[nameParamLbl]=qMakePair(pparam, widget);
	m_ui->gridLayout->addWidget(nameParamLbl, line, 0, 1, 1);

	QPushButton* helpParamBtn = new QPushButton(tr("?"), m_ui->tab);
	helpParamBtn->setObjectName(QString::fromUtf8("helpParamBtn"));
	QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(helpParamBtn->sizePolicy().hasHeightForWidth());
	helpParamBtn->setSizePolicy(sizePolicy1);
	helpParamBtn->setMaximumSize(QSize(40, 16777215));
	helpParamBtn->setToolTip(pparam.help_text);
	paramAssociated[helpParamBtn]=qMakePair(pparam, widget);
	m_ui->gridLayout->addWidget(helpParamBtn, line, 2, 1, 1);

	QDialogButtonBox* rstParamBtn = new QDialogButtonBox(m_ui->tab);
	rstParamBtn->setObjectName(QString::fromUtf8("rstParamBtn"));
	sizePolicy1.setHeightForWidth(rstParamBtn->sizePolicy().hasHeightForWidth());
	rstParamBtn->setSizePolicy(sizePolicy1);
	rstParamBtn->setStandardButtons(QDialogButtonBox::Reset);
	rstParamBtn->setToolTip(tr("Reset to the default value"));
	paramAssociated[rstParamBtn]=qMakePair(pparam, widget);
	m_ui->gridLayout->addWidget(rstParamBtn, line, 3, 1, 1);

	connect(helpParamBtn, SIGNAL(clicked()), this, SLOT(helpParamClicked()));
	connect(rstParamBtn, SIGNAL(clicked(QAbstractButton*)), this, SLOT(resetParamClicked()));
}

void EditParams::resetParamClicked()
{
	QObject* resetSender=this->sender();

	resetParam(resetSender);
}

void EditParams::helpParamClicked()
{
	QObject* resetSender=this->sender();
	ppk_proto_param senderProtoParam=paramAssociated[resetSender].first;
	QWidget* widgetParamValue=paramAssociated[resetSender].second;

	QMessageBox::information(this, tr("Help on the parameter '%1'").arg(senderProtoParam.name),
						tr("Help on the parameter '%1' : %2").arg(senderProtoParam.name).arg(senderProtoParam.help_text));
}

void EditParams::resetParam(QObject* resetSender)
{
	ppk_proto_param senderProtoParam=paramAssociated[resetSender].first;
	QWidget* widgetParamValue=paramAssociated[resetSender].second;

	switch(cvariant_get_type(senderProtoParam.default_value))
	{
		case cvariant_string:
		{
			QLineEdit* lineEdit = (QLineEdit*)widgetParamValue;
			lineEdit->setText(cvariant_get_string(senderProtoParam.default_value));
			break;
		}

		case cvariant_int:
		{
			QSpinBox* spinBox = (QSpinBox*)widgetParamValue;
			spinBox->setValue(cvariant_get_int(senderProtoParam.default_value));
			break;
		}

		case cvariant_float:
		{
			QDoubleSpinBox* doubleSpinBox = (QDoubleSpinBox*)widgetParamValue;
			doubleSpinBox->setValue(cvariant_get_int(senderProtoParam.default_value));
			break;
		}

		case cvariant_none:{}
	}
}
