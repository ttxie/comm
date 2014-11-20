#include "http.h"
#include <unistd.h>
#include <fcntl.h>


int main()
{
	int fd, n;
        struct http_request_s *hreq;
        struct http_response_s *hres;
        
        hreq = (struct http_request_s *)calloc(1, sizeof(struct http_request_s));
        if (hreq == NULL) return 0;

        hreq->domain = "114.80.154.210";
        hreq->port = 8001;
        hreq->url = "/SPSInterface/jizhiIVRIVRChannel.aspx?sendid=64704055&mobile=18775069753&linkid=&retvalue=&serviceid=1010006&senddate=20141027112400&destnum=125905731333&duration=2";


/*
        hreq->domain = "www.jizhiinfo.net";
        hreq->port = 80;
        hreq->url = "/images/logo.png";
*/
        hres = http_get(hreq);
        if (hres == NULL) {
                perror("http err");
                free(hreq);
                return 1;
        }

        printf("http status:%d %s\ndata_len:%dcontent_length:%d\n", 
                hres->status_code, hres->status_text, hres->data_len, hres->content_length);
        
        
        fd = open("logo.gif", O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (fd == -1) perror("openfile");
        n = write(fd, hres->data, hres->data_len);
        if (n==-1) perror("write file");
        
        close(fd);
        
        free(hres->status_text);
        free(hres->data);
        //free(hres);
        free(hreq);
        
        return 0;
}


