#include <ppasskeeper.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>

#include "../error.h"

#define TEST_STRING "je suis mupuf"

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

void usage(int argc, char** argv)
{
	if(argc>=1)
		std::cout << "usage : '" << argv[0] << " module_name'" << std::endl;
	else
		std::cout << "usage : 'utstblob module_name'" << std::endl;
}

int main(int argc, char** argv)
{
	if(argc!=2)
	{
		usage(argc, argv);
		exit(-1);
	}
	const char* module=argv[1];

	
	if(ppk_is_locked()==PPK_TRUE)
		die(UTST_LIB_LOCKED);

	if(ppk_module_is_available(module)==PPK_FALSE)
		die(UTST_INVALID_MODULE_NAME);

	size_t max_size=ppk_module_max_data_size(module, ppk_string);
	if(max_size==0)
		die(UTST_NO_STRING_SUPPORT);

	ppk_entry* entry=ppk_item_entry_new("utststring_write_string");
	ppk_data* data=ppk_string_data_new(TEST_STRING);

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

	if(edata->type==ppk_string)
	{
		std::string res=edata->string;

		ppk_data_free(edata);

		if(res==TEST_STRING)
			die(UTST_OK);
		else
		{
			std::cout << "Before : size = " << strlen(TEST_STRING) << " and data = " << TEST_STRING << std::endl;
			std::cout << "After  : size = " << res.size() << " and data = " << res << std::endl;
			die(UTST_INVALID_DATA);
		}
	}
	else
	{
		ppk_data_free(data);
		die(UTST_INVALID_ENTRY_TYPE);
	}
	
	return 0;
}
