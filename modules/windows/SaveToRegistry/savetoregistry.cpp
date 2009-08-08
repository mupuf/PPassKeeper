#include "ppasskeeper.h"
#include "tokenizer.h"
#include "list_pwd.h"

#include <string>
#include <windows.h>


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

ppk_error getPassword(const char* key, ppk_data** edata)
{
	HKEY hk;
	if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, KEY_QUERY_VALUE, &hk))
	{
		DWORD size=0;
		DWORD type;
		
		//Get the size of the entry
		if(RegQueryValueEx(hk, key, 0, &type, NULL, &size)!=ERROR_SUCCESS)
			return PPK_UNKNOWN_ERROR;

		//Get the data
		char* tmpBuf=new char[size];
		long res=RegQueryValueEx(hk, key, 0, &type, (BYTE*)tmpBuf, &size);
		
		RegCloseKey(hk);
		
		if(res==ERROR_SUCCESS)
		{
			if(type==REG_BINARY)
			{
				*edata=ppk_blob_data_new(tmpBuf, size);
				delete[] tmpBuf;
				return PPK_OK;
			}
			else if(type==REG_SZ)
			{
				*edata=ppk_string_data_new(tmpBuf);
				delete[] tmpBuf;
				return PPK_OK;
			}
			else
				return PPK_UNKNOWN_ENTRY_TYPE;
		}
		else
			return PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_ENTRY_UNAVAILABLE;
}

ppk_error setPassword(const char* key, const ppk_data* edata)
{
	HKEY hk;
	if(!RegCreateKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hk, 0))
	{
		long res;
		
		if(edata->type==ppk_string)
			res=RegSetValueEx(hk, key, 0, REG_SZ, (const BYTE*)edata->string, strlen(edata->string)+1);
		else if(edata->type==ppk_blob)
			res=RegSetValueEx(hk, key, 0, REG_BINARY, (const BYTE*)edata->blob.data, edata->blob.size);
		else
			return PPK_UNKNOWN_ENTRY_TYPE;
		
		RegCloseKey(hk);
		
		return res==ERROR_SUCCESS?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_ENTRY_UNAVAILABLE;
}

ppk_error removePassword(const char* key)
{
	HKEY hk;
	if(!RegCreateKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hk, 0))
	{
		long res=RegDeleteValue(hk, key);
		RegCloseKey(hk);
		
		return res==ERROR_SUCCESS?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_ENTRY_UNAVAILABLE;
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
std::string prefix(const ppk_entry* entry)
{
	std::string prefix="ppasskeeper_";
	switch(entry->type)
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
extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	static std::string pwd;

	std::string text;
	if(entry->type==ppk_network)
		text=generateNetworkKey(entry->net.host, entry->net.port, entry->net.login);
	else if(entry->type==ppk_application)
		text=generateApplicationKey(entry->app.app_name, entry->app.username);
	else if(entry->type==ppk_item)
		text=generateItemKey(entry->item);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
	
	return getPassword(text.c_str(), edata);
}

extern "C" ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
{
	std::string text;
	if(entry->type==ppk_network)
		text=generateNetworkKey(entry->net.host, entry->net.port, entry->net.login);
	else if(entry->type==ppk_application)
		text=generateApplicationKey(entry->app.app_name, entry->app.username);
	else if(entry->type==ppk_item)
		text=generateItemKey(entry->item);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
	
	//if everything went fine
	return setPassword(text.c_str(), edata);
}

extern "C" ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
{
	std::string text;
	if(entry->type==ppk_network)
		text=generateNetworkKey(entry->net.host, entry->net.port, entry->net.login);
	else if(entry->type==ppk_application)
		text=generateApplicationKey(entry->app.app_name, entry->app.username);
	else if(entry->type==ppk_item)
		text=generateItemKey(entry->tem);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
		
	return removePassword(text.c_str());
}

extern "C" ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
{
	ppk_data* edata;
	
	ppk_boolean res=getEntry(entry, &edata, flags)==PPK_OK?PPK_TRUE:PPK_FALSE;
	if(res==PPK_TRUE)
		ppk_data_free(edata);
	
	return res;
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
