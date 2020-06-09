|***
 * entropy
 * class.mag.inc
 ***|



|***
 * note: primary initilization of all magician non shared data
 * use: i think we've been over this once already
 ***|
sub set_MAG(bool _debug)
	DEBUG \atset_MAG\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of the Elements"
	/call set_data FALSE maData stSynergy																map outer "Conjurer's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Orb of Mastery"
	/call set_data FALSE maData stEpic15																map outer "Staff of Elemental Essence"
	/call set_data FALSE maData stEpic2																	map outer "Focus of Primal Elements"
	/call set_data FALSE maData stFade																	map outer "Drape of Shadows"
	/call set_data_map FALSE maChr stAAUnity														"Thaumaturge's Unity"					"${auxna} class aa unity buff"

	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} class Alliance spell"
	/call set_data_map FALSE maChr stDichotomic													PREGEN|1											"${auxna} Dichotomic spell"
	/call set_data_map FALSE maChr stCoHGroup														PREGEN|1											"${auxna} group target call of the hero"
	/call set_data_map FALSE maChr stCoH																PREGEN|1											"${auxna} single target call of the hero"


	/invoke ${set_data_timer[FALSE, Check_Gather, 3s]}
	/call set_data_map FALSE maChr stGather															PREGEN|1											"${auxna} spell or item to use to recover manna (not rods or aas)"
	/call set_data_map FALSE maChr stPctGather			 										0															"${auxnum} percent to start using gather tricks"

	| current nuke line
	| /declare lsNukeLine list outer
	| /invoke ${lsNukeLine.Append[base,fire,magic]}
	/call set_data_map FALSE maChr stCurrent														base
	| /call set_data FALSE FALSE stLine																		string outer ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}
	| /call set_data_map FALSE maChr stNukeSalvo													PREGEN|1											"${auxna} nuke that relys on area pet count to do more damage"
	/call set_data_map FALSE maChr stNukeServant 												PREGEN|1											"${auxna} the little spam pets mages love to use so much."

| 	/invoke ${set_data_timer[FALSE, Force_TC, 6s]}
| 	/call set_data_map FALSE maChr swForceTC														FALSE													"${auxsw} force using twincast aa/spells as much as possible"
| 	/call set_data_map FALSE maChr stTC																	PREGEN|1											"${auxna} whatever the hell you want to use for twincast"
| 	/call set_data_map FALSE maChr stNukeTCFire													PREGEN|1											"${auxna} fire line twincast spell"
| 	/call set_data_map FALSE maChr stNukeTCMagic												PREGEN|1											"${auxna} magic line twincast spell"
| 	/call set_data_map FALSE maChr stNukeTCBase													FALSE													"${auxna} base line twincast spell"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	/call set_data_map FALSE maChr swCauldron														FALSE													"${auxsw} click cauldron to get prizes or not?"
	/call set_data_map FALSE maChr stCauldron														FALSE													"${auxna} name of the cauldron item you wish to use"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: main magician routine
 * use:
 ***|
sub main_MAG(bool _debug)
	DEBUG \atmain_MAG\ax()
	GETINPUT
	ISMEDEAD
	CHECKTIE

	/if (AUTO) {
		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		/if (${maChr.Find[stPctGather].Value} && !${timer_Check_Gather}) /call check_gather_${Me.Class.ShortName} ${maDebug.Find[gather].Value}
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
		/if (${maChr.Find[swCauldron].Value}) /call check_cauldron  ${maDebug.Find[cauldron].Value}
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return

		/if (${check_class_loop[FALSE]}) /return FALSE
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			CHECKDEFENSE
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[temp].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			| /if (${swAlliance}) /call trigger_alliance TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) {
				/if (${maChr.Find[swForceTC].Value}) /call force_twincast_${Me.Class.ShortName} TRUE
				/call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			}
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		}

		/if (AUTO) {
			| /call Bind_setMAG rods

			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			/if (${maChr.Find[stPctGather].Value} && !${timer_Check_Gather}) /call check_gather_${Me.Class.ShortName} ${maDebug.Find[gather].Value}
		}
	}
