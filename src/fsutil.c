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

#include "fsutil.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int isabs(const char *p)
{
    return p && p[0] == '/';
}

int fs_xdg_config_dir(char *out, size_t cap)
{
    const char *xdg = getenv("XDG_CONFIG_HOME");
    if (xdg && *xdg)
    {
        if (strlen(xdg) + 1 > cap)
            return -1;
        strcpy(out, xdg);
        return 0;
    }
    const char *home = getenv("HOME");
    if (!home || !*home)
        return -1;
    size_t need = strlen(home) + strlen("/.config") + 1;
    if (need > cap)
        return -1;
    strcpy(out, home);
    strcat(out, "/.config");
    return 0;
}

int fs_mkdir_p(const char *path)
{
    if (!path || !*path)
        return -1;
    size_t n0 = strlen(path);
    char *p = (char *)malloc(n0 + 1);
    if (!p)
        return -1;
    memcpy(p, path, n0 + 1);
    size_t n = strlen(p);
    if (n && p[n - 1] == '/')
        p[n - 1] = 0;
    char *q = p;
    if (isabs(q))
        q++;
    for (; *q; q++)
    {
        if (*q == '/')
        {
            *q = 0;
            if (strlen(p) && mkdir(p, 0700) && errno != EEXIST)
            {
                free(p);
                return -1;
            }
            *q = '/';
        }
    }
    if (mkdir(p, 0700) && errno != EEXIST)
    {
        free(p);
        return -1;
    }
    free(p);
    return 0;
}

int fs_read_all(const char *path, char **data, size_t *len)
{
    if (!path || !data)
        return -1;
    FILE *f = fopen(path, "rb");
    if (!f)
        return -2;
    if (fseek(f, 0, SEEK_END))
    {
        fclose(f);
        return -1;
    }
    long sz = ftell(f);
    if (sz < 0)
    {
        fclose(f);
        return -1;
    }
    if (fseek(f, 0, SEEK_SET))
    {
        fclose(f);
        return -1;
    }
    char *buf = (char *)malloc((size_t)sz + 1);
    if (!buf)
    {
        fclose(f);
        return -1;
    }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[rd] = 0;
    *data = buf;
    if (len)
        *len = rd;
    return 0;
}

int fs_atomic_write(const char *path, const char *data, size_t len)
{
    if (!path || !data)
        return -1;
    char tmp[4096];
    size_t lp = strlen(path);
    if (lp + 5 >= sizeof(tmp))
        return -1;
    memcpy(tmp, path, lp);
    memcpy(tmp + lp, ".tmp", 5);
    FILE *f = fopen(tmp, "wb");
    if (!f)
        return -1;
    size_t wr = fwrite(data, 1, len, f);
    if (wr != len)
    {
        fclose(f);
        unlink(tmp);
        return -1;
    }
    if (fflush(f))
    {
        fclose(f);
        unlink(tmp);
        return -1;
    }
    if (fclose(f))
    {
        unlink(tmp);
        return -1;
    }
    if (rename(tmp, path))
    {
        unlink(tmp);
        return -1;
    }
    return 0;
}
