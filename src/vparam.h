#ifndef _VPARAMS_H
#define _VPARAMS_H

#include <string>
#include <vector>
#include <ppasskeeper/cvariant.h>

class VParamImpl;

class VParam
{
private:
	VParamImpl &impl();
	VParam(); // unconstructible, use instance()

public:
	static VParam &instance();

	bool saveParam(const char* module_id, const char* key, const cvariant value);
	cvariant getParam(const char* module_id, const char* key);
	std::vector<std::string> listParams(const char* module_id);
	bool removeParam(const char* module_id, const char* key);
};

#endif
