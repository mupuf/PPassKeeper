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

Q_DECLARE_METATYPE(char*); // to make it storable in QVariant

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
	connect(pwdlistModel, SIGNAL(onPasswordSelected(const char *)), this, SLOT(onPasswordSelected()));
	connect(pwdlistModel, SIGNAL(noItemSelected()), this, SLOT(onNoItemSelected()));
	connect(pwdlistModel, SIGNAL(modelReset()), pwdlistView, SLOT(expandAll())); //always expand the tree
	connect(pwdlistView, SIGNAL(pressed(QModelIndex)), this, SLOT(onPwdViewClick(QModelIndex)));

	connect(showButton, SIGNAL(toggled(bool)), this, SLOT(onShowButtonToggled(bool)));
	connect(showButton, SIGNAL(clicked(bool)), this, SLOT(setPasswordVisible(bool)));

	connect(saveValueButton, SIGNAL(clicked()), this, SLOT(saveValueToFile()));
	connect(setBlobButton, SIGNAL(clicked()), this, SLOT(setBlobFromFile()));

	connect(filterNameCheckbox, SIGNAL(toggled(bool)), pwdlistModel, SLOT(useFilter(bool)));
	connect(filterNameText, SIGNAL(textEdited(QString)), pwdlistModel, SLOT(setFilter(QString)));

	connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));

	action_Del->setEnabled(false);
}

void MainWindow::fillModulesBox()
{
	modulesBox->clear();
	
	char** list=ppk_module_list_new();

	modulesBox->addItem(tr("Select one:"), qVariantFromValue(QString()));
	modulesBox->insertSeparator(1);

	for (unsigned int i = 0; list[i]!=NULL; ++i)
	{
		modulesBox->addItem(QString::fromUtf8(ppk_module_display_name(list[i])), qVariantFromValue(QString::fromUtf8(list[i])));
	}
	ppk_module_list_free(list);
}

void MainWindow::listCurrentModule()
{
	pwdlistModel->setupModelData(module());
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

void MainWindow::showInfoMessageUnderDevelopment()
{
	QMessageBox::information(this, tr("PPassKeeper: This is still a development version"), tr("This function has not been implemented yet. \n\nPPassKeeper is still under heavy development, so, we do apologize."));
}

bool MainWindow::unlockPPK(bool force)
{
	if(ppk_is_locked()==PPK_TRUE)
	{
		bool ok;
		std::string pwd=QInputDialog::getText(NULL, tr("Unlock PPassKeeper"), tr("Please key in the password to unlock PPassKeeper:"), QLineEdit::Password,QString(), &ok).toStdString();

		if(ok)
		{
			ppk_unlock(pwd.c_str());
			if(ppk_is_locked()==PPK_FALSE)
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

ppk_data* MainWindow::getSelectedEntryData()
{
	ppk_data* data=NULL;

	ppk_entry* entry=ppk_entry_new_from_key(qPrintable(pwdlistModel->selectedEntry()));
	if(!entry)
		return NULL;

	ppk_error res = ppk_module_get_entry(module(), entry, &data, 0);
	if(res!=PPK_OK)
	{
		char key[101];
		if(ppk_get_key(entry, key, sizeof(key)-1)==PPK_FALSE)
			strncpy(key, "<invalid_entry>", sizeof(key)-1);

		QString error=tr("An error occured while accessing the entry '%1'\n\nError : %2").arg(QString::fromUtf8(key)).arg(QString::fromUtf8(ppk_error_get_string(res)));
		QMessageBox::critical(this, tr("PPK: Access error"), error);
	}

	//Free the entry
	ppk_entry_free(entry);

	return data;
}

bool MainWindow::updateSelectedPassword(ppk_data* data)
{
	ppk_entry* entry=ppk_entry_new_from_key(qPrintable(pwdlistModel->selectedEntry()));
	if(!entry)
		return false;

	ppk_error res = ppk_module_set_entry(module(), entry, data, 0);
	if(res!=PPK_OK)
	{
		char key[101];
		ppk_get_key(entry, key, sizeof(key)-1);
		QString error=tr("An error occured while updating the entry '%1'\n\nError: %2").arg(QString::fromUtf8(key)).arg(QString::fromUtf8(ppk_error_get_string(res)));
		QMessageBox::critical(this, tr("PPK: Update Error"), error);
	}

	//Free the entry
	ppk_entry_free(entry);

	return res==PPK_OK;
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



/**********************
 *****   Public   *****
 **********************/
MainWindow::MainWindow()
	: QMainWindow(),
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

}


/************************
 ***** Public slots *****
 ************************/
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
				ppk_error res=ppk_set_password(pwd.c_str());
				if(res==PPK_OK)
					QMessageBox::information(this, tr("The password has been set"), tr("The password you entered has been set as the new master password."));
				else
					QMessageBox::critical(this, tr("Error: An error occured"), tr("PPassKeeper was unable to set this password.\nError : %1").arg(QString::fromUtf8(ppk_error_get_string(res))));
			}
			else
				QMessageBox::critical(this, tr("Error: The password are not the same"), tr("The two passwords you entered are not matching.\nTry again ..."));
		}
	}
}


/*************************
 ***** Private slots *****
 *************************/
