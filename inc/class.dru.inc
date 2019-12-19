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
 	/call set_data_map FALSE maChr lsHealAdrenaline				 							PREGEN|1											"${auxli} Adrenaline line of heals (medium cast)"
 	/call set_data_map FALSE maChr lsHealPuravida				 								PREGEN|2											"${auxli} Puravida line of heals (long cast)"

 	/call set_data_map FALSE maChr stWolfSpirit													FALSE													"${auxna} name of group wolf aa [\a-wFALSE - white - black\ax]"

	| temp buff: growth family
  /call set_data_list FALSE maChr lsGrowthToon	 											EMPTY													"${auxli} list of toons to use ward temp hp growth on"
	/call set_data_map FALSE maChr stGrowth			 												PREGEN|1											"${auxna} temp hp growth spell"
  /call set_data_list FALSE maChr lsHealRejuvilation									PREGEN|1											"${auxli} Rejuvilation line of heals (fast cast)"

  /call set_data_map FALSE maChr stTwincast														PREGEN|1											"${auxna} twincast spell"


	/declare lsHealOrderbase list outer
	/varset dump ${lsHealOrderbase.Delimiter["|"]}
	/varset dump ${lsHealOrderbase.Append[lsHealBaseLine]}

	/declare lsHealOrderaggressive list outer
	/varset dump ${lsHealOrderaggressive.Delimiter["|"]}
	/varset dump ${lsHealOrderaggressive.Append[lsHealRejuvilation|lsHealAdrenaline|lsHealPuravida]}

	/declare lsHealOrderpassive list outer
	/varset dump ${lsHealOrderpassive.Delimiter["|"]}
	/varset dump ${lsHealOrderpassive.Append[lsHealPuravida|lsHealAdrenaline|lsHealRejuvilation]}
	
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
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Group
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/if (${maHeal.Find[swHealXTarget].Value}) /call check_heal_${Me.Class.ShortName} FALSE XTarget
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Pet
		/if (${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle ${maDebug.Find[def].Value}
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
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (!${timer_Check_deBuffs}) /call check_deBuffs FALSE TARGET
			/if (DOT) /call cast_dot FALSE lsDot TARGET
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/call check_item_cycle FALSE TARGET
		}

		/if (AUTO) {	
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
			CHECKFADE
		}

	}

/return



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: pushmorebuttons!!
 ***|
sub misc_combat_DRU(bool _debug)
	DEBUG \atmisc_combat_DRU\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

	/if (${is_ready[FALSE, "Season's Wrath"]})) /call voodoo FALSE "Season's Wrath" TARGET FALSE
	/if (${is_ready[FALSE, "Storm Strike"]})) /call voodoo FALSE "Storm Strike" TARGET FALSE
	
	| wolf spirit AAs
	/if (${maChr.Find[stWolfSpirit].Value.Equal[black]}) {	
		/if (!${Me.Buff[Spirit of the Black Wolf].ID} && ${Me.AltAbilityReady[Group Spirit of the Black Wolf]}) {
			/if (${is_ready[FALSE, "Group Spirit of the Black Wolf"]}) /call voodoo FALSE "Group Spirit of the Black Wolf" 0 FALSE
		} else /if (!${Me.Buff[Group Spirit of the Black Wolf].ID} && !${Me.AltAbilityReady[Group Spirit of the Black Wolf]} && ${Me.AltAbilityReady[Spirit of the Black Wolf]}) {
			/if (${is_ready[FALSE, "Spirit of the Black Wolf"]}) /call voodoo FALSE "Spirit of the Black Wolf" 0 FALSE
		}
	} else /if (${maChr.Find[stWolfSpirit].Value.Equal[white]}) {	
		/if (!${Me.Buff[Spirit of the White Wolf].ID} && ${Me.AltAbilityReady[Group Spirit of the White Wolf]}) {
			/if (${is_ready[FALSE, "Group Spirit of the White Wolf"]}) /call voodoo FALSE "Group Spirit of the White Wolf" 0 FALSE
		} else /if (!${Me.Buff[Group Spirit of the White Wolf].ID} && !${Me.AltAbilityReady[Group Spirit of the White Wolf]} && ${Me.AltAbilityReady[Spirit of the White Wolf]}) {
			/if (${is_ready[FALSE, "Spirit of the White Wolf"]}) /call voodoo FALSE "Spirit of the White Wolf" 0 FALSE
		}
	}
	
