/**
 * command_line.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H
#define HAVE_EXTENSIONS

#include "shared.h"
#include "tabeld.h"

typedef struct {
	global_flags_t g;
	uint32_t about:1;
	uint32_t version:1;
	uint32_t format:4;
	uint32_t secure:1;
	uint32_t dry_run:1;
	uint32_t trust:1;
	uint32_t plugins:1;
	uint32_t traps;
	uint32_t hello_interval;
	uint32_t half_time;
	uint16_t kernel_priority;
	uint16_t duplicate_priority;
	uint16_t table;
	uint16_t table_export;
	uint32_t wired_hello_interval;
	uint8_t port;
	uint8_t config_port;
	uint8_t diversity;
	int8_t output_format;
	char *config_local;
	char *mcast_address;
	char *state_file;
	char *plugin;
	char *config_file;
	char *pidfile;
} CommandLineOpts_t;

extern CommandLineOpts_t process_options(int argc, char **argv, CommandLineOpts_t o);

#endif
