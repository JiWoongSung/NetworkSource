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

int count=1;

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
    struct sockaddr_in serv_addr, cli_addr; //

    if (argc < 2) { // 인풋값이 적으면 오류 
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
     

     //  socket생성 
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
     
    memset((char *) &serv_addr,0, sizeof(serv_addr)); // 구조체를 초기화 하는 함수 (초기화할 시작주소 , 0 , 초기화할 사이즈);
    portno = atoi(argv[1]); //atoi converts from String to Integer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);    
     
    //  bind
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");
          
    while(1)
    {      
        clilen = sizeof(cli_addr); // client주소길이를 저장할 변수를 생성 


        if(recvfrom(sockfd, buff_rcv,BUFF_SIZE, 0 ,(struct sockaddr *) &cli_addr, &clilen) <0)
            error("recvfrom errror");
                
        printf("receive: %s\n", buff_rcv);

        sleep(5);

        if(sendto(sockfd, buff_snd, strlen(buff_snd)+1,0,(struct sockaddr *) &cli_addr,sizeof(cli_addr))<0)
            error("sendto errror");

    }

    close(sockfd);

    printf("\n------Close------\n");
    return 0; 
}



