/* 
    ./server port (E.g. ./server 10000 )
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>


#define BUFFSIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    int sockfd, newsockfd; //서버의 소켓, 클라이언트와 연결된 소켓 
    int portno; 
    char buffer[BUFFSIZE];
    char buff_snd[BUFFSIZE] = "Server : 전송이 완료되었습니다.\n";
    char buff_snd2[BUFFSIZE] = "Server : 전송이 실패되었습니다.\n";
    char buff_addr[BUFFSIZE];

     
    socklen_t clilen; // 클라이언트 주소의 길이를 담음 
    
    /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in6 serv_addr, cli_addr; //ipv6정보를 담을 구조체
     
    if (argc < 2) { // 인풋값이 적으면 오류 
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
     

     //  socket생성 
    sockfd = socket(PF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
     
    memset((char *) &serv_addr,0, sizeof(serv_addr)); // 구조체를 초기화 하는 함수 (초기화할 시작주소 , 0 , 초기화할 사이즈);
    portno = atoi(argv[1]); //atoi converts from String to Integer
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(portno); 
    serv_addr.sin6_flowinfo = 0;
    serv_addr.sin6_addr = in6addr_any; //for the server the IP address is always the address that the server is running on
     
     
    //  bind
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");
     
    //  listen
    listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5
     
    clilen = sizeof(cli_addr); // client주소길이를 저장할 변수를 생성 
    printf("------server waiting------\n");

    //  accept
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //accept호출후 에러가 없으면 client정보를 담을 소켓을 생성 
   
    while(1){
        if (newsockfd < 0) 
            error("ERROR on accept");
        printf("------Client connect------\n");
        inet_ntop(AF_INET6,&cli_addr.sin6_addr.s6_addr,buff_addr,sizeof(buff_addr));
        printf("IPv6 : %s  --- client connect. \n", buff_addr);

        //  read 
        memset(buffer,0,sizeof(buffer)); // 버퍼사이즈 초기화 
        recv(newsockfd,buffer,sizeof(buffer),0); 

        printf("Client : %s\n",buffer); // 읽어온 정보를 출력 

    
        if(strcmp(buffer,"hi")==0){
            send(newsockfd,buff_snd,sizeof(buff_snd),0);
        }
        else if(strcmp(buffer,"bye")==0){
            break;
        }

        else{
            send(newsockfd,buff_snd2,sizeof(buff_snd2),0);
        }
    

    }

    close(newsockfd);
    printf("------Client closed------\n");
    
    close(sockfd);
    printf("\n------Close------\n");
    return 0; 
}



