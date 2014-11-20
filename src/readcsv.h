#ifndef __READ_CSV_H__
#define __READ_CSV_H__

#include "utl.h"
#include "darray.h"
#include "file.h"
#include "dstr.h"


struct csv_s {
        char * filename;
        uint32_t columns;
        uint32_t records;
        struct hash_s *fhash;
        struct hash_s *khash;
        struct darray_s *darr;       /* csv varint array*/
};

#ifdef __cplusplus
extern "C" {
#endif


/*
create struct csv_s and load data into memory.
 */
struct csv_s* parse_csv(char *data, size_t len);


/* 
从二维数组提取数据
按行号，列号提取
n:行号
col:列号
 */
char* csv_data(struct csv_s *csv, size_t n, size_t col);


/*
按行号，字段名提取
n:行号
field:字段名
*/
char* csv_data_f(struct csv_s *csv, size_t n, const char* field);

/*
按ID检索记录，提取指定列号的数据
id:id所在行
m:列号
*/
char* csv_data_id(struct csv_s *csv, const char* id, size_t m);


/*
按ID检索记录，通过字段名提取
id:id所在行
field:字段名
*/
char* csv_data_fid(struct csv_s *csv, const char* id, const char* field);

/*
free
*/
void free_csv(struct csv_s *csv);

        

#ifdef __cplusplus
}
#endif


#endif 

