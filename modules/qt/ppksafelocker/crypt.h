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
	crypt_error cryptToFile(const char* filepath, const char* data, const char* passphrase);
	crypt_error decryptFromFile(const char* filepath, char** data, const char* passphrase);
#ifdef __cplusplus
}
#endif



