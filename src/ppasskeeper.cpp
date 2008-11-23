#include "ppasskeeper.h"
#include "ppasskeeper-module.h"
#include "ppk_modules.h"
#include <iostream>

//global variables
PPK_Modules modules;

//Public functions
extern "C"
{
	unsigned int ppk_getAvailableModulesCount()
	{
		return modules.size();
	}

	unsigned int ppk_getAvailableModules(ppk_module* pmodules, unsigned int nbModules)
	{
		return modules.getModulesList(pmodules,nbModules);
	}

	ppk_boolean ppk_moduleAvailable(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return PPK_TRUE;
		else
			return PPK_FALSE;
	}

	unsigned int ppk_readFlagsAvailable(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->readFlagsAvailable();
		else
			return ppk_rf_none;
	}

	unsigned int ppk_writeFlagsAvailable(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->writeFlagsAvailable();
		else
			return ppk_wf_none;
	}

	unsigned int ppk_listingFlagsAvailable(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->listingFlagsAvailable();
		else
			return ppk_lf_none;
	}

	ppk_boolean ppk_entryExists(const char* module_id, const ppk_entry entry, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->entryExists(entry, flags);
		else
			return PPK_FALSE;
	}
	
	unsigned int ppk_maxDataSize(const char* module_id, ppk_data_type type)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->maxDataSize(type);
		else
			return 0;
	}
	
	unsigned int ppk_getEntryListCount(const char* module_id, unsigned int entry_types, unsigned int flags)
	{
     	const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->getEntryListCount(entry_types, flags);
		else
			return 0;
     }
     
     unsigned int ppk_getEntryList(const char* module_id, unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
     {
     	const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->getEntryList(entry_types, entryList, nbEntries, flags);
		else
			return 0;
     }
	
	ppk_boolean ppk_getEntry(const char *module_id, const ppk_entry entry, ppk_data *edata, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->getEntry(entry, edata, flags);
		else
			return PPK_FALSE;
	}

	ppk_boolean ppk_setEntry(const char *module_id, const ppk_entry entry, const ppk_data edata, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->setEntry(entry, edata, flags);
		else
			return PPK_FALSE;
	}
	
	ppk_boolean ppk_removeEntry(const char* module_id, const ppk_entry entry, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->removeEntry(entry, flags);
		else
			return PPK_FALSE;
	}

	//Information about the module
	ppk_boolean ppk_isWritable(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->isWritable();
		else
			return PPK_FALSE;
	}

	ppk_security_level ppk_securityLevel(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->securityLevel();
		else
			return ppk_sec_lowest;
	}


	//Errors
	const char* ppk_getLastError(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->getLastError();
		else
			return NULL;
	}

	/****************************************************************************************************/
	/****************************************************************************************************/
	/*                                                                                                  */
	/*                                              OPTIONAL                                            */
	/*                                                                                                  */
	/****************************************************************************************************/
	/****************************************************************************************************/
	
	ppk_boolean ppk_setCustomPromptMessage(const char* module_id, const char* customMessage)
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
	
	/****************************************************************************************************/
	/****************************************************************************************************/
	/*                                                                                                  */
	/*                                       Convenient functions                                       */
	/*                                                                                                  */
	/****************************************************************************************************/
	/****************************************************************************************************/
	ppk_entry ppk_createNetworkEntry(const char* host, const char* login, unsigned int port)
	{
		ppk_entry entry;
		entry.type=ppk_network;
		entry.net.host=host;
		entry.net.port=port;
		entry.net.login=login;
		return entry;
	}
	
     ppk_entry ppk_createAppEntry(const char* app_name, const char* username)
     {
		ppk_entry entry;
		entry.type=ppk_application;
		entry.app.app_name=app_name;
		entry.app.username=username;
		return entry;
	}
	
     ppk_entry ppk_createItemEntry(const char* item)
     {
		ppk_entry entry;
		entry.type=ppk_item;
		entry.item=item;
		return entry;
	}
	
	ppk_data ppk_createStringData(const char* string)
	{
		ppk_data data;
		data.type=ppk_string;
		data.string=string;
		return data;
	}
	
	ppk_data ppk_createBlobData(void* data, unsigned long size)
	{
		ppk_data edata;
		edata.type=ppk_blob;
		edata.blob.data=data;
		edata.blob.size=size;
		return edata;
	}
}
