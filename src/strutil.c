#include "strutil.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *s_dup(const char *s)
{
    if (!s)
        return NULL;
    size_t n = strlen(s);
    char *p = malloc(n + 1);
    if (!p)
        return NULL;
    memcpy(p, s, n + 1);
    return p;
}

char *s_trim(char *s)
{
    if (!s)
        return s;
    while (isspace((unsigned char)*s))
        s++;
    if (!*s)
        return s;
    char *e = s + strlen(s) - 1;
    while (e > s && isspace((unsigned char)*e))
        *e-- = 0;
    return s;
}

int s_split_kv(const char *in, char **name, char **body)
{
    if (!in || !name || !body)
        return -1;
    const char *eq = strchr(in, '=');
    if (!eq || eq == in || eq[1] == 0)
        return -1;
    size_t ln = (size_t)(eq - in);
    size_t lb = strlen(eq + 1);
    char *n = malloc(ln + 1);
    char *b = malloc(lb + 1);
    if (!n || !b)
    {
        free(n);
        free(b);
        return -1;
    }
    memcpy(n, in, ln);
    n[ln] = 0;
    memcpy(b, eq + 1, lb + 1);
    char *nt = s_trim(n);
    char *bt = s_trim(b);
    if (nt != n)
        memmove(n, nt, strlen(nt) + 1);
    if (bt != b)
        memmove(b, bt, strlen(bt) + 1);
    *name = n;
    *body = b;
    return 0;
}

char *s_escape_sh_single(const char *s)
{
    if (!s)
        return NULL;
    size_t n = 0, i = 0;
    while (s[i])
    {
        n += (s[i] == '\'') ? 4 : 1;
        i++;
    }
    char *o = malloc(n + 3);
    if (!o)
        return NULL;
    size_t k = 0;
    i = 0;
    while (s[i])
    {
        if (s[i] == '\'')
        {
            o[k++] = '\'';
            o[k++] = '\\';
            o[k++] = '\'';
            o[k++] = '\'';
        }
        else
            o[k++] = s[i];
        i++;
    }
    o[k] = 0;
    return o;
}
