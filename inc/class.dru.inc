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
 	/call set_data_map FALSE maChr stHealPanic													PREGEN|1											"${auxna} purple club spell"
 	/call set_data_list FALSE maChr lsHealAdrenaline										PREGEN|1											"${auxli} Adrenaline line of heals (medium cast)"
 	/call set_data_list FALSE maChr lsHealPuravida				 							PREGEN|2											"${auxli} Puravida line of heals (long cast)"

 	/call set_data_map FALSE maChr stWolfSpirit													FALSE													"${auxna} name of group wolf aa [\a-wFALSE - white - black\ax]"

	| temp buff: growth family
  /call set_data_list FALSE maChr lsGrowthToon	 											EMPTY													"${auxli} list of toons to use ward temp hp growth on"
	/call set_data_map FALSE maChr stGrowth			 												PREGEN|1											"${auxna} temp hp growth spell"
  /call set_data_list FALSE maChr lsHealRejuvilation									PREGEN|1											"${auxli} Rejuvilation line of heals (fast cast)"

  /call set_data_map FALSE maChr stTwincast														PREGEN|1											"${auxna} twincast spell"


	/declare lsHealOrderbase list outer
	/invoke ${lsHealOrderbase.Delimiter["|"]}
	/invoke ${lsHealOrderbase.Append[lsHealBaseLine]}

	/declare lsHealOrderaggressive list outer
	/invoke ${lsHealOrderaggressive.Delimiter["|"]}
	/invoke ${lsHealOrderaggressive.Append[lsHealRejuvilation|lsHealAdrenaline|lsHealPuravida]}

	/declare lsHealOrderpassive list outer
	/invoke ${lsHealOrderpassive.Delimiter["|"]}
	/invoke ${lsHealOrderpassive.Append[lsHealPuravida|lsHealAdrenaline|lsHealRejuvilation]}

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
	DEBUG main_DRU()
	GETINPUT
	ISMEDEAD
	CHECKTIE


	| correct any and all XTarget healing spots
	/if (AUTO) {
	  /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
	    /call check_xtarget_build FALSE
		}
	}

	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[group].Value} Group
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (${maHeal.Find[swHealXTarget].Value}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[xt].Value} XTarget
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[heal].Value} Pet
		CHECKDEFENSE
		CHECKFADE

		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stGrowth lsGrowthToon
		}
		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
		/if (${check_class_loop[FALSE]}) /return

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
	DEBUG \atmisc_combat_DRU\ax()

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}
	CHECKEXIT
	GETINPUT
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	/if (${is_ready[${_debug}, "Season's Wrath"]} && ${madeBuff.Find[malo].Value}) {
		/call voodoo ${_debug} "Season's Wrath" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "Storm Strike"]}) {
		/call voodoo ${_debug} "Storm Strike" ${_tmpID} FALSE
	}

	| wolf spirit AAs
	/if (${maChr.Find[stWolfSpirit].Value.Equal[black]}) {
		/if (!${Me.Buff[Spirit of the Black Wolf].ID} && ${Me.AltAbilityReady[Group Spirit of the Black Wolf]}) {
			/if (${is_ready[${_debug}, "Group Spirit of the Black Wolf"]}) {
				/call voodoo ${_debug} "Group Spirit of the Black Wolf" 0 FALSE
			}
		} else /if (!${Me.Buff[Group Spirit of the Black Wolf].ID} && !${Me.AltAbilityReady[Group Spirit of the Black Wolf]} && ${Me.AltAbilityReady[Spirit of the Black Wolf]}) {
			/if (${is_ready[${_debug}, "Spirit of the Black Wolf"]}) {
				/call voodoo ${_debug} "Spirit of the Black Wolf" 0 FALSE
			}
		}
	} else /if (${maChr.Find[stWolfSpirit].Value.Equal[white]}) {
		/if (!${Me.Buff[Spirit of the White Wolf].ID} && ${Me.AltAbilityReady[Group Spirit of the White Wolf]}) {
			/if (${is_ready[${_debug}, "Group Spirit of the White Wolf"]}) {
				/call voodoo ${_debug} "Group Spirit of the White Wolf" 0 FALSE
			}
		} else /if (!${Me.Buff[Group Spirit of the White Wolf].ID} && !${Me.AltAbilityReady[Group Spirit of the White Wolf]} && ${Me.AltAbilityReady[Spirit of the White Wolf]}) {
			/if (${is_ready[${_debug}, "Spirit of the White Wolf"]}) {
				/call voodoo ${_debug} "Spirit of the White Wolf" 0 FALSE
			}
		}
	}

/return TRUE



|***
 * note: healing.. cause every druid likes healing
 * use: fuckers...
 ***|
sub check_heal_DRU(bool _debug, string _type)
	DEBUG check_heal_DRU(${_debug}, ${_type})

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/call get_hurt FALSE ${_type}
	/declare _count int local 0

	/if (!${hurtCount}) {
		SETHUD FALSE
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
			/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) {
			  /call cast_list ${_debug} lsHealGroup ${Me.ID}
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

	/call cast_cycle_heal ${_debug} ${hurtID}

	SETHUD FALSE

/return TRUE



|***
 * note: Druid burn routines
 * use: dpsing the things
 ***|
sub burn_DRU(bool _debug, int _tmpID)
	DEBUG \atburn_DRU\ax()

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
		/declare lsAAWolf list local
		/invoke ${lsAAWolf.Append[false,black,white]}

		/if (${lsAAWolf.Contains[${_verbage.Lower}]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stWolfSpirit, "${_verbage.Lower}"]}
		} else {
			OUT /chr wolf [\a-wfalse\ax|\a-wwhite\ax|\a-wblack\ax]
			/return
		}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return



