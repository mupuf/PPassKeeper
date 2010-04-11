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

#ifndef PPK_ENTRY_P_H
#define PPK_ENTRY_P_H

#include "entry.h"

//===----------------------------------------------------------------------===//
// Entry functions (private)
//===----------------------------------------------------------------------===//

#ifdef __cplusplus
extern "C"
{
#endif
	/* These functions fill the data inside a ppk_entry structure which is already allocated.
	   Not for general usage, see entry.h */
	void ppk_network_entry_fill(ppk_entry* entry, const char* protocol, const char* login, const char* host, unsigned short port);
	void ppk_application_entry_fill(ppk_entry* entry, const char* username, const char* app_name);
	void ppk_item_entry_fill(ppk_entry* entry, const char* item);
	void ppk_entry_free_contents(ppk_entry* entry);

#ifdef __cplusplus
}
#endif

#endif
