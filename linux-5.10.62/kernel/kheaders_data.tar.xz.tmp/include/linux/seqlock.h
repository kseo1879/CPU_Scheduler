/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SEQLOCK_H
#define __LINUX_SEQLOCK_H



#include <linux/compiler.h>
#include <linux/kcsan-checks.h>
#include <linux/lockdep.h>
#include <linux/mutex.h>
#include <linux/ww_mutex.h>
#include <linux/preempt.h>
#include <linux/spinlock.h>

#include <asm/processor.h>


#define KCSAN_SEQLOCK_REGION_MAX 1000


typedef struct seqcount {
	unsigned sequence;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
} seqcount_t;

static inline void __seqcount_init(seqcount_t *s, const char *name,
					  struct lock_class_key *key)
{
	
	lockdep_init_map(&s->dep_map, name, key, 0);
	s->sequence = 0;
}

#ifdef CONFIG_DEBUG_LOCK_ALLOC

# define SEQCOUNT_DEP_MAP_INIT(lockname)				\
		.dep_map = { .name = #lockname }


# define seqcount_init(s)						\
	do {								\
		static struct lock_class_key __key;			\
		__seqcount_init((s), #s, &__key);			\
	} while (0)

static inline void seqcount_lockdep_reader_access(const seqcount_t *s)
{
	seqcount_t *l = (seqcount_t *)s;
	unsigned long flags;

	local_irq_save(flags);
	seqcount_acquire_read(&l->dep_map, 0, 0, _RET_IP_);
	seqcount_release(&l->dep_map, _RET_IP_);
	local_irq_restore(flags);
}

#else
# define SEQCOUNT_DEP_MAP_INIT(lockname)
# define seqcount_init(s) __seqcount_init(s, NULL, NULL)
# define seqcount_lockdep_reader_access(x)
#endif


#define SEQCNT_ZERO(name) { .sequence = 0, SEQCOUNT_DEP_MAP_INIT(name) }




#if defined(CONFIG_LOCKDEP) || defined(CONFIG_PREEMPT_RT)
#define __SEQ_LOCK(expr)	expr
#else
#define __SEQ_LOCK(expr)
#endif





#define seqcount_LOCKNAME_init(s, _lock, lockname)			\
	do {								\
		seqcount_##lockname##_t *____s = (s);			\
		seqcount_init(&____s->seqcount);			\
		__SEQ_LOCK(____s->lock = (_lock));			\
	} while (0)

#define seqcount_raw_spinlock_init(s, lock)	seqcount_LOCKNAME_init(s, lock, raw_spinlock)
#define seqcount_spinlock_init(s, lock)		seqcount_LOCKNAME_init(s, lock, spinlock)
#define seqcount_rwlock_init(s, lock)		seqcount_LOCKNAME_init(s, lock, rwlock);
#define seqcount_mutex_init(s, lock)		seqcount_LOCKNAME_init(s, lock, mutex);
#define seqcount_ww_mutex_init(s, lock)		seqcount_LOCKNAME_init(s, lock, ww_mutex);


#define SEQCOUNT_LOCKNAME(lockname, locktype, preemptible, lockmember, lockbase, lock_acquire) \
typedef struct seqcount_##lockname {					\
	seqcount_t		seqcount;				\
	__SEQ_LOCK(locktype	*lock);					\
} seqcount_##lockname##_t;						\
									\
static __always_inline seqcount_t *					\
__seqprop_##lockname##_ptr(seqcount_##lockname##_t *s)			\
{									\
	return &s->seqcount;						\
}									\
									\
static __always_inline unsigned						\
__seqprop_##lockname##_sequence(const seqcount_##lockname##_t *s)	\
{									\
	unsigned seq = READ_ONCE(s->seqcount.sequence);			\
									\
	if (!IS_ENABLED(CONFIG_PREEMPT_RT))				\
		return seq;						\
									\
	if (preemptible && unlikely(seq & 1)) {				\
		__SEQ_LOCK(lock_acquire);				\
		__SEQ_LOCK(lockbase##_unlock(s->lock));			\
									\
									\
		seq = READ_ONCE(s->seqcount.sequence);			\
	}								\
									\
	return seq;							\
}									\
									\
static __always_inline bool						\
__seqprop_##lockname##_preemptible(const seqcount_##lockname##_t *s)	\
{									\
	if (!IS_ENABLED(CONFIG_PREEMPT_RT))				\
		return preemptible;					\
									\
			\
	return false;							\
}									\
									\
static __always_inline void						\
__seqprop_##lockname##_assert(const seqcount_##lockname##_t *s)		\
{									\
	__SEQ_LOCK(lockdep_assert_held(lockmember));			\
}



static inline seqcount_t *__seqprop_ptr(seqcount_t *s)
{
	return s;
}

static inline unsigned __seqprop_sequence(const seqcount_t *s)
{
	return READ_ONCE(s->sequence);
}

static inline bool __seqprop_preemptible(const seqcount_t *s)
{
	return false;
}

static inline void __seqprop_assert(const seqcount_t *s)
{
	lockdep_assert_preemption_disabled();
}

#define __SEQ_RT	IS_ENABLED(CONFIG_PREEMPT_RT)

SEQCOUNT_LOCKNAME(raw_spinlock, raw_spinlock_t,  false,    s->lock,        raw_spin, raw_spin_lock(s->lock))
SEQCOUNT_LOCKNAME(spinlock,     spinlock_t,      __SEQ_RT, s->lock,        spin,     spin_lock(s->lock))
SEQCOUNT_LOCKNAME(rwlock,       rwlock_t,        __SEQ_RT, s->lock,        read,     read_lock(s->lock))
SEQCOUNT_LOCKNAME(mutex,        struct mutex,    true,     s->lock,        mutex,    mutex_lock(s->lock))
SEQCOUNT_LOCKNAME(ww_mutex,     struct ww_mutex, true,     &s->lock->base, ww_mutex, ww_mutex_lock(s->lock, NULL))



#define SEQCOUNT_LOCKNAME_ZERO(seq_name, assoc_lock) {			\
	.seqcount		= SEQCNT_ZERO(seq_name.seqcount),	\
	__SEQ_LOCK(.lock	= (assoc_lock))				\
}

#define SEQCNT_RAW_SPINLOCK_ZERO(name, lock)	SEQCOUNT_LOCKNAME_ZERO(name, lock)
#define SEQCNT_SPINLOCK_ZERO(name, lock)	SEQCOUNT_LOCKNAME_ZERO(name, lock)
#define SEQCNT_RWLOCK_ZERO(name, lock)		SEQCOUNT_LOCKNAME_ZERO(name, lock)
#define SEQCNT_MUTEX_ZERO(name, lock)		SEQCOUNT_LOCKNAME_ZERO(name, lock)
#define SEQCNT_WW_MUTEX_ZERO(name, lock) 	SEQCOUNT_LOCKNAME_ZERO(name, lock)

#define __seqprop_case(s, lockname, prop)				\
	seqcount_##lockname##_t: __seqprop_##lockname##_##prop((void *)(s))

#define __seqprop(s, prop) _Generic(*(s),				\
	seqcount_t:		__seqprop_##prop((void *)(s)),		\
	__seqprop_case((s),	raw_spinlock,	prop),			\
	__seqprop_case((s),	spinlock,	prop),			\
	__seqprop_case((s),	rwlock,		prop),			\
	__seqprop_case((s),	mutex,		prop),			\
	__seqprop_case((s),	ww_mutex,	prop))

