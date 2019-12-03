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
 ***|
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



|***
 *	DES: stop all non standard modes
 *	USE: /call switch_clear_mode
 *	INI: NA
 *  NOTE: 
 ***|
sub switch_clear_mode(bool _debug)
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
/return




|***
 *	DES: central get target and kill for hunting modes
 *	USE: /call mode_get_mob
 *  NOTE: 
 ***|
sub mode_get_mob(bool _debug)
	:movetomob
	GETINPUT
	ISMEDEAD

	CLEARCURSOR TRUE

	/if (!${Spawn[TARGET].ID} || ${Spawn[TARGET].Dead} || ${Target.Dead}) {
		/call set_stop NOECHO
		/call clear_combat
		/return
	}

	| /call check_watch_spawn

	| get us to the mob
	| /if (${Spawn[TARGET].Distance} > ${setCombatDistance} && ${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} > ${setMobAgro}) {
	/if (${Spawn[TARGET].Distance} > ${setCombatDistance} && ${Spawn[TARGET].Distance} > ${setMobAgro}) {
		/squelch /face ID TARGET nolook ${If[${swFaceFast},fast,]}
		
		| use nav for movement
		/if (${setTieMode.Equal[nav]} && ${Bool[${Plugin[MQ2Nav]}]} && !${Navigation.Active}) {
			| /echo nav to TARGET
			/if (${Navigation.Active}) /nav stop log=off
			/squelch /nav ID TARGET log=off

		| use stick for movement
		} else /if (${setTieMode.Equal[stick]} && ${Bool[${Plugin[MQ2MoveUtils]}]} && !${Stick.Active}) {
			/squelch /stick ID TARGET ${setCombatDistance} UW
		}
		
	| at the mob, do something about it		
	} else /if (${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} <= ${setMobAgro}) {
		/squelch /face ID TARGET nolook ${If[${swFaceFast},fast,]}
		/squelch /target ID TARGET
	}
					
	/if (${Spawn[TARGET].Distance} < ${setMobAgro} && ${Spawn[TARGET].Distance} > 31) {
		/squelch /target ID TARGET
		/if (${Me.Pet.ID} && !${Me.Pet.Target.ID}) /pet attack
		/if (${Target.ID} && ${Spawn[TARGET].LineOfSight}) /range
	} else /if (${Spawn[TARGET].Distance} < ${setCombatDistance}) {
		/call set_combat COMBAT
		/if (${Me.Pet.ID} && !${Me.Pet.Target.ID}) /pet attack
		
		/if (${Bool[${ModeNuke}]}) {
			/if (${validate_cast[FALSE, FIND, "${ModeNuke}", TARGET]}) /call core_cast2 "${ModeNuke}" FIND TARGET FALSE 
		}
	} 

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
		/if (${Me.Skill[Bind Wound]} < ${Skill[Bind Wound].SkillCap} && ${Me.PctHPs} < 99*.${tmpHealOverride}) {
			/squelch /target ID ${Me.ID}
		  /delay 3s ${Target.ID} == ${Me.ID}
			/delay 2s
			/doability "Bind Wound"
		}
	}
	
	SETHUD FALSE
/return



|***
 *	DES: to help your rusty dagger collection.. why else would you fish?
 *	USE: /call do_fish_mode | /mode fish
 *  NOTE: 
 ***|
