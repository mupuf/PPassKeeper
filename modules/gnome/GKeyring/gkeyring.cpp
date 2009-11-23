#include <ppasskeeper.h>
#include <ppasskeeper/entry_p.h>
#include "gkey_implement.h"
#include "tokenizer.h"
#include "base64.h"

#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <list>

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

//private functions prototypes
bool matchNetworkPassword(std::string name, std::string& user, std::string& host, unsigned short& port, ppk_entry** entry);
bool matchAppPassword(std::string name, std::string& user, std::string& app, ppk_entry** entry);
bool matchItemPassword(const std::string name, std::string& item, ppk_entry** entry);

std::string encrypt(const ppk_data* edata)
{
	std::string res;
	
	int size=ap_base64encode_len(edata->blob.size);
	char* buf=new char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64encode_binary(buf, (const unsigned char*)edata->blob.data, edata->blob.size);
		res.assign(buf, final_len);
		delete[] buf;
	}
	
	return res;
}

std::string decrypt(const char* data)
{
	std::string res;
	int len_data=strlen(data);
	
	int size=ap_base64decode_len((const char*)data, len_data);
	unsigned char* buf=new unsigned char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64decode_binary(buf, (const char*)data, len_data);
		res.assign((char*)buf, final_len);
		delete[] buf;
	}
	
	return res;
}

//Public functions
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

extern "C" size_t getEntryListCount(unsigned int entry_types, unsigned int flags)
{
	//Get the list
	char** list=getItemList(flags);

	size_t count = 0;
	if(list!=NULL)
	{
		//tmp
		std::string host, user, app, itm;
		unsigned short port;

		int i=0;
		while(list[i]!=NULL)
		{
			if ((entry_types&ppk_network && matchNetworkPassword(list[i], user, host, port, NULL)) ||
				(entry_types&ppk_application && matchAppPassword(list[i], user, app, NULL)) ||
				(entry_types&ppk_item && matchItemPassword(list[i], itm, NULL)))
				count++;

			free(list[i]);
			i++;
		}

		free(list);
	}

	return count;
}

extern "C" ppk_error getEntryList(unsigned int entry_types, ppk_entry*** entryList, size_t* p_count, unsigned int flags)
{
	//Get the list
	char** list = getItemList(flags);

	ppk_entry* entry;
	std::vector<ppk_entry*> entries;
	size_t count = 0;
	
	//List the entries and put it into the entries's vector
	if(list != NULL)
	{
		//Parse the whole list
		for (char** _list = list; *_list != NULL; ++_list)
		{
			std::string user, login, host, app, item;
			unsigned short port;
			
			if ((entry_types&ppk_network && matchNetworkPassword(*_list, user, host, port, &entry)) ||
				(entry_types&ppk_application && matchAppPassword(*_list, user, app, &entry)) ||
				(entry_types&ppk_item && matchItemPassword(*_list, item, &entry)))
			{
				entries.push_back(entry);
				count++;
			}
			
			//Free the element
			free(*_list);
		}
		
		//Free the list
		free(list);
	}

	//Copy entries to entryList
	*p_count = count;
	*entryList = new ppk_entry*[count+1];
	(*entryList)[count]=NULL;
	for (int i = 0; i < entries.size(); ++i)
		(*entryList)[i] = entries[i];
	
	return PPK_OK;
}

extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	ppk_error res;
	ppk_data* tmp_data;
	
	if(entry->type == ppk_network)
		res=getNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, &tmp_data, flags);
	else if(entry->type == ppk_application)
		res=getApplicationPassword(entry->app.app_name, entry->app.username, &tmp_data, flags);
	else if(entry->type == ppk_item)
		res=getItem(entry->item, &tmp_data, flags);
	else
		res=PPK_UNKNOWN_ENTRY_TYPE;
		
	if(res==PPK_OK)
	{
		if(strncmp(tmp_data->string, STR_STRING, strlen(STR_STRING))==0)
			*edata=ppk_string_data_new(tmp_data->string+strlen(STR_STRING));
		else if(strncmp(tmp_data->string, BLOB_STRING, strlen(STR_STRING))==0)
		{
			std::string ret=decrypt(tmp_data->string+strlen(BLOB_STRING));
			*edata=ppk_blob_data_new(ret.data(), ret.size());
		}
		else
		{
			ppk_data_free(*edata);
			return PPK_UNKNOWN_ENTRY_TYPE;
		}
	}
	
	return res;
}

extern "C" ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
{
	static std::string data;
	ppk_data* edata_mod;
	
	if(edata->type==ppk_blob)
	{
		data=BLOB_STRING+encrypt(edata);
		edata_mod=ppk_blob_data_new(data.c_str(), data.size());
	}
	else if(edata->type==ppk_string)
	{
		data=STR_STRING+std::string(edata->string);
		edata_mod=ppk_string_data_new(data.c_str());
	}
	else
		return PPK_UNKNOWN_ENTRY_TYPE;

	ppk_error res;
	if(entry->type == ppk_network)
		res=setNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, edata_mod, flags);
	else if(entry->type == ppk_application)
		res=setApplicationPassword(entry->app.app_name, entry->app.username, edata_mod, flags);
	else if(entry->type == ppk_item)
		res=setItem(entry->item, edata_mod, flags);

	ppk_data_free(edata_mod);

	return res;
}

extern "C" ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
{
	if(entry->type == ppk_network)
		return removeNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, flags);
	else if(entry->type == ppk_application)
		return removeApplicationPassword(entry->app.app_name, entry->app.username, flags);
	else if(entry->type == ppk_item)
		return removeItem(entry->item, flags);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
}

extern "C" ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
{
	ppk_data* edata;

	ppk_error res;
	if(entry->type == ppk_network)
		res=getNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, &edata, flags);
	else if(entry->type == ppk_application)
		res=getApplicationPassword(entry->app.app_name, entry->app.username, &edata, flags);
	else if(entry->type == ppk_item)
		res=getItem(entry->item, &edata, flags);
	else
		return PPK_FALSE;

	return res==PPK_OK?PPK_TRUE:PPK_FALSE;
}

extern "C" unsigned int maxDataSize(ppk_data_type type)
{
	switch(type)
	{
		case ppk_string:
			return 7930;
		case ppk_blob:
			return 5926;
	}
	
	return 0;
}

//Private functions
bool matchNetworkPassword(const std::string name, std::string& user, std::string& host, unsigned short& port, ppk_entry** entry)
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

			if (entry)
				*entry=ppk_network_entry_new(NULL, user.c_str(), host.c_str(), port);
			return true;
		}
	}

	return false;
}

bool matchAppPassword(const std::string name, std::string& user, std::string& app, ppk_entry** entry)
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
				if(entry)
					*entry=ppk_application_entry_new(user.c_str(), app.c_str());
				return true;
			}
		}
	}
	return false;
}

bool matchItemPassword(const std::string name, std::string& item, ppk_entry** entry)
{
	if(name.substr(0, 6) == "itm://")
	{
		if(name.size()>6)
		{
			item=name.substr(6);
			if (entry)
				*entry=ppk_item_entry_new(item.c_str());
			return true;
		}
	}
	
	return false;
}
