#include "http.h"
#include <unistd.h>
#include <fcntl.h>

#define DOWN_PARTSIZE (512*1024)

static int32_t down_filesize;
static uint32_t down_percent;

int download(struct http_request_s *hreq)
{
        struct http_response_s *hres = NULL;
        char *filename=NULL, idx_filename[128], idx[32];
        int n=0, index=0, fd=0, fdidx=0, ret=0, len=0;

        down_filesize = http_headsize(hreq);
        if (down_filesize == -1) {
                perror("filesize");
                return -1;
        }
        printf("url:%s filesize:%d\n", hreq->url, down_filesize);

        filename = strsub(hreq->url, strlastpos(hreq->url, "\/")+1, 
                        strlen(hreq->url)-strlastpos(hreq->url, "\/")-1);
        printf("filename:%s\n", filename);
        
        fd = open(filename, O_WRONLY|O_CREAT|O_APPEND, 0666);
        if (fd == -1) {
                perror("open file");
                ret = -1;
                goto END;
        }

        strcpy(idx_filename, filename); /*todo:?& */
        strcat(idx_filename, ".idx"); 
        fdidx = open(idx_filename, O_RDWR|O_CREAT, 0666);
        if (fdidx == -1) {
                perror("open idx file");
                ret = -1;
                goto END;
        }
        if (read(fdidx, idx, 32) == -1) {
                perror("read idx file"); 
                ret = -1;
                goto END;
        }
        if (strlen(idx) == 0) index = 0;
        else index = atoi(trim(idx));
        
        if (filesize_fd(fd) != index) {
                index = 0;//filesize_fd(fd);
                ftruncate(fd, 0);
                lseek(fd, 0, SEEK_SET);
        }
        printf("index:%d filesize:%d\n", index, filesize_fd(fd));
        
        n = ceil((float)index/DOWN_PARTSIZE);
        printf("n:%d\n", n);
        while (n*DOWN_PARTSIZE < down_filesize) {
                hreq->start_byte = n*DOWN_PARTSIZE;
                hreq->end_byte = (n+1)*DOWN_PARTSIZE-1;
                if (hreq->end_byte > down_filesize)
                        hreq->end_byte = down_filesize;

                hres = http_get(hreq);
                if (hres == NULL) {
                        ret = -1;
                        goto END;
                }

                printf("http status:%d %s\ndata_len:%dcontent_length:%d\n", 
                        hres->status_code, hres->status_text, hres->data_len, hres->content_length);
                
                len = write(fd, hres->data, hres->data_len);
                if (len==-1) perror("write file");
                
                down_percent = (hreq->end_byte*100)/down_filesize;
                
                printf("percent:%d\n", down_percent);
                n++;

                lseek(fdidx, 0, SEEK_SET);
                if (n*DOWN_PARTSIZE > down_filesize)
                        sprintf(idx, "%d        ", down_filesize);
                else        
                        sprintf(idx, "%d        ", n*DOWN_PARTSIZE);
                write(fdidx, idx, strlen(idx));
                
                free(hres->status_text);
                free(hres->data);
                free(hres);                
        }
        
END:

        if (filename) free(filename);
        if (fdidx) close(fdidx);
        if (fd) close(fd);

        return ret;
}


int main()
{
        struct http_request_s *hreq;

        //printf("test:%d\n", strlastpos("/baidu/logo.gif/", "\/"));
        
        hreq = (struct http_request_s *)calloc(1, sizeof(struct http_request_s));
        if (hreq == NULL) return 0;

        hreq->domain = "www.baidu.com";
        hreq->port = 80;
        hreq->url = "/img/bdlogo.gif";

        hreq->domain = "www.eagle-game.com";
        hreq->port = 80;
        hreq->url = "/static/gupload/com.ResUpdateTest.www-1.apk";

        hreq->timeout = 1000000;

        
        if (download(hreq) == -1) perror("downlaod file");

        free(hreq);
        
        return 0;
}


