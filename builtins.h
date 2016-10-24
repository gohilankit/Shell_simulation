//Header file for shell built-ins

#ifndef BUILTINS_H
#define BUILTINS_H

int builtin_cd(char **args);
int builtin_echo(char **args);
int builtin_jobs(char **args);
int builtin_pwd(char **args);
int builtin_setenv(char **args);
int builtin_unsetenv(char **args);
int builtin_where(char **args);
int builtin_logout(char **args);

int exec_if_builtin(char** args);

#endif
