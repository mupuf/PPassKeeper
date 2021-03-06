/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "ppasskeeper.h"
#include "ppasskeeper/entry_p.h"

#include "ppk_modules.h"
#include "vparam.h"
#include "import-export.h"

#include <stdlib.h>
#include <stdio.h>

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
	
	//Private
	static size_t returnEntryListCount(char** list, unsigned int entry_types)
	{
		size_t count = 0;
		if(list!=NULL)
		{
			int i=0;
			while(list[i]!=NULL)
			{
				ppk_entry* entry=ppk_entry_new_from_key(list[i]);
				if(entry!=NULL)
				{
					ppk_entry_type type=entry->type;

					if ((entry_types&ppk_network && type==ppk_network) ||
						(entry_types&ppk_application && type==ppk_application) ||
						(entry_types&ppk_item && type==ppk_item))
						count++;
				}

				free(list[i]);
				i++;
			}

			free(list);
		}

		return count;
	}

	static ppk_error returnEntryList(char** list, unsigned int entry_types, ppk_entry*** entryList, size_t* p_count)
	{
		std::vector<ppk_entry*> entries;
		size_t count = 0;
		
		//List the entries and put it into the entries's vector
		if(list != NULL)
		{
			//Parse the whole list
			for (int i=0; list[i]!=NULL; i++)
			{
				ppk_entry* entry=ppk_entry_new_from_key(list[i]);
				if(entry!=NULL)
				{
					ppk_entry_type type=entry->type;

					if ((entry_types&ppk_network && type==ppk_network) ||
						(entry_types&ppk_application && type==ppk_application) ||
						(entry_types&ppk_item && type==ppk_item))
					{
						entries.push_back(entry);
						count++;
					}

					//Free the element
					free(list[i]);
				}
			}
			
			//Free the list
			free(list);
		}

		//Copy entries to entryList
		*p_count = count;
		*entryList = new ppk_entry*[count+1];
		(*entryList)[count]=NULL;
		for (size_t i = 0; i < entries.size(); ++i)
			(*entryList)[i] = entries[i];
		
		return PPK_OK;
	}
	// End Private

	size_t ppk_module_get_entry_count(const char* module_id, int entry_types, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
			{
				if(mod->getEntryListCount!=NULL)
					return mod->getEntryListCount(entry_types, flags);
				else if(mod->getSimpleEntryList!=NULL)
				{
					char** list;
					/*ppk_error ret=*/mod->getSimpleEntryList(&list, flags);
					return returnEntryListCount(list, entry_types);
				}
				else
					return 0;
			}
			else
				return 0;
		}
		else
			return 0;
	}

	ppk_error ppk_module_get_entry_list(const char* module_id, int entry_types, ppk_entry*** entryList, size_t* nbEntries, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
			{
				if(mod->getEntryList!=NULL)
					return mod->getEntryList(entry_types, entryList, nbEntries, flags);
				else if(mod->getSimpleEntryList!=NULL)
				{
					//get the list of entries
					char** list;
					ppk_error ret=mod->getSimpleEntryList(&list, flags);
					
					//If it worked, fill up entryList. Otherwise, set it to NULL
					if(ret==PPK_OK)
						returnEntryList(list, entry_types, entryList, nbEntries);
					else
						*entryList=NULL;
					
					return ret;
				}
				else
					return PPK_UNSUPPORTED_METHOD;
			}
			else
				return PPK_MODULE_UNAVAILABLE;
		}
		else
			return PPK_LOCKED_NO_ACCESS;
	}

	void ppk_module_free_entry_list(ppk_entry** entry_list)
	{
		if(entry_list==NULL)
			return;
		
		int i=0;
		while(entry_list[i]!=NULL)
		{
			ppk_entry_free_contents(entry_list[i]);
			i++;
		}
		delete[] entry_list;
	}

	ppk_error ppk_module_get_entry(const char* module_id, const ppk_entry* entry, ppk_data** edata, unsigned int flags)
	{
		if(!ppk_is_locked())
		{
			if(entry==NULL || edata==NULL)
				return PPK_INVALID_ARGUMENTS;
				
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
			if(entry==NULL || edata==NULL)
				return PPK_INVALID_ARGUMENTS;
				
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
			//Validate the parameter if it is not for the library itself (ppk)
			if(strcmp(module_id, LIBPPK_MODULE_NAME)!=0)
			{
				if(ppk_param_validation(module_id, key, value)==PPK_FALSE)
					return PPK_CANNOT_VALIDATE_PARAM;
			}

			const _module* mod=modules.getModuleByID(module_id);
			if(mod==NULL)
				return PPK_MODULE_UNAVAILABLE;
			
			//Save the param & set the new param to the module
			VParam& param = VParam::instance();
			if(param.saveParam(module_id, key, value))
			{
				//set the new param to the module
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

	size_t ppk_module_list_params(const char* module_id, char*** list)
	{
		if(!ppk_is_locked())
		{
			//Get the parameters
			VParam& param = VParam::instance();
			static std::vector<std::string> vlist;
			vlist = param.listParams(module_id);
			
			//Create a list
			char** l=*list;
			l=new char*[vlist.size()+1];
			
			//Copy values to the list
			size_t i;
			for(i=0;i<vlist.size();i++)
			{
				l[i]=new char[vlist[i].size()];
				strcpy(l[i], vlist[i].c_str());
			}
			
			//End the list
			l[vlist.size()+1]=NULL;

			return i;
		}
		else
			return 0;
	}

	void ppk_module_free_params_list(char** list)
	{
		if(list==NULL)
			return;
		
		//Read the list and delete elements after elements
		int i=0;
		char* l;
		do
		{
			l=list[i];
			if(l!=NULL)
				delete[] l;
		}while(l!=NULL);
		
		//Remove the list
		delete list;
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
					return NULL;
			else
				return NULL;
		}
		else
			return NULL;
	}
	
	const ppk_proto_param* ppk_module_parameter_prototype(const char* module_id, const char* name)
	{
		if(!ppk_is_locked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
			{
				if(mod->availableParameters!=NULL)
				{
					//Get the prototype list
					const ppk_proto_param** list=mod->availableParameters();
					
					//Return the prototype corresponding to the parameter 'name'
					int i=0;
					while(list!=NULL && list[i]!=NULL)
					{
						if(strcmp(list[i]->name, name)==0)
							return list[i];
						i++;
					}
					
					return NULL;
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		else
			return NULL;
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
	
	ppk_error ppk_module_reset_params(const char* module_id)
	{
		ppk_error globalRes=PPK_OK;
		
		//Get all the parameters
		char** list;
		size_t nbParameters=ppk_module_list_params(module_id, &list);
		
		//Remove all the parameters
		for(size_t i=0;i<nbParameters; i++)
		{
			ppk_error res=ppk_module_remove_param(module_id, list[i]);
			if(res!=PPK_OK && globalRes==PPK_OK)
				globalRes=res;
		}
		
		return globalRes;
	} 
	
	ppk_error ppk_module_empty(const char* module_id)
	{
		ppk_error globalRes=PPK_OK;

		ppk_entry **entryList;
		size_t nbEntries;
    
		//Get the entry list
		ppk_error res=ppk_module_get_entry_list(module_id, ppk_network|ppk_application|ppk_item, &entryList, &nbEntries, ppk_rf_none);
		if(res!=PPK_OK)
			return res;
		
		//remove every entry
		for(size_t i=0; i<nbEntries; i++)
		{
			res=ppk_module_remove_entry(module_id, entryList[i], ppk_rf_none);
			if(res!=PPK_OK && globalRes==PPK_OK)
				globalRes=res;
		}

		//Remove this list
		ppk_module_free_entry_list(entryList);

		return globalRes;
	}
	
	ppk_error ppk_module_import(const char* module_id, const char* importFile)
	{
		ImportExport impExp;
		
		return impExp.importFromFileToModule(module_id, importFile);
	}

	ppk_error ppk_module_export(const char* module_id, const char* exportFile)
	{
		return ImportExport::dumpModuleToFile(module_id, exportFile);
	}
}
