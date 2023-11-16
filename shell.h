#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

/* ShellLooper.c */
int ShellLooper(info_t *, char **);
int BuiltIn_Finder(info_t *);
void CMD_Finder(info_t *);
void ForkExecute_Cmd(info_t *);
