#include"j_socket.h"
#include"link.h"
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>

void resend(struct Mes mes);
void sendMessage();
void recvMessage();
void login();
void sig_alarm();
void help();

int sock;
struct sockaddr_in target;
struct Mes sdmes;
struct MAck ack;
struct MAck sdack;
struct Mes rcmes;
struct Wait *head;
struct ErrorInfor error;
struct LogInfor infor;
struct LogAccess logaccess;
struct NewUser newuser;
int addrLen;
int mesLen;
int ackLen;
pthread_t thread;
char is_recv;	//n is not log in,y is contrary
sem_t sem;	//登陆时的信号量
unsigned int id;	//聊天对方id
unsigned int idlocal;	//本机id
char namelocal[16];	//本机昵称
char isnew;	//是否是新用户标志

int main()
{
	//全局变量初始化
	addrLen=sizeof(struct sockaddr);
	mesLen=sizeof(struct Mes);
	ackLen=sizeof(struct MAck);
	sock=socket(AF_INET,SOCK_DGRAM,0);
	target.sin_family=AF_INET;
	target.sin_port=htons(9000);
	inet_aton("127.0.0.1",&target.sin_addr);
	head=newList();
	is_recv='n';
	sdack.flag='A';
	newuser.flag='N';
	isnew='n';

	//登录信号量
	if(sem_init(&sem,0,0)<0)
	{
		perror("sem:");
		exit(0);
	}
	
	//以下到bind仅作测试
	/*struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(10000);
	inet_aton("127.0.0.1",&local.sin_addr);
	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		exit(0);
	}*/

	login();

	if(pthread_create(&thread,NULL,(void*)recvMessage,NULL)!=0)
	{
		perror("thread:");
		exit(0);
	}

	//创建登录超时检测
	signal(SIGALRM,sig_alarm);
	
	//登录成功才进行后续
	sem_wait(&sem);
	sem_destroy(&sem);

	help();
	char command[16];
	int i;
	for(i=0;i<16;i++)
	{
		scanf("%c",&command[i]);
		if(command[i]==' ')
		{
			command[i]=0;
			break;
		}
		if(command[i]=='\n')
		{
				command[i]=0;
				break;
		}
	}
	//判断命令
	while(strcmp(command,"quit")!=0)
	{
		if(strcmp(command,"send")==0)
		{
			fgets(sdmes.mes,MESLEN,stdin);
			if(sdmes.mes[0]=='\0')
			{
				printf("message cannot be null\n");
				//scanf("%s",command);
				//continue;
			}
			else
			{
				time(&(sdmes.time));
				sendMessage();
			}
		}
		else if(strcmp(command,"chatwith")==0)
		{
			scanf("%u",&id);
			getchar();
			if(id==0)
			{
				printf("id wrong\n");
			}
			else
			{
				sdmes.idto=id;
			}
		}
		else
		{
			printf("please input exact command!\n");
		}

		for(i=0;i<16;i++)	//注意之前的换行符有没删
		{
			scanf("%c",&command[i]);
			if(command[i]==' ')
			{
				command[i]=0;
				break;
			}
			if(command[i]=='\n')
			{
				command[i]=0;
				break;
			}
		}
		printf("command:%s\n",command);
	}

	//登出
	infor.flag='O';
	infor.id=idlocal;
	if(sendto(sock,&infor,sizeof(struct LogInfor),0,(struct sockaddr*)&target,addrLen)<0)
	{
		perror("sendto");
	}

	close(sock);
	destroyList(head);

	return 0;
}


void sendMessage()
{
	int size=sendto(sock,&sdmes,mesLen,0,(struct sockaddr*)&target,addrLen);
	if(size<0)
	{
		perror("send:");
		return;
	}
	in(head,sdmes);
}

