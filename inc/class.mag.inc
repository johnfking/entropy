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
	
	/call set_data FALSE maData stSpire1																map outer "Fundament: First Spire of the Elements"
	/call set_data FALSE maData stSpire2																map outer "Fundament: First Spire of the Elements"
	/call set_data FALSE maData stSpire3																map outer "Fundament: Third Spire of the Elements"
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
	
	
	/call set_data_map FALSE maChr stGather															PREGEN|1											"${auxna} spell or item to use to recover manna (not rods or aas)"
	/call set_data_map FALSE maChr stPctGather			 										0															"${auxnum} percent to start using gather tricks"

	| current nuke line
	/declare lsNukeLine list outer
	/varset dump ${lsNukeLine.Append[base,fire,magic]}
	/call set_data_map FALSE maChr stCurrent														base
	/call set_data FALSE FALSE stLine																		string outer ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}
	/call set_data_map FALSE maChr stNukeSalvo													PREGEN|1											"${auxna} nuke that relys on area pet count to do more damage"
	/call set_data_map FALSE maChr stNukeServant 												PREGEN|1											"${auxna} the little spam pets mages love to use so much."


	/varset dump ${set_data_timer[FALSE, Force_TC, 6s]}
	/call set_data_map FALSE maChr swForceTC														FALSE													"${auxsw} force using twincast aa/spells as much as possible"
	/call set_data_map FALSE maChr stTC																	PREGEN|1											"${auxna} whatever the hell you want to use for twincast"
	/call set_data_map FALSE maChr stNukeTCFire													PREGEN|1											"${auxna} fire line twincast spell"
	/call set_data_map FALSE maChr stNukeTCMagic												PREGEN|1											"${auxna} magic line twincast spell"
	/call set_data_map FALSE maChr stNukeTCBase													FALSE													"${auxna} base line twincast spell"


/return



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
		/if (${maChr.Find[stPctGather].Value}) {
			/if (${Me.PctMana} <= ${maChr.Find[stPctGather].Value}*.PCTHO) /call check_gather
		}
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle FALSE
	}

	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		| /if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		| /if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Pet
		/if (${check_class_loop[FALSE]}) /return

		/if (${check_class_loop[FALSE]}) /return FALSE
			/call set_combat FALSE
			/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
			/if (!${timer_Check_deBuffs}) /call check_deBuffs FALSE TARGET
			/if (BURN) /call burn_${Me.Class.ShortName}
			/if (PET || SWARM) /call send_pet FALSE TARGET
			/if (SWARM) /call cast_temp_pet FALSE TARGET "${maChr.Find[stNukeServant].Value}"
			| /if (${swAlliance}) /call trigger_alliance TARGET
			/if (DOT) /call cast_dot FALSE lsDoT TARGET
			/if (NUKE) {
				/if (${maChr.Find[swForceTC].Value}) /call force_twincast_${Me.Class.ShortName} TRUE
				/call cast_nuke FALSE lsNuke${stLine} TARGET
			}
			/if (MELEE) /call use_skill_melee TARGET
			/call misc_combat_${Me.Class.ShortName}
			/call check_item_cycle FALSE TARGET			
		}

		/if (AUTO) {	
			| /call Bind_setMAG rods

			/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
			/if (${maChr.Find[stPctGather].Value}) /call check_gather FALSE
		}
	}
/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat_MAG(bool _debug)
	DEBUG \atmisc_combat_MAG\ax()
	
	CHECKEXIT
	GETINPUT
	CHECKTIE
	
	/if (${is_target_dead[FALSE, TARGET]}) /return

	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
	
	| Forece of Elements AA
	/if (${is_ready[FALSE, "Force of Elements"]}) {
		/call voodoo FALSE "Force of Elements" TARGET FALSE
	}
	
	
/return





|***
 * note: Pet nuke
 * use: auto when 3+ pets in range
 ***|
