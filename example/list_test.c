#include "list.h" /*�����ҵĻ�����û��list.h,�����ҿ�����һ���������������У�Ӧ���Ǽ�#include <linux/list.h>*/  
#include <stdio.h>   
#include <string.h>  

#define MAX_USER_LEN 32  
#define MAX_PAS_LEN 32  
#define MAX_SERVER_LEN 1024  


typedef struct server_detect_ftp  
{  
        struct list_head list;  
        char server[MAX_SERVER_LEN];  
        int port;  
        char username[MAX_USER_LEN];  
        char password[MAX_PAS_LEN];  
}server_detect_ftp_t;  

int main(void)  
{  
        struct list_head head;//ͷ��  
        server_detect_ftp_t ftp_link;  
        server_detect_ftp_t ftp_link1;  
        server_detect_ftp_t *entry;  
        struct list_head *p;  

        strcpy(ftp_link.server,"www.163.com");  
        ftp_link.port=34;  
        strcpy(ftp_link.username,"good");  
        strcpy(ftp_link.password,"good");  


        strcpy(ftp_link1.server,"www.baidu.com");  
        ftp_link1.port=32;  
        strcpy(ftp_link1.username,"baidu");  
        strcpy(ftp_link1.password,"baidu");  

        INIT_LIST_HEAD(&head);//��ʼ��ͷ��  

        list_myadd(&ftp_link.list, &head);   /*��ͷ������*/
        list_myadd(&ftp_link1.list, &head);     /*��ͷ������*/

        //list_del(&ftp_link1.list);//ɾ������  
        
        list_for_each(p,&head)//����  
        {
                entry=list_entry(p,struct server_detect_ftp,list);//��ȡĳ��ֵ  
                printf("%s\n",entry->username);  
        }

        return 0;  
}  
