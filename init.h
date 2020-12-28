//init.h
#include "structs.h"


void initUsers(int *userCount,struct User *users);

void initTopics(int *topicCount,struct Topic *topics);

void initMsgs(int *msgCount,int *msgsIDs);


void updateTopicFile(struct Topic topic);

void createMsgFile(struct Message msg);

int scanMsg(struct Message *msg,int recipient_id,int indx);

void delMsg(int id);
