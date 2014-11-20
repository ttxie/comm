#include <pthread.h>
#include <string.h>
#include "utl.h"
#include "client.h"
#include "ringbuf.h"
#include <sys/types.h>


struct entry_s {
    char    msg_id[32];
    void    *data; 
};

void cmd(struct client_s *cli)
{
    struct entry_s *entry = (struct entry_s*)calloc(1, sizeof(struct entry_s));
    if (entry ==NULL) perror("callc");

    printf("tid:%u, len:%d\n", gettid(), cli->rbuf->buf_len);
    
    strcpy(entry->msg_id, "skill_upgrade");
    entry->data = NULL;

    rb_push(cli->rb, (unsigned char*) entry);

    sleep(1);

}


int main()
{
    struct entry_s *entry;
    struct client_s *client = client_open("121.201.6.2", 80);
    if (client == NULL) perror("client open");
    client->read_filter = cmd;

    while(1) {
        int i=0;
        printf("tid:%u\n", gettid());
        buf_append(client->wbuf, "GET / HTTP/1.0 \nConnection:keep-alive \n\nABCDE", 44);

        while (entry = (struct entry_s*)rb_pop(client->rb)) {
            printf("%d:%s\n", i++, entry->msg_id);
            free(entry);
        }
        
        sleep(1);
    } 
    
    client_close(client);
}

