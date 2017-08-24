#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>


int main(){
	int clientSocket;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	
	clientSocket=socket(PF_INET,SOCK_STREAM,0);
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(8888);
	serverAddr.sin_addr.s_addr=inet_addr("10.10.39.151");
	memset(serverAddr.sin_zero,0,sizeof serverAddr.sin_zero);
	addr_size=sizeof serverAddr;
	connect(clientSocket,(struct sockaddr *) &serverAddr,addr_size);
	recv(clientSocket,buffer,1024,0);
	printf("data received: %s\n",buffer);	

return 0;
}
