#include "ppk_modules.h"
#include "tokenizer.h"
#include "vparam.h"

#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <cstring>

//portability functions
void* openLibrary(std::string lib_path);
void* loadSymbol(void* dlhandle, const char* symbolName);
const char* libraryError();

void PPK_Modules::debug(std::string msg)
{
	#ifdef DEBUG_MSG
		std::cout << msg;
	#endif
}

#if defined(WIN32) || defined(WIN64)
	#include <windows.h>
	void* openLibrary(std::string lib_path){return LoadLibraryA(lib_path.c_str());}
	void* loadSymbol(void* dlhandle, const char* symbolName){return (void*)GetProcAddress((HINSTANCE__*)dlhandle, symbolName);}
	const char* libraryError(){return "";}
	int closeLibrary(void* dlhandle){ return (int)FreeLibrary(dlhandle);}

	static const char* baseKey="Software\\PPassKeeper\\";
	size_t getRegistryValue(const char* key, char* ret, size_t max_size)
	{
		HKEY hk;
		char tmpBuf[512];
		if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE, baseKey, 0, KEY_QUERY_VALUE, &hk))
		{
			DWORD size=sizeof(tmpBuf);
			RegQueryValueEx(hk, key, 0, 0, (BYTE*)tmpBuf, &size);
			RegCloseKey(hk);
		
			strncpy(ret, tmpBuf, max_size-1);

			return size;
		}
		else
			return 0;
	}
	
	void PPK_Modules::loadList(void)
	{	
		WIN32_FIND_DATA File;
		HANDLE hSearch;

		modules.clear();
	
		//char dir_path[2048];
		//unsigned int len=GetEnvironmentVariableA("ppasskeeperMods", dir_path, (DWORD)sizeof(dir_path));
		char dir_path[512];
		getRegistryValue("mods_path", dir_path, sizeof(dir_path));
		std::string dirpath=dir_path;

		debug("--------------- <PPassKeeper> ---------------\nIf you don't want to see theses messages, recompile ppasskeeper with the cmake switch '-DPPK_DEBUG=OFF'\n");

		hSearch = FindFirstFile((dirpath+"\\*.dll").c_str(), &File);
		if (hSearch != INVALID_HANDLE_VALUE)
		{
			do
			{
				loadPlugin(dirpath, File.cFileName);
			}while (FindNextFile(hSearch, &File));

			FindClose(hSearch);
		}
		else
			debug("Could not open plugins directory : "+dirpath);

		debug("--------------- </PPassKeeper> ---------------\n\n");
	}
#else
	#include <dlfcn.h>
	void* openLibrary(std::string lib_path){return dlopen(lib_path.c_str(), RTLD_LAZY);}
	void* loadSymbol(void* dlhandle, const char* symbolName){return dlsym(dlhandle, symbolName);}
	const char* libraryError(){return dlerror();}
	int closeLibrary(void* dlhandle){ return (int)dlclose(dlhandle);}
	
	void PPK_Modules::loadList(void)
	{	
		DIR * plugindir;
		struct dirent * mydirent;

		modules.clear();
		
		debug("--------------- <PPassKeeper> ---------------\nIf you don't want to see theses messages, recompile ppasskeeper with the cmake switch '-DPPK_DEBUG=OFF'\n");

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
		else
			debug("Could not open plugins directory : "DIRECTORY_PATH);

		debug("--------------- </PPassKeeper> ---------------\n\n");
	}
#endif

