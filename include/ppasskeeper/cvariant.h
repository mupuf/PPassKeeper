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

/**
 * \file cvariant.h
 * \author MùPùF - Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 04-08-2009
 */

#ifndef _CVARIANT_H_
#define _CVARIANT_H_

#include <sys/types.h>

#ifdef __cplusplus 
extern "C"
{
#endif

	//Definitions
	///CVariant Types 
	typedef enum
	{
		///None
		cvariant_none=0,
		///String
		cvariant_string,
		///Int
		cvariant_int,
		///Float
		cvariant_float
	} cvariant_type;
	
	///CVariant (It is not meant to be read directly! Respect the abstract type and use the function we made!)
	typedef struct
	{
		///Real type of the CVariant
		cvariant_type type;
		///Should we free it or not ?
		int to_free;
		///The data
		union
		{
			const char* string;
			int int_value;
			double float_value;
		};
	} cvariant;
	
	///Private: The "emtpy" string
	#define CVARIANT_EMPTY_STRING NULL
	
	///Private: The "emtpy" int
	#define CVARIANT_EMPTY_INT 0
	
	///Private: The "emtpy" float
	#define CVARIANT_EMPTY_FLOAT 0.0
	
	//Setters
	/*! \brief Create an empty CVariant
	* \return Returns a null cvariant*/
	cvariant cvariant_null();
	
	/*! \brief Create a CVariant from a given string. 
	* WARNING: Do not free the pointer and still use the cvariant. If you want to do so, use cvariant_from_string_copy.
	* \param string The string to be stored into the cvariant
	* \return  Returns the corresponding cvariant.*/
	cvariant cvariant_from_string(const char* string);
	
	/*! \brief Create a CVariant from a given string (copy, dynamically allocated)
	* \param string The string to be copied and stored into the cvariant.
	* \param n The number of characters you want to copy. If n==0, then it will copy the whole string.
	* \return Returns the corresponding cvariant.*/
	cvariant cvariant_from_string_copy(const char* string, size_t n);
	
	/*! \brief Create a CVariant from a given integer
	* \param value The integer to be stored into the cvariant.
	* \return Returns the corresponding cvariant.*/
	cvariant cvariant_from_int(int value);
	
	/*! \brief Create a CVariant from a given 
	* \param value: The float to be stored into the cvariant.
	* \return Returns the corresponding cvariant.*/
	cvariant cvariant_from_float(double value);
	
	//Getters
	/*! \brief Test if a cvariant is null
	* \param cv The cvariant to be tested.
	* \return Returns a non-zero value if the cvariant is not null, 0 otherwise*/
	int cvariant_not_null(cvariant cv);
	
	/*! \brief Get the current type of a cvariant
	* \param cv The cvariant you want to get the type from.
	* \return Returns a the type*/
	cvariant_type cvariant_get_type(cvariant cv);
	
	/*! \brief Get a string from a cvariant
	* \param cv The cvariant you want to get the string from.
	* \return Returns a null-terminated string or CVARIANT_EMPTY_STRING if it is not a string.*/
	const char* cvariant_get_string(cvariant cv);
	
	/*! \brief Get an integer from a cvariant
	* \param cv The cvariant you want to get the integer from.
	* \return Returns the integer or CVARIANT_EMPTY_INTEGER if it is not an integer.*/
	int cvariant_get_int(cvariant cv);
	
	/*! \brief Get an float from a cvariant
	* \param cv The cvariant you want to get the float from.
	* \return Returns the float or CVARIANT_EMPTY_FLOAT if it is not an integer.*/
	double cvariant_get_float(cvariant cv);

	//Comparaison
	/*! \brief Compare two cvariants
	* \param a The first cvariant.
	* \param b The second cvariant.
	* \return Returns a non-zero value if a!=b, 0 otherwise.*/
	int cvariant_compare(cvariant a, cvariant b);
	
	/*! \brief Frees the memory allocated by the variant (if any).
	* \param cv The cvariant to be freed.*/
	void cvariant_free(cvariant cv);

#ifdef __cplusplus 
}
#endif
#endif
