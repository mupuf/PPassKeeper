#include "mainwindow.h"

MainWindow::MainWindow()
	: QMainWindow(),
	cur_availability(false)
{
	pwdlistModel = new PasswordListModel(this);

	setupUi(this);
	setupActions();

	fillModulesBox();
	pwdlistView->setModel(pwdlistModel);
}

void MainWindow::fillModulesBox()
{
	modulesBox->clear();

	unsigned int n = ppk_getAvailableModulesCount();
	struct PPassKeeper_Module modules[n];
	ppk_getAvailableModules(modules, n);

	modulesBox->addItem("Select one:");
	modulesBox->insertSeparator(1);

	for (int i = 0; i < n; ++i)
	{
		/* we are only interested in modules that can be listed */
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
	connect(pwdlistView, SIGNAL(activated(const QModelIndex &)),
			pwdlistModel, SLOT(activatedInView(const QModelIndex &)));
	connect(pwdlistModel,
			SIGNAL(appPasswordActivated(const char *, const char *)),
			this,
			SLOT(onAppPasswordActivated(const char *, const char *)));
	connect(pwdlistModel,
			SIGNAL(netPasswordActivated(const char *, const char *, unsigned short int)),
			this,
			SLOT(onNetPasswordActivated(const char *, const char *, unsigned short int)));
	connect(pwdlistModel,
			SIGNAL(itemPasswordActivated(const char *)),
			this,
			SLOT(onItemPasswordActivated(const char *)));

	connect(showButton, SIGNAL(toggled(bool)), this, SLOT(onShowButtonToggled(bool)));
	connect(showButton, SIGNAL(clicked(bool)), this, SLOT(setPasswordVisible(bool)));
}

void MainWindow::onShowButtonToggled(bool b)
{
	if (b) showButton->setText(tr("Hide value"));
	else showButton->setText(tr("Show value"));
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

		const char *pwd;
		if (cur_type == ppk_application)
		{
			pwd = ppk_getApplicationPassword(m_moduleId.toLocal8Bit().constData(),
					cur_app.app_name.toLocal8Bit().constData(),
					cur_app.username.toLocal8Bit().constData(), 0);
		}
		else if (cur_type == ppk_network)
		{
			pwd = ppk_getNetworkPassword(m_moduleId.toLocal8Bit().constData(),
					cur_net.host.toLocal8Bit().constData(), cur_net.port,
					cur_net.login.toLocal8Bit().constData(), 0);
		}
		else if (cur_type == ppk_item)
		{
			pwd = ppk_getItem(m_moduleId.toLocal8Bit().constData(),
					cur_item.key.toLocal8Bit().constData(), 0);
		}
		if (pwd)
		{
			passwordLabel->setText(pwd);
			timerValue = 0;
			passwordTimer.start(1000, this);
		}
	} else {
		passwordTimer.stop();
		progressBar->setValue(0);
		passwordLabel->setText(tr("(hidden)"));
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

void MainWindow::onAppPasswordActivated(const char *app_name, const char *username)
{
	cur_availability = true;
	cur_type = ppk_application;
	cur_app.app_name = app_name;
	cur_app.username = username;
	updateInfoLabel();
}

void MainWindow::onNetPasswordActivated(const char *host, const char *login, unsigned short int port)
{
	cur_availability = true;
	cur_type = ppk_network;
	cur_net.host = host;
	cur_net.login = login;
	cur_net.port = port;
	updateInfoLabel();
}

void MainWindow::onItemPasswordActivated(const char *key)
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
	}
}

void MainWindow::listCurrentModule()
{
	pwdlistModel->setupModelData(m_moduleId.toLocal8Bit().constData());
}
