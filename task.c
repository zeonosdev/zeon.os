#include "zeonos_h"

struct Task task_array[MAX_TASKS];
int current_task_index = -1;
int total_task_count = 0;

void task_idle(void) {
    while (1) { asm volatile("hlt"); }
}

void create_new_task(void (*entry_point)(void), const char* name) {
    if (total_task_count >= MAX_TASKS) return;
    struct Task* new_task = &task_array[total_task_count];
    new_task->id = total_task_count;
    new_task->state = 1;
    new_task->entry_function = entry_point;
    int i = 0;
    while(name[i] != '\0' && i < 31) {
        new_task->task_name[i] = name[i];
        i++;
    }
    new_task->task_name[i] = '\0';
    new_task->stack_top = &new_task->stack[STACK_SIZE - 1];
    total_task_count++;
}

void scheduler_initialize(void) {
    create_new_task(task_idle, "idle_task");
    current_task_index = 0;
    kernel_print("Scheduler: Initialized Successfully\n", COLOR_GREEN);
}

void scheduler_switch_task(void) {
    if (total_task_count == 0) return;
    current_task_index++;
    if (current_task_index >= total_task_count) {
        current_task_index = 0;
    }
}
