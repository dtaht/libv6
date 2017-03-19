/**
 * commands.h
 *
 * Dave Taht
 * 2017-03-18
 */

#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
	ROUTER_NONE = 0,
	ROUTER_FILT_ME = 1,
	ROUTER_FILT_DEAD =2,
	ROUTER_FILT_LAG = 4,
	ROUTER_FILT_ROUTERS = 8,
	ROUTER_FILT_ROUTES = 16,
	ROUTER_FILT_XROUTES = 32,
	ROUTER_FILT_INTERFACES = 64,
	ROUTER_FILT_ALL = (ROUTER_FILT_ME | ROUTER_FILT_DEAD | ROUTER_FILT_LAG | ROUTER_FILT_ROUTERS),
	ROUTER_FILT_INTERFACES_ALL = (ROUTER_FILT_ME | ROUTER_FILT_DEAD | ROUTER_FILT_LAG | ROUTER_FILT_INTERFACES)
} router_filt_t;

// The second arg became perms. 0 is root required. 1 is membership in the erm group
// 2 is anyone that can connect. 3 everyone else. There may be more levels in the future.
// Or I may flip the levels around as I think about the security model more.

typedef void (*event_cb_t)(router_filt_t filt, int arg);

void command_dump(router_filt_t filt, int arg);
void command_monitor(router_filt_t filt, int arg);
void command_unmonitor(router_filt_t filt, int arg);
void command_reload(router_filt_t filt, int arg);
void command_quit(router_filt_t filt, int arg);
void command_stats(router_filt_t filt, int arg1);

void command_open_pod_bay_doors(router_filt_t filt, int arg);
void command_erm_stats(router_filt_t filt, int arg1);
void command_erm_dump(router_filt_t filt, int arg1);

#endif
