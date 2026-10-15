#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/ioctl.h>
#define MAX_CLIENTS 10
#define PORT 5003
int clientCount = 0;
int clients[MAX_CLIENTS];



int removeClientFromArray(int sClientfd){
	int index;
	for(int i = 0; i < clientCount;i++){
		if(clients[i] == sClientfd){
			index = i;
			break;
		}
	return -1;
	}
	for (int i = index; i < clientCount - 1;i++){
		clients[i] = clients[i+1];
	}
	clientCount--;
	return 0;
}



int main(int agrc, char *argv[]){
	struct sockaddr_in my_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("sokcet creation failed!");
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int Bind = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr));
	if(Bind < 0){
		printf("Binding failed!");
	}

	int listening = listen(sockfd, 5);
	if(listening < 0){
		perror("no");
		close(sockfd);
		exit(1);
	}

	printf("WAITING FOR A CLIENT\n");

	struct sockaddr_in client_addr;


	fd_set read_fds;

	FD_ZERO(&read_fds);
	while(true){
		FD_SET(sockfd, &read_fds);
		
		int result = select(FD_SETSIZE, &read_fds, (fd_set*) 0, (fd_set*) 0, (struct timeval*) 0);

		if(result<0){
			perror("Failed to select");
			exit(1);
		}	

		for(int fd = 0; fd<FD_SETSIZE;fd++){
			if(FD_ISSET(fd, &read_fds)){
				if(fd == sockfd){
					socklen_t client_len = sizeof(client_addr);

					int clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
					if(clientCount >= MAX_CLIENTS){
						char buffer[1024] = "no fuck you lmao\0";
						send(clientfd, buffer, strlen(buffer) - 1, 0);
						close(clientfd);
					}
					if (clientfd < 0 ){
						printf("%s\n", "Client failed to connect");
						close(clientfd);
					}

					printf("Connection with client started\n");
					printf("%d\n", clientfd);
					clients[clientCount++] = clientfd;

					FD_SET(clientfd, &read_fds);
					printf("%s\n", "Added the client to file descriptors!");
				}else{
					int nread;
					ioctl(fd, FIONREAD, &nread);
					if(nread == 0){
						removeClientFromArray(fd);
						FD_CLR(fd, &read_fds);
					}else{
						char buffer[1024];
						int n = recv(fd, buffer, strlen(buffer), 0);
						if(n<0){
							printf("%s\n", "Client failed to get the message lmao");
							continue;
						}
						buffer[n] = '\0';
						for(int i = 0;i<clientCount;i++){
							if(clients[i] == fd){
								printf("%s\n", "Skipped a message");
								continue;
							}
							int s = send(fd, buffer, strlen(buffer), 0);
							if(s<0){
								printf("%s\n", "Failed to send the message to a client");
								continue;
							}
							printf("%s\n", "Sent message to a client");
						}
					}
				}
			}
		}

	}
	close(sockfd);
	return 0;
}
