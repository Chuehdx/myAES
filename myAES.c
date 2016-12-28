# include <stdio.h>
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

void myAES_Encrypt_init(EVP_CIPHER_CTX *e_ctx, char *key, char *iv){//Create new encryption block
	EVP_CIPHER_CTX_init(e_ctx);
	EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
}

void myAES_Decrypt_init(EVP_CIPHER_CTX *d_ctx, char *key, char *iv){//Create new decryption block
	EVP_CIPHER_CTX_init(d_ctx);
        EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
}

int myAES_generate_key_iv(unsigned char* password, unsigned char * salt,unsigned char * key,unsigned char * iv){//generate new key and iv
	int result, round = 5;
	result = EVP_BytesToKey(EVP_aes_256_cbc(),EVP_sha1(),salt,password,strlen((unsigned char*)password),round,key,iv);//turning password into key
	if(result != 32){
		printf("failed to generate key.\n");
		return 1;	
	}
	return 0;
}

int myAES_Encrypt(char* filename, int changekey, int retry){//Main encryption process
	EVP_CIPHER_CTX en;
	unsigned char *password=malloc(sizeof(unsigned char)*32),*salt=malloc(sizeof(unsigned char)*8),*key=malloc(sizeof(unsigned char)*32),*iv=malloc(sizeof(unsigned char)*16);
	char encryptedfilename[40],decryptedfilename[30],destr[] = "-de.txt",enstr[] = "-en-00.txt";
	unsigned char *inputbuffer=malloc(sizeof(unsigned char)*SIZE),*outputbuffer=malloc(sizeof(unsigned char)*(SIZE));
	int input_len = 0, final_len = 0, output_len = 0, inputfile, encryptedfile, file_pos,file_count = 0;	

	file_pos = myAESStorage_find_file_position(filename);//check if the file is already existed in storage
	if(file_pos == STORAGE_SIZE){
		printf("Error, Storage is full.\n");		
		return 1;
	}
	
	//Clear file name array
	memset(encryptedfilename, 0, 30);
	memset(decryptedfilename, 0, 30);

	//Open source file and create encrypted file
	
	strncpy(encryptedfilename,filename,strlen(filename)-4);
	//char tmpstr[] = "-en.txt";
	strcat(encryptedfilename,enstr);
	//encryptedfile = open(encryptedfilename,O_WRONLY|O_CREAT|O_TRUNC,0400|0200);

	strncpy(decryptedfilename,filename,strlen(filename)-4);
	strcat(decryptedfilename,destr);

	inputfile = open(filename,O_RDONLY);
	if(inputfile == -1){
		printf("faild to open source file %s.\n",filename);
		return 1;
	}
	
	
	if(changekey){//need to change key
		srand(time(NULL)*getpid());
		myAES_generate_new_password(password);
		myAES_generate_new_salt(salt);
		if(myAES_generate_key_iv(password,salt,key,iv)){
			printf("Error, failed to initialize key and IV.\n");
			return -1;
		}
		myAESStorage_set_encryptblock(key,iv,password);//store new key and iv info to storage
	}else{//get current key and iv info from storage
		struct myAES_encryptblock *encryptblock = myAESStorage_get_encryptblock();
		memcpy(key,encryptblock->key,strlen((unsigned char*)encryptblock->key));
		memcpy(iv,encryptblock->iv, strlen((unsigned char*)encryptblock->iv));
		memcpy(password,encryptblock->password, strlen((unsigned char*)encryptblock->password));
	}
	//start of encryption process
	myAES_Encrypt_init(&en,key,iv);
	
	int total_len=0;
	while((input_len = read(inputfile,inputbuffer,SIZE)) > 0){ // read source file and write it into inputbuffer
		//printf("in en-update input_len:%d\n",input_len);
		if(!EVP_EncryptUpdate(&en,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		//* turning plain text in input file into cipher text and store in outputbuffer
			printf("Error, failed to update encryption.\n");
			return 1;
		}
		//memset(encryptedfileblockname, 0, 40);
		//strcpy(encryptedfileblockname,encryptedfilename);
		file_count = file_count +1;
		encryptedfilename[strlen(encryptedfilename)-6] = file_count/10 + '0';
		encryptedfilename[strlen(encryptedfilename)-5] = file_count%10 + '0';
		//strcat(encryptedfileblockname,enstr);
		//printf("filename :%s\n",encryptedfilename);
		encryptedfile = open(encryptedfilename,O_WRONLY|O_CREAT|O_TRUNC,0400|0200);
		if(write(encryptedfile,outputbuffer,output_len) != output_len){//write to encrypted file from output buffer
			printf("Error, failed to write encryption to file.\n");
			return 1;
		}
		close(encryptedfile);
		total_len+=input_len;
		//printf("file length :%d\n",total_len);
	}
	if(!EVP_EncryptFinal_ex(&en, (unsigned char*) outputbuffer, &final_len)){//encrypt the remaining bytes of file
		printf("Error, failed to update final encryption.\n");
		return 1;
	}
	total_len+=final_len;
	//printf("file length :%d\n",total_len);
	encryptedfile = open(encryptedfilename,O_WRONLY|O_APPEND,0400|0200);
	if(write(encryptedfile,outputbuffer,final_len) != final_len){//write the remaining bytes of source file to encrypted file
		printf("Error, failed to write fianl encryption to file.\n");
		return 1;
	}
	myAESStorage_store_decryptblock(filename,encryptedfilename,decryptedfilename,key,iv,password,file_pos,file_count);//Store the decrypt info of this file into its block
	
	//close file and clear memory
	close(inputfile);
	close(encryptedfile);
	//if(retry)
		//printf("retryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy\n");
	if(!retry){
		printf("encryption successed with key ");
			for(int i=0;i<32;i++){
			printf("%c",password[i]);
		}
		printf("\n");
	}
	free(key);
	free(password);
	free(iv);
	free(salt);
	free(inputbuffer);
	free(outputbuffer);
	EVP_CIPHER_CTX_cleanup(&en);
	return 0;
}

int myAES_Decrypt(char* filename){
	EVP_CIPHER_CTX de;
	int input_len = 0, final_len = 0, output_len = 0, encryptedfile, outputfile, file_pos, file_count;
	size_t file_len = 0,last_file_len = 0;	

	file_pos = myAESStorage_find_file_position(filename);
	if(file_pos == myAESStorage_get_number_of_storage()){
		printf("Error, failed to find encrypted file.\n");
		return 1;
	}
	printf("in here\n");
	unsigned char *key=malloc(sizeof(unsigned char)*32),*iv=malloc(sizeof(unsigned char)*16),*password=malloc(sizeof(unsigned char)*32),*file;
	//unsigned char *inputbuffer = malloc(sizeof(unsigned char)*(SIZE+AES_BLOCK_SIZE)),*outputbuffer= malloc(sizeof(unsigned char)*(SIZE+AES_BLOCK_SIZE));
	unsigned char *outputbuffer;
	char encryptedfilename[40];

	//find its decryption block position
	printf("finish malloc\n");
	
	//get the decrypt info of the file from its decryption block
	struct myAES_decryptblock *myAESCrypt = myAESStorage_get_decryptblock(file_pos);
	memcpy(key,myAESCrypt->key, strlen((unsigned char*)myAESCrypt->key));
	memcpy(iv,myAESCrypt->iv, strlen((unsigned char*)myAESCrypt->iv));
	memcpy(password,myAESCrypt->password, strlen((unsigned char*)myAESCrypt->password));
	strcpy(encryptedfilename,myAESCrypt->encryptedfilename);
	file_count = myAESCrypt->file_count;
	//encryptedfile = open(myAESCrypt->encryptedfilename,O_RDONLY);
	printf("filename: %s\n",encryptedfilename);
	
	last_file_len = myAES_get_file_length(encryptedfilename);
	printf("getlastfile_len\n");
	file_len = last_file_len + SIZE*(file_count-1);
	/*for(int i=1;i<=file_count;i++){
		encryptedfilename[strlen(encryptedfilename)-6] = i/10 + '0';
		encryptedfilename[strlen(encryptedfilename)-5] = i%10 + '0';
		printf("filename :%s\n",encryptedfilename);
		file_len+=myAES_get_file_length(encryptedfilename);
		printf("file_len_get: %d\n",file_len);
	}*/
	
	printf("file_len_get: %lu",file_len);
	file = malloc(sizeof(unsigned char)*file_len);
	for(int i=1;i<=file_count;i++){
		encryptedfilename[strlen(encryptedfilename)-6] = i/10 + '0';
		encryptedfilename[strlen(encryptedfilename)-5] = i%10 + '0';
		int len;
		if(i==file_count)len = last_file_len;
		else len = SIZE;
		myAES_read_file(encryptedfilename,file+SIZE*(i-1),len);
		printf("read %s.\n",encryptedfilename);
	}
	
	/*printf("file in:  ");	
	for(int i=0;i<file_len;i++){
		printf("%c",file[i]);
	}
	printf("\nEND\n");
	printf("file in len: %d\n",file_len);*/	
	
	/*if((lseek(encryptedfile,0,SEEK_SET)) != 0){	
		printf("Error, failed to seek encrypted file.\n");
		free(key);
		free(password);
		free(iv);
		//free(inputbuffer);
		//free(outputbuffer);
		close(encryptedfile);
		EVP_CIPHER_CTX_cleanup(&de);
		return 1;
	}*/
	
	outputfile = open(myAESCrypt->decryptedfilename,O_WRONLY|O_CREAT|O_TRUNC,0400|0200);
	//start of decryption process
	myAES_Decrypt_init(&de,key,iv);
	//new method here
	outputbuffer = malloc(sizeof(unsigned char)*file_len);
	
	int ret = EVP_DecryptUpdate(&de,(unsigned char*) outputbuffer, &output_len,(unsigned char*) file,file_len);
	if (ret == 0){
		printf("Error, failed to update decryption.\n");
		return 1;
	}
	if(write(outputfile,outputbuffer,output_len) != output_len){//write to output file from output buffer
		printf("Error, failed to write decryption to file.\n");
		return 1;
	}
	ret = EVP_DecryptFinal_ex(&de, (unsigned char*) outputbuffer + output_len, &final_len);
	int fin_len = output_len + final_len;
	if(ret == 0 || fin_len > file_len){
		printf("Error, failed to update final decryption.\n");
		return 1;
	}
	if(write(outputfile,outputbuffer+output_len,final_len) != final_len){
		printf("Error, failed to write fianl decryption to file.\n");//write the remaining bytes of encrypted file to output file
		return 1;
	}

	/*
	
	while((input_len = read(encryptedfile,inputbuffer,SIZE)) > 0){ // read encrypted file and write it into inputbuffer
		//printf("inde-update input_len:%d\n",input_len);
		if(!EVP_DecryptUpdate(&de,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		//*turning cipher text in encrypted file into plain text and store in outputbuffer 
			printf("Error, failed to update decryption.\n");
			free(key);
			free(password);
			free(iv);
			free(inputbuffer);
			free(outputbuffer);
			close(encryptedfile);
			close(outputfile);
			EVP_CIPHER_CTX_cleanup(&de);
			return 1;
		}if(write(outputfile,outputbuffer,output_len) != output_len){//write to output file from output buffer
			printf("Error, failed to write decryption to file.\n");
			free(key);
			free(password);
			free(iv);
			free(inputbuffer);
			free(outputbuffer);
			close(encryptedfile);
			close(outputfile);
			EVP_CIPHER_CTX_cleanup(&de);
			return 1;
		}	
	}
	if(!EVP_DecryptFinal_ex(&de, (unsigned char*) outputbuffer, &final_len)){//encrypt the remaining bytes of file 
		printf("Error, failed to update final decryption.\n");
		free(key);
		free(password);
		free(iv);
		free(inputbuffer);
		free(outputbuffer);
		close(encryptedfile);
		close(outputfile);
		EVP_CIPHER_CTX_cleanup(&de);
		return 1;
	}
	if(write(outputfile,outputbuffer,final_len) != final_len){
		printf("Error, failed to write fianl decryption to file.\n");//write the remaining bytes of encrypted file to output file
		free(key);
		free(password);
		free(iv);
		free(inputbuffer);
		free(outputbuffer);
		close(encryptedfile);
		close(outputfile);
		EVP_CIPHER_CTX_cleanup(&de);
		return 1;
	}*/
	
	//close file and clear memory
	//close(encryptedfile);
	printf("before close\n");
	close(outputfile);
	printf("decryption successed with key ");	
	for(int i=0;i<32;i++){
		printf("%c",password[i]);
	}
	printf("\n");
	printf("before free\n");
	free(key);
	free(password);
	free(iv);
	printf("before free file\n");
	free(file);
	//free(inputbuffer);
	printf("before free buffer\n");
	free(outputbuffer);
	EVP_CIPHER_CTX_cleanup(&de);
	return 0;
}

void myAES_generate_new_password(unsigned char* password){
	for(int i=0;i<32;i++){
		int tmp = (rand()+getpid())%75 + 48;
		password[i] = (char) tmp;
	}
}

void myAES_generate_new_salt(unsigned char* salt){	
	for(int i=0;i<8;i++){
		int tmp = (rand()+getpid())%75 + 48;
		//int tmp = rand()%26 + 97;
		salt[i] = (char) tmp;
	}
}

size_t myAES_get_file_length(char* filename){
	FILE *tmp = fopen(filename,"rb");
	if(tmp!=NULL){
		printf("opened\n");
		fseek(tmp, 0, SEEK_END); 
		printf("find last\n");
		size_t length = ftell(tmp);
		fclose(tmp);
		return length;
	}
}

void myAES_read_file(char* filename,char* file,size_t file_len){
	FILE *tmp = fopen(filename,"r");
	fread(file,1,file_len,tmp);
	fclose(tmp);
}

