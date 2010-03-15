#include "qmodulefield.h"
#include <QVariant>

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

		if(parameters->allow_self==PPK_FALSE && cur_module==QString::fromUtf8(c_mod))
			continue;
		if(parameters->writable_only==PPK_TRUE && ppk_module_is_writable(c_mod)==PPK_FALSE)
			continue;
		if(parameters->min_sec_level>ppk_module_security_level(qPrintable(cur_module)))
			continue;
		
		//get the flags
		unsigned int read_flags, write_flags, listing_flags;
		ppk_module_read_flags(c_mod, &read_flags);
		ppk_module_write_flags(c_mod, &write_flags);
		ppk_module_listing_flags(c_mod, &listing_flags);
		
		//test the flags
		if((read_flags&parameters->needed_read_flags)==read_flags)
			continue;
		if((write_flags&parameters->needed_write_flags)==write_flags)
			continue;
		if((listing_flags&parameters->needed_listing_flags)==listing_flags)
			continue;
		
		//Add to the list
		newList.push_back(QString::fromUtf8(c_mod));
	}

	//Set the current list as the combo's list
	setList(newList);
	
	//Free the ppk module list
	ppk_module_list_free(list);
	
}
