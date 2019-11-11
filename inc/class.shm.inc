|***
 * entropy
 * class.shm.inc
 ***|
 


|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub set_SHM(bool _debug)
	DEBUG \atset_SHM\ax()

	/call set_data FALSE maData stSpire1																map outer "Fundament: First Spire of Ancestors"
	/call set_data FALSE maData stSpire2																map outer "Fundament: Second Spire of Ancestors"
	/call set_data FALSE maData stSpire3																map outer "Fundament: Third Spire of Ancestors"
	/call set_data FALSE maData stSynergy																map outer "Luminary's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Spear of Fate"
	/call set_data FALSE maData stEpic15																map outer "Crafted Talisman of Fates"
	/call set_data FALSE maData stEpic2																	map outer "Blessed Spiritstaff of the Heyokah"
	/call set_data FALSE maData stFade																	map outer DALSE
	/call set_data_map FALSE maChr stAAUnity														"Visionary's Unity"								"${auxna} class aa unity buff"

		
	/c/varset dump ${set_data_timer[FALSE, Check_Talisman, 30s]}
	/call sql_control FALSE ${chrID} stTalisman	 												PREGEN|1
	/call sql_control FALSE ${chrID} swTalisman			 										FALSE
                                          		
	                                        		
	/call set_data_list FALSE maChr lsNukeTC									 					PREGEN|2											"${auxli} twincast nuke procs"
	
	/call set_data_map FALSE maChr stCannibalization										PREGEN|1											"${auxnum} canni spell"
	/call set_data_map FALSE maChr stPctCanniHP				 									60														"${auxnum} HP level to stop using canni"
	/call set_data_map FALSE maChr stPctCanniManna				 							60														"${auxnum} mana level to start using canni"

	/call set_data_map FALSE maChr swSurge															FALSE													"${auxsw} use surge healing"
 
 	/call set_data_list FALSE maChr lsHealSurge													PREGEN|1											"${auxli} area surg spells"
  /call set_data_list FALSE maChr lsHealMain													PREGEN|3											"${auxli} main list of heal spells"											
	/call set_data_list FALSE maChr lsHealIntervention									PREGEN|3											"${auxli} intervention nuke/healing spells"
	/call set_data_list FALSE maChr lsHealReckless											PREGEN|3											"${auxli} reckless healing spells"



  /call set_data_map FALSE maChr swHealGroupOverTime									FALSE													"${auxsw} use group heal over time spells/aas"
  /call set_data_map FALSE maChr stHealGroupOverTime									PREGEN|1											"${auxna} group heal over time spell"

	/call set_data_map FALSE maChr swHealOverTime												FALSE													"${auxsw} % of members health to use heal over time"
	/call set_data_map FALSE maChr stHealOverTime												PREGEN|1											"${auxna} heal over time spelll"

	/call set_data_map FALSE maChr stCountHealGroup											0															"${auxnum} number in group hurt to use a group heal"
 	/call set_data_list FALSE maChr lsHealGroup													PREGEN|2											"${auxli} group heal spells"


	| temp buff: growth family
  /call set_data_list FALSE maChr lsGrowthToon	 											EMPTY													"${auxli} list of toons to use ward temp hp growth on"
	/call set_data_map FALSE maChr stGrowth			 												PREGEN|1											"${auxna} temp hp growth spell"





/return























