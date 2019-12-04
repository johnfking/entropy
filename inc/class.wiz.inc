|***
 * entropy
 * class.wiz.inc
 ***|
 


|***
 * note: primary initilization of all wizard non shared data
 * use: not for you
 ***|
sub set_WIZ(bool _debug)
	DEBUG \atset_WIZ\ax()

	/call set_data FALSE maData stSpire																	map outer "Spire of Arcanum"
	/call set_data FALSE maData stSynergy																map outer "Evoker's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Staff of the Four"
	/call set_data FALSE maData stEpic15																map outer "Staff of Prismatic Power"
	/call set_data FALSE maData stEpic2																	map outer "Staff of Phenomenal Power"
	/call set_data FALSE maData stFade																	map outer "A Hole in Space"
	/call set_data FALSE maData stCompleteHeal													map outer "Complete Heal"
	/call set_data_map FALSE maChr stAAUnity														"Etherealist's Unity"					"${auxna} class aa unity buff"

	/call set_data_map FALSE maChr stPctGather													0															"${auxnum} % of your mana to start using gather spells and AAs"
	/call set_data_map FALSE maChr stGather															PREGEN|1											"${auxna} name of mana gather spell (\a-wNOT THE AA\ax)"
	
	/call set_data_map FALSE maChr stTempPet														PREGEN|1											"${auxna} temp flame'ie pig sticker pet spell"

	/call set_data_map FALSE maChr swManaBurn		 												FALSE													"${auxsw} use aa manaburn during burns"
	
	/call set_data_map FALSE maChr stConcussion													PREGEN|1											"${auxna} name of anti agro spell cause you nuke to damn much and dont let tank get agro"
	/call set_data_map FALSE maChr stPctConcussion											0															"${auxnum} % of your agro you want to try and cover your ass at"

	| current nuke line
	/declare lsNukeLine list outer
	/varset dump ${lsNukeLine.Append[base,fire,ice,magic]}
	/call set_data_map FALSE maChr stCurrent														base
	/call set_data FALSE FALSE stLine																		string outer ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}
	| /call set_data_map FALSE maChr stAABase															FALSE													"${auxna} pick a boost aa for DPS"
	
	/varset dump ${set_data_timer[FALSE, Force_TC, 6s]}
	/call set_data_map FALSE maChr stTC																	PREGEN|1											"${auxna} whatever the hell you want to use for twincast"
	/call set_data_map FALSE maChr swForceTC														FALSE													"${auxsw} force using twincast aa/spells as much as possible"
	/call set_data_map FALSE maChr stNukeTCFire													PREGEN|1											"${auxna} fire line twincast spell"
	/call set_data_map FALSE maChr stNukeTCIce													PREGEN|1											"${auxna} ice line twincast spell"
	/call set_data_map FALSE maChr stNukeTCMagic												PREGEN|1											"${auxna} magic line twincast spell"
	/call set_data_map FALSE maChr stNukeTCBase													FALSE													"${auxna} base line twincast spell"
	
	
/return	

	
	
|***
 * note: main wizzeurd routine
 * use: 
 ***|
sub main_WIZ(bool _debug)
	DEBUG \atmain_WIZ\ax()

	/if (AUTO) {
	}
	
	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[heal].Value} Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (${check_class_loop[FALSE]}) /return
	
		/if (VALID) {
			/call set_combat FALSE
			/if (!${is_defensive_active[FALSE, FALSE, SAFEENV]}) /call cast_defensive_cycle FALSE
			/if (!${timer_Check_deBuffs}) /call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			/if (BURN) /call burn_${Me.Class.ShortName}  ${maDebug.Find[burn].Value}
			/if (PET || SWARM) /call send_pet FALSE TARGET
			/if (SWARM) /call cast_temp_pet FALSE TARGET "${maChr.Find[stTempPet].Value}"
			/if (${maCC.Find[swAlliance].Value}) /call trigger_alliance TARGET
			/if (DOT) /call cast_dot ${maDebug.Find[castdot].Value} lsDoT TARGET
			/if (NUKE) {
				/if (${maChr.Find[swForceTC].Value}) /call force_twincast_${Me.Class.ShortName} TRUE
				/call cast_nuke ${maDebug.Find[castnuke].Value} lsNuke${stLine} TARGET
			}
			/if (MELEE) /call use_skill_melee TARGET
			/call misc_combat_${Me.Class.ShortName}
			/call check_item_cycle ${maDebug.Find[castitem].Value} TARGET

		}
		
		/if (AUTO) {	
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			/if (${Me.PctMana} <= ${maChr.Find[stPctGather].Value}*.PCTHO) /call check_harvest ${maDebug.Find[mannarecover].Value}
			CHECKFADE
		}	
	}	
	
