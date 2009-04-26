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

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::InfoModule *m_ui;
};

#endif // INFOMODULE_H
