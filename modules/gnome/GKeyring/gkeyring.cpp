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

#include <ppasskeeper.h>
#include <ppasskeeper/entry_p.h>
#include "gkey_implement.h"
#include "tokenizer.h"
#include "base64.h"

#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <list>

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

std::string encrypt(const ppk_data* edata)
{
	std::string res;
	
	int size=ap_base64encode_len(edata->blob.size);
	char* buf=new char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64encode_binary(buf, (const unsigned char*)edata->blob.data, edata->blob.size);
		res.assign(buf, final_len);
		delete[] buf;
	}
	
	return res;
}

std::string decrypt(const char* data)
{
	std::string res;
	int len_data=strlen(data);
	
	int size=ap_base64decode_len((const char*)data, len_data);
	unsigned char* buf=new unsigned char[size+1];
	if(buf!=NULL)
	{
		int final_len=ap_base64decode_binary(buf, (const char*)data, len_data);
		res.assign((char*)buf, final_len);
		delete[] buf;
	}
	
	return res;
}

//Public functions
extern "C" ppk_boolean isWritable()
{
	return PPK_TRUE;
}

extern "C" ppk_security_level securityLevel(const char* module_id)
{
	return ppk_sec_safe;
}

//Get available flags
extern "C" unsigned int readFlagsAvailable()
{
	return ppk_rf_none|ppk_rf_silent;
}

extern "C" unsigned int writeFlagsAvailable()
{
	return ppk_wf_none|ppk_wf_silent;
}

extern "C" unsigned int listingFlagsAvailable()
{
	return ppk_lf_none|ppk_lf_silent;
}

//functions
extern "C" const char* getModuleID()
{
	return "GKeyring";
}

extern "C" const char* getModuleName()
{
	return "GKeyring - Store it into Gnome's Keyring";
}

extern "C" const int getABIVersion()
{
	return 1;
}

extern "C" ppk_error getSimpleEntryList(char*** list, unsigned int flags)
{
	return getItemList(list, flags);
}

extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	ppk_error res;
	ppk_data* tmp_data;
	
	if(entry->type == ppk_network)
		res=getNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, &tmp_data, flags);
	else if(entry->type == ppk_application)
		res=getApplicationPassword(entry->app.app_name, entry->app.username, &tmp_data, flags);
	else if(entry->type == ppk_item)
		res=getItem(entry->item, &tmp_data, flags);
	else
		res=PPK_UNKNOWN_ENTRY_TYPE;
		
	if(res==PPK_OK)
	{
		if(strncmp(tmp_data->string, STR_STRING, strlen(STR_STRING))==0)
			*edata=ppk_string_data_new(tmp_data->string+strlen(STR_STRING));
		else if(strncmp(tmp_data->string, BLOB_STRING, strlen(STR_STRING))==0)
		{
			std::string ret=decrypt(tmp_data->string+strlen(BLOB_STRING));
			*edata=ppk_blob_data_new(ret.data(), ret.size());
		}
		else
		{
			ppk_data_free(*edata);
			return PPK_UNKNOWN_ENTRY_TYPE;
		}
	}
	
	return res;
}

extern "C" ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
{
	static std::string data;
	ppk_data* edata_mod;
	
	if(edata->type==ppk_blob)
	{
		data=BLOB_STRING+encrypt(edata);
		edata_mod=ppk_blob_data_new(data.c_str(), data.size());
	}
	else if(edata->type==ppk_string)
	{
		data=STR_STRING+std::string(edata->string);
		edata_mod=ppk_string_data_new(data.c_str());
	}
	else
		return PPK_UNKNOWN_ENTRY_TYPE;

	ppk_error res = PPK_OK;
	if(entry->type == ppk_network)
		res=setNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, edata_mod, flags);
	else if(entry->type == ppk_application)
		res=setApplicationPassword(entry->app.app_name, entry->app.username, edata_mod, flags);
	else if(entry->type == ppk_item)
		res=setItem(entry->item, edata_mod, flags);

	ppk_data_free(edata_mod);

	return res;
}

extern "C" ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
{
	if(entry->type == ppk_network)
		return removeNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, flags);
	else if(entry->type == ppk_application)
		return removeApplicationPassword(entry->app.app_name, entry->app.username, flags);
	else if(entry->type == ppk_item)
		return removeItem(entry->item, flags);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
}

extern "C" ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
{
	ppk_data* edata;

	ppk_error res;
	if(entry->type == ppk_network)
		res=getNetworkPassword(entry->net.host, entry->net.login, entry->net.port, entry->net.protocol, &edata, flags);
	else if(entry->type == ppk_application)
		res=getApplicationPassword(entry->app.app_name, entry->app.username, &edata, flags);
	else if(entry->type == ppk_item)
		res=getItem(entry->item, &edata, flags);
	else
		return PPK_FALSE;

	return res==PPK_OK?PPK_TRUE:PPK_FALSE;
}

extern "C" unsigned int maxDataSize(ppk_data_type type)
{
	switch(type)
	{
		case ppk_string:
			return 7930;
		case ppk_blob:
			return 5926;
	}
	
	return 0;
}
