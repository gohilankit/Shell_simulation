//Header file for shell built-ins

#ifndef BUILTINS_H
#define BUILTINS_H

#include "parse.h"


int builtin_cd(Cmd cmd);
int builtin_echo(Cmd cmd);
int builtin_jobs(Cmd cmd);
int builtin_pwd(Cmd cmd);
int builtin_setenv(Cmd cmd);
int builtin_unsetenv(Cmd cmd);
int builtin_where(Cmd cmd);
int builtin_logout(Cmd cmd);

int exec_builtin(Cmd c, int infile_fd, int outfile_fd, int index);

int is_builtin(char** args);
#endif
