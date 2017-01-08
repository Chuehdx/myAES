# include <stdlib.h>
# include <stdio.h>
# include <string.h>   
# include <sys/socket.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <arpa/inet.h> 
# include <unistd.h> 
# include <openssl/err.h>
# include "myAESstorage.h" 
# include "myAES.h"

int main(void)
{
	int server_socket, client_socket, c , read_size , server_loop = 1 ,socket_loop = 1;
	struct sockaddr_in server, client;
	char message[67];
	char *user_name,*token,*command_type,*command,*file_name;
	
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
			if((read_size = recv(client_socket , message , sizeof(message) , 0)) > 0 ){//received message from client or TPAserver
				char *copy = malloc(sizeof(message));
				strcpy(copy,message);
				command_type = strsep(&copy,",");//get command type
				if(!strcmp(command_type,"0")){//from TPAserver for registeration
					//split string into smaller parts
					user_name = strsep(&copy,",");
					printf("User name:%s\n",user_name);
					token = strsep(&copy,",");
					printf("Token received from TPAserver:%s\n",token);
					myAESStorage_set_usertoken(user_name,token);//registeration
					write(client_socket , "1", 1);
					socket_loop=0;
					puts("Registeration successed");
					for(int i=0;i<50;i++)printf("-");
					puts("");
				}else if(!strcmp(command_type,"1")){//from client for authentication
					//split string into smaller parts
					user_name = strsep(&copy,",");
					printf("User name:%s\n",user_name);
					token = strsep(&copy,",");
					printf("Token received from client:%s\n",token);

					if(myAESStorage_check_usertoken(user_name,token)){//user name and token matches
						write(client_socket , "1", 1);
						printf("User %s log in successfully with token %s\n",user_name,token);
					}
					else{//user name and token doesn't match
						write(client_socket , "0", 1);
						puts("Error, unauthentiacted user or wrong token");
					}
					for(int i=0;i<50;i++)printf("-");
					puts("");
				}
				else if(!strcmp(command_type,"2")){//from client for encryption or decryption
					//split string into smaller parts
					command = strsep(&copy,",");
					file_name = strsep(&copy,",");
					if(!strcmp(command,"en")){//encryption
						if(!myAES_Encrypt(file_name,1)){
							ERR_print_errors_fp(stderr);
							printf("%s\n","Error, failed to encrypt.");
							write(client_socket , "0", 1);
						}else{
							write(client_socket , "1", 1);
							puts("encrytion successed");
						}
					}else{//decryption
						if(!myAES_Decrypt(file_name,1)){
							ERR_print_errors_fp(stderr);
							printf("%s\n","Error, failed to decrypt.");
							write(client_socket , "0", 1);
						}else{
							write(client_socket , "1", 1);
							puts("decrytion successed");
						}
					}
					myAESStorage_print_storage();
					puts("");
				}else{//from client for exit
					socket_loop=0;
					//server_loop=0;
					write(client_socket , "1", 1);
				}
				free(copy);
			}
		}
	}
    	close(client_socket);
	close(server_socket);
	puts("Storageserver closed");
    	return 0;
}
