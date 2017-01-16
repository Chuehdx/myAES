# include <stdio.h> 
# include <stdlib.h> 
# include <unistd.h>
# include <string.h>    
# include <sys/socket.h> 
# include <openssl/evp.h>
# include <openssl/err.h>
# include <openssl/aes.h>
# include <arpa/inet.h>
# include "myAESstorage.h"
# include "myAES.h"

int main(void){
    	int socket_to_TPA ,socket_to_storage, loop = 1, file_count;
    	struct sockaddr_in TPAserver,Storageserver;
    	char user_name[32], password[32], *token = (char*)malloc(sizeof(char)*32), out_message[500], command[5], file_name[32],storage_reply[32] ,storage_message[500],TPA_message[1000];
	char *key,*salt,*num,*tmp;
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
		memset(TPA_message,0,sizeof(TPA_message));
		strcat(TPA_message,"1");
		strcat(TPA_message,",");
		strcat(TPA_message,user_name);
		strcat(TPA_message,",");
		strcat(TPA_message,password);

		//send message to TPAserver
		if(send(socket_to_TPA , TPA_message , strlen(TPA_message) , 0) < 0){
		   	puts("Error, failed to send user info to TPAserver");
		}else{
			memset(token,0,sizeof(token));//clear the content in the input buffer
			memset(TPA_message,0,sizeof(TPA_message));//clear the content in the ouput buffer
			
			//wait for reply from TPAserver
			if(recv(socket_to_TPA , TPA_message, sizeof(TPA_message), 0) < 0)
		    		puts("Error, failed to receive respond from server");
			else{
				//received reply(token) from TPAserver, if token is empty means that user is not authenticated
				if(strcmp(TPA_message,"\0")){
					memcpy(token,TPA_message,32);
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
	memset(storage_message,0,sizeof(storage_message));
	strcat(storage_message,"1");
	strcat(storage_message,",");
	strcat(storage_message,user_name);
	strcat(storage_message,",");
	strcat(storage_message,token);
	
	//send the authentication token to Storageserver
	write(socket_to_storage,storage_message,sizeof(storage_message));

	//wait for respond from Storageserver
	memset(storage_reply,0,sizeof(storage_reply));
	while(recv(socket_to_storage , storage_reply , sizeof(storage_reply) , 0) < 0);
	
	if(!strcmp(storage_reply,"1"))//token and user name matches
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
			memset(storage_message,0,sizeof(storage_message));
			strcat(storage_message,"4");
			write(socket_to_storage,storage_message,sizeof(storage_message));
			//wait for respond from Storageserver to avoid packet loss
			while(recv(socket_to_storage , storage_reply , sizeof(storage_reply) , 0) < 0);
			if(!strcmp(storage_reply,"1"))//close
				loop = 0;
			else
				puts("Error, failed to close connection with server");	
		}else if(!strcmp(command,"en")){//Command of encryption or decryption
			printf("Input the name of file to encrypt: ");
			memset(file_name,0,sizeof(file_name));
			scanf("%s",file_name);
			
			//prepare message being sent to Storageserver
			memset(storage_message,0,sizeof(storage_message));
			strcat(storage_message,"2");
			strcat(storage_message,",");
			strcat(storage_message,user_name);
			strcat(storage_message,",");

			memset(TPA_message,0,sizeof(TPA_message));
			memcpy(TPA_message,storage_message,strlen(storage_message));

			strcat(storage_message,file_name);
			
			if(!myAES_Encrypt(file_name,1,TPA_message)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to encrypt.");
			}else{
				write(socket_to_TPA,TPA_message,sizeof(TPA_message));
				memset(TPA_message,0,sizeof(TPA_message));
				while(recv(socket_to_TPA , TPA_message , sizeof(TPA_message) , 0) < 0);
				if(!strcmp(TPA_message,"1")){
					printf("Key of encrypted file %s stored successfully\n",file_name);
				}else{
					printf("Error, failed to store key of encrypted file %s\n",file_name);
				}
				//send message to Storageserver
				write(socket_to_storage,storage_message,sizeof(storage_message));
				//wait for respond from Storageserver
				while(recv(socket_to_storage , storage_reply , sizeof(storage_reply) , 0) < 0);
				if(!strcmp(storage_reply,"1")){
					printf("Encrypted file %s upload successfully.\n",file_name);
				}else{
					printf("Error, failed to upload Encrypted file %s upload successfully.\n",file_name);
				}
			}
			for(int i=0;i<50;i++)printf("-");
			puts("");
		}else if(!strcmp(command,"de")){
			printf("Input the name of file to decrypt: ");
			memset(file_name,0,sizeof(file_name));
			scanf("%s",file_name);
			//prepare message being sent to Storageserver
			memset(storage_message,0,sizeof(storage_message));
			strcat(storage_message,"3");
			strcat(storage_message,",");
			strcat(storage_message,user_name);
			strcat(storage_message,",");
			strcat(storage_message,file_name);

			//send message to Storageserver
			write(socket_to_storage,storage_message,sizeof(storage_message));
			
			//wait for respond from Storageserver
			while(recv(socket_to_storage , storage_reply , sizeof(storage_reply) , 0) < 0);
			if(!strcmp(storage_reply,"1")){
				printf("Encrypted file %s received successfully\n",file_name);
				memset(TPA_message,0,sizeof(TPA_message));
				memcpy(TPA_message,storage_message,strlen(storage_message));
					
				write(socket_to_TPA,TPA_message,sizeof(TPA_message));
				memset(TPA_message,0,sizeof(TPA_message));
				while(recv(socket_to_TPA , TPA_message , sizeof(TPA_message),0)<0);

				char *copy = malloc(sizeof(TPA_message));
				memset(copy,0,sizeof(copy));
				strcpy(copy,TPA_message);
				key = strsep(&copy,",");
				//printf("Key received from server: %s\n",key);
				salt = strsep(&copy,",");
				printf("Key:%s\n",key);
				printf("Salt:%s\n",salt);
				//memcpy(salt,tmp,8);
				num = strsep(&copy,",");
				file_count = atoi(num);
				if(!myAES_Decrypt(file_name,1,key,salt,file_count)){
					ERR_print_errors_fp(stderr);
					printf("%s\n","Error, failed to decrypt.");
				}
				puts("end de");
			}else{
				puts("Error, failed to receive key from server");
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
