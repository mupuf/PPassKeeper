/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QBasicTimer>

#include "ui_mainwindow.h"
#include "passwordlistmodel.h"

#include <ppasskeeper/module.h>

class MainWindow : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT
private:
	char* m_module; //current module
	QString tmp_sensitive_data;
	PasswordListModel *pwdlistModel;

	/* description of the current selected password
	 * availability: whether the password can be read/written
	 *    false when the program was just started (no pass selected),
	 *    or a module change..
	 */
	bool cur_availability;

	uint timerValue;
	QBasicTimer passwordTimer;

	void setupActions();
	void fillModulesBox();
	void listCurrentModule();

	bool parseAndGetNetworkEntry(std::string str, ppk_entry& entry);
	bool parseAndGetAppEntry(std::string str, ppk_entry& entry);
	bool parseAndGetItemEntry(std::string str, ppk_entry& entry);

	void showInfoMessageUnderDevelopment();
	bool unlockPPK(bool force=false);

	ppk_data* getSelectedEntryData();
	bool updateSelectedPassword(ppk_data* data);

	void timerEvent(QTimerEvent *event);

public:
	MainWindow();
	~MainWindow();
	const char* module() const { return m_module; };

public slots:
	void setMasterPwd();

private slots:
	//Tool bar
	void onAddButtonClicked();
	void onDelButtonClicked();
	void onImportButtonClicked();
	void onExportButtonClicked();
	void onInfoModuleButtonClicked();
	void onParamsTriggered();

	//Module
	void moduleChanged(int index);
	void onSetDefaultModule();

	//Password management
	/*void onAppPasswordSelected(const char *app_name, const char *username);
	void onNetPasswordSelected(const char *host, const char *login, unsigned short int port);
	void onItemPasswordSelected(const char *key);*/
	void onNoItemSelected();
	void onPwdViewClick(const QModelIndex& item);

	//Entry column
	void onPasswordSelected();
	void updateInfoLabel();
	void setPasswordVisible(bool b);
	void onShowButtonToggled(bool b);
	void saveValueToFile();
	void setBlobFromFile();
	void focusChanged(QWidget* q_old, QWidget* q_new);
};

#endif