sub main_SHM(bool _debug)
	DEBUG main_SHM()
	GETINPUT
	ISMEDEAD
	CHECKTIE
	
	| correct any and all XTarget healing spots
	/if (AUTO) {
	  /if (${swHealXTarget}) {
	  	/if (!${timer_set_xtarget}) {
		    /call set_xtarget
		    /call timer_update timer_set_xtarget ${tXHeal}
		  }	
		}
		
		/if (!${Me.Invis}) {
			/call check_cani
			/if (!${check_engaged[FALSE]} && ${swPet}) /call check_pet
		}
	}	

	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Group
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/if (${maHeal.Find[swHealXTarget].Value}) /call check_heal_${Me.Class.ShortName} FALSE XTarget
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Pet
		/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
		CHECKFADE
		| /if (${swAlliance}) /call trigger_alliance

		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list FALSE stGrowth lsGrowthToon
		}

		/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
		/if (BURN) /call burn_${Me.Class.ShortName} FALSE
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call set_combat FALSE
			/if (${validate_mitigation[FALSE, FALSE, ${setCombatRadius}]}) /call mitigation_cycle FALSE
			/if (${swBurnControl}) /call burn_${Me.Class.ShortName}
			/if (${swCombatMelee}) /call use_skill_melee ${Target2Kill}
			/call disc_${Me.Class.ShortName}
			/call check_deBuffs FALSE ${Target2Kill}
			/if (${swCombatDoT}) /call cast_DOT FALSE ${Target2Kill}
			/if (${swCombatNuke}) /call cast_nuke FALSE ${Target2Kill}
			/if (${swAlliance}) /call trigger_alliance ${Target2Kill}
			/if (${swSwarm} || ${swPet}) /call send_pet FALSE ${Target2Kill}
			/call click_BP
			/call click_item
			/if (${swSwarm}) /call click_item_swarm FALSE ${Target2Kill}
			/call click_DPS_${Me.Class.ShortName}
		}

		/if (AUTO) {
			CHECKFADE
			/call check_cani	
			/if (${swModRod} && ${validate_class[FALSE, ${lstClassCast}]}) /call use_mod_rod
			/if (${validate_class[FALSE, ${lstClassMelee}]}) /call cast_stamina_recovery
		}
	}

/return


|***
 *	DES: Combat skills, Disciplines and Alternate abilities.
 *	USE: Disc
 *  NOTE: 
 ***|
sub disc_SHM(bool _debug)
	DEBUG disc_SHM()
	CHECKEXIT
	GETINPUT
	/if (${swTalisman}) /call cast_talisman
	/if (${validate_mitigation[FALSE, FALSE, ${setCombatRadius}]}) /call mitigation_cycle FALSE
/return






|***
 *	DES: 
 *	USE: 
 *  NOTE: 
 ***|
sub check_heal_SHM(bool _debug, string _type)
	DEBUG check_heal_SHM(${_debug}), ${_type}
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	UPDATEHUD "HEAL ${_type}"
	
	/call check_hurt FALSE ${_type}
	/declare _count int local 0
	
	| Group regeneration
 	/if (${useGroupHoT} && !${hurtCount}) { 
 		/if (${SpawnCount[npc radius ${setMobAgro} playerstate 4 targetable]}) {
	 	 	/if (${Me.SpellReady[${Spell[${stHealGroupOverTime}].RankName}]} && !${Bool[${Me.Song[Celestial Regeneration]}]} && !${Bool[${Me.Song[${stHealGroupOverTime}]}]} && !${Bool[${Me.Song[Elixir of the Seas]}]} && !${Bool[${Me.Song[Elixir of Wulthan]}]}) {
		    /if (${validate_cast[FALSE, spell, "${stHealGroupOverTime}", ${Me.ID}]}) {
		    	/call core_cast2 "${stHealGroupOverTime}" 0 ${Me.ID} FALSE
		    }
		  }
		}
	}	
	
	| no one is hurt. we're out of here
	/if (!${hurtCount}) {
	  | check for surge
	  /if (${swSurge}) {
			/if (${SpawnCount[npc radius ${setMobAgro} playerstate 4 targetable]}) /call cast_surge_heal
		}	
		UPDATEHUD FALSE
		/return
	}

	CHECKREZ
	
  | stationary ward heal
  /if (${hurtCount} >= ${useGroupHeal}) {
    /if (${validate_cast[FALSE, alt, "Call of the Ancients", ${Me.ID}]}) {
    	/call core_cast2 "Call of the Ancients" alt ${Me.ID} FALSE
    }
  } 
  
	| Panic Ancestral Guard
	/if (${hurtPctHP} < ${PanicPoint}*.${tmpHealOverride}) {
		/if (${validate_cast[FALSE, alt, "Ancestral Guard", ${Me.ID}]}) {
			/if (${Me.Casting.ID}) /stopcast
			/call core_cast2 "Ancestral Guard" alt 0 FALSE
		}
	}
  
  /if (!${check_hurt[FALSE, ${_type}]}) /return FALSE
  
 	| agressive healing sort
 	/if (${switchHealAggressive}) {
 		/if (${Me.AltAbility[${lsHealIntervention}]} || ${Me.SpellReady[${Spell[${lsHealIntervention}].RankName}]}) /goto :aggressivesort
 		/for _count 1 to 3
 			/if (${Me.SpellReady[${Spell[${healReckless${_count}}].RankName}]}) /goto :aggressivesort
 		/next _count
 	} 
 	
 	| main heal
	/if (${hurtPctHP} <= ${${hurtCLS}Point}*.${tmpHealOverride}) {
		/if (${validate_cast[FALSE, spell, "${lsHealMain}", ${hurtID}]}) {
			/call core_cast2 "${lsHealMain}" 0 ${hurtID} FALSE
		}
	}
  
	:aggressivesort
	
	| intervention
	/call cast_heal ${_debug} 1 Intervention
 	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE	
	
	| reckless
	/call cast_heal ${_debug} 3 Reckless
	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE	
 	
	UPDATEHUD FALSE
