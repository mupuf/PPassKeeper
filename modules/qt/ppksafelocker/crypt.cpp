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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "crypt.h"
#include "sha512.h"

void sha512(char* hash, const void* data, unsigned int length)
{
	uint8_t sha512[SHA512_HASH_SIZE];
	SHA512Context s;

	SHA512Init(&s);
	SHA512Update(&s, data, length);
	SHA512Final(&s,sha512);

	for(int i = 0 ; i< SHA512_HASH_SIZE; i++)
		snprintf(hash+i*2, SHA512_HASH_SIZE+7, "%02x", sha512[i]);
}

char* getKeyFromPassphrase(const char* passphrase, const char* salt)
{
	//Salt the passphrase
	char* salted_pwd=(char*)malloc((strlen(salt)+strlen(passphrase)+1)*sizeof(char));
	sprintf(salted_pwd, "%s%s", salt, passphrase);

	//Generate the key
	char* key=(char*)malloc((SHA512_HASH_SIZE*2+1)*sizeof(char));

	sha512(key, salted_pwd, strlen(salted_pwd));
	for(int i=0; i<65000; i++)
		sha512(key, key, SHA512_HASH_SIZE*2);

	free(salted_pwd);

	return key;
}

void freeKeyFromPassphrase(char* key)
{
	if(key!=NULL)
		free(key);
}

#include <openssl/evp.h>
crypt_error do_crypt(const char* key, const unsigned char* in, const char* outpath)
{
	//Get the length of in
	int inlen=strlen((const char*)in)+1;

	// Allow enough space in output buffer for additional block
	unsigned char* outbuf=(unsigned char*)malloc(inlen + EVP_MAX_BLOCK_LENGTH);

	//Fixed IV
	const unsigned char iv[] = "12345678";

	//Set up the crypting method
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit(&ctx, EVP_bf_cbc(), (const unsigned char*)key, iv);

	//Encrypt the chain
	int outlen=0, tmplen;
	if(!EVP_EncryptUpdate(&ctx, outbuf, &tmplen, in, inlen))
	{
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return crypt_unknown_error;
	}
	outlen+=tmplen;

	if(!EVP_EncryptFinal(&ctx, outbuf+outlen, &tmplen))
	{
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return crypt_unknown_error;
	}
	outlen+=tmplen;

	EVP_CIPHER_CTX_cleanup(&ctx);

	//Save to disk
	FILE* outFile=fopen(outpath, "wb");
	if(outFile)
	{
		fwrite((char*)outbuf, outlen, sizeof(char), outFile);
		fclose(outFile);
	}
	else
	{
		fprintf(stderr, "The file '%s' cannot be openned for writing purpose. Aborting.\n", outpath);
		return crypt_file_cannot_be_oppenned;
	}

	//Free buffers
	free(outbuf);

	return crypt_ok;
}

crypt_error do_decrypt(const char* key, const char* inpath, unsigned char** out)
{
	//Set out to NULL in case we get an error
	*out=NULL;

	//Open the file
	FILE* in=fopen(inpath, "rb");
	if(!in)
	{
		fprintf(stderr, "The file '%s' cannot be openned for reading purpose. Aborting.\n", inpath);
		return crypt_file_cannot_be_oppenned;
	}

	//Get the file's length
	int pos, end, inlen;
	pos = ftell (in);
	fseek (in, 0, SEEK_END);
	end = ftell (in);
	fseek (in, pos, SEEK_SET);
	inlen=end-pos;

	//Allow enough space in output buffer for additional block
	*out=(unsigned char*)malloc(inlen*sizeof(char));
	int outpos=0;

	//Bogus IV
	unsigned char iv[] = "12345678";

	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_DecryptInit(&ctx, EVP_bf_cbc(), (const unsigned char*)key, iv);

	unsigned char inbuf[1024];
	int outlen;
	for(;;)
	{
		inlen = fread((char*)inbuf, 1, sizeof(inbuf), in);
		if(inlen <= 0) break;
		if(!EVP_DecryptUpdate(&ctx, (*out)+outpos, &outlen, inbuf, inlen))
		{
			// Error
			EVP_CIPHER_CTX_cleanup(&ctx);

			//Free the output buffer
			free(*out);
			*out=NULL;

			return crypt_unknown_error;
		}
		outpos+=outlen;
	}

	if(!EVP_DecryptFinal(&ctx, (*out)+outpos, &outlen))
	{
		// Error
		EVP_CIPHER_CTX_cleanup(&ctx);

		//Free the output buffer
		free(*out);
		*out=NULL;

		return crypt_invalid_key;
	}
	outpos+=outlen;

	EVP_CIPHER_CTX_cleanup(&ctx);

	//Close the file
	fclose(in);

	return crypt_ok;
}

extern "C" crypt_error cryptToFileUsingPassphrase(const char* filepath, const char* data, const char* passphrase)
{
	//Create a key from the passphrase
	char* key=getKeyFromPassphrase(passphrase, "ppk");

	//Encrypt the file
	crypt_error ret=do_crypt(key, (const unsigned char*)data, filepath);

	//Free the key
	free(key);

	return ret;
}

extern "C" crypt_error decryptFromFileUsingPassphrase(const char* filepath, char** data, const char* passphrase)
{
	//Create a key from the passphrase
	char* key=getKeyFromPassphrase(passphrase, "ppk");

	//Decrypt the file
	crypt_error ret=do_decrypt(key, filepath, (unsigned char**)data);

	//Free the key
	free(key);

	return ret;
}

extern "C" crypt_error cryptToFile(const char* filepath, const char* data, const char* key)
{
	return do_crypt(key, (const unsigned char*)data, filepath);
}

extern "C" crypt_error decryptFromFile(const char* filepath, char** data, const char* key)
{
	//Decrypt the file
	return do_decrypt(key, filepath, (unsigned char**)data);
}
