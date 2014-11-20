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

//计算函数
int count(lua_State *L)
{
    //得到UpValue
    double m_ = lua_tonumber(L, lua_upvalueindex(1));
    printf("_m:%lf\n", m_);
    //更改UpValue
    lua_pushnumber(L, ++m_);
    lua_replace(L, lua_upvalueindex(1));
    //返回结果（直接复制一份UpValue作为结果）
    lua_pushvalue(L, lua_upvalueindex(1));
    return 1; 
}


//工厂函数，把一个数字和count函数关联打包后返回闭包。
int newCount(lua_State *L)
{
    //计数器初值（即UpValue）
    lua_pushnumber(L,0);
    //放入计算函数，告诉它与这个函数相关联的数据个数
    lua_pushcclosure(L, count, 1);
    return 1;//一个结果,即函数体
}

int main()
{
    char *szLua_code =
        "c1 = NewCount() "
        "c2 = NewCount() "
        "for i=1,5 do print(c1()) end "
        "for i=1,5 do print(c2()) end";

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    //放入C函数
    lua_pushcfunction(L,newCount) ;     //注册在lua中使用的c函数  
    lua_setglobal(L,"NewCount") ;       //绑定到lua中的名字csum  

    //执行
    int err = luaL_loadstring(L, szLua_code) || lua_pcall(L, 0, 0, 0);
    if(err)
    {
        printf("err:%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    lua_close(L);
    return 0;
}

