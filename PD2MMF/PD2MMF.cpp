#include "stdafx.h"
#include "PD2MMF.h"

int nullFunction(lua_State *L)
{
	lua_pushinteger(L, 0);
	return 1;
}

PD2MMF::PD2MMF()
{
	RegisterCallback(NEWSTATE_CALLBACK, (CallbackFunction)&PD2MMF::LuaNewState, this);
	RegisterCallback(REQUIRE_CALLBACK, (CallbackFunction)&PD2MMF::LuaPostRequire, this);
}

void PD2MMF::doDisableAchievements(lua_State *L)
{
	lua_getglobal(L, "AchievmentManager");
	lua_pushcfunction(L, &nullFunction);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, "award");
	lua_setfield(L, -2, "award_progress");
}

void PD2MMF::doDisableXP(lua_State *L)
{
	lua_getglobal(L, "ExperienceManager");
	lua_getglobal(L, "Application");
	lua_getfield(L, -1, "digest_value");
	lua_insert(L, -2);
	lua_pushinteger(L, 0);
	lua_pushboolean(L, LUA_TTRUE);
	lua_call(L, 3, 1);
	lua_setfield(L, -2, "LEVEL_CAP");
}

void PD2MMF::doDisableMoney(lua_State *L)
{
	lua_getglobal(L, "MoneyManager");
	lua_pushcfunction(L, &nullFunction);
	lua_pushvalue(L, -1);
	lua_pushvalue(L, -1);
	lua_pushvalue(L, -1);
	lua_pushvalue(L, -1);
	lua_setfield(L, -6, "on_mission_completed");
	lua_setfield(L, -5, "civilian_killed");
	lua_setfield(L, -4, "on_buy_mission_asset");
	lua_setfield(L, -3, "on_buy_preplanning_type");
	lua_setfield(L, -2, "on_buy_preplanning_votes");
}

void PD2MMF::doDisableSaving(lua_State *L)
{
	lua_getglobal(L, "SaveFileManager");
	lua_getfield(L, -1, "_save");
	lua_pushcclosure(L, &PD2MMF::LuaSaveCheck, 1);
	lua_setfield(L, -2, "_save");
}

void PD2MMF::doDisableStatistics(lua_State *L, bool isNetworkAccountSteam)
{
	if (isNetworkAccountSteam)
	{
		lua_getglobal(L, "NetworkAccountSTEAM");
		lua_pushcfunction(L, &nullFunction);
		lua_setfield(L, -2, "publish_statistics");
		return;
	}
	lua_getglobal(L, "StatisticsManager");
	lua_getfield(L, -1, "_reset_session");
	lua_setfield(L, -2, "start_session");
	lua_pushcfunction(L, &nullFunction);
	for (int i = 0; i < 11; i++)
		lua_pushvalue(L, -1);
	lua_setfield(L, -13, "stop_session");
	lua_setfield(L, -12, "_increment_misc");
	lua_setfield(L, -11, "publish_to_steam");
	lua_setfield(L, -10, "reloaded");
	lua_setfield(L, -9, "downed");
	lua_setfield(L, -8, "shot_fired");
	lua_setfield(L, -7, "objective_completed");
	lua_setfield(L, -6, "camera_destroyed");
	lua_setfield(L, -5, "revived");
	lua_setfield(L, -4, "tied");
	lua_setfield(L, -3, "completed_job");
	lua_setfield(L, -2, "killed");
}

void PD2MMF::doDisableLootdrop(lua_State *L)
{
	lua_getglobal(L, "LootDropManager");
	lua_getfield(L, -1, "_new_make_drop");
	lua_pushcclosure(L, (lua_CFunction)static_cast<int(__cdecl *)(lua_State*)>([](lua_State *L) -> int {
		lua_pushboolean(L, LUA_TFALSE);
		lua_replace(L, 3);
		lua_pushvalue(L, lua_upvalueindex(1));
		lua_insert(L, 1);
		lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
		return lua_gettop(L);
	}), 1);
	lua_setfield(L, -2, "_new_make_drop");
}

void WINAPI PD2MMF::LuaNewState(lua_State *L, LPCSTR _, PD2MMF *self)
{
	lua_pushlightuserdata(L, self);
	lua_pushcclosure(L, PD2MMF::LuaRegisterMod, 1);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "RegisterMutator");
	lua_setglobal(L, "RegisterMod");
	self->mods.clear();
}

void PD2MMF::setupModKey(lua_State *L)
{
	if (this->mods.empty())
		return;
	lua_getglobal(L, "NetworkMatchMakingSTEAM");
	lua_getfield(L, -1, "_BUILD_SEARCH_INTEREST_KEY");
	string sKey = lua_tostring(L, -1);
	lua_remove(L, -1);
	string digestMe = "";
	for (auto i = this->mods.begin(); i != this->mods.end(); i++)
	{
		digestMe += i->first;
		digestMe += i->second;
	}
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;
	CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0);
	CryptCreateHash(hCryptProv, CALG_SHA1, 0, 0, &hHash);
	CryptHashData(hHash, (const LPBYTE)digestMe.c_str(), digestMe.length(), 0);
	DWORD hashSize, dwLen = sizeof(DWORD);
	CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&hashSize, &dwLen, 0);
	unique_ptr<BYTE[]> hashBuf(new BYTE[hashSize]);
	CryptGetHashParam(hHash, HP_HASHVAL, hashBuf.get(), &hashSize, 0);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hCryptProv, 0);
	ostringstream tmp;
	for (DWORD i = 0; i < hashSize; i++)
		tmp << hex << (DWORD)hashBuf[i];
	sKey += tmp.str();
	lua_pushstring(L, sKey.c_str());
	lua_setfield(L, -2, "_BUILD_SEARCH_INTEREST_KEY");
}

void WINAPI PD2MMF::LuaPostRequire(lua_State *L, LPCSTR file, PD2MMF *self)
{
	string sFile = file;
	if (sFile == "lib/managers/achievmentmanager" && self->disableAchievements)
		self->doDisableAchievements(L);
	else if (sFile == "lib/managers/experiencemanager" && self->disableXP)
		self->doDisableXP(L);
	else if (sFile == "lib/managers/lootdropmanager")
		self->doDisableLootdrop(L);
	else if (sFile == "lib/managers/moneymanager" && self->disableMoney)
		self->doDisableMoney(L);
	else if ((sFile == "lib/network/matchmaking/networkaccountsteam" || sFile == "lib/managers/statisticsmanager") && self->disableStatistics)
		self->doDisableStatistics(L, sFile != "lib/managers/statisticsmanager");
	else if (sFile == "lib/network/matchmaking/networkmatchmakingsteam")
		self->setupModKey(L);
}

int PD2MMF::LuaSaveCheck(lua_State *L)
{
	lua_getfield(L, 1, "PROGRESS_SLOT");
	int progress_slot = lua_tointeger(L, -1);
	lua_remove(L, -1);
	int slot = lua_tointeger(L, 2);
	if (slot == progress_slot)
	{
		lua_pushboolean(L, LUA_TTRUE);
		lua_replace(L, 3);
	}
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_insert(L, 1);
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	return lua_gettop(L);
}

int PD2MMF::LuaRegisterMod(lua_State *L)
{
	PD2MMF *self = (PD2MMF*)lua_touserdata(L, lua_upvalueindex(1));
	self->mods[lua_tostring(L, 1)] = lua_tostring(L, 2);
	lua_pushnil(L);
	while (lua_next(L, 3) != 0)
	{
		string key = lua_tostring(L, -2);
		if (key == "disableXP")
			self->disableXP |= lua_toboolean(L, -1) == LUA_TTRUE;
		else if (key == "disableAchievements")
			self->disableAchievements |= lua_toboolean(L, -1) == LUA_TTRUE;
		else if (key == "disableMoney")
			self->disableMoney |= lua_toboolean(L, -1) == LUA_TTRUE;
		else if (key == "disableSaving")
			self->disableSaving |= self->disableAchievements |= self->disableStatistics |= lua_toboolean(L, -1) == LUA_TTRUE;
		else if (key == "disableStatistics")
			self->disableStatistics |= lua_toboolean(L, -1) == LUA_TTRUE;
		else if (key == "disableLootdrop")
			self->disableLootdrop |= lua_toboolean(L, -1) == LUA_TTRUE;
		lua_remove(L, -1);
	}
	return 0;
}
