|***
 *	modes.inc
 *	Mode Specific Functions
 *	for doing strange stuff....
 *
 *
 ***|




|***
 *	DES: stop all non standard modes
 *	USE: /call switch_clear_mode
 *	INI: NA
 *  NOTE:

sub switch_clear_mode(bool _debug)
/return


	/declare _count int local 0
	/declare _list string local ${sql_return[FALSE, lstTypesMode]}

	/if (${swModeToken}) {
		/varset swModeToken FALSE
	}

	/for _count 1 to ${_list.Count[|]}
		/if (${swMode${_list.Arg[${_count},|]}}) {
			/varset swMode${_list.Arg[${_count},|]} FALSE
		}
	/next _count

	/varset swOverrideLOS ${sql_return[FALSE, swOverrideLOS]}
	/varset swOverrideEngagePct ${sql_return[FALSE, swOverrideEngagePct]}
	/varset swAutoCursor ${sql_return[FALSE, swAutoCursor]}
	/varset swModeTrainspellSPC FALSE

/return TRUE
 ***|


|***
 *	DES: central get target and kill for hunting modes
 *	USE: /call mode_get_mob
 *  NOTE:

sub mode_get_mob(bool _debug)
	:movetomob
	GETINPUT
	ISMEDEAD

	/call check_cursor ${_debug} CLEAR

	/if (!${Spawn[id TARGET].ID} || ${Spawn[id TARGET].Dead} || ${Target.Dead}) {
		/call set_stop ${_debug} SILENT
		/call clear_combat ${_debug}
		/return FALSE
	}

	/call get_target_navpull ${_debug} TARGET

	/if (${Spawn[TARGET].ID} && ${Spawn[TARGET].Distance} > ${setCombatDistance}) /goto :movetomob

/return
 ***|


|***
 * note: Train non spell skills. Best to use during a duel with a pet.
 * use: /mode trainskill
 ***|
sub mode_trainskill(bool _debug)
	DEBUG \atmode_train\ax()

	ISMEDEAD
	SETHUD "mode trainskill"
	/makemevisible
	/delay 1

	/if (${Me.Skill[Taunt]} < ${Skill[Taunt].SkillCap} && ${Me.AbilityReady[Taunt]}) {
		DEBUG ${sep}taunt
		/doability Taunt
	}

	/if (${Me.Skill[Disarm]} < ${Skill[Disarm].SkillCap} && ${Me.AbilityReady[Disarm]}) {
		DEBUG ${sep}disarm
		/doability Disarm
	}

	/if (${Me.Skill[Bash]} < ${Skill[Bash].SkillCap} && ${Me.AbilityReady[Bash]}) {
		DEBUG ${sep}bash
		/doability Bash
	}

	/if (${Me.Skill[Kick]} < ${Skill[Kick].SkillCap} && ${Me.AbilityReady[Kick]}) {
		DEBUG ${sep}kick
		/doability Kick
	}

	/if (${Me.Skill[Sneak]} < ${Skill[Sneak].SkillCap} && ${Me.AbilityReady[Sneak]}) {
		DEBUG ${sep}sneak
		/doability sneak
	}

	/if (${Target.ID} && ${Target.ID} != ${Me.ID}) {
		/if (${Me.Skill[Begging]} < ${Skill[Begging].SkillCap} && ${Me.AbilityReady[Begging]}) /doability begging
	}

	| Bind Wounds
	/if (${Me.AbilityReady[Bind Wound]} && ${FindItem[=Bandages].ID}) {
		/if (${Me.Skill[Bind Wound]} < ${Skill[Bind Wound].SkillCap} && ${Me.PctHPs} < 99*.PCTHO) {
			DEBUG ${sep}bind
			/invoke ${set_target[${Me.ID}]}
			/doability "Bind Wound"
			/delay 14s
		}
	}

	SETHUD EMPTY

/return TRUE



|***
 * note: everyone needs spare sandals
 * use: /mode fish
 ***|
