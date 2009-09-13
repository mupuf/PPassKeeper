/**
 * \file get-set-string.cpp
 * \author Denis Martinez
 * \date 2009-09-06
 */

#include <ppasskeeper.h>
#include "unittest.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define TEST_STRING "Ici mùpùf à l'élysée! Tchüss niña. العربية, and so on ;)"

ppk_entry* app_entry, * net_entry, * item_entry;
ppk_data* data;
std::vector<ppk_entry*> entries;

char* module_id;

ppk_entry **network_list, **application_list, **item_list, **whole_list;
size_t network_count, application_count, item_count, count;

void delete_all()
{
	delete app_entry;
	delete net_entry;
	delete item_entry;
	delete data;
}

void delete_lists()
{
	if (network_list) { ppk_module_free_entry_list(network_list); network_list = NULL; }
	if (application_list) { ppk_module_free_entry_list(application_list); application_list = NULL; }
	if (item_list) { ppk_module_free_entry_list(item_list); item_list = NULL; }
	if (whole_list) { ppk_module_free_entry_list(whole_list); whole_list = NULL; }
}

size_t count_for_entry_type(ppk_entry_type type)
{
	switch (type)
	{
	case ppk_network:
		return network_count;
	case ppk_application:
		return application_count;
	case ppk_item:
		return item_count;
	}
}

ppk_entry** list_for_entry_type(ppk_entry_type type)
{
	switch (type)
	{
	case ppk_network:
		return network_list;
	case ppk_application:
		return application_list;
	case ppk_item:
		return item_list;
	}
}

void list_all()
{
	network_count = 0;
	application_count = 0;
	item_count = 0;
	count = 0;

	delete_lists();

	ASSERT(ppk_module_get_entry_list(module_id, ppk_network, &network_list, &network_count, 0) == PPK_OK);
	ASSERT(ppk_module_get_entry_list(module_id, ppk_application, &application_list, &application_count, 0) == PPK_OK);
	ASSERT(ppk_module_get_entry_list(module_id, ppk_item, &item_list, &item_count, 0) == PPK_OK);
	ASSERT(ppk_module_get_entry_list(module_id, ppk_network|ppk_application|ppk_item, &whole_list, &count, 0) == PPK_OK);
	ASSERT(count == network_count + application_count + item_count);
}

void run(int argc, char** argv)
{
	module_id = argv[1];

	app_entry=ppk_application_entry_new("test","utstlisting");
	net_entry=ppk_network_entry_new(NULL, "test", "utstlisting", 99);
	item_entry=ppk_item_entry_new("utstlisting");
	entries.push_back(app_entry);
	entries.push_back(net_entry);
	entries.push_back(item_entry);

	FAILIF(ppk_is_locked()==PPK_TRUE);
	FAILIF(ppk_module_is_available(module_id)==PPK_FALSE);

	data=ppk_string_data_new(TEST_STRING);

	for (int i = 0; i < entries.size(); ++i)
	{
		ppk_entry* entry = entries[i];

		// get the list before adding entry
		list_all();
		size_t prev_count = count;
		size_t prev_count_same_type = count_for_entry_type(entry->type);

		// add entry
		ASSERT(ppk_module_set_entry(module_id, entry, data, 0) == PPK_OK);

		// get the new list, check counts
		list_all();
		size_t count_same_type = count_for_entry_type(entry->type);
		ppk_entry** list = list_for_entry_type(entry->type);
		bool ok_listing = list != NULL;
		ASSERT(ok_listing);
		ASSERT(ok_listing && prev_count + 1 == count);
		ASSERT(ok_listing && prev_count_same_type + 1 == count_same_type);

		// remove
		ASSERT(ppk_module_remove_entry(module_id, entry, 0) == PPK_OK);
	}

	delete_all();
}