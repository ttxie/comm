/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include <math.h>
#include "timer.h"

#define SEC_SLOTS 3600
#define HOUR_SLOTS 8784



struct timer_s* timer_init()
{        
    struct timer_s *timer = NULL;
    time_t cur = time(NULL);
    do {
        timer = (struct timer_s*)calloc(1, sizeof(struct timer_s));
        if (timer == NULL) break;

        timer->base_time = cur;
        timer->last_time = cur;
        timer->last_hour = cur;
        timer->nodes = 0;
        timer->slots = (struct timer_entry_s**)calloc(SEC_SLOTS, sizeof(struct timer_entry_s*));
        if (timer->slots == NULL) break;
        timer->ext_slots = (struct timer_entry_s**)calloc(HOUR_SLOTS, sizeof(struct timer_entry_s*));
        if (timer->ext_slots == NULL) break;
        
        return timer;

    }while(0);
    
    if (timer) {
        if (timer->slots) free(timer->slots);
        if (timer->ext_slots) free(timer->ext_slots);
        free(timer);
        timer = NULL;
    }

    return NULL;
}

void timer_destroy(struct timer_s *timer)
{
    int i=0;
    struct timer_entry_s *entry, *oentry;

    if (timer) {
        if (timer->slots) {
            //free entrys
            for(i=0; i<SEC_SLOTS; i++){
                entry = timer->slots[i];
                while(entry) {
                    oentry = entry;
                    entry = oentry->next;
                    free(oentry);
                } 
            }
            free(timer->slots);
        }

        if (timer->ext_slots) {
            //free entrys    
            for(i=0; i<HOUR_SLOTS; i++){
                entry = timer->ext_slots[i];
                while(entry) {
                    oentry = entry;
                    entry = oentry->next;
                    free(oentry);
                } 
            }
            free(timer->ext_slots);
        }
        free(timer);
        timer = NULL;
    }
}

int timer_add(struct timer_s *timer, struct timer_entry_s *entry)
{
    struct timer_entry_s *head;
    time_t cur = time(NULL);
    int index=0;
    if (timer == NULL || entry == NULL) return -1;
    
    if (entry->expired <= cur) {
        entry->callback(timer, entry);
        return 0;
    }

    /*slots*/
    if ((entry->expired-cur) < SEC_SLOTS) {
        index = (entry->expired-timer->base_time)%SEC_SLOTS;
        entry->slot_index = index;
        entry->position = 1;
        head = timer->slots[index];
        timer->slots[index] = entry;
        /*has next*/
        if (head) {
            head->prev = entry; /**/
            entry->next = head;
        }
    } else { /*ext slots*/
        index = ceil((entry->expired-timer->base_time)/SEC_SLOTS);
        index = index % HOUR_SLOTS;
        //if (index > 366) return -1;
        entry->slot_index = index;
        entry->position = 2;
        head = timer->ext_slots[index];
        timer->ext_slots[index] = entry;
        /*has next*/
        if (head) {
            head->prev = entry; /**/
            entry->next = head;
        }
    }
    timer->nodes++;
    return 1;
}

int timer_add_p(struct timer_s *timer, time_t expired, void *data, timer_cb callback)
{
    struct timer_entry_s *entry = (struct timer_entry_s*)calloc(1, sizeof(struct timer_entry_s));
    time_t cur = time(NULL);

    if (timer == NULL || entry == NULL) return -1;

    entry->started = cur;
    entry->expired = expired;
    entry->callback = callback;
    entry->data = data;

    return timer_add(timer, entry);
}



void timer_remove(struct timer_s *timer, struct timer_entry_s *entry)
{
    struct timer_entry_s *next, *prev;

    if (timer == NULL || entry == NULL) return;
    //printf("entry:%p, position:%d\n", entry, entry->position);
    if (entry->position == 0) return;

    next = entry->next;
    prev = entry->prev;

    if (timer->nodes)
        timer->nodes--;

    /*ONLY ONE*/
    if (next == NULL && prev == NULL) {
        if (entry->position == 1)
            timer->slots[entry->slot_index] = NULL;
        if (entry->position == 2)
            timer->ext_slots[entry->slot_index] = NULL;

        goto END;
    }

    /*FIRST ENTRY*/
    if (prev==NULL) {
        if (entry->position == 1)
            timer->slots[entry->slot_index] = next;
        if (entry->position == 2)
            timer->ext_slots[entry->slot_index] = next;

        next->prev = NULL;
        goto END;
    }

    /*NORMAL*/
    if (prev) prev->next = next;
    if (next) next->prev = prev;

END:
    entry->prev = entry->next = NULL;
    entry->position = 0;
    return ;
}


int timer_update(struct timer_s *timer, struct timer_entry_s *entry)
{
    if (timer == NULL || entry == NULL) return -1;

    timer_remove(timer, entry);
    return timer_add(timer, entry);
}


int timer_process(struct timer_s *timer)
{
    time_t cur = time(NULL);
    struct timer_entry_s *entry, *oentry;
    int index;
    
    if (timer == NULL) return -1;

    if (timer->nodes < 1) {
        timer->last_time = cur;
        timer->last_hour = cur;
        return 0;
    }

    while (timer->last_time <= cur) {
        index = (timer->last_time-timer->base_time) % SEC_SLOTS;
        entry = timer->slots[index];
        while (entry) {
            oentry = entry;
            entry = oentry->next;
            if (oentry->expired <= cur) {
                printf("oentry:%p, slot[%d], position:%d, start_time:%d, last_time:%d, cur:%d\n", 
                        oentry, index, oentry->position, oentry->started, oentry->expired, cur);
                timer_remove(timer, oentry);
                oentry->callback(timer, oentry);
            }
            if (oentry == entry) break;
        } 

        timer->last_time++;
    }

    if ((cur-timer->last_hour) > (SEC_SLOTS/6)) {
        index = ceil((cur-timer->base_time+SEC_SLOTS)/SEC_SLOTS);
        index = index % HOUR_SLOTS;
        entry = timer->ext_slots[index];
        while (entry) {
            oentry = entry;
            entry = entry->next;
            if (oentry->expired-cur < SEC_SLOTS) {
                timer_remove(timer, oentry);
                timer_add(timer, oentry);
            }
            if (oentry == entry) break;
        }

        timer->last_hour = cur;
    }

    return 1;
}

