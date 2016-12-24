# ifndef MYAES_H
# define MYAES_H

# define SIZE 1024
# define TIMEFRAME 5.0

struct AES_encryptblock{
	char password[32],salt[8];
};

struct AES_decryptblock{
	char filename[20],encryptedfilename[30],decryptedfilename[30];
	EVP_CIPHER_CTX *de;
};

int aes_init(unsigned char* password, unsigned int password_len, unsigned char * salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);

int aes_encrypt(char* filename, bool changekey);

int aes_decrypt(char* filename);

void generate_new_password(unsigned char* password);

void generate_new_salt(unsigned char* salt);

#endif