sub mode_fish(bool _debug)

	ISMEDEAD
	GETINPUT

	SETHUD "mode fish"

	/if (!${Me.Standing}) /stand

	/call check_cursor ${_debug} TRUE

	/if (${Me.ItemReady[Fisherman's Companion]} && !${Me.ItemReady[Brell's Fishin' Pole]} && !${Me.Inventory[mainhand].ID}) {
		/if (${Me.Invis}) /docommand /makemevisable
		/if (${is_ready[${_debug}, "Fisherman's Companion"]}) {
			/call voodoo ${_debug} "Fisherman's Companion" ${Me.ID} FALSE
		}
	}

	/call check_cursor ${_debug} TRUE

	/if (${Me.AbilityReady[fishing]}) {
		DEBUG ${sep}fish
		/doability fishing
	}

	/call check_cursor ${_debug} TRUE

	SETHUD EMPTY

/return TRUE



|***
 * note: you got shit under your finger nails
 * use: /mode forage
 ***|
sub mode_forage(bool _debug)
	DEBUG \atmode_forage\ax()

	/if (${Select[${Me.CombatState},combat]}) {
		/return FALSE
	}

	/if (!${Me.AbilityReady[forage]}) {
		/return FALSE
	}

	ISMEDEAD
	SETHUD "mode forage"

	/if (!${Me.Standing}) {
		/stand
	}

	DEBUG ${sep}forage
	/doability forage
	/delay 1s

	/while (${Cursor.ID})	{
		GETINPUT
		ISMEDEAD

		/call check_cursor ${_debug} TRUE
		/if (!${Cursor.ID}) /break
		/delay 2
	}

	SETHUD EMPTY

/return TRUE



|***
 * note: Kill people then drag them around like trophys!!! Win Win...
 * use: /mode drag
 ***|
sub mode_drag(bool _debug)
	DEBUG \atmode_drag\ax()

	/declare _count 		int local
	/declare _corpseID 	int local

	SETHUD "mode drag"

	GETINPUT
	ISMEDEAD

	/delay 1
	/if (${SpawnCount[pccorpse radius 110]}) {
		/for _count 1 to ${SpawnCount[pccorpse radius 110]}
			ISMEDEAD

			/varset _corpseID ${NearestSpawn[${_count},pccorpse radius 110].ID}
			/if (${Spawn[${_corpseID}].Distance} > 15) {
				/squelch /target id ${_corpseID}
				/delay 5 ${Target.ID} == ${_corpseID}
				/corpse
			}
		/next _count
	}

	SETHUD EMPTY

/return



|***
 * note: turns the toon into a hunter of things.
 *	USE: /mode hunt
 ***|
sub mode_hunt2(bool _debug)
	DEBUG \atmode_hunt2\ax()

	GETINPUT
	ISMEDEAD

	SETHUD "mode hunt"

	/declare _lsHunt list local
	/invoke ${_lsHunt.Delimiter[|]}
	/invoke ${_lsHunt.Append[${stModeData}]}
	/declare _lihunt listiterator local
	/vardata _lihunt _lsHunt.First.Clone
	/while (!${_lihunt.IsEnd}) {
		/if (${Spawn[npc ${_lihunt.Value} radius 999999 targetable].ID}) {
			/varset stAssTarID ${NearestSpawn[npc ${_lihunt.Value}].ID}
			/break
		}
		/invoke ${_lihunt.Advance}
	}

	/if (${Spawn[id TARGET].ID}) {
		/call get_target_navpull ${_debug} TARGET
	}	else /if (!${Spawn[id TARGET].ID}) {
		/call clear_combat ${_debug}
		SETHUD EMPTY
		/return FALSE
	}

	/call set_combat ${_debug}
	/delay 1s
	SETHUD EMPTY

/return TRUE





	CLEARCURSOR TRUE
	/varset swModeToken TRUE

	/for _count 1 to ${HuntMobList.Count[|]}
		/if (${Spawn[npc ${HuntMobList.Arg[${_count},|]} radius PULLENV targetable].ID}) {
			/varset Target2Kill ${NearestSpawn[npc ${HuntMobList.Arg[${_count},|]}].ID}
			/break
		}
	/next _count

	/if (!TARGET) {
		/call timer_set timer_wait_for_mob_to_kill ${tHunt_delay}
		/call check_buff_routines TRUE
	}

	/if (!${Bool[${FindItemCount[${lstRangeItemSummon}]}]} && ${Bool[${lstRangeItemSummon}]}) /call check_ammo
	/if (${swADVL}) {
		/if ((AUTO || ${swOverrideLoot}) && !${SafeZone} && !${swOverrideSafeZone}) /call do_loot
	}
	/if (${Spawn[TARGET].ID}) {
		/call mode_get_mob
	} else /if (${Spawn[TARGET].Dead} || !${Spawn[TARGET].ID} || !${timer_scorched_earth_fail}) {
		/if (!${timer_scorched_earth_fail}) /call timer_set timer_scorched_earth_ignore_$TARGET ${tScorchedEarth_ignore}
		/call set_stop NOECHO
		/call clear_combat
	}

	/varset swModeToken FALSE
	SETHUD EMPTY
/return



|***
 * note: Pet Farm
 * use: /mode petfarm
 ***|
sub mode_petfarm(bool _debug)
	DEBUG \atmode_petfarm\ax()

	/if (!${lsClassPet.Contains[${Me.Class.ShortName}]} || SAFEZONE) {
		/invoke ${maMode.Add[${_mimode.Key},FALSE]}
		/return FALSE
	}

	ISMEDEAD

	/invoke ${maOver.Add[swOverPctEngage,TRUE]}
	/invoke ${maOver.Add[swOverLOS,TRUE]}
	/varset swModeToken TRUE

	SETHUD "mode petfarm"

	GETINPUT

	/if (AUTO) {
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) {
			/call check_minion_cycle ${maDebug.Find[minion].Value} FALSE
		}
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) {
			/call check_heal_${Me.Class.ShortName} ${maDebug.Find[minion].Value} Pet
		}
	}

	| find a mob
	/call pull_mob_find ${_debug} 0 tagpets
	DEBUG pull_mob_find.return${sep}\a-w${Macro.Return}\ax

	/if (!TARGET) {
		DEBUG \arNO\ax stAssTarID
		SETHUD EMPTY
		GETINPUT
		/call where_the_fuck_is_my_pet ${_debug} FORCE
		/return FALSE
	}

	CHECKTIE
	/doevents
	/invoke ${set_target[TARGET]}

	/pet attack

	CHECKTIE

	/invoke ${set_timer[${_debug}, timer_mob_get, 20s]}

	/while (TARGET) {
		GETINPUT
		/delay 1s

		/if (${Spawn[TARGET].Type.Equal[CORPSE]} || !${Spawn[TARGET].ID}) /break
		/if (${Me.Pet.Target.ID} != TARGET) /break
		/if (${Spawn[TARGET].Distance} > PULLENV) /break
		DEBUG ${sep}here${sep}\a-w${timer_mob_get}\ax \agTARGET\ax
		/if (!${timer_mob_get}) /break

	}

	/invoke ${maOver.Add[swOverPctEngage,FALSE]}
	/invoke ${maOver.Add[swOverLOS,FALSE]}
	/varset swModeToken FALSE
	/call where_the_fuck_is_my_pet ${_debug} FORCE
	/call clear_combat

	SETHUD EMPTY

