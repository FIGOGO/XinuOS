#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  
#define PORT 1111 /*使用的port*/  
main(){  
    int sockfd,len;  
    struct sockaddr_in addr;  
    int addr_len = sizeof(struct sockaddr_in);  
    char buffer[256];  
    /*建立socket*/  
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){  
        perror ("socket");  
        exit(1);  
    }  
    /*填写sockaddr_in 结构*/  
    bzero ( &addr, sizeof(addr) );  
    addr.sin_family=AF_INET;  
    addr.sin_port=htons(PORT);  
    addr.sin_addr.s_addr=htonl(INADDR_ANY) ;  
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))<0){  
        perror("connect");  
        exit(1);  
    }  
    while(1){  
        bzero(buffer,sizeof(buffer));  
        len = recvfrom(sockfd,buffer,sizeof(buffer), 0 , (struct sockaddr *)&addr ,&addr_len);  
        /*显示client端的网络地址*/  
	printf("recerved: %s\n", buffer);
        printf("receive from Xinu: %s\n" , inet_ntoa( addr.sin_addr));  
        /*将字串返回给client端*/  
        sendto(sockfd,buffer,len,0,(struct sockaddr *)&addr,addr_len);  
    }  
}
