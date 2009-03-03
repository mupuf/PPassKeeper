#include "../../ppasskeeper.h"
#include "gkey_implement.h"
#include "../../tokenizer.h"

#include <string>
#include <iostream>

extern "C" ppk_boolean isWritable()
{
	return PPK_TRUE;
}

extern "C" ppk_security_level securityLevel(const char* module_id)
{
	return ppk_sec_safe;
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

//functions
extern "C" const char* getModuleID()
{
	return "GKeyring";
}

extern "C" const char* getModuleName()
{
	return "GKeyring - Store it into Gnome's Keyring";
}

extern "C" const int getABIVersion()
{
	return 1;
}

/*extern "C" unsigned int getEntryListCount(unsigned int entry_types, unsigned int flags)
{
	if(false)
	{
	}
	else
		return 0;
}

extern "C" unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
{
	if(false)
	{
	}
	else
		return 0;
}*/

extern "C" ppk_boolean getEntry(const ppk_entry entry, ppk_data *edata, unsigned int flags)
{
	if(entry.type == ppk_network)
		return getNetworkPassword(entry.net.host, entry.net.login, entry.net.port, edata, flags)==1?PPK_TRUE:PPK_FALSE;
	else if(entry.type == ppk_application)
		return getApplicationPassword(entry.app.app_name, entry.app.username, edata, flags)==1?PPK_TRUE:PPK_FALSE;
	else if(entry.type == ppk_item)
		return getItem(entry.item, edata, flags)==1?PPK_TRUE:PPK_FALSE;
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean setEntry(const ppk_entry entry, const ppk_data edata, unsigned int flags)
{
	if(entry.type == ppk_network)
		return setNetworkPassword(entry.net.host, entry.net.login, entry.net.port, edata, flags)==1?PPK_TRUE:PPK_FALSE;
	else if(entry.type == ppk_application)
		return setApplicationPassword(entry.app.app_name, entry.app.username, edata, flags)==1?PPK_TRUE:PPK_FALSE;
	else if(entry.type == ppk_item)
		return setItem(entry.item, edata, flags)==1?PPK_TRUE:PPK_FALSE;
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean removeEntry(const ppk_entry entry, unsigned int flags)
{
	return PPK_FALSE;
}

extern "C" ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
{
	return PPK_FALSE;
}

extern "C" unsigned int maxDataSize(ppk_data_type type)
{
	switch(type)
	{
		case ppk_string:
			return -1;
		case ppk_blob:
			return 0;
	}
	
	return 0;
}

std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}

extern "C" void setError(char* error)
{
	*(last_error())= getModuleID() + toString(" : ") + error;
	std::cerr << getLastError() << std::endl;
}
