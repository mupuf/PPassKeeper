#include <ppasskeeper.h>
#include <string>
#include <iostream>

#include "../errors.h"

void die(unsigned int error_num, const std::string error)
{
	std::cerr << "Fatal Error : " << error << std::endl;
	exit(error_num);
}

std::string readFile(std::string path)
{
	//open the file
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		//Read the password from the file
		std::ostringstream pwd_s;
		pwd_s << inputfile.rdbuf();

		//close the file
		inputfile.close();
		
		return pwd_s.str();
	}
	else
		return std::string();
}

int main(int argc, char** argv)
{
	if(ppk_isLocked()==PPK_TRUE)
		die(UTST_LIB_LOCKED, "The library is locked, please reset the password in order to let the test run !");
	
	std::string file=readFile(argv[0]);
	if(file!=std::string())
	{
		
	}
	else
		die(
	
	return 0;
}
