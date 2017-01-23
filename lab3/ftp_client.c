
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<sys/stat.h>


char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

 /* // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;*/
}


int main(int argc,char *argv[]){
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage client_addr;
	struct stat st;
	socklen_t client_addrlen;
	int status,sockfd,clientfd,yes=1;
	int numbytes;
	char ip[INET6_ADDRSTRLEN];

	if(argc!=3){
		printf("IP Address and Port Number needed.\n");
		exit(1);
	}
	
	memset(&hints,0,sizeof(hints));
	hints.ai_familiy=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	
	if((status=getaddrinfo(argv[1],argv[2],&hints,&res))==-1){
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
		if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);
			perror("ftpserver:connect");
			continue;
		}
		break;
	}
	if(p==NULL){
		printf("Client couldn't connect to ftp-server\n");
		exit(1);
	}
	
	while(1){
		int size;
		printf("ftp> ");
		char cmd[30];
		scanf("%s",&cmd);
		trimwhitespace(&cmd);//remove leading space from the command
		int len=sizeof(cmd);
		cmd[len]='\0';	
		char *main_cmd,*filename;
		main_cmd=strtok(cmd," ");
		
		if(strcmp("put",main_cmd)==0){
			filename=strtok(NULL," ");

			FILE *fp;
			fp=fopen(filename,"r");
			if(fp==NULL){
				printf("filename: no such file exists\n");
				fclose(fp);
				continue;
			}

			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_put1");
				continue;
			}

			sleep(1);//sleep........................................................

			stat(filename,&st);
			size=st.st_size;
			if(send(sockfd,&size,sizeof(size),0)==-1){
				perror("client:send_put2");
				continue;
			}

			numbytes=sendfile(sockfd,fd,NULL,size);
			fclose(fp);

			if(numbytes==size){
				printf("File Successfully Uploaded\n");
			}else{
				printf("Did not upload complete file\n");
			}
		}//if of put command
		else if(strcmp("get",main_cmd)==0){
			filename=strtok(NULL," ");
			FILE *fp;
			fp=fopen(filename,"w");
			if(fp!=NULL){
				printf("filename already exists\n");
				fclose(fp);
				continue;
			}
			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_get");
				continue;
			}
			if(recv(sockfd,&size,sizeof(int),0)==-1){
				perror("client:recv_get1");
				continue;
			}
			
			char *buf=malloc(size);
			if((numbytes=recv(sockfd,buf,size,0))==-1){
				perror("client:recv_get2");
				continue;
			}
			fwrite(buf,1,size,fp);
			fclose(fp);
			if(numbytes==size){
				printf("File Successfully Downloaded\n");
			}else{
				printf("Did not download complete file\n");
			}

		}//else if of get command
		else if(strcmp("cd",main_cmd)==0){
			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_cd");
				continue;
			}
			//do something..................
		}//else if of cd command
		else if(strcmp("ls",main_cmd)==0){
			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_ls");
				continue;
			}
			//do something..................
		}//else if of ls command
		else if(strcmp("pwd",main_cmd)==0){
			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_pwd");
				continue;
			}
			//do something..................
		}//else if of pwd command
		else if(strcmp("!cd",main_cmd)==0){
			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_!cd");
				continue;
			}
			//do something..................
		}//else if of !cd command
		else if(strcmp("!ls",main_cmd)==0){
			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_!ls");
				continue;
			}
			//do something..................
		}//else if of !ls command
		else if(strcmp("!pwd",main_cmd)==0){
			if(send(sockfd,cmd,sizeof(cmd),0)==-1){
				perror("client:send_!pwd");
				continue;
			}
			//do something..................
		}//else if of !pwd command
		else if(strcmp("quit",main_cmd)==0){
			
			//do something..................
		}//else if of quit command


	}//end while loop
	

return 0;
}












