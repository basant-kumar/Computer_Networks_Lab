#include<stdio.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/time.h>

#define PORT "8888"
#define max_size 2048


int main(int argc,char *argv[]){
	struct addrinfo hints,*res,*p;
	int status,sockfd,yes=1,numbytes;
	char buf[max_size];
	if(argc != 3){
		printf("Need ip address for connect and filename too\n");
		exit(1);
	}
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	if((status=getaddrinfo(argv[1],PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo error:%s\n",gai_strerror(status));
		return 1;
	}

	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("client:socket");
			continue;
		}
		//to handle Address already in Use error
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("server:setsockopt");
			exit(1);
		}
		if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);			
			perror("client:connect");
			continue;
		}
		break;
	}
	if(p==NULL){
		printf("client:cannot connect to server\n");
		exit(1);
	}
	printf("connecting to server\n");
	if((numbytes=send(sockfd,argv[2],sizeof(argv[2]),0))==-1){
		perror("client:send");
		exit(1);
	}
	//printf("sent bytes :%d\n",numbytes);
	FILE *fp;
	fp=fopen(argv[2],"w");
	if(fp==NULL){
		perror("file did not open:");
		exit(1);
	}


	if((numbytes=recv(sockfd,buf,sizeof(buf)-1,0))==-1){
		perror("client:receive");
		exit(1);
	}
	fwrite(buf,1,numbytes,fp);
	printf("writing to file.....\n");
	
	close(sockfd);

return 0;
}
