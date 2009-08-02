#ifndef PPASSKEEPER_KEY_H
#define PPASSKEEPER_KEY_H

#include <ppasskeeper.h>

#define URL_PREFIX "ppasskeeper"

size_t ppk_key_length(const ppk_entry* entry);
ppk_boolean ppk_get_key(const ppk_entry* entry, char* returned_key, size_t max_key_size);
ppk_boolean ppk_get_entry_from_key(ppk_entry* entry, const char* key);

#endif //PPASSKEEPER_KEY_H
