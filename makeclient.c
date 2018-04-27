#include<stdio.h>
#include<stdlib.h>
int main()
{
	if(system("gcc client.c link.c -o client -lpthread")>=0)
		printf("done\n");
}
