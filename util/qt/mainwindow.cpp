#include "mainwindow.h"
#include <QMessageBox>
#include <iostream>
#include <QInputDialog>
#include <sstream>
#include <ppasskeeper.h>
#include <stdio.h>
#include <QFileDialog>

#include "addpwd.h"
#include "infomodule.h"

MainWindow::MainWindow()
	: QMainWindow(),
	cur_availability(false)
{
	setupUi(this);

	pwdlistModel = new PasswordListModel(this);
	pwdlistView->setModel(pwdlistModel);

	setupActions();

	setWindowTitle(qApp->applicationName());

	unlockPPK(true);

	fillModulesBox();
}

void MainWindow::fillModulesBox()
{
	modulesBox->clear();

	unsigned int n = ppk_getAvailableModulesCount();
	ppk_module modules[n];									//HACK !
	ppk_getAvailableModules(modules, n);

	modulesBox->addItem("Select one:");
	modulesBox->insertSeparator(1);

	for (unsigned int i = 0; i < n; ++i)
	{
		/* we are only interested in modules that can be listed and that can actually store data */
		if (ppk_isWritable(modules[i].id)==PPK_TRUE)
		{
			modulesBox->addItem(modules[i].display_name, QVariant(modules[i].id));
		}
	}
}

void MainWindow::showInfoMessageUnderDevelopment()
{
	QMessageBox::information(this, "PPassKeeper : This is still a development version", "This function has not been implemented yet. \n\nPPassKeeper is still under heavy development, so, we do apologize.");
}

void MainWindow::setupActions()
{
	connect(action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actionLock_ppasskeeper, SIGNAL(triggered()), this, SLOT(setMasterPwd()));
	connect(actionSetModuleDefault, SIGNAL(triggered()), this, SLOT(onSetDefaultModule()));
	connect(actionAbout_Qt, SIGNAL(triggered()), QApplication::instance(), SLOT(aboutQt()));

	connect(modulesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(moduleChanged(int)));
	connect(modulesBox, SIGNAL(currentIndexChanged(int)), pwdlistView, SLOT(expandAll()));

	connect(action_Add, SIGNAL(triggered()), this, SLOT(onAddButtonClicked()));
	connect(action_Del, SIGNAL(triggered()), this, SLOT(onDelButtonClicked()));
	connect(actionInfos, SIGNAL(triggered()), this, SLOT(onInfoModuleButtonClicked()));
	connect(action_Import, SIGNAL(triggered()), this, SLOT(onImportButtonClicked()));
	connect(action_Export, SIGNAL(triggered()), this, SLOT(onExportButtonClicked()));
	connect(pwdlistView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
			pwdlistModel, SLOT(rowSelected(const QModelIndex &, const QModelIndex &)));
	connect(pwdlistModel,
			SIGNAL(appPasswordSelected(const char *, const char *)),
			this,
			SLOT(onAppPasswordSelected(const char *, const char *)));
	connect(pwdlistModel,
			SIGNAL(netPasswordSelected(const char *, const char *, unsigned short int)),
			this,
			SLOT(onNetPasswordSelected(const char *, const char *, unsigned short int)));
	connect(pwdlistModel,
			SIGNAL(itemPasswordSelected(const char *)),
			this,
			SLOT(onItemPasswordSelected(const char *)));
	connect(pwdlistModel,
			SIGNAL(noItemSelected()),
			this,
			SLOT(onNoItemSelected()));

	connect(showButton, SIGNAL(toggled(bool)), this, SLOT(onShowButtonToggled(bool)));
	connect(showButton, SIGNAL(clicked(bool)), this, SLOT(setPasswordVisible(bool)));

	connect(saveValueButton, SIGNAL(clicked()), this, SLOT(saveValueToFile()));
	connect(setBlobButton, SIGNAL(clicked()), this, SLOT(setBlobFromFile()));

	connect(filterNameCheckbox, SIGNAL(toggled(bool)), pwdlistModel, SLOT(useFilter(bool)));
	connect(filterNameCheckbox, SIGNAL(toggled(bool)), pwdlistView, SLOT(expandAll()));
	connect(filterNameText, SIGNAL(textEdited(QString)), pwdlistModel, SLOT(setFilter(QString)));
	connect(filterNameText, SIGNAL(textEdited(QString)), pwdlistView, SLOT(expandAll()));

	connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));

	action_Del->setEnabled(false);

	/*if(ppk_removeParam("KWallet4", "osef")==PPK_TRUE)
		printf("OK, il a bien été viré !\n");
	else
		printf("NOK, il n'a pas bien été viré !\n");

	char** list=new char*[100];
	unsigned int size=ppk_listParam("KWallet4", (const char***)&list, 100);

	std::cout << "Il y a " << size << " params !" << std::endl;
	for(unsigned int i=0;i<size;i++)
	{
		std::cout << "list[" << i << "] = " << list[i] << std::endl;
	}
	delete[] list;*/
}

