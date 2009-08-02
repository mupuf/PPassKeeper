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
		
		const char* autoModule();
		void sendParameters(_module m);

	public:
		PPK_Modules();
		~PPK_Modules();

		void reload(void);
	
		unsigned int size();
		unsigned int getModulesList(ppk_module* pmodules, unsigned int ModulesCount);
		
		const _module* getModuleByID(const char* module_id);
};

#endif