/return



|***
 *	DES: surge heal
 *	USE: part of heal routine
 *  NOTE: this will try and get a twinheal proc first.
 ***|
sub cast_surge_heal(bool _debug)
	DEBUG cast_surge_heal(${_debug})
	/if (!${Bool[${TC1}]} && !${Bool[${TC2}]}) /return
	/if (!${Me.SpellReady[${Spell[${TC1}].RankName}]} && !${Me.SpellReady[${Spell[${TC2}].RankName}]}) /return
	/if (${is_target_dead[${Target2Kill}]}) /return	

	CHECKEXIT
	ISMEDEAD
	GETINPUT

	| get the main assisst ID
	/declare _tmpID int local ${get_assist_info[FALSE, ID]}
	DEBUG assist_name: ${_tmpID}

	/if (!${Spawn[${_tmpID}].LineOfSight}) /return FALSE
	
  /if (${Me.SpellReady[${Spell[${TC1}].RankName}]} && !${Me.Song[Healing Twincast].ID}) {
		/if (${validate_cast[FALSE, spell, "${TC1}", ${_tmpID}]}) /call core_cast2 "${TC1}" 0 ${_tmpID} FALSE
	} else /if (${Me.SpellReady[${Spell[${TC2}].RankName}]} && !${Me.Song[Healing Twincast].ID}) {
		/if (${validate_cast[FALSE, spell, "${TC2}", ${_tmpID}]}) /call core_cast2 "${TC2}" 0 ${_tmpID} FALSE
	}

	| spalsh surge heal	
	/if (${Me.SpellReady[${Spell[${lsHealSurge}].RankName}]} && ${Me.Song[Healing Twincast].ID}) {
		/delay 2.25s ${Me.SpellReady[${Spell[${lsHealSurge}].RankName}]}
		/if (${validate_cast[FALSE, spell, "${lsHealSurge}", ${_tmpID}]}) /call core_cast2 "${lsHealSurge}" 0 ${_tmpID} FALSE
	}
/return



 |***
 *	DES: Shaman burn routines
 *	USE: establish all comp/trigger skills
 *	NOTE: 
 ***| 
