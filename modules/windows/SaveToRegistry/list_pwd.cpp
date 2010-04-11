/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "list_pwd.h"
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

#include <windows.h>

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

void ListPwd::appendEntries(const char* baseKey, unsigned int entry_types, unsigned int flags, std::vector<ppk_entry*>& list)
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

