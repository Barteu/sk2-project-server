//init.h
#include "structs.h"


void initUsers(int *userCount,struct User *users);

void initTopics(int *topicCount,struct Topic *topics);

void updateTopicFile(struct Topic topic);

void createMsgFile(struct Message msg);
