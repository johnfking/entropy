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

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

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
	/invoke ${lsNukeLine.Append[base,fire,ice,magic]}
	/call set_data_map FALSE maChr stCurrent														base
	/call set_data FALSE FALSE stLine																		string outer ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}

	/invoke ${set_data_timer[FALSE, Force_TC, 6s]}
	/call set_data_map FALSE maChr stTC																	PREGEN|1											"${auxna} whatever the hell you want to use for twincast"
	/call set_data_map FALSE maChr swForceTC														FALSE													"${auxsw} force using twincast aa/spells as much as possible"
	/call set_data_map FALSE maChr stNukeTCFire													PREGEN|1											"${auxna} fire line twincast spell"
	/call set_data_map FALSE maChr stNukeTCIce													PREGEN|1											"${auxna} ice line twincast spell"
	/call set_data_map FALSE maChr stNukeTCMagic												PREGEN|1											"${auxna} magic line twincast spell"
	/call set_data_map FALSE maChr stNukeTCBase													FALSE													"${auxna} base line twincast spell"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



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
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call set_combat FALSE
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (MELEE) /call do_melee FALSE TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[temp].Value} TARGET "${maChr.Find[stTempPet].Value}"
			/if (${maCC.Find[swAlliance].Value}) /call trigger_alliance TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) {
				/if (${maChr.Find[swForceTC].Value}) {
					/call force_twincast_${Me.Class.ShortName} ${maDebug.Find[twincast].Value} TARGET
				}
				/call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			}
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET

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
 * use: /call misc_combat_${Me.Class.ShortName} DEBUG TARGETID
 ***|
sub misc_combat_WIZ(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_WIZ\ax(\a-w${_tmpID}\ax)

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
		/if (${is_ready[${_debug}, "Concussive Intuition"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value}) /{
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

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${Me.Buff[Improved Twincast].ID} || ${Me.Buff[${maChr.Find[stTC].Value}].ID}) /return

	/if (!${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
		/if (${is_ready[${_debug}, "Forceful Rejuvenation"]}) {
			/call voodoo ${_debug} "Forceful Rejuvenation" 0 FALSE
		}
		/delay 5
	}
	/if (${Me.PctMana} < ${maChr.Find[stPctGather].Value}*.PCTHO) {
		/if (${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stGather].Value}" 0 FALSE
			}
		}
		/if (${Me.AltAbilityReady[Harvest of Druzzil]}) {
			/if (${is_ready[${_debug}, "Harvest of Druzzil"]}) {
				/call voodoo ${_debug} "Harvest of Druzzil" 0 FALSE
			}
		}
	}

/return TRUE



|***
 * note: healing. no. you dont get any. suck it up butercup
 * use: /call check_heal_${Me.Class.ShortName}
 ***|
sub check_heal_WIZ(bool _debug, string _type)
	DEBUG \atcheck_heal_WIZ\ax(\a-w${_type}\ax)


	/if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) /return FALSE

	ISMEDEAD
	CHECKEXIT

	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}
	/call cast_cycle_heal ${_debug} ${hurtID}

/return TRUE



|***
 * note: Wizard burn routines
 * use: bug guns?
 ***|
sub burn_WIZ(bool _debug, int _tmpID)
	DEBUG \atburn_WIZ\ax(\a-w${_tmpID}\ax)

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
	} else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.AltAbilityReady[Improved Twincast]} && ${is_ready[${_debug}, "${maChr.Find[stTC].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stTC].Value}" 0 FALSE
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
				/call voodoo ${_debug} "Mana Burn" TARGET FALSE
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

/return TRUE



|***
 * note: Control for getting Twincast either spell or Proc
 * use: /call force_twincast_wiz DEBUG
 ***|
sub force_twincast_WIZ(bool _debug, int _tmpID)
	DEBUG \atforce_twincast_WIZ\ax(\a-w${_tmpID}\ax)

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
		/if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return

	| set concussion use %
	} else /if (${_type.Equal[concussion]}) {
		/if (!${set_control_num_range[${_debug}, stPctConcussion, "${_verbage}", 0, 99]}) /return

	| set AA fade use %
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return


	} else /if (${_type.Equal[line]}) {
		/if (!${lsNukeLine.Contains[${_verbage.Lower}]}) {
			OUT usage${sep}/chr line [\a-wbase\ax|\a-wfire\ax|\a-wice\ax|\a-wmagic\ax]
			/return
		}
		/invoke ${do_raw_edit[${_debug}, SILENT, stCurrent, "${_verbage.Lower}"]}
		/varset stLine ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}


	| set gather use %
	} else /if (${_type.Equal[gather]}) {
		/if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return

	| use manaburn
	} else /if (${_type.Equal[mb]}) {
		/invoke ${set_switch_env[${_debug}, swManaBurn, TRUE, ${_verbage}]}

	| force twincast attempts
	} else /if (${_type.Equal[forcetc]}) {
		/invoke ${set_switch_env[${_debug}, swForceTC, TRUE, ${_verbage}]}


	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE




