#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <windows.h>
#include "list_pwd.h"


//Local definitions
static const char* baseKey="SOFTWARE\\PPassKeeper\\";

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

bool getPassword(const char* key, ppk_data* edata)
{
	static char tmpBuf[1000000];

	HKEY hk;
	if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, KEY_QUERY_VALUE, &hk))
	{
		DWORD size=sizeof(tmpBuf);
		DWORD type;
		long res=RegQueryValueEx(hk, key, 0, &type, (BYTE*)tmpBuf, &size);
		RegCloseKey(hk);
		
		if(res==ERROR_SUCCESS)
		{
			if(type==REG_BINARY)
			{
				edata->blob.data=tmpBuf;
				edata->blob.size=size;
				return true;
			}
			else if(type==REG_SZ)
			{
				edata->string=tmpBuf;
				return true;
			}
			else
			{
				setError("Unknown data type !");
				return false;
			}
		}
		else
			return false;
	}
	else
		return false;
}

bool setPassword(const char* key, const ppk_data edata)
{
	HKEY hk;
	if(!RegCreateKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hk, 0))
	{
		long res;
		
		if(edata.type==ppk_string)
			res=RegSetValueEx(hk, key, 0, REG_SZ, (const BYTE*)edata.string, strlen(edata.string)+1);
		else if(edata.type==ppk_blob)
			res=RegSetValueEx(hk, key, 0, REG_BINARY, (const BYTE*)edata.blob.data, edata.blob.size);
		else
			setError("setPassword : Undefined data type !");
			
		RegCloseKey(hk);
		
		return res==ERROR_SUCCESS;
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
extern "C" unsigned int readFlagsAvailable()
{
	return ppk_rf_none|ppk_rf_silent;
}

extern "C" unsigned int writeFlagsAvailable()
{
	return ppk_wf_none|ppk_wf_silent;
}

extern "C" unsigned int listingFlagsAvailable()
{
	return ppk_lf_none|ppk_lf_silent;
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
	else
	{
		setError("getEntry : Invalid entry type.");
		return PPK_FALSE;
	}
	
	//if everything went fine
	if(getPassword(text.c_str(), edata))
	{
		setError("");
		return PPK_TRUE;
	}
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean setEntry(const ppk_entry entry, const ppk_data edata, unsigned int flags)
{
	static std::string pwd;
	
	std::string text;
	if(entry.type==ppk_network)
		text=generateNetworkKey(entry.net.host, entry.net.port, entry.net.login);
	else if(entry.type==ppk_application)
		text=generateApplicationKey(entry.app.app_name, entry.app.username);
	else if(entry.type==ppk_item)
		text=generateItemKey(entry.item);
	else
	{
		setError("setEntry : Invalid entry type.");
		return PPK_FALSE;
	}
	
	//if everything went fine
	if(setPassword(text.c_str(), edata))
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
	else
	{
		setError("removeEntry : Invalid entry type.");
		return PPK_FALSE;
	}
		
	return removePassword(text.c_str())?PPK_TRUE:PPK_FALSE;
}

extern "C" ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
{
	ppk_data edata;
	return getEntry(entry, &edata, flags);
}

extern "C" unsigned int maxDataSize(ppk_data_type type)
{
	switch(type)
	{
		case ppk_string:
			return 1000000;
		case ppk_blob:
			return 1000000;
	}
	
	return 0;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}
