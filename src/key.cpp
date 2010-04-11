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

#include <ppasskeeper/key.h>

#include <cstring>
#include <sstream>
#include <cstdlib>
#include <iostream>

#define URL_PREFIX "ppasskeeper"

static size_t digits(unsigned int number)
{
	int digits = 0;
	unsigned int step = 1;
	while (step <= number)
	{
		digits++;
		step *= 10;
	}
	return digits ? digits : 1;
}

extern "C"
{

size_t ppk_key_length(const ppk_entry* entry)
{
	size_t len = 0;
	switch(entry->type)
	{
	case ppk_network:
		if (entry->net.protocol && entry->net.protocol[0] != '\0')
			len = strlen(entry->net.protocol);

		len	+= strlen("://")
			+ strlen(entry->net.login) + strlen("@")
			+ strlen(entry->net.host) + strlen(":")
			+ digits(entry->net.port);
			
		break;
	case ppk_application:
		len = strlen(entry->app.username) + strlen("@") + strlen(entry->app.app_name);
		break;
	case ppk_item:
		len = strlen(entry->item);
		break;
	default:
		return 0;
	}
	return len + 1; // +1 for '\0'
}

ppk_boolean ppk_get_key(const ppk_entry* entry, char* returned_key, size_t max_key_length)
{
	std::string protocol="";
	
	if (max_key_length == 0)
		return PPK_FALSE;

	std::ostringstream key;
	switch(entry->type)
	{
	case ppk_network:
		if(entry->net.protocol!=NULL)
			protocol=entry->net.protocol;
		key << protocol << "://" << entry->net.login << '@' << entry->net.host << ':' << entry->net.port;
		break;
	case ppk_application:
		key << entry->app.username << '@' << entry->app.app_name;
		break;
	case ppk_item:
		key << entry->item;
		break;
	default:
		std::cerr << "ppk_get_key: Unknown entry type" << entry->type << " !" << std::endl;
		return PPK_FALSE;
	}

	std::string key_str = key.str();
	std::size_t key_len = key_str.size();
	if (key_len >= max_key_length)
	{
		//not enough room in returned_key for returning
		return PPK_FALSE;
	}
	else
	{
		key_str.copy(returned_key, key_len);
		returned_key[key_len]='\0';
		return PPK_TRUE;
	}
}

ppk_entry* ppk_entry_new_from_key(const char* key)
{
	std::string key_str(key);
	std::size_t key_len = key_str.size();
	std::size_t pos = key_str.find("://");
	if (pos != std::string::npos)
	{
		if (pos + 6 > key_len)
			return NULL;
		std::string protocol = key_str.substr(0, pos);
		std::size_t loginpos = pos + 3;
		std::size_t hostpos = key_str.find('@', loginpos + 1);
		if (hostpos == std::string::npos || hostpos == key_len - 1)
			return NULL;
		else
		{
			std::string login = key_str.substr(loginpos, hostpos - loginpos);
			hostpos++;
			std::size_t portpos = key_str.find(':', hostpos + 1);
			unsigned int port;
			std::string host;
			if (portpos == std::string::npos)
			{
				//port unspecified, use 0
				port = 0;
				host = key_str.substr(hostpos).c_str();
			}
			else
			{
				portpos++;
				// ppk://aaaaaaaaa@bbbbbbbb:111
				//       ^loginpos ^hostpos ^portpos
				//       6         17       26
				if (portpos == key_len || (port = atoi(key_str.substr(portpos).c_str())) == 0)
					return NULL;
				host = key_str.substr(hostpos, portpos - hostpos - 1);
			}
			return ppk_network_entry_new((protocol == URL_PREFIX) ? NULL : protocol.c_str(), login.c_str(), host.c_str(), port);
		}
	}
	else
	{
		pos = key_str.find('@');
		if (pos == std::string::npos)
		{
			return ppk_item_entry_new(key_str.c_str());
		}
		else
		{
			//it's an application key
			if (pos == key_len - 1)
				return NULL;
			std::string username = key_str.substr(0, pos);
			std::string app_name = key_str.substr(pos + 1);
			return ppk_application_entry_new(username.c_str(), app_name.c_str());
		}
	}
}

} /* extern "C" */
