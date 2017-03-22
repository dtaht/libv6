/**
 * command_line.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H
#define HAVE_EXTENSIONS

#include "protocol.h"
#include "shared.h"
#include "erm_types.h"

typedef struct {
  global_flags_t g;
  u32 about : 1;
  u32 version : 1;
  u32 format : 4;
  u32 secure : 1;
  u32 dry_run : 1;
  u32 trust : 1;
  u32 plugins : 1;
  u32 udplite : 1;
  u32 mux : 1;
  u32 diversity : 2;
  u32 daemonize;
  u32 traps;
  u32 hello_interval;
  u32 wired_hello_interval;
  u32 resend_delay;
  u32 half_time;
  u32 allowed_gid;
  u16 kernel_priority;
  u16 duplicate_priority;
  u16 table;
  u16 table_export;
  u16 port;
  u16 config_port;
  u16 v4kernel_metric;
  u16 v6kernel_metric;
  u32 output_format;
  u32 transport1;
  u32 transport2;
  char* config_local;
  char* mcast_address;
  char* config_address;
  char* unicast_address;
  char* state_file;
  char* id_file;
  char* plugin;
  char* config_file;
  char* pid_file;
  char* keys_file;
  char* config_keys_file;
  char* my_keys_file;
  char* my_pub_file;

} CommandLineOpts_t;

extern CommandLineOpts_t process_options(int argc, char** argv, CommandLineOpts_t o);

#endif
