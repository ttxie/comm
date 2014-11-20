#include <sys/stat.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "utl.h"

static void stackDump(lua_State* L)
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


time_t old_modi;
static void reload(lua_State* L)
{
    uint64_t start, end;
    static time_t mt;
    struct stat fs;

    start = tm64();
    stat("win.lua", &fs);
    end = tm64();
    printf("stat:%llu\n", end-start);
    if (old_modi != fs.st_mtime) {
        old_modi = fs.st_mtime;

        start = tm64();
        luaL_loadfile(L, "win.lua");
        end = tm64();
        printf("loadfile:%llu\n", end-start);

stackDump(L);
        start = tm64();
        lua_pcall(L, 0, 0, 0);
        end = tm64();
        printf("pcall:%llu\n", end-start);
    }
}


int main()
{
    uint64_t start, end;
    int new=0;
    lua_State *L = NULL;

    start = tm64();
    L = luaL_newstate();
    end = tm64();
    printf("newstate:%llu\n", end-start);

    lua_pushboolean(L, 1);
    lua_pushnumber(L, 10);
    lua_pushnil(L);
    lua_pushstring(L, "hello");
    stackDump(L);

    lua_remove(L, -3);
    stackDump(L);

    lua_settop(L, 0);
    stackDump(L);

    while(1) {
        reload(L);

/*
数组模式:
        lua_getglobal(L, "tt");
stackDump(L);
        lua_pushnumber(L, 1);
        lua_gettable(L, -2);
stackDump(L);
        lua_pushstring( L, "width" );
        lua_gettable( L, -2 );
stackDump(L);
        printf("os_width:%d\n", lua_tointeger(L, -1));

lua_settop(L, 0);
*/



        lua_getglobal(L, "tt");
stackDump(L);
        //lua_getfield(L, -1, "iOS");
        lua_pushstring(L, "android");
        lua_gettable(L, -2);
stackDump(L);

        lua_pushstring( L, "width" );
        lua_gettable( L, -2 );
stackDump(L);

        printf("os_width:%d\n", lua_tointeger(L, -1));



        sleep(10);
    }

}

