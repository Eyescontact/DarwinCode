/*
client code
some by www
xzy 2017.3.21
socket client try

*/
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <fcntl.h>

#include <signal.h>    //518 for sign

#define sign518  1     //for 5.18 change
 
#define ALL_DARWIN_PORT 8887 /* 客户机连接远程主机的端口 */ 
#define MAXDATASIZE 100 /* 每次可以接收的最大字节 */ 
#define SERVER_IP "192.168.1.128" /* 服务器的IP地址 */

volatile int i = 0;     //changable  5.21 xzy

void  ch_sign(int sig)
{
	i++;
}

int main()
{ 
    int sockfd  , numbytes; 
    char buf[MAXDATASIZE]; 
    struct sockaddr_in server_addr;			     
    int len, bytes_sent;
   	char msg[MAXDATASIZE];
   	
#if sign518  //5.21  xzy
   //SIGN_CH();
    struct sigaction chsign_act;
    chsign_act.sa_handler = ch_sign;
    sigemptyset(&chsign_act.sa_mask);
    chsign_act.sa_flags =  0;
    sigaction(SIGUSR2,&chsign_act,0);
                    
#endif
 
	
    printf("\n======================client init========================\n"); 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    { 
        perror("socket"); 
        exit(1); 
    } 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(ALL_DARWIN_PORT); 
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    bzero(&(server_addr.sin_zero),sizeof(server_addr.sin_zero)); 

    if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr_in)) == -1) 
    { 
        perror("connect");
        exit(1); 
    }

    //循环输入文字
   	pid_t pid;
   	pid = fork();
   	
    if(pid < 0)
           printf("fork failed");   
    else if(pid > 0)
    {
     while(1)
     {      
        bzero(buf,MAXDATASIZE);
        printf("\nBegin receive...\n");

//#endif	    // fcntl(sockfd,F_SETFL,O_NONBLOCK);
	
	       // bzero(buf,MAXDATASIZE);
		 if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1)        
		   { 
		     perror("recv");
		     exit(1); 
		   }
		 else if (numbytes > 0)        
		   {
		     
		     buf[numbytes] = '\0';
		     printf("Received: %s\n",buf);
//             if(strcmp(buf,"play"))
//             	kill(getppid(),SIGUSR1);
			     
		     printf("Send:");
		   // char *msg;
		   // fgets(msg,sizeof(msg),stdin);
		   // char show_plz = 'p' ;
  	     	if( i % 2 == 1)//5.21  xzy
  	     		{	
  	     			if(send(sockfd, "plz" , strlen("plz"),0) == -1)
  	     				{
  	     					perror("send error");
  	     				}
  	     		}
  	     	 scanf("%s",msg);            
		     //len = strlen(msg);
		   //	printf("%s\n",msg);
		     //发送至服务器
		     if(send(sockfd, msg ,sizeof(msg),0) == -1)
		       { 
				 perror("send error");
		       }
		   
		     
		   }
		 else
		   {
		     //numbytes=0，表示socket已断开
		     printf("socket end!\n");
	//	             break; //  break error 
		   }
	      }
	 } 
		
	 else if(pid == 0)
	 	{	
	 		printf("soccer fork!\n");
	 		execl("/darwin/Linux/project/demo/demo","soccer",NULL);

	 	}	
      
        
    close(sockfd); 
 

    return 0;
}
