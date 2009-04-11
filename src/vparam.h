#ifndef _VPARAMS_H
#define _VPARAMS_H

#include <string>

class VParam
{
	public:
		virtual bool saveParam(const char* module_id, const char* key, const char* value)=0;
		virtual bool getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize)=0;
};

#endif
