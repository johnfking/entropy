|***
 * entropy
 * class.dru.inc
 ***|



|***
 * note: primary initilization of all druid non shared data
 * use:
 ***|
sub set_DRU(bool _debug)
	DEBUG \atset_DRU\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of Nature"
	/call set_data FALSE maData stSynergy																map outer "Preserver's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Nature Walker's Scimitar"
	/call set_data FALSE maData stEpic15																map outer "Staff of Living Brambles"
	/call set_data FALSE maData stEpic2																	map outer "Staff of Everliving Brambles"
	/call set_data FALSE maData stFade																	map outer "Veil of the Underbrush"
	/call set_data_map FALSE maChr stAAUnity														"Wildtender's Unity"					"${auxna} class aa unity buff"

 	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance spell"

	/call set_data_map FALSE maChr stCountHealGroup											0															"${auxnum} number in group hurt to use a group heal"
 	/call set_data_list FALSE maChr lsHealGroup													PREGEN|2											"${auxli} group heal spells"
 	/call set_data_map FALSE maChr swWolfSpirit													FALSE													"${auxsw} enable spirit of the great wolf aa (self/group)"

	| temp buff: growth family
  /call set_data_list FALSE maChr lsGrowthToon	 											EMPTY													"${auxli} list of toons to use ward temp hp growth on"
	/call set_data_map FALSE maChr stGrowth			 												PREGEN|1											"${auxna} temp hp growth spell"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

  /call set_data_map FALSE maChr stTwincast														PREGEN|1											"${auxna} twincast spell"


	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return



|***
 * note: main druid routine
 * use:
 ***|
sub main_DRU(bool _debug)
	DEBUG \atmain_DRU\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE


	| correct any and all XTarget healing spots
	/if (AUTO) {
	  /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
	    /call check_xtarget_build FALSE
		}
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
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		}

		/if (AUTO) {
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			CHECKFADE
		}

	}

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: pushmorebuttons!!
 ***|
sub misc_combat_DRU(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_DRU\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat_DRU

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

	/if (ENGINE2) {
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	/if (${is_ready[${_debug}, "Season's Wrath"]} && ${madeBuff.Find[malo].Value}) {
		/call voodoo ${_debug} "Season's Wrath" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "Storm Strike"]}) {
		/call voodoo ${_debug} "Storm Strike" ${_tmpID} FALSE
	}

	| great wolf self/group aa
	/if (${maChr.Find[swWolfSpirit].Value}) {
		/if (${Me.AltAbilityReady[Spirit of the Great Wolf]} && !${Me.Buff[Spirit of the Great Wolf].ID} && !${Me.Buff[Group Spirit of the Great Wolf].ID}) {
			/if (${is_ready[${_debug}, "Spirit of the Great Wolf"]}) {
				/call voodoo ${_debug} "Spirit of the Great Wolf" 0 FALSE
			}
		} else /if (${Me.AltAbilityReady[Group Spirit of the Great Wolf]} && !${Me.AltAbilityReady[Spirit of the Great Wolf]} && !${Me.Buff[Spirit of the Great Wolf].ID} && !${Me.Buff[Group Spirit of the Great Wolf].ID}) {
			/if (${is_ready[${_debug}, "Group Spirit of the Great Wolf"]}) {
				/call voodoo ${_debug} "Group Spirit of the Great Wolf" 0 FALSE
			}
		}
	}


/return TRUE



|***
 * note: healing.. cause every druid likes healing
 * use: fuckers...
 ***|
sub check_heal_DRU(bool _debug, string _type)
	DEBUG \atcheck_heal_DRU\ax(\a-w${_type}\ax)
	FLOW check_heal_DRU ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/call get_hurt FALSE ${_type}
	/declare _count int local 0

	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return
	}

	CHECKEXIT

	/if (!SAFEZONE) {

		/if (${hurtPctHP} < ${maHeal.Find[stHealPointPanic].Value}*.PCTHO) {
			/if (${Me.Casting.ID}) /stopcast
			/if (${is_ready[${_debug}, "${maChr.Find[stHealPanic].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stHealPanic].Value}" ${hurtID} FALSE
			}
			/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
		}

		/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "Swarm of Fireflies"]}) {
				/call voodoo ${_debug} "Swarm of Fireflies" ${hurtID} FALSE
			}
		}

		/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "Convergence of Spirits"]}) {
				/call voodoo ${_debug} "Convergence of Spirits" ${hurtID} FALSE
			}
		}

	}

	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

	GETINPUT

  | Group Heal
  /if (${Select[${_type},group]} && ${maChr.Find[stCountHealGroup].Value}) {
	  /if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) {

			/if (!SAFEZONE) {

			/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
				/if (${is_ready[${_debug}, "Wildtender's Survival"]}) {
					/call voodoo ${_debug} "Wildtender's Survival" 0 FALSE
				}
			}

		  	/if (${is_ready[${_debug}, "Peaceful Spirit of the Wood"]}) {
		  		/call voodoo ${_debug} "Peaceful Spirit of the Wood" ${Me.ID} FALSE
		  		/return
		  	}

		  	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

			 	/if (${is_ready[${_debug}, "Blessing of Tunare"]}) {
			 		/call voodoo ${_debug} "Blessing of Tunare" 0 FALSE
			 		/return
			 	}
			}

		 	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

		  | group heal?
			/if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
				/call cast_cycle_heal ${_debug} 0 healgroup
 			}

	  }
	}

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	ISMEDEAD
	CHECKEXIT
	CHECKFADE
	CHECKREZ

	/call cast_cycle_heal ${_debug} ${hurtID} healsingle

	SETHUD EMPTY

/return TRUE



|***
 * note: Druid burn routines
 * use: dpsing the things
 ***|
sub burn_DRU(bool _debug, int _tmpID)
	DEBUG \atburn_DRU\ax()
	FLOW burn_DRU

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

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}

	| swarm pets
	/if (SWARM) {
		/if (${is_ready[${_debug}, "Nature's Guardian"]})) {
			/call voodoo ${_debug} "Nature's Guardian" ${_tmpID} FALSE
		}
	}

	| defense
	/if (${is_ready[${_debug}, "Protection of Direwood"]}) {
		/call voodoo ${_debug} "Protection of Direwood" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Silent Casting"]}) {
		/call voodoo ${_debug} "Silent Casting" 0 FALSE
	}

	| damage boost
	/if (${is_ready[${_debug}, "Distructive Vortex"]}) {
		/call voodoo ${_debug} "Distructive Vortex" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Natures Fury"]}) {
		/call voodoo ${_debug} "Natures Fury" 0 FALSE
	}

	| /if (${is_ready[${_debug}, "Group Spirit of the Black Wolf"]}) /call voodoo ${_debug} "Group Spirit of the Black Wolf" 0 FALSE

	| heal boost
	/if (${is_ready[${_debug}, "Nature's Blessing"]}) {
		/call voodoo ${_debug} "Nature's Blessing" 0 FALSE
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	| AA AE heal
	/if (!${Raid.Members}) {
		/if (${is_ready[${_debug}, "Nature's Boon"]}) {
			/call voodoo ${_debug} "Nature's Boon" 0 FALSE
		}
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}

/return TRUE




|***
 * note: cleric specific control
 * use: /chr
 ***|
sub set_control_DRU(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_DRU\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| wolf spirit AAs
	} else /if (${_type.Equal[wolf]}) {
		/invoke ${set_switch_env[${_debug}, swWolfSpirit, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return



