/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "addpwd.h"
#include "ui_addpwd.h"
#include <QMessageBox>
#include <string>

//private
void AddPWD::hideAllEntryTypes()
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

QString AddPWD::generateKey(const ppk_entry* entry)
{
	QString generatedKey;

	size_t size=ppk_key_length(entry);
	if(size>0)
	{
		char* buf=new char[size+1];

		ppk_boolean ret=ppk_get_key(entry, buf, size);
		if(ret==PPK_TRUE)
		{
			generatedKey=QString::fromUtf8(buf);
			delete[] buf;
			return generatedKey;
		}
		else
			delete[] buf;
	}

	return QString();
}


//protected
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


//public
AddPWD::AddPWD(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::AddPWD),
	cancel(false),
	success(false),
	module(NULL),
	_addedEntry()
{
	m_ui->setupUi(this);

	connect(m_ui->entryTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(entryTypeChanged(int)));

	//Check updates
	connect(m_ui->userEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConsistancy()));
	connect(m_ui->appEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConsistancy()));
	connect(m_ui->hostEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConsistancy()));
	connect(m_ui->loginEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConsistancy()));
	connect(m_ui->itemEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConsistancy()));
	connect(m_ui->protocolEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConsistancy()));

	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(onOK()));

	checkConsistancy();
}

AddPWD::~AddPWD()
{
	delete m_ui;
}

bool AddPWD::succeeded()
{
	return success;
}

bool AddPWD::cancelled()
{
	return cancel;
}

QString AddPWD::addedEntry()
{
	return _addedEntry;
}

void AddPWD::setModule(const char* module)
{
	this->module = module;
}

void AddPWD::setType(ppk_entry_type type)
{
	int index=0;

	if(type==ppk_application)
		index=0;
	else if(type==ppk_network)
		index=1;
	else if(type==ppk_item)
		index=2;

	m_ui->entryTypeCombo->setCurrentIndex(index);
	entryTypeChanged(index);
}

void AddPWD::entryTypeChanged(int index)
{
	hideAllEntryTypes();

	if(index==0)
		showApp(true);
	else if(index==1)
		showNet(true);
	else if(index==2)
		showItem(true);

	//Reduce its vertical size to strict minimum
	this->adjustSize();
}

#include <QPushButton>
void AddPWD::checkConsistancy()
{
	bool consistent=false;

	//get the current type
	int index=m_ui->entryTypeCombo->currentIndex();
	if(index==0)
	{
		//Application
		QString user=m_ui->userEdit->text();
		QString app=m_ui->appEdit->text();

		consistent = (user!=QString() && app!=QString());
	}
	else if(index==1)
	{
		//Network
		QString login=m_ui->loginEdit->text();
		QString host=m_ui->hostEdit->text();
		QString protocol=m_ui->protocolEdit->text();

		consistent = (login!=QString() && host!=QString() && protocol!=QString());
	}
	else if(index==2)
	{
		//Item
		QString item=m_ui->itemEdit->text();

		consistent = item!=QString();
	}

	//Enable/Disable the OK button
	m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(consistent);
}

void AddPWD::onOK()
{
	cancel=false;

	QString default_string=tr("Replace me");

	QString error_fill_field_caption=tr("PPassKeeper : Error");
	QString error_fill_field_text=tr("Error : You must fill every field of the form !");

	ppk_entry* entry;

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
		entry=ppk_application_entry_new(qPrintable(user), qPrintable(app));

		if(app==QString() || user==QString())
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}
	else if (index==1)
	{
		entry=ppk_network_entry_new(protocol.size()==0 ? NULL : qPrintable(protocol), qPrintable(login), qPrintable(host), port);

		if(login==QString() || host==QString())
		{
			QMessageBox::critical(this, error_fill_field_caption, error_fill_field_text);
			return;
		}
	}
	else if (index==2)
	{
		entry=ppk_item_entry_new(qPrintable(item));

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
	
	char ret[101];
	ppk_get_key(entry, ret, 100);

	ppk_data* data=ppk_string_data_new(qPrintable(default_string));
	ppk_error res = ppk_module_set_entry(module, entry, data, 0);
	if(res!=PPK_OK)
	{
		QString error=tr("An error occured while adding the entry '%1'\n\nError : %2").arg(generateKey(entry)).arg(QString::fromUtf8(ppk_error_get_string(res)));
		QMessageBox::critical(this, tr("PPassKeeper : Error while adding ..."), error);
	}
	else
	{
		success=true;
		_addedEntry=generateKey(entry);
	}

	ppk_data_free(data);
	ppk_entry_free(entry);
}

void AddPWD::onCancel()
{
	cancel=true;
}
