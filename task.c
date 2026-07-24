#include "zeonos.h"

struct Task task_array[MAX_TASKS];

void scheduler_initialize(void) {
    kernel_print("Scheduler: Multi-tasking Engine Ready\\n", COLOR_LIGHT_BLUE);
}

void scheduler_switch_task(void) {}
