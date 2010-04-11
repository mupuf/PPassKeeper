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

#include "ppasskeeper/entry_p.h"
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
		ppk_network_entry_fill(entry, protocol, login, host, port);
		return entry;
	}

	ppk_entry* ppk_application_entry_new(const char* username, const char* app_name)
	{
		ppk_entry *entry = new ppk_entry;
		ppk_application_entry_fill(entry, username, app_name);
		return entry;
	}

	ppk_entry* ppk_item_entry_new(const char* item)
	{
		ppk_entry *entry = new ppk_entry;
		ppk_item_entry_fill(entry, item);
		return entry;
	}
	
	ppk_entry_type ppk_get_entry_type(const ppk_entry* entry)
	{
		return entry->type;
	}

	void ppk_entry_free(ppk_entry* entry)
	{
		if(entry!=NULL)
		{
			ppk_entry_free_contents(entry);
			delete entry;
		}
	}

	void ppk_network_entry_fill(ppk_entry* entry, const char* protocol, const char* login, const char* host, unsigned short port)
	{
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
	}

	void ppk_application_entry_fill(ppk_entry* entry, const char* username, const char* app_name)
	{
		size_t len;
		entry->type=ppk_application;
		len = strlen(app_name);
		entry->app.app_name = new char[len + 1];
		strcpy((char*)entry->app.app_name, app_name);
		len = strlen(username);
		entry->app.username = new char[len + 1];
		strcpy((char*)entry->app.username, username);
	}

	void ppk_item_entry_fill(ppk_entry* entry, const char* item)
	{
		size_t len;
		entry->type=ppk_item;
		len = strlen(item);
		entry->item = new char[len + 1];
		strcpy((char*)entry->item, item);
	}

	void ppk_entry_free_contents(ppk_entry* entry)
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
	}
}
