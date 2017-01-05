/*
    C socket server example
*/
#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
 
int main(void)
{
    int socket_desc , client_sock , c , read_size , authenticated = 0;
    struct sockaddr_in server , client;
    char client_message[2000];
    char user_name[32],password[32];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 1231 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
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
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
    
    //Receive a message from client
    while(!authenticated){
 	memset(client_message,0,strlen(client_message));
	if((read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ){
		strcpy(user_name,client_message);
		printf("user name:%s\n",user_name);
		write(client_sock , "1" , 1);
	}
	memset(client_message,0,strlen(client_message));
	if((read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ){
		strcpy(password,client_message);
		printf("password:%s\n",password);
		if(!strcmp(user_name,"tommy")&&!strcmp(password,"820105")){
			authenticated = 1;
			write(client_sock , "1" , 1);
		}else
			write(client_sock , "0" , 1);
		
	}
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
