#ifndef _LINK_SERVER_H
#define _LINK_SERVER_H 1

#include"j_socket.h"
extern struct Wait* newList();
extern void in(struct Wait *head,struct Mes mes,struct sockaddr_in c);
extern void out(struct Wait *head,time_t t);
extern void showList(struct Wait *head);
extern void destroyList(struct Wait *head);

struct Wait
{
	struct Mes mes;
	int reSum;
	struct Wait *next;
};

#endif
