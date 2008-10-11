#include "list_pwd.h"
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

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

bool ListPwd::parseFileName(std::string prefix, std::string filename, ppk_password_type type)
{
	//Example : filename=ENC_NET_mupuf@mupuf.fr.nf:21, prefix=ENC_NET_, type=network
	
	//Strip the prefix from file's name, so,filename = mupuf@mupuf.fr.nf:21
	std::string suffix=filename.substr(prefix.size());

	//Depending on the type
	switch(type)
	{
		case ppk_network:
		{
			return addNetworkPassword(suffix);
		}
		case ppk_application:
		{
			return addAppPassword(suffix);
		}
		case ppk_item:
		{
			return addItemPassword(suffix);
		}
	}

	return false;
}

#if defined(WIN32) || defined(WIN64)
	#include <windows.h>
	
#else
	#include <dlfcn.h>
	#include <sys/types.h>
	#include <dirent.h>
	
	unsigned int ListPwd::updateDataBase(const char* dir, const char* prefix, ppk_password_type type)
	{	
		DIR * pwddir;
		struct dirent * mydirent;
		unsigned int prefix_len=strlen(prefix);
		unsigned int pwdCount=0;
		
		//Open Plugin's directory
		pwddir = opendir(dir);
		if(pwddir!=NULL)
		{
			while ((mydirent = readdir(pwddir))!=NULL)
				if(strncmp (mydirent->d_name, prefix, prefix_len)==0)
					if(parseFileName(prefix, mydirent->d_name, type))
						pwdCount++;

			closedir(pwddir);
		}

#ifdef DEBUG_MSG
		else
			std::cerr << "Could not open password directory: " << dir << std::endl;
#endif
	}
#endif

unsigned int ListPwd::copyDBToPwdList(ppk_password_type type, void* pwdList, unsigned int maxPasswordCount)
{
	//Depending on the type
	switch(type)
	{
		case ppk_network:
		{
			return copyNetworkToPwdList(pwdList, maxPasswordCount);
		}
		case ppk_application:
		{
			return copyApplicationToPwdList(pwdList, maxPasswordCount);
		}
		case ppk_item:
		{
			return copyItemToPwdList(pwdList, maxPasswordCount);
		}
	}
}
unsigned int ListPwd::copyNetworkToPwdList(void* pwdList, unsigned int maxPasswordCount)
{
	PPassKeeper_module_entry_net* list=(PPassKeeper_module_entry_net*) pwdList;

	int i;
	for(i=0;i<listNet.size() && i<maxPasswordCount;i++)
	{
		list[i].host=listNet[i].host.c_str();
		list[i].login=listNet[i].user.c_str();
		list[i].port=listNet[i].port;
	}

	return listNet.size();
}
unsigned int ListPwd::copyApplicationToPwdList(void* pwdList, unsigned int maxPasswordCount)
{
	PPassKeeper_module_entry_app* list=(PPassKeeper_module_entry_app*) pwdList;

	for(int i=0;i<listApp.size() && i<maxPasswordCount;i++)
	{
		list[i].app_name=listApp[i].app.c_str();
		list[i].username=listApp[i].user.c_str();
	}

	return listApp.size();
}

unsigned int ListPwd::copyItemToPwdList(void* pwdList, unsigned int maxPasswordCount)
{
	PPassKeeper_module_entry_item* list=(PPassKeeper_module_entry_item*) pwdList;

	for(int i=0;i<listItem.size() && i<maxPasswordCount;i++)
	{
		list[i].key=listItem[i].key.c_str();
	}

	return listItem.size();
}

unsigned int ListPwd::getPasswordListCount(const char* dir, const char* prefix, ppk_password_type type)
{
	//Update the database and return how many password were found
	return updateDataBase(dir, prefix, type);
}

unsigned int ListPwd::getPasswordList(const char* dir, const char* prefix, ppk_password_type type, void* pwdList, unsigned int maxPasswordCount)
{
	//Update the database before putting data into pwdList
	updateDataBase(dir, prefix, type);

	std::cout << "Il y a " << (int)listNet.size() << " passwords !" << std::endl;

	//Put data into pwdList
	return copyDBToPwdList(type, pwdList, maxPasswordCount);
}
