#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

#define PORT "8888"

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc,char *argv[]){
	/*if(strlen(argv)<2){
		printf("need ip address\n");
		exit(1);
	}*/

	struct addrinfo hints,*res,*p;
	int status,sockfd,newfd,numbytes,i,size;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	
	
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	//hints.ai_flags=AI_PASSIVE;

	if((status=getaddrinfo(argv[1],PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo erro:%s\n",gai_strerror(status));
		return 1;
	}
	
	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("client:socket");
			continue;
		}
		if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);
			perror("client:connect");
			continue;
		}
		break;
	}	
	if(p==NULL){
		printf("connecting error....\n");
		return 2;
	}
	freeaddrinfo(res);
	char s[1024];
	printf("Enter the data\n");
	scanf("%s",&s);
	if(send(sockfd,s,strlen(s),0)==-1){
		perror("client:send");
		exit(2);
	}
	while(1){
		char buf[1024];
		numbytes=recv(sockfd,buf,sizeof buf,0);
		if(numbytes==0){
			perror("no data came from server\n");
			exit(3);
		}
		else if(numbytes<0){
			perror("client:receive");
			exit(4);
		}
		printf("server response: %s\n",buf);
		if((strlen(buf	))==1)
			break;
		//memset(buf,0,sizeof(buf));
	}
	printf("successful\n");
	close(sockfd);
	
	

return 0;
}
