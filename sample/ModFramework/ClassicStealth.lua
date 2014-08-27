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
		if self._unit:character_damage()._dead then
			return oldPager(self, ...)
		end
		return false
	end

elseif RequiredScript == "lib/units/enemies/cop/logics/coplogicintimidated" then
	CopLogicIntimidated._chk_begin_alarm_pager = function() end
	
	local oldIntimidated = CopLogicIntimidated.on_intimidated
	function CopLogicIntimidated.on_intimidated(...)
		local state = managers.groupai:state()
		local oldmode = state.whisper_mode
		state.whisper_mode = function() return false end
		oldIntimidated(...)
		state.whisper_mode = oldmode
	end
	local oldHandsUp = CopLogicIntimidated._start_action_hands_up
	function CopLogicIntimidated._start_action_hands_up(...)
		local state = managers.groupai:state()
		local oldmode = state.whisper_mode
		state.whisper_mode = function() return false end
		oldHandsUp(...)
		state.whisper_mode = oldmode
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
			while managers.enemy._enemy_data.nr_units + self._group_data.amount > 5 and self._group_data.amount > 0 do
				self._group_data.amount = self._group_data.amount - 1
			end
		end
		if self._group_data.amount > 0 then
			return oldExec(self, ...)
		end
	end
end