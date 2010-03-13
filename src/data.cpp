#include <ppasskeeper/data.h>

#include <assert.h>

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
	size_t len;
	edata->type=ppk_blob;
	const_cast<ppk_data_blob*>(&(edata->blob))->data = new char[size];
	memcpy((char*)edata->blob.data, data, size);
	const_cast<ppk_data_blob*>(&(edata->blob))->size=size;
	return edata;
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

