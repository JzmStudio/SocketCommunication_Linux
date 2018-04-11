#include<stdio.h>
#include<mysql/mysql.h>
#include<string.h>

MYSQL mysql;
MYSQL_RES *res=NULL;
MYSQL_ROW row;
char isconnected='n';

void connectSQL()
{
	if(mysql_init(&mysql)==NULL)
	{
		printf("mysql_init(): %s\n", mysql_error(&mysql));  
		return;
	}
	if(mysql_real_connect(&mysql,"localhost","root","jiang","mm",0,NULL,0)==NULL)
	{
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));  
		return;
	}
	isconnected='y';
}

void closeSQL()
{
	isconnected='n';
	mysql_close(&mysql);
}

char* findname(unsigned int id)
{
	int i,rows=0,fields=0;
	if(isconnected!='y')
	{
		printf("please connect first\n");
		return NULL;
	}
	char l[64];
	sprintf(l,"select name from mm.users where id=%u",id);	//
	if(mysql_real_query(&mysql,l,strlen(l))!=0)
	{
		printf("mysql_real_query(): %s\n", mysql_error(&mysql)); 
		return NULL;
	}
	res=mysql_store_result(&mysql);
	if(res==NULL)
	{
		printf("mysql_restore_result(): %s\n",mysql_error(&mysql));
		return NULL;
	}
	row=mysql_fetch_row(res);
	return row[0];
}

char* login(unsigned int id,char *password)
{
	int i,rows=0,fields=0;
	if(isconnected!='y')
	{
		printf("please connect first!\n");
		return NULL;
	}
	char l[128];
	char *r;
	sprintf(l,"call login(%d,'%s',@name,@bool)",id,password);
	if(mysql_real_query(&mysql,l,strlen(l))!=0)
	{
		printf("mysql_real_query(): %s\n", mysql_error(&mysql));  
		return NULL;
	}
	r="select @bool";
	if(mysql_real_query(&mysql,r,strlen(r))!=0)
	{
		printf("mysql_real_query(): %s\n", mysql_error(&mysql));  
		return NULL;
	}
	res=mysql_store_result(&mysql);
	if(res==NULL)
	{
		printf("mysql_restore_result(): %s\n",mysql_error(&mysql));
		return NULL;
	}
	row=mysql_fetch_row(res);
	if(strcmp(row[0],"n")==0)
	{
		printf("no match\n");
		return NULL;
	}
	else
	{
		r="select @name";
		if(mysql_real_query(&mysql,r,strlen(r))!=0)
		{
			printf("mysql_real_query(): %s\n",mysql_error(&mysql));
		}
		res=mysql_store_result(&mysql);
		if(res==NULL)
		{
			printf("mysql_restore_result(): %s\n",mysql_error(&mysql));
			return NULL;
		}
		row=mysql_fetch_row(res);
		return row[0];
	}
		
	return NULL;
}

int newUser(unsigned int id,char *password,char *name)	//-1 error otherwise 0
{
	if(isconnected!='y')
	{
		printf("please connect first!\n");
	}
	char r[128];
	if(name==NULL)
	{
		sprintf(r,"insert into users values(%d,'%s','%d')",id,password,id);
	}
	else
	{
		sprintf(r,"insert into users values(%d,'%s','%s')",id,password,name);
	}
	if(mysql_real_query(&mysql,r,strlen(r))!=0)
	{
		printf("mysql_real_query(): %s\n", mysql_error(&mysql));  
		return -1;
	}
	return 0;
}
