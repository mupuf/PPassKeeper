#ifndef PPASSWORDKEEPER_H
#define PPASSWORDKEEPER_H
//

#ifdef __cplusplus 
extern "C"
{
	namespace ppk
	{
#endif
	//Module's definition
	struct PPassKeeper_Module
	{
		const char* id;
		const char* display_name;
	};
	int getAvailableModulesCount(); //returns the number of modules
	#ifdef __cplusplus 
		int getAvailableModules(ppk::PPassKeeper_Module* modules, unsigned int nbModules); //returns the number of modules
	#else
		int getAvailableModules(struct PPassKeeper_Module* modules, unsigned int nbModules); //returns the number of modules
	#endif

	//Available functions for each module
		//Non-Silent operations
		const char* getNetworkPassword(const char* module_id, const char* server, int port, const char* username);
		int setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd);
		const char* getApplicationPassword(const char* module_id, const char* application_name, const char* username);
		int setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd);
		const char* getItem(const char* module_id, const char* key);
		int setItem(const char* module_id, const char* key, const char* item);

		//Silent operations
		const char* getNetworkPassword_silent(const char* module_id, const char* server, int port, const char* username);
		int setNetworkPassword_silent(const char* module_id, const char* server, int port, const char* username,  const char* pwd);
		const char* getApplicationPassword_silent(const char* module_id, const char* application_name, const char* username);
		int setApplicationPassword_silent(const char* module_id, const char* application_name, const char* username,  const char* pwd);
		const char* getItem_silent(const char* module_id, const char* key);
		int setItem_silent(const char* module_id, const char* key, const char* item);
	
	//Errors
	const char* getLastError(const char* module_id);
#ifdef __cplusplus 
	}
}
#endif

//
#endif
