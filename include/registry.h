#pragma once
typedef int (*reg_cb)(const char *name, const char *body, const char *shell, void *ud);
int registry_add(const char *kv);
int registry_add_shell(const char *shell, const char *kv);
int registry_rm(const char *name);
int registry_list(void);
int registry_iterate(reg_cb cb, void *ud);
