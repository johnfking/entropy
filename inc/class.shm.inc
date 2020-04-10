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

	/invoke ${set_data_timer[${_debug}, Check_Talisman, 30s]}
	/call set_data_map ${_debug} maChr stTalisman	 											PREGEN|1											"${auxna} talisman spell"
	/call set_data_map ${_debug} maChr swTalisman			 									FALSE													"${auxsw} use talisman spell"

	/call set_data_list ${_debug} maChr lsNukeTC									 			PREGEN|2											"${auxli} heal twincast nukes"
	/call set_data_map ${_debug} maChr stCannibalization								PREGEN|1											"${auxna} canni spell"

	/call set_data_map ${_debug} maChr stPctCanniHP				 							60														"${auxnum} HP level to stop using canni"
	/call set_data_map ${_debug} maChr stPctCanniManna				 					60														"${auxnum} mana level to start using canni"

	/call set_data_map ${_debug} maChr swSurge													FALSE													"${auxsw} use surge healing"

 	/call set_data_map ${_debug} maChr stDichotomic											PREGEN|1											"${auxna} dichotomic spell"
 	/call set_data_map ${_debug} maChr stAlliance												PREGEN|1											"${auxna} alliance spell"

 	/call set_data_map ${_debug} maChr stHealSurge											PREGEN|1											"${auxna} area surge spell"

  /call set_data_map ${_debug} maHeal swHealGroupOverTime							FALSE													"${auxsw} use group heal over time spells/aas" heal
  /call set_data_map ${_debug} maHeal stHealGroupOverTime							PREGEN|1											"${auxna} group heal over time spell" heal

	/call set_data_map ${_debug} maHeal swHealOverTime									FALSE													"${auxsw} % of members health to use heal over time" heal
	/call set_data_map ${_debug} maHeal stHealOverTime									PREGEN|1											"${auxna} heal over time spelll" heal

	/call set_data_map ${_debug} maChr stCountHealGroup									0															"${auxnum} number in group hurt to use a group heal"
 	/call set_data_list ${_debug} maChr lsHealGroup											PREGEN|2											"${auxli} group heal spells"

	| temp buff: growth family
  /call set_data_list ${_debug} maChr lsGrowthToon	 									EMPTY													"${auxli} list of toons to use ward temp hp growth on"
	/call set_data_map ${_debug} maChr stGrowth			 										PREGEN|1											"${auxna} temp hp growth spell"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"


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
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
		/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET

	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/call check_heal_routines
		CHECKDEFENSE
		CHECKFADE

		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stGrowth lsGrowthToon
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}

		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return

		/if (VALID) {

			CHECKREZ
			/call set_combat FALSE
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/if (${maChr.Find[swTalisman].Value}) /call cast_talisman ${maDebug.Find[talisman].Value}
			/if (${maCC.Find[swAlliance].Value}) /call cast_alliance_SHM ${maDebug.Find[alliance].Value}
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[minion].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		}

		/if (AUTO) {
			CHECKFADE
			/call check_cani ${maDebug.Find[canni].Value}
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		}
	}

/return


|***
 * noote: Combat skills, Disciplines and Alternate abilities.
 * use: yes, there are a lot here.
 ***|
sub misc_combat_SHM(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_SHM\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat_SHM

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	/call check_cani ${maDebug.Find[canni].Value}


	/if (ENGINE2) {
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}


/return



|***
 * note:
 * use:
 ***|
sub check_heal_SHM(bool _debug, string _type)
	DEBUG \atcheck_heal_SHM\ax(\a-w${_type}\ax)
	FLOW check_heal_SHM ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}
	/declare _count int local 0

	| Group Regen Heal
	/call cast_regen ${maDebug.Find[regen].Value} group

	| no one is hurt. we're out of here
	/if (!${hurtCount}) {
	  | check for surge
	  /if (${maChr.Find[swSurge].Value}) {
			/if (${SpawnCount[npc radius ENV PS4 targetable]}) {
				/call cast_surge_heal ${maDebug.Find[surge].Value}
			}
		}

		SETHUD EMPTY
		/return FALSE

	}

  | stationary ward heal
  /if (${hurtCount} >= ${maHeal.Find[swHealGroup].Value}) {
    /if (${is_ready[${_debug}, "Call of the Ancients"]}) {
    	/call voodoo ${_debug} "Call of the Ancients" ${Me.ID} FALSE
    }
  }

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

|***	| aa Ancestral Guard
	/if (${Me.PctHPs} <= ${${Me.Class.ShortName}Point}*.PCTHO) {
		/if (${is_ready[${_debug}, "Ancestral Guard"]}) {
			/call voodoo ${_debug} "Ancestral Guard" 0 FALSE
		}
	}
***|

 	| single heal over time
	/call cast_regen ${maDebug.Find[regen].Value} single


	ISMEDEAD
	CHECKEXIT
	CHECKFADE
	CHECKREZ

	/call cast_cycle_heal ${_debug} ${hurtID}

	SETHUD EMPTY

/return



|***
 * note: surge heal
 * use: part of heal routine
 ***|
