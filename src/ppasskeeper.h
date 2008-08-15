#ifndef PPASSWORDKEEPER_H
#define PPASSWORDKEEPER_H
//

#ifdef __cplusplus 
extern "C"
{
#endif
	//Module's definition
	struct PPassKeeper_Module
	{
		const char* id;
		const char* display_name;
	};
	int getAvailableModulesCount(); //returns the number of modules
	int getAvailableModules(struct PPassKeeper_Module* modules, unsigned int nbModules); //returns the number of modules

	//Available functions for each module
	const char* getNetworkPassword(const char* module_id, const char* server, int port, const char* username);
	int setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd);
	const char* getApplicationPassword(const char* module_id, const char* application_name, const char* username);
	int setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd);
	const char* getItem(const char* module_id, const char* key);
	int setItem(const char* module_id, const char* key, const char* item);
	
	//Errors
	const char* getLastError(const char* module_id);
#ifdef __cplusplus 
}
#endif

//
#endif
