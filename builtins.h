//Header file for shell built-ins

#ifndef BUILTINS_H
#define BUILTINS_H

#include "parse.h"


int builtin_cd(char **args);
int builtin_echo(char **args);
int builtin_jobs(char **args);
int builtin_pwd(char **args);
int builtin_setenv(char **args);
int builtin_unsetenv(char **args);
int builtin_where(char **args);
int builtin_logout(char **args);

int exec_builtin(Cmd c, int infile_fd, int outfile_fd, int index);

int is_builtin(char** args);
#endif
