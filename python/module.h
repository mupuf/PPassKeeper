#ifndef PPK_PYTHON_MODULE_H
#define PPK_PYTHON_MODULE_H

#include <boost/python.hpp>
using namespace boost::python;

#include "../include/ppasskeeper.h"

#include "exception.h"

#include <iostream>

struct Module
{
	Module(const std::string& id) : id(id) {}
	std::string id;

	template <ppk_error (*func)(const char* module_id, unsigned int* flags)>
	unsigned int get_flags()
	{
		unsigned int flags;
		ppk_error err = func(id.c_str(), &flags);
		if (err != PPK_OK)
			throw PPassKeeperError(err);
		return flags;
	}
	unsigned int read_flags() { return get_flags<ppk_module_read_flags>(); }
	unsigned int write_flags() { return get_flags<ppk_module_write_flags>(); }
	unsigned int listing_flags() { return get_flags<ppk_module_listing_flags>(); }
	const char* display_name() { return ppk_module_display_name(id.c_str()); }
	ppk_security_level security_level() { return ppk_module_security_level(id.c_str()); }
	bool is_writable() { return ppk_module_is_writable(id.c_str()); }
	size_t max_data_size(ppk_data_type type) { return ppk_module_max_data_size(id.c_str(), type); }
	size_t get_entry_count(int entry_types, unsigned int flags) { return ppk_module_get_entry_count(id.c_str(), entry_types, flags); }
	static Module* get_default()
	{
		const char* id = ppk_module_get_default();
		if (id == NULL) return NULL;
		else return new Module(id);
	}
	static void set_default(const Module& module)
	{
		ppk_error err = ppk_module_set_default(module.id.c_str());
		if (err != PPK_OK)
			throw PPassKeeperError(err);
	}
};
std::ostream& operator<<(std::ostream& s, const Module& m) { return s << m.id; }

struct ModuleList
{
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
		else
		{
			m_length = 0;
			while (m_list[m_length] != NULL)
				m_length++;
		}
		return m_length;
	};

	Module *getitem(int index)
	{
		if (index < 0 || index >= length())
		{
			PyErr_SetObject(PyExc_IndexError, Py_None);
			throw error_already_set();
		}
		char* id = m_list[index];
		return new Module(id);
	}

	bool contains(const std::string& id)
	{
		for (char** cur = m_list; *cur != NULL; ++cur)
			if (id == *cur)
				return true;
		return false;
	}

	char** m_list;
	int m_length;
};

#endif