/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat_MAG(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_MAG\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat_MAG

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) /return FALSE

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

	| Forece of Elements AA
	/if (${is_ready[${_debug}, "Force of Elements"]}) {
		/call voodoo ${_debug} "Force of Elements" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: "One can never have enough socks." - dumbledor
 * use:  when you or your agro whore pet get hurt
 ***|
sub check_heal_MAG(bool _debug, string _type)
	DEBUG \atcheck_heal_MAG\ax(\a-w${_type}\ax)
	FLOW check_heal_MAG ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return
	}

	/if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value}) {
		/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "Mend Companion"]}) {
				/call voodoo ${_debug} "Mend Companion" ${Pet.ID} FALSE
			}
		}

		/call cast_cycle_heal ${_debug} ${Pet.ID} healminion
	}

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}
	| heal anything spell
	/call cast_cycle_heal ${_debug} ${hurtID} healsingle

	SETHUD EMPTY

/return TRUE



|***
 * note: we like fire...
 * use: establish all comp/trigger skills
 *	NOTE:
 ***|
sub burn_MAG(bool _debug, int _tmpID)
	DEBUG \atburn_MAG\ax(\a-w${_tmpID}\ax)
	FLOW burn_MAG

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) /return FALSE

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

	/if (${is_ready[${_debug}, "Elemental Ward"]}) {
		/call voodoo ${_debug} "Elemental Ward" 0 FALSE
	}

	/if (${Target.PctHPs} > 20) {
		/if (${is_ready[${_debug}, "Focus of Arcanum"]}) {
			/call voodoo ${_debug} "Focus of Arcanum" 0 FALSE
		}
	}

	| direct damage bonus
	/if (${Target.PctHPs} > 10) {
		/if (${Me.Buff[Elemental Union].ID}) {
			/if (${is_ready[${_debug}, "Heart of Flames"]}) {
				/call voodoo ${_debug} "Heart of Flames" 0 FALSE
			}
		} else /if (!${Me.Buff[Heart of Flames].ID} && !${Me.AltAbilityReady[Heart of Flames]}) {
			/if (${is_ready[${_debug}, "Elemental Union"]}) {
				/call voodoo ${_debug} "Elemental Union" 0 FALSE
			}
		} else /if (!${Me.AltAbilityReady[Heart of Flames]} && !${Me.Buff[Heart of Flames].ID} && !${Me.AltAbilityReady[Elemental Union]} && !${Me.Buff[Elemental Union].ID}) {
			/call cast_glyph ${maDebug.Find[glyph].Value}
		}
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "Silent Casting"]}) {
		/call voodoo ${_debug} "Silent Casting" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Heart of Stone"]}) {
		/call voodoo ${_debug} "Heart of Stone" 0 FALSE
	}

	| pet AAs
	/if (PET) {
		/if (${is_ready[${_debug}, "Servant of Ro"]}) {
			/call voodoo ${_debug} "Servant of Ro" TARGET FALSE
		}
		/if (${is_ready[${_debug}, "Companion's Fury"]}) {
			/call voodoo ${_debug} "Companion's Fury" ${Pet.ID} FALSE
		}
	}

	| swarm pets
	/if (SWARM) {
		/if (${is_ready[${_debug}, "Host of the Elements"]}) {
			/call voodoo ${_debug} "Host of the Elements" TARGiiET FALSE
		}
	}

/return TRUE



|***
 * note: Control for getting Twincast either spell or Proc
 * use: /call force_twincast_MAG

sub force_twincast_MAG(bool _debug)
	DEBUG \atforce_twincast_MAG\ax()
	FLOW force_twincast_MAG

	/if (!${maChr.Find[swForceTC].Value} || ${timer_Force_TC}) /return
	/if (${target[isdead, TARGET]}) /return FALSE

	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/invoke ${set_data_timer[FALSE, Force_TC, RESTART]}

	/if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[Twincast].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stTC].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stTC].Value}" 0 FALSE
		}
		/return
	} else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[${maChr.Find[stTC].Value}].ID} && !${Me.Buff[Twincast].ID}) {
		/if (${is_ready[${_debug}, "${stNukeTC${maChr.Find[stLine].Value}}"]}) {
			/call voodoo ${_debug} "${stNukeTC${maChr.Find[stLine].Value}}" TARGET FALSE
		}
	}

/return TRUE
 ***|




|***
 * note: All that you buy, beg, borrow or steal.
 * use:
 ***|
