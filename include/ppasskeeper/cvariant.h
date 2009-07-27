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
	
	///CVariant
	typedef struct
	{
		///Real type of the CVariant
		cvariant_type type;
		int to_free;
		union
		{
			const char* string;
			int int_value;
			double float_value;
		};
	} cvariant;
	
	#define CVARIANT_EMPTY_STRING NULL
	#define CVARIANT_EMPTY_INT 0
	#define CVARIANT_EMPTY_FLOAT 0.0
	
	//Setters
	///Create an empty CVariant
	cvariant cvariant_null();
	
	///Create a CVariant from a given string
	cvariant cvariant_from_string(const char* string);
	
	///Create a CVariant from a given string (copy, dynamically allocated)
	cvariant cvariant_from_string_copy(const char* string, size_t n);
	
	///Create a CVariant from a given integer
	cvariant cvariant_from_int(int value);
	
	///Create a CVariant from a given float
	cvariant cvariant_from_float(double value);
	
	//Getters
	///returns a non-zero value if valid, 0 otherwise.
	int cvariant_not_null(cvariant cv);
	
	///Get the current type of a cvariant
	cvariant_type cvariant_get_type(cvariant cv);
	
	/*!
	 * Get a string from a cvariant. If the cvariant is not 
	 * a string, NULL is returned.
	 */
	const char* cvariant_get_string(cvariant cv);
	
	/*!
	 * Get an integer from a cvariant. If the cvariant is not 
	 * an integer, -1 is returned.
	 */
	int cvariant_get_int(cvariant cv);
	
	/*!
	 * Get a float from a cvariant. If the cvariant is not 
	 * a float, -1.0 is returned.
	 */
	double cvariant_get_float(cvariant cv);

	//Comparaison
	/*!
	 * Compare two cvariants. if a!=b, returns zero, otherwise, 
	 * return a non-zero value.
	 */
	int cvariant_compare(cvariant a, cvariant b);
	
	
	/*!
	 * Frees the memory allocated by the variant (if any).
	 */
	void cvariant_free(cvariant cv);

#ifdef __cplusplus 
}
#endif
#endif
