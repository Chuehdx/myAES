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


int aes_init(unsigned char* password, unsigned int password_len, unsigned char * salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx){
	int result, round = 5;
	unsigned char key[32], iv[32];

	result = EVP_BytesToKey(EVP_aes_256_cbc(),EVP_sha1(),salt,password,password_len,round,key,iv);
	if(result != 32){
		printf("failed to generate key.\n");
		return -1;	
	}
	/*for(int i=0;i<32;i++)
		printf("%c",key[i]);
	printf("\n");*/
	EVP_CIPHER_CTX_init(e_ctx);
	EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
	EVP_CIPHER_CTX_init(d_ctx);
        EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
	return 0;
}

int aes_encrypt(char* filename){//(EVP_CIPHER_CTX *e,int inputfile,int encryptedfile){
	EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new(),*de = EVP_CIPHER_CTX_new();
	unsigned char password[32],salt[8];
	unsigned int password_len;
	char encryptedfilename[30],decryptedfilename[30];	
	char inputbuffer[SIZE],outputbuffer[SIZE+AES_BLOCK_SIZE];
	int input_len = 0, final_len = 0, output_len = 0, inputfile, encryptedfile;

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

	encryptedfile = open(encryptedfilename,O_RDWR|O_CREAT,0400|0200);
	srand(time(NULL));
	generate_new_password(password);
	password_len = strlen((const char *)password);
	//password_len=sizeof(password)/ sizeof(password[0]);
	//printf("password_len:%d\n",password_len);
	generate_new_salt(salt);

	//aesencryption init process	
	if(aes_init(password,password_len,(unsigned char*) salt,en,de)){
		printf("Error, failed to initialize key and IV.\n");
		return -1;
	}
	//start of encryption process
	
	if(!EVP_EncryptInit_ex(en, NULL, NULL, NULL, NULL)){ /*int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type,
        ENGINE *impl, unsigned char *key, unsigned char *iv);
	should add key here
*/
		printf("Error, failed to init encryption.\n");
		return 1;
	}
	while((input_len = read(inputfile,inputbuffer,SIZE)) > 0){ // read return bytes read, 0:EOF, -1:error
		//printf("%d\n",input_len);
		if(!EVP_EncryptUpdate(en,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		/* Update cipher text */
			printf("Error, failed to update encryption.\n");
			return 1;
		}if(write(encryptedfile,outputbuffer,output_len) != output_len){
			printf("Error, failed to write encryption to file.\n");
			return 1;
		}	
	}
	//printf("%d end of enc\n",input_len);
	if(!EVP_EncryptFinal_ex(en, (unsigned char*) outputbuffer, &final_len)){			/* updates the remaining bytes */
		printf("Error, failed to update final encryption.\n");
		return 1;
	}
	if(write(encryptedfile,outputbuffer,final_len) != final_len){
		printf("Error, failed to write fianl encryption to file.\n");
		return 1;
	}
	AEScrypt_store(filename,encryptedfilename,decryptedfilename,de);
	close(inputfile);
	close(encryptedfile);
	return 0;
}

int aes_decrypt(char* filename){//(EVP_CIPHER_CTX *d, int encryptedfile,int outputfile){
	char inputbuffer[SIZE+AES_BLOCK_SIZE];
	char outputbuffer[SIZE+AES_BLOCK_SIZE];
	int input_len = 0, final_len = 0, output_len = 0, encryptedfile, outputfile;
	
	struct AESCrypt *myAESCrypt = AESCrypt_load(filename);
	
	encryptedfile = open(myAESCrypt->encryptedfilename,O_RDONLY);
	if((lseek(encryptedfile,0,SEEK_SET)) != 0){	
		printf("Error, failed to seek encrypted file.\n");
		return 1;
	}
	outputfile = open(myAESCrypt->decryptedfilename,O_RDWR|O_CREAT,0400|0200);

	//start decryption
	//EVP_CIPHER_CTX_set_padding(d, 8);
	if(!EVP_DecryptInit_ex(myAESCrypt->de, NULL, NULL, NULL, NULL)) {
		printf("Error, failed to init decryption.\n");
		return 1;
	}
	while((input_len = read(encryptedfile,inputbuffer,SIZE)) > 0){ // read return bytes read, 0:EOF, -1:error
		//printf("%d\n",input_len);
		if(!EVP_DecryptUpdate(myAESCrypt->de,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		/* Update cipher text */
			printf("Error, failed to update decryption.\n");
			return 1;
		}if(write(outputfile,outputbuffer,output_len) != output_len){
			printf("Error, failed to write decryption to file.\n");
			return 1;
		}	
	}
	//printf("%d\n",input_len);
	if(!EVP_DecryptFinal_ex(myAESCrypt->de, (unsigned char*) outputbuffer, &final_len)){			/* updates the remaining bytes */
		printf("Error, failed to update final decryption.\n");
		return 1;
	}
	if(write(outputfile,outputbuffer,final_len) != final_len){
		printf("Error, failed to write fianl decryption to file.\n");
		return 1;
	}
	close(encryptedfile);
	close(outputfile);
	return 0;
}

void generate_new_password(unsigned char* password){
	printf("password : ");	
	for(int i=0;i<32;i++){
		int tmp = rand()%75 + 48;
		password[i] = (char) tmp;
		printf("%c",password[i]);	
	}
	printf("\n");	
}

void generate_new_salt(unsigned char* salt){
	printf("salt : ");	
	for(int i=0;i<8;i++){
		int tmp = rand()%26 + 97;
		salt[i] = (char) tmp;
		printf("%c",salt[i]);
	}
	printf("\n");
}

