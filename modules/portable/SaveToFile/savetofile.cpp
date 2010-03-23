#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include "ppasskeeper-dir.h"

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

//local functions
std::string shortName();
ppk_error readFile(std::string filename, std::string& filecontent, unsigned int flags);
ppk_error writeFile(std::string filename, std::string secret, unsigned int flags);

//private functions
ppk_error deletePassword(std::string path, unsigned int flags)
{
	if(remove(path.c_str())==0)
		return PPK_OK;
	else
		return PPK_FILE_CANNOT_BE_ACCESSED;
}

ppk_boolean fileExists(std::string filepath)
{
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		inputfile.close();
		return PPK_TRUE;
	}
	else
		return PPK_FALSE;
}

bool string_replace_first(std::string& str, const std::string to_replace, const std::string replaced_by)
{
	size_t found=str.find(to_replace);
	if (found!=std::string::npos)
	{
		str=str.replace(found, to_replace.size(), replaced_by);
		return true;
	}
	else
		return false;
}

//functions
extern "C"
{
	const char* getModuleName();

	const int getABIVersion()
	{
		return 1;
	}
	
	ppk_boolean isWritable()
	{
		return PPK_TRUE;
	}

	//Get available flags
	unsigned int readFlagsAvailable()
	{
		return ppk_rf_none|ppk_rf_silent;
	}

	unsigned int writeFlagsAvailable()
	{
		return ppk_wf_none|ppk_wf_silent;
	}
	
	unsigned int listingFlagsAvailable()
	{
		return ppk_lf_none|ppk_lf_silent;
	}

	std::string getKey(const ppk_entry* entry)
	{
		size_t len=ppk_key_length(entry);
		char* entry_cstr=new char[len+1];

		if(ppk_get_key(entry, entry_cstr, len)==PPK_TRUE)
		{
			std::string entry_std=entry_cstr;
			std::string type_s;
			switch(ppk_get_entry_type(entry))
			{
			case ppk_application:
				type_s="APP";
				break;
			case ppk_network:
				type_s="NET";
				string_replace_first(entry_std, "://", "¤");
				break;
			case ppk_item:
				type_s="ITM";
				break;
			}

			return setting_dir()+toString("/")+shortName()+"_"+type_s+"_"+entry_std;
		}
		else
			return std::string();
	}
	
	#if defined(WIN32) || defined(WIN64)
		#include <windows.h>
		
		static std::vector<std::string>& listEntries(const char* dir, unsigned int flags)
		{
			WIN32_FIND_DATA File;
			HANDLE hSearch;
			unsigned int pwdCount=0;
			std::vector<std::string> entries;

			std::string path;
			char* tmp=getenv("HOMEDRIVE");
			if(tmp!=NULL)
			{
				path=tmp;
				tmp=getenv("HOMEPATH");
				
				if(tmp!=NULL)
				{
					path+=tmp;
					
					hSearch = FindFirstFile((path+"\\ppasskeeper\\*").c_str(), &File);
					if (hSearch != INVALID_HANDLE_VALUE)
					{
						do {
							entries.push_back(File.cFileName);
						} while (FindNextFile(hSearch, &File));
						
						FindClose(hSearch);
					}
				}
			}
	#ifdef DEBUG_MSG
			else
				std::cerr << "Could not open pwd directory" << std::endl;
	#endif
	
			return entries;
		}
	#else
		#include <dlfcn.h>
		#include <sys/types.h>
		#include <dirent.h>
		
		static std::vector<std::string> listEntries(const char* dir, unsigned int flags)
		{
			std::vector<std::string> entries;
			DIR * pwddir;
			struct dirent* mydirent;

			//Open Plugin's directory
			pwddir = opendir(dir);
			if(pwddir!=NULL)
			{
				while ((mydirent = readdir(pwddir))!=NULL)
					entries.push_back(mydirent->d_name);

				closedir(pwddir);
			}
	#ifdef DEBUG_MSG
			else
				std::cerr << "Could not open password directory: " << dir << std::endl;
	#endif
			return entries;
		}
	#endif
	
	ppk_error getSimpleEntryList(char*** list, unsigned int flags)
	{
		if(list==NULL)
			return PPK_INVALID_ARGUMENTS;
		
		std::vector<std::string> entries=listEntries(setting_dir().c_str(), flags);
		
		if(entries.size()>0)
		{
			//Get the length of the prefix in order to compare it later to 
			//the begining of the entry name
			int lenPrefix=shortName().size();
			
			//Only keep entries of our own module (PT or ENC)
			std::vector<std::string> filtered;
			for(size_t i=0; i<entries.size(); i++)
			{
				std::string val=entries.at(i); //Pattern: PT_NET_http¤mupuf@mupuf.org:80
				if(val.substr(0, lenPrefix)==shortName())
				{
					val=val.substr(lenPrefix+1); //Pattern: NET_http¤mupuf@mupuf.org:80
					
					size_t us_pos=val.find('_');
					if(us_pos!=std::string::npos)
					{
						std::string type=val.substr(0, us_pos);	//Pattern: NET
						val=val.substr(us_pos+1);	//Pattern: http¤mupuf@mupuf.org:80
						
						if(type=="APP" || type=="ITM" || type=="NET")
						{
							if(type=="NET")
								string_replace_first(val, "¤", "://");	//Pattern: http://mupuf@mupuf.org:80
							
							filtered.push_back(val);
						}
					}
				}
			}

			//Copy to a char** list
			(*list)=new char*[filtered.size()+1];
			if((*list)!=NULL)
			{
				for(size_t i=0; i<filtered.size(); i++)
				{
					std::string val=filtered.at(i);

					(*list)[i]=new char[val.size()];
					strcpy((*list)[i], val.c_str());
				}
				(*list)[filtered.size()]=NULL;
			}
		}
		
		return PPK_OK;
	}

	//Get and Set passwords
	#include "ppasskeeper/data.h"
	ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
	{
		std::string pwd;
		ppk_error res_read=readFile(getKey(entry), pwd, flags);
		if(res_read==PPK_OK)
		{
			if(pwd.substr(0,strlen(STR_STRING))==STR_STRING)
				*edata=ppk_string_data_new(pwd.c_str()+strlen(STR_STRING));
			else if(pwd.substr(0,strlen(BLOB_STRING))==BLOB_STRING)
				*edata=ppk_blob_data_new(pwd.data()+strlen(BLOB_STRING), pwd.size()-strlen(BLOB_STRING));
			else
				return PPK_UNKNOWN_ENTRY_TYPE;
			
			return PPK_OK;
		}
		else
			return res_read;
	}

	ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
	{
		std::string data;
		if (edata->type==ppk_blob)
		{
			if(edata->blob.data!=NULL)
				data.assign((const char *) edata->blob.data, edata->blob.size);
			else
				data="";
			data=BLOB_STRING+data;
		}
		else if(edata->type==ppk_string)
		{
			if(edata->string!=NULL)
				data.assign(edata->string);
			else
				data = "";
			data=STR_STRING+data;
		}
		else
			return PPK_UNKNOWN_ENTRY_TYPE;
		
		return writeFile(getKey(entry).c_str(), data, flags);
	}
	
	ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
	{
		return deletePassword(getKey(entry), flags);
	}

	ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
	{
		return fileExists(getKey(entry));
	}
	
	unsigned int maxDataSize(ppk_data_type type)
	{
		switch(type)
		{
			case ppk_string:
				return (unsigned int)-1;
			case ppk_blob:
				return (unsigned int)-1;
		}
	
		return 0;
	}
}
