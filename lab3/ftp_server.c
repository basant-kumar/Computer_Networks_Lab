
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>



int main(int argc,char *argv[]){
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	int status,sockfd,clientfd,yes=1;
	char ip[INET6_ADDRSTRLEN];

	if(argc!=2){
		printf("Port Number needed.\n");
		exit(1);
	}
	
	memset(&hints,0,sizeof(hints));
	hints.ai_familiy=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	
	if((status=getaddrinfo(NULL,argv[1],&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo error :%s\n",gai_strerror(status));
		return 1;
	}
	
	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("fptserver:socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("ftpserver:setsockopt");
			exit(1);
		}
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);
			perror("ftpserver:bind");
			continue;
		}
		break;
	}
	if(p==NULL){
		printf("ftpserver couldn't bind to socket\n");
		exit(1);
	}
	if(listen(sockfd,10)==-1){
		perror("ftpserver:listen");
		exit(1);
	}
	printf("ftp-server is waiting for connections.........\n");
	
	while(1){
		client_addrlen=sizeof(client_addr);
		if((clientfd=accept(sockfd,(struct sockaddr*)&client_addr,&client_addrlen))==-1){
			perror("fptserver:accept");
			continue;
		}
		
		pid_t pid;
		pid=fork();
		if(pid==0){//child process
			
		}


	}//end main while loop

return 0;
}












