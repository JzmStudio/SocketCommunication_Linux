#include"j_socket.h"
//此链表用于维护通信双方都在线的情况
extern void resend(struct Mes mes,struct sockaddr_in client);
struct Wait
{
	struct Mes mes;
	int reSum;
	struct sockaddr_in client;
	struct Wait *next;
};

struct Wait* newList()
{
	struct Wait* head;
	head=(struct Wait*)malloc(sizeof(struct Wait));
	head->next=NULL;
	return head;
}

void in(struct Wait* head,struct Mes mes,struct sockaddr_in c)
{
	struct Wait *p;
	struct Wait *q;
	p=head;
	while(p->next!=NULL)
	{
		p=p->next;
	}
	q=(struct Wait*)malloc(sizeof(struct Wait));
	q->mes=mes;
	q->next=NULL;
	q->reSum=0;
	q->client=c;
	p->next=q;
}

void out(struct Wait* head,time_t t)
{
	char f='n';
	struct Wait *p;
	struct Wait *q;
	time_t now;
	p=head;
	while(p->next!=NULL)
	{
		q=p->next;
		if(q->mes.time==t)
		{
			f='y';
			printf("time:%s is arrived\n",ctime(&t));
			p->next=q->next;
			free(q);
			break;
		}
		else if(q->reSum>2)
		{
			p->next=q->next;
			printf("time:%s is lost",ctime(&(q->mes.time)));
			free(q);
			p=p->next;
			continue;
		}
		else if(difftime(time(&now),q->mes.time)>1.0)
		{
			resend(q->mes,q->client);
			q->reSum++;
		}
		p=p->next;
	}
	if(f=='n')
		printf("no find:%s\n",ctime(&t));
}

void showList(struct Wait *head)
{
	struct Wait *p=head;
	p=p->next;
	while(p!=NULL)
	{
		printf("Time is:%sMes is:%s\n",ctime(&(p->mes.time)),p->mes.mes);
		p=p->next;
	}
}

void destroyList(struct Wait *head)
{
	struct Wait *p=head;
	struct Wait *q;
	while(p!=NULL)
	{
		q=p->next;
		free(p);
		p=q;
	}
	head=NULL;
}
