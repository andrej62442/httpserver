#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define MAX_CLIENTS 10
#define PORT 5003
int clientCount = 0;
int clients[3];
struct msgData{
	int* clientfdp;
	char msg[1024];
};


void sendMessages(struct msgData data){
	for (int i = 0;i<clientCount;i++){
		
		if(clients[i] == *data.clientfdp){
			continue;
		}
		int sendMessage = send(clients[i], data.msg, strlen(data.msg), 0);
		if(sendMessage<0){
			printf("%s\n", "haha gg bozo");
		}else{
			printf("%s\n", "Message sent to a client");
		}
	}
}



int searchClientID(int sClientfd){
	for(int i = 0; i < clientCount;i++){
		if(clients[i] == sClientfd){
			return i;
		}
	}
}



void removeClientFromArray(int index){
	for (int i = index; i < clientCount - 1;i++){
		clients[i] = clients[i+1];
	}
	clientCount--;
}



void* recvMessage(void* args){
	int* clientfdp = (int*) args;
	int clientfd = *clientfdp;			
	printf("Connection with client started\n");
	while(true){
	
		char buffer[1024];
		int n = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
		
		if(n>0){	

			buffer[n] = '\0';
			printf("client says: %s\n", buffer);
			struct msgData data;
			strcpy(data.msg, buffer);
			data.clientfdp = clientfdp;
			sendMessages(data);		
		}else{
			break;
		}
	}	
	close(clientfd);
	printf("%s\n","closed the socket!");
	int clientID = searchClientID(clientfd);
	removeClientFromArray(clientID);
	printf("Connetion with client ended\n");
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
	socklen_t client_len = sizeof(client_addr);
		while(true){	
		int clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
		if(clientCount >= MAX_CLIENTS){
			break;
		}
		if (clientfd < 0 ){
			printf("%s\n", "Client failed to connect");
			close(clientfd);
			break;
		}
		printf("%d\n", clientfd);
		clients[clientCount++] = clientfd;
		pthread_t recvThread;
		pthread_create(&recvThread, NULL, recvMessage, &clients[clientCount - 1]);
		
	}
	close(sockfd);
	return 0;
}
