#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>

#include "utl.h"

struct stu {
    int id;
    char username[10];
    char passwd[10];
    char email[20];
    char gendor;
    int age;
};



int main()
{
    struct stu *pstu;
    redisContext *c;
    redisReply *reply;
    char *p;
    uint64_t start, end, n;
    struct timeval timeout={ 1, 500000 };;

    pstu = (struct stu*)calloc(1, sizeof(struct stu));
    if (pstu == NULL) return 0;

    pstu->id = 10001;
    strcpy(pstu->username, "user");
    strcpy(pstu->passwd, "pwd");
    strcpy(pstu->email, "tt@tom.com");
    pstu->gendor = 0;
    pstu->age = 25;

    c = redisConnectWithTimeout((char*)"127.0.0.1", 6379, timeout);
    if (c->err) {
        printf("Connection error: %s\n", c->errstr);
        freeReplyObject(c);
        free(pstu);
        exit(1);
    }


    start = tm64();
    for(n=0; n<10000; n++) {
    /* Set a key using binary safe API */
    reply = redisCommand(c,"SET %s %b", "stu_pk_10001", pstu, sizeof(*pstu));
    //printf("SET (binary API): %s\n", reply->str);
    freeReplyObject(reply);

    /* Set a key using binary safe API */
/*
    p = redisff("stu_pk_10001", 12);
    reply = redisCommand(c,"SET %s %b", "stu_fk_user", p, 14);
    //printf("SET (binary API): %s\n", reply->str);
    freeReplyObject(reply);
    free(p);
*/
    /* Try a GET and two INCR */
    reply = redisCommand(c,"GET stu_pk_10001");
//    hexdump(reply->str, reply->len);
    //printf("GET foo: %s\n", reply->str);
    //printf("reply->type:%u, len:%d\n", reply->type, reply->len);
    freeReplyObject(reply);


    /* Try a GET and two INCR */
/*
    reply = redisCommand(c,"GET stu_fk_user");
//    hexdump(reply->str, reply->len);
    //printf("GET foo: %s\n", reply->str);
    //printf("reply->type:%u, len:%d\n", reply->type, reply->len);   
    freeReplyObject(reply);
*/
    }
    end=tm64();

    printf("time:%llu\n", end-start);

    freeReplyObject(c);
    free(pstu);

}

