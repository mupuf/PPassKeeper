#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <windows.h>
#include "list_pwd.h"


//Local definitions
static const char* baseKey="Software\\PPassKeeper\\";

//local functions
std::string* last_error()
{
	static std::string lerror;
	return &lerror;
}
void setError(std::string error)
{
	*(last_error())="PPK_SaveToRegistry : " + error;
}

const char* getPassword(const char* key)
{
	static std::string pwd;

	HKEY hk;
	static char tmpBuf[101];
	if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, KEY_QUERY_VALUE, &hk))
	{
		DWORD size=sizeof(tmpBuf);
		RegQueryValueEx(hk, key, 0, 0, (BYTE*)tmpBuf, &size);
		RegCloseKey(hk);
		
		pwd=tmpBuf;
		return pwd.c_str();
	}
	else
		return "";
}

bool setPassword(const char* key, const char* pwd)
{
	HKEY hk;
	if(!RegCreateKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hk, 0))
	{
		RegSetValueEx(hk, key, 0, REG_SZ, (BYTE*)pwd, strlen(pwd)+1);
		RegCloseKey(hk);
	}
	else
		return false;
}

std::string generateNetworkKey(std::string server, int port, std::string username)
{
	return "ppasskeeper_network://"+username+"@"+server+":"+toString(port);
}

std::string generateApplicationKey(std::string application_name, std::string username)
{
	return "ppasskeeper_app://"+username+"@"+application_name;
}

std::string generateItemKey(std::string key)
{
	return "ppasskeeper_item://"+key;
}

//exported functions
extern "C" const char* getModuleID()
{
	return "SaveToRegistry";
}

extern "C" const char* getModuleName()
{
	return "SaveToRegistry - Store it into the registry";
}

extern "C" const int getABIVersion()
{
	return 1;
}

extern "C" ppk_boolean isWritable()
{
	return PPK_TRUE;
}

extern "C" ppk_security_level securityLevel(const char* module_id)
{
	return ppk_sec_lowest;
}

//Get available flags
extern "C" ppk_readFlag readFlagsAvailable()
{
	return ppk_rd_none;
}

extern "C" ppk_writeFlag writeFlagsAvailable()
{
	return ppk_wt_none;
}

//List passwords available
std::string prefix(ppk_password_type type)
{
	std::string prefix="ppasskeeper_";
	switch(type)
	{
		case ppk_network:
			prefix+="network";
			break;
		case ppk_application:
			prefix+="app";
			break;
		case ppk_item:
			prefix+="item";
			break;
	}
	return prefix+"://";
}
extern "C" unsigned int getPasswordListCount(ppk_password_type type, unsigned int flags)
{
	/*ListPwd pwdl;		
	return pwdl.getPasswordListCount(dir().c_str(), prefix(type).c_str(), type);*/
return 0;
}
extern "C" unsigned int getPasswordList(ppk_password_type type, void* pwdList, unsigned int maxModuleCount, unsigned int flags)
{
	/*static ListPwd pwdl;	
	return pwdl.getPasswordList(dir().c_str(), prefix(type).c_str(), type, pwdList, maxModuleCount);*/
	return 0;
}

//Functions 
extern "C" const char* getNetworkPassword(const char* server, int port, const char* username, unsigned int flags)
{
	return getPassword(generateNetworkKey(server, port, username).c_str());
}

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd, unsigned int flags)
{
	return setPassword(generateNetworkKey(server, port, username).c_str(), pwd)?0:-1;
}

extern "C" const char* getApplicationPassword(const char* application_name, const char* username, unsigned int flags)
{
	return getPassword(generateApplicationKey(application_name, username).c_str());
}

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd, unsigned int flags)
{
	return setPassword(generateApplicationKey(application_name, username).c_str(), pwd)?0:-1;
}

extern "C" const char* getItem(const char* key, unsigned int flags)
{
	return getPassword(generateItemKey(key).c_str());
}

extern "C" int setItem(const char* key, const char* item, unsigned int flags)
{
	return setPassword(generateItemKey(key).c_str(), item)?0:-1;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}
