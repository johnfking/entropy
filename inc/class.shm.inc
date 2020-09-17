|***
 * entropy
 * class.shm.inc
 ***|



|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
	DEBUG \atclass_init\ax()

	/call set_data ${_debug} maData stSpire															map outer "Spire of Ancestors"
	/call set_data ${_debug} maData stSynergy														map outer "Luminary's Synergy"
	/call set_data ${_debug} maData stFade															map outer FALSE
	/call set_data_map ${_debug} maChr stAAUnity												"Visionary's Unity"						"${auxna} class aa unity buff"

	| group regen timer
	/invoke ${set_data_timer[${_debug}, Check_Regen, 3s]}

	/invoke ${set_data_timer[${_debug}, Check_Talisman, 30s]}
	/call set_data_map ${_debug} maChr stTalisman	 											PREGEN|1											"${auxna} talisman spell"
	/call set_data_map ${_debug} maChr swTalisman			 									FALSE													"${auxsw} use talisman spell"

	/call set_data_map ${_debug} maChr stCannibalization								PREGEN|1											"${auxna} canni spell"

	/call set_data_map ${_debug} maChr stPctCanniHP				 							60														"${auxnum} HP level to stop using canni"
	/call set_data_map ${_debug} maChr stPctCanniManna				 					60														"${auxnum} mana level to start using canni"

	/call set_data_map ${_debug} maHeal stCountSurge										0															"${auxnum} number of people hurt to use surge healing"

 	/call set_data_map ${_debug} maChr stDichotomic											PREGEN|1											"${auxna} dichotomic spell"
 	/call set_data_map ${_debug} maChr stAlliance												PREGEN|1											"${auxna} alliance spell"

  /call set_data_map ${_debug} maHeal swHealGroupOverTime							FALSE													"${auxsw} use group heal over time spells/aas" 						heal

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
sub class_main(bool _debug)
	DEBUG \atclass_main\ax()
	GETINPUT
	ISMEDEAD
	CHECKTIE

	| correct any and all XTarget healing spots
	/if (AUTO && !ENGINE3) {
	  /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
	    /call check_xtarget_build FALSE
		}
		/call check_cani ${maDebug.Find[canni].Value}
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
		/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
	} else /if (AUTO && ENGINE3) {
		/if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
			/call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
		}

	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		/call check_heal_routines
		CHECKFADE
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stGrowth lsGrowthToon
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}

		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		/if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
		/if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

		/if (VALID && !ENGINE3) {
			CHECKREZ
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/call misc_combat ${maDebug.Find[misc].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/if (${maChr.Find[swTalisman].Value}) /call cast_talisman ${maDebug.Find[talisman].Value}
			/if (${maCC.Find[swAlliance].Value}) /call cast_alliance ${maDebug.Find[alliance].Value}
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[minion].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		} else /if (VALID && ENGINE3) {
			/if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
				/call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}

		/if (AUTO && !ENGINE3) {
			CHECKFADE
			/call check_cani ${maDebug.Find[canni].Value}
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		} else /if (AUTO && !ENGINE3) {
			/if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
				/call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}
	}

/return


|***
 * noote: Combat skills, Disciplines and Alternate abilities.
 * use: yes, there are a lot here... lies..
 ***|
