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

#ifndef PPK_PYTHON_UTILS_H
#define PPK_PYTHON_UTILS_H

#include <boost/python.hpp>
using namespace boost::python;

#include "../include/ppasskeeper.h"

#include "exception.h"

bool is_locked() { return ppk_is_locked() == PPK_TRUE; }

void set_password(const char* password)
{
	ppk_error err = ppk_set_password(password);
	if (err != PPK_OK)
		throw PPassKeeperError(err);
}

void unlock(const char* password)
{
	ppk_error err = ppk_unlock(password);
	if (err != PPK_OK)
		throw PPassKeeperError(err);
}

#endif
