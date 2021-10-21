// This is the main branch
#include "linux/list.h"
#include "linux/types.h"
#include "sched.h"
#include "pelt.h"

#define TICK_COUNT 2
#define FIRST_PRIO_TICK_COUNT (TICK_COUNT)
#define SECOND_PRIO_TICK_COUNT (TICK_COUNT * 2)
#define THIRD_PRIO_TICK_COUNT (TICK_COUNT * 4)
#define PROMOTE_TICK_COUNT (TICK_COUNT * 128) //This is the global tick_count where all of the processes gets promoted

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
 */


/**
 * This will put the task into the queue base on the schedule entity number
 * This function will not only be used for enqueuing the task but also help when promoting the lower priority queues to higher priority
 */
static void put_task_queue(struct rq *rq, struct task_struct *p, int flags, int se_num) {
	struct comp3520_rq *comp3520_rq = &(rq -> comp3520);
	struct sched_comp3520_entity *task_se = &(p -> comp3520_se);
	struct sched_comp3520_entity **rq_se; //This variable will be used to store the address of the address of the shed_entity

	//se_num is refering to the priority number
	switch (se_num) {
		case 1:
			rq_se = &(comp3520_rq -> first_prio);
			break;
		case 2: 
			rq_se = &(comp3520_rq -> second_prio);
			break;
		case 3:
			rq_se = &(comp3520_rq -> third_prio);
			break;
	}

	//If there is no running task
	if (*rq_se == NULL) {
		*rq_se = task_se;
		//Since run_list is a doubly linked list we need to initialize the next and prev to it self.
		INIT_LIST_HEAD(&(task_se -> run_list));
		task_se -> on_rq = true;
	} else { // We need to add the task to the queue @param (new, list)
		list_add_tail(&(task_se -> run_list), &((*rq_se) -> run_list));
		task_se->on_rq = true;
	}
	
	//Update the running process
	rq->nr_running += 1;
	comp3520_rq->nr_running += 1;
	comp3520_rq->nr_running_queue[se_num-1] += 1;
	
	//Update the in comming task
	task_se->prio_queue_num = se_num;
	task_se->tick_count = 0;
}

/**
 * Called when a task entera a runnable stat. 
 * It puts the scheduling entity (task) into the run queue and increments the nr_running 
 * 	(number of runnable processes in a run queue variable)
 * 
 * In the multi-level feedback queue this will be used to add the item to the top priority item of the list
 */
static void enqueue_task_comp3520(struct rq *rq, struct task_struct *p, int flags) {
	//We need to add the item in the first priority queue since it's a newly comming task
	put_task_queue(rq, p, flags, 1);
}

/**
 * When a task is no longer runnable, this function is called to keep the corresponding scheduling entity out of the run queue. 
 * It also decrements the nr_running variable;
 */
static void dequeue_task_comp3520(struct rq *rq, struct task_struct *p, int flags) {
	struct comp3520_rq *comp3520_rq = &(rq -> comp3520);
	struct sched_comp3520_entity *task_se = &(p -> comp3520_se);
	struct sched_comp3520_entity **rq_se;

	//Value from 1 to 3
	int prio_queue_num = task_se -> prio_queue_num;
	switch (prio_queue_num) {
		case 1:
			rq_se = &(comp3520_rq -> first_prio);
			break;
		case 2:
			rq_se = &(comp3520_rq -> second_prio);
			break;
		case 3:
			rq_se = &(comp3520_rq -> third_prio);
			break;
	}

	//This means that the task is currently running
	if (*rq_se == task_se) {
		// Here there is also two cases where if the item there is currently only one task running on the queue vs more than one are running
		if (comp3520_rq->nr_running_queue[prio_queue_num - 1] == 1) {
			*rq_se = NULL;
		} else {
			//we need to set the comp3520 -> curr to the next task @params (ptr, type, member)
			*rq_se = list_entry(comp3520_rq->first_prio->run_list.next, struct sched_comp3520_entity, run_list);
		}
	}

	//Update the number of running tasks
	rq -> nr_running -= 1;
	comp3520_rq -> nr_running -= 1;  
	comp3520_rq -> nr_running_queue[prio_queue_num - 1] -= 1;

	//Set it to the value that is invalid
	task_se -> prio_queue_num = -1;
	task_se -> tick_count = 0;
	task_se -> on_rq = false;
	list_del_init(&(task_se -> run_list));
}

