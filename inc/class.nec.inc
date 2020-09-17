|***
 * entropy
 * class.nec.inc
 ***|



|***
 * note: primary initilization of all necromancer non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
	DEBUG \atclass_init\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of Necromancy"
	/call set_data FALSE maData stSynergy																map outer "Defiler's Synergy"
	/call set_data FALSE maData stFade																	map outer "A Hole in Space"
	/call set_data FALSE maData stCompleteHeal													map outer "Complete Heal"
	/call set_data_map ${_debug} maChr stAAUnity												"Mortifier's Unity"					"${auxna} class aa unity buff"


	/call set_data_map ${_debug} maChr stNukeServant 										PREGEN|1											"${auxna} spam nuke pet spell"
 	/call set_data_list ${_debug} maChr lsHealPetMend										PREGEN|3											"${auxli} mending spells for pet"
 	/call set_data_map ${_debug} maChr stAlliance 											PREGEN|1											"${auxna} alliance spell"
  /call set_data_map ${_debug} maChr stDichotomic 										PREGEN|1											"${auxna} dichotomic spell"

	/invoke ${set_data_timer[${_debug}, Check_Blood, 8s]}
	/call set_data_map ${_debug} maChr swblood													FALSE													"${auxsw} use  blood spell"
	/call set_data_map ${_debug} maChr stBlood													PREGEN|1											"${auxna} name of current call for blood spell"

	/call set_data_map ${_debug} maChr stPctBloom				 								0															"${auxnum} % of manna you want to use bloom at"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return



|***
 * note: Main Necro Routine
 * use:
 ***|
sub class_main(bool _debug)
	DEBUG \atclass_main\ax()

	/if (AUTO && !ENGINE3) {
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) {
			/call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
		}
		/if (PET || SWARM) {
			/call send_pet ${maDebug.Find[sic].Value} TARGET
		}
	} else /if (AUTO && ENGINE3) {
		/if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
			/call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
		}
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

		/if (VALID && !ENGINE3) {
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			CHECKDEFENSE
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
			/call check_heal_routines
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[minion].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			| /if (${maCC.Find[swAlliance].Value}) /call trigger_alliance TARGET
			/if (${maChr.Find[swblood].Value} && !${timer_Check_Blood}) /call cast_blood ${maDebug.Find[blood].Value} TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/call misc_combat ${maDebug.Find[misc].Value} TARGET
			/call check_item_cycle ${maDebug.Find[castitem].Value} TARGET

		} else /if (VALID && ENGINE3) {
			/if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
				/call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}

		/if (AUTO && !ENGINE3) {
			/if (${maChr.Find[stPctManaRecovery].Value}) {
				/call do_mana_recovery ${maDebug.Find[manarecover].Value}
			}
			/if (${Me.PctMana} <= ${maChr.Find[stPctBloom].Value}*.PCTHO) {
				/call cast_bloom ${maDebug.Find[bloom].Value}
			}
			CHECKFADE
		} else /if (AUTO && !ENGINE3) {
			/if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
				/call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}
	}

/return TRUE



|***
 * note: why are we healing the dead?
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

		/if (${Select[${_type},self]} && ${hurtCount}) {

			| aa Dying grasp
			/if (${Me.PctHPs} <= ${${Me.Class.ShortName}Point}*.PCTHO) {
				/if (${is_ready[${_debug}, "Dying Grasp"]}) {
					/call voodoo ${_debug} "Dying Grasp" ${_type} FALSE
				}
			}

		}

		/if (${Select[${_type},pet]} && ${hurtCount}) {
			/if (${is_ready[${_debug}, "Mend Companion"]}) {
				/call voodoo ${_debug} "Mend Companion" ${Pet.ID} FALSE
			}
		}
	}

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}

	/call cast_cycle_heal ${_debug} ${hurtID} single

	SETHUD EMPTY

/return tRUE



