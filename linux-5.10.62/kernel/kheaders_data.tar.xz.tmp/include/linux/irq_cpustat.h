/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __irq_cpustat_h
#define __irq_cpustat_h






#ifndef __ARCH_IRQ_STAT
DECLARE_PER_CPU_ALIGNED(irq_cpustat_t, irq_stat);	
#define __IRQ_STAT(cpu, member)	(per_cpu(irq_stat.member, cpu))
#endif


#define nmi_count(cpu)		__IRQ_STAT((cpu), __nmi_count)	

#endif	
