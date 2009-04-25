#include "xmlParam.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>

bool XMLParam::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if (tag_name == "entry")
	{
		std::pair<std::string, std::string> pair(attributes["module"], attributes["key"]);
		params[pair]=attributes["value"];
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
		
		std::map<std::pair<std::string, std::string>, std::string>::iterator iter;   
		for( iter = params.begin(); iter != params.end(); iter++ )
			os << "	<entry module=\"" << iter->first.first << "\" key=\"" << iter->first.second << "\" value=\"" <<  iter->second << "\" />" << std::endl;

		os << "</ppk_params>" << std::endl;
		
		return true;
	}
	else
		std::cerr << "XMLParam::ppk_saveParam : '" << xml_path << "' cannot be openned." << std::endl << "Check your file permission." << std::endl;
	
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
	else
		std::cerr << "XMLParam::XMLParam : '" << xmlPath << "' cannot be openned." << std::endl << "Check your file permission." << std::endl;
		
	this->xml_path=xmlPath;
}

bool XMLParam::saveParam(const char* module_id, const char* key, const char* value)
{
	params[std::make_pair(module_id, key)]=value;
	
	return flush();
}

bool XMLParam::getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize)
{
	std::pair<std::string, std::string> pair(module_id, key);
	std::string value=params[pair];
	
	if(value!=std::string())
	{
		strncpy(returnedString, value.c_str(), maxSize);
		return true;
	}
	else
		return false;
}

std::vector<std::string> XMLParam::listParams(const char* module_id)
{
	std::vector<std::string> list;
	
	std::map<std::pair<std::string, std::string>, std::string>::iterator iter;   
	for( iter = params.begin(); iter != params.end(); iter++ )
		list.push_back(iter->first.second);
		
	return list;
}

bool XMLParam::removeParam(const char* module_id, const char* key)
{
	std::map<std::pair<std::string, std::string>, std::string>::iterator iter;   
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
