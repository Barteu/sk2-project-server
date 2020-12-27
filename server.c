#include "init.h"


int main(int argc, char** argv) {

// struktury itp
    struct User users[1024];
    struct Topic topics[1024];
    int userCount=0;
    int topicCount=0;

   initUsers(&userCount,users);
   initTopics(&topicCount,topics);
  printf("\nA teraz czesc servera\n");

  socklen_t slt;
  int sfd, cfd, fdmax, fda, rc, i, on = 1;
  struct sockaddr_in saddr, caddr;
  static struct timeval timeout;
  fd_set rmask, wmask, r2mask, w2_err_mask,w2_ok_mask;

  char buffer[128];
  char index[7] = "141324";
  int rc2=0;

  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(1234);
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
  bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
  listen(sfd, 5);

  FD_ZERO(&rmask);
  FD_ZERO(&wmask);
  FD_ZERO(&r2mask);
  FD_ZERO(&w2_err_mask);
  FD_ZERO(&w2_ok_mask);

  fdmax = sfd;




  while(1) {
    
    rmask = r2mask;
    wmask= w2_err_mask;
    for(i = sfd+1; i <= fdmax; i++)
	{
		if(FD_ISSET(i,&w2_ok_mask))
		{FD_SET(i,&wmask);}
	}


    FD_SET(sfd, &rmask);

    timeout.tv_sec = 5 * 60;
    timeout.tv_usec = 0;
    rc = select(fdmax+1, &rmask, &wmask, (fd_set*)0, &timeout);
    if (rc == 0) {
      printf("timed out\n");
      continue;
    }
    fda = rc;
    if ((FD_ISSET(sfd, &rmask))&&(!FD_ISSET(sfd, &r2mask))) {
      fda -= 1;
      slt = sizeof(caddr);
      cfd = accept(sfd, (struct sockaddr*)&caddr, &slt);
      printf("new connection: %s\n",
             inet_ntoa((struct in_addr)caddr.sin_addr));
      FD_SET(cfd, &r2mask);
      FD_CLR(cfd,&rmask);
      if (cfd > fdmax) fdmax = cfd;
    }

    for (i = sfd+1; i <= fdmax && fda > 0; i++)
	{
		if(FD_ISSET(i,&r2mask)&&FD_ISSET(i,&rmask))
		{
		memset(buffer, 0, 128);
		rc2 = read(i,buffer,128);
	    	write(1,"recieved number: ",18);
	    	write(1,buffer,rc2);
	    	write(1,"\n",1);
		
		if(!strcmp(index,buffer))
		{
		 FD_SET(i, &w2_ok_mask);
		}
		else
		{
		FD_SET(i, &w2_err_mask);
		}

		FD_CLR(i,&r2mask);
		
		}
	      else if ((FD_ISSET(i, &wmask))&&(FD_ISSET(i, &w2_err_mask))) {
		fda -= 1;
		write(i, "Error!\n", 7);
		close(i);
		FD_CLR(i, &w2_err_mask);
		if (i == fdmax)
		  while(fdmax > sfd && !FD_ISSET(fdmax, &w2_err_mask))
		    fdmax -= 1;
	      }
	      else if((FD_ISSET(i, &wmask))&&(FD_ISSET(i, &w2_ok_mask)))
		{
		fda -= 1;
		write(i, "Bartłomiej Szymkowiak\n", 23);
		close(i);
		FD_CLR(i, &w2_ok_mask);
		if (i == fdmax)
		  while(fdmax > sfd && !FD_ISSET(fdmax, &w2_ok_mask))
		    fdmax -= 1;
		}
	
        }

  }


  close(sfd);
  return EXIT_SUCCESS;
}

