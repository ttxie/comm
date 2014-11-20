/*
 * Implementation of the hash table type.
 *
 * Date : 2011-07-28
 *
 * 原始代码来自：security/selinux/ss/hash.c
 * hash参考：http://www.cppblog.com/bellgrade/archive/2009/10/06/97926.html
 *  
 * 
 *  hash->ht_htable->[slot0|slot1|slot2|...|slotn]
 *
 *  [slot0]--->struct(hash_node)->next--->struct(hash_node)   有序存放
 *  [slot1]--->struct(hash_node)
 *  ...
 *  [slotn]
 *  
 */

#include "hash.h"

#define HASHTAB_MAX_NODES	0xFFFFFFFF

/* 
   hash值生成算法(BKDR hash)
   key的长度在20以内bkdr效率更高
 */
static uint32_t bkdr_hash(u_char *key)
{

        uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
        uint32_t hash = 0;

        while (*key) {
                hash = hash * seed + (*key++);
        }

        return (hash & 0x7FFFFFFF);
}

/*

   code from :
http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash2.cpp

// MurmurHash2, by Austin Appleby

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

 */
uint32_t murmur2_hash (u_char *data, size_t len, uint32_t seed )
{
        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.
        const uint32_t m = 0x5bd1e995;
        const uint32_t r = 24;

        // Initialize the hash to a 'random' value
        uint32_t h = seed ^ len; 

        // Mix 4 bytes at a time into the hash
        while(len >= 4) {
                uint32_t k = *(uint32_t*)data;

                k *= m;
                k ^= k >> r;
                k *= m;

                h *= m;
                h ^= k;

                data += 4;
                len -= 4;
        }

        // Handle the last few bytes of the input array
        switch(len) {
                case 3: h ^= data[2] << 16;
                case 2: h ^= data[1] << 8;
                case 1: h ^= data[0];
                        h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
} 



/* 
   hash值生成
 */
static uint32_t  hash_value(const void *key)
{
        u_char *data;

        data = (u_char*)key;
        //return murmur2_hash(data, strlen(data), 0); 
        return bkdr_hash(data);
}


/* 
   将要添加的记录，定位槽位
 */
static uint32_t get_hash_value(const void *key, const uint32_t table_size)
{
        uint32_t new_key = 0;
        uint32_t hash_key = 0;
        uint32_t max_size = 0;

        hash_key = hash_value(key);	/*hash值生成*/
        max_size = table_size-1;
        new_key = (hash_key%max_size);	/*定槽位*/

        return new_key;
}


/* 
   键值比较
 */
static int key_cmp(const void *key1, const void *key2)
{
        const char *keyp1, *keyp2;

        keyp1 = (const char*)key1;
        keyp2 = (const char*)key2;

        return strcmp(keyp1, keyp2);
}

/*
 * 	创建哈希表，预分配槽位指针内存
 */
struct hash_s *hash_create(uint32_t size)
{
        struct hash_s *p;

        /*分配hash结构内存*/
        p = (struct hash_s *)calloc(1, sizeof(struct hash_s));
        if (p == NULL)	/*内存分配失败*/
                return NULL;
        p->ht_size = size;

        /* 预分配指向节点指针的指针并指向NULL（也就是槽指针），而不是预分配节点的内存 */
        p->ht_htable = (struct  hash_node_s **)calloc(size, sizeof(struct hash_node_s*));
        if (p->ht_htable == NULL) {		/*内存分配失败*/
                free(p);
                return NULL;
        }

        return p;
}

/*
 * 说明: 哈希表中，槽中各个链表的结点是排序的，它们以关键字key由小到大进行排列，
 * 　　　h->htable[hvalue]即为链表结点中关键字最小的一个．
 *
 *	参数：
 *		mode=0时，为不可重复插入
 *       	mode=1时，可重复插入。
 *
 *	返回值:
 *		0:成功
 *		-1:失败
 *		1:已存在的Key
 *
 */
int hash_insert(struct hash_s *h, void *key, void *datum, uint32_t mode)
{
        uint32_t hvalue;
        struct hash_node_s *prev, *cur, *newnode;

        /*如果没有分配hash结构内存，或者节点到上限，返回错误*/
        if (!h || h->ht_nel == HASHTAB_MAX_NODES)
                return -1;

        hvalue = get_hash_value(key, h->ht_size);
        prev = NULL;
        cur = h->ht_htable[hvalue];
        switch (mode) {
                case 0:   /*去重复*/
                        while (cur && (key_cmp(key, cur->key) > 0)) { /*顺序存放*/
                                prev = cur;	/*取得>key的前一个结点，赋值给prev*/
                                cur = cur->next;/* 当前结点指向当前的下一个结点 */
                        }

                        if (cur && (key_cmp(key, cur->key) == 0)){
                                errno = EEXIST;
                                return -1;
                        }
                        break;
                case 1: /*不去重复*/
                        prev = cur;	/*取得>key的前一个结点，赋值给prev*/
                        //if (curl) cur = cur->next;/* 当前结点指向当前的下一个结点 */
                        break;
        }

        /*添加节点*/
        newnode = (struct hash_node_s *)calloc(1, sizeof(struct hash_node_s));
        if (newnode == NULL)
                return -1;

        newnode->key = key;
        newnode->datum = datum;
        if (prev) {
                newnode->next = prev->next;	
                prev->next = newnode;
        } else {
                newnode->next = h->ht_htable[hvalue];	/*next指向NULL*/
                h->ht_htable[hvalue] = newnode;		/*链表头节点*/
        }

        h->ht_nel++;
        return 0;
}


/*
 * 说明: 通过key获取节点数据datum
 */
void *hash_search(struct hash_s *h, const void *key)
{
        uint32_t hvalue;
        struct hash_node_s *cur;

        if (!h) return NULL;

        hvalue = get_hash_value(key, h->ht_size);
        cur = h->ht_htable[hvalue];

        while (cur && (key_cmp(key, cur->key) != 0)){	
                cur = cur->next;
        }

        if (cur == NULL || (key_cmp(key, cur->key) != 0))
                return NULL;
        return cur->datum;
}

/*
 * 销毁哈希表
 */
static int _hash_free(struct hash_s *h, char flag)
{
        uint32_t i;
        struct hash_node_s *cur, *temp;

        if (!h) return -1;

        /* 逐一销毁链表节点数据 */
        for (i = 0; i < h->ht_size; i++) {
                cur = h->ht_htable[i];
                while (cur) {
                        temp = cur;
                        cur = cur->next;
                        if (flag&0x01) { /*free key */
                                if (temp->key) free(temp->key);
                        }
                        if (flag&0x02) { /*free datum */
                                if (temp->datum) free(temp->datum);
                        }
                        free(temp);
                }
                h->ht_htable[i] = NULL;
        }

        free(h->ht_htable);	/*销毁槽指针*/
        memset(h, 0, sizeof(struct hash_s));	/*清空结构*/
        free(h);
        return 0;
}

int hash_free(struct hash_s *h)
{
        return _hash_free(h, 0x00);
}

/*
flag:
        0x01: free key
        0x02: free datum
        0x03: free key & datum
*/
int hash_free_f(struct hash_s *h, char flag)
{
        return _hash_free(h, flag);
}

/*
 * 移除一个结点
 *	返回值:
 *		0:成功
 *		-1:失败
 *		1:未存在的Key
 *
 */
int hash_remove(struct hash_s *h, const void *key, char flag)
{
        uint32_t hvalue;
        struct hash_node_s *cur, *prev;

        if (!h) return -1;

        hvalue = get_hash_value(key, h->ht_size);
        prev = NULL;
        cur = h->ht_htable[hvalue];
        while (cur != NULL && (key_cmp(key, cur->key) > 0)) {
                prev = cur;	/* cur结点赋值给prev，记录前一个结点 */
                cur = cur->next;
        }

        /*没有找到key键的节点*/
        if (cur == NULL || (key_cmp(key, cur->key) != 0)) {
                return 1;
        }


        if (prev == NULL) {	/*为第一个*/
                h->ht_htable[hvalue] = cur->next;	
        } else { 		/*队列中存在多个冲突所产生的结点*/
                prev->next = cur->next;
        }


        if (flag&0x01) { /*free key */
                if (cur->key) free(cur->key);
        }
        if (flag&0x02) { /*free datum */
                if (cur->datum) free(cur->datum);
        }
        free(cur);
        cur = NULL;

        h->ht_nel--;
        return 0;
}


/*
 * must be free older datum
 * 更新以key为关键字结点的 数据区域.
 *
 *	返回值:
 *		0:成功
 *		-1:失败
 *		1:未存在的Key
 *
 */
int hash_update(struct hash_s *h, const void *key, void *datum)
{
        uint32_t hvalue;
        struct hash_node_s *cur;

        if (!h) return -1;

        hvalue = get_hash_value(key, h->ht_size);
        cur = h->ht_htable[hvalue];

        while (cur && (key_cmp(key, cur->key) > 0)) {
                cur = cur->next;
        }

        if (cur == NULL || (key_cmp(key, cur->key) != 0)) {	/*未找到的key*/
                return 1;
        }

        cur->datum = datum;
        return 0;
}


/*
   遍历hash表进行操作
 */
int hash_map(struct hash_s *h,
                int (*apply)(void *k, void *d, void *args),
                void *args)
{
        uint32_t i;
        int ret;
        struct hash_node_s *cur;

        if (!h) return -1;

        //printf("size:%d\n", h->ht_size);
        for (i = 0; i < h->ht_size; i++) {
                cur = h->ht_htable[i];
                //printf("slot[%d]:%x\n", i, h->ht_htable[i]);
                while (cur) {
                        ret = apply(cur->key, cur->datum, args);
                        if (ret == -1)
                                return ret;
                        cur = cur->next;
                }
        }
        return 0;
}

/*
   统计hashle数据：已用槽位，槽位最大链表长度
 */
void hash_stat(struct hash_s *h)
{
        uint32_t i, chain_len, slots_used, max_chain_len;
        struct hash_node_s *cur;

        slots_used = 0;
        max_chain_len = 0;
        for (slots_used = max_chain_len = i = 0; i < h->ht_size; i++) {
                cur = h->ht_htable[i];
                if (cur) {
                        slots_used++;		/*槽位*/
                        chain_len = 0;
                        while (cur) {
                                chain_len++;
                                cur = cur->next;
                        }

                        if (chain_len > max_chain_len)	/*最大链表长度*/
                                max_chain_len = chain_len;
                }
        }

        h->ht_slots_used = slots_used;
        h->ht_max_chain_len = max_chain_len;
}


