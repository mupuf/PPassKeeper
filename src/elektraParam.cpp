#include "elektraParam.h"
#include <sstream>
#include <fstream>
#include <iostream>

std::string ElektraParam::elektraKeyName(const char* module_id, const char* key)
{
	std::ostringstream keyName;
	keyName << "user/sw/ppasskeeper/" << module_id << '/' << key;
	return keyName.str();
}

#ifdef USE_ELEKTRA

	//libelektra
	#include <kdb.h>

	bool ElektraParam::saveParam(const char* module_id, const char* key, const char* value)
	{
		ckdb::KDB* handle;
		if ((handle = ckdb::kdbOpen()) == NULL)
		{
			perror("kdbOpen");
			return false;
		}

		ckdb::Key *k = ckdb::keyNew(elektraKeyName(module_id, key).c_str(), KEY_VALUE, value, KEY_END);

		bool r;
		if (k == NULL)
		{
			std::cerr << "Invalid parameter name" << std::endl;
			r = false;
		}
		else
		{
			int err = ckdb::kdbSetKey(handle, k);
			ckdb::keyDel(k);

			if (err == 0)
				r = true;
			else
			{
				perror("kbdSetKey");
				r = false;
			}
		}

		if (ckdb::kdbClose(handle) != 0)
			perror("kdbClose");
		return r;
	}

	bool ElektraParam::getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize)
	{
		ckdb::KDB* handle;
		if ((handle = ckdb::kdbOpen()) == NULL)
		{
			perror("kdbOpen");
			return false;
		}

		int size = ckdb::kdbGetString(handle, elektraKeyName(module_id, key).c_str(), returnedString, maxSize);

		//avoid potential buffer overflows
		returnedString[maxSize - 1] = '\0';

		bool r = (size > 0);

		if (ckdb::kdbClose(handle) != 0)
			perror("kdbClose");
		return r;
	}
#else
	bool ElektraParam::saveParam(const char* module_id, const char* key, const char* value)
	{
		return false;
	}

	bool ElektraParam::getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize)
	{
		return false;
	}
#endif
