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

	/call set_data ${_debug} maData stSpire															map outer "Spire of Ancestors"
	/call set_data ${_debug} maData stSynergy														map outer "Luminary's Synergy"
	/call set_data ${_debug} maData stEpic1															map outer "Spear of Fate"
	/call set_data ${_debug} maData stEpic15														map outer "Crafted Talisman of Fates"
	/call set_data ${_debug} maData stEpic2															map outer "Blessed Spiritstaff of the Heyokah"
	/call set_data ${_debug} maData stFade															map outer FALSE
	/call set_data_map ${_debug} maChr stAAUnity												"Visionary's Unity"						"${auxna} class aa unity buff"
		
	/varset dump ${set_data_timer[${_debug}, Check_Talisman, 30s]}
	/call set_data_map ${_debug} maChr stTalisman	 											PREGEN|1											"${auxna} talisman spell"
	/call set_data_map ${_debug} maChr swTalisman			 									FALSE													"${auxsw} use talisman spell"
	                                        		
	/call set_data_list ${_debug} maChr lsNukeTC									 			PREGEN|2											"${auxli} heal twincast nukes"
	/call set_data_map ${_debug} maChr stCannibalization								PREGEN|1											"${auxna} canni spell"
	                                         		
	/call set_data_map ${_debug} maChr stPctCanniHP				 							60														"${auxnum} HP level to stop using canni"
	/call set_data_map ${_debug} maChr stPctCanniManna				 					60														"${auxnum} mana level to start using canni"

	/call set_data_map ${_debug} maChr swSurge													FALSE													"${auxsw} use surge healing"
 
 	/call set_data_map ${_debug} maChr stDichotomic											PREGEN|1											"${auxna} dichotomic spell"
 	/call set_data_map ${_debug} maChr stAlliance												PREGEN|1											"${auxna} alliance spell"
 
 	/call set_data_map ${_debug} maChr stHealSurge											PREGEN|1											"${auxna} area surg spell"
  /call set_data_list ${_debug} maChr lsHealMending										PREGEN|3											"${auxli} main list of heal spells"											
	/call set_data_list ${_debug} maChr lsHealIntervention							PREGEN|3											"${auxli} intervention nuke/healing spells"
	/call set_data_list ${_debug} maChr lsHealReckless									PREGEN|3											"${auxli} recless healing spells"

  /call set_data_map ${_debug} maChr swHealGroupOverTime							FALSE													"${auxsw} use group heal over time spells/aas"
  /call set_data_map ${_debug} maChr stHealGroupOverTime							PREGEN|1											"${auxna} group heal over time spell"

	/call set_data_map ${_debug} maChr swHealOverTime										FALSE													"${auxsw} % of members health to use heal over time"
	/call set_data_map ${_debug} maChr stHealOverTime										PREGEN|1											"${auxna} heal over time spelll"

	/call set_data_map ${_debug} maChr stCountHealGroup									0															"${auxnum} number in group hurt to use a group heal"
 	/call set_data_list ${_debug} maChr lsHealGroup											PREGEN|2											"${auxli} group heal spells"

	| temp buff: growth family
  /call set_data_list ${_debug} maChr lsGrowthToon	 									EMPTY													"${auxli} list of toons to use ward temp hp growth on"
	/call set_data_map ${_debug} maChr stGrowth			 										PREGEN|1											"${auxna} temp hp growth spell"

	| set heal orders
	/declare lsHealOrderbase list outer
	/varset dump ${lsHealOrderbase.Delimiter["|"]}
	/varset dump ${lsHealOrderbase.Append[lsHealBaseLine]}

	/declare lsHealOrdermending list outer
	/varset dump ${lsHealOrdermending.Delimiter["|"]}
	/varset dump ${lsHealOrdermending.Append[lsHealMending|lsHealReckless]}

	/declare lsHealOrderreckless list outer
	/varset dump ${lsHealOrderreckless.Delimiter["|"]}
	/varset dump ${lsHealOrderreckless.Append[lsHealReckless|lsHealMending]}

/return



|***
 * note: main shaman routine
 * use: 
 ***|
