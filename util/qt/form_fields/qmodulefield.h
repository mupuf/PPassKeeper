#ifndef QMODULEFIELD_H
#define QMODULEFIELD_H

#include "qcombofield.h"
#include "ppasskeeper.h"
#include "ppasskeeper/param_prototype.h"

class QModuleField : public QComboField
{
	Q_OBJECT

public:
	QModuleField(QWidget* parent=NULL, QString cur_module=QString(), const ppk_proto_param_module* parameters=NULL);
};

#endif // QTEXTFIELD_H
