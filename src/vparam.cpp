#include "vparam.h"

#include <ppasskeeper.h>

#ifdef USE_ELEKTRA

//===----------------------------------------------------------------------===//
// VParamImpl class implementation for elektra
//===----------------------------------------------------------------------===//

#include <kdb.h>

class VParamImpl
{
	friend class VParam;

	std::string elektraKeyName(const char* module_id, const char* key)
		{
			std::ostringstream keyName;
			keyName << "user/sw/ppasskeeper/" << module_id << '/' << key;
			return keyName.str();
		}

	bool saveParam(const char* module_id, const char* key, const char* value)
		{
			ckdb::KDB* handle;
			if ((handle = ckdb::kdbOpen()) == NULL)
			{
				perror("kdbOpen");
				return false;
			}

			ckdb::Key *k = ckdb::keyNew(elektraKeyName(module_id, key).c_str(), KEY_VALUE, value, KEY_END);

			bool r;
			if (k == NULL)
			{
				std::cerr << "Invalid parameter name" << std::endl;
				r = false;
			}
			else
			{
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

			if (ckdb::kdbClose(handle) != 0)
				perror("kdbClose");
			return r;
		}

	bool getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize)
		{
			ckdb::KDB* handle;
			if ((handle = ckdb::kdbOpen()) == NULL)
			{
				perror("kdbOpen");
				return false;
			}

			int size = ckdb::kdbGetString(handle, elektraKeyName(module_id, key).c_str(), returnedString, maxSize);

			//avoid potential buffer overflows
			returnedString[maxSize - 1] = '\0';

			bool r = (size > 0);

			if (ckdb::kdbClose(handle) != 0)
				perror("kdbClose");
			return r;
		}

	std::vector<std::string> listParams(const char* module_id)
		{
			std::vector<std::string> list;

			ckdb::KeySet *myConfig=ckdb::ksNew(0);
			ckdb::KDB * handle=ckdb::kdbOpen();

			int rc = 0;
			int size = 0;

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
					list.push_back((const char*)keyBaseName(current));
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
#include "tokenizer.h"
#include "xmlsp.h"

class VParamImpl : public XMLSP::Parser
{
	friend class VParam;

	std::string xml_path;
	//We associate <module, key> to <type, value>
	std::map< std::pair<std::string, std::string>, std::pair<int, std::string> > params;

	VParamImpl()
		{
			xml_path = ppk_settingDirectory() + std::string("/xmlParam.xml");

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

				if(attributes["type"]=="string")
					params[pair]=std::make_pair(cvariant_string, attributes["value"]);
				else if(attributes["type"]=="int")
					params[pair]=std::make_pair(cvariant_int, attributes["value"]);
				else if(attributes["type"]=="float")
					params[pair]=std::make_pair(cvariant_float, attributes["value"]);
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
						type="string";
						break;

					case cvariant_int:
						type="int";
						break;

					case cvariant_float:
						type="float";
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