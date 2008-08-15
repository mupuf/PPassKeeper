#include <string>

#if defined(WIN32) || defined(WIN64)
	#include <direct.h>

	bool mkdir(std::string path)
	{
		return _mkdir(path.c_str())==0;
	}
#else	#include <sys/types.h>
	#include <sys/stat.h>
	
	bool mkdir(std::string path)
	{
		return mkdir(path.c_str(), 0)==0;
	}
#endif
