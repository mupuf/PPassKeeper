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

#include <ppasskeeper/data.h>

#include <assert.h>

#include "base64.h"

extern "C" ppk_data* ppk_string_data_new(const char* string)
{
	ppk_data* data = new ppk_data;
	size_t len;
	data->type=ppk_string;
	len = strlen(string);
	data->string = new char[len + 1];
	strcpy((char*)data->string, string);
	return data;
}

extern "C" ppk_data* ppk_blob_data_new(const void* data, size_t size)
{
	ppk_data* edata = new ppk_data;
	edata->type=ppk_blob;
	const_cast<ppk_data_blob*>(&(edata->blob))->data = new char[size];
	memcpy((char*)edata->blob.data, data, size);
	const_cast<ppk_data_blob*>(&(edata->blob))->size=size;
	return edata;
}

extern "C" ppk_data* ppk_blob_data_new_from_base64(const char* base64_data)
{
	int elen = ap_base64decode_len(base64_data, -1);
	char *out = new char[elen + 1];

	int len=ap_base64decode_binary((unsigned char *)out, base64_data, -1);
	ppk_data* data=ppk_blob_data_new(out, len);
	
	delete[] out;
	
	return data;
}

extern "C" char* ppk_blob_data_to_base64(const ppk_data* data)
{
	if(ppk_get_data_type(data)==ppk_blob)
	{
		int elen = ap_base64decode_len((const char*)data->blob.data, data->blob.size);
		char* out = new char[elen + 1];

		/*int len=*/ap_base64encode_binary(out, (const unsigned char*)data->blob.data, data->blob.size);
		return out;
	}
	else
		return NULL;
}

extern "C" void ppk_blob_base64_string_free(char* base64_data)
{
	if(base64_data!=NULL)
		delete[] base64_data;
}

extern "C" ppk_data_type ppk_get_data_type(const ppk_data* data)
{
	return data->type;
}

const char* ppk_get_data_string(const ppk_data* data)
{
	if(ppk_get_data_type(data)==ppk_string)
		return data->string;
	else
		return NULL;
}
	
const ppk_data_blob* ppk_get_data_blob(const ppk_data* data)
{
	if(ppk_get_data_type(data)==ppk_blob)
		return &(data->blob);
	else
		return NULL;
}

extern "C" void ppk_data_free(ppk_data* data)
{
	if (data == NULL)
		return;
	switch (data->type)
	{
	case ppk_string:
		delete[] data->string;
		break;
	case ppk_blob:
		delete[] (char*)data->blob.data;
		break;
	default:
		assert(false);
	};
	delete data;
}

