# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include "myAES.h"
# include "AESstorage.h"

struct myAES_decryptblock *decryptblock_storage[STORAGE_SIZE];
struct myAES_encryptblock *encryptblock = NULL;
static int number_of_storage = 0;

void myAESStorage_store_decryptblock(char* filename, char* encryptedfilename, char* decryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password,int password_len ,int file_pos, int file_count){
	if(file_pos == number_of_storage){//if the file isn't existed
		//create decrypt block and allocate memory
		struct myAES_decryptblock *myAESCrypt = NULL;
		myAESCrypt =(struct myAES_decryptblock*) malloc(sizeof(struct myAES_decryptblock)+sizeof(unsigned char)*((KEY_SIZE*3/2)+password_len));
		strcpy(myAESCrypt->filename,filename);
		strcpy(myAESCrypt->encryptedfilename,encryptedfilename);
		strcpy(myAESCrypt->decryptedfilename,decryptedfilename);
		myAESCrypt->key = (unsigned char*)malloc(sizeof(unsigned char)*KEY_SIZE);
		myAESCrypt->iv = (unsigned char*)malloc(sizeof(unsigned char)*KEY_SIZE/2);
		myAESCrypt->password = (unsigned char*)malloc(sizeof(unsigned char)*password_len);
		myAESCrypt->password_len = password_len;
		myAESCrypt->file_count = file_count;
		decryptblock_storage[file_pos] = myAESCrypt;
		number_of_storage++;
	}
	//if the file already existed we just update key iv and password
	memcpy(decryptblock_storage[file_pos]->key,key,KEY_SIZE);	
	memcpy(decryptblock_storage[file_pos]->iv,iv,KEY_SIZE/2);
	memcpy(decryptblock_storage[file_pos]->password,password,password_len);
}

void myAESStorage_set_encryptblock(unsigned char *key, unsigned char* iv, unsigned char* password, int password_len){
	if(encryptblock == NULL){//initialize
		encryptblock = malloc(sizeof(struct myAES_encryptblock)+sizeof(unsigned char*)*(KEY_SIZE*3/2)+sizeof(char));
		encryptblock->key = malloc(sizeof(unsigned char)*KEY_SIZE);
		encryptblock->iv = malloc(sizeof(unsigned char)*KEY_SIZE/2);
		encryptblock->password = malloc(sizeof(unsigned char)*password_len);
	}
	//set value
	memcpy(encryptblock->key,key,KEY_SIZE);
	memcpy(encryptblock->iv,iv,KEY_SIZE/2);
	memcpy(encryptblock->password,password,password_len);
	encryptblock->password_len = password_len;
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
