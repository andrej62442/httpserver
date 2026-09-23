#include <stdio.h> 
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

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
	return 0;
}
