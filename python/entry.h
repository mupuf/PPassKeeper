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

#ifndef PPK_PYTHON_ENTRY_H
#define PPK_PYTHON_ENTRY_H

#include <boost/python.hpp>
using namespace boost::python;

#include <iostream>
#include <vector>

#include "../include/ppasskeeper.h"

struct Entry
{
	Entry() : m_entry(NULL) {}
	~Entry() { if (m_entry != NULL) ppk_entry_free(m_entry); }
	static Entry* create_network_entry(const char* protocol, const char* login, const char* host, unsigned short port)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_network_entry_new(protocol, login, host, port);
		return entry;
	}
	static Entry* create_application_entry(const char* username, const char* app_name)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_application_entry_new(username, app_name);
		return entry;
	}
	static Entry* create_item_entry(const char* item)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_item_entry_new(item);
		return entry;
	}
	static Entry* from_key(const char* key)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_entry_new_from_key(key);
		return entry;
	}
	std::string to_key() const
	{
		size_t len = ppk_key_length(m_entry);
		
		char* buf=new char[len];
		std::string ret=(ppk_get_key(m_entry, buf, len) == PPK_TRUE) ? buf : std::string();
		delete[] buf;
		
		return ret;
	}
	ppk_entry* m_entry;
};

std::ostream& operator<<(std::ostream& s, const Entry& e) { return s << e.to_key(); }

struct EntryList
{
	EntryList()	{}
	~EntryList() {
		for(std::vector<Entry*>::iterator it = entries.begin(); it != entries.end(); ++it)
			delete *it;
	}
	int length() { return entries.size(); }
	Entry* getitem(int index)
	{
		if (index < 0 || index >= length())
		{
			PyErr_SetObject(PyExc_IndexError, Py_None);
			throw error_already_set();
		}
		return entries[index];
	}
	bool contains(Entry* e)
	{
		for(std::vector<Entry*>::iterator it = entries.begin(); it != entries.end(); ++it)
			if (*it == e)
				return true;
		return false;
	}
	std::vector<Entry*> entries;
};

#endif
