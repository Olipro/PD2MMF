class PD2MMF
{
	bool disableAchievements;
	bool disableXP;
	bool disableMoney;
	bool disableSaving;
	bool disableStatistics;
	bool disableLootdrop;
	map<string, string> mods;
	void doDisableAchievements(lua_State*);
	void doDisableXP(lua_State*);
	void doDisableMoney(lua_State*);
	void doDisableSaving(lua_State*);
	void doDisableStatistics(lua_State*, bool);
	void doDisableLootdrop(lua_State*);
	void setupModKey(lua_State*);
	static int LuaRegisterMod(lua_State*);
	static int LuaSaveCheck(lua_State*);
public:
	static void WINAPI LuaNewState(lua_State*, LPCSTR, PD2MMF*);
	static void WINAPI LuaPostRequire(lua_State*, LPCSTR, PD2MMF*);
	PD2MMF();
};
