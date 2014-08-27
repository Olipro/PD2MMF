#PD2MMF v0.1.0 - Payday 2 Mod/Mutator Management Framework.

The purpose of this framework is to provide a convenient way for multiple
separate mods to register themselves and request disabling of various game
mechanics. Additionally, it ensures that when someone runs a particular
combination of mechanics mods, they will only be able to play with people who
also have that combination of mods.

Please consider this framework to currently be a beta - the functionality for
disabling certain game aspects is as-yet not fully tested. Backing up your save
file is always recommended.

A relatively simple sample is included that uses the PostRequire mechanism of
the Lua hook to provide the old style of pre-deathwish stealth mechanics to
non-deathwish difficulties. Probably the most relevant part is the function
call that starts on the first line as it is that function call which does the
registration with the framework.

##Disabling Game Aspects
The sample Lua should make everything fairly obvious in terms of what you can
ask PD2MMF to disable. Please note that if you disable saving it will ALSO
disable achievements and statistics. It does not disable XP and money however
since that obviously won't be preserved after closing the game anyway.

Disabling money does not FULLY disable the money aspect of the game. It does
however make all mission-related money aspects disabled: assets are free and
there is no cost for killing civilians. conversely, no money is actually paid
out for succeeding.

Disabling lootdrops will not suppress the lootdrop screen, but anything that is
dropped will not actually go into the player's inventory.
