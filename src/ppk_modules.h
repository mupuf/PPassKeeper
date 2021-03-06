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

#ifndef PPK_MODULES_H
#define PPK_MODULES_H

#include <map>
#include <string>

#include "ppasskeeper.h"
#include "ppasskeeper-module.h"

#define LIBPPK_DEFAULT_MODULE "Automatic"
#define LIBPPK_DEFAULT_MODULE_DESC "Auto - Automatic choice of the most suitable module."

class PPK_Modules
{
	private:
		std::map<std::string, _module> modules;
		
		void loadPlugin(std::string dirpath, std::string filepath);
		void loadList(void);
		void eraseList(void);
		
		std::string autoModule();
		void sendParameters(_module m);

		void debug(std::string msg);

	public:
		PPK_Modules();
		~PPK_Modules();

		void reload(void);
	
		unsigned int size();
		char** getModulesList();
		void freeModuleList(char** list);
		
		const _module* getModuleByID(const char* module_id);
		const char* getDisplayNameByID(const char* module_id);
};

#endif
