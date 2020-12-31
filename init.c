// init.c

#include "init.h"



void initUsers(int *userCount,struct User *users)
{
    FILE *fp;
    fp = fopen("users.txt", "r");
    while(fscanf(fp,"%s %s %s", users[*userCount].login, users[*userCount].password, users[*userCount].nickname)>0)
    {
      printf("loaded user: %s %s %s\n",   users[*userCount].login, users[*userCount].password, users[*userCount].nickname); 
      users[*userCount].id=((*userCount)+1);  
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
    if(!dir) 
    {
      printf("Error: directory did not open!\n");
    }
    while((direntry=readdir(dir))!=NULL) 
    {
	if(strlen(direntry->d_name)>2)
	{
		memset(path2, 0, 64); 
		strcat(path2,path);
		strcat(path2,"/");
		strcat(path2,direntry->d_name);
		fp = fopen(path2, "r");	
		fscanf(fp,"%s",topics[*topicCount].name);
		printf("loaded topic: %s\n", topics[*topicCount].name  );

		while(fscanf(fp,"%d",&topics[*topicCount].subscribers[topics[*topicCount].subCount])>0)
		{
		  //printf(" %d,", topics[*topicCount].subscribers[topics[*topicCount].subCount]  ); 
		  topics[*topicCount].subCount++;	
		  topics[*topicCount].id=(*topicCount);
		}
		
		(*topicCount)++;
		fclose(fp);
	}
    }

   closedir(dir);
}


void initMsgs(int *msgCount,int *msgsIDs)
{

    FILE *fp;
    char path[11]="./messages";
    char path2[64]="./messages";
 
    DIR *dir;
    struct dirent *direntry; 
    dir = opendir(path);
    if(!dir) 
    {
      printf("Error: directory did not open!\n");
    }

    while((direntry=readdir(dir))!=NULL) 
    {
	if(strlen(direntry->d_name)>2)
	{
		memset(path2, 0, 64); 
		strcat(path2,path);
		strcat(path2,"/");
		strcat(path2,direntry->d_name);
		fp = fopen(path2, "r");	
		fscanf(fp,"%d",&msgsIDs[*msgCount]);
		printf("loaded msg Id: %d\n", msgsIDs[*msgCount]);
		(*msgCount)++;
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
    msg_id
    topic_id
    ;
    title
    ;
    text
    ;
    to_Send
    ;
    user_id 
    user_id
    user_id  
    */
    fprintf(fp,"%d\n",msg.id);
    fprintf(fp,"%d\n;\n",msg.topicId);
    fprintf(fp,"%s\n;\n",msg.title);
    fprintf(fp,"%s\n;\n",msg.text);
    fprintf(fp,"%d\n;\n",msg.toSend);

    for(int i=0;i<msg.toSend;i++)
    {
      fprintf(fp,"%d\n",msg.recipients[i]);
    }
    fclose(fp);
}


int scanMsg(struct Message *msg,int recipient_id,int indx)
{
 
    FILE *fp;
    char path[64]="./messages/";
    char s_id[32];
    snprintf(s_id,32,"%d",indx);
    strcat(path,s_id);
    strcat(path,".txt");
    fp = fopen(path,"r");
    int l=0;
    char c;
    char buffer[1024];
    memset(buffer,0,1024);
    int found=0;

    if((*msg).id==-1) //jezeli nie wczytano jeszcze wiadomosci
    {
	    fscanf(fp,"%d",&msg->id);
	    fscanf(fp,"%d",&msg->topicId);

	    do
	    {
	      c=fgetc(fp);
	    }while(c!=';');

	    do
	    {
		c=fgetc(fp);
		if(c!='\n'&&c!=';')
		{
		  buffer[l]=c;
		  l++;
		}
		
	    }while(c!=';');

	    fgetc(fp);
	    strncpy((*msg).title,buffer,strlen(buffer));
	    memset(buffer,0,1024);
	    l=0;

	    do
	    {
		c=fgetc(fp);
		if(c!=';')
		{
		  buffer[l]=c;
		  l++;
		}
	    }while(c!=';');

	    strncpy((*msg).text,buffer,strlen(buffer)-1);
	    fscanf(fp,"%d",&msg->toSend);

	    do
	    {
		c=fgetc(fp);
	    }while(c!=';');

	    for(int i=0;i<(*msg).toSend;i++)
	    {
	        fscanf(fp,"%d",&msg->recipients[i]);

		if((*msg).recipients[i]==recipient_id)
		{
		  found=1;
		  printf("found msg for user: %d\n", recipient_id);
		}

	    }
	    if(found==0)
	    {
	     (*msg).id=-1;
		  memset((*msg).recipients,0,sizeof (*msg).recipients);
	       	  memset((*msg).title,0,64);
	     	  memset((*msg).text,0,1024);
			
	    }
    }
// jezeli juz wczesniej wczytano jedna wiadomosc to
// sprawdzam tylko czy uzytkownik jest adresatem innych wiadomosci i nie zapisuje
    else 
    {
	    fscanf(fp,"%d",&l);
	    fscanf(fp,"%d",&l);
	    l=0;
	    do
	    {
		c=fgetc(fp);
	    }while(c!=';');

	    do
	    {
		c=fgetc(fp);
		buffer[l]=c;
		l++;
	    }while(c!=';');

	    memset(buffer,0,1024);

	    l=0;
	    do
	    {
		c=fgetc(fp);
		buffer[l]=c;
		l++;
	    }while(c!=';');

	    fscanf(fp,"%d",&l);
	    int toSend=l;
	    do
	    {
		c=fgetc(fp);
	    }while(c!=';');

	    for(int i=0;i<toSend;i++)
	    {
	        fscanf(fp,"%d",&l);
		if(l==recipient_id)
		{
		found=1;
		}
	    }
    }
   fclose(fp);
   if(found)
   {
	return 1;
   }

   return 0;

}


void delMsg(int id)
{
 char buff[32];
 snprintf(buff,32,"%d",id);
 char path[256];
 strcpy(path,"./messages/");
 strcat(path,buff);
 strcat(path,".txt");
 remove(path);

}


