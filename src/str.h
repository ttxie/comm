/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef  _COMM_STR_H_
#define _COMM_STR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utl.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
remove the space, enter, new line, tab...from heading and trailing of the string 

@str the string needed to trim. 
return the string trimed if succeed, otherwise NULL on error.
*/
char * trim(char *str);

/*
remove the space, enter, new line, tab...from heading of the string 
return the string trimed if succeed, otherwise NULL on error.
*/
char * ltrim(char *str);

/*
remove the space, enter, new line, tab...from trailing of the string 
return the string trimed if succeed, otherwise NULL on error.
*/
char * rtrim(char *str);


/*
count of the char @find in the @str. 
        example @str="abceaaa", @find='a' then return 4
@str  original string with terminating '\0' character
@find character that need find
return  count of character @find 

*/
size_t strcount(char *str, char find);


char *strsub(char *str, int start, int len);


//char *strreplace(char *str, char *old, char *new);

int strpos(char *str, char *find);
int strlastpos(char *str, char *find);


/*


*/
char* strupper(char *src);
char* strlower(char *src);


#ifdef __cplusplus
}
#endif

#endif