/return



|***
 *	DES: runs around and picks up shit
 *	USE: /mode harvest
 ***|
sub mode_harvest(bool _debug)
	DEBUG \atmode_harvest\ax()

	/if (!NEXT) {
 		/invoke ${out[0, "${notice} ${dot} this has been depreciated in MQ2Live."]}
 		/invoke ${maMode.Add[harvest,FALSE]}
		/return FALSE
	}

	ISMEDEAD
	GETINPUT

	SETHUD "mode harvest"

	/declare _lsCollect list local
	/invoke ${_lsCollect.Delimiter[|]}
	/invoke ${_lsCollect.Append[${stModeData}]}
	/declare _licol listiterator local
	/vardata _licol _lsCollect.First.Clone

	/while (!${_licol.IsEnd}) {
		/if (${GroundItemCount[${_licol.Value}]}) {
			/call get_ground_item ${_debug} "${_licol.Value}"
		}
		/invoke ${_licol.Advance}
	}

	/delay 1s
	SETHUD EMPTY

/return



|***
 *	DES: moves to the ground item and picks it up
 *	USE: /call get_ground_harvest DEBUG [ITEM NAME]
 *  NOTE:
 ***|
sub get_ground_item(bool _debug, string _tmpItem)
	DEBUG \atget_ground_item\ax(\a-w"${_tmpItem}"\ax)

	/if (${Cursor.ID}) {
		/call check_cursor ${_debug} CLEAR
	}

	/if (!${GroundItemCount[${_tmpItem}]}) {
		DEBUG ${sep}no ground item count
		/return FALSE
		/delay 2
	}

	/if (!${Navigation.MeshLoaded}) {
 		/invoke ${maMode.Add[harvest,FALSE]}
 		/invoke ${out[0, "${notice} ${dot} NO Nav Mesh for this zone."]}
 		/return	FALSE
	}

	ISMEDEAD

	/invoke ${Ground.Search[${_tmpItem}]}
	/declare _item ground local
	/vardata _item Ground.First
	/declare _distfinal int local 999999
	/declare _validdis int local 0
	/declare _validpath bool local FALSE
	/declare _finalGround ground local

	/while (${_item.ID}) {
		/varset _validdis ${Navigation.PathLength[item ${_item.ID}]}
		/varset _validpath ${Navigation.PathExists[item ${_item.ID}]}
		DEBUG ID:\a-w${_item.ID}\ax .. Dis:${If[${_validdis} > 0,\a-w${_validdis}\ax,\arinvalid\ax]}

		/if (${_validdis} <= 0 || !${_validpath}) {
			/vardata _item Ground.Next
			/continue
		}

		/if (${_validdis} < ${_distfinal}) {
			/varset _distfinal ${_validdis}
			/vardata _finalGround Ground
		}
		/vardata _item Ground.Next
	}

	DEBUG FINAL ID:${_finalGround.ID}
	/invoke ${Ground.Search[${_finalGround.ID}]}
	/nav item |dist=10 log=off
	/delay 1s

	/while (${Me.Moving} || ${_finalGround.Distance3D} <= 19) {
		DEBUG ${sep}whileloop${sep}\a-wMe.Moving\ax

		/if (${Cursor.ID}) {
			/call check_cursor ${_debug} CLEAR
		}

		/if (${_finalGround.Distance3D} <= 19) {
			DEBUG ${sep}distance${sep}\a-w${_finalGround.Distance3D}\ax
			/nav stop |log=off
			/call Bind_command_grab "${_tmpItem}" ${_debug}
			/if (${Cursor.ID}) {
				/call check_cursor ${_debug} CLEAR
			}
			/break
		}

		/if (!${Navigation.Active} || !${Me.Moving}) {
			/break
		}

		/delay 2
	}

/return TRUE



|***
 *	DES: Alcohol Tolerance
 *	USE: /call mode_get_waisted
 *  NOTE:
 ***|
sub mode_get_waisted(bool _debug)
	/declare _ale string 	local Summoned: Ale
	/declare _aleCount int local 20
	/declare _pole string	local Brell's Fishin' Pole
	/declare _box string local Fisherman's Companion

	| no fishermans companion. end this mess
	/if (!${FindItem[=${_box}].ID}) {
		VOUT ${onotice} ${sep} No ${_box}
		VOUT This Mode uses Fisherman's Companion.
		VOUT Go get one.
		/varset swModeLush FALSE
		/return
	}

	SETHUD "Drink"
	/if (!${swAutoCursor}) /varset swAutoCursor TRUE
	| check max stats too ?

	| need booze! keep a stock on hand
	/if (${FindItemCount[${_ale}]} < ${_aleCount} && ${Me.ItemReady[${_pole}]} && !${Me.Drunk}) {
		/if (${validate_cast[FALSE, item, "${_pole}", ${Me.ID}]}) /call core_cast2 "${_pole}" item ${Me.ID} FALSE
		CLEARCURSOR TRUE
	}

	| lets get lit
	/if (${FindItemCount[${_ale}]} && ${Me.Drunk} < ${Skill[Alcohol Tolerance].SkillCap}) {
		VOUT Drinking ${sep} ${cinfo}${_ale}\ax
		/useitem ${_ale}
	}
	SETHUD EMPTY
