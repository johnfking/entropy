|***
 *	modes.inc
 *	Core Mode Specific Functions
 *	for doing strange stuff....
 *
 *
 ***|




|***
 *	DES: set mode type
 *	USE: /mode []
 *  NOTE: starts/ends non-standard modes

#bind fredmode /fredmode
sub Bind_fredmode(string _type, string _verbage, bool _debug)
	/declare _bind string local Mode
	/declare _list string local ${sql_return[FALSE, lstTypes${_bind}]}
	/if (${Bool[${_type}]}) {
		/varset _type ${list_isoloate[lstTypes${_bind}, ${_type}]}
		}
	/declare _count int local 0
	/declare _listout string local
	/declare _sep bool local		
	
	| set mode agro range
	/if (${_type.Equal[agro]}) {
		/if (${Range.Between[1,999999:${Int[${_verbage}]}]}) {
			/call sql_update FALSE ${chrID} setModeAgro ${_verbage}  
		} else {
			VOUT /mode agro [${cinfo}${setModeAgro}\ax] allowable range is ${cinfo}1 - 99999\ax
			/return
		}	
		
	| set harvest counts	
	} else /if (${_type.Find[harvestcount]}) {
		/if (${Range.Between[1,999999:${Int[${_verbage}]}]}) {
			/call sql_update FALSE ${chrID} setModeHarvestCount ${_verbage}  
		} else {
			VOUT /${_bind.Lower} harvestcount [${cinfo}${setModeHarvestCount}\ax] allowable range is ${cinfo}1 - 999999\ax
			/return
		}	

	| all other switches
	} else /if (${_list.Find[${_type}]}) {
		/varset sw${_bind}${_type} ${If[${sw${_bind}${_type}},FALSE,TRUE]}
		/if ((${_type.Equal[hunt]} || ${_type.Equal[scorch]})) {
			/call set_stop NOECHO
			/call clear_combat
		}
	}		

	| output
	/for _count 1 to ${_list.Count[|]}
		/varset _listout ${_listout} ${If[${_sep},${sep},]} ${If[${sw${_bind}${_list.Arg[${_count},|]}},${cgood},${cinfo}]}${_list.Arg[${_count},|].Lower}\ax
		/if (${_list.Arg[${_count},|].Equal[agro]}) /varset _listout ${_listout}:${cnum}${setModeAgro}\ax
		/if (${_list.Arg[${_count},|].Equal[harvestcount]}) /varset _listout ${_listout}:${cnum}${setModeHarvestCount}\ax
		/varset _sep TRUE
	/next _count
	VOUT /${_bind.Lower} [${_listout}]				

	/varset swOverrideLOS ${sql_return[FALSE, swOverrideLOS]}
	/varset swOverrideEngagePct ${sql_return[FALSE, swOverrideEngagePct]}
	/varset swAutoCursor ${sql_return[FALSE, swAutoCursor]}

/return
 ***|


|***
 *	DES: stop all non standard modes
 *	USE: /call switch_clear_mode
 *	INI: NA
 *  NOTE: 
 ***|
sub switch_clear_mode(bool _debug)
/return


	/declare _count int local 0
	/declare _list string local ${sql_return[FALSE, lstTypesMode]}
	/if (${swModeToken}) /varset swModeToken FALSE

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




|***
 *	DES: central get target and kill for hunting modes
 *	USE: /call mode_get_mob
 *  NOTE: 
 ***|
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



|***
 *	DES: Train non spell skills. Best to use during a duel with a pet.
 *	USE:
 *  NOTE: This is not working yet...
 ***|
