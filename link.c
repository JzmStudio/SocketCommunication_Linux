#include"j_socket.h"
extern void resend(struct Mes mes);
struct Wait
{
	struct Mes mes;
	int reSum;
	struct Wait *next;
};

struct Wait* newList()
{
	struct Wait* head;
	head=(struct Wait*)malloc(sizeof(struct Wait));
	head->next=NULL;
	return head;
}

void in(struct Wait* head,struct Mes mes)
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
	p->next=q;
}

void out(struct Wait* head,time_t t)
{
	struct Wait *p;
	struct Wait *q;
	time_t now;
	p=head;
	while(p->next!=NULL)
	{
		q=p->next;
		if(q->mes.time==t)
		{
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
			resend(q->mes);
			q->reSum++;
		}
		p=p->next;
	}
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
