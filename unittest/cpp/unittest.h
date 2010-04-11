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

/**
 * \file unittest.h
 * \author Denis Martinez
 * \date 2009-09-06
 */

#ifndef UNITTEST_H
#define UNITTEST_H

#include <cstdlib>
#include <iostream>

/*
 * Function implemented by C++ tests
 */
void run(int argc, char** argv);

/* Testing macros */
#define ASSERT(CONDITION) \
{ \
	bool b = (CONDITION); \
    if (b) passed++; else \
	{ \
		failed++; \
		std::cerr << "line " << __LINE__ << ": assertion failed: " << #CONDITION << std::endl; \
	} \
}
#define FAILIF(CONDITION) \
{ \
	bool b = (CONDITION); \
    if (b) \
	{ \
		failed++; \
		std::cerr << "line " << __LINE__ << ": failure: " << #CONDITION << std::endl; \
		failure = true; \
		format_results(); \
		exit(1); \
	} else passed++;							\
}

/* Main program */
int passed;
int failed;
bool failure;

void format_results()
{
	std::cout << "{'passed':" << passed
			  << ",'failed':" << failed
			  << ",'failure':" << (failure ? "True" : "False") << "}"
			  << std::endl;
}

int main(int argc, char** argv)
{
	passed = 0;
	failed = 0;
	failure = false;
	run(argc, argv);
	format_results();
	return 0;
};

#endif // UNITTEST_H
