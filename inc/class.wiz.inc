|***
 * entropy
 * class.wiz.inc
 ***|



|***
 * note: primary initilization of all wizard non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
	DEBUG \atclass_init\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data ${_debug} maData stSpire															map outer "Spire of Arcanum"
	/call set_data ${_debug} maData stSynergy														map outer "Evoker's Synergy"
	/call set_data ${_debug} maData stFade															map outer "A Hole in Space"
	/call set_data ${_debug} maData stCompleteHeal											map outer "Complete Heal"
	/call set_data_map ${_debug} maChr stAAUnity												"Etherealist's Unity"					"${auxna} class aa unity buff"

	/call set_data_map ${_debug} maChr stPctGather											0															"${auxnum} % of your mana to start using gather spells and AAs"
	/call set_data_map ${_debug} maChr stGather													PREGEN|1											"${auxna} name of mana gather spell (\a-wNOT THE AA\ax)"
	/call set_data_map ${_debug} maChr stTempPet												PREGEN|1											"${auxna} temp flame'ie pig sticker pet spell"

	/call set_data_map ${_debug} maChr swManaBurn		 										FALSE													"${auxsw} use aa manaburn during burns"

	/call set_data_map ${_debug} maChr stConcussion											PREGEN|1											"${auxna} name of anti agro spell cause you nuke to damn much and dont let tank get agro"
	/call set_data_map ${_debug} maChr stPctConcussion									0															"${auxnum} % of your agro you want to try and cover your ass at"

	| current nuke line
	/call	set_data ${_debug} FALSE lsNukeLine														list outer base|fire|ice|magic
	/call set_data_map ${_debug} maChr stCurrent												base
	/call set_data ${_debug} FALSE stLine																string outer ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}

	/invoke ${set_data_timer[FALSE, Force_TC, 6s]}
	/call set_data_map ${_debug} maChr stTC															PREGEN|1											"${auxna} whatever the hell you want to use for twincast"
	/call set_data_map ${_debug} maChr swForceTC												FALSE													"${auxsw} force using twincast aa/spells as much as possible"
	/call set_data_map ${_debug} maChr stNukeTCFire											PREGEN|1											"${auxna} fire line twincast spell"
	/call set_data_map ${_debug} maChr stNukeTCIce											PREGEN|1											"${auxna} ice line twincast spell"
	/call set_data_map ${_debug} maChr stNukeTCMagic										PREGEN|1											"${auxna} magic line twincast spell"
	/call set_data_map ${_debug} maChr stNukeTCBase											FALSE													"${auxna} base line twincast spell"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE








Arcane Whisper
Mind Crash
Concussive Intuition
Harvest of Druzzil
Silent Casting
Frenzied Devastation
Focus of Arcanum
Improved Twincast
Fury of the Gods
Arcane Destruction
Arcane Fury
Arcane Fury
Call of Xuzl



Arcane Whisper
Mind Crash
Concussive Intuition
Harvest of Druzzil
Silent Casting
Frenzied Devastation
Focus of Arcanum
Improved Twincast
Fury of the Gods
Arcane Destruction
Arcane Fury
Arcane Fury
Call of Xuzl




|***
 * note: main wizzeurd routine
 * use:
 ***|
sub class_main(bool _debug)
	DEBUG \atclass_main\ax()

	/if (AUTO && !ENGINE3) {
	} else /if (AUTO && ENGINE3) {
		/if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
			/call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
		}
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/call check_heal_routines
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

		/if (VALID && !ENGINE3) {
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[temp].Value} TARGET "${maChr.Find[stTempPet].Value}"
			/if (${maCC.Find[swAlliance].Value}) /call trigger_alliance TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) {
				/if (${maChr.Find[swForceTC].Value}) {
					/call force_twincast ${maDebug.Find[twincast].Value} TARGET
				}
				/call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			}
			/call misc_combat ${maDebug.Find[misc].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		} else /if (VALID && ENGINE3) {
			/if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
				/call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}

		}

		/if (AUTO && !ENGINE3) {
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			/if (${Me.PctMana} <= ${maChr.Find[stPctGather].Value}*.PCTHO) /call check_harvest ${maDebug.Find[mannarecover].Value}
			CHECKFADE
		} else /if (AUTO && !ENGINE3) {
			/if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
				/call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}
	}

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName} DEBUG TARGETID
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

	/if (ENGINE2) {
		/call cast_cycle_miscdps ${maDebug.Find[misc].Value} ${_tmpID}
		/return TRUE
	}

	| agro control
	/if (AGRO) {
	} else /if (!AGRO) {
		/if (${is_ready[${_debug}, "Arcane Whisper"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value}) {
			/call voodoo ${_debug} "Arcane Whisper" ${_tmpID} FALSE
		}
		/if (${is_ready[${_debug}, "Mind Crash"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value} && ${lsZoneNamed.Contains[${Spawn[TARGET].DisplayName}]}) {
			/call voodoo ${_debug} "Mind Crash" ${_tmpID} FALSE
		}
		/if (${is_ready[${_debug}, "Concussive Intuition"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value}) {
			call voodoo ${_debug} "Concussive Intuition" ${_tmpID} FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stConcussion].Value}"]} && ${Me.PctAggro} > ${maChr.Find[stPctConcussion].Value}) {
			/call voodoo ${_debug} "${maChr.Find[stConcussion].Value}" ${_tmpID} FALSE
		}
	}

/return TRUE



|***
 * note: But you say I'm a dog when I leave you fucked and deserted.
 * use: /call check_harvest DEBUG
 ***|
sub check_harvest(bool _debug)
	DEBUG \atcheck_harvest\ax()
	FLOW check_harvest

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${Me.Buff[Improved Twincast].ID}) {
		/return FALSE
	}

	/if (${Bool[${maChr.Find[stTC].Value}]} && ${Me.Buff[${maChr.Find[stTC].Value}].ID}) {
		/return FALSE
	}

	/if (!${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
		/if (${is_ready[${_debug}, "Forceful Rejuvenation"]}) {
			/call voodoo ${_debug} "Forceful Rejuvenation" 0 FALSE
			/delay 5
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stGather].Value}" 0 FALSE
		/return TRUE
	}

	/if (${is_ready[${_debug}, "Harvest of Druzzil"]}) {
		/call voodoo ${_debug} "Harvest of Druzzil" 0 FALSE
		/return TRUE
	}

