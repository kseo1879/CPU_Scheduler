#ifndef _LINUX_TIMEKEEPING32_H
#define _LINUX_TIMEKEEPING32_H


static inline unsigned long get_seconds(void)
{
	return ktime_get_real_seconds();
}

#endif
