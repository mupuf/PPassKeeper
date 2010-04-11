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
