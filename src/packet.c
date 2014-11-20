/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 *
 */
#include "packet.h"

static int debody(struct buf_s *body, const size_t arr[][2], int rows, u_char *ptr);


/*
   pack/unpack a signed number, return the number of bytes needed
   Negative numbers are packed as twos-complement 64-bit integers.
   static size_t pack_sint64(int64_t num, struct buf_s *buf)
   {
   size_t s;
   u_char varint[10];

   s = itov(num, varint);
   buf_append(buf, varint, s);

   return s;
   }
   static size_t unpack_sint64(struct buf_s *buf, int64_t *num)
   {
   size_t s;

   s = vtoi(buf->buf_pos, buf->buf_len, num);
   buf_del(buf, s);

   return s;
   }
   */


/*
   pack/unpack a unsigned number, return the number of bytes needed
   */
static size_t pack_uint64(uint64_t num, struct buf_s *buf)
{
    size_t s;
    u_char varint[10];

    if (buf == NULL) return 0;
    s = utov(num, varint);
    buf_append_unlock(buf, varint, s);

    return s;
}
static size_t unpack_uint64(struct buf_s *buf, uint64_t *num)
{
    size_t s;

    if (buf == NULL || num == NULL) return 0;

    s = vtou(buf->buf_pos, buf->buf_len, num);
    buf_del_unlock(buf, s);

    return s;
}

/*
   pack/unpack a string with terminate '\0', return the number of bytes needed
   */
static size_t pack_string(const char *str, struct buf_s *buf)
{
    size_t s;
    u_char varint[10];

    if (buf == NULL || str == NULL) return 0;

    s = utov(strlen(str), varint);
    buf_append_unlock(buf, varint, s);
    buf_append_unlock(buf, (u_char *)str, strlen(str));

    return s+strlen(str);
}
static size_t unpack_string(struct buf_s *buf, char *str)
{
    size_t s;
    uint64_t len;

    if (buf == NULL || str == NULL) return 0;

    s = vtou(buf->buf_pos, buf->buf_len, &len);
    memcpy(str, buf->buf_pos+s, len);
    str[len] = '\0';


    buf_del_unlock(buf, s+len);

    return s+len;
}




/*
 * import poiter to structure and array to property of structure,  and gererate body of packet.
 *
 * @ptr: specifies pointer to void structure for encoding
 * @arr: indicate  field's offset and field's type in struct
 * @rows: row num of array
 * @buf(output): pointer to body;
 *
 * return number of bytes encoded on success, or -1 if error occurred.
 *
 */