void recvMessage()
{
	char c;
	while(1)
	{
		if(recvfrom(sock,&c,1,MSG_PEEK,(struct sockaddr *)&target,(socklen_t *)&addrLen)<0)
		{
			perror("recv:");
		}
		printf("recv %c\n",c);
		if(c==0)
		{
			recvfrom(sock,&rcmes,mesLen,0,(struct sockaddr *)&target,(socklen_t *)&addrLen);
			printf("Time:%s",ctime(&rcmes.time));
			printf("From:%s Message:%s\n",rcmes.name,rcmes.mes);
			sdack.time=rcmes.time;
			sendto(sock,&sdack,ackLen,0,(struct sockaddr*)&target,addrLen);
		}
		else if(c=='A')
		{
			recvfrom(sock,&ack,ackLen,0,(struct sockaddr *)&target,(socklen_t *)&addrLen);
			out(head,ack.time);
		}
		else if(c=='B')	//登录成功
		{
			if(is_recv=='y')
			{
				//若已登录过
				recvfrom(sock,&logaccess,mesLen,0,(struct sockaddr *)&target,(socklen_t *)&addrLen);
				//sdack.time=rcmes.time;	//注意ack前应修改time
				//sendto(sock,&sdack,ackLen,0,(struct sockaddr*)&target,addrLen);
			}
			else
			{
				is_recv='y';
				recvfrom(sock,&logaccess,mesLen,0,(struct sockaddr *)&target,(socklen_t *)&addrLen);
				printf("Log in! Hello:%s\n!",logaccess.name);
				if(isnew=='y')
				{
					printf("Please remember your id:%u\n",logaccess.id);
					idlocal=logaccess.id;
					isnew='n';
				}
				strcpy(namelocal,logaccess.name);
				strcpy(sdmes.name,namelocal);	//在发送信息中添加自己的昵称
				sdmes.idfrom=idlocal;
				printf("mes name:%s id:%u",sdmes.name,sdmes.idfrom);
				sem_post(&sem);	//发出信号量使主线程继续进行
				sdmes.idfrom=idlocal;	//在发送信息中添加自己的id
			}
		}
		else if(c=='E')
		{
			is_recv='y';
			recvfrom(sock,&error,sizeof(struct ErrorInfor),0,(struct sockaddr *)&target,(socklen_t *)&addrLen);
			if(error.infor=='a')	//登录失败
			{
				printf("id or password error\n");
				login();
			}
			else if(error.infor=='b')
			{
				printf("server error!can't create new user\n");
			}
		}
		else
		{
			while(recvfrom(sock,&rcmes,mesLen,MSG_DONTWAIT,(struct sockaddr *)&target,(socklen_t *)&addrLen)>0);
			printf("socket flushed\n");
		}
	}
}

void resend(struct Mes mes)
{
	int size=sendto(sock,&mes,mesLen,0,(struct sockaddr*)&target,addrLen);
	if(size<0)
	{
		perror("send:");
	}
}

void login()
{
	int i;
	alarm(0);
	is_recv='n';
	printf("Please choose to log in or create new User(input q to quit)\n");
	printf("		input l to log in or c to create:");
	char c;
	scanf("%c",&c);
	getchar();
	while(1)
	{
		if(c=='l')
		{
			isnew='n';
			infor.flag='L';
			printf("Id:");
			scanf("%u",&infor.id);
			idlocal=infor.id;
			getchar();
			printf("Password:");
			fgets(infor.password,16,stdin);
			for(i=15;i>=0;i--)
			{
				if(infor.password[i]=='\n')
				{
					infor.password[i]=0;
					break;
				}
			}
			int size=sendto(sock,&infor,sizeof(struct LogInfor),0,(struct sockaddr*)&target,addrLen);
			if(size<0)
			{
				perror("send:");
				continue;
			}
			break;
		}
		else if(c=='c')
		{
			isnew='y';
			printf("Input your name(max is 16 words):");
			fgets(newuser.name,16,stdin);
			for(i=15;i>=0;i--)
			{
				if(newuser.name[i]=='\n')
				{
					newuser.name[i]=0;
					break;
				}
			}
			printf("Input your password(max is 16 words):");	//此时如果多于16字节则截断
			fgets(newuser.password,16,stdin);
			for(i=15;i>=0;i--)
			{
				if(newuser.password[i]=='\n')
				{
					newuser.password[i]=0;
					break;
				}
			}
			int size=sendto(sock,&newuser,sizeof(struct NewUser),0,(struct sockaddr*)&target,addrLen);
			if(size<0)
			{
				perror("send:");
				continue;
			}
			break;
		}
		else if(c=='q')
		{
			exit(0);
		}
		else
		{
			printf("please input correct command!\n");
		}
		scanf("%c",&c);
		getchar();	//去回车
	}
	alarm(3);
}

void sig_alarm()
{
	if(is_recv=='n')
	{
		printf("log in over time\n");
		login();
	}
}

void help()
{
	printf("You can input send or quit or chatwith+id\n");
}
