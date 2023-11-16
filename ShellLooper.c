#include "shell.h"
#define DENIED_PERMISSION 126
#define ERROR_GE 127
#define MAX_COMMAND_LENGTH 100

/**
 * ShellLooper - Main shell loop responsible managing the shell execution flow.
 * @Shellinfo: the parameter & return Shellinfo struct
 * @argument: the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 */
int ShellLooper(info_t *Shellinfo, char **argument)
{
	ssize_t r = 0;
	int builtin_ret = 0;

	record_history(Shellinfo);

	while (r != -1 && builtin_ret != -2)
	{
		init_info(Shellinfo);
		if (CheckIntMode(Shellinfo))
			print_string("$ ");
		_PutcharE(BUF_FLUSH);
		r = Input_getter(Shellinfo);
		if (r != -1)
		{
			popul_info(Shellinfo, argument);
			builtin_ret = BuiltIn_Finder(Shellinfo);
			if (builtin_ret == -1)
				CMD_Finder(Shellinfo);
		} else if (CheckIntMode(Shellinfo))
			put_char('\n');
		release_info(Shellinfo, 0);
	}

	release_info(Shellinfo, 1);

	if (!CheckIntMode(Shellinfo) && Shellinfo->status)
		exit(Shellinfo->status);

	if (builtin_ret == -2)
	{
		if (Shellinfo->err_num == -1)
			exit(Shellinfo->status);
		exit(Shellinfo->err_num);
	}

	return (builtin_ret);
}