/return TRUE



|***
 * note: healing. no. you dont get any. suck it up butercup
 * use: /call check_heal
 ***|
sub check_heal(bool _debug, string _type)
	DEBUG \atcheck_heal\ax(\a-w${_type}\ax)
	FLOW check_heal ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return TRUE
	}

	/if (!SAFEZONE) {

		| group healing
		/if (${Select[${_type},group]} && ${hurtCount}) {

			| group heal
			/if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
				/call cast_cycle_heal ${_debug} 0 group
 			}
		}

	}

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}

	/call cast_cycle_heal ${_debug} ${hurtID} single

	SETHUD EMPTY

/return TRUE



|***
 * note: Wizard burn routines
 * use: bug guns?
 ***|
sub burn(bool _debug, int _tmpID)
	DEBUG \atburn\ax(\a-w${_tmpID}\ax)
	FLOW burn

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

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}

	| shhhhh..
	/if (${is_ready[${_debug}, "Silent Casting"]}) {
		/call voodoo ${_debug} "Silent Casting" 0 FALSE
	}

	| next 45 spells +51% crit rate
	/if (${is_ready[${_debug}, "Frenzied Devastation"]}) {
		/call voodoo ${_debug} "Frenzied Devastation" 0 FALSE
	}
	| increase chance to land
	/if (${is_ready[${_debug}, "Focus of Arcanum"]}) {
		/call voodoo ${_debug} "Focus of Arcanum" 0 FALSE
	}


	| alternate twincast
	/if (!${Me.Buff[${maChr.Find[stTC].Value}].ID}) {
		/if (${is_ready[${_debug}, "Improved Twincast"]}) {
			/call voodoo ${_debug} "Improved Twincast" 0 FALSE
		}
	} else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.AltAbilityReady[Improved Twincast]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stTC].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stTC].Value}" 0 FALSE
		}
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	| boost spell type damage
	/if (${is_ready[${_debug}, "Fury of the Gods"]}) {
		/call voodoo ${_debug} "Fury of the Gods" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Arcane Destruction"]}) {
		/call voodoo ${_debug} "Arcane Destruction" 0 FALSE
	}

	| increase damage
	/if (${is_ready[${_debug}, "Arcane Fury"]}) {
		/call voodoo ${_debug} "Arcane Fury" 0 FALSE
	}

	| aww baby
	/if (${maChr.Find[swManaBurn].Value}) {
		/invoke ${set_target[TARGET]}
		/if (!${Target.Buff[Mana Burn].ID}) {
			/if (${is_ready[${_debug}, "Mana Burn"]}) {
				/call voodoo ${_debug} "Mana Burn" ${_tmpID} FALSE
			}
		}
	}

	| swarm pet
	/if (SWARM) {
		/if (${is_ready[${_debug}, "Call of Xuzl"]}) {
			/call voodoo ${_debug} "Call of Xuzl" ${_tmpID} FALSE
		}
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 * note: Control for getting Twincast either spell or Proc
 * use: /call force_twincast DEBUG
 ***|
sub force_twincast(bool _debug, int _tmpID)
	DEBUG \atforce_twincast\ax(\a-w${_tmpID}\ax)
	FLOW force_twincast

	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/invoke ${set_data_timer[${_debug}, Force_TC, 6s]}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}


	/if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[Twincast].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stTC].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stTC].Value}" 0 FALSE
		}
		/return

	} else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[${maChr.Find[stTC].Value}].ID} && !${Me.Buff[Twincast].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stNukeTC${stLine}].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stNukeTC${stLine}].Value}" ${_tmpID} FALSE
		}
	}

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
	DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

	| set concussion use %
	/if (${_type.Equal[concussion]}) {
		/if (!${set_control_num_range[${_debug}, stPctConcussion, "${_verbage}", 0, 99]}) /return FALSE

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return FALSE

	| set gather use %
	} else /if (${_type.Equal[gather]}) {
		/if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return FALSE

	| use manaburn
	} else /if (${_type.Equal[mb]}) {
		/invoke ${set_switch_env[${_debug}, swManaBurn, TRUE, ${_verbage}]}

	| force twincast attempts
	} else /if (${_type.Equal[forcetc]}) {
		/invoke ${set_switch_env[${_debug}, swForceTC, TRUE, ${_verbage}]}

	}

	/call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
	/if (${Macro.Return.Equal[SKIP]}) /return FALSE

	/if (${_verbage2.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE




