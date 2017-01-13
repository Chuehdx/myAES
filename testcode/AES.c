#include <openssl/aes.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#define BLK_SIZE 16
#define IP_SIZE 1024
#define OP_SIZE 1024 + EVP_MAX_BLOCK_LENGTH

int do_decrypt(unsigned char *inbuff, unsigned char *outbuf,AES_KEY key,unsigned char *iv, int size){
	/*int olen=0, tlen=0, n=0;
	EVP_CIPHER_CTX  ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(),NULL, key, iv);

	if (EVP_DecryptUpdate(&ctx, outbuf, &olen, inbuff, size) != 1) {
		printf("error in decrypt update\n");
		return 0;
	}
	if (EVP_DecryptFinal_ex(&ctx, outbuf + olen, &tlen) != 1) {
        	perror("error in decrypt final");
        	return 0;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);*/
	AES_cbc_encrypt(inbuff, outbuf, size, &key, iv, AES_DECRYPT);
	return 1;
}

int do_encrypt(unsigned char *inbuff, unsigned char *outbuf,AES_KEY key,unsigned char *iv, int size){
	int olen=0, tlen=0;
	/*EVP_CIPHER_CTX  ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit_ex(&ctx, EVP_aes_256_cbc(),NULL, key, iv);

        if (EVP_EncryptUpdate(&ctx, outbuf, &olen, inbuff, size) != 1) {
		printf("error in encrypt update\n");
		return 0;
        }
    	if (EVP_EncryptFinal_ex(&ctx, outbuf + olen, &tlen) != 1) {
		printf("error in encrypt final\n");
        	return 0;
    	}
	EVP_CIPHER_CTX_cleanup(&ctx);*/
	AES_cbc_encrypt(inbuff, outbuf, size , &key, iv, AES_ENCRYPT);
	return (olen+tlen);
}

size_t get_file_length(char* filename){
	FILE *tmp = fopen(filename,"r");
	if(tmp!=NULL){
		fseek(tmp, 0, SEEK_END);
		size_t length = ftell(tmp);
		fclose(tmp);
		return length;
	}
	fclose(tmp);
}

void read_file(char* filename,char* file,size_t file_len){
	FILE *tmp = fopen(filename,"r");
	if(tmp != NULL){
		fread(file,1,file_len,tmp);
	}
	fclose(tmp);
}

void generate_new_key(unsigned char* key){
	for(int i=0;i<32;i++){
		int tmp = (rand())%75 + 48;
		key[i] = (char) tmp;
	}
}

void generate_new_iv(unsigned char* iv){	
	for(int i=0;i<8;i++){
		int tmp = (rand())%75 + 48;
		iv[i] = (char) tmp;
	}
}

int
main(void){
	char file_name[] = "testfile.txt";
	int file_size,en_size,blks;
	unsigned char key[32], iv[AES_BLOCK_SIZE],*plain,*entext,*detext;
	struct timespec start,end;		//used to record time difference
	double time_start,time_end;
	AES_KEY enc_key, dec_key;

	file_size = get_file_length(file_name);
	blks = (file_size/BLK_SIZE) + 1;
	
	plain = (unsigned char*)malloc(file_size);
	entext = (unsigned char*)malloc(blks*BLK_SIZE);
	detext = (unsigned char*)malloc(blks*BLK_SIZE);
	read_file(file_name,plain,file_size);
	for(int i=0;i<10;i++){
		clock_gettime( CLOCK_MONOTONIC, &start);
		time_start = (double)start.tv_sec + 1.0e-9*start.tv_nsec;	

		generate_new_key(key);
		//generate_new_iv(iv);
		memset(iv, 0x00, AES_BLOCK_SIZE);
		//printf("%d\n",file_size);
		AES_set_encrypt_key(key, 256, &enc_key);
    		do_encrypt(plain, entext,enc_key,iv,file_size);
		AES_set_decrypt_key(key, 256, &dec_key);	
		memset(iv, 0x00, AES_BLOCK_SIZE);	
		do_decrypt(entext,detext,dec_key,iv,en_size);
		//printf("Decrypted text:\n%s\n",detext);
		/*//en_size = do_encrypt(plain, entext,key,iv,file_size);
		if(en_size)
			printf("");
			//printf("Encrypted text:\n%s\n",entext);
		else
			puts("en fail");

		if(do_decrypt(entext,detext,key,iv,en_size))
			printf("");
			//printf("Decrypted text:\n%s\n",detext);
		else 
			puts("de fail");*/
		clock_gettime( CLOCK_MONOTONIC, &end);
		time_end = (double)end.tv_sec + 1.0e-9*end.tv_nsec;
		printf("%lf\n",time_end-time_start);
	}
	free(plain);
	free(entext);
	free(detext);
	return 0;
}

