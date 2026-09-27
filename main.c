#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <pthread.h>
void* clientConnection(void* args){
	int* clientfdp = (int*) args;
	int clientfd = *clientfdp;			
	printf("Connection with client started\n");
		
	char buffer[1024];

	int n = recv(clientfd, buffer, sizeof(buffer) - 1, 0);

	if(n>0){
		buffer[n] = '\0';
		printf("client says; %s\n", buffer);

	}
	close(clientfd);
	printf("COnnetion with client ended\n");
}

int main(int agrc, char *argv[]){
	struct sockaddr_in my_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("sokcet creation failed!");
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(5100);

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
	socklen_t client_len = sizeof(client_addr);
		while(true){	
		int clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
	
		if (clientfd < 0 ){
			perror("no brah");
			close(sockfd);
			exit(1);

		}
		pthread_t clientThread;
		pthread_create(&clientThread, NULL, clientConnection, &clientfd);
		
	}
	close(sockfd);
	return 0;
}
