# include <stdio.h> 
# include <unistd.h>
# include <string.h>    
# include <sys/socket.h> 
# include <arpa/inet.h>
# include "myAESstorage.h"

int main(void){
    	int socket_to_TPA ,socket_to_storage, loop = 1;
    	struct sockaddr_in TPAserver,Storageserver;
    	char user_name[32], password[32], token[32], out_message[67],command[5],file_name[20],storage_message[32];
	
    	//create socket for connection
    	socket_to_TPA = socket(AF_INET , SOCK_STREAM , 0);
    	if (socket_to_TPA == -1){
        	printf("Could not create socket");
    	}

	//setup info about TPAserver
    	TPAserver.sin_addr.s_addr = inet_addr("127.0.0.1");
    	TPAserver.sin_family = AF_INET;
    	TPAserver.sin_port = htons( 1231 );
 
    	//Connect to TPAserver
    	if (connect(socket_to_TPA , (struct sockaddr *)&TPAserver , sizeof(TPAserver)) < 0){
        	perror("Error, failed to connect to TPAserver");
        	return 1;
    	}
    	puts("Established connection with TPAserver");
    
    	//keep communicating with TPAserver
    	while(loop){
		//get user name and password
		printf("User name:");
		scanf("%s", user_name);
		printf("Password:");
		scanf("%s", password);

		//prepare message being sent to TPAserver
		memset(out_message,0,sizeof(out_message));
		strcat(out_message,user_name);
		strcat(out_message,",");
		strcat(out_message,password);

		//send message to TPAserver
		if(send(socket_to_TPA , out_message , strlen(out_message) , 0) < 0){
		   	puts("Error, failed to send user info to TPAserver");
		}else{
			memset(token,0,sizeof(token));//clear the content in the input buffer
			memset(out_message,0,sizeof(out_message));//clear the content in the ouput buffer
			
			//wait for reply from TPAserver
			if(recv(socket_to_TPA , token , sizeof(token), 0) < 0)
		    		puts("Error, failed to receive respond from server");
			else{
				//received reply(token) from TPAserver, if token is empty means that user is not authenticated
				if(strcmp(token,"\0")){
					printf("Token received from TPAserver:%s\n",token);
					loop = 0;
				}
				else
					puts("Error, Invalid user name or wrong password.");
			}
		}
		for(int i=0;i<50;i++)printf("-");
		puts("");
	}

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

	loop=1;
	//Keep sending commant to Storageserver
	while(loop){
		printf("Input command : ");
		scanf("%s",command);
		if(!strcmp(command,"exit")){//Command of exit
			memset(out_message,0,sizeof(out_message));
			strcat(out_message,"3");
			write(socket_to_storage,out_message,sizeof(out_message));
			
			//wait for respond from Storageserver to avoid packet loss
			while(recv(socket_to_storage , storage_message , sizeof(storage_message) , 0) < 0);
			if(!strcmp(storage_message,"1"))//close
				loop = 0;
			else
				puts("Error, failed to close connection with server");	
		}else if(!strcmp(command,"en")||!strcmp(command,"de")){//Command of encryption or decryption
			if(!strcmp(command,"en"))
				printf("Input the name of file to encrypt: ");
			else
				printf("Input the name of file to decrypt: ");
			scanf("%s",file_name);
			
			//prepare message being sent to Storageserver
			memset(out_message,0,sizeof(out_message));
			strcat(out_message,"2");
			strcat(out_message,",");
			strcat(out_message,command);
			strcat(out_message,",");
			strcat(out_message,file_name);
			
			//send message to Storageserver
			write(socket_to_storage,out_message,sizeof(out_message));

			//wait for respond from Storageserver
			while(recv(socket_to_storage , storage_message , sizeof(storage_message) , 0) < 0);
			if(!strcmp(storage_message,"1")){
				if(!strcmp(command,"en"))
					puts("Encryption successed");
				else
					puts("Decryption successed");
			}else{
				if(!strcmp(command,"en"))
					puts("Error, encryption failed");
				else
					puts("Error, decryption failed");
			}
			for(int i=0;i<50;i++)printf("-");
			puts("");
		}else{
			printf("Error, invalid command\n");
			for(int i=0;i<50;i++)printf("-");
			puts("");
		}
	}
	//close socket
 	close(socket_to_TPA);
	close(socket_to_storage);
    	return 0;
}
