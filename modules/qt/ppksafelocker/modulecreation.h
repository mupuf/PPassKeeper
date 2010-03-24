#ifdef _MODULE_CREATION_H_
#define _MODULE_CREATION_H_

#include "ui_modulecreation"

class ModuleCreation : public QDialog, private Ui_Dialog
{
Q_OBJECT

	ModuleCreation();
	
	QString passphrase();
	
private slots:
	void onPassphraseUpdated();
}

#endif