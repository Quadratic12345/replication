#define _POSIX_C_SOURCE 200112L
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<netdb.h>

#define PORT "9000"

int main()
{
    //for the type of address we want
    struct addrinfo hints={0};
    struct addrinfo *result;

    int server_fd;
    int replica_fd;

    hints.ai_family=AF_INET;
    //sock stream is used because we want a stream socket
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    int rv = getaddrinfo(NULL, PORT, &hints, &result);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    //creating the socket now
    //server_fd are file desciptor some resources are represented using descriptors
    server_fd=socket(
    result->ai_family,
    result->ai_socktype,
    result->ai_protocol
    );
    if (server_fd==-1){
        perror("socket");//the error is of the socket
        freeaddrinfo(result);
        return 0;
    }
    //now to bind the socket to a local address and port
    if (bind(
        server_fd,
        result->ai_addr,
        result->ai_addrlen)==-1){
            perror("bind");
            close(server_fd);
            freeaddrinfo(result);
            return 0;
        }
    freeaddrinfo(result);

    if(listen(server_fd,5)==-1){
        perror("listen");
        close(server_fd);
        return 0;

    }
    printf("primarily waiting for the replica \n");
    replica_fd=accept(server_fd,NULL,NULL);
    if(replica_fd==-1){
        perror("accept");
        close(server_fd);
        return 0;
    }
    printf("The Replica connected \n");

    //creating an user array which temporarily stores the operation entered by the user
    char buffer[1024];

    while(1){
        printf("db > ");
        fflush(stdout);
        if(fgets(buffer,sizeof(buffer),stdin)==NULL)
            break;
        buffer[strcspn(buffer,"\n")] = '\0';

        if(strcmp(buffer,"QUIT")==0){
            send(replica_fd,buffer,strlen(buffer),0);
            break;
        }

        if(send(replica_fd,buffer,strlen(buffer),0)==-1){
            perror("send");
            break;
        }
        printf("Replica sent: %s\n",buffer);
    }
    close(replica_fd);
    close(server_fd);
    return 0;
}
