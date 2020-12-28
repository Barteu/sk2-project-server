// init.c

#include "init.h"



void initUsers(int *userCount,struct User *users)
{
    FILE *fp;
    fp = fopen("users.txt", "r");
    while(fscanf(fp,"%s %s %s", users[*userCount].login, users[*userCount].password, users[*userCount].nickname)>0)
    {
    printf("Odczytano: %s %s %s\n",   users[*userCount].login, users[*userCount].password, users[*userCount].nickname); 
    users[*userCount].id=((*userCount)+1);  // uwaga zmienione bardzo pozno
    users[*userCount].state='n';
    (*userCount)++;
    }   
     
    fclose(fp);
}

void initTopics(int *topicCount,struct Topic *topics)
{
    
    FILE *fp;
    char path[9]="./topics";
    char path2[64]="./topics";
    *topicCount=0;

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
		fscanf(fp,"%s",topics[*topicCount].name);
		printf("\nOdczytano topic: %s", topics[*topicCount].name  );
		while(fscanf(fp,"%d",&topics[*topicCount].subscribers[topics[*topicCount].subCount])>0)
		{
		printf(" %d,", topics[*topicCount].subscribers[topics[*topicCount].subCount]  ); 
        	topics[*topicCount].subCount++;	
		topics[*topicCount].id=(*topicCount);
		}
		(*topicCount)++;
		fclose(fp);
	}
   }

   closedir(dir);
}


void updateTopicFile(struct Topic topic)
{
    FILE *fp;
    char path[64]="./topics/";
    strcat(path,topic.name);
    strcat(path,".txt");
    fp = fopen(path,"w+");
    fprintf(fp,"%s\n",topic.name);
    for(int i=0;i<topic.subCount;i++)
    {
    fprintf(fp,"%d\n",topic.subscribers[i]);
    }
    fclose(fp);
}

void createMsgFile(struct Message msg)
{
    FILE *fp;
    char path[64]="./messages/";
    char s_id[64];

    snprintf(s_id,64,"%d",msg.id);
    strcat(path,s_id);
    strcat(path,".txt");
    fp = fopen(path,"w+");
    /* zapisze:
    id
    topicId
    ;
    title
    ;
    text
    ;
    to Send
    ;
    recipients id id id id     
    */


    fprintf(fp,"%d\n",msg.id);
    fprintf(fp,"%d\n;",msg.topicId);
    fprintf(fp,"%s\n;\n",msg.title);
    fprintf(fp,"%s\n;\n",msg.text);
    fprintf(fp,"%d\n;\n",msg.toSend);

    for(int i=0;i<msg.toSend;i++)
    {
    fprintf(fp,"%d\n",msg.recipients[i]);
    }


    fclose(fp);



}




