#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mainwindow_ui.h"
#include "passwordlistmodel.h"

class MainWindow : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT
public:
	MainWindow();
	const QString &moduleId() const { return m_moduleId; };

private:
	QString m_moduleId;
	PasswordListModel *pwdlistModel;

	void setupActions();
	void fillModulesBox();
	void listCurrentModule();

private slots:
	void moduleChanged(int index);
};

#endif
