# include <stdio.h>
# include <sys/types.h>    
# include <sys/stat.h>    
# include <fcntl.h>
# include <time.h>
# include <stdlib.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <string.h>
# include <unistd.h>
# include "myAES.h"
# include "myAESstorage.h"

void myAES_Encrypt_init(EVP_CIPHER_CTX *e_ctx, char *key, char *iv){//Create new encryption block
	EVP_CIPHER_CTX_init(e_ctx);
	EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
}

void myAES_Decrypt_init(EVP_CIPHER_CTX *d_ctx, char *key, char *iv){//Create new decryption block
	EVP_CIPHER_CTX_init(d_ctx);
        EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
}

int myAES_generate_key_iv(unsigned char* password,int password_len, unsigned char * salt,unsigned char * key,unsigned char * iv){//generate new key and iv
	int result, round = 5;
	result = EVP_BytesToKey(EVP_aes_256_cbc(),EVP_sha1(),salt,password,password_len,round,key,iv);//turning password into key
	if(result != 32){
		printf("failed to generate key.\n");
		return 0;	
	}
	return 1;
}

int myAES_Encrypt(char* filename, int changekey, char *outputstr){//Main encryption process
	EVP_CIPHER_CTX en;
	int final_len = 0, output_len = 0, password_len, inputfile, encryptedfile, file_count = 0, blks, encrypt_result, total_len;	
	size_t file_len;

	inputfile = open(filename,O_RDONLY);//no such file
	if(inputfile == -1){
		printf("faild to open source file %s.\n",filename);
		return 0;
	}
	
	//make sure it can open file successfully then allocate memory to variables
	unsigned char *password=(unsigned char *)malloc(sizeof(unsigned char)*32),*salt=(unsigned char *)malloc(sizeof(unsigned char)*8),*key=(unsigned char *)malloc(sizeof(unsigned char)*32),*iv=(unsigned char *)malloc(sizeof(unsigned char)*16), *outputbuffer, *file;
	char encryptedfilename[40],decryptedfilename[30],destr[] = "-de.txt",enstr[] = "-en-00.txt";

	//Clear file name array
	memset(encryptedfilename, 0, 40);
	memset(decryptedfilename, 0, 30);

	//decide encrypted and decrypted file name
	strncpy(encryptedfilename,filename,strlen(filename)-4);
	strcat(encryptedfilename,enstr);
	strncpy(decryptedfilename,filename,strlen(filename)-4);
	strcat(decryptedfilename,destr);

	//read file
	file_len = myAES_get_file_length(filename);
	file = (unsigned char *)malloc(sizeof(unsigned char)*file_len);
	myAES_read_file(filename,file,file_len);

	if(changekey){//need to change key
		srand(time(NULL));
		myAES_generate_new_password(password);
		myAES_generate_new_salt(salt);
		password_len = strlen((char*)(password));
		
		if(!myAES_generate_key_iv(password,password_len,salt,key,iv)){
			printf("Error, failed to initialize key and IV.\n");
			return 0;
		}
		myAESStorage_set_encryptblock(key,iv,password,password_len);//store new key and iv info to storage
	}else{//get current key and iv info from storage
		struct myAES_encryptblock *encryptblock = myAESStorage_get_encryptblock();
		memcpy(key,encryptblock->key,KEY_SIZE);
		memcpy(iv,encryptblock->iv, KEY_SIZE/2);
		password_len = encryptblock->password_len;
		memcpy(password,encryptblock->password, password_len);
	}
	//start of encryption process
	myAES_Encrypt_init(&en,key,iv);
	printf("en use key:%s\n",key);
	printf("en use iv:%s\n",iv);
	blks = (file_len/BLK_SIZE)+1;
	outputbuffer = (unsigned char *)malloc(sizeof(unsigned char)*blks * BLK_SIZE);
	
	encrypt_result = EVP_EncryptUpdate(&en,(unsigned char*) outputbuffer, &output_len,(unsigned char*) file,file_len);//encrypt source file and write it into outputbuffer
	if (encrypt_result == 0){
		printf("failed to update.\n");
		return 0;
	}
	encrypt_result = EVP_EncryptFinal_ex(&en, (unsigned char*) outputbuffer+output_len, &final_len);//encrypt the remaining bit of source file and write it into outputbuffer
	total_len=output_len+final_len;
	if (encrypt_result == 0 || total_len != (blks * BLK_SIZE)){
		printf("failed to final update.\n");
		return 0;
	}
	file_count = total_len/SIZE + 1;
	for(int i=1;i<=file_count;i++){//write outputbuffer into files
		encryptedfilename[strlen(encryptedfilename)-6] = i/10 + '0';
		encryptedfilename[strlen(encryptedfilename)-5] = i%10 + '0';
		encryptedfile = open(encryptedfilename,O_WRONLY|O_CREAT|O_TRUNC,0400|0200);
		int write_len;
		if(i!=file_count)write_len = SIZE;
		else write_len = total_len%SIZE;
		if(write(encryptedfile,outputbuffer+SIZE*(i-1),write_len)!= write_len){
			printf("Error, failed to write encryption to file.\n");
			return 0;
		}
		close(encryptedfile);
	}
	//myAESStorage_set_root(myAESStorage_insert_node(myAESStorage_get_root(),filename,encryptedfilename,decryptedfilename,key,iv,password,password_len,salt,file_count));//Store the decrypt info of this file into its block

	printf("Encryption successed with key ");//encryption successed
	for(int i=0;i<32;i++)
		printf("%c",password[i]);
	printf("\n");
	//set outputstr
	strcat(outputstr,filename);
	strcat(outputstr,",");
	strcat(outputstr,password);
	strcat(outputstr,",");
	strcat(outputstr,salt);
	strcat(outputstr,",");
	char num[3];
	sprintf(num,"%d",file_count);
	strcat(outputstr,num);

	//close file and clear memory
	close(inputfile);
	free(key);
	free(password);
	free(iv);
	free(salt);
	free(outputbuffer);
	EVP_CIPHER_CTX_cleanup(&en);
	return 1;
}

