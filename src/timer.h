/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 * [TIME WHEEL]
 * ref: http://www.ibm.com/developerworks/cn/linux/1308_liuming_linuxtime3/index.html
 */

#ifndef _COMM_TIMER_H_
#define _COMM_TIMER_H_

#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct timer_s;
struct timer_entry_s;
typedef void (*timer_cb)(struct timer_s *timer, struct timer_entry_s *entry);

/*
小于1小时按秒存，60*60=1H=3600
>1H按小时存，366*24=8784槽位，1小时检测一次，检测的时候将小于1个小时的数据填充到秒格内

[slot0]--> list
[slot1]--> list
[slot2]--> list
 ......
[slotn]--> list

slot is list.
*/
struct timer_entry_s{
    time_t started;
    time_t expired;
    int slot_index;
    short position;    /*0:remove 1:slots, 2:ext_slots */
    short cmdid;
    void *data;
    timer_cb callback;
    struct timer_entry_s *prev;
    struct timer_entry_s *next;
};

struct timer_s {
    time_t base_time;    
    time_t last_time;
    time_t last_hour;
    int nodes;
    struct timer_entry_s **slots;  /*second=60*60=3600*/
    struct timer_entry_s **ext_slots;  /*hour=24*366=8784*/
};

/*C++ support*/
#ifdef __cplusplus
extern "C"{
#endif

struct timer_s* timer_init();
void timer_destroy(struct timer_s *timer);

int timer_add(struct timer_s *timer, struct timer_entry_s *entry);
int timer_add_p(struct timer_s *timer, time_t expired, void *data, timer_cb callback);

void timer_remove(struct timer_s *timer, struct timer_entry_s *entry);
int timer_update(struct timer_s *timer, struct timer_entry_s *entry);

int timer_process(struct timer_s *timer);

/*C++ support*/
#ifdef __cplusplus
}
#endif

#endif

