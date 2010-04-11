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

#ifndef _VPARAMS_H
#define _VPARAMS_H

#include <string>
#include <vector>
#include <ppasskeeper/cvariant.h>

class VParamImpl;

class VParam
{
private:
	VParamImpl &impl();
	VParam(); // unconstructible, use instance()

public:
	static VParam &instance();

	bool saveParam(const char* module_id, const char* key, const cvariant value);
	cvariant getParam(const char* module_id, const char* key);
	std::vector<std::string> listParams(const char* module_id);
	bool removeParam(const char* module_id, const char* key);
};

#endif
