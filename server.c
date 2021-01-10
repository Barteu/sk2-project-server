
#include "fun_serv.h"

int main(int argc, char** argv) {

  struct User users[1024];
  struct Topic topics[1024];
  int msgsIDs[2048];  // identyfikatory wiadomosci

  int userCount=0;  //liczba wczytanych kont uzytkownikow
  int topicCount=0; // liczba tematow
  int msgCount=0; // liczba wiadomosci oczekujacych na wyslanie
  
  initUsers(&userCount,users);
  initTopics(&topicCount,topics);
  initMsgs(&msgCount,msgsIDs);
  
  socklen_t slt;
  int sfd, cfd, fdmax, fda, rc, i, on = 1;
  struct sockaddr_in saddr, caddr;
  static struct timeval timeout;

  
  

  fd_set rmask, wmask, rLoginMask,wOkLoginMask,wBadLoginMask,wAlreadyLoggedMask,rLoggedMask, wTopicsMask,wMyTopicsMask,wSubMask,wUnsubMask,wNewTopicOk,wNewTopicError,wMsgOkMask,wMsgErrorMask,wMsgRequestMask;
  // rLoginMask - maska do odczytu danych logowania
  // wBadLoginMask -  podano zle dane logowania
  // wOkLoginMask - podano poprawne dane logowania
  // wAlreadyLoggedMask - uzytkownik jest juz zalogowany
  // rLoggedMask - odczyt danych od zalogowanych uzytkownikow
  // wTopicsMask -  wysylka listy wszystkich tematow 
  // wMyTopicsMask - wysylka listy zasubskrybowanych tematow klienta
  // wSubMask - wysyla info o zasubskrybowaniu tematu
  // wUnsubMask - wysyla info o odsubskrybowaniu tematu
  // wNewTopicOk - pomyslnie utworzono temat
  // wNewTopicError - blad w tworzeniu tematu
  // wMsgOkMask - pomyslnie umieszczono wiadomosc
  // wMsgErrorMask - nie umieszczono wiadomosci
  // wMsgRequestMask - wysylka wiadomosci

  char buffer[1024];  
  int rc2=0; // ile znakow otrzymano od klienta
  int loginCorrect=0;

  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(1234);
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
  bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
  listen(sfd, 50);

  FD_ZERO(&rmask);  
  FD_ZERO(&wmask);
  FD_ZERO(&rLoginMask);
  FD_ZERO(&wOkLoginMask);
  FD_ZERO(&wBadLoginMask);
  FD_ZERO(&wAlreadyLoggedMask);
  FD_ZERO(&rLoggedMask);
  FD_ZERO(&wTopicsMask);
  FD_ZERO(&wMyTopicsMask);
  FD_ZERO(&wSubMask);
  FD_ZERO(&wUnsubMask);
  FD_ZERO(&wNewTopicOk);
  FD_ZERO(&wNewTopicError);
  FD_ZERO(&wMsgOkMask);
  FD_ZERO(&wMsgErrorMask);
  FD_ZERO(&wMsgRequestMask);
  fdmax = sfd;

  while(1) {
    
    timeout.tv_sec = 600;
    timeout.tv_usec = 0;

    // skopiowanie cfd z masek pomocniczych do glownych(rmask,wmask)
    FD_SET(sfd, &rmask);
    for(i = sfd+1; i <= fdmax; i++)
    {
	if(FD_ISSET(i,&rLoggedMask)||FD_ISSET(i,&rLoginMask))
	{FD_SET(i,&rmask);}
	if(FD_ISSET(i,&wOkLoginMask)||FD_ISSET(i,&wBadLoginMask)||FD_ISSET(i,&wAlreadyLoggedMask)||FD_ISSET(i,&wTopicsMask)||FD_ISSET(i,&wMyTopicsMask)||FD_ISSET(i,&wSubMask)||FD_ISSET(i,&wUnsubMask)||FD_ISSET(i,&wNewTopicOk)||FD_ISSET(i,&wNewTopicError)||FD_ISSET(i,&wMsgOkMask)||FD_ISSET(i,&wMsgErrorMask)||FD_ISSET(i,&wMsgRequestMask))
	{FD_SET(i,&wmask);}
    }
	
    rc = select(fdmax+1, &rmask, &wmask, (fd_set*)0, &timeout);
	
    if (rc == 0) {
      printf("timed out\n");
      continue;
    }
    fda = rc;

    //fragment przyjmujacy polaczenia klienow
    if ((FD_ISSET(sfd, &rmask))&&(!FD_ISSET(sfd, &rLoginMask))&&(!FD_ISSET(sfd, &rLoggedMask))) { 
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

		rc2 = correctRead(i,buffer,1024); 
		
		loginCorrect = checkLogin(users,buffer,rc2,userCount,i); 
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
		else // rozlaczyl sie
		{
		disconnectUser(i,sfd,&fdmax,userCount,users,&rmask,&wmask,&rLoginMask,&rLoggedMask);
		}
	    }
	   // wysylka info o dobrym/zlym zalogowaniu
	   else if ( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wOkLoginMask) ) )
	   {
		
		correctWrite(i,"Logged in", 9);
	        FD_SET(i,&rLoggedMask);
		FD_CLR(i,&wOkLoginMask);
		FD_CLR(i,&wmask);
		
	   }
	   else if ( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wBadLoginMask) ) )
	   {
		
		correctWrite(i,"Incorrect login or password!", 28);
		
	 	FD_SET(i,&rLoginMask);
		FD_CLR(i,&wBadLoginMask);
		FD_CLR(i,&wmask);
	   }
	   else if ( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wAlreadyLoggedMask) ) )
	   {
		correctWrite(i,"User is already logged in!", 26);
	 	FD_SET(i,&rLoginMask);
		FD_CLR(i,&wAlreadyLoggedMask);
		FD_CLR(i,&wmask);
	   }
	   // wybor akcji przez klienta:
	   // 't' - lista wszystkich tematow
	   // '?' - rozlaczenie
  	   // 'o' - wylogowanie
	   // 'm' - moje subskrybcje
	   // 's' - (un)subskrybuj grupe 
	   // 'n' - nowy temat
           // 'e' - wyslij wiadomosc
	   // 'r' - odbierz wiadomosci
	   else if ( (FD_ISSET(i, &rmask))&&(FD_ISSET(i, &rLoggedMask))  ) 
	   {
		
		memset(buffer, 0, 1024);
		rc2 = correctRead(i,buffer,1024);

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
		else if(buffer[0]=='m')
		{
			FD_SET(i,&wMyTopicsMask);
			FD_CLR(i,&rmask);
		}
		else if(buffer[0]=='s')
		{
			if(subUnsub(i,users,topics,userCount,buffer+2,topicCount))	//buffer: "s|TOPIC_ID~"
			{
			FD_SET(i,&wSubMask);
			}
			else
			{
			FD_SET(i,&wUnsubMask);
			}
			FD_CLR(i,&rmask);
		}
		else if(buffer[0]=='n')
		{
			if(insertTopic(i,users,topics,userCount,&topicCount,buffer+2))
			{
			FD_SET(i,&wNewTopicOk);
			}
			else
			{
			FD_SET(i,&wNewTopicError);
			}
			FD_CLR(i,&rmask);
		}
		else if(buffer[0]=='e')	//buffer: "e|TOPIC_ID|TITLE|TEXT~"
		{
			if(addMessage(i,users,topics,userCount,topicCount,buffer+2,&msgCount,msgsIDs)) 
			{
			FD_SET(i,&wMsgOkMask);
			}
			else
			{
			FD_SET(i,&wMsgErrorMask);
			}
			FD_CLR(i,&rmask);
		}
		else if(buffer[0]=='r')
		{
			FD_SET(i,&wMsgRequestMask);
			FD_CLR(i,&rmask);
			printf("message request from cfd: %d\n",i);
		}

	   }
	   // wyslij liste wszystkich tematow
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wTopicsMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		sendTopicList(i,topics,topicCount);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wTopicsMask);	
	   }
	   // wyslij liste obserwowanych tematow uzytkownika
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wMyTopicsMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		sendMyTopicList(i,topics,topicCount,users,userCount);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wMyTopicsMask);	
	   }
	   // informacja Zasubskrybowano/Odsubskrybowano
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wSubMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		correctWrite(i,"You subscribed to the topic", 27);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wSubMask);	
	   }
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wUnsubMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		correctWrite(i,"You unsubscribed to the topic", 29);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wUnsubMask);	
	   }
	   // dodano / niedodano nowy temat
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wNewTopicOk) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		correctWrite(i,"Topic inserted", 14);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wNewTopicOk);	
	   }
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wNewTopicError) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		correctWrite(i,"This topic alredy exists", 24);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wNewTopicError);	
	   }
	   // wyslano/niewyslano wiadomosc
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wMsgOkMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		correctWrite(i,"Message sent", 12);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wMsgOkMask);	
	   }
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wMsgErrorMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		correctWrite(i,"Message not sent", 16);
		FD_CLR(i,&wmask);
		FD_CLR(i,&wMsgErrorMask);	
	   }
	   // wysylka wiadomosci
	   else if( (FD_ISSET(i, &wmask))&&(FD_ISSET(i, &wMsgRequestMask) )&&(FD_ISSET(i, &rLoggedMask) ) )
	   {
		if(sendMsg(i,users,topics,userCount,topicCount,&msgCount,msgsIDs)<1)
		{
		FD_CLR(i,&wMsgRequestMask);
		}
		FD_CLR(i,&wMsgRequestMask);
		FD_CLR(i,&wmask);
		
	   }

	}//end for loop
     
  }//end while loop

  close(sfd);
  return EXIT_SUCCESS;
}