sub cast_nuke_salvo(bool _debug)
	DEBUG \atcast_nuke_salvo\ax()

	/if (${SpawnCount[pet radius ENV]} < 3) /return

	CHECKEXIT
	CHECKTIE
	
	/if (${is_ready[FALSE, "${maChr.Find[stNukeSalvo].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stNukeSalvo].Value}" TARGET FALSE
	}
/return



 
|***
 * note: "One can never have enough socks." - dumbledor
 * use:  when you or your agro whore pet get hurt
 ***|
sub check_heal_MAG(bool _debug, string _type)
	DEBUG \atcheck_heal_MAG\ax(\a-w${_type}\ax)
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"
	
 	/if (!${get_hurt[FALSE, ${_type}]}) {
 		/return FALSE
 	}
 	  	
	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return	
	}  	

	/if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value}) {
		/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Mend Companion"]}) {
				/call voodoo FALSE "Mend Companion" ${Pet.ID} FALSE
			}
		}

		/call cast_list FALSE lsHealPetLine ${Pet.ID}
	}

 	/if (!${get_hurt[FALSE, ${_type}]}) {
 		/return FALSE
 	}
	| heal anything spell
	/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
		/call cast_list FALSE lsHealBaseLine ${hurtID}
	}

	SETHUD FALSE

/return FALSE



|***
 * note: we like fire...
 * use: establish all comp/trigger skills
 *	NOTE: 
 ***| 
sub burn_MAG(bool _debug)
	DEBUG \atburn_MAG\ax()
	
	CHECKEXIT
	CHECKTIE
	/if (${is_ready[FALSE, "Elemental Ward"]}) {
		/call voodoo FALSE "Elemental Ward" 0 FALSE
	}
	
	/if (${Target.PctHPs} > 20) {
		/if (${is_ready[FALSE, "Focus of Arcanum"]}) {
			/call voodoo FALSE "Focus of Arcanum" 0 FALSE
		}
	}
	
	| direct damage bonus
	/if (${Target.PctHPs} > 10) {
		/if (${Me.Buff[Elemental Union].ID}) {
			/if (${is_ready[FALSE, "Heart of Flames"]}) {
				/call voodoo FALSE "Heart of Flames" 0 FALSE
			}
		} else /if (!{Me.Buff[Heart of Flames].ID} && !${Me.AltAbilityReady[Heart of Flames]}) {
			/if (${is_ready[FALSE, "Elemental Union"]}) {
				/call voodoo FALSE "Elemental Union" 0 FALSE
			}
		} else /if (!${Me.AltAbilityReady[Heart of Flames]} && !${Me.Buff[Heart of Flames].ID} && !${Me.AltAbilityReady[Elemental Union]} && !${Me.Buff[Elemental Union].ID}) {
			/call cast_glyph FALSE
		}
	}

	/call cast_AA_spire
	/if (${is_ready[FALSE, "${SilentCast}"]}) {
		/call voodoo FALSE "${SilentCast}" 0 FALSE
	}
	/if (${is_ready[FALSE, "Heart of Stone"]}) {
		/call voodoo FALSE "Heart of Stone" 0 FALSE
	}

	| pet AAs
	/if (PET) {
		/if (${is_ready[FALSE, "Servant of Ro"]}) {
			/call voodoo FALSE "Servant of Ro" TARGET FALSE
		}
		/if (${is_ready[FALSE, "Companion's Fury"]}) {
			/call voodoo FALSE "Companion's Fury" ${Pet.ID} FALSE
		}
	}
	
	| swarm pets
	/if (SWARM) {
		/if (${is_ready[FALSE, "Host of the Elements"]}) {
			/call voodoo FALSE "Host of the Elements" TARGET FALSE
		}
	}

/return



|***
 * note: Control for getting Twincast either spell or Proc
 * use: /call force_twincast_MAG
 ***|
sub force_twincast_MAG(bool _debug)
	DEBUG \atforce_twincast_MAG\ax()
	
	/if (!${maChr.Find[swForceTC].Value} || ${timer_Force_TC}) /return
	/if (${is_target_dead[FALSE, TARGET]}) /return
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	
	/varset dump ${set_data_timer[FALSE, Force_TC, RESTART]}
	
	/if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[Twincast].ID}) {
		/if (${is_ready[FALSE, "${maChr.Find[stTC].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stTC].Value}" 0 FALSE
		}
		/return
	} else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[${maChr.Find[stTC].Value}].ID} && !${Me.Buff[Twincast].ID}) {
		/if (${is_ready[FALSE, "${stNukeTC${maChr.Find[stLine].Value}}"]}) {
			/call voodoo FALSE "${stNukeTC${maChr.Find[stLine].Value}}" TARGET FALSE
		}
	}	
	
