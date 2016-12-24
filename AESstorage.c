# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include "myAES.h"

struct AESCrypt *myAESCryptstorage[20];
static int number_of_storage = 0;

void AEScrypt_store(char* filename, char* encryptedfilename,char* decryptedfilename, EVP_CIPHER_CTX* de){
	struct AESCrypt *myAESCrypt = NULL;
	myAESCrypt = malloc(sizeof(struct AESCrypt));
	strcpy(myAESCrypt->filename,filename);
	strcpy(myAESCrypt->encryptedfilename,encryptedfilename);
	strcpy(myAESCrypt->decryptedfilename,decryptedfilename);
	myAESCrypt->de = de;
	myAESCryptstorage[number_of_storage] = myAESCrypt;
	number_of_storage++;
}



struct AESCrypt *AESCrypt_load(char* filename){
	for(int i = 0;i<number_of_storage;i++){
		//printf("file in storage : %s\n",myAESCryptstorage[i]->filename);
		if(!strcmp(filename, myAESCryptstorage[i]->filename)){
			//printf("find file at position %d.\n", i);
			return myAESCryptstorage[i];
		}	
	}
	printf("Error, can't find %s.\n", filename);
	return;
}
