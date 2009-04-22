#include "ppasskeeper.h"
#include "ppasskeeper-module.h"
#include "ppk_modules.h"
#include "ppasskeeper-dir.h"
#include "sha512.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string.h>

//Param
#ifdef USE_ELEKTRA
	#include "elektraParam.h"
	
	ElektraParam elektraParam;
	VParam* vparam=&elektraParam;
#else
	#include "xmlParam.h"
	
	XMLParam xmlParam(ppk_settingDirectory()+std::string("/xmlParam.xml"));
	VParam* vparam=&xmlParam;
#endif
#define LIBPPK_MODULE_NAME "libppasskeeper"
#define LIBPPK_KEY_DEFAULT_MODULE "default_module"
#define LIBPPK_AUTO_MODULE "Automatic"
#define LIBPPK_DEFAULT_MODULE "Automatic"

//global variables
PPK_Modules modules;

//local definitions
enum State {undefined, unlocked, locked };

/*********************************************************************
 *                                                                   *
 *                          Local Functions                          *
 *                                                                   *
 *********************************************************************/

void sha512(char* hash, const void* data, unsigned int length)
{
	uint8_t	sha512[SHA512_HASH_SIZE];
	SHA512Context s;
	
	SHA512Init(&s);
	SHA512Update(&s, data, length);
	SHA512Final(&s,sha512);
	
	for(int i = 0 ; i< SHA512_HASH_SIZE; i++)
		snprintf(hash + i *2,SHA512_HASH_SIZE+7,"%02x",sha512[i]);
}

State& cState()
{
	static State state=undefined;
	return state;
}

bool grantAccess(const char* pwd="")
{
	char hash_pwd[SHA512_HASH_SIZE*2+1];
	char hash_file[SHA512_HASH_SIZE*2+1];
	
	FILE* f=fopen((setting_dir()+"/lock").c_str(), "r");
	if(f!=NULL)
	{
		fread(hash_file, sizeof(char), sizeof(hash_file),f);
		fclose(f);

		sha512(hash_pwd, pwd, strlen(pwd));
		
		if(strcmp(hash_pwd, hash_file)==0)
		{
			cState()=unlocked;
			return true;
		}
		else
		{
			cState()=locked;
			return false;
		}
	}
	else
	{
		cState()=unlocked;
		
		/*
		 * Here is the problem for people who wants this "security", 
		 * you just have to delete the file and all the security is gone ...
		 */
		return true;
	}
}

bool isLocked()
{
	if(cState() == undefined)
		cState()=grantAccess()?unlocked:locked;
		
	return cState()==locked;
}

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
 *                          Public Functions                          *
 *                                                                   *
 *********************************************************************/
