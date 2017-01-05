# ifndef MYAES_H
# define MYAES_H

#define SIZE 1024
#define BLK_SIZE 16
#define TIMEFRAME 0

void myAES_Encrypt_init(EVP_CIPHER_CTX *e_ctx, char *key, char *iv);
/*
Using given key and iv to create and init a new encrypt cipher block.
*/

void myAES_Decrypt_init(EVP_CIPHER_CTX *d_ctx, char *key, char *iv);
/*
Using given key and iv to create and init a new decrypt cipher block.
*/

int myAES_generate_key_iv(unsigned char* password, int password_len, unsigned char * salt,unsigned char * key,unsigned char * iv);
/*
Using given password and salt to create new key and iv.
EVP_BytesToKey will turning password into unreadable key
salt is used to increase the security of the key
*/

int myAES_Encrypt(char* filename, int changekey);
/*
main encryption process
change key is used to tell if we need to generate a new key and iv.
the function we are using here is EVP_Encrypt_ex provide by Openssl library.
we encrypt the plain text here and store cipher into certain file.
*/

int myAES_Decrypt(char* filename, int type);
/*
main decryption process
We use the info in myAES_decryptblock to find the cipher file first.
Then we use EVP_Decrypt_ex to decrypt cipher into plain text and store it in another file.
type 0 for decrypting account list
type 1 for decrypting normal file
*/

void myAES_generate_new_password(unsigned char* password);
/*
generate new password consisted of A-Z and 0-9 by rand()
*/

void myAES_generate_new_salt(unsigned char* salt);
/*
generate new salt consisted of a-z by rand()
*/

size_t myAES_get_file_length(char* filename);
/*
return the size of file
*/

void myAES_read_file(char* filename,char* file,size_t file_len);
/*
read the content of file and write it into buffer.
*/
#endif
