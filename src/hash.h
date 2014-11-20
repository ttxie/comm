#ifndef _COMM_HASH_H_
#define _COMM_HASH_H_

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 
#include <WinSock2.h>
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#define O_KEY 0x01
#define O_DATUM 0x02


/*
 * hash table node structure.
 */
struct hash_node_s {
        void *key;		/* the keyword */
        void *datum;		/* the data storage */
        struct hash_node_s *next;
};

/*
 * hash table structure.
 */
struct hash_s {
        struct hash_node_s **ht_htable;	/* hash table */
        uint32_t ht_size;			/* number of slots in hash table */
        uint32_t ht_nel;			/* number of elements in hash table */
        uint32_t ht_slots_used; 	/* number of  used slots in hash table */
        uint32_t ht_max_chain_len;	/* number of max chain len in hash table */
};

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Create hash table.
 */
struct hash_s *hash_create(uint32_t size);

/*
 * Insert "d" to the hashle.To "k" as a key.
 * mode:
 *	0: would dup
 *	1: can't dup key
 *
 * return value: 0:success 1:failure
 */
int hash_insert(struct hash_s *h, void *k, void *d, uint32_t mode);


/*
 * Searches for the entry with the specified key in the hash table.
 */
void *hash_search(struct hash_s *h, const void *k);

/*
 * Update the datum ,the node which k as the keyword.
 */
int hash_update(struct hash_s *h, const void *k, void *d);

/*
 * Remove one node where from the hashle.
 */
int hash_remove(struct hash_s *h, const void *key, char flag);

/*
 * Destroys the specified hash table.
 */
int hash_free(struct hash_s *h);
int hash_free_f(struct hash_s *h, char flag);

/*
 * Applies the specified apply function to (key,datum,args)
 * for each entry in the specified hash table.
 *
 * The order in which the function is applied to the entries
 * is dependent upon the internal structure of the hash table.
 *
 * If apply returns a non-zero status, then hash_map will cease
 * iterating through the hash table and will propagate the error
 * return to its caller.
 */
int hash_map(struct hash_s *h,
                int (*apply)(void *k, void *d, void *args),
                void *args);


/* Fill info with some hash table statistics */
void hash_stat(struct hash_s *h);

#ifdef __cplusplus
}
#endif

#endif	/* ____HASH_H_ */
