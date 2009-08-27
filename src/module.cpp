#include "ppasskeeper.h"

#include "ppk_modules.h"
#include "vparam.h"

#define LIBPPK_MODULE_NAME "libppasskeeper"
#define LIBPPK_KEY_DEFAULT_MODULE "default_module"
#define LIBPPK_AUTO_MODULE LIBPPK_DEFAULT_MODULE

//global variables
PPK_Modules modules;

extern "C"
{
	unsigned int ppk_module_count()
	{
		if(!ppk_is_locked())
			return modules.size();
		else
			return 0;
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

	char** ppk_module_list_new()
	{
		if(!ppk_is_locked())
		{
			return modules.getModulesList();
		}
		else
			return NULL;
	}

	void ppk_module_list_free(char** list)
	{
		modules.freeModuleList(list);
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
			return PPK_FALSE;
	}

	const char* ppk_module_display_name(const char* module_id)
	{
		if(!ppk_is_locked())
			return modules.getDisplayNameByID(module_id);
		else
			return NULL;
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

	ppk_error ppk_module_has_entry(const char* module_id, const ppk_entry* entry, unsigned int flags, ppk_boolean* result)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
			{
				*result = mod->entryExists(entry, flags);
				return PPK_OK;
			}
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

	ppk_error ppk_module_save_param(const char* module_id, const char* key, const cvariant value)
	{
		if(!ppk_is_locked())
		{
			VParam& param = VParam::instance();
			if(param.saveParam(module_id, key, value))
			{
				const _module* mod=modules.getModuleByID(module_id);
				if(mod!=NULL)
					mod->setParam(key, value);

				return PPK_OK;
			}
			else
				return PPK_CANNOT_SAVE_PARAM;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}

	cvariant ppk_module_get_param(const char* module_id, const char* key)
	{
		if(!ppk_is_locked())
		{
			VParam& param = VParam::instance();
			return param.getParam(module_id, key);
		}
		else
			return cvariant_null();
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
			return 0;
	}

	ppk_error ppk_module_remove_param(const char* module_id, const char* key)
	{
		if(!ppk_is_locked())
		{
			VParam& param = VParam::instance();
			return param.removeParam(module_id, key)?PPK_OK:PPK_CANNOT_REMOVE_PARAM;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
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
			return 0;
	}

	ppk_error ppk_module_set_default(const char* module_id)
	{
		if(!ppk_is_locked())
		{
			if(std::string(module_id)==LIBPPK_AUTO_MODULE || ppk_module_is_available(module_id)==PPK_TRUE)
				return ppk_module_save_param(LIBPPK_MODULE_NAME, LIBPPK_KEY_DEFAULT_MODULE, cvariant_from_string(module_id));
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

	ppk_error ppk_afp_set_custom_prompt_message(const char* module_id, const char* customMessage)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				if(mod->setCustomPromptMessage!=NULL)
					return mod->setCustomPromptMessage(customMessage)==PPK_TRUE?PPK_OK:PPK_UNKNOWN_ERROR;
				else
					return PPK_UNSUPPORTED_METHOD;
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}
}
