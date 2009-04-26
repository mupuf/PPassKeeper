#include "infomodule.h"
#include "ui_infomodule.h"

InfoModule::InfoModule(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::InfoModule)
{
    m_ui->setupUi(this);
}

InfoModule::~InfoModule()
{
    delete m_ui;
}

void InfoModule::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