sub burn_SHM(bool _debug)
	DEBUG burn_SHM()
	CHECKEXIT
	GETINPUT

	| epic 2.0
	/if (${validate_cast[FALSE, epicitem, "2", ${Me.ID}]}) /call core_cast2 "${${Me.Class.ShortName}Epic2}" item 0 FALSE

	| swarm aa pet
	/if (${swSwarm}) {
		/if (${validate_cast[FALSE, alt, "Spirit Call", ${Target2Kill}]}) /call core_cast2 "Spirit Call" alt ${Target2Kill} FALSE
	}

	| Lower spell resists
	/if (${validate_cast[FALSE, alt, "Dampen Resistance", ${Target2Kill}]}) /call core_cast2 "Dampen Resistance" alt ${Target2Kill} FALSE

	| Twincast next 6 DOTs/HOTs
	/if (${validate_cast[FALSE, alt, "Fleeting Spirit", ${Target2Kill}]}) /call core_cast2 "Fleeting Spirit" alt ${Target2Kill} FALSE

	| Increase spell cost by 14% for 100% exceptional heals on instant heals for 18 seconds
	/if (${validate_cast[FALSE, alt, "Spiritual Blessing", ${Target2Kill}]}) /call core_cast2 "Spiritual Blessing" alt ${Target2Kill} FALSE

	/call cast_AA_spire

	/call use_glyph
	/call click_burn_circle
	/call click_item_burn
/return



|***
 *	DES: Control for Talisman Buff
 *	USE: /call cast_talisman
 *  NOTE: keeps talisman buff line up
 ***|
sub cast_talisman(bool _debug)
	DEBUG cast_talisman()
	/if (${Me.Buff[${stTalisman}].ID}) /return
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/call timer_update timer_cast_Talisman ${tTalisman}
	/if (${validate_cast[FALSE, FIND, "${stTalisman}", ${Me.ID}]}) /call core_cast2 "${stTalisman}" FIND ${Me.ID} FALSE
/return



|***
 *	DES: We want Mana
 *	USE: 
 *  NOTE: 
 ***|
sub check_cani(bool _debug)
	DEBUG check_cani(${_debug})
	
	/if (!${stPctCanniManna}) /return
	
	CHECKEXIT
	GETINPUT
	ISMEDEAD
		
	| use the aa cani
	/if (${Me.PctMana} <= ${stPctCanniManna}*.${tmpHealOverride}*.5 && ${Me.PctHPs} >= ${stPctCanniHP}) {
		/if (${validate_cast[FALSE, alt, "Cannibalization", ${Me.ID}]}) /call core_cast2 "Cannibalization" alt 0 FALSE
	}
	
	/declare _spell string local ${Spell[${stCannibalization}].RankName}
	
	| canni spell first...
	/if (${Me.PctMana} <= ${stPctCanniManna} && ${Me.PctHPs} >= ${stPctCanniHP}) {
		/declare _cancast bool local FALSE
	
		/if (${_spell.Equal[Cannibalize]} && ${Me.CurrentHPs} > 50) /varset _cancast TRUE
		/if (${_spell.Equal[Cannibalize II]} && ${Me.CurrentHPs} > 67) /varset _cancast TRUE
		/if (${_spell.Equal[Cannibalize III]} && ${Me.CurrentHPs} > 74) /varset _cancast TRUE
		/if (${_spell.Equal[Cannibalize IV]} && ${Me.CurrentHPs} > 148) /varset _cancast TRUE
		/if (${_spell.Equal[Ancient Chaotic Pain]} && ${Me.CurrentHPs} > 668) /varset _cancast TRUE
		/if (${_spell.Equal[Pained Memory]} && ${Me.CurrentHPs} > 688) /varset _cancast TRUE
		/if (${_spell.Equal[Ancient: Ancestral Calling]} && ${Me.CurrentHPs} > 868) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Bargain]} && ${Me.CurrentHPs} > 1042) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Bargain Rk. II]} && ${Me.CurrentHPs} > 1085) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Bargain Rk. III]} && ${Me.CurrentHPs} > 1128) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Hearkening]} && ${Me.CurrentHPs} > 1161) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Hearkening Rk. II]} && ${Me.CurrentHPs} > 1324) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Hearkening Rk. III]} && ${Me.CurrentHPs} > 1141) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Obligation]} && ${Me.CurrentHPs} > 1417) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Obligation Rk. II]} && ${Me.CurrentHPs} > 1615) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Obligation Rk. III]} && ${Me.CurrentHPs} > 1721) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Covenant]} && ${Me.CurrentHPs} > 1559) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Covenant Rk. II]} && ${Me.CurrentHPs} > 1777) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Covenant Rk. III]} && ${Me.CurrentHPs} > 1893) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Arrangement]} && ${Me.CurrentHPs} > 2366) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Arrangement Rk. II]} && ${Me.CurrentHPs} > 2484) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Arrangement Rk. III]} && ${Me.CurrentHPs} > 2608) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Pact]} && ${Me.CurrentHPs} > 4564) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Pact Rk. II]} && ${Me.CurrentHPs} > 4792) /varset _cancast TRUE
		/if (${_spell.Equal[Ancestral Pact Rk. III]} && ${Me.CurrentHPs} > 5032) /varset _cancast TRUE
		/if (${_spell.Equal[Tribal Pact]} && ${Me.CurrentHPs} > 6693) /varset _cancast TRUE
		/if (${_spell.Equal[Tribal Pact Rk. II]} && ${Me.CurrentHPs} > 7028) /varset _cancast TRUE
		/if (${_spell.Equal[Tribal Pact Rk. III]} && ${Me.CurrentHPs} > 7379) /varset _cancast TRUE
		/if (${_spell.Equal[Tribal Bargain]} && ${Me.CurrentHPs} > 8117) /varset _cancast TRUE
		/if (${_spell.Equal[Tribal Bargain Rk. II]} && ${Me.CurrentHPs} > 8523) /varset _cancast TRUE
		/if (${_spell.Equal[Tribal Bargain Rk. III]} && ${Me.CurrentHPs} > 8949) /varset _cancast TRUE

		/if (${_cancast}) {
			/if (${Me.SpellReady[${_spell}]}) {
				/if (${validate_cast[FALSE, spell, "${stCannibalization}", ${Me.ID}]}) /call core_cast2 "${stCannibalization}" 0 0 FALSE
				/return
			}
		}

	}
	
