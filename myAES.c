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


int aes_init(unsigned char* password, unsigned int password_len, unsigned char * salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx){
	int result, round = 5;
	unsigned char key[32], iv[32];

	result = EVP_BytesToKey(EVP_aes_256_cbc(),EVP_sha1(),salt,password,password_len,round,key,iv);
	if(result != 32){
		printf("%s\n","failed to generate key.");
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

int aes_encrypt(EVP_CIPHER_CTX *e,int inputfile,int encryptedfile){
	char inputbuffer[SIZE];
	char outputbuffer[SIZE+AES_BLOCK_SIZE];
	int input_len = 0, final_len = 0, output_len = 0;
	if(!EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL)){ /*int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type,
        ENGINE *impl, unsigned char *key, unsigned char *iv);
	should add key here
*/
		printf("%s\n","Error, failed to init encryption.");
		return 1;
	}
	while((input_len = read(inputfile,inputbuffer,SIZE)) > 0){ // read return bytes read, 0:EOF, -1:error
		//printf("%d\n",input_len);
		if(!EVP_EncryptUpdate(e,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		/* Update cipher text */
			printf("%s\n","Error, failed to update encryption.");
			return 1;
		}if(write(encryptedfile,outputbuffer,output_len) != output_len){
			printf("%s\n","Error, failed to write encryption to file.");
			return 1;
		}	
	}
	//printf("%d end of enc\n",input_len);
	if(!EVP_EncryptFinal_ex(e, (unsigned char*) outputbuffer, &final_len)){			/* updates the remaining bytes */
		printf("%s\n","Error, failed to update final encryption.");
		return 1;
	}
	if(write(encryptedfile,outputbuffer,final_len) != final_len){
		printf("%s\n","Error, failed to write fianl encryption to file.");
		return 1;
	}
	return 0;
}

int aes_decrypt(EVP_CIPHER_CTX *d, int encryptedfile,int outputfile){
	char inputbuffer[SIZE+AES_BLOCK_SIZE];
	char outputbuffer[SIZE+AES_BLOCK_SIZE];
	int input_len = 0, final_len = 0, output_len = 0;
	//EVP_CIPHER_CTX_set_padding(d, 8);
	if(!EVP_DecryptInit_ex(d, NULL, NULL, NULL, NULL)) {
		printf("%s\n","Error, failed to init decryption.");
		return 1;
	}
	while((input_len = read(encryptedfile,inputbuffer,SIZE)) > 0){ // read return bytes read, 0:EOF, -1:error
		//printf("%d\n",input_len);
		if(!EVP_DecryptUpdate(d,(unsigned char*) outputbuffer, &output_len,(unsigned char*) inputbuffer,input_len)){ 		/* Update cipher text */
			printf("%s\n","Error, failed to update decryption.");
			return 1;
		}if(write(outputfile,outputbuffer,output_len) != output_len){
			printf("%s\n","Error, failed to write decryption to file.");
			return 1;
		}	
	}
	//printf("%d\n",input_len);
	if(!EVP_DecryptFinal_ex(d, (unsigned char*) outputbuffer, &final_len)){			/* updates the remaining bytes */
		printf("%s\n","Error, failed to update final decryption.");
		return 1;
	}
	if(write(outputfile,outputbuffer,final_len) != final_len){
		printf("%s\n","Error, failed to write fianl decryption to file.");
		return 1;
	}
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

