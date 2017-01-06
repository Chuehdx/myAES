/*
    C socket server example
*/
# include <stdlib.h>
# include <stdio.h>
# include <string.h>    //strlen
# include <sys/socket.h>
# include <arpa/inet.h> //inet_addr
# include <unistd.h>    //write
# include "myTPA.h"
# include "myAESstorage.h" 

int main(void)
{
	int socket_for_client, socket_to_storage , client_sock , c , read_size , authenticated = 0;
	struct sockaddr_in TPAserver, Storageserver,client;
	char client_message[65],token[32],message_to_storage[67],message_from_storage[32];
	char *user_name,*password;

	//Create socket for client
	socket_for_client = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_for_client == -1){
		printf("Could not create socket");
	}
	puts("Socket created");
	     
	//Prepare the sockaddr_in structure
	TPAserver.sin_family = AF_INET;
	TPAserver.sin_addr.s_addr = INADDR_ANY;
	TPAserver.sin_port = htons( 1231 );
	     
	//Bind
	if( bind(socket_for_client,(struct sockaddr *)&TPAserver , sizeof(TPAserver)) < 0){
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	 

	//connect to Storageserver
	socket_to_storage = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_to_storage == -1){
		printf("Could not create socket");
	}
	puts("Socket created");
	Storageserver.sin_addr.s_addr = inet_addr("127.0.0.1");
	Storageserver.sin_family = AF_INET;
	Storageserver.sin_port = htons( 1233 );

	//Connect to Storageserver
	if (connect(socket_to_storage , (struct sockaddr *)&Storageserver , sizeof(Storageserver)) < 0){
		perror("connect failed. Error");
	//return 1;
	}else 
		puts("Connected\n");

	//Listen to max of 3 clients
	listen(socket_for_client , 3);
	     
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	     
	//accept connection from an incoming client
	client_sock = accept(socket_for_client, (struct sockaddr *)&client, (socklen_t*)&c);
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
			printf("user name:%s\n",user_name);
			password = strsep(&copy,",");
			printf("password:%s\n",password);
			free(copy);

			memset(client_message,0,sizeof(client_message));
			if(myTPA_authentication(user_name,password,token)){
				printf("token:%s\n",token);
				authenticated = 1;
		
				//send message
				memset(message_to_storage,0,sizeof(message_to_storage));//not sure if needed
				strcat(message_to_storage,"0");
				strcat(message_to_storage,",");
				strcat(message_to_storage,user_name);
				strcat(message_to_storage,",");
				strcat(message_to_storage,token);
				write(socket_to_storage , message_to_storage, sizeof(message_to_storage));
		
				//wait for respond
				memset(message_from_storage,0,sizeof(message_from_storage));
				while((read_size = recv(socket_to_storage , message_from_storage , sizeof(message_from_storage) , 0)) < 0);
				if(!strcmp(message_from_storage,"1"))
					puts("register successed");
				else
					puts("register failed");
			}
			write(client_sock , token, sizeof(token));
		}else
			puts("Failed to receive message from client.");
	}
   	//close(socket_to_storage);
    	if(read_size == 0)
    	{
        	puts("Client disconnected");
       		fflush(stdout);
    	}else if(read_size == -1){
        	perror("recv failed");
    	}
        return 0;
}
