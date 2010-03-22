#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

#include "safelock.h"

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

//Private functions
SafeLock& safeLock()
{
	static SafeLock sf(QString::fromUtf8(ppk_settings_directory())+QString::fromUtf8("/ppk_safelock.crypted"));
	return sf;
}

ppk_error lazyInit()
{
	if(!safeLock().isOpen())
	{
		if(!safeLock().open("passphrase"))
		{
			return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
		}
	}
	
	return PPK_OK;
}

//functions
extern "C"
{
	const char* getModuleID()
	{
		return "PPKSafeLocker";
	}

	const char* getModuleName()
	{
		return "PPassKeeper's Safe Locker";
	}

	const int getABIVersion()
	{
		return 1;
	}
	
	ppk_security_level securityLevel(const char* module_id)
	{
		return ppk_sec_safe;
	}
	
	ppk_boolean isWritable()
	{
		return PPK_TRUE;
	}

	//Get available flags
	unsigned int readFlagsAvailable()
	{
		return ppk_rf_none|ppk_rf_silent;
	}

	unsigned int writeFlagsAvailable()
	{
		return ppk_wf_none|ppk_wf_silent;
	}
	
	unsigned int listingFlagsAvailable()
	{
		return ppk_lf_none|ppk_lf_silent;
	}

	char** getSimpleEntryList(unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return NULL;
		
		QList<QString> entries=safeLock().list();
		
		if(entries.size()==0)
			return NULL;
		else
		{
			//Copy to a char** list
			char** ret=new char*[entries.size()+1];
			if(ret!=NULL)
			{
				for(int i=0; i<entries.size(); i++)
				{
					QString entry=entries.at(i);

					ret[i]=new char[entry.size()];
					strcpy(ret[i], qPrintable(entry));
				}
				ret[entries.size()]=NULL;
			}

			return ret;
		}
	}

	//Get and Set passwords
	#include "ppasskeeper/data.h"
	ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return ret;
		
		const SFEntry sfentry=safeLock().get(entry);
		if(sfentry==SFEntry())
			return PPK_ENTRY_UNAVAILABLE;
		
		*edata=sfentry.ppkData_new();
		
		return PPK_OK;
	}

	ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return ret;
		
		if(!safeLock().reset(SFEntry(entry, edata)))
			return PPK_ENTRY_UNAVAILABLE;
		else
			return PPK_OK;
	}
	
	ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return ret;
		
		if(safeLock().remove(entry)==false)
			return PPK_ENTRY_UNAVAILABLE;
		else
			return PPK_OK;
	}

	ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
	{
		ppk_error ret=lazyInit();
		if(ret!=PPK_OK)
			return PPK_FALSE;
		
		if(safeLock().get(entry)==SFEntry())
			return PPK_FALSE;
		else
			return PPK_TRUE;
	}
	
	unsigned int maxDataSize(ppk_data_type type)
	{
		switch(type)
		{
			case ppk_string:
				return (unsigned int)-1;
			case ppk_blob:
				return (unsigned int)-1;
		}
	
		return 0;
	}
}
