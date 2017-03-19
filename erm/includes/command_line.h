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
#include "tabeld.h"

typedef struct {
  global_flags_t g;
  uint32_t about : 1;
  uint32_t version : 1;
  uint32_t format : 4;
  uint32_t secure : 1;
  uint32_t dry_run : 1;
  uint32_t trust : 1;
  uint32_t plugins : 1;
  uint32_t udplite : 1;
  uint32_t mux : 1;
  uint32_t diversity : 2;
  uint32_t daemonize;
  uint32_t traps;
  uint32_t hello_interval;
  uint32_t wired_hello_interval;
  uint32_t resend_delay;
  uint32_t half_time;
  uint32_t allowed_gid;
  uint16_t kernel_priority;
  uint16_t duplicate_priority;
  uint16_t table;
  uint16_t table_export;
  uint16_t port;
  uint16_t config_port;
  uint16_t v4kernel_metric;
  uint16_t v6kernel_metric;
  uint8_t output_format;
  uint8_t transport1;
  uint8_t transport2;
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
