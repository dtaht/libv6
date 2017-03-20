/**
 * commands.c
 *
 * Dave Taht
 * 2017-03-13
 */

#include <stdint.h>
#include "commands.h"
#include "init.h"
#include "io.h"
#include "shared.h"

typedef struct {
  char* cmd;
  event_cb_t c;
  router_filt_t arg1;
  int arg2;
} cb_t;

// This started getting ambitious on me - as in being able to specify
// a neighbor, interface or router. Also enabling/disabling logging.
// It's already a sizeable increase on the existing babel interface
// reload, restart, retract, query neighbour, stats, rtt? what else?
// add an interface, down an interface... flush routes to a router...
// json output, text, logging... the config itself... memory usage...
// Aggghhhhh

// The R/W aspects need to be thought about carefully

const cb_t callbacks[] =
{ { "dump", command_dump, ROUTER_FILT_ALL, 1 },
  { "dump routers", command_dump, ROUTER_FILT_ROUTERS, 1 },
  { "dump neighbours", command_dump, ROUTER_FILT_ROUTERS, 1 },
  { "dump routes", command_dump, ROUTER_FILT_ROUTES, 1 },
  { "dump xroutes", command_dump, ROUTER_FILT_XROUTES, 1 },
  { "dump interfaces", command_dump, ROUTER_FILT_INTERFACES, 1 },

  { "monitor", command_monitor, 15, 1 },
  { "monitor routers", command_monitor, 1, 1 },
  { "monitor routes", command_monitor, 2, 1 },
  { "monitor xroutes", command_monitor, 4, 1 },
  { "monitor interfaces", command_monitor, 8, 1 },

  { "unmonitor", command_unmonitor, 15, 1 },
  { "umonitor routers", command_unmonitor, 1, 1 },
  { "unmonitor routes", command_unmonitor, 2, 1 },
  { "unmonitor xroutes", command_unmonitor, 4, 1 },
  { "unmonitor interfaces", command_unmonitor, 8, 1 },

  { "stats", command_dump, ROUTER_FILT_ALL, 1 },
  { "stats routers", command_dump, ROUTER_FILT_ROUTERS, 1 },
  { "stats neighbours", command_dump, ROUTER_FILT_ROUTERS, 1 },
  { "stats routes", command_dump, ROUTER_FILT_ROUTES, 1 },
  { "stats xroutes", command_dump, ROUTER_FILT_XROUTES, 1 },
  { "stats interfaces", command_dump, ROUTER_FILT_INTERFACES, 1 },

  { "restart", command_reload, 0, 0 },
  { "reload", command_reload, 0, 0 },

  { "quit", command_quit, 0, 1 },

  // Erm specific commands

  { "open the pod bay doors", command_open_pod_bay_doors, 0, 1 },

  { "erm", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm memory", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm cpu", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm arch", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm version", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm clients", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm uptime", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm credits", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm config", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },

  { "erm log", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm log on", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm log off", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },

  { "erm monitor lag", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm monitor cpu", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm monitor ermcpu", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm monitor bandwidth", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm unmonitor", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },

  { "erm output", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm output text", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm output json", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },

  { "erm output raw", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },

  { "erm flush", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm split", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm join", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm clone", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm radiate", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm kickstart", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },
  { "erm gc", command_erm_dump, ROUTER_FILT_INTERFACES, 0 },

  { "erm snapshot", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },
  { "erm consistency", command_erm_dump, ROUTER_FILT_INTERFACES, 1 },

  { NULL, NULL, 0 } };

void command_open_pod_bay_doors(router_filt_t filt, int arg1)
{
  printf("I'm sorry dave, I can't do that.\n");
}

void command_monitor(router_filt_t filt, int arg1) {}

void command_unmonitor(router_filt_t filt, int arg1) {}

void command_reload(router_filt_t filt, int arg1) {}

void command_quit(router_filt_t filt, int arg1)
{
  //	erm_detach(); (atexit should just do it for us?)
  // exit(0);
}

void command_dump(router_filt_t filt, int arg1)
{
  /*	int c = 0;
          for (int i = 0; i < routers.size; i++) {
                  if(routers[i].flags & filt) {
                  c++;
                  printf("%s\n",
                          format_eui64(routers.a[i]));
                  }
          }
          return c;
  */
}

void command_erm_dump(router_filt_t filt, int arg1)
{
  /*	int c = 0;
          for (int i = 0; i < routes.size; i++) {
                  if(routes[i].flags & filt) {
                  c++;
                  printf("%s\n",
                          format_eui64(routers.a[i]));
                  }
          }
          return c;
  */
}

// Used to quickly dump the command strings to a file

#ifdef CMD_DUMP
int main()
{
  for(int i = 0; callbacks[i].cmd != NULL; i++)
    printf("%s\n", callbacks[i].cmd);
  return 0;
}
#endif

#ifdef DEBUG_MODULE
int main()
{
  command_open_pod_bay_doors(0, 0);
  printf("success! (otherwise)\n");
  return 0;
}
#endif
