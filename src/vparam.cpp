#include "vparam.h"

#include <ppasskeeper.h>
#include "tokenizer.h"

#define STRING_TYPE "string"
#define INT_TYPE "int"
#define FLOAT_TYPE "float"
#define BOOL_TYPE "boolean"
#define TYPE_SEPARATOR ":"

#ifdef USE_ELEKTRA

//===----------------------------------------------------------------------===//
// VParamImpl class implementation for elektra
//===----------------------------------------------------------------------===//

#include <kdb.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class VParamImpl
{
	friend class VParam;

	std::string elektraKeyName(const char* module_id, const char* key)
	{
		std::ostringstream keyName;
		keyName << "user/sw/ppasskeeper/" << module_id << '/' << key;
		return keyName.str();
	}

	bool saveParam(const char* module_id, const char* key, const cvariant value)
	{
		//Get the type of the value to store
		std::string type, s_value, final_value;
		switch(cvariant_get_type(value))
		{
		case cvariant_string:
			type=STRING_TYPE;
			s_value=cvariant_get_string(value);
			break;

		case cvariant_int:
			type=INT_TYPE;
			s_value=toString(cvariant_get_int(value));
			break;

		case cvariant_float:
			type=FLOAT_TYPE;
			s_value=toString(cvariant_get_float(value));
			break;
			
		case cvariant_boolean:
			type=BOOL_TYPE;
			s_value=cvariant_get_bool(value)==cvariant_true?"true":"false";
			break;
			
		case cvariant_none:
			break;
		}

		//Create the value name
		final_value=type+TYPE_SEPARATOR+s_value;

		//Open libElektra
		ckdb::KDB* handle;
		if ((handle = ckdb::kdbOpen()) == NULL)
		{
			perror("kdbOpen");
			return false;
		}

		//Create the key
		ckdb::Key *k = ckdb::keyNew(elektraKeyName(module_id, key).c_str(), KEY_VALUE, final_value.c_str(), KEY_END);

		bool r;
		if (k == NULL)
		{
			std::cerr << "Invalid parameter name" << std::endl;
			r = false;
		}
		else
		{
			//Set the key
			int err = ckdb::kdbSetKey(handle, k);
			ckdb::keyDel(k);

			if (err == 0)
				r = true;
			else
			{
				perror("kbdSetKey");
				r = false;
			}
		}

		//Close libelektra
		if (ckdb::kdbClose(handle) != 0)
			perror("kdbClose");
		return r;
	}

	cvariant getParam(const char* module_id, const char* key)
	{
		cvariant result=cvariant_null();
		char returnedString[1001];

		//Get data from elektra
		ckdb::KDB* handle;
		if ((handle = ckdb::kdbOpen()) == NULL)
		{
			perror("kdbOpen");
			return result;
		}

		/*int size = */ckdb::kdbGetString(handle, elektraKeyName(module_id, key).c_str(), returnedString, sizeof(returnedString)-1);

		if (ckdb::kdbClose(handle) != 0)
			perror("kdbClose");

		//avoid potential buffer overflows
		returnedString[sizeof(returnedString)-1] = '\0';
		std::string value=returnedString;

		if(value.substr(0, strlen(STRING_TYPE TYPE_SEPARATOR))==STRING_TYPE TYPE_SEPARATOR)
		{
			value=value.substr(strlen(STRING_TYPE TYPE_SEPARATOR));
			result=cvariant_from_string_copy(value.c_str(), value.size());
		}
		else if(value.substr(0, strlen(INT_TYPE TYPE_SEPARATOR))==INT_TYPE TYPE_SEPARATOR)
		{
			value=value.substr(strlen(INT_TYPE TYPE_SEPARATOR));
			result=cvariant_from_int(atoi(value.c_str()));
		}
		else if(value.substr(0, strlen(INT_TYPE TYPE_SEPARATOR))==INT_TYPE TYPE_SEPARATOR)
		{
			value=value.substr(strlen(FLOAT_TYPE TYPE_SEPARATOR));
			result=cvariant_from_int(atof(value.c_str()));
		}
		else if(value.substr(0, strlen(INT_TYPE TYPE_SEPARATOR))==INT_TYPE TYPE_SEPARATOR)
		{
			value=value.substr(strlen(INT_TYPE TYPE_SEPARATOR));
			result=cvariant_from_bool(value=="true"?cvariant_true:cvariant_false);
		}

		return result;
	}

	std::vector<std::string> listParams(const char* module_id)
	{
		std::vector<std::string> list;

		ckdb::KeySet *myConfig=ckdb::ksNew(0);
		ckdb::KDB * handle=ckdb::kdbOpen();

		/* Get all value keys for this application */
		if (ckdb::kdbGetByName(handle, myConfig, elektraKeyName(module_id,"").c_str(),  0) == -1)
		{
			perror("Couldn't get user configuration. Reason");
		} else {
			//size = (int)ksGetSize(myConfig);

			ckdb::Key *current;
			cursor_t cursor = ckdb::ksGetCursor (myConfig);
			ckdb::ksRewind (myConfig);
			while ((current = ckdb::ksNext (myConfig)) != 0)
				list.push_back((const char*)ckdb::keyBaseName(current));
			ckdb::ksSetCursor(myConfig, cursor);
		}

		ckdb::kdbClose(handle);
		ckdb::ksDel(myConfig);

		return list;
	}

	bool removeParam(const char* module_id, const char* key)
	{
		bool r;

		ckdb::KDB* handle;
		if ((handle = ckdb::kdbOpen()) == NULL)
		{
			perror("kdbOpen");
			return false;
		}

		int err=ckdb::kdbRemove(handle, elektraKeyName(module_id, key).c_str());
		if (err == 0)
			r = true;
		else
		{
			perror("kdbRemove");
			r = false;
		}

		if (ckdb::kdbClose(handle) != 0)
			perror("kdbClose");

		return r;
	}
};

