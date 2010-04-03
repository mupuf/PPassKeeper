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
		cvariant_string=1,
		///Int
		cvariant_int=2,
		///Float
		cvariant_float=3,
		///Boolean
		cvariant_boolean=4
		
	} cvariant_type;
	
	///What is a boolean ?
	typedef enum
	{
		cvariant_false=0,
		cvariant_true=1
	}cvariant_bool;
	
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
			cvariant_bool bool_value;
		};
	} cvariant;
	
	///Private: The "emtpy" string
	#define CVARIANT_EMPTY_STRING NULL
	
	///Private: The "emtpy" int
	#define CVARIANT_EMPTY_INT 0
	
	///Private: The "emtpy" float
	#define CVARIANT_EMPTY_FLOAT 0.0
	
	///Private: The "emtpy" boolean
	#define CVARIANT_EMPTY_BOOL cvariant_false
	
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
	
	/*! \brief Create a CVariant from a given float
	* \param value: The float to be stored into the cvariant.
	* \return Returns the corresponding cvariant.*/
	cvariant cvariant_from_float(double value);
	
	/*! \brief Create a CVariant from a given boolean
	* \param value: The bool to be stored into the cvariant.
	* \return Returns the corresponding cvariant.*/
	cvariant cvariant_from_bool(cvariant_bool value);
	
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
	
	/*! \brief Get a float from a cvariant
	* \param cv The cvariant you want to get the float from.
	* \return Returns the float or CVARIANT_EMPTY_FLOAT if it is not a float.*/
	double cvariant_get_float(cvariant cv);
	
	/*! \brief Get a boolean from a cvariant
	* \param cv The cvariant you want to get the bool from.
	* \return Returns the boolean or CVARIANT_EMPTY_BOOL if it is not a boolean.*/
	cvariant_bool cvariant_get_bool(cvariant cv);

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