//Private functions
void PPK_Modules::loadPlugin(std::string dirpath, std::string filename)
{
	//if the filename doesn't have the right prefix then try to load it as a shared object
	if(filename.size()>7 && filename.substr(0,7)=="libppk_")
	{
		//debug
		debug("Load the plugin '"+dirpath+"/"+filename+"': ");

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
			tm.maxDataSize=(_maxDataSize)loadSymbol(dlhandle, "maxDataSize");
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
			tm.constructor=(_constructor)loadSymbol(dlhandle, "constructor");
			tm.destructor=(_destructor)loadSymbol(dlhandle, "destructor");
			tm.availableParameters=(_availableParameters)loadSymbol(dlhandle, "availableParameters");
			tm.setParam=(_setParam)loadSymbol(dlhandle, "setParam");

		#ifdef DEBUG_MSG
			if(tm.getModuleID==NULL)std::cerr << "missing : getModuleID();";
			if(tm.getModuleName==NULL)std::cerr << "missing : getModuleName();";
			if(tm.getABIVersion==NULL)std::cerr << "missing : getABIVersion();";
			if(tm.readFlagsAvailable==NULL)std::cerr << "missing : readFlagsAvailable();";
			if(tm.writeFlagsAvailable==NULL)std::cerr << "missing : writeFlagsAvailable();";
			if(tm.listingFlagsAvailable==NULL)std::cerr << "missing : listingFlagsAvailable();";
			if(tm.maxDataSize==NULL)std::cerr << "missing : maxDataSize();";
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
			if(tm.getModuleID!=NULL && tm.getModuleName!=NULL && tm.getABIVersion!=NULL && tm.readFlagsAvailable!=NULL && tm.writeFlagsAvailable!=NULL && tm.listingFlagsAvailable!=NULL && tm.entryExists!=NULL && tm.maxDataSize!=NULL && tm.getEntry!=NULL && tm.setEntry!=NULL && tm.removeEntry!=NULL && tm.getLastError!=NULL && tm.isWritable!=NULL && tm.securityLevel!=NULL && tm.getEntryListCount!=NULL && tm.getEntryList!=NULL)
			{
				//Get the ID of the library
				tm.id=tm.getModuleID();
				tm.display_name=tm.getModuleName();
				
				//Call its constructor
				if(tm.constructor)
					tm.constructor();
					
				//Send the parameters
				if(tm.setParam!=NULL)
					sendParameters(tm);

				//Copy it into the modules list if it doesn't already exist
				if(getModuleByID(tm.id)==NULL)
				{
					modules[tm.id]=tm;
					debug((std::string("OK (ID=")+tm.id)+")\n");
				}
				else
					debug((std::string("FAILED (ID=") + tm.id) + " already exist in modules list)\n");
			}
			else
				debug("FAILED (not all symbols are present, check version numbers)\n");
		}
		else
			debug((std::string("FAILED (") + libraryError()) + ")\n");
	}
}

void PPK_Modules::eraseList(void)
{
	//Call the destructor on every module
	std::map<std::string, _module>::iterator iter;
	for( iter = modules.begin(); iter != modules.end(); iter++ )
	{
		if(iter->second.destructor)
			iter->second.destructor();

		closeLibrary(iter->second.dlhandle);
	}
}

void PPK_Modules::reload(void)
{
	eraseList();
	loadList();
}

void PPK_Modules::sendParameters(_module m)
{
	VParam& param = VParam::instance();
	
	if(m.setParam!=NULL)
	{
		std::vector<std::string> listParams = param.listParams(m.id);
		for(int i=0;i<listParams.size();i++)
		{
			cvariant cv = param.getParam(m.id, listParams[i].c_str());
			m.setParam(listParams[i].c_str(), cv);
		}
	}
}

//Public functions
PPK_Modules::PPK_Modules()
{
	loadList();
	
}

PPK_Modules::~PPK_Modules()
{
	eraseList();
}

unsigned int PPK_Modules::size()
{
	return modules.size()+1;
}

unsigned int PPK_Modules::getModulesList(ppk_module* pmodules, unsigned int ModulesCount)
{
	if(modules.size()>0)
	{
		//Automatic module
		pmodules[0].id=LIBPPK_DEFAULT_MODULE;
		pmodules[0].display_name=LIBPPK_DEFAULT_MODULE_DESC;
			
		std::map<std::string,_module>::iterator iter;
		unsigned int i;
		for(i=1,iter=modules.begin(); i<ModulesCount && iter!=modules.end(); i++,iter++)
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
	//Get the real module name
	if(strcmp(module_id, LIBPPK_DEFAULT_MODULE)==0)
		module_id=autoModule();
		
	//Does the module exist ?
	std::map<std::string,_module>::iterator fter = modules.find(module_id);
	if(fter!=modules.end())
		return &(fter->second);
	else
		return NULL;
}

const char* PPK_Modules::autoModule()
{
	if(getenv("GNOME_KEYRING_PID")!=NULL && \
	   getenv("GNOME_DESKTOP_SESSION_ID")!=NULL && \
	   ppk_module_is_available("GKeyring")
	  )
	{
		return "GKeyring";
	}
	else if(getenv("KDE_FULL_SESSION")!=NULL && \
	   strcmp(getenv("KDE_SESSION_VERSION"), "4")==0 && \
	   ppk_module_is_available("KWallet4")
	  )
	{
		return "KWallet4";
	}
	else if(ppk_module_is_available("SaveToFile_Enc"))
		return "SaveToFile_Enc";
	else if(ppk_module_is_available("SaveToFile_PT"))
		return "SaveToFile_PT";
	else if(ppk_module_count()>1)
	{
		ppk_module mods[2];
		ppk_module_list(mods, sizeof(mods));
		return mods[1].id;
	}
	else
		return "";
		
}


