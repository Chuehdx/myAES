# include <stdio.h> 
# include <unistd.h>
# include <string.h>    
# include <sys/socket.h> 
# include <arpa/inet.h>
# include "myAESstorage.h"

int main(void){
    	int socket_to_storage, loop = 1;
    	struct sockaddr_in Storageserver;
    	char user_name[32], token[32], out_message[67],storage_message[32];
	
	//create socket for new connection
	socket_to_storage = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_to_storage == -1){
		printf("Could not create socket");
	}
	//setup info about Storageserver
	Storageserver.sin_addr.s_addr = inet_addr("127.0.0.1");
	Storageserver.sin_family = AF_INET;
	Storageserver.sin_port = htons( 1233 );

	//Connect to Storageserver
	if (connect(socket_to_storage , (struct sockaddr *)&Storageserver , sizeof(Storageserver)) < 0){
		perror("Error, Established connection with Storageserver");
	}else 
		puts("Established connection with Storageserver");
	
	while(loop){
		printf("User name:");
		scanf("%s",user_name);
		printf("Token:");
		scanf("%s",token);
		
		//prepare the authentication token used to enter Storageserver
		memset(out_message,0,sizeof(out_message));
		strcat(out_message,"1");
		strcat(out_message,",");
		strcat(out_message,user_name);
		strcat(out_message,",");
		strcat(out_message,token);

		//send the authentication token to Storageserver
		write(socket_to_storage,out_message,sizeof(out_message));

		//wait for respond from Storageserver
		memset(storage_message,0,sizeof(storage_message));
		while(recv(socket_to_storage , storage_message , sizeof(storage_message) , 0) < 0);
		if(!strcmp(storage_message,"1"))//token and user name matches
			puts("Log in to Storageserver successfully");
		else
			puts("Error, failed to Log in to Storageserver");

		for(int i=0;i<50;i++)printf("-");
		puts("");
	}
	//close socket
	close(socket_to_storage);
    	return 0;
}
