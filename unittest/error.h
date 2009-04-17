#define UTST_OK 0
#define UTST_LIB_LOCKED 1
#define UTST_CANNOT_OPEN_FILE 2

const char** utst_msg = {
	"The Unit Test ran fine !", //UTST_OK
	"The library is locked",	//UTST_LIB_LOCKED
	"A file cannot be oppened" //UTST_CANNOT_OPEN_FILE
}
