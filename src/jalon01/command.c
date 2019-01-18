#include "command.h"


int bc(user_t person[],struct pollfd sock_table[],int i,int nfds,char* client_message){
  char *suiteChaine = malloc(TAILLE_MAX);
  char *tosend=malloc(TAILLE_MAX);
  suiteChaine = strchr(client_message, ' ') + sizeof(char);
  sprintf(tosend,"[%s]:%s",person[i-1].nickname,suiteChaine);
  printf("[%s]:%s\n",person[i-1].nickname,suiteChaine);
  int j;
  for(j = 1; j < nfds ; j++)
  {
    if(j != i)
    {
      send(sock_table[j].fd, tosend, TAILLE_MAX, 0);
    }
  }
  return 0;
}


int msg(user_t person[],struct pollfd sock_table[],int i,int nfds,char* client_message){

char *suiteChaine1 = malloc(TAILLE_MAX);
char *suiteChaine2 = malloc(TAILLE_MAX);
char *pseudo = malloc(TAILLE_MAX);
char *tosend=malloc(TAILLE_MAX);
suiteChaine1 = strchr(client_message, ' ') + sizeof(char);
suiteChaine2 = strchr(suiteChaine1, ' ');
strncpy(pseudo, suiteChaine1, strlen(suiteChaine1) - strlen(suiteChaine2));
suiteChaine2 = suiteChaine2 + sizeof(char);
sprintf(tosend,"[%s]:%s",person[i-1].nickname,suiteChaine2);
int j;
for(j = 0; j < nfds ; j++)
{
  if(strcmp(person[j].nickname, pseudo) == 0)
  {
    send(sock_table[j+1].fd, tosend, TAILLE_MAX, 0);
  }
}
return 0;
}




int nick(user_t person[],struct pollfd sock_table[],char* client_message,int i,int nfds)
{
  int k=0;
  int use=0;
  char *suiteChaine = malloc(TAILLE_MAX);
  suiteChaine = strchr(client_message, ' ');
  for (k=0;k<nfds;k++){
    if(strcmp(person[k].nickname, suiteChaine+1) == 0){
      use=1;
      break;
    }
  }
  if (use==1){
    send(sock_table[i].fd,"Pseudo already used", TAILLE_MAX,0);
    recv(sock_table[i].fd, client_message, TAILLE_MAX,0);
  }
  else{
    strcpy(person[i-1].nickname, suiteChaine +1);
    strcat(person[i-1].nickname, "\0");
    send(sock_table[i].fd,"Change done", TAILLE_MAX,0);
    printf("%s \n", person[i-1].nickname);
    recv(sock_table[i].fd, client_message, TAILLE_MAX,0);
    sprintf(person[i-1].IP, "%s", client_message);
    date(person,i-1);
  }
  return 0;
}



//Returns information about online users
int who(int nfds,user_t person[],struct pollfd sock_table[],int i){
  int k=0;
  char* tosend=malloc(TAILLE_MAX);
  send(sock_table[i].fd,"Connected users:",TAILLE_MAX,0);
  for (k=0; k < nfds-1; k++)
  {
    printf("\t-%s\n",person[k].nickname );
    sprintf(tosend,"\t-%s",person[k].nickname);
    send(sock_table[i].fd,tosend, TAILLE_MAX,0);
  }
  return 0;
}


//Returns information about a specific user set in command line argument
int whois(int nfds,user_t person[],struct pollfd sock_table[],char* client_message,int i, char *port){
  int k=0;
  int check =0;
  char *whoIs = malloc(TAILLE_MAX);

  whoIs = strchr(client_message, ' ') + sizeof(char);
  for (k = 0; k < nfds; k++)
  {
    if (strcmp(whoIs, person[k].nickname) == 0)
    {
      char message[TAILLE_MAX];
      sprintf(message, "Pseudo %s connected since %s with IP address %s on port number %s", person[k].nickname, person[k].date,person[k].IP,port);
      send(sock_table[i].fd, message, TAILLE_MAX,0);
      check=1;
      break;

    }
  }
  if (check==0){
    send(sock_table[i].fd, "Invalid Client", TAILLE_MAX,0);
  }
  return 0;
}




int quit(int nfds,struct pollfd sock_table[],char* client_message,int i,user_t person[]){
  printf("Client %d disconected\n",i);
  close(sock_table[i].fd);
  sock_table[i].fd = sock_table[nfds-1].fd;
  sock_table[nfds-1].fd=-1;
  sock_table[i].events = POLLIN;
  person[i-1]=person[nfds-2];


  return 0;
}
