#include "list_pwd.h"
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

#include <kwallet.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

const char* ListPwd::prefix(ppk_entry_type type)
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
	return NULL;
}

ppk_entry* ListPwd::parseNetworkPassword(const std::string& stripped_name)
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
	if (stripped_name.size() > 0)
		return ppk_item_entry_new(stripped_name.c_str());
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

unsigned int ListPwd::getEntryListCount(KWallet::Wallet* wallet, unsigned int entry_types, unsigned int flags)
{
	//XXX: KWallet doesn't provide a way to recount the entries without relisting
	unsigned int count;
	ppk_entry** list = getEntryList(wallet, entry_types, flags, &count);
	ppk_module_free_entry_list(list);
	return count;
}

ppk_entry** ListPwd::getEntryList(KWallet::Wallet* wallet, unsigned int entry_types, unsigned int flags, unsigned int* count)
{
	QList<ppk_entry*> entries;
	QStringList list = wallet->entryList();
	//put the data in entries
	//build a new list in ppk_entry format, only the entry types we want
	for(int i=0;i<list.size();i++)
	{
		ppk_entry* entry = parseFileName(list[i].toStdString().c_str(), entry_types, flags);
		if(entry != NULL)
			entries.append(entry);
	}

	*count = entries.size();

	//allocate the array
	ppk_entry** res = new ppk_entry*[(*count) + 1];
	res[*count] = NULL;
	for (int i = 0; i < entries.size(); ++i)
		res[i] = entries[i];
	
	return res;
}
