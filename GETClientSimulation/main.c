//
//  main.c
//  GETClientSimulation
//
//  Created by Ozan Deniz on 31/08/15.
//  Copyright (c) 2015 Ozan Deniz. All rights reserved.
//

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h> //for threading , link with lpthread
#include <time.h>

void* simulate_eye_tracker(void* sock_val){
    
    int* sock = (int*)sock_val;
    
    char* message = "Merhaba";
    
    clock_t start = clock();
    clock_t diff;
    
    diff = clock() - start;
    
    int msec = (int)(diff * 1000 / CLOCKS_PER_SEC);
    printf("%d\n",msec/1000);
    
    while(msec/1000 < 30){

        //Send some data
        if( send(*sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
        }
        
        diff = clock() - start;
        msec = (int)(diff * 1000 / CLOCKS_PER_SEC);
        
        printf("%d\n",msec/1000);
    }
    return 0;
}

void* listener_handler(void* sock_val){
    
    int* sock = (int*)sock_val;
    char server_reply[8];
    
    //keep communicating with server
    while(1)
    {
        //Receive a reply from the server
        if( recv(*sock , server_reply , 7 , 0) < 0)
        {
            puts("Recv failed");
            break;
        }
        
        printf("xxx");
        printf("%s",server_reply);
        printf("xxx\n");
    }
    return 0;
}

int create_socket(){
    
    int sock;
    
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket\n");
    }
    puts("Socket created");
    
    return sock;
}

int create_handlers(int sock){
    
    pthread_t listener_thread;
    pthread_t simulator_thread;
    
    if( pthread_create( &listener_thread , NULL ,  listener_handler , &sock) < 0)
    {
        perror("Could not create listener thread");
        return -1;
    }
    
    //pthread_join( listener_thread , NULL);
    puts("Listener handler assigned");
    
    if( pthread_create( &simulator_thread , NULL ,  simulate_eye_tracker , &sock) < 0)
    {
        perror("Could not create simulator thread");
        return -1;
    }
    
    pthread_join( simulator_thread , NULL);
    puts("Simulator handler assigned");
    
    return 0;
}

int create_connection(char *ip_address, int port){
    
    // IP: 127.0.0.1
    // PORT: 8888
    
    struct sockaddr_in server;
    
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    
    int sock = create_socket();
    
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Connection failed. Error\n");
        return -1;
    }
    puts("Connected\n");
    
    create_handlers(sock);
    return 0;
}

int main(int argc , char *argv[])
{
    create_connection("127.0.0.1",8888);
    return 0;
}