/return






|***
 *	DES: Shaman Alliance
 *	USE: 
 *  NOTE: 
 ***|
 sub alliance_SHM(bool _debug)
 	DEBUG alliance_SHM()
	CHECKEXIT
	GETINPUT

	/if (!${Me.SpellReady[${Spell[${stAlliance}].RankName}]}) /return FALSE
	
	/if (${swAssistG}) {
		/target pc ${Group.MainAssist}
		/delay 5 ${Target.BuffsPopulated}
		/if (${Target.BuffsPopulated} && !${Target.Buff[${stAlliance}].ID}) {
			/if (${validate_cast[FALSE, spell, "${stAlliance}", ${Group.MainAssist}]}) /call core_cast2 "${stAlliance}" 0 ${Group.MainAssist} FALSE
		}
	} else /if (${swAssist1}) {
		/target pc ${Raid.MainAssist}
		/delay 5 ${Target.BuffsPopulated}
		/if (${Target.BuffsPopulated} && !${Target.Buff[${stAlliance}].ID}) {
			/if (${validate_cast[FALSE, spell, "${stAlliance}", ${Raid.MainAssist}]}) /call core_cast2 "${stAlliance}" 0 ${Raid.MainAssist} FALSE
		}
	}
	/if (${Bool[${setMA1}]} && ${Spawn[pc ${setMA1}].ID}) {
		/target pc {setMA1}
		/delay 5 ${Target.BuffsPopulated}
		/if (${Target.BuffsPopulated} && !${Target.Buff[${stAlliance}].ID}) {
			/if (${validate_cast[FALSE, spell, "${stAlliance}", ${setMA1}]}) /call core_cast2 "${stAlliance}" 0 ${setMA1} FALSE
		}
	} else /if (${Bool[${setMA2}]} && ${Spawn[pc ${setMA2}].ID}) {
		/target pc ${setMA2}
		/delay 5 ${Target.BuffsPopulated}
		/if (${Target.BuffsPopulated} && !${Target.Buff[${stAlliance}].ID}) {
			/if (${validate_cast[FALSE, spell, "${stAlliance}", ${setMA2}]}) /call core_cast2 "${stAlliance}" 0 ${setMA2} FALSE
		}
	} 
