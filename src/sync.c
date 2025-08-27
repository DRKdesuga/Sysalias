#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sync.h"
#include "gen_shell.h"
#include "fsutil.h"

static int str_find(const char *h, const char *n, size_t *p) {
  const char *x = strstr(h, n);
  if (!x) return -1;
  *p = (size_t)(x - h);
  return 0;
}

static char *mk_block(const char *shell) {
  const char *file = strcmp(shell, "bash")==0 ? "aliases.bash" : "aliases.zsh";
  const char *a = "# >>> sysalias start >>>\n";
  const char *b = "[ -f \"$HOME/.config/sysalias/";
  const char *c = "\" ] && . \"$HOME/.config/sysalias/";
  const char *d = "\"\n";
  const char *e = "# <<< sysalias end <<<\n";
  size_t n = strlen(a)+strlen(b)+strlen(file)+strlen(c)+strlen(file)+strlen(d)+strlen(e)+1;
  char *s = (char*)malloc(n);
  if (!s) return NULL;
  s[0]=0;
  strcat(s,a); strcat(s,b); strcat(s,file); strcat(s,c); strcat(s,file); strcat(s,d); strcat(s,e);
  return s;
}

static int rc_path(const char *shell, char *out, size_t cap) {
  const char *home = getenv("HOME");
  if (!home) return -1;
  const char *rc = strcmp(shell,"bash")==0 ? "/.bashrc" : "/.zshrc";
  size_t need = strlen(home) + strlen(rc) + 1;
  if (need > cap) return -1;
  strcpy(out, home);
  strcat(out, rc);
  return 0;
}

static int insert_block(const char *rcfile, const char *block) {
  char *data = NULL; size_t len = 0;
  int r = fs_read_all(rcfile, &data, &len);
  if (r == -2) { data = (char*)malloc(1); if (!data) return -1; data[0]=0; len = 0; }
  else if (r) return -1;
  const char *start = "# >>> sysalias start >>>";
  const char *end   = "# <<< sysalias end <<<";
  size_t ps=0, pe=0;
  int has_s = !str_find(data, start, &ps);
  int has_e = !str_find(data, end, &pe);
  char *out = NULL;
  size_t outn = 0;
  if (has_s && has_e && pe >= ps) {
    size_t pre = ps;
    size_t post = strlen(data) - (pe + strlen(end));
    outn = pre + strlen(block) + post;
    out = (char*)malloc(outn + 1);
    if (!out) { free(data); return -1; }
    memcpy(out, data, pre);
    memcpy(out + pre, block, strlen(block));
    memcpy(out + pre + strlen(block), data + pe + strlen(end), post);
    out[outn] = 0;
  } else {
    size_t add_nl = (len && data[len-1] != '\n') ? 1 : 0;
    outn = len + add_nl + strlen(block);
    out = (char*)malloc(outn + 1);
    if (!out) { free(data); return -1; }
    memcpy(out, data, len);
    if (add_nl) out[len] = '\n';
    memcpy(out + len + add_nl, block, strlen(block));
    out[outn] = 0;
  }
  free(data);
  if (fs_atomic_write(rcfile, out, outn)) { free(out); return -1; }
  free(out);
  return 0;
}

int sync_run(void) {
  if (gen_generate_all()) return 1;
  char p[1024];
  char *b1 = mk_block("bash");
  char *b2 = mk_block("zsh");
  if (!b1 || !b2) { free(b1); free(b2); return 1; }
  if (rc_path("bash", p, sizeof(p))) { free(b1); free(b2); return 1; }
  if (insert_block(p, b1)) { free(b1); free(b2); return 1; }
  if (rc_path("zsh", p, sizeof(p))) { free(b1); free(b2); return 1; }
  if (insert_block(p, b2)) { free(b1); free(b2); return 1; }
  free(b1); free(b2);
  return 0;
}
