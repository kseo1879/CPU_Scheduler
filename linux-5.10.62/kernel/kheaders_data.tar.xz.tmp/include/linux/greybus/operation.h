/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __OPERATION_H
#define __OPERATION_H

#include <linux/completion.h>
#include <linux/kref.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/workqueue.h>

struct gb_host_device;
struct gb_operation;


#define GB_OPERATION_TIMEOUT_DEFAULT	1000	


#define GB_MESSAGE_TYPE_RESPONSE	((u8)0x80)

enum gb_operation_result {
	GB_OP_SUCCESS		= 0x00,
	GB_OP_INTERRUPTED	= 0x01,
	GB_OP_TIMEOUT		= 0x02,
	GB_OP_NO_MEMORY		= 0x03,
	GB_OP_PROTOCOL_BAD	= 0x04,
	GB_OP_OVERFLOW		= 0x05,
	GB_OP_INVALID		= 0x06,
	GB_OP_RETRY		= 0x07,
	GB_OP_NONEXISTENT	= 0x08,
	GB_OP_UNKNOWN_ERROR	= 0xfe,
	GB_OP_MALFUNCTION	= 0xff,
};

#define GB_OPERATION_MESSAGE_SIZE_MIN	sizeof(struct gb_operation_msg_hdr)
#define GB_OPERATION_MESSAGE_SIZE_MAX	U16_MAX


struct gb_message {
	struct gb_operation		*operation;
	struct gb_operation_msg_hdr	*header;

	void				*payload;
	size_t				payload_size;

	void				*buffer;

	void				*hcpriv;
};

#define GB_OPERATION_FLAG_INCOMING		BIT(0)
#define GB_OPERATION_FLAG_UNIDIRECTIONAL	BIT(1)
#define GB_OPERATION_FLAG_SHORT_RESPONSE	BIT(2)
#define GB_OPERATION_FLAG_CORE			BIT(3)

#define GB_OPERATION_FLAG_USER_MASK	(GB_OPERATION_FLAG_SHORT_RESPONSE | \
					 GB_OPERATION_FLAG_UNIDIRECTIONAL)


typedef void (*gb_operation_callback)(struct gb_operation *);
struct gb_operation {
	struct gb_connection	*connection;
	struct gb_message	*request;
	struct gb_message	*response;

	unsigned long		flags;
	u8			type;
	u16			id;
	int			errno;		

	struct work_struct	work;
	gb_operation_callback	callback;
	struct completion	completion;
	struct timer_list	timer;

	struct kref		kref;
	atomic_t		waiters;

	int			active;
	struct list_head	links;		

	void			*private;
};

static inline bool
gb_operation_is_incoming(struct gb_operation *operation)
{
	return operation->flags & GB_OPERATION_FLAG_INCOMING;
}

static inline bool
gb_operation_is_unidirectional(struct gb_operation *operation)
{
	return operation->flags & GB_OPERATION_FLAG_UNIDIRECTIONAL;
}

static inline bool
gb_operation_short_response_allowed(struct gb_operation *operation)
{
	return operation->flags & GB_OPERATION_FLAG_SHORT_RESPONSE;
}

static inline bool gb_operation_is_core(struct gb_operation *operation)
{
	return operation->flags & GB_OPERATION_FLAG_CORE;
}

void gb_connection_recv(struct gb_connection *connection,
					void *data, size_t size);

int gb_operation_result(struct gb_operation *operation);

size_t gb_operation_get_payload_size_max(struct gb_connection *connection);
struct gb_operation *
gb_operation_create_flags(struct gb_connection *connection,
				u8 type, size_t request_size,
				size_t response_size, unsigned long flags,
				gfp_t gfp);

static inline struct gb_operation *
gb_operation_create(struct gb_connection *connection,
				u8 type, size_t request_size,
				size_t response_size, gfp_t gfp)
{
	return gb_operation_create_flags(connection, type, request_size,
						response_size, 0, gfp);
}

struct gb_operation *
gb_operation_create_core(struct gb_connection *connection,
				u8 type, size_t request_size,
				size_t response_size, unsigned long flags,
				gfp_t gfp);

void gb_operation_get(struct gb_operation *operation);
void gb_operation_put(struct gb_operation *operation);

bool gb_operation_response_alloc(struct gb_operation *operation,
					size_t response_size, gfp_t gfp);

int gb_operation_request_send(struct gb_operation *operation,
				gb_operation_callback callback,
				unsigned int timeout,
				gfp_t gfp);
int gb_operation_request_send_sync_timeout(struct gb_operation *operation,
						unsigned int timeout);
static inline int
gb_operation_request_send_sync(struct gb_operation *operation)
{
	return gb_operation_request_send_sync_timeout(operation,
			GB_OPERATION_TIMEOUT_DEFAULT);
}

void gb_operation_cancel(struct gb_operation *operation, int errno);
void gb_operation_cancel_incoming(struct gb_operation *operation, int errno);

void greybus_message_sent(struct gb_host_device *hd,
				struct gb_message *message, int status);

int gb_operation_sync_timeout(struct gb_connection *connection, int type,
				void *request, int request_size,
				void *response, int response_size,
				unsigned int timeout);
int gb_operation_unidirectional_timeout(struct gb_connection *connection,
				int type, void *request, int request_size,
				unsigned int timeout);

static inline int gb_operation_sync(struct gb_connection *connection, int type,
		      void *request, int request_size,
		      void *response, int response_size)
{
	return gb_operation_sync_timeout(connection, type,
			request, request_size, response, response_size,
			GB_OPERATION_TIMEOUT_DEFAULT);
}

static inline int gb_operation_unidirectional(struct gb_connection *connection,
				int type, void *request, int request_size)
{
	return gb_operation_unidirectional_timeout(connection, type,
			request, request_size, GB_OPERATION_TIMEOUT_DEFAULT);
}

static inline void *gb_operation_get_data(struct gb_operation *operation)
{
	return operation->private;
}

static inline void gb_operation_set_data(struct gb_operation *operation,
					 void *data)
{
	operation->private = data;
}

int gb_operation_init(void);
void gb_operation_exit(void);

#endif 
