#include <stdio.h>   
#include <string.h>   
#include <errno.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h> 
#include <cstdlib>
#include <iostream> 
#include <vector>
#include <sstream>

using namespace std; 
  
#define UDP_PORT 33430  
#define PORTa 30430 
#define PORTb 31430 
#define PORTc 32430 
#define TCP_PORT 34430
#define MAXLINE 40
#define localhost  "127.0.0.1"   

  
int main()  
{  
    // UDP
    // create an UDP socket
    // refernce: Beej's
  	int udp_sockfd = socket(AF_INET, SOCK_DGRAM,0);//IPv4
	  if(udp_sockfd < 0){
      perror("socket error\n");
      exit(1);
  	}

    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv, 0 ,sizeof (struct sockaddr_in));//fill every byte with 0
    addr_serv.sin_family = AF_INET;//IPv4
    addr_serv.sin_port = htons(UDP_PORT);//port #
    addr_serv.sin_addr.s_addr = inet_addr(localhost);//IP add
    len = sizeof(addr_serv);

    struct sockaddr_in hospital1;
    int len1;
    memset(&hospital1, 0 ,sizeof (struct sockaddr_in));//fill every byte with 0
    hospital1.sin_family = AF_INET;//IPv4
    hospital1.sin_port = htons(PORTa);//port #
    hospital1.sin_addr.s_addr = inet_addr(localhost);//IP add
    len1 = sizeof(hospital1);

    struct sockaddr_in hospital2;
    int len2;
    memset(&hospital2, 0 ,sizeof (struct sockaddr_in));//fill every byte with 0
    hospital2.sin_family = AF_INET;//IPv4
    hospital2.sin_port = htons(PORTb);//port #
    hospital2.sin_addr.s_addr = inet_addr(localhost);//IP add
    len2 = sizeof(hospital2);

    struct sockaddr_in hospital3;
    int len3;
    memset(&hospital3, 0 ,sizeof (struct sockaddr_in));//fill every byte with 0
    hospital3.sin_family = AF_INET;//IPv4
    hospital3.sin_port = htons(PORTc);//port #
    hospital3.sin_addr.s_addr = inet_addr(localhost);//IP add
    len3 = sizeof(hospital3);

	// bind udp socket
    // refernce: Beej's
	if(bind(udp_sockfd,(struct sockaddr *)&addr_serv,sizeof(addr_serv))<0)
	{
		perror("bind error\n");
		exit(1);
	}

    // TCP
    // refernce: Beej's
    int tcp_sockfd,connfd;
    struct sockaddr_in servaddr, tcp_cliser_addr;
    char buff[30];
    int n;
    // create a TCP socket
    // refernce: Beej's
    if( (tcp_sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        printf(" create socket error: %s (errno :%d)\n",strerror(errno),errno);
        return 0;
    }
	
    // clear servaddr and IP:127.0.0.1
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(localhost);
    servaddr.sin_port = htons(TCP_PORT);
	
    // bind
    // refernce: Beej's
    if ( bind(tcp_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        printf(" bind socket error: %s (errno :%d)\n",strerror(errno),errno);
        return 0;
    }
    
    printf("The Scheduler is up and running.\n");
    // listen
    // refernce: Beej's
    if( listen(tcp_sockfd,10) == -1) {
        printf(" listen socket error: %s (errno :%d)\n",strerror(errno),errno);
        return 0;
    }
  
    int send_num;
    int recv_num;
    char send_buf[30];
    char recv_buf[300];
    int c1,c2,c3;
    int cap1,cap2,cap3;
    int occ1,occ2,occ3;
    int i=0;
    while(1){
        // receive total capacity and occupation from Hospital A/B/C
        while(i<3){
            recv_num = recvfrom(udp_sockfd, send_buf, sizeof(send_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);
            if(recv_num < 0)
            {
                perror("recvfrom error:");
                exit(1);
            }
            if(i==0){                        // HospitalA
                string compare1 = send_buf;
                istringstream com1(compare1);
                com1 >> cap1;
                com1 >> occ1;
                c1 = (cap1 <= occ1);         // whether have availability
                cout <<"The Scheduler has received information from Hospital A: total capacity is "<< cap1 <<" and initial occupancy is "<< occ1 <<endl;
          }else if(i==1){                    // HospitalB
              string compare2 = send_buf;
              istringstream com2(compare2);
              com2 >> cap2;
              com2 >> occ2;
              c2 = (cap2 <= occ2);           // whether have a availability
              cout <<"The Scheduler has received information from Hospital B: total capacity is "<< cap2 <<" and initial occupancy is "<< occ2 <<endl;
          }else{                             // HospitalC
              string compare3 = send_buf;
              istringstream com3(compare3);
              com3 >> cap3;
              com3 >> occ3;
              c3 = (cap3 <= occ3);           // whether have a availability
              cout <<"The Scheduler has received information from Hospital C: total capacity is "<< cap3 <<" and initial occupancy is "<< occ3 <<endl;
          }
          i++;
        }
        // accept
        // reference: Beej Guide
        socklen_t tcp_client_addr_len = sizeof tcp_cliser_addr;
		int connfd = accept(tcp_sockfd, (struct sockaddr *)&tcp_cliser_addr, &tcp_client_addr_len);
        if( connfd  == -1) {
            printf("accpt socket error: %s (errno :%d)\n",strerror(errno),errno);
            exit(1);
        }

        // get location from client
        n = recv(connfd,send_buf,MAXLINE,0);
        send_buf[n] = '\0';
        printf("The Scheduler has received client at location %s from the client using TCP over port %d\n",send_buf,TCP_PORT);

        // send to hospitalA
        if(!c1){                    // if have availability
            send_num = sendto(udp_sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&hospital1, len1);
            if(send_num < 0)
            {
                perror("sendto error:");
                exit(1);
            }
            printf("The Scheduler has sent client location to Hospital A using UDP over port %d\n", UDP_PORT);
        }
        // send to hospitalB
        if(!c2){                    // if have availability
            send_num = sendto(udp_sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&hospital2, len2);
            if(send_num < 0)
            {
                perror("sendto error:");
                exit(1);
            }
            printf("The Scheduler has sent client location to Hospital B using UDP over port %d\n", UDP_PORT);
        }

        // send to hospitalC
        if(!c3){                    // if have availability
            send_num = sendto(udp_sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&hospital3, len3);
            if(send_num < 0)
            {
                perror("sendto error:");
                exit(1);
            }
            printf("The Scheduler has sent client location to Hospital C using UDP over port %d\n", UDP_PORT);
        }

        float score1 = 0;
        float score2 = 0;
        float score3 = 0;
        float dis1, dis2, dis3;
        // receive score and dis from hospitalA
        if(!c1){
            recv_num = recvfrom(udp_sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&hospital1, (socklen_t *)&len1);
            if(recv_num < 0)
            {
                perror("recvfrom error:");
                exit(1);
            }
            recv_buf[recv_num] = '\0';
            string sd1;
            sd1 = recv_buf;
            if(sd1 == "location not found"){
                if ( send(connfd,recv_buf,1024,0) <0) {
                    printf("send msg error: %s(errno :%d)\n",strerror(errno),errno);
                    exit(1);
                }
                printf("The Scheduler has received map information from Hospital A: the score = None and the distance = None\n");
            }else{
                istringstream ss1(sd1);
                ss1 >> score1;
                ss1 >> dis1;
                if (dis1 == 0){             // location of the client is the same as Hospital A
                    printf("The Scheduler has received map information from Hospital A: the score = None and the distance = None\n");
                }else{
                    printf("The Scheduler has received map information from Hospital A: the score = %.4f and the distance = %.2f\n", score1,dis1);
                }
            }
        }else{
            printf("The Scheduler has received map information from Hospital A: the score = None and the distance = None\n");
        }
        // receive score and dis from hospitalB
        if(!c2){
            recv_num = recvfrom(udp_sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&hospital2, (socklen_t *)&len2);
            if(recv_num < 0)
            {
                perror("recvfrom error:");
                exit(1);
            }
            string sd2;
            sd2 = recv_buf;
            if(sd2 == "location not found"){
                if ( send(connfd,recv_buf,1024,0) <0) {
                    printf("send msg error: %s(errno :%d)\n",strerror(errno),errno);
                    exit(1);
                }
                printf("The Scheduler has received map information from Hospital B: the score = None and the distance = None\n");
            }else{
                istringstream ss2(sd2);
                ss2 >> score2;
                ss2 >> dis2;
                if (dis2 == 0){             // location of the client is the same as Hospital B
                    printf("The Scheduler has received map information from Hospital B: the score = None and the distance = None\n");
                }else{
                    printf("The Scheduler has received map information from Hospital B: the score = %.4f and the distance = %.2f\n", score2,dis2);
                }
            }
        }else{
            printf("The Scheduler has received map information from Hospital B: the score = None and the distance = None\n");
        }
        // receive score and dis from hospitalC
        if(!c3){
            recv_num = recvfrom(udp_sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&hospital3, (socklen_t *)&len3);
            if(recv_num < 0)
            {
                perror("recvfrom error:");
                exit(1);
            }
            string sd3;
            sd3 = recv_buf;
            if(sd3 == "location not found"){
                if ( send(connfd,recv_buf,1024,0) <0) {
                    printf("send msg error: %s(errno :%d)\n",strerror(errno),errno);
                    exit(1);
                }
                printf("The Scheduler has received map information from Hospital C: the score = None and the distance = None\n");
            }else{
                istringstream ss3(sd3);
                ss3 >> score3;
                ss3 >> dis3;
                if (dis3 == 0){             // location of the client is the same as Hospital C
                    printf("The Scheduler has received map information from Hospital C: the score = None and the distance = None\n");
                }else{
                    printf("The Scheduler has received map information from Hospital C: the score = %.4f and the distance = %.2f\n", score3,dis3);
                }
            }
        }else{
            printf("The Scheduler has received map information from Hospital C: the score = None and the distance = None\n");
        }
        // make assignment: compare scores
        string max = "None";
        if(score1 < score2){
            if(score2 < score3){
                max = "HospitalC";
            }else if(score2 == score3){         // if the scores are the same, assign to the hospital with shorter distance
                if(dis2>dis3){
                    max = "HospitalC";
                }else{
                    max = "HospitalB";          // if three scores are the same, saaign to None
                }
            }else{
                max = "HospitalB";
            }
        }else if(score1 == score2){
            if(score1 < score3){
                max = "HospitalC";
            }else if(score1 > score3){
                if(dis1 > dis2){
                    max = "HospitalB";
                }else{
                    max = "HospitalA";
                }
            }else{
                if(dis1 > dis2){
                    if(dis2 > dis3){
                        max = "HospitalC";
                    }else{
                        max = "HospitalB";
                    }
                }else{
                    if(dis1 > dis3){
                        max = "HospitalC";
                    }else{
                        max = "HospitalA";
                    }
                }
            }
        }else{
            if(score1 < score3){
                max = "HospitalC";
            }else if(score1 == score3){
                if(dis1 > dis3){
                    max = "HospitalC";
                }else{
                    max = "HospitalA";
                }
            }else{
                max = "HospitalA";
            }
        }
        if((dis1*dis2*dis3==0 && c1+c2+c3==0) || c1*c2*c3 == 1){max = "None";} //same location as hospital or all fully occupied
        cout << "The Scheduler has assigned " << max << " to the client" << endl;
        char result[20];
        strcpy(result,max.c_str());
        // send result to client
        // reference: Beej's
        if ( send(connfd,result,1024,0) <0) {
            printf("send msg error: %s(errno :%d)\n",strerror(errno),errno);
            exit(1);
        }
        printf("The Scheduler has sent the result to client using TCP over port %d\n",TCP_PORT);
        
        // send to hospital
        // reference: Beej's
        if(max != "None"){
            char s[20] = "we pick you";
            if(max == "HospitalA" && c1 ==0){
                send_num = sendto(udp_sockfd, s, strlen(s), 0, (struct sockaddr *)&hospital1, len1);
                if(send_num < 0)
                {
                    perror("sendto error:");
                    exit(1);
                }
                printf("The Scheduler has sent the result to Hospital A using UDP over port %d\n", UDP_PORT);
            }else if(max == "HospitalB" && c2 == 0){
                send_num = sendto(udp_sockfd, s, strlen(s), 0, (struct sockaddr *)&hospital2, len2);
                if(send_num < 0)
                {
                    perror("sendto error:");
                    exit(1);
                }
                printf("The Scheduler has sent the result to Hospital B using UDP over port %d\n", UDP_PORT);
            }else if(max == "HospitalC" && c3 == 0){
                send_num = sendto(udp_sockfd, s, strlen(s), 0, (struct sockaddr *)&hospital3, len3);
                if(send_num < 0)
                {
                    perror("sendto error:");
                    exit(1);
                }
                printf("The Scheduler has sent the result to Hospital C using UDP over port %d\n", UDP_PORT);
            }
        }
    }
    close(connfd);
    close(tcp_sockfd);
    close(udp_sockfd);
    return 0;
}
