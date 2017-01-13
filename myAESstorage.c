# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include "myAES.h"
# include "myAESstorage.h"

static struct myAES_encryptblock *encryptblock = NULL;
static struct myAES_decryptblock *root = NULL,*first = NULL,*last = NULL;
static char user_token[USER_LIMIT][2][32];
static int user_count = 0;

void myAESStorage_set_encryptblock(unsigned char *key, unsigned char* iv, unsigned char* password, int password_len){
	if(encryptblock == NULL){//initialize
		encryptblock = malloc(sizeof(struct myAES_encryptblock)+sizeof(unsigned char*)*(KEY_SIZE*3/2)+sizeof(char));
		encryptblock->key = malloc(sizeof(unsigned char)*KEY_SIZE);
		encryptblock->iv = malloc(sizeof(unsigned char)*KEY_SIZE/2);
		encryptblock->password = malloc(sizeof(unsigned char)*password_len);
	}
	//set value
	memcpy(encryptblock->key,key,KEY_SIZE);
	memcpy(encryptblock->iv,iv,KEY_SIZE/2);
	memcpy(encryptblock->password,password,password_len);
	encryptblock->password_len = password_len;
}

struct myAES_encryptblock *myAESStorage_get_encryptblock(){
	return encryptblock;
}

void myAESStorage_print_storage(){
	struct myAES_decryptblock *now = first;
	int count = 0;
	for(int i=0;i<50;i++)printf("-");
	printf("\n");
	printf("Key in storage:\n");
	while(now != NULL){
		count = count + 1;
		printf("(%d) %s ",count,now->filename);
		now = now->next;
	}
	printf("\n");	
	for(int i=0;i<50;i++)printf("-");
	printf("\n");	
}

void myAESStorage_set_root(struct myAES_decryptblock *new_root){
	root = new_root;
}

struct myAES_decryptblock *myAESStorage_get_root(){
	return root;
}


int myAESStorage_get_height(struct myAES_decryptblock *node){
	if (node == NULL)
        	return 0;
	return node->height;
}
 

int myAESStorage_max(int a, int b){
	return (a > b)? a : b;
}

struct myAES_decryptblock* myAESStorage_create_node(char* filename, char* encryptedfilename, char* decryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password,int password_len, unsigned char* salt, int file_count){
    	struct myAES_decryptblock* new_node = (struct myAES_decryptblock*) malloc(sizeof(struct myAES_decryptblock)+sizeof(unsigned char)*((KEY_SIZE*3)+password_len));
	strcpy(new_node->filename,filename);
	strcpy(new_node->encryptedfilename,encryptedfilename);
	strcpy(new_node->decryptedfilename,decryptedfilename);
	new_node->key = (unsigned char*)malloc(sizeof(unsigned char)*KEY_SIZE);
	new_node->iv = (unsigned char*)malloc(sizeof(unsigned char)*KEY_SIZE/2);
	new_node->password = (unsigned char*)malloc(sizeof(unsigned char)*password_len);
	new_node->salt = (unsigned char*)malloc(sizeof(unsigned char)*KEY_SIZE/4);
	
	new_node->password_len = password_len;
	new_node->file_count = file_count;
	memcpy(new_node->key,key,KEY_SIZE);	
	memcpy(new_node->iv,iv,KEY_SIZE/2);
	memcpy(new_node->password,password,password_len);
	memcpy(new_node->salt,salt,KEY_SIZE/4);
   	new_node->left   = NULL;
    	new_node->right  = NULL;
	new_node->next 	 = NULL;
    	new_node->height = 1;  // new node is initially added at leaf*/
    	return(new_node);
}

void myAESStorage_update_node(struct myAES_decryptblock* node,char* encryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password, int password_len, unsigned char* salt, int file_count){
	//only update the info that may change
	strcpy(node->encryptedfilename,encryptedfilename);
	node->password_len = password_len;
	node->file_count = file_count;
	memcpy(node->key,key,KEY_SIZE);	
	memcpy(node->iv,iv,KEY_SIZE/2);
	memcpy(node->password,password,password_len);
	memcpy(node->salt,salt,KEY_SIZE/4);
}

struct myAES_decryptblock *myAESStorage_rightRotate(struct myAES_decryptblock *y){
    	struct myAES_decryptblock *x = y->left;
    	struct myAES_decryptblock *T2 = x->right;
 
    	// Perform rotation
    	x->right = y;
    	y->left = T2;
 
