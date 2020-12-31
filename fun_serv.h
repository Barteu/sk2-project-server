#include "init.h"

//sprawdza dane logowania
int checkLogin(struct User *users, char *buffer,int len, int userCount,int cfd);

//wylogowuje uzytkownika
void logOut(int cfd,struct User *users, int userCount);

//rozlacza klienta
void disconnectUser(int cfd,int sfd,int *fdmax,int userCount, struct User *users,fd_set *rmask,fd_set *wmask,fd_set *rLoginMask,fd_set *rLoggedMask);

//sprawdza czy fd klienta jest aktaulnie przypisany jakiemus uzytkownikowi
int existsCfd(struct User *users,int cfd,int userCount);

//wysyla liste wszystkich tematow klientowi
void sendTopicList(int cfd,struct Topic *topics,int topicCount);

//zwraca ID uzytkownika o uzywanym cfd
int userIdByCfd(struct User *users,int cfd,int userCount);

//sprawdza czy uzytkownik o ID jest subskrybentem danego tematu
int isSubscriber(struct Topic topic,int id);

//wysyla liste obserwowanych tematow klientowi
void sendMyTopicList(int cfd,struct Topic *topics,int topicCount,struct User *users,int userCount);

//subskrbuje lub odsubskrybuje temat przez uzytkownika
int subUnsub(int cfd,struct User *users,struct Topic *topics,int userCount,char *buffer);

//dodaje nowy temat
int insertTopic(int cfd,struct User *users,struct Topic *topics,int userCount,int *topicCount,char *buffer);

//dodaje nowa wiadomosc
int addMessage(int cfd,struct User *users,struct Topic *topics,int userCount,int topicCount,char *buffer,int *msgCount,int *msgsIDs);

//wysyla wiadomosc uzytkownikowi o danym cfd
int sendMsg(int cfd,struct User *users,struct Topic *topics,int userCount,int topicCount,int *msgCount,int *msgsIDs);
