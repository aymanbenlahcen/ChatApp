#include "command.h"



supervision_t *initialize(salon_t *salon)
{
  supervision_t *salon_list = malloc(sizeof(supervision_t));
  if (salon_list == NULL)
  {
    printf("Error\n");
  }
  salon_list->salon = salon;
  salon_list->number_clients = 1;
  salon_list->next = NULL;
  return salon_list;
}


salon_t *create(char *name, user_t *person)
{
  salon_t *salon = malloc(sizeof(salon_t));
  if(salon == NULL)
  {
    printf("Error\n");
  }

  strcpy(salon->name_salon, name);
  salon->person = person;
  salon->next = NULL;

  return salon;
}


salon_t *join(salon_t *head, user_t *person)
{
  salon_t *salon = malloc(sizeof(salon_t));
  if(salon == NULL)
  {
    printf("Error\n");
  }

  strcpy(salon->name_salon, head->name_salon);
  salon->person = person;
  salon->next = head;
  return salon;
}



supervision_t *add(supervision_t *head, salon_t *salon)
{
  supervision_t *salon_list = malloc(sizeof(supervision_t));
  if (salon_list == NULL)
  {
    printf("Error\n");
  }

  salon_list->salon = salon;
  salon_list->number_clients = 1;
  salon_list->next = head;
  return salon_list;
}

salon_t *quit_salon(salon_t *pos, user_t *person)
{
  if (pos == NULL)
  return NULL;

  if (pos->person == person) {
    salon_t *tmp;
    tmp = pos->next;
    free(pos);
    return tmp;
  }
  pos->next = quit_salon(pos->next, person);
  return pos;
}



supervision_t *Delete(supervision_t *pos, salon_t *salon)
{
  if (pos == NULL)
  return NULL;

  if (pos->salon == salon) {
    supervision_t *tmp;
    tmp = pos->next;
    free(pos);
    return tmp;
  }
  pos->next = Delete(pos->next, salon);
  return pos;
}


void *date(user_t users[],int i)
{
  time_t now;
  struct tm *ts;
  char buf[80];
  now = time(NULL);
  ts = localtime(&now);
  strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
  sprintf(users[i].date,"%s",buf);
}
