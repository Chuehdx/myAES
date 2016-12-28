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
	unsigned char *password=malloc(sizeof(unsigned char)*32),*salt=malloc(sizeof(unsigned char)*8),*key=malloc(sizeof(unsigned char)*32),*iv=malloc(sizeof(unsigned char)*16),*file;
	char encryptedfilename[30],decryptedfilename[30];
	//unsigned char *inputbuffer=malloc(sizeof(unsigned char)*SIZE),*outputbuffer=malloc(sizeof(unsigned char)*(SIZE+AES_BLOCK_SIZE));
	unsigned char *inputbuffer,*outputbuffer;
	int input_len = 0, final_len = 0, output_len = 0, inputfile, encryptedfile, file_pos,blks;
	size_t file_len;	

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
	file_len = myAES_get_file_length(filename);
	file = malloc(file_len);
	myAES_read_file(filename,file,file_len);
	printf("file:");	
	for(int i=0;i<file_len;i++){
		printf("%c",file[i]);
	}
	printf("file_len: %lu.\n",file_len);

	encryptedfile = open(encryptedfilename,O_WRONLY|O_CREAT|O_TRUNC,0400|0200);
	
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
	//new method here;
	blks = (file_len/BLK_SIZE)+1;
	outputbuffer = malloc(blks * BLK_SIZE);
	
	int ret = EVP_EncryptUpdate(&en,(unsigned char*) outputbuffer, &output_len,(unsigned char*) file,file_len);
	if (ret == 0){
		printf("failed to update.\n");
		return 1;
	}
	if(write(encryptedfile,outputbuffer,output_len)!= output_len){
		printf("Error, failed to write encryption to file.\n");
		return 1;
	}
	ret = EVP_EncryptFinal_ex(&en, (unsigned char*) outputbuffer+output_len, &final_len);
	int fin_len=output_len+final_len;
	if (ret == 0 || fin_len != (blks * BLK_SIZE)){
		printf("failed to final update.\n");
		return 1;
	}
	if(write(encryptedfile,outputbuffer+output_len,final_len)!=final_len){
		printf("Error, failed to write fianl encryption to file.\n");
		return 1;
	}
	myAESStorage_store_decryptblock(filename,encryptedfilename,decryptedfilename,key,iv,password,file_pos);


	/*
	while((input_len = read(inputfile,inputbuffer,SIZE)) > 0){ // read source file and write it into inputbuffer
		//printf("in en-update input_len:%d\n",input_len);
		if(!EVP_EncryptUpdate(&en,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		//* turning plain text in input file into cipher text and store in outputbuffer
			printf("Error, failed to update encryption.\n");
			return 1;
		}
		if(write(encryptedfile,outputbuffer,output_len) != output_len){//write to encrypted file from output buffer
			printf("Error, failed to write encryption to file.\n");
			return 1;
		}	
	}
	//printf("pre final-en-update input_len:%d\n",input_len);
	if(!EVP_EncryptFinal_ex(&en, (unsigned char*) outputbuffer, &final_len)){//encrypt the remaining bytes of file
		printf("Error, failed to update final encryption.\n");
		return 1;
	}
	//printf("en-final_len:%d\n",final_len);
	if(write(encryptedfile,outputbuffer,final_len) != final_len){//write the remaining bytes of source file to encrypted file
		printf("Error, failed to write fianl encryption to file.\n");
		return 1;
	}
	myAESStorage_store_decryptblock(filename,encryptedfilename,decryptedfilename,key,iv,password,file_pos);//Store the decrypt info of this file into its block
	*/
	//close file and clear memory
	close(inputfile);
	close(encryptedfile);
		if(retry)
		printf("retryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy\n");
	//if(!retry){
		printf("encryption successed with key ");
			for(int i=0;i<32;i++){
			printf("%c",password[i]);
		}
		printf("\n");
	//}else
	
	printf("file out len: %d\n",fin_len);	
	printf("file out: ");	
	for(int i=0;i<fin_len;i++){
		printf("%c",outputbuffer[i]);
	}
	printf(".......END\n");
	free(key);
	free(password);
	free(iv);
	free(salt);
	//free(inputbuffer);
	free(outputbuffer);
	free(file);
	EVP_CIPHER_CTX_cleanup(&en);
	return 0;
}

int myAES_Decrypt(char* filename){
	EVP_CIPHER_CTX de;
	int input_len = 0, final_len = 0, output_len = 0, encryptedfile, outputfile, file_pos;

	file_pos = myAESStorage_find_file_position(filename);
	if(file_pos == myAESStorage_get_number_of_storage()){
		printf("Error, failed to find encrypted file.\n");
		return 1;
	}

	unsigned char *key=malloc(sizeof(unsigned char)*32),*iv=malloc(sizeof(unsigned char)*16),*password=malloc(sizeof(unsigned char)*32),*file;
	//unsigned char *inputbuffer = malloc(sizeof(unsigned char)*(SIZE+AES_BLOCK_SIZE)),*outputbuffer= malloc(sizeof(unsigned char)*(SIZE+AES_BLOCK_SIZE));
	unsigned char *inputbuffer,*outputbuffer;
	
	size_t file_len;
	
	//find its decryption block position
	
	//get the decrypt info of the file from its decryption block
	struct myAES_decryptblock *myAESCrypt = myAESStorage_get_decryptblock(file_pos);
	memcpy(key,myAESCrypt->key, strlen((unsigned char*)myAESCrypt->key));
	memcpy(iv,myAESCrypt->iv, strlen((unsigned char*)myAESCrypt->iv));
	memcpy(password,myAESCrypt->password, strlen((unsigned char*)myAESCrypt->password));
	encryptedfile = open(myAESCrypt->encryptedfilename,O_RDONLY);
	
	file_len = myAES_get_file_length(myAESCrypt->encryptedfilename);
	file = malloc(file_len);
	myAES_read_file(myAESCrypt->encryptedfilename,file,file_len);
	printf("file in:  ");	
	for(int i=0;i<file_len;i++){
		printf("%c",file[i]);
	}
	printf(".......END\n");
	printf("file in len: %d\n",file_len);	
	//printf("file_len: %lu.\n",file_len);
	if((lseek(encryptedfile,0,SEEK_SET)) != 0){	
		printf("Error, failed to seek encrypted file.\n");
		free(key);
		free(password);
		free(iv);
		//free(inputbuffer);
		//free(outputbuffer);
		close(encryptedfile);
		EVP_CIPHER_CTX_cleanup(&de);
		return 1;
	}
	
	outputfile = open(myAESCrypt->decryptedfilename,O_WRONLY|O_CREAT|O_TRUNC,0400|0200);
	
	
	//start of decryption process
	myAES_Decrypt_init(&de,key,iv);
	//new method here
	outputbuffer = malloc(file_len);
	
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
	//printf("pre final-de-update input_len:%d\n",input_len);
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
	//printf("final-de-_len:%d\n",final_len);
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
	}
	*/
	//close file and clear memory
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
	free(file);
	//free(inputbuffer);
	free(outputbuffer);
	EVP_CIPHER_CTX_cleanup(&de);
	return 0;
}

size_t myAES_get_file_length(char* filename){
	FILE *tmp = fopen(filename,"r");
	fseek(tmp, 0, SEEK_END); 
	size_t length = ftell(tmp);
	//fseek(tmp, 0, SEEK_SET);
	fclose(tmp);
	return length;
}

void myAES_read_file(char* filename,char* file,size_t file_len){
	FILE *tmp = fopen(filename,"r");
	fread(file,1,file_len,tmp);
	fclose(tmp);
}

void myAES_generate_new_password(unsigned char* password){
	for(int i=0;i<32;i++){
		//int tmp1 = rand()%3,tmp2;
		//if(tmp1)
		   	//tmp2 = rand()%26 + 65;
		//else
		int 	tmp =  (rand()+getpid())%75 + 48;
		password[i] = (char) tmp;
	}
	//strcpy(password,"5BZtQ_1TYRTp@^7Le9TKLeZhfmgRP=>4");
}

void myAES_generate_new_salt(unsigned char* salt){	
	for(int i=0;i<8;i++){
		int tmp = (rand()+getpid())%75 + 48;
		//int tmp = rand()%26 + 97;
		salt[i] = (char) tmp;
	}/*
	strcpy(salt,"qqqqqqqq");
	for(int i=0;i<8;i++){
		printf("%c",salt[i]);
	}*/
}

