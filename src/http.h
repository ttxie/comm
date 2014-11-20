#if !defined(httpclient_C4C48927_C4A2_489b_ABD4_A6B9D3317A90__INCLUDED_)
#define httpclient_C4C48927_C4A2_489b_ABD4_A6B9D3317A90__INCLUDED_
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "sk.h"
#include "dstr.h"

struct http_request_s
{
        char *	        domain;
        uint16_t	port;
        char            keepalive;      /*0:close 1:keepalive*/
        int	        fd;
        char *	        url;
        char *	        body;
        uint32_t        start_byte;
        uint32_t        end_byte;
        uint64_t        timeout;
        /*char *          request_headers;*/

};


struct http_response_s
{
        int             status_code;
        char *          status_text;
        uint32_t        content_length;   /*from head*/
        uint32_t        data_len;
        u_char *        data;           /* start from '\r\n\r\n' */
        /*char *          response_headers;*/
};

#ifdef __cplusplus
extern "C" {
#endif
        /*
         * must get socket fd call sk_connect() first;
         * and call close(fd) after http_get/reget;
         * 
         * return pointer to http_response_s struct, return NULL on failure.
         */
        struct http_response_s* http_get(struct http_request_s *hreq);

        struct http_response_s* http_post(struct http_request_s *hreq);


        //struct http_response_s* http_reget(struct http_request_s *hreq);



        /*
         * get size of url file
         */
        int http_headsize(struct http_request_s *hreq);


#ifdef __cplusplus
}
#endif

#endif // !defined(httpclient_C4C48927_C4A2_489b_ABD4_A6B9D3317A90__INCLUDED_)

