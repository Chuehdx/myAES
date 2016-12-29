# ifndef AESSTORAGE_H
# define AESSTORAGE_H

# define STORAGE_SIZE 20
# define KEY_SIZE 32

void myAESStorage_store_decryptblock(char* filename, char* encryptedfilename, char* decryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password,int password_len ,int file_pos, int file_count);
/*
store the decrypt info of the file into its block
*/


void myAESStorage_set_encryptblock(unsigned char *key, unsigned char* iv, unsigned char* password, int password_len);
/*
set the new key iv and password for encryption
*/

struct myAES_encryptblock *myAESStorage_get_encryptblock();
/*
get current encryptblock
*/

int myAESStorage_get_number_of_storage();
/*
return the number of file in storage now
*/

void myAESStorage_print_storage();
/*
print the file int storage
*/

int myAESStorage_find_file_position(char* filename);
/*
find the given file and return its position
if the given file isn't in storage it will return STORAGE_SIZE
*/

struct myAES_decryptblock *myAESStorage_get_decryptblock(int i);
/*
get the decryptblock on position i
*/
#endif
