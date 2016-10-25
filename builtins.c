//File for shell built-ins

/*The handling for built-ins is maily based on the code at tutorial
  https://brennan.io/2015/01/16/write-a-shell-in-c/
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "builtins.h"
#include "parse.h"

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

//The order of function pointers should remain same as the order of commands in 'builtins' array
int (*builtin_funcs[]) (Cmd) = {
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

int is_builtin(char** args){
  int i;

  if(args[0] == NULL){
    //Blank command entered
    return 0;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtins[i]) == 0) {
      printf("It's a built-in \n");

      //(i+1) is returned because to avoid 0 being returned on match with 0th index.
      return (i+1);

    }
  }

  return 0;
}

int exec_builtin(Cmd c, int infile_fd, int outfile_fd, int index){
  if (infile_fd != STDIN_FILENO){
     dup2 (infile_fd, STDIN_FILENO);
     close (infile_fd);
  }
  if (outfile_fd != STDOUT_FILENO){
     dup2 (outfile_fd, STDOUT_FILENO);
     close (outfile_fd);
  }

  printf("Executing a built-in . Index = %d\n",index);
  //1 subtracted from index as index was passed after adding +1 in `is_builtin`
  return (*builtin_funcs[index - 1])(c);
}

int builtin_cd(Cmd cmd){
  char** args = cmd->args;
  if (args[1] == NULL) {
    fprintf(stderr, "ush: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("ush");
    }
  }
  return 1;
}

int builtin_echo(Cmd cmd){
  return 1;
}

int builtin_jobs(Cmd cmd){

  return 1;
}

int builtin_pwd(Cmd cmd){
  char curr_dir[256];
  if( getcwd(curr_dir,256) != 0)
      printf("%s\n",curr_dir);
  else
      printf("Error in getting current directory\n");
  return 1;
}

int builtin_setenv(Cmd cmd){
  return 1;
}

int builtin_unsetenv(Cmd cmd){
  return 1;
}

int builtin_where(Cmd cmd){
  return 1;
}

int builtin_logout(Cmd cmd){
  printf("Logout issued \n");
  exit(0);
  //return 1;
}
