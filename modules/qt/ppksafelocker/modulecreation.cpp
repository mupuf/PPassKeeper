#include "modulecreation.h"

ModuleCreation::ModuleCreation()
{
	setupUi();

	connect(passphraseEdit, SIGNAL(textChanged()), this, SLOT(onPassphraseUpdated()));
	connect(passphraseEdit2, SIGNAL(textChanged()), this, SLOT(onPassphraseUpdated()));
}

QString ModuleCreation::passphrase()
{
	return passphraseEdit->text();
}

void ModuleCreation::onPassphraseUpdated()
{
	//Enable/disable the OK button
	bool pwdMatch=passphraseEdit->text()==passphraseEdit2->text();
	buttonBox->buttons(QDialogButtonBox::Ok)->setEnabled(pwdMatch);
	
	//Check the current security
	//TODO
	
	//Update tips_lbl
	//TODO
}
