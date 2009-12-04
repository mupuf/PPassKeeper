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
