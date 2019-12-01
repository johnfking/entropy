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
		
	/varset dump ${set_data_timer[FALSE, Check_Talisman, 30s]}
	/call sql_control FALSE ${chrID} stTalisman	 												PREGEN|1											"${auxna} talisman spell"
	/call sql_control FALSE ${chrID} swTalisman			 										FALSE													"${auxsw} use talisman spell"
	                                        		
	/call set_data_list FALSE maChr lsNukeTC									 					PREGEN|2											"${auxli} heal twincast nukes"
	/call set_data_map FALSE maChr stCannibalization										PREGEN|1											
	                                         		
	/call set_data_map FALSE maChr stPctCanniHP				 									60														"${auxnum} HP level to stop using canni"
	/call set_data_map FALSE maChr stPctCanniManna				 							60														"${auxnum} mana level to start using canni"

	/call set_data_map FALSE maChr swSurge															FALSE													"${auxsw} use surge healing"
 
 	/call set_data_list FALSE maChr lsHealSurge													PREGEN|1											"${auxli} area surg spells"
  /call set_data_list FALSE maChr lsHealMain													PREGEN|3											"${auxli} main list of heal spells"											
	/call set_data_list FALSE maChr lsHealIntervention									PREGEN|3											"${auxli} intervention nuke/healing spells"
	/call set_data_list FALSE maChr lsHealReckless											PREGEN|3											"${auxli} recless healing spells"

  /call set_data_map FALSE maChr swHealGroupOverTime									FALSE													"${auxsw} use group heal over time spells/aas"
  /call set_data_map FALSE maChr stHealGroupOverTime									PREGEN|1											"${auxna} group heal over time spell"

	/call set_data_map FALSE maChr swHealOverTime												FALSE													"${auxsw} % of members health to use heal over time"
	/call set_data_map FALSE maChr stHealOverTime												PREGEN|1											"${auxna} heal over time spelll"

	/call set_data_map FALSE maChr stCountHealGroup											0															"${auxnum} number in group hurt to use a group heal"
 	/call set_data_list FALSE maChr lsHealGroup													PREGEN|2											"${auxli} group heal spells"

	| temp buff: growth family
  /call set_data_list FALSE maChr lsGrowthToon	 											EMPTY													"${auxli} list of toons to use ward temp hp growth on"
	/call set_data_map FALSE maChr stGrowth			 												PREGEN|1											"${auxna} temp hp growth spell"

	| set heal orders
	/declare lsHealOrderbase list outer
	/varset dump ${lsHealOrderbase.Delimiter["|"]}
	/varset dump ${lsHealOrderbase.Append[lsHealBaseLine]}

	/declare lsHealOrderaggressive list outer
	/varset dump ${lsHealOrderaggressive.Delimiter["|"]}
	/varset dump ${lsHealOrderaggressive.Append[lsHealIntervention|lsHealReckless|lsHealMain]}

	/declare lsHealOrderpassive list outer
	/varset dump ${lsHealOrderpassive.Delimiter["|"]}
	/varset dump ${lsHealOrderpassive.Append[lsHealMain|lsHealReckless|lsHealIntervention]}
	
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
		
		/if (!${Me.Invis}) {
			/call check_cani
			/if (!${check_engaged[FALSE]} && PET) /call check_pet
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
		/if (!${is_defensive_active[FALSE, FALSE, SAFEENV]}) /call cast_defensive_cycle FALSE
		CHECKFADE

		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list FALSE stGrowth lsGrowthToon
		}

		/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
		/if (BURN) /call burn_${Me.Class.ShortName} FALSE
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			
			CHECKREZ
			/call set_combat FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (!${timer_Check_deBuffs}) /call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			/if (DOT) /call cast_dot ${maDebug.Find[castdot].Value} lsDot TARGET
			/if (NUKE) /call cast_nuke ${maDebug.Find[castnuke].Value} lsNuke TARGET
			/if (${maChr.Find[swTalisman].Value}) /call cast_talisman ${maDebug.Find[talisman].Value}
			| /if (${swAlliance}) /call cast_alliance_SHM ${maDebug.Find[alliance].Value}
			/if (PET || SWARM) /call send_pet ${maDebug.Find[minon].Value} TARGET
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[minion].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			/call check_item_cycle ${maDebug.Find[castitem].Value} TARGET
		}

		/if (AUTO) {
			CHECKFADE
			/call check_cani	
			/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
		}
	}

/return


|***
 * noote: Combat skills, Disciplines and Alternate abilities.
 * use: yes, there are a lot here.
 ***|
