#include "addpwd.h"
#include "ui_addpwd.h"
#include <QMessageBox>
#include <string>

AddPWD::AddPWD(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AddPWD),
    cancel(false),
    success(false)
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

void AddPWD::setModule(QString module)
{
	module_id=module;
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

	ppk_entry entry;
	std::string key;

	std::string app=m_ui->appEdit->text().toStdString();
	std::string user=m_ui->userEdit->text().toStdString();
	std::string login=m_ui->loginEdit->text().toStdString();
	std::string host=m_ui->hostEdit->text().toStdString();
	std::string protocol=m_ui->protocolEdit->text().toStdString();
	int port=m_ui->portSpin->value();
	std::string item=m_ui->itemEdit->text().toStdString();

	int index=m_ui->entryTypeCombo->currentIndex();
	if (index==0)
	{
		key=user+"@"+app;
		entry=ppk_createAppEntry(app.c_str(), user.c_str());

		if(app=="" || user=="")
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}
	else if (index==1)
	{
		entry=ppk_createNetworkEntry(host.c_str(), login.c_str(), port, protocol.empty() ? NULL : protocol.c_str());
		key=login+"@"+host+":"+QString::number(port).toStdString();

		if(login=="" || host=="")
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}
	else if (index==2)
	{
		entry=ppk_createItemEntry(item.c_str());
		key=item;

		if(item=="")
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}

	bool res = ppk_setEntry(module_id.toUtf8().constData(), entry, ppk_createStringData(default_string.toAscii().data()), 0)==PPK_TRUE;
	if(!res)
	{
		QString error=QString("An error occured while adding the entry '%1'\n\nError : %2").arg(key.c_str()).arg(ppk_getLastError(module_id.toUtf8().constData()));
		QMessageBox::critical(this, tr("PPassKeeper : Error while adding ..."), error);
	}
	else
		success=true;
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
