# include <stdio.h>
# include <sys/types.h>    
# include <sys/stat.h>    
# include <fcntl.h>
# include <time.h>
# include <stdlib.h>
# include <string.h>
# include "myTPA.h"

static struct Node* head_node = NULL;

int myTPA_insert_node(char *user_name, int user_name_len, char *password, int password_len){
	struct Node *now = head_node, *parent_node = head_node;
	int is_left_node = 1;
	while(now != NULL){
		if(!strcmp(user_name,now->user_name)){
			printf("Error, user name already existed.\n");
			return 0;
		}else if(strcmp(user_name,now->user_name)<0){
			//printf("%s < %s\n",user_name,now->user_name);
			parent_node = now;  
			now = now->left_node;
			is_left_node = 1;
		}else{
			//printf("%s > %s\n",user_name,now->user_name);
			parent_node = now; 
			now = now->right_node;
			is_left_node = 0;
		}
	}
	struct Node *new_node = (struct Node*)malloc(sizeof(struct Node*)+sizeof(char)*(user_name_len+password_len));
	new_node->user_name = (char*)malloc(sizeof(char)*user_name_len);
	new_node->password = (char*)malloc(sizeof(char)*password_len);
	strcpy(new_node->user_name,user_name);
	strcpy(new_node->password,password);
	new_node->left_node = NULL;
	new_node->right_node = NULL;
	if(now == head_node){
		//new_node->height = 0;
		head_node = new_node;
	}else if(is_left_node){
		parent_node->left_node = new_node;
		//new_node->height = parent_node->height + 1;
	}else{
		parent_node->right_node = new_node;
		//new_node->height = parent_node->height + 1;
	}
	//printf("Create account Node successfully.\n");
	return 1;
}

int myTPA_authentication(char *user_name, char *password){
	struct Node *now = head_node;
	while(now != NULL){
		if(!strcmp(user_name,now->user_name)){
			if(!strcmp(password,now->password)){
				printf("Log in successfully.\n");
				return 1;
			}else{
				printf("Error, wrong password.\n");
				return 0;
			}
		}else if(strcmp(user_name,now->user_name)<0){
			//printf("%s < %s\n",user_name,now->user_name);
			now = now->left_node;
		}else{
			//printf("%s > %s\n",user_name,now->user_name);
			now = now->right_node;
		}
	}
	printf("Error, invalid user name.\n");
	return 0;
}

void myTPA_load_account(){
	FILE * file = fopen( "Accountlist.txt" , "r");
	if (file) {
		char user_name[32],password[32];
		while (fscanf(file,"%s %s",user_name,password)!=EOF){
        		printf(" ");
			myTPA_insert_node(user_name,strlen(user_name),password,strlen(password));
		}
	}
	fclose(file);	
}
