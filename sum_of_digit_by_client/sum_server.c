#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define PORT "8888"

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc,char *argv[]){
	struct addrinfo hints,*res,*p;
	int status,sockfd,newfd,numbytes,i,size,yes=1;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	char buf[1024];
	
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;

	if((status=getaddrinfo(NULL,PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo erro:%s\n",gai_strerror(status));
		return 1;
	}
	
	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("server:socket");
			continue;
		}
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);
			perror("server:bind");
			continue;
		}
		break;
	}	
	if(p==NULL){
		printf("bind error\n");
		return 2;
	}
	freeaddrinfo(res);
	if(listen(sockfd,5)==-1){
		perror("server:listen");
		exit(1);
	}
	while(1){
		client_addrlen=sizeof(client_addr);
		if((newfd=accept(sockfd,get_in_addr((struct sockaddr*)&client_addr),&client_addrlen))==-1){
			perror("server:accept");
			continue;
		}
		numbytes=recv(newfd,buf,sizeof buf,0);
		if(numbytes==0){
			printf("server:client %d hung up\n",newfd);
			exit(1);
		}
		else if(numbytes<0){
			perror("server:receive");
			exit(1);
		}
		size=strlen(buf);
		while(size!=1){
			//char sum[1024];
			int sum1=0,num1,remainder,t;
			num1=atoi(buf);	
			//int size1=sizeof(num1);
			
			   t = num1;
			   while (t != 0)
			   {
			      remainder = t % 10;
			      sum1       = sum1 + remainder;
			      t         = t / 10;
			   }
			memset(buf,0,sizeof buf);
			sprintf(buf,"%d",sum1);
			size=strlen(buf);
			printf("size is bro:%d\n",size);
			if(send(newfd,buf,size+1,0)==-1){
				perror("server:send");
				exit(1);
			}	
			sleep(1);
			if(size==1)
				break;	
		}
		/*if(send(newfd,buf,size+1,0)==-1){
			perror("server:send");
			exit(1);
		}*/	
		
	}
	printf("close ho rha h\n");
	close(newfd);
	close(sockfd);

return 0;
}
