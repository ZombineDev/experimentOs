
#pragma once

#include <common/basic_defs.h>
#include <exos/list.h>
#include <exos/paging.h>
#include <exos/irq.h>
#include <exos/hal.h>
#include <exos/sync.h>
#include <exos/fs/exvfs.h>

// This is the biggest usermode addr + 1
#define OFFLIMIT_USERMODE_ADDR 0xC0000000UL

#define KTHREAD_STACK_SIZE (PAGE_SIZE)

typedef enum {
   TASK_STATE_RUNNABLE = 0,
   TASK_STATE_RUNNING = 1,
   TASK_STATE_SLEEPING = 2,
   TASK_STATE_ZOMBIE = 3
} task_state_enum;

struct task_info {

   list_node list;
   list_node runnable_list;
   list_node sleeping_list;

   int pid; /* global user/kernel thread identifier */
   int owning_process_pid; /* The pid of the process owning this thread. */
   int parent_pid;

   task_state_enum state;
   u8 exit_status;
   bool running_in_kernel;

   u64 ticks;
   u64 total_ticks;
   u64 kernel_ticks;

   void *kernel_stack;

   wait_obj wobj;

   regs state_regs;
   regs kernel_state_regs;
   page_directory_t *pdir;

   char cwd[256]; /* current working directory */

   /* Handles */
   fs_handle handles[16]; /* for the moment, just a fixed-size small array */
};

typedef struct task_info task_info;

extern volatile u64 jiffies;
extern int current_max_pid;
extern task_info *current;

extern list_node tasks_list;
extern list_node runnable_tasks_list;
extern list_node sleeping_tasks_list;

static ALWAYS_INLINE bool running_in_kernel(task_info *t)
{
   return t->running_in_kernel;
}

static ALWAYS_INLINE u64 get_ticks()
{
   return jiffies;
}

static ALWAYS_INLINE task_info *get_current_task()
{
   return current;
}

void save_current_task_state(regs *);

void account_ticks();
bool need_reschedule();

NORETURN void switch_to_task(task_info *ti);

void schedule(void);
void schedule_outside_interrupt_context();

NORETURN void switch_to_idle_task_outside_interrupt_context();

task_info *create_usermode_task(page_directory_t *pdir,
                                void *entry,
                                void *stack_addr,
                                task_info *task_to_use,
                                char *const *argv,
                                char *const *env);

bool is_kernel_thread(task_info *ti);

void set_current_task_in_kernel();
void set_current_task_in_user_mode();

task_info *get_task(int pid);

void task_info_reset_kernel_stack(task_info *ti);

void add_task(task_info *ti);
void remove_task(task_info *ti);

task_info *get_current_task();
void initialize_scheduler(void);

void task_change_state(task_info *ti, task_state_enum new_state);

typedef void (*kthread_func_ptr)();

task_info *kthread_create(kthread_func_ptr fun, void *arg);

// It is called when each kernel thread returns. May be called explicitly too.
void kthread_exit();

/*
 * Saves the current state and calls schedule().
 * That after, typically after some time, the scheduler will restore the thread
 * as if kernel_yield() returned and nothing else happened.
 */

void kernel_yield();

void kernel_sleep(u64 ticks);

int set_task_to_wake_after(task_info *task, u64 ticks);
void cancel_timer(int timer_num);


// TODO: consider moving these functions and the sched ones in sched.h

extern volatile u32 disable_preemption_count;

static ALWAYS_INLINE void disable_preemption(void) {
   disable_preemption_count++;
}

static ALWAYS_INLINE void enable_preemption(void) {
   ASSERT(disable_preemption_count > 0);
   disable_preemption_count--;
}

static ALWAYS_INLINE bool is_preemption_enabled(void) {
   return disable_preemption_count == 0;
}