sub cast_surge_heal(bool _debug)
	DEBUG \atcast_surge_heal\ax()
	FLOW cast_surge_heal

	/if (${target[isdead, TARGET]}) {
		/return FALSE
	}


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

			/if (${target[isdead, ${_tmpID}]}) {
				/return FALSE
			}

			/if (${Me.PctMana} < 2) /continue

			/if (${is_ready[${_debug}, "${_li.Value}"]}) {
				/call voodoo ${_debug} "${_li.Value}" ${_tmpID} FALSE
			}
			/delay 15 !${Me.Casting.ID}
			/if (${Me.Song[Healing Twincast].ID}) /break

			/invoke ${_li.Advance}
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
sub burn_SHM(bool _debug, int _tmpID)
	DEBUG \atburn_SHM\ax(\a-w${_tmpID}\ax)
	FLOW burn_SHM

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}


	CHECKEXIT
	GETINPUT
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	| swarm aa pet
	/if (SWARM) {
		/if (${is_ready[${_debug}, "Spirit Call"]}) {
			/call voodoo ${_debug} "Spirit Call" ${_tmpID} FALSE
		}
	}

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}

	| Lower spell resists
	/if (${is_ready[${_debug}, "Dampen Resistance"]}) {
		/call voodoo ${_debug} "Dampen Resistance" ${_tmpID} FALSE
	}

	| Twincast next 6 DOTs/HOTs
	/if (${is_ready[${_debug}, "Fleeting Spirit"]}) {
		/call voodoo ${_debug} "Fleeting Spirit" 0 FALSE
	}

	| Increase spell cost by 14% for 100% exceptional heals on instant heals for 18 seconds
	/if (${is_ready[${_debug}, "Spiritual Blessing"]}) {
		/call voodoo ${_debug} "Spiritual Blessing" 0 FALSE
	}

	|Mega bear crap
	/if (AUTO && MELEE && ${is_ready[${_debug}, "Rabid Bear"]}) {
		/call voodoo ${_debug} "Rabid Bear" ${_tmpID} FALSE
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/call cast_glyph ${maDebug.Find[glyph].Value}

/return



|***
 *	DES: Control for Talisman Buff
 *	USE: /call cast_talisman DEBUG
 ***|
sub cast_talisman(bool _debug)
	DEBUG \atcast_talisman\ax()
	FLOW cast_talisman

	/invoke ${set_data_timer[${_debug}, Check_Talisman, RESTART]}

	/if (${Me.Buff[${maChr.Find[stTalisman].Value}].ID}) /return FALSE

	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/if (${is_ready[${_debug}, "${maChr.Find[stTalisman].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stTalisman].Value}" ${Me.ID} FALSE
	}

/return TRUE



|***
 * note: We want Mana
 * use: /chr
 ***|
sub check_cani(bool _debug)
	DEBUG \atcheck_cani\ax()
	FLOW check_cani

	/if (!${maChr.Find[stPctCanniManna].Value}) {
		DEBUG ${sep}canni disabled
		/return TRUE
	}


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
	/if (!${is_ready[${_debug}, "${maChr.Find[stCannibalization].Value}"]}) {
		DEBUG ${sep}not ready${sep}\a-w${maChr.Find[stCannibalization].Value}\ax
		/return FALSE
	}
	/if (${Me.PctMana} >= ${maChr.Find[stPctCanniManna].Value}) {
		DEBUG ${sep}returning - too much mana${sep}\a-w${Me.PctMana} >= ${maChr.Find[stPctCanniManna].Value}\ax
		/return FALSE
	}
	/if (${Me.PctHPs} <= ${maChr.Find[stPctCanniHP].Value}) {
		DEBUG ${sep}returning - HP too low${sep}\a-w${Me.PctHPs} <= ${maChr.Find[stPctCanniHP].Value}\ax
		/return FALSE
	}

	| check HP requirements
	/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM mana_recovery WHERE env_var="${maChr.Find[stCannibalization].Value}"
	/if (${Me.CurrentHPs} < ${sqlite.Result[dummy 1 value]}) {
		DEBUG ${sep}canni usage would kill us${sep}\a-w${Me.CurrentHPs} < ${sqlite.Result[dummy 1 value]}\ax
		/return FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stCannibalization].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCannibalization].Value}" 0 FALSE
	}

/return TRUE



|***
 * note: check/cast shaman alliance
 * use: yahh... not really sure this even works anymore
 ***|
sub cast_alliance_SHM(bool _debug)
 	DEBUG \atcast_alliance_SHM\ax()
 	FLOW cast_alliance_SHM

	CHECKEXIT
	GETINPUT

	/if (!${is_ready[${_debug}, "${maChr.Find[stAlliance].Value}"]}) /return FALSE

	/declare _name string local ${get_assist_info[${_debug}, name}
	/declare _tmpID int local ${get_assist_info[${_debug}, id}

	/invoke ${set_target[${_name}]}

	/delay 5 ${Target.BuffsPopulated}
	/if (${Target.BuffsPopulated} && !${Target.Buff[${maChr.Find[stAlliance].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stAlliance].Value}" ${_tmpID} FALSE
	}

/return TRUE



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

	|
	} else /if (${_type.Equal[surge]}) {
		/invoke ${set_switch_env[${_debug}, swSurge, TRUE, ${_verbage}]}

	|
	} else /if (${_type.Equal[talisman]}) {
		/invoke ${set_switch_env[${_debug}, swTalisman, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return


	}


	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return


