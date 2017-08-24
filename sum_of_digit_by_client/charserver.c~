#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<netdb.h>

#define PORT "8888"

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc,char *argv[]){
	//ready server socket()
	//printf("save me");
	int sockfd,newfd,status;
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage their_addr;
	socklen_t size_addr;
	char buf[256];
	char ip[INET6_ADDRSTRLEN]; //to store client ip address
	int numbytes;
	int yes=1;        // for setsockopt() SO_REUSEADDR, below
	int i,j;

	//ready select() to start fun
	fd_set master,readfds;
	int fdmax;
	FD_ZERO(&master);
	FD_ZERO(&readfds);

	//arrays to store ip addresses of clients
	char *ipaddr[100];

	
	
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	if((status=getaddrinfo(NULL,PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo error...:%s\n",gai_strerror(status));
		return 1;
	}
	for(p=res;p!=NULL;p=p->ai_next){
		sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if(sockfd<0){
			perror("server:socket");
			continue;
		}
		// to resolve "address already in use" error message
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if(bind(sockfd,p->ai_addr,p->ai_addrlen)<0){
			close(sockfd);			
			//perror("server:bind");
			continue;
		}
		break;
	}
	if(p==NULL){
		fprintf(stderr,"server:failed to bind\n");
		exit(2);
	}
	freeaddrinfo(res);
	
	if(listen(sockfd,10)==-1){
		perror("server:listen");
		exit(3);
	}

	FD_SET(sockfd,&master);
	fdmax=sockfd;
	//printf("sockfd is :%d\n",fdmax);
	printf("waiting for connections.....\n");
	while(1){
		readfds=master;
		if(select(fdmax+1,&readfds,NULL,NULL,NULL)==-1){
			perror("server:select");
			exit(4);
		}
		
		for(i=0;i<=fdmax;i++){//printf("i is :%d\n",i);
			if(FD_ISSET(i,&readfds)){
				if(i==sockfd){
					//new connection
					size_addr=sizeof their_addr;
					newfd=accept(sockfd,(struct sockaddr*)&their_addr,&size_addr);
					if(newfd==-1){
						perror("server:accept");
						//exit(5);
					}
					else{
						FD_SET(newfd,&master);
						if(newfd>fdmax){
							fdmax=newfd;
						}
						inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),ip,sizeof ip);
						printf("server:new connection from %s on socket %d\n",ip,newfd);
						char msg[56]="\n\n***************WELCOME TO CHAT-SERVER***************\n\n";
						if(send(newfd,msg,56,0)==-1){
							perror("server:namesend");
						}
						ipaddr[newfd]=ip;
					}
					
				}
				else{
					if((numbytes=recv(i,buf,sizeof buf,0))<=0){//no data received
						if(numbytes==0){
							printf("server:socket %d hung up\n",i);
						}
						else{
							perror("receive");
						}
						close(i);
						FD_CLR(i,&master);
					}
					else{//some data received
					
						for(j=0;j<=fdmax;j++){
							if(FD_ISSET(j,&master)){
								if(j!=i && j!=sockfd){
									char str[100];
									int s1;
									s1=strlen(ipaddr[i]);
									strcpy(str,"[");
									strcat(str,ipaddr[i]); strcat(str,"]:");strcat(buf,"\n");
									if(send(j,strcat(str,buf),(numbytes+s1+3),0)==-1){
										perror("send");
									}
								}
							}
						}
					}
				}
			}
		}
	}
	

return 0;
}
