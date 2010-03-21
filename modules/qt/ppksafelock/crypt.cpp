#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

char* createKey(const char* passphrase, const char* salt)
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

#include <openssl/evp.h>
/*int encrypt (const unsigned char* key, const unsigned char* iv, int infd, int outfd)
{
	unsigned char outbuf[10];
	int olen, tlen, n;
	char inbuff[8];
	
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit(&ctx, EVP_bf_cbc(), key, iv);

	while(true)
	{
		bzero (&inbuff, IP_SIZE);

		if ((n = read (infd, inbuff, IP_SIZE)) == -1)
		{
			perror ("read error");
			break;
		}
		else if (n == 0)
			break;

		if (EVP_EncryptUpdate(&ctx, outbuf, &olen, inbuff, n) != 1)
		{
			printf ("error in encrypt update\n");
			return 0;
		}

		if (EVP_EncryptFinal(&ctx, outbuf+olen, &tlen) != 1)
		{
			printf ("error in encrypt final\n");
			return 0;
		}
		
		olen += tlen;
		if ((n = write (outfd, outbuf, olen)) == -1)
			perror ("write error");
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
	
	return 1;
}*/

int do_crypt(FILE *in, FILE *out, int do_encrypt)
{
	/* Allow enough space in output buffer for additional block */
	inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
	int inlen, outlen;
	/* Bogus key and IV: we'd normally set these from
		* another source.
		*/
	unsigned char key[] = "0123456789";
	unsigned char iv[] = "12345678";
	
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit(&ctx, EVP_bf_cbc(), key, iv);
	
	for(;;)
	{
		inlen = fread(inbuf, 1, 1024, in);
		if(inlen <= 0) break;
		if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
		{
			/* Error */
			EVP_CIPHER_CTX_cleanup(&ctx);
			return 0;
		}
		fwrite(outbuf, 1, outlen, out);
	}
	
	if(!EVP_EncryptFinal(&ctx, outbuf, &outlen))
	{
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return 0;
	}
	
	fwrite(outbuf, 1, outlen, out);
	EVP_CIPHER_CTX_cleanup(&ctx);
	
	return 1;
}


int do_decrypt(FILE *in, FILE *out, int do_encrypt)
{
	/* Allow enough space in output buffer for additional block */
	inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
	int inlen, outlen;
	/* Bogus key and IV: we'd normally set these from
		* another source.
		*/
	unsigned char key[] = "0123456789";
	unsigned char iv[] = "12345678";
	
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_DecryptInit(&ctx, EVP_bf_cbc(), key, iv);
	
	for(;;)
	{
		inlen = fread(inbuf, 1, 1024, in);
		if(inlen <= 0) break;
		if(!EVP_DecryptUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
		{
			/* Error */
			EVP_CIPHER_CTX_cleanup(&ctx);
			return 0;
		}
		fwrite(outbuf, 1, outlen, out);
	}
	
	if(!EVP_DecryptFinal(&ctx, outbuf, &outlen))
	{
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return 0;
	}
	
	fwrite(outbuf, 1, outlen, out);
	EVP_CIPHER_CTX_cleanup(&ctx);
	
	return 1;
}



int main(int argc, char** argv)
{
	if(argc!=2)
	{
		printf("Usage: %s \"passphrase\"", argv[0]);
		return 1;
	}
	
	const char* passphrase=argv[1];
	
	//Create a key from the passphrase
	printf("Create key from passphrase '%s'\n\n", passphrase);
	char* key=createKey(passphrase, "ppk");
	printf("Key = '%s'\n", key);
	

	
	//Free the key
	free(key);
	
	return 0;
}