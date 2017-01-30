#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>

#define PORT "8888"

int main(){
	int sockfd,clientfd,status;
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	char buf[1024];
	int numbytes;
	int yes=1;        	
	
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

	printf("waiting for connections.....\n");
	while(1){
		client_addrlen=sizeof(client_addr);
		if((clientfd=accept(sockfd,(struct sockaddr *)&client_addr,&client_addrlen))==-1){
			perror("server:accept");
			continue;
		}
		pid_t pid;
		pid=fork();
		if(pid==0){//client process
			int b,size,k,sum,c=0,yes1=1,j,i;
			if((numbytes=recv(clientfd,&b,sizeof(int),0))<0){
				perror("server:revc");
				exit(0);
			}
			printf("server received block size is :%d\n",b);
			if((numbytes=recv(clientfd,buf,sizeof(buf),0))<0){
				perror("server:revc");
				exit(0);
			}
			printf("server received msg is :%s\n",buf);
			size=strlen(buf);
			printf("size is: %d\n",size);
			char arr[size-b];
			for(i=0;i<size;i++){
				arr[i]=buf[i];
			}
			k=2;
			int blah=((size-b)/b)-1;
			char csum[b];
			while(blah--){
				j=((b*k)-1);
				for( i=(b-1);i>=0;i--){
					int z1=arr[i]-'0';
					int z2=arr[j]-'0';
					//if(i>=0 && j>=b){
						sum=z1+z2;
						if(sum+c==0){arr[i]='0';c=0;}
						if(sum+c==1){arr[i]='1';c=0;}
						if(sum+c==2){arr[i]='0';c=1;}
						if(sum+c==3){arr[i]='1';c=1;}
					//}
					j--;
				}
				k++;
			}
			for( i=0;i<b;i++){
				if(arr[i]=='0'){arr[i]='1';}
				else {arr[i]='0';}
			}
			for(i=0;i<b;i++){csum[i]=arr[i];}
			printf("checksum is :%s\n",arr);
			int aa=size-1;
			for( i=b-1;i>=0;i--){
				if(buf[aa--]!=arr[i]){ yes1=0;}	
			}
			send(clientfd,&yes1,sizeof(int),0);
			printf("server received msg is :%s\n",arr);
			exit(0);	
		}//pid==0
		close(clientfd);
	}//while loop ends

return 0;
}











