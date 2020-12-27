// init.c

#include "init.h"



void initUsers(int *userCount,struct User *users)
{
    FILE *fp;
    fp = fopen("users.txt", "r");
    while(fscanf(fp,"%s %s %s", users[*userCount].login, users[*userCount].password, users[*userCount].nickname)>0)
    {
    printf("Odczytano: %s %s %s\n",   users[*userCount].login, users[*userCount].password, users[*userCount].nickname); 
    users[*userCount].id=(*userCount);
    users[*userCount].state='n';
    (*userCount)++;
    }   
     
    fclose(fp);
}

void initTopics(int *topicCount,struct Topic *topics)
{
    FILE *fp;
    fp = fopen("topics.txt", "r");
  
    
    while(fscanf(fp,"%s",topics[*topicCount].name)>0)
    {
    topics[*topicCount].subCount=0;
    printf("\nOdczytano topic: %s", topics[*topicCount].name  ); 
     while(fscanf(fp,"%d",&topics[*topicCount].subscribers[topics[*topicCount].subCount])>0)
	{
	printf(" %d,", topics[*topicCount].subscribers[topics[*topicCount].subCount]  ); 
        topics[*topicCount].subCount++;	
	}
    (*topicCount)++;
    }
    fclose(fp);

}




