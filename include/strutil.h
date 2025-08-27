#pragma once
char *s_dup(const char *s);
char *s_trim(char *s);
int s_split_kv(const char *in, char **name, char **body);
char *s_escape_sh_single(const char *s);
