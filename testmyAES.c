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
	timer_t start,end;
	time(&start);
	while(loop){
		myAESStorage_print_storage();
		printf("Input command : ");
		scanf("%s",command);
		if(!strcmp(command,"exit")){
			loop = 0;		
		}else if(!strcmp(command,"en")){
			time(&end);
			printf("Input the name of file to encrypt: ");
			scanf("%s",filename);
			if(isFirst){
				changekey=true;
				isFirst=false;
			}else
				changekey = (difftime(end,start)>=TIMEFRAME?true:false);
			if(myAES_Encrypt(filename,changekey)){
				ERR_print_errors_fp(stderr);
				printf("%s\n","Error, failed to encrypt.");
			}
			time(&start);
		}else if(!strcmp(command,"de")){
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