sub main_SHM(bool _debug)
	DEBUG main_SHM()
	GETINPUT
	ISMEDEAD
	CHECKTIE
	
	| correct any and all XTarget healing spots
	/if (AUTO) {
	  /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
	    /call check_xtarget_build FALSE
		}
		/call check_cani ${maDebug.Find[canni].Value}
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
		/if (PET || SWARM) /call send_pet FALSE TARGET

	}	

	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Group
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/if (${maHeal.Find[swHealXTarget].Value}) /call check_heal_${Me.Class.ShortName} FALSE XTarget
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Pet
		/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
		CHECKFADE

		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list FALSE stGrowth lsGrowthToon
		}

		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
		/if (BURN) /call burn_${Me.Class.ShortName} FALSE
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			
			CHECKREZ
			/call set_combat FALSE
			/if (PET || SWARM) /call send_pet ${maDebug.Find[minion].Value} TARGET
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (!${timer_Check_deBuffs}) /call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			/if (DOT) /call cast_dot ${maDebug.Find[castdot].Value} lsDoT TARGET
			/if (NUKE) /call cast_nuke ${maDebug.Find[castnuke].Value} lsNuke TARGET
			/if (${maChr.Find[swTalisman].Value}) /call cast_talisman ${maDebug.Find[talisman].Value}
			/if (${maCC.Find[swAlliance].Value}) /call cast_alliance_SHM ${maDebug.Find[alliance].Value}
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[minion].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			/call check_item_cycle ${maDebug.Find[castitem].Value} TARGET
		}

		/if (AUTO) {
			CHECKFADE
			/call check_cani ${maDebug.Find[canni].Value}
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
		}
	}

/return


|***
 * noote: Combat skills, Disciplines and Alternate abilities.
 * use: yes, there are a lot here.
 ***|
sub misc_combat_SHM(bool _debug)
	DEBUG \atmisc_combat_SHM\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
	
	/call check_cani ${maDebug.Find[canni].Value}
	
/return



|***
 * note: 
 * use: 
 ***|
sub check_heal_SHM(bool _debug, string _type)
	DEBUG \atcheck_heal_SHM\ax(\a-w${_type}\ax)
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"
	
	/call get_hurt ${_debug} ${_type}
	/declare _count int local 0
	
	| Group Regen Heal 
 	/if (${maChr.Find[swHealGroupOverTime].Value} && !SAFEZONE && ${Group.Members} && COMBAT && ${maChr.Find[stCountAE].Value}) {
	 	/if (${SpawnCount[npc radius ENV PS4 targetable]} >= ${maChr.Find[stCountAE].Value}) {
	 		/declare _groupHOT string local ${Set_Rank[False, ${maChr.Find[stHealGroupOverTime].Value.Arg[1,+]}]}
			/if (!${Me.Song[${_groupHOT}].ID} && !${Me.Song[Shear of Renewal].ID} && !${Me.Song[Ghost of Renewal].ID} && !${Me.Song[Cloud of Renewal].ID}) {

				/declare _found bool TRUE
				| check regen SPA
				/if (${Spell[${_groupHOT}].HasSPA[374]}) {
					/for _count 1 to ${Spell[${_groupHOT}].NumEffects}
						/if (!${Spell[${Spell[${_groupHOT}].Base2[${_count}]}].Duration}) /continue
						/if (!${Me.Song[${Spell[${_groupHOT}].Base2[${_count}]}].ID}) /varset _found FALSE
						/if (!${Spell[${Spell[${_groupHOT}].Base2[${_count}]}].Stacks}) /varset _found TRUE
						/if (${_found}) /break
					/next _count
				} else {
					/if (!${Me.Song[${_groupHOT}].ID}) /varset _found FALSE
					/if (${Spell[${_groupHOT}].Stacks}) /varset _found FALSE
				}
				/if (!${_found} && ${is_ready[FALSE, ${_groupHOT}]}) {
					/call voodoo ${_debug} "${maChr.Find[stHealGroupOverTime].Value}" 0 FALSE
				}
			}
		}
	}
	
	| no one is hurt. we're out of here
	/if (!${hurtCount}) {
	  | check for surge
	  /if (${maChr.Find[swSurge].Value}) {
			/if (${SpawnCount[npc radius ENV PS4 targetable]}) /call cast_surge_heal ${maDebug.Find[surge].Value}
		}	
		
		SETHUD FALSE
		/return FALSE
		
	}

	CHECKREZ
	
  | stationary ward heal
  /if (${hurtCount} >= ${maHeal.Find[swHealGroup].Value}) {
    /if (${is_ready[FALSE, "Call of the Ancients"]}) {
    	/call voodoo ${_debug} "Call of the Ancients" ${Me.ID} FALSE
    }
  }
  
	|** Panic Ancestral Guard
	/if (${hurtPctHP} < ${PanicPoint}*.PCTHO) {
		/if (${is_ready[FALSE, "Ancestral Guard"]}) {
			/if (${Me.Casting.ID}) /stopcast
			/call voodoo ${_debug} "Ancestral Guard" 0 FALSE
		}
	}
	
	**|
  
 	/if (!${get_hurt[FALSE, ${_type}]}) {
 		/return FALSE
 	}

	ISMEDEAD
	CHECKEXIT
	CHECKFADE	
	CHECKREZ
		
	/declare _liHM listiterator local
	/vardata _liHM lsHealOrder${maHeal.Find[stHealMode].Value}.First.Clone
	/while (!${_liHM.IsEnd}) {
		DEBUG ${sep}_liHM.Value${sep}\a-w${_liHM.Value}\ax
		/call cast_list FALSE "${_liHM.Value}" ${hurtID}
		/varset dump ${_liHM.Advance}
	}  

	SETHUD FALSE
	
