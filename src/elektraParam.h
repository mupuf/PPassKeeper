#ifndef _ELEKTRAPARAM_H
#define _ELEKTRAPARAM_H

#include "vparam.h"

#include <string>

class ElektraParam: public VParam
{
	private:
		std::string elektraKeyName(const char* module_id, const char* key);
		
	public:
		bool saveParam(const char* module_id, const char* key, const char* value);
		bool getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize);
};

#endif
