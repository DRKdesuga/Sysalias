#pragma once
#include <stddef.h>
int fs_xdg_config_dir(char *out, size_t cap);
int fs_mkdir_p(const char *path);
int fs_read_all(const char *path, char **data, size_t *len);
int fs_atomic_write(const char *path, const char *data, size_t len);
