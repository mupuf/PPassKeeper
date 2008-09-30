#include "ppk_modules.h"
#include "tokenizer.h"

#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <cstring>

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
    
	    hSearch = FindFirstFile("ppasskeeper/*.dll*", &File);
	    if (hSearch != INVALID_HANDLE_VALUE)
	    {
	        do {
				loadPlugin(File.cFileName);
	        } while (FindNextFile(hSearch, &File));
	        
	        FindClose(hSearch);
	    }
#ifdef DEBUG_MSG
		else
			std::cerr << "Could not open plugins directory" << std::endl;
#endif
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
		{
			while ((mydirent = readdir(plugindir))!=NULL)
			{
				int i = strlen(mydirent->d_name) - 3;
				//suffix check: don't load libtool (.la) files
				if (i >= 0 && strcmp(mydirent->d_name + i, ".la")!=0)
					loadPlugin(mydirent->d_name);
			}
			
			closedir(plugindir);
		}
#ifdef DEBUG_MSG
		else
			std::cerr << "Could not open plugins directory: " << DIRECTORY_PATH << std::endl;
#endif
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

#ifdef DEBUG_MSG
		//debug
		std::cerr << "Load the plugin '" << filepath << "' : ";
#endif

		//Load the shared object
		void* dlhandle = openLibrary(filepath);
		if (dlhandle!=NULL)
		{
			_module tm; //Temporary module

			//Try to fill the module structure with the symbols
			tm.dlhandle=dlhandle;

			tm.getModuleID=(_getModuleID)loadSymbol(dlhandle, "getModuleID");
			tm.getModuleName=(_getModuleName)loadSymbol(dlhandle, "getModuleName");
			tm.getABIVersion=(_getABIVersion)loadSymbol(dlhandle, "getABIVersion");
			tm.readFlagsAvailable=(_readFlagsAvailable)loadSymbol(dlhandle, "readFlagsAvailable");
			tm.writeFlagsAvailable=(_writeFlagsAvailable)loadSymbol(dlhandle, "writeFlagsAvailable");

			//
			tm.getNetworkPassword=(_getNetworkPassword)loadSymbol(dlhandle, "getNetworkPassword");
			tm.setNetworkPassword=(_setNetworkPassword)loadSymbol(dlhandle, "setNetworkPassword");
			tm.getApplicationPassword=(_getApplicationPassword)loadSymbol(dlhandle, "getApplicationPassword");
			tm.setApplicationPassword=(_setApplicationPassword)loadSymbol(dlhandle, "setApplicationPassword");
			tm.getItem=(_getItem)loadSymbol(dlhandle, "getItem");
			tm.setItem=(_setItem)loadSymbol(dlhandle, "setItem");

			tm.isWritable=(_isWritable)loadSymbol(dlhandle, "isWritable");
			tm.securityLevel=(_securityLevel)loadSymbol(dlhandle, "securityLevel");

			tm.getPasswordListCount=(_getPasswordListCount)loadSymbol(dlhandle, "getPasswordListCount");
			tm.getPasswordList=(_getPasswordList)loadSymbol(dlhandle, "getPasswordList");

			//errors
			tm.getLastError=(_getLastError)loadSymbol(dlhandle, "getLastError");

		#ifdef DEBUG_MSG
			if(tm.getModuleID==NULL)std::cerr << "missing : getModuleID();";
			if(tm.getModuleName==NULL)std::cerr << "missing : getModuleName();";
			if(tm.getABIVersion==NULL)std::cerr << "missing : getABIVersion();";
			if(tm.readFlagsAvailable==NULL)std::cerr << "missing : readFlagsAvailable();";
			if(tm.writeFlagsAvailable==NULL)std::cerr << "missing : writeFlagsAvailable();";
			if(tm.getNetworkPassword==NULL)std::cerr << "missing : getNetworkPassword();";
			if(tm.setNetworkPassword==NULL)std::cerr << "missing : setNetworkPassword();";
			if(tm.getApplicationPassword==NULL)std::cerr << "missing : getApplicationPassword();";
			if(tm.setApplicationPassword==NULL)std::cerr << "missing : setApplicationPassword();";
			if(tm.getItem==NULL)std::cerr << "missing : getItem();";
			if(tm.setItem==NULL)std::cerr << "missing : setItem();";
			if(tm.isWritable==NULL)std::cerr << "missing : isWritable();";
			if(tm.securityLevel==NULL)std::cerr << "missing : securityLevel();";
			if(tm.getPasswordListCount==NULL)std::cerr << "missing : getPasswordListCount();";
			if(tm.getPasswordList==NULL)std::cerr << "missing : getPasswordList();";
			if(tm.getLastError==NULL)std::cerr << "missing : getLastError();";
		#endif
			
			//if minimal functions are here, add the lib to available modules
			if(tm.getModuleID!=NULL && tm.getModuleName!=NULL && tm.getABIVersion!=NULL && tm.readFlagsAvailable!=NULL && tm.writeFlagsAvailable!=NULL && tm.getNetworkPassword!=NULL && tm.setNetworkPassword!=NULL && tm.getApplicationPassword!=NULL && tm.setApplicationPassword!=NULL && tm.getItem!=NULL && tm.setItem!=NULL && tm.getLastError!=NULL && tm.isWritable!=NULL && tm.securityLevel!=NULL && tm.getPasswordListCount!=NULL && tm.getPasswordList!=NULL)
			{
				//Get the ID of the library
				tm.id=(tm.getModuleID)();
				tm.display_name=(tm.getModuleName)();

				//Copy it into the modules list if it doesn't already exist
				if(getModuleByID(tm.id)==NULL)
				{
					modules[tm.id]=tm;
#ifdef DEBUG_MSG
					std::cerr << "OK (ID=" << tm.id << ")" << std::endl;
#endif
				}
#ifdef DEBUG_MSG
				else
					std::cerr << "FAILED (ID=" << tm.id << " already exist in modules list)" << std::endl;
#endif
			}
#ifdef DEBUG_MSG
			else
				std::cerr << "FAILED (not all symbols are present, check version numbers)" << std::endl;
#endif
		}
#ifdef DEBUG_MSG
		else
			std::cerr << "FAILED (" << libraryError() << ")" << std::endl;
#endif
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

unsigned int PPK_Modules::getModulesList(PPassKeeper_Module* pmodules, unsigned int ModulesCount)
{
	if(modules.size()>0)
	{
		std::map<std::string,_module>::iterator iter;
		unsigned int i;
		for(i=0,iter=modules.begin(); i<ModulesCount && iter!=modules.end(); i++,iter++)
		{
			pmodules[i].id=iter->second.id;
			pmodules[i].display_name=iter->second.display_name;
		}

		return i;
	}
	else
		return false;
}

const _module* PPK_Modules::getModuleByID(const char* module_id) //return the corresponding module or NULL if the module is not referenced
{
	//Does the module exist ?
	std::map<std::string,_module>::iterator fter = modules.find(module_id);
	if(fter!=modules.end())
		return &(fter->second);
	else
		return NULL;
}


