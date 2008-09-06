#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <iostream>

//Local definitions
std::string last_error;

//local functions
std::string shortName();
const char* getPassword(std::string filepath);
bool setPassword(std::string filepath, std::string secret);

//Personal portable functions
std::string dir();
bool mkdir(std::string path);

extern "C" const char* getModuleID();
void setError(std::string error)
{
	last_error="PPK_"+toString(getModuleID())+" : "+error; 
}


std::string generateNetworkPath(std::string server, int port, std::string username)
{
	return dir()+toString("/")+shortName()+"_"+username+toString("@")+server+toString(":")+toString(port);
}

std::string generateApplicationPath(std::string application_name, std::string username)
{
	return dir()+toString("/")+shortName()+"_"+username+toString("@")+application_name;
}

std::string generateItemPath(std::string key)
{
	return dir()+toString("/")+"_"+shortName()+"key-"+key;
}

//functions
extern "C" const char* getModuleName();

extern "C" const int getABIVersion()
{
	return 1;
}

extern "C" const char* getNetworkPassword(const char* server, int port, const char* username)
{
	return getPassword(generateNetworkPath(server, port, username).c_str());
}

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd)
{
	return setPassword(generateNetworkPath(server, port, username).c_str(), pwd)?0:-2;
}

extern "C" const char* getApplicationPassword(const char* application_name, const char* username)
{
	return getPassword(generateApplicationPath(application_name, username).c_str());
}

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd)
{
	return setPassword(generateApplicationPath(application_name, username).c_str(), pwd)?0:-2;
}

extern "C" const char* getItem(const char* key)
{
	return getPassword(generateItemPath(key).c_str());
}

extern "C" int setItem(const char* key, const char* item)
{
	return setPassword(generateItemPath(key).c_str(), item)?0:-2;
}

extern "C" const char* getLastError()
{
	return last_error.c_str();
}