/return



|***
 * note: surge heal
 * use: part of heal routine
 ***|
sub cast_surge_heal(bool _debug)
	DEBUG \atcast_surge_heal\ax()

	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return

	CHECKEXIT
	ISMEDEAD
	GETINPUT

	| get the main assisst ID
	/declare _tmpID int local ${get_assist_info[FALSE, ID]}
	DEBUG assist_name: ${_tmpID}

	/if (!${Spawn[${_tmpID}].LineOfSight}) /return FALSE
	
  /if (!${Me.Song[Healing Twincast].ID}) {
	 	/declare _li listiterator local
		/vardata _li lsNukeTC.First.Clone
		/while (!${_li.IsEnd}) {

			/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return
			/if (${Me.PctMana} < 2) /continue 

			/if (${is_ready[${_debug}, "${_li.Value}"]}) {
				/call voodoo ${_debug} "${_li.Value}" ${_tmpID} FALSE
			}
			/delay 15 !${Me.Casting.ID}
			/if (${Me.Song[Healing Twincast].ID}) /break

			/varset dump ${_li.Advance}
		}	 
 	 
	}
	
	/if (!${Me.Song[Healing Twincast].ID}) /return FALSE

	| spalsh surge heal	
	/delay 2.25s ${Me.SpellReady[${Spell[${maChr.Find[stHealSurge].Value}].RankName}]}
	/if (${is_ready[${_debug}, "${maChr.Find[stHealSurge].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stHealSurge].Value}" ${_tmpID} FALSE
	}
	
/return TRUE



|***
 * note: Shaman burn routines
 * use: normal burn stuf
 ***| 
sub burn_SHM(bool _debug)
	DEBUG \atburn_SHM\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
	
	| swarm aa pet
	/if (SWARM) {
		/if (${is_ready[FALSE, "Spirit Call"]}) /call voodoo ${_debug} "Spirit Call" TARGET FALSE
	}

	| Lower spell resists
	/if (${is_ready[FALSE, "Dampen Resistance"]}) /call voodoo ${_debug} "Dampen Resistance" TARGET FALSE

	| Twincast next 6 DOTs/HOTs
	/if (${is_ready[FALSE, "Fleeting Spirit"]}) /call voodoo ${_debug} "Fleeting Spirit" TARGET FALSE

	| Increase spell cost by 14% for 100% exceptional heals on instant heals for 18 seconds
	/if (${is_ready[FALSE, "Spiritual Blessing"]}) /call voodoo ${_debug} "Spiritual Blessing" TARGET FALSE

	/call cast_AA_spire ${_debug}

	/call cast_glyph ${_debug}

/return



|***
 *	DES: Control for Talisman Buff
 *	USE: /call cast_talisman DEBUG
 ***|
sub cast_talisman(bool _debug)
	DEBUG \atcast_talisman\ax()
	
	/varset dump ${set_data_timer[${_debug}, Check_Talisman, RESTART]}
	
	/if (${Me.Buff[${maChr.Find[stTalisman].Value}].ID}) /return FALSE
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	
	/if (${is_ready[${_debug}, "${maChr.Find[stTalisman].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stTalisman].Value}" ${Me.ID} FALSE
	}
	
/return TRUE



|***
 *	DES: We want Mana
 *	USE: 
 *  NOTE: 
 ***|
sub check_cani(bool _debug)
	DEBUG \atcheck_cani\ax()
	
	/if (!${maChr.Find[stPctCanniManna].Value}) /return
	
	CHECKEXIT
	GETINPUT
	ISMEDEAD
		
	| use the aa cani
	/if (${Me.PctMana} <= ${maChr.Find[stPctCanniManna].Value}*.PCTHO && ${Me.PctHPs} >= ${maChr.Find[stPctCanniHP].Value}) {
		/if (${is_ready[${_debug}, "Cannibalization"]}) {
			/call voodoo ${_debug} "Cannibalization" 0 FALSE
		}
	}
	
	| if canni spell is not ready bail..
	/if (!${is_ready[${_debug}, "${maChr.Find[stCannibalization].Value}"]}) /return FALSE
	/if (${Me.PctMana} >= ${maChr.Find[stPctCanniManna].Value}) /return FALSE
	/if (${Me.PctHPs} <= ${maChr.Find[stPctCanniHP].Value}) /return FALSE

	| check HP requirements
	/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM mana_recovery WHERE env_var="${maChr.Find[stCannibalization].Value}"
	/if (${Me.PctHPs} < ${sqlite.Result[dummy 1 value]}) /return FALSE

	/if (${is_ready[FALSE, "${maChr.Find[stCannibalization].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCannibalization].Value}" 0 FALSE
	}
	
/return



|***
 * note: check/cast shaman alliance
 * use: yahh... not really sure this even works anymore
 ***|
sub cast_alliance_SHM(bool _debug)
 	DEBUG \atcast_alliance_SHM\ax()

	CHECKEXIT
	GETINPUT

	/if (!${is_ready[${_debug}, "${maChr.Find[stAlliance].Value}"]}) /return FALSE
	
	/declare _name string local ${get_assist_info[${_debug}, name}
	/declare _tmpID int local ${get_assist_info[${_debug}, id}
	
	/varset dump ${set_target[${_name}]}
	
	/delay 5 ${Target.BuffsPopulated}
	/if (${Target.BuffsPopulated} && !${Target.Buff[${maChr.Find[stAlliance].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stAlliance].Value}" ${_tmpID} FALSE
	}
	
/return



|***
 * note: shaman specific control
 * use: /chr
 ***|
sub set_control_SHM(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_SHM\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
		/return
		
	| use mana click items
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return
		
	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| 	
	} else /if (${_type.Equal[cannihps]}) {
		/if (!${set_control_num_range[${_debug}, stPctCanniHP, "${_verbage}", 0, 99]}) /return

	| 
	} else /if (${_type.Equal[cannimanna]}) {
		/if (!${set_control_num_range[${_debug}, stPctCanniManna, "${_verbage}", 0, 99]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return

	| use group Heal over Time	
	} else /if (${_type.Equal[grouphot]}) {
		/varset dump ${set_switch_env[${_debug}, swHealGroupOverTime, TRUE, ${_verbage}]}

	| 
	} else /if (${_type.Equal[surge]}) {
		/varset dump ${set_switch_env[${_debug}, swSurge, TRUE, ${_verbage}]}

	| 
	} else /if (${_type.Equal[talisman]}) {
		/varset dump ${set_switch_env[${_debug}, swTalisman, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[hot]}) {
		/varset dump ${set_switch_env[${_debug}, swHealOverTime, TRUE, ${_verbage}]}

	}

	
	/if (${_silent.Equal[SILENT]}) /return 
	/varset dump ${set_control_output[${_debug}, maChr, chr, maChrControl]}
	
/return


