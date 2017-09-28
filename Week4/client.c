// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <math.h>
  
int main(int argc, char const *argv[]){
    if(argc < 3 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 || atoi(argv[2]) <= 0 || atoi(argv[2]) > 10){
        printf("Usage: %s <port> <group no>\n", argv[0]);
        printf("<port> : port on which server will listen\n");
        printf("<group no> : chat group which you want to join, value: 1-10\n");
        exit(0);
    }
    // extract port from the input arguments
    // printf("port %s\n", argv[1]);
    int port = atoi(argv[1]);
    int group_no = atoi(argv[2]);
    // printf("port %d\n", port);

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);

    int client_fd;
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }

    // connect to a server
    struct  sockaddr_in serv_addr;
    int serv_addr_len = sizeof(serv_addr);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if(connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }

    char buffer[1024] = {0};
    int valread;    

    // receive welcome message
    valread = read( client_fd , buffer, 1024);
    printf("%s\n", buffer );

    // send the very first message which is the group no
    send(client_fd , argv[2] , strlen(argv[2]) , 0 );
    // read the message
    valread = read( client_fd , buffer, 1024);
    printf("%s\n", buffer );

    // maintain a list of server fd and client fd
    // check from where the data is comming
    fd_set readfds;
    
    int input_fd = fileno(stdin);
    
    fcntl(input_fd, F_SETFL, O_NONBLOCK);
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    int max_fd = input_fd;
    if(client_fd > max_fd) max_fd = client_fd;
    max_fd += 1;

    char msg[1024] = {0};

    while(true){
        FD_ZERO(&readfds);
        FD_SET(input_fd, &readfds);
        FD_SET(client_fd, &readfds);
        int activity = select(max_fd, &readfds, NULL, NULL, NULL);
        // client wants to send some message
        if(FD_ISSET(input_fd, &readfds)){
            fgets(msg, 1024, stdin);
            //printf("echo: %s", msg);
            send(client_fd, msg, strlen(msg), 0);
        }
        // check if there some message is recieved from the server
        if(FD_ISSET(client_fd, &readfds)){
            valread = read(client_fd, msg, 1024);
            if(valread != 0){
                printf("%.*s", valread, msg);
            }
            // connection is closed by the server
            else{
                printf("connection is closed by the server.\nbye\n");
                exit(0);
            }
        }
    }
    return 0;
}