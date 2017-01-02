# ifndef myTPA_H
# define myTPA_H
struct Node{
	char *user_name,*password;
	struct Node *left_node,*right_node;
};

int myTPA_insert_node(char *user_name, int user_name_len, char *password, int password_len);

int myTPA_authentication(char *user_name, char *password);

void myTPA_load_account();

# endif
