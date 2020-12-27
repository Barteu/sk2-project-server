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

    char index[7];
    
    addrent = gethostbyname(argv[1]);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[2]));
    memcpy(&saddr.sin_addr.s_addr,addrent->h_addr, addrent->h_length);
    connect(fd, (struct sockaddr*)&saddr,sizeof(saddr));

    
    strcpy(index, argv[3]);    
    write(fd,index,7);
    rc = read(fd,buffer,1024);
    write(1,buffer,rc);
    close(fd);

    return EXIT_SUCCESS;
}


