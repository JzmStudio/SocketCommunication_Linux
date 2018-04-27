#include<stdio.h>
#include<stdlib.h>
int main()
{
	if(system("gcc server.c linkserver.c users.c sql.c userinfor.c -o server -lmysqlclient")>=0)
			printf("done\n");
}
