#include"j_socket.h"
#include"jzm_sql.h"
#include"linkserver.h"
#include"user_controller.h"
#include"j_infor.h"

#define INIT_ID 1024	//设置初始的分配id(从id+1开始分配后面的id给用户)

void resend(struct Mes mes,struct sockaddr_in client);
void printClient();
void recvMessage();
void sendMes(struct Mes mes,struct sockaddr_in ter);

int sock;
struct sockaddr_in client;	//接收的地址
struct User *toclient;	//转发的地址指针
struct Mes sdmes;
struct MAck ack;
struct MAck sdack;
struct Mes rcmes;
struct Wait *head;
struct LogInfor infor;
struct ErrorInfor error_infor;
struct LogAccess logaccess;
struct NewUser newuser;
int addrLen;
int mesLen;
int ackLen;
int loginLen;
int errorLen;
unsigned int idlast;	//最后一个被分配的id（id顺序分配）
char *sql_infor;
pthread_t thread;

int main()
{
	addrLen=sizeof(struct sockaddr);
	mesLen=sizeof(struct Mes);
	ackLen=sizeof(struct MAck);
	loginLen=sizeof(struct LogInfor);
	errorLen=sizeof(struct ErrorInfor);
	sock=socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in server;
	server.sin_family=AF_INET;
	server.sin_port=htons(9000);
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	head=newList();
	sql_infor=NULL;
	error_infor.flag='E';
	sdmes.flag=0;
	logaccess.flag='B';
	newuser.flag='N';

	//id相关 之后id相关操作可能移动到另一进程里
	initial(1024);
	idlast=getlastId();

	if(bind(sock,(struct sockaddr*)&server,sizeof(server))<0)
	{
		perror("bind:");
		exit(0);
	}
	
	sdack.flag='A';
	time_t recTime;
	int c=0;
	
	//链接数据库
	connectSQL();

	/*if(pthread_create(&thread,NULL,(void*)recvMessage,NULL)!=0)
	{
		perror("thread:");
		exit(0);
	}*/

	while(c<10)
	{
		c++;
		recvMessage();
		/*if(searchUser(497545029)!=NULL)
		{
			printf("497545029 is online\n");
		}*/
	}

	destroyList(head);
	closeSQL();
	close(sock);

	return 0;
}



void printClient()
{
	printf("addr is:%s .port is:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
}

void recvMessage()
{
	char c;
	if(recvfrom(sock,&c,1,MSG_PEEK,(struct sockaddr *)&client,(socklen_t *)&addrLen)<0)
	{
		perror("recv:");
	}
	printf("recv %c\n",c);
	switch(c)
	{
		case 0:
			recvfrom(sock,&rcmes,mesLen,0,(struct sockaddr *)&client,(socklen_t *)&addrLen);
			printClient();
			printf("Time:%sMessage:%s\n",ctime(&rcmes.time),rcmes.mes);
			//send ack
			sdack.time=rcmes.time;
			sdack.flag='A';
			sendto(sock,&sdack,ackLen,0,(struct sockaddr*)&client,addrLen);
			printf("send ack\n");
			//resend to other client
			toclient=searchUser(rcmes.idto);
			if(toclient==NULL)
			{
				//若不在线
				printf("he is not online\n");
				writeMes(rcmes.idto,rcmes);
			}
			else
			{
				//resend to other
				sdmes.flag=0;
				printf("from:%s\n",rcmes.name);
				strcpy(sdmes.name,rcmes.name);
				strcpy(sdmes.mes,rcmes.mes);
				time(&sdmes.time);
				sendto(sock,&sdmes,mesLen,0,(struct sockaddr*)&toclient->addr,addrLen);
				in(head,sdmes,toclient->addr);
			}
			break;
		case 'A':
			recvfrom(sock,&ack,ackLen,0,(struct sockaddr *)&client,(socklen_t *)&addrLen);
			printf("Ack time:%s\n",ctime(&ack.time));
			out(head,ack.time);
			break;
		case 'L':	//接收到登录信息
			recvfrom(sock,&infor,loginLen,0,(struct sockaddr *)&client,(socklen_t *)&addrLen);
			printf("id:%u,password:%s\n",infor.id,infor.password);
			sql_infor=login(infor.id,infor.password);
			if(sql_infor==NULL)
			{
				error_infor.infor='a';
				sendto(sock,&error_infor,errorLen,0,(struct sockaddr*)&client,addrLen);
			}	
			else
			{
				sprintf(logaccess.name,"%s",sql_infor);
				logaccess.id=infor.id;
				sendto(sock,&logaccess,sizeof(struct LogAccess),0,(struct sockaddr*)&client,addrLen);
				addUser(infor.id,client);
			}
			//检查有没有历史信息在缓存中 并发送
			sendMesbyId(infor.id,client);
			break;
		case 'N':	//创建新用户信息
			recvfrom(sock,&newuser,sizeof(struct NewUser),0,(struct sockaddr *)&client,(socklen_t *)&addrLen);
			printf("New user name:%s password:%s\n",newuser.name,newuser.password);
			//分配id
			idlast++;
			//更新id——last
			updateId(idlast);
			logaccess.id=idlast;
			strcpy(logaccess.name,newuser.name);
			//更新数据库,若无法创建，则需要修理SQL，屏蔽创建新用户操作
			if(newUser(idlast,newuser.password,newuser.name)<0)
			{
				printf("SQL need repair!\n");
				error_infor.infor='b';
				sendto(sock,&error_infor,errorLen,0,(struct sockaddr*)&client,addrLen);
				break;
			}
			sendto(sock,&logaccess,sizeof(struct LogAccess),0,(struct sockaddr*)&client,addrLen);
			//更新在线用户列表
			addUser(idlast,client);
			break;
		case 'O':	//Log out
			recvfrom(sock,&infor,loginLen,0,(struct sockaddr *)&client,(socklen_t *)&addrLen);
			removeUser(infor.id);
			printf("%u log out\n",infor.id);
			break;
		default:
			while(recvfrom(sock,&rcmes,mesLen,MSG_DONTWAIT,(struct sockaddr *)&client,(socklen_t *)&addrLen)>0);
			printf("socket flushed\n");
			break;
	}
}

void resend(struct Mes mes,struct sockaddr_in client)
{
	printf("resend\n");
	int size=sendto(sock,&mes,mesLen,0,(struct sockaddr*)&client,addrLen);
	if(size<0)
	{
		perror("send:");
	}
}

void sendMes(struct Mes mes,struct sockaddr_in ter)
{
	printf("Mes is %s",mes.mes);
	sendto(sock,&mes,mesLen,0,(struct sockaddr*)&ter,addrLen);
}
