//This is the main branch
#include "linux/list.h"
#include "linux/types.h"
#include "sched.h"
#include "pelt.h"

const struct sched_class comp3520_sched_class;

/**
 * Run Queue <kernel/sched/sched.h>
 * nr_running: total number of processes on the runqueue, including all scheduling classes;
 * load:provides a measure for the current load of the runqueue. It is essentially proportional to the amount of processes on the queue with each process being weighted by its priority;
 * cpu_load: keeps history of runqueue load in the past
 * cfs and rt: embedded runqueue for CFS and real-time schedulers, respectively.
 * curr: a pointer to the process descriptor of currently running task
 * idle: a pointer to an idle thread-the process that starts if there is nothing to run;
 * clock: a per-runqueue clock
 * /

// TODO: Complete me
/* 
Called when a task entera a runnable stat. 
It puts the scheduling entity (task) into the run queue and increments the nr_running 
	(number of runnable processes in a run queue variable)
*/
static void enqueue_task_comp3520(struct rq *rq, struct task_struct *p,
				  int flags)
{
	struct comp3520_rq comp3520_rq = rq->comp3520;
	struct sched_comp3520_entity *se = &p->comp3520_se;

	struct sched_comp3520_entity *new_sched_entity = 

	add_nr_running(comp3520_rq, 1);

	//hello main
}

// TODO: Complete me
/*
When a task is no longer runnable, this function is called to keep the corresponding scheduling entity out of the run queue. 
It also decrements the nr_running variable;
*/
static void dequeue_task_comp3520(struct rq *rq, struct task_struct *p,
				  int flags)
{
	struct comp3520_rq *comp3520_rq = &rq->comp3520;
	struct sched_comp3520_entity *se = &p->comp3520_se;
}

// TODO: Complete me
/*
Called when a task wants to voluntarily give up CPU, but not going out of runnable state. 
Basically this means a dequeue followed by an enqueue.
*/
static void yield_task_comp3520(struct rq *rq){};

// TODO: Complete me
static bool yield_to_task_comp3520(struct rq *rq, struct task_struct *p)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;

	return false;
}

// TODO: Complete me
/*
This function checks if a task that entered runnable state should preempt the currently running task. 
Called, for example, from wake_up_new_task(..);
*/
static void check_preempt_curr_comp3520(struct rq *rq, struct task_struct *p,
					int wake_flags)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;
}

// TODO: Complete me
/*
This function chooses the most appropriate task eligible to run next.
Note, that this is not the same as enqueuing and dequeuing tasks;
*/
struct task_struct *pick_next_task_comp3520(struct rq *rq)
{
	return NULL;
}

// TODO: Complete me
static void put_prev_task_comp3520(struct rq *rq, struct task_struct *prev)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &prev->comp3520_se;
}

// TODO: Complete me
static void set_next_task_comp3520(struct rq *rq, struct task_struct *p,
				   bool first)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;
}

// TODO: Complete me
/*
Mostly called from time tick function;
it might lead to process witch.
This drives the running preemption
*/
static void task_tick_comp3520(struct rq *rq, struct task_struct *curr,
			       int queued)
{
}

// TODO: Complete me
/*
Notify the scheduler tha ta new task was spawned
*/
static void task_fork_comp3520(struct task_struct *p)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;
}
// TODO: Complete me
static void prio_changed_comp3520(struct rq *rq, struct task_struct *p,
				  int oldprio)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;
}

// TODO: Complete me
static void switched_from_comp3520(struct rq *rq, struct task_struct *p)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;
}

// TODO: Complete me
static void switched_to_comp3520(struct rq *rq, struct task_struct *p)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;
}

// TODO: Complete me
static unsigned int get_rr_interval_comp3520(struct rq *rq,
					     struct task_struct *task)
{
	return 0;
}

// TODO: Complete me
static void update_curr_comp3520(struct rq *rq)
{
}

const struct sched_class
	comp3520_sched_class __section("__comp3520_sched_class") = {
		.enqueue_task = enqueue_task_comp3520,
		.dequeue_task = dequeue_task_comp3520,
		.yield_task = yield_task_comp3520,
		.yield_to_task = yield_to_task_comp3520,

		.check_preempt_curr = check_preempt_curr_comp3520,

		.pick_next_task = pick_next_task_comp3520,
		.put_prev_task = put_prev_task_comp3520,
		.set_next_task = set_next_task_comp3520,

#ifdef CONFIG_SMP
		.balance = balance_comp3520,
		.select_task_rq = select_task_rq_comp3520,
		.migrate_task_rq = migrate_task_rq_comp3520,

		.rq_online = rq_online_comp3520,
		.rq_offline = rq_offline_comp3520,

		.task_dead = task_dead_comp3520,
		.set_cpus_allowed = set_cpus_allowed_common,
#endif

		.task_tick = task_tick_comp3520,
		.task_fork = task_fork_comp3520,

		.prio_changed = prio_changed_comp3520,
		.switched_from = switched_from_comp3520,
		.switched_to = switched_to_comp3520,

		.get_rr_interval = get_rr_interval_comp3520,

		.update_curr = update_curr_comp3520,

#ifdef CONFIG_UCLAMP_TASK
		.uclamp_enabled = 1,
#endif

	};

// TODO: Complete me
void init_comp3520_rq(struct comp3520_rq *comp3520_rq)
{
	comp3520_rq->nr_running = 0;
	comp3520_rq->curr = NULL;
	// Don't forget to initialize the list comp3520 list
}

#ifdef CONFIG_SCHED_DEBUG
extern void print_comp3520_stats(struct seq_file *m, int cpu);
extern void print_comp3520_rq(struct seq_file *m, int cpu, struct rt_rq *rt_rq);
#endif