//Tool bar
void MainWindow::onAddButtonClicked()
{
	AddPWD addpwd;

	addpwd.setType(pwdlistModel->selectedType());
	addpwd.setModule(module());
	addpwd.setModal(true);
	addpwd.show();
	addpwd.exec();

	if(addpwd.succeeded())
		listCurrentModule();
}

void MainWindow::onDelButtonClicked()
{
	QString error;


	QString title=tr("Are your sure ?");
	QString text= tr("Are you sure you want to delete the entry '%1' from the module '%2' ?").arg(pwdlistModel->selectedEntry()).arg(QString::fromUtf8(module()));
	if(QMessageBox::question(this, title, text, QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes)
	{
		ppk_entry* entry=ppk_entry_new_from_key(qPrintable(pwdlistModel->selectedEntry()));
		if(!entry)
		{
			QMessageBox::critical(this, tr("PPassKeeper : Error while deleting ..."), tr("The current type of the selected password is unknown."));
			return;
		}

		ppk_error res = ppk_module_remove_entry(module(), entry, 0);
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
	infomod.setModule(module());
	infomod.setModal(true);
	infomod.show();
	infomod.exec();
}

void MainWindow::onParamsTriggered()
{
	EditParams params;
	params.setModule(module());

	params.exec();

}


//Module
void MainWindow::moduleChanged(int index)
{
	QString module = modulesBox->itemData(index).toString();
	if (module != QString())
	{
		if(m_module!=NULL)
			delete[] m_module;
		m_module=new char[module.size()+2];
		strncpy(m_module, qPrintable(module), module.size()+1);

		listCurrentModule();

		/* the displayed password becomes unavailable
		 * after a module change */
		cur_availability = false;
	}

	bool isValid=index>0;

	bool is_default=(module == QString::fromUtf8(ppk_module_get_default()));
	actionSetModuleDefault->setEnabled(isValid);
	actionSetModuleDefault->setChecked(is_default);

	action_Add->setEnabled(isValid);
	actionInfos->setEnabled(isValid);
	actionParams->setEnabled(isValid);
	action_Import->setEnabled(isValid);
	action_Export->setEnabled(isValid);
}

void MainWindow::onSetDefaultModule()
{
	ppk_module_set_default(module());
}


//Password management
void MainWindow::onNoItemSelected()
{
	showButton->setEnabled(false);
	action_Del->setEnabled(false);
	setBlobButton->setEnabled(false);
	saveValueButton->setEnabled(false);

	cur_availability = false;
	updateInfoLabel();
}

void MainWindow::onPwdViewClick(const QModelIndex& item)
{
	if(item.isValid() && QApplication::mouseButtons()==Qt::RightButton)
	{
		QMenu menu(this);

		menu.addAction(this->action_Add);
		menu.addAction(this->action_Del);

		menu.exec(QCursor::pos());
	}
}


//Entry column
void MainWindow::onPasswordSelected()
{
	cur_availability = true;
	showButton->setEnabled(true);
	setBlobButton->setEnabled(true);
	saveValueButton->setEnabled(true);
	action_Del->setEnabled(true);
	updateInfoLabel();
}

void MainWindow::updateInfoLabel()
{
	QString str;

	ppk_entry* entry=ppk_entry_new_from_key(qPrintable(pwdlistModel->selectedEntry()));
	if (entry==NULL || !cur_availability)
	{
		str = tr("(none selected)");
	}
	else if (entry->type == ppk_application)
	{
		str = tr("Application name: %1\n"
				"User name: %2").arg(QString::fromUtf8(entry->app.app_name)).arg(QString::fromUtf8(entry->app.username));
	}
	else if (entry->type == ppk_network)
	{
		QString host=QString::fromUtf8(entry->net.host);
		QString login=QString::fromUtf8(entry->net.login);
		QString protocol=QString::fromUtf8(entry->net.protocol);

		str = tr("Host name: %1\n"
				"Login: %2\n"
				"Port: %3\n"
				"Protocol: %4").arg(host).arg(login).arg(entry->net.port).arg(protocol);
	}
	else if (entry->type == ppk_item)
	{
		str = tr("Key: %1").arg(QString::fromUtf8(entry->item));
	}

	infoLabel->setText(str);
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

		ppk_data* data=getSelectedEntryData();
		if (data)
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
			setPasswordVisible(false);
	} else {
		passwordTimer.stop();
		progressBar->setValue(0);
		passwordEdit->setPlainText(tr("(hidden)"));
		passwordEdit->setReadOnly(true);
		tmp_sensitive_data=QString();
		showButton->setChecked(false);
	}
}

void MainWindow::onShowButtonToggled(bool b)
{
	if (b) showButton->setText(tr("Hide value"));
	else showButton->setText(tr("Show value"));
}

void MainWindow::saveValueToFile()
{
	ppk_data* data=getSelectedEntryData();
	if (data)
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
}

void MainWindow::setBlobFromFile()
{
	QString filepath=QFileDialog::getOpenFileName(this, tr("Open a file to save"), QString(), tr("All (*.*)"));
	if (filepath.isEmpty())
		return;

	QFile file(filepath);
	if((size_t)file.size()<=ppk_module_max_data_size(module(), ppk_blob))
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
					QMessageBox::critical(this, tr("PPassKeeper Error : The entry cannot be set"), tr("PPassKeeper Error : The entry cannot be set."));

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
