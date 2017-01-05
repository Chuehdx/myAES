/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
 
int main(void)
{
    int sock , loop = 1;
    struct sockaddr_in server;
    char user_name[32],password[32], server_reply[32],message[65];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 1231 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //keep communicating with server
    while(loop){
        printf("User name : ");
        scanf("%s" , user_name);
	printf("password : ");
	scanf("%s" , password);

	memset(message,0,sizeof(message));
	strcat(message,user_name);
	strcat(message,",");
	strcat(message,password);

	if(send(sock , message , strlen(message) , 0) < 0){
           	puts("Failed to send user info to server, please try again.");
        }else{
		if(recv(sock , server_reply , sizeof(server_reply) , 0) < 0)
            		puts("Failed to receive respond from server, please try again.");
		else{
			if(!strcmp(server_reply,"1"))
				loop = 0;
			else
				puts("Failed to log in, please try again.");
		}
	}
	
	
	
        //Send some data
        /*if( send(sock , message , strlen(message) , 0) < 0){
            puts("Send failed");
            return 1;
        }
        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0){
            puts("recv failed");
            break;
        }
	if(!strcmp(server_reply,"1")){
		memset(message,0,strlen(message));
		printf("password : ");
		scanf("%s" , message);
		//Send some data
		if( send(sock , message , strlen(message) , 0) < 0){
		    puts("Send failed");
		    return 1;
		}
		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0){
		    puts("recv failed");
		    break;
		}
		if(!strcmp(server_reply,"1"))
			loop = 0;
		else
			puts("Failed to send the user info.");
		
	}else{
		puts("Failed to send the user info.");
	}*/
		
    }
     
    close(sock);
    return 0;
}
