#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <ppasskeeper.h>
#include <QBasicTimer>

#include "mainwindow_ui.h"
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

	QBasicTimer passwordTimer;

	void setupActions();
	void fillModulesBox();
	void listCurrentModule();

protected:
	void timerEvent(QTimerEvent *event);
	uint timerValue;

public slots:
	void onAppPasswordSelected(const char *app_name, const char *username);
	void onNetPasswordSelected(const char *host, const char *login, unsigned short int port);
	void onItemPasswordSelected(const char *key);
	void setPasswordVisible(bool b);
	void updateSelectedPassword(QString pwd);
	void focusChanged(QWidget* q_old, QWidget* q_new);

private slots:
	void moduleChanged(int index);
	void updateInfoLabel();
	void onShowButtonToggled(bool b);
};

#endif