/return



|***
 * note: alcohol tolerance
 * use: /call mode_lush
 ***|
sub mode_lush(bool _debug)
	DEBUG \atmode_lush\ax()

	/if (!${FindItem[=${stModeData}].ID} || !${Bool[${stModeData}]}) {
		/varset swModeDrink FALSE
		/return
	}

	SETHUD "mode lush"

	| start drinking.
	/while (${Me.Skill[Alcohol Tolerance]} < ${Skill[Alcohol Tolerance].SkillCap}) {
		/if (!${Me.ItemReady[=${stModeData}]}) /break
		/if (${Me.Drunk} < 200) {
			/call voodoo ${_debug} "${stModeData}" 0 FALSE
		}
		/delay 2
		/delay 2s ${Me.Drunk} <  190
	}

	| maxed skill turn this off
	/if (${Me.Skill[Alcohol Tolerance]} == ${Skill[Alcohol Tolerance].SkillCap}) {
		OUT Alcohol Tolerance Maxed ${sep} (${num}${Me.Skill[Alcohol Tolerance]}\ax)
		/return TRUE
	}

	SETHUD EMPTY

/return TRUE



|***
 *	DES: train spells
 *	USE: /call mode_trainspell | /mode trainspell
 *  NOTE:
 ***|
sub mode_trainspell(string _type, bool _debug)
	ISMEDEAD
	SETHUD "mode trainspell"

	| /if (${_type.Equal[specialize]} && !${swModeTrainspellSPC}) {
	| 	/varset swModeTrainspellSPC TRUE
	| }

	/declare _skill string local |Alteration|Abjuration|Conjuration|Divination|Evocation
	/declare _count int local 0
	/declare _gem[5] bool local FALSE

	| figure out what we want to cast
	/for _count 1 to ${Me.NumGems}

		| validate skill caps and spell. if cap is maxed, move on
		/if (!${maOver.Find[swOverTrainSpell].Value}) {
		| 	/if (${Me.Skill[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]} == ${Me.SkillCap[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]}) /continue
		}

		| if the skill spell is set to FALSE, move on
		| /if (!${Bool[${_skill.Arg[${_count},|]}]}) /continue

		| if the spell is FALSE, move on
		| /if (!${Bool[${Spell[${${_skill.Arg[${_count},|]}}].Skill}]}) /continue

		| if I already have the spell memed, move on
		| /if (!${Me.Gem[${_count}].Name.Equal[${${_skill.Arg[${_count},|]}}]}) {
		| 	/call mem_spell TRUE "${${_skill.Arg[${_count},|]}}" ${_count} FALSE
		| }

		| /varset _gem[${_count}] TRUE


		/if (!${Me.Gem[${_count}].ID}) {
			/delay 1
			/continue
		}

		| empty hands
		/if (${Cursor.ID}) {
			/call check_cursor ${_debug} CLEAR
		}

		| cast the spell
		/if (${is_ready[${_debug}, "${Me.Gem[${_count}].Name}"]}) {
			/call voodoo ${_debug} "${Me.Gem[${_count}].Name}" 0 FALSE
		}
		/delay 5s !${Me.Casting.ID}

		| seriously theres an apocalypse going on, shoud wash your hands
		/if (${Cursor.ID}) {
			/call check_cursor ${_debug} CLEAR
		}


	/next _count


	SETHUD EMPTY

/return


${Me.Gem[${_count}].Name}



	ISMEDEAD
	SETHUD "mode trainspell"

	/declare swTrainAlteration bool local TRUE

	| /if (${_type.Equal[specialize]} && !${swModeTrainspellSPC}) {
	| 	/varset swModeTrainspellSPC TRUE
	| }

	/declare _skill string local |Alteration|Abjuration|Conjuration|Divination|Evocation
	/declare _count int local 0
	/declare _gem[5] bool local FALSE

	| figure out what we want to cast
	/for _count 1 to ${_skill.Count[|]}

		| validate skill caps and spell. if cap is maxed, move on
		/if (!${swOverrideTrainSpell}) {
			/if (${Me.Skill[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]} == ${Me.SkillCap[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]}) /continue
		}

		| if the skill spell is set to FALSE, move on
		/if (!${Bool[${_skill.Arg[${_count},|]}]}) /continue

		| if the spell is FALSE, move on
		/if (!${Bool[${Spell[${${_skill.Arg[${_count},|]}}].Skill}]}) /continue

		| if I already have the spell memed, move on
		/if (!${Me.Gem[${_count}].Name.Equal[${${_skill.Arg[${_count},|]}}]}) {
			/call mem_spell TRUE "${${_skill.Arg[${_count},|]}}" ${_count} FALSE
		}

		/varset _gem[${_count}] TRUE
	/next _count

	| start getting things done
	/for _count 1 to ${_skill.Count[|]}
		/if (!${_gem[${_count}]}) /continue
		/if (${is_ready[${_debug}, "${${_skill.Arg[${_count},|]}}"]}) /call voodoo ${_debug} "${${_skill.Arg[${_count},|]}}" 0 0 FALSE
		/call sort_cursor TRUE
	/next _count


	SETHUD EMPTY



