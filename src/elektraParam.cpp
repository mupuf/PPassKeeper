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
	
	std::vector<std::string> ElektraParam::listParams(const char* module_id)
	{
		std::vector<std::string> list;
		
		ckdb::KeySet *myConfig=ckdb::ksNew(0);
		ckdb::KDB * handle=ckdb::kdbOpen();

		int rc = 0;
		int size = 0;

		/* Get all value keys for this application */
		if (ckdb::kdbGetByName(handle, myConfig, elektraKeyName(module_id,"").c_str(),  0) == -1)
		{
			perror("Couldn't get user configuration. Reason");
		} else {
			//size = (int)ksGetSize(myConfig);
			
			ckdb::Key *current;
			cursor_t cursor = ckdb::ksGetCursor (myConfig);
			ckdb::ksRewind (myConfig);
			while ((current = ckdb::ksNext (myConfig)) != 0)
				list.push_back((const char*)keyBaseName(current));
			ckdb::ksSetCursor(myConfig, cursor);
		}

		ckdb::kdbClose(handle);
		ckdb::ksDel(myConfig);
		
		return list;
	}
	
	bool ElektraParam::removeParam(const char* module_id, const char* key)
	{
		bool r;
		
		ckdb::KDB* handle;
		if ((handle = ckdb::kdbOpen()) == NULL)
		{
			perror("kdbOpen");
			return false;
		}
		
		int err=ckdb::kdbRemove(handle, elektraKeyName(module_id, key).c_str());
		if (err == 0)
			r = true;
		else
		{
			perror("kdbRemove");
			r = false;
		}

		if (ckdb::kdbClose(handle) != 0)
			perror("kdbClose");
			
		return r;
	}
	
#else
	bool ElektraParam::saveParam(const char* module_id, const char* key, const cvariant value)
	{
		return false;
	}

	cvariant ElektraParam::getParam(const char* module_id, const char* key)
	{
		return cvariant_null();
	}
	
	std::vector<std::string> ElektraParam::listParams(const char* module_id)
	{
		return std::vector<std::string>();
	}
	
	bool ElektraParam::removeParam(const char* module_id, const char* key)
	{
		return false;
	}
#endif
