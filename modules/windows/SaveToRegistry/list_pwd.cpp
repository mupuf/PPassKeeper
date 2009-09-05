#include "list_pwd.h"
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

#include <windows.h>

bool ListPwd::addNetworkPassword(std::string stripped_name)
{
	//Parse the file's name
	unsigned int pos_at=stripped_name.find_first_of("@");
	unsigned int pos_sc=stripped_name.find(":",pos_at+1);

	//if it has found the separators
	if(pos_at!=std::string::npos && pos_sc!=std::string::npos)
	{
		std::string user=stripped_name.substr(0,pos_at);
		std::string host=stripped_name.substr(pos_at+1,pos_sc-(pos_at+1));
		std::string s_port=stripped_name.substr(pos_sc+1);

		//Get the port into a number
		unsigned int port;
		std::istringstream i(s_port);
		if (!(i >> port))
			return false;

		//Add the content to pwdList
		networkList tmp;
		tmp.host=host;
		tmp.user=user;
		tmp.port=port;
		listNet.push_back(tmp);

		return true;
	}
	else
		return false;
}
bool ListPwd::addAppPassword(std::string stripped_name)
{
	//Parse the file's name
	unsigned int pos_at=stripped_name.find_first_of("@");

	//if it has found the separators
	if(pos_at!=std::string::npos)
	{
		std::string user=stripped_name.substr(0,pos_at);
		std::string app=stripped_name.substr(pos_at+1);

		//Add the content to pwdList
		appList tmp;
		tmp.app=app;
		tmp.user=user;
		listApp.push_back(tmp);

		return true;
	}
	else
		return false;
}

bool ListPwd::addItemPassword(std::string stripped_name)
{
	if(	stripped_name.size()>0)
	{
		//Add the content to pwdList
		itemList tmp;
		tmp.key=stripped_name;
		listItem.push_back(tmp);

		return true;
	}
	else
		return false;
}

bool ListPwd::parseFileName(std::string filename, unsigned int entry_types, unsigned int flags)
{
	std::string prefix_net=prefix(ppk_network);
	std::string prefix_app=prefix(ppk_application);
	std::string prefix_item=prefix(ppk_item);

	if(filename.size() > prefix_net.size() && strncmp(filename.c_str(), prefix_net.c_str(), prefix_net.size())==0)
		return (entry_types|ppk_network)>0 && addNetworkPassword(filename.substr(prefix_net.size()));
	else if(filename.size() > prefix_app.size() && strncmp(filename.c_str(), prefix_app.c_str(), prefix_app.size())==0)
		return (entry_types|ppk_application)>0 && addAppPassword(filename.substr(prefix_app.size()));
	else if(filename.size() > prefix_item.size() && strncmp(filename.c_str(), prefix_item.c_str(), prefix_item.size())==0)
		return (entry_types|ppk_item)>0 && addItemPassword(filename.substr(prefix_item.size()));
	else
		return false;
}

	
unsigned int ListPwd::updateDataBase(const char* baseKey, unsigned int entry_types, unsigned int flags)
{	
	HKEY hk;
	unsigned int pwdCount=0;
	static char name[101];
	
	//Clear needed buffers
	if((entry_types&ppk_network)>0)
		listNet.clear();
	if((entry_types&ppk_application)>0)
		listApp.clear();
	if((entry_types&ppk_item)>0)	
		listItem.clear();
	
	if(!RegOpenKeyEx(HKEY_CURRENT_USER, baseKey, 0, KEY_ALL_ACCESS, &hk))
	{
		int i=0, res;
		do
		{
			DWORD nameSize=sizeof(name)-1;
			res=RegEnumValue(hk, i, name, &nameSize, 0, 0, NULL, NULL);

			if(res==ERROR_SUCCESS && parseFileName(name, entry_types, flags))
				pwdCount++;
				
			i++;
		}while(res==ERROR_SUCCESS);
	}
	
	return pwdCount;
}

unsigned int ListPwd::copyDBToPwdList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries)
{
	unsigned int len=0;
	
	if((entry_types&ppk_network)>0)
		len+=copyNetworkToPwdList(entryList+len, nbEntries-len);
	if((entry_types&ppk_application)>0)
		len+=copyApplicationToPwdList(entryList+len, nbEntries-len);
	if((entry_types&ppk_item)>0)
		len+=copyItemToPwdList(entryList+len, nbEntries-len);

	return len;
}
unsigned int ListPwd::copyNetworkToPwdList(ppk_entry *entryList, unsigned int nbEntries)
{
	int i;
	for(i=0;i<listNet.size() && i<nbEntries;i++)
	{
		entryList[i].type=ppk_network;
		entryList[i].net.host=listNet[i].host.c_str();
		entryList[i].net.login=listNet[i].user.c_str();
		entryList[i].net.port=listNet[i].port;
	}

	return i;
}
unsigned int ListPwd::copyApplicationToPwdList(ppk_entry *entryList, unsigned int nbEntries)
{
	int i;
	for(i=0;i<listApp.size() && i<nbEntries;i++)
	{
		entryList[i].type=ppk_application;
		entryList[i].app.app_name=listApp[i].app.c_str();
		entryList[i].app.username=listApp[i].user.c_str();
	}

	return i;
}

unsigned int ListPwd::copyItemToPwdList(ppk_entry *entryList, unsigned int nbEntries)
{
	int i;
	for(i=0;i<listItem.size() && i<nbEntries;i++)
	{
		entryList[i].type=ppk_item;
		entryList[i].item=listItem[i].key.c_str();
	}

	return i;
}