|***
 *	DES: open something
 *	USE: /call mode_open | /mode open
 *  NOTE:
 ***|
sub mode_open(string _type, bool _debug)
	ISMEDEAD
	SETHUD "Open"

	/call timer_set timer_mode_open ${tMode_Open}
	/declare _tmpID int local

	/if (${SpawnCount[${modeOpenItem} npc radius 20]}) {
		/varset _tmpID ${Spawn[${modeOpenItem} npc radius 20].ID}
		/squelch /target ID ${_tmpID}
		/if (${Target.Dead}) /return
		/delay 2s ${Target.ID} == ${_tmpID}
		/open
		/delay 1s
		/return
	}

	SETHUD EMPTY
/return


| /edit modeOpenItem "a hollows mushroom"



|***
 * note: summons a corpse in lobby
 * use: /mode summon DEBUG
 ***|
sub mode_summon(bool _debug)
	DEBUG \atmode_summon\ax()

	| in the lobby?
	/if (${Zone.ShortName.NotEqual[GuildLobby]}) {
		/invoke ${maMode.Add[summon,FALSE]}
		/return FALSE
	}

	SETHUD "mode summon"

	| set stone to buy
	/declare _soulstone string local
	/if (${Me.Level} <= 115) /varset _soulstone Velium Soulstone
	/if (${Me.Level} <= 110) /varset _soulstone Draconic Soulstone
	/if (${Me.Level} <= 105) /varset _soulstone	Torrential Soulstone
	/if (${Me.Level} <= 100) /varset _soulstone	Coalescent Soulstone
	/if (${Me.Level} <= 95) /varset _soulstone Luminous Soulstone
	/if (${Me.Level} <= 90)	/varset _soulstone Phantasmal Soulstone
	/if (${Me.Level} <= 85)	/varset _soulstone Iridescent Soulstone
	/if (${Me.Level} <= 80)	/varset _soulstone Prismatic Soulstone
	/if (${Me.Level} <= 75)	/varset _soulstone Glowing Soulstone
	/if (${Me.Level} <= 70)	/varset _soulstone Pristine Soulstone
	/if (${Me.Level} <= 55)	/varset _soulstone Faceted Soulstone
	/if (${Me.Level} <= 50)	/varset _soulstone Greater Soulstone
	/if (${Me.Level} <= 40)	/varset _soulstone Soulstone
	/if (${Me.Level} <= 30)	/varset _soulstone Lesser Soulstone
	/if (${Me.Level} <= 20)	/varset _soulstone Minor Soulstone

	:start
	GETINPUT
	ISMEDEAD

	| do we need to buy a stone?
  /if (!${FindItemCount[=${_soulstone}]}) {
		/nav spawn npc A Disciple of Luclin |log=off

		:priestinventory
			/delay 10s !${Navigation.Active}
			/delay 1s
			/squelch /target A Disciple of Luclin
			/delay 1s
			/nomodkey /click right target
			/delay 5s ${Window[MerchantWnd].Open}
			/if (!${Window[MerchantWnd].Open}) /goto :priestinventory
		/delay 1s
		/call vendor_buy ${maDebug.Find[vendor].Value} "${_soulstone}" 1

		:closepriestinventory
			/nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
			/delay 10
			/if (${Window[MerchantWnd].Open}) /goto :closepriestinventory
  }
	/if (!${FindItemCount[=${_soulstone}]}) /goto :start

	| move to rezer
  /nav spawn npc A Priest of Luclin |log=off distance=6
  /delay 10s !${Navigation.Active}

	| get stone on cursor
  /itemnotify "${_soulstone}" leftmouseup
  /delay 5

	:givetopriest
	  /squelch /target A Priest of Luclin
	  /delay 5
	  /nomodkey /click left target
	  /delay 5
	  /delay 5s ${Window[GiveWnd].Open}
	  /if (!${Window[GiveWnd].Open}) /goto :givetopriest
		/if (${Window[GiveWnd].Open}) {
			/notify GiveWnd GVW_Give_Button leftmouseup
			/delay 3s
			/if (${Window[GiveWnd].Open}) /goto :GiveSoulStone
		}

	:CloseInvWnd
		/if (${Window[InventoryWindow].Open}) {
			/nomodkey /notify InventoryWindow IW_DoneButton LeftMouseUp
			/delay 5
			/if (${Window[InventoryWindow].Open}) /goto :CloseInvWnd
		}

  /if (${Window[InventoryWindow].Open}) /nomodkey /notify InventoryWindow IW_DoneButton LeftMouseUp
  /delay 5

	/squelch /target clear
	/invoke ${maMode.Add[summon,FALSE]}

/return FALSE



|***
 * note: Scorched Earth Mode
 * use: /mode hunt
 ***|