void MainWindow::onShowButtonToggled(bool b)
{
	if (b) showButton->setText(tr("Hide value"));
	else showButton->setText(tr("Show value"));
}

bool MainWindow::updateSelectedPassword(ppk_data data)
{
	bool res=false;
	ppk_entry_type cur_type=pwdlistModel->currentSelectedType();
	if (cur_type == ppk_application)
	{
		res = ppk_setEntry(m_moduleId.toUtf8().constData(),
				ppk_createAppEntry(cur_app.app_name.toUtf8().constData(), cur_app.username.toUtf8().constData()),
				data, 0)==PPK_TRUE;
	}
	else if (cur_type == ppk_network)
	{
		res = ppk_setEntry(m_moduleId.toUtf8().constData(),
				ppk_createNetworkEntry(cur_net.host.toUtf8().constData(), cur_net.login.toUtf8().constData(), cur_net.port, cur_net.protocol.toUtf8().constData()),
				data, 0)==PPK_TRUE;
	}
	else if (cur_type == ppk_item)
	{
		res = ppk_setEntry(m_moduleId.toUtf8().constData(),
				ppk_createItemEntry(cur_item.key.toUtf8().constData()),
				data, 0)==PPK_TRUE;
	}

	return res;
}

bool MainWindow::unlockPPK(bool force)
{
	if(ppk_isLocked()==PPK_TRUE)
	{
		bool ok;
		std::string pwd=QInputDialog::getText(NULL,"Unlock PPassKeeper","Please key in the password to unlock PPassKeeper :", QLineEdit::Password,"", &ok).toStdString();

		if(ok)
		{
			if(ppk_unlock(pwd.c_str())==PPK_TRUE)
				return true;
			else
			{
				QMessageBox::critical(this, "Error : Incorrect password", "The password you entered is wrong.\nTry again ...");

				if(force)
					return unlockPPK(force);
				else
					return false;
			}
		}
		else
			exit(1);
	}
	else
		return true;
}

void MainWindow::setMasterPwd()
{
	bool ok;
	std::string pwd=QInputDialog::getText(NULL,"Please set up the new master password","Please key in the new master password :", QLineEdit::Password,"", &ok).toStdString();
	if(ok)
	{
		std::string pwd2=QInputDialog::getText(NULL,"Unlock PPassKeeper","Please key in the password to unlock PPassKeeper a second time :", QLineEdit::Password,"", &ok).toStdString();
		if(ok)
		{
			if(pwd==pwd2)
			{
				if(ppk_setPassword(pwd.c_str())==PPK_TRUE)
					QMessageBox::information(this, "The password has been set", "The password you entered has been set as the new master password.");
				else
					QMessageBox::critical(this, "Error : An error occured", QString("PPassKeeper was unable to set this password.\nError : ")+ppk_getLastError(NULL));
			}
			else
				QMessageBox::critical(this, "Error : The password are not the same", "The two passwords you entered are not matching.\nTry again ...");
		}
	}
}

void MainWindow::onSetDefaultModule()
{
	ppk_setDefaultModule(m_moduleId.toAscii().data());
}

void MainWindow::onAddButtonClicked()
{
	AddPWD addpwd;

	addpwd.setType(pwdlistModel->currentSelectedType());
	addpwd.setModule(m_moduleId);
	addpwd.setModal(true);
	addpwd.show();
	addpwd.exec();

	if(addpwd.succeeded())
		listCurrentModule();
}

