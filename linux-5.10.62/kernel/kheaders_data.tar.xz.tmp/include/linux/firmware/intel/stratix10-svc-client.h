/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __STRATIX10_SVC_CLIENT_H
#define __STRATIX10_SVC_CLIENT_H


#define SVC_CLIENT_FPGA			"fpga"
#define SVC_CLIENT_RSU			"rsu"


#define SVC_STATUS_OK			0
#define SVC_STATUS_BUFFER_SUBMITTED	1
#define SVC_STATUS_BUFFER_DONE		2
#define SVC_STATUS_COMPLETED		3
#define SVC_STATUS_BUSY			4
#define SVC_STATUS_ERROR		5
#define SVC_STATUS_NO_SUPPORT		6


#define COMMAND_RECONFIG_FLAG_PARTIAL	0


#define SVC_RECONFIG_REQUEST_TIMEOUT_MS         300
#define SVC_RECONFIG_BUFFER_TIMEOUT_MS          720
#define SVC_RSU_REQUEST_TIMEOUT_MS              300

struct stratix10_svc_chan;


enum stratix10_svc_command_code {
	COMMAND_NOOP = 0,
	COMMAND_RECONFIG,
	COMMAND_RECONFIG_DATA_SUBMIT,
	COMMAND_RECONFIG_DATA_CLAIM,
	COMMAND_RECONFIG_STATUS,
	COMMAND_RSU_STATUS,
	COMMAND_RSU_UPDATE,
	COMMAND_RSU_NOTIFY,
	COMMAND_RSU_RETRY,
	COMMAND_RSU_MAX_RETRY,
	COMMAND_RSU_DCMF_VERSION,
};


struct stratix10_svc_client_msg {
	void *payload;
	size_t payload_length;
	enum stratix10_svc_command_code command;
	u64 arg[3];
};


struct stratix10_svc_command_config_type {
	u32 flags;
};


struct stratix10_svc_cb_data {
	u32 status;
	void *kaddr1;
	void *kaddr2;
	void *kaddr3;
};


struct stratix10_svc_client {
	struct device *dev;
	void (*receive_cb)(struct stratix10_svc_client *client,
			   struct stratix10_svc_cb_data *cb_data);
	void *priv;
};


struct stratix10_svc_chan
*stratix10_svc_request_channel_byname(struct stratix10_svc_client *client,
	const char *name);


void stratix10_svc_free_channel(struct stratix10_svc_chan *chan);


void *stratix10_svc_allocate_memory(struct stratix10_svc_chan *chan,
				    size_t size);


void stratix10_svc_free_memory(struct stratix10_svc_chan *chan, void *kaddr);


int stratix10_svc_send(struct stratix10_svc_chan *chan, void *msg);


void stratix10_svc_done(struct stratix10_svc_chan *chan);
#endif

