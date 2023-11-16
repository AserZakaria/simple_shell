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

/**
 * BuiltIn_Finder -  Searches for and handles built-in commands in the shell.
 * @Shellinfo: the parameter & return Shellinfo struct
 *
 * Return: -1 if builtin not found,
 *			0 if builtin executed successfully,
 *			1 if builtin found but not successful,
 *			-2 if builtin signals exit()
 */
int BuiltIn_Finder(info_t *Shellinfo) {
    builtin_table builtintbl[] = {
        {"exit", _myexit},
        {"env", mine_env},
        {"help", Dir_changer},
        {"history", list_history},
        {"setenv", mine_New_env},
        {"unsetenv", mine_Remove_env},
        {"cd", _Cd},
        {"alias", Alias_mimic},
        {NULL, NULL}
    };

    int i;
    for (i = 0; builtintbl[i].type; i++) {
        if (Str_Comparison(Shellinfo->arg_list[0], builtintbl[i].type) == 0) {
            Shellinfo->line_count++;
            return builtintbl[i].func(Shellinfo);
        }
    }
    return -1;
}
/**
 * CMD_Finder - Locates command within the system's PATH variable to execute.
 * @Shellinfo: the parameter & return Shellinfo struct
 *
 * Return: void
 */
void CMD_Finder(info_t *Shellinfo)
{
	char *path = NULL;
	int index = 0, counter = 0;
	
	Shellinfo->path = Shellinfo->arg_list[0];
	if (Shellinfo->linecount_flag == 1)
	{
		Shellinfo->line_count++;
		Shellinfo->linecount_flag = 0;
    }
	
	for (index = 0; Shellinfo->argument[index]; index++)
	{
		if (!IS_delimeter(Shellinfo->argument[index], " \t\n"))
		{
			counter++;
        }
    }
    if (!counter)
	{
        return;
    }
	
	path = CMD_locator(Shellinfo, env_get(Shellinfo, "PATH="),
	Shellinfo->arg_list[0]);
	if (path)
	{
		Shellinfo->path = path;
		ForkExecute_Cmd(Shellinfo);
		} else
		{
			if ((CheckIntMode(Shellinfo) || env_get(Shellinfo, "PATH=") ||
			Shellinfo->arg_list[0][0] == '/') &&
			is_cmd_Exec(Shellinfo, Shellinfo->arg_list[0]))
			{
				ForkExecute_Cmd(Shellinfo);
			 } else if (*(Shellinfo->argument) != '\n')
				{
					Shellinfo->status = 127;
					Err_print(Shellinfo, "not found\n");
				}
		}
}

/**
 * ForkExecute_Cmd - Forks a child process and executes a command
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void ForkExecute_Cmd(info_t *info)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{
		perror("Error:");
		return;
	}
	if (child_pid == 0)
	{
		if (execve(info->path, info->arg_list, copy_str_env(info)) == -1)
		{
			release_info(info, 1);
			if (errno == EACCES)
			{
				fprintf(stderr, "Err: Denied Permission '%s'\n", info->arg_list[0]);
				exit(DENIED_PERMISSION);
			} else
			{
				fprintf(stderr, "Err: Failed to exec cmd '%s'\n", info->arg_list[0]);
				exit(ERROR_GE);
			}
		}
	} else
	{
		wait(&(info->status));
		if (WIFEXITED(info->status))
		{
			info->status = WEXITSTATUS(info->status);
			if (info->status == 126)
				Err_print(info, "Denied Permission\n");
		}
	}
}
