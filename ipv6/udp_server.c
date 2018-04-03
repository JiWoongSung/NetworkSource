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

#define BUFF_SIZE 1024


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    int sockfd; //서버의 소켓
    int portno; 
    char buff_snd[BUFF_SIZE]="전송완료!";
    char buff_rcv[BUFF_SIZE];

    socklen_t clilen; // 클라이언트 주소의 길이를 담음 
    
    /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in6 serv_addr, cli_addr; //ipv6정보를 담을 구조체
     
    if (argc < 2) { // 인풋값이 적으면 오류 
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
     

     //  socket생성 
    sockfd = socket(PF_INET6, SOCK_DGRAM, 0);
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
          
    while(1)
    {      
        clilen = sizeof(cli_addr); // client주소길이를 저장할 변수를 생성 
        recvfrom(sockfd, buff_rcv,BUFF_SIZE, 0 ,(struct sockaddr *) &cli_addr, &clilen); //accept호출후 에러가 없으면 client정보를 담을 소켓을 생성 
        printf("receive: %s\n", buff_rcv);
        sendto(sockfd, buff_snd, strlen(buff_snd)+1,0,(struct sockaddr *) &cli_addr,sizeof(cli_addr));     
    }

    close(sockfd);

    printf("\n------Close------\n");
    return 0; 
}



