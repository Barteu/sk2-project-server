//init.h
#include "structs.h"

//wczytuje uzytkownikow z pliku
void initUsers(int *userCount,struct User *users);

//wczytuje tematy z pliku
void initTopics(int *topicCount,struct Topic *topics);

//wczytuje identyfikatory wiadomosci z pliku
void initMsgs(int *msgCount,int *msgsIDs);

//aktualizuje lub tworzy plik z tematem
void updateTopicFile(struct Topic topic);

//aktualizuje lub tworzy plik z wiadomoscia
void createMsgFile(struct Message msg);

//wczytuje wiadomosc z pliku do struktury
int scanMsg(struct Message *msg,int recipient_id,int indx);

//usuwa wiadomosc
void delMsg(int id);
