#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/des.h>

void DES_Encrypt( char *Key, char *Msg, int size, char *Res){
        int             n=0;
        DES_cblock      Key2;
        DES_key_schedule schedule;
 
        /* Prepare the key for use with DES_cfb64_encrypt */
        memcpy( Key2, Key,8);
        DES_set_odd_parity( &Key2 );
        DES_set_key_checked( &Key2, &schedule );
 
        /* Encryption occurs here */
        DES_cfb64_encrypt( ( unsigned char * ) Msg, ( unsigned char * ) Res,
                           size, &schedule, &Key2, &n, DES_ENCRYPT );
}
 
 
void DES_Decrypt( char *Key, char *Msg, int size, char *Res){
        int             n=0;
        DES_cblock      Key2;
        DES_key_schedule schedule;
 
        /* Prepare the key for use with DES_cfb64_encrypt */
        memcpy( Key2, Key,8);
        DES_set_odd_parity( &Key2 );
        DES_set_key_checked( &Key2, &schedule );
 
        /* Decryption occurs here */
        DES_cfb64_encrypt( ( unsigned char * ) Msg, ( unsigned char * ) Res,
                           size, &schedule, &Key2, &n, DES_DECRYPT );
}

size_t get_file_length(char* filename){
	FILE *tmp = fopen(filename,"r");
	if(tmp!=NULL){
		fseek(tmp, 0, SEEK_END);
		size_t length = ftell(tmp);
		fclose(tmp);
		return length;
	}
	fclose(tmp);
}

void read_file(char* filename,char* file,size_t file_len){
	FILE *tmp = fopen(filename,"r");
	if(tmp != NULL){
		fread(file,1,file_len,tmp);
	}
	fclose(tmp);
}

void generate_new_key(char* key){
	for(int i=0;i<8;i++){
		int tmp = (rand())%75 + 48;
		key[i] = (char) tmp;
	}
}


int main(){
	char key[8];
	char file_name[] = "input2.txt";
	char *content;
	char *decrypted;
	char *encrypted;
	int size;
	 
	generate_new_key(key);
	size = get_file_length(file_name);
	content = (char*)malloc(size);
	encrypted = (char*)malloc(size);
	decrypted = (char*)malloc(size);

	read_file(file_name,content,size);	
	printf("Clear text:\n%s\n",content); 
	DES_Encrypt(key,content,size,encrypted);
	printf("Encrypted text:\n%s\n",encrypted);
	DES_Decrypt(key,encrypted,size,decrypted);
	printf("Decrypted text:\n%s\n",decrypted);

	return (0);
}

