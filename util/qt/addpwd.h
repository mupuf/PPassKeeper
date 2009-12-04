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
private:
	Ui::AddPWD *m_ui;
	bool cancel, success;
	const char* module;
	QString _addedEntry;

	void hideAllEntryTypes();
	void showApp(bool show);
	void showNet(bool show);
	void showItem(bool show);

	QString generateKey(const ppk_entry* entry);

protected:
	virtual void changeEvent(QEvent *e);

public:
	explicit AddPWD(QWidget *parent = 0);
	virtual ~AddPWD();

	bool succeeded();
	bool cancelled();
	QString addedEntry();

	void setModule(const char* module);
	void setType(ppk_entry_type type);

private slots:
	void entryTypeChanged(int);
	void checkConsistancy();
	void onOK();
	void onCancel();
};

#endif // ADDPWD_H
