# include <stdio.h>
# include <stdbool.h>
# include <sys/types.h>    
# include <sys/stat.h>    
# include <fcntl.h>
# include <time.h>
# include <stdlib.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <string.h>
# include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
# include "myAES.h"
# include "AESstorage.c"

void myAES_Encrypt_init(EVP_CIPHER_CTX *e_ctx, char *key, char *iv){
	EVP_CIPHER_CTX_init(e_ctx);
	EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
}

void myAES_Decrypt_init(EVP_CIPHER_CTX *d_ctx, char *key, char *iv){
	EVP_CIPHER_CTX_init(d_ctx);
        EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
}

int myAES_generate_key_iv(unsigned char* password, unsigned int password_len, unsigned char * salt,unsigned char * key,unsigned char * iv){
	int result, round = 5;
	result = EVP_BytesToKey(EVP_aes_256_cbc(),EVP_sha1(),salt,password,password_len,round,key,iv);
	if(result != 32){
		printf("failed to generate key.\n");
		return 1;	
	}
	return 0;
}

int myAES_Encrypt(char* filename, bool changekey){
	EVP_CIPHER_CTX en;
	unsigned char *password=malloc(sizeof(unsigned char)*32),*salt=malloc(sizeof(unsigned char)*8),*key=malloc(sizeof(unsigned char)*32),*iv=malloc(sizeof(unsigned char)*16);
	char encryptedfilename[30],decryptedfilename[30];	
	char *inputbuffer=malloc(SIZE),*outputbuffer=malloc(SIZE+AES_BLOCK_SIZE);
	int input_len = 0, final_len = 0, output_len = 0, inputfile, encryptedfile, password_len, file_pos;

	file_pos = myAESStorage_find_file_position(filename);
	if(file_pos == STORAGE_SIZE){
		printf("Error, Storage is full.\n");		
		return 1;
	}	
	
	//init array
	memset(encryptedfilename, 0, 30);
	memset(decryptedfilename, 0, 30);

	strncpy(encryptedfilename,filename,strlen(filename)-4);
	char tmpstr[] = "-en.txt";
	strcat(encryptedfilename,tmpstr);

	strncpy(decryptedfilename,filename,strlen(filename)-4);
	strcpy(tmpstr,"-de.txt");
	strcat(decryptedfilename,tmpstr);

	inputfile = open(filename,O_RDONLY);
	if(inputfile == -1){
		printf("faild to open source file %s.\n",filename);
		return 1;
	}
	
	encryptedfile = open(encryptedfilename,O_RDWR|O_CREAT|O_TRUNC,0400|0200);
	if(changekey){
		srand(time(NULL));
		myAES_generate_new_password(password);
		myAES_generate_new_salt(salt);
		password_len = strlen((unsigned char*)password);
		if(myAES_generate_key_iv(password,password_len,salt,key,iv)){
			printf("Error, failed to initialize key and IV.\n");
			return -1;
		}
		myAESStorage_set_encryptblock(key,iv,password);
	}else{
		struct myAES_encryptblock *encryptblock = myAESStorage_get_encryptblock();
		strcpy(key,encryptblock->key);
		strcpy(iv,encryptblock->iv);
		strcpy(password,encryptblock->password);
	}
	
	//start of encryption process
	//aesencryption init process
	myAES_Encrypt_init(&en,key,iv);

	while((input_len = read(inputfile,inputbuffer,SIZE)) > 0){ // read return bytes read, 0:EOF, -1:error
		
		if(!EVP_EncryptUpdate(&en,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		//* Update cipher text 
			printf("Error, failed to update encryption.\n");
			return 1;
		}
		if(write(encryptedfile,outputbuffer,output_len) != output_len){
			printf("Error, failed to write encryption to file.\n");
			return 1;
		}	
	}
	if(!EVP_EncryptFinal_ex(&en, (unsigned char*) outputbuffer, &final_len)){			//* updates the remaining bytes 
		printf("Error, failed to update final encryption.\n");
		return 1;
	}
	if(write(encryptedfile,outputbuffer,final_len) != final_len){
		printf("Error, failed to write fianl encryption to file.\n");
		return 1;
	}
	myAESStorage_store_decryptblock(filename,encryptedfilename,decryptedfilename,key,iv,password,file_pos);
	close(inputfile);
	close(encryptedfile);
	printf("encryption successed with key ");	
	for(int i=0;i<32;i++){
		printf("%c",password[i]);
	}
	printf("\n");
	free(key);
	free(password);
	free(iv);
	free(inputbuffer);
	free(outputbuffer);
	EVP_CIPHER_CTX_cleanup(&en);
	return 0;
}

int myAES_Decrypt(char* filename){//(EVP_CIPHER_CTX *d, int encryptedfile,int outputfile){
	EVP_CIPHER_CTX de;
	unsigned char *key=malloc(sizeof(unsigned char)*32),*iv=malloc(sizeof(unsigned char)*16),*password=malloc(sizeof(unsigned char)*32);	
	char *inputbuffer = malloc(SIZE+AES_BLOCK_SIZE);
	char *outputbuffer= malloc(SIZE+AES_BLOCK_SIZE);
	int input_len = 0, final_len = 0, output_len = 0, encryptedfile, outputfile, file_pos , password_len;
	
	file_pos = myAESStorage_find_file_position(filename);
	if(file_pos == myAESStorage_get_number_of_storage()){
		printf("Error, failed to find encrypted file.\n");
		return 1;
	}
	struct myAES_decryptblock *myAESCrypt = myAESStorage_get_decryptblock(file_pos);
	strcpy(key,myAESCrypt->key);
	strcpy(iv,myAESCrypt->iv);
	strcpy(password,myAESCrypt->password);

	encryptedfile = open(myAESCrypt->encryptedfilename,O_RDONLY);
	if((lseek(encryptedfile,0,SEEK_SET)) != 0){	
		printf("Error, failed to seek encrypted file.\n");
		return 1;
	}
	
	outputfile = open(myAESCrypt->decryptedfilename,O_RDWR|O_CREAT|O_TRUNC,0400|0200);

	
	//start decryption
	myAES_Decrypt_init(&de,key,iv);
	while((input_len = read(encryptedfile,inputbuffer,SIZE)) > 0){ // read return bytes read, 0:EOF, -1:error
		if(!EVP_DecryptUpdate(&de,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		/* Update cipher text */
			printf("Error, failed to update decryption.\n");
			return 1;
		}if(write(outputfile,outputbuffer,output_len) != output_len){
			printf("Error, failed to write decryption to file.\n");
			return 1;
		}	
	}
	if(!EVP_DecryptFinal_ex(&de, (unsigned char*) outputbuffer, &final_len)){			/* updates the remaining bytes */
		printf("Error, failed to update final decryption.\n");
		return 1;
	}
	if(write(outputfile,outputbuffer,final_len) != final_len){
		printf("Error, failed to write fianl decryption to file.\n");
		return 1;
	}
	close(encryptedfile);
	close(outputfile);
	printf("decryption successed with key ");	
	for(int i=0;i<32;i++){
		printf("%c",password[i]);
	}
	printf("\n");
	free(key);
	free(password);
	free(iv);
	free(inputbuffer);
	free(outputbuffer);
	EVP_CIPHER_CTX_cleanup(&de);
	return 0;
}

void myAES_generate_new_password(unsigned char* password){
	for(int i=0;i<32;i++){
		int tmp = rand()%75 + 48;
		password[i] = (char) tmp;
			
	}
}

void myAES_generate_new_salt(unsigned char* salt){	
	for(int i=0;i<8;i++){
		int tmp = rand()%26 + 97;
		salt[i] = (char) tmp;
	}
}

