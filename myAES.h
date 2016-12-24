# ifndef MYAES_H
# define MYAES_H
# ifndef SIZE
# define SIZE 1024
# endif

struct AESCrypt{
	char filename[20],encryptedfilename[30],decryptedfilename[30];
	EVP_CIPHER_CTX *de;
};

int aes_init(unsigned char* password, unsigned int password_len, unsigned char * salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);

int aes_encrypt(EVP_CIPHER_CTX *e,int inputfile,int encryptedfile);

int aes_decrypt(EVP_CIPHER_CTX *d, int encryptedfile,int outputfile);

void generate_new_password(unsigned char* password);

void generate_new_salt(unsigned char* salt);

#endif
