|***
 * entropy
 * class.rng.inc
 ***|




|***
 * note: primary initilization of all ranger non shared data
 * use: not for you
 ***|
sub set_RNG(bool _debug)
	DEBUG \atset_RNG\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of the Pathfinders"
	/call set_data FALSE maData stSynergy																map outer "Outrider's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Earthcaller"
	/call set_data FALSE maData stEpic15																map outer "Heartwood Blade"
	/call set_data FALSE maData stEpic2																	map outer "Aurora, the Heartwood Blade"
	/call set_data FALSE maData stFade																	map outer "Cover Tracks"
	/call set_data_map FALSE maChr stAAUnity														"Wildstalker's Unity (Azia)"	"${auxna} class aa unity buff"

	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance spell"

	| spam summer nuke
	/invoke ${set_data_timer[FALSE, Summer_Nuke, 1s]}
	/call set_data_map FALSE maChr stSummer		 													PREGEN|1											"${auxna} summer line nuke spell"
	/call set_data_map FALSE maChr swSummer															FALSE													"${auxsw} use summer nuke line spell"

	| rest disc
 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr stHiatus															PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"

	| heals
 	/call set_data_list FALSE maChr lsHealToT		 												PREGEN|3											"${auxli} targets target healing line of spells"
  /call set_data_map FALSE maChr stHealDesperate											PREGEN|1											"${auxna} fast cast heal"
  /call set_data_map FALSE maChr stHealFont														PREGEN|1											"${auxna} normal heal"
	/call set_data_map FALSE maChr stDiscHeal		 												PREGEN|1											"${auxna} group heal disc"

	| bow disc
	/call set_data_map FALSE maChr stDiscRange			 										PREGEN|1											"${auxna} ranged DPS disc"

	| melee disc
	/call set_data_map FALSE maChr stDiscMelee			 										PREGEN|1											"${auxna} melee DPS disc"

	| agro/anti agro kicks
	/call set_data_map FALSE maChr stDiscJolt			 											PREGEN|1											"${auxna} hate reduction disc"
	/call set_data_map FALSE maChr stDiscEnrage		 											PREGEN|1											"${auxna} hate increase disc"

	| ae melee disc
	/call set_data_map FALSE maChr stDiscMeleeAE		 										PREGEN|1											"${auxna} ae melee disc"

	| focused melee disc
	/call set_data_map FALSE maChr stDiscMeleeSingle			 							PREGEN|1											"${auxna} single melee disc"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE





|***
 * note: Main Ranger Routine
 * use:
 ***|
sub main_RNG(bool _debug)
	DEBUG \atmain_RNG\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ

	/while (${check_combat_status[FALSE]}) {
		/call check_heal_routines
		/if (${check_class_loop[FALSE]}) /return FALSE

		/if (VALID) {
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/call check_heal_routines
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value}
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/if (${maChr.Find[swSummer].Value} && !${timer_Summer_Nuke}) /call cast_nuke_summer ${maDebug.Find[summer].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
			| /if (${swAlliance}) /call trigger_alliance TARGET

		}

		/if (AUTO) {
			CHECKFADE
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
		}

	}

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat_RNG(bool _debug)
	DEBUG \atmisc_combat_RNG\ax()

	ISMEDEAD
	/if (${target[isdead, TARGET]}) {
		/return FALSE
	}


	CHECKEXIT
	GETINPUT
	CHECKTIE

	/call check_heal_routines
	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[sic].Value} TARGET
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} TARGET FORCE
	}

	/if (ENGINE2) {
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	| agro control
	/if (AGRO) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscEnrage].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscEnrage].Value}" TARGET FALSE
		}
	} else /if (!AGRO) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscJolt].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscJolt].Value}" TARGET FALSE
		}
	}

	| elemental arrows rotation
	/if (!${Target.Buff[Elemental Arrow].ID}) {
		/if (${is_ready[${_debug}, "Elemental Arrow"]}) {
			/call voodoo ${_debug} "Elemental Arrow" TARGET FALSE
		}
	}

	/if (!${maChr.Find[stCountAE].Value}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscMeleeSingle].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscMeleeSingle].Value}" TARGET FALSE
		}
	} else /if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= ${maChr.Find[stCountAE].Value} && ${maChr.Find[stCountAE].Value}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscMeleeAE].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscMeleeAE].Value}" TARGET FALSE
		}
	}

/return TRUE



|***
 * note: MGB ranger Auspice if both AAs are ready
 * use:
 ***|
sub cast_AA_auspice(bool _debug)
	DEBUG \atcast_AA_auspice\ax()

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[Auspice of the Hunter]} && !${Bool[${Me.Song[Auspice of the Hunter]}]}) {
		/if (${is_ready[${_debug}, "Mass Group Buff"]}) {
			/call voodoo ${_debug} "Mass Group Buff" 0 FALSE
		}
		/if (${is_ready[${_debug}, "Auspice of the Hunter"]}) {
			/call voodoo ${_debug} "Auspice of the Hunter" 0 FALSE
		}
	} else /if (${Me.Song[Auspice of the Hunter].ID}) {
		OUT ${bad}QUIT\ax trying to cast ${sp}Auspice of the Hunter\ax while its running.
	}
/return TRUE



|***
 * note: healing
 * use: /call check_heal_self_${Me.Class.ShortName}
 ***|