sub misc_combat_SHM(bool _debug)
	DEBUG \atmisc_combat_SHM\ax()
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKREZ
	
	/if (!${is_defensive_active[FALSE, FALSE, SAFEENV]}) /call cast_defensive_cycle FALSE
	
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
	
	/call check_hurt FALSE ${_type}
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
  
  /if (!${check_hurt[FALSE, ${_type}]}) /return FALSE
  


















 	
	SETHUD FALSE
	
	
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
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return

	CHECKEXIT
	ISMEDEAD
	GETINPUT

	| get the main assisst ID
	/declare _tmpID int local ${get_assist_info[FALSE, ID]}
	DEBUG assist_name: ${_tmpID}

	/if (!${Spawn[${_tmpID}].LineOfSight}) /return FALSE
	
  /if (${Me.SpellReady[${Spell[${TC1}].RankName}]} && !${Me.Song[Healing Twincast].ID}) {
		/if (${is_ready[FALSE, spell, "${TC1}", ${_tmpID}]}) /call voodoo ${_debug} "${TC1}" 0 ${_tmpID} FALSE
	} else /if (${Me.SpellReady[${Spell[${TC2}].RankName}]} && !${Me.Song[Healing Twincast].ID}) {
		/if (${is_ready[FALSE, spell, "${TC2}", ${_tmpID}]}) /call voodoo ${_debug} "${TC2}" 0 ${_tmpID} FALSE
	}

	| spalsh surge heal	
	/if (${Me.SpellReady[${Spell[${lsHealSurge}].RankName}]} && ${Me.Song[Healing Twincast].ID}) {
		/delay 2.25s ${Me.SpellReady[${Spell[${lsHealSurge}].RankName}]}
		/if (${is_ready[FALSE, spell, "${lsHealSurge}", ${_tmpID}]}) /call voodoo ${_debug} "${lsHealSurge}" 0 ${_tmpID} FALSE
	}
/return



|***
 * note: Shaman burn routines
 * use: normal burn stuf
 ***| 
sub burn_SHM(bool _debug)
	DEBUG \atburn_SHM\ax()
	
	CHECKEXIT
	GETINPUT

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
	DEBUG check_cani(${_debug})
	
	/if (!${maChr.Find[stPctCanniManna].Value}) /return
	
	CHECKEXIT
	GETINPUT
	ISMEDEAD
		
	| use the aa cani
	/if (${Me.PctMana} <= ${maChr.Find[stPctCanniManna].Value}*.PCTHO*.5 && ${Me.PctHPs} >= ${maChr.Find[stPctCanniHP].Value}) {
		/if (${is_ready[FALSE, "Cannibalization"]}) {
			/call voodoo ${_debug} "Cannibalization" 0 FALSE
		}
	}
	
		
	| canni spell first...
	/if (${Me.PctMana} <= ${maChr.Find[stPctCanniManna].Value} && ${Me.PctHPs} >= ${maChr.Find[stPctCanniHP].Value}) {
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
			/if (${is_ready[FALSE, "${stCannibalization}"]}) {
				/call voodoo ${_debug} "${stCannibalization}" 0 FALSE
			}
			/return
		}

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
	} else /if (${_type.Equal[manaclick]}) {
		/if (!${set_control_num_range[${_debug}, stPctManaClick, "${_verbage}", 0, 99]}) /return
		
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
	/varset _out
	/vardata li lsChr.First.Clone
	/while (!${li.IsEnd}) {
		/varset _out ${_out} ${If[${li.Value.Equal[${lsChr.First}]},,${dot}]} ${info}${li.Value}\ax
		/if (${li.Value.Equal[ae]}) /varset _out ${_out}:${If[${maChr.Find[stCountAE].Value},${num}${maChr.Find[stCountAE].Value}\ax,${r0}]}
		/if (${li.Value.Equal[def]}) /varset _out ${_out}:${If[${maChr.Find[stCountDefensive].Value},${num}${maChr.Find[stCountDefensive].Value}\ax,${r0}]}
		/if (${li.Value.Equal[fade]}) /varset _out ${_out}:${If[${maChr.Find[stPctFade].Value},${num}${maChr.Find[stPctFade].Value}\ax,${r0}]}
		/if (${li.Value.Equal[spire]}) /varset _out ${_out}:${submenu}
		/if (${li.Value.Equal[vp]}) /varset _out ${_out}:${If[${maChr.Find[stPctVP].Value},${num}${maChr.Find[stPctVP].Value}\ax,${r0}]}
		/if (${li.Value.Equal[groupheal]}) /varset _out ${_out}:${If[${maChr.Find[stCountHealGroup].Value},${num}${maChr.Find[stCountHealGroup].Value}\ax,${r0}]}
		/if (${li.Value.Equal[grouphot]}) /varset _out ${_out}:${If[${maChr.Find[swHealGroupOverTime].Value},${on},${off}]}
		/if (${li.Value.Equal[hot]}) /varset _out ${_out}:${If[${maChr.Find[swHealOverTime].Value},${on},${off}]}
		/if (${li.Value.Equal[manaclick]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaClick].Value},${num}${maChr.Find[stPctManaClick].Value}\ax,${r0}]}
	
		/if (${li.Value.Equal[cannihps]}) /varset _out ${_out}:${If[${maChr.Find[].Value},${num}${maChr.Find[].Value}\ax,${r0}]}
		/if (${li.Value.Equal[cannimanna]}) /varset _out ${_out}:${If[${maChr.Find[].Value},${num}${maChr.Find[].Value}\ax,${r0}]}
		/if (${li.Value.Equal[surge]}) /varset _out ${_out}:${If[${maChr.Find[].Value},${on},${off}]}
		/if (${li.Value.Equal[talisman]}) /varset _out ${_out}:${If[${maChr.Find[].Value},${on},${off}]}
	
	
	
		/varset dump ${li.Advance}
	}	
	/varset dump ${out[14, "${_out}"]}

/return

