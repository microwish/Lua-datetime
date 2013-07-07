/**
 * datetime module compatible with PHP's datetime lib
 * @author microwish@gmail.com
 */
#include <timelib/timelib.h>
#include <timelib/timezonedb.h>

#include <lua.h>
#include <lauxlib.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define MICRO_IN_SEC 1000000.00
#define SEC_IN_MIN 60

#define DEFAULT_TIMEZONE "Asia/Shanghai"

/***************
 * timezonedb.h:const timelib_tzdb timezonedb_builtin = { "2012.3", 576, timezonedb_idx_builtin, timelib_timezone_db_data_builtin };
 ***************/

static timelib_tzinfo *get_timezone_info(char *tz_str)
{
	timelib_tzinfo *tzip;

	tzip = timelib_parse_tzfile(tz_str, &timezonedb_builtin);

	return tzip;
}

static int l_gettimeofday(lua_State *L)
{
	int return_float = 0;

	switch (lua_gettop(L)) {
		case 1:
			return_float = lua_toboolean(L, 1);
		case 0:
			break;
		default:
			lua_pushboolean(L, 0);
			return 1;
	}

	struct timeval tv = {0};

	if (gettimeofday(&tv, NULL)) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if (return_float) {
		lua_pushnumber(L, (lua_Number)(tv.tv_sec + tv.tv_usec / MICRO_IN_SEC));
		return 1;
	}

	timelib_time_offset *offset = timelib_get_time_zone_info(tv.tv_sec, get_timezone_info(DEFAULT_TIMEZONE));

	lua_createtable(L, 0, 4);
	lua_pushnumber(L, tv.tv_sec);
	lua_setfield(L, -2, "sec");
	lua_pushnumber(L, tv.tv_usec);
	lua_setfield(L, -2, "usec");
	lua_pushnumber(L, -offset->offset / SEC_IN_MIN);
	lua_setfield(L, -2, "minuteswest");
	lua_pushnumber(L, offset->is_dst);
	lua_setfield(L, -2, "dsttime");

	timelib_time_offset_dtor(offset);

	return 1;
}

static int l_microtime(lua_State *L)
{
	int return_float = 0;

	switch (lua_gettop(L)) {
		case 1:
			return_float = lua_toboolean(L, 1);
		case 0:
			break;
		default:
			lua_pushboolean(L, 0);
			lua_pushliteral(L, "argument error");
			return 2;
	}

	struct timeval tv = {0};

	if (gettimeofday(&tv, NULL)) {
		lua_pushboolean(L, 0);
		lua_pushliteral(L, "Internal error");
		return 2;
	}

	if (return_float) {
		lua_pushnumber(L, (lua_Number)(tv.tv_sec + tv.tv_usec / MICRO_IN_SEC));
		return 1;
	} else {
		char ret[100];
		int l;
		l = snprintf(ret, 100, "%.8F %ld", tv.tv_usec / MICRO_IN_SEC, tv.tv_sec);
		if (l < 0) {
			lua_pushboolean(L, 0);
			lua_pushliteral(L, "Internal error");
			return 2;
		}
		lua_pushlstring(L, ret, l);
		return 1;
	}
}

static const luaL_Reg l_funcs[] = {
	{ "gettimeofday", l_gettimeofday },
	{ "microtime", l_microtime },
	{ NULL, NULL }
};

static int setreadonly(lua_State *L)
{
	return luaL_error(L, "Must not update the read-only");
}

#define LUA_DATETIMELIBNAME "datetime"

LUALIB_API int luaopen_datetime(lua_State *L)
{
	//main table for this module
	lua_newtable(L);

	//metatable
	lua_createtable(L, 0, 2);

	luaL_register(L, LUA_DATETIMELIBNAME, l_funcs);

	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, setreadonly);
	lua_setfield(L, -2, "__newindex");

	lua_setmetatable(L, -2);

	return 1;
}
