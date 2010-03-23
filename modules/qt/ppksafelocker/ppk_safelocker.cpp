#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

#include "safelock.h"

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

//Parameters
std::map<std::string, cvariant> parameters;
std::map<std::string, ppk_settings_group*> params_group;
std::map<std::string, ppk_proto_param*> proto_params;
ppk_proto_param** availParams;
#define PARAM_MOD_PASSPHRASE "module passphrase"
#define PARAM_CLOSING_DELAY "closing delay"

#define PARAM_MOD_PASSPHRASE_DEFAULT "AskForPass_Qt"
#define PARAM_CLOSING_DELAY_DEFAULT 10

//
extern "C" const char* getModuleID();

//Private functions
SafeLock& safeLock()
{
	static SafeLock sf(QString::fromUtf8(ppk_settings_directory())+QString::fromUtf8("/ppk_safelock.crypted"));
	return sf;
}

ppk_error lazyInit()
{
	static QString passphrase;
	
	//get the passphrase
	if(passphrase==QString())
	{
		const char* module=cvariant_get_string(parameters[PARAM_MOD_PASSPHRASE]);
		ppk_entry* entry=ppk_application_entry_new("passphrase", getModuleID());
		
		ppk_data* edata;
		ppk_error res=ppk_module_get_entry(module, entry, &edata, ppk_rf_none);
		ppk_entry_free(entry);
		
		if(res==PPK_OK)
			passphrase=QString::fromUtf8(ppk_get_data_string(edata));
		else
			return res;
	}
	
	if(!safeLock().isOpen())
	{
		ppk_error ret=safeLock().open(qPrintable(passphrase));
		
		//If the password is incorrect, reset the password
		if(ret==PPK_INVALID_PASSWORD)
			passphrase=QString();
		
		return ret;
	}
	
	return PPK_OK;
}

