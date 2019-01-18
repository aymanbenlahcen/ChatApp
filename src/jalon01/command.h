#ifndef COMMAND
#define COMMAND

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

#define TAILLE_MAX 1000
#define CLIENT_MAX 20

//Structures
struct arg {
  int port;
  char const *adr;
  char const *path;
};


//User structure
typedef struct
{
  char nickname[50];
  char date[80];
  char IP[50];
  char port[50];
  int sock;
}user_t;

//Salon Structure
typedef struct salon
{
  char name_salon[50];
  user_t *person;
  struct salon *next;
}salon_t;

//Salon supervision Structure (Pointeur placement)
typedef struct supervision
{
  salon_t *salon;
  int number_clients;
  struct supervision *next;
}supervision_t;





//Chat fonctions (command.c)


//Send a message in broadcast
int bc(user_t person[],struct pollfd sock_table[],int i,int nfds,char* client_message);

//Send a message to a specidied user
int msg(user_t person[],struct pollfd sock_table[],int i,int nfds,char* client_message);

//Change user nickname
int nick(user_t person[],struct pollfd sock_table[],char* client_message,int i,int nfds);

//Returns online users
int who(int nfds,user_t person[],struct pollfd sock_table[],int i);

//Returns information about a specific user set in command line argument
int whois(int nfds,user_t person[],struct pollfd sock_table[],char* client_message,int i, char *port);

//Disconect a user from the server
int quit(int nfds,struct pollfd sock_table[],char* client_message,int i,user_t person[]);







//Linked list fonctions(chaine.c)


//Initialize a linked list
supervision_t *initialize(salon_t *salon);

//Create a room
salon_t *create(char *name, user_t *person);

//Join a room
salon_t *join(salon_t *head, user_t *person);

//Leave a channel
salon_t *quit_salon(salon_t *pos, user_t *person);

//Control a new room
supervision_t *add(supervision_t *head, salon_t *salon);

//Delete a room
supervision_t *Delete(supervision_t *pos, salon_t *salon);

//Return hour and date
void *date(user_t users[],int i);









//send a file
int transfile(void *argument);

//recv a file
int recvfile(void *argument);





#endif
