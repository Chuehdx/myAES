# include <stdio.h>
# include <sys/types.h>    
# include <sys/stat.h>    
# include <fcntl.h>
# include <time.h>
# include <stdlib.h>
# include <openssl/evp.h>
# include <openssl/aes.h>
# include <string.h>
# include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
# include "myAES.h"


int main(void){
	char command[5],filename[20];
	int loop=1,isFirst=1,changekey=1;
	struct timespec start,end;		//used to record time difference
	double time_start,time_end;
	clock_gettime( CLOCK_MONOTONIC, &start);
	time_start = (double)start.tv_sec + 1.0e-9*start.tv_nsec;
	while(loop){
		if(!myAES_Encrypt("input1.txt",1,0)){
			while(myAES_Decrypt("input1.txt")){
				printf("in retrying plz\n");
				sleep(1);
				myAES_Encrypt("input1.txt",1,1);
				
			}
		}else
			break;
		sleep(1);
		/*myAESStorage_print_storage();
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
				changekey=true;
				isFirst=false;
			}else //check the time difference between encrytion to tell if we need new key and iv
				changekey = ((time_end-time_start)>=TIMEFRAME?true:false);
			if(myAES_Encrypt(filename,changekey)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to encrypt.");
			}
			clock_gettime( CLOCK_MONOTONIC, &start);
			time_start =  (double)start.tv_sec + 1.0e-9*start.tv_nsec;
		}else if(!strcmp(command,"de")){//Command of decryption
			printf("Input the name of file to decrypt: ");
			scanf("%s",filename);
			if(myAES_Decrypt(filename)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to decrypt.");
			}
		}else{
			printf("invalid command.\n");
		}*/
	}
	return 0;
}
