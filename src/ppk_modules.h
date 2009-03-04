#ifndef PPK_MODULES_H
#define PPK_MODULES_H

#include <map>
#include <string>

#include "ppasskeeper.h"
#include "ppasskeeper-module.h"

class PPK_Modules
{
	private:
		std::map<std::string, _module> modules;
		
		void loadPlugin(std::string dirpath, std::string filepath);
		void loadPlugins(void);

	public:
		PPK_Modules();
		~PPK_Modules();
	
		unsigned int size();
		unsigned int getModulesList(ppk_module* pmodules, unsigned int ModulesCount);
		
		const _module* getModuleByID(const char* module_id);
};

#endif
