#include "commands.h"
#include "../libc/screen.h"
#include "../kernel/IDT/keyboard.h"
#include "../libc/string.h"
#include "../libc/system.h"
#define NULL 0
#define FALSE 0
#define TRUE !FALSE

const char* getArg(const char* argv, int argc, int argNum)
{
	if (argNum >= argc)		// prevent buffer overflow
		return NULL;
	const char* res = argv;
	for (int i = 0; i < argNum; i++)		// run until wanted argument reached
	{
		res += strlen(res) + 1;		// next argument
	}
	return res;
}

void unknown_command(char* argv, int argc)
{
	puts("Command \'");
	puts(argv);
	puts("\' not found. Try \'help\'.");
}

void echo(char* argv, int argc)
{
	for (int i = 1; i < argc; i++)		// for every argument except command name
	{
		puts(getArg(argv, argc, i));		// print it
		putc(' ');		// add the space
	}
}

void color(char* argv, int argc)
{
	if (argc != 3)
		puts("Invalid syntax. Try \'help color\'.");
	else
	{
		char fore = (char)atoi(getArg(argv, argc, 1));		// get foreground color
		char back = (char)atoi(getArg(argv, argc, 2));		// get background color
		setScreenColor(fore, back);				// set screen colors
	}
}

void help(char* argv, int argc)
{
	if (argc == 1)		// no specific command
	{
		puts(
			"For specific command:\n"
			"help <command name>\n\n"
			"Available Commands:\n"
			"help - Print information about available commands.\n"
			"echo - Print message to the screen.\n"
			"color - Change the shell's colors.\n"
			"grep - \n"
			"shutdown - shutdown the computer.\n"
			"bc - basic calculator.\n"
		);
	}
	else if (argc == 2)		// for specific command
	{
		for (int i = 0; i < FUNS_NUM; i++)		// search functions
		{
			if (strncmp(getArg(argv, argc, 1), fun_names[i], strlen(fun_names[i])) == 0)		// if found
			{
				puts(getArg(fun_info, FUNS_NUM, i));		// print info
				return;
			}
		}
		puts("Unknown command. Try \'help\'.");
	}
	else
	{
		puts("Invalid syntax. Try \'help help\'.");
	}
}

void shutdownCommand(char* argv, int argc)
{
	puts("Are you sure? (y/n): ");
	bflush();
	char res = getchar();
	if (res == 'y')
		shutdown();
}

void bc(char* argv, int argc)
{
	// local functions
	int isValidExp(const char* exp, int size)			// check for only ints and +-*/
	{
		if (exp[0] == '*' || exp[0] == '/' || exp[0] == '+' || exp[0] == '-')
			return FALSE;
		if (exp[size - 1] == '*' || exp[size - 1] == '/' || exp[size - 1] == '+' || exp[size - 1] == '-')
			return FALSE;
		for (int i = 1; i < size - 1; i++)
			if (!((exp[i] >= 48 && exp[i] < 58) || exp[i] == '*' || exp[i] == '/' 
				|| exp[i] == '+' || exp[i] == '-'))
				return FALSE;
		return TRUE;
	}

	// main code
	if (argc != 2)
	{
		puts("Invalid syntax. Try \'help bc\'.");
		return;
	}

	const char* exp = getArg(argv, argc, 1);		// get math expression
	int size = strlen(exp);					// get its size
	puti(isValidExp(exp, size));
}