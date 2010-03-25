#ifndef _MODULE_CREATION_H_
#define _MODULE_CREATION_H_

#include "ui_modulecreation.h"

class ModuleCreation : public QDialog, private Ui_createDB_dlg
{
Q_OBJECT

private:
	int secLevel(const QString& pwd, QString& tips);

public:
	ModuleCreation();
	
	QString passphrase();
	
private slots:
	void onPassphraseUpdated();
};

#endif