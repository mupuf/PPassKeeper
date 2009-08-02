#define UTST_OK 0
#define UTST_LIB_LOCKED 1
#define UTST_CANNOT_OPEN_FILE 2
#define UTST_INVALID_MODULE_NAME 3
#define UTST_WRITING_ERROR 4
#define UTST_READING_ERROR 5
#define UTST_INVALID_ENTRY_TYPE 6
#define UTST_INVALID_DATA 7
#define UTST_NO_BLOB_SUPPORT 8
#define UTST_NO_STRING_SUPPORT 9

const char* utst_msg[] = {
	"The Unit Test ran fine !", //UTST_OK
	"The library is locked",	//UTST_LIB_LOCKED
	"A file cannot be oppened", //UTST_CANNOT_OPEN_FILE
	"Invalid module name", //UTST_INVALID_MODULE_NAME
	"An error occured while writing the password", //UTST_WRITING_ERROR
	"An error occured while reading the password", //UTST_READING_ERROR
	"The entry type is invalid", //UTST_INVALID_ENTRY_TYPE
	"The retrieved data is invalid", //UTST_INVALID_DATA
	"The module doesn't support blobs", //UTST_NO_BLOB_SUPPORT
	"The module doesn't support strings" //UTST_NO_STRING_SUPPORT
};

#ifdef __cplusplus
extern "C"
{
#endif
	void die(unsigned int error_num, const char* error=NULL);
#ifdef __cplusplus
}
#endif
