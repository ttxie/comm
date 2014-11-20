#include "readcsv.h"
#include "hash.h"


/* 
 field key value for hash search
        key:field name
        value:field index
 */
struct item_s {
        char *name;
        int idx;
};


/*
free
*/
void free_csv(struct csv_s *csv)
{
        uint32_t n=0, m=0;
        struct darray_s **item;
        char **p;

        if (csv == NULL) return ;
        if (csv->fhash)  hash_free_f(csv->fhash, O_DATUM);
        if (csv->khash)  hash_free_f(csv->khash, O_DATUM);

        item = (struct darray_s **)csv->darr->da_values;
        for (n=0; n<csv->records; n++) {
                /*free dstr memory*/
                if (csv->filename) {
                        for (m=0; m<csv->columns; m++) {
                                p = (char **)(item[n]->da_values + m*item[n]->da_size);
                                //printf("data[%d %d]:%s[%p]\n", n, m, *p, *p);
                                if (*p) dstrfree(*p);
                        }
                }
                /*free record darray memory*/
                darray_free(item[n]);
        }

        darray_free(csv->darr);
        free(csv);
}

/*
create struct csv_s and load data into memory.
 */
struct csv_s* parse_csv(char *data, size_t len)
{
        struct csv_s *csv = NULL;
        char *buf, *orgbuf=NULL, *p, *tt;
        char **ptr;
        uint32_t size=0, curline=0, curcol=0, m=0, line_count=0;
        size_t n=0;
        struct darray_s *darr_line = NULL; 
        struct darray_s **item;
        struct item_s *fitem;
        char **pp;

        if (data == NULL) return NULL;
        /*count number of line*/
        for(n=0; n < len; n++) {
                if (data[n] == 0x0A) {
                        data[n] = 0x00;
                        line_count++;
                }
        }

        csv = (struct csv_s*)calloc(1, sizeof(struct csv_s));
        if (csv == NULL) goto ERR;
        csv->records = line_count;
        csv->khash = hash_create(line_count*2);
        if (csv->khash == NULL) goto ERR;

        /*   load data into dynamic array  */
        orgbuf = buf = data;
        csv->darr = darray_calloc(csv->records, sizeof(char *));
        if (csv->darr == NULL) goto ERR;

        for (n=0; n<line_count; n++) {
                /*mark the total columns*/
                if (buf == NULL) continue;
                if (curline == 0) { 
                        csv->columns = size = (uint32_t)strcount(buf, ',')+1;
                        csv->fhash = hash_create(size);
                        if (csv->fhash == NULL) goto ERR;
                } else if (csv->columns > (strcount(buf, ',')+1)) {
                        /*next line*/
                        size = strchr(buf,0x00)-buf;
                        buf = buf+size+1;
                        continue;
                }

                darr_line = NULL;
                darr_line = darray_calloc(csv->columns, sizeof(char*));
                if (darr_line == NULL) goto ERR;
                //printf("darr_line:%p, value:%p, size:%d\n",darr_line, darr_line->da_values, csv->columns*sizeof(char*));
        
                tt = (char *)darray_pushback(csv->darr);
                memcpy(tt, &darr_line, sizeof(darr_line));
                //printf("tt:%p\n", tt);

                curcol=0;
                while ((p = strchr(buf, ',')) && curcol < (csv->columns-1)) {
                        size = p-buf;
                        buf[size] = '\0';
                        ptr = (char **)darray_pushback(darr_line);
                        memcpy(ptr, &buf, sizeof(char*));
                        buf = p+1;
                        if (curline == 0) {
                                fitem = (struct item_s*)malloc(sizeof(struct item_s));
                                trim(*ptr);
                                fitem->name = *ptr;
                                fitem->idx = curcol;
                                hash_insert(csv->fhash, *ptr, fitem, 1);
                        }
                        if (curcol == 0) {
                                trim(*ptr);
                                fitem = (struct item_s*)malloc(sizeof(struct item_s));
                                fitem->name = *ptr;
                                fitem->idx = curline;
                                hash_insert(csv->khash, *ptr, fitem, 1);
                        }
                        curcol++;
                }

                size = strchr(buf,0x00)-buf;
                ptr = (char **)darray_pushback(darr_line);
                //printf("buf:%s, size:%d\n", buf, size);
                memcpy(ptr, &buf, sizeof(char*));
                buf = buf+size+1;
                if (curline == 0) {
                        fitem = (struct item_s*)malloc(sizeof(struct item_s));
                        trim(*ptr);
                        fitem->name = *ptr;
                        fitem->idx = curcol;
                        hash_insert(csv->fhash, *ptr, fitem, 1);
                }

                curline++;
                csv->records = curline;
        }

        return csv;

ERR:
        if (csv) {
                if (csv->fhash) hash_free_f(csv->fhash, O_DATUM);
                if (csv->khash) hash_free_f(csv->khash, O_DATUM);

                item = (struct darray_s **)csv->darr->da_values;
                for (n=0; n<csv->records; n++) {
                        darray_free(item[n]);
                }

                darray_free(csv->darr);
                free(csv);
        } 
        return NULL;
}


/*
get data of csv  
*/
char* csv_data(struct csv_s *csv, size_t n, size_t col)
{
        struct darray_s **item;
        char **p;

        if (csv == NULL) return NULL;
        if (n > csv->records || col> csv->columns)
                return NULL;

        item = (struct darray_s **)csv->darr->da_values;
        p = (char **)(item[n]->da_values + col*item[n]->da_size);

        if (*p) return *p;
        else return "";
}

/*
get data of csv  
*/
char* csv_data_f(struct csv_s *csv, size_t n, const char* field) 
{
        size_t m=0;
        struct item_s *fitem;
        
        if (csv == NULL) return NULL;
        fitem  = (struct item_s*)hash_search(csv->fhash, field);
        if (fitem == NULL) return NULL;
        m = fitem->idx;

        return csv_data(csv, n, m);
}

/*
get data of csv  
*/
char* csv_data_id(struct csv_s *csv, const char* id, size_t m) 
{
        size_t n=0;
        struct item_s *fitem;
        
        if (csv == NULL) return NULL;
        fitem  = (struct item_s*)hash_search(csv->khash, id);
        if (fitem == NULL) return NULL;
        n = fitem->idx;

        return csv_data(csv, n, m);
}

/*
get data of csv  
*/
char* csv_data_fid(struct csv_s *csv, const char* id, const char* field) 
{
        size_t n=0, m=0;
        struct item_s *fitem;
        
        if (csv == NULL) return NULL;
        fitem  = (struct item_s*)hash_search(csv->khash, id);
        if (fitem == NULL) return NULL;
        n = fitem->idx;
        fitem  = (struct item_s*)hash_search(csv->fhash, field);
        if (fitem == NULL) return NULL;
        m = fitem->idx;

        return csv_data(csv, n, m);
}