int myAES_Decrypt(char* filename, int type , char *en_password, char *en_salt, int en_file_count){
	EVP_CIPHER_CTX de;
	int final_len = 0, output_len = 0, password_len,encryptedfile, outputfile, file_count, decrypt_result, total_len;
	size_t file_len = 0,last_file_len = 0;
	struct myAES_decryptblock *myAESCrypt;
	
	if(type){//decrypt normal file
		//search the decrypt block in the tree structure
		/*myAESCrypt = myAESStorage_search_node(filename);
		if(myAESCrypt == NULL){
			printf("Error, failed to find the encrypted file.\n");
			return 0;
		}*/
	}else{//decrypt account list
		//check if the account list file exists or not
		int test_accountlist = open(filename,O_RDONLY,0400|0200);
		if(test_accountlist==-1){
			printf("Error, failed to find account list.\n");
			return 0;
		}
		close(test_accountlist);
	}

	//make sure it can find file then allocate memory to variables
	unsigned char *key=(unsigned char *)malloc(sizeof(unsigned char)*32),*iv=(unsigned char *)malloc(sizeof(unsigned char)*16),*password=(unsigned char *)malloc(sizeof(unsigned char)*32),*salt=(unsigned char *)malloc(sizeof(unsigned char)*8), *file, *outputbuffer;
	char encryptedfilename[40],decryptedfilename[30],destr[] = "-de.txt",enstr[] = "-en-00.txt";

	if(type){//decrypt normal file
		//get the decrypt info of the file from its decryption block
		file_count = en_file_count;
		memset(encryptedfilename, 0, 40);
		strncpy(encryptedfilename,filename,strlen(filename)-4);
		strcat(encryptedfilename,enstr);
		memset(decryptedfilename, 0, 30);
		strncpy(decryptedfilename,filename,strlen(filename)-4);
		strcat(decryptedfilename,destr);
		encryptedfilename[strlen(encryptedfilename)-6] = file_count/10 + '0';
		encryptedfilename[strlen(encryptedfilename)-5] = file_count%10 + '0';
		myAES_generate_key_iv(en_password,32,en_salt,key,iv);
		
	}else{//decrypt account list
		memcpy(password,SYSTEM_PASSWORD,KEY_SIZE);		
		memcpy(salt,SYSTEM_SALT,KEY_SIZE/4);
		memset(encryptedfilename, 0, 40);
		strcpy(encryptedfilename,filename);
		memset(decryptedfilename, 0, 30);
		strcpy(decryptedfilename,"Accountlist-de.txt");
		file_count = 1;
		myAES_generate_key_iv(password,strlen(password),salt,key,iv);//generate key and iv by system password and salt
	}
	//get the length of encrypted file
	last_file_len = myAES_get_file_length(encryptedfilename);
	file_len = last_file_len + SIZE*(file_count-1);
	//read file into buffer
	file =(unsigned char *) malloc(sizeof(unsigned char)*file_len);

	for(int i=1;i<=file_count;i++){
		if(type){
			encryptedfilename[strlen(encryptedfilename)-6] = i/10 + '0';
			encryptedfilename[strlen(encryptedfilename)-5] = i%10 + '0';
		}
		int read_len;
		if(i==file_count)read_len = last_file_len;
		else read_len = SIZE;
		myAES_read_file(encryptedfilename,file+SIZE*(i-1),read_len);
	}
		
	outputfile = open(decryptedfilename,O_WRONLY|O_CREAT|O_TRUNC,0400|0200);
	//start of decryption process
	myAES_Decrypt_init(&de,key,iv);
	printf("de use key:%s\n",key);
	printf("de use iv:%s\n",iv);
	outputbuffer =(unsigned char *) malloc(sizeof(unsigned char)*file_len);
	
	decrypt_result = EVP_DecryptUpdate(&de,(unsigned char*) outputbuffer, &output_len,(unsigned char*) file,file_len);//decrypt encrypted file and write it into outputbuffer
	if (decrypt_result == 0){
		printf("Error, failed to update decryption.\n");
		return 0;
	}
	if(write(outputfile,outputbuffer,output_len) != output_len){//write to output file from output buffer
		printf("Error, failed to write decryption to file.\n");
		return 0;
	}
	decrypt_result = EVP_DecryptFinal_ex(&de, (unsigned char*) outputbuffer + output_len, &final_len);//decrypt the remaining bit of encrypted file and write it into outputbuffer
	total_len = output_len + final_len;
	if(decrypt_result == 0 || total_len > file_len){
		printf("Error, failed to update final decryption.\n");
		return 0;
	}
	if(write(outputfile,outputbuffer+output_len,final_len) != final_len){
		printf("Error, failed to write fianl decryption to file.\n");//write the remaining bytes of encrypted file to output file
		return 0;
	}
	
	if(type){
		printf("Decryption successed with key ");	
		for(int i=0;i<32;i++){
			printf("%c",en_password[i]);
		}
		printf("\n");
	}
	
	//close file and clear memory
	close(outputfile);
	free(key);
	free(password);
	free(iv);
	free(file);
	free(outputbuffer);
	EVP_CIPHER_CTX_cleanup(&de);
	return 1;
}

void myAES_generate_new_password(unsigned char* password){
	srand(time(NULL));
	for(int i=0;i<32;i++){
		int tmp = (rand()+getpid())%75 + 48;
		password[i] = (char) tmp;
	}
}

void myAES_generate_new_salt(unsigned char* salt){	
	for(int i=0;i<8;i++){
		int tmp = (rand()+getpid())%75 + 48;
		salt[i] = (char) tmp;
	}
}

size_t myAES_get_file_length(char* filename){
	FILE *tmp = fopen(filename,"r");
	if(tmp!=NULL){
		fseek(tmp, 0, SEEK_END); 
		size_t length = ftell(tmp);
		fclose(tmp);
		return length;
	}
	fclose(tmp);
}

void myAES_read_file(char* filename,char* file,size_t file_len){
	FILE *tmp = fopen(filename,"r");
	if(tmp != NULL){
		fread(file,1,file_len,tmp);
	}
	fclose(tmp);
}

