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
#include "parse.h"
#include "init.h"

void launch_process(Cmd command, int infile_fd, int outfile_fd){
  //Check if command is in shell_builtins, execute it


  /* Set the standard input/output channels of the new process.  */
  if (infile_fd != STDIN_FILENO){
     dup2 (infile_fd, STDIN_FILENO);
     close (infile_fd);
  }
  if (outfile_fd != STDOUT_FILENO){
     dup2 (outfile_fd, STDOUT_FILENO);
     close (outfile_fd);
  }

  if (execvp(command->args[0], command->args) == -1) {
    perror("ush");
  }
  exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
  int run_shell = 1;
  Pipe p;
  char *host = NULL;

  init_shell();

  char host[60];
  gethostname(host,60);
  Cmd c;
  int pipe_fd[2], infile_fd, oufile_fd;
  pid_t pid, wpid;
  int status;

  /*The pipe handling is inspired by code at
  https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html#Launching-Jobs
  */
  while(run_shell){
    printf("%s%% ", host);
    p = parse();

    if (p == NULL)
      return;

      c = p->head;

      //If command has input redirection
      if(c->in == Tin){
        infile_fd = fileno(c->infile);   //FIX
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
          oufile_fd = pipe_fd[1];
        }else{
          if(cout!=Tnil){
            outfile_fd = fileno(c->infile);  //FIX
          }else{
            outfile_fd = STDOUT_FILENO;
          }
        }

        //Fork a process and execute it
        pid = fork();
        if (pid == 0) {
          // Child process
          launch_process(c, infile_fd, outfile_fd);
          if (execvp(args[0], args) == -1) {
            perror("lsh");
          }
          exit(EXIT_FAILURE);
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
        if (infile_fd != j->stdin)
        close (infile_fd);
        if (outfile_fd != j->stdout)
        close (outfile_fd);

        infile_fd = mypipe[0];
      }
  }
  /*while ( 1 ) {
    printf("%s%% ", host);
    p = parse();
    prPipe(p);
    freePipe(p);
  }*/
}

/*........................ end of main.c ....................................*/
