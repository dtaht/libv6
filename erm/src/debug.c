/**
 * debug.c
 *
 * Dave Taht
 * 2017-03-15
 */
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif

void mignore(const char* msg)
{
  printf("ignored: %s\n", msg);
  exit(0);
}

void unfinished(const char* msg)
{
  printf("unfinished: %s\n", msg);
  exit(-1);
}

#define BT_BUF_SIZE 100

int debug_backtrace(char* msg)
{
#ifdef HAVE_EXECINFO
  int j, nptrs;
  void* buffer[BT_BUF_SIZE];
  char** strings;

  nptrs = backtrace(buffer, BT_BUF_SIZE);
  printf("backtrace() returned %d addresses\n", nptrs);

  /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
           would produce similar output to the following: */

  strings = backtrace_symbols(buffer, nptrs);
  if(strings == NULL) {
    perror("backtrace_symbols");
    exit(EXIT_FAILURE);
  }

  for(j = 0; j < nptrs; j++) printf("%s\n", strings[j]);

  free(strings);
  return -1;
#else
  printf(msg);
  return -1;
#endif

}
#ifdef DEBUG_MODULE
MFIXME
#endif
