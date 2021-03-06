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

#ifndef PPK_PYTHON_MODULE_H
#define PPK_PYTHON_MODULE_H

#include <boost/python.hpp>
using namespace boost::python;

#include "../include/ppasskeeper.h"

#include "exception.h"
#include "entry.h"
#include "data.h"

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
	bool has_entry(const Entry& e, unsigned int flags)
	{
		ppk_boolean result;
		ppk_error err = ppk_module_has_entry(id.c_str(), e.m_entry, flags, &result);
		if (err == PPK_OK) return result == PPK_TRUE;
		throw PPassKeeperError(err);
	}
	void remove_entry(const Entry& e, unsigned int flags)
	{
		ppk_error err = ppk_module_remove_entry(id.c_str(), e.m_entry, flags);
		if (err != PPK_OK)
			throw PPassKeeperError(err);
	}
	Data* get_entry(const Entry& e, unsigned int flags)
	{
		Data* d = new Data;
		ppk_error err = ppk_module_get_entry(id.c_str(), e.m_entry, &(d->m_data), flags);
		if (err != PPK_OK)
		{
			delete d;
			throw PPassKeeperError(err);
		}
		else
			return d;
	}
	void set_entry(const Entry& e, const Data& d, unsigned int flags)
	{
		ppk_error err = ppk_module_set_entry(id.c_str(), e.m_entry, d.m_data, flags);
		if (err != PPK_OK)
			throw PPassKeeperError(err);
	}
	EntryList* get_entry_list(int entry_types, unsigned int flags)
	{
		ppk_entry **list;
		size_t count;
		ppk_error err = ppk_module_get_entry_list(id.c_str(), entry_types, &list, &count, flags);
		if (err != PPK_OK)
			throw PPassKeeperError(err);
		EntryList* elist = new EntryList;
		for (size_t i = 0; i < count; ++i)
		{
			Entry* entry = new Entry;
			entry->m_entry = list[i];
			elist->entries.push_back(entry);
		}
		return elist;
	}
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
