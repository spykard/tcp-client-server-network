CC = gcc
	
all : serv1 serv2 serv3 serv4 client

serv1 : serv1.o
	$(CC) -o serv1 serv1.o 

serv1.o : serv1.c
	$(CC) -c serv1.c

serv2 : serv2.o
	$(CC) -o serv2 serv2.o 

serv2.o : serv2.c
	$(CC) -c serv2.c
	
serv3 : serv3.o
	$(CC) -o serv3 serv3.o 

serv3.o : serv3.c
	$(CC) -c serv3.c
	
serv4 : serv4.o
	$(CC) -pthread -o serv4 serv4.o 

serv4.o : serv4.c
	$(CC) -c serv4.c	
	
client : client.o
	$(CC) -o client client.o 

client.o : client.c
	$(CC) -c client.c	
	
clean :
	rm *.o