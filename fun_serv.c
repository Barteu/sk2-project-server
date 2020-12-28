#include "fun_serv.h"

int checkLogin(struct User *users, char *buffer,int len,int userCount, int cfd)
{
        
	if(buffer[0]=='?')
	{
	  return -1;  // client wylaczyl program
	}
	
	write(1,"Otrzymano login i haslo: ",25);
	write(1,buffer,len);
	write(1,"\n",1);

	char login[20];
	char password[20];
	int j=0;
	while(buffer[j]!=';')
	{
	login[j]=buffer[j];
	j++;
	}

	j++;
	strncpy(password,buffer+j,len-j);

	printf("log: %s, pass: %s\n:", login,password);
	for(int i=0;i<userCount;i++)
	{
	 if(strcmp(login,users[i].login)==0&&strcmp(password,users[i].password)==0)
		{
			if(users[i].state!='l')
			{
			users[i].state='l';
			users[i].cfd=cfd;
			printf("Pomyslne zalogowano: %s\n",users[i].login);
			return 1;	//poprawne dane
			}
			else
			{
			return 2;
			}
		}
	}

	return 0;  // niepoprawne dane
}


void logOut(int cfd,struct User *users, int userCount)
{
	for(int i=0;i<userCount;i++)
	{
		if(users[i].cfd==cfd)
		{
		users[i].cfd=-1;
		users[i].state='n';
		break;
		}
	}
}

void disconnectUser(int cfd,int sfd,int *fdmax,int userCount, struct User *users,fd_set *rmask,fd_set *wmask,fd_set *rLoginMask,fd_set *rLoggedMask){

printf("Klient o fd: %d ,rozlaczyl sie!\n",cfd);
logOut(cfd,users,userCount);
FD_CLR(cfd,rmask);
FD_CLR(cfd,wmask);
FD_CLR(cfd,rLoggedMask);	
FD_CLR(cfd,rLoginMask);

if (cfd == *fdmax)
while((*fdmax) > sfd && !FD_ISSET(*fdmax, rLoggedMask) && !FD_ISSET(*fdmax, rLoginMask)&& !existsCfd(users,*fdmax,userCount))
{
close(*fdmax);
(*fdmax) -= 1;
}

};


int existsCfd(struct User *users,int cfd,int userCount)
{
	for(int i=0;i<userCount;i++)
	{
	 if(users[i].cfd==cfd)
		{
		return 1;
		}
	}
	return 0;
}

int userIdByCfd(struct User *users,int cfd,int userCount)
{
	for(int i=0;i<userCount;i++)
	{
	 if(users[i].cfd==cfd)
		{
		return users[i].id;
		}
	}
	return -1;
}

int isSubscriber(struct Topic topic,int id)
{
  for(int i=0;i<topic.subCount;i++)
	{
	if(id==topic.subscribers[i])
		{
		return 1;
		}
	}
  return 0;
}

void sendTopicList(int cfd,struct Topic *topics,int topicCount)
{
 	char buffer[2048]="\0";
	char bufferID[4]="\0";
 	for(int i=0;i<topicCount;i++)
	{
	strcat(buffer,topics[i].name);
	strcat(buffer,";");
	snprintf(bufferID,4,"%d",topics[i].id);
	strcat(buffer,bufferID);
	strcat(buffer,";");
	}
 	write(cfd,buffer,strlen(buffer));
}



void sendMyTopicList(int cfd,struct Topic *topics,int topicCount,struct User *users,int userCount)
{
	char buffer[2048]="\0";
	char bufferID[4]="\0";
	int id = userIdByCfd(users,cfd,userCount);
 	for(int i=0;i<topicCount;i++)
	{
	if(isSubscriber(topics[i],id))
		{
		strcat(buffer,topics[i].name);
		strcat(buffer,";");
		snprintf(bufferID,4,"%d",topics[i].id);
		strcat(buffer,bufferID);
		strcat(buffer,";");
		}
	}
	if(strlen(buffer)>0)
	{
	write(cfd,buffer,strlen(buffer));
	}
 	else
	{
	write(cfd,";",2);
	}
}





int subUnsub(int cfd,struct User *users,struct Topic *topics,int userCount,char *buffer)
{
	int id = userIdByCfd(users,cfd,userCount);
	int topic_id=atoi(buffer);
	
	if(isSubscriber(topics[topic_id],id))
	{
	for(int i=0;i<topics[topic_id].subCount;i++)
	{
		if(id==topics[topic_id].subscribers[i])  // wyszukuje na ktorym jest miejsu i zastepuje go ostatnim
		{
		 topics[topic_id].subscribers[i]=topics[topic_id].subscribers[topics[topic_id].subCount-1];		
		 topics[topic_id].subCount-=1;
		 topics[topic_id].subscribers[topics[topic_id].subCount]=-1;
		 break;
		}
	}	
	updateTopicFile(topics[topic_id]);

	return 0;
	}
	else
	{
	topics[topic_id].subscribers[ topics[topic_id].subCount ] = id;
	topics[topic_id].subCount+=1;

	updateTopicFile(topics[topic_id]);
	return 1;
	}
	

}


int insertTopic(int cfd,struct User *users,struct Topic *topics,int userCount,int *topicCount,char *buffer)
{
	// sprawdzam czy jest juz taki temat
	for(int i=0;i<*topicCount;i++)
	{
		if(strcmp(buffer,topics[i].name)==0)
		{
		return 0;
		}
	}
	
	strcpy(topics[*topicCount].name,buffer);
	topics[*topicCount].id=*topicCount;
	topics[*topicCount].subCount=1;
	topics[*topicCount].subscribers[0]=userIdByCfd(users,cfd,userCount);
	(*topicCount)++;

	updateTopicFile(topics[(*topicCount)-1]);
	return 1;
}

int addMessage(int cfd,struct User *users,struct Topic *topics,int userCount,int topicCount,char *buffer,int *msgCount,int *msgsIDs)
{
  struct Message msg;
  int j=0;
  int z=0;
  char buff[64];  // na poczatku buforuje ID tematu a potem tytul

  int sender_id=userIdByCfd(users,cfd,userCount);

  while(buffer[j]!=';')
  {
    buff[j]=buffer[j];
    j++;
  }
  
  msg.id=(*msgCount)+1;
  msg.topicId=atoi(buff);
  
  if(isSubscriber(topics[msg.topicId],sender_id))
  {
	  memset(buff,0,64);
	  j++;
	  while(buffer[j]!=';')
	  {
	    buff[z]=buffer[j];
	    j++;
	    z++;
	  }
	  strcpy(msg.title,buff);
	  strcpy(msg.text,buffer+j+1);
	  
	  msg.toSend=0;
	  
	  for(int i=0;i<userCount;i++)
	  {
	    if(isSubscriber(topics[msg.topicId], users[i].id)&&users[i].id!=sender_id)
		{
		  msg.recipients[msg.toSend]=users[i].id;
		  msg.toSend+=1;
		}
	  }
		
        msgsIDs[*msgCount]=msg.id;
        (*msgCount)+=1;

        createMsgFile(msg);

	return 1;
  }
  else
  {
  	return 0; //proba wyslania wiadomosci do niesubskrybowanego tematu
  }  
}


int sendMsg(int cfd,struct User *users,struct Topic *topics,int userCount,int topicCount,int *msgCount,int *msgsIDs)
{
	struct Message msg;
	msg.id=-1;

	int recipient_id=userIdByCfd(users,cfd,userCount);

	int msgCountCpy=*msgCount;
	int found=0;
	char buffer[1024]="/0";
	char buff[64]="/0";

	for(int j=0;j<msgCountCpy;j++)
	{
	found+=scanMsg(&msg,recipient_id,msgsIDs[j]);		
		if(found==0)
		{
		memset(msg.recipients,0,sizeof msg.recipients);
	       	memset(msg.title,0,64);
	     	memset(msg.text,0,1024);
		}	
	   printf("Stitle: %d %s\n",j,msg.title);
		printf("Stext: %d %s\n",j,msg.text);
	}
	printf("-----------\n");
	if(found)
	{

		printf("title: %s\n",msg.title);
		printf("text: %s\n",msg.text);




		snprintf(buffer,64,"%d",found);		// ILE_WIADOMOSCI;NAZWA_KOLEJKI;ID_KOLEJKI;TEMAT;WIADOMOSC
		strcat(buffer,";");		
		for(int i=0;i<topicCount;i++)
		{
			if(topics[i].id==msg.topicId)
			{
			strcpy(buff,topics[i].name);
			break;
			}
		}
		strcat(buffer,buff);
		strcat(buffer,";");
		memset(buff,0,64);
		snprintf(buff,64,"%d",msg.topicId);	
		strcat(buffer,buff);
		strcat(buffer,";");
		strcat(buffer,msg.title);
		strcat(buffer,";");
		strcat(buffer,msg.text);
		write(cfd,buffer,strlen(buffer));

		if(msg.toSend>1)
		{
		for(int i=0;i<msg.toSend;i++)
		{
			if(recipient_id==msg.recipients[i])
			{
			msg.recipients[i]=msg.recipients[msg.toSend-1];
			msg.recipients[msg.toSend-1]=0;
			break;
			}
		}
		msg.toSend-=1;
		createMsgFile(msg);
		memset(msg.text,0,1024);
		}
		else
		{
		// mozna usunac wiadomosc
			for(int i=0;i<*msgCount;i++)
			{
				if(msgsIDs[i]==msg.id)
				{
					msgsIDs[i]=msgsIDs[(*msgCount)-1];
					msgsIDs[(*msgCount)-1]=0;
					(*msgCount)-=1;
					delMsg(msg.id);
					break;
				}
			}		
		}
	}
	else
	{
	write(cfd,"0;",3);

	}
	
	return found-1;
}








