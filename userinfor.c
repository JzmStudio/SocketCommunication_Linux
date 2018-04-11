#include"j_socket.h"
extern void sendMes(struct Mes mes,struct sockaddr_in ter);	//此函数需在使用者代码中提供实现

FILE *fp=NULL;	//只用于持有根
const char *root="./infor/no.infor";

void initial(unsigned int id)	//从id+1开始都为未分配
{
	system("mkdir infor");
	fp=fopen(root,"r");
	unsigned int l=-1;
	if(fp!=NULL)
	{
		printf("Have existed!");
		if(fscanf(fp,"%u",&l)<=0)
		{
			perror("fscanf");
		}
		printf("%u\n",l);
		fclose(fp);
		return;
	}
	fp=fopen(root,"wb+");
	if(fp==NULL)
	{
		perror("fopen");
		return;
	}
	fprintf(fp,"%u",id);
	fclose(fp);
	fp=NULL;
}

unsigned int getlastId()
{
	fp=fopen(root,"rb");
	if(fp==NULL)
	{
		perror("fopen");
		return -1;
	}
	unsigned int no;
	if(fscanf(fp,"%u",&no)<=0)
	{
		perror("fscanf");
	}
	fclose(fp);
	fp=NULL;
	return no;
}

void updateId(unsigned int id)
{
	fp=fopen(root,"wb");
	if(fp==NULL)
	{
		perror("fopen");
		return;
	}
	fprintf(fp,"%u",id);
	fclose(fp);
}

void writeMes(unsigned int id,struct Mes mes)	//创建新用户并把最后id刷新
{
	char name[20];
	sprintf(name,"./infor/%u.infor",id);
	printf("path is %s\n",name);
	FILE *p=fopen(name,"ab");
	if(p==NULL)
	{
		perror("fopen");
		return;
	}
	fwrite(&mes,sizeof(struct Mes),1,p);
	fclose(p);
	fp=fopen(root,"wb");
	if(fp==NULL)
	{
		perror("fopen");
		return;
	}
	fprintf(fp,"%u",id);
	fclose(fp);
}

void sendMesbyId(unsigned int id,struct sockaddr_in ter)	//读一个id的缓存并发送,之后删除缓存
{
	char path[30];
	sprintf(path,"./infor/%u.infor",id);
	FILE *p=fopen(path,"rb");
	if(p==NULL)
	{
		printf("No mes left\n");
		return;
	}
	struct Mes mes;
	while(fread(&mes,sizeof(struct Mes),1,p)>0)
	{
		sendMes(mes,ter);
	}
	fclose(p);
	char com[36];
	sprintf(com,"rm ./infor/%u.infor",id);
	system(com);
}