/**
 * Called when a task wants to voluntarily give up CPU, but not going out of runnable state. 
 * Basically this means a dequeue followed by an enqueue.
 */
static void yield_task_comp3520(struct rq *rq) {
	struct task_struct *task = rq -> curr;
	int prio = task -> comp3520_se.prio_queue_num;

	//Dequeue the task and enqueue it on the same previous priority level
	dequeue_task_comp3520(rq, task, 0);
	put_task_queue(rq, task, 0, prio);
};

static bool yield_to_task_comp3520(struct rq *rq, struct task_struct *p) {
	//We need to dequeue the task and add the task p to the begining
	struct comp3520_rq *comp3520_rq = &(rq -> comp3520);
	struct sched_comp3520_entity *se = &(p -> comp3520_se);
	return false;
}

/**
 * This function checks if a task that entered runnable state should preempt the currently running task. 
 * Called, for example, from wake_up_new_task(..);
 * 
 * This deosn't have to be implemented since pick next task will always return the task in the highest priority queue
 */
static void check_preempt_curr_comp3520(struct rq *rq, struct task_struct *p, int wake_flags) {
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &(p -> comp3520_se);
	return;
}

/**
 * This function chooses the most appropriate task eligible to run next.
 * Note, that this is not the same as enqueuing and dequeuing tasks;
 * In the multilevel feed back queue we need to pick the task that is in the highest level of priority
 */
struct task_struct *pick_next_task_comp3520(struct rq *rq) {
	struct comp3520_rq *comp3520_rq = &(rq -> comp3520);
	struct sched_comp3520_entity **rq_se;

	//If there is no task then it should return NULL
	if (comp3520_rq -> nr_running == 0) {
		return NULL;
	}

	int i = 0;
	for(i = 0; i < 3; i++) {
		if(comp3520_rq -> nr_running_queue[i] != 0) {
			switch(i) {
				case 0:
					rq_se = &(comp3520_rq -> first_prio);
					break;
				case 1:
					rq_se = &(comp3520_rq -> second_prio);
					break;
				case 2:
					rq_se = &(comp3520_rq -> third_prio);
					break;
			}
		}
	}
	

	struct sched_comp3520_entity *ptr = list_entry((*rq_se) -> run_list.next, struct sched_comp3520_entity, run_list);
	return list_entry(ptr, struct task_struct, comp3520_se);
}

static void put_prev_task_comp3520(struct rq *rq, struct task_struct *prev) {
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &(prev -> comp3520_se);
}

static void set_next_task_comp3520(struct rq *rq, struct task_struct *p, bool first) {
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &(p -> comp3520_se);
	return;
}

/**
 * This function will help tack_tick. 
 * IT will promote the queue as long as the tick count has exceeded the limit
 */
static void promote_queue(struct rq *rq) {
	struct comp3520_rq *comp3520_rq = &(rq -> comp3520);
	
	int i;
	if(comp3520_rq -> second_prio != NULL) {
		for(i = 0; i < comp3520_rq -> nr_running_queue[1]; i ++) {
			struct task_struct *ptr = list_entry(comp3520_rq -> second_prio, struct task_struct, comp3520_se);
			dequeue_task_comp3520(rq, ptr, 0);
			enqueue_task_comp3520(rq, ptr, 0);
		}
	}

	int j;
	if(comp3520_rq -> third_prio != NULL) {
		for(j = 0; j < comp3520_rq -> nr_running_queue[2]; j ++) {
			struct task_struct *ptr = list_entry(comp3520_rq -> third_prio, struct task_struct, comp3520_se);
			dequeue_task_comp3520(rq, ptr, 0);
			enqueue_task_comp3520(rq, ptr, 0);
		}
	}
	comp3520_rq -> global_tick_count = 0;
	return;
}

