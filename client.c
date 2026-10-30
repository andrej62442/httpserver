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
#include <stdint.h>
#define BUFF_LEN_OFFSET 4
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
		printf("%s\n", buffer);
		int empty[1024];
		memcpy(buffer, empty, 1024);
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
		char message[1020];
		char exitMsg[5] = "exit";
		if(!(fgets(message, sizeof message, stdin) != NULL)){
			continue;
		}
		int new_line_pos = strcspn(message, "\n");
		message[new_line_pos] = '\0';
		uint8_t buffer[1024];
		//for(int i = BUFF_LEN_OFFSET;i<strlen(message) + BUFF_LEN_OFFSET + 1;i++){
		//	buffer[i] = message[i - BUFF_LEN_OFFSET - 1];
		//}
		if(strcmp(message, exitMsg) == 0){
			close(sockfd);
			break;
		}
		uint32_t buffLen = (uint32_t)strlen(message);
		uint32_t fullLen = buffLen + BUFF_LEN_OFFSET;
		buffer[3] = (uint8_t)(fullLen >> 0) & 0xFF;
		buffer[2] = (uint8_t)(fullLen >> 8) & 0xFF;
		buffer[1] = (uint8_t)(fullLen >> 16) & 0xFF;
		buffer[0] = (uint8_t)(fullLen >> 24) & 0xFF;
		memcpy(buffer + BUFF_LEN_OFFSET, message, buffLen);
		size_t total_sent = 0;
		while (total_sent < fullLen) {
    		ssize_t n = send(sockfd, buffer + total_sent, fullLen - total_sent, 0);
			if(n<=0){
				break;
			}
			total_sent += (size_t)n;
		}	
	}
	return 0;
}
