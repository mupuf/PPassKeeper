#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <iostream>

//local functions
std::string shortName();
const char* getPassword(std::string filepath);
bool setPassword(std::string filepath, std::string secret);

//Personal portable functions
std::string dir();
bool mkdir(std::string path);

std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}
extern "C" const char* getModuleID();
void setError(std::string error)
{
	*(last_error())="PPK_"+toString(getModuleID())+" : " + error;
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
extern "C"
{
	const char* getModuleName();

	const int getABIVersion()
	{
		return 1;
	}

	//Non-Silent operations
	const char* getNetworkPassword(const char* server, int port, const char* username)
	{
		static std::string pwd=getPassword(generateNetworkPath(server, port, username));
		std::cout << "pass1=" << (int)pwd.c_str() << std::endl;
		return pwd.c_str();
	}

	const char* getNetworkPassword2(const char* server, int port, const char* username)
	{
		static std::string pwd=getPassword(generateNetworkPath(server, port, username));
		std::cout << "pass3=" << (int)pwd.c_str() << std::endl;
		return pwd.c_str();
	}

	int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd)
	{
		return setPassword(generateNetworkPath(server, port, username).c_str(), pwd)?0:-2;
	}

	const char* getApplicationPassword(const char* application_name, const char* username)
	{
		static std::string pwd=getPassword(generateApplicationPath(application_name, username));
		return pwd.c_str();
	}

	int setApplicationPassword(const char* application_name, const char* username, const char* pwd)
	{
		return setPassword(generateApplicationPath(application_name, username).c_str(), pwd)?0:-2;
	}

	const char* getItem(const char* key)
	{
		static std::string pwd=getPassword(generateItemPath(key));
		return pwd.c_str();
	}

	int setItem(const char* key, const char* item)
	{
		return setPassword(generateItemPath(key).c_str(), item)?0:-2;
	}

	//Silent operations
	const char* getNetworkPassword_silent(const char* server, int port, const char* username)
	{
		return getNetworkPassword(server, port, username);
	}

	int setNetworkPassword_silent(const char* server, int port, const char* username, const char* pwd)
	{
		return setNetworkPassword(server, port, username, pwd);
	}

	const char* getApplicationPassword_silent(const char* application_name, const char* username)
	{
		return getApplicationPassword(application_name, username);
	}

	int setApplicationPassword_silent(const char* application_name, const char* username, const char* pwd)
	{
		return setApplicationPassword(application_name, username, pwd);
	}

	const char* getItem_silent(const char* key)
	{
		return getItem(key);
	}

	int setItem_silent(const char* key, const char* item)
	{
		return setItem(key, item);
	}


	const char* getLastError()
	{
		return last_error()->c_str();
	}
}
