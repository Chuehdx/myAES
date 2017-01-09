#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>
#include <time.h>
 
int padding = RSA_PKCS1_PADDING;
 
RSA *createRSA(unsigned char *key, int public)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL){
        printf( "Failed to create key BIO");
        return 0;
    }
    if(public)
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    else
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    if(rsa == NULL)
        printf( "Failed to create RSA");
    return rsa;
}
 
int public_encrypt(unsigned char *data,int data_len,unsigned char *key, RSA *rsa,unsigned char *encrypted)
{
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
int private_decrypt(unsigned char *enc_data,int data_len,unsigned char *key, RSA *rsa, unsigned char *decrypted)
{
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}
 
int private_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,0);
    int result = RSA_private_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
int public_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,1);
    int  result = RSA_public_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}
 
void printLastError(char *msg)
{
    char * err = malloc(130);;
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n",msg, err);
    free(err);
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

RSA *generate_new_key(int size,char* pub_key, char* pri_key){
	RSA *keypair = RSA_generate_key(2048,3, NULL, NULL);
	// To get the C-string PEM form:
	BIO *pri = BIO_new(BIO_s_mem());
	BIO *pub = BIO_new(BIO_s_mem());

	PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
	PEM_write_bio_RSAPublicKey(pub, keypair);

	int pri_len = BIO_pending(pri);
	int pub_len = BIO_pending(pub);
	//pri_key = malloc(pri_len + 1);
    	//pub_key = malloc(pub_len + 1);
	BIO_read(pri, pri_key, pri_len);
	BIO_read(pub, pub_key, pub_len);
	pri_key[pri_len] = '\0';
	pub_key[pub_len] = '\0';
	//printf("in:\n%d\n%d\n", pub_len, pri_len);
	return keypair;

}

int main(){
	char file_name[] = {"input3.txt"};
	int size = get_file_length(file_name);
	char *plainText = (char*)malloc(size);
	read_file(file_name,plainText,size);
	 printf("%d\n",size);
	char publicKey[2048],privateKey[2048];
	RSA *keypair = generate_new_key(size,publicKey,privateKey);
	//printf("out:\n%s\n%s\n", privateKey, publicKey);

	unsigned char encrypted[4098]={};
	unsigned char decrypted[4098]={};
	
	int encrypted_length = public_encrypt(plainText,size,publicKey,keypair,encrypted);
	if(encrypted_length == -1){
	    printLastError("Public Encrypt failed ");
	    exit(0);
	}
	printf("Encrypted length =%d\n",encrypted_length);

	int decrypted_length = private_decrypt(encrypted,encrypted_length,privateKey,keypair,decrypted);
	if(decrypted_length == -1)
	{
	    printLastError("Private Decrypt failed ");
	    exit(0);
	}
	printf("Decrypted Text =%s\n",decrypted);
	printf("Decrypted Length =%d\n",decrypted_length); 
	puts("---------------------------------------------");
	/*
	encrypted_length= private_encrypt(plainText,size,privateKey,encrypted);
	if(encrypted_length == -1)
	{
	    printLastError("Private Encrypt failed");
	    exit(0);
	}
	printf("Encrypted length =%d\n",encrypted_length);
	 
	decrypted_length = public_decrypt(encrypted,encrypted_length,publicKey, decrypted);
	if(decrypted_length == -1)
	{
	    printLastError("Public Decrypt failed");
	    exit(0);
	}
	printf("Decrypted Text =%s\n",decrypted);
	printf("Decrypted Length =%d\n",decrypted_length);*/
	return 0;
}
