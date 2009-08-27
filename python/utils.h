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
