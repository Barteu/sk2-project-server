
#include "fun_serv.h"

int main(int argc, char** argv) {

// struktury itp
    struct User users[1024];
    struct Topic topics[1024];
    int userCount=0;
    int topicCount=0;

   initUsers(&userCount,users);
   initTopics(&topicCount,topics);
  printf("\nA teraz czesc servera\n");
//

  socklen_t slt;
  int sfd, cfd, fdmax, fda, rc, i, on = 1;
  struct sockaddr_in saddr, caddr;
  static struct timeval timeout;
  fd_set rmask, wmask, rLoginMask,wOkLoginMask,wBadLoginMask,wAlreadyLoggedMask,rLoggedMask, wTopicsMask;
  // rLoginMask - maska do odczytu danych logowania
  // wBadLoginMask -  podano zle dane logowania
  // wOkLoginMask - poprawne dane
  // wAlreadyLoggedMask - jest juz zalogowany
  // rLoggedMask - zalogowani uzytkownicy
  // wTopicsMask - do klienta wyslemy liste wszystkich tematow 

  char buffer[1024];  
  int rc2=0; // ile znakow otrzymano od klienta

  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(1234);
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
  bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
  listen(sfd, 5);

  FD_ZERO(&rmask);  
  FD_ZERO(&wmask);
  FD_ZERO(&rLoginMask);
  FD_ZERO(&wOkLoginMask);
  FD_ZERO(&wBadLoginMask);
  FD_ZERO(&wAlreadyLoggedMask);
  FD_ZERO(&rLoggedMask);
  FD_ZERO(&wTopicsMask);

  fdmax = sfd;

  while(1) {
    
  
   
    prepareMasks(sfd,fdmax,&rmask,&wmask,&rLoginMask,&wOkLoginMask,&wBadLoginMask,&wAlreadyLoggedMask,&rLoggedMask,&wTopicsMask);

	


    timeout.tv_sec = 5 * 60;
    timeout.tv_usec = 0;
    rc = select(fdmax+1, &rmask, &wmask, (fd_set*)0, &timeout);
	

    if (rc == 0) {
      printf("timed out\n");
      continue;
    }
    fda = rc;

     //fragment przyjmujacy polaczenia klienow
    if ((FD_ISSET(sfd, &rmask))&&(!FD_ISSET(sfd, &rLoginMask))&&(!FD_ISSET(sfd, &rLoggedMask))) {  //rLoginMask jest dla juz polaczonych klientow
      fda -= 1;
      slt = sizeof(caddr);
      cfd = accept(sfd, (struct sockaddr*)&caddr, &slt);
      printf("new connection: %s\n",
      inet_ntoa((struct in_addr)caddr.sin_addr));
      FD_SET(cfd, &rLoginMask);
      FD_CLR(cfd,&rmask);

      if (cfd > fdmax) fdmax = cfd;
    }

    for(i = sfd+1; i<=fdmax && fda>0; i++)
	{
	    //fragment do logowania
	    if( (FD_ISSET(i, &rmask))&&(FD_ISSET(i, &rLoginMask) ) )
	    {
		fda -= 1;
		memset(buffer, 0, 1024);
		rc2 = read(i,buffer,1024);
		int loginCorrect = checkLogin(users,buffer,rc2,userCount,i); 
		if(loginCorrect==1) // zalogowano
		{
		    FD_CLR(i,&rLoginMask);
		    FD_CLR(i,&rmask);
		    FD_SET(i, &wOkLoginMask);
		}
		else if(loginCorrect==0) // zly login
		{
		    FD_CLR(i,&rLoginMask);
		    FD_CLR(i,&rmask);
		    FD_SET(i, &wBadLoginMask);
		}
		else if(loginCorrect==2) // jest juz zalogowany
		{
		    FD_CLR(i,&rLoginMask);
		    FD_CLR(i,&rmask);
		    FD_SET(i, &wAlreadyLoggedMask);
		}
		else
		{
		disconnectUser(i,sfd,&fdmax,userCount,users,&rmask,&wmask,&rLoginMask,&rLoggedMask);
		}
	    }
	   // wyslanie info o dobrym/zlym zalogowaniu
	   else if ( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wOkLoginMask) ) )
	   {
		write(i,"Poprawnie zalogowano", 21);
	        FD_SET(i,&rLoggedMask);
		FD_CLR(i,&wOkLoginMask);
		FD_CLR(i,&wmask);
		
	   }
	   else if ( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wBadLoginMask) ) )
	   {
		write(i,"Bledne dane logowania!", 23);
	 	FD_SET(i,&rLoginMask);
		FD_CLR(i,&wBadLoginMask);
		FD_CLR(i,&wmask);
	   }
	   else if ( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wAlreadyLoggedMask) ) )
	   {
		write(i,"Uzytkownik jest juz zalogowany!", 32);
	 	FD_SET(i,&rLoginMask);
		FD_CLR(i,&wAlreadyLoggedMask);
		FD_CLR(i,&wmask);
	   }
	   // wybor akcji:
	   // 't'-lista wszystkich tematow, '?' - nagle rozlaczenie
  	   // 'o'-wylogowanie, 
	   else if ( (FD_ISSET(i, &rmask))&&(FD_ISSET(i, &rLoggedMask))  ) //&&(!FD_ISSET(i, &wmask))
	   {
		
		memset(buffer, 0, 1024);
		rc2 = read(i,buffer,1024);

		if(buffer[0]=='t')
		{
	
			FD_SET(i,&wTopicsMask);
			FD_CLR(i,&rmask);
		}
		else if(buffer[0]=='o')
		{
			logOut(i,users,userCount);
			FD_CLR(i,&rmask);
			FD_CLR(i,&rLoggedMask);
			FD_SET(i,&rLoginMask);
		}
		else if(buffer[0]=='?')
		{
		disconnectUser(i,sfd,&fdmax,userCount,users,&rmask,&wmask,&rLoginMask,&rLoggedMask);
		}

	   }
	   // wyslij liste tematow
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wTopicsMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		sendTopicList(i,topics,topicCount);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wTopicsMask);
		
	   }
	}
     


   
  }

  close(sfd);
  return EXIT_SUCCESS;
}