/return



|***
 * note: All that you buy, beg, borrow or steal.
 * use: 
 ***|
sub check_gather(bool _debug)
	DEBUG \atcheck_gather\ax()
	
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${Me.Buff[Improved Twincast].ID} || ${Me.Buff[Twincast].ID}) /return FALSE

	| pet consumption for manna		
	/if (${Pet.ID} && ${Me.PctMana} <= 10 ) {
		/if (${is_ready[FALSE, "Elemental Conversion"]}) {
			/call voodoo FALSE "Elemental Conversion" 0 FALSE
		}
	}
	

	/if (${is_ready[FALSE, "${maChr.Find[stGather].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stGather].Value}" 0 FALSE
	}
	
	| stop here to save time if nothing else is available
	/if (${Me.PctMana} > ${maChr.Find[stPctGather].Value}*.PCTHO) /return TRUE
	/if (!${is_ready[FALSE, "${maChr.Find[stGather].Value}"]} && !${Me.AltAbilityReady[Forceful Rejuvenation]}) /return FALSE
		
	| force rejuv if we can proc gather spell again
	/if (!${is_ready[FALSE, "${maChr.Find[stGather].Value}"]}) {
		/if (${is_ready[FALSE, "Forceful Rejuvenation"]}) {
			/call voodoo FALSE "Forceful Rejuvenation" 0 FALSE
		}
	}
	
	/if (${is_ready[FALSE, "${maChr.Find[stGather].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stGather].Value}" 0 FALSE
	}

/return



|***
 * note: mage execution for /coh command from other toons
 * use: /call_of_the_hero [group]
 ***|
#bind control_mag_coh /call_of_the_hero
sub Bind_control_mag_coh(string _sender, string _verbage, bool _debug)
	DEBUG \atBind_control_mag_coh\ax(\a-w${_sender}, ${_verbage}\ax)

  /if (${_verbage.Equal[group]}) {
  	/call set_spell_to FALSE TRUE "Call of the Heroes" ${maEnv.Find[stBuffGem].Value} TRUE
	  /makemevisible

 		/if (${is_ready[FALSE, "Call of the Heroes"]}) {
 			/call voodoo FALSE "Call of the Heroes" 0 FALSE
 		}
  } else {
		/delay 15s ${Me.AltAbilityReady[Call of the Hero]}
	  /makemevisible
 		/if (${is_ready[FALSE, "Call of the Hero"]}) {
 			/call voodoo FALSE "Call of the Hero" ${Spawn[pc ${_sender}].ID} FALSE
 		}
  }
/return


		
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
	} else /if (${_type.Equal[manaclick]}) {
		/if (!${set_control_num_range[FALSE, stPctManaClick, "${_verbage}", 0, 99]}) /return
		
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
			OUT usage${sep}/chr line [\a-wbase\ax|\a-wfire\ax|\a-wmagic\ax]
			/return
		}
		/varset dump ${do_raw_edit[${_debug}, SILENT, stCurrent, "${_verbage.Lower}"]}
		/varset stLine ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}

	| set gather use %	
	} else /if (${_type.Equal[gather]}) {
		/if (!${set_control_num_range[FALSE, stPctGather, "${_verbage}", 0, 99]}) /return
	
	| force twincast attempts
	} else /if (${_type.Equal[forcetc]}) {
		/varset dump ${set_switch_env[${_debug}, swForceTC, TRUE, ${_verbage}]}


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
		/if (${li.Value.Equal[manaclick]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaClick].Value},${num}${maChr.Find[stPctManaClick].Value}\ax,${r0}]}
		/if (${li.Value.Equal[forcetc]}) /varset _out ${_out}:${If[${maChr.Find[swForceTC].Value},${on},${off}]}
		/if (${li.Value.Equal[line]}) /varset _out ${_out}:[${num}${maChr.Find[stCurrent].Value}\ax]
		/if (${li.Value.Equal[gather]}) /varset _out ${_out}:${If[${maChr.Find[stPctGather].Value},${num}${maChr.Find[stPctGather].Value}\ax,${r0}]}

		/varset dump ${li.Advance}
	}	
	OUT /chr ${_out}

	
	
	
/return


