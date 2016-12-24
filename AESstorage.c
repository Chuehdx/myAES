# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include "myAES.h"

# define STORAGE_SIZE 20

struct AES_decryptblock *myAESCryptstorage[STORAGE_SIZE];
struct AES_encryptblock *now_encryptblock=NULL;
static int number_of_storage = 0;

void AEScrypt_store(char* filename, char* encryptedfilename,char* decryptedfilename, EVP_CIPHER_CTX* de){
	struct AES_decryptblock *myAESCrypt = NULL;
	myAESCrypt = malloc(sizeof(struct AES_decryptblock));
	strcpy(myAESCrypt->filename,filename);
	strcpy(myAESCrypt->encryptedfilename,encryptedfilename);
	strcpy(myAESCrypt->decryptedfilename,decryptedfilename);
	myAESCrypt->de = de;
	myAESCryptstorage[number_of_storage] = myAESCrypt;
	number_of_storage++;
}

void set_now_encryptblock(char* password, char* salt){
	if(now_encryptblock == NULL){
		now_encryptblock = malloc(sizeof(struct AES_decryptblock));
	}
	strcpy(now_encryptblock->password,password);
	strcpy(now_encryptblock->salt,salt);
}

struct AES_encryptblock *get_now_encryptblock(){
	return now_encryptblock;
}


struct AES_decryptblock *AESCrypt_load(char* filename){
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
