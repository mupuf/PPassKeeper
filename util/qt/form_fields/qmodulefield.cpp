#include "qmodulefield.h"
#include <QVariant>

#include <ppasskeeper/param_prototype.h>

QModuleField::QModuleField(QWidget* parent, QString cur_module, const ppk_proto_param_module* parameters) : QComboField(parent)
{
	QList<QString> newList;

	char** list=ppk_module_list_new();

	//For every module listed
	int i=0;
	while(list!=NULL && list[i]!=NULL)
	{
		const char* c_mod=list[i];

		//increment the iterator pointer
		i++;

		//Add to the list
		if(ppk_param_module_validation(qPrintable(cur_module), parameters, c_mod))
			newList.push_back(QString::fromUtf8(c_mod));
	}

	//Set the current list as the combo's list
	setList(newList);
	
	//Free the ppk module list
	ppk_module_list_free(list);
	
}
