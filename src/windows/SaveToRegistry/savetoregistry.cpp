#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <windows.h>


//Local definitions
static const char* baseKey="Software\\PPassKeeper\\";

//local functions
extern "C" const char* getLastError()
{
	return last_error()->c_str();
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

extern "C" const char* getNetworkPassword(const char* server, int port, const char* username)
{
	return getPassword(generateNetworkKey(server, port, username).c_str());
}

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd)
{
	return setPassword(generateNetworkKey(server, port, username).c_str(), pwd)?0:-1;
}

extern "C" const char* getApplicationPassword(const char* application_name, const char* username)
{
	return getPassword(generateApplicationKey(application_name, username).c_str());
}

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd)
{
	return setPassword(generateApplicationKey(application_name, username).c_str(), pwd)?0:-1;
}

extern "C" const char* getItem(const char* key)
{
	return getPassword(generateItemKey(key).c_str());
}

extern "C" int setItem(const char* key, const char* item)
{
	return setPassword(generateItemKey(key).c_str(), item)?0:-1;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}
