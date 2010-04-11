/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

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
