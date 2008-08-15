#include <string>
#include <iostream>
#include <stdlib.h>
#if defined(WIN32) || defined(WIN64)
	std::string dir()
	{
		std::string userprofile=getenv("USERPROFILE");
		return userprofile+"/ppasskeeper/";
	}
#else	std::string dir()
	{
		std::string logname=getenv("LOGNAME");
		return "/home/"+logname+"/.ppasskeeper/";
	}
#endif
