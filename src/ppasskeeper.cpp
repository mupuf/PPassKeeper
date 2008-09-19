#include "ppasskeeper.h"
#include "ppasskeeper-module.h"
#include "ppk_modules.h"

//global variables
PPK_Modules modules;


//Public functions
unsigned int ppk::getAvailableModulesCount()
{
	return modules.size();
}

unsigned int ppk::getAvailableModules(PPassKeeper_Module* pmodules, unsigned int nbModules)
{
	return modules.getModulesList(pmodules,nbModules);
}

ppk::boolean moduleAvailable(const char* module_id)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return ppk::BTRUE;
	else
		return ppk::BFALSE;
}

ppk::readFlag readFlagsAvailable(const char* module_id)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->readFlagsAvailable();
	else
		return ppk::rd_none;
}

ppk::writeFlag writeFlagsAvailable(const char* module_id)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->writeFlagsAvailable();
	else
		return ppk::wt_none;
}

//Non-silent operations
const char* ppk::getNetworkPassword(const char* module_id, const char* server, int port, const char* username, unsigned int flags)
{
	static std::string pwd;
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
	{
		const char* res=mod->getNetworkPassword(server,port,username, flags);
		if(res!=NULL)
			pwd=res;
		else
			pwd="";
		return pwd.c_str();
	}
	else
		return NULL;
}

ppk::boolean ppk::setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd, unsigned int flags)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setNetworkPassword(server,port,username,pwd,flags);
	else
		return BFALSE;
}

const char* ppk::getApplicationPassword(const char* module_id, const char* application_name, const char* username, unsigned int flags)
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

ppk::boolean ppk::setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd, unsigned int flags)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setApplicationPassword(application_name,username,pwd,flags);
	else
		return BFALSE;
}

const char* ppk::getItem(const char* module_id, const char* key, unsigned int flags)
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

ppk::boolean ppk::setItem(const char* module_id, const char* key, const char* item, unsigned int flags)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setItem(key, item, flags);
	else
		return ppk::BFALSE;
}

//Information about the module
ppk::boolean ppk::isWritable(const char* module_id)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->isWritable();
	else
		return ppk::BFALSE;
}

ppk::security_level ppk::securityLevel(const char* module_id)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->securityLevel();
	else
		return ppk::sec_lowest;
}

//module's passwords's listing
unsigned int ppk::getPasswordListCount(const char* module_id, enum password_type type)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getPasswordListCount();
	else
		return 0;
}

unsigned int ppk::getPasswordList(const char* module_id, ppk::password_type type, void* pwdList, unsigned int maxPasswordCount)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getPasswordList(type, pwdList, maxPasswordCount);
	else
		return 0;
}

//Errors
const char* ppk::getLastError(const char* module_id)
{
	const _module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getLastError();
	else
		return NULL;
}
