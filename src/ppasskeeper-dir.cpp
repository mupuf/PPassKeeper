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

