#include "sync.h"
#include "gen_shell.h"

int sync_run(void) {
    if (gen_generate_all()) return 1;
    return 0;
}
