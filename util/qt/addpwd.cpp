#include "addpwd.h"
#include "ui_addpwd.h"
#include <QMessageBox>
#include <string>

AddPWD::AddPWD(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AddPWD),
    cancel(false),
    success(false),
	module(NULL)
{
	m_ui->setupUi(this);

	connect(m_ui->entryTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(entryTypeChanged(int)));
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(onOK()));

	entryTypeChanged(0);
}

AddPWD::~AddPWD()
{
	delete m_ui;
}

void AddPWD::changeEvent(QEvent *e)
{
	switch (e->type()) {
	case QEvent::LanguageChange:
	   m_ui->retranslateUi(this);
	   break;
	default:
	   break;
	}
}

void AddPWD::hideAll()
{
	showApp(false);
	showNet(false);
	showItem(false);
}

void AddPWD::showApp(bool show)
{
	m_ui->userLabel->setVisible(show);
	m_ui->userEdit->setVisible(show);
	m_ui->appLabel->setVisible(show);
	m_ui->appEdit->setVisible(show);
}

void AddPWD::showNet(bool show)
{
	m_ui->hostLabel->setVisible(show);
	m_ui->hostEdit->setVisible(show);
	m_ui->loginLabel->setVisible(show);
	m_ui->loginEdit->setVisible(show);
	m_ui->portLabel->setVisible(show);
	m_ui->portSpin->setVisible(show);
	m_ui->protocolLabel->setVisible(show);
	m_ui->protocolEdit->setVisible(show);
}

void AddPWD::showItem(bool show)
{
	m_ui->itemLabel->setVisible(show);
	m_ui->itemEdit->setVisible(show);
}

void AddPWD::entryTypeChanged(int index)
{
	hideAll();

	if(index==0)
		showApp(true);
	else if(index==1)
		showNet(true);
	else if(index==2)
		showItem(true);

	//Reduce its vertical size to strict minimum
	this->adjustSize();
}

void AddPWD::setModule(ppk_module* module)
{
	this->module = module;
}

void AddPWD::setType(ppk_entry_type type)
{
	if(type==ppk_application)
		m_ui->entryTypeCombo->setCurrentIndex(0);
	else if(type==ppk_network)
		m_ui->entryTypeCombo->setCurrentIndex(1);
	else if(type==ppk_item)
		m_ui->entryTypeCombo->setCurrentIndex(2);
}

void AddPWD::onOK()
{
	QString default_string=tr("Replace me");

	QString error_fill_field_caption=tr("PPassKeeper : Error");
	QString error_fill_field_text=tr("Error : You must fill every field of the form !");

	ppk_entry* entry;
	QString key;

	QString app=m_ui->appEdit->text();
	QString user=m_ui->userEdit->text();
	QString login=m_ui->loginEdit->text();
	QString host=m_ui->hostEdit->text();
	QString protocol=m_ui->protocolEdit->text();
	int port=m_ui->portSpin->value();
	QString item=m_ui->itemEdit->text();

	int index=m_ui->entryTypeCombo->currentIndex();
	if (index==0)
	{
		key=user+QString::fromUtf8("@")+app;
		entry=ppk_application_entry_new(qPrintable(app), qPrintable(user));

		if(app==QString() || user==QString())
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}
	else if (index==1)
	{
		entry=ppk_network_entry_new(qPrintable(host), qPrintable(login), port, protocol.size()==0 ? NULL : qPrintable(protocol));
		key=login+QString::fromUtf8("@")+host+QString::fromUtf8(":")+QString::number(port);

		if(login==QString() || host==QString())
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}
	else if (index==2)
	{
		entry=ppk_item_entry_new(qPrintable(item));
		key=item;

		if(item==QString())
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}
	else
	{
		QMessageBox::critical(this, tr("PPassKeeper Error: Unknown password type"), tr("PPassKeeper Error: Cannot add the password because it is of un unknown type"));
		return;
	}

	ppk_data* data=ppk_string_data_new(qPrintable(default_string));
	ppk_error res = ppk_module_set_entry(module->id, entry, data, 0);
	if(res!=PPK_OK)
	{
		QString error=tr("An error occured while adding the entry '%1'\n\nError : %2").arg(key).arg(QString::fromUtf8(ppk_error_get_string(res)));
		QMessageBox::critical(this, tr("PPassKeeper : Error while adding ..."), error);
	}
	else
		success=true;

	ppk_data_free(data);
	ppk_entry_free(entry);
}

void AddPWD::onCancel()
{

}

bool AddPWD::succeeded()
{
	return success;
}

bool AddPWD::cancelled()
{
	return cancel;
}
