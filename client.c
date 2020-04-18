#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    //Gives usage information if an incorect number of arguments are used
    if (argc < 3) {
       fprintf(stderr,"please give a hostname followed by a port number", argv[0]);
       exit(0);
    }
    
    portno = atoi(argv[2]);
    //creates a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    //looks up the host that will be revieving the packets
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"Error getting host by name");
        exit(0);
    }
    memset(&serv_addr,0, sizeof(serv_addr));
    //we use AF_INET because we are using IP
    serv_addr.sin_family = AF_INET;
    //copy the address of the server to the this other struct we're going to use to make a connection
    strncpy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    // the port number needs to be in network byte order
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    //while(1){
    printf("Enter message:");
    memset(buffer,0,256);
    //get the message to be sent from the user
    fgets(buffer,255,stdin);
    //write the message to the server
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    memset(buffer,0,256);
    //read the message back from the server
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("From server: the last input's length is %s\n",buffer);
    //}
    return 0;
}
