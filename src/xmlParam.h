#ifndef _XMLPARAMS_H
#define _XMLPARAMS_H

#include "xmlsp.h"
#include "vparam.h"
#include <ppasskeeper/cvariant.h>

#include <string>
#include <map>

class XMLParam: public XMLSP::Parser, public VParam
{
	private:
		std::string xml_path;
		
		//We associate <module, key> to <type, value>
		std::map< std::pair<std::string, std::string>, std::pair<int, std::string> > params;
		
		virtual bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes);
		virtual bool on_cdata(const std::string& cdata);
		virtual bool on_tag_close(const std::string& tag_name);
		virtual bool on_comment(const std::string& comment);
		virtual bool on_processing(const std::string& value);
		virtual bool on_doctype(const std::string& value);
		virtual bool on_document_begin();
		virtual bool on_document_end();
		virtual void on_error(int errnr, int line, int col, const std::string& message);
		
		bool flush();
		
	public:
		XMLParam(const std::string xmlPath);

		bool saveParam(const char* module_id, const char* key, const cvariant value);
		cvariant getParam(const char* module_id, const char* key);
		std::vector<std::string> listParams(const char* module_id);
		bool removeParam(const char* module_id, const char* key);
};

#endif