sub mode_hunt(bool _debug)
	DEBUG \atmode_hunt\ax(\a-w\ax)

	/invoke ${maOver.Add[swOverPctEngage,TRUE]}
	/invoke ${maOver.Add[swOverLOS,TRUE]}
	/varset swModeToken TRUE
	/call clear_combat ${_debug}
	/declare _count int local 0
	/declare _lsHunt list local
	/declare _lihunt listiterator local


	SETHUD "mode hunt"

	:checkpath
	GETINPUT

	/if (${Bool[${stModeData}]}) {
		/invoke ${_lsHunt.Delimiter[|]}
		/invoke ${_lsHunt.Append[${stModeData}]}
		/vardata _lihunt _lsHunt.First.Clone
		/while (!${_lihunt.IsEnd}) {
			/if (${Spawn[npc ${_lihunt.Value} radius PULLENV targetable].ID}) {
				/varset stAssTarID ${NearestSpawn[npc ${_lihunt.Value}].ID}
				/break
			}
			/invoke ${_lihunt.Advance}
		}

	} else /if (!${Bool[${stModeData}]}) {
		/call pull_mob_find ${_debug} ${_count}
		DEBUG pull_mob_find.return${sep}\a-w${Macro.Return}\ax
	}

	/if (!TARGET) {
		DEBUG \arNO\ax stAssTarID
		/call clear_combat ${_debug}
		/return FALSE
	}

	/if	(!${Navigation.PathExists[locyxz ${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]}) {
		GETINPUT
		/invoke ${out[0, "${notice}${sep}No Nav Path${sep}\a-w${Spawn[TARGET].DisplayName}\ax"]}
		/varcalc _count ${_count}+1
		/if (${_count} > 10) /return FALSE
		/delay 1s
		/goto :checkpath
	 }

	| get to the mob
	/call pull_mob_move_to ${_debug} TARGET ${Math.Calc[${Spawn[id TARGET].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} on

	/if (${Spawn[id TARGET].Dead}) /return TRUE


	/invoke ${maCC.Add[swSetCombat,TRUE]}
	/invoke ${set_target[TARGET]}

	| /call check_facing ${_debug} TARGET FORCE

	/call Bind_control_cc FORCE
	/call main_${Me.Class.ShortName}
	/call clear_combat

	/invoke ${maOver.Add[swOverPctEngage,FALSE]}
	/invoke ${maOver.Add[swOverLOS,FALSE]}
	/varset swModeToken FALSE

	SETHUD EMPTY

/return TRUE



|***
 * note: basic combat routine used by modes
 * use: /mode hunt
 ***|
sub mode_combat_routine(bool _debug)
	DEBUG \atmode_combat_routine\ax()

	/invoke ${set_target[TARGET]}

	/while (TARGET) {
	/call set_combat ${_debug}
	/call check_facing ${_debug} TARGET FORCE
	/if (PET || SWARM) /call send_pet ${_debug} TARGET
	/if (SWARM) /call cast_temp_pet FALSE TARGET "${maChr.Find[stTempPet].Value}"
	/if (AGRO && ${SubDefined[get_agro_${Me.Class.ShortName}]}) /call get_agro_${Me.Class.ShortName}
	/call misc_combat_${Me.Class.ShortName} FALSE
	/if (BURN) /call burn_${Me.Class.ShortName} ${_debug}
	/call check_facing ${_debug} TARGET FORCE
	/if (MELEE) /call do_melee FALSE TARGET
	/if (DOT) /call cast_DOT ${_debug} lsDoT TARGET
	/if (NUKE) /call cast_nuke ${_debug} lsNuke TARGET
	/call check_item_cycle ${_debug} TARGET


/return












quest giver: Missionary Ada
quest Name: The Way Back Home
quest phrases: go look

|*********************
farms
********************|



sub tov_ew_ada(bool _debug)
	DEBUG \attov_ew_ada\ax()

	/if (${Target.ID}) /squelch /target clear

	/declare _quest string local The Way Back Home
	/declare _count int local 0

	/if (!${Task[${_quest}].ID}) {
		/call task_get ${_debug} "${_quest}"
		/if (!${Macro.Return}) {
			/invoke ${out[23, "Failed to get${sep}\a-w${_quest}\ax"]}
		} else /if (${Macro.Return}) {
			/invoke ${out[23, "Quest started${sep}\a-w${_quest}\ax"]}
		}
	}

	/if (${Me.Sitting}) /stand
	/squelch /hidecorpse always

	/call Bind_control_cc pull rad 45
	/call Bind_control_cc pull active on
	/call Bind_control_cc agro on

	| how big is the quest?
	/sqlite ${maData.Find[DBstatic].Value} farmcount SELECT count(*) as data from farm where task="${_quest}"
	DEBUG ${sep}count${sep}\a-w${sqlite.Result[farmcount 1 data]}\ax

	| get the data we need for this step
	/for _count 0 to ${Math.Calc[${sqlite.Result[farmcount 1 data]} -1]}
		/sqlite ${maData.Find[DBstatic].Value} farmquery${_count} SELECT * FROM farm WHERE task="${_quest}" AND stepn="${_count}"
		/if (!${_count}) /continue
		OUT ${dot}loading${sep}\aw${_count}\ax${sep}\a-w${sqlite.Result[farmquery${_count} 1 step]}\ax
	/next _count
	/echo


	| step 1
		/invoke ${out[23, "\agSTEP 1\ax${sep}\a-w${sqlite.Result[farmquery1 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status}\ax)"]}
		:tov_ew_ada_step1
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status.NotEqual[Done]}) {
			/call task_output ${_debug} 1
			DEBUG ${sep}step${sep}${sqlite.Result[farmquery1 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status}\ax)
			GETINPUT
			ISMEDEAD
			/if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery1 1 ivu]}]} && ${Me.AltAbilityReady[Group Perfected Invisibility to Undead]}) {
				/call voodoo FALSE "Group Perfected Invisibility to Undead" 0 FALSE
			}
			/call Bind_command_navto loc "${sqlite.Result[farmquery1 1 locyxz]}" FALSE
			/delay 2s
		}
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step1


	| step 2
		/invoke ${out[23, "\agSTEP 2\ax${sep}\a-w${sqlite.Result[farmquery2 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status}\ax)"]}
		:tov_ew_ada_step2
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual[Done]}) {
			/call task_output ${_debug} 3
			DEBUG ${sep}step${sep}${sqlite.Result[farmquery2 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status}\ax)
			GETINPUT
			ISMEDEAD
			/if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery2 1 ivu]}]} && ${Me.AltAbilityReady[Group Perfected Invisibility to Undead]}) {
				/call voodoo FALSE "Group Perfected Invisibility to Undead" 0 FALSE
			}
			/call Bind_command_navto loc "${sqlite.Result[farmquery2 1 locyxz]}" FALSE
			/delay 2s
		}
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step2


	| step 3
		/invoke ${out[23, "\agSTEP 3\ax${sep}\a-w${sqlite.Result[farmquery3 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status}\ax)"]}
		:tov_ew_ada_step3
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status.NotEqual[Done]}) {
			/call task_output ${_debug} 2
			DEBUG ${sep}step${sep}${sqlite.Result[farmquery3 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status}\ax)
			GETINPUT
			ISMEDEAD
			/if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery3 1 ivu]}]} && ${Me.AltAbilityReady[Group Perfected Invisibility to Undead]}) {
				/call voodoo ${_debug} "Group Perfected Invisibility to Undead" 0 FALSE
			}
			/call Bind_command_navto loc "${sqlite.Result[farmquery3 1 locyxz]}" FALSE
			/delay 2s
		}
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step3


	| step 4
		/invoke ${out[23, "\agSTEP 5\ax${sep}\a-w${sqlite.Result[farmquery4 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status}\ax)"]}
		:tov_ew_ada_step4
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status.NotEqual[Done]}) {
			/call task_output ${_debug} 4
			DEBUG ${sep}step${sep}${sqlite.Result[farmquery4 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status}\ax)
			GETINPUT
			ISMEDEAD
			/if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery4 1 ivu]}]} && ${Me.AltAbilityReady[Group Perfected Invisibility to Undead]}) {
				/call voodoo ${_debug} "Group Perfected Invisibility to Undead" 0 FALSE
			}
			/call Bind_command_navto loc "${sqlite.Result[farmquery4 1 locyxz]}" FALSE
			/delay 2s
		}
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step4


	| step 5/6
		/declare _tmpStep int local 0
		/declare _tmpMob string local
		/declare _tmpObj string local ${Task[${_quest}].Step}
		/if (${_tmpObj.Find[huge]}) {
			/varset _tmpStep 6
		} else /if (${_tmpObj.Find[small]}) {
			/varset _tmpStep 5
		}
		| set mob name for use
		/varset _tmpMob ${sqlite.Result[farmquery${_tmpStep} 1 questmob]}


		/invoke ${out[23, "\agSTEP ${_tmpStep}\ax${sep}\a-w${sqlite.Result[farmquery${_tmpStep} 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery${_tmpStep} 1 step]}].Status}\ax)"]}
		:tov_ew_ada_step5_6
		/if (${Task[${_quest}].Objective[${sqlite.Result[farmquery${_tmpStep} 1 step]}].Status.NotEqual[Done]}) {
			/call task_output ${_debug} 4
			DEBUG ${sep}step${sep}${sqlite.Result[farmquery${_tmpStep} 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery${_tmpStep} 1 step]}].Status}\ax)
			GETINPUT
			ISMEDEAD
			/if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery${_tmpStep} 1 ivu]}]} && ${Me.AltAbilityReady[Group Perfected Invisibility to Undead]}) {
				/call voodoo ${_debug} "Group Perfected Invisibility to Undead" 0 FALSE
			}
			/call Bind_command_navto loc "${sqlite.Result[farmquery${_tmpStep} 1 locyxz]}" FALSE

		}

		:tov_ew_ada_step5_6_wait
		SDEBUG waiting for${sep}\a-w${_tmpMob}\ax

		| kill spawn
		/if (!${Spawn[npc radius 45 ${_tmpMob}].ID}) {
			/delay 1s
			/goto :tov_ew_ada_step5_6
		} else /if (${Spawn[npc radius 45 ${_tmpMob}].ID}) {
			/dismount
			SDEBUG ${dot}found${sep}\a-w${_tmpMob}\ax
			/invoke ${set_target[${Spawn[${_tmpMob}].ID}]}
			/target ID ${set_target[${Spawn[${_tmpMob}].ID}]}
			/delay 1s ${Target.ID} == ${Spawn[${_tmpMob}].ID}
		}

		:tov_ew_ada_step5_6_killing
			/face fast nolook
			/squelch /stick id moveback ${Math.Calc[${Spawn[id ${Target.ID}].MaxRangeTo}*.6]}
			/call task_combatloop ${Spawn[${_tmpMob}].ID}
			/delay 5
			/if (${sqlite.Result[farmquery${_tmpStep} 1 step].NotEqual[done]}) /goto :tov_ew_ada_step5_6_killing


	| step 7
		/invoke ${out[23, "\agSTEP 6\ax${sep}\a-w${sqlite.Result[farmquery7 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery7 1 step]}].Status}\ax)"]}
		/delay 5
		/advloot shared "Simple Velium Pendant" giveto ${Me.Name}
		/delay 5
		/advloot personal "Simple Velium Pendant" loot
		/delay 5
		/if (${Window[ConfirmationDialogBox].Open}) {
			/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
		}
		/delay 5

	| step 8


	/return TRUE







