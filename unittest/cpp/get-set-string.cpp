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

void delete_all()
{
	delete app_entry;
	delete net_entry;
	delete item_entry;
	delete data;
}

void run(int argc, char** argv)
{
	char *module_id = argv[1];

	app_entry=ppk_application_entry_new("test","utstblob_write_string");
	net_entry=ppk_network_entry_new(NULL, "test", "utstblob_write_string", 99);
	item_entry=ppk_item_entry_new("utstblob_write_string");
	entries.push_back(app_entry);
	entries.push_back(net_entry);
	entries.push_back(item_entry);

	FAILIF(ppk_is_locked()==PPK_TRUE);
	FAILIF(ppk_module_is_available(module_id)==PPK_FALSE);

	size_t max_size=ppk_module_max_data_size(module_id, ppk_string);
	if(max_size==0)
	{
		std::cerr << "* No string support in " << module_id << std::endl;
		delete_all();
		return;
	}

	data=ppk_string_data_new(TEST_STRING);

	for (int i = 0; i < entries.size(); ++i)
	{
		ppk_entry* entry = entries[i];

		/* Write */
		ppk_error res=ppk_module_set_entry(module_id, entry, data, ppk_wf_none);
		ASSERT(res == PPK_OK);

		/* Read */
		ppk_data* edata;
		res=ppk_module_get_entry(module_id, entry, &edata, ppk_rf_none);
		ASSERT(res == PPK_OK);
		if (res != PPK_OK)
			continue;

		/* Delete */
		res=ppk_module_remove_entry(module_id, entry, ppk_rf_none);
		ASSERT(res == PPK_OK);

		bool valid_type = edata->type == ppk_string;
		ASSERT(valid_type);
		if (valid_type)
		{
			std::string res;
			res = edata->string;
			ppk_data_free(edata);

			bool same_data = res == TEST_STRING;
			ASSERT(same_data);
		}
	}
	delete_all();
}
