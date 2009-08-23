#ifndef PPK_UTILS_H
#define PPK_UTILS_H

/**
 * \file utils.h
 * \author MùPùF - Martin Peres (martin<dot>peres<At>ensi-bourges<dot>fr)
 * \date 27-07-2009
 */

#ifdef __cplusplus 
extern "C"
{
#endif
	/*! \brief Returns whether the library is locked, waiting for the right password, or not
	* \return PPK_TRUE is the library is locked, PPK_FALSE else */
	ppk_boolean ppk_is_locked();
	
	/*! \brief Lock the library with the given password.
	 * \param pwd The password that will lock the library
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_set_password(const char* password);
	
	/*! \brief Try to unlock the library using the right password
	 * \param password The password to try to unlock the library
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_unlock(const char* password);

	/*! \brief Get a string from an error code explaining the error code
	* \param error_code The error code you would like to be translated into human-readable text
	* \return  Return the string corresponding to the error code (should not be freed)*/
	const char *ppk_error_get_string(ppk_error error_code);

	/*! \brief Get ppk's settings directory
	* \return  Return the settings directory of ppk*/
	const char* ppk_settings_directory();

#ifdef __cplusplus 
}
#endif

#endif
