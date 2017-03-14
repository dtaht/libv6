/**
 * commands.c
 *
 * Dave Taht
 * 2017-03-13
 */

#include "commands.h"

#include "shared.h"
#include "io.h"
#include "init.h"

#define ROUTER_FILT_ME 1
#define ROUTER_FILT_DEAD 2
#define ROUTER_FILT_LAG 4

#define ROUTER_FILT_ALL (ROUTER_FILT_ME | ROUTER_FILT_DEAD | ROUTER_FILT_LAG)

enum router_filt {
	none = 0,
	me = 1,
	dead = 2,
	lagging = 4,
	// wireless? wired? imported?
	all = 15,
	MAX_router_filt
};

typedef void (*event_cb_t)(int arg1, int arg2);
struct cb { char *cmd, *event_cb_t cb, int arg1, int arg2} ;
void dump_routers(router_filt filt, int arg);

// This started getting ambitious on me - as in being able to specify
// a neighbor, interface or router. Also enabling/disabling logging.
// It's already a sizeable increase on the existing babel interface
// reload, restart, retract, query neighbour, stats, rtt? what else?
// add an interface, down an interface... flush routes to a router...
// json output, text, logging... the config itself... memory usage...
// Aggghhhhh

const cb callbacks[] = {
	{ "dump", dump_routers, ROUTER_FILT_ALL, 0 },
	{ "dump routers", dump_routers, ROUTER_FILT_ALL, 0 },
	{ "dump neighbours", dump_routers, ROUTER_FILT_ALL, 0 },
	{ "dump routes", dump_routers, ROUTER_FILT_ALL, 0 },
	{ "dump xroutes", dump_routers, ROUTER_FILT_ALL, 0 },
	{ "dump interfaces", dump_routers, ROUTER_FILT_ALL, 0 },

	{ "monitor", monitor, 15 , 0 },
	{ "monitor routers", monitor, 1 ,0 },
	{ "monitor routes", monitor, 2, 0 },
	{ "monitor xroutes", monitor, 4, 0 },
	{ "monitor interfaces", monitor, 8, 0 },

	{ "unmonitor", unmonitor, 15, 0  },
	{ "umonitor routers", unmonitor, 1, 0 },
	{ "unmonitor routes", unmonitor, 2, 0 },
	{ "unmonitor xroutes", unmonitor, 4, 0 },
	{ "unmonitor interfaces", unmonitor, 8, 0 },

	{ "reload", command_reload, 0, 0 },
	{ "quit", command_quit, 0, 0 },
	{ NULL, NULL, 0 }
};

// Use duff's device?

void dump_routers(router_filt filt, int arg) {
	int c = 0;
	for (int i = 0; i < routers.size; i++) {
		if(routers[i].flags & filt) {
		c++;
		print("%s\n",
			format_eui64(routers.a[i]));
		}
	}
	return c;
}

int dump_routes(routes_filt filt) {
	int c = 0;
	for (int i = 0; i < routes.size; i++) {
		if(routes[i].flags & filt) {
		c++;
		print("%s\n",
			format_eui64(routers.a[i]));
		}
	}
	return c;
}

