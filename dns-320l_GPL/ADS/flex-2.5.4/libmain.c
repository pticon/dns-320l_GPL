/* libmain - flex run-time support library "main" function */

/* $Header: /cvshome/Gandolf5/module/ads/packages/flex-2.5.4/libmain.c,v 1.1.1.1 2009/04/20 07:15:35 jack Exp $ */

extern int yylex();

int main( argc, argv )
int argc;
char *argv[];
	{
	while ( yylex() != 0 )
		;

	return 0;
	}
