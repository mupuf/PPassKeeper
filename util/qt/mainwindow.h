#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QBasicTimer>

#include "ui_mainwindow.h"
#include "passwordlistmodel.h"

class MainWindow : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT
public:
	MainWindow();
	const QString &moduleId() const { return m_moduleId; };

private:
	QString m_moduleId, tmp_sensitive_data;
	PasswordListModel *pwdlistModel;

	/* description of the current selected password
	 * availability: whether the password can be read/written
	 *    false when the program was just started (no pass selected),
	 *    or a module change..
	 * type: type of the current password
	 *    describes what data structure contains the corresponding data
	 */
	bool cur_availability;
	struct
	{
		QString app_name;
		QString username;
	} cur_app;
	struct
	{
		QString host;
		QString login;
		unsigned short int port;
		QString protocol;
	} cur_net;
	struct
	{
		QString key;
	} cur_item;

	QBasicTimer passwordTimer;

	void setupActions();
	void fillModulesBox();
	void listCurrentModule();

	bool parseAndGetNetworkEntry(std::string str, ppk_entry& entry);
	bool parseAndGetAppEntry(std::string str, ppk_entry& entry);
	bool parseAndGetItemEntry(std::string str, ppk_entry& entry);

	void showInfoMessageUnderDevelopment();
	bool unlockPPK(bool force=false);

	ppk_data* getSelectedEntryData(bool& ok);
	bool updateSelectedPassword(ppk_data* data);

protected:
	void timerEvent(QTimerEvent *event);
	uint timerValue;

public slots:
	void setMasterPwd();
	void onAppPasswordSelected(const char *app_name, const char *username);
	void onNetPasswordSelected(const char *host, const char *login, unsigned short int port);
	void onItemPasswordSelected(const char *key);
	void onNoItemSelected();
	void onAddButtonClicked();
	void onDelButtonClicked();
	void onImportButtonClicked();
	void onExportButtonClicked();
	void onInfoModuleButtonClicked();
	void setPasswordVisible(bool b);
	void focusChanged(QWidget* q_old, QWidget* q_new);

private slots:
	void moduleChanged(int index);
	void updateInfoLabel();
	void onShowButtonToggled(bool b);
	void onPasswordSelected();
	void saveValueToFile();
	void setBlobFromFile();
	void onSetDefaultModule();
	void onParamsTriggered();
};

#endif
