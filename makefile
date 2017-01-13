ALL : TPAserver.o Storageserver.o client.o myAES.o myAESstorage.o myTPA.o
	gcc TPAserver.o myAES.o myAESstorage.o myTPA.o -o TPAserver -lssl -lcrypto
	gcc Storageserver.o  myAESstorage.o myAES.o -o Storageserver -lssl -lcrypto
	gcc client.o myAES.o myAESstorage.o  -o client -lssl -lcrypto
TPAserver.o : TPAserver.c
	gcc TPAserver.c -c 
Storageserver.o : Storageserver.c
	gcc Storageserver.c -c
client.o : client.c
	gcc client.c -c
myAES.o : myAES.c
	gcc myAES.c -c
myAESstorage.o : myAESstorage.c
	gcc myAESstorage.c -c
myTPA.o : myTPA.c
	gcc myTPA.c -c
clean :
	rm -rf TPAserver.o Storageserver.o client.o myAES.o myAESstorage.o myTPA.o
