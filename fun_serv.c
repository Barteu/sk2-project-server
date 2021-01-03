#include "fun_serv.h"

int checkLogin(struct User *users, char *buffer,int len,int userCount, int cfd)
{
        
	if(buffer[0]=='?')
	{
	  return -1;  // client wylaczyl program
	}

	char login[20]="/0";
	char password[20]="/0";
	int j=0;
	while(buffer[j]!='|')
	{
	  login[j]=buffer[j];
	  j++;
	}

	j++;
	strncpy(password,buffer+j,len-j);

	printf("recieved login: %s, password: %s\n", login,password);
	for(int i=0;i<userCount;i++)
	{
	  if(strcmp(login,users[i].login)==0&&strcmp(password,users[i].password)==0)
	  {
		if(users[i].state!='l')
		{
		  users[i].state='l';
		  users[i].cfd=cfd;
		  printf("successfully logged in -> Login: %s cfd: %d\n",users[i].login,users[i].cfd);
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
	     printf("user (id: %d) logged out! \n",users[i].id);
             break;
	  }
	}
}

void disconnectUser(int cfd,int sfd,int *fdmax,int userCount, struct User *users,fd_set *rmask,fd_set *wmask,fd_set *rLoginMask,fd_set *rLoggedMask){

  
  logOut(cfd,users,userCount);
  FD_CLR(cfd,rmask);
  FD_CLR(cfd,wmask);
  FD_CLR(cfd,rLoggedMask);	
  FD_CLR(cfd,rLoginMask);
  printf("client disconnected! (cfd: %d) \n",cfd);
  if (cfd == *fdmax)
  {
    while((*fdmax) > sfd && !FD_ISSET(*fdmax, rLoggedMask) && !FD_ISSET(*fdmax, rLoginMask)&& !existsCfd(users,*fdmax,userCount))
    {
       close(*fdmax);
       (*fdmax) -= 1;
    }

  }

}


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


int topicIndxByID(struct Topic *topics,int topic_id,int topicCount)
{
	for(int i=0;i<topicCount;i++)
	{
	  if(topics[i].id==topic_id)
	  {
	  return i;
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
	  strcat(buffer,"|");
	  snprintf(bufferID,4,"%d",topics[i].id);
	  strcat(buffer,bufferID);
	  strcat(buffer,"|");
	}
 	write(cfd,buffer,strlen(buffer));
	printf("sent topic list to cfd: %d\n",cfd);
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
		  strcat(buffer,"|");
		  snprintf(bufferID,4,"%d",topics[i].id);
		  strcat(buffer,bufferID);
		  strcat(buffer,"|");
		}
	}
	if(strlen(buffer)>0)
	{
	  write(cfd,buffer,strlen(buffer));
	  printf("sent subscription list to cfd: %d\n",cfd);
	}
 	else
	{
	  write(cfd,"|",2);
	  printf("sent empty list to cfd: %d\n",cfd);
	}
}





int subUnsub(int cfd,struct User *users,struct Topic *topics,int userCount,char *buffer, int topicCount)
{
	int id = userIdByCfd(users,cfd,userCount);
	int topic_id=atoi(buffer);
	
	int topicINDX = topicIndxByID(topics,topic_id,topicCount);

	if(isSubscriber(topics[topicINDX],id))
	{
	for(int i=0;i<topics[topicINDX].subCount;i++)
	{
		if(id==topics[topicINDX].subscribers[i])  // wyszukuje na ktorym jest miejsu i zastepuje go ostatnim
		{
		   topics[topicINDX].subscribers[i]=topics[topicINDX].subscribers[topics[topicINDX].subCount-1];		
		   topics[topicINDX].subCount-=1;
		   topics[topicINDX].subscribers[topics[topicINDX].subCount]=-1;
		   break;
		}
	}	
	updateTopicFile(topics[topicINDX]);
	printf("user (id: %d) unsubscribed topic (id: %d)\n",id,topic_id);
	return 0;
	}
	else
	{
	topics[topicINDX].subscribers[ topics[topicINDX].subCount ] = id;
	topics[topicINDX].subCount+=1;

	updateTopicFile(topics[topicINDX]);
	printf("user (id: %d) subscribed topic (id: %d)\n",id,topic_id);
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
	
	int userID=userIdByCfd(users,cfd,userCount);

	strcpy(topics[*topicCount].name,buffer);
	topics[*topicCount].id=*topicCount+1;
	topics[*topicCount].subCount=1;
	topics[*topicCount].subscribers[0]=userID;
	(*topicCount)++;
	
	updateTopicFile(topics[(*topicCount)-1]);
	printf("user (id: %d) inserted new topic: %s\n",userID,buffer);
	return 1;
}

int addMessage(int cfd,struct User *users,struct Topic *topics,int userCount,int topicCount,char *buffer,int *msgCount,int *msgsIDs)
{
  struct Message msg;
  int j=0;
  int z=0;
  char buff[64];  // na poczatku buforuje ID tematu a potem tytul

  int sender_id=userIdByCfd(users,cfd,userCount);

  while(buffer[j]!='|')
  {
    buff[j]=buffer[j];
    j++;
  }
  
  msg.id=(*msgCount)+1;
  msg.topicId=atoi(buff);
  
  int topicINDX = topicIndxByID(topics,msg.topicId,topicCount);

  if(isSubscriber(topics[topicINDX],sender_id))
  {
	  memset(buff,0,64);
	  j++;
	  while(buffer[j]!='|')
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
	    if(isSubscriber(topics[topicINDX], users[i].id)&&users[i].id!=sender_id)
		{
		  msg.recipients[msg.toSend]=users[i].id;
		  msg.toSend+=1;
		}
	  }
		
        msgsIDs[*msgCount]=msg.id;
        (*msgCount)+=1;

        createMsgFile(msg);
	printf("user (id: %d) added new message\n",sender_id);
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
		//if(found==0)
		//{
		//  memset(msg.recipients,0,sizeof msg.recipients);
	       //	  memset(msg.title,0,64);
	     	//  memset(msg.text,0,1024);
		//}	
	}
	if(found)
	{
		snprintf(buffer,64,"%d",found);		// ILE_WIADOMOSCI|NAZWA_KOLEJKI|ID_KOLEJKI|TEMAT|WIADOMOSC
		strcat(buffer,"|");		
		for(int i=0;i<topicCount;i++)
		{
			if(topics[i].id==msg.topicId)
			{
			  strcpy(buff,topics[i].name);
			  break;
			}
		}
		strcat(buffer,buff);
		strcat(buffer,"|");
		memset(buff,0,64);
		snprintf(buff,64,"%d",msg.topicId);	
		strcat(buffer,buff);
		strcat(buffer,"|");
		strcat(buffer,msg.title);
		strcat(buffer,"|");
		strcat(buffer,msg.text);
		write(cfd,buffer,strlen(buffer));
		printf("sent message to user (id: %d)\n",recipient_id);

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
					memset(msg.recipients,0,sizeof msg.recipients);
	       	  			memset(msg.title,0,64);
	     	  			memset(msg.text,0,1024);
					break;
				}
			}		
		}
	}
	else // nie znaleziono
	{
	  write(cfd,"0|",3);
	  printf("sent empty message to user (id: %d)\n",recipient_id);

	}
	
	return found-1;
}








