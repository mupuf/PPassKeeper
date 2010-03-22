#include <string>
#include <iostream>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)
	//******************************************
	//*********        Windows         *********
	//******************************************
	
	#include <direct.h>

	int mkdir(const char* path, int /*chmod*/)
	{
		return _mkdir(path.c_str());
	}
	
	std::string setting_dir()
	{
		std::string userprofile=getenv("USERPROFILE");
		std::string dir=userprofile+"/ppasskeeper/";
		mkdir(dir.c_str(), 0700);
		return dir.c_str();
	}
	
#else
	//******************************************
	//*********       Linux/BSD        *********
	//******************************************
	
	#include <sys/types.h>
	#include <sys/stat.h>
	
	int ppk_mkdir(const char* path, int chmod)
	{
		return mkdir(path, chmod);
	}
	
	std::string setting_dir()
	{
		std::string home=getenv("HOME");
		std::string dir=home+"/.ppasskeeper/";
		mkdir(dir.c_str(), 0700);
		return dir.c_str();
	}
	
#endif

