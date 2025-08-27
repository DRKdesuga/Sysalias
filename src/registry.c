#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "registry.h"
#include "fsutil.h"
#include "strutil.h"

typedef struct {
  char *name;
  char *body;
} alias_t;

static alias_t *R = NULL;
static size_t RN = 0;
static size_t RC = 0;

static int reg_dir(char *out, size_t cap) {
  if (fs_xdg_config_dir(out, cap)) return -1;
  size_t need = strlen(out) + strlen("/sysalias") + 1;
  if (need > cap) return -1;
  strcat(out, "/sysalias");
  return 0;
}

static int reg_path(char *out, size_t cap) {
  if (reg_dir(out, cap)) return -1;
  size_t need = strlen(out) + strlen("/aliases.json") + 1;
  if (need > cap) return -1;
  strcat(out, "/aliases.json");
  return 0;
}

static void reg_free(void) {
  for (size_t i = 0; i < RN; i++) { free(R[i].name); free(R[i].body); }
  free(R);
  R = NULL;
  RN = 0;
  RC = 0;
}

static int reg_push(const char *n, const char *b) {
  if (!n || !*n || !b) return -1;
  for (size_t i = 0; i < RN; i++) {
    if (strcmp(R[i].name, n) == 0) {
      char *nb = s_dup(b);
      if (!nb) return -1;
      free(R[i].body);
      R[i].body = nb;
      return 1;
    }
  }
  if (RN == RC) {
    size_t nc = RC ? RC * 2 : 8;
    alias_t *nr = realloc(R, nc * sizeof(alias_t));
    if (!nr) return -1;
    R = nr;
    RC = nc;
  }
  R[RN].name = s_dup(n);
  R[RN].body = s_dup(b);
  if (!R[RN].name || !R[RN].body) return -1;
  RN++;
  return 0;
}

static int reg_del(const char *n) {
  for (size_t i = 0; i < RN; i++) {
    if (strcmp(R[i].name, n) == 0) {
      free(R[i].name);
      free(R[i].body);
      memmove(&R[i], &R[i+1], (RN - i - 1) * sizeof(alias_t));
      RN--;
      return 0;
    }
  }
  return -1;
}

static int reg_load(void) {
  char p[1024];
  if (reg_path(p, sizeof(p))) return -1;
  char *buf = NULL;
  size_t len = 0;
  int rc = fs_read_all(p, &buf, &len);
  if (rc == -2) return 0;
  if (rc) return -1;
  char *a = strstr(buf, "\"aliases\":[");
  if (!a) { free(buf); return 0; }
  a = strchr(a, '[');
  if (!a) { free(buf); return 0; }
  char *q = a + 1;
  while (1) {
    char *o = strchr(q, '{');
    if (!o) break;
    char *c = strchr(o, '}');
    if (!c) { free(buf); return -1; }
    size_t ln = (size_t)(c - o + 1);
    char *obj = malloc(ln + 1);
    if (!obj) { free(buf); return -1; }
    memcpy(obj, o, ln); obj[ln] = 0;
    char *n = NULL;
    char *b = NULL;
    char *kn = strstr(obj, "\"name\":\"");
    if (kn) {
      kn += 8;
      char *en = strchr(kn, '"');
      if (en) { *en = 0; n = s_dup(kn); *en = '"'; }
    }
    char *kb = strstr(obj, "\"body\":\"");
    if (kb) {
      kb += 8;
      char *eb = strchr(kb, '"');
      if (eb) { *eb = 0; b = s_dup(kb); *eb = '"'; }
    }
    if (n && b) {
      if (reg_push(n, b) < 0) { free(n); free(b); free(obj); free(buf); return -1; }
      free(n);
      free(b);
    }
    free(obj);
    q = c + 1;
  }
  free(buf);
  return 0;
}

static int reg_save(void) {
  char d[1024];
  char p[1024];
  if (reg_dir(d, sizeof(d))) return -1;
  if (fs_mkdir_p(d)) return -1;
  if (reg_path(p, sizeof(p))) return -1;
  size_t cap = 128 + RN * 128;
  char *buf = malloc(cap);
  if (!buf) return -1;
  size_t off = 0;
  off += snprintf(buf + off, cap - off, "{\"aliases\":[");
  for (size_t i = 0; i < RN; i++) {
    if (i) off += snprintf(buf + off, cap - off, ",");
    off += snprintf(buf + off, cap - off, "{\"name\":\"");
    off += snprintf(buf + off, cap - off, "%s", R[i].name);
    off += snprintf(buf + off, cap - off, "\",\"body\":\"");
    off += snprintf(buf + off, cap - off, "%s", R[i].body);
    off += snprintf(buf + off, cap - off, "\",\"shell\":\"any\",\"type\":\"alias\"}");
    if (off + 64 > cap) {
      cap *= 2;
      char *nb = realloc(buf, cap);
      if (!nb) { free(buf); return -1; }
      buf = nb;
    }
  }
  off += snprintf(buf + off, cap - off, "]}");
  int rc = fs_atomic_write(p, buf, off);
  free(buf);
  return rc;
}

int registry_add(const char *kv) {
  if (!kv) return -1;
  char *name = NULL;
  char *body = NULL;
  if (s_split_kv(kv, &name, &body)) return -1;
  int rc = reg_load();
  if (rc) { free(name); free(body); return -1; }
  rc = reg_push(name, body);
  if (rc < 0) { free(name); free(body); reg_free(); return -1; }
  rc = reg_save();
  free(name);
  free(body);
  reg_free();
  if (rc) return -1;
  return 0;
}

int registry_rm(const char *name) {
  if (!name || !*name) return -1;
  int rc = reg_load();
  if (rc) return -1;
  rc = reg_del(name);
  if (rc) { reg_free(); fprintf(stderr, "Alias not found\n"); return 2; }
  rc = reg_save();
  reg_free();
  if (rc) return -1;
  return 0;
}

int registry_list(void) {
  int rc = reg_load();
  if (rc) return -1;
  for (size_t i = 0; i < RN; i++) printf("%s=%s\n", R[i].name, R[i].body);
  reg_free();
  return 0;
}
