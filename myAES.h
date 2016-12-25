# ifndef MYAES_H
# define MYAES_H

#define SIZE 1024
#define TIMEFRAME 5

struct myAES_encryptblock{
	unsigned char *key,*iv,*password;
};

struct myAES_decryptblock{
	unsigned char filename[20],encryptedfilename[30],decryptedfilename[30],*key,*iv,*password;
};


void myAES_Encrypt_init(EVP_CIPHER_CTX *e_ctx, char *key, char *iv);

void myAES_Decrypt_init(EVP_CIPHER_CTX *d_ctx, char *key, char *iv);

int myAES_generate_key_iv(unsigned char* password, unsigned int password_len, unsigned char * salt,unsigned char * key,unsigned char * iv);

int myAES_Encrypt(char* filename, bool changekey);

int myAES_Decrypt(char* filename);

void myAES_generate_new_password(unsigned char* password);

void myAES_generate_new_salt(unsigned char* salt);

#endif
