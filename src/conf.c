/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "conf.h"

#define MAXLINE 1024


/*
 * allocate the memory for conf_s consture
 * @count: count of key-value item 
 * return: pointer to conf_s structure on success or NULL on error.
 */
static struct conf_s* conf_alloc(int count, char *file)
{
        struct conf_s *conf;
        int n=0;
        int mem=0;

        if (file == NULL) return NULL;

        conf = (struct conf_s*)calloc(1, sizeof(struct conf_s));
        if (conf == NULL) return NULL;

        conf->conf_file = (char*)calloc(1, strlen(file)+1);
        if (conf->conf_file == NULL) {
                free(conf);
                return NULL;
        }
        strcpy(conf->conf_file, file);

        conf->conf_items = (struct conf_item_s*)calloc(count, sizeof(struct conf_item_s));
        if (conf->conf_items == NULL) {
                free(conf->conf_file);
                free(conf);
                return NULL;
        }
 
        conf->conf_count = count;

        return conf;
}


/*
 * add the item into the conf_s structure from line of file
 *  
 * return 0 on success, or -1 if error occurred.
 */
static int add_item(struct conf_s *conf, char *section, char *line, int *idx)
{
        char *key, *val, *sec, *p;
        
        p = strchr(line, '=');
        if (p == (line+strlen(line)-1) || p == line) return 0;   /*format err, have no key or have no val */  
        
        /*allocate memory for config item*/
        key = (char *)calloc(1, p-line+1);
        val = (char *)calloc(1, (line+strlen(line)-p));
        if (key == NULL || val == NULL) {
                if (key != NULL) free(key);
                return -1;
        }
        
        memcpy(key, line, p-line);
        memcpy(val, p+1, (line+strlen(line)-p-1));

        conf->conf_items[*idx].item_section  = NULL;
        if (section != NULL) {
                sec = (char *)calloc(1, strlen(section)+1);
                if (sec == NULL) {
                        free(key);
                        free(val);
                        return -1;
                }
                strcpy(sec, trim(section));
                conf->conf_items[*idx].item_section  = sec;
        }
        conf->conf_items[*idx].item_key = trim(key);
        conf->conf_items[*idx].item_value = trim(val);
        (*idx)++;
        
        return 0; 
}


/*
 * count the item
 */
static int get_item_count(FILE *fd)
{
        int count = 0;        
        char *line = NULL;

        if (fd == NULL) return -1;
        line = (char *)calloc(1, MAXLINE);
        if (line == NULL) {
                return -1;
        }

        while (fgets(line, MAXLINE, fd)) {
                line = ltrim(line);
                if (strchr(line, '=') != NULL) { /* add key-val to conf structure */
                        char *p = strchr(line, '=');
                        if (p == (line+strlen(line)-1) || p == line) {
                                memset(line, 0, MAXLINE);
                                continue;   /*format err, have no key or have no val */  
                        } else 
                                count++;
                }
                memset(line, 0, MAXLINE);
        }

        if (line != NULL) free(line);
        fseek(fd, 0L, SEEK_SET);

        return count;
} 


/*
load the conf item into memory

@file config file name
return pointer to structure on success, NULL on failure
*/
struct conf_s* conf_init(const char *file)
{
        FILE *fd;
        char *line, *comment;
        char section[64];
        struct conf_s *conf; 
        int count=0, idx=0;
        

        if (file == NULL) return NULL;

        fd = fopen(file, "r");
        if (fd == NULL)  return NULL;
        
        if ((count = get_item_count(fd)) <= 0) {
                fclose(fd);
                return NULL;
        }

        conf = conf_alloc(count, (char *)file);
        if (conf == NULL) {
                fclose(fd);
                return NULL;
        }
        
        line = (char *)calloc(1, MAXLINE);
        if (line == NULL) {
                fclose(fd);
                conf_free(conf);
                return NULL;
        }
        /*load the key and value*/
        while (fgets(line, MAXLINE, fd)) {
                line = trim(line);
                comment = strchr(line, '#');
                if (comment != NULL) *comment = '\0';

                if (strlen(line) == 0) goto LOOP;  /*empty line*/
                //if (*line == '#') goto LOOP;     /*commenting line*/

                if (*line == '[' && line[strlen(line)-1] == ']') { /*get section*/
                        memcpy(section, line+1, strlen(line)-2);
                        section[strlen(line)-2] = '\0';
                        goto LOOP;
                }
                if (strchr(line, '=') != NULL) { /* add key-val to conf structure */
                        if (add_item(conf, section, line, &idx) == -1) {
                                free(line);
                                conf_free(conf);
                                fclose(fd);
                                return NULL;
                        }
                }
LOOP:
                memset(line, 0, MAXLINE);
                continue;
        }

/*list the item for debug*/

#if 0
        int n = 0;
        struct conf_item_s item;

        printf("file:%s, item count:%d\n", conf->conf_file, (int)conf->conf_count);
        for (n=0; n<(int)conf->conf_count; n++) {
                item = conf->conf_items[n];
                printf("[idx:%d][section:%s][key:%s][value:%s]\n", 
                        n, item.item_section, item.item_key, item.item_value);
        }
#endif

        free(line);
        fclose(fd);
        return conf;
}


/*
get the item from memory

@conf structure
@section
@key of item
return value on found, return NULL if not found.
*/
char* conf_get(struct conf_s *conf, const char *section, const char *key)
{
        unsigned n = 0;
        struct conf_item_s item;

        if (conf == NULL) return NULL;

        for (n=0; n<conf->conf_count; n++) {
                item = conf->conf_items[n]; 
                if (item.item_section == NULL) return NULL;

                if (section == NULL) {
                        if (strcmp(key, item.item_key) == 0)
                                return item.item_value;
                } else {
                        if (strcmp(section, item.item_section) == 0 
                                && strcmp(key, item.item_key) == 0)
                                return item.item_value;
                }
        }

        return NULL;
}


/*
destory the config item in  memory

*/
void conf_free(struct conf_s *conf)
{
        unsigned n = 0;
        
        if (conf == NULL) return ;

        for (n=0; n<conf->conf_count; n++) {
                if (conf->conf_items[n].item_section != NULL) {
                        free(conf->conf_items[n].item_section);
                        conf->conf_items[n].item_section = NULL;
                }
                if (conf->conf_items[n].item_key != NULL) {
                        free(conf->conf_items[n].item_key);
                        conf->conf_items[n].item_key = NULL;
                }
                if (conf->conf_items[n].item_value != NULL) {
                        free(conf->conf_items[n].item_value);
                        conf->conf_items[n].item_value = NULL;
                }
        }
        
        if (conf->conf_items != NULL) free(conf->conf_items);
        if (conf->conf_file != NULL) free(conf->conf_file);
        if (conf != NULL) free(conf);
}

