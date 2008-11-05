#include "mainwindow.h"

MainWindow::MainWindow()
	: QMainWindow(),
	cur_availability(false)
{
	setupUi(this);

	pwdlistModel = new PasswordListModel(this);
	pwdlistView->setModel(pwdlistModel);

	setupActions();

	setWindowTitle(qApp->applicationName());

	fillModulesBox();
}

void MainWindow::fillModulesBox()
{
	modulesBox->clear();

	unsigned int n = ppk_getAvailableModulesCount();
	struct ppk_module modules[n];									//HACK !
	ppk_getAvailableModules(modules, n);

	modulesBox->addItem("Select one:");
	modulesBox->insertSeparator(1);

	for (int i = 0; i < n; ++i)
	{
		/* we are only interested in modules that can be listed and that can actually store data */
		if (ppk_isWritable(modules[i].id))
		{
			modulesBox->addItem(modules[i].display_name, QVariant(modules[i].id));
		}
	}
}

void MainWindow::setupActions()
{
	connect(action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(modulesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(moduleChanged(int)));
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

	connect(showButton, SIGNAL(toggled(bool)), this, SLOT(onShowButtonToggled(bool)));
	connect(showButton, SIGNAL(clicked(bool)), this, SLOT(setPasswordVisible(bool)));
	connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
}

void MainWindow::onShowButtonToggled(bool b)
{
	if (b) showButton->setText(tr("Hide value"));
	else showButton->setText(tr("Show value"));
}

void MainWindow::updateSelectedPassword(QString pwd)
{
	bool res;
	if (cur_type == ppk_application)
	{
		res = ppk_setEntry(m_moduleId.toLocal8Bit().constData(),
				createAppEntry(cur_app.app_name.toLocal8Bit().constData(), cur_app.username.toLocal8Bit().constData()),
				createStringEntryData(pwd.toLocal8Bit().constData()), 0)==PPK_TRUE;
	}
	else if (cur_type == ppk_network)
	{
		res = ppk_setEntry(m_moduleId.toLocal8Bit().constData(),
				createNetworkEntry(cur_net.host.toLocal8Bit().constData(), cur_net.login.toLocal8Bit().constData(), cur_net.port),
				createStringEntryData(pwd.toLocal8Bit().constData()), 0)==PPK_TRUE;
	}
	else if (cur_type == ppk_item)
	{
		res = ppk_setEntry(m_moduleId.toLocal8Bit().constData(),
				createItemEntry(cur_item.key.toLocal8Bit().constData()),
				createStringEntryData(pwd.toLocal8Bit().constData()), 0)==PPK_TRUE;
	}
}

#include <QMessageBox>
void MainWindow::setPasswordVisible(bool b)
{
	if (b)
	{
		if (! cur_availability)
		{
			showButton->setChecked(false);
			return;
		}

		struct ppk_data data;
		ppk_boolean res;
		if (cur_type == ppk_application)
		{
			res = ppk_getEntry(m_moduleId.toLocal8Bit().constData(),
					createAppEntry(cur_app.app_name.toLocal8Bit().constData(), cur_app.username.toLocal8Bit().constData()),
					&data, 0);
		}
		else if (cur_type == ppk_network)
		{
			res = ppk_getEntry(m_moduleId.toLocal8Bit().constData(),
					createNetworkEntry(cur_net.host.toLocal8Bit().constData(), cur_net.login.toLocal8Bit().constData(), cur_net.port),
					&data, 0);
		}
		else if (cur_type == ppk_item)
		{
			res = ppk_getEntry(m_moduleId.toLocal8Bit().constData(),
					createItemEntry(cur_item.key.toLocal8Bit().constData()),
					&data, 0);
		}
		if (res==PPK_TRUE)
		{		
			passwordEdit->setPlainText(data.string);
			tmp_sensitive_data=data.string;
			timerValue = 0;
			passwordTimer.start(1000, this);
		}
	} else {
		passwordTimer.stop();
		progressBar->setValue(0);
		passwordEdit->setPlainText(tr("(hidden)"));
		tmp_sensitive_data=QString();
		showButton->setChecked(false);
	}
}

void MainWindow::timerEvent(QTimerEvent *event)
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

void MainWindow::onAppPasswordSelected(const char *app_name, const char *username)
{
	cur_availability = true;
	cur_type = ppk_application;
	cur_app.app_name = app_name;
	cur_app.username = username;
	updateInfoLabel();
}

void MainWindow::onNetPasswordSelected(const char *host, const char *login, unsigned short int port)
{
	cur_availability = true;
	cur_type = ppk_network;
	cur_net.host = host;
	cur_net.login = login;
	cur_net.port = port;
	updateInfoLabel();
}

void MainWindow::onItemPasswordSelected(const char *key)
{
	cur_availability = true;
	cur_type = ppk_item;
	cur_item.key = key;
	updateInfoLabel();
}

void MainWindow::updateInfoLabel()
{
	QString str;
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
	}
}

void MainWindow::listCurrentModule()
{
	pwdlistModel->setupModelData(m_moduleId.toLocal8Bit().constData());
}

void MainWindow::focusChanged(QWidget* q_old, QWidget* q_new)
{
	//If focus was on passwordEdit and passwordEdit's content has changed and the password was currently edited, save it
	if (passwordTimer.isActive() && q_old==passwordEdit && passwordEdit->toPlainText()!=tmp_sensitive_data)
	{
		if(QMessageBox::question(this, tr("Would you like to save ..."), tr("You have modified the data stored.\n\nWould you like to save your modifications ?"), QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes)		
			updateSelectedPassword(passwordEdit->toPlainText());	
		setPasswordVisible(false);	
	}
}
