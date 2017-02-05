#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#include<sys/select.h>
#include<sys/time.h>
#include<signal.h>

#define PORT "8888"

struct client{
	int pid;
	int p1[2];
	int p2[2];
	int clientfd;
	int client_no;
	char name[30];
	struct sockaddr_storage client_addr;
	struct client *next_client;
};
struct info{
	int client_no;
	char name[30];
};

struct message{
	int fun;
	char data[1024];
	struct info src;
	struct info dest;	
};

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void 
client_process(struct client *new_client,int flag){
	struct sockaddr_storage client_addr;
	struct message *msg;
	msg=(struct message*)malloc(sizeof(struct message));
	client_addr=new_client->client_addr;
	int clientfd=new_client->clientfd;
	char buf[100],ip[INET6_ADDRSTRLEN];
	inet_ntop(client_addr.ss_family,get_in_addr((struct sockaddr *)&client_addr),ip,sizeof ip);
	int pid=fork();
	if(pid<0){fprintf(stderr,"forking error\n");}
	while(1){
		memset(buf,0,sizeof(buf));
		if(pid>0){
			if(read(clientfd,buf,sizeof(buf))>0){
				//printf("message received from client :%s\n",buf);
				if(flag==1){
					int size=strlen(buf);
					int r=rand() % (size-1);
					if(buf[r]=='1') buf[r]='0';
					else buf[r]='1';
				}
				write(new_client->p1[1],buf,sizeof(buf));
			}
			else{
				printf("Client %s from socket %d disconnected\n",ip,clientfd);
				kill(pid,SIGKILL);
				return;
			}
		}else{
			if(read(new_client->p2[0],buf,sizeof(buf))){
				//printf("message received from server :%s\n",buf);
				write(clientfd,buf,sizeof(buf));
			}
		}

	}
	
}

struct client* 
server_process(struct client *clients){
	struct client *p,*q;
	struct message *msg;
	msg=(struct message*)malloc(sizeof(struct message));
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=5000;
	fd_set fds;
	p=clients;
	char buf[256];
	
	while(p!=NULL){
		q=clients;
		FD_ZERO(&fds);
		FD_SET(p->p1[0],&fds);
		if(select(p->p1[0]+1,&fds,NULL,NULL,&tv)){
			read(p->p1[0],buf,sizeof(buf));
			//printf("message received :%s\n",buf);
			int c_no=1; //msg->dest.client_no;
			while(q!=NULL){
				if(q->client_no==c_no){
					break;
				}
				q=q->next_client;
			}
			write(q->p2[1],buf,sizeof(buf));
		}
		
		p=p->next_client;
	}
	return clients;
}


int main(int argc,char *argv[]){


/* *********************** Normal work to prepare socket for server **************************************************/
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	int status,yes=1,sockfd,clientfd,n=1;
	char ip[INET6_ADDRSTRLEN];
	char buf[256];

	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;	
	hints.ai_flags=AI_PASSIVE;

	if((status=getaddrinfo(NULL,PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo error %s\n",gai_strerror(status));
		return 1;
	}
	
	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("server:socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("server:setsockopt");
			exit(0);
		}
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			perror("server:bind");
			continue;
		}
		break;
	}

	if(p==NULL){
		fprintf(stderr,"server can't bind\n");
		exit(0);
	}
	if(listen(sockfd,5)==-1){
		perror("server:listen");
		exit(0);
	}
/********************************************************************************************************/
	struct timeval tv;
	tv.tv_sec=5;	
	fd_set fds;
	struct client *clients,*end;
	clients=NULL;	
	static int flag;
	printf("do you want to currupt data?\n0:No\n1:Yes\n");
	scanf("%d",&flag);
	printf("server is online\n");
	while(1){
		FD_ZERO(&fds);
		FD_SET(sockfd,&fds);
		client_addrlen=sizeof(client_addr);
		if(select(sockfd+1,&fds,NULL,NULL,&tv)){
			if((clientfd=accept(sockfd,(struct sockaddr*)&client_addr,&client_addrlen))==-1){
				perror("server:accept");
				continue;
			}
			inet_ntop(client_addr.ss_family,get_in_addr((struct sockaddr *)&client_addr),ip,sizeof ip);
			printf("Connection received from %s on socket %d\n",ip,clientfd);
			struct client *new_client,*p;
			p=clients;
			if(p==NULL){
				new_client=(struct client*)malloc(sizeof(struct client));
				clients=new_client;
				end=clients;
			}else{
				while(p!=NULL){
					end=p; p=p->next_client;
				}
				new_client=(struct client*)malloc(sizeof(struct client));
				end->next_client=new_client;
			}

			new_client->next_client=NULL;
			new_client->clientfd=clientfd;
			new_client->client_addr=client_addr;
			new_client->client_no=n; n++;
			pipe(new_client->p1);
			pipe(new_client->p2);
			int pid=fork();
			new_client->pid=pid;
			if(pid==0){
				client_process(new_client,flag);
			}
			close(clientfd);
		}
		clients=server_process(clients);
		sleep(1);		
	
	}
	
return 0;
}
