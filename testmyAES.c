# include <stdio.h>
# include <stdbool.h>
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
	int loop=1;
	bool isFirst=true,changekey=true;
	timer_t start,end;		//used to record time difference
	time(&start);
	while(loop){
		myAESStorage_print_storage();
		printf("Input command : ");
		scanf("%s",command);
		if(!strcmp(command,"exit")){//Command of exit
			loop = 0;		
		}else if(!strcmp(command,"en")){//Command of encryption
			time(&end);
			printf("Input the name of file to encrypt: ");
			scanf("%s",filename);
			if(isFirst){ //first time we need to generate a new key
				changekey=true;
				isFirst=false;
			}else //check the time difference between encrytion to tell if we need new key and iv
				changekey = (difftime(end,start)>=TIMEFRAME?true:false);
			if(myAES_Encrypt(filename,changekey)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to encrypt.");
			}
			time(&start);
		}else if(!strcmp(command,"de")){//Command of decryption
			printf("Input the name of file to decrypt: ");
			scanf("%s",filename);
			if(myAES_Decrypt(filename)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to decrypt.");
			}
		}else{
			printf("invalid command.\n");
		}
	}
	return 0;
}
