/*
 service code
 some by www
 xzy 2017.5.15
 for one service many client
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>                         // 包含套接字函数库
#include <stdio.h>
#include <netinet/in.h>                         // 包含AF_INET相关结构
#include <arpa/inet.h>                      // 包含AF_INET相关操作的函数
#include <unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include <signal.h>

#define Avoid 0
#define ALL_DARWIN_PORT    8887  
#define MYKEY   12345
#define SIZE    10240
#define sign518  1

#if  Avoid 
int sign = 0;

void  sig_chsign(int sig)
{
  printf("change!\n");

  sign = 1;
}
#endif

int main()
{    
    char buf[100];
    memset(buf,0,100);   
   
    int server_sockfd,client_sockfd;   
    socklen_t server_len,client_len;   
    struct sockaddr_in server_sockaddr,client_sockaddr;   
    printf("\n======================server init==========================\n"); 
    server_sockfd = socket(AF_INET,SOCK_STREAM, 0); // 定义套接字类型   
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(ALL_DARWIN_PORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_len = sizeof(server_sockaddr);
   
    //允许重复使用本地地址和套接字绑定
    int on = 1;
    setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
   
    //绑定端口
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,server_len)==-1)
    {
        perror("bind");
        exit(1);
    }   
   
    //监听端口
    if(listen(server_sockfd,5) == -1)
    {
        perror("listen");
        exit(1);   
    }
      
    client_len = sizeof(client_sockaddr);   
    pid_t ppid,pid;
  
    while(1)
    {  
        if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&client_sockaddr,&client_len))==-1)
        {
             perror("accept error");
             exit(1);
        }
        else
        {
            send(client_sockfd,"You have connect Server!",strlen("You have connect Server!"),0);
        
   
        printf("\n%s:%d Login server!\n\n",inet_ntoa(client_sockaddr.sin_addr), ntohs(client_sockaddr.sin_port));
   
        ppid = fork();    //创建子进程
   
        if(ppid == -1)
        {
            printf("fork 1 failed:");
        }
        else if(ppid == 0)    //子进程用于接收客户端信息并发送
        {           
            int recvbytes;
            pid = fork();    //再次创建子进程
            
            if(pid == -1)
            {
                printf("fork 2 failed:");
                exit(1);
            }
            else if(pid == 0) //子进程的子进程用于接收消息
		    {  
		      printf("soccer fork!\n");
              execl("/darwin/Linux/project/demo/demo","soccer",NULL);
            }
            else if(pid > 0)  //此时的id为father进程id 
            {
#if Avoid
	      struct sigaction action;  //  bind sig_chsign <==> SIGUSR2
	     
	      action.sa_handler = sig_chsign;
	      sigemptyset(&action.sa_mask);
	      action.sa_flags = 0;

	      sigaction(SIGUSR2,&action,0);
	     
	      // printf("%d\n",getpid());
	      // kill(getpid(),SIGUSR2);
#endif
	      while(1)
                {          
		  bzero(buf,100);//clear buf to 0 
                    if((recvbytes = recv(client_sockfd,buf,100,0))==-1)
                    {
                        perror("read client_sockfd failed:");
                    }
                    else if(recvbytes != 0)
                    {                        
                        buf[recvbytes] = '\0';
                        usleep(10000);
                        printf("%s:%d said:%s\n",inet_ntoa(client_sockaddr.sin_addr), ntohs(client_sockaddr.sin_port), buf);
#if sign518			
			if(!strcmp(buf,"plz"))
			  {
				printf("plz!\n");
			    kill(getpid()+1,SIGUSR1);
			  }
#endif         
               //将客户端发送过来的消息发回给客户
			 if(send(client_sockfd,buf,recvbytes,0)==-1){ 
			   perror("send error");
				     break;
                        }
                    }
                }

	      		//将fu进程替换为walking
//#endif	        
	       /* while(1) */
	       /* 	{ */
	       /*  printf("Send:"); */
	       /*  char *msg; */
	       /*  int len; */
	       /*  scanf("%s",msg);             */
	       /*  len = strlen(msg); */
	       /*    if(send(client_sockfd,msg,len,0)==-1){  */
	       /*           perror("send error"); */
	       /*            break; */
	       /* 	  } */

	       /* 	} */

            }    
        }
         else if(ppid>0)
        {    
         
	  //总父进程中关闭client_sockfd(因为有另一个副本在子进程中运行了)返回等待接收消息
            close(client_sockfd);
        }
      }    
    }
    return 0;
}

