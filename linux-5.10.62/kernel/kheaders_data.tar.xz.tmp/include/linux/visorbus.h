// SPDX-License-Identifier: GPL-2.0+




#ifndef __VISORBUS_H__
#define __VISORBUS_H__

#include <linux/device.h>

#define VISOR_CHANNEL_SIGNATURE ('L' << 24 | 'N' << 16 | 'C' << 8 | 'E')


enum channel_serverstate {
	CHANNELSRV_UNINITIALIZED = 0,
	CHANNELSRV_READY = 1
};


enum channel_clientstate {
	CHANNELCLI_DETACHED = 0,
	CHANNELCLI_DISABLED = 1,
	CHANNELCLI_ATTACHING = 2,
	CHANNELCLI_ATTACHED = 3,
	CHANNELCLI_BUSY = 4,
	CHANNELCLI_OWNED = 5
};


#define VISOR_DRIVER_ENABLES_INTS (0x1ULL << 1)
#define VISOR_CHANNEL_IS_POLLING (0x1ULL << 3)
#define VISOR_IOVM_OK_DRIVER_DISABLING_INTS (0x1ULL << 4)
#define VISOR_DRIVER_DISABLES_INTS (0x1ULL << 5)
#define VISOR_DRIVER_ENHANCED_RCVBUF_CHECKING (0x1ULL << 6)


struct channel_header {
	u64 signature;
	u32 legacy_state;
	
	u32 header_size;
	u64 size;
	u64 features;
	guid_t chtype;
	u64 partition_handle;
	u64 handle;
	u64 ch_space_offset;
	u32 version_id;
	u32 partition_index;
	guid_t zone_guid;
	u32 cli_str_offset;
	u32 cli_state_boot;
	u32 cmd_state_cli;
	u32 cli_state_os;
	u32 ch_characteristic;
	u32 cmd_state_srv;
	u32 srv_state;
	u8 cli_error_boot;
	u8 cli_error_os;
	u8 filler[1];
	u8 recover_channel;
} __packed;

#define VISOR_CHANNEL_ENABLE_INTS (0x1ULL << 0)


struct signal_queue_header {
	
	u32 version;
	u32 chtype;
	u64 size;
	u64 sig_base_offset;
	u64 features;
	u64 num_sent;
	u64 num_overflows;
	u32 signal_size;
	u32 max_slots;
	u32 max_signals;
	u32 head;
	
	u64 num_received;
	u32 tail;
	u32 reserved1;
	u64 reserved2;
	u64 client_queue;
	u64 num_irq_received;
	u64 num_empty;
	u32 errorflags;
	u8 filler[12];
} __packed;



#define VISOR_VHBA_CHANNEL_GUID \
	GUID_INIT(0x414815ed, 0xc58c, 0x11da, \
		  0x95, 0xa9, 0x0, 0xe0, 0x81, 0x61, 0x16, 0x5f)
#define VISOR_VHBA_CHANNEL_GUID_STR \
	"414815ed-c58c-11da-95a9-00e08161165f"
struct visorchipset_state {
	u32 created:1;
	u32 attached:1;
	u32 configured:1;
	u32 running:1;
	
};


struct visor_device {
	struct visorchannel *visorchannel;
	guid_t channel_type_guid;
	
	struct device device;
	struct list_head list_all;
	struct timer_list timer;
	bool timer_active;
	bool being_removed;
	struct mutex visordriver_callback_lock; 
	bool pausing;
	bool resuming;
	u32 chipset_bus_no;
	u32 chipset_dev_no;
	struct visorchipset_state state;
	guid_t inst;
	u8 *name;
	struct controlvm_message_header *pending_msg_hdr;
	void *vbus_hdr_info;
	guid_t partition_guid;
	struct dentry *debugfs_dir;
	struct dentry *debugfs_bus_info;
};

#define to_visor_device(x) container_of(x, struct visor_device, device)

typedef void (*visorbus_state_complete_func) (struct visor_device *dev,
					      int status);


struct visor_channeltype_descriptor {
	const guid_t guid;
	const char *name;
	u64 min_bytes;
	u32 version;
};


struct visor_driver {
	const char *name;
	struct module *owner;
	struct visor_channeltype_descriptor *channel_types;
	int (*probe)(struct visor_device *dev);
	void (*remove)(struct visor_device *dev);
	void (*channel_interrupt)(struct visor_device *dev);
	int (*pause)(struct visor_device *dev,
		     visorbus_state_complete_func complete_func);
	int (*resume)(struct visor_device *dev,
		      visorbus_state_complete_func complete_func);

	
	struct device_driver driver;
};

#define to_visor_driver(x) (container_of(x, struct visor_driver, driver))

int visor_check_channel(struct channel_header *ch, struct device *dev,
			const guid_t *expected_uuid, char *chname,
			u64 expected_min_bytes,	u32 expected_version,
			u64 expected_signature);

int visorbus_register_visor_driver(struct visor_driver *drv);
void visorbus_unregister_visor_driver(struct visor_driver *drv);
int visorbus_read_channel(struct visor_device *dev,
			  unsigned long offset, void *dest,
			  unsigned long nbytes);
int visorbus_write_channel(struct visor_device *dev,
			   unsigned long offset, void *src,
			   unsigned long nbytes);
int visorbus_enable_channel_interrupts(struct visor_device *dev);
void visorbus_disable_channel_interrupts(struct visor_device *dev);

int visorchannel_signalremove(struct visorchannel *channel, u32 queue,
			      void *msg);
int visorchannel_signalinsert(struct visorchannel *channel, u32 queue,
			      void *msg);
bool visorchannel_signalempty(struct visorchannel *channel, u32 queue);
const guid_t *visorchannel_get_guid(struct visorchannel *channel);

#define BUS_ROOT_DEVICE UINT_MAX
struct visor_device *visorbus_get_device_by_id(u32 bus_no, u32 dev_no,
					       struct visor_device *from);
#endif
