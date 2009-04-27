#include "cvariant.h"

//Setters
///Create an empty CVariant
cvariant cvariant_null()
{
	cvariant cv;
	cv.type=cvariant_none;
	return cv;
}

///Create a CVariant from a given string
cvariant cvariant_from_string(const char* string)
{
	cvariant cv;
	cv.type=cvariant_string;
	cv.string=string;
	return cv;
}

///Create a CVariant from a given integer
cvariant cvariant_from_int(int value)
{
	cvariant cv;
	cv.type=cvariant_int;
	cv.int_value=value;
	return cv;
}

///Create a CVariant from a given float
cvariant cvariant_from_float(double value)
{
	cvariant cv;
	cv.type=cvariant_float;
	cv.float_value=value;
	return cv;
}

//Getters
///returns a non-zero value if valid, 0 otherwise.
int cvariant_not_null(cvariant cv)
{
	return cv.type!=cvariant_none;
}

///Get the current type of a cvariant
cvariant_type cvariant_get_type(cvariant cv)
{
	return cv.type;
}

/*!
 * Get a string from a cvariant. If the cvariant is not 
 * a string, NULL is returned.
 */
const char* cvariant_get_string(cvariant cv)
{
	if(cv.type==cvariant_string)
		return cv.string;
	else
		return 0;
}

/*!
 * Get an integer from a cvariant. If the cvariant is not 
 * an integer, -1 is returned.
 */
int cvariant_get_int(cvariant cv)
{
	if(cv.type==cvariant_int)
		return cv.int_value;
	else
		return -1;
}

/*!
 * Get a float from a cvariant. If the cvariant is not 
 * a float, -1.0 is returned.
 */
double cvariant_get_float(cvariant cv)
{
	if(cv.type==cvariant_float)
		return cv.float_value;
	else
		return -1.0;
}
