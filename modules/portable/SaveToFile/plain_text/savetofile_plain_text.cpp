#include "ppasskeeper.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef HAVE_CHMOD
	#include <sys/stat.h>
	#include <unistd.h>
	#include <sys/syscall.h>
	#include <errno.h>
#endif

//functions
extern void setError(std::string error);
std::string dir();


std::string shortName()
{
	return "PT";
}

extern "C" const char* getModuleID()
{
	return "SaveToFile_PT";
}

extern "C" const char* getModuleName()
{
	return "Plain-text storage";
}

extern "C" ppk_security_level securityLevel(const char* module_id)
{
	return ppk_sec_lowest;
}

ppk_error readFile(std::string filepath, std::string& filecontent, unsigned int flags)
{
	//open the file
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		//Read the password from the file
		std::ostringstream pwd_s;
		pwd_s << inputfile.rdbuf();
		filecontent=pwd_s.str();

		//close the file
		inputfile.close();

		return PPK_OK;
	}
	else
		return PPK_ENTRY_UNAVAILABLE;
}

ppk_error writeFile(std::string filepath, std::string secret, unsigned int flags)
{
	//open the file
	std::ofstream outputfile(filepath.c_str());
	if(outputfile.is_open())
	{		
		//Save it
		outputfile << secret;

		//close the file
		outputfile.close();

		//Allow the file just to be read and written by its owner
		#ifdef HAVE_CHMOD
			int rc;
			rc = chmod(filepath.c_str(), 0600);
			if (rc == -1)
				fprintf(stderr, "Chmod on '%s' failed, errno = %d\n", filepath.c_str(), errno);
		#endif

		return PPK_OK;
	}
	else
	{
		return PPK_ENTRY_UNAVAILABLE;
	}
}

