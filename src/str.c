/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "str.h"

/*
remove the space, enter, new line, tab...from heading and trailing of the string 

@str the string needed to trim. 
return the string trimed if succeed, otherwise NULL on error.
*/
char * trim(char *str)
{
        if (str == NULL) return NULL;

        return rtrim(ltrim(str));       
}

/*
remove the space, enter, new line, tab...from heading of the string 
@ str in/out parameters
return the string trimed if succeed, otherwise NULL on error.
*/
char * ltrim(char *str)
{
        int pos = 0;
        char *head;

        if (str == NULL) return NULL;
        head = str;

        while ((*head) == ' ' || (*head) == '\t'
                 || (*head) == '\r' || (*head) == '\n') {
                head++;
                pos++;
        }
        
        if (pos) memmove(str, head, strlen(head)+1);

        return str;
}

/*
remove the space, enter, new line, tab...from trailing of the string, 
@ str in/out parameters
return the string trimed if succeed, otherwise NULL on error.
*/
char * rtrim(char *str)
{
        char *trail=NULL;

        if (str == NULL) return NULL;
        trail = str + strlen(str);        /*pointer to terminating '\0' character */

        while(trail > str) {
            trail--;
            if (*trail == ' ' || *trail == '\t' 
                || *trail == '\r' || *trail == '\n') 
                continue;

            trail++;
            break;
        }
        (*trail) = '\0';
        
        return str;
}

/*
count of the char @find in the @str. 
        example @str="abceaaa", @find='a' then return 4
@str  original string with terminating '\0' character
@find character that need find
return  count of character @find 

*/
size_t strcount(char *str, char find)
{
        size_t count = 0;
	if (strlen(str) == 0) return count;

        while(1) {
                if (*str == 0x00) return count;
                if (*str == find) count++;
                str++;      
        }

}



/*

to be require free(result) on call location
*/
char * strsub(char *str, int start, int len)
{
        char *sub = NULL;

	if (strlen(str) == 0) return NULL;
        if ((int)strlen(str) < (start+len)) return NULL;

        sub = (char*)calloc(1, len+1);
        if (sub == NULL) return NULL;

        memcpy(sub, str+start, len);
        
        return sub; 
}



/**/
char* strupper(char *src)
{
        char *ptr = src;

        while (*src) {
                if (*src >= 'a' && *src <= 'z') 
                        *src = *src - ('a' - 'A');
                src++;
        }
        
        return ptr;
}


char* strlower(char *src)
{
        char *ptr = src;
        while (*src) {
                if (*src >= 'A' && *src <= 'Z') 
                        *src = *src + ('a' - 'A');
                src++;
        }
        
        return ptr;

}

int strpos(char *str, char *find)
{
        char* pos = strstr(str, find);
        if (pos == NULL) return -1;
        return pos - str;
}

int strlastpos(char *str, char *find)
{
        char* lastpos = NULL, *pos = str;
        
        while ((pos = strstr(++pos, find)) != NULL) {
                printf("pos:%p\n",pos);
                lastpos = pos;
        }
        
        if (lastpos == NULL) return -1;
        return lastpos - str;
}


