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

#include "base64.h"

//functions
extern void setError(std::string error);
extern std::string setting_dir();

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

std::string encrypt(const std::string pwd)
{
	std::string res;
	
	int size=ap_base64encode_len(pwd.size());
	char* buf=new char[size+1];
	int final_len=ap_base64encode_binary(buf, (const unsigned char*)pwd.data(), pwd.size());
	res.assign(buf, final_len);
	delete[] buf;
	
	return res;
}

std::string decrypt(const std::string pwd_enc)
{
	std::string res;
	
	int size=ap_base64decode_len(pwd_enc.data(), pwd_enc.size());
	unsigned char* buf=new unsigned char[size+1];
	int final_len=ap_base64decode_binary(buf, (const char*)pwd_enc.data(), pwd_enc.size());
	res.assign((char*)buf, final_len);
	delete[] buf;
	
	return res;
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
		
		//Set the error to none
		setError("");

		pwd=decrypt(pwd);
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
