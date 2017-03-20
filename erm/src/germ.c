/**
 * germ.c
 * command line shared memory interface for the embedded routing machine
 *
 * Dave Taht
 * 2017-03-17
 */

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <iconv.h>
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */

#include "c11threads.h"
#include "defaults.h"
#include "erm_client.h"
#include "init.h"
#include "io.h"
#include "shared.h"
#include "tabeld.h"
#include "traps.h"

#include "arch.c"

#define GERM_VERSION ".01"

static int usage(char* err)
{
  if(err) fprintf(stderr, "%s\n", err);
  printf("germ [options]\n");
  printf("-V --version                  print the version and exit\n"
         "-a --attach [ erm machine ] \n");
  exit(0);
}

static const struct option long_options[] = { { "version", no_argument, NULL, 'v' },
                                              { "attach", required_argument, NULL, 'a' },
                                              { "help", no_argument, NULL, '?' } };

typedef struct {
  char* attach;
} GermCommandLineOpts_t;

GermCommandLineOpts_t germ_default_options(GermCommandLineOpts_t o)
{
  o.attach = "/tabelb";
  return o;
}

#define QSTRING "?a:v"

GermCommandLineOpts_t process_options(int argc, char** argv, GermCommandLineOpts_t o)
{
  int option_index = 0;
  int opt = 0;

  optind = 1;

  while(true) {
    opt = getopt_long(argc, argv, QSTRING, long_options, &option_index);
    if(opt == -1) break;

    switch(opt) {
    case 'V':
      printf("germ-%s\n", GERM_VERSION);
      break;
    case 'a':
      o.attach = optarg;
      break;
    case '?':
      usage("germ:");
    default:
      fprintf(stderr, "%d", opt);
      usage("Invalid option");
      break;
    }
  }
  return o;
}

int main(int argc, char** argv)
{
  GermCommandLineOpts_t o = { 0 };
  erm_t e;
  o = process_options(argc, argv, germ_default_options(o));
  TRAP_WEQ(e = erm_attach_client(o.attach), (void*)-1,
           "Couldn't attach to erm instance");
  return 0;
}
