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
#include "editparams.h"

#include <QMetaType>

Q_DECLARE_METATYPE(ppk_module*); // to make it storable in QVariant

MainWindow::MainWindow()
	: QMainWindow(),
	  m_moduleList(NULL),
	  m_module(NULL),
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

MainWindow::~MainWindow()
{
	delete[] m_moduleList;
}

void MainWindow::fillModulesBox()
{
	modulesBox->clear();

	unsigned int n = ppk_module_count();
	m_moduleList = new ppk_module[n];
	ppk_module_list(m_moduleList, n);

	modulesBox->addItem(tr("Select one:"), qVariantFromValue((ppk_module*) NULL));
	modulesBox->insertSeparator(1);

	for (unsigned int i = 0; i < n; ++i)
		modulesBox->addItem(QString::fromUtf8(m_moduleList[i].display_name), qVariantFromValue(&(m_moduleList[i])));
}

void MainWindow::showInfoMessageUnderDevelopment()
{
	QMessageBox::information(this, tr("PPassKeeper: This is still a development version"), tr("This function has not been implemented yet. \n\nPPassKeeper is still under heavy development, so, we do apologize."));
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
	connect(actionParams, SIGNAL(triggered()), this, SLOT(onParamsTriggered()));
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
}

void MainWindow::onShowButtonToggled(bool b)
{
	if (b) showButton->setText(tr("Hide value"));
	else showButton->setText(tr("Show value"));
}

bool MainWindow::updateSelectedPassword(ppk_data* data)
{
	ppk_error res;
	ppk_entry* entry;

	ppk_entry_type cur_type=pwdlistModel->currentSelectedType();
	if (cur_type == ppk_application)
	{
		entry=ppk_application_entry_new(qPrintable(cur_app.username), qPrintable(cur_app.app_name));
		res = ppk_module_set_entry(m_module->id, entry, data, 0);
	}
	else if (cur_type == ppk_network)
	{
		entry=ppk_network_entry_new(qPrintable(cur_net.protocol), qPrintable(cur_net.login), qPrintable(cur_net.host), cur_net.port);
		res = ppk_module_set_entry(m_module->id, entry, data, 0);
	}
	else if (cur_type == ppk_item)
	{
		entry=ppk_item_entry_new(qPrintable(cur_item.key));
		res = ppk_module_set_entry(m_module->id, entry, data, 0);
	}
	else
		return false;

	//Show errors
	if(res!=PPK_OK)
	{
		char key[101];
		ppk_get_key(entry, key, sizeof(key)-1);
		QString error=tr("An error occured while updating the entry '%1'\n\nError: %2").arg(QString::fromUtf8(key)).arg(QString::fromUtf8(ppk_error_get_string(res)));
		QMessageBox::critical(this, tr("PPassKeeper: Error while updating the password"), error);
	}

	//Free the entry
	ppk_entry_free(entry);

	return res==PPK_OK;
}

