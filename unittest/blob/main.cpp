#include <ppasskeeper.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>

#include "../error.h"

void die(unsigned int error_num, const std::string error)
{
	std::cerr << "Fatal Error : " << error << std::endl;
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
	
	if(ppk_isLocked()==PPK_TRUE)
		die(UTST_LIB_LOCKED, "The library is locked, please reset the password in order to let the test run !");
	
	std::string file=readFile(argv[0]);
	if(file!=std::string())
	{
		if(ppk_moduleAvailable(module)==PPK_FALSE)
			die(UTST_INVALID_MODULE_NAME, "Invalid module name");
		
		unsigned int max_size=ppk_maxDataSize(module, ppk_blob);
		if(max_size==0)
			die(UTST_NO_BLOB_SUPPORT, utst_msg[UTST_NO_BLOB_SUPPORT]);
		else if(max_size<file.size())
			die(-2, "The maximum size admissible by the module is smaller than the testing file !");
			
		if(ppk_setEntry(module, ppk_createItemEntry("utstblob_write_blob"), ppk_createBlobData(file.data(), file.size()), ppk_wf_none)==PPK_FALSE)
			die(UTST_WRITING_ERROR, "setEntry failed :\n" + std::string(ppk_getLastError(module)));
			
		ppk_data edata;
		if(ppk_getEntry(module, ppk_createItemEntry("utstblob_write_blob"), &edata, ppk_wf_none)==PPK_FALSE)
			die(UTST_READING_ERROR, "getEntry failed :\n" + std::string(ppk_getLastError(module)));
			
		if(edata.type==ppk_blob)
		{
			std::string res;
			res.assign((const char*)edata.blob.data, edata.blob.size);
			
			if(res==file)
			{
				std::cout << "BRAVO : Valid Module !" << std::endl;
				return UTST_OK;
			}
			else
			{
				std::cout << "Before : size = " << file.size() << " and data = " << (const char*)file.data() << std::endl;
				std::cout << "After  : size = " << edata.blob.size << " and data = " << (const char*)edata.blob.data << std::endl;
				die(UTST_INVALID_DATA, "Invalid return data !");
			}
		}
		else
			die(UTST_INVALID_ENTRY_TYPE, "Wrong entry type, should have been 'ppk_blob'");
		
	}
	else
		die(UTST_CANNOT_OPEN_FILE, "Cannot read file '" + std::string(argv[0]) + "'");
	
	return 0;
}
