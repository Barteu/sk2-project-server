#include "init.h"

int checkLogin(struct User *users, char *buffer,int len, int userCount,int cfd);

void prepareMasks(int sfd,int fdmax,fd_set *rmask,fd_set *wmask,fd_set *rLoginMask,fd_set *wOkLoginMask,fd_set *wBadLoginMask,fd_set *rLoggedMask,fd_set *wTopicsMask);
