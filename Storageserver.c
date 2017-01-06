/*
    C socket server example
*/
# include <stdlib.h>
# include <stdio.h>
# include <string.h>    //strlen
# include <sys/socket.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <arpa/inet.h> //inet_addr
# include <unistd.h>    //write
# include "myAESstorage.h" 
//# include "myAES.h"

int main(void)
{
	int socket_desc , client_sock, c , read_size , server_loop = 1 ,socket_loop = 1;
	struct sockaddr_in server , client;
	char message[67];
	char *user_name,*token,*client_type;
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1){
		printf("Could not create socket");
	}
	puts("Socket created");
	     
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 1233 );
	     
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	     
	
	     
	while(server_loop){
		//Listen to max of 5 clients
		listen(socket_desc , 5);
		//Accept and incoming connection
		puts("Waiting for incoming connections...");
		c = sizeof(struct sockaddr_in);
		
		//accept connection from an incoming client
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0){
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");
		while(socket_loop){
	memset(message,0,sizeof(message));//not sure if needed
			if((read_size = recv(client_sock , message , sizeof(message) , 0)) > 0 ){
				printf("in: %s\n",message);
				char *copy = malloc(sizeof(message));
				strcpy(copy,message);
				client_type = strsep(&copy,",");
				printf("client type:%s\n",client_type);
				if(!strcmp(client_type,"0")){
					user_name = strsep(&copy,",");
					printf("user_name:%s\n",user_name);
					token = strsep(&copy,",");
					printf("token:%s\n",token);
					myAESStorage_set_usertoken(user_name,token);
					write(client_sock , "1", 1);
					socket_loop=0;
				}else{
					//
				}
				free(copy);
			}
		}
		/*else
		puts("Failed to receive message from client.");*/
		puts("not waiting");
	}
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
     
    return 0;
}
