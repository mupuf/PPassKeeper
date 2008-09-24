#include "../../ppasskeeper-module.h"
#include <vector>
#include <string>

class ListPwd
{
	private:
		struct networkList{std::string host; std::string user; unsigned short int port;};
		struct appList{std::string app; std::string user;};
		struct itemList{std::string key;};

		std::vector<networkList> listNet;
		std::vector<appList> listApp;
		std::vector<itemList> listItem;

		bool addNetworkPassword(std::string stripped_name);
		bool addAppPassword(std::string stripped_name);
		bool addItemPassword(std::string stripped_name);
		bool parseFileName(std::string prefix, std::string filename, ppk_password_type type);
		unsigned int updateDataBase(const char* dir, const char* prefix, ppk_password_type type);

		unsigned int copyDBToPwdList(ppk_password_type type, void* pwdList, unsigned int maxPasswordCount);
		unsigned int copyNetworkToPwdList(void* pwdList, unsigned int maxPasswordCount);
		unsigned int copyApplicationToPwdList(void* pwdList, unsigned int maxPasswordCount);
		unsigned int copyItemToPwdList(void* pwdList, unsigned int maxPasswordCount);
	public:
		unsigned int getPasswordListCount(const char* dir, const char* prefix, ppk_password_type type);
		unsigned int getPasswordList(const char* dir, const char* prefix, ppk_password_type type, void* pwdList, unsigned int maxPasswordCount);
};
