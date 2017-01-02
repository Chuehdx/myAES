# include <stdio.h>
# include <sys/types.h>    
# include <sys/stat.h>    
# include <fcntl.h>
# include <time.h>
# include <stdlib.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <openssl/err.h>
# include <string.h>
# include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
# include "myAES.h"
# include "AESstorage.h"
# include "myTPA.h"


int main(void){
	char command[5],filename[20],username[32],password[32];
	int loop=1,isFirst=1,changekey=1,authenticated=0;
	struct timespec start,end;		//used to record time difference
	double time_start,time_end;
	clock_gettime( CLOCK_MONOTONIC, &start);
	time_start = (double)start.tv_sec + 1.0e-9*start.tv_nsec;	
	myTPA_load_account();
	while(!authenticated){//authentication process
		printf("Please log in:\n");
		printf("User : ");
		scanf("%s",username);
		printf("password : ");
		scanf("%s",password);
		if(myTPA_authentication(username,password))
			authenticated = 1;
	}
	while(loop){
		myAESStorage_print_storage();
		printf("Input command : ");
		scanf("%s",command);
		if(!strcmp(command,"exit")){//Command of exit
			loop = 0;		
		}else if(!strcmp(command,"en")){//Command of encryption
			clock_gettime( CLOCK_MONOTONIC, &end);
			time_end = (double)end.tv_sec + 1.0e-9*end.tv_nsec;
			printf("Input the name of file to encrypt: ");
			scanf("%s",filename);
			if(isFirst){ //first time we need to generate a new key
				changekey=1;
				isFirst=0;
			}else //check the time difference between encrytion to tell if we need new key and iv
				changekey = ((time_end-time_start)>=TIMEFRAME?1:0);
			if(!myAES_Encrypt(filename,changekey)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to encrypt.");
			}
			clock_gettime( CLOCK_MONOTONIC, &start);
			time_start =  (double)start.tv_sec + 1.0e-9*start.tv_nsec;
		}else if(!strcmp(command,"de")){//Command of decryption
			printf("Input the name of file to decrypt: ");
			scanf("%s",filename);
			if(!myAES_Decrypt(filename,1)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to decrypt.");
			}
		}else{
			printf("invalid command.\n");
		}
	}
	return 0;
}
