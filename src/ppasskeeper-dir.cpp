#include <string>
#include <iostream>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)
	//******************************************
	//*********        Windows         *********
	//******************************************
	
	#include <direct.h>

	bool mkdir(std::string path)
	{
		return _mkdir(path.c_str())==0;
	}
	
	std::string dir()
	{
		std::string userprofile=getenv("USERPROFILE");
		std::string dir=userprofile+"/ppasskeeper/";
		mkdir(dir);
		return dir;
	}
	
#else
	//******************************************
	//*********       Linux/BSD        *********
	//******************************************
	
	#include <sys/types.h>
	#include <sys/stat.h>
	
	bool mkdir(std::string path)
	{
		return mkdir(path.c_str(), 0700)==0;
	}
	
	std::string dir()
	{
		std::string home=getenv("HOME");
		std::string dir=home+"/.ppasskeeper/";
		mkdir(dir);
		return dir;
	}
	
#endif

