#include "import-export.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include "tokenizer.h"
#include "base64.h"

#define STRING_TYPE "string"
#define BLOB_TYPE "blob"

ImportExport::ImportExport()
{
	
}

bool ImportExport::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if (tag_name == "entry")
	{
		if(attributes["key"]=="")
			return true;
			
		ppk_entry* entry=ppk_entry_new_from_key(attributes["key"].c_str());
		ppk_data* data=NULL;
		
		if(attributes["data_type"]==STRING_TYPE)
			data=ppk_string_data_new(attributes["value"].c_str());
		else if(attributes["data_type"]==BLOB_TYPE)
		{
			std::string decoded_value=decrypt(attributes["value"].c_str());
			data=ppk_blob_data_new(decoded_value.c_str(), decoded_value.size());
		}
		
		if(data!=NULL)
			entries[entry]=data;
	}
	return true;
}

std::string ImportExport::encrypt(const ppk_data* edata)
{
	std::string res;
	
	int size=ap_base64encode_len(edata->blob.size);
	char* buf=new char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64encode_binary(buf, (const unsigned char*)edata->blob.data, edata->blob.size);
		res.assign(buf, final_len);
		delete[] buf;
	}
	
	return res;
}

std::string ImportExport::decrypt(const char* data)
{
	std::string res;
	int len_data=strlen(data);
	
	int size=ap_base64decode_len((const char*)data, len_data);
	unsigned char* buf=new unsigned char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64decode_binary(buf, (const char*)data, len_data);
		res.assign((char*)buf, final_len);
		delete[] buf;
	}
	
	return res;
}

bool ImportExport::flush(std::map<ppk_entry*, ppk_data*> &entries, std::string xml_path)
{
	std::ofstream os(xml_path.c_str());
	if(os.is_open())
	{
		os << "<ppk_dump>" << std::endl;
		
		char* key=NULL;
		size_t curKeySize=0;

		std::map<ppk_entry*, ppk_data*>::iterator iter;
		for( iter = entries.begin(); iter != entries.end(); iter++ )
		{
			std::string type;
			std::string value;
			
			//Get the value
			switch(ppk_get_data_type(iter->second))
			{
			case ppk_string:
				type=STRING_TYPE;
				value=ppk_get_data_string(iter->second);
				break;

			case ppk_blob:
				type=BLOB_TYPE;
				value=encrypt(iter->second);
				break;
			}
			
			//Get the key size and alloc a big-enough buffer to store the key
			size_t size=ppk_key_length(iter->first);
			if(size>curKeySize)
			{
				if(key!=NULL)
					delete[] key;
				key=new char[size];
				curKeySize=size;
			}

			//Get the key
			ppk_boolean res=ppk_get_key(iter->first, key, curKeySize);
			if(res==PPK_TRUE)
				os << " <entry key=\"" << key << "\" data_type=\"" <<  type << "\" value=\"" <<  value << "\" />" << std::endl;
		}

		os << "</ppk_dump>" << std::endl;
		
		if(key!=NULL)
			delete[] key;

		return true;
	}
	else
		std::cerr << "ppk_dump : '" << xml_path << "' cannot be opened for writing purpose." << std::endl << "Check your file permission." << std::endl;

	return false;
}

void ImportExport::free_entry_data(std::map<ppk_entry*, ppk_data*> &entries)
{
	std::map<ppk_entry*, ppk_data*>::iterator iter;
	for( iter = entries.begin(); iter != entries.end(); iter++ )
	{
		ppk_data_free(iter->second);
	}
	
	entries.clear();
}

ppk_error ImportExport::dumpModuleToFile(const char* module_id, const char* filepath)
{
	ppk_error globalRes=PPK_OK;
	std::map< ppk_entry*, ppk_data* > entries;
	
	//Get all entries and store them in entries
	ppk_entry **entryList;
	size_t nbEntries;

	ppk_error res=ppk_module_get_entry_list(module_id, ppk_network|ppk_application|ppk_item, &entryList, &nbEntries, ppk_rf_none);
	if(res!=PPK_OK)
		return res;

	for(int i=0; i<nbEntries; i++)
	{
		ppk_data* edata;
		
		res=ppk_module_get_entry(module_id, entryList[i], &edata, ppk_rf_none);
		if(res!=PPK_OK && globalRes==PPK_OK)
			globalRes=res;
		else if(res==PPK_OK)
			entries[entryList[i]]=edata;
	}
	
	//Store all the entries
	flush(entries, filepath);
	
	//Free the list entry list
	ppk_module_free_entry_list(entryList);
	
	//Free all entry data
	free_entry_data(entries);

	return globalRes;
}

ppk_error ImportExport::importFromFileToModule(const char* module_id, const char* filepath)
{
	ppk_error globalRes=PPK_OK;
	
	std::ifstream is(filepath);
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();

		if(parse(iss.str()))
		{
			//Set all the entries
			std::map<ppk_entry*, ppk_data*>::iterator iter;
			for( iter = entries.begin(); iter != entries.end(); iter++ )
			{
				ppk_error res=ppk_module_set_entry(module_id, iter->first, iter->second, ppk_rf_none);
					
				if(res!=PPK_OK && globalRes==PPK_OK)
					globalRes=res;
			}
		}
		else
			std::cerr << "importFromFile : parse(" << filepath << ") returned false !" << std::endl;
	}
	
	return globalRes;
}
