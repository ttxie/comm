/*
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "hash.h"
#include "utl.h"
#include "file.h"

#define MAXLINE 100 


int main(int argc, char** argv)
{
	FILE *fdbl, *fdsrc, *fdresult;
	char *line, *lndata; 	/*带换行符*/
	char buf[200], fn[200], hduan[8], hao[12];
	char ch;
	struct hash_s *hash;
	struct hash_node_s *hnode;
	int total, filter, line_num, cur;
	
	if(argc != 4){
		printf("Usage: ./search 1 filename1 filename2\n");;
		return 0;
	}

	/*打开黑名单文件*/
	fdbl = fopen(argv[2], "r");
	if (fdbl == NULL) {
		printf("file open fail \n");	
		exit(1);
	}

	/*取文件行数*/
	line_num = linecount_fd(fileno(fdbl));
        /*
	while ((ch=fgetc(fdbl)) != EOF) if (ch == 0x0a) line_num++;
        if (fseek(fdbl, 0L, SEEK_SET) == -1) {
                printf("file seek err:%d,%s\n", errno, strerror(errno));
                exit(1);
        }*/

	line = (char*)calloc(line_num, MAXLINE);
	if (line == NULL) {
		printf("memory calloc err\n");
		exit(1);
	}
        
	/*建立hashtable, 并将黑名单逐行加入hashtable*/
	hash = hash_create(line_num);
	if(hash == NULL) {
		printf("hash create err\n");
		exit(1);
	}

	printf("file line count:%d\n", line_num);

	cur = 0;
	while (fgets(line+cur*MAXLINE, MAXLINE, fdbl)) {
                lndata = line+cur*MAXLINE;
		//line[cur*MAXLINE + strlen(line+cur*MAXLINE) - 1] = '\0';  /*去换行符*/
                trim(lndata);
                //printf("line:%s, len:%d\n", line+cur*MAXLINE, strlen(line+cur*MAXLINE));
                /*
		while (line[cur*MAXLINE + strlen(line+cur*MAXLINE) - 1] == '\r' ||
			line[cur*MAXLINE + strlen(line+cur*MAXLINE) - 1] == '\t' ||
			line[cur*MAXLINE + strlen(line+cur*MAXLINE) - 1] == ' ')
			line[cur*MAXLINE + strlen(line+cur*MAXLINE) - 1] = '\0';  去DOS ^M换行符, TAB, 空格*/

		if (strlen(line+cur*MAXLINE) > 0) {
                        if (hash_insert(hash, lndata, lndata, 1) == -1) {
                                printf("hash_insert err:%d, %s\n", errno, strerror(errno));
                                //if (errno != EEXIST)
                                //      printf("hash insert err.\n");			
                        }
		}
		cur++;
	}
        hash_stat(hash);
	//printf("number of hash node:%d ht_slots_used:%d ht_slots_used:%d\n", hash->ht_nel, hash->ht_slots_used, hash->ht_max_chain_len);
	printf("number of hash node:%d \n", hash->ht_nel);

	/*打开源文件，与黑名单文件比较*/
	fdsrc = fopen(argv[3], "r");
	if (fdsrc == NULL) {
		printf("src file open fail \n");	
		exit(1);
	}
	
	memset(fn, 0, 200);	
	strcat(fn, argv[3]);
	strcat(fn, argv[2]);
	fdresult = fopen(fn, "a");
	if (fdresult == NULL) {
		printf("result file open fail \n");	
		exit(1);
	}
	
	total = 0;
	filter = 0;
	while (fgets(buf, MAXLINE, fdsrc)) {
                trim(buf);
		//buf[strlen(buf)-1] = '\0';
		if (strcmp(argv[1], "2") == 0) { /*过滤黑名单*/
			memset(hduan, 0, 8);
			strncpy(hduan, buf, 7);
			memset(hao, 0, 12);
			strncpy(hao, buf, 11);
			if (hash_search(hash, hduan) == NULL && hash_search(hash, hao) == NULL) {
				fputs(buf, fdresult);
				fputs("\r\n", fdresult);
			} else filter++;
		}

		if (strcmp(argv[1], "1") == 0) { /*取交集*/
			memset(hao, 0, 12);
			strncpy(hao, buf, 11);
			if (hash_search(hash, hao) != NULL) {
				fputs(buf, fdresult);
				fputs("\r\n", fdresult);
			} else filter++;
		}

                memset(buf, 0, sizeof(buf));
		total++;
	}

	/*summary*/
	sprintf(buf, "src total line:%d\t filter line:%d\t result:%d\n", total, filter, total-filter);	
	printf("%s", buf);	
	fputs(buf, fdresult);

	free(line);

	fclose(fdbl);
	fclose(fdresult);
	fclose(fdsrc);

	/*清空hash表*/
//	hash_map(hash, free_hnode, NULL);
	hash_free(hash);  /*非常低效率*/

}

