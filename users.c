#include"j_socket.h"

//还未加超出1024时的索引控制
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

struct UserController users[1024];

struct User* searchUser(unsigned int idin)
{
	int b=idin>>22;
	struct User *p=users[b].user;
	while(p!=NULL)
	{
		if(p->id==idin)
			return p;
		p=p->next;
	}
	return NULL;
}

void addUser(unsigned int idin,struct sockaddr_in addrin)
{
	if(searchUser(idin)!=NULL)
	{
		printf("Id have been exist\n");
		return;
	}
	unsigned int b=idin>>22;
	struct User *u=malloc(sizeof(struct User));
	u->addr=addrin;
	u->id=idin;
	struct User *p=users[b].user;
	users[b].user=u;
	u->next=p;
}

void showLine(int index)
{
	printf("line:%d\n",index);
	if(index<0||index>1023)
	{
		printf("over index\n");
		return;
	}
	struct User *p=users[index].user;
	int i=1;
	while(p!=NULL)
	{
		printf("%d id:%u\n",i,p->id);
		i++;
		p=p->next;
	}
}

void removeUser(unsigned int idin)
{
	unsigned int b=idin>>22;
	struct User *u;
	struct User *p=users[b].user;
	struct User *q;
	if(p==NULL)
		return;
	if(p->id==idin)
	{
		q=p->next;
		users[b].user=q;
		free(p);
		return;
	}
	u=p->next;
	while(u!=NULL)
	{
		if(u->id==idin)
		{
			q=u->next;
			p->next=q;
			free(u);
			return;
		}
		p=u;
		u=u->next;
	}
	printf("no find\n");
}
