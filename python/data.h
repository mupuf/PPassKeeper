#ifndef PPK_PYTHON_DATA_H
#define PPK_PYTHON_DATA_H

#include <boost/python.hpp>
using namespace boost::python;

#include "../include/ppasskeeper.h"

struct Data
{
	Data() : m_data(NULL) {}
	~Data() { if (m_data != NULL) ppk_data_free(m_data); }
	static Data* create_string_data(const char* string)
	{
		Data* data = new Data;
		data->m_data = ppk_string_data_new(string);
		return data;
	}
	static Data* create_blob_data(const void* data_, size_t size)
	{
		Data* data = new Data;
		data->m_data = ppk_blob_data_new(data_, size);
		return data;
	}
	ppk_data* m_data;
};

#endif