#define __seqcount_ptr(s)		__seqprop(s, ptr)
#define __seqcount_sequence(s)		__seqprop(s, sequence)
#define __seqcount_lock_preemptible(s)	__seqprop(s, preemptible)
#define __seqcount_assert_lock_held(s)	__seqprop(s, assert)


#define __read_seqcount_begin(s)					\
({									\
	unsigned seq;							\
									\
	while ((seq = __seqcount_sequence(s)) & 1)			\
		cpu_relax();						\
									\
	kcsan_atomic_next(KCSAN_SEQLOCK_REGION_MAX);			\
	seq;								\
})


#define raw_read_seqcount_begin(s)					\
({									\
	unsigned seq = __read_seqcount_begin(s);			\
									\
	smp_rmb();							\
	seq;								\
})


#define read_seqcount_begin(s)						\
({									\
	seqcount_lockdep_reader_access(__seqcount_ptr(s));		\
	raw_read_seqcount_begin(s);					\
})


#define raw_read_seqcount(s)						\
({									\
	unsigned seq = __seqcount_sequence(s);				\
									\
	smp_rmb();							\
	kcsan_atomic_next(KCSAN_SEQLOCK_REGION_MAX);			\
	seq;								\
})


#define raw_seqcount_begin(s)						\
({									\
									\
	raw_read_seqcount(s) & ~1;					\
})


#define __read_seqcount_retry(s, start)					\
	__read_seqcount_t_retry(__seqcount_ptr(s), start)

static inline int __read_seqcount_t_retry(const seqcount_t *s, unsigned start)
{
	kcsan_atomic_next(0);
	return unlikely(READ_ONCE(s->sequence) != start);
}


