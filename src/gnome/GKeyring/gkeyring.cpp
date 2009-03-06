#include "../../ppasskeeper.h"
#include "gkey_implement.h"
#include "../../tokenizer.h"

#include <string>
#include <iostream>

//private functions prototypes
bool matchNetworkPassword(std::string name, std::string& user, std::string& host, unsigned short& port);
bool matchAppPassword(std::string name, std::string& user, std::string& app);
bool matchItemPassword(const std::string name, std::string& item);

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

extern "C" unsigned int getEntryListCount(unsigned int entry_types, unsigned int flags)
{
	unsigned int count=0;
	
	//Get the list
	char** list=getItemList();
	
	//
	std::string host, user, app, itm;
	unsigned short port;
	
	int i=0;
	while(list[i]!=NULL)
	{
		if(entry_types&ppk_network && matchNetworkPassword(list[i], user, host, port))
			count++;
		else if(entry_types&ppk_application && matchAppPassword(list[i], user, app))
			count++;
		else if(entry_types&ppk_item && matchItemPassword(list[i], itm))
			count++;
		
		free(list[i]);
		i++;
	}
	
	free(list);	
	
	return count;
}

//declarations
struct networkList{std::string host; std::string user; unsigned short int port;};
struct appList{std::string app; std::string user;};
struct itemList{std::string key;};

extern "C" unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
{
	int count=0;
	static std::vector<networkList> listNet;
	static std::vector<appList> listApp;
	static std::vector<itemList> listItem;
	
	//Clear needed buffers
	if((entry_types&ppk_network)>0)
		listNet.clear();
	if((entry_types&ppk_application)>0)
		listApp.clear();
	if((entry_types&ppk_item)>0)	
		listItem.clear();
	
	//Get the list
	char** list=getItemList();
	
	//Parse the whole list
	int i=0;
	while(list[i]!=NULL && count < nbEntries)
	{
		networkList net;
		appList app;
		itemList itm;
		
		if(entry_types&ppk_network && matchNetworkPassword(list[i], net.user, net.host, net.port))
		{
			listNet.push_back(net);
			
			entryList[count].type=ppk_network;
			entryList[count].net.host=listNet.back().host.c_str();
			entryList[count].net.login=listNet.back().user.c_str();
			entryList[count].net.port=listNet.back().port;

			count++;
		}
		else if(entry_types&ppk_application && matchAppPassword(list[i], app.user, app.app))
		{
			listApp.push_back(app);
			
			entryList[count].type=ppk_application;
			entryList[count].app.app_name=listApp.back().app.c_str();
			entryList[count].app.username=listApp.back().user.c_str();
			count++;
		}
		else if(entry_types&ppk_item && matchItemPassword(list[i], itm.key))
		{
			listItem.push_back(itm);
			
			entryList[count].type=ppk_item;
			entryList[count].item=listItem.back().key.c_str();
			count++;
		}
		
		free(list[i]);
		i++;
	}
	free(list);	
	
	return count;
}

extern "C" ppk_boolean getEntry(const ppk_entry entry, ppk_data *edata, unsigned int flags)
{
	if(entry.type == ppk_network)
		return getNetworkPassword(entry.net.host, entry.net.login, entry.net.port, edata, flags);
	else if(entry.type == ppk_application)
		return getApplicationPassword(entry.app.app_name, entry.app.username, edata, flags);
	else if(entry.type == ppk_item)
		return getItem(entry.item, edata, flags);
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean setEntry(const ppk_entry entry, const ppk_data edata, unsigned int flags)
{
	if(entry.type == ppk_network)
		return setNetworkPassword(entry.net.host, entry.net.login, entry.net.port, edata, flags);
	else if(entry.type == ppk_application)
		return setApplicationPassword(entry.app.app_name, entry.app.username, edata, flags);
	else if(entry.type == ppk_item)
		return setItem(entry.item, edata, flags);
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean removeEntry(const ppk_entry entry, unsigned int flags)
{
	if(entry.type == ppk_network)
		return removeNetworkPassword(entry.net.host, entry.net.login, entry.net.port, flags);
	else if(entry.type == ppk_application)
		return removeApplicationPassword(entry.app.app_name, entry.app.username, flags);
	else if(entry.type == ppk_item)
		return removeItem(entry.item, flags);
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
{
	ppk_data edata;
	
	if(entry.type == ppk_network)
		return getNetworkPassword(entry.net.host, entry.net.login, entry.net.port, &edata, flags);
	else if(entry.type == ppk_application)
		return getApplicationPassword(entry.app.app_name, entry.app.username, &edata, flags);
	else if(entry.type == ppk_item)
		return getItem(entry.item, &edata, flags);
	else
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

extern "C" void setError(const std::string error)
{
	*(last_error())= getModuleID() + toString(" : ") + error;
	std::cerr << getLastError() << std::endl;
}


//Private functions
bool matchNetworkPassword(const std::string name, std::string& user, std::string& host, unsigned short& port)
{
	if(name.substr(0, 6) == "net://")
	{
		std::string s_name = name.substr(6);
		
		//Parse the file's name
		unsigned int pos_at=s_name.find_first_of("@");
		unsigned int pos_sc=s_name.find(":",pos_at+1);

		//if it has found the separators
		if(pos_at!=std::string::npos && pos_sc!=std::string::npos)
		{
			user=s_name.substr(0,pos_at);
			host=s_name.substr(pos_at+1,pos_sc-(pos_at+1));
			std::string s_port=s_name.substr(pos_sc+1);

			//Get the port into a number
			std::istringstream i(s_port);
			if (!(i >> port))
				return false;

			return true;
		}
	}
		
	return false;
}

bool matchAppPassword(const std::string name, std::string& user, std::string& app)
{
	if(name.substr(0, 6) == "app://")
	{
		std::string s_name = name.substr(6);
		
		//Parse the file's name
		unsigned int pos_at=s_name.find_first_of("@");

		//if it has found the separators
		if(pos_at!=std::string::npos)
		{
			user=s_name.substr(0,pos_at);
			
			if(pos_at<name.size()-1)
			{
				app=s_name.substr(pos_at+1);
				return true;
			}
		}
	}
	return false;
}

bool matchItemPassword(const std::string name, std::string& item )
{
	if(name.substr(0, 6) == "itm://")
	{
		if(name.size()>6)
		{
			item=name.substr(6);
			return true;
		}
	}
	
	return false;
}
