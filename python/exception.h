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

#ifndef PPK_PYTHON_EXCEPTION_H
#define PPK_PYTHON_EXCEPTION_H

#include <boost/python.hpp>
using namespace boost::python;

#include <exception>

#include "../include/ppasskeeper.h"

struct PPassKeeperError : std::exception
{
	PPassKeeperError(ppk_error err) { this->err = err; }
	ppk_error err;
	const char* what() const throw() { return ppk_error_get_string(err); }
};

void translate(const PPassKeeperError &e)
{
	PyErr_SetString(PyExc_RuntimeError, e.what());
}

#endif
