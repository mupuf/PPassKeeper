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
    if (b) passed++; else \
	{ \
		failed++; \
		std::cerr << "line " << __LINE__ << ": failure: " << #CONDITION << std::endl; \
		failure = true; \
		format_results(); \
		exit(1); \
	} \
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
