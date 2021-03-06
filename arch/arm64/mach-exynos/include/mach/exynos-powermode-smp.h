/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * EXYNOS - PMU(Power Management Unit) support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __EXYNOS_POWERMODE_SMP_H
#define __EXYNOS_POWERMODE_SMP_H __FILE__

#include <asm/io.h>

enum sys_powerdown {
	SYS_AFTR,
	SYS_DSTOP,
	SYS_DSTOP_PSR,
	SYS_LPD,
	SYS_LPA,
	SYS_CP_CALL,
	SYS_SLEEP,
	NUM_SYS_POWERDOWN,
	/* Not used, only for compatiblity */
	SYS_ALPA,
};

struct exynos_pmu_conf {
	void __iomem *reg;
	unsigned int val[NUM_SYS_POWERDOWN];
};

struct sfr_save {
        void __iomem    *reg;
        unsigned int    val;
};

#define SFR_SAVE(_reg)                  \
        {                               \
                .reg    = _reg,         \
        }

/* setting SFR helper function */
static inline void exynos_save_sfr(struct sfr_save *ptr, int count)
{
        for (; count > 0; count--, ptr++)
                ptr->val = __raw_readl(ptr->reg);
}

static inline void exynos_restore_sfr(struct sfr_save *ptr, int count)
{
        for (; count > 0; count--, ptr++)
                __raw_writel(ptr->val, ptr->reg);
}


/* power mode check flag */
#define EXYNOS_CHECK_SLEEP	0x00000BAD
#define EXYNOS_CHECK_CPCALL	0x000000CC

#define MAX_NUM_REGS		20

/* cpuidle profile state */
#define C2_STATE		0x1
#define LPM_STATE		0x3

#define LPM_SUB_STATE_OFFSET	24
#define LPM_SUB_STATE		(0xF << LPM_SUB_STATE_OFFSET)
#define C2_SUB_STATE_OFFSET	16
#define C2_SUB_STATE		(0xF << C2_SUB_STATE_OFFSET)
#define MAJOR_STATE		(0xF)

#define SUB_STATE_OFFSET	16
#define SUB_STATE		(0xF << SUB_STATE_OFFSET)
#define CPD_STATE		(1 << SUB_STATE_OFFSET)
#define LPC_STATE		(2 << SUB_STATE_OFFSET)

/* cpuidle profile helper function */
#ifdef CONFIG_CPU_IDLE_EXYNOS
extern void lpa_blocking_counter(int);
#else
static inline void lpa_blocking_counter(int blocker) { }
#endif

struct hw_info {
	void __iomem *addr;
	unsigned int mask;
	unsigned int val;
};

#define HW_INFO_ALL(_addr, _mask, _val) \
{					\
	.addr = _addr,			\
	.mask = _mask,			\
	.val = _val,			\
}

#define HW_INFO_ADDR(_addr)		\
{					\
	.addr = _addr,			\
}

static inline void set_hw(struct hw_info *regs, int unsigned num)
{
	unsigned int tmp;

	while (num--) {
		tmp  = __raw_readl(regs[num].addr);
		tmp &= ~(regs[num].mask);
		tmp |= regs[num].val;
		__raw_writel(tmp, regs[num].addr);
	}
}

static inline void save_hw_status(struct hw_info *regs, unsigned int num)
{
	while (num--)
		regs[num].val = __raw_readl(regs[num].addr);
}

static inline void restore_hw_status(struct hw_info *regs, unsigned int num)
{
	while (num--)
		__raw_writel(regs[num].val, regs[num].addr);
}

static inline int check_hw_status(struct hw_info *regs, unsigned int num)
{
	while (num--)
		if (__raw_readl(regs[num].addr) & regs[num].mask)
			return -EBUSY;

	return 0;
}

#if defined(CONFIG_PWM_SAMSUNG)
extern int pwm_check_enable_cnt(void);
#else
static int pwm_check_enable_cnt(void)
{
	return 0;
}
#endif

#ifdef CONFIG_SERIAL_SAMSUNG
extern void s3c24xx_serial_fifo_wait(void);
#else
static void s3c24xx_serial_fifo_wait(void)
{
}
#endif

extern int determine_lpm(void);
extern int determine_cpd(int index, int c2_index, unsigned int cpu,
			 unsigned int target_residency);
extern void wakeup_from_c2(unsigned int cpu);

/* system power down function */
extern void exynos_prepare_sys_powerdown(enum sys_powerdown mode);
extern void exynos_wakeup_sys_powerdown(enum sys_powerdown mode,
					bool early_wakeup);
extern bool exynos_sys_powerdown_enabled(void);

#endif /* __EXYNOS_POWERMODE_SMP_H */
