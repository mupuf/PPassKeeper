#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <iostream>
#include "list_pwd.h"

//local functions
std::string shortName();
const char* getPassword(std::string filepath, unsigned int flags);
bool setPassword(std::string filepath, std::string secret, unsigned int flags);

//Personal portable functions
std::string dir();
bool mkdir(std::string path);

std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}
extern "C" const char* getModuleID();
void setError(std::string error)
{
	*(last_error())="PPK_"+toString(getModuleID())+" : " + error;
}

std::string generateNetworkPath(std::string server, int port, std::string username)
{
	return dir()+toString("/")+shortName()+"_NET_"+username+toString("@")+server+toString(":")+toString(port);
}

std::string generateApplicationPath(std::string application_name, std::string username)
{
	return dir()+toString("/")+shortName()+"_APP_"+username+toString("@")+application_name;
}

std::string generateItemPath(std::string key)
{
	return dir()+toString("/")+shortName()+"_ITM_"+key;
}

//functions
extern "C"
{
	const char* getModuleName();

	const int getABIVersion()
	{
		return 1;
	}


	ppk_boolean isWritable()
	{
		return PPK_TRUE;
	}

	//Get available flags
	ppk_readFlag readFlagsAvailable()
	{
		return ppk_rd_silent;
	}

	ppk_writeFlag writeFlagsAvailable()
	{
		return ppk_wt_silent;
	}

	//List passwords available
	std::string prefix(ppk_password_type type)
	{
		std::string prefix=shortName();
		switch(type)
		{
			case ppk_network:
				prefix+="_NET_";
				break;
			case ppk_application:
				prefix+="_APP_";
				break;
			case ppk_item:
				prefix+="_ITM_";
				break;
		}
		return prefix;
	}

	unsigned int getPasswordListCount(ppk_password_type type)
	{
		ListPwd pwdl;		
		return pwdl.getPasswordListCount(dir().c_str(), prefix(type).c_str(), type);
	}

	unsigned int getPasswordList(ppk_password_type type, void* pwdList, unsigned int maxModuleCount)
	{
		static ListPwd pwdl;	
		return pwdl.getPasswordList(dir().c_str(), prefix(type).c_str(), type, pwdList, maxModuleCount);
	}

	//Get and Set passwords
	const char* getNetworkPassword(const char* server, int port, const char* username, unsigned int flags)
	{
		static std::string pwd=getPassword(generateNetworkPath(server, port, username), flags);
		return pwd.c_str();
	}

	ppk_boolean setNetworkPassword(const char* server, int port, const char* username,  const char* pwd, unsigned int flags)
	{
		return setPassword(generateNetworkPath(server, port, username).c_str(), pwd, flags)?PPK_TRUE:PPK_FALSE;
	}

	const char* getApplicationPassword(const char* application_name, const char* username, unsigned int flags)
	{
		static std::string pwd=getPassword(generateApplicationPath(application_name, username), flags);
		return pwd.c_str();
	}

	ppk_boolean setApplicationPassword(const char* application_name, const char* username, const char* pwd, unsigned int flags)
	{
		return setPassword(generateApplicationPath(application_name, username).c_str(), pwd, flags)?PPK_TRUE:PPK_FALSE;
	}

	const char* getItem(const char* key, unsigned int flags)
	{
		static std::string pwd=getPassword(generateItemPath(key), flags);
		return pwd.c_str();
	}

	ppk_boolean setItem(const char* key, const char* item, unsigned int flags)
	{
		return setPassword(generateItemPath(key).c_str(), item, flags)?PPK_TRUE:PPK_FALSE;
	}

	const char* getLastError()
	{
		return last_error()->c_str();
	}
}
