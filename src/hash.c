/*
 * Implementation of the hash table type.
 *
 * Date : 2011-07-28
 *
 * ԭʼ�������ԣ�security/selinux/ss/hash.c
 * hash�ο���http://www.cppblog.com/bellgrade/archive/2009/10/06/97926.html
 *  
 * 
 *  hash->ht_htable->[slot0|slot1|slot2|...|slotn]
 *
 *  [slot0]--->struct(hash_node)->next--->struct(hash_node)   ������
 *  [slot1]--->struct(hash_node)
 *  ...
 *  [slotn]
 *  
 */

#include "hash.h"

#define HASHTAB_MAX_NODES	0xFFFFFFFF

/* 
   hashֵ�����㷨(BKDR hash)
   key�ĳ�����20����bkdrЧ�ʸ���
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
   hashֵ����
 */
static uint32_t  hash_value(const void *key)
{
        u_char *data;

        data = (u_char*)key;
        //return murmur2_hash(data, strlen(data), 0); 
        return bkdr_hash(data);
}


/* 
   ��Ҫ��ӵļ�¼����λ��λ
 */
static uint32_t get_hash_value(const void *key, const uint32_t table_size)
{
        uint32_t new_key = 0;
        uint32_t hash_key = 0;
        uint32_t max_size = 0;

        hash_key = hash_value(key);	/*hashֵ����*/
        max_size = table_size-1;
        new_key = (hash_key%max_size);	/*����λ*/

        return new_key;
}


/* 
   ��ֵ�Ƚ�
 */
static int key_cmp(const void *key1, const void *key2)
{
        const char *keyp1, *keyp2;

        keyp1 = (const char*)key1;
        keyp2 = (const char*)key2;

        return strcmp(keyp1, keyp2);
}

/*
 * 	������ϣ��Ԥ�����λָ���ڴ�
 */
struct hash_s *hash_create(uint32_t size)
{
        struct hash_s *p;

        /*����hash�ṹ�ڴ�*/
        p = (struct hash_s *)calloc(1, sizeof(struct hash_s));
        if (p == NULL)	/*�ڴ����ʧ��*/
                return NULL;
        p->ht_size = size;

        /* Ԥ����ָ��ڵ�ָ���ָ�벢ָ��NULL��Ҳ���ǲ�ָ�룩��������Ԥ����ڵ���ڴ� */
        p->ht_htable = (struct  hash_node_s **)calloc(size, sizeof(struct hash_node_s*));
        if (p->ht_htable == NULL) {		/*�ڴ����ʧ��*/
                free(p);
                return NULL;
        }

        return p;
}

/*
 * ˵��: ��ϣ���У����и�������Ľ��������ģ������Թؼ���key��С����������У�
 * ������h->htable[hvalue]��Ϊ�������йؼ�����С��һ����
 *
 *	������
 *		mode=0ʱ��Ϊ�����ظ�����
 *       	mode=1ʱ�����ظ����롣
 *
 *	����ֵ:
 *		0:�ɹ�
 *		-1:ʧ��
 *		1:�Ѵ��ڵ�Key
 *
 */
int hash_insert(struct hash_s *h, void *key, void *datum, uint32_t mode)
{
        uint32_t hvalue;
        struct hash_node_s *prev, *cur, *newnode;

        /*���û�з���hash�ṹ�ڴ棬���߽ڵ㵽���ޣ����ش���*/
        if (!h || h->ht_nel == HASHTAB_MAX_NODES)
                return -1;

        hvalue = get_hash_value(key, h->ht_size);
        prev = NULL;
        cur = h->ht_htable[hvalue];
        switch (mode) {
                case 0:   /*ȥ�ظ�*/
                        while (cur && (key_cmp(key, cur->key) > 0)) { /*˳����*/
                                prev = cur;	/*ȡ��>key��ǰһ����㣬��ֵ��prev*/
                                cur = cur->next;/* ��ǰ���ָ��ǰ����һ����� */
                        }

                        if (cur && (key_cmp(key, cur->key) == 0)){
                                errno = EEXIST;
                                return -1;
                        }
                        break;
                case 1: /*��ȥ�ظ�*/
                        prev = cur;	/*ȡ��>key��ǰһ����㣬��ֵ��prev*/
                        //if (curl) cur = cur->next;/* ��ǰ���ָ��ǰ����һ����� */
                        break;
        }

        /*��ӽڵ�*/
        newnode = (struct hash_node_s *)calloc(1, sizeof(struct hash_node_s));
        if (newnode == NULL)
                return -1;

        newnode->key = key;
        newnode->datum = datum;
        if (prev) {
                newnode->next = prev->next;	
                prev->next = newnode;
        } else {
                newnode->next = h->ht_htable[hvalue];	/*nextָ��NULL*/
                h->ht_htable[hvalue] = newnode;		/*����ͷ�ڵ�*/
        }

        h->ht_nel++;
        return 0;
}


/*
 * ˵��: ͨ��key��ȡ�ڵ�����datum
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
 * ���ٹ�ϣ��
 */
static int _hash_free(struct hash_s *h, char flag)
{
        uint32_t i;
        struct hash_node_s *cur, *temp;

        if (!h) return -1;

        /* ��һ��������ڵ����� */
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

        free(h->ht_htable);	/*���ٲ�ָ��*/
        memset(h, 0, sizeof(struct hash_s));	/*��սṹ*/
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
 * �Ƴ�һ�����
 *	����ֵ:
 *		0:�ɹ�
 *		-1:ʧ��
 *		1:δ���ڵ�Key
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
                prev = cur;	/* cur��㸳ֵ��prev����¼ǰһ����� */
                cur = cur->next;
        }

        /*û���ҵ�key���Ľڵ�*/
        if (cur == NULL || (key_cmp(key, cur->key) != 0)) {
                return 1;
        }


        if (prev == NULL) {	/*Ϊ��һ��*/
                h->ht_htable[hvalue] = cur->next;	
        } else { 		/*�����д��ڶ����ͻ�������Ľ��*/
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
 * ������keyΪ�ؼ��ֽ��� ��������.
 *
 *	����ֵ:
 *		0:�ɹ�
 *		-1:ʧ��
 *		1:δ���ڵ�Key
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

        if (cur == NULL || (key_cmp(key, cur->key) != 0)) {	/*δ�ҵ���key*/
                return 1;
        }

        cur->datum = datum;
        return 0;
}


/*
   ����hash����в���
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
   ͳ��hashle���ݣ����ò�λ����λ���������
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
                        slots_used++;		/*��λ*/
                        chain_len = 0;
                        while (cur) {
                                chain_len++;
                                cur = cur->next;
                        }

                        if (chain_len > max_chain_len)	/*���������*/
                                max_chain_len = chain_len;
                }
        }

        h->ht_slots_used = slots_used;
        h->ht_max_chain_len = max_chain_len;
}


