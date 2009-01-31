#ifndef _PPK_GKEY_IMPLEMENT_H_
#define _PPK_GKEY_IMPLEMENT_H_

#include "../../ppasskeeper.h"

extern "C" int setNetworkPassword(const char* host, const char* login, unsigned short port, const ppk_data edata, unsigned int flags);
extern "C" int getNetworkPassword(const char* host, const char* login, unsigned short port, ppk_data *edata, unsigned int flags);

extern "C" int setApplicationPassword(const char* appName, const char* user, const ppk_data edata, unsigned int flags);
extern "C" int getApplicationPassword(const char* appName, const char* user, ppk_data *edata, unsigned int flags);

extern "C" int setItem(const char* item, const ppk_data edata, unsigned int flags);
extern "C" int getItem(const char* item, ppk_data *edata, unsigned int flags);


#endif
