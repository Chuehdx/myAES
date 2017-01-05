/*
    C socket server example
*/
#include<stdlib.h>
#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
# include "myTPA.h"
# include "myAESstorage.h" 

int main(void)
{
	int socket_desc , client_sock , c , read_size , authenticated = 0;
	struct sockaddr_in server , client;
	char client_message[65],token[32];
	char *user_name,*password;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1){
		printf("Could not create socket");
	}
	puts("Socket created");
	     
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 1231 );
	     
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	     
	//Listen to max of 3 clients
	listen(socket_desc , 3);
	     
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
	    
	myTPA_load_account();
	puts("Load account list successfully");
    	//Receive a message from client
    	while(!authenticated){
 	memset(client_message,0,sizeof(client_message));//not sure if needed
	if((read_size = recv(client_sock , client_message , sizeof(client_message) , 0)) > 0 ){
		printf("in: %s\n",client_message);
		char *copy = malloc(sizeof(client_message));
		strcpy(copy,client_message);
		user_name = strsep(&copy,",");
		printf("user name:%s.\n",user_name);
		password = strsep(&copy,",");
		printf("password:%s.\n",password);
		free(copy);

		memset(client_message,0,sizeof(client_message));
		if(myTPA_authentication(user_name,password,token)){
			printf("token:%s\n",token);
			authenticated = 1;
		}
		write(client_sock , token, sizeof(token));
	}else
		puts("Failed to receive message from client.");
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
