#include <ppasskeeper/ppasskeeper-key.h>

#include <cstring>
#include <sstream>
#include <cstdlib>

static size_t digits(unsigned int number)
{
	int digits = 0, step = 1;
	while (step <= number)
	{
		digits++;
		step *= 10;
	}
	return digits ? digits : 1;
}

size_t ppk_keyLength(const ppk_entry* entry)
{
	size_t len = 0;
	switch(entry->type)
	{
	case ppk_network:
		if (entry->net.protocol && entry->net.protocol[0] != '\0')
			len = strlen(entry->net.protocol) + sizeof("://");
		else
			len = sizeof(URL_PREFIX "://");
		len += strlen(entry->net.login) + sizeof("@")
		     + strlen(entry->net.host);
		if (entry->net.port != 0)
			len += sizeof(":") + digits(entry->net.port);
		break;
	case ppk_application:
		len = strlen(entry->app.username) + sizeof("@") + strlen(entry->app.app_name);
		break;
	case ppk_item:
		len = strlen(entry->item);
	default:
		return 0;
	}
	return len + 1; // +1 for '\0'
}

#include <iostream>
#include <stdio.h>
ppk_boolean ppk_getKey(const ppk_entry* entry, char* returned_key, size_t max_key_length)
{
	if (max_key_length == 0)
		return PPK_FALSE;

	std::ostringstream key;
	switch(entry->type)
	{
	case ppk_network:
		std::cout << "before" << std::endl;
		if (entry->net.protocol!=NULL && entry->net.protocol[0] != '\0')
			key << entry->net.protocol << "://";
		key << entry->net.login << '@' << entry->net.host << ':' << entry->net.port;
		std::cout << "key = " << key.str() << std::endl;
		break;
	case ppk_application:
		key << entry->app.username << '@' << entry->app.app_name;
		break;
	case ppk_item:
		key << entry->item;
	}

	std::string key_str = key.str();
	std::size_t key_len = key_str.size();
	if (key_len >= max_key_length)
		//not enough room in returned_key for returning
		return PPK_FALSE;
	else
	{
		key_str.copy(returned_key, key_len);
		returned_key[key_len]='\0';
		return PPK_TRUE;
	}
}

ppk_boolean ppk_getEntryFromKey(ppk_entry* entry, const char* key)
{
	static std::string login, host, protocol, username, app_name, item;

	std::string key_str(key);
	std::size_t key_len = key_str.size();
	std::size_t pos = key_str.find("://");
	if (pos != std::string::npos)
	{
		if (pos == 0 || pos + 6 > key_len) return PPK_FALSE;
		entry->type = ppk_network;
		protocol = key_str.substr(0, pos);
		entry->net.protocol = (protocol == URL_PREFIX) ? NULL : protocol.c_str();

		std::size_t loginpos = pos + 3;
		std::size_t hostpos = key_str.find('@', loginpos + 1);
		if (hostpos == std::string::npos || hostpos == key_len - 1) return PPK_FALSE;
		else
		{
			hostpos++;
			std::size_t portpos = key_str.find(':', hostpos + 1);
			if (portpos == std::string::npos)
			{
				//port unspecified, use 0
				entry->net.port = 0;
				host = key_str.substr(hostpos).c_str();
				entry->net.host = host.c_str();
			}
			else
			{
				portpos++;
				// ppk://aaaaaaaaa@bbbbbbbb:111
				//       ^loginpos ^hostpos ^portpos
				//       6         17       26
				if (portpos == key_len) return PPK_FALSE;
				unsigned int port = atoi(key_str.substr(portpos).c_str());
				if (port == 0) return PPK_FALSE;
				entry->net.port = port;
				host = key_str.substr(hostpos, portpos - hostpos - 1);
				entry->net.host = host.c_str();
			}
		}
	}
	else
	{
		pos = key_str.find('@');
		if (pos == std::string::npos)
		{
			//no @, it's an item
			entry->type = ppk_item;
			item = key_str;
			entry->item = key_str.c_str();
		}
		else
		{
			//it's an application key
			if (pos == key_len - 1) return PPK_FALSE;
			entry->type = ppk_application;
			username = key_str.substr(0, pos);
			app_name = key_str.substr(pos + 1);
			entry->app.username = username.c_str();
			entry->app.app_name = app_name.c_str();
		}
	}
	return PPK_TRUE;
}
