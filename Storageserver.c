# include <stdlib.h>
# include <stdio.h>
# include <string.h>   
# include <sys/socket.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <arpa/inet.h> 
# include <unistd.h> 
# include <openssl/err.h>

static char user_token[32][2][32];
static int user_count = 0;

void set_usertoken(char *user_name, char* token){
	strcpy(user_token[user_count][0],user_name);
	strcpy(user_token[user_count][1],token);
	user_count = user_count + 1;
}

int check_usertoken(char *user_name, char* token){
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

int main(void)
{
	int server_socket, client_socket, c , read_size , server_loop = 1 ,socket_loop = 1,list_count=0;
	struct sockaddr_in server, client;
	char message[500],file_list[500][32];
	char *user_name,*token,*command_type,*command,*file_name,*reply = malloc(sizeof(char)),tmp[32];
	
	//Create socket for connection
	server_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (server_socket == -1){
		printf("Could not create socket");
	}
	     
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 1233 );
	     
	//Bind
	if( bind(server_socket,(struct sockaddr *)&server , sizeof(server)) < 0){
		//print the error message
		perror("Error, failed to bind");
		return 1;
	}
	puts("Server create successfully");

	//keep receiving connection from client or TPAserver
	while(server_loop){
		//Listen to max of 5 clients
		listen(server_socket , 5);
		//Accept and incoming connection
		puts("Waiting for incoming connections...");
		c = sizeof(struct sockaddr_in);
		
		//accept connection from client or TPAserver
		client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_socket < 0){
			perror("Error, failed to accept connection");
			return 1;
		}
		puts("New Connection accepted");
		socket_loop = 1;
		while(socket_loop){
			memset(message,0,sizeof(message));//clear buffer
			memset(reply,0,sizeof(reply));
			if((read_size = recv(client_socket , message , sizeof(message) , 0)) > 0 ){//received message from client or TPAserver
				char *copy = malloc(sizeof(message));
				memset(copy,0,sizeof(copy));
				strcpy(copy,message);
				command_type = strsep(&copy,",");//get command type
				if(!strcmp(command_type,"0")){//from TPAserver for registeration
					//split string into smaller parts
					user_name = strsep(&copy,",");
					printf("User name:%s\n",user_name);
					token = strsep(&copy,",");
					printf("Token received from TPAserver:%s\n",token);
					set_usertoken(user_name,token);//registeration
					strcat(reply,"1");
					write(client_socket , reply, sizeof(reply));
					socket_loop=0;
					//free(copy);
					puts("Registeration successed");
					for(int i=0;i<50;i++)printf("-");
					puts("");

				}else if(!strcmp(command_type,"1")){//from client for authentication
					//split string into smaller parts
					user_name = strsep(&copy,",");
					printf("User name:%s\n",user_name);
					token = strsep(&copy,",");
					printf("Token received from client:%s\n",token);
					if(check_usertoken(user_name,token)){//user name and token matches
						strcat(reply,"1");
						write(client_socket ,reply,sizeof(reply));
						printf("User %s log in successfully with token %s\n",user_name,token);
					}else{//user name and token doesn't match
						strcat(reply,"0");
						write(client_socket ,reply,sizeof(reply));
						puts("Error, unauthentiacted user or wrong token");
					}
					free(copy);
					for(int i=0;i<50;i++)printf("-");
					puts("");
				}else if(!strcmp(command_type,"2")){//from client for encryption 
					//split string into smaller parts
					user_name = strsep(&copy,",");
					printf("User name:%s\n",user_name);
					file_name = strsep(&copy,",");
					
					
					//check repeat
					int repeat = 0;
					for(int i=0;i<list_count;i++){
						if(!strcmp(file_list[i],file_name)){
							repeat=1;
							i=list_count;
						}
					}
					if(!repeat){
						strcpy(file_list[list_count],file_name);
						list_count = list_count + 1;
						printf("New file %s upload successfully\n",file_name);
					}else
						printf("File %s update successfully\n",file_name);
					
					strcat(reply,"1");
					write(client_socket,reply,sizeof(reply));
					for(int i=0;i<50;i++)printf("-");
					printf("\n");
					printf("File in storage:\n");
					for(int i=0;i<list_count;i++){
						printf("(%d) %s ",i+1,file_list[i]);
					}
					printf("\n");	
					for(int i=0;i<50;i++)printf("-");	
					puts("\n");
					free(copy);
				}else if(!strcmp(command_type,"3")){//from client decryption
					user_name = strsep(&copy,",");
					printf("User name:%s\n",user_name);
					file_name = strsep(&copy,",");
					int exist=0;
					for(int i=0;i<list_count;i++){
						if(!strcmp(file_list[i],file_name)){
							exist=1;
							i=list_count;
						}
					}
					if(exist){
						printf("Sent back encrypted file %s\n",file_name);
						strcat(reply,"1");
					}else{
						printf("Error, Can not find file %s\n",file_name);
						strcat(reply,"0");
					}
					write(client_socket ,reply,sizeof(reply));
					for(int i=0;i<50;i++)printf("-");
					printf("\n");
					printf("File in storage:\n");
					for(int i=0;i<list_count;i++){
						printf("(%d) %s ",i+1,file_list[i]);
					}
					printf("\n");	
					for(int i=0;i<50;i++)printf("-");	
					puts("\n");
					free(copy);
				}else if(!strcmp(command_type,"4")){//from client for exit
					socket_loop=0;
					//server_loop=0;
					strcat(reply,"1");
					write(client_socket ,reply,sizeof(reply));
					free(copy);
				}
			}
		}
	}
    	close(client_socket);
	close(server_socket);
	puts("Storageserver closed");
    	return 0;
}
