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
#include<fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parse.h"
#include "init.h"

int get_fd(char* file_name){
  int fd;
  fd = open(file_name, O_RDWR|O_CREAT, 0666);
  return fd;
}

void launch_process(Cmd command, int infile_fd, int outfile_fd){
  //Check if command is in shell_builtins, execute it
  printf("In launch process");


  /* Set the standard input/output channels of the new process.  */
  if (infile_fd != STDIN_FILENO){
     dup2 (infile_fd, STDIN_FILENO);
     close (infile_fd);
  }
  if (outfile_fd != STDOUT_FILENO){
     dup2 (outfile_fd, STDOUT_FILENO);
     close (outfile_fd);
  }

  /*if (execvp(command->args[0], command->args) == -1) {
    perror("ush");
  }
  exit(EXIT_FAILURE);*/

  /* Exec the new process.  Make sure we exit.  */
  execvp (command->args[0], command->args);
  perror ("execvp");
  exit (1);
}

void exec_pipe(Pipe p){
  Cmd c;
  int pipe_fd[2], infile_fd, outfile_fd;
  pid_t pid, wpid;
  int status;

  c = p->head;

  //If command has input redirection
  if(c->in == Tin){
    infile_fd = get_fd(c->infile);
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
        outfile_fd = get_fd(c->outfile);
      }else{
        outfile_fd = STDOUT_FILENO;
      }
    }

    //Fork a process and execute it
    pid = fork();
    if (pid == 0) {
      // Child process
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

    /* Clean up after pipes.  */
    if (infile_fd != STDIN_FILENO)
    close (infile_fd);
    if (outfile_fd != STDOUT_FILENO)
    close (outfile_fd);

    infile_fd = pipe_fd[0];

    c=c->next;
  }
}


int main(int argc, char *argv[]){
  int run_shell = 1;
  Pipe p;

  //init_shell();

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