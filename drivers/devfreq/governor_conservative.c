/*
 *  governor_conservative.c
 *
 *  Copyright (C) 2013 Fluxi <linflux@arcor.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/errno.h>
#include <linux/module.h>
#include <linux/devfreq.h>
#include <linux/msm_adreno_devfreq.h>

#include "governor.h"

#define DEVFREQ_CONSERVATIVE	"conservative"

#define DEF_UPTHRESH		50
#define DEF_DOWNTHRESH		20

/*
 * CONSERVATIVENESS is a factor that is applied
 * to up/downthresholds. It will make the governor
 * scale up later and down earlier. Values over 40
 * are generally not recommended.
*/
#define DEF_CONSERVATIVENESS	0

/*
 * FLOOR is 5msec to capture up to 3 re-draws
 * per frame for 60fps content.
 */
#define FLOOR			5000

/*
 * CEILING is 50msec, larger than any standard
 * frame length.
 */
#define CEILING			50000

unsigned int upthreshold = DEF_UPTHRESH;
unsigned int downthreshold = DEF_DOWNTHRESH;
unsigned int conservativeness = DEF_CONSERVATIVENESS;

static int devfreq_conservative_func(struct devfreq *devfreq,
				     unsigned long *freq, u32 * flag)
{
	int ret;
	unsigned int level, load;
	unsigned long max = devfreq->max_freq ? devfreq->max_freq : UINT_MAX;
	struct devfreq_dev_status stat;
	struct devfreq_conservative_data *priv = devfreq->data;
	struct devfreq_dev_profile *profile = devfreq->profile;

	stat.private_data = NULL;

	ret = profile->get_dev_status(devfreq->dev.parent, &stat);
	if (ret)
		return ret;

	priv->bin.total_time += stat.total_time;
	priv->bin.busy_time += stat.busy_time;

	/* 
	 * Do not waste CPU cycles running this algorithm if
	 * the GPU just started, or if less than FLOOR time
	 * has passed since the last run.
	 */
	if (stat.total_time == 0 || priv->bin.total_time < FLOOR)
		return 0;

	/* Prevent overflow */
	if (stat.busy_time >= 1 << 24 || stat.total_time >= 1 << 24) {
		stat.busy_time >>= 7;
		stat.total_time >>= 7;
	}

	/* If current level is unknown, default to max */
	level = devfreq_get_freq_level(devfreq, stat.current_frequency);
	if (unlikely(level < 0)) {
		*freq = max;
		goto clear;
	}

	/*
	 * If there is an extended block of busy processing,
	 * increase frequency. Otherwise run the normal algorithm.
	 */
	if (priv->bin.busy_time > CEILING) {
		*freq = max;
		goto clear;
	}

	/* Apply conservativeness factor */
	if (conservativeness) {
		upthreshold = (upthreshold * (100 + conservativeness)) / 100;
		downthreshold =
		    (downthreshold * (100 + conservativeness)) / 100;
	}

	load = (100 * priv->bin.busy_time) / priv->bin.total_time;

	if (load > upthreshold)
		level = max_t(int, level - 1, 0);
	else if (load < downthreshold)
		level = min_t(int, level + 1, profile->max_state);

	*freq = profile->freq_table[level];

clear:
	priv->bin.total_time = 0;
	priv->bin.busy_time = 0;

	return ret;
}
EXPORT_SYMBOL(devfreq_conservative_func);

static int devfreq_conservative_start(struct devfreq *devfreq)
{
	if (devfreq->data == NULL) {
		pr_err("data is required for this governor\n");
		return -EINVAL;
	}

	devfreq_monitor_start(devfreq);

	return 0;
}
EXPORT_SYMBOL(devfreq_conservative_start);

static int devfreq_conservative_stop(struct devfreq *devfreq)
{
	devfreq_monitor_stop(devfreq);

	return 0;
}
EXPORT_SYMBOL(devfreq_conservative_stop);

static int devfreq_conservative_interval(struct devfreq *devfreq, void *data)
{
	devfreq_interval_update(devfreq, (unsigned int *)data);

	return 0;
}
EXPORT_SYMBOL(devfreq_conservative_interval);

static int devfreq_conservative_suspend(struct devfreq *devfreq)
{
	struct devfreq_conservative_data *priv = devfreq->data;

	devfreq_monitor_suspend(devfreq);
	priv->bin.total_time = 0;
	priv->bin.busy_time = 0;

	return 0;
}
EXPORT_SYMBOL(devfreq_conservative_suspend);

static int devfreq_conservative_resume(struct devfreq *devfreq)
{
	unsigned long freq;
	struct devfreq_dev_profile *profile = devfreq->profile;

	freq = profile->initial_freq;
	devfreq_monitor_resume(devfreq);

	return profile->target(devfreq->dev.parent, &freq, 0);
}
EXPORT_SYMBOL(devfreq_conservative_resume);

static int devfreq_conservative_handler(struct devfreq *devfreq,
					unsigned int event, void *data)
{
	switch (event) {
	case DEVFREQ_GOV_START:
		devfreq_conservative_start(devfreq);
		break;

	case DEVFREQ_GOV_STOP:
		devfreq_conservative_stop(devfreq);
		break;

	case DEVFREQ_GOV_INTERVAL:
		devfreq_conservative_interval(devfreq, data);
		break;

	case DEVFREQ_GOV_SUSPEND:
		devfreq_conservative_suspend(devfreq);
		break;

	case DEVFREQ_GOV_RESUME:
		devfreq_conservative_resume(devfreq);
		break;

	default:
		break;
	}

	return 0;
}
EXPORT_SYMBOL(devfreq_conservative_handler);

static struct devfreq_governor devfreq_conservative = {
	.name = DEVFREQ_CONSERVATIVE,
	.get_target_freq = devfreq_conservative_func,
	.event_handler = devfreq_conservative_handler,
};

static int __init devfreq_conservative_init(void)
{
	return devfreq_add_governor(&devfreq_conservative);
}
EXPORT_SYMBOL(devfreq_conservative_init);

subsys_initcall(devfreq_conservative_init);

static void __exit devfreq_conservative_exit(void)
{
	int ret;

	ret = devfreq_remove_governor(&devfreq_conservative);
	if (ret)
		pr_err("%s: failed to remove governor %d\n", __func__, ret);

	return;
}
EXPORT_SYMBOL(devfreq_conservative_exit);

module_exit(devfreq_conservative_exit);
MODULE_AUTHOR("Fluxi <linflux@arcor.de>");
MODULE_DESCRIPTION("Conservative governor for devfreq framework");
MODULE_LICENSE("GPLv2");
