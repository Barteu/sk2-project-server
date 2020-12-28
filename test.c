#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string.h>

#include <dirent.h> 


struct User{
  char login[20];
  char password[20];
  char nickname[20];
  int id;
};

struct Topic {
  char name[40];
  int subscribers[1024];
  int subCount;
};


void dodaj(int sd,fd_set *rmask)
{
FD_SET(sd, rmask);
FD_SET(6, rmask);
}


int main(int argc,char* argv[])
{
    struct User users[1024];
    struct Topic topics[1024];
    int userCount=0;

    FILE *fp;

    fp = fopen("users.txt", "r");

    while(fscanf(fp,"%s %s %s", users[userCount].login, users[userCount].password, users[userCount].nickname)>0)
    {
    printf("Odczytano: %s %s %s\n",   users[userCount].login, users[userCount].password, users[userCount].nickname); 
    users[userCount].id=userCount;
    userCount++;
    }   
     
    fclose(fp);

  
    fp = fopen("topics.txt", "r");
    
  
    int topicCount=0;
    
   
    char path[9]="./topics";
    char path2[64];
    

    DIR *dir;
    struct dirent *direntry; 
    dir = opendir(path);
    if(!dir) {
      printf("Error: directory did not open!\n");
    }
    while((direntry=readdir(dir))!=NULL) {
	if(strlen(direntry->d_name)>2)
	{
		memset(path2, 0, 64); 
		strcat(path2,path);
		strcat(path2,"/");
		strcat(path2,direntry->d_name);
		fp = fopen(path2, "r");	
		fscanf(fp,"%s",topics[topicCount].name);
		printf("\nOdczytano topic: %s", topics[topicCount].name  );
		while(fscanf(fp,"%d",&topics[topicCount].subscribers[topics[topicCount].subCount])>0)
		{
		printf(" %d,", topics[topicCount].subscribers[topics[topicCount].subCount]  ); 
        	topics[topicCount].subCount++;	
		}
		(topicCount)++;
		fclose(fp);
	}
   }


    printf("\n\n");
    
    
 
    
 
   printf("\n");


    return EXIT_SUCCESS;
}




