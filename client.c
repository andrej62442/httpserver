#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#define PORT 5003
bool recvDataState = false;
void* recvData(void* args){
	while(true){
		recvDataState = true;
		int* serverfdp = (int*) args;
		int serverfd = *serverfdp;
		char buffer[1024];
		int n = recv(serverfd, buffer, sizeof(buffer) - 1, 0);
		if(!(n>0)){
			printf("%s\n", "Failed to fetch message");
			break;
		}
		buffer[n] = '\0';
		printf("%s\n", buffer);
	}
	recvDataState = false;
}
int main(){
	struct sockaddr_in server_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Failed to create the socked on client!");
		exit(1);
	}
	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	int conn = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	if(conn <= 0){
		perror("Invalid server adress");
		close(sockfd);
		exit(1);
	}
	int connection = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if(connection < 0){
		perror("Connection failed");
		close(sockfd);
		exit(1);
	}
	pthread_t recvThread;
	pthread_create(&recvThread, NULL, recvData, &sockfd);
	printf("%s\n", "type exit in order to stop the connection");
	while(true){
		char message[50];
		char exitMsg[50] = "exit";
		scanf("%s", &message);
		int sendMessage = send(sockfd, message, strlen(message), 0);
		if(strcmp(message, exitMsg) == 0){
			close(sockfd);
			break;
		}

		if(sendMessage < 0){
			perror("Send failed");
		}
	}
	return 0;
}
