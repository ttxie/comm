/*
input cvs:
name, gendor
0, 1   #0:string, 1:int
tom, 1
jack, 2


output json:
[
{"name":"tom", "gendor":1},
{"name":"jack", "gendor":2}
]
*/

/*darray*/
#include <stdio.h>

#include "file.h"
#include "darray.h"
#include "utl.h"

#define  LINE 4096

int main(int argc, char **argv)
{
        FILE *fd, *fdw;
        char *orgbuf,*buf, *p, *ptr, *tt;
        int size, n, j;
        struct darray_s *darr, *darr_line; 
        struct darray_s **item;
	char filename[50], outfile[50];

        if (argc < 2) {
                printf("format error: csv2json {filename}\n");
                exit(0);
        }
        strcpy(filename, argv[1]);
        strcpy(outfile, argv[1]);
        strcat(outfile, ".json");

        printf("filename:%s, outfile:%s\n", filename, outfile);

        fd = fopen(filename, "r+");
        if (fd == NULL) {
                perror("open input file err");        
                exit(0);
        }
        //fdw = fopen(strcat(argv[1],".json"), "w");
        fdw = fopen(outfile, "w");
        if (fdw == NULL) {
                perror("open write file err");        
                exit(0);
        }
        
/*   load data into dynamic array  */
        orgbuf = buf = (char*)calloc(1, LINE);
        darr = (u_char *)darray_calloc(linecount_fd(fd), sizeof(buf));

        while (fgets(buf, LINE, fd)) {
        //        printf("buf:%s\n", buf);
                darr_line = darray_calloc(strcount(buf, ',')+1, LINE);
                //printf("%p\n", darr_line);

                tt = darray_pushback(darr);
                //printf("1 values: %p\n", tt);
                memcpy(tt, &darr_line, sizeof(darr_line));
                
                trim(buf);
                while (p = strchr(buf, ',')) {
                        size = p-buf;
                        ptr = darray_pushback(darr_line); 
                        //printf("2 buf:%s, size:%d, ptr:%p\n", buf, size, ptr);
                        strncpy(ptr, buf, size);
                        buf = p+1;
                        trim(buf);
                } 

                ptr = darray_pushback(darr_line); 
                size = strlen(buf);
                //printf("2 buf:%s, size:%d, ptr:%p\n", buf, size, ptr);
                strncpy(ptr, buf, size);

                buf = orgbuf;
                memset(buf, 0, LINE);
        }


/*      write json file */
        item = darr->da_values;
        
        if (darray_count(darr) > 3) fprintf(fdw, "[");
        for (n=3; n<darray_count(darr); n++) {                
                fprintf(fdw, "{");
                
                for (j=0; j<darray_count(*item); j++) {

                        //printf("tt:%s\n", item[1]->da_values+j*item[1]->da_size);
                        if (strcmp(item[1]->da_values+j*item[1]->da_size, "0") == 0)
                                fprintf(fdw, "\"%s\":\"%s\"", item[0]->da_values+j*item[n]->da_size, item[n]->da_values+j*item[n]->da_size);
                        else if (strcmp(item[1]->da_values+j*item[1]->da_size, "2") == 0) {
                                fprintf(fdw, "\"%s\":\"%s\"", item[0]->da_values+j*item[n]->da_size, item[n]->da_values+j*item[n]->da_size);
/*
                                char tt[1024];
                                memset(tt, 0, 1024);
                                printf("len:%d\n",  strlen(*(item[n]->da_values+j*item[n]->da_size)));
                                memcpy(tt, item[n]->da_values+j*item[n]->da_size+1, strlen(item[n]->da_values+j*item[n]->da_size)-2);
                                fprintf(fdw, "\"%s\":%s", item[0]->da_values+j*item[n]->da_size, item[n]->da_values+j*item[n]->da_size);
*/
                        } else if (strcmp(item[1]->da_values+j*item[1]->da_size, "1") == 0)
                                fprintf(fdw, "\"%s\":%d", item[0]->da_values+j*item[n]->da_size, atoi(item[n]->da_values+j*item[n]->da_size));
                        else
                                printf("data tyep err.");

                        if (j<darray_count(*item)-1)
                                fprintf(fdw, ", ");
                }
                
                if (n < darray_count(darr)-1)
                        fprintf(fdw, "}, \n");
                else
                        fprintf(fdw, "}");
        }
        if (darray_count(darr) > 3) fprintf(fdw, "]\n");

        free(orgbuf);
        darray_free(darr);
        fclose(fd);
        fclose(fdw);

        return 0;
}