/return



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
	
	/call check_hurt FALSE ${_type}
	/declare _count int local 0
	
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return	
	}
  
	CHECKEXIT

	/if (!SAFEZONE) {

		/if (${hurtPctHP} < ${maHeal.Find[stHealPointPanic].Value}*.PCTHO) {
			/if (${Me.Casting.ID}) /stopcast
			/if (${is_ready[FALSE, "${maChr.Find[stHealPanic].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stHealPanic].Value}" ${hurtID} FALSE
			}
			/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		}
	
		/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Swarm of Fireflies"]}) {
				/call voodoo FALSE "Swarm of Fireflies" ${hurtID} FALSE
			}
		}
		
		/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Convergence of Spirits"]}) {
				/call voodoo FALSE "Convergence of Spirits" ${hurtID} FALSE
			}
		}
		  
	}

	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE

	GETINPUT

  | Group Heal
  /if (${Select[${_type},group]} && ${maChr.Find[stCountHealGroup].Value}) {
	  /if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) {
	
			/if (!SAFEZONE) {

			/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
				/if (${is_ready[FALSE, "Wildtender's Survival"]}) {
					/call voodoo FALSE "Wildtender's Survival" 0 FALSE
				}
			}	
				
		  	/if (${is_ready[FALSE, "Peaceful Spirit of the Wood"]}) {
		  		/call voodoo FALSE "Peaceful Spirit of the Wood" ${Me.ID} FALSE
		  		/return
		  	}
		  	
		  	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE
		  	
			 	/if (${is_ready[FALSE, "Blessing of Tunare"]}) {
			 		/call voodoo FALSE "Blessing of Tunare" 0 FALSE
			 		/return
			 	}
			}
		 	
		 	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE
	  	
		  | group heal?
			/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) {
			  /call cast_list ${_debug} lsHealGroup ${Me.ID}
			}	    
	    
	  } 
	}
	
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
		/return TRUE
		/varset dump ${_liHM.Advance}
	}
    
	SETHUD FALSE
	
/return



|***
 * note: Druid burn routines
 * use: dpsing the things
 ***| 
sub burn_DRU(bool _debug)
	DEBUG \atburn_DRU\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
	
	| swarm pets
	/if (SWARM) {
		/if (${is_ready[FALSE, "Nature's Guardian"]})) /call voodoo FALSE "Nature's Guardian" TARGET FALSE
	}
	
	| defense
	/if (${is_ready[FALSE, "Protection of Direwood"]}) /call voodoo FALSE "Protection of Direwood" 0 FALSE
	/if (${is_ready[FALSE, "Silent Casting"]}) /call voodoo FALSE "Silent Casting" 0 FALSE

	| damage boost
	/if (${is_ready[FALSE, "Distructive Vortex"]}) /call voodoo FALSE "Distructive Vortex" 0 FALSE
	/if (${is_ready[FALSE, "Natures Fury"]}) /call voodoo FALSE "Natures Fury" 0 FALSE
	| /if (${is_ready[FALSE, "Group Spirit of the Black Wolf"]}) /call voodoo FALSE "Group Spirit of the Black Wolf" 0 FALSE
	/if (${is_ready[FALSE, "Seasons Wrath"]})) /call voodoo FALSE "Seasons Wrath" TARGET FALSE
	
	| heal boost
	/if (${is_ready[FALSE, "Nature's Blessing"]}) /call voodoo FALSE "Nature's Blessing" 0 FALSE

	/call cast_AA_spire ${maDebug.Value[spire].Value}
	
	| AA AE heal
	/if (!${Raid.Members}) {
		/if (${is_ready[FALSE, "Nature's Boon"]}) {
			/call voodoo FALSE "Nature's Boon" 0 FALSE
		}
	}

	/call cast_glyph ${maDebug.Value[glyph].Value}

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
		/if (!${set_control_num_range[FALSE, stPctManaRecovery, "${_verbage}", 0, 99]}) /return
		
	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire FALSE ${_verbage} ${_silent}
		/return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[FALSE, stPctFade, "${_verbage}", 0, 99]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[FALSE, stCountHealGroup, "${_verbage}", 0, 6]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return


	| wolf spirit AAs
	} else /if (${_type.Equal[wolf]}) {
		/declare lsAAWolf list local
		/varset dump ${lsAAWolf.Append[false,black,white]}

		/if (${lsAAWolf.Contains[${_verbage.Lower}]}) {
			/varset dump ${do_raw_edit[${_debug}, SILENT, stWolfSpirit, "${_verbage.Lower}"]}
		} else {
			OUT /chr wolf [\a-wfalse\ax|\a-wwhite\ax|\a-wblack\ax]
			/return
		}

	}

	
	/if (${_silent.Equal[SILENT]}) /return 
	/varset dump ${set_control_output[${_debug}, maChr, chr, maChrControl]}
	
/return



