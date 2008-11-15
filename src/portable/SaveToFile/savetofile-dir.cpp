#include <string>
#include <iostream>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)
	std::string dir()
	{
		std::string userprofile=getenv("USERPROFILE");
		return userprofile+"/ppasskeeper/";
	}
#else
	std::string dir()
	{
		std::string home=getenv("HOME");
		return home + "/.ppasskeeper/";
	}
#endif
