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
typedef int (*reg_cb)(const char *name, const char *body, const char *shell, void *ud);
int registry_add(const char *kv);
int registry_add_shell(const char *shell, const char *kv);
int registry_rm(const char *name);
int registry_list(void);
int registry_iterate(reg_cb cb, void *ud);
int registry_export_stdout(void);
int registry_import_path(const char *path);
