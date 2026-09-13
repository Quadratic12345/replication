#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>

#define PRIMARY_HOST"127.0.0.1"
#define PORT "9000"

int main(){
    struct addrinfo hints={0};
    struct addrinfo *result;
    int replica_fd;
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    int rv = getaddrinfo(
            PRIMARY_HOST,
            PORT,
            &hints,
            &result
        );

    if (rv != 0) {
           fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
           return 0;
       }
    replica_fd=socket(
        result->ai_family,
        result->ai_socktype,
        result->ai_protocol
    );
    if(replica_fd==-1){
        perror("this is the socket issue");
        freeaddrinfo(result);
        return 0;
    }
    if(connect(replica_fd,result->ai_addr,result->ai_addrlen)==-1){
        perror("connect");
        close(replica_fd);
        freeaddrinfo(result);
        return 0;
    }
    freeaddrinfo(result);
    printf("Connected to the primary \n");
    char buffer[1024];

    while(1){
        ssize_t received=recv(replica_fd, buffer, sizeof(buffer)-1,0);
        if(received==0){
            printf("The primary is disconnected \n");
            break;
        }
        if(received==-1){
            perror("received");
            break;
        }
        buffer[received]='\0';
        printf("Received: %s \n",buffer);
        if(strcmp(buffer,"QUIT")==0)
            break;
    }
    close(replica_fd);
    return 0;
}