sub mode_fish(bool _debug)
	ISMEDEAD
	SETHUD "Fish"
	/if (!${Me.Standing}) /stand
	/if (${Me.ItemReady[Fisherman's Companion]} && !${Me.ItemReady[Brell's Fishin' Pole]}) {
		/if (${Me.Invis}) /docommand /makemevisable
		/if (${validate_cast[FALSE, item, "Fisherman's Companion", ${Me.ID}]}) /call core_cast2 "Fisherman's Companion" item ${Me.ID}
	}
	/if (${Me.AbilityReady[fishing]}) /doability fishing
	SETHUD FALSE
/return



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
 *	USE: /call mode_hunt | /mode hunt
 *  NOTE: works better in open zones. don't say i didn't wan you.
 ***|
sub mode_hunt(bool _debug)
	/declare _count int local 0
	/doevents
	ISMEDEAD

	SETHUD "Hunt"
	CLEARCURSOR TRUE
	/varset swModeToken TRUE
	
	/for _count 1 to ${HuntMobList.Count[|]}
		/if (${Spawn[npc ${HuntMobList.Arg[${_count},|]} radius ${setModeAgro} targetable].ID}) {
			/varset Target2Kill ${NearestSpawn[npc ${HuntMobList.Arg[${_count},|]}].ID}
			/call echos hunt 0 0 TARGET 0
			/goto :haveatarget
		}
	/next _count	

	:haveatarget
	/if (!TARGET) {
		/call timer_set timer_wait_for_mob_to_kill ${tHunt_delay}
		/call check_buff_routines TRUE
	}

	/if (!${Bool[${FindItemCount[${lstRangeItemSummon}]}]} && ${Bool[${lstRangeItemSummon}]}) /call check_ammo
	/if (${swADVL}) {
		/if ((${coreAuto} || ${swOverrideLoot}) && !${SafeZone} && !${swOverrideSafeZone}) /call do_loot
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
 *	DES: Pet Farm
 *	USE: /call mode_petfarm | /mode petfarm
 *  NOTE: Use with caution. For static pet classes only.
 ***|
sub mode_petfarm(bool _debug)
	ISMEDEAD

	/declare _tmpID					int local 0
	/declare _spawnCount 		int local 0
	/declare _resetCount		int local 0
	/declare _nameloop			int local 1

	/varset swOverrideEngagePct TRUE
	/varset swOverrideLOS TRUE
	/varset swModeToken TRUE
	
	SETHUD "Pet Farm"
	:loopfindmob
	/doevents
	/if (!${Select[${Me.Class.ShortName},MAG,ENC,NEC,BST,SHD,SHM,RNG,DRU]} || (${SafeZone} && !${swOverrideSafeZone})) {
		/call echos failboat 0 0 0 0
		/call switch_clear_mode
		/return
	}
	/call check_watch_spawn
	/if ((${coreAuto} || ${swOverrideLoot}) && !${SafeZone} && !${swOverrideSafeZone}) /call do_loot
	
	/if (${Select[${Me.Class.ShortName},MAG]} && ${switchHealPet}) /call pet_heal_MAG

	/if (${SpawnCount[npc ${If[${swOverrideLOS},,los]} radius ${setMobAgro} zradius ${setzradius} targetable]}) {
		/varset _spawnCount ${SpawnCount[npc ${If[${swOverrideLOS},,los]} radius ${setMobAgro} zradius ${setzradius}]}	
		/varset Target2Kill ${Spawn[${NearestSpawn[1, ${If[${swOverrideLOS},,los]} npc radius ${setMobAgro} zradius ${setzradius} targetable]}].ID}

		| skip excluded mobs
		:loopnamedmob
		/if (${ExcludeList.Find[${Spawn[id TARGET].DisplayName}]})	{
			/varcalc _nameloop ${_nameloop}+1
			/if (${_spawnCount} > 1) {
				/varset Target2Kill ${Spawn[${NearestSpawn[${_nameloop}, ${If[${swOverrideLOS},,los]} npc radius ${setMobAgro} zradius ${setzradius} targetable]}].ID}

				/goto :loopnamedmob
			} else {
				/varset Target2Kill 0
				/varset _nameloop 1
			}
		}
				
		/if (!TARGET) /goto :nomob
		/varset _resetCount 0
	} else {
		:nomob
		/doevents
		/varset _resetCount 0
		/varset _spawnCount 0
		/varset Target2Kill 0
		/varset _nameloop 1
		/if (${timer_Petfarm_delay}) /goto :loopfindmob

		/if (!${timer_Petfarm_delay}) /call timer_update timer_Petfarm_delay ${tPetfarm_delay}
		/if (${Me.Pet.Target.ID}) /pet back off
		/if (${Me.AltAbilityReady[Summon Companion]} && ${Me.Pet.Distance} > 80) {
			/if (${validate_cast[FALSE, alt, "Summon Companion", ${Me.Pet.ID}]}) /call core_cast2 "Summon Companion" alt 0 FALSE
		}

		/return
	}
	
	:gettarget
	CHECKTIE
	/doevents
	/squelch /target ID TARGET
	/if (!${ExcludeList.Find[${Spawn[id TARGET].DisplayName}]}) {
		/if (!${Pet.Target.ID}) {
			| /delay 5
			/call echos petfarm 0 TARGET 0 0
			/pet attack
			/delay 2s
		}
	}
	/varcalc _resetCount ${_resetCount}+1
	
	/if (${_resetCount} > 30) {
		/varset _resetCount 0
		/if (${validate_cast[FALSE, alt, "Summon Companion", ${Me.Pet.ID}]}) /call core_cast2 "Summon Companion" alt 0 FALSE
		/goto :loopfindmob
	}		
		
	:loopwaittilldead
	/delay 1s
	/doevents
	CHECKTIE
	/if (${Spawn[TARGET].Type.Equal[CORPSE]} || !${Spawn[${_tmpID}].ID}) /goto :loopfindmob
	/if (${Spawn[TARGET].Distance} > ${setMobAgro}) {
		/goto :loopfindmob
	}
	/if (${Me.Pet.Target.ID} != TARGET) /goto :gettarget
	/if (${Spawn[TARGET].Type.NotEqual[CORPSE]}) /goto :loopwaittilldead

	/varset swModeToken FALSE
	SETHUD FALSE
/return



|***
 *	DES: Scorched Earth Mode
 *	USE: /call mode_scorched_earth | /mode scorch
 *  NOTE: This will kill everything in a zone. or at least try /shrug
 ***|
sub mode_scorched_earth2(bool _debug)
	ISMEDEAD
	/declare _spawnCount 		int local 0
	/declare _resetCount		int local 0
	/declare _nameloop			int local 1
	/declare _tmpID 				int local
	/declare _tmpRange 			int local ${Math.Calc[${setMobAgro}/10]}
	/declare _count 				int local 0


	/varset swOverrideEngagePct TRUE
	/varset swOverrideLOS TRUE
	/call clear_combat
	
	:loopfindmob
	SETHUD "Kill 'Em All"
	/doevents
	/if (!${swModeScorch}) /return
	/if (${SafeZone} && !${swOverrideSafeZone}) {
		/call echos failboat 0 0 0 0
		/call switch_clear_mode
		/return
	}
	/call check_watch_spawn
	/if ((${coreAuto} || ${swOverrideLoot}) && !${SafeZone} && !${swOverrideSafeZone}) /call do_loot
	/call check_buff_routines TRUE
	/if (!${Bool[${FindItemCount[${lstRangeItemSummon}]}]} && ${Bool[${lstRangeItemSummon}]}) /call check_ammo



	/if (${SpawnCount[npc ${If[${swOverrideLOS},,los]} radius ${setModeAgro} targetable]}) {
		
		| find something in our local range first
		/while (${_tmpRange} <= ${setMobAgro} && !TARGET) {
			/if (${SpawnCount[npc ${If[${swOverrideLOS},,los]} range ${setMobLvlMin} ${setMobLvlMax} radius ${_tmpRange} zradius ${setzradius} targetable]}) {
				/for _count 1 to ${SpawnCount[npc ${If[${swOverrideLOS},,los]} range ${setMobLvlMin} ${setMobLvlMax} radius ${_tmpRange} zradius ${setzradius} targetable]}
					ISMEDEAD
					/if (!${ExcludeList.Find[${Spawn[id ${_tmpID}].DisplayName}]}) {
						/varset _tmpID ${NearestSpawn[${_count}, npc ${If[${swOverrideLOS},,los]} range ${setMobLvlMin} ${setMobLvlMax} radius ${_tmpRange} zradius ${setzradius} targetable].ID}
					}
			
				/next _count
			}
			/varcalc _tmpRange ${_tmpRange}+10
		}	
		
		| nothing in local range, check our max range
		/if (!TARGET) {
			/if (${SpawnCount[npc ${If[${swOverrideLOS},,los]} radius ${setModeAgro} targetable]}) {
				/varset _spawnCount ${SpawnCount[npc ${If[${swOverrideLOS},,los]} radius ${setModeAgro} targetable]}	
				/varset Target2Kill ${Spawn[${NearestSpawn[1, npc ${If[${swOverrideLOS},,los]} radius ${setModeAgro} targetable]}].ID}
				/call timer_set timer_scorched_earth_fail ${tScorchedEarth_fail}

				:loopignoreddmob
				
				/if (${ExcludeList.Find[${Spawn[id TARGET].DisplayName}]})	{
					/varcalc _nameloop ${_nameloop}+1
					/if (${_spawnCount} > 1) {
						/varset Target2Kill ${Spawn[${NearestSpawn[${_nameloop}, npc ${If[${swOverrideLOS},,los]} radius ${setModeAgro} targetable]}].ID}
						/goto :loopignoreddmob
					} else {
						/varset Target2Kill 0
						/varset _nameloop 1
					}
				}
				/if (!TARGET) /goto :nomob
				/varset _resetCount 0
			}
		}
				
	} else {
		:nomob
		/doevents
		/varset _resetCount 0
		/varset _spawnCount 0
		/varset Target2Kill 0
		/varset _nameloop 1
		/if (!${timer_ScorchedEarth_delay}) /call timer_set timer_ScorchedEarth_delay ${tScorchedEarth_delay}
		/if (${Me.Pet.Target.ID}) /pet back off
		/if (${Me.Pet.ID} && ${Me.AltAbilityReady[Summon Companion]} && ${Me.Pet.Distance} > 80) {
			/if (${validate_cast[FALSE, alt, "Summon Companion", ${Me.Pet.ID}]}) /call core_cast2 "Summon Companion" alt 0 FALSE
		}
		/return
	}
	/call echos hunt 0 0 TARGET 0

	:gettarget
	/doevents
	
	/if (!${ExcludeList.Find[${Spawn[id TARGET].DisplayName}]} && !${timer_scorched_earth_ignore_$TARGET}) {
		/if (${Spawn[TARGET].ID}) {
			/call mode_get_mob
		} else /if (${Spawn[TARGET].Dead} || !${Spawn[TARGET].ID} || !${timer_scorched_earth_fail}) {
			/if (!${timer_scorched_earth_fail}) /call timer_set timer_scorched_earth_ignore_$TARGET ${tScorchedEarth_ignore} 
			/call set_stop NOECHO
			/call clear_combat
		}
	}
	
	/varcalc _resetCount ${_resetCount}+1

	/if (${_resetCount} > 30) {
		/varset _resetCount 0
		/if (${Me.Pet.ID} && ${Me.AltAbilityReady[Summon Companion]} && ${Me.Pet.Distance} > 80) {
			/if (${validate_cast[FALSE, alt, "Summon Companion", ${Me.Pet.ID}]}) /call core_cast2 "Summon Companion" alt 0 FALSE
		}
		/goto :loopfindmob
	}		
		
	:loopwaittilldead
	/delay 1s
	/doevents
	/if (${Spawn[TARGET].Type.Equal[CORPSE]} || !${Spawn[TARGET].ID}) /goto :loopfindmob
	/if (${Spawn[TARGET].Distance} > ${setMobAgro}) {
		/goto :loopfindmob
	}
	/if (${Me.Pet.Target.ID} != TARGET) /goto :gettarget
	/if (${Spawn[TARGET].Type.NotEqual[CORPSE]}) /goto :loopwaittilldead

	SETHUD FALSE
/return



|***
 *	DES: Harvest Mode
 *	USE: /call mode_Harvest | /mode harvest
 *  NOTE: Will run around and pick up shit off the ground
 ***|
sub mode_harvest(bool _debug)
	ISMEDEAD
	/declare _count int local
	SETHUD "Harvest"
	/if (!${swAutoCursor}) /varset swAutoCursor TRUE

	CLEARCURSOR TRUE
	/for _count 1 to ${HarvestList.Count[|]}
		/if (${FindItemCount[${HarvestList.Arg[${_count},|]}]} >= (${setModeHarvestCount})) {
			/varset swModeHarvest FALSE
			/autoinventory
			/return
		}
		/if (${GroundItemCount[${HarvestList.Arg[${_count},|]}]}) {
			/call get_ground_harvest "${HarvestList.Arg[${_count},|]}"
		} else {
			/if (!${timer_Harvest_delay}) /call timer_set timer_Harvest_delay ${tHarvest_delay}
		}
	/next _count	

	SETHUD FALSE
/return



|***
 * note: Collection go getter thingie
 * use: /mode collect
 ***|
sub mode_collect(bool _debug)
	DEBUG \atmode_collect\ax()

	/if (!${GroundItemCount[Glowing Globe]}) {
		/return FALSE
		/delay 2
	}

	/if (!${Navigation.MeshLoaded}) {
 		/varset dump ${maMode.Add[collect,FALSE]}
 		/varset dump ${out[0, "${notice} ${dot} NO Nav Mesh for this zone."]}
 		/return	FALSE
	}

	ISMEDEAD
	SETHUD "mode collect"

	/declare _item ground local
	/vardata _item Ground.First
	/declare _count int local 1
	/declare _distfinal int local 999999
	/declare _locY int local
	/declare _locX int local
	/declare _locZ int local
	
	/while (${_count} <= ${GroundItemCount}) {
		/if (${_item.DisplayName.Find[Glowing Globe]}) {
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

	/nav locyxz ${_locY} ${_locX} ${_locZ} | dist=16 log=off
	/delay 2s
	/delay 3m !${Navigation.Active}
					
	/if (${Ground[Glowing Globe].Distance3D} < 19) {
		/call Bind_command_grab "glowing" ${_debug}
		/delay 5
		
		/call check_cursor ${_debug} TRUE
	}

	SETHUD FALSE

/return














|***
 *	DES: moves to the ground item and picks it up
 *	USE: /call get_ground_harvest "ITEM NAME"
 *  NOTE: 
 ***|
sub get_ground_harvest(string _tmpItem, bool _debug)
	/squelch /itemtarget ${_tmpItem}
	/delay 2s ${Bool[${Ground.ID}]}
	/if (${Bool[${Ground.ID}]}) {
		/if (${Ground.Distance3D} < ${setModeAgro}) {
			
		/if (${Bool[${Plugin[MQ2Nav]}]} && ${setTieMode.Equal[nav]}) {	
			/if (${Navigation.MeshLoaded}) {	
				/delay 1 
				
				/if (${Navigation.PathExists[item]}) {
	        /nav item log=off
	    	} else /if (!${Navigation.PathExists[item]}) {
	    		/echo no path
	    	}
				
				/if (${Ground.Distance3D} < 19) /call Bind_grab ${_tmpItem}
	
				/nav stop log=off
				
			} else /if (!${Navigation.MeshLoaded}) {
				OUT NO Nav Mesh for this zone. Returning to Stick.
				/call Bind_setTieControl mode stick
			}
				

		} else {
			:loopMove 
			/face nolook loc ${Ground.Y}, ${Ground.X}
			/delay 1 
			/keypress forward hold 
			/if (${Ground.Distance3D} > 19) /goto :loopMove
			/keypress forward
			/click left item
			/delay 5s (${Cursor.ID})
			/delay ${Math.Rand[10]}
			/autoinventory
			CLEARCURSOR TRUE
			/delay 5s (!${Cursor.ID})
			/delay ${Math.Rand[10]}
		}

	}
/return




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
 * use: /mode scorch
 ***|
sub mode_scorch(bool _debug, int _tmpID)
	DEBUG \atmode_scorch\ax(\a-w${_tmpID}\ax)

	/varset swOverrideEngagePct TRUE
	/varset swOverrideLOS TRUE
	/varset swModeToken TRUE
	
	/call clear_combat


	/declare _count int local 0


	SETHUD "mode scorch"
	| find a mob
	/call pull_mob_find ${_debug}
	
	DEBUG pull_mob_find.return${dep}\a-w${Macro.Return}\ax

	/if (!TARGET) {
		DEBUG \arNO\ax stAssTarID
		SETHUD FALSE
		/return FALSE
	} else {
		/if (!${_tmpID}) /varset _tmpID TARGET
	}

	:checkpath
	/if	(!${Navigation.PathExists[locyxz ${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]}) {
		GETINPUT
		OUT ${notice}${sep}No Nav Path${sep}\a-w${Spawn[TARGET].DisplayName}\ax
		/varcalc _count ${_count}+1
		/if (${_count} > 10) /return FALSE
		/delay 1s
		/call pull_mob_find ${_debug} ${_count}
		/goto :checkpath
	}	

	| get to the mob
	/declare _movetorange float local ${Math.Calc[${Spawn[TARGET].MaxRangeTo}*.8]}

	/nav ID TARGET |distance=${_dist} lineofsight=${_LoS} log=off
	/delay 1s ${Me.Moving}
	
	/while (${Navigation.Active}) {
		DEBUG moving to ${dep} \a-w$TARGET\ax
		/delay 5
		
		GETINPUT
		/if (!TARGET) /return FALSE
		/call pull_mob_tag ${_debug} TARGET
		/if (${Macro.Return}) {
			/nav stop |log=off
			/break
		}
	}
	
	/varset swModeToken FALSE
	SETHUD FALSE

/return