ppk_data MainWindow::getSelectedEntryData(bool& ok)
{
	ppk_data data;
	ppk_boolean res=PPK_FALSE;

	ppk_entry_type cur_type=pwdlistModel->currentSelectedType();

	if (cur_type == ppk_application)
	{
		res = ppk_getEntry(m_moduleId.toUtf8().constData(),
				ppk_createAppEntry(cur_app.app_name.toUtf8().constData(), cur_app.username.toUtf8().constData()),
				&data, 0);
	}
	else if (cur_type == ppk_network)
	{
		res = ppk_getEntry(m_moduleId.toUtf8().constData(),
				ppk_createNetworkEntry(cur_net.host.toUtf8().constData(), cur_net.login.toUtf8().constData(), cur_net.port, cur_net.protocol.toUtf8().constData()),
				&data, 0);
	}
	else if (cur_type == ppk_item)
	{
		res = ppk_getEntry(m_moduleId.toUtf8().constData(),
				ppk_createItemEntry(cur_item.key.toUtf8().constData()),
				&data, 0);
	}

	ok=(res==PPK_TRUE);
	return data;
}

void MainWindow::onDelButtonClicked()
{
	QString error="", entry_name;

	ppk_entry_type cur_type=pwdlistModel->currentSelectedType();

	//Get the entry name
	if (cur_type == ppk_application)
		entry_name=cur_app.username+"@"+cur_app.app_name;
	else if (cur_type == ppk_network)
		entry_name=cur_net.login+"@"+cur_net.host+":"+QString::number(cur_net.port);
	else if (cur_type == ppk_item)
		entry_name=cur_item.key;


	QString title=tr("Are your sure ?");
	QString text= tr("Are you sure you want to delete the entry '%1' from the module '%2' ?").arg(entry_name).arg(m_moduleId);
	if(QMessageBox::question(this, title, text, QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes)
	{
		ppk_boolean res=PPK_FALSE;
		if (cur_type == ppk_application)
		{
			res = ppk_removeEntry(m_moduleId.toUtf8().constData(),
					ppk_createAppEntry(cur_app.app_name.toUtf8().constData(), cur_app.username.toUtf8().constData()), 0);
		}
		else if (cur_type == ppk_network)
		{
			res = ppk_removeEntry(m_moduleId.toUtf8().constData(),
					ppk_createNetworkEntry(cur_net.host.toUtf8().constData(), cur_net.login.toUtf8().constData(), cur_net.port, cur_net.protocol.toUtf8().constData()), 0);
		}
		else if (cur_type == ppk_item)
		{
			res = ppk_removeEntry(m_moduleId.toUtf8().constData(),
					ppk_createItemEntry(cur_item.key.toUtf8().constData()), 0);
		}

		if(!res)
		{
			error=ppk_getLastError(m_moduleId.toUtf8().constData());
			QMessageBox::critical(this, "PPassKeeper : Error while adding ...", error);
		}
		else
			listCurrentModule();
	}
}

void MainWindow::onImportButtonClicked()
{
	showInfoMessageUnderDevelopment();
}


void MainWindow::onExportButtonClicked()
{
	showInfoMessageUnderDevelopment();
}

void MainWindow::onInfoModuleButtonClicked()
{
	InfoModule infomod(this);
	infomod.setModule(m_moduleId.toAscii().data());
	infomod.setModal(true);
	infomod.show();
	infomod.exec();
}

void MainWindow::setPasswordVisible(bool b)
{
	if (b)
	{
		if (! cur_availability)
		{
			showButton->setChecked(false);
			return;
		}

		bool ok=false;
		ppk_data data=getSelectedEntryData(ok);
		if (ok)
		{
			if(data.type==ppk_string)
			{
				passwordEdit->setPlainText(QString::fromUtf8(data.string));
				passwordEdit->setReadOnly(false);
				tmp_sensitive_data=QString::fromUtf8(data.string);

				timerValue = 0;
				passwordTimer.start(1000, this);
			}
			else
			{
				QString blob_str=tr("<blob> : Binary Value.\nNot editable.");
				passwordEdit->setPlainText(blob_str);
				passwordEdit->setReadOnly(true);
				tmp_sensitive_data=blob_str;
			}
		}
		else
		{
			QMessageBox::critical(this, tr("An error occured while reading the password"), tr("An error occured while reading the password !\n\nThe error is : \n")+ppk_getLastError(m_moduleId.toUtf8().constData()));
			setPasswordVisible(false);
		}
	} else {
		passwordTimer.stop();
		progressBar->setValue(0);
		passwordEdit->setPlainText(tr("(hidden)"));
		passwordEdit->setReadOnly(true);
		tmp_sensitive_data=QString();
		showButton->setChecked(false);
	}
}

void MainWindow::timerEvent(QTimerEvent* /*event*/)
{
	if (timerValue == 30)
	{
		passwordTimer.stop();
		timerValue = 0;
		setPasswordVisible(false);
		return;
	}

	progressBar->setValue(timerValue * 100 / 30);
	++timerValue;
}

void MainWindow::onPasswordSelected()
{
	cur_availability = true;
	showButton->setEnabled(true);
	setBlobButton->setEnabled(true);
	saveValueButton->setEnabled(true);
	action_Del->setEnabled(true);
	updateInfoLabel();
}

void MainWindow::onAppPasswordSelected(const char *app_name, const char *username)
{
	cur_app.app_name = app_name;
	cur_app.username = username;

	onPasswordSelected();
}

void MainWindow::onNetPasswordSelected(const char *host, const char *login, unsigned short int port)
{
	cur_net.host = host;
	cur_net.login = login;
	cur_net.port = port;

	onPasswordSelected();
}

void MainWindow::onItemPasswordSelected(const char *key)
{
	cur_item.key = key;

	onPasswordSelected();
}

void MainWindow::onNoItemSelected()
{
	showButton->setEnabled(false);
	action_Del->setEnabled(false);
	setBlobButton->setEnabled(false);
	saveValueButton->setEnabled(false);

	cur_availability = false;
	updateInfoLabel();
}

void MainWindow::saveValueToFile()
{
	bool ok=false;
	ppk_data data=getSelectedEntryData(ok);
	if (ok)
	{
		QString filepath=QFileDialog::getSaveFileName(this, tr("Save Value"), "", tr("All (*.*)"));
		if (filepath.isEmpty())
			return;

		QFile file(filepath);
		file.open(QIODevice::WriteOnly);
		if(file.isWritable())
		{
			if(data.type==ppk_string)
				file.write(data.string);
			else if(data.type==ppk_blob)
				file.write((const char*)data.blob.data, data.blob.size);
			else
				QMessageBox::critical(this, tr("An error occured while reading the password"), tr("Reading error : Unknown data type."));
		}
		else
			QMessageBox::critical(this, tr("Writing Error : The file is not writable"), tr("Writing error : The file is not writable.\n\nCheck your file permission or try another one."));
	}
	else
		QMessageBox::critical(this, tr("An error occured while reading the password"), tr("An error occured while reading the password !\n\nThe error is : \n")+ppk_getLastError(m_moduleId.toUtf8().constData()));
}

void MainWindow::setBlobFromFile()
{
	QString filepath=QFileDialog::getOpenFileName(this, tr("Open a file to save"), "", tr("All (*.*)"));
	if (filepath.isEmpty())
		return;

	QFile file(filepath);
	if(file.size()<=ppk_maxDataSize(this->m_moduleId.toAscii().data(), ppk_blob))
	{
		file.open(QIODevice::ReadOnly);
		if(file.isReadable())
		{
			QByteArray fdata=file.readAll();
			if(updateSelectedPassword(ppk_createBlobData(fdata.data(), fdata.size())))
				QMessageBox::information(this, tr("Success : The blob has been set correctly"), tr("The blob has been set correctly."));
			else
				QMessageBox::critical(this, tr("PPassKeeper Error : The entry cannot be set"), tr("PPassKeeper Error : The entry cannot be set.\n\nThe error is : \n")+ppk_getLastError(m_moduleId.toUtf8().constData()));
		}
		else
			QMessageBox::critical(this, tr("Error : The file is not readable"), tr("Error : The file is not readable.\n\nCheck your file permission."));
	}
	else
		QMessageBox::critical(this, tr("Error : The file is too big"), tr("Error : The file is bigger than the maximum size allowed by the module.\n\nTry to use another module."));
}

void MainWindow::updateInfoLabel()
{
	QString str;
	ppk_entry_type cur_type=pwdlistModel->currentSelectedType();

	if (! cur_availability)
	{
		str = tr("(none selected)");
	}
	else if (cur_type == ppk_application)
	{
		str = tr("Application name: %1\n"
				"User name: %2").arg(cur_app.app_name).arg(cur_app.username);
	}
	else if (cur_type == ppk_network)
	{
		str = tr("Host name: %1\n"
				"Login: %2\n"
				"Port: %3").arg(cur_net.host).arg(cur_net.login).arg(cur_net.port);
	}
	else if (cur_type == ppk_item)
	{
		str = tr("Key: %1").arg(cur_item.key);
	}
	infoLabel->setText(str);
}

void MainWindow::moduleChanged(int index)
{
	QString id = modulesBox->itemData(index).toString();
	if (! id.isNull() && ! id.isEmpty())
	{
		m_moduleId = id;
		listCurrentModule();

		/* the displayed password becomes unavailable
		 * after a module change */
		cur_availability = false;

		bool is_default=(m_moduleId==ppk_getDefaultModule());

		toolBar->setEnabled(true);
		actionSetModuleDefault->setEnabled(true);
		actionSetModuleDefault->setChecked(is_default);
	}
}

void MainWindow::listCurrentModule()
{
	pwdlistModel->setupModelData(m_moduleId.toUtf8().constData());
}

void MainWindow::focusChanged(QWidget* q_old, QWidget* /*q_new*/)
{
	//If focus was on passwordEdit and passwordEdit's content has changed and the password was currently edited, save it
	if (passwordTimer.isActive() && q_old==passwordEdit && passwordEdit->toPlainText()!=tmp_sensitive_data)
	{
		if(QMessageBox::question(this, tr("Would you like to save ..."), tr("You have modified the data stored.\n\nWould you like to save your modifications ?"), QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes)
			updateSelectedPassword(ppk_createStringData(passwordEdit->toPlainText().toAscii().data()));
		setPasswordVisible(false);
	}
}

bool MainWindow::parseAndGetNetworkEntry(std::string str, ppk_entry& entry)
{
	//Parse the file's name
	unsigned int pos_at=str.find_first_of("@");
	unsigned int pos_sc=str.find(":",pos_at+1);

	//if it has found the separators
	if(pos_at!=std::string::npos && pos_sc!=std::string::npos)
	{
		static std::string user;
		static std::string host;
		user=str.substr(0,pos_at);
		host=str.substr(pos_at+1,pos_sc-(pos_at+1));
		std::string s_port=str.substr(pos_sc+1);

		//Get the port into a number
		unsigned int port;
		std::istringstream i(s_port);
		if (!(i >> port))
			return false;

		entry.type=ppk_network;
		entry.net.host=host.c_str();
		entry.net.port=port;
		entry.net.login=user.c_str();

		return true;
	}
	else
		return false;
}
bool MainWindow::parseAndGetAppEntry(std::string str, ppk_entry& entry)
{
	//Parse the file's name
	unsigned int pos_at=str.find_first_of("@");

	//if it has found the separators
	if(pos_at!=std::string::npos)
	{
		static std::string user;
		static std::string app;
		user=str.substr(0,pos_at);
		app=str.substr(pos_at+1);

		//Add the content to pwdList
		entry.type=ppk_application;
		entry.app.app_name=app.c_str();
		entry.app.username=user.c_str();

		return true;
	}
	else
		return false;
}

bool MainWindow::parseAndGetItemEntry(std::string str, ppk_entry& entry)
{
	if(str.size()>0)
	{
		static std::string item;
		item=str;

		entry.type=ppk_item;
		entry.item=str.c_str();

		return true;
	}
	else
		return false;
}

