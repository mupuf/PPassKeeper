#include "list_pwd.h"
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

//List passwords available
std::string shortName(); //Declared in submodules encrypted and plain_text
std::string ListPwd::prefix(ppk_entry_type type)
{
	std::string prefix=shortName();
	switch(type)
	{
		case ppk_network:
			prefix+="_NET_";
			break;
		case ppk_application:
			prefix+="_APP_";
			break;
		case ppk_item:
			prefix+="_ITM_";
			break;
	}
	return prefix;
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

#if defined(WIN32) || defined(WIN64)
	#include <windows.h>
	
	void ListPwd::appendEntries(const char* dir, unsigned int entry_types, unsigned int flags, std::vector<ppk_entry*>& list)
	{	
		WIN32_FIND_DATA File;
		HANDLE hSearch;
		unsigned int pwdCount=0;

		std::string path;
		char* tmp=getenv("HOMEDRIVE");
		if(tmp!=NULL)
		{
			path=tmp;
			tmp=getenv("HOMEPATH");
			
			if(tmp!=NULL)
			{
				path+=tmp;
				
				hSearch = FindFirstFile((path+"\\ppasskeeper\\*").c_str(), &File);
				if (hSearch != INVALID_HANDLE_VALUE)
				{
					do {
						ppk_entry* entry = parseFileName(File.cFileName, entry_types, flags);
						if (entry != NULL)
							list.push_back(entry);
					} while (FindNextFile(hSearch, &File));
					
					FindClose(hSearch);
				}
			}
		}
		
#ifdef DEBUG_MSG
		std::cerr << "Could not open pwd directory" << std::endl;
#endif
	}
#else
	#include <dlfcn.h>
	#include <sys/types.h>
	#include <dirent.h>
	
	void ListPwd::appendEntries(const char* dir, unsigned int entry_types, unsigned int flags, std::vector<ppk_entry*>& list)
	{
		DIR * pwddir;
		struct dirent * mydirent;
		unsigned int pwdCount=0;

		//Open Plugin's directory
		pwddir = opendir(dir);
		if(pwddir!=NULL)
		{
			while ((mydirent = readdir(pwddir))!=NULL)
			{
				ppk_entry* entry = parseFileName(mydirent->d_name, entry_types, flags);
				if (entry != NULL)
					list.push_back(entry);
			}

			closedir(pwddir);
		}

#ifdef DEBUG_MSG
		else
			std::cerr << "Could not open password directory: " << dir << std::endl;
#endif
	}
#endif

unsigned int ListPwd::getEntryListCount(const char* dir, unsigned int entry_types, unsigned int flags)
{
	size_t count;
	ppk_entry** entries = getEntryList(dir, entry_types, &count, flags);
	ppk_module_free_entry_list(entries);
	return count;
}

ppk_entry** ListPwd::getEntryList(const char* dir, unsigned int entry_types, size_t* count, unsigned int flags)
{
	std::vector<ppk_entry*> entries;
	
	//Update the database before putting data into pwdList
	appendEntries(dir, entry_types, flags, entries);

	*count = entries.size();

	//allocate the array
	ppk_entry** res = new ppk_entry*[(*count) + 1];
	res[*count] = NULL;
	for (int i = 0; i < entries.size(); ++i)
		res[i] = entries[i];
	
	return res;
}
