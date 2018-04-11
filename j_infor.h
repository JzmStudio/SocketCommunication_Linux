#ifndef J_INFOR_H
#define J_INFOR_H 1
#include"j_socket.h"

extern void sendMes(struct Mes mes,struct sockaddr_in ter);	//此函数需在使用者代码中提供实现

void initial(unsigned int id);	//从id+1开始都为未分配
unsigned int getlastId();
void updateId(unsigned int id);
void writeMes(unsigned int id,struct Mes mes);	//创建新用户并把最后id刷新
void sendMesbyId(unsigned int id,struct sockaddr_in ter);	//读一个id的缓存并发送,之后删除缓存

#endif
