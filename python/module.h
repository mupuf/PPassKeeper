#ifndef PPK_PYTHON_MODULE_H
#define PPK_PYTHON_MODULE_H

#include <boost/python.hpp>
using namespace boost::python;

#include "../include/ppasskeeper.h"

#include "exception.h"

class ModuleList
{
public:
	ModuleList()
		: m_length(-1)
	{
		m_list = ppk_module_list_new();
		if (m_list == NULL)
			throw PPassKeeperError(PPK_LOCKED_NO_ACCESS);
	}
	~ModuleList() { ppk_module_list_free(m_list); }
	int length()
	{
		if (m_length != -1)
			return m_length;
		if (m_list == NULL)
			m_length = 0;
		else
		{
			m_length = 0;
			while (m_list[m_length] != NULL)
				m_length++;
		}
		return m_length;
	};

	const char* getitem(int index)
	{
		if (index < 0 || index >= length())
		{
			PyErr_SetObject(PyExc_IndexError, Py_None);
			throw error_already_set();
		}
		return m_list[index];
	}
private:
	char** m_list;
	int m_length;
};

#endif
