#pragma once
#include <stddef.h>

typedef enum {
  CMD_NONE = 0,
  CMD_ADD,
  CMD_RM,
  CMD_LIST,
  CMD_SYNC,
  CMD_DOCTOR,
  CMD_HELP,
  CMD_VERSION
} cmd_t;

typedef struct {
  cmd_t cmd;
  const char *arg1;
  const char *arg2;
} cli_opts;

int cli_parse(int argc, char **argv, cli_opts *out);
const char *cli_cmd_str(cmd_t c);
