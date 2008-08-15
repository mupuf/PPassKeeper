#include <string>
#include <iostream>
#include <fstream>

//#define USE_CHMOD

#ifdef USE_CHMOD
	#include <sys/stat.h>
	#include <unistd.h>
	#include <sys/syscall.h>
	#include <errno.h>
#endif

std::string last_pwd;

//functions
extern void setError(std::string error);
bool mkdir(std::string path);
std::string dir();


std::string shortName()
{
	return "PT";
}

extern "C" const char* getModuleID()
{
	return "Save_To_File_PT";
}

extern "C" const char* getModuleName()
{
	return "Plain-text storing";
}

const char* getPassword(std::string filepath)
{
	//try to create the wanted directory
	mkdir(dir());

	//open the file
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		//Read the password from the file
		getline (inputfile,last_pwd);

		//close the file
		inputfile.close();

		return last_pwd.c_str();
	}
	else
	{
		setError("Could not open " + filepath + " for reading access.");
		return false;
	}
}

bool setPassword(std::string filepath, std::string secret)
{
	//open the file
	std::cout << "File '" << filepath << "' wiil be oppenned !" << std::endl;
	std::ofstream outputfile(filepath.c_str());
	if(outputfile.is_open())
	{
		std::cout << "File '" << filepath << "' oppenned !" << std::endl;
		
		//Save it
		outputfile << secret;

		//close the file
		outputfile.close();

		//Allow the file just to be read and written by its owner
		#ifdef USE_CHMOD
			int rc;
			rc = chmod(filepath.c_str(), 0600);
			if (rc == -1)
				fprintf(stderr, "Chmod on '%s' failed, errno = %d\n", filepath.c_str(), errno);
		#endif

		return true;
	}
	else
	{
		setError("Could not open " + filepath + " for reading access.");
		return false;
	}
}


//functions
