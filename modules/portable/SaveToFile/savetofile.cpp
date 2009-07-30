#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include "list_pwd.h"

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

//local functions
std::string shortName();
std::string& readFile(std::string filename, unsigned int flags);
bool writeFile(std::string filename, std::string secret, unsigned int flags);

//Personal portable functions
std::string setting_dir();

//******************************************
//*********        Windows         *********
//******************************************
#if defined(WIN32) || defined(WIN64)
	#include <direct.h>
	std::string setting_dir()
	{
		std::string userprofile=getenv("USERPROFILE");
		std::string dir=userprofile+"/ppasskeeper/";
		return dir;
	}
#else
	#include "ppasskeeper-dir.h"
	std::string setting_dir();
#endif


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
	return setting_dir()+toString("/")+shortName()+"_NET_"+username+toString("@")+server+toString("%")+toString(port);
}

std::string generateApplicationPath(std::string application_name, std::string username)
{
	return setting_dir()+toString("/")+shortName()+"_APP_"+username+toString("@")+application_name;
}

std::string generateItemPath(std::string key)
{
	return setting_dir()+toString("/")+shortName()+"_ITM_"+key;
}

bool deletePassword(std::string path, unsigned int flags)
{
	if(remove(path.c_str())==0)
		return true;
	else
	{
		setError("The file '"+path+"' cannot removed. Check your permissions !");
		return false;
	}
}

ppk_boolean fileExists(std::string filepath)
{
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		inputfile.close();
		return PPK_TRUE;
	}
	else
	{
		setError("The file '" + filepath + "' cannot be oppened. Check your permissions !");
		return PPK_FALSE;
	}
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

	std::string getKey(const ppk_entry entry)
	{
		std::string key;
		
		switch(entry.type)
		{
			case ppk_network:
			{
				key=generateNetworkPath(entry.net.host, entry.net.port, entry.net.login);
				break;
			}
			case ppk_application:
			{
				key=generateApplicationPath(entry.app.app_name, entry.app.username);
				break;
			}
			case ppk_item:
			{
				key=generateItemPath(entry.item);
				break;
			}
		}

		return key;
	}

	unsigned int getEntryListCount(unsigned int entry_types, unsigned int flags)
	{
		ListPwd pwdl;		
		return pwdl.getEntryListCount(setting_dir().c_str(), entry_types, flags);
	}

	unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
	{
		static ListPwd pwdl;	
		return pwdl.getEntryList(setting_dir().c_str(), entry_types, entryList, nbEntries, flags);
	}

	//Get and Set passwords
	ppk_boolean getEntry(const ppk_entry entry, ppk_data *edata, unsigned int flags)
	{
		std::string pwd=readFile(getKey(entry), flags);
		if(pwd!=std::string())
		{
			if(pwd.substr(0,strlen(STR_STRING))==STR_STRING)
			{
				edata->type=ppk_string;
				edata->string=pwd.c_str()+strlen(STR_STRING);
			}
			else if(pwd.substr(0,strlen(BLOB_STRING))==BLOB_STRING)
			{
				edata->type=ppk_blob;
				edata->blob.data=pwd.data()+strlen(BLOB_STRING);
				edata->blob.size=pwd.size()-strlen(BLOB_STRING);
			}
			else
			{
				setError("Unknown entry type");
				return PPK_FALSE;
			}
			
			return PPK_TRUE;
		}
		else
		{
			setError("The entry doesn't exist");
			return PPK_FALSE;
		}
	}

	ppk_boolean setEntry(const ppk_entry entry, const ppk_data edata, unsigned int flags)
	{
		std::string data;
		if (edata.type==ppk_blob)
		{
			data.assign((const char *) edata.blob.data, edata.blob.size);
			data=BLOB_STRING+data;
		}
		else if(edata.type==ppk_string)
		{
			data.assign(edata.string);
			data=STR_STRING+data;
		}
		else
		{
			setError("Unknown entry type");
			return PPK_FALSE;
		}
		
		return writeFile(getKey(entry).c_str(), data, flags)?PPK_TRUE:PPK_FALSE;
	}
	
	ppk_boolean removeEntry(const ppk_entry entry, unsigned int flags)
	{
		return deletePassword(getKey(entry), flags)?PPK_TRUE:PPK_FALSE;
	}

	ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
	{
		return fileExists(getKey(entry));
	}
	
	extern "C" unsigned int maxDataSize(ppk_data_type type)
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

	const char* getLastError()
	{
		return last_error()->c_str();
	}
}
