#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <ppasskeeper.h>

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

	/* description of the current selected password
	 * availability: whether the password can be read/written
	 *    false when the program was just started (no pass selected),
	 *    or a module change..
	 * type: type of the current password
	 *    describes what data structure contains the corresponding data
	 */
	bool cur_availability;
	enum ppk_password_type cur_type;
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
	} cur_net;
	struct
	{
		QString key;
	} cur_item;

	void setupActions();
	void fillModulesBox();
	void listCurrentModule();

public slots:
	void onAppPasswordActivated(const char *app_name, const char *username);
	void onNetPasswordActivated(const char *host, const char *login, unsigned short int port);
	void onItemPasswordActivated(const char *key);
	void setPasswordVisible(bool b);

private slots:
	void moduleChanged(int index);
	void updateInfoLabel();
};

#endif