|***
 * note: gets a task
 * use: /call get_task DEBUG [name of task/quest]
 ***|
sub task_get(bool _debug, string _verbage)
	DEBUG \attask_get\ax(\a-w${_verbage}\ax)

	| get the dat a we need to get quest started
	/sqlite ${maData.Find[DBstatic].Value} farmquery SELECT * FROM farm WHERE step="start" AND task="${_verbage}"

	| are we in the right zone?
	/if (${Zone.ShortName.NotEqual[${sqlite.Result[farmquery 1 zonesn]}]}) {
		DEBUG ${sep}wrong zone${sep}\a-w${sqlite.Result[farmquery 1 zonesn]}\ax
		/return FALSE
	}

	| target mob up
	/if (!${Spawn[${sqlite.Result[farmquery 1 questmob]}].ID}) {
		DEBUG ${sep}mob not up${sep}\a-w${Spawn[${sqlite.Result[farmquery 1 questmob]}].DisplayName}\ax
		/return FALSE
	}

	| get to quest target we should be kinda close for a say
	/while (${Spawn[${sqlite.Result[farmquery 1 questmob]}].Distance} > 60) {
		/if (!${Me.Moving} || !${Navigation.Active}) {
			/call Bind_command_navto "${Spawn[${sqlite.Result[farmquery 1 questmob]}].ID}"
		}
		/delay 5
	}

	/invoke ${set_target[${Spawn[${sqlite.Result[farmquery 1 questmob]}].ID}]}

	/makemevisible
	/say ${sqlite.Result[farmquery 1 phrase]}

	| give quest window time to populate
	/delay 5s ${Task[${_verbage}].Index}

	/if (!${Task[${_verbage}].Index}) /return FALSE

