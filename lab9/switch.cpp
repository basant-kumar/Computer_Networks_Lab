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
#include<map>
#include<iostream>

using namespace std;

#define PORT "8888"

struct client{
	int pid;
	int p1[2];
	int p2[2];
	int clientfd;
	int client_no;
	char name[30];
	char MAC[4];
	struct sockaddr_storage client_addr;
	struct client *next_client;
};
struct info{
	int client_no;
	char name[30];
	char MAC[4];
	//string name;
};
struct getlist{
	int size;
	int cno[20];
	char name1[20][30];
	char MAC1[20][4];
	//string name1[20];
};	

struct message{
	int fun;
	char data[256];
	struct info src;
	struct info dest;
	struct getlist list;
};

map<string,int> table;	

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void printtable(){
	if(table.empty()){
		printf("Table is empty\n");
		return;
	}
	map<string,int>::iterator it;
	for(it=table.begin();it!=table.end();++it){
		cout<<"table["<<it->first<<"]="<<it->second<<endl;
	}
	cout<<endl;
}


void client_process(struct client *new_client ){
	struct sockaddr_storage client_addr;
	struct message *msg;
	msg=(struct message*)malloc(sizeof(struct message));
	client_addr=new_client->client_addr;
	int clientfd=new_client->clientfd;
	int client_no=new_client->client_no;
	char buf[100],ip[INET6_ADDRSTRLEN];
	inet_ntop(client_addr.ss_family,get_in_addr((struct sockaddr *)&client_addr),ip,sizeof ip);
	write(clientfd,&client_no,sizeof(int));
	int pid=fork();
	if(pid<0){fprintf(stderr,"forking error\n");}
	while(1){
		if(pid>0){
			if(read(clientfd,(char*)msg,sizeof(struct message))>0){
				if(strcmp(msg->data,"bye")==0){
					printf("Client %s from socket %d disconnected\n",ip,clientfd);
					kill(pid,SIGKILL);
					free(msg);
					return;
				}
				write(new_client->p1[1],(char*)msg,sizeof(struct message));
			}
			else{   
				printf("Client %s from socket %d disconnected\n",ip,clientfd);
				kill(pid,SIGKILL);
				return;
				msg->src.client_no=client_no;
				write(new_client->p1[1],(char*)msg,sizeof(struct message));
				free(msg);
			}
		}else{
			if(read(new_client->p2[0],(char*)msg,sizeof(struct message))){
				//printf("message received from server :%s\n",msg->data);
				
				write(clientfd,(char*)msg,sizeof(struct message));
			}
		}
	}
}

struct client* server_process(struct client *clients){
	struct client *p,*q,*r,*temp;
	struct message *msg,*msg1;
	struct getlist *list;
	
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=5000;
	fd_set fds;
	p=clients;
	
	char buf[256],name[30];
	int fun,c_no;
	
