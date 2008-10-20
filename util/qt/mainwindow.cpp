#include "mainwindow.h"

#include <ppasskeeper.h>

MainWindow::MainWindow()
	: QMainWindow()
{
	setupUi(this);
	setupActions();

	fillModulesBox();
	pwdlistModel = new PasswordListModel(this);
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
	pwdlistModel->setupModelData(m_moduleId.toLocal8Bit().data());
}