unsigned int ListPwd::getEntryListCount(const char* baseKey, unsigned int entry_types, unsigned int flags)
{
	//Update the database and return how many password were found
	return updateDataBase(baseKey, entry_types, flags);
}

unsigned int ListPwd::getEntryList(const char* baseKey, unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
{
	//Update the database before putting data into pwdList
	updateDataBase(baseKey, entry_types, flags);

	//Put data into pwdList
	return copyDBToPwdList(entry_types, entryList, nbEntries);
}


#include "list_pwd.h"
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

//List passwords available
std::string ListPwd::prefix(ppk_entry_type type)
{
	static const char* ppk_network_string = "ppasskeeper_network://";
	static const char* ppk_app_string = "ppasskeeper_app://";
	static const char* ppk_item_string = "ppasskeeper_item://";

	switch(type)
	{
		case ppk_network:
			return ppk_network_string;
			break;
		case ppk_application:
			return ppk_app_string;
			break;
		case ppk_item:
			return ppk_item_string;
			break;
	}
	return std::string();
}

ppk_entry* ListPwd::parseNetworkPassword(const std::string& stripped_name)
{
	//Parse the file's name
	unsigned int pos_at=stripped_name.find_first_of("@");
	unsigned int pos_sc=stripped_name.find("%",pos_at+1);

	//if it has found the separators
	if(pos_at!=std::string::npos && pos_sc!=std::string::npos)
	{
		std::string user=stripped_name.substr(0,pos_at);
		std::string host=stripped_name.substr(pos_at+1,pos_sc-(pos_at+1));
		std::string s_port=stripped_name.substr(pos_sc+1);

		//Get the port into a number
		unsigned int port;
		std::istringstream i(s_port);
		if (!(i >> port))
			return NULL;

		return ppk_network_entry_new(NULL, user.c_str(), host.c_str(), port);
	}
	else
		return NULL;
}

ppk_entry* ListPwd::parseAppPassword(const std::string& stripped_name)
{
	//Parse the file's name
	unsigned int pos_at=stripped_name.find_first_of("@");

	//if it has found the separators
	if(pos_at!=std::string::npos)
	{
		std::string user=stripped_name.substr(0,pos_at);
		std::string app=stripped_name.substr(pos_at+1);

		return ppk_application_entry_new(user.c_str(), app.c_str());
	}
	else
		return NULL;
}

ppk_entry* ListPwd::parseItemPassword(const std::string& stripped_name)
{
	if(stripped_name.size()>0)
	{
		return ppk_item_entry_new(stripped_name.c_str());
	}
	else
		return NULL;
}

ppk_entry* ListPwd::parseFileName(const std::string& filename, unsigned int entry_types, unsigned int flags)
{
	std::string prefix_net=prefix(ppk_network);
	std::string prefix_app=prefix(ppk_application);
	std::string prefix_item=prefix(ppk_item);

	if((entry_types&ppk_network)>0 && filename.size() > prefix_net.size() && strncmp(filename.c_str(), prefix_net.c_str(), prefix_net.size())==0)
		return parseNetworkPassword(filename.substr(prefix_net.size()));
	else if((entry_types&ppk_application)>0 && filename.size() > prefix_app.size() && strncmp(filename.c_str(), prefix_app.c_str(), prefix_app.size())==0)
		return parseAppPassword(filename.substr(prefix_app.size()));
	else if((entry_types&ppk_item)>0 && filename.size() > prefix_item.size() && strncmp(filename.c_str(), prefix_item.c_str(), prefix_item.size())==0)
		return parseItemPassword(filename.substr(prefix_item.size()));
	else
		return NULL;
}

void ListPwd::appendEntries(const char* basekey, unsigned int entry_types, unsigned int flags, std::vector<ppk_entry*>& list)
{	
	HKEY hk;
	unsigned int pwdCount=0;
	char name[101];
	
	if(!RegOpenKeyEx(HKEY_CURRENT_USER, baseKey, 0, KEY_ALL_ACCESS, &hk))
	{
		int i=0, res;
		do
		{
			DWORD nameSize=sizeof(name)-1;
			res=RegEnumValue(hk, i, name, &nameSize, 0, 0, NULL, NULL);
			
			if(res==ERROR_SUCCESS)
			{
				ppk_entry* entry = parseFileName(name, entry_types, flags);
				if (entry != NULL)
					list.push_back(entry);
			}
				
			i++;
		}while(res==ERROR_SUCCESS);
	}
}

unsigned int ListPwd::getEntryListCount(const char* basekey, unsigned int entry_types, unsigned int flags)
{
	size_t count;
	ppk_entry** entries = getEntryList(basekey, entry_types, &count, flags);
	ppk_module_free_entry_list(entries);
	return count;
}

ppk_entry** ListPwd::getEntryList(const char* basekey, unsigned int entry_types, size_t* count, unsigned int flags)
{
	std::vector<ppk_entry*> entries;
	
	//Update the database before putting data into pwdList
	appendEntries(basekey, entry_types, flags, entries);

	*count = entries.size();

	//allocate the array
	ppk_entry** res = new ppk_entry*[(*count) + 1];
	res[*count] = NULL;
	for (int i = 0; i < entries.size(); ++i)
		res[i] = entries[i];
	
	return res;
}

