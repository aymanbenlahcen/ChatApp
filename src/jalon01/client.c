#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>
#include "command.h"

#define TAILLE_MAX 1000
char namefile[];



void *receive(void *sock){
  char *server_response = malloc(TAILLE_MAX);
  do {
    int rece=recv(*((int *)sock), server_response, TAILLE_MAX, 0);
    if (rece == -1) {
      perror("Failed to receive message from the server");
      exit(EXIT_FAILURE);
    }
    printf("%s\n", server_response);
    if (strncmp(server_response,"A file named",12)==0){
      recv(*((int *)sock), server_response, TAILLE_MAX, 0);
      }
  }while(1);
  return NULL;
}





int main(int argc,char* argv[])
{
  if (argc != 3) //port number + IP
  {
    fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
    return 1;
  }

  //get address info from the server
  struct sockaddr_in sock_host;
  int sock;


  //get the socket
  sock = socket(AF_INET, SOCK_STREAM,0);
  if (sock == -1) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  memset(&sock_host, '\0', sizeof(sock_host));
  sock_host.sin_family = AF_INET;
  sock_host.sin_port  = htons(atoi(argv[1]));
  inet_aton(argv[2], &sock_host.sin_addr);


  //connect to remote socket
  int con;
  con = connect(sock, (struct sockaddr *)&sock_host, sizeof(sock_host));
  if (con == -1) {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }

  //get the client IP address
  struct hostent * host;
  char *namen=malloc(5000);
  gethostname(namen,500);
  host = gethostbyname(namen);
  char * name = inet_ntoa(*(struct in_addr *)host->h_addr);


  char *message = malloc(TAILLE_MAX);
  char*message2=malloc(TAILLE_MAX);
  char *server_response = malloc(TAILLE_MAX);

  int check=0;
  //Ensure the user enters a login
  do{
    do {
      printf("Please login with /nick <pseudo> (no space)\n");
      gets(message);
      strcpy(message2,message);
    } while((strncmp(message,"/nick",5) != 0 )||(strchr(strchr(message2,' ')+sizeof(char),' ')!=NULL));
    send(sock, message, TAILLE_MAX,0);
    recv(sock,message,TAILLE_MAX,0);
    send(sock, name, TAILLE_MAX,0);
    if (strcmp("Change done",message)==0){
      check=1;
    }
    else{
      printf("Pseudo already used\n");
    }

  }while (check==0);
  pthread_t thread;
  pthread_create(&thread, NULL, receive, (void *) &sock);
  printf("\n");
  printf("Type /help for commands list\n");
  printf("\n");
  do{
    gets(message);

    if(strncmp(message,"/help",5) == 0)
    {
      printf("[/nick] : set/change the nickname\n");
      printf("[/who]  : returns online users\n");
      printf("[/whois]: returns information about a specific user\n");
      printf("[/quit] : leave the chat\n");
      printf("[/bc]   : send a message to all connected users\n");
      printf("[/msg <pseudo>]: send a message to <pseudo>\n");
      printf("[/create]: create a channel \n");
      printf("[/join]: join a channel \n");
      printf("[/send <channel>]: send a message to <channel>\n");
      printf("[/which] : display all existing channel\n");
      printf("[/file <pseudo> <pathfile>] : send a file to <pseudo>\n");
    }

    //send message to the server
    else{
      int envoi = send(sock, message, TAILLE_MAX,0);
      if (envoi == -1) {
        perror("Failed to send the message");
        exit(EXIT_FAILURE);
      }
      if(strncmp(message,"/nick",5) == 0){
        send(sock, name, TAILLE_MAX,0);
      }
      if (strcmp(message,"/quit") == 0)
      {
        printf("Disconected\n");
        close(sock);
        break;
      }
      if(strcmp(message, "Y") == 0) {

        //struct arg arg1 = {9005,"127.0.0.1","/net/t/abenlahcen/Desktop/Pg_reseaux/src/jalon01/inbox/smthg"};
        char namefile2[] = "/net/t/abenlahcen/Desktop/Pg_reseaux/src/jalon01/inbox/smthg";
        //struct arg arg2 = {9005,"127.0.0.1","/net/t/abenlahcen/Desktop/RE2164.tar.gz"};
        char namefile[] = "/net/t/abenlahcen/Desktop/bdx.jpg";
        printf("%s \n", namefile);
        pthread_t thread1;
        //transfile : qui va recevoir (Ce que tu penses et ben le contraire qui est correct)
        pthread_create(&thread1, NULL, transfile,(void *) &namefile);
        //recvfile(9005,"127.0.0.1","./inbox/patate");
        pthread_t thread2;
        pthread_create(&thread2, NULL, recvfile, (void *) &namefile2);
        //transfile(9005,"127.0.0.1", "/net/t/abenlahcen/Desktop/exemple.c");
      }
      if(strcmp(message, "n") == 0) {
        printf("File transfer cancelled\n");
      }
    }
  }while (strcmp(message,"/quit")!=0);


  return 0;
}
