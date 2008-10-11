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

	unsigned int ppk_getAvailableModules(PPassKeeper_Module* pmodules, unsigned int nbModules)
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

	ppk_readFlag ppk_readFlagsAvailable(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->readFlagsAvailable();
		else
			return ppk_rf_none;
	}

	ppk_writeFlag ppk_writeFlagsAvailable(const char* module_id)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->writeFlagsAvailable();
		else
			return ppk_wf_none;
	}

	//Non-silent operations
	const char* ppk_getNetworkPassword(const char* module_id, const char* server, int port, const char* username, unsigned int flags)
	{
		static std::string pwd;
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
		{
			const char* res=mod->getNetworkPassword(server,port,username, flags);
			if(res!=NULL)
			{
				pwd=res;
				return pwd.c_str();
			}
			else
				return NULL;
		}
		else
			return NULL;
	}

	ppk_boolean ppk_setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->setNetworkPassword(server,port,username,pwd,flags);
		else
			return PPK_FALSE;
	}

	const char* ppk_getApplicationPassword(const char* module_id, const char* application_name, const char* username, unsigned int flags)
	{
		static std::string pwd;
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
		{
			const char* res=mod->getApplicationPassword(application_name, username,flags);
			if(res!=NULL)
			{
				pwd=res;
				return pwd.c_str();
			}
			else
				return NULL;
		}
		else
			return NULL;
	}

	ppk_boolean ppk_setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->setApplicationPassword(application_name,username,pwd,flags);
		else
			return PPK_FALSE;
	}

	const char* ppk_getItem(const char* module_id, const char* key, unsigned int flags)
	{
		static std::string pwd;
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
		{
			const char* res=mod->getItem(key,flags);
			if(res!=NULL)
			{
				pwd=res;
				return pwd.c_str();
			}
			else
				return NULL;
		}
		else
			return NULL;
	}

	ppk_boolean ppk_setItem(const char* module_id, const char* key, const char* item, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->setItem(key, item, flags);
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

	//module's passwords's listing
	unsigned int ppk_getPasswordListCount(const char* module_id, enum ppk_password_type type, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->getPasswordListCount(type, flags);
		else
			return 0;
	}

	unsigned int ppk_getPasswordList(const char* module_id, ppk_password_type type, void* pwdList, unsigned int maxPasswordCount, unsigned int flags)
	{
		const _module* mod=modules.getModuleByID(module_id);
		if(mod!=NULL)
			return mod->getPasswordList(type, pwdList, maxPasswordCount, flags);
		else
			return 0;
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
	/*																									*/
	/*											OPTIONNAL !												*/
	/*																									*/
	/****************************************************************************************************/
	/****************************************************************************************************/
	
	enum ppk_boolean ppk_setCustomPromptMessage(const char* module_id, const char* customMessage)
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
}