sub mode_skill_train(string _skilltype, bool _debug)
	
	|**
	/declare _sep bool local
	/declare _listout string local
	/declare _count int local 0
	/declare _bind string local ${Me.Class.ShortName.Lower}
  /declare _type string	local |beg|bindwound
	**|
	
	
	ISMEDEAD
	SETHUD "Skill Train"
	/delay 1

	/if (${Me.Skill[Taunt]} < ${Skill[Taunt].SkillCap} && ${Me.AbilityReady[Taunt]}) /doability Taunt
	/if (${Me.Skill[Disarm]} < ${Skill[Disarm].SkillCap} && ${Me.AbilityReady[Disarm]}) /doability Disarm
	/if (${Me.Skill[Bash]} < ${Skill[Bash].SkillCap} && ${Me.AbilityReady[Bash]}) /doability Bash
	/if (${Me.Skill[Kick]} < ${Skill[Kick].SkillCap} && ${Me.AbilityReady[Kick]}) /doability Kick
	/if (${Me.Skill[Sneak]} < ${Skill[Sneak].SkillCap} && ${Me.AbilityReady[Sneak]}) /doability sneak
	
	/if (${Target.ID} && ${Target.ID} != ${Me.ID}) {
		/if (${Me.Skill[Begging]} < ${Skill[Begging].SkillCap} && ${Me.AbilityReady[Begging]}) /doability begging
	}
		
	| Bind Wounds
	/if (${Me.AbilityReady[Bind Wound]} && ${FindItem[=Bandages].ID}) {
		/if (${Me.Skill[Bind Wound]} < ${Skill[Bind Wound].SkillCap} && ${Me.PctHPs} < 99*.PCTHO) {
			/varset dump ${set_target[${Me.ID}]}
			/doability "Bind Wound"
		}
	}
	
	SETHUD FALSE
	
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
	/if (${Me.AbilityReady[fishing]}) /doability fishing
	
	/call check_cursor ${_debug} TRUE
	
	SETHUD FALSE

/return TRUE



|***
 * note: you got shit under your finger nails
 * use: /mode forage
 ***|
sub mode_forage(bool _debug)
	DEBUG \atmode_forage\ax()

	/if (${Select[${Me.CombatState},combat]}) /return
	
	/if (!${Me.AbilityReady[forage]}) {
		/return FALSE
	}	
	
	ISMEDEAD
	SETHUD "mode forage"
	
	/if (!${Me.Standing}) {
		/stand
	}
	
	/doability forage
	/delay 1s
	
	/call check_cursor ${_debug} TRUE
	SETHUD FALSE
	
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
	
	SETHUD FALSE
	
/return



|***
 *	DES: turns the toon into a hunter of things.
 *	USE: /mode hunt
  ***|
sub mode_hunt2(bool _debug)
	DEBUG \atmode_hunt2\ax()
	
	GETINPUT
	ISMEDEAD

	SETHUD "mode hunt"
	
	/declare _lsHunt list local
	/varset dump ${_lsHunt.Delimiter[|]}
	/varset dump ${_lsHunt.Append[${stModeData}]}
	/declare _lihunt listiterator local
	/vardata _lihunt _lsHunt.First.Clone
	/while (!${_lihunt.IsEnd}) {
		/if (${Spawn[npc ${_lihunt.Value} radius 999999 targetable].ID}) {
			/varset stAssTarID ${NearestSpawn[npc ${_lihunt.Value}].ID}
			/break 
		}
		/varset dump ${_lihunt.Advance}
	}	
	
	/if (${Spawn[id TARGET].ID}) {
		/call get_target_navpull ${_debug} TARGET
	}	else /if (!${Spawn[id TARGET].ID}) {
		/call clear_combat ${_debug}
		SETHUD FALSE
		/return FALSE		
	}
	
	/call set_combat ${_debug}
	/delay 1s
	SETHUD FALSE
	
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
	SETHUD FALSE
/return



|***
 * note: Pet Farm
 * use: /mode petfarm
 ***|
