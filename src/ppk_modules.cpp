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

	static const char* baseKey="Software\\PPassKeeper\\";
	const char* getRegistryValue(const char* key)
	{
		static std::string value;

		HKEY hk;
		static char tmpBuf[101];
		if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, KEY_QUERY_VALUE, &hk))
		{
			DWORD size=sizeof(tmpBuf);
			RegQueryValueEx(hk, key, 0, 0, (BYTE*)tmpBuf, &size);
			RegCloseKey(hk);
		
			value=tmpBuf;
			return value.c_str();
		}
		else
			return "";
	}
	
	void PPK_Modules::loadPlugins(void)
	{	
		WIN32_FIND_DATA File;
		HANDLE hSearch;
	
		//char dir_path[2048];
		//unsigned int len=GetEnvironmentVariableA("ppasskeeperMods", dir_path, (DWORD)sizeof(dir_path));
		std::string dirpath=getRegistryValue("mods_path");

		hSearch = FindFirstFile((dirpath+"\\*.dll").c_str(), &File);
		if (hSearch != INVALID_HANDLE_VALUE)
		{
			do
			{
				loadPlugin(dirpath, File.cFileName);
			}while (FindNextFile(hSearch, &File));

			FindClose(hSearch);
		}
#ifdef DEBUG_MSG
		else
			std::cerr << "Could not open plugins directory : " << dirpath << std::endl;
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
					loadPlugin(DIRECTORY_PATH, mydirent->d_name);
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
void PPK_Modules::loadPlugin(std::string dirpath, std::string filename)
{
	//if the filename doesn't have the right prefix then try to load it as a shared object
	if(filename.size()>7 && filename.substr(0,7)=="libppk_")
	{
#ifdef DEBUG_MSG
		//debug
		std::cerr << "Load the plugin '" << dirpath+"/"+filename << "' : ";
#endif

		//Load the shared object
		void* dlhandle = openLibrary(dirpath+"/"+filename);
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
			tm.listingFlagsAvailable=(_listingFlagsAvailable)loadSymbol(dlhandle, "listingFlagsAvailable");

			//
			tm.entryExists=(_entryExists)loadSymbol(dlhandle, "entryExists");
			tm.getEntry=(_getEntry)loadSymbol(dlhandle, "getEntry");
			tm.setEntry=(_setEntry)loadSymbol(dlhandle, "setEntry");
			tm.removeEntry=(_removeEntry)loadSymbol(dlhandle, "removeEntry");

			tm.isWritable=(_isWritable)loadSymbol(dlhandle, "isWritable");
			tm.securityLevel=(_securityLevel)loadSymbol(dlhandle, "securityLevel");

			tm.getEntryListCount=(_getEntryListCount)loadSymbol(dlhandle, "getEntryListCount");
			tm.getEntryList=(_getEntryList)loadSymbol(dlhandle, "getEntryList");

			//errors
			tm.getLastError=(_getLastError)loadSymbol(dlhandle, "getLastError");

			//optionnal
			tm.setCustomPromptMessage=(_setCustomPromptMessage)loadSymbol(dlhandle, "setCustomPromptMessage");

		#ifdef DEBUG_MSG
			if(tm.getModuleID==NULL)std::cerr << "missing : getModuleID();";
			if(tm.getModuleName==NULL)std::cerr << "missing : getModuleName();";
			if(tm.getABIVersion==NULL)std::cerr << "missing : getABIVersion();";
			if(tm.readFlagsAvailable==NULL)std::cerr << "missing : readFlagsAvailable();";
			if(tm.writeFlagsAvailable==NULL)std::cerr << "missing : writeFlagsAvailable();";
			if(tm.listingFlagsAvailable==NULL)std::cerr << "missing : listingFlagsAvailable();";
			if(tm.entryExists==NULL)std::cerr << "missing : entryExists();";
			if(tm.getEntry==NULL)std::cerr << "missing : getEntry();";
			if(tm.setEntry==NULL)std::cerr << "missing : setEntry();";
			if(tm.removeEntry==NULL)std::cerr << "missing : removeEntry();";
			if(tm.isWritable==NULL)std::cerr << "missing : isWritable();";
			if(tm.securityLevel==NULL)std::cerr << "missing : securityLevel();";
			if(tm.getEntryListCount==NULL)std::cerr << "missing : getEntryListCount();";
			if(tm.getEntryList==NULL)std::cerr << "missing : getEntryList();";
			if(tm.getLastError==NULL)std::cerr << "missing : getLastError();";
		#endif
			
			//if minimal functions are here, add the lib to available modules
			if(tm.getModuleID!=NULL && tm.getModuleName!=NULL && tm.getABIVersion!=NULL && tm.readFlagsAvailable!=NULL && tm.writeFlagsAvailable!=NULL && tm.listingFlagsAvailable!=NULL && tm.entryExists!=NULL && tm.getEntry!=NULL && tm.setEntry!=NULL && tm.removeEntry!=NULL && tm.getLastError!=NULL && tm.isWritable!=NULL && tm.securityLevel!=NULL && tm.getEntryListCount!=NULL && tm.getEntryList!=NULL)
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

unsigned int PPK_Modules::getModulesList(ppk_module* pmodules, unsigned int ModulesCount)
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


