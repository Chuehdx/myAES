# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include "myAES.h"

# define STORAGE_SIZE 20

struct myAES_decryptblock *decryptblock_storage[STORAGE_SIZE];
struct myAES_encryptblock *encryptblock = NULL;
static int number_of_storage = 0;

void myAESStorage_store_decryptblock(char* filename, char* encryptedfilename, char* decryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password, int file_pos){//store the decrypt info of the file into its block
	if(file_pos == number_of_storage){//if the file isn't existed
		struct myAES_decryptblock *myAESCrypt = NULL;
		myAESCrypt = malloc(sizeof(struct myAES_decryptblock)+sizeof(unsigned char*)*(32*2+16));
		strcpy(myAESCrypt->filename,filename);
		strcpy(myAESCrypt->encryptedfilename,encryptedfilename);
		strcpy(myAESCrypt->decryptedfilename,decryptedfilename);
		myAESCrypt->key = malloc(sizeof(unsigned char*)*32);
		myAESCrypt->iv = malloc(sizeof(unsigned char*)*16);
		myAESCrypt->password = malloc(sizeof(unsigned char*)*32);
		decryptblock_storage[file_pos] = myAESCrypt;
		number_of_storage++;
	}
	//if the file already existed we just update key iv and password
	memcpy(decryptblock_storage[file_pos]->key,key,strlen((unsigned char*)key));	
	memcpy(decryptblock_storage[file_pos]->iv,iv,strlen((unsigned char*)iv));
	memcpy(decryptblock_storage[file_pos]->password,password,strlen((unsigned char*)password));
	/*printf("key in storage: ");	
	for(int i=0;i<32;i++){
		printf("%c",decryptblock_storage[file_pos]->password[i]);
	}
	printf("\n");*/
}

void myAESStorage_set_encryptblock(unsigned char *key, unsigned char* iv, unsigned char* password){//set the new key iv and password for encryption
	if(encryptblock == NULL){//initialize
		encryptblock = malloc(sizeof(struct myAES_encryptblock)+sizeof(unsigned char*)*(32*2+16));
		encryptblock->key = malloc(sizeof(unsigned char*)*32);
		encryptblock->iv = malloc(sizeof(unsigned char*)*16);
		encryptblock->password = malloc(sizeof(unsigned char*)*32);
	}
	memcpy(encryptblock->key,key,strlen((unsigned char*)key));
	memcpy(encryptblock->iv,iv,strlen((unsigned char*)iv));
	memcpy(encryptblock->password,password,strlen((unsigned char*)password));
}

struct myAES_encryptblock *myAESStorage_get_encryptblock(){
	return encryptblock;
}

int myAESStorage_get_number_of_storage(){
	return number_of_storage;
}

void myAESStorage_print_storage(){
	for(int i=0;i<50;i++)printf("-");
	printf("\n");	
	printf("File in storage:\n");
	for(int i = 0;i<number_of_storage;i++){
		printf("(%d) %s ",i+1,decryptblock_storage[i]->filename);
	}
	printf("\n");
	for(int i=0;i<50;i++)printf("-");
	printf("\n");
}

int myAESStorage_find_file_position(char* filename){//find the block location of given file
	int i;	
	for(i = 0;i<number_of_storage;i++){
		if(!strcmp(filename, decryptblock_storage[i]->filename)){//find it			
			return i;
		}	
	}
	return i;
}

struct myAES_decryptblock *myAESStorage_get_decryptblock(int i){
	return decryptblock_storage[i];
}
