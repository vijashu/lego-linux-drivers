/*
 * Tacho motor helpers
 *
 * Copyright (C) 2016 David Lechner <david@lechnology.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.

 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _TACHO_MOTOR_HELPER_H
#define _TACHO_MOTOR_HELPER_H

#include <linux/ktime.h>
#include <linux/math64.h>

/* important! size must be a power of 2 */
#define BUFFER_SIZE 256

/**
 * struct tm_speed - private data
 *
 * @pos: Circular buffer that holds position readings.
 * @time: Circular buffer that holds timestamps of pos.
 * @head: Index for writing to buffers.
 * @tail: Index for reading from buffers.
 * @speed: The most recent speed value.
 *
 * The fields of this struct should not be accessed directly. Use tm_speed_get()
 * to read the speed.
 */
struct tm_speed {
	int pos[BUFFER_SIZE];
	ktime_t time[BUFFER_SIZE];
	unsigned head;
	unsigned tail;
	int speed;
};

extern void tm_speed_init(struct tm_speed *spd, int pos, ktime_t t, int count);
extern void tm_speed_update(struct tm_speed *spd, int pos, ktime_t t);
#define tm_speed_update_now(s, p) \
	tm_speed_update((s), (p), ktime_get())
#define tm_speed_get(s) ((s)->speed)
/* Use this template to implement tacho_motor_ops.get_speed */
#define TM_SPEED_GET_SPEED_FUNC(prefix, type, field) \
static int prefix##_get_speed(void *context, int *speed)	\
{								\
	struct type *data = context;				\
								\
	*speed = tm_speed_get(&data->field);			\
								\
	return 0;						\
}

struct tm_pid {
	int setpoint;
	int Kp;
	int Ki;
	int Kd;
	int integral;
	int prev_error;
	bool overloaded;
};

extern void tm_pid_reinit(struct tm_pid *pid);
extern void tm_pid_init(struct tm_pid *pid, int Kp, int Ki, int Kd);
extern int tm_pid_update(struct tm_pid *pid, int speed);
#define tm_pid_is_overloaded(pid) ((pid)->overloaded)
/*
 * Use this template to implement tacho_motor_ops.get_{speed,position}_K{p,i.d}.
 */
#define TM_PID_GET_FUNC(prefix, suffix, type, field) \
static int prefix##_get_##suffix(void *context)			\
{								\
	struct type *data = context;				\
								\
	return data->field;					\
}
/*
 * Use this template to implement tacho_motor_ops.set_{speed,position}_K{p,i.d}
 */
#define TM_PID_SET_FUNC(prefix, suffix, type, field) \
static int prefix##_set_##suffix(void *context, int value)	\
{								\
	struct type *data = context;				\
								\
	data->field = value;					\
								\
	return 0;						\
}

#endif /* _TACHO_MOTOR_HELPER_H */
