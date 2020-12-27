#include "fun_serv.h"

int checkLogin(struct User *users, char *buffer,int len,int userCount, int cfd)
{
        write(1,"Otrzymano login i haslo: ",25);
	write(1,buffer,len);
	write(1,"\n",1);

	if(buffer[0]=='?')
	{
	  return -1;  // client wylaczyl program
	}
	
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
			users[i].state='l';
			users[i].cfd=cfd;
			printf("Pomyslne zalogowano: %s\n",users[i].login);
			return 1;	//poprawne dane
		}
	}

	return 0;  // niepoprawne dane
}

void prepareMasks(int sfd,int fdmax,fd_set *rmask,fd_set *wmask,fd_set *rLoginMask,fd_set *wOkLoginMask,fd_set *wBadLoginMask,fd_set *rLoggedMask,fd_set *wTopicsMask)
{ 
   
    FD_SET(sfd, rmask);
    for(int i = sfd+1; i <= fdmax; i++)
    {
	if(FD_ISSET(i,rLoggedMask)||FD_ISSET(i,rLoginMask))
	{FD_SET(i,rmask);}
	else if(FD_ISSET(i,wOkLoginMask)||FD_ISSET(i,wBadLoginMask)||FD_ISSET(i,wTopicsMask))
	{FD_SET(i,wmask);}
    }



}
