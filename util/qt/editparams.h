#ifndef EDITPARAMS_H
#define EDITPARAMS_H

#include <QtGui/QDialog>
#include "paramslistmodel.h"
#include <QAbstractButton>

namespace Ui {
    class EditParams;
}

class EditParams : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(EditParams)
public:
	explicit EditParams(QWidget *parent = 0);
	virtual ~EditParams();

	void setModule(QString module_name);

protected:
	virtual void changeEvent(QEvent *e);

private slots:
	void buttonBoxClicked(QAbstractButton* bbox);
	void resetParamClicked();
	void helpParamClicked();
private:
	Ui::EditParams *m_ui;
	ParamsListModel model;

	QMap<QObject*, QPair<ppk_proto_param, QWidget*> > paramAssociated;

	void resetParam(QObject* resetSender);
	void addParam(int line, ppk_proto_param pparam);
	QWidget* paramEditWidget(ppk_proto_param pparam);
};

#endif // EDITPARAMS_H
