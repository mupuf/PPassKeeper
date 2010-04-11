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

extern "C" ppk_error getItemList(char*** list, unsigned int flags);

extern "C" void constructor(void);
extern "C" void destructor(void);

#endif
