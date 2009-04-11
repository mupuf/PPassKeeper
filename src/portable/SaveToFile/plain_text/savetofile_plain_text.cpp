#include "../../../ppasskeeper-module.h"
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

std::string& readFile(std::string filepath, unsigned int flags)
{
	static std::string pwd;
	
	pwd=std::string();

	//open the file
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		//Read the password from the file
		std::ostringstream pwd_s;
		pwd_s << inputfile.rdbuf();
		pwd=pwd_s.str();

		//close the file
		inputfile.close();

		return pwd;
	}
	else
	{
		setError("Could not open " + filepath + " for reading access.");
		return pwd;
	}
}

bool writeFile(std::string filepath, std::string secret, unsigned int flags)
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

		return true;
	}
	else
	{
		setError("Could not open " + filepath + " for write access.");
		return false;
	}
}

