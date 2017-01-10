#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <openssl/des.h>
 
/* Triple DES key for Encryption and Decryption */
/*DES_cblock Key1 = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
DES_cblock Key2 = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
DES_cblock Key3 = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };*/

 
/* Print Encrypted and Decrypted data packets */
void print_data(const char *tittle, const void* data, int len);

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

void generate_new_key(unsigned char* key){
	for(int i=0;i<8;i++){
		int tmp = (rand())%75 + 48;
		key[i] = (char) tmp;
	}
}

int main(){
	DES_cblock Key1,Key2,Key3;
	DES_key_schedule SchKey1,SchKey2,SchKey3;
	unsigned char *input_data,*en_data,*de_data;
	char file_name[] = "input1.txt";
	int size;

	/* Input data to encrypt */
	size = get_file_length(file_name);
	input_data = (unsigned char*)malloc(size);
	read_file(file_name,input_data,size);

	generate_new_key(Key1);
	generate_new_key(Key2);
	generate_new_key(Key3);

	/* Init vector */
	DES_cblock iv = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	DES_set_odd_parity(&iv);
	
	/* Check for Weak key generation */
	if ( -2 == (DES_set_key_checked(&Key1, &SchKey1) || DES_set_key_checked(&Key2, &SchKey2) || DES_set_key_checked(&Key3, &SchKey3)))
	{
		printf(" Weak key ....\n");
		return 1;
	}
	
	/* Buffers for Encryption and Decryption */
	unsigned char cipher[size];
	unsigned char text[size];
	
	/* Triple-DES CBC Encryption */
	DES_ede3_cbc_encrypt( (unsigned char*)input_data, (unsigned char*)cipher,size, &SchKey1, &SchKey2, &SchKey3,&iv, DES_ENCRYPT);
	
	/* Triple-DES CBC Decryption */
	memset(iv,0,sizeof(DES_cblock)); // You need to start with the same iv value
	DES_set_odd_parity(&iv);
	DES_ede3_cbc_encrypt( (unsigned char*)cipher, (unsigned char*)text,size, &SchKey1, &SchKey2, &SchKey3,&iv,DES_DECRYPT);
	
	/* Printing and Verifying */
	printf("\n Encrypted:\n%s\n",cipher);
	printf("\n Decrypted:\n%s\n",text);
	
	return 0;
}
