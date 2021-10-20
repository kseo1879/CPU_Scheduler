/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_EVENTPOLL_H
#define _LINUX_EVENTPOLL_H

#include <uapi/linux/eventpoll.h>
#include <uapi/linux/kcmp.h>



struct file;


#ifdef CONFIG_EPOLL

#ifdef CONFIG_KCMP
struct file *get_epoll_tfile_raw_ptr(struct file *file, int tfd, unsigned long toff);
#endif


static inline void eventpoll_init_file(struct file *file)
{
	INIT_LIST_HEAD(&file->f_ep_links);
	INIT_LIST_HEAD(&file->f_tfile_llink);
}



void eventpoll_release_file(struct file *file);


static inline void eventpoll_release(struct file *file)
{

	
	if (likely(list_empty(&file->f_ep_links)))
		return;

	
	eventpoll_release_file(file);
}

int do_epoll_ctl(int epfd, int op, int fd, struct epoll_event *epds,
		 bool nonblock);


static inline int ep_op_has_event(int op)
{
	return op != EPOLL_CTL_DEL;
}

#else

static inline void eventpoll_init_file(struct file *file) {}
static inline void eventpoll_release(struct file *file) {}

#endif

#endif 
