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

int main(void){
	EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new(),*de = EVP_CIPHER_CTX_new();
	unsigned char password[32];
	unsigned int password_len;
	unsigned char salt[8];
	char filename[20],encryptedfilename[30],decryptedfilename[30];
	printf("%s","Input the name of file to encrypt: ");
	scanf("%s",filename);
	int inputfile, encryptedfile, outputfile;
	
	inputfile = open(filename,O_RDONLY);

	strncpy(encryptedfilename,filename,strlen(filename)-4);
	char entmpstr[] = "-en.txt";
	strcat(encryptedfilename,entmpstr);
	printf("%s\n",encryptedfilename);

	strncpy(decryptedfilename,filename,strlen(filename)-4);
	char detmpstr[] = "-de.txt";
	strcat(decryptedfilename,detmpstr);
	printf("%s\n",decryptedfilename);

	encryptedfile = open(encryptedfilename,O_RDWR|O_CREAT,0400|0200);
	outputfile = open(decryptedfilename,O_RDWR|O_CREAT,0400|0200);
	//encryptedfile = open("enc.txt",O_RDWR|O_CREAT,0400|0200);	
	//outputfile = open("out.txt",O_RDWR|O_CREAT,0400|0200);

	AEScrypt_init();
	srand(time(NULL));
	generate_new_password(password);
	password_len = strlen((const char *)password);
	//password_len=sizeof(password)/ sizeof(password[0]);
	//printf("password_len:%d\n",password_len);
	generate_new_salt(salt);


	if(aes_init(password,password_len,(unsigned char*) salt,en,de)){
		printf("%s\n","Error, failed to initialize key and IV.");
		return -1;
	}
	AEScrypt_store(filename,encryptedfilename,decryptedfilename,de);
	
	if(aes_encrypt(en,inputfile,encryptedfile)){
		printf("%s\n","Error, failed to encrypt.");
		exit(-1);
	}else{	
		close(inputfile);
	}
	
	struct AESCrypt *myAESCrypt = AESCrypt_load("input1.txt");
	encryptedfile = open(myAESCrypt->encryptedfilename,O_RDWR|O_CREAT,0400|0200);
		//encryptedfile = myAESCrypt->encryptedfile;
		//outputfile = myAESCrypt->outputfile;
	outputfile = open(myAESCrypt->decryptedfilename,O_RDWR|O_CREAT,0400|0200);		
	de = myAESCrypt->de;
		//encryption successed
	if((lseek(encryptedfile,0,SEEK_SET)) != 0){	
		printf("%s\n","Error, failed to seek encrypted file.");
		exit(-1);
	}
	if(aes_decrypt(de,encryptedfile,outputfile)){
		ERR_print_errors_fp(stderr);
		printf("%s\n","Error, failed to decrypt.");
		exit(-1);
	}
	close(encryptedfile);
	close(outputfile);
		
	EVP_CIPHER_CTX_cleanup(en);
	EVP_CIPHER_CTX_cleanup(de);
	return 0;
}
