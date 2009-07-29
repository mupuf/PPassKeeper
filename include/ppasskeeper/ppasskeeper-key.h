#ifndef PPASSKEEPER_KEY_H
#define PPASSKEEPER_KEY_H

#include <ppasskeeper.h>

#define URL_PREFIX "ppasskeeper"

size_t ppk_keyLength(const ppk_entry* entry);
ppk_boolean ppk_getKey(const ppk_entry* entry, char* returned_key, size_t max_key_size);
ppk_boolean ppk_getEntryFromKey(ppk_entry* entry, const char* key);

#endif //PPASSKEEPER_KEY_H
