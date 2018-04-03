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


#define BUFF_SIZE 1024


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    int sockUDP,sockTCP, newsockfd; 
    int portno; 
    char udp_snd[BUFF_SIZE] = "Server(UDP) : 메세지 전송 완료!\n";
    char buff_snd[BUFF_SIZE]="Server : hi라고 입력했습니다.";
    char buff_snd2[BUFF_SIZE] = "Server(TCP) : 메세지 전송 완료!\n";
    char buff_rcv[BUFF_SIZE];
    char buffer[BUFF_SIZE];
    char buff_addr[BUFF_SIZE];
    char stop_buff[BUFF_SIZE];

    socklen_t clilen; 
    socklen_t clilen2; 
    
    struct sockaddr_in serv_addr, cli_addr; //
     
    if (argc < 2) { // 인풋값이 적으면 오류 
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
     
    //  socket생성 
    sockUDP = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockUDP < 0) 
       error("ERROR opening socket");
     
    memset((char *) &serv_addr,0, sizeof(serv_addr)); 
    portno = atoi(argv[1]); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);    
     
    //  bind
    if (bind(sockUDP, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
          
    printf("\n------UDP Start------\n");      
      while(1)
      {      
        memset(buff_rcv,0,sizeof(buff_rcv));
        clilen = sizeof(cli_addr); // client주소길이를 저장할 변수를 생성 
        
        if(recvfrom(sockUDP, buff_rcv,BUFF_SIZE , 0 ,(struct sockaddr *) &cli_addr, &clilen) <0){
            error("recvfrom errror");
        }
        printf("Client(UDP): %s\n", buff_rcv);

        strncpy(stop_buff,buff_rcv,strlen(buff_rcv)-1);
        if(strcmp(stop_buff,"bye")==0){
            break;
        }     

        sendto(sockUDP, udp_snd, strlen(buff_snd)+1,0,(struct sockaddr *) &cli_addr,sizeof(cli_addr));     
       }
    close(sockUDP);
    printf("\n------UDP Closed------\n");




    printf("\n------TCP Start------\n");

    sockTCP = socket(PF_INET, SOCK_STREAM, 0);
    if (sockTCP < 0) 
       error("ERROR opening socket");

    if (bind(sockTCP, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");

    listen(sockTCP,5); // Listen for socket connections. Backlog queue (connections to wait) is 5
    clilen2 = sizeof(cli_addr); // client주소길이를 저장할 변수를 생성 
    printf("------Server Waiting------\n");

    newsockfd = accept(sockTCP, (struct sockaddr *) &cli_addr, &clilen); //accept호출후 에러가 없으면 client정보를 담을 소켓을 생성 
    if (newsockfd < 0) 
        error("ERROR on accept");
    printf("------TCP Connect------\n");

    inet_ntop(AF_INET,&cli_addr.sin_addr.s_addr,buff_addr,sizeof(buff_addr));
    printf("IPv4 : %s  --- client connect. \n", buff_addr);
 

    while(1){  
        //  read 
        memset(buff_rcv,0,sizeof(buff_rcv));
        recv(newsockfd,buff_rcv,sizeof(buff_rcv),0); 

        printf("Client(TCP): %s\n",buff_rcv); // 읽어온 정보를 출력 

    
        if(strcmp(buff_rcv,"hi")==0){
            send(newsockfd,buff_snd,sizeof(buff_snd),0);
        }
        else if(strcmp(buff_rcv,"bye")==0){
            break;
        }
        else{
            send(newsockfd,buff_snd2,sizeof(buff_snd2),0);
        }
    }

    close(newsockfd);
    printf("------TCP Client closed------\n");
    
    close(sockTCP);
    printf("\n------Server Close------\n");
    return 0; 
}