sub check_gather_MAG(bool _debug)
	DEBUG \atcheck_gather_${Me.Class.ShortName}\ax()
	FLOW check_gather_MAG (1)

	/invoke ${set_data_timer[${_debug}, Check_Gather, RESTART]}

	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) {
		DEBUG ${sep} manna: ${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${Me.Buff[Improved Twincast].ID} || ${Me.Buff[Twincast].ID}) {
		DEBUG ${sep}twincast running. get manna later
		/return FALSE
	}

	| pet consumption for manna
	/if (${Pet.ID} && ${Me.PctMana} <= 10 ) {
		/if (${is_ready[${_debug}, "Elemental Conversion"]}) {
			/call voodoo ${_debug} "Elemental Conversion" 0 FALSE
		}
	}

	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) {
		DEBUG ${sep} manna: ${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO
		/return FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stGather].Value}" 0 FALSE
	}


	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) {
		/return FALSE
	}

	/if (!${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]} && !${Me.AltAbilityReady[Forceful Rejuvenation]}) {
		/return FALSE
	}

	| force rejuv if we can proc gather spell again
	/if (!${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
		/if (${is_ready[${_debug}, "Forceful Rejuvenation"]}) {
			/call voodoo ${_debug} "Forceful Rejuvenation" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stGather].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stGather].Value}" 0 FALSE
	}

/return TRUE



|***
 * note: mage execution for /coh command from other toons
 * use: /call_of_the_hero [group]
 ***|
#bind control_mag_coh /call_of_the_hero
sub Bind_control_mag_coh(string _sender, string _verbage, bool _debug)
	DEBUG \atBind_control_mag_coh\ax(\a-w${_sender}, ${_verbage}\ax)
	FLOW Bind_control_mag_coh

	| output control map
	/if (${_sender.Equal[aux]}) {
		/call Bind_command_aux coh
		/return
	}

  /if (${_verbage.Equal[group]}) {
  	/call set_spell_to ${_debug} TRUE "Call of the Heroes" ${maEnv.Find[stBuffGem].Value} TRUE
	  /makemevisible

 		/if (${is_ready[${_debug}, "Call of the Heroes"]}) {
 			/call voodoo ${_debug} "Call of the Heroes" 0 FALSE
 		}
  } else {
		/delay 15s ${Me.AltAbilityReady[Call of the Hero]}
	  /makemevisible
 		/if (${is_ready[${_debug}, "Call of the Hero"]}) {
 			/call voodoo ${_debug} "Call of the Hero" ${Spawn[pc ${_sender}].ID} FALSE
 		}
  }
/return TRUE



|***
 * note: summon cauldron item
 * use: auto on timer
 ***|
sub check_cauldron(bool _debug)
	DEBUG \atcheck_cauldron\ax()
	FLOW check_cauldron

	/if (!${Bool[${maChr.Find[stCauldron].Value}]}) {
		OUT You need to set up your cauldron if you want to use it.
		/invoke ${set_switch_env[${_debug}, swCauldron, TRUE, FALSE]}
		/return FALSE
	}

	/if (!${is_ready[${_debug}, "${maChr.Find[stCauldron].Value}"]}) {
		/invoke ${set_timer[${_debug}, Check_Cauldron, ${Math.Calc[${FindItem[${maChr.Find[stCauldron].Value}].Timer.TotalMinutes}]}m]}
		/return FALSE
	}

	CHECKEXIT
	CHECKTIE

	/call voodoo ${_debug} "${maChr.Find[stCauldron].Value}" 0 FALSE

	/while (${Cursor.ID}) {
		/call check_cursor ${_debug} CLEAR
		/delay 5
	}

	/invoke ${set_timer[${_debug}, Check_Cauldron, ${Math.Calc[${FindItem[${maChr.Find[stCauldron].Value}].Timer.TotalMinutes} + 1]}m]}

/return TRUE



|***
 * note: mage specific control
 * use: /chr
 ***|
sub set_control_MAG(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_Mag\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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


	| set ae on/off / count
	} else /if (${_type.Equal[cauldron]}) {

		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swCauldron, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[item]}) {
			/invoke ${do_raw_edit[TRUE, SILENT, stCauldron, "${_silent}"]}
		}

		/varset _out ${info}use\ax:${If[${maChr.Find[swCauldron].Value},${on},${off}]}
		/varset _out ${_out} ${dot} ${info}item\ax:[${If[${Bool[${maChr.Find[stCauldron].Value}]},${num}${maChr.Find[stCauldron].Value}\ax,${off}]}]
		/invoke ${out[12, "/chr cauldron ${_out}"]}
		/return

	| set gather use %
	} else /if (${_type.Equal[gather]}) {
		/if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return

	| force twincast attempts
	| } else /if (${_type.Equal[forcetc]}) {
	| 	/invoke ${set_switch_env[${_debug}, swForceTC, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE


