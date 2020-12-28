#include "init.h"

int checkLogin(struct User *users, char *buffer,int len, int userCount,int cfd);

void logOut(int cfd,struct User *users, int userCount);

void disconnectUser(int cfd,int sfd,int *fdmax,int userCount, struct User *users,fd_set *rmask,fd_set *wmask,fd_set *rLoginMask,fd_set *rLoggedMask);

int existsCfd(struct User *users,int cfd,int userCount);

void sendTopicList(int cfd,struct Topic *topics,int topicCount);

int userIdByCfd(struct User *users,int cfd,int userCount);

int isSubscriber(struct Topic topic,int id);

void sendMyTopicList(int cfd,struct Topic *topics,int topicCount,struct User *users,int userCount);

int subUnsub(int cfd,struct User *users,struct Topic *topics,int userCount,char *buffer);

int insertTopic(int cfd,struct User *users,struct Topic *topics,int userCount,int *topicCount,char *buffer);

int addMessage(int cfd,struct User *users,struct Topic *topics,int userCount,int topicCount,char *buffer,int *msgCount,int *msgsIDs);
