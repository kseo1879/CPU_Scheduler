/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __INTEL_SVM_H__
#define __INTEL_SVM_H__

struct device;

struct svm_dev_ops {
	void (*fault_cb)(struct device *dev, u32 pasid, u64 address,
			 void *private, int rwxp, int response);
};


#define SVM_REQ_READ	(1<<3)
#define SVM_REQ_WRITE	(1<<2)
#define SVM_REQ_EXEC	(1<<1)
#define SVM_REQ_PRIV	(1<<0)


#define SVM_FLAG_PRIVATE_PASID		(1<<0)


#define SVM_FLAG_SUPERVISOR_MODE	(1<<1)

#define SVM_FLAG_GUEST_MODE		(1<<2)

#define SVM_FLAG_GUEST_PASID		(1<<3)

#endif 
