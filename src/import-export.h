#ifndef _IMPORTEXPORT_H
#define _IMPORTEXPORT_H

#include <string>
#include <vector>

#include <ppasskeeper.h>

#include "xmlsp.h"

class ImportExport : public XMLSP::Parser
{
private:
	//We associate a key to its value
	std::map< ppk_entry*, ppk_data* > entries;
	
	virtual bool on_tag_open(const std::string&, XMLSP::StringMap&);
	
	static std::string encrypt(const ppk_data* edata);
	static std::string decrypt(const char* data);
	
	static bool flush(std::map<ppk_entry*, ppk_data*> &entries, std::string xml_path);
	
	static void free_entry_data(std::map<ppk_entry*, ppk_data*> &entries);
	
public:
	ImportExport();

	static ppk_error dumpModuleToFile(const char* module_id, const char* filepath);
	ppk_error importFromFileToModule(const char* module_id, const char* filepath);
};

#endif
