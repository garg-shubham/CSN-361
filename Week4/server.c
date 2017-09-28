// server side c program
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
// ^include the structure for sock addr
// https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
// refer the above link for more details
#define max_groups 10
#define mcig 3

void append(char* s, int n) {
        int len = strlen(s);
        if(n!=10){
            s[len] = n + '0';
            s[len+1] = ' ';
            s[len+2] = '\0';
        }
        else{
            s[len] = '1';
            s[len+1] = '0';
            s[len+2] = ' ';
            s[len+2] = '\0';
        }
}

int main(int argc, char* argv[]){
    if(argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
        printf("Usage: %s <port>\n", argv[0]);
        printf("<port> : port on which server will listen\n");
        exit(0);
    }
    // extract port from the input arguments
    // printf("port %s\n", argv[1]);
    int port = atoi(argv[1]);
    // printf("port %d\n", port);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if( inet_aton("127.0.0.1", (struct in_addr *) &serv_addr.sin_addr.s_addr) == 0){
        perror("invalid address");
        exit(EXIT_FAILURE);
    }  

    // create a socket file descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // print the value of socketfd so that i understand better
    //printf("sockfd = %d\n", sockfd);
    if(sockfd == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // set the server socket to the NONBLOCKING mode
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // binding the socket to a specific address and port
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("blind failed");
        exit(EXIT_FAILURE);
    }

    // now prepare some structure for the various clients
    // max 10 groups with at max 3 members each
    int count_clients[max_groups], client_fd[max_groups*mcig], max_fd;
    // set of socket descriptors
    fd_set readfds;
    // iniatialize aa clientfds to 0
    for(int i=0;i<max_groups;i++){
        count_clients[i] = 0;
        for(int j=0;j<mcig;j++){
            client_fd[i*mcig+j] = 0;
        }
    }

    // listen to the client requests
    if(listen(sockfd, 3) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    int fd, new_clinet_fd, addr_len, activity;
    struct sockaddr_in new_clinet_addr;
    int client_addr_len = sizeof(new_clinet_addr);
    char wel_msg[] = "Welcome";
    char first_msg[1024] = {0};

    printf("waiting for connections...\n");

    while(true){
        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to the set
        FD_SET(sockfd, &readfds);
        max_fd = sockfd;

        // add child sockets to the set
        for(int i=0;i<max_groups;i++){
            for(int j=0; j<mcig; j++){
                fd = client_fd[i*mcig + j];
                //printf("%d %d %d\n", i, j, fd);
                if(fd > 0){
                    FD_SET(fd, &readfds);
                }
                // highest file descriptor number needed for the select function
                if(fd > max_fd) max_fd = fd;
            }
        }

        //printf("working fine till this stage 1\n");

        // wait for an activity on one of the sockets
        activity = select(max_fd+1, &readfds, NULL, NULL, NULL);

        if((activity < 0) && (errno!=EINTR)){
            printf("select error\n");
        }

        // now either a new connection request at server
        if(FD_ISSET(sockfd, &readfds)){
            if((new_clinet_fd = accept(sockfd, (struct sockaddr *) &new_clinet_addr, (socklen_t *) &client_addr_len)) < 0){
                perror("accept error");
                exit(EXIT_FAILURE);
            }
            printf("New connection received, socket fd is %d , ip is : %s , port : %d \n" , new_clinet_fd , inet_ntoa(new_clinet_addr.sin_addr) , ntohs(new_clinet_addr.sin_port));
            // send new connection welcome message
            if(send(new_clinet_fd, wel_msg, strlen(wel_msg), 0) < 0){
                perror("send error");
            }
            //read the first message
            if(read(new_clinet_fd, first_msg, 1024) != 0){
                int grp_no = atoi(first_msg);
                printf("new client want to join group: %d\n", grp_no);
                if(count_clients[grp_no-1] < 3){
                    int indx = (grp_no-1)*mcig;
                    while( (indx <(grp_no)*mcig) && (client_fd[indx] != 0) ) indx++;
                    if(indx >= (grp_no)*mcig){
                        printf("error in finding empty room\n");
                    }
                    else{
                        //add to the set of sockets
                        count_clients[grp_no-1]++;
                        char msg[] = "You can now send messages in the group.\n";
                        send(new_clinet_fd, msg, strlen(msg), 0);
                        client_fd[indx] = new_clinet_fd;
                    }
                }
                else{
                    char msg[] = "Room you requested to join is full.\n";
                    send(new_clinet_fd, msg, strlen(msg), 0);
                    int count_empty_rooms = 0;
                    char msg2[512] = "Following rooms are available: \n";
                    for(int i=0; i<max_groups; i++){
                        if(count_clients[i] < 3){
                            count_empty_rooms++;
                            append(msg2, i+1);
                        }
                    }
                    if(count_empty_rooms == 0){
                        char msg3[512] = "All the rooms are currently full. Try to join after sometime.\n";
                        send(new_clinet_fd, msg3, strlen(msg3), 0);
                    }
                    else{
                        strcat(msg2, "\n");
                        send(new_clinet_fd, msg2, strlen(msg2), 0);
                    }
                    shutdown(new_clinet_fd, 2);
                }
            }

        }

        // or a client has sent some message
        // check for all the client_fds
        char client_msg[1024] = {0};
        int fd2, valread;
        for(int i=0; i<max_groups; i++){
            for(int j=0; j<mcig; j++){
                fd = client_fd[i*mcig + j];
                if(FD_ISSET(fd, &readfds)){
                    valread = read(fd, client_msg, 1024);
                    if(valread != 0){
                        printf("message received from client_fd %d grp_no %d: %s", fd, i, client_msg);
                        // some message is received
                        // broadcast it to all members of the room
                        char final_msg[1024];
                        snprintf(final_msg, sizeof(final_msg), "[%d]: %s", fd, client_msg);
                        for(int k=0; k<mcig; k++){
                            fd2 = client_fd[i*mcig + k];
                            if(fd2 > 0 && fd2 != fd){
                                send(fd2, final_msg, sizeof(final_msg), 0);
                            }
                        }

                    }
                    else{
                        // the client is disconnected
                        // update the information of clients
                        count_clients[i]--;
                        client_fd[i*mcig + j] = 0;
                    }
                }
            }
        }

    }
    return 0;
}
