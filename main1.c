/******************************************************************************
 *
 *  File Name........: main.c
 *
 *  Description......: Simple driver program for ush's parser
 *
 *  Author...........: Vincent W. Freeh
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "init.h"


int main(int argc, char *argv[])
{
  Pipe p;
  char *host = "armadillo";

  init_shell();

  /*while ( 1 ) {
    printf("%s%% ", host);
    p = parse();
    prPipe(p);
    freePipe(p);
  }*/
}

/*........................ end of main.c ....................................*/
