# ifndef MYAES_H
# define MYAES_H
# ifndef SIZE
# define SIZE 1024
# endif

struct AESCrypt{
	char filename[20],encryptedfilename[30],decryptedfilename[30];
	EVP_CIPHER_CTX *en,*de;
};

int aes_init(unsigned char* password, unsigned int password_len, unsigned char * salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);

int aes_encrypt(char* filename);

int aes_decrypt(char* filename);

void generate_new_password(unsigned char* password);

void generate_new_salt(unsigned char* salt);

#endif
