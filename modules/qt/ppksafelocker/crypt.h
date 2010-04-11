/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

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



