#ifdef __cplusplus
extern "C"
{
#endif
	int cryptToFile(const char* data, const char* filepath, const char* passphrase);
	int decryptFromFile(const char* filepath, char** data, const char* passphrase);
#ifdef __cplusplus
}
#endif