	while(p!=NULL){
		msg=(struct message*)malloc(sizeof(struct message));
		FD_ZERO(&fds);
		FD_SET(p->p1[0],&fds);
		if(select(p->p1[0]+1,&fds,NULL,NULL,&tv)){printf("start 1\n");
			read(p->p1[0],(char*)msg,sizeof(struct message));
			//printf("message received :%s\n",msg->data);
			c_no=msg->src.client_no;
			fun=msg->fun; //printf("fun is %d\n",fun);
			if(fun==4){
				
				string dmac,smac;
				smac=msg->src.MAC;
				dmac=msg->dest.MAC;
				if(table.find(smac)==table.end()){
					table[smac]=msg->src.client_no;
				}
				printf("table before check\n");
				printtable();
				if(table.find(dmac)!=table.end()){
					fun=1;
					msg->dest.client_no=table[dmac];
				}
				
				else if(table.find(dmac)==table.end()){
					int dcc;					
					q=clients;
					while(q!=NULL){
						if(q->client_no!=p->client_no){
							if(strcmp(q->MAC,msg->dest.MAC)==0){
								dcc=q->client_no; //printf("found client no %d\n",dcc);
							}	
						}
						q=q->next_client;
					}
					table[dmac]=dcc;
					msg->dest.client_no=dcc;
					fun=1;
				}
				printf("table after check\n");
				printtable();
			}
			if(fun==0){//new connection
				strcpy(p->name,msg->src.name);
				strncpy(p->MAC,msg->src.MAC,strlen(msg->src.MAC)); //printf("MAC is %s\n",p->MAC);
				msg1=(struct message*)malloc(sizeof(struct message));
				//msg1->fun=2;
				strcpy(msg1->src.name,"server");
				msg1->src.client_no=0;
				msg1->dest.client_no=-1;
				char data1[256],z[30];
				strcpy(z,msg->src.name);
				int sz=strlen(z);
				z[sz-1]='\0';
				sprintf(name,"%d",msg->src.client_no);
				strcat(name," ("); strcat(name,z); strcat(name,") "); strcat(name," is online\n");
				strcpy(data1,"Client no ");
				strcat(data1,name);
				strcpy(msg1->data,data1);
				
				q=clients;
				while(q!=NULL){
					if(q==p){
						q=q->next_client;continue;
					}
					write(q->p2[1],(char*)msg1,sizeof(struct message));
					q=q->next_client;
				}
				free(msg1);
			}
			else if(fun==1){//private message
				q=clients;
				while(q!=NULL){
					if(q->client_no==msg->dest.client_no){
						write(q->p2[1],(char*)msg,sizeof(struct message));
					}
					q=q->next_client;
				}
			}
			else if(fun==2){//broadcast message
				printf("this is boradcast message\n");
			}
			else if(fun==3){//get list
				msg1=(struct message*)calloc(sizeof(struct message),1);
				int i=0,j=0;
				msg1->fun=fun;
				msg1->list.size=0;
				q=clients;
				while(q!=NULL){ //printf("client no is %d : %d\n",q->client_no,p->client_no);
					if(q->client_no!=p->client_no){
						(msg1->list.size)++;
						msg1->list.cno[i]=q->client_no; //printf("\ncno is %d\n",msg1->list.cno[i]);
						strncpy(msg1->list.name1[i],q->name,strlen(q->name));
						strncpy(msg1->list.MAC1[i],q->MAC,strlen(q->MAC));
						//printf("MAC:name %s: %s\n",q->MAC,q->name);
						//printf("MAC0: %s\n",msg1->list.MAC1[0]);
						//printf("MAC1: %s\n",msg1->list.MAC1[1]);		
						i++;
					}
					q=q->next_client;
					
				}
				
				
				write(p->p2[1],(char*)msg1,sizeof(struct message));
				free(msg1);
			}
			else if(fun==4){//disconnect
				msg1=(struct message*)malloc(sizeof(struct message));
				//msg1->fun=2;
				strcpy(msg1->src.name,"server");
				msg1->src.client_no=0;
				msg1->dest.client_no=-1;
				char data1[256],z[30];
				strcpy(z,p->name);
				int sz=strlen(z);
				z[sz-1]='\0';
				sprintf(name,"%d",msg->src.client_no);
				strcat(name," ("); strcat(name,z); strcat(name,") "); strcat(name," is offline\n");
				strcpy(data1,"Client no ");
				strcat(data1,name); 
				strcpy(msg1->data,data1);
				
				q=clients;
				while(q!=NULL){
					if(q->client_no==p->client_no){
						q=q->next_client;continue;
					}
					write(q->p2[1],(char*)msg1,sizeof(struct message));
					q=q->next_client;
				}
				q=clients;
				if(p==clients){
					clients=p->next_client;
					p=p->next_client;
					free(q);
				}else{  
					temp = p;
					p = p->next_client;
					free(temp);
				}
				free(msg1);
				continue;
			}
			
		}
		free(msg);
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
	tv.tv_usec = 5000;	
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd,&fds);
	struct client *clients,*end;
	clients=NULL;
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
				client_process(new_client);
				struct message *exit_msg;
				exit_msg=(struct message*)malloc(sizeof(struct message));
				exit_msg->fun=4;
				exit_msg->src.client_no=new_client->client_no;
				write(new_client->p1[1],(char*)exit_msg,sizeof(struct message));
				free(exit_msg);
				exit(0);
			}
			close(clientfd);
		}
		clients=server_process(clients);
		sleep(1);		
	
	}
	
return 0;
}
