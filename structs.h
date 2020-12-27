// header structs.h

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string.h>



struct User{
  char login[20];
  char password[20];
  char nickname[20];
  int id;
  char state; // 'l' - zalogowany 'n' - niezalogowany 'x' - usuniety
  int cfd;
};

struct Topic {
  char name[40];
  int subscribers[1024];
  int subCount;
};
