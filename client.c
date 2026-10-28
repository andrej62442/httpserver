#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#define PORT 5003
bool recvDataState = false;
void* recvData(void* args){
	while(true){
		recvDataState = true;
		int* serverfdp = (int*) args;
		int serverfd = *serverfdp;
		char buffer[1024];
		char message[1024];
		int n = recv(serverfd, buffer, sizeof(buffer) - 1, 0);
		if(!(n>0)){
			printf("%s\n", "Failed to fetch message");
			break;
		}
		buffer[n] = '\0';
		char* end_of_message = strchr(buffer, 3);
		if(end_of_message == NULL){
			strcat(message, buffer);
		}else{
			strcat(message,buffer);
			//int end_location = end_of_message - message;
			//printf("%d\n", end_location);
			//char temp_message[1024];
			//strcpy(temp_message, message);
			//message[end_location] = '\0';
			printf("%s\n", message);
			strcpy(message, "\0");
		}
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
	int conn = inet_pton(AF_INET, "192.168.0.151", &server_addr.sin_addr);
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
	fflush(stdout);
	while(true){
		char message[50];
		char exitMsg[50] = "exit";
		if(!(fgets(message, sizeof message, stdin) != NULL)){
			continue;
		}
		int new_line_pos = strcspn(message, "\n");
		message[new_line_pos] = 3;
		message[new_line_pos + 1] = '\0';
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
