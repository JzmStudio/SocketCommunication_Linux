#ifndef _JZM_SQL
#define _JZM_SQL 1

#include<stdio.h>
#include<mysql/mysql.h>

extern void connectSQL();
extern void closeSQL();
extern char* login(unsigned int id,char *password);
extern int newUser(unsigned int id,char *password,char *name);	//success is 0 otherwise -1
extern char* findname(unsigned int id);	//根据id找用户名

#endif