sub mode_petfarm(bool _debug)
	DEBUG \atmode_petfarm\ax()

	/if (!${lsClassPet.Contains[${Me.Class.ShortName}]} || SAFEZONE) {
		/varset dump ${maMode.Add[${_mimode.Key},FALSE]}
		/return FALSE
	}

	ISMEDEAD

	/varset dump ${maOver.Add[swOverPctEngage,TRUE]}
	/varset dump ${maOver.Add[swOverLOS,TRUE]}
	/varset swModeToken TRUE
	
	SETHUD "mode petfarm"
	
	GETINPUT

	/if (AUTO) {
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} FALSE
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[minion].Value} Pet
	}

	SETHUD "mode scorch"
	| find a mob
	/call pull_mob_find ${_debug}
	DEBUG pull_mob_find.return${sep}\a-w${Macro.Return}\ax

	/if (!TARGET) {
		DEBUG \arNO\ax stAssTarID
		SETHUD FALSE
		GETINPUT
		/call where_the_fuck_is_my_pet ${_debug} FORCE
		/return FALSE
	}
	
	CHECKTIE
	/doevents
	/varset dump ${set_target[TARGET]}

	/pet attack
	
	CHECKTIE
	
	/varset dump ${set_timer[${_debug}, timer_mob_get, 20s]}
		
	/while (TARGET) {
		GETINPUT
		/delay 1s

		/if (${Spawn[TARGET].Type.Equal[CORPSE]} || !${Spawn[TARGET].ID}) /break
		/if (${Me.Pet.Target.ID} != TARGET) /break
		/if (${Spawn[TARGET].Distance} > PULLENV) /break
		DEBUG ${sep}here${sep}\a-w${timer_mob_get}\ax \agTARGET\ax
		/if (!${timer_mob_get}) /break
	
	}		

	/varset dump ${maOver.Add[swOverPctEngage,FALSE]}
	/varset dump ${maOver.Add[swOverLOS,FALSE]}
	/varset swModeToken FALSE	
	/call where_the_fuck_is_my_pet ${_debug} FORCE
	/call clear_combat
	
	SETHUD FALSE
	
/return



|***
 *	DES: runs around and picks up shit
 *	USE: /mode harvest
 ***|
sub mode_harvest(bool _debug)
	DEBUG \atmode_harvest\ax()

	ISMEDEAD
	GETINPUT
	
	SETHUD "mode harvest"
	
	/declare _lsCollect list local
	/varset dump ${_lsCollect.Delimiter[|]}
	/varset dump ${_lsCollect.Append[${stModeData}]}
	/declare _licol listiterator local
	/vardata _licol _lsCollect.First.Clone
	
	/while (!${_licol.IsEnd}) {
		/if (${GroundItemCount[${_licol.Value}]}) {
			/call get_ground_item ${_debug} "${_licol.Value}"
		} 
		/varset dump ${_licol.Advance}
	}	
	
	/delay 1s
	SETHUD FALSE

/return
	


|***
 *	DES: moves to the ground item and picks it up
 *	USE: /call get_ground_harvest DEBUG [ITEM NAME]
 *  NOTE: 
 ***|
