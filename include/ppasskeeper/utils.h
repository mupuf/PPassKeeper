#ifndef PPK_UTILS_H
#define PPK_UTILS_H

#ifdef __cplusplus 
extern "C"
{
#endif
	/*! \brief Returns whether the library is locked, waiting for the right password, or not
	* \return PPK_TRUE is the library is locked, PPK_FALSE else */
	ppk_boolean ppk_is_locked();
	
	/*! \brief Lock the library with the given password.
	 * \param in pwd The password that will lock the library
	* \return PPK_TRUE is the library has been locked with the password, PPK_FALSE else */
	ppk_error ppk_set_password(const char* password);
	
	/*! \brief Try to unlock the library using the right password
	 * \param in password The password to try to unlock the library
	* \return PPK_TRUE is the library is unlocked, PPK_FALSE else */
	ppk_error ppk_unlock(const char* password);

	/*! \brief Get a string from an error code explaining the error code
	* \param error_code in: The error code you would like to be translated into human-readable text
	* \return  Return the string corresponding to the error code (should not be freed)*/
	const char *ppk_error_get_string(ppk_error error_code);

	/*! \brief Get ppk's settings directory
	* \return  Return the settings directory of ppk*/
	const char* ppk_settings_directory();

#ifdef __cplusplus 
}
#endif

#endif
