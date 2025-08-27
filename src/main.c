#include <stdio.h>
#include <stdlib.h>
#include "cli.h"
#include "registry.h"
#include "sync.h"
#include "doctor.h"

static void print_help(void) {
  printf("Usage:\n");
  printf("  sysalias add [--shell bash|zsh] NAME=BODY\n");
  printf("  sysalias rm NAME\n");
  printf("  sysalias list\n");
  printf("  sysalias sync\n");
  printf("  sysalias doctor\n");
  printf("  sysalias help\n");
  printf("  sysalias version\n");
}

int main(int argc, char **argv) {
  cli_opts opts;
  int rc = cli_parse(argc, argv, &opts);
  if (rc == -1) { fprintf(stderr,"No command provided\n"); print_help(); return 2; }
  if (rc == -2) { fprintf(stderr,"Unknown command\n"); print_help(); return 2; }
  if (rc == -3) { fprintf(stderr,"Missing argument\n"); print_help(); return 2; }

  switch (opts.cmd) {
    case CMD_HELP:    print_help(); return 0;
    case CMD_VERSION: printf("sysalias %s\n", "0.1.0"); return 0;
    case CMD_LIST:    return registry_list() ? 1 : 0;
    case CMD_ADD:     return registry_add_shell(opts.shell, opts.arg1) ? 1 : 0;
    case CMD_RM:      return registry_rm(opts.arg1);
    case CMD_SYNC:    return sync_run();
    case CMD_DOCTOR:  return doctor_run();
    default:          fprintf(stderr,"Internal error\n"); return 1;
  }
}
