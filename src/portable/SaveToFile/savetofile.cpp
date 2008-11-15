#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include "list_pwd.h"

//local functions
std::string shortName();
const char* readFile(std::string filename, unsigned int flags);
bool writeFile(std::string filename, std::string secret, unsigned int flags);

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

bool deletePassword(std::string path, unsigned int flags)
{
	return remove(path.c_str())==0;
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
		return PPK_FALSE;
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
		return pwdl.getEntryListCount(dir().c_str(), entry_types, flags);
	}

	unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
	{
		static ListPwd pwdl;	
		return pwdl.getEntryList(dir().c_str(), entry_types, entryList, nbEntries, flags);
	}

	//Get and Set passwords
	ppk_boolean getEntry(const ppk_entry entry, ppk_data *edata, unsigned int flags)
	{
		static std::string pwd;	

		const char* res=readFile(getKey(entry), flags);
		if(res!=NULL)
		{
			pwd=res;
			edata->type=ppk_string;
			edata->string=pwd.c_str();
			return PPK_TRUE;
		}
		else
			return PPK_FALSE;
	}

	ppk_boolean setEntry(const ppk_entry entry, const ppk_data edata, unsigned int flags)
	{
		return writeFile(getKey(entry).c_str(), edata.string, flags)?PPK_TRUE:PPK_FALSE;
	}
	
	ppk_boolean removeEntry(const ppk_entry entry, unsigned int flags)
	{
		return deletePassword(getKey(entry), flags)?PPK_TRUE:PPK_FALSE;
	}

	ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
	{
		return fileExists(getKey(entry));
	}

	const char* getLastError()
	{
		return last_error()->c_str();
	}
}