sub misc_combat(bool _debug, int _tmpID)
	DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE
	CHECKDEFENSE
	/if (!PULL) {
		/call get_assist_target ${maDebug.Find[target].Value}
		/if (TARGET != ${_tmpID}) /varset _tmpID TARGET
	}

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
sub check_heal(bool _debug, string _type)
	DEBUG \atcheck_heal\ax(\a-w${_type}\ax)
	FLOW check_heal ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	| Group Regen Heal
	/if (${maHeal.Find[swHealGroupOverTime].Value} && !${timer_Check_Regen}) {
		/call cast_cycle_heal_regen ${_debug} 0 grouphot
	}

	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

	/if (!SAFEZONE) {

			| check for surge
		/if (${Select[${_type},group,xtarget]} && ${maHeal.Find[stCountSurge].Value}) {
			/if (${hurtCount} >= ${maHeal.Find[stCountSurge].Value}) {
				/call cast_cycle_heal_surge ${maDebug.Find[surge].Value}
			}
		}


		| group healing
		/if (${Select[${_type},group]} && ${Group}) {

			| group heal
			/if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
				/call cast_cycle_heal ${_debug} 0 group
			}

		 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
		 		/return FALSE
		 	}

			| stationary ward heal
			/if (${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
			  /if (${is_ready[${_debug}, "Call of the Ancients"]}) {
			  	/call voodoo ${_debug} "Call of the Ancients" ${Me.ID} FALSE
			  }
			}

		}

		| PET HEALING
		/if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value} && ${Pet.ID}) {
			/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
				/call cast_cycle_heal ${_debug} ${Pet.ID} minion
			}
		}

	 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
	 		/return FALSE
	 	}

		| single heal over time
		/if (${maHeal.Find[swHealOverTime].Value} && !${timer_Check_Regen}) {
			/if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
				/call cast_cycle_heal_regen ${_debug} ${hurtID} hot
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

	/call cast_cycle_heal ${_debug} ${hurtID} single

	SETHUD EMPTY

/return



|***
 * note: surge heal
 * use: part of heal routine
 ***|
sub cast_cycle_heal_surge(bool _debug)
	DEBUG \atcast_cycle_heal_surge\ax()
	FLOW cast_cycle_heal_surge

	CHECKEXIT
	ISMEDEAD
	GETINPUT

	| get the main assisst ID
	/declare _tmpID int local ${get_assist_info[${_debug}, ID]}
	DEBUG ${sep}assist ID${sep}\a-w${_tmpID}\ax

	/if (!${Spawn[${_tmpID}].LineOfSight}) {
		DEBUG ${break}.LineOfSight
		/return FALSE
	}

	| get our twincast proc
  /if (!${Me.Song[Healing Twincast].ID}) {
		/call cast_cycle_nuke ${_debug} ${_tmpID} twincast
	}

	/if (!${Me.Song[Healing Twincast].ID}) {
		DEBUG ${break}no twincast song
		/return FALSE
	}

	| get our surge spell

	| if the user has not set the line order, set it to a default for now
	/if (${lsLineHeal.Contains[EMPTY]} && ${lsLineHeal.Count} == 1) {
		DEBUG ${sep}resetting the empty heal line
		/invoke ${lsLineHeal.Clear}
		/invoke ${lsLineHeal.Append[${stCount50}]}
	}

	/declare _myHeal string local FALSE
	/declare _liHeal listiterator local
	/vardata _liHeal lsLineHeal.First.Clone
	/while (!${_liHeal.IsEnd}) {
		DEBUG ${sep}heal${sep}\a-w${maHeal.Find[stHeal${_liHeal.Value}].Value}${sep}${maHeal.Find[stHeal${_liHeal.Value}]}\ax
		GETINPUT cast_cycle_heal_surge NA|NA
		ISMEDEAD
		CHECKEXIT cast_cycle_heal_surge
		FLOW cast_cycle_heal_surge ${_tmpID} (1)

		| check the switch
		/if (!${Bool[${maHeal.Find[swHeal${_liHeal.Value}].Value}]}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		| check the item name
		/if (!${Bool[${maHeal.Find[stHeal${_liHeal.Value}].Value}]}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		| check matching tag for failure
		/if (!${lsHeal${_liHeal.Value}Tag.Contains[surge]}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		| check matching tag pass
		/if (${lsHeal${_liHeal.Value}Tag.Contains[surge]}) {
			/varset _myHeal ${maHeal.Find[stHeal${_liHeal.Value}].Value}
		}

		| ready?.. or continue
		/if (!${is_ready[${_debug}, "${_myHeal}"]}) {
			/varset _myHeal FALSE
			/invoke ${_liHeal.Advance}
			/continue
		}

		| cycle the tags
		/call tag_evaluation ${maDebug.Find[tag].Value} Heal ${_liHeal.Value} ${Spawn[${_tmpID}].DisplayName} "${maHeal.Find[stHeal${_liHeal.Value}Con].Value}" cast_cycle_heal_surge
		/if (!${Macro.Return}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		/call voodoo ${_debug} "${maHeal.Find[stHeal${_liHeal.Value}].Value}" ${_tmpID} FALSE

		/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
			/return TRUE
		}

		/invoke ${_liHeal.Advance}
	}

/return TRUE



|***
 * note: Shaman burn routines
 * use: normal burn stuf
 ***|
sub burn(bool _debug, int _tmpID)
	DEBUG \atburn\ax(\a-w${_tmpID}\ax)
	FLOW burn_SHM

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE
	CHECKDEFENSE
	/if (!PULL) {
		/call get_assist_target ${maDebug.Find[target].Value}
		/if (TARGET != ${_tmpID}) /varset _tmpID TARGET
	}

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

	| Mega bear crap
	/if (AUTO && MELEE && ${is_ready[${_debug}, "Rabid Bear"]}) {
		/call voodoo ${_debug} "Rabid Bear" ${_tmpID} FALSE
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 *	DES: Control for Talisman Buff
 *	USE: /call cast_talisman DEBUG
 ***|
sub cast_talisman(bool _debug)
	DEBUG \atcast_talisman\ax()
	FLOW cast_talisman

	/invoke ${set_data_timer[${_debug}, Check_Talisman, RESTART]}

	/if (${Me.Buff[${maChr.Find[stTalisman].Value}].ID}) {
		/return FALSE
	}

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
	/sqlite query ${maData.Find[DBstatic].Value} dummy SELECT * FROM mana_recovery WHERE env_var="${maChr.Find[stCannibalization].Value}";
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
sub cast_alliance(bool _debug)
 	DEBUG \atcast_alliance\ax()
 	FLOW cast_alliance

	CHECKEXIT
	GETINPUT

	/if (!${is_ready[${_debug}, "${maChr.Find[stAlliance].Value}"]}) /return FALSE

	/declare _name string local ${get_assist_info[${_debug}, name]}
	/declare _tmpID int local ${get_assist_info[${_debug}, id]}

	/invoke ${set_target[${_name}]}

	/delay 5 ${Target.BuffsPopulated}
	/if (${Target.BuffsPopulated} && !${Target.Buff[${maChr.Find[stAlliance].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stAlliance].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
	DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

	| % of HP to canni at
	/if (${_type.Equal[cannihps]}) {
		/if (!${set_control_num_range[${_debug}, stPctCanniHP, "${_verbage}", 0, 99]}) /return FALSE

	|
	} else /if (${_type.Equal[cannimanna]}) {
		/if (!${set_control_num_range[${_debug}, stPctCanniManna, "${_verbage}", 0, 99]}) /return FALSE

	| use talisman
	} else /if (${_type.Equal[talisman]}) {
		/invoke ${set_switch_env[${_debug}, swTalisman, TRUE, ${_verbage}]}

	}

	/call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
	/if (${Macro.Return.Equal[SKIP]}) /return FALSE

	/if (${_verbage2.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE


