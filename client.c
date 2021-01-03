#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string.h>



int main(int argc,char* argv[])
{
 
    int fd, rc;
    char buffer[1024];
    struct sockaddr_in saddr;
    struct hostent* addrent;

    char okLogin[40]="karol3|Rower1234"; 
    //char badLogin[40]="siema;eniu";
   
    addrent = gethostbyname(argv[1]);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[2]));
    memcpy(&saddr.sin_addr.s_addr,addrent->h_addr, addrent->h_length);
    connect(fd, (struct sockaddr*)&saddr,sizeof(saddr));

    
    int loop = 1;
    int opcja=0;
    while(loop)
    {
	printf("1-poprawnie zaloguj 2-recznie zaloguj \n3-odbierz topicsy 4-logOut \n5-moje topic 0-exit\n6-sub/unsub 2|7-nowy temat\n8-wyslij wiadomosc|9- popros o wiadomosc \n"); 
	memset(buffer, 0, 1024);   
	scanf("%d",&opcja);
	switch(opcja)
	{
	case 1:
	 
    	 write(fd,okLogin,17);
         write(1,"wyslano log\n",12);

         rc = read(fd,buffer,1024);
         write(1,buffer,rc);	
	 printf("\n\n");			
	break;
	
	case 2:

 	  printf("podaj: login|haslo\n");
	char dane[64];
	scanf("%s",dane);
	 write(fd,dane,strlen(dane)+1);

         rc = read(fd,buffer,1024);
         write(1,buffer,rc);	
	 printf("\n\n");	
	break;

	case 3:
	write(fd,"t",2);
	rc = read(fd,buffer,1024);
	write(1,buffer,rc);
	printf("\n\n");
	break;
	case 4:
	write(fd,"o",2);
	break;


	case 5:
	write(fd,"m",2);
	rc = read(fd,buffer,1024);
	write(1,buffer,rc);
	printf("\n\n");
	break;

	case 6:
	printf("Sub/unsub topic id2\n");
	write(fd,"s|2",4);
	rc = read(fd,buffer,1024);
	write(1,buffer,rc);
	printf("\n\n");
	break;

	case 7:
	printf("Probuje dodac temat Klocki\n");
	write(fd,"n|Klocki",8);
	rc = read(fd,buffer,1024);
	write(1,buffer,rc);
	printf("\n\n");
	break;

	case 8:
	printf("Podaj ID tematu|Tytul|Tresc  jezeli dasz ? to automatycznie\n"); 
        char id_tyt_tr[1024];
	scanf("%s",id_tyt_tr);
	char do_wyslania[1024];
	if(id_tyt_tr[0]!='?')
	{
		strcpy(do_wyslania,"e|");
        	strcat(do_wyslania,id_tyt_tr);
		
	}
	else
	{
	strcpy(do_wyslania,"e|2|Przykladowy Tytul|Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc dui neque, pulvinar vitae est sed, congue vestibulum urna. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Proin interdum ante neque, vitae consequat turpis varius sed. Nulla nec facilisis mauris. Aenean volutpat ullamcorper nisi, in auctor felis tristique malesuada. Quisque eleifend consequat ligula, sit amet luctus neque scelerisque at. Duis vel neque sit amet lacus volutpat consequat. Sed sollicitudin massa nec ante porttitor bibendum.\n Sed rutrum, justo rutrum tempor euismod, dui mauris pharetra dui, et rhoncus magna sem id nisi. Quisque imperdiet turpis sed lacus pulvinar malesuada. Vestibulum laoreet elit in orci convallis aliquet. Sed mollis sem id mollis molestie. In eu est est. Etiam ut bibendum lacus. Donec tincidunt volutpat bibendum. Vestibulum sed nisi eget velit ultrices nec.");
	}
	
        write(fd,do_wyslania,strlen(do_wyslania));
	rc = read(fd,buffer,1024);
	write(1,buffer,rc);
	printf("\n\n");
	break;


	case 9:
	printf("Prosze o wiadomosc\n");
	write(fd,"r",2);
	rc = read(fd,buffer,1024);
	write(1,buffer,rc);
	printf("\n\n");
	break;


	case 0:
	default:
	loop=0;
	break;
	}
    }
    

    write(fd,"?",2);
    close(fd);

    return EXIT_SUCCESS;
}


