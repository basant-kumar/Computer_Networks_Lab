
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

#define PORT "8888"  //port number for server socket, will use in bind()
#define max_size 2048   //max size of file, please increase if your file has more size to send all data




//will return ip address of client based on ipv4 or ipv6 address
void * get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc,char *argv[]){	
	struct addrinfo hints,*res,*p; 					//structure to store server socket details
	struct sockaddr_storage client_addr; 				//structure to store client socket details
	struct timeval tv; 						//structure for time, will use in gettimeofday
	socklen_t client_addrlen; 					//size of client address 
	int status,sockfd,clientfd,yes=1,numbytes=0,sentbytes=0;
	int count=1; 							//to count number of send() 
	unsigned int time;
	char ip[INET6_ADDRSTRLEN];					 //will store ip address of client
	char buf[256],buf1[max_size];					 // buffer to use data transfer

	/************creating log file to store all the activities of server ********************/
	FILE *lf;
	lf=fopen("logfile.txt","w");
	if(lf==NULL){
		perror("server:error in logfile:>");
		exit(1);
	}
	char s[100];
	strcpy(s,"No of Send()          Total Bytes          Client ID                  Filename          Time Taken(micro sec)\n");
	size_t size = strlen(s); 
	char * s1 = (char *)malloc(size); 
	memcpy(s1, s, size);
	fwrite(s1,1,size,lf);
	fclose(lf);
	/************* It was just structure of log file, will fill later in every child process **************************/	

    	//fill the server sokcet structure 
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	
	if((status=getaddrinfo(NULL,PORT,&hints,&res))==-1){
		fprintf(stderr,"error in getaddrinfo:%s\n",gai_strerror(status));
		return 1;
	}
	//traverse through linkedlist to find empty socket for creation of new socket
	//this linked list can have more than two nodes according to your machine network cards and number of ip address (e.g. IPv4 and IPv6)
	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("server:socket");
			continue;
		}
		//to handle "Address already in Use error", this happens when a socket is hanging in the kernel and blocking the port number
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("server:setsockopt");
			exit(1);
		}
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);
			perror("server:bind");
			continue;
		}
		break;
	}
	if(p==NULL){
		printf("server:error in binding\n");
		exit(1);
	}
	//free the structure, we don't need it because now we have socket
	freeaddrinfo(res);
	
	//listen for max 100 clients simultaneously 
	if(listen(sockfd,100)==-1){
		perror("server:listen");
		exit(1);
	}
	printf("server:waiting for connections......\n");
	while(1){
		//will user pipe to count no of send()'s 
		int fd[2],val=0;
		pipe(fd);
	
		
		client_addrlen=sizeof(client_addr);
		if((clientfd=accept(sockfd,(struct sockaddr*)&client_addr,&client_addrlen))==-1){
			perror("server:accept");
			continue;
		}
		//convert ip address from struct in_addr or in_addr6 to number-and-dot notation
		inet_ntop(client_addr.ss_family,get_in_addr((struct sockaddr*)&client_addr),ip,sizeof(ip));

		printf("getting connection from :%s\n",ip);
		if((numbytes=recv(clientfd,buf,sizeof(buf),0))==-1){
			perror("server:receive");
			continue;
		}
		if(numbytes==0){
			printf("server:client did not send any data\n");
			continue;
		}		
		buf[numbytes]='\0';
		//create child process
		pid_t child_pid=fork();
		
		if(child_pid==0){//child process
			close(fd[0]);
			gettimeofday(&tv,NULL);// get current time
			time=tv.tv_usec;

			FILE *fp,*log;
			//printf("the file name is :%s\n",buf);
			//int length=sizeof(buf);
			//char z[length+5];
			//strcpy(z,"data/");
			//strcat(z,buf);
			
			fp=fopen(buf,"r");
			if(fp==NULL){
				perror("server:file does not exits:>");
				close(clientfd);
				exit(0);
			}
			fread(buf1,1,max_size,fp);
			fclose(fp);
			int len=strlen(buf1);
			//sleep(1);
			if((sentbytes=send(clientfd, buf1,len,0)) ==-1){
				perror("server:send");
				continue;
			}
			gettimeofday(&tv,NULL);
			time=((tv.tv_usec)-time);
			
			int val=1;

			//sending count value to parent using pipe
			write(fd[1],&val,sizeof(val));
			close(fd[1]);


			// all this formating is for making log file properly readable
			char buf2[100];
			memset(buf2,0,sizeof(buf2));
			log=fopen("logfile.txt","a");
			if(log==NULL){
				perror("server:error in logfile:>");
				close(clientfd);
				exit(0);
			}
			char str[30];
			sprintf(str, "%d", count);
			strcpy(buf2,str);
			strcat(buf2,"                        ");
			sprintf(str, "%d", sentbytes);
			strcat(buf2,str);
			strcat(buf2,"                ");
			strcat(buf2,ip);
			strcat(buf2,"            ");
			strcat(buf2,buf);
			strcat(buf2,"              ");
			sprintf(str, "%d", time);
			strcat(buf2,str);
			strcat(buf2,"\n");

			size_t len1 = strlen(buf2); 
			char * newBuf = (char *)malloc(len1); 
			memcpy(newBuf, buf2, len1);
			fwrite(newBuf,1,len1,log);
			fclose(log);


			close(clientfd);//close for child
			printf("server:close the connect for socket %d:%s\n",clientfd,ip);
			exit(0);
		}//child ends
		close(clientfd);//close for parent
		close(fd[1]);
		sleep(1);
		read(fd[0],&val,sizeof(val));
		count+=val;
		close(fd[0]);

	}//while loop end

return 0;
}
