#ifndef PPK_DATA_H
#define PPK_DATA_H

/**
 * \file data.h
 * \author MùPùF - Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 27-07-2009
 */

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
		const ppk_data_blob blob;
	};
} ppk_data;

#ifdef __cplusplus
extern "C"
{
#endif

	/*! \brief Generate a string ppk_data.
	* \param string The string to be stored (in Utf8 format, as every string in PPK)(example : MySecretPassword).
	* \return  Return the ppk_data corresponding to the parameters*/
	ppk_data* ppk_string_data_new(const char* string);

	/*! \brief Generate a blob ppk_data.
	* \param data The blob to be stored (example : 01110101011100101010101011010000101101111011010 (anything that's not human readable ;) ).
	* \param size The blob size.
	* \return  Return the ppk_data corresponding to the parameters*/
	ppk_data* ppk_blob_data_new(const void* data, size_t size);
	
	/*! \brief Generate a blob ppk_data from a base64 string
	* \param base64_data The base64-encoded string to be stored as a blob.
	* \return  Return the ppk_data corresponding to the parameters*/
	ppk_data* ppk_blob_data_new_from_base64(const char* base64_data);
	
	/*! \brief Get a base64 string from a blob.
	* \param data The blob to get the base64-encoded string from.
	* \return  Return a base64-encoded string by the user or NULL if data is not a blob. The string returned must be freed using ppk_blob_data_to_base64()*/
	char* ppk_blob_data_to_base64(const ppk_data* data);
	
	/*! \brief Free a string returned by ppk_blob_data_to_base64
	* \param base64_data The blob string to be freed.*/
	void ppk_blob_base64_string_free(char* base64_data);
	
	/*! \brief Get the type of a data entry
	* \param data The ppk_data to get the type from.
	* \return  Return the ppk_data_type of data*/
	ppk_data_type ppk_get_data_type(const ppk_data* data);
	
	/*! \brief Get a string from the a data entry
	* \param data The ppk_data to get the string from.
	* \return  Return a string if data is a string, NULL otherwise*/
	const char* ppk_get_data_string(const ppk_data* data);
	
	/*! \brief Get a blob from the a data entry
	* \param data The ppk_data to get the blob from.
	* \return  Return a blob if data is a blob, NULL otherwise*/
	const ppk_data_blob* ppk_get_data_blob(const ppk_data* data);

	/*! \brief Frees a ppk_data.
	* \param data The ppk_data to be freed.*/
	void ppk_data_free(ppk_data* data);

#ifdef __cplusplus
}
#endif

#endif
