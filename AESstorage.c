# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include "myAES.h"

# define STORAGE_SIZE 20

struct myAES_decryptblock *decryptblock_storage[STORAGE_SIZE];
struct myAES_encryptblock *encryptblock = NULL;
static int number_of_storage = 0;

void myAESStorage_store_decryptblock(char* filename, char* encryptedfilename,char* decryptedfilename, char *key, char* iv, char* password){
	struct myAES_decryptblock *myAESCrypt = NULL;
	myAESCrypt = malloc(sizeof(struct myAES_decryptblock)+sizeof(unsigned char*)*(32*2+16));
	strcpy(myAESCrypt->filename,filename);
	strcpy(myAESCrypt->encryptedfilename,encryptedfilename);
	strcpy(myAESCrypt->decryptedfilename,decryptedfilename);
	myAESCrypt->key = malloc(sizeof(unsigned char*)*32);
	strcpy(myAESCrypt->key,key);
	myAESCrypt->iv = malloc(sizeof(unsigned char*)*16);
	strcpy(myAESCrypt->iv,iv);
	myAESCrypt->password = malloc(sizeof(unsigned char*)*32);
	strcpy(myAESCrypt->password,password);
	decryptblock_storage[number_of_storage] = myAESCrypt;
	number_of_storage++;
}

void myAESStorage_set_encryptblock(char *key, char* iv, char* password){
	if(encryptblock == NULL){
		encryptblock = malloc(sizeof(struct myAES_encryptblock)+sizeof(unsigned char*)*(32*2+16));
	}
	encryptblock->key = malloc(sizeof(unsigned char*)*32);
	strcpy(encryptblock->key,key);
	encryptblock->iv = malloc(sizeof(unsigned char*)*16);
	strcpy(encryptblock->iv,iv);
	encryptblock->password = malloc(sizeof(unsigned char*)*32);
	strcpy(encryptblock->password,password);
}

struct myAES_encryptblock *myAESStorage_get_encryptblock(){
	return encryptblock;
}

int myAESStorage_get_number_of_storage(){
	return number_of_storage;
}

void myAESStorage_print_storage(){
	printf("File in storage:\n");	
	for(int i = 0;i<number_of_storage;i++){
		printf("(%d) %s ",i+1,decryptblock_storage[i]->filename);
	}
	printf("\n");
}

int myAESStorage_find_file_position(char* filename){
	int i;	
	for(i = 0;i<number_of_storage;i++){
		if(!strcmp(filename, decryptblock_storage[i]->filename)){			
			return i;
		}	
	}
	return i;
}

struct myAES_decryptblock *myAESStorage_get_decryptblock(int i){
	return decryptblock_storage[i];
}
