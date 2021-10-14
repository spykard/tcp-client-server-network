#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <stdint.h>

#define LISTENQ 200 // Backlog 

int sockfd, data=0;
char KEYY[100][50];
char VALUEE[100][50];

// Crash/Error Function
void setupfail() {
    perror("Communication Crashed");
    exit(-1);
}

char *get(char *key) {
	int i;
	for (i = 0; i<data; i++){
		if (strcmp(KEYY[i], key) == 0)
			return (char *)VALUEE[i];
	}
	return "\0";
}

void put(char *key, char *value) {
	int i;
	for (i = 0; i<data; i++) {
		if (strcmp(KEYY[i], key) == 0) {
			strcpy(VALUEE[i], value);
			return;}		
	}
	strcpy(KEYY[data], key);
	strcpy(VALUEE[data], value);	
	data++;
	return;
}

ssize_t	writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

void *itterative(void *arg){

	int i, j, y, n, n1, o, flag, flag1, eof, last, server_port, pid, listenfd;
	char buffer[255], key1[255], value1[255], temp1[255];
	
	int id = (intptr_t) arg; // sockfd
	
	pthread_detach(pthread_self());
	
		eof = 0;
		flag = 0;
		flag1 = 0;		
		/* ENCODER */
	while(1){
		/* FLAG 0 = input exists and good to go, FLAG 1 = there was no protocol error */
		if (flag1 == 1) break;
		/* CLEAR BUFFER */
		bzero(buffer,255);	
		n1 = read(id,buffer,255);
		if (n1 < 0) perror("ERROR Reading");
		else if (n1 == 0) eof = 1; /* EOF */
		y=0;
		o=0;

		/* FLAG turns to 1 if non-ASCII character found */  
		/*for (j = 0; j < 255; j++) {
			cc = buffer[j];
			if(cc<0 || cc>=128)
				flag = 1;
			}*/		

		if (eof == 1) break;
		if (flag == 1) break;
				
		while (o<n1){
			if (buffer[o] == 'p'){ // PUT ( , )		
				bzero(key1,255);
				bzero(value1,255);
				if (buffer[o+1] == '\0') // Protocol Error
					{//flag1 = 1;
					break;} 
				for (i = o+1; i < 255; i++){
					if (buffer[i] != '\0')
						key1[y++] = buffer[i];
					else
						break;
				}
				o=o+2+y;
				y=0;
				if (buffer[++i] == '\0') // Protocol Error
					{//flag1 = 1;
					break;} 
				for ( i; i < 255; i++){
					if (buffer[i] != '\0')
						value1[y++] = buffer[i];			
					else
						break;
				}
				o=o+1+y;
				y=0;
				/* Character longer than 50 was given */
				if(strlen(key1)>50 || strlen(key1)<1) // Protocol Error
					{flag1 = 1;
					break;}
				if(strlen(value1)>50 || strlen(value1)<1) // Protocol Error
					{flag1 = 1;
					break;}				
	
				put(key1,value1);
			}
			else if (buffer[o] == 'g'){ // GET ( )
				bzero(key1,255);
				y=0;		
				if (buffer[o+1] == '\0') // Protocol Error
					{//flag1 = 1;
					break;} 
				for (i = o+1; i < 255; i++){
					if (buffer[i] != '\0')
						key1[y++] = buffer[i]; 	
					else
						break;
				}
				//key1[y] = '\0';
				o=o+2+y;
				y=0;
				/* Character longer than 50 was given */
				if(strlen(key1)>50 || strlen(key1)<1) // Protocol Error
					{flag1 = 1;
					break;}
		
				char *temp = get(key1);
				if (strcmp(temp, "\0") == 0){
					n = writen(id,"n",1);
					if (n < 0) perror("ERROR Writing");
				}
				else{
					bzero(temp1,255);					
					temp1[0] = 'f';
					strcat(temp1, temp);
					n = writen(id,temp1,strlen(temp1)+1);
					if (n < 0) perror("ERROR Writing");
				}
			}
			else{
			/* WRONG CODE, TERMINATE */
				flag1 = 1;
				break;}
		}}
		close(id);
	
	return (NULL);
}

void sig_chld(int signo) /* Terminate */
{ 	
    pid_t pid;
	while (pid = waitpid(0, NULL, WNOHANG) > 0) {} 
	signal(SIGCHLD, sig_chld); }

int main(int argc, char **argv) {
    int i, j, y, n, n1, o, flag, flag1, eof, last, server_port, pid, listenfd;
	socklen_t client_size;
	unsigned int cc;
	struct sockaddr_in server_addr, client_addr;
	pthread_t tid;
	char buffer[255], key1[255], value1[255], temp1[255];
  
    /* ARGUMENT SETUP */
    if(argc<2){
        printf("You didn't specify PORT\n");
        return -1;
    } else {
        server_port = atoi(argv[1]);
        if (server_port<0 || server_port>80808){
            printf("PORT out of bounds\n");
            return -1;
        }
    }

	/* SOCKETS */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        setupfail();

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        setupfail();
	/* Client -> Server Socket */
    if (listen(listenfd, LISTENQ) == -1) // 200 Backlog
        setupfail();
	
    while(1){
		/* 4th SERVER TYPE */		
		client_size = sizeof(client_addr);
		sockfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_size);
		if(sockfd < 0) continue;

		pthread_create(&tid, NULL, &itterative, (void *) (intptr_t) sockfd);

		}
}
