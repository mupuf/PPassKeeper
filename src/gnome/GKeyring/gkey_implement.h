#ifndef _PPK_GKEY_IMPLEMENT_H_
#define _PPK_GKEY_IMPLEMENT_H_

#include "../../ppasskeeper.h"

extern "C" ppk_boolean setNetworkPassword(const char* host, const char* login, unsigned short port, const ppk_data edata, unsigned int flags);
extern "C" ppk_boolean getNetworkPassword(const char* host, const char* login, unsigned short port, ppk_data *edata, unsigned int flags);

extern "C" ppk_boolean setApplicationPassword(const char* appName, const char* user, const ppk_data edata, unsigned int flags);
extern "C" ppk_boolean getApplicationPassword(const char* appName, const char* user, ppk_data *edata, unsigned int flags);

extern "C" ppk_boolean setItem(const char* item, const ppk_data edata, unsigned int flags);
extern "C" ppk_boolean getItem(const char* item, ppk_data *edata, unsigned int flags);

extern "C" ppk_boolean removeNetworkPassword(const char* host, const char* login, unsigned short port, unsigned int flags);
extern "C" ppk_boolean removeApplicationPassword(const char* appName, const char* user, unsigned int flags);
extern "C" ppk_boolean removeItem(const char* item, unsigned int flags);

extern "C" char** getItemList(unsigned int flags);

extern "C" void constructor(void);
extern "C" void destructor(void);

#endif
