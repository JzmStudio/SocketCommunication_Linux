#ifndef _J_SOCKET_H
#define _J_SOCKET_H 1

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<errno.h>
#include<unistd.h>

#define FMESLEN 4096
#define MESLEN 2048

struct Mes
{
	char flag;	//Mes flag is 0
	char mes[MESLEN];
	unsigned int idto;
	unsigned int idfrom;
	char name[16];	//发送者昵称
	time_t time;
};

struct MAck
{
	char flag;	//MAck flag is A
	time_t time;
};

struct LogInfor	//登录信息/登出信息	查找用户
{
	char flag;	//Log in flag is L,log out is O(大写),根据id请求用户名F,反馈用户名R
	unsigned int id;
	char password[16];
};

struct NewUser
{
	char flag;	//New flag is N
	char password[16];
	char name[16];
};

struct LogAccess	//登录成功或新用户创建成功
{
	char flag;	//log success is B
	char name[16];
	unsigned int id;
};

struct ErrorInfor
{
	char flag;	//E
	char infor;	//登录用户名/密码错误 a，创建用户时服务器出错b
};

struct FMes
{
	char mes[FMESLEN];
};

struct FMesInfor
{
	unsigned int len;
};

#endif
