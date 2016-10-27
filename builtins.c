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

extern char **environ;

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

int is_builtin(char* arg){
  int i;

  if(arg == NULL){
    //Blank command entered
    return 0;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(arg, builtins[i]) == 0) {

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

  //1 subtracted from index as index was passed after adding +1 in `is_builtin`
  return (*builtin_funcs[index - 1])(c);
}

int builtin_cd(Cmd cmd){
  char** args = cmd->args;
  char* dir_name;
  
  if (args[1] == NULL) {
    dir_name = getenv("HOME");
  } else {
    dir_name = args[1];
  }

  if (chdir(dir_name) != 0) {
    perror("ush");
  }
  return 1;
}

int builtin_echo(Cmd cmd){
  //Path($) expansion is not handled
  char** args = cmd->args;

  int i;
  for (i=1; i < cmd->nargs; i++){
    printf("%s ",args[i]);
  }
  printf("\n");

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
      printf("Error in pwd\n");
  return 1;
}

int builtin_setenv(Cmd cmd){
  char** args = cmd->args;
  if (args[1] == NULL){ //setenv called with no args
    //Print environment variables
    int i;
    for(i=0; environ[i]; i++){
      printf("%s\n", environ[i]);
    }
  } else if(args[2] == NULL){ //setenv called with 1 arg, set it to NULL
    return setenv(args[1], (char *)"", 1);
  }else{                      // setenv called with both args
    return setenv(args[1],args[2],1);
  }
    return 1;
}

int builtin_unsetenv(Cmd cmd){
  return unsetenv(cmd->args[1]);
}

int builtin_where(Cmd cmd){
  char** args = cmd->args;
  int i;
  for (i=1; i < cmd->nargs; i++){
    if(is_builtin(args[i])){
      printf("%s is a shell built-in command\n",args[i]);
    }

    char cmd_path[1024], curr_path[256];
    char* path = getenv("PATH");
    strcpy(cmd_path, path);

    char* token = strtok(cmd_path, ":");

    while(token != NULL){
      strcpy(curr_path,token);
      strcat(curr_path, "/");
      strcat(curr_path, args[i]);

      if(access(curr_path, F_OK) == 0)
        printf("%s\n",curr_path);
      token = strtok(NULL, ":");
    }
  }
}

int builtin_logout(Cmd cmd){

  exit(0);
  //return 1;
}
