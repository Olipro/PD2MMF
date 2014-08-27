#pragma once

#define LUA_MULTRET (-1)
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)
#define lua_upvalueindex(i)   (LUA_GLOBALSINDEX-(i))
#define lua_pushcfunction(L, f)	lua_pushcclosure(L, f, 0)
#define lua_tostring(L, i)		lua_tolstring(L, i, 0)
#define lua_getglobal(L, f)		lua_getfield(L, LUA_GLOBALSINDEX, f)
#define lua_setglobal(L, f)		lua_setfield(L, LUA_GLOBALSINDEX, f)
#define luaL_register(L, i, b)	luaL_openlib(L, i, b, 0)
#define lua_pop(L,n)			lua_settop(L, -(n)-1)

enum LuaTypes {
	LUA_TNONE = -1,
	LUA_TNIL,
	LUA_TBOOLEAN,
	LUA_TLIGHTUSERDATA,
	LUA_TNUMBER,
	LUA_TSTRING,
	LUA_TTABLE,
	LUA_TFUNCTION,
	LUA_TUSERDATA,
	LUA_TTHREAD,
};

enum LuaBool
{
	LUA_TFALSE = 0,
	LUA_TTRUE = 1,
};

enum HookCallbackType
{
	NEWSTATE_CALLBACK,
	GAMETICK_CALLBACK,
	REQUIRE_CALLBACK,
};

#ifndef WINAPI
#define WINAPI __stdcall
#endif

typedef void lua_State;
typedef lua_State* pLua_State;
typedef int(*lua_CFunction) (lua_State *L);
typedef const char*(*lua_Reader) (lua_State *L, void *data, size_t *size);
typedef int(*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);
typedef void(WINAPI* CallbackFunction)(lua_State*, LPCSTR, LPVOID);
struct CallbackContainer {
	CallbackFunction func;
	LPVOID ptr;
};
#ifndef luaL_reg
typedef struct luaL_Reg {
	const char *name;
	lua_CFunction func;
} luaL_Reg;
#define luaL_reg luaL_Reg
#endif

#ifndef PAYDAY2HOOK_EXPORTS

#define EXPORT	extern "C"
#pragma comment(lib, "PD2HookAPI.lib")

#else

#define EXPORT __declspec(dllexport) static
#define ELc(x)	((decltype(HookAPI::##x##)*) LuaHooking::Lua[PD2Functions::##x##].lpAddr)

class HookAPI
{

#endif
	EXPORT void RegisterCallback(HookCallbackType, CallbackFunction, LPVOID);
	EXPORT int luaL_ref(lua_State *L, int t);
	EXPORT void luaL_unref(lua_State *L, int t, int ref);
	EXPORT void luaL_openlib(lua_State *L, const char *libname, const luaL_reg *l, int nup);
	EXPORT int luaL_loadstring(lua_State *L, const char *s);
	EXPORT int luaL_loadfile(lua_State *L, const char *filename);
	EXPORT int lua_pcall(lua_State *L, int nargs, int nresults, int errfunc);
	EXPORT int lua_load(lua_State *L, const char *chunk, size_t len, const char *chunkname);
	EXPORT int lua_call(lua_State *L, int nargs, int nresults);
	EXPORT void lua_concat(lua_State *L, int n);
	EXPORT void lua_createtable(lua_State *L, int narr, int nrec);
	EXPORT int lua_getmetatable(lua_State *L, int idx);
	EXPORT int lua_setmetatable(lua_State *L, int idx);
	EXPORT void lua_gettable(lua_State *L, int idx);
	EXPORT void lua_settable(lua_State *L, int idx);
	EXPORT lua_State* lua_newthread(lua_State *L);
	EXPORT void* lua_newuserdata(lua_State *L, size_t size);
	EXPORT void lua_xmove(lua_State *from, lua_State *to, int n);
	EXPORT int lua_toboolean(lua_State *L, int idx);
	EXPORT void* lua_tocfunction(lua_State *L, int idx);
	EXPORT const char* lua_tolstring(lua_State *L, int idx, size_t *len);
	EXPORT float lua_tonumber(lua_State *L, int idx);
	EXPORT int lua_tointeger(lua_State *L, int idx);
	EXPORT void* lua_topointer(lua_State *L, int idx);
	EXPORT void* lua_touserdata(lua_State *L, int idx);
	EXPORT void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
	EXPORT void lua_pushfloat(lua_State *L, float n);
	EXPORT void lua_pushinteger(lua_State *L, int n);
	EXPORT void lua_pushlightuserdata(lua_State *L, void *ptr);
	EXPORT void lua_pushboolean(lua_State *L, LuaBool b);
	EXPORT void lua_pushstring(lua_State *L, const char* str);
	EXPORT const char* lua_pushfstring(lua_State *L, const char* fmt, ...);
	EXPORT void lua_pushlstring(lua_State *L, const char *s, size_t len);
	EXPORT const char* lua_pushvfstring(lua_State *L, const char* fmt, va_list argp);
	EXPORT int lua_pushthread(lua_State *L);
	EXPORT void lua_pushvalue(lua_State *L, int idx);
	EXPORT void lua_pushnil(lua_State *L);
	EXPORT size_t lua_objlen(lua_State *L, int idx);
	EXPORT int lua_type(lua_State *L, int idx);
	EXPORT void lua_insert(lua_State *L, int idx);
	EXPORT void lua_remove(lua_State *L, int idx);
	EXPORT void lua_replace(lua_State *L, int idx);
	EXPORT int lua_next(lua_State *L, int idx);
	EXPORT void lua_setfield(lua_State *L, int idx, const char *k);
	EXPORT void lua_getfield(lua_State *L, int idx, const char *k);
	EXPORT int lua_gettop(lua_State *L);
	EXPORT void lua_settop(lua_State *L, int idx);
	EXPORT int lua_equal(lua_State *L, int idx1, int idx2);
	EXPORT int lua_lessthan(lua_State *L, int idx1, int idx2);
	EXPORT void lua_rawget(lua_State *L, int idx);
	EXPORT void lua_rawset(lua_State *L, int idx);
	EXPORT void lua_rawgeti(lua_State *L, int idx, int n);
	EXPORT void lua_rawseti(lua_State *L, int idx, int n);
	EXPORT int lua_dump(lua_State *L, lua_Writer w, void* data);
#ifdef PAYDAY2HOOK_EXPORTS
public:
	static vector<CallbackContainer> NewStateCallbacks;
	static vector<CallbackContainer> GameTickCallbacks;
	static vector<CallbackContainer> RequireCallbacks;
};
#endif
