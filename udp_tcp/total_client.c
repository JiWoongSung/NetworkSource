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
    int sockUDP,sockTCP; 
    int portno; 
    socklen_t serv_addr_size;
    struct sockaddr_in serv_addr; 
    struct hostent *server; 

    char buffer[BUFF_SIZE];
    char buff_rcv[BUFF_SIZE];
    char buff_snd[BUFF_SIZE]="안녕하세요";
    char stop_buff[BUFF_SIZE];

    if (argc < 3) { 
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }
    
    portno = atoi(argv[2]); 
    sockUDP = socket(PF_INET,SOCK_DGRAM, 0); 
    if (sockUDP < 0)  
        error("ERROR opening socket");
    
    server = gethostbyname(argv[1]); 
    if (server == NULL) { 
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }
    
    memset((char *) &serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(portno); 
    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    



    while(1){
        memset(buffer,0,sizeof(buffer)); // 버퍼사이즈 초기화 
        memset(stop_buff,0,sizeof(stop_buff)); 
        memset(buff_rcv,0,sizeof(buff_rcv));

        printf("Please enter the message: ");
        fgets(buffer,255,stdin);
        sendto(sockUDP, buffer, strlen(buffer)+1,0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

        strncpy(stop_buff,buffer,strlen(buffer)-1);
        if(strcmp(stop_buff,"bye")==0){
            break;
        }        

        serv_addr_size = sizeof(serv_addr);
        recvfrom(sockUDP, buff_rcv, BUFF_SIZE, 0 ,(struct sockaddr *)&serv_addr, &serv_addr_size );
        printf("%s\n",buff_rcv);
    }

    close(sockUDP); //close socket
    printf("UDP종료\n");






    sockTCP = socket(PF_INET, SOCK_STREAM, 0); 

    if (connect(sockTCP,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) //서버와 연결을 시도 
        error("ERROR connecting");

    printf("\nTCP연결 완료!\n");
    
    while(1){
        memset(buffer,0,sizeof(buffer)); // 버퍼사이즈 초기화 
        memset(stop_buff,0,sizeof(stop_buff)); 
        memset(buff_rcv,0,sizeof(buff_rcv));

        printf("Please enter the message: ");
        fgets(buffer,255,stdin);
    
        send(sockTCP,buffer,strlen(buffer)-1,0);      


        strncpy(stop_buff,buffer,strlen(buffer)-1);       
        if(strcmp(stop_buff,"bye")==0){
            break;
        }
        recv(sockTCP,buff_rcv,sizeof(buff_rcv),0);
        printf("%s\n",buff_rcv);
    }
    
    close(sockTCP); //close socket

    printf("------TCP Client Closed------\n");

    
    return 0;
}