extern "C"
{	
	ppk_boolean ppk_setPassword(const char* pwd)
	{
		if(!isLocked())
		{
			char hash_pwd[SHA512_HASH_SIZE*2+1];
			
			FILE* f=fopen((setting_dir()+"/lock").c_str(), "w");
			if(f!=NULL)
			{
				sha512(hash_pwd, pwd, strlen(pwd));
				fwrite(hash_pwd, sizeof(char), sizeof(hash_pwd),f);
				fclose(f);
				
				return PPK_TRUE;
			}
			else
				setError("ppk_setPassword : The lock cannot be openned. Check your file permission !");
		}
		
		return PPK_FALSE;
	}

	ppk_boolean ppk_isLocked()
	{
		return isLocked()?PPK_TRUE:PPK_FALSE;
	}
	
	ppk_boolean ppk_unlock(const char* password)
	{
		grantAccess(password);
		return !isLocked()?PPK_TRUE:PPK_FALSE;
	}

	unsigned int ppk_getAvailableModulesCount()
	{
		if(!isLocked())
			return modules.size();
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
	}

	unsigned int ppk_getAvailableModules(ppk_module* pmodules, unsigned int nbModules)
	{
		if(!isLocked())
		{
			return modules.getModulesList(pmodules,nbModules);
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
	}

	ppk_boolean ppk_moduleAvailable(const char* module_id)
	{
		if(!isLocked())
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

	unsigned int ppk_readFlagsAvailable(const char* module_id)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->readFlagsAvailable();
			else
				return ppk_rf_none;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return ppk_rf_none;
		}
	}

	unsigned int ppk_writeFlagsAvailable(const char* module_id)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->writeFlagsAvailable();
			else
				return ppk_wf_none;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return ppk_wf_none;
		}
	}

	unsigned int ppk_listingFlagsAvailable(const char* module_id)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->listingFlagsAvailable();
			else
				return ppk_lf_none;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return ppk_lf_none;
		}
	}

	ppk_boolean ppk_entryExists(const char* module_id, const ppk_entry entry, unsigned int flags)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->entryExists(entry, flags);
			else
			{
				setError("You cannot access any information. The library is locked !");
				return PPK_FALSE;
			}
		}
		else
			return PPK_FALSE;
	}
	
	unsigned int ppk_maxDataSize(const char* module_id, ppk_data_type type)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->maxDataSize(type);
			else
				return 0;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
	}
	
	unsigned int ppk_getEntryListCount(const char* module_id, unsigned int entry_types, unsigned int flags)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->getEntryListCount(entry_types, flags);
			else
				return 0;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
     }
     
     unsigned int ppk_getEntryList(const char* module_id, unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
     {
     	if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->getEntryList(entry_types, entryList, nbEntries, flags);
			else
				return 0;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return 0;
		}
     }
	
	ppk_boolean ppk_getEntry(const char *module_id, const ppk_entry entry, ppk_data *edata, unsigned int flags)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->getEntry(entry, edata, flags);
			else
				return PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}

	ppk_boolean ppk_setEntry(const char *module_id, const ppk_entry entry, const ppk_data edata, unsigned int flags)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->setEntry(entry, edata, flags);
			else
				return PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}
	
	ppk_boolean ppk_removeEntry(const char* module_id, const ppk_entry entry, unsigned int flags)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->removeEntry(entry, flags);
			else
				return PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}

	//Information about the module
	ppk_boolean ppk_isWritable(const char* module_id)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->isWritable();
			else
				return PPK_FALSE;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return PPK_FALSE;
		}
	}

	ppk_security_level ppk_securityLevel(const char* module_id)
	{
		if(!isLocked())
		{
			const _module* mod=modules.getModuleByID(module_id);
			if(mod!=NULL)
				return mod->securityLevel();
			else
				return ppk_sec_lowest;
		}
		else
		{
			setError("You cannot access any information. The library is locked !");
			return ppk_sec_lowest;
		}
	}
	
	ppk_boolean ppk_saveParam(const char* module_id, const char* key, const char* value)
	{
		if(vparam->saveParam(module_id, key, value))
			return PPK_TRUE;
		else
			return PPK_FALSE;
	}

	ppk_boolean ppk_getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize)
	{
		if(vparam->getParam(module_id, key, returnedString, maxSize))
			return PPK_TRUE;
		else
			return PPK_FALSE;
	}

	ppk_boolean ppk_setDefaultModule(const char* module_id)
	{
		if(std::string(module_id)==LIBPPK_AUTO_MODULE || ppk_moduleAvailable(module_id)==PPK_TRUE)
			return ppk_saveParam(LIBPPK_MODULE_NAME, LIBPPK_KEY_DEFAULT_MODULE, module_id);
		else
			return PPK_FALSE;
	}
	
	const char* ppk_getDefaultModule()
	{
		static char buf[PPK_PARAM_MAX];
		if(ppk_getParam(LIBPPK_MODULE_NAME, LIBPPK_KEY_DEFAULT_MODULE, buf, sizeof(buf))==PPK_TRUE)
		{
			if(ppk_moduleAvailable(buf)==PPK_TRUE)
				return buf;
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
	
	ppk_boolean ppk_setCustomPromptMessage(const char* module_id, const char* customMessage)
	{
		if(!isLocked())
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
	

/*********************************************************************
 *                                                                   *
 *                      Convenient functions                         *
 *                                                                   *
 *********************************************************************/
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
	
	ppk_data ppk_createBlobData(const void* data, unsigned long size)
	{
		ppk_data edata;
		edata.type=ppk_blob;
		edata.blob.data=data;
		edata.blob.size=size;
		return edata;
	}
	
	const char* ppk_settingDirectory()
	{
		static std::string settingDir=setting_dir();
		return settingDir.c_str();	
	}
}