int enbody(u_char *ptr, const size_t arr[][2], int rows, struct buf_s *body, size_t bm_pos)
{
    int8_t s8=0;
    int16_t s16=0;
    int32_t s32=0;
    int64_t s64=0, n=0;
    uint8_t u8=0;
    uint16_t u16=0;
    uint32_t u32=0;
    uint64_t u64=0, pos=0;
    struct bitset_s *fmap;
    size_t len;

    fmap = bitset_create(rows);
    if (fmap == NULL) return -1;
    len = (fmap->size-1)/8==fmap->size/8 ? fmap->size/8+1: fmap->size/8;
    buf_append_unlock(body, (u_char*)fmap->bits, len);

    for (n=0; n<rows; n++) {

        pos = arr[n][0];

        switch(arr[n][1]) {
            case PACK_SINT8:
                memcpy(&s8, ptr+pos, 1);
                if (s8 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(zigzag(s8), body);
                break;
            case PACK_SINT16:
                memcpy(&s16, ptr+pos, 2);
                if (s16 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(zigzag(s16), body);
                break;
            case PACK_SINT32:
                memcpy(&s32, ptr+pos, 4);
                if (s32 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(zigzag(s32), body);
                break;
            case PACK_SINT64:
                memcpy(&s64, ptr+pos, 8);
                if (s64 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(zigzag(s64), body);
                break;
            case PACK_UINT8:
                memcpy(&u8, ptr+pos, 1);
                if (u8 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(u8, body);
                break;
            case PACK_UINT16:
                memcpy(&u16, ptr+pos, 2);
                if (u16 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(u16, body);
                break;
            case PACK_UINT32:
                memcpy(&u32, ptr+pos, 4);
                //printf("[%d], ptr:%p pos:%u, u32:%x\n", n, ptr, pos, u32);
                if (u32 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(u32, body);
                break;
            case PACK_UINT64:
                memcpy(&u64, ptr+pos, 8);
                if (u64 == 0) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_uint64(u64, body);
                break;
            case PACK_STRING:
                if ((ptr+pos) == NULL) {
                    bitset_set(fmap, n);
                    continue;
                }
                pack_string((const char*)ptr+pos, body);
                break;
            case PACK_CHARP: {
                                 u_char **p = (u_char **)(ptr + pos);
                                 if (*p == NULL) {
                                     bitset_set(fmap, n);
                                     continue;
                                 }
                                 pack_string((const char*)*p, body);
                                 break;
                             }

        }
    } 

    memcpy(body->buf_pos+bm_pos, fmap->bits, len);
    if (fmap) bitset_free(fmap);

    return body->buf_len;
}


/*
 *  封装网络包
 * uint16_t cmdid, uint32_t seqid, uint32_t uid, uint32_t aid, 
 */
int enpacket(struct head_s *head, u_char *ptr, int item_len, int item_size,
        const size_t arr[][2], int rows, struct buf_s *buf) 
{
    struct buf_s *body = NULL;
    int n=0, size=0, len=0; /*length of body*/

    if (head == NULL || buf == NULL) return -1;
    size = item_size*item_len*5/4+16+HEAD_LEN;
    if (size > 0xFFFF) return -1;
    
    body =  (struct buf_s*)buf_calloc(size);
    if (body == NULL) goto errret;

    /*pack data into packet body*/
    buf_append_unlock(body, (u_char*)head, HEAD_LEN);
    len = HEAD_LEN;
    for (n=0; n<item_len; n++) {
        len = enbody(ptr, arr, rows, body, len);
        if (len == -1  || (len > (int)(buf->buf_size-buf->buf_len)))
            goto errret; /*overflow the length of header*/

        ptr += item_size;
    }

    /*packet head*/
    head->flag = 0xFFFF;
    head->len = hton16(len);
    head->cmdid = hton16(head->cmdid);
    head->seq = hton32(head->seq);
    head->crc16 = crc16(body->buf_pos+HEAD_LEN, body->buf_len-HEAD_LEN);
    head->uid = hton32(head->uid);
    head->aid = hton32(head->aid);

    /*into buf_s*/
    memcpy(body->buf_start, head, HEAD_LEN);
    buf_append(buf, body->buf_pos, body->buf_len);
    buf_free(body);

    return len+HEAD_LEN;

errret:
    if (body) buf_free(body);
    return -1;
}


/*
 *  解析网络包头
 */
int dehead(struct buf_s *buf, struct head_s *head)
{
    if (buf == NULL) return -1;

    while (1) {
        /*just a part of packet, required more data */
        if (buf->buf_len < sizeof(struct head_s)) { /* length of buf LT length of header */
            errno = EINVAL;
            return -1;
        }

        memset(head, 0, HEAD_LEN);
        memcpy(head, buf->buf_pos, HEAD_LEN); 

        head->seq = ntoh32(head->seq);
        head->cmdid = ntoh16(head->cmdid);
        head->len = ntoh16(head->len);
        head->uid = ntoh32(head->uid);
        head->aid = ntoh32(head->aid);

        if (head->flag != 0xFFFF) {
            buf_del(buf, 1);
            continue;
        }

        if (head->len > buf->buf_len) {
            errno = EINVAL;
            return -1;      /*pls check cmdid out of the func*/
        }

        /*crc16 check*/ 
        if (head->crc16 !=  crc16(buf->buf_pos+HEAD_LEN, head->len-HEAD_LEN)) {
            buf_del(buf, 1);
            continue;
            /*
               errno = EBADMSG; 
               head = NULL;
               return -1;
               */
        } else break;
    }

    return head->len;
}




/*
   decode the buf 

buf:
arr:
rows:
 *ptr:

 return：length of the body
 */
int depacket(struct buf_s *buf, const size_t arr[][2], int rows, u_char *ptr)
{
    struct head_s head;
    struct buf_s *body;

    if (buf == NULL || ptr == NULL) return -1;

    /*header structure*/
    memset(&head, 0, HEAD_LEN);
    memcpy(&head, buf->buf_pos, HEAD_LEN); 
    head.len = ntoh16(head.len);
    if (head.len > buf->buf_len) {
        errno = EINVAL;
        return -1;
    }

    //body = buf_calloc(head.len - HEAD_LEN);
    body = buf_calloc(head.len);
    if (body == NULL)  return -1;

    buf_pull(buf, body->buf_pos, head.len);
    body->buf_len = head.len;
    buf_del_unlock(body, HEAD_LEN);
    if (debody(body, arr, rows, ptr) == -1) {
        buf_free(body);
        return -1;
    }
    buf_free(body);

    return head.len;
}


int depacket_da(struct buf_s *buf, const size_t arr[][2], int rows, struct darray_s *da)
{
    struct head_s head;
    u_char *ptr;
    struct buf_s *body;

    if (buf == NULL || da == NULL) return -1;

    /*header structure*/
    memset(&head, 0, HEAD_LEN);
    memcpy(&head, buf->buf_pos, HEAD_LEN); 
    head.len = ntoh16(head.len);
    if (head.len > buf->buf_len) {
        errno = EINVAL;
        return -1;      /*pls check cmdid out of the func*/
    }

    body = buf_calloc(head.len);
    if (body == NULL)  return -1;
    buf_pull(buf, body->buf_pos, head.len);
    body->buf_len = head.len;
    buf_del_unlock(body, HEAD_LEN);

    while (body->buf_len){
        u_char *ptr = darray_pushback(da);
        if (debody(body, arr, rows, ptr) == -1) break;
    }

    buf_free(body);
    return head.len;
}



static int debody(struct buf_s *body, const size_t arr[][2], int rows, u_char *ptr)
{
    int64_t s64=0;
    uint64_t u64=0;
    size_t pos=0, len, ret=0;
    int n=0;

    bitset_t *bitmap;

    if (body == NULL || ptr == NULL) return -1;

    /*bitset  len: length of bitset */
    bitmap = bitset_create(rows);
    if (bitmap == NULL) return -1; 
    len = (bitmap->size-1)/8==bitmap->size/8 ? bitmap->size/8+1: bitmap->size/8;

    memcpy(bitmap->bits, body->buf_pos, len);
    buf_del(body, len);


    for (n=0; n<rows; n++) {
        pos = arr[n][0];

        switch(arr[n][1]) {
            case PACK_SINT8:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 1);
                    continue;
                }
                unpack_uint64(body, &u64);
                s64 = unzigzag(u64);
                memcpy(ptr+pos, &s64, 1);
                break;
            case PACK_SINT16:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 2);
                    continue;
                }
                unpack_uint64(body, &u64);
                s64 = unzigzag(u64);
                memcpy(ptr+pos, &s64, 2);
                break;
            case PACK_SINT32:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 4);
                    continue;
                }
                unpack_uint64(body, &u64);
                s64 = unzigzag(u64);
                memcpy(ptr+pos, &s64, 4);
                break;
            case PACK_SINT64:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 8);
                    continue;
                }
                unpack_uint64(body, &u64);
                s64 = unzigzag(u64);
                memcpy(ptr+pos, &s64, 8);
                break;
            case PACK_UINT8:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 1);
                    continue;
                }
                unpack_uint64(body, &u64);
                memcpy(ptr+pos, &u64, 1);
                //unpack_uint64(body, (uint64_t*)(ptr+pos));
                break;
            case PACK_UINT16:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 2);
                    continue;
                }
                unpack_uint64(body, &u64);
                memcpy(ptr+pos, &u64, 2);
                break;
            case PACK_UINT32:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 4);
                    continue;
                }
                unpack_uint64(body, &u64);
                memcpy(ptr+pos, &u64, 4);
                break;
            case PACK_UINT64:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 8);
                    continue;
                }
                unpack_uint64(body, &u64);
                memcpy(ptr+pos, &u64, 8);
                break;

            case PACK_STRING:
                if (bitset_isset(bitmap, n)) {
                    memset(ptr+pos, 0, 1);
                    continue;
                }
                unpack_string(body, (char*)ptr+pos);
                break;
            case PACK_CHARP: {
                                 u_char **p = (u_char **)(ptr + pos);
                                 if (bitset_isset(bitmap, n)) {
                                     *p = NULL;
                                     continue;
                                 }
                                 unpack_string(body, (char *)*p);
                                 break;
                             }
        }

    } 

    bitset_free(bitmap);

    return body->buf_size - body->buf_len;
}
