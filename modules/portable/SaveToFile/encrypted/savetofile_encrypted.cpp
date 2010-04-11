/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

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

ppk_error encrypt(std::string& pwd)
{
	int size=ap_base64encode_len(pwd.size());
	char* buf=new char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64encode_binary(buf, (const unsigned char*)pwd.data(), pwd.size());
		pwd.assign(buf, final_len);
		delete[] buf;

		return PPK_OK;
	}
	else
		return PPK_INSUFFICIENT_MEMORY;
}

ppk_error decrypt(std::string& pwd_enc)
{
	int size=ap_base64decode_len(pwd_enc.data(), pwd_enc.size());
	unsigned char* buf=new unsigned char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64decode_binary(buf, (const char*)pwd_enc.data(), pwd_enc.size());
		pwd_enc.assign((char*)buf, final_len);
		delete[] buf;

		return PPK_OK;
	}
	else
		return PPK_INSUFFICIENT_MEMORY;
}

ppk_error readFile(std::string filepath, std::string& filecontent, unsigned int flags)
{
	filecontent=std::string();

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

		return decrypt(filecontent);
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
		ppk_error enc_res=encrypt(secret);
		if(enc_res==PPK_OK)
		{
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
			return enc_res;
	}
	else
		return PPK_FILE_CANNOT_BE_ACCESSED;
}
