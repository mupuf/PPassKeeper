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

#include "infomodule.h"
#include "ui_infomodule.h"
#include <ppasskeeper.h>

InfoModule::InfoModule(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::InfoModule)
{
    m_ui->setupUi(this);
}

InfoModule::~InfoModule()
{
    delete m_ui;
}

void InfoModule::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
	   m_ui->retranslateUi(this);
	   break;
    default:
	   break;
    }
}

QString InfoModule::listFlags(unsigned int flags)
{
	QString list=QString::fromUtf8("none");

	if(flags|ppk_rf_silent)
		list+=QString::fromUtf8(", silent");

	return list;
}

void InfoModule::setModule(const char* m_id)
{
	//Module Name
	m_ui->moduleNameLabel->setText(QString::fromUtf8(m_id));

	//Security
	ppk_security_level slevel=ppk_module_security_level(m_id);
	QString txt_security=QString::fromUtf8("Security Level : ");
	if(slevel==ppk_sec_lowest)
		txt_security+=QString::fromUtf8("Not secured at all !");
	else if(slevel==ppk_sec_scrambled)
		txt_security+=QString::fromUtf8("Not secured");
	else if(slevel==ppk_sec_safe)
		txt_security+=QString::fromUtf8("Secured for most usage");
	else if(slevel==ppk_sec_perfect)
		txt_security+=QString::fromUtf8("Perfect, is it stored ?");
	else
		txt_security+=QString::fromUtf8("Unknown security level");
	m_ui->securityLabel->setText(txt_security);
	m_ui->securityProgress->setValue(slevel);

	//Limits
	QString supportWriting=(ppk_module_is_writable(m_id)==PPK_TRUE?tr("yes"):tr("no"));
	size_t sizeMaxString=ppk_module_max_data_size(m_id, ppk_string);
	size_t sizeMaxBlob=ppk_module_max_data_size(m_id, ppk_blob);
	m_ui->supportWritingLabel->setText(m_ui->supportWritingLabel->text().arg(supportWriting));
	m_ui->maxSizeStringLabel->setText(m_ui->maxSizeStringLabel->text().arg(QString::number(sizeMaxString)));
	m_ui->maxSizeBlobLabel->setText(m_ui->maxSizeBlobLabel->text().arg(QString::number(sizeMaxBlob)));

	//Supported flags
	unsigned int flags;

	ppk_module_read_flags(m_id, &flags);
	QString rflags=listFlags(flags);

	ppk_module_write_flags(m_id, &flags);
	QString wflags=listFlags(flags);

	ppk_module_listing_flags(m_id, &flags);
	QString lflags=listFlags(flags);

	m_ui->readFlagsLabel->setText(m_ui->readFlagsLabel->text().arg(rflags));
	m_ui->writeFlagsLabel->setText(m_ui->writeFlagsLabel->text().arg(rflags));
	m_ui->listFlagsLabel->setText(m_ui->listFlagsLabel->text().arg(rflags));
}
