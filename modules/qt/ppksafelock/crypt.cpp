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

unsigned char* createKey(const char* passphrase, const char* salt)
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
	
	return (unsigned char*)key;
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

int do_crypt(const unsigned char* key, const unsigned char* in, const char* outpath)
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
	EVP_EncryptInit(&ctx, EVP_bf_cbc(), key, iv);
	
	//Encrypt the chain
	int outlen;
	if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, in, inlen))
	{
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return 0;
	}
	
	if(!EVP_EncryptFinal(&ctx, outbuf+outlen, &outlen))
	{
		/* Error */
		EVP_CIPHER_CTX_cleanup(&ctx);
		return 0;
	}
	
	EVP_CIPHER_CTX_cleanup(&ctx);
	
	printf("Crypt: in='%s'\nout='%s'\n", in, outbuf);
	
	//Save to disk
	FILE* outFile=fopen(outpath, "wb");
	if(outFile)
	{
		fwrite((char*)outbuf, strlen((char*)outbuf), sizeof(char), outFile);
		fclose(outFile);
	}
	else
	{
		fprintf(stderr, "The file '%s' cannot be openned for writing purpose. Aborting.\n", outpath);
		return 0;
	}
	
	//Free buffers
	free(outbuf);
	
	return 1;
}

int do_decrypt(const unsigned char* key, const char* inpath, unsigned char** out)
{
	//Open the file
	FILE* in=fopen(inpath, "rb");
	if(!in)
	{
		fprintf(stderr, "The file '%s' cannot be openned for reading purpose. Aborting.\n", inpath);
		return 0;
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
	EVP_DecryptInit(&ctx, EVP_bf_cbc(), key, iv);
	
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
			return 0;
		}
		outpos+=outlen;
	}
	
	if(!EVP_DecryptFinal(&ctx, (*out)+outpos, &outlen))
	{
		// Error
		EVP_CIPHER_CTX_cleanup(&ctx);
		return 0;
	}
	outpos+=outlen;
	
	EVP_CIPHER_CTX_cleanup(&ctx);
	
	//Close the file
	fclose(in);
	
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
	unsigned char* key=createKey(passphrase, "ppk");
	printf("Key = '%s'\n", key);
	
	do_crypt(key, (const unsigned char*)"je suis un connard", "poulpe.crypt");
	
	unsigned char* poulpe;
	do_decrypt(key, "poulpe.crypt", &poulpe);
	printf("Decrypt: out='%s'\n", (char*)poulpe);
	
	//Free the key
	free(key);
	
	return 0;
}