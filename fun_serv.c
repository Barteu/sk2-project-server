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


void prepareMasks(int sfd,int fdmax,fd_set *rmask,fd_set *wmask,fd_set *rLoginMask,fd_set *wOkLoginMask,fd_set *wBadLoginMask,fd_set *wAlreadyLoggedMask,fd_set *rLoggedMask,fd_set *wTopicsMask)
{ 
   
    FD_SET(sfd, rmask);
    for(int i = sfd+1; i <= fdmax; i++)
    {
	if(FD_ISSET(i,rLoggedMask)||FD_ISSET(i,rLoginMask))
	{FD_SET(i,rmask);}
	if(FD_ISSET(i,wOkLoginMask)||FD_ISSET(i,wBadLoginMask)||FD_ISSET(i,wAlreadyLoggedMask)||FD_ISSET(i,wTopicsMask))
	{FD_SET(i,wmask);}
    }
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

void sendTopicList(int cfd,struct Topic *topics,int topicCount)
{
 	char buffer[2048]="\0";
 	for(int i=0;i<topicCount;i++)
	{
	strcat(buffer,topics[i].name);
	strcat(buffer,";");
	}
 	write(cfd,buffer,strlen(buffer));
}



