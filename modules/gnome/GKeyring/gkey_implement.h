#ifndef _PPK_GKEY_IMPLEMENT_H_
#define _PPK_GKEY_IMPLEMENT_H_

#include <ppasskeeper.h>

extern "C" const char* getModuleID();

extern "C" ppk_error setNetworkPassword(const char* host, const char* login, unsigned short port, const char* protocol, const ppk_data* edata, unsigned int flags);
extern "C" ppk_error getNetworkPassword(const char* host, const char* login, unsigned short port, const char* protocol, ppk_data** edata, unsigned int flags);

extern "C" ppk_error setApplicationPassword(const char* appName, const char* user, const ppk_data* edata, unsigned int flags);
extern "C" ppk_error getApplicationPassword(const char* appName, const char* user, ppk_data** edata, unsigned int flags);

extern "C" ppk_error setItem(const char* item, const ppk_data* edata, unsigned int flags);
extern "C" ppk_error getItem(const char* item, ppk_data** edata, unsigned int flags);

extern "C" ppk_error removeNetworkPassword(const char* host, const char* login, unsigned short port, const char* protocol, unsigned int flags);
extern "C" ppk_error removeApplicationPassword(const char* appName, const char* user, unsigned int flags);
extern "C" ppk_error removeItem(const char* item, unsigned int flags);

extern "C" char** getItemList(unsigned int flags);

extern "C" void constructor(void);
extern "C" void destructor(void);

#endif
