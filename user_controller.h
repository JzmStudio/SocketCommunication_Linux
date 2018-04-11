#ifndef _U_CONTROL_H
#define _U_CONTROL_H 1

#include"j_socket.h"

struct User
{
	unsigned int id;
	char name[16];
	struct sockaddr_in addr;
	struct User *next;
};

struct UserController
{
	struct User *user;	//此队user的第一个地址
};

extern void addUser(unsigned int id,struct sockaddr_in addr);
extern struct User* searchUser(unsigned int id);	//找到返回地址，否则NULL
extern void showLine(int index);	//index from 0 to 1023
extern void removeUser(unsigned int id);

#endif
