#include "editparams.h"
#include "ui_editparams.h"

EditParams::EditParams(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::EditParams)
{
    m_ui->setupUi(this);
}

EditParams::~EditParams()
{
    delete m_ui;
}

void EditParams::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