/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat_WIZ(bool _debug)
	DEBUG \atmisc_combat_WIZ\ax()

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return

	/if (!${is_defensive_active[${_debug}, FALSE, SAFEENV]}) /call cast_defensive_cycle FALSE
		

	| agro control
	/if (AGRO) {
	} else /if (!AGRO) {
		/if (${is_ready[FALSE, "Arcane Whisper"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value}) /call voodoo FALSE "Arcane Whisper" TARGET FALSE
		/if (${is_ready[FALSE, "Mind Crash"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value} && ${lsZoneNamed.Contains[${Spawn[TARGET].DisplayName}]}) /call voodoo FALSE "Mind Crash" TARGET FALSE
		/if (${is_ready[FALSE, "Concussive Intuition"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value}) /call voodoo FALSE "Concussive Intuition" TARGET FALSE
		/if (${is_ready[FALSE, "${maChr.Find[stConcussion].Value}"]} && ${Me.PctAggro} > ${maChr.Find[stPctConcussion].Value}) /call voodoo FALSE "${maChr.Find[stConcussion].Value}" TARGET FALSE
	}
	
/return



|***
 * note: But you say I'm a dog when I leave you fucked and deserted.
 * use: /call check_harvest DEBUG
 ***|
sub check_harvest(bool _debug)
	DEBUG \atcheck_harvest\ax()
	
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${Me.Buff[Improved Twincast].ID} || ${Me.Buff[${maChr.Find[stTC].Value}].ID}) /return

	/if (!${is_ready[FALSE, "${maChr.Find[stGather].Value}"]}) {
		/if (${is_ready[FALSE, "Forceful Rejuvenation"]}) {
			/call voodoo FALSE "Forceful Rejuvenation" 0 FALSE
		}
		/delay 5			
	}
	/if (${Me.PctMana} < ${maChr.Find[stPctGather].Value}*.PCTHO) {
		/if (${is_ready[FALSE, "${maChr.Find[stGather].Value}"]}) {
			/if (${is_ready[FALSE, "${maChr.Find[stGather].Value}"]}) {
				/call voodoo FALSE "${maChr.Find[stGather].Value}" 0 FALSE
			}
		}
		/if (${Me.AltAbilityReady[Harvest of Druzzil]}) {
			/if (${is_ready[FALSE, "Harvest of Druzzil"]}) {
				/call voodoo FALSE "Harvest of Druzzil" 0 FALSE
			}
		}
	}

/return



|***
 * note: healing. no. you dont get any. suck it up butercup
 * use: /call check_heal_${Me.Class.ShortName}
 ***|
sub check_heal_WIZ(bool _debug, string _type)
	DEBUG \atcheck_heal_WIZ\ax(\a-w${_type}\ax)


	/if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) /return FALSE

	ISMEDEAD
	CHECKEXIT

	/call get_hurt FALSE ${_type} 
	/call cast_list FALSE lsHealBaseLine ${Me.ID}

/return



|***
 * note: Wizard burn routines
 * use: bug guns?
 ***| 
sub burn_WIZ(bool _debug)
	DEBUG \atburn_WIZ\ax()
	
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return
	
	| shhhhh..
	/if (${is_ready[FALSE, "Silent Casting"]}) /call voodoo FALSE "Silent Casting" 0 FALSE

	| next 45 spells +51% crit rate
	/if (${is_ready[FALSE, "Frenzied Devastation"]}) /call voodoo FALSE "Frenzied Devastation" 0 FALSE
	| increase chance to land
	/if (${is_ready[FALSE, "Focus of Arcanum"]}) /call voodoo FALSE "Focus of Arcanum" 0 FALSE


	| alternate twincast
	/if (!${Me.Buff[${maChr.Find[stTC].Value}].ID}) {
		/if (${is_ready[FALSE, "Improved Twincast"]}) /call voodoo FALSE "Improved Twincast" 0 FALSE
	} else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.AltAbilityReady[Improved Twincast]} && ${is_ready[FALSE, "${maChr.Find[stTC].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stTC].Value}" 0 FALSE
	}
	
	/call cast_AA_spire FALSE

	| boost spell type damage
	/if (${is_ready[FALSE, "Fury of the Gods"]}) /call voodoo FALSE "Fury of the Gods" 0 FALSE

	/if (${is_ready[FALSE, "Arcane Destruction"]}) /call voodoo FALSE "Arcane Destruction" 0 FALSE
	
	| increase damage
	/if (${is_ready[FALSE, "Arcane Fury"]}) /call voodoo FALSE "Arcane Fury" 0 FALSE
	
	| aww baby
	/if (${maChr.Find[swManaBurn].Value}) {
		/varset dump ${set_target[TARGET]}
		/if (!${Target.Buff[Mana Burn].ID}) {
			/if (${is_ready[FALSE, "Mana Burn"]}) /call voodoo FALSE "Mana Burn" TARGET FALSE
		}
	}

	| swarm pet
	/if (SWARM) {
		/if (${is_ready[FALSE, "Call of Xuzl"]}) /call voodoo FALSE "Call of Xuzl" TARGET FALSE
	}

	/call cast_glyph FALSE
	
/return



|***
 * note: Control for getting Twincast either spell or Proc
 * use: /call force_twincast_wiz DEBUG
 ***|
sub force_twincast_WIZ(bool _debug)
	DEBUG \atforce_twincast_WIZ\ax()

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	
	/varset dump ${set_data_timer[FALSE, Force_TC, 6s]}
	
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return

	/if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[Twincast].ID} && ${is_ready[FALSE, "${maChr.Find[stTC].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stTC].Value}" 0 FALSE
		/return

	} else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[${maChr.Find[stTC].Value}].ID} && !${Me.Buff[Twincast].ID} && ${is_ready[FALSE, "${maChr.Find[stNukeTC${stLine}].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stNukeTC${stLine}].Value}" TARGET FALSE
	}
	
/return



|***
 * note: Class controls
 * use: /chr []
 ***|
sub set_control_WIZ(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_WIZ\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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
		
	| set concussion use %	
	} else /if (${_type.Equal[concussion]}) {
		/if (!${set_control_num_range[FALSE, stPctConcussion, "${_verbage}", 0, 99]}) /return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[FALSE, stPctFade, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return


	} else /if (${_type.Equal[line]}) {
		/if (!${lsNukeLine.Contains[${_verbage.Lower}]}) {
			OUT usage${sep}/chr line [\a-wbase\ax|\a-wfire\ax|\a-wice\ax|\a-wmagic\ax]
			/return
		}
		/varset dump ${do_raw_edit[${_debug}, SILENT, stCurrent, "${_verbage.Lower}"]}
		/varset stLine ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}


	| set gather use %	
	} else /if (${_type.Equal[gather]}) {
		/if (!${set_control_num_range[FALSE, stPctGather, "${_verbage}", 0, 99]}) /return

	| use manaburn
	} else /if (${_type.Equal[mb]}) {
		/varset dump ${set_switch_env[${_debug}, swManaBurn, TRUE, ${_verbage}]}
		
	| force twincast attempts
	} else /if (${_type.Equal[forcetc]}) {
		/varset dump ${set_switch_env[${_debug}, swForceTC, TRUE, ${_verbage}]}
		

	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset _out
	/declare _lichr listiterator local
	/vardata _lichr lsChr.First.Clone
	/while (!${_lichr.IsEnd}) {
		/varset _out ${_out} ${If[${_lichr.Value.Equal[${lsChr.First}]},,${dot}]} ${info}${_lichr.Value}\ax
		/if (${_lichr.Value.Equal[ae]}) /varset _out ${_out}:${If[${maChr.Find[stCountAE].Value},${num}${maChr.Find[stCountAE].Value}\ax,${r0}]}
		/if (${_lichr.Value.Equal[def]}) /varset _out ${_out}:${If[${maChr.Find[stCountDefensive].Value},${num}${maChr.Find[stCountDefensive].Value}\ax,${r0}]}
		/if (${_lichr.Value.Equal[fade]}) /varset _out ${_out}:${If[${maChr.Find[stPctFade].Value},${num}${maChr.Find[stPctFade].Value}\ax,${r0}]}
		/if (${_lichr.Value.Equal[spire]}) /varset _out ${_out}:${submenu}
		/if (${_lichr.Value.Equal[manarecover]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaRecovery].Value},${num}${maChr.Find[stPctManaRecovery].Value}\ax,${r0}]}
		/if (${_lichr.Value.Equal[line]}) /varset _out ${_out}:[${num}${maChr.Find[stCurrent].Value}\ax]
		/if (${_lichr.Value.Equal[mb]}) /varset _out ${_out}:${If[${maChr.Find[swManaBurn].Value},${on},${off}]}
		/if (${_lichr.Value.Equal[gather]}) /varset _out ${_out}:${If[${maChr.Find[stPctGather].Value},${num}${maChr.Find[stPctGather].Value}\ax,${r0}]}
		/if (${_lichr.Value.Equal[concussion]}) /varset _out ${_out}:${If[${maChr.Find[stPctConcussion].Value},${num}${maChr.Find[stPctConcussion].Value}\ax,${r0}]}
		/if (${_lichr.Value.Equal[forcetc]}) /varset _out ${_out}:${If[${maChr.Find[swForceTC].Value},${on},${off}]}
		/varset dump ${_lichr.Advance}
	}	
	/varset dump ${out[14, "${_out}"]}

/return



