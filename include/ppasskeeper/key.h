#ifndef PPASSKEEPER_KEY_H
#define PPASSKEEPER_KEY_H

#include <ppasskeeper.h>

#define URL_PREFIX "ppasskeeper"

#ifdef __cplusplus
extern "C"
{
#endif

size_t ppk_key_length(const ppk_entry* entry);
ppk_boolean ppk_get_key(const ppk_entry* entry, char* returned_key, size_t max_key_size);
ppk_entry* ppk_entry_new_from_key(const char* key);

#ifdef __cplusplus
}
#endif

#endif //PPASSKEEPER_KEY_H
