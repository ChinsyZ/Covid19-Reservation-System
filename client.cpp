#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <typeinfo>
        
using namespace std;

#define MAXLINE 4096
#define sch_port 34430
#define localhost "127.0.0.1"

int main(int argc, char*argv[])
{
    int sockfd,n;
    char recvline[4096];
    char buff[4096];
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    int clilen = sizeof(clientaddr);

    if(argc !=2)
    {
        printf("usage: ./client <location>\n");
        return 0;
    }

    printf("The client is up and running\n");

    // create a tcp socket
    // refernce: Beej's
    if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        printf(" create socket error: %s (errno :%d)\n",strerror(errno),errno);
        return 0;
    }
    
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(sch_port);
    if( inet_pton(AF_INET,localhost, &servaddr.sin_addr) <=0 ) {
        printf("inet_pton error for %s\n",argv[1]);
        return 0;
    }
    // connect
    // refernce: Beej's
    if( connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) <0) {
        printf(" connect socket error: %s(errno :%d)\n",strerror(errno),errno);
        return 0;
    }
    int client_port;
    
    // get dynammic port#
    // reference: project guide
    int getsock_check = getsockname(sockfd,(struct sockaddr *)&clientaddr,(socklen_t *)&clilen);
    if(getsock_check == -1){perror("getsockname");}
    // client_port = ntohs(clientaddr.sin_port);
    // printf("TCP client port #:%d\n",client_port);


    // send location
    // refernce: Beej's
    if ( send(sockfd,argv[1],strlen(argv[1]),0) <0) {
        printf("send msg error: %s(errno :%d)\n",strerror(errno),errno);
        return 0;
    }
    printf("The client has sent query to Scheduler using TCP: client location %s\n",argv[1]);
    // receive assignment
    // refernce: Beej's
    n = recv(sockfd,buff,MAXLINE,0);
    buff[n]='\0';
    string s;
    s = buff;
    if(s == "location not found"){
        printf("Location %s not found\n",argv[1]);
    }else{
        printf("The client has received results from the Scheduler: assigned to %s\n",buff);
        if(s == "None"){
            printf("Score = None, No assignment\n");
        }
    }
    close(sockfd);
    return 0;
}