/**
 * Mostly called from time tick function
 * it might lead to process switch.
 * This drives the running preemption
 */
static void task_tick_comp3520(struct rq *rq, struct task_struct *curr, int queued) {
	struct comp3520_rq *comp3520_rq = &(rq -> comp3520);
	struct sched_comp3520_entity *task_se = &(curr -> comp3520_se);
	struct sched_comp3520_entity **rq_se;

	//Pick the current priority queue
	int prio_queue_num = task_se -> prio_queue_num;
	int allowed_tick_count = 0;
	int next_queue_num = -1;

	switch (prio_queue_num) {
		case 1:
			rq_se = &(comp3520_rq -> first_prio);
			allowed_tick_count = FIRST_PRIO_TICK_COUNT;
			next_queue_num = 2;
			break;
		case 2:
			rq_se = &(comp3520_rq -> second_prio);
			allowed_tick_count = SECOND_PRIO_TICK_COUNT;
			next_queue_num = 3;
			break;
		case 3:
			rq_se = &(comp3520_rq -> third_prio);
			allowed_tick_count = THIRD_PRIO_TICK_COUNT;
			next_queue_num = 3; //Since there is no more queue that has lower priority
			break;
	}

	//Increments the tickcount for the tasks
	comp3520_rq -> global_tick_count ++;
	task_se -> tick_count ++;

	if(comp3520_rq -> global_tick_count >= PROMOTE_TICK_COUNT) {
		promote_queue(rq);
		return;
	}

	if(task_se -> tick_count >= allowed_tick_count) {
		dequeue_task_comp3520(rq, curr, queued);
		put_task_queue(rq, curr, queued, next_queue_num);
		resched_curr(rq);
	}
}

/**
 * Notify the scheduler tha ta new task was spawned
 */
static void task_fork_comp3520(struct task_struct *p) {
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &(p -> comp3520_se);
	return;
}

static void prio_changed_comp3520(struct rq *rq, struct task_struct *p, int oldprio) {
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &(p -> comp3520_se);
}

static void switched_from_comp3520(struct rq *rq, struct task_struct *p) {
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &(p -> comp3520_se);
}

static void switched_to_comp3520(struct rq *rq, struct task_struct *p) {
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &(p -> comp3520_se);
}

/**
 * 	if (rq->cfs.load.weight)
 * 		rr_interval = NS_TO_JIFFIES(sched_slice(cfs_rq_of(se), se));
 * this is what get_rr_interval_comp3520 returns. It is returning Jifflies. for example is HZ is 1000 in Jifflies it is 0.001 second.  
 * In the multilayer feedback queue it will return the currently running tasks run queue's second. 
 */
static unsigned int get_rr_interval_comp3520(struct rq *rq, struct task_struct *task) {
	//This will return the allowed tickcount for the current queue
	int queue_num = task -> comp3520_se.prio_queue_num;
	switch (queue_num) {
		case 1:
			return FIRST_PRIO_TICK_COUNT;
		case 2:
			return SECOND_PRIO_TICK_COUNT;
		case 3:
			return THIRD_PRIO_TICK_COUNT;
	}
	return 1 / HZ;
}

static void update_curr_comp3520(struct rq *rq) {
	return;
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
	//Initialize the runqueue
	comp3520_rq->nr_running = 0;
	comp3520_rq->nr_running_queue[0] = 0;
	comp3520_rq->nr_running_queue[1] = 0;
	comp3520_rq->nr_running_queue[2] = 0;
	comp3520_rq->first_prio = NULL;
	comp3520_rq->second_prio = NULL;
	comp3520_rq->third_prio = NULL;
	comp3520_rq->global_tick_count = 0;
	// Don't forget to initialize the list comp3520 list
}

#ifdef CONFIG_SCHED_DEBUG
extern void print_comp3520_stats(struct seq_file *m, int cpu);
extern void print_comp3520_rq(struct seq_file *m, int cpu, struct rt_rq *rt_rq);
#endif
