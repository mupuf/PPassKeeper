#ifndef PPK_DATA_H
#define PPK_DATA_H

#include <string.h>

///Data type
typedef enum
{
	///String
	ppk_string=1,

	///Binary
	ppk_blob=2
} ppk_data_type;

///Definition of what a blob is (basically just binary data)
typedef struct
{
	///A pointer
	const void *data;
	///And the size of the memory zone to store
	size_t size;
} ppk_data_blob;

///Definition of what a storable data is (Basically, binary or string data)
typedef struct
{
	///The type
	ppk_data_type type;
	union
	{
		///Basic text (UTF8)
		const char *string;
		///Binary value
		ppk_data_blob blob;
	};
} ppk_data;

#ifdef __cplusplus
extern "C"
{
#endif

	/*! \brief Generate a string ppk_data.
	* \param string in: The string to be stored (in Utf8 format, as every string in PPK)(example : MySecretPassword).
	* \return  Return the ppk_data corresponding to the parameters*/
	ppk_data* ppk_string_data_new(const char* string);

	/*! \brief Generate a blob ppk_data.
	* \param data in: The blob to be stored (example : 01110101011100101010101011010000101101111011010 (anything that's not human readable ;) ).
	* \return  Return the ppk_data corresponding to the parameters*/
	ppk_data* ppk_blob_data_new(const void* data, size_t size);

	void ppk_data_free(ppk_data* data);

#ifdef __cplusplus
}
#endif

#endif
