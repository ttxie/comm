/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "luautl.h"


void stackhump(lua_State* L)
{
    int i;
    int top = lua_gettop(L);   // 获取栈中元素数量
    for ( i=1; i<=top; i++)    // 遍历
    {
        int t = lua_type(L,i); // 获取元素类型
        switch(t)
        {
            case LUA_TSTRING:      // 字符串
                printf("'%s'",lua_tostring(L,i));
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(L,i)?"true":"false");
                break;
            case LUA_TNUMBER:      // 数字
                printf("%g",lua_tonumber(L,i));
                break;
            default:               // 其它值
                printf("%s",lua_typename(L,t));
                break;
        }
        printf("\t");
    }
    printf("\n");
}




void lua_err(lua_State* L, int err, struct log_s *log)
{  
    const char* error;  
    char errstr[256]={0};  

    if (err == 0) return;

    switch(err)  {  
        case LUA_ERRSYNTAX://编译时错误  
            strcpy(errstr, "syntax error during pre-compilation");  
            break;  
        case LUA_ERRMEM://内存错误  
            strcpy(errstr, "memory allocation error");  
            break;  
        case LUA_ERRRUN://运行时错误  
            strcpy(errstr, "a runtime error");  
            break;  
        case LUA_YIELD://线程被挂起错误  
            strcpy(errstr, "Thread has Suspended");  
            break;  
        case LUA_ERRERR://在进行错误处理时发生错误  
            strcpy(errstr, "error while running the error handler function");  
            break;  
        default:  
            break;  
    }  

    error = lua_tostring(L, -1);//打印错误结果  
    if (log == NULL)
        printf("[%s]: %s\n", errstr, error);  
    else 
        log_printf(log, "[%s]: %s\n", errstr, error); 

    lua_pop(L, 1);   
}  

