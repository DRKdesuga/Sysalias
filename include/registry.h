#pragma once
typedef int (*reg_cb)(const char *name, const char *body, void *ud);
int registry_add(const char *kv);
int registry_rm(const char *name);
int registry_list(void);
int registry_iterate(reg_cb cb, void *ud);