/return



|***
 *	DES: MGB shaman AA Ancestral Aid if both AAs are ready
 *	USE: 
 *  NOTE:
 ***|
sub cast_AA_aid(bool _debug)
	DEBUG cast_AA_aid()
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[${SHMAid}]} && !${Bool[${Me.Song[${SHMAid}]}]}) {
		/if (${validate_cast[FALSE, alt, "Mass Group Buff", ${Me.ID}]}) /call core_cast2 "Mass Group Buff" alt 0 FALSE
		/if (${validate_cast[FALSE, alt, "${SHMAid}", ${Me.ID}]}) /call core_cast2 "${SHMAid}" alt 0 FALSE
	} else /if (${Bool[${Me.Song[${SHMAid}]}]}) {
		VOUT ${cbad}QUIT\ax trying to cast ${csp}Ancestral Aid\ax while its running.	
	}		
/return TRUE



|***
 *	DES: Class controls
 *	USE: /shm []
 *  NOTE:
 ***|
#bind setSHM /shm
sub Bind_setSHM(string _type, string _verbage, bool _debug)
	/if (!${validate_class[TRUE, |SHM]}) /return

	/declare _sep bool local
	/declare _listout string local
	/declare _count int local 0
	/declare _bind string local ${Me.Class.ShortName.Lower}
  /declare _lstClass string	local |ae|aid|aggressive|cannihps|cannimanna|defcount|epic|fade|groupheal|grouphot|grow|healpoint|rampage|rod|spiregroup|spireraid|surge|talisman|xtclass|xttime

	| enable/disable talisman line of spells
	/if (${_type.Find[talisman]}) {
		/call set_switch ${chrID} swTalisman ${_verbage}

	| set rampage tank toon
	} else /if (${_type.Equal[rampage]}) {
		/if (${Bool[${_verbage}]}) {
			/call sql_update FALSE ${chrID} RampageTank ${_verbage.Left[1].Upper}${_verbage.Right[-1].Lower}
		} else /if (!${Bool[${_verbage}]}) {
			/call sql_update FALSE ${chrID} RampageTank FALSE
		}

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/call set_character_fade FALSE ${_verbage}

	| set the XTarget class to heal
	} else /if (${_type.Equal[xtclass]}) {
		/call set_character_xthealclass FALSE ${_verbage}
		/return	
		
	| use Modulation Rods
	} else /if (${_type.Equal[rod]}) {
		/call set_switch ${chrID} swModRod ${_verbage} 	
		
	| use grow temp buff
	} else /if (${_type.Equal[grow]}) {
		/call set_switch ${chrID} switchGrowth ${_verbage}	
	
	| force epic click
	} else /if (${_type.Equal[epic]}) {
		/if (${Me.Casting.ID}) /stopcast
		/if (${validate_cast[FALSE, epicitem, "2", ${Me.ID}]}) /call core_cast2 "${${Me.Class.ShortName}Epic2}" item 0 FALSE	
		/return

	| use group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/call set_character_groupheal FALSE ${_verbage}

	| use surge Heals
	} else /if (${_type.Equal[surge]}) {
		/call set_switch ${chrID} swSurge ${_verbage}

	
	| use group Heal over Time	
	} else /if (${_type.Equal[grouphot]}) {
		/call set_switch ${chrID} useGroupHoT ${_verbage}
	 	
	| set Class Heal Point	
	} else /if (${_type.Equal[healpoint]}) {
		/call set_character_healpoint FALSE ${_verbage}
		/return

	| canni HP pct
	} else /if (${_type.Equal[cannihps]}) {
		/if (${Range.Between[1,99:${Int[${_verbage}]}]}) {
			/call sql_update FALSE ${chrID} stPctCanniHP ${_verbage}
		} else {
			VOUT /${_bind} cannihp [${cinfo}${stPctCanniHP}\ax] allowable range ${sep} ${cinfo}1 - 99\ax
			/return
		}	

	| mgb aid
	} else /if (${Bool[${_type.Find[aid]}]}) {
		/stopcast
		/delay 2s !${Me.Casting.ID}
		/call cast_AA_aid
		/return
		
	| enable/disable lead with aggerssive heals
	} else /if (${_type.Equal[aggressive]}) {
		/call set_switch ${chrID} switchHealAggressive ${_verbage}

	| canni Manna pct
	} else /if (${_type.Equal[cannimanna]}) {
		/if (${Range.Between[0,99:${Int[${_verbage}]}]}) {
			/call sql_update FALSE ${chrID} stPctCanniManna ${_verbage}
		} else {
			VOUT /${_bind} cannimanna [${cinfo}${stPctCanniHP}\ax] allowable range ${sep} ${cinfo}0 - 99\ax (\a-w0\ax=off)
			/return
		}	
		
	| set XT refresh list Time
	} else /if (${_type.Equal[xttime]}) {
		/if (${Range.Between:${Int[${_verbage}]}]}) {
			/call sql_update FALSE timer tXHeal ${_verbage}
		} else {
			VOUT /${_bind} xttime [${cinfo}${tXHeal}m\ax] allowable range ${sep} ${cinfo}1 - 5 min\ax
			/return
		}	

	| spire control
	} else /if (${_type.Equal[spiregroup]} || ${_type.Equal[spireraid]}) {
		/call set_character_spire FALSE ${_type} ${_verbage}

	| defensive mob agro count
	} else /if (${_type.Equal[defcount]}) {
		/call set_character_defcount FALSE ${_verbage}
		
	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/call set_character_ae FALSE ${_verbage}

	}
	
	/if (${swMQClear}) /mqclear
	/for _count 1 to ${_lstClass.Count[|]}
		/varset _listout ${_listout} ${If[${_sep},${sep},]} ${cinfo}${_lstClass.Arg[${_count},|]}\ax
		/if (${_lstClass.Arg[${_count},|].Equal[ae]}) /varset _listout ${_listout}:${If[${AEMelee},${cnum}${AEMelee}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[defcount]}) /varset _listout ${_listout}:${If[${defCount},${cnum}${defCount}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[spiregroup]}) /varset _listout ${_listout}:${If[${spireGroup},${cnum}${spireGroup}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[spireraid]}) /varset _listout ${_listout}:${If[${spireRaid},${cnum}${spireRaid}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[xttime]}) /varset _listout ${_listout}:${cnum}${tXHeal}\ax
		/if (${_lstClass.Arg[${_count},|].Equal[cannihps]}) /varset _listout ${_listout}:${cnum}${stPctCanniHP}\ax
		/if (${_lstClass.Arg[${_count},|].Equal[cannimanna]}) /varset _listout ${_listout}:${If[${stPctCanniManna},${cnum}${stPctCanniManna}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[groupheal]}) /varset _listout ${_listout}:${If[${useGroupHeal},${cnum}${useGroupHeal}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[grouphot]}) /varset _listout ${_listout}:${If[${useGroupHoT},${oon},${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[talisman]}) /varset _listout ${_listout}:${If[${swTalisman},${oon},${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[surge]}) /varset _listout ${_listout}:${If[${swSurge},${oon},${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[grow]}) /varset _listout ${_listout}:${If[${switchGrowth},${oon},${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[aggressive]}) /varset _listout ${_listout}:${If[${switchHealAggressive},${oon},${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[rampage]}) /varset _listout ${_listout}:${If[${Bool[${RampageTank}]},${cnum}${RampageTank}\ax,${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[healpoint]}) /varset _listout ${_listout}:${cnum}->\ax
		/if (${_lstClass.Arg[${_count},|].Equal[rod]}) /varset _listout ${_listout}:${If[${Bool[${swModRod}]},${oon},${ooff}]}
		/if (${_lstClass.Arg[${_count},|].Equal[fade]}) /varset _listout ${_listout}:${If[${switchAAFade},${cnum}${useAAFadePct}\ax,${cbad}${useAAFadePct}\ax]}

		/varset _sep TRUE
	/next _count
	VOUT /${_bind} [${_listout}]

/return

