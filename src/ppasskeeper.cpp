#include "ppasskeeper.h"
#include "ppasskeeper-module.h"
#include "ppk_modules.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include "vparam.h"

#define LIBPPK_MODULE_NAME "libppasskeeper"
#define LIBPPK_KEY_DEFAULT_MODULE "default_module"
#define LIBPPK_AUTO_MODULE LIBPPK_DEFAULT_MODULE

//global variables
PPK_Modules modules;

std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}

void setError(std::string error)
{
	*(last_error())="libppasskeeper : " + error;
}

/*********************************************************************
 *                                                                   *
 *                          Public Functions                         *
 *                                                                   *
 *********************************************************************/
extern "C"
{	
	unsigned int ppk_module_count()
	{
		if(!ppk_is_locked())
			return modules.size();
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
	}

	ppk_error ppk_module_reload_list()
	{
		if(!ppk_is_locked())
		{
			modules.reload();
			return PPK_OK;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}
	
	size_t ppk_module_list(ppk_module* pmodules, unsigned int nbModules)
	{
		if(!ppk_is_locked())
		{
			return modules.getModulesList(pmodules,nbModules);
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
	}

	ppk_boolean ppk_module_is_available(const char* module_id)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return PPK_TRUE;
			else
				return PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}

	ppk_error ppk_module_read_flags(const char* module_id, unsigned int* flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
			{
				if(flags!=NULL)
					*flags=mod->readFlagsAvailable();

				return PPK_OK;
			}
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}

	ppk_error ppk_module_write_flags(const char* module_id, unsigned int* flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
			{
				if(flags!=NULL)
					*flags=mod->writeFlagsAvailable();

				return PPK_OK;
			}
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}

	ppk_error ppk_module_listing_flags(const char* module_id, unsigned int* flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
			{
				if(flags!=NULL)
					*flags=mod->listingFlagsAvailable();

				return PPK_OK;
			}
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}

	ppk_error ppk_module_has_entry(const char* module_id, const ppk_entry* entry, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->entryExists(entry, flags);
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}
	
	size_t ppk_module_max_data_size(const char* module_id, ppk_data_type type)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->maxDataSize(type);
			else
				return 0;
		}
		else
			return 0;
	}
	
	size_t ppk_module_get_entry_count(const char* module_id, int entry_types, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->getEntryListCount(entry_types, flags);
			else
				return 0;
		}
		else
			return 0;
	}

	size_t ppk_module_get_entry_list(const char* module_id, int entry_types, ppk_entry* entryList, size_t nbEntries, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->getEntryList(entry_types, entryList, nbEntries, flags);
			else
				return 0;
		}
		else
			return 0;
	}
	
	ppk_error ppk_module_get_entry(const char* module_id, const ppk_entry* entry, ppk_data** edata, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->getEntry(entry, edata, flags);
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}

	ppk_error ppk_module_set_entry(const char *module_id, const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->setEntry(entry, edata, flags);
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}
	
	ppk_error ppk_module_remove_entry(const char* module_id, const ppk_entry* entry, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->removeEntry(entry, flags);
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}

	//Information about the module
	ppk_boolean ppk_module_is_writable(const char* module_id)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->isWritable();
			else
				return PPK_FALSE;
		}
		else
			return PPK_FALSE;
	}

	ppk_security_level ppk_module_security_level(const char* module_id)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->securityLevel();
			else
				return ppk_sec_lowest;
		}
		else
			return ppk_sec_lowest;
	}
	
	ppk_boolean ppk_module_save_param(const char* module_id, const char* key, const cvariant value)
	{
		if(!ppk_is_locked())
		{
			VParam& param = VParam::instance();
			if(param.saveParam(module_id, key, value))
			{
				const _module* mod=modules.getModuleByID(module_id);
				if(mod!=NULL)
					mod->setParam(key, value);
				
				return PPK_TRUE;
			}
			else
				return PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}

	cvariant ppk_module_get_param(const char* module_id, const char* key)
	{
		if(!ppk_is_locked())
		{
			VParam& param = VParam::instance();
			return param.getParam(module_id, key);
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return cvariant_null();
		}
	}
	
	size_t ppk_module_list_params(const char* module_id, const char*** list, size_t maxEntries)
	{
		if(!ppk_is_locked())
		{
			VParam& param = VParam::instance();
			static std::vector<std::string> vlist;
			vlist = param.listParams(module_id);
			
			int i;
			for(i=0;i<maxEntries && i<vlist.size();i++)
				(*list)[i]=vlist[i].c_str();
			
			return i;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
	}
	
	ppk_boolean ppk_module_remove_param(const char* module_id, const char* key)
	{
		if(!ppk_is_locked())
		{
			VParam& param = VParam::instance();
			return param.removeParam(module_id, key)?PPK_TRUE:PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}
	
	const ppk_proto_param** ppk_module_available_parameters(const char* module_id)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				if(mod->availableParameters!=NULL)
					return mod->availableParameters();
				else
					return 0;
			else
				return 0;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
	}

	ppk_error ppk_module_set_default(const char* module_id)
	{
		if(!ppk_is_locked())
		{
			if(std::string(module_id)==LIBPPK_AUTO_MODULE || ppk_module_is_available(module_id)==PPK_TRUE)
			{
				ppk_boolean res=ppk_module_save_param(LIBPPK_MODULE_NAME, LIBPPK_KEY_DEFAULT_MODULE, cvariant_from_string(module_id));
				return res==PPK_TRUE?PPK_OK:PPK_CANNOT_SAVE_PARAM;
			}
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}
	
	const char* ppk_module_get_default()
	{
		//static char buf[PPK_PARAM_MAX];
		cvariant cv=ppk_module_get_param(LIBPPK_MODULE_NAME, LIBPPK_KEY_DEFAULT_MODULE);
		if(cvariant_not_null(cv))
		{
			if(cvariant_get_type(cv)==cvariant_string)
			{
				const char* buf=cvariant_get_string(cv);
				if(ppk_module_is_available(buf)==PPK_TRUE)
					return buf;
				else
					return LIBPPK_DEFAULT_MODULE;
			}
			else
				return LIBPPK_DEFAULT_MODULE;
		}
		else
			return LIBPPK_DEFAULT_MODULE;
	}

	//Errors
	const char* ppk_getLastError(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->getLastError();
		else
			return last_error()->c_str();
	}

	/****************************************************************************************************/
	/****************************************************************************************************/
	/*                                                                                                  */
	/*                                              OPTIONAL                                            */
	/*                                                                                                  */
	/****************************************************************************************************/
	/****************************************************************************************************/
	
	ppk_boolean ppk_afp_set_custom_prompt_message(const char* module_id, const char* customMessage)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				if(mod->setCustomPromptMessage!=NULL)
					return mod->setCustomPromptMessage(customMessage);
				else
					return PPK_FALSE;
			else
				return PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}
}
