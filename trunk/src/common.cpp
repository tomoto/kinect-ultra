#include "common.h"

static void (*s_errorExitFunc)();

void errorExit()
{
	printf("Error occured. Press any key to close this window.\n");
	if (s_errorExitFunc) {
		s_errorExitFunc();
	}
	getchar();
	exit(1);
}

void registerErrorExitFunc(void(*f)())
{
	s_errorExitFunc = f;
}
