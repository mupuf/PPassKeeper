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
	static Data* create_blob_data(const char* data_, size_t size)
	{
		Data* data = new Data;
		data->m_data = ppk_blob_data_new(data_, size);
		return data;
	}
	ppk_data* m_data;
};

std::ostream& operator<<(std::ostream& s, const Data& d)
{
	switch (d.m_data->type)
	{
	case ppk_string:
		return s << d.m_data->string;
	case ppk_blob:
		return s.write(static_cast<const char*>(d.m_data->blob.data), d.m_data->blob.size);
	}
	
	return s;
}

#endif
