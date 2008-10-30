#include "../../../ppasskeeper-module.h"
#include <string>
#include <iostream>
#include <fstream>

#ifdef HAVE_CHMOD
	#include <sys/stat.h>
	#include <unistd.h>
	#include <sys/syscall.h>
	#include <errno.h>
#endif

#include "base64.h"

//functions
extern void setError(std::string error);
bool mkdir(std::string path);
std::string dir();

//Interface
std::string shortName()
{
	return "ENC";
}

extern "C" const char* getModuleID()
{
	return "SaveToFile_Enc";
}

extern "C" const char* getModuleName()
{
	return "Poorly-encrypted storage";
}

extern "C" ppk_security_level securityLevel(const char* module_id)
{
	return ppk_sec_scrambled;
}

std::string encrypt(std::string pwd)
{
	//encryption algorithm
	return base64_enc(pwd);
}

std::string decrypt(std::string pwd_enc)
{
	//decryption algorithm
	return base64_dec(pwd_enc);
}

const char* readFile(std::string filepath, unsigned int flags)
{
	static std::string pwd;

	//open the file
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		//Read the password from the file
		getline (inputfile, pwd);

		//close the file
		inputfile.close();
		
		//Set the error to none
		setError("");

		return decrypt(pwd).c_str();
	}
	else
	{
		setError("Could not open " + filepath + " for reading access.");
		return NULL;
	}
}

bool writeFile(std::string filepath, std::string secret, unsigned int flags)
{
	//try to create the wanted directory
	mkdir(dir());

	//open the file
	std::ofstream outputfile(filepath.c_str());
	if(outputfile.is_open())
	{
		//Save it
		outputfile << encrypt(secret);

		//close the file
		outputfile.close();

		//Allow the file just to be read and written by its owner
		#ifdef HAVE_CHMOD
			int rc;
			rc = chmod(filepath.c_str(), 0600);
			if (rc == -1)
				fprintf(stderr, "Chmod on '%s' failed, errno = %d\n", filepath.c_str(), errno);
		#endif

		//Set the error to none
		setError("");

		return true;
	}
	else
	{
		setError("Could not open " + filepath + " for write access.");
		return false;
	}
}
