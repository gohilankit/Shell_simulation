#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<signal.h>
#include "init.h"
#include "parse.h"

static void prCmd(Cmd c)
{
  int i;

  if ( c ) {
    printf("%s%s ", c->exec == Tamp ? "BG " : "", c->args[0]);
    if ( c->in == Tin )
      printf("<(%s) ", c->infile);
    if ( c->out != Tnil )
      switch ( c->out ) {
      case Tout:
	printf(">(%s) ", c->outfile);
	break;
      case Tapp:
	printf(">>(%s) ", c->outfile);
	break;
      case ToutErr:
	printf(">&(%s) ", c->outfile);
	break;
      case TappErr:
	printf(">>&(%s) ", c->outfile);
	break;
      case Tpipe:
	printf("| ");
	break;
      case TpipeErr:
	printf("|& ");
	break;
      default:
	fprintf(stderr, "Shouldn't get here\n");
	exit(-1);
      }

    if ( c->nargs > 1 ) {
      printf("[");
      for ( i = 1; c->args[i] != NULL; i++ )
	printf("%d:%s,", i, c->args[i]);
      printf("\b]");
    }
    putchar('\n');
    // this driver understands one command
    if ( !strcmp(c->args[0], "end") )
      exit(0);
  }
}

static void prPipe(Pipe p)
{
  int i = 0;
  Cmd c;

  if ( p == NULL )
    return;

  printf("Begin pipe%s\n", p->type == Pout ? "" : " Error");
  for ( c = p->head; c != NULL; c = c->next ) {
    printf("  Cmd #%d: ", ++i);
    prCmd(c);
  }
  printf("End pipe\n");
  prPipe(p->next);
}



void ignore_signals(){
  //'trap -l' gives a list of signals

  signal(SIGQUIT,SIG_IGN);
  signal(SIGHUP,SIG_IGN);

}

void exec_ushrc(char *file){
  int stdin_fd;
  stdin_fd = dup(STDIN_FILENO);

 //printf("File descriptor = %d \n", stdin_fd);
 //printf("UshRc = %s \n", file);

/* int ushrc_fd;
 if (ushrc_fd = open(file, O_RDONLY) < 0){
   printf("Error opening .ushrc file");
 }else{
   if(dup2(ushrc_fd,STDIN_FILENO) < 0){
     print("Dup2 behaved unexpectedly");
     return;
   }
   close(ushrc_fd);
 } */

 freopen(file,"r",stdin);
 Pipe p = parse();

 prPipe(p);
 freePipe(p);

}



void init_shell(){
  char ushrc_file[100];
  char* home;

  home=getenv("HOME");

  sprintf(ushrc_file, "%s/.ushrc", home);

  ignore_signals();
  exec_ushrc(ushrc_file);


}
