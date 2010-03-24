typedef enum {
	crypt_unknown_error=-2,
	crypt_file_cannot_be_oppenned=-1,
	crypt_invalid_key=0,
	crypt_ok=1
} crypt_error;

#ifdef __cplusplus
extern "C"
{
#endif
	char* getKeyFromPassphrase(const char* passphrase, const char* salt);
	void freeKeyFromPassphrase(char* key);
	crypt_error cryptToFileUsingPassphrase(const char* filepath, const char* data, const char* passphrase);
	crypt_error decryptFromFileUsingPassphrase(const char* filepath, char** data, const char* passphrase);
	crypt_error cryptToFile(const char* filepath, const char* data, const char* key);
	crypt_error decryptFromFile(const char* filepath, char** data, const char* key);
#ifdef __cplusplus
}
#endif



