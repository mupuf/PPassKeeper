#ifndef EDITPARAMS_H
#define EDITPARAMS_H

#include <QtGui/QDialog>

namespace Ui {
    class EditParams;
}

class EditParams : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(EditParams)
public:
    explicit EditParams(QWidget *parent = 0);
    virtual ~EditParams();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::EditParams *m_ui;
};

#endif // EDITPARAMS_H
