#include "command.h"



int main (int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: RE216_SERVER port\n");
    return 1;
  }

  //defining variables
  int server_socket;
  int client_socket;
  char *client_message = malloc(TAILLE_MAX);
  salon_t *salon = NULL;
  supervision_t *salon_list = NULL;
  int counter = 0; //nombre d'éléments dans la liste chainée salon_list;
  char *name_user = malloc(TAILLE_MAX);

  server_socket = socket(AF_INET, SOCK_STREAM,0);
  if (server_socket == -1) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }
  char *name = malloc(sizeof(TAILLE_MAX));
  gethostname(name,TAILLE_MAX);

  //define the server address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(atoi(argv[1]));
  server_address.sin_addr.s_addr = INADDR_ANY;

  //bind the socket to our specified Ip and port
  if (bind (server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
    perror("failed to bind");
    exit(EXIT_FAILURE);
  }
  //listen through the server socket
  if (listen(server_socket, 20) == -1) {   //max of 20 clients
    perror("failed to listen");
    exit(EXIT_FAILURE);
  }

  //structure declaration
  struct pollfd sock_table[CLIENT_MAX + 1];
  memset(sock_table, '\0', sizeof(sock_table));
  int nfds = 0;
  sock_table[nfds].fd = server_socket;
  sock_table[nfds].events = POLLIN;
  nfds++;


  int check;
  user_t person[CLIENT_MAX];
  while (1) {
    //check the activity
    check = poll(sock_table, nfds, -1);
    if ( check == -1)
    {
      perror("Error in poll");
      exit(EXIT_FAILURE);
    }
    else
    {
      int i;
      for ( i = 0; i < nfds; i++ ) {
        if( sock_table[i].revents & POLLIN ) {
          if(sock_table[i].fd == server_socket) {
            //accept new client
            client_socket = accept(server_socket, NULL, NULL);
            if(client_socket == -1)
            {
              perror("failed to accept");
              exit(EXIT_FAILURE);
            }
            else {
              printf("New client\n");
              sock_table[nfds].fd = client_socket;
              person[nfds].sock = sock_table[nfds].fd;
              sock_table[nfds].events = POLLIN;
              nfds++;
            }
          }
          else
          {
            recv(sock_table[i].fd, client_message, TAILLE_MAX, 0);
            if(strncmp(client_message,"/quit",5) == 0 && strncmp(client_message,"/quitsalon",10) != 0){
              quit(nfds,sock_table,client_message,i,person);
              nfds--;
            }
            else if (strncmp(client_message, "/nick", 5) == 0)
            {
              nick(person,sock_table,client_message,i,nfds);
              sprintf(person[i-1].port, "%s",argv[1]);
            }
            else if(strncmp(client_message,"/whois",6) == 0)
            {
              whois(nfds,person,sock_table,client_message,i, argv[1]);
            }
            else if (strncmp(client_message,"/who", 4) == 0 && strncmp(client_message,"/whois",6) !=0)
            {
              who(nfds,person,sock_table,i);
            }
            else if(strncmp(client_message,"/bc",3) == 0){
              bc(person,sock_table,i,nfds,client_message);
            }
            else if(strncmp(client_message,"/msg",4) == 0){
              msg(person,sock_table,i,nfds,client_message);
            }
            else if(strncmp(client_message,"/create",7) == 0) {
              char *suiteChaine = malloc(TAILLE_MAX);
              char *tosend=malloc(TAILLE_MAX);
              suiteChaine = strchr(client_message, ' ') + sizeof(char);
              salon = create(suiteChaine, &person[i]);
              printf("%s has created %s\n",&person[i-1].nickname,salon->name_salon);
              supervision_t *tmp = salon_list;
              while(tmp != NULL)
              {
                counter++;
                tmp = tmp->next;
              }
              if(counter == 0)
              {
                salon_list = initialize(salon);
                sprintf(tosend, "You have created channel %s\n",salon_list->salon->name_salon);
                send(sock_table[i].fd, tosend, TAILLE_MAX,0);
              }
              if (counter >= 1)
              {
                salon_list = add(salon_list,salon);
                sprintf(tosend, "You have created channel %s\n",salon_list->salon->name_salon);
                send(sock_table[i].fd, tosend, TAILLE_MAX,0);
              }
            }
            else if(strncmp(client_message,"/join",5) == 0) {
              char *suiteChaine = malloc(TAILLE_MAX);
              char *tosend=malloc(TAILLE_MAX);
              suiteChaine = strchr(client_message, ' ') + sizeof(char);
              supervision_t *tmp = salon_list;
              int check=0;
              while(tmp != NULL)
              {
                printf("%s \n", tmp->salon->name_salon);
                if(strcmp(tmp->salon->name_salon, suiteChaine) == 0)
                {
                  tmp->salon = join(tmp->salon, &person[i]);
                  printf("%s has joined %s\n",&person[i-1].nickname,tmp->salon->name_salon);
                  sprintf(tosend, "You have joined channel %s\n",tmp->salon->name_salon);
                  tmp->number_clients++;
                  printf("%i \n", tmp->number_clients);
                  send(sock_table[i].fd, tosend, TAILLE_MAX,0);
                  check=1;
                }
                tmp = tmp->next;
              }
            }
            else if(strncmp(client_message,"/send",5) == 0 && strncmp(client_message,"/sendfile", 9) != 0) {
              char *suiteChaine1 = malloc(TAILLE_MAX);
              char *suiteChaine2 = malloc(TAILLE_MAX);
              char *nom_salon = malloc(TAILLE_MAX);
              char *tosend=malloc(TAILLE_MAX);
              suiteChaine1 = strchr(client_message, ' ') + sizeof(char);
              suiteChaine2 = strchr(suiteChaine1, ' ');
              strncpy(nom_salon, suiteChaine1, strlen(suiteChaine1) - strlen(suiteChaine2));
              suiteChaine2 = suiteChaine2 + sizeof(char);
              sprintf(tosend,"[%s]:%s",person[i-1].nickname,suiteChaine2);
              supervision_t *tmp1 = salon_list;
              salon_t *tmp2 = salon;
              salon_t *tmp3 = salon;
              int check=0;
              while(tmp1 != NULL){
                if(strcmp(tmp1->salon->name_salon, nom_salon) == 0)
                {
                  printf("salon existe\n");
                  tmp2 = tmp1->salon;
                  tmp3 = tmp1->salon;
                  while(tmp2 != NULL)
                  {
                    printf("en recherche si le client envoyant le msg est dans le salon\n");
                    if(strcmp(tmp2->person->nickname, person[i].nickname) == 0) {
                      printf("le client voulant envoyer le msg appartient au salon\n");

                      while(tmp3 != NULL)
                      {
                        printf("On est la\n");
                        int j = 0;
                        for(j = 0; j <= nfds; j++)
                        {
                          if(strcmp(tmp3->person->nickname, person[j].nickname) == 0 && strcmp(tmp3->person->nickname, tmp2->person->nickname) != 0)
                          {
                            send(person[j].sock, tosend, TAILLE_MAX, 0);
                          }
                        }
                        tmp3 = tmp3->next;
                      }
                    }
                    tmp2 = tmp2->next;
                  }
                  check=1;
                }
                tmp1 = tmp1->next;
              }
            }
            else if(strncmp(client_message,"/quitsalon",10) == 0) {
              char *suiteChaine = malloc(TAILLE_MAX);
              char *tosend = malloc(TAILLE_MAX);
              suiteChaine = strchr(client_message, ' ') + sizeof(char);
              supervision_t *tmp = salon_list;
              salon_t *tmp2 = salon;
              while(tmp != NULL)
              {
                printf("%s \n", tmp->salon->name_salon);
                if(strcmp(tmp->salon->name_salon, suiteChaine) == 0)
                {
                  tmp2 = tmp->salon;
                  tmp->number_clients--;
                  printf("nombre de clients est :%i \n", tmp->number_clients);
                  if(tmp->number_clients > 0)
                  {
                    printf("%s has quit %s\n",&person[i-1].nickname,tmp2->name_salon);
                    sprintf(tosend, "You have quit channel %s\n",tmp2->name_salon);
                    send(sock_table[i].fd, tosend, TAILLE_MAX,0);
                    tmp2 = quit_salon(tmp2, &person[i]);
                  }
                  if(tmp->number_clients == 0)
                  {
                    sprintf(tosend, "You have quit channel %s\n",tmp2->name_salon);
                    send(sock_table[i].fd, tosend, TAILLE_MAX,0);
                    tmp = Delete(tmp, tmp2);
                  }
                }
                tmp = tmp->next;
              }
              salon = tmp2;
              salon_list->salon = salon;
            }
            else if(strncmp(client_message,"/file",5) == 0) {
              char *suiteChaine1 = malloc(TAILLE_MAX);
              char *suiteChaine2 = malloc(TAILLE_MAX);
              char *user= malloc(TAILLE_MAX);
              char *tosend=malloc(TAILLE_MAX);
              char *addr1=malloc(TAILLE_MAX);
              char *addr2=malloc(TAILLE_MAX);
              strcpy(addr1,person[i-1].IP);
              suiteChaine1 = strchr(client_message, ' ') + sizeof(char);
              suiteChaine2 = strchr(suiteChaine1, ' ');
              strncpy(user, suiteChaine1, strlen(suiteChaine1) - strlen(suiteChaine2));
              suiteChaine2 = suiteChaine2 + sizeof(char);
              sprintf(tosend, "A file named %s is sent to you from %s. Do you accept ? [Y/n]\n", suiteChaine2,person[i-1].nickname);
              int j;
              for(j = 0; j < nfds ; j++)
              {
                if(strcmp(person[j].nickname, user) == 0)
                {
                  strcpy(addr2,person[j].IP);
                  send(sock_table[j+1].fd,tosend, TAILLE_MAX, 0);
                  send(sock_table[j+1].fd,addr2,TAILLE_MAX,0);
                }
              }
            }
            else if(strncmp(client_message,"/which",6) == 0) {
              supervision_t *tmp = salon_list;
              char *tosend=malloc(TAILLE_MAX);
              send(sock_table[i].fd,"Channels: ",TAILLE_MAX,0);
              while(tmp != NULL)
              {
                sprintf(tosend, "\t-%s",tmp->salon->name_salon);
                send(sock_table[i].fd, tosend, TAILLE_MAX,0);
                tmp = tmp->next;
              }
            }
          }
        }
      }
    }
  }
  return 0;
}
