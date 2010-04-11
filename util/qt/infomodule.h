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

#ifndef INFOMODULE_H
#define INFOMODULE_H

#include <QtGui/QDialog>

namespace Ui {
    class InfoModule;
}

class InfoModule : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(InfoModule)
public:
    explicit InfoModule(QWidget *parent = 0);
    virtual ~InfoModule();

    void setModule(const char* m_id);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::InfoModule *m_ui;

    QString listFlags(unsigned int flags);
};

#endif // INFOMODULE_H
