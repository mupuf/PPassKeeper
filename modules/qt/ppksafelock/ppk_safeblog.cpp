#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

#include "safelock.h"

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

//private functions
ppk_error deletePassword(std::string path, unsigned int flags)
{
	if(remove(path.c_str())==0)
		return PPK_OK;
	else
		return PPK_FILE_CANNOT_BE_ACCESSED;
}

SafeLock& safeLock()
{
	static SafeLock sf(std::string(ppk_settings_directory())+"/ppk_safelock.crypted");
	return sf;
}

ppk_error lazyInit()
{
	if(!safeLock().isOpen())
	{
		if(safeLock().open("passphrase"));
	}
	
	return PPK_OK;
}

//functions
extern "C"
{
	const char* getModuleName()
	{
		return "PPK_SafeLock";
	}

	const int getABIVersion()
	{
		return 1;
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
		std::vector<std::string> entries=listEntries(setting_dir().c_str(), flags);
		
		if(entries.size()==0)
			return NULL;
		else
		{
			

			//Copy to a char** list
			char** ret=new char*[filtered.size()+1];
			if(ret!=NULL)
			{
				for(size_t i=0; i<filtered.size(); i++)
				{
					std::string val=filtered.at(i);

					ret[i]=new char[val.size()];
					strcpy(ret[i], val.c_str());
				}
				ret[filtered.size()]=NULL;
			}

			return ret;
		}
	}

	//Get and Set passwords
	#include "ppasskeeper/data.h"
	ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
	{
		std::string pwd;
		ppk_error res_read=readFile(getKey(entry), pwd, flags);
		if(res_read==PPK_OK)
		{
			if(pwd.substr(0,strlen(STR_STRING))==STR_STRING)
				*edata=ppk_string_data_new(pwd.c_str()+strlen(STR_STRING));
			else if(pwd.substr(0,strlen(BLOB_STRING))==BLOB_STRING)
				*edata=ppk_blob_data_new(pwd.data()+strlen(BLOB_STRING), pwd.size()-strlen(BLOB_STRING));
			else
				return PPK_UNKNOWN_ENTRY_TYPE;
			
			return PPK_OK;
		}
		else
			return res_read;
	}

	ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
	{
		std::string data;
		if (edata->type==ppk_blob)
		{
			if(edata->blob.data!=NULL)
				data.assign((const char *) edata->blob.data, edata->blob.size);
			else
				data="";
			data=BLOB_STRING+data;
		}
		else if(edata->type==ppk_string)
		{
			if(edata->string!=NULL)
				data.assign(edata->string);
			else
				data = "";
			data=STR_STRING+data;
		}
		else
			return PPK_UNKNOWN_ENTRY_TYPE;
		
		return writeFile(getKey(entry).c_str(), data, flags);
	}
	
	ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
	{
		return deletePassword(getKey(entry), flags);
	}

	ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
	{
		return fileExists(getKey(entry));
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
