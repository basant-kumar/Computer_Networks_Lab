#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>


int main(){
	int welcomeSocket,newSocket;
	char buffer[1024],buffer1[1024];
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	
	if((welcomeSocket=socket(PF_INET,SOCK_STREAM,0))==-1){
		perror("server:socket");
		return 3;
	}
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(8888);
	serverAddr.sin_addr.s_addr=inet_addr("10.10.39.153");
	memset(serverAddr.sin_zero,0,sizeof serverAddr.sin_zero);
	
	if(bind(welcomeSocket,(struct sockaddr *) &serverAddr, sizeof(serverAddr))==-1){
		perror("server:bind");
		return 1;
	}
	if(listen(welcomeSocket,5)==-1){
		perror("server:listen");
		return 2;
	}
	else
		printf("listening...\n");
	addr_size=sizeof serverStorage;
	newSocket=accept(welcomeSocket,(struct sockaddr *) &serverStorage,&addr_size);
	strcpy(buffer,"hello world\n");
	send(newSocket,buffer,13,0);
	recv(newSocket,buffer1,1024,0);
	printf("data mila: %s\n",buffer1);
	
return 0;
}