/return TRUE



|***
 * note: outputs task details
 * use: /call task_output DEBUG [query#]
 ***|
sub task_output(bool _debug, string _verbage)
	DEBUG \attask_output\ax(\a-w${_verbage}\ax)

	/if (${Bool[${sqlite.Result[farmquery${_verbage} 1 questmob]}]}) {
		/invoke ${out[23, "mob${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 questmob]}\ax"]}
	}
	/if (${Bool[${sqlite.Result[farmquery 1 locyxz]}]}) {
		/invoke ${out[23, "locyxz${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 locyxz]}"]}
	}
	/if (${Bool[${sqlite.Result[farmquery${_verbage} 1 invis]}]} || ${Bool[${sqlite.Result[farmquery${_verbage} 1 ivu]}]} || ${Bool[${sqlite.Result[farmquery${_verbage} 1 sos]}]}) {
		/invoke ${out[23, "IV${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 invis]}\ax IVU${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 ivu]}\ax SoS${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 sos]}\ax"]}
	}
	/if (${Bool[${sqlite.Result[farmquery${_verbage} 1 phrase]}]}) {
		/invoke ${out[23, "phrase${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 phrase]}\ax"]}
	}

/return TRUE




























/return


| current step.. always
${Task[The Way Back Home].Step}

/nav locyxz 1605.96 -1847.82 -24.6491

| id if have task?
${Task[The Way Back Home].ID}



${Task[The Way Back Home].Objective[1].Instruction}


${Task[The Way Back Home].Objective[${Task[The Way Back Home].Step}].Status.Equal[Done]}



${Task[The Way Back Home].Objective[1].Status}

${sqlite.Result[farmquery 1 locyxz]}

${Task[The Way Back Home].Index}



${Task[The Way Back Home].Step.Equal[kodiak]}





sub task_combatloop(int _tmpID)

	/call check_heal_routines
	/if (${maChr.Find[swYaulp].Value}) {
		/if (!${timer_Check_Yaulp}) /call cast_yaulp FALSE
	}

	/call set_combat_control ${maDebug.Find[decision].Value} ${_tmpID}
	/if (AGRO && ${8}) /call get_agro_${Me.Class.ShortName} FALSE ${_tmpID}
	/if (${maChr.Find[stPctShieldFlash].Value} || ${maChr.Find[stCountShieldFlash].Value}) /call cast_shield_flash FALSE aa

	/call check_item_cycle ${maDebug.Find[item].Value} ${_tmpID} epic
	/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} ${_tmpID}
	/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} ${_tmpID}
	/if (MELEE) /call do_melee FALSE ${_tmpID}
	/if (DOT) /call cast_dot ${maDebug.Find[dot].Value} lsDoT ${_tmpID}
	/if (NUKE) /call cast_nuke ${maDebug.Find[nuke].Value} lsNuke ${_tmpID}
	/call check_item_cycle ${maDebug.Find[item].Value} ${_tmpID}
	/call check_deBuffs ${maDebug.Find[debuff].Value} ${_tmpID}


	| class specific thingies
	/if (${Select[${Me.Class.ShortName},PAL]}) {
		/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) /call check_harmonious
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_self ${maDebug.Find[temp].Value} stSteel
			/call check_buff_cycle_temp_self ${maDebug.Find[temp].Value} stStance
		}
	}

	/if (${maChr.Find[stPctManaRecovery].Value}) {
		/call do_mana_recovery ${maDebug.Find[manarecover].Value}
	}
	/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) {
		/call cast_stamina_recovery ${maDebug.Find[rest].Value}
	}



/return TRUE
