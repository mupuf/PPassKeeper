#include "xmlParam.h"
#include "tokenizer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>

bool XMLParam::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
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

bool XMLParam::on_cdata(const std::string& cdata)
{
	return true;
}

bool XMLParam::on_tag_close(const std::string& tag_name)
{
	return true;
}

bool XMLParam::on_comment(const std::string& comment)
{
	return true;
}

bool XMLParam::on_processing(const std::string& value)
{
	return true;
}

bool XMLParam::on_doctype(const std::string& value)
{
	return true;
}

bool XMLParam::on_document_begin()
{
	return true;
}

bool XMLParam::on_document_end()
{
	return true;
}

void XMLParam::on_error(int errnr, int line, int col, const std::string& message)
{
}

bool XMLParam::flush()
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
			
			os << "	<entry module=\"" << iter->first.first << "\" key=\"" << iter->first.second << "\" type=\"" <<  type << "\" value=\"" <<  iter->second.second << "\" />" << std::endl;
		}

		os << "</ppk_params>" << std::endl;
		
		return true;
	}
	else
		std::cerr << "XMLParam::ppk_saveParam : '" << xml_path << "' cannot be opened for writing purpose." << std::endl << "Check your file permission." << std::endl;
	
	return false;
}

XMLParam::XMLParam(const std::string xmlPath)
{
	std::ifstream is(xmlPath.c_str());
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();
		
		if(!parse(iss.str()))
		{
			std::cerr << "XMLParam::XMLParam : parse(iss.str()) returned false !" << std::endl;
		}
	}
		
	this->xml_path=xmlPath;
}

bool XMLParam::saveParam(const char* module_id, const char* key, const cvariant value)
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

cvariant XMLParam::getParam(const char* module_id, const char* key)
{
	std::pair<std::string, std::string> pair(module_id, key);
	std::pair<int, std::string> value=params[pair];
	
	cvariant cv;
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

std::vector<std::string> XMLParam::listParams(const char* module_id)
{
	std::vector<std::string> list;
	
	std::map<std::pair<std::string, std::string>, std::pair<int, std::string> >::iterator iter;
	for( iter = params.begin(); iter != params.end(); iter++ )
		if(iter->first.first==module_id)
			list.push_back(iter->first.second);
		
	return list;
}

bool XMLParam::removeParam(const char* module_id, const char* key)
{
	std::map<std::pair<std::string, std::string>, std::pair<int, std::string> >::iterator iter;
	for( iter = params.begin(); iter != params.end(); iter++)
	{
		if(iter->first.second==key)
		{
			params.erase(iter);
			return flush();
		}
	}

	return false;
}