|***
 * note: the extra stuff
 * use: normal routine
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
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	| aa wake the dead
	/if (SWARM) {
		| wake the dead
		/if (${SpawnCount[corpse radius ${Spell[Wake the Dead].MyRange}]} > 3) {
			/if (${is_ready[${_debug}, "Wake the Dead"]} && ${Target.PctHPs} > 15) {
				/call voodoo ${_debug} "Wake the Dead" ${_tmpID} FALSE
			}
		}
		| aa swarm of decay
		/if (${is_ready[${_debug}, "Swarm of Decay"]}) {
			/call voodoo ${_debug} "Swarm of Decay" ${_tmpID} FALSE
		}
		| aa Rise of bones
		/if (${is_ready[${_debug}, "Rise of Bones"]}) {
			/call voodoo ${_debug} "Rise of Bones" ${_tmpID} FALSE
		}
	}

	/if (${is_ready[${_debug}, "Hand of Death"]} && !${Me.Song[Funeral Pyre].ID}) {
		/call voodoo ${_debug} "Hand of Death" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: “You should never attack a necromancer in a cemetery; it’s like chasing Rambo into a building full of loaded guns." --Laurell K. Hamilton, Dead Ice
 * use: burn the dead
 ***|
sub burn(bool _debug, int _tmpID)
	DEBUG \atburn\ax(\a-w${_tmpID}\ax)
	FLOW burn=

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

	| /if (${Me.Song[Gift of Deathly Resolve].ID}) /call cast_DOT FALSE TARGET

	| /if (!${Me.Song[Gift of Deathly Resolve].ID} && ${Me.Buff[Deathly Resolve].ID}) /call cast_nuke FALSE TARGET

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	| Pet Burnage
	/if (PET && ${Me.Pet.ID}) {
		/if (${is_ready[${_debug}, "Companion's Fury"]}) {
			/call voodoo ${_debug} "Companion's Fury" ${Me.Pet.ID} FALSE
		}
	}

	/if (${Target.PctHPs} > 15 && ${Me.PctHPs} > 90) {
		/if (${is_ready[${_debug}, "Life Burn"]}) {
			/call voodoo ${_debug} "Life Burn" ${_tmpID} FALSE
		}
	}

	/if (!${Me.Buff[Spire of Necromancy].ID}) {
		/if (${is_ready[${_debug}, "Embalmer's Carapace"]}) {
			/call voodoo ${_debug} "Embalmer's Carapace" ${_tmpID} FALSE
		}
	}

	/if (${is_ready[${_debug}, "Funeral Pyre"]} && !${Me.Song[Hand of Death].ID}) {
		/call voodoo ${_debug} "Funeral Pyre" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "Gift of Deathly Resolve"]}) {
		/call voodoo ${_debug} "Gift of Deathly Resolve" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Mercurial Torment"]}) {
		/call voodoo ${_debug} "Mercurial Torment" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Focus of Arcanum"]}) {
		/call voodoo ${_debug} "Focus of Arcanum" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Heretic's Twincast"]}) {
		/call voodoo ${_debug} "Heretic's Twincast" 0 FALSE
	}


	/call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 * note: casts call of blood line of spells
 * use: /call cast_blood DEBUG [ID]
 ***|
sub cast_blood(bool _debug, int _tmpID)
	DEBUG \atcast_blood\ax(\a-w${_tmpID}\ax)
	FLOW cast_blood

	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/invoke ${set_data_timer[${_debug}, Check_Blood, RESTART]}
	/if (${Me.Song[Chaotic Power].ID}) /return FALSE

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}


	/if (${is_ready[${_debug}, "${maChr.Find[stBlood].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stBlood].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: We want Mana
 * use: /chr bloom #
 ***|
sub cast_bloom(bool _debug)
	DEBUG \atcast_bloom\ax()
	FLOW cast_bloom

	/if (${Zone.ID} == ${Me.BoundLocation[0].ID} || SAFEZONE) {
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${is_ready[${_debug}, "Death Bloom"]}) {
		/call voodoo ${_debug} "Death Bloom" 0 FALSE
	}

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
	DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

	| use blood nuke line
	/if (${_type.Equal[blood]}) {
		/invoke ${set_switch_env[${_debug}, swblood, TRUE, ${_verbage}]}

		/if (${_verbage.Equal[name]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stBlood, "${_verbage2}"]}
		} else /if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swblood, TRUE, ${_verbage2}]}
		}
		/invoke ${maChrControl.Clear}
		/invoke ${maChrControl.Add[name,stBlood|br]}
		/invoke ${maChrControl.Add[use,swblood]}
		/invoke ${set_control_output[${_debug}, maHeal, "chr blood", maChrControl]}
		/return TRUE

	| use bloom mana recovery
	} else /if (${_type.Equal[bloom]}) {
		/invoke ${set_switch_env[${_debug}, stPctBloom, TRUE, ${_verbage}]}

	}

	/call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
	/if (${Macro.Return.Equal[SKIP]}) /return FALSE

	/if (${_verbage2.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE