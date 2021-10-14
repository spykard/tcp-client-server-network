#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Crash/Error Function
void setupfail() {
    perror("Communication Crashed");
    exit(-1);
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

int main(int argc, char **argv){
    int i, y, j, o, j1, j2, j3, sockfd, flag1, n, eof, server_port, rv; // Flag to check if get exists
	in_addr_t server_ip;
	struct addrinfo hints, *servinfo, *p;
    char clientbuffer[255], buffertemp[255], temp[255];

	/* ARGUMENT SETUP */
    if(argc<3){
        printf("You didn't specify PORT\n");
        return -1;
    } else {
        server_port = atoi(argv[2]);
        if (server_port<0 || server_port>80808){
            printf("PORT out of bounds\n");
            return -1;
        }
    }
		
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;
	rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo);
	if (rv != 0) setupfail();
	
	for (p = servinfo; p != NULL; p = p->ai_next) {
		/* SOCKETS */
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        setupfail();
		
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		setupfail();}
	
	freeaddrinfo(servinfo);

	bzero(clientbuffer,255);
	/* Store the position of the buffer where NULLs will be placed */
	j1=0; j2=0; j3=-1; flag1=0; eof=0;

	/* ALTERNATIVE IMPLEMENTATION: everything in a single write */	
	/* ENCODER */
	/*for (i = 3; i < argc; i++){
		if (strcmp(argv[i], "put") == 0){
			j1 = j3 + 1;
			clientbuffer[j1++] = 'p';
			for (y = 0; y < strlen(argv[i+1]); y++){
				clientbuffer[j1+y] = argv[i+1][y];}
			j2 = j1 + strlen(argv[i+1]);
			clientbuffer[j2++] = '\0';
			for (y = 0; y < strlen(argv[i+2]); y++){
				clientbuffer[j2+y] = argv[i+2][y];}	
			j3 = j2 + strlen(argv[i+2]);
			clientbuffer[j3] = '\0';
			i = i + 2;
		}
		else if (strcmp(argv[i], "get") == 0){
			flag = 1;
			j1 = j3 + 1;
			clientbuffer[j1++] = 'g';
			for (y = 0; y < strlen(argv[i+1]); y++){
				clientbuffer[j1+y] = argv[i+1][y];}
			j2 = j1 + strlen(argv[i+1]);
			clientbuffer[j2] = '\0';
			j3 = j2;
			i = i + 1;
		}			
	}*/

	for (i = 3; i < argc; i++){
		bzero(clientbuffer,255);	
		if (strcmp(argv[i], "put") == 0){
			j = 0;
			clientbuffer[j++] = 'p';
			for (y = 0; y < strlen(argv[i+1]); y++){
				clientbuffer[j++] = argv[i+1][y];}
			clientbuffer[j++] = '\0';
			for (y = 0; y < strlen(argv[i+2]); y++){
				clientbuffer[j++] = argv[i+2][y];}	
			clientbuffer[j] = '\0';
			
			n = writen(sockfd,clientbuffer,j+1);
			if (n < 0) perror("ERROR Writing");	

			i = i + 2;
		}
		else if (strcmp(argv[i], "get") == 0){
			j = 0;
			clientbuffer[j++] = 'g';
			for (y = 0; y < strlen(argv[i+1]); y++){
				clientbuffer[j++] = argv[i+1][y];}
			clientbuffer[j] = '\0';

			n = writen(sockfd,clientbuffer,j+1);
			if (n < 0) perror("ERROR Writing");	

			bzero(buffertemp,255);
			n = 0;
			
				bzero(clientbuffer,255);
				n = read(sockfd,clientbuffer,255);
				if (n < 0) perror("ERROR Reading");
				else if (n == 0) eof = 1; /* EOF */
				if (eof == 1) break;
				strcat(buffertemp, clientbuffer);

				y=0;
				
					if (buffertemp[0] == 'n'){
						printf("\n");}
					else if	(buffertemp[0] == 'f'){
						bzero(temp,255);
						if (buffertemp[1] == '\0') // Protocol Error
						{
							//flag1 = 1;
							//break;
							while(n<3){ // In more than 1 get packet start missing in intervals of 2
								bzero(clientbuffer,255);
								n = read(sockfd,clientbuffer,255);
								if (n < 0) perror("ERROR Reading");
								else if (n == 0) eof = 1; /* EOF */
								if (eof == 1) break;
								strcat(buffertemp, clientbuffer);}} 

						for (j = 1; j < 255; j++){
							if (buffertemp[j] != '\0')
								temp[y++] = buffertemp[j];
							else
								break;
						}
						y=0;			
						printf("%s\n", temp);}
					else break;	

			i = i + 1;
		}
	}
	close(sockfd);
    exit(0);
}
