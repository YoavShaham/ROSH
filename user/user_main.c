#include "user_main.h"
#define INPUT_SIZE 41

void callCommand(char* argv, int argc);

void umain()
{
	init_commands();
	initUserHeap();		// init the usermode heap

	char input[INPUT_SIZE] = {0};
	while (1)
	{
		uputs("\n\n");
		pwd(0, 0);
		uputs(" $ ");
		ubflush();		// clear buffer
		ugetline(input, INPUT_SIZE);		// get input
		cmd(input);				// get output
	}
}

void cmd(char* input)
{
	int count = 1;		// including command name
	int size = strlen(input);
	for (int i = 0; i < size; i++)		// replace space with 0
	{
		if (input[i] == ' ')
		{
			input[i] = 0;
			count++;
		}
	}
	callCommand(input, count);		// call function
}

/*
	call correct command from list
	@param argv: argument values
	@param argc: number of arguments
*/
void callCommand(char* argv, int argc)
{
	for (int i = 0; i < FUNS_NUM; i++)		// search functions
	{
		if (strncmp(argv, fun_names[i], strlen(fun_names[i])) == 0)		// if found
		{
			commands[i](argv, argc);		// call it
			return;
		}
	}
	unknown_command(argv, argc);		// print undefined command message
}