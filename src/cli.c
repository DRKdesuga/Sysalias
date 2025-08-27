#include <string.h>
#include <stdio.h>
#include "cli.h"

static int eq(const char *a, const char *b) { return a && b && strcmp(a,b)==0; }

const char *cli_cmd_str(cmd_t c) {
  switch (c) {
    case CMD_ADD: return "add";
    case CMD_RM: return "rm";
    case CMD_LIST: return "list";
    case CMD_SYNC: return "sync";
    case CMD_DOCTOR: return "doctor";
    case CMD_HELP: return "help";
    case CMD_VERSION: return "version";
    case CMD_EXPORT: return "export";
    case CMD_IMPORT: return "import";
    default: return "none";
  }
}

static int parse_cmd(const char *s, cmd_t *out) {
  if (!s || !out) return -1;
  if (eq(s,"add")) *out = CMD_ADD;
  else if (eq(s,"rm")) *out = CMD_RM;
  else if (eq(s,"list")) *out = CMD_LIST;
  else if (eq(s,"sync")) *out = CMD_SYNC;
  else if (eq(s,"doctor")) *out = CMD_DOCTOR;
  else if (eq(s,"export")) *out = CMD_EXPORT;
  else if (eq(s,"import")) *out = CMD_IMPORT;
  else if (eq(s,"help") || eq(s,"-h") || eq(s,"--help")) *out = CMD_HELP;
  else if (eq(s,"version") || eq(s,"-v") || eq(s,"--version")) *out = CMD_VERSION;
  else return -1;
  return 0;
}

int cli_parse(int argc, char **argv, cli_opts *out) {
  if (argc < 2 || !out) return -1;
  cmd_t c = CMD_NONE;
  if (parse_cmd(argv[1], &c) != 0) return -2;
  out->cmd = c;
  out->arg1 = NULL;
  out->arg2 = NULL;
  out->shell = "any";
  if (c == CMD_ADD) {
    int i = 2;
    if (i < argc && eq(argv[i], "--shell")) {
      if (i+1 >= argc) return -3;
      if (!eq(argv[i+1],"bash") && !eq(argv[i+1],"zsh")) return -3;
      out->shell = argv[i+1];
      i += 2;
    }
    if (i >= argc) return -3;
    out->arg1 = argv[i];
    if (i+1 < argc) out->arg2 = argv[i+1];
  } else if (c == CMD_RM) {
    if (argc < 3) return -3;
    out->arg1 = argv[2];
  } else if (c == CMD_IMPORT) {
    if (argc < 3) return -3;
    out->arg1 = argv[2];
  }
  return 0;
}