//functions
extern "C"
{
	void constructor()
	{
		availParams=NULL;
		
		//Create the group categories
		ppk_settings_group *ppk_settings_security, *ppk_settings_network;
		
		ppk_settings_security=ppk_settings_group_create("Security", "Security related parameters");
		ppk_settings_network=ppk_settings_group_create("Network", "Network related parameters");
		
		params_group["Security"]=ppk_settings_security;
		params_group["Network"]=ppk_settings_network;
		
		
		//Create the parameters' prototypes
		ppk_proto_param *mod_passphrase, *close_dly;
		
		mod_passphrase=ppk_param_proto_create_module(PARAM_MOD_PASSPHRASE,
											"The ppk module you would like the passphrase to be got from.",
											PARAM_MOD_PASSPHRASE_DEFAULT,
											ppk_settings_security,
											PPK_FALSE,	//Allow self
											PPK_FALSE, //writable only
											ppk_sec_lowest,
											ppk_rf_none,
											ppk_wf_none,
											ppk_lf_none);
		proto_params[PARAM_MOD_PASSPHRASE]=mod_passphrase;
		
		close_dly=ppk_param_proto_create_ranged_int(PARAM_CLOSING_DELAY,
											"Closing delay when the module is not used. Set it to 0 if you would like it not to be closed.",
											PARAM_CLOSING_DELAY_DEFAULT,
											ppk_settings_security,
											0,
											99);
		proto_params[PARAM_CLOSING_DELAY]=close_dly;
		
		//Get a list of available parameters
		availParams=new ppk_proto_param*[proto_params.size()+1];
		int i=0;
		std::map<std::string, ppk_proto_param*>::const_iterator itr;
		for(itr = proto_params.begin(); itr != proto_params.end(); ++itr, ++i)
			availParams[i]=itr->second;
		availParams[proto_params.size()]=NULL;
		
		//Set parameters's default value
		parameters[PARAM_MOD_PASSPHRASE]=cvariant_from_string(PARAM_MOD_PASSPHRASE_DEFAULT);
 		parameters[PARAM_CLOSING_DELAY]=cvariant_from_int(PARAM_CLOSING_DELAY_DEFAULT);
	}

	void destructor()
	{
		//Free the list of parameter
		delete[] availParams;
		
		//Free the param prototypes
		std::map<std::string, ppk_proto_param*>::const_iterator itr;
		for(itr = proto_params.begin(); itr != proto_params.end(); ++itr)
			ppk_param_proto_free(itr->second);
		
		//Free the setting groups
		std::map<std::string, ppk_settings_group*>::const_iterator itr2;
		for(itr2 = params_group.begin(); itr2 != params_group.end(); ++itr2)
			ppk_settings_group_free(itr2->second);
	}
	
	const char* getModuleID()
	{
		return "PPKSafeLocker";
	}

	const char* getModuleName()
	{
		return "PPK SafeLocker - PPassKeeper's Safe Locker";
	}

	const int getABIVersion()
	{
		return 1;
	}
	
	ppk_security_level securityLevel(const char* module_id)
	{
		return ppk_sec_safe;
	}
	
	ppk_boolean isWritable()
	{
		return PPK_TRUE;
	}

	//Get available flags
	unsigned int readFlagsAvailable()
	{
		return ppk_rf_none|ppk_rf_silent;
	}

	unsigned int writeFlagsAvailable()
	{
		return ppk_wf_none|ppk_wf_silent;
	}
	
	unsigned int listingFlagsAvailable()
	{
		return ppk_lf_none|ppk_lf_silent;
	}

	ppk_error getSimpleEntryList(char*** list, unsigned int flags)
	{
		if(list==NULL)
			return PPK_INVALID_ARGUMENTS;
		
		(*list)=NULL;
			
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return ret;
		
		QList<QString> entries=safeLock().list();
		
		if(entries.size()>0)
		{
			//Copy to a char** list
			(*list)=new char*[entries.size()+1];
			if((*list)!=NULL)
			{
				for(int i=0; i<entries.size(); i++)
				{
					QString entry=entries.at(i);

					(*list)[i]=new char[entry.size()];
					strcpy((*list)[i], qPrintable(entry));
				}
				(*list)[entries.size()]=NULL;
			}
		}
		
		return PPK_OK;
	}

	//Get and Set passwords
	#include "ppasskeeper/data.h"
	ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return ret;
		
		const SFEntry sfentry=safeLock().get(entry);
		if(sfentry==SFEntry())
			return PPK_ENTRY_UNAVAILABLE;
		
		*edata=sfentry.ppkData_new();
		
		return PPK_OK;
	}

	ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return ret;
		
		if(!safeLock().reset(SFEntry(entry, edata)))
			return PPK_ENTRY_UNAVAILABLE;
		else
			return PPK_OK;
	}
	
	ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return ret;
		
		if(safeLock().remove(entry)==false)
			return PPK_ENTRY_UNAVAILABLE;
		else
			return PPK_OK;
	}

	ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return PPK_FALSE;
		
		if(safeLock().get(entry)==SFEntry())
			return PPK_FALSE;
		else
			return PPK_TRUE;
	}
	
	unsigned int maxDataSize(ppk_data_type type)
	{
		switch(type)
		{
			case ppk_string:
				return (unsigned int)-1;
			case ppk_blob:
				return (unsigned int)-1;
		}
	
		return 0;
	}
	
	const ppk_proto_param** availableParameters()
	{
		return const_cast<const ppk_proto_param**>(availParams);
	}

	void setParam(const char* paramName, const cvariant value)
	{
		std::string key(paramName);

		if(key == PARAM_MOD_PASSPHRASE)
		{
			if(cvariant_get_type(value)==cvariant_string)
				parameters[PARAM_MOD_PASSPHRASE]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_CLOSING_DELAY)
		{
			if(cvariant_get_type(value)==cvariant_int)
				parameters[PARAM_CLOSING_DELAY]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else
			printf("%s: Unknown param(%s) !!\n", getModuleID(), paramName);
	}
}