#define read_seqcount_retry(s, start)					\
	read_seqcount_t_retry(__seqcount_ptr(s), start)

static inline int read_seqcount_t_retry(const seqcount_t *s, unsigned start)
{
	smp_rmb();
	return __read_seqcount_t_retry(s, start);
}


#define raw_write_seqcount_begin(s)					\
do {									\
	if (__seqcount_lock_preemptible(s))				\
		preempt_disable();					\
									\
	raw_write_seqcount_t_begin(__seqcount_ptr(s));			\
} while (0)

static inline void raw_write_seqcount_t_begin(seqcount_t *s)
{
	kcsan_nestable_atomic_begin();
	s->sequence++;
	smp_wmb();
}


#define raw_write_seqcount_end(s)					\
do {									\
	raw_write_seqcount_t_end(__seqcount_ptr(s));			\
									\
	if (__seqcount_lock_preemptible(s))				\
		preempt_enable();					\
} while (0)

static inline void raw_write_seqcount_t_end(seqcount_t *s)
{
	smp_wmb();
	s->sequence++;
	kcsan_nestable_atomic_end();
}


#define write_seqcount_begin_nested(s, subclass)			\
do {									\
	__seqcount_assert_lock_held(s);					\
									\
	if (__seqcount_lock_preemptible(s))				\
		preempt_disable();					\
									\
	write_seqcount_t_begin_nested(__seqcount_ptr(s), subclass);	\
} while (0)

static inline void write_seqcount_t_begin_nested(seqcount_t *s, int subclass)
{
	raw_write_seqcount_t_begin(s);
	seqcount_acquire(&s->dep_map, subclass, 0, _RET_IP_);
}


#define write_seqcount_begin(s)						\
do {									\
	__seqcount_assert_lock_held(s);					\
									\
	if (__seqcount_lock_preemptible(s))				\
		preempt_disable();					\
									\
	write_seqcount_t_begin(__seqcount_ptr(s));			\
} while (0)

static inline void write_seqcount_t_begin(seqcount_t *s)
{
	write_seqcount_t_begin_nested(s, 0);
}


#define write_seqcount_end(s)						\
do {									\
	write_seqcount_t_end(__seqcount_ptr(s));			\
									\
	if (__seqcount_lock_preemptible(s))				\
		preempt_enable();					\
} while (0)

static inline void write_seqcount_t_end(seqcount_t *s)
{
	seqcount_release(&s->dep_map, _RET_IP_);
	raw_write_seqcount_t_end(s);
}


#define raw_write_seqcount_barrier(s)					\
	raw_write_seqcount_t_barrier(__seqcount_ptr(s))

static inline void raw_write_seqcount_t_barrier(seqcount_t *s)
{
	kcsan_nestable_atomic_begin();
	s->sequence++;
	smp_wmb();
	s->sequence++;
	kcsan_nestable_atomic_end();
}


#define write_seqcount_invalidate(s)					\
	write_seqcount_t_invalidate(__seqcount_ptr(s))

static inline void write_seqcount_t_invalidate(seqcount_t *s)
{
	smp_wmb();
	kcsan_nestable_atomic_begin();
	s->sequence+=2;
	kcsan_nestable_atomic_end();
}


typedef struct {
	seqcount_t seqcount;
} seqcount_latch_t;


#define SEQCNT_LATCH_ZERO(seq_name) {					\
	.seqcount		= SEQCNT_ZERO(seq_name.seqcount),	\
}


#define seqcount_latch_init(s) seqcount_init(&(s)->seqcount)


static inline unsigned raw_read_seqcount_latch(const seqcount_latch_t *s)
{
	
	return READ_ONCE(s->seqcount.sequence);
}


static inline int
read_seqcount_latch_retry(const seqcount_latch_t *s, unsigned start)
{
	return read_seqcount_retry(&s->seqcount, start);
}


static inline void raw_write_seqcount_latch(seqcount_latch_t *s)
{
	smp_wmb();	
	s->seqcount.sequence++;
	smp_wmb();      
}


typedef struct {
	
	seqcount_spinlock_t seqcount;
	spinlock_t lock;
} seqlock_t;

#define __SEQLOCK_UNLOCKED(lockname)					\
	{								\
		.seqcount = SEQCNT_SPINLOCK_ZERO(lockname, &(lockname).lock), \
		.lock =	__SPIN_LOCK_UNLOCKED(lockname)			\
	}


#define seqlock_init(sl)						\
	do {								\
		spin_lock_init(&(sl)->lock);				\
		seqcount_spinlock_init(&(sl)->seqcount, &(sl)->lock);	\
	} while (0)


