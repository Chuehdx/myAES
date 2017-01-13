# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <time.h>
# define SIZE 1000
struct Node{
	char user_name[32], password[32];
	struct Node *left, *right;
	int height;
};

static struct Node *root = NULL;
static struct Node* head_node = NULL;

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
	FILE * file = fopen("testAccountlist.txt","r");//load info from encrypted file
	if(file){
		char user_name[32],password[32];
		while (fscanf(file,"%s %s",user_name,password)!=EOF){//if there are users left
			root = myTPA_insert_node(root,user_name,password);//insert new user node into AVL-tree
		}
	}
	fclose(file);
}

void myTPA_linear_load_account(char userstore[SIZE][32],char passwordstore[SIZE][32]){
	FILE * file = fopen("testAccountlist.txt","r");//load info from encrypted file
	int i=0;
	if(file){
		char user_name[32],password[32];
		while (fscanf(file,"%s %s",user_name,password)!=EOF){//if there are users left
			strcpy(userstore[i],user_name);
			strcpy(passwordstore[i],password);
			//printf("%s\n%s\n",userstore[i],passwordstore[i]);
			i++;
		}
	}
	fclose(file);
}

int myTPA_linear_authentication(char *user_name, char *password,char userstore[SIZE][32],char passwordstore[SIZE][32]){
	for(int i=0;i<SIZE;i++){
		if(!strcmp(user_name,userstore[i])){
			if(!strcmp(password,passwordstore[i])){//password is correct			
				return 1;
			}else{//wrong password
				printf("Error, wrong password.\n");
				return 0;
			}
		}
	}
	printf("Error, invalid user name.\n");
	/*struct Node *now = root;
	while(now != NULL){//check if the user name is in the AVL-tree
		if(!strcmp(user_name,now->user_name)){//find user node
			if(!strcmp(password,now->password)){//password is correct			
				//myAES_generate_new_password(token);//generate authentication token			
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
	printf("Error, invalid user name.\n");*/
	return 0;
}

int myTPA_authentication(char *user_name, char *password){
	struct Node *now = root;
	while(now != NULL){//check if the user name is in the AVL-tree
		if(!strcmp(user_name,now->user_name)){//find user node
			if(!strcmp(password,now->password)){//password is correct			
				//myAES_generate_new_password(token);//generate authentication token			
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

int myTPA_BST_insert_node(char *user_name, char *password){
	struct Node *now = head_node, *parent_node = head_node;
	int is_left_node = 1;
	//printf("now:%s\n",user_name);
	while(now != NULL){
		//printf("%s\n",now->user_name);
		if(!strcmp(user_name,now->user_name)){
			printf("Error, user name already existed.\n");
			return 0;
		}else if(strcmp(user_name,now->user_name)>0){
			//printf("%s < %s\n",user_name,now->user_name);
			parent_node = now;
			now = now->left;
			is_left_node = 1;
		}else{
			//printf("%s > %s\n",user_name,now->user_name);
			parent_node = now; 
			now = now->right;
			is_left_node = 0;
		}
	}
	//printf("findnew\n");
	struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
	strcpy(new_node->user_name,user_name);
	strcpy(new_node->password,password);
	new_node->left = NULL;
	new_node->right = NULL;
	if(now == head_node){
		//new_node->height = 0;
		head_node = new_node;
	}else if(is_left_node){
		parent_node->left = new_node;
		//new_node->height = parent_node->height + 1;
	}else{
		parent_node->right = new_node;
		//new_node->height = parent_node->height + 1;
	}
	//printf("Create account Node successfully.\n");
	return 1;
}

int myTPA_BST_authentication(char *user_name, char *password){
	struct Node *now = head_node;
	while(now != NULL){
		if(!strcmp(user_name,now->user_name)){
			if(!strcmp(password,now->password)){
				//printf("Log in successfully.\n");
				return 1;
			}else{
				printf("Error, wrong password.\n");
				return 0;
			}
		}else if(strcmp(user_name,now->user_name)>0){
			//printf("%s < %s\n",user_name,now->user_name);
			now = now->left;
		}else{
			//printf("%s > %s\n",user_name,now->user_name);
			now = now->right;
		}
	}
	printf("Error, invalid user name.\n");
	return 0;
}

void myTPA_BST_load_account(){
	FILE * file = fopen("testAccountlist.txt","r");//load info from encrypted file
	if(file){
		char user_name[32],password[32];
		while (fscanf(file,"%s %s",user_name,password)!=EOF){//if there are users left
			myTPA_BST_insert_node(user_name,password);//insert new user node into AVL-tree
		}
	}
	fclose(file);
}

int main(void){
	char userstore[SIZE][32],passwordstore[SIZE][32];
	char user[] = "user",number[10],username[32],password[32];
	int num=1;
	struct timespec start,end;		//used to record time difference
	double time_start,time_end;
	/*int i=1;
	FILE * file = fopen("testAccountlist.txt","w");//load info from encrypted file
	if (file) {
		while(i<=SIZE){
			//printf("%d\n",i);
			fprintf(file,"user%d\n%d\n",i,i);
			i = i + 1;		
		}
	}
	fclose(file);*/
	myTPA_BST_load_account();
	//myTPA_linear_load_account(userstore,passwordstore);
	
	srand(time(NULL));
	for(int i=0;i<10;i++){
		num=1;
		clock_gettime( CLOCK_MONOTONIC, &start);
		time_start = (double)start.tv_sec + 1.0e-9*start.tv_nsec;
		for(int k=0;k<SIZE;k++){
			memset(number,0,10);
			memset(username,0,32);
			memset(password,0,32);
			strcat(username,user);
			//num = rand()%500 + 1;
			sprintf(number, "%d", num);
			strcat(username,number);
			//myTPA_authentication(username,number);
			//myTPA_linear_authentication(username,number,userstore,passwordstore);
			myTPA_BST_authentication(username,number);
			//printf("%s\n%s\n",username,number);
			num = num + 1;
		}
		clock_gettime( CLOCK_MONOTONIC, &end);
		time_end = (double)end.tv_sec + 1.0e-9*end.tv_nsec;	
		printf("%.9lf\n",1000000*(time_end-time_start)/SIZE);
	}
	return 0;
}

