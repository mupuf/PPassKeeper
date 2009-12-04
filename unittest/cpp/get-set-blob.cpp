/**
 * \file get-set-blob.cpp
 * \author Denis Martinez
 * \date 2009-09-06
 */

#include <ppasskeeper.h>
#include "unittest.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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

std::string readFile(std::string filepath)
{
	//open the file
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		//Read the password from the file
		std::ostringstream pwd_s;
		pwd_s << inputfile.rdbuf();

		//close the file
		inputfile.close();

		return pwd_s.str();
	}
	else
		return std::string();
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

	size_t max_size=ppk_module_max_data_size(module_id, ppk_blob);
	if(max_size==0)
	{
		std::cerr << "* No blob support in " << module_id << std::endl;
		delete_all();
		return;
	}
	
	//If the module doesn't support writing/listing
	ppk_boolean writable=ppk_module_is_writable(module_id);
	if(writable==PPK_FALSE)
	{
		std::cerr << "* The module " << module_id << " is read-only" << std::endl;
	}

	std::string file=readFile(argv[0]);
	bool read_file_ok = file != std::string();
	FAILIF(! read_file_ok);

	if (max_size<file.size())
		file=file.substr(0, max_size);

	data=ppk_blob_data_new(file.data(), file.size());

	for (int i = 0; i < entries.size(); ++i)
	{
		ppk_entry* entry = entries[i];

		/* Write */
		printf("Set entry !\n");
		ppk_error res=ppk_module_set_entry(module_id, entry, data, ppk_wf_none);
		ASSERT(writable == PPK_TRUE && res == PPK_OK);
		if(res == PPK_OK)
		{
			/* Read */
			printf("read\n");
			ppk_data* edata;
			res=ppk_module_get_entry(module_id, entry, &edata, ppk_rf_none);
			ASSERT(res == PPK_OK);
			if (res != PPK_OK)
				continue;

			/* Delete */
			printf("delete\n");
			res=ppk_module_remove_entry(module_id, entry, ppk_rf_none);
			ASSERT(writable == PPK_TRUE && res == PPK_OK);

			printf("Test la correspondance\n");
			bool valid_type = edata->type == ppk_blob;
			ASSERT(valid_type);
			if (valid_type)
			{
				std::string res;
				res.assign((const char*)edata->blob.data, edata->blob.size);
				ppk_data_free(edata);

				bool same_data = res == file;
				ASSERT(same_data);
				if(! same_data)
				{
					std::cerr << "* Before : size = " << file.size() << " and data = " << (const char*)file.data() << std::endl;
					std::cerr << "* After  : size = " << edata->blob.size << " and data = " << (const char*)edata->blob.data << std::endl;
				}
			}
		}
	}
	delete_all();
}
