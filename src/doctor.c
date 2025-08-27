#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doctor.h"
#include "fsutil.h"
#include "registry.h"

static int check_file_exists(const char *path) {
  FILE *f = fopen(path,"r");
  if (!f) return -1;
  fclose(f);
  return 0;
}

static int check_block(const char *path) {
  char *data = NULL; size_t len=0;
  int rc = fs_read_all(path,&data,&len);
  if (rc) return -1;
  int ok1 = strstr(data,"# >>> sysalias start >>>")!=NULL;
  int ok2 = strstr(data,"# <<< sysalias end <<<")!=NULL;
  free(data);
  return (ok1 && ok2) ? 0 : -1;
}

static char *dup_cstr(const char *s) {
  if (!s) return NULL;
  size_t n = strlen(s);
  char *p = (char*)malloc(n+1);
  if (!p) return NULL;
  memcpy(p,s,n+1);
  return p;
}

static int cb_dup(const char *n,const char *b,void *ud) {
  (void)b;
  char **seen = (char**)ud;
  for (int i=0; seen[i]; i++) {
    if (strcmp(seen[i],n)==0) {
      fprintf(stderr,"Duplicate alias detected: %s\n",n);
      return -1;
    }
  }
  int cnt=0; while (seen[cnt]) cnt++;
  seen[cnt]=dup_cstr(n);
  if (!seen[cnt]) return -1;
  seen[cnt+1]=NULL;
  return 0;
}

int doctor_run(void) {
  char dir[1024], path[1024]; int errors=0;
  if (fs_xdg_config_dir(dir,sizeof(dir))) { fprintf(stderr,"No XDG config dir\n"); return 1; }
  snprintf(path,sizeof(path),"%s/sysalias/aliases.json",dir);
  if (check_file_exists(path)) { fprintf(stderr,"Missing registry: %s\n",path); errors++; }
  snprintf(path,sizeof(path),"%s/sysalias/aliases.bash",dir);
  if (check_file_exists(path)) { fprintf(stderr,"Missing fragment: %s\n",path); errors++; }
  snprintf(path,sizeof(path),"%s/sysalias/aliases.zsh",dir);
  if (check_file_exists(path)) { fprintf(stderr,"Missing fragment: %s\n",path); errors++; }

  const char *home = getenv("HOME");
  if (home) {
    snprintf(path,sizeof(path),"%s/.bashrc",home);
    if (check_block(path)) { fprintf(stderr,".bashrc missing sysalias block\n"); errors++; }
    snprintf(path,sizeof(path),"%s/.zshrc",home);
    if (check_block(path)) { fprintf(stderr,".zshrc missing sysalias block\n"); errors++; }
  }

  // Check duplicates
  char *seen[256]={0};
  if (registry_iterate(cb_dup,seen)) errors++;
  for (int i=0; seen[i]; i++) free(seen[i]);

  return errors?1:0;
}
