# ifndef myTPA_H
# define myTPA_H

// An AVL tree node
struct Node{
	char user_name[32], password[32];
	struct Node *left, *right;
	int height;
};


int myTPA_get_height(struct Node *node);
/* 
A utility function to get height of the tree
*/

int myTPA_max(int a, int b);
/*
A utility function to get maximum of two integers
*/

struct Node* myTPA_create_node(char *user_name,char *password);
/*
Helper function that allocates a new node with the given key and NULL left and right pointers
*/

struct Node *myTPA_rightRotate(struct Node *y);
/*
A utility function to right rotate subtree rooted with y
*/

struct Node *myTPA_leftRotate(struct Node *x);
/*
A utility function to left rotate subtree rooted with x
*/

int myTPA_getBalance(struct Node *node);
/*
A utility function to get Balance factor of given node
*/

struct Node* myTPA_insert_node(struct Node* node, char *user_name, char *password);
/*
Recursive function to insert key in subtree rooted with node and returns new root of subtree
*/

void myTPA_load_account();
/*
A function to load account info from file and build them as an AVL-tree
*/

int myTPA_authentication(char *user_name, char *password, char *token);
/*
A function to check if the user name is in the tree and its password matches
*/

void preOrder(struct Node *root);
/*
A utility function to print preorder traversal of the tree.
*/
# endif
