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
