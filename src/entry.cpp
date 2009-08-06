#include "ppasskeeper/entry.h"
#include "ppasskeeper/key.h"
#include "ppasskeeper/definitions.h"

#include <stdlib.h>
#include <assert.h>

#include <cstring>
#include <string>

extern "C"
{
	ppk_entry* ppk_network_entry_new(const char* protocol, const char* login, const char* host, unsigned short port)
	{
		ppk_entry *entry = new ppk_entry;
		size_t len;
		entry->type=ppk_network;
		len = strlen(host);
		entry->net.host = new char[len + 1];
		strcpy((char*)entry->net.host, host);
		entry->net.port=port;
		len = strlen(login);
		entry->net.login = new char[len + 1];
		strcpy((char*)entry->net.login, login);
		if (protocol)
		{
			len = strlen(protocol);
			entry->net.protocol = new char[len + 1];
			strcpy((char*)entry->net.protocol, protocol);
		}
		else
			entry->net.protocol = NULL;
		return entry;
	}

	ppk_entry* ppk_application_entry_new(const char* username, const char* app_name)
	{
		ppk_entry *entry = new ppk_entry;
		size_t len;
		entry->type=ppk_application;
		len = strlen(app_name);
		entry->app.app_name = new char[len + 1];
		strcpy((char*)entry->app.app_name, app_name);
		len = strlen(username);
		entry->app.username = new char[len + 1];
		strcpy((char*)entry->app.username, username);
		return entry;
	}

	ppk_entry* ppk_item_entry_new(const char* item)
	{
		ppk_entry *entry = new ppk_entry;
		size_t len;
		entry->type=ppk_item;
		len = strlen(item);
		entry->item = new char[len + 1];
		strcpy((char*)entry->item, item);
		return entry;
	}

	void ppk_entry_free(ppk_entry* entry)
	{
		if (entry == NULL)
			return;
		switch (entry->type)
		{
		case ppk_item:
			delete[] entry->item;
			break;
		case ppk_application:
			delete[] entry->app.app_name;
			delete[] entry->app.username;
			break;
		case ppk_network:
			delete[] entry->net.host;
			delete[] entry->net.login;
			delete[] entry->net.protocol;
			break;
		default:
			assert(false);
		};
		delete entry;
	}
}