	// Update heights
	y->height = myAESStorage_max(myAESStorage_get_height(y->left), myAESStorage_get_height(y->right))+1;
	x->height = myAESStorage_max(myAESStorage_get_height(x->left), myAESStorage_get_height(x->right))+1;
	 
	// Return new root
	return x;
}

struct myAES_decryptblock *myAESStorage_leftRotate(struct myAES_decryptblock *x){
	struct myAES_decryptblock *y = x->right;
	struct myAES_decryptblock *T2 = y->left;
	 
	// Perform rotation
	y->left = x;
	x->right = T2;
	 
	//  Update heights
	x->height = myAESStorage_max(myAESStorage_get_height(x->left), myAESStorage_get_height(x->right))+1;
	y->height = myAESStorage_max(myAESStorage_get_height(y->left), myAESStorage_get_height(y->right))+1;
	 
	// Return new root
	return y;
}

int myAESStorage_getBalance(struct myAES_decryptblock *node){
	if (node == NULL)
        	return 0;
	return myAESStorage_get_height(node->left) - myAESStorage_get_height(node->right);
}

struct myAES_decryptblock* myAESStorage_insert_node(struct myAES_decryptblock* node, char* filename, char* encryptedfilename, char* decryptedfilename, unsigned char *key, unsigned char* iv, unsigned char* password, int password_len, unsigned char* salt, int file_count){
	/* 1.  Perform the normal BST insertion */
	if (node == NULL){//create a new node
		struct myAES_decryptblock* new_node = (myAESStorage_create_node(filename,encryptedfilename, decryptedfilename,key,iv,password,password_len,salt,file_count));
		if(last == NULL)//if it is the first node in the storage
			first = new_node;
		else	//make the last node point to the new node
			last->next=new_node;
		last = new_node;//new node become the last node
		return new_node;
	}	
    	if (strcmp(filename,node->filename)<0)
        	node->left  = myAESStorage_insert_node(node->left,filename,encryptedfilename,decryptedfilename,key,iv,password,password_len,salt,file_count);
    	else if (strcmp(filename,node->filename)>0)
        	node->right = myAESStorage_insert_node(node->right,filename,encryptedfilename,decryptedfilename,key,iv,password,password_len,salt,file_count);
    	else{ // Update existed node in BST
		printf("update %s\n",filename);
		myAESStorage_update_node(node,encryptedfilename,key,iv,password,password_len,salt,file_count);
        	return node;
 	}

    	/* 2. Update height of this ancestor node */
    	node->height = 1 + myAESStorage_max(myAESStorage_get_height(node->left),myAESStorage_get_height(node->right));
 
    	/* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    	int balance = myAESStorage_getBalance(node);
 
    	// If this node becomes unbalanced, then
    	// there are 4 cases
     	// Left Left Case
    	if (balance > 1 && strcmp(filename,node->left->filename)<0)
        	return myAESStorage_rightRotate(node);
 
    	// Right Right Case
    	if (balance < -1 && strcmp(filename,node->right->filename)>0)
        	return myAESStorage_leftRotate(node);
 
    	// Left Right Case
    	if (balance > 1 && strcmp(filename,node->left->filename)>0){
        	node->left =  myAESStorage_leftRotate(node->left);
        	return myAESStorage_rightRotate(node);
    	}
    	// Right Left Case
    	if (balance < -1 && strcmp(filename,node->right->filename)<0){
       		node->right = myAESStorage_rightRotate(node->right);
        	return myAESStorage_leftRotate(node);
    	}
    	/* return the (unchanged) node pointer */
    	return node;
}
 
struct myAES_decryptblock* myAESStorage_search_node(char *filename){
	struct myAES_decryptblock *now = root;
	while(now != NULL){//check if the given node is in the tree structure or not
		if(!strcmp(filename,now->filename)){//find user node
			return now;
		}else if(strcmp(filename,now->filename)<0){
			now = now->left;
		}else{
			now = now->right;
		}
	}
	//node is not in the tree structure
	return now;
}

void myAESStorage_set_usertoken(char *user_name, char* token){
	strcpy(user_token[user_count][0],user_name);
	strcpy(user_token[user_count][1],token);
	user_count = user_count + 1;
}

int myAESStorage_check_usertoken(char *user_name, char* token){
	int count = 0;
	while(count < user_count){
		if(!strcmp(user_token[count][0],user_name)){
			if(!strcmp(user_token[count][1],token)){
				user_count = user_count - 1;
				return 1;
			}else
				return 0;
		}else
			count = count + 1;
	}
	return 0;

}
