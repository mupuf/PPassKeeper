#ifndef _ELEKTRAPARAM_H
#define _ELEKTRAPARAM_H

#include "vparam.h"
#include <ppasskeeper/cvariant.h>

#include <string>

class ElektraParam: public VParam
{
	private:
		std::string elektraKeyName(const char* module_id, const char* key);
		
	public:
		bool saveParam(const char* module_id, const char* key, const cvariant value);
		cvariant getParam(const char* module_id, const char* key);
		std::vector<std::string> listParams(const char* module_id);
		bool removeParam(const char* module_id, const char* key);
};

#endif
