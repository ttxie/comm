/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef  _LUA_UTL_H_
#define _LUA_UTL_H_

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 
#include <WinSock2.h>
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "log.h"


#ifdef __cplusplus
extern "C" {
#endif

void stackhump(lua_State* L);

void lua_err(lua_State* L, int err, struct log_s *log);


#ifdef __cplusplus
}
#endif

#endif