#else // Use XML

//===----------------------------------------------------------------------===//
// VParamImpl class implementation for XML
//===----------------------------------------------------------------------===//

#include <fstream>
#include <sstream>
#include <iostream>
#include "xmlsp.h"

#include <ppasskeeper/utils.h>

class VParamImpl : public XMLSP::Parser
{
	friend class VParam;

	std::string xml_path;
	//We associate <module, key> to <type, value>
	std::map< std::pair<std::string, std::string>, std::pair<int, std::string> > params;

	VParamImpl()
	{
		xml_path = ppk_settings_directory() + std::string("/xmlParam.xml");

		std::ifstream is(xml_path.c_str());
		if(is.is_open())
		{
			std::ostringstream iss;
			iss << is.rdbuf();

			if(!parse(iss.str()))
			{
				std::cerr << "XMLParamImpl : parse(iss.str()) returned false !" << std::endl;
			}
		}

		this->xml_path = xml_path;
	}

	virtual bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
	{
		if (tag_name == "entry")
		{
			std::pair<std::string, std::string> pair(attributes["module"], attributes["key"]);

			if(attributes["type"]==STRING_TYPE)
				params[pair]=std::make_pair(cvariant_string, attributes["value"]);
			else if(attributes["type"]==INT_TYPE)
				params[pair]=std::make_pair(cvariant_int, attributes["value"]);
			else if(attributes["type"]==FLOAT_TYPE)
				params[pair]=std::make_pair(cvariant_float, attributes["value"]);
			else if(attributes["type"]==BOOL_TYPE)
				params[pair]=std::make_pair(cvariant_boolean, attributes["value"]);
		}
		return true;
	}

	virtual bool on_cdata(const std::string& cdata) { return true; }
	virtual bool on_tag_close(const std::string& tag_name) { return true; }
	virtual bool on_comment(const std::string& comment) { return true; }
	virtual bool on_processing(const std::string& value) { return true; }
	virtual bool on_doctype(const std::string& value) { return true; }
	virtual bool on_document_begin() { return true; }
	virtual bool on_document_end() { return true; }
	virtual void on_error(int errnr, int line, int col, const std::string& message) {}

