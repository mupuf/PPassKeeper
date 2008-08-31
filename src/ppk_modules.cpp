#include "ppk_modules.h"
#include "tokenizer.h"

#include <sys/types.h>
#include <dirent.h>

#include <iostream>

//portability functions
void* openLibrary(std::string lib_path);
void* loadSymbol(void* dlhandle, const char* symbolName);
const char* libraryError();

#if defined(WIN32) || defined(WIN64)
	#include <windows.h>
	void* openLibrary(std::string lib_path){return LoadLibraryA(lib_path.c_str());}
	void* loadSymbol(void* dlhandle, const char* symbolName){return (void*)GetProcAddress((HINSTANCE__*)dlhandle, symbolName);}
	const char* libraryError(){return "";}
	
	void PPK_Modules::loadPlugins(void)
	{	
		WIN32_FIND_DATA File;
		HANDLE hSearch;
    
	    hSearch = FindFirstFile("*.dll*", &File);
	    if (hSearch != INVALID_HANDLE_VALUE)
	    {
	        do {
	            printf("%s\n", File.cFileName);
	        } while (FindNextFile(hSearch, &File));
	        
	        FindClose(hSearch);
	    }
		else
			std::cout << "Could not open plugins directory: " << DIRECTORY_PATH << std::endl;
	}
#else
	#include <dlfcn.h>
	void* openLibrary(std::string lib_path){return dlopen(lib_path.c_str(), RTLD_LAZY);}
	void* loadSymbol(void* dlhandle, const char* symbolName){return dlsym(dlhandle, symbolName);}
	const char* libraryError(){return dlerror();}
	
	void PPK_Modules::loadPlugins(void)
	{	
		DIR * plugindir;
		struct dirent * mydirent;
		
		//Open Plugin's directory
		plugindir = opendir(DIRECTORY_PATH);
		if(plugindir!=NULL)
			while ((mydirent = readdir(plugindir))!=NULL)
				loadPlugin(mydirent->d_name);
		else
			std::cout << "Could not open plugins directory: " << DIRECTORY_PATH << std::endl;
	}
#endif

//Private functions
void PPK_Modules::loadPlugin(std::string filename)
{
	std::string filepath;
	
	//if the filename doesn't have the right prefix then try to load it as a shared object
	if(filename.size()>7 && filename.substr(0,7)=="libppk_")
	{
		//generate the filepath
		filepath=toString(DIRECTORY_PATH)+"/"+filename;

		//debug
		std::cout << "Load the plugin '" << filepath << "' : ";

		//Load the shared object
		void* dlhandle = openLibrary(filepath);
		if (dlhandle!=NULL)
		{
			_module tm; //Temporary module

			//Try to fill the module structure with the symbols
			tm.dlhandle=dlhandle;
			tm.getModuleID=(_getModuleID)loadSymbol(dlhandle, "getModuleID");			if(tm.getModuleID==NULL)std::cout << "missing : getModuleID()";
			tm.getModuleName=(_getModuleName)loadSymbol(dlhandle, "getModuleName");		if(tm.getModuleName==NULL)std::cout << "missing : getModuleName()";
			tm.getNetworkPassword=(_getNetworkPassword)loadSymbol(dlhandle, "getNetworkPassword"); if(tm.getNetworkPassword==NULL)std::cout << "missing : getNetworkPassword()";
			tm.setNetworkPassword=(_setNetworkPassword)loadSymbol(dlhandle, "setNetworkPassword"); if(tm.setNetworkPassword==NULL)std::cout << "missing : setNetworkPassword()";
			tm.getApplicationPassword=(_getApplicationPassword)loadSymbol(dlhandle, "getApplicationPassword"); if(tm.getApplicationPassword==NULL)std::cout << "missing : getApplicationPassword()";
			tm.setApplicationPassword=(_setApplicationPassword)loadSymbol(dlhandle, "setApplicationPassword"); if(tm.setApplicationPassword==NULL)std::cout << "missing : setApplicationPassword()";
			tm.getItem=(_getitem)loadSymbol(dlhandle, "getItem");	if(tm.getItem==NULL)std::cout << "missing : getItem()";
			tm.setItem=(_setItem)loadSymbol(dlhandle, "setItem");	if(tm.setItem==NULL)std::cout << "missing : setItem()";
			tm.getLastError=(_getLastError)loadSymbol(dlhandle, "getLastError");	if(tm.getLastError==NULL)std::cout << "missing : getLastError()";

			//if everything went fine, get the ID of the library and add it to the list
			if(tm.getModuleID!=NULL && tm.getModuleName!=NULL && tm.getNetworkPassword!=NULL && tm.setNetworkPassword!=NULL && tm.getApplicationPassword!=NULL && tm.setApplicationPassword!=NULL && tm.getItem!=NULL && tm.setItem!=NULL && tm.getLastError!=NULL)
			{
				//Get the ID of the library
				tm.id=(tm.getModuleID)();
				tm.display_name=(tm.getModuleName)();

				//Copy it into the modules list if it doesn't already exist
				if(getModuleByID(tm.id)==NULL)
				{
					modules[tm.id]=tm;
					std::cout << "OK (ID=" << tm.id << ")" << std::endl;
				}
				else
					std::cout << "FAILED (ID=" << tm.id << " already exist in modules list)" << std::endl;
			}
			else
				std::cout << "FAILED (not all symbols are present, check version numbers)" << std::endl;
		}
		else
			std::cout << "FAILED (" << libraryError() << ")" << std::endl;
	}
}

//Public functions
PPK_Modules::PPK_Modules()
{
	loadPlugins();
}

unsigned int PPK_Modules::size()
{
	return modules.size();
}

unsigned int PPK_Modules::getModulesList(PPassKeeper_Module* pmodules, unsigned int nbModules)
{
	if(modules.size()>0)
	{
		std::map<std::string,_module>::iterator iter;
		unsigned int i;
		for(i=0,iter=modules.begin(); i<nbModules && iter!=modules.end(); i++,iter++)
		{
			pmodules[i].id=iter->second.id;
			pmodules[i].display_name=iter->second.display_name;
		}

		return i;
	}
	else
		return false;
}

_module* PPK_Modules::getModuleByID(const char* module_id) //return the corresponding module or NULL if the module is not referenced
{
	//Does the module exist ?
	std::map<std::string,_module>::iterator fter = modules.find(module_id);
	if(fter!=modules.end())
		return &(fter->second);
	else
		return NULL;
}


