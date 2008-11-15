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
		long res=RegQueryValueEx(hk, key, 0, 0, (BYTE*)tmpBuf, &size);
		RegCloseKey(hk);
		
		if(res==ERROR_SUCCESS)
		{
			pwd=tmpBuf;
			return pwd.c_str();
		}
		else
			return NULL;
	}
	else
		return NULL;
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

bool removePassword(const char* key)
{
	HKEY hk;
	if(!RegCreateKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hk, 0))
	{
		long res=RegDeleteValue(hk, key);
		RegCloseKey(hk);
		
		return res==ERROR_SUCCESS;
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
	return ppk_rf_none;
}

extern "C" ppk_writeFlag writeFlagsAvailable()
{
	return ppk_wf_none;
}

extern "C" ppk_listingFlag listingFlagsAvailable()
{
	return ppk_lf_none;
}

//List passwords available
std::string prefix(const ppk_entry entry)
{
	std::string prefix="ppasskeeper_";
	switch(entry.type)
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

extern "C" unsigned int getEntryListCount(unsigned int entry_types, unsigned int flags)
{
	ListPwd pwdl;		
	return pwdl.getEntryListCount(baseKey, entry_types, flags);
}

extern "C" unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
{	
	static ListPwd pwdl;	
	return pwdl.getEntryList(baseKey, entry_types, entryList, nbEntries, flags);
}

//Functions 
extern "C" ppk_boolean getEntry(const ppk_entry entry, ppk_data *edata, unsigned int flags)
{
	static std::string pwd;

	std::string text;
	if(entry.type==ppk_network)
		text=generateNetworkKey(entry.net.host, entry.net.port, entry.net.login);
	else if(entry.type==ppk_application)
		text=generateApplicationKey(entry.app.app_name, entry.app.username);
	else if(entry.type==ppk_item)
		text=generateItemKey(entry.item);
	
	const char* res=getPassword(text.c_str());

	//if everything went fine
	if(res!=NULL)
	{
		setError("");
		pwd=res;
		edata->string=pwd.c_str();
		return PPK_TRUE;
	}
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean setEntry(const ppk_entry entry, const ppk_data *edata, unsigned int flags)
{
	static std::string pwd;
	
	std::string text;
	if(entry.type==ppk_network)
		text=generateNetworkKey(entry.net.host, entry.net.port, entry.net.login);
	else if(entry.type==ppk_application)
		text=generateApplicationKey(entry.app.app_name, entry.app.username);
	else if(entry.type==ppk_item)
		text=generateItemKey(entry.item);

	//if everything went fine
	if(setPassword(text.c_str(), edata->string))
	{
		setError("");
		return PPK_TRUE;
	}
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean removeEntry(const ppk_entry entry, unsigned int flags)
{
	std::string text;
	if(entry.type==ppk_network)
		text=generateNetworkKey(entry.net.host, entry.net.port, entry.net.login);
	else if(entry.type==ppk_application)
		text=generateApplicationKey(entry.app.app_name, entry.app.username);
	else if(entry.type==ppk_item)
		text=generateItemKey(entry.item);
		
	return removePassword(text.c_str())?PPK_TRUE:PPK_FALSE;
}

extern "C" ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
{
	ppk_data edata;
	return getEntry(entry, &edata, flags);
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}
