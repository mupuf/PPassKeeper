#ifndef PPK_ASK_FOR_PASS_H
#define PPK_ASK_FOR_PASS_H

#ifdef __cplusplus 
extern "C"
{
#endif

	/*! \brief Allow to change the default password prompt message and replace it by your own.
	* \param module_id in: Module's ID.
	* \param customMessage in: Message to be used
	* \return  Return PPK_TRUE if the module is compatible with changing the prompt message, PPK_FALSE else.*/
	ppk_boolean ppk_afp_set_custom_prompt_message(const char* module_id, const char* customMessage);

#ifdef __cplusplus 
}
#endif

#endif
