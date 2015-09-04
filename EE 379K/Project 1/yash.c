#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *arg[])
{
	printf("$");
	int rc = fork();
	
}