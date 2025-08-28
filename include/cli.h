/*
 * sysalias - centralized alias manager for bash and zsh
 * Copyright (C) 2025  DRKdesuga
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
  CMD_VERSION,
  CMD_EXPORT,
  CMD_IMPORT
} cmd_t;

typedef struct {
  cmd_t cmd;
  const char *arg1;
  const char *arg2;
  const char *shell;
} cli_opts;

int cli_parse(int argc, char **argv, cli_opts *out);
const char *cli_cmd_str(cmd_t c);
