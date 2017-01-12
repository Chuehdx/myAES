# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# define SIZE 100

void generate_new_key(unsigned char* key){
	for(int i=0;i<32;i++){
		int tmp = (rand())%75 + 48;
		key[i] = (char) tmp;
	}
}

void generate_new_iv(unsigned char* iv){	
	for(int i=0;i<8;i++){
		int tmp = (rand())%75 + 48;
		iv[i] = (char) tmp;
	}
}

int main(void){
	char userstore[SIZE][32],passwordstore[SIZE][32];
	char loc[] = "./Storage/db",txt[]=".txt",number[10],filename[32],key[32],iv[8];
	int num=1,len,count;
	struct timespec start,end;		//used to record time difference
	double time_start,time_end;
	/*for(int i=1;i<=SIZE;i++){
		memset(filename,0,32);
		strcat(filename,loc);
		sprintf(number, "%d", i);
		strcat(filename,number);
		strcat(filename,txt);
	
	
		FILE * file = fopen(filename,"w");//load info from encrypted file
		if (file) {
			generate_new_key(key);
			generate_new_iv(iv);
			int tmp = rand()%10+1;
			fprintf(file,"%s\n",filename);
			for(int k=0;k<32;k++)
				fprintf(file,"%c",key[k]);
			fprintf(file,"\n");
			fprintf(file,"%s\n",iv);
			for(int k=0;k<32;k++)
				fprintf(file,"%c",key[k]);
			fprintf(file,"\n");
			fprintf(file,"%d\n%d\n",32,tmp);
		}
		fclose(file);
	}*/
	
	/*FILE * file = fopen("./Storage/db.txt","w");//load info from encrypted file
	if (file) {
		for(int i=1;i<=SIZE;i++){
				generate_new_key(key);
				generate_new_iv(iv);
				memset(filename,0,32);
				strcat(filename,loc);
				sprintf(number, "%d", i);
				strcat(filename,number);
				strcat(filename,txt);
				int tmp = rand()%10+1;
				fprintf(file,"%s\n",filename);
				for(int k=0;k<32;k++)
					fprintf(file,"%c",key[k]);
				fprintf(file,"\n");
				fprintf(file,"%s\n",iv);
				for(int k=0;k<32;k++)
					fprintf(file,"%c",key[k]);
				fprintf(file,"\n");
				fprintf(file,"%d\n%d\n",32,tmp);
		}
	}
	fclose(file);*/
	
	srand(time(NULL));
	//for(int i=0;i<10;i++){
		num=1;
		clock_gettime( CLOCK_MONOTONIC, &start);
		time_start = (double)start.tv_sec + 1.0e-9*start.tv_nsec;
		/*FILE * file = fopen("./Storage/db.txt","r");
		for(int k=1;k<=SIZE;k++){
			fscanf(file,"%s",filename);
			fscanf(file,"%s",key);
			fscanf(file,"%s",iv);
			fscanf(file,"%s",key);
			fscanf(file,"%d",&len);
			fscanf(file,"%d",&count);
			//printf("%d\n",count);
		}*/
		/*clock_gettime( CLOCK_MONOTONIC, &end);
		time_end = (double)end.tv_sec + 1.0e-9*end.tv_nsec;	
		printf("%.9lf\n",1000000*(time_end-time_start));*/
		
		for(int k=1;k<=SIZE;k++){
			memset(filename,0,32);
			strcat(filename,loc);
			sprintf(number, "%d", k);
			strcat(filename,number);
			strcat(filename,txt);
			FILE * filet = fopen(filename,"r");//load info from encrypted file
			fscanf(filet,"%s",filename);
			fscanf(filet,"%s",key);
			fscanf(filet,"%s",iv);
			fscanf(filet,"%s",key);
			fscanf(filet,"%d",&len);
			fscanf(filet,"%d",&count);
		}
		clock_gettime( CLOCK_MONOTONIC, &end);
			time_end = (double)end.tv_sec + 1.0e-9*end.tv_nsec;	
			printf("%.9lf\n",1000000*(time_end-time_start));
	//}
	return 0;
}