bool MainWindow::unlockPPK(bool force)
{
	if(ppk_is_locked()==PPK_TRUE)
	{
		bool ok;
		std::string pwd=QInputDialog::getText(NULL, tr("Unlock PPassKeeper"), tr("Please key in the password to unlock PPassKeeper:"), QLineEdit::Password,QString(), &ok).toStdString();

		if(ok)
		{
			if(ppk_unlock(pwd.c_str())==PPK_TRUE)
				return true;
			else
			{
				QMessageBox::critical(this, tr("Error: Incorrect password"), tr("The password you entered is wrong.\nTry again ..."));

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
	std::string pwd=QInputDialog::getText(NULL, tr("Please set up the new master password"), tr("Please key in the new master password :"), QLineEdit::Password, QString(), &ok).toStdString();
	if(ok)
	{
		std::string pwd2=QInputDialog::getText(NULL,tr("Unlock PPassKeeper"), tr("Please key in the password to unlock PPassKeeper a second time :"), QLineEdit::Password, QString(), &ok).toStdString();
		if(ok)
		{
			if(pwd==pwd2)
			{
				if(ppk_set_password(pwd.c_str())==PPK_TRUE)
					QMessageBox::information(this, tr("The password has been set"), tr("The password you entered has been set as the new master password."));
				else
					QMessageBox::critical(this, tr("Error: An error occured"), tr("PPassKeeper was unable to set this password.\nError : TODO")/*ppk_getLastError(NULL)*/);
			}
			else
				QMessageBox::critical(this, tr("Error: The password are not the same"), tr("The two passwords you entered are not matching.\nTry again ..."));
		}
	}
}

void MainWindow::onSetDefaultModule()
{
	ppk_module_set_default(m_module->id);
}

void MainWindow::onAddButtonClicked()
{
	AddPWD addpwd;

	addpwd.setType(pwdlistModel->currentSelectedType());
	addpwd.setModule(m_module);
	addpwd.setModal(true);
	addpwd.show();
	addpwd.exec();

	if(addpwd.succeeded())
		listCurrentModule();
}

ppk_data* MainWindow::getSelectedEntryData(bool& ok)
{
	ppk_data* data;
	ppk_error res;;
	ppk_entry* entry;

	ok=false;

	ppk_entry_type cur_type=pwdlistModel->currentSelectedType();
	if (cur_type == ppk_application)
	{
		entry=ppk_application_entry_new(qPrintable(cur_app.username), qPrintable(cur_app.app_name));
		res = ppk_module_get_entry(m_module->id, entry, &data, 0);
	}
	else if (cur_type == ppk_network)
	{
		entry=ppk_network_entry_new(qPrintable(cur_net.protocol), qPrintable(cur_net.login), qPrintable(cur_net.host), cur_net.port);
		res = ppk_module_get_entry(m_module->id, entry, &data, 0);
	}
	else if (cur_type == ppk_item)
	{
		entry=ppk_item_entry_new(qPrintable(cur_item.key));
		res = ppk_module_get_entry(m_module->id, entry, &data, 0);
	}
	else
		return data;

	//Show errors
	if(res!=PPK_OK)
	{
		char key[101];
		if(ppk_get_key(entry, key, sizeof(key)-1)==PPK_FALSE)
			strncpy(key, "<invalid_entry>", sizeof(key)-1);

		QString error=tr("An error occured while accessing the entry '%1'\n\nError : %2").arg(QString::fromUtf8(key)).arg(QString::fromUtf8(ppk_error_get_string(res)));
		QMessageBox::critical(this, tr("PPassKeeper : Error while accessing the entry"), error);
	}

	//Free the entry
	ppk_entry_free(entry);

	ok=(res==PPK_OK);

	return data;
}

void MainWindow::onDelButtonClicked()
{
	QString error, entry_name;

	ppk_entry_type cur_type=pwdlistModel->currentSelectedType();

	//Get the entry name
	if (cur_type == ppk_application)
		entry_name=cur_app.username+QString::fromUtf8("@")+cur_app.app_name;
	else if (cur_type == ppk_network)
		entry_name=cur_net.login+QString::fromUtf8("@")+cur_net.host+QString::fromUtf8(":")+QString::number(cur_net.port);
	else if (cur_type == ppk_item)
		entry_name=cur_item.key;


	QString title=tr("Are your sure ?");
	QString text= tr("Are you sure you want to delete the entry '%1' from the module '%2' ?").arg(entry_name).arg(QString::fromUtf8(m_module->id));
	if(QMessageBox::question(this, title, text, QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes)
	{
		ppk_error res;
		ppk_entry* entry;

		if (cur_type == ppk_application)
		{
			entry=ppk_application_entry_new(qPrintable(cur_app.username), qPrintable(cur_app.app_name));
			res = ppk_module_remove_entry(m_module->id, entry, 0);
		}
		else if (cur_type == ppk_network)
		{
			entry=ppk_network_entry_new(qPrintable(cur_net.protocol), qPrintable(cur_net.login), qPrintable(cur_net.host), cur_net.port);
			res = ppk_module_remove_entry(m_module->id, entry, 0);
		}
		else if (cur_type == ppk_item)
		{
			entry=ppk_item_entry_new(qPrintable(cur_item.key));
			res = ppk_module_remove_entry(m_module->id, entry, 0);
		}
		else
		{
			QMessageBox::critical(this, tr("PPassKeeper : Error while deleting ..."), tr("The current type of the selected password is unknown."));
			return;
		}

		if(res!=PPK_OK)
		{
			char key[101];
			ppk_get_key(entry, key, sizeof(key)-1);
			QString error=tr("An error occured while deleting the entry '%1'\n\nError : %2").arg(QString::fromUtf8(key)).arg(QString::fromUtf8(ppk_error_get_string(res)));
			QMessageBox::critical(this, tr("PPassKeeper : Error while deleting ..."), error);
		}
		else
			listCurrentModule();
	}
}

void MainWindow::onParamsTriggered()
{
	EditParams params;
	params.setModule(m_module);

	params.exec();

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
	infomod.setModule(m_module->id);
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
		ppk_data* data=getSelectedEntryData(ok);
		if (ok)
		{
			if(data->type==ppk_string)
			{
				passwordEdit->setPlainText(QString::fromUtf8(data->string));
				passwordEdit->setReadOnly(false);
				tmp_sensitive_data=QString::fromUtf8(data->string);

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

			ppk_data_free(data);
		}
		else
		{
			QMessageBox::critical(this, tr("An error occured while reading the password"), tr("An error occured while reading the password !\n\nThe error is : TODO\n"));
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
	cur_app.app_name = QString::fromUtf8(app_name);
	cur_app.username = QString::fromUtf8(username);

	onPasswordSelected();
}

void MainWindow::onNetPasswordSelected(const char *host, const char *login, unsigned short int port)
{
	cur_net.host = QString::fromUtf8(host);
	cur_net.login = QString::fromUtf8(login);
	cur_net.port = port;

	onPasswordSelected();
}

void MainWindow::onItemPasswordSelected(const char *key)
{
	cur_item.key = QString::fromUtf8(key);

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
	ppk_data* data=getSelectedEntryData(ok);
	if (ok)
	{
		QString filepath=QFileDialog::getSaveFileName(this, tr("Save Value"), QString(), tr("All (*.*)"));
		if (filepath.isEmpty())
			return;

		QFile file(filepath);
		file.open(QIODevice::WriteOnly);
		if(file.isWritable())
		{
			if(data->type==ppk_string)
				file.write(data->string);
			else if(data->type==ppk_blob)
				file.write((const char*)data->blob.data, data->blob.size);
			else
				QMessageBox::critical(this, tr("An error occured while reading the password"), tr("Reading error : Unknown data type."));
		}
		else
			QMessageBox::critical(this, tr("Writing Error : The file is not writable"), tr("Writing error : The file is not writable.\n\nCheck your file permission or try another one."));

		ppk_data_free(data);
	}
	else
		QMessageBox::critical(this, tr("An error occured while reading the password"), tr("An error occured while reading the password !\n\nThe error is : TODO\n"));
}

void MainWindow::setBlobFromFile()
{
	QString filepath=QFileDialog::getOpenFileName(this, tr("Open a file to save"), QString(), tr("All (*.*)"));
	if (filepath.isEmpty())
		return;

	QFile file(filepath);
	if((size_t)file.size()<=ppk_module_max_data_size(m_module->id, ppk_blob))
	{
		file.open(QIODevice::ReadOnly);
		if(file.isReadable())
		{
			QByteArray fdata=file.readAll();
			ppk_data* data=ppk_blob_data_new(fdata.data(), fdata.size());
			if(data)
			{
				if(updateSelectedPassword(data))
					QMessageBox::information(this, tr("Success : The blob has been set correctly"), tr("The blob has been set correctly."));
				else
					QMessageBox::critical(this, tr("PPassKeeper Error : The entry cannot be set"), tr("PPassKeeper Error : The entry cannot be set.\n\nThe error is : TODO\n"));

				ppk_data_free(data);
			}
			else
				QMessageBox::critical(this, tr("Error : Could not create the blob"), tr("The blob cannot be created."));
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
	ppk_module* module = modulesBox->itemData(index).value<ppk_module*>();
	if (module != NULL)
	{
		m_module = module;
		listCurrentModule();

		/* the displayed password becomes unavailable
		 * after a module change */
		cur_availability = false;

		bool is_default=(m_module->id==ppk_module_get_default());

		toolBar->setEnabled(true);
		actionSetModuleDefault->setEnabled(true);
		actionSetModuleDefault->setChecked(is_default);
	}
}

void MainWindow::listCurrentModule()
{
	pwdlistModel->setupModelData(m_module->id);
}

void MainWindow::focusChanged(QWidget* q_old, QWidget* /*q_new*/)
{
	//If focus was on passwordEdit and passwordEdit's content has changed and the password was currently edited, save it
	if (passwordTimer.isActive() && q_old==passwordEdit && passwordEdit->toPlainText()!=tmp_sensitive_data)
	{
		if(QMessageBox::question(this, tr("Would you like to save ..."),
							tr("You have modified the data stored.\n\nWould you like to save your modifications ?"),
							QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes)
		{
			ppk_data* data=ppk_string_data_new(passwordEdit->toPlainText().toUtf8().data());
			updateSelectedPassword(data);
			ppk_data_free(data);
		}

		setPasswordVisible(false);
	}
}

bool MainWindow::parseAndGetNetworkEntry(std::string str, ppk_entry& entry)
{
	//Parse the file's name
	size_t pos_at=str.find_first_of("@");
	size_t pos_sc=str.find(":",pos_at+1);

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
	size_t pos_at=str.find_first_of("@");

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

