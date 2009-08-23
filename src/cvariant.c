/*
CVariant 1.0
Copyright (C) 2009  Martin Peres <martin dot peres at ensi-bourges dot fr
Contributors: Denis Martinez

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

#include <ppasskeeper/cvariant.h>

#include <string.h>
#include <stdlib.h>

//Setters
///Create an empty CVariant
cvariant cvariant_null()
{
	cvariant cv;
	cv.type=cvariant_none;
	cv.to_free=0;
	return cv;
}

///Create a CVariant from a given string
cvariant cvariant_from_string(const char* string)
{
	cvariant cv;
	cv.type=cvariant_string;
	cv.string=string;
	cv.to_free=0;
	return cv;
}

///Create a CVariant from a given string (copy, dynamically allocated)
cvariant cvariant_from_string_copy(const char* string, size_t n)
{
	if(n==0)
		n=strlen(string);
	
	cvariant cv;
	cv.type = cvariant_string;
	char *copy = (char *) malloc(n + 1);
	strncpy(copy, string, n);
	// cv.string may or may not be NULL-terminated, so we add an extra '\0'
	copy[n] = '\0';
	cv.string = copy;
	cv.to_free=1;
	return cv;
}

///Create a CVariant from a given integer
cvariant cvariant_from_int(int value)
{
	cvariant cv;
	cv.type=cvariant_int;
	cv.int_value=value;
	cv.to_free=0;
	return cv;
}

///Create a CVariant from a given float
cvariant cvariant_from_float(double value)
{
	cvariant cv;
	cv.type=cvariant_float;
	cv.float_value=value;
	cv.to_free=0;
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
		return CVARIANT_EMPTY_STRING;
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
		return CVARIANT_EMPTY_INT;
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
		return CVARIANT_EMPTY_FLOAT;
}

//Comparaison
/*!
* Compare two cvariants. if a!=b, returns zero, otherwise, 
* return a non-zero value.
*/
int cvariant_compare(cvariant a, cvariant b)
{
	if(a.type==b.type)
	{
		if(a.type==cvariant_int)
			return a.int_value-b.int_value;
		else if(a.type==cvariant_float)
			return a.float_value-b.float_value;
		else if(a.type==cvariant_string)
			return strcmp(a.string, b.string);
	}
	else
		return 0;
}

void cvariant_free(cvariant cv)
{
	if (cv.type == cvariant_string && cv.to_free)
		free((char *) cv.string);
}

