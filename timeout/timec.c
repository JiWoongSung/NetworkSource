/*
    ./client address port (E.g. ./client ::1 10000)
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // hostent 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd; // 클라이언트의 소켓 
    int portno; // 포트번호저장
    socklen_t serv_addr_size;
    struct sockaddr_in serv_addr; // ipv6정보를 담을 구조체 
    struct hostent *server; // 서버의 ip주소 정보가 담긴 구조체 

    char buffer[BUFF_SIZE];
    char buff_rcv[BUFF_SIZE];
    char buff_snd[BUFF_SIZE]="안녕하세요";

    //select time out 설정을 위한 timeval 구조체
    struct timeval tv;
    fd_set readfds;
    int state;

    if (argc < 3) { // 인풋값이 적으면 오류 
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }
    
    portno = atoi(argv[2]); // 포트번호가 문자라면 숫자로 만들어준다. 
    sockfd = socket(PF_INET,SOCK_DGRAM, 0); 
    if (sockfd < 0)  //소켓생성 실패면 오류 
        error("ERROR opening socket");
    
    //getaddrinfo(host,port,&hints,&res)
    server = gethostbyname(argv[1]); //argv[1]의 문자열(DNS)를 IP 주소, 주소 유형, 주소 길이 등의 정보가 들어있는 struct hostent로 반환
    if (server == NULL) { // return 값 hostent * 구조체 null이면 오류 
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }
    
    memset((char *) &serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //인터넷프로토콜 설정 
    serv_addr.sin_port = htons(portno); // 포트번호 설정
    //struct in6_addr sin6_addr : IPv6의 주소 
    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);


    
    //sendto()

    while(1){
        printf("Please enter the message: ");
        fgets(buffer,255,stdin);
        sendto(sockfd, buffer, strlen(buffer)+1,0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));


        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        // 약 n초간 기다린다
        tv.tv_sec = 2;
        tv.tv_usec = 10; // 마이크로 초 100만분의 1초 

        //입력이 있는지 기다린다.
        state = select(sockfd+1, &readfds,(fd_set *)0, (fd_set *)0, &tv);

        switch(state)
        {
            case -1:
                error("select error :");

            case 0:
                printf("Time out error\n");
                close(sockfd);
                return 0;

            //5초안에 입력이 들어왔을경우 처리한다.
            default:
                serv_addr_size = sizeof(serv_addr);
                recvfrom(sockfd, buff_rcv, BUFF_SIZE, 0 ,(struct sockaddr *)&serv_addr, &serv_addr_size );
                printf("receive: %s\n",buff_rcv);

        }

    }
    close(sockfd); //close socket
    
    return 0;
}