sub get_ground_item(bool _debug, string _tmpItem)
	DEBUG \atget_ground_item\ax(\a-w"${_tmpItem}"\ax)
	
	/call check_cursor ${_debug} CLEAR

	/if (!${GroundItemCount[${_tmpItem}]}) {
		/return FALSE
		/delay 2
	}

	/if (!${Navigation.MeshLoaded}) {
 		/varset dump ${maMode.Add[harvest,FALSE]}
 		/varset dump ${out[0, "${notice} ${dot} NO Nav Mesh for this zone."]}
 		/return	FALSE
	}

	ISMEDEAD

	/declare _item ground local
	/vardata _item Ground.First
	/declare _count int local 1
	/declare _distfinal int local 999999
	/declare _locY int local
	/declare _locX int local
	/declare _locZ int local
	
	/while (${_count} <= ${GroundItemCount}) {
		/if (${_item.DisplayName.Find[${_tmpItem}]}) {
			DEBUG ${Navigation.PathExists[locyxz ${_item.Y} ${_item.X} ${_item.Z}]}${sep}\a-wlocyxz ${_item.Y} ${_item.X} ${_item.Z}\ax
			/if (${Navigation.PathExists[locyxz ${_item.Y} ${_item.X} ${_item.Z}]}) {
				/if (${_item.Distance} < ${_distfinal}) {
					DEBUG ${_count}/${GroundItemCount} -- ${_item.DisplayName}\ax${sep}Path-\a-w${Navigation.PathExists[locyxz ${_item.Y} ${_item.X} ${_item.Z}]}\ax${sep}dis-\a-w${_item.Distance}\ax
					/varset _distfinal ${_item.Distance}
					/varset _locY ${_item.Y}
					/varset _locX ${_item.X}
					/varset _locZ ${_item.Z}
				}
			}

		}
		/if (${_count} == ${GroundItemCount}) /break
		/varcalc _count ${_count} + 1
		/if (${_count} >= ${GroundItemCount}) /break
		/vardata _item _item.Next
	}

	/nav locyxz ${_locY} ${_locX} ${_locZ} |dist=18 log=off
	/delay 2s
	/delay 3m !${Navigation.Active}
					
	/if (${Ground[${_tmpItem}].Distance3D} <= 19) {
		/call Bind_command_grab "${_tmpItem}" ${_debug}
		/delay 5
		
		/call check_cursor ${_debug} CLEAR
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
	SETHUD FALSE
/return



|***
 *	DES: 
 *	USE: /call mode_get_drunk 
 *  NOTE: 
 ***|
sub mode_get_drunk(bool _debug)
	/if (!${FindItem[${modeDrinkofChoice}].ID} || !${Bool[${modeDrinkofChoice}]}) {
		/varset swModeDrink FALSE
		/return
	}
	SETHUD "mode lush"

	| start drinking.
	/while (${Me.Skill[Alcohol Tolerance]} < ${Skill[Alcohol Tolerance].SkillCap}) {
		/if (!${Me.ItemReady[=${modeDrinkofChoice}]}) /break
		| /if (${Me.Drunk} < 200) /useitem ${modeDrinkofChoice}
		/if (${Me.Drunk} < 200) /call core_cast2 "${modeDrinkofChoice}" item 0 FALSE
		/delay 2
		/delay 2s ${Me.Drunk} < 200
	}

	| maxed skill turn this off
	/if (${Me.Skill[Alcohol Tolerance]} == ${Skill[Alcohol Tolerance].SkillCap}) {	
		VOUT Alcohol Tolerance Maxed ${sep} (${cnum}${Me.Skill[Alcohol Tolerance]}\ax)
		/varset swModeDrink FALSE
		/return
	}
	
	SETHUD FALSE
/return



|***
 *	DES: train spells
 *	USE: /call mode_trainspell | /mode trainspell
 *  NOTE: 
 ***|
sub mode_trainspell(string _type, bool _debug)
	ISMEDEAD
	SETHUD "Train Spell"
	
	/if (${_type.Equal[specialize]} && !${swModeTrainspellSPC}) {
		/varset swModeTrainspellSPC TRUE
	}

	/declare _skill string local |alteration|abjuration|conjuration|divination|evocation
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
		/if (!${Me.Gem[${_count}].Name.Equal[${${_skill.Arg[${_count},|]}}]}) /call mem_spell TRUE "${${_skill.Arg[${_count},|]}}" ${_count} FALSE
		
		/varset _gem[${_count}] TRUE
	/next _count

	| start getting things done
	/for _count 1 to ${_skill.Count[|]}
		/if (!${_gem[${_count}]}) /continue
		/if (${validate_cast[FALSE, spell, "${${_skill.Arg[${_count},|]}}", ${Me.ID}]}) /call core_cast2 "${${_skill.Arg[${_count},|]}}" 0 0 FALSE
		/call sort_cursor TRUE
	/next _count
/return



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

	SETHUD FALSE
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
		/varset dump ${maMode.Add[summon,FALSE]}
		/return FALSE
	}

	SETHUD "mode fetch corpse"

	| set stone to buy
	/declare _soulstone string local
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
	/varset dump ${maMode.Add[summon,FALSE]}

