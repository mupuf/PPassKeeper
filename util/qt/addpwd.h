#ifndef ADDPWD_H
#define ADDPWD_H

#include <QtGui/QDialog>
#include <ppasskeeper.h>

namespace Ui {
    class AddPWD;
}

class AddPWD : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(AddPWD)
public:
	explicit AddPWD(QWidget *parent = 0);
	virtual ~AddPWD();

	bool succeeded();
	bool cancelled();

	void setModule(QString module);
	void setType(ppk_entry_type type);

protected:
	virtual void changeEvent(QEvent *e);

private:
	void hideAll();
	void showApp(bool show);
	void showNet(bool show);
	void showItem(bool show);

	Ui::AddPWD *m_ui;
	bool cancel, success;
	QString module_id;

private slots:
	void entryTypeChanged(int);
	void onOK();
	void onCancel();
};

#endif // ADDPWD_H
