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
#include <stdbool.h>
#define MAX_CLIENTS 10
#define PORT 5003
int clientCount = 0;
int clients[MAX_CLIENTS];
int sockfd;
fd_set read_fds;
fd_set temp_fds;
struct sockaddr_in client_addr;

int removeClientFromArray(int sClientfd){
	int index;
	bool found = false;
	for(int i = 0; i < clientCount;i++){
		printf("Checking client fd %d\n", clients[i]);
		if(clients[i] == sClientfd){
			index = i;
			found = true;
			break;
		}
	}
	if(!found){
		return -1;
	}
	printf("The index of the removed client is %d\n", index);
	for (int i = index; i < clientCount - 1;i++){
		printf("a client has been shifted in the clients array\n");
		clients[i] = clients[i+1];
	}
	clientCount--;
	return 0;
}

void send_to_all(char buff[1024], int fd_to_skip){

	for(int i = 0;i<clientCount;i++){
		if(clients[i] == fd_to_skip){
			printf("%s\n", "Skipped a message");
			continue;
		}
		int s = send(clients[i], buff, strlen(buff), 0);
		if(s<0){
			printf("%s\n", "Failed to send the message to a client");
			continue;
		}
		printf("%s\n", "Sent message to a client");
	}
}

int manage_setted_fds(int fd){
	// if the data comes from the server socket (client connection pending)
	
	if(fd == sockfd){ 
		socklen_t client_len = sizeof(client_addr);
		int clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);

		if (clientfd < 0 ){
			printf("%s\n", "Client failed to connect");
			close(clientfd);
			return 1;
		}
		if(clientCount >= MAX_CLIENTS){
			char buffer[1024] = "no fuck you lmao\0";
			send(clientfd, buffer, strlen(buffer) + 1, 0);
			close(clientfd);
			return 1;
		}

		printf("Connection with client started\n");
		printf("%d\n", clientfd);
		clients[clientCount++] = clientfd;
		FD_SET(clientfd, &read_fds);
		printf("%s\n", "Added the client to file descriptors!");
		printf("Current ammount of clients is %d\n", clientCount);
		return 0;
	} 	
	// otherwise if the data comes from the client socket, recieve the data

	char buffer[1024];
	printf("%s\n", "got to the client stuff");
	int nread;
	ioctl(fd, FIONREAD, &nread);
	int n = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if((nread == 0) && (n == 0)){

		printf("Client has been disconnected with fd number %d\n", fd);
		int deletion_result = removeClientFromArray(fd);
		if(deletion_result < 0){
			printf("Didn't find the client inside the array\n");
		}
		FD_CLR(fd, &read_fds);
		FD_CLR(fd, &temp_fds);
		close(fd);
		return 1;
	}
	if(n<0){
		printf("%s\n", "Client failed to get the message lmao");
		return 1;
	}
	buffer[n] = '\0';
	send_to_all(buffer, fd);
	return 0;
}
	

void manage_FD_sets(){
	printf("Activity detected\n");
	for(int fd = 0; fd<FD_SETSIZE;fd++){
		if(!(FD_ISSET(fd, &temp_fds))){
			continue;
		}

		int continue_loop = manage_setted_fds(fd);
		if(continue_loop == 1){
			continue;
		}
		break;
	}
}


/*****************
 * MAIN FUNCTION * 
 *****************/
int main(int agrc, char *argv[]){
	struct sockaddr_in my_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("sokcet creation failed!");
		return 1;
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int Bind = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(my_addr));
	if(Bind < 0){
		printf("Binding failed!");
		return 1;
	}

	int listening = listen(sockfd, 5);
	if(listening < 0){
		perror("no");
		close(sockfd);
		exit(1);
	}

	printf("WAITING FOR A CLIENT\n");
	FD_ZERO(&read_fds);
	FD_ZERO(&temp_fds);
	while(true){
		FD_SET(sockfd, &read_fds);
		FD_SET(sockfd, &temp_fds);
		int result = select(FD_SETSIZE, &temp_fds, (fd_set*) 0, (fd_set*) 0, (struct timeval*) 0);
		
		if(result<0){
			printf("Failed to select. Retrying\n");
			continue;
		}
		manage_FD_sets();
		temp_fds = read_fds;
		printf("The end of the activity\n");
	}
	close(sockfd);
	return 0;
}
