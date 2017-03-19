/**
 * traps.c
 *
 * Dave Taht
 * 2017-03-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "preprocessor.h"
#include "traps.h"

// I learned something today. I thought errno was a tls variable that you just
// got. No. It's a subroutine. Yuck. I'm not doing that. To hell with errno as
// we know it... eventually. As well as printf.

//  400906:       e8 d5 fe ff ff          callq  4007e0 <__errno_location@plt>
//  40090b:       8b 38                   mov    (%rax),%edi

#ifdef TRAP_BASIC

noreturn void errcmd(const char* msg)
{
  perror(msg);
  exit(-1);
}

void wrncmd(const char* msg) { perror(msg); }

#endif

// incidentally, this may be STILL buggy and scribbles on the stack in some
// undefined way. Hmm. where does __func__ come from? Not clear if this is the
// right way to get this right.

#ifdef TRAP_BIG
void errcmd(const char* msg, const char* f)
{
  const char* str = "fail: %s in %s()";
  int len = strlen(msg);
  int lenf = strlen(f);
  char buf[lenf + len + sizeof(str) + 8]; // FIXME: grab as fixed mem from
                                          // elsewhere.
  sprintf(buf, str, msg, f);
  /*      it is not too early to stop using sprintf
          strncpy(buf,msg,len);
          buf[len++] = ':';
          buf[len++] = ' ';
          strncpy(&buf[len],f,strlen(f));
          buf[len++] = '(';
          buf[len++] = ')';
          buf[len++] = 0;
  */
  perror(buf); // where's my errno?
  exit(-1);
}

void wrncmd(const char* msg, const char* f)
{
  int len = strlen(msg);
  int lenf = strlen(f);
  char buf[lenf + len + 8];
  // FIXME: grab as fixed mem from elsewhere and this is not really an inline
  sprintf(buf, "warning: %s in %s()", msg, f);
  perror(buf); // where's my errno?
}

#endif

#ifdef DEBUG_MODULE
int main(void)
{
  printf("Let's trap some stuff!\n");
  // install signal handler, have fun
  printf("unfinished.\n");
  return -1;
}
#endif