#define DEFINE_SEQLOCK(sl) \
		seqlock_t sl = __SEQLOCK_UNLOCKED(sl)


static inline unsigned read_seqbegin(const seqlock_t *sl)
{
	unsigned ret = read_seqcount_begin(&sl->seqcount);

	kcsan_atomic_next(0);  
	kcsan_flat_atomic_begin();
	return ret;
}


static inline unsigned read_seqretry(const seqlock_t *sl, unsigned start)
{
	
	kcsan_flat_atomic_end();

	return read_seqcount_retry(&sl->seqcount, start);
}




static inline void write_seqlock(seqlock_t *sl)
{
	spin_lock(&sl->lock);
	write_seqcount_t_begin(&sl->seqcount.seqcount);
}


static inline void write_sequnlock(seqlock_t *sl)
{
	write_seqcount_t_end(&sl->seqcount.seqcount);
	spin_unlock(&sl->lock);
}


static inline void write_seqlock_bh(seqlock_t *sl)
{
	spin_lock_bh(&sl->lock);
	write_seqcount_t_begin(&sl->seqcount.seqcount);
}


static inline void write_sequnlock_bh(seqlock_t *sl)
{
	write_seqcount_t_end(&sl->seqcount.seqcount);
	spin_unlock_bh(&sl->lock);
}


static inline void write_seqlock_irq(seqlock_t *sl)
{
	spin_lock_irq(&sl->lock);
	write_seqcount_t_begin(&sl->seqcount.seqcount);
}


static inline void write_sequnlock_irq(seqlock_t *sl)
{
	write_seqcount_t_end(&sl->seqcount.seqcount);
	spin_unlock_irq(&sl->lock);
}

static inline unsigned long __write_seqlock_irqsave(seqlock_t *sl)
{
	unsigned long flags;

	spin_lock_irqsave(&sl->lock, flags);
	write_seqcount_t_begin(&sl->seqcount.seqcount);
	return flags;
}


#define write_seqlock_irqsave(lock, flags)				\
	do { flags = __write_seqlock_irqsave(lock); } while (0)


static inline void
write_sequnlock_irqrestore(seqlock_t *sl, unsigned long flags)
{
	write_seqcount_t_end(&sl->seqcount.seqcount);
	spin_unlock_irqrestore(&sl->lock, flags);
}


static inline void read_seqlock_excl(seqlock_t *sl)
{
	spin_lock(&sl->lock);
}


static inline void read_sequnlock_excl(seqlock_t *sl)
{
	spin_unlock(&sl->lock);
}


static inline void read_seqlock_excl_bh(seqlock_t *sl)
{
	spin_lock_bh(&sl->lock);
}


static inline void read_sequnlock_excl_bh(seqlock_t *sl)
{
	spin_unlock_bh(&sl->lock);
}


static inline void read_seqlock_excl_irq(seqlock_t *sl)
{
	spin_lock_irq(&sl->lock);
}


static inline void read_sequnlock_excl_irq(seqlock_t *sl)
{
	spin_unlock_irq(&sl->lock);
}

static inline unsigned long __read_seqlock_excl_irqsave(seqlock_t *sl)
{
	unsigned long flags;

	spin_lock_irqsave(&sl->lock, flags);
	return flags;
}


#define read_seqlock_excl_irqsave(lock, flags)				\
	do { flags = __read_seqlock_excl_irqsave(lock); } while (0)


static inline void
read_sequnlock_excl_irqrestore(seqlock_t *sl, unsigned long flags)
{
	spin_unlock_irqrestore(&sl->lock, flags);
}


static inline void read_seqbegin_or_lock(seqlock_t *lock, int *seq)
{
	if (!(*seq & 1))	
		*seq = read_seqbegin(lock);
	else			
		read_seqlock_excl(lock);
}


static inline int need_seqretry(seqlock_t *lock, int seq)
{
	return !(seq & 1) && read_seqretry(lock, seq);
}


static inline void done_seqretry(seqlock_t *lock, int seq)
{
	if (seq & 1)
		read_sequnlock_excl(lock);
}


static inline unsigned long
read_seqbegin_or_lock_irqsave(seqlock_t *lock, int *seq)
{
	unsigned long flags = 0;

	if (!(*seq & 1))	
		*seq = read_seqbegin(lock);
	else			
		read_seqlock_excl_irqsave(lock, flags);

	return flags;
}


static inline void
done_seqretry_irqrestore(seqlock_t *lock, int seq, unsigned long flags)
{
	if (seq & 1)
		read_sequnlock_excl_irqrestore(lock, flags);
}
#endif 
