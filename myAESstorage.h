# ifndef MYAESSTORAGE_H
# define MYAESSTORAGE_H

# define KEY_SIZE 32
# define USER_LIMIT 32
# define SYSTEM_PASSWORD "rdcVEzNO[5j?DUE<1vQpbu`mFNz_5t;p"
# define SYSTEM_SALT "@Px71yP>"

struct myAES_encryptblock{	//This block is used to store key and iv for current encryption process. Password is stored here only to show the difference between different encryption key, since it is hard to tell the difference between key. password_len is used to record the length of password to make sure it copy correctly when passed back to process
	unsigned char *key,*iv,*password;
	int password_len;
};

struct myAES_decryptblock{	//This block is used to store key and iv and the name of each single encrypted file for decryption. Password is stored here only to show that we used the same "key" to decrypt. password_len is used to record the length of password to make sure it  copy correctly when passed back to process
	char filename[20],encryptedfilename[40],decryptedfilename[30];
	unsigned char *key,*iv,*password,*salt;
	int password_len,file_count,height;
	struct myAES_decryptblock *left, *right, *next;
};

void myAESStorage_set_encryptblock(unsigned char *key, unsigned char* iv, unsigned char* password, int password_len);
/*
set the new key iv and password for encryption
*/

struct myAES_encryptblock *myAESStorage_get_encryptblock();
/*
get current encryptblock
*/

void myAESStorage_print_storage();
/*
print the file in storage
*/

void myAESStorage_set_root(struct myAES_decryptblock *new_root);
/*
set the new root of tree structure to the given node
*/

struct myAES_decryptblock *myAESStorage_get_root();
/*
return the current root of tree structure
*/

int myAESStorage_get_height(struct myAES_decryptblock *node);
/*
return the height of the given node
NULL will return 0
*/

int myAESStorage_max(int a, int b);
/*
compare two variables and return the greater one
*/

struct myAES_decryptblock* myAESStorage_create_node(char* filename, char* encryptedfilename, char* decryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password,int password_len, unsigned char* salt, int file_count);
/*
create a new node of decrypt block with the given info
*/

struct myAES_decryptblock *myAESStorage_rightRotate(struct myAES_decryptblock *y);
/*
A utility function to right rotate subtree rooted with y
*/

struct myAES_decryptblock *myAESStorage_leftRotate(struct myAES_decryptblock *x);
/*
A utility function to left rotate subtree rooted with x
*/

int myAESStorage_getBalance(struct myAES_decryptblock *node);
/*
A utility function to get Balance factor of given node
*/

struct myAES_decryptblock* myAESStorage_insert_node(struct myAES_decryptblock* node, char* filename, char* encryptedfilename, char* decryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password, int password_len, unsigned char* salt, int file_count);
/*
insert a new node of decryptblock or update the existing decryptblock depending on it is already in the tree structure or not
*/

struct myAES_decryptblock* myAESStorage_search_node(char *filename);
/*
search the node of the given file in the tree structure and return the decryptblock
*/

void myAESStorage_set_usertoken(char *user_name, char* token);


int myAESStorage_check_usertoken(char *user_name, char* token);

#endif
