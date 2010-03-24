#include "modulecreation.h"

#include <QDialogButtonBox>
#include <QPushButton>

ModuleCreation::ModuleCreation()
{
	setupUi(this);
	
	onPassphraseUpdated();

	connect(passphraseEdit, SIGNAL(textChanged(QString)), this, SLOT(onPassphraseUpdated()));
	connect(passphraseEdit2, SIGNAL(textChanged(QString)), this, SLOT(onPassphraseUpdated()));
}

QString ModuleCreation::passphrase()
{
	return passphraseEdit->text();
}

void ModuleCreation::onPassphraseUpdated()
{
	//Enable/disable the OK button
	bool pwdMatch=passphraseEdit->text().size()>0 && passphraseEdit->text()==passphraseEdit2->text();
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(pwdMatch);
	
	//Check the current security
	//TODO
	
	//Update tips_lbl
	//TODO
}
