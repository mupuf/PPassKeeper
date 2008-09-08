#include "ppasskeeper.h"
#include "ppasskeeper-module.h"
#include "ppk_modules.h"

//global variables
PPK_Modules modules;


//Public functions
int ppk::getAvailableModulesCount()
{
	return modules.size();
}

int ppk::getAvailableModules(PPassKeeper_Module* pmodules, unsigned int nbModules)
{
	return modules.getModulesList(pmodules,nbModules);
}

//Non-silent operations
const char* ppk::getNetworkPassword(const char* module_id, const char* server, int port, const char* username)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getNetworkPassword(server,port,username);
	else
		return NULL;
}

int ppk::setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setNetworkPassword(server,port,username,pwd);
	else
		return -1;
}

const char* ppk::getApplicationPassword(const char* module_id, const char* application_name, const char* username)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getApplicationPassword(application_name,username);
	else
		return NULL;
}

int ppk::setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setApplicationPassword(application_name,username,pwd);
	else
		return -1;
}

const char* ppk::getItem(const char* module_id, const char* key)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getItem(key);
	else
		return NULL;
}

int ppk::setItem(const char* module_id, const char* key, const char* item)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setItem(key, item);
	else
		return -1;
}

//silent operations
const char* ppk::getNetworkPassword_silent(const char* module_id, const char* server, int port, const char* username)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getNetworkPassword_silent(server,port,username);
	else
		return NULL;
}

int ppk::setNetworkPassword_silent(const char* module_id, const char* server, int port, const char* username,  const char* pwd)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setNetworkPassword(server,port,username,pwd);
	else
		return -1;
}

const char* ppk::getApplicationPassword_silent(const char* module_id, const char* application_name, const char* username)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getApplicationPassword_silent(application_name,username);
	else
		return NULL;
}

int ppk::setApplicationPassword_silent(const char* module_id, const char* application_name, const char* username,  const char* pwd)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setApplicationPassword_silent(application_name,username,pwd);
	else
		return -1;
}

const char* ppk::getItem_silent(const char* module_id, const char* key)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getItem_silent(key);
	else
		return NULL;
}

int ppk::setItem_silent(const char* module_id, const char* key, const char* item)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->setItem_silent(key, item);
	else
		return -1;
}

//Errors
const char* ppk::getLastError(const char* module_id)
{
	_module* mod=modules.getModuleByID(module_id);
	if(mod!=NULL)
		return mod->getLastError();
	else
		return NULL;
}
