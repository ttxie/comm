#include <pthread.h>
#include <string.h>
#include "utl.h"


uint64_t start, end;
pthread_mutex_t mutex;
pthread_spinlock_t spinlock;
int n=0;

volatile int lock=0;


void* consumer(void *ptr) 
{
    int m=0;
    char me[128];
    start = tm64();
    for (m=0; m<1000000; m++) {
        //while (__sync_lock_test_and_set(&lock,1)) {}
        pthread_mutex_lock(&mutex);
        //pthread_spin_lock(&spinlock);
        strcpy(me, "hello word"); 
        n++;
        //__sync_lock_release(&lock);
        pthread_mutex_unlock(&mutex);
        //pthread_spin_unlock(&spinlock);
    }
    end = tm64();
    printf("n:%d, end:%llu, start:%llu, time:%d\n", n, end, start, end-start);


}

int main()
{
    char *p=NULL;
    pthread_t thr1, thr2;

    pthread_mutex_init(&mutex, NULL);
    pthread_spin_init(&spinlock, 0);

    pthread_create(&thr1, NULL, consumer, NULL);
    pthread_create(&thr2, NULL, consumer, NULL);

    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

/*    pthread_mutex_init(&mutex, NULL);
    pthread_spin_init(&spinlock, 0);
    start = tm64();
    for (m=0; m<10000; m++) {
        //while (__sync_lock_test_and_set(&lock,1)) {}
        //pthread_mutex_lock(&mutex);
        pthread_spin_lock(&spinlock);
        n++;
        //__sync_lock_release(&lock);
        //pthread_mutex_unlock(&mutex);
        pthread_spin_unlock(&spinlock);
    }
    end = tm64();
    printf("end:%llu, start:%llu, time:%d\n", end, start, end-start);

*/

    return 0;
}
