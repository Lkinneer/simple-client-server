#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>


void *handle_connection(void *void_ptr){
	//read the client request into the buffer response
	char request[16384];
	int recv_output;
	memset(request,0,sizeof(request));
	recv_output = recv(*(int *)void_ptr,request,16384,0);

	printf("%s\n",request);
	int stringlength = strlen(request);

	//convert the integer to a string 
	int length = snprintf( NULL, 0, "%d", stringlength );
	char* str  = malloc( length + 1 );
	snprintf( str, length + 1, "%d", stringlength );

	//respond to the client request
	send(*(int *)void_ptr, str, length+1 , 0);
	//close the session file descriptor
	close(*(int *)void_ptr);

}

int main(int argc , char * argv[]){

	pthread_t thread[100];
	int threadcounter = 0;

	const char* hostname = 0;
	const char* portname = argv[1];
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=0;
	hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
	struct addrinfo* res=0;
	int error=getaddrinfo(hostname,portname,&hints,&res);
	if(error!=0){
		perror("get addrinfo has failed");
		exit(1);
	}

	//set the SO_REUSEADDR socket option
	int fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if(fd == -1){
		perror("There was an error setting up the socket");
		exit(1);
	}

	//bind the address to the socket
	if(bind(fd,res->ai_addr,res->ai_addrlen)==-1){
		perror("error binding the address to the socket");
		exit(1);
	}

	freeaddrinfo(res);
	//now listen for connections
	if(listen(fd,SOMAXCONN)){
		perror("failed to listen");
		exit(1);
	}
	printf("have succeded in listening");
	while(1){
		//will hang here until a client attempts to connect
		int session = accept(fd,0,0);
		//error handeling for accept
		if (session == -1){
			if (errno==EINTR){
				continue;	
			}
			perror("failed to accept connection");
			exit(1);
		}
		printf("session created\n");

		if(pthread_create(&thread[threadcounter],NULL,handle_connection,&session)){
			perror("error creating thead");
			exit(1);
		}
		printf("a thread has been created\n");
		threadcounter++;
		threadcounter = threadcounter%100;
		if (threadcounter == 5){
			printf("exiting");
			exit(1);	
		}
	}
}
