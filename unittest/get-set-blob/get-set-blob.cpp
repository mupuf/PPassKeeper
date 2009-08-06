#include <ppasskeeper.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>

#include "../error.h"

extern "C" void die(unsigned int error_num, const char* error)
{
	if(error_num>0)
	{
		std::cerr << "Fatal Error : " << utst_msg[error_num];
		if(error)
			std::cerr << std::endl << "	Error was : '" << error << "'";
	}
	else if(error_num==0)
		std::cerr << utst_msg[error_num];
	else
		std::cerr << "Unknown Error Code!";

	std::cerr << std::endl;
	exit(error_num);
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

void usage(int argc, char** argv)
{
	if(argc>=1)
		std::cout << "usage : '" << argv[0] << " module_name <app|net|item>'" << std::endl;
	else
		std::cout << "usage : 'utstblob module_name <app|net|item>'" << std::endl;
}

int main(int argc, char** argv)
{
	ppk_entry* entry;
	if(argc!=3)
	{
		usage(argc, argv);
		exit(-1);
	}

	std::string type=argv[2];
	if(type=="app")
		entry=ppk_application_entry_new("test","utstblob_write_string");
	else if(type=="net")
		entry=ppk_network_entry_new(NULL, "test", "utstblob_write_string", 99);
	else if(type=="item")
		entry=ppk_item_entry_new("utstblob_write_string");
	else
	{
		usage(argc, argv);
		exit(-1);
	}
	
	const char* module=argv[1];
	
	if(ppk_is_locked()==PPK_TRUE)
		die(UTST_LIB_LOCKED);
	
	std::string file=readFile(argv[0]);
	if(file!=std::string())
	{
		if(ppk_module_is_available(module)==PPK_FALSE)
			die(UTST_INVALID_MODULE_NAME);
		
		size_t max_size=ppk_module_max_data_size(module, ppk_blob);
		if(max_size==0)
			die(UTST_NO_BLOB_SUPPORT);
		else if(max_size<file.size())
			file=file.substr(0, max_size);

		ppk_data* data=ppk_blob_data_new(file.data(), file.size());

		printf("Write: ");
		ppk_error res=ppk_module_set_entry(module, entry, data, ppk_wf_none);
		if(res!=PPK_OK)
		{
			printf("NOK\n");
			die(UTST_WRITING_ERROR, ppk_error_get_string(res));
		}
		else
			printf("OK\n");

		ppk_data_free(data);

		printf("Read: ");
		ppk_data* edata;
		res=ppk_module_get_entry(module, entry, &edata, ppk_rf_none);
		if(res!=PPK_OK)
		{
			printf("NOK\n");
			die(UTST_READING_ERROR, ppk_error_get_string(res));
		}
		else
			printf("OK\n");

		printf("Delete: ");
		res=ppk_module_remove_entry(module, entry, ppk_rf_none);
		if(res!=PPK_OK)
		{
			printf("NOK\n");
			die(UTST_WRITING_ERROR, ppk_error_get_string(res));
		}
		else
			printf("OK\n");

		ppk_entry_free(entry);
			
		if(edata->type==ppk_blob)
		{
			std::string res;
			res.assign((const char*)edata->blob.data, edata->blob.size);

			ppk_data_free(edata);
			
			if(res==file)
				die(UTST_OK);
			else
			{
				std::cout << "Before : size = " << file.size() << " and data = " << (const char*)file.data() << std::endl;
				std::cout << "After  : size = " << edata->blob.size << " and data = " << (const char*)edata->blob.data << std::endl;
				die(UTST_INVALID_DATA);
			}
		}
		else
		{
			ppk_data_free(data);
			die(UTST_INVALID_ENTRY_TYPE);
		}
	}
	else
		die(UTST_CANNOT_OPEN_FILE, ("Cannot read file '" + std::string(argv[0]) + "'").c_str());
	
	return 0;
}
