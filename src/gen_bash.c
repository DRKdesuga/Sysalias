#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gen_shell.h"
#include "registry.h"
#include "fsutil.h"
#include "strutil.h"

typedef struct { char *buf; size_t cap; size_t len; const char *want; } sb_t;

static int sb_add(sb_t *s, const char *a, const char *b) {
    char *eb = s_escape_sh_single(b);
    if (!eb) return -1;
    size_t need = s->len + strlen("alias =''\n") + strlen(a) + strlen(eb) + 1;
    if (need > s->cap) {
        size_t nc = s->cap ? s->cap * 2 : 1024;
        while (nc < need) nc *= 2;
        char *nb = realloc(s->buf, nc);
        if (!nb) { free(eb); return -1; }
        s->buf = nb; s->cap = nc;
    }
    s->len += sprintf(s->buf + s->len, "alias %s='%s'\n", a, eb);
    free(eb);
    return 0;
}

static int cb_filter(const char *n, const char *b, const char *sh, void *ud) {
    sb_t *s = (sb_t*)ud;
    if (strcmp(s->want,"bash")==0) {
        if (!(strcmp(sh,"any")==0 || strcmp(sh,"bash")==0)) return 0;
    } else {
        if (!(strcmp(sh,"any")==0 || strcmp(sh,"zsh")==0)) return 0;
    }
    return sb_add(s, n, b);
}

static int write_file(const char *name, const char *data, size_t len) {
    char dir[1024], path[1024];
    if (fs_xdg_config_dir(dir, sizeof(dir))) return -1;
    size_t need = strlen(dir) + strlen("/sysalias") + 1;
    if (need > sizeof(dir)) return -1;
    strcat(dir, "/sysalias");
    if (fs_mkdir_p(dir)) return -1;
    if (snprintf(path, sizeof(path), "%s/%s", dir, name) <= 0) return -1;
    return fs_atomic_write(path, data, len);
}

int gen_generate_all(void) {
    sb_t sb = {0};
    sb.want = "bash";
    if (registry_iterate(cb_filter, &sb)) { free(sb.buf); return -1; }
    if (write_file("aliases.bash", sb.buf ? sb.buf : "", sb.len)) { free(sb.buf); return -1; }
    free(sb.buf);
    sb.buf = NULL; sb.cap = 0; sb.len = 0;
    sb.want = "zsh";
    if (registry_iterate(cb_filter, &sb)) { free(sb.buf); return -1; }
    if (write_file("aliases.zsh", sb.buf ? sb.buf : "", sb.len)) { free(sb.buf); return -1; }
    free(sb.buf);
    return 0;
}
