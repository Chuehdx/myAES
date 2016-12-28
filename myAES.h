# ifndef MYAES_H
# define MYAES_H

#define SIZE 1024
#define BLK_SIZE 16
#define TIMEFRAME 5.0

struct myAES_encryptblock{	//This block is used to store key and iv for current encryption process. Password is stored here only to show the difference between different encryption key, since it is hard to tell the difference between key.
	unsigned char *key,*iv,*password;
};

struct myAES_decryptblock{	//This block is used to store key and iv and the name of each single encrypted file for decryption. Password is stored here only to show that we used the same "key" to decrypt.
	char filename[20],encryptedfilename[30],decryptedfilename[30];
	unsigned char *key,*iv,*password;
};


void myAES_Encrypt_init(EVP_CIPHER_CTX *e_ctx, char *key, char *iv);
/*
Using given key and iv to create and init a new encrypt cipher block.
*/

void myAES_Decrypt_init(EVP_CIPHER_CTX *d_ctx, char *key, char *iv);
/*
Using given key and iv to create and init a new decrypt cipher block.
*/

int myAES_generate_key_iv(unsigned char* password, unsigned char * salt,unsigned char * key,unsigned char * iv);
/*
Using given password and salt to create new key and iv.
EVP_BytesToKey will turning password into unreadable key
salt is used to increase the security of the key
*/

int myAES_Encrypt(char* filename, int changekey, int retry);
/*
main encryption process
change key is used to tell if we need to generate a new key and iv.
the function we are using here is EVP_Encrypt_ex provide by Openssl library.
we encrypt the plain text here and store cipher into certain file.
*/

int myAES_Decrypt(char* filename);
/*
main decryption process
We use the info in myAES_decryptblock to find the cipher file first.
Then we use EVP_Decrypt_ex to decrypt cipher into plain text and store it in another file.
*/

void myAES_generate_new_password(unsigned char* password);
/*
generate new password consisted of A-Z and 0-9 by rand()
*/

void myAES_generate_new_salt(unsigned char* salt);
/*
generate new salt consisted of a-z by rand()
*/

#endif
