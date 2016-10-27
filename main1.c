/******************************************************************************
 *
 *  File Name........: main1.c
 *
 *  Description......: Micro shell
 *
 *  Author...........: Ankit Gohil
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>         //getpriority() & setpriority()
#include <sys/resource.h>     //getpriority() & setpriority()
#include <signal.h>
#include "parse.h"
//#include "init.h"
#include "builtins.h"


int nice_flag;
long int priority_val;
long int original_priority_val;

int get_fd(Token t, char* file_name){
  int fd;
  switch(t){
    case Tin:
      fd = open(file_name, O_RDONLY, 0666);
      break;

    case Tout:
    case ToutErr:
      fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
      break;

    case Tapp:
    case TappErr:
      fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0666);
      break;
  }
  return fd;
}

/*
* If nice command, check if a numeric value is passed with the command.
* If yes, change the Cmd struct by shifting left the args to two places.
* Else, shift left by one position and set the default priority to 4.
*/
int get_priority(Cmd c, long int* priority){
  int shift = 2;
  char* remain_str;

  if(c->nargs == 1){
    printf("Incorrect number of arguments passed to %s\n", c->args[0]);
    return 0;
  }

  if(c->nargs > 1){
    *priority = strtol(c->args[1], &remain_str, 10);

    if(*remain_str != '\0'){
      *priority = 4;    //Default priority
      shift = 1;
    }
  }

  int i;
  for(i=0; i < c->nargs - shift; i++){
    strcpy(c->args[i], c->args[i+shift]);
  }

  int j;
  for(j=i; j < c->nargs; j++){
    c->args[j] = '\0';
    free(c->args[j]);
  }
  c->nargs = c->nargs-shift;

  //ps -fl -C "perl test.pl" to display nice value of the program
}

void launch_process(Cmd command, int infile_fd, int outfile_fd){
  int index;

  /* Set the standard input/output channels of the new process.  */
  if (infile_fd != STDIN_FILENO){
     dup2 (infile_fd, STDIN_FILENO);
     close (infile_fd);
  }
  if (outfile_fd != STDOUT_FILENO){
     dup2 (outfile_fd, STDOUT_FILENO);
     close (outfile_fd);
  }

  //Check if command is in shell_builtins, execute it
  if((index = is_builtin(command->args[0]))){
    exec_builtin(command, infile_fd, outfile_fd, index);
    return;
  }

  if (execvp(command->args[0], command->args) == -1) {
    perror("ush");
  }
}

void exec_pipe(Pipe p){
  Cmd c;
  int pipe_fd[2], infile_fd, outfile_fd;
  pid_t pid, wpid;
  int status;
  int index; //Index for built-in command in the builtins array

  c = p->head;

  if(c==NULL)
    return;

  //If command has input redirection
  if(c->in == Tin){
    infile_fd = get_fd(c->in, c->infile);
  }else{
    infile_fd = STDIN_FILENO;
  }

  while(c!=NULL){
    //Set pipes if there is more than one command in the pipeline
    if(c->next){
      if(pipe(pipe_fd) == -1){
        perror("Pipe error");
        return;
      }
      outfile_fd = pipe_fd[1];
    }else{
      if(c->out!=Tnil){
        outfile_fd = get_fd(c->out, c->outfile);
      }else{
        outfile_fd = STDOUT_FILENO;
      }
    }

    if(strcmp(c->args[0], "nice") == 0){
      get_priority(c,&priority_val);
      nice_flag = 1;
    //  printf("Priority Value = %ld \n", priority_val);
    //  printf("Number of args = %d \n", c->nargs);
    }

    /*
    *Built-in commands are executed within shell.
    *If a built-in command occurs as any component of a pipeline
    *except the last, it is executed in a subshell
    */

    //Check if it's a built-in command and last in the pipeline
    if((index = is_builtin(c->args[0])) && c->next == NULL){

      /*If built-in was launched with modified priority, change
      it's priority*/
      if(nice_flag){
        original_priority_val = getpriority(PRIO_PROCESS, 0);
        setpriority(PRIO_PROCESS, 0, priority_val);
      }

      exec_builtin(c, infile_fd, outfile_fd, index);

      if(nice_flag){
        setpriority(PRIO_PROCESS, 0, original_priority_val);
      }


    }else{
      //Fork a process and execute it
      pid = fork();
      if (pid == 0) {
        // Child process

        if(nice_flag){
          setpriority(PRIO_PROCESS, 0, priority_val);
          //printf("Priority = %ld", priority_val);
        }

        launch_process(c, infile_fd, outfile_fd);
      }else if (pid < 0) {
        // Error forking
        perror("lsh");
      } else {
        // Parent process
        do{
          wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
      }
    }

    /* Clean up after pipes.  */
    if (infile_fd != STDIN_FILENO)
    close (infile_fd);
    if (outfile_fd != STDOUT_FILENO)
    close (outfile_fd);

    infile_fd = pipe_fd[0];

    c=c->next;
  }
}


void ignore_signals(){
  //'trap -l' gives a list of signals

  signal(SIGQUIT,SIG_IGN);
  signal(SIGHUP,SIG_IGN);

}

void exec_ushrc(char *file){
  Pipe p;
  int stdin_fd;
  stdin_fd = dup(STDIN_FILENO);

  FILE* fp;
  int ushrc_fd;
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  int no_lines = 0;

  fp = fopen(file, "r");
  if (fp == NULL){
    //printf(".ushrc file does not exist in home directory");
    return;
  }
  ushrc_fd = fileno(fp);

  dup2(ushrc_fd, STDIN_FILENO);
  while ((read = getline(&line, &len, fp)) != -1) {
    no_lines++;
  }
  rewind(fp);  //To the beginning of file

  int i;
  for(i=0; i<no_lines; i++){
    fflush(stdout);

    p = parse();

    while(p != NULL){
      exec_pipe(p);
      p = p->next;
    }
    //exec_pipe(p);
    freePipe(p);
  }

  dup2(stdin_fd, STDIN_FILENO);
}

void init_shell(){
  char ushrc_file[100];
  char* home;

  home=getenv("HOME");

  sprintf(ushrc_file, "%s/.ushrc", home);
  ignore_signals();
  exec_ushrc(ushrc_file);
}

int main(int argc, char *argv[]){
  int run_shell = 1;
  Pipe p;

  init_shell();

  fflush(stdin);
  char host[64];
  gethostname(host,64);


  /*The pipe handling is inspired by code at
  https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html#Launching-Jobs
  */
  while(run_shell){
    printf("%s%% ", host);
    fflush(stdout);

    p = parse();

    while(p != NULL){
      exec_pipe(p);
      p = p->next;
    }
    //exec_pipe(p);
    freePipe(p);
  }
  /*while ( 1 ) {
    printf("%s%% ", host);
    p = parse();
    prPipe(p);
    freePipe(p);
  }*/
}

/*........................ end of main.c ....................................*/