/return FALSE



|***
 * note: Scorched Earth Mode
 * use: /mode hunt
 ***|
sub mode_hunt(bool _debug)
	DEBUG \atmode_hunt\ax(\a-w\ax)

	/varset dump ${maOver.Add[swOverPctEngage,TRUE]}
	/varset dump ${maOver.Add[swOverLOS,TRUE]}
	/varset swModeToken TRUE
	/call clear_combat ${_debug}
	/declare _count int local 0

	SETHUD "mode hunt"

	:checkpath

	/if (${Bool[${stModeData}]}) {
		/declare _lsHunt list local
		/varset dump ${_lsHunt.Delimiter[|]}
		/varset dump ${_lsHunt.Append[${stModeData}]}
		/declare _lihunt listiterator local
		/vardata _lihunt _lsHunt.First.Clone
		/while (!${_lihunt.IsEnd}) {
			/if (${Spawn[npc ${_lihunt.Value} radius PULLENV targetable].ID}) {
				/varset stAssTarID ${NearestSpawn[npc ${_lihunt.Value}].ID}
				/break 
			}
			/varset dump ${_lihunt.Advance}
		}	

	} else /if (!${Bool[${stModeData}]}) {
		/call pull_mob_find ${_debug} ${_count}
		DEBUG pull_mob_find.return${sep}\a-w${Macro.Return}\ax		
	}

	/if (!TARGET) {
		DEBUG \arNO\ax stAssTarID
		/call clear_combat ${_debug}
		/return
	}

	/if	(!${Navigation.PathExists[locyxz ${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]}) {
		GETINPUT
		/varset dump ${out[0, "${notice}${sep}No Nav Path${sep}\a-w${Spawn[TARGET].DisplayName}\ax"]}
		/varcalc _count ${_count}+1
		/if (${_count} > 10) /return FALSE
		/delay 1s
		| /call pull_mob_find FALSE ${_count}
		/goto :checkpath
	 }

	| get to the mob
	| /declare _movetorange float local ${Math.Calc[${Spawn[TARGET].MaxRangeTo}*.9]}
	
	/call pull_mob_move_to ${_debug} TARGET ${Math.Calc[${Spawn[id TARGET].MaxRangeTo}*.9]} on
	
	| /delay 15s !${Navigation.Active}
	
	/if (${Spawn[id TARGET].Dead}) /return TRUE


	/varset dump ${maCC.Add[swSetCombat,TRUE]}
	/varset dump ${set_target[TARGET]}
		
	| /call check_facing ${_debug} TARGET FORCE
	
	| /if (MELEE) /squelch /attack on
	/call Bind_control_cc FORCE
	/call main_${Me.Class.ShortName}
	/call clear_combat
	
	
	|**
	| kill our  mob
	:checkforbullshitadds
	/while (TARGET || (${SpawnCount[NPC radius ENV PS4]} && !TARGET)) {
		ISMEDEAD
		GETINPUT
	
		/if (!TARGET) {
			/varset stAssTarID ${NearestSpawn[npc LOS radius ENV zradius ${maCC.Find[stZRadius].Value}].ID}
		}
		
		/if (TARGET) {
			/call Bind_control_cc FORCE
			/delay 1s
		}
		/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE	
		
	}	
	
	**|
	
	/varset dump ${maOver.Add[swOverPctEngage,FALSE]}
	/varset dump ${maOver.Add[swOverLOS,FALSE]}
	/varset swModeToken FALSE

	SETHUD FALSE

/return







|***
 * note: basic combat routine used by modes
 * use: /mode hunt
 ***|
sub mode_combat_routine(bool _debug)
	DEBUG \atmode_combat_routine\ax()

	/varset dump ${set_target[TARGET]}	

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
