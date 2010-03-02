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
