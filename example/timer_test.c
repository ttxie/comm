
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utl.h"
#include "timer.h"



int callback(struct timer_s *timer, struct timer_entry_s *entry);

struct timer_entry_s* get_entry()
{
    time_t cur = time(NULL);
    struct timer_entry_s *entry = (struct timer_entry_s*)calloc(1, sizeof(struct timer_entry_s));
    if (!entry) return NULL;
    
    entry->started = cur;
    entry->expired = cur+1;
    entry->callback = callback;


    printf("get_entry:%p, cur:%d, cur+3:%d\n", entry, entry->started, entry->expired);
    return entry;
}

int callback(struct timer_s *timer, struct timer_entry_s *entry)
{
    struct timer_entry_s *new;

    printf("timer focus! slots:%d\n", timer->slots);
    printf("callback slot:%d, prev:%p, next:%p, data:%s\n", entry->slot_index, entry->prev, entry->next, entry->data);

    timer_remove(timer, entry);
    free(entry);
    
    
   // new = get_entry();
    //timer_add(timer, new);


    return 0; 
}



int main(int argc, char **argv)
{
    struct timer_s *timer;
    struct timer_entry_s *entry;
    int i=0;

    timer = timer_init();
    if (!timer) return -1;
    
//    timer_add_p(timer, time(NULL)+4, "TT", callback);

    entry = get_entry();
    timer_add(timer, entry);

    timer_remove(timer, entry);

    entry = get_entry();
    timer_add(timer, entry);

    entry = get_entry();
    timer_add(timer, entry);
    entry->expired = time(NULL)+3;
    timer_update(timer, entry);

    while (1) {
    //while (1 && i<10) {
        timer_process(timer);

//        printf(". \n");
        usleep(50*1000);
        i++;
    }

    timer_destroy(timer);
	return 0;
}

