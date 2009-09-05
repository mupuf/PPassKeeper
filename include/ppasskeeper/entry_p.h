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
