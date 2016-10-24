//File for shell built-ins

/*The handling for built-ins is maily based on the code at tutorial
  https://brennan.io/2015/01/16/write-a-shell-in-c/
*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "builtins.h"

char *builtins[] = {
  "cd",
  "echo",
  "jobs",
  "pwd",
  "setenv",
  "unsetenv",
  "where",
  "logout",
};

int (*builtin_funcs[]) (char **) = {
  &builtin_cd,
  &builtin_echo,
  &builtin_jobs,
  &builtin_pwd,
  &builtin_setenv,
  &builtin_unsetenv,
  &builtin_where,
  &builtin_logout,
};

int num_builtins() {
  return sizeof(builtins) / sizeof(char *);
}

int exec_if_builtin(char** args){
  int i;

  if(args[0] == NULL){
    //Blank command entered
    return 1;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtins[i]) == 0) {
      printf("Executing a built-in \n");
      return (*builtin_funcs[i])(args);
    }
  }
}

int builtin_cd(char **args){
  if (args[1] == NULL) {
    fprintf(stderr, "ush: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("ush");
    }
  }
  return 1;
}

int builtin_echo(char **args){
  return 1;
}

int builtin_jobs(char **args){
  return 1;
}

int builtin_pwd(char **args){
  return 1;
}

int builtin_setenv(char **args){
  return 1;
}

int builtin_unsetenv(char **args){
  return 1;
}

int builtin_where(char **args){
  return 1;
}

int builtin_logout(char **args){
  return 1;
}
