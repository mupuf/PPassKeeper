#ifndef _VPARAMS_H
#define _VPARAMS_H

#include <string>
#include <vector>
#include "cvariant.h"

class VParam
{
	public:
		virtual bool saveParam(const char* module_id, const char* key, const cvariant value)=0;
		virtual cvariant getParam(const char* module_id, const char* key)=0;
		virtual std::vector<std::string> listParams(const char* module_id)=0;
		virtual bool removeParam(const char* module_id, const char* key)=0;
};

#endif
