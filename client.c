#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
	struct sockaddr_in server_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Failed to create the socked on client!");
		exit(1);
	}
	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5100);
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
	char message[50];
	scanf("%s", &message);
	int sendMessage = send(sockfd, message, strlen(message), 0);
	
	if(sendMessage < 0){
		perror("Send failed");
	}
	close(sockfd);

	return 0;
}
