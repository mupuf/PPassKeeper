#include <ppasskeeper/data.h>

ppk_data* ppk_string_data_new(const char* string)
{
	char *copy = strdup(string);
	if (copy == NULL)
		return NULL;

	ppk_data* data = new ppk_data;
	data->type = ppk_string;
	data->string = copy;
	return data;
}

/*
ppk_data* ppk_blob_data_new(const void* data, size_t size)
{
}
*/
