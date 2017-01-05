# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include "myTPA.h"
# include "myAES.h"
# include "myAESstorage.h"

static struct Node *root = NULL;

int myTPA_get_height(struct Node *node){
	if (node == NULL)
        	return 0;
	return node->height;
}
 

int myTPA_max(int a, int b){
	return (a > b)? a : b;
}

struct Node* myTPA_create_node(char *user_name,char *password){
    	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    	strcpy(new_node->user_name,user_name);
    	strcpy(new_node->password,password);
   	new_node->left   = NULL;
    	new_node->right  = NULL;
    	new_node->height = 1;  // new node is initially added at leaf
    	return(new_node);
}

struct Node *myTPA_rightRotate(struct Node *y){
    	struct Node *x = y->left;
    	struct Node *T2 = x->right;
 
    	// Perform rotation
    	x->right = y;
    	y->left = T2;
 
	// Update heights
	y->height = myTPA_max(myTPA_get_height(y->left), myTPA_get_height(y->right))+1;
	x->height = myTPA_max(myTPA_get_height(x->left), myTPA_get_height(x->right))+1;
	 
	// Return new root
	return x;
}
 

struct Node *myTPA_leftRotate(struct Node *x){
	struct Node *y = x->right;
	struct Node *T2 = y->left;
	 
	// Perform rotation
	y->left = x;
	x->right = T2;
	 
	//  Update heights
	x->height = myTPA_max(myTPA_get_height(x->left), myTPA_get_height(x->right))+1;
	y->height = myTPA_max(myTPA_get_height(y->left), myTPA_get_height(y->right))+1;
	 
	// Return new root
	return y;
}

int myTPA_getBalance(struct Node *node){
	if (node == NULL)
        	return 0;
	return myTPA_get_height(node->left) - myTPA_get_height(node->right);
}

struct Node* myTPA_insert_node(struct Node* node, char *user_name, char *password){
	/* 1.  Perform the normal BST insertion */
	if (node == NULL)
		return(myTPA_create_node(user_name,password));
    	if (strcmp(user_name,node->user_name)<0)
        	node->left  = myTPA_insert_node(node->left,user_name,password);
    	else if (strcmp(user_name,node->user_name)>0)
        	node->right = myTPA_insert_node(node->right,user_name,password);
    	else // Equal keys are not allowed in BST
        	return node;
 
    	/* 2. Update height of this ancestor node */
    	node->height = 1 + myTPA_max(myTPA_get_height(node->left),myTPA_get_height(node->right));
 
    	/* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    	int balance = myTPA_getBalance(node);
 
    	// If this node becomes unbalanced, then
    	// there are 4 cases
     	// Left Left Case
    	if (balance > 1 && strcmp(user_name,node->left->user_name)<0)
        	return myTPA_rightRotate(node);
 
    	// Right Right Case
    	if (balance < -1 && strcmp(user_name,node->right->user_name)>0)
        	return myTPA_leftRotate(node);
 
    	// Left Right Case
    	if (balance > 1 && strcmp(user_name,node->left->user_name)>0){
        	node->left =  myTPA_leftRotate(node->left);
        	return myTPA_rightRotate(node);
    	}
    	// Right Left Case
    	if (balance < -1 && strcmp(user_name,node->right->user_name)<0){
       		node->right = myTPA_rightRotate(node->right);
        	return myTPA_leftRotate(node);
    	}
    	/* return the (unchanged) node pointer */
    	return node;
}
 
void myTPA_load_account(){
	myAES_Decrypt("Accountlist.txt",0);//decrypt the account list and write it to encrypted file
	FILE * file = fopen("Accountlist-de.txt","r");//load info from encrypted file
	if (file) {
		char user_name[32],password[32];
		while (fscanf(file,"%s %s",user_name,password)!=EOF){//if there are users left
			root = myTPA_insert_node(root,user_name,password);//insert new user node into AVL-tree
		}
	}
	fclose(file);
	remove("Accountlist-de.txt");//delete encrypted file
}

int myTPA_authentication(char *user_name, char *password, char *token){
	struct Node *now = root;
	while(now != NULL){//check if the user name is in the AVL-tree
		if(!strcmp(user_name,now->user_name)){//find user node
			if(!strcmp(password,now->password)){//password is correct
				myAES_generate_new_password((unsigned char*)token);//generate authentication token
				myAESStorage_set_usertoken(user_name,token);			
				return 1;
			}else{//wrong password
				printf("Error, wrong password.\n");
				return 0;
			}
		}else if(strcmp(user_name,now->user_name)<0){
			now = now->left;
		}else{
			now = now->right;
		}
	}
	printf("Error, invalid user name.\n");
	return 0;
}

void preOrder(struct Node *root){
    if(root != NULL)
    {
        printf("user:%s\n", root->user_name);
	printf("pw:%s\n", root->password);
        preOrder(root->left);
        preOrder(root->right);
    }
}