	bool flush()
	{
		std::ofstream os(xml_path.c_str());
		if(os.is_open())
		{
			os << "<ppk_params>" << std::endl;

			std::map<std::pair<std::string, std::string>, std::pair<int, std::string> >::iterator iter;
			for( iter = params.begin(); iter != params.end(); iter++ )
			{
				std::string type;
				switch(iter->second.first)
				{
				case cvariant_string:
					type=STRING_TYPE;
					break;

				case cvariant_int:
					type=INT_TYPE;
					break;

				case cvariant_float:
					type=FLOAT_TYPE;
					break;
					
				case cvariant_boolean:
					type=BOOL_TYPE;
					break;
					
				case cvariant_none:
					break;
				}

				os << " <entry module=\"" << iter->first.first << "\" key=\"" << iter->first.second << "\" type=\"" <<  type << "\" value=\"" <<  iter->second.second << "\" />" << std::endl;
			}

			os << "</ppk_params>" << std::endl;

			return true;
		}
		else
			std::cerr << "ppk_saveParam : '" << xml_path << "' cannot be opened for writing purpose." << std::endl << "Check your file permission." << std::endl;

		return false;
	}

	bool saveParam(const char* module_id, const char* key, const cvariant value)
	{
		std::string s_value;
		switch(cvariant_get_type(value))
		{
		case cvariant_string:
			s_value=cvariant_get_string(value);
			break;

		case cvariant_int:
			s_value=toString(cvariant_get_int(value));
			break;

		case cvariant_float:
			s_value=toString(cvariant_get_float(value));
			break;
			
		case cvariant_boolean:
			s_value=(cvariant_get_bool(value)==cvariant_true?"true":"false");
			break;

		case cvariant_none:
			return false;
			break;
		}

		params[std::make_pair(module_id, key)]=std::make_pair(cvariant_get_type(value), s_value);

		return flush();
	}

	cvariant getParam(const char* module_id, const char* key)
	{
		std::pair<std::string, std::string> pair(module_id, key);
		std::pair<int, std::string> value=params[pair];

		cvariant cv=cvariant_null();
		switch(value.first)
		{
		case cvariant_string:
			cv=cvariant_from_string(value.second.c_str());
			break;

		case cvariant_int:
			cv=cvariant_from_int(stringTo<int>(value.second.c_str()));
			break;

		case cvariant_float:
			cv=cvariant_from_float(stringTo<double>(value.second.c_str()));
			break;

		case cvariant_none:
			//Will return cvariant_null()
			break;
		}

		return cv;
	}

	std::vector<std::string> listParams(const char* module_id)
	{
		std::vector<std::string> list;

		std::map<std::pair<std::string, std::string>, std::pair<int, std::string> >::iterator iter;
		for(iter = params.begin(); iter != params.end(); iter++)
			if(iter->first.first==module_id)
				list.push_back(iter->first.second);

		return list;
	}

	bool removeParam(const char* module_id, const char* key)
	{
		std::map<std::pair<std::string, std::string>, std::pair<int, std::string> >::iterator iter;
		for(iter = params.begin(); iter != params.end(); iter++)
		{
			if(iter->first.second==key)
			{
				params.erase(iter);
				return flush();
			}
		}

		return false;
	}
};

#endif

//===----------------------------------------------------------------------===//
// VParam class implementation
//===----------------------------------------------------------------------===//

VParam::VParam() {}

VParam& VParam::instance()
{
	static VParam inst;

	//initialize impl immediately (thread-safety)
	(void) inst.impl();

	return inst;
}

VParamImpl& VParam::impl()
{
	static VParamImpl _impl;
	return _impl;
}

bool VParam::saveParam(const char* module_id, const char* key, const cvariant value)
{
	return impl().saveParam(module_id, key, value);
}

cvariant VParam::getParam(const char* module_id, const char* key)
{
	return impl().getParam(module_id, key);
}

std::vector<std::string> VParam::listParams(const char* module_id)
{
	return impl().listParams(module_id);
}

bool VParam::removeParam(const char* module_id, const char* key)
{
	return impl().removeParam(module_id, key);
}
