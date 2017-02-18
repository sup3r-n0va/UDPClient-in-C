#include <stdio.h>	// for perror()
#include <stdlib.h>	//for exit()

void DieWithError(char *errormessage) 
{
	perror(errormessage);
	exit(1);
}


