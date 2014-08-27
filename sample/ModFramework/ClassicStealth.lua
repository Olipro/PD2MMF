RegisterMod("ClassicStealth", "1.0.0", {
	disableXP = false,
	disableAchievements = true,
	disableLootdrop = false,
	disableMoney = false,
	disableSaving = false,
	disableStatistics = false,
})

if RequiredScript == "lib/units/enemies/cop/copbrain" then
	local oldPager = CopBrain.is_pager_started
	function CopBrain:is_pager_started(...)
		return self._unit:character_damage()._dead and oldPager(self, ...) or false
	end

elseif RequiredScript == "lib/units/enemies/cop/logics/coplogicintimidated" then
	CopLogicIntimidated._chk_begin_alarm_pager = function() end
	
	for _,v in ipairs({"on_intimidated", "_start_action_hands_up"}) do 
		local upvalue = CopLogicIntimidated[v]
		CopLogicIntimidated[v] = function(...)
			local state = managers.groupai:state()
			local oldmode = state.whisper_mode
			state.whisper_mode = function() return false end
			upvalue(...)
			state.whisper_mode = oldmode
		end
	end
	
elseif RequiredScript == "lib/units/interactions/interactionext" then
	local oldBlock = IntimitateInteractionExt._interact_blocked
	function IntimitateInteractionExt:_interact_blocked(...)
		if self.tweak_data == "hostage_convert" then
			return not ( managers.player:has_category_upgrade( "player", "convert_enemies" ) and not managers.player:chk_minion_limit_reached() )
		elseif self.tweak_data == "corpse_dispose" then
			return not managers.player:has_category_upgrade( "player", "corpse_dispose" )
		end
		return oldBlock(self, ...)
	end
	
elseif RequiredScript == "lib/managers/group_ai_states/groupaistatebase" then
	local oldCheck = GroupAIStateBase.has_room_for_police_hostage
	
	function GroupAIStateBase:has_room_for_police_hostage(...)
		if self:whisper_mode() then
			return self._police_hostage_headcount < 1
		end
		return oldCheck(self, ...)
	end
	
elseif RequiredScript == "lib/managers/playermanager" then
	function PlayerManager:_set_body_bags_amount()
		self._local_player_body_bags = 1
	end
	
elseif RequiredScript == "lib/managers/mission/elementspawnenemydummy" then
	local oldSpawn = ElementSpawnEnemyDummy.on_executed
	function ElementSpawnEnemyDummy.on_executed(...)
		if Global.game_settings.difficulty ~= "overkill_290" and managers.groupai:state():whisper_mode() and Application:time() > 20 then
			return
		end
		return oldSpawn(...)
	end
	
elseif RequiredScript == "lib/managers/mission/elementspawnenemygroup" then
	local oldExec = ElementSpawnEnemyGroup.on_executed
	function ElementSpawnEnemyGroup:on_executed(...)
		if managers.groupai:state():whisper_mode() and Global.game_settings.difficulty ~= "overkill_290" then
			self._group_data.amount = managers.enemy._enemy_data.nr_units + self._group_data.amount > 5 and 5 - managers.enemy._enemy_data.nr_units or self._group_data.amount
		end
		if self._group_data.amount > 0 then
			return oldExec(self, ...)
		end
	end
end