sub check_heal_RNG(bool _debug, string _type)
	DEBUG \atcheck_heal_RNG\ax(\a-w${_type}\ax)

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/call get_hurt FALSE ${_type}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return	FALSE
	}

	| ToT heals
	/if (${Select[${_type},tot]} && ${maHeal.Find[swHealToT].Value}) {
		/if (${hurtCount} >= 1)  {
			/call cast_list ${_debug} lsHealToT ${hurtID}
			/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
		}
	}


	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

	| Reflexive Healing Disc
	/if (${hurtCount} && ${Select[${_type},self,group,tot]}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
			/call voodoo ${_debug} "${maChr.Find[stDiscHeal].Value}" TARGET FALSE
		}
	}


	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
	CHECKREZ
	CHECKFADE

	/call cast_cycle_heal ${_debug} ${hurtID}

	SETHUD FALSE

/return TRUE



|***
 * note: Control for Spam Agro Nuke
 * use: /call cast_nuke_summer DEBUG [TARGETID]
 ***|
sub cast_nuke_summer(bool _debug, int _tmpID)
	DEBUG \atcast_nuke_summer\ax(\a-w${_tmpID}\ax)

	/if (${target[isdead, TARGET]}) {
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT
	/invoke ${set_data_timer[${_debug}, Summer_Nuke, RESTART]}

	/if (${is_ready[${_debug}, "${maChr.Find[stSummer].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stSummer].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: really should be starting fires
 * use:
 ***|
sub burn_RNG(bool _debug, int _tmpID)
	DEBUG \atburn_RNG\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (${target[isdead, TARGET]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	| cheeta
	/if (${is_ready[${_debug}, "Scarlet Cheetah's Fang"]}) {
		/call voodoo ${_debug} "Scarlet Cheetah's Fang" 0 FALSE
	}

	| auspice
	/if (${is_ready[${_debug}, "Auspice of the Hunter"]}) {
		/call voodoo ${_debug} "Auspice of the Hunter" 0 FALSE
	}

	| MELEE
	|**
	/if (MELEE) {
		/if (${Target.Distance} <= ${Spawn[${_tmpID}].MaxRangeTo}*.9) {

			| weapon proc
			/if (${is_ready[${_debug}, "Imbued Ferocity"]}) {
				/call voodoo ${_debug} "Imbued Ferocity" 0 FALSE
			}

			/if (!${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
				/if (${is_ready[${_debug}, "Empowered Blades"]}) {
					/call voodoo ${_debug} "Empowered Blades" 0 FALSE
				}

			} else /if (!${Me.ActiveDisc.ID} && !${Me.Song[Empowered Blades].ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
				/if (${is_ready[${_debug}, "${maChr.Find[stDiscMele].Value}"]}) {
					/call voodoo ${_debug} "${maChr.Find[stDiscMele].Value}" 0 FALSE
				}

			} else /if (!${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stDiscMele].Value}"]} && !${Me.AltAbilityReady[Empowered Blades]} && !${Me.Song[Empowered Blades].ID}) {
				/call cast_glyph ${maDebug.Find[glyph].Value}
			}
		}


	}
	**|

	|**
	| RANGE
	/if (RANGE && ${Target.Distance} >= 31) {
			/if (${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
				/if (${is_ready[${_debug}, "${maChr.Find[stDiscRange].Value}"]}) {
					/call voodoo ${_debug} "${maChr.Find[stDiscRange].Value}" 0 FALSE
				}
			} else /if (!${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stDiscRange].Value}"]}) {
				/call cast_glyph ${maDebug.Find[glyph].Value}
			}
		}
	**|


	| attack / dmg AA
	/if (${is_ready[${_debug}, "Outrider's Accuracy"]}) {
		/call voodoo ${_debug} "Outrider's Accuracy" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Outrider's Attack"]}) {
		/call voodoo ${_debug} "Outrider's Attack" 0 FALSE
	}

	| guardian buff (group first)
	/if (!${Me.Song[Guardian of the Forest].ID} && ${Me.AltAbilityReady[Group Guardian of the Forest]}) {
		/if (${is_ready[${_debug}, "Group Guardian of the Forest"]}) {
			/call voodoo ${_debug} "Group Guardian of the Forest" 0 FALSE
		}

	} else /if (!${Me.Song[Group Guardian of the Forest].ID} && ${Me.AltAbilityReady[Guardian of the Forest]}) {
		/if (${is_ready[${_debug}, "Guardian of the Forest"]}) {
			/call voodoo ${_debug} "Guardian of the Forest" 0 FALSE
		}
	}

	| swarm pets
	/if (SWARM) {
		/if (${is_ready[${_debug}, "Pack Hunt"]}) {
			/call voodoo ${_debug} "Pack Hunt" ${_tmpID} FALSE
		}
	}

	| ae regen
	/if (${is_ready[${_debug}, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stDiscHeal].Value}" ${_tmpID} FALSE
	}

 /return TRUE



|***
 * note: ranger specific control
 * use: /chr
 ***|
sub set_control_RNG(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_RNG\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local

	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
		/return

	| use mana click items
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set AA fade use %
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return

	| summer switch
	} else /if (${_type.Equal[summer]}) {
		/invoke ${set_switch_env[${_debug}, swSummer, TRUE, ${_verbage}]}

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return


