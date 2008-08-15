#include "ppasskeeper.h"
#include "ppasskeeper-module.h"
#include "ppk_modules.h"

//global variables
PPK_Modules modules;

//Public functions
int getAvailableModulesCount()
{
	return modules.size();
}

int getAvailableModules(PPassKeeper_Module* pmodules, unsigned int nbModules)
{
	return modules.getModulesList(pmodules,nbModules);
}

const char* getNetworkPassword(const char* module_id, const char* server, int port, const char* username)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getNetworkPassword(server,port,username);
	else
		return NULL;
}

int setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setNetworkPassword(server,port,username,pwd);
	else
		return -1;
}

const char* getApplicationPassword(const char* module_id, const char* application_name, const char* username)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getApplicationPassword(application_name,username);
	else
		return NULL;
}

int setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setApplicationPassword(application_name,username,pwd);
	else
		return -1;
}

const char* getItem(const char* module_id, const char* key)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getItem(key);
	else
		return NULL;
}

int setItem(const char* module_id, const char* key, const char* item)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setItem(key, item);
	else
		return -1;
}

//Errors
const char* getLastError(const char* module_id)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getLastError();
	else
		return NULL;
}
