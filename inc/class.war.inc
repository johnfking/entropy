
|***
 * entropy
 * class.war.inc
 ***|



|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub set_WAR(bool _debug)
	DEBUG \atset_WAR\ax()

	/call set_data FALSE maData stSpire 																map outer "Spire of the Warlord"
	/call set_data FALSE maData stSynergy																map outer "Myrmidon's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Jagged Blade of War"
	/call set_data FALSE maData stEpic15																map outer "Champion's Sword of Eternal Power"
	/call set_data FALSE maData stEpic2																	map outer "Kreljnok's Sword of Eternal Power"
	/call set_data FALSE maData stFade																	map outer FALSE
	/call set_data_map FALSE maChr stAAUnity														FALSE													"${auxna} class aa unity buff"

	| attraction AA used for pulling / agro
	/call set_data_map FALSE maChr swAttraction													FALSE													"${auxsw} use the attraction aa to help with agro/pulling"

 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr stHiatus															PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"


	| /call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stDichotomic													PREGEN|1											"${auxna} dichotomic disc"
	/call set_data_map FALSE maChr stRunes															PREGEN|1											"${auxna} Aura of Runes Discipline family disc"
	/call set_data_map FALSE maChr stKnuckle														PREGEN|1											"${auxna} Knuckle Smash family disc"
	/call set_data_map FALSE maChr stCommanding													PREGEN|1											"${auxna} Commanding Voice family disc"
	/call set_data_map FALSE maChr stReprisal														PREGEN|1											"${auxna} Determined Reprisal family disc"
	/call set_data_map FALSE maChr stEvasive 														PREGEN|1											"${auxna} Evasive Discipline family disc"
	/call set_data_map FALSE maChr stProvoke 														PREGEN|1											"${auxna} Provoke family disc"
	/call set_data_map FALSE maChr stWhirling			 											PREGEN|1											"${auxna} Whirling Blade family disc"
	/call set_data_map FALSE maChr stWade				 												PREGEN|1											"${auxna} Wade into Battle family disc"
	/call set_data_map FALSE maChr stRallying														PREGEN|1											"${auxna} Rallying Roar family disc"
	/call set_data_map FALSE maChr stShout				 											PREGEN|1											"${auxna} Harrassing Shout family disc"
	/call set_data_map FALSE maChr stDefense 														PREGEN|1											"${auxna} Bracing Defense family disc"
	/call set_data_map FALSE maChr stField				 											PREGEN|1											"${auxna} Field Armorer family disc"

	| shield flash
	/call set_data_map FALSE maChr stPctShieldFlash											0															"${auxnum} % of HP to try to get shield flash running"
	/call set_data_map FALSE maChr stCountShieldFlash										0															"${auxnum} # of mobs to hit shield flash"
	/call set_data_map FALSE maChr stFlash				 											PREGEN|1											"${auxna} Flash of Anger family disc"

	/call set_data_map FALSE maChr stNoTime				 											PREGEN|1											"${auxna} No Time to Bleed family disc"
	/call set_data_map FALSE maChr stAttention													PREGEN|1											"${auxna} Undivided Attention family disc"
	/call set_data_map FALSE maChr stPhantom														PREGEN|1											"${auxna} Phantom Aggressor family disc"
	/call set_data_map FALSE maChr stShield 														PREGEN|1											"${auxna} Shield Topple family disc"
	/call set_data_map FALSE maChr stExpanse 														PREGEN|1											"${auxna} Harmonious Expanse family disc"
	/call set_data_map FALSE maChr stPrecision 													PREGEN|1											"${auxna} Harmonious Precision family disc"
	/call set_data_map FALSE maChr stWarriors 													PREGEN|1											"${auxna} Warrior's Auspice family disc"
	/call set_data_map FALSE maChr stBazu 															PREGEN|1											"${auxna} Bazu Bellow family disc"
	/call set_data_map FALSE maChr stShocking 													PREGEN|1											"${auxna} Shocking Defense Discipline family disc"
	/call set_data_map FALSE maChr stMighty															PREGEN|1											"${auxna} Mighty Strike Discipline family disc"
	/call set_data_map FALSE maChr stOnslaught													PREGEN|1											"${auxna} Savage Onslaught Discipline family disc"
	/call set_data_map FALSE maChr stOffensive													PREGEN|1											"${auxna} Offensive Discipline family disc"

/return



|***
 * note: main warrior routine
 * use:
 ***|
sub main_WAR(bool _debug)
	DEBUG \atmain_WAR\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE

	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		/if (${maHeal.Find[swHealSelf].Value}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call set_combat FALSE
			/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (AGRO) /call get_agro_${Me.Class.ShortName}
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value}
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value}
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_dot ${maDebug.Find[dot].Value} lsDoT TARGET
			/if (NUKE) /call cast_nuke ${maDebug.Find[nuke].Value} lsNuke TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			| /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
		}

		/if (AUTO) {
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery
		}
	}
/return



|***
 * note: the things we to to irritate chanters and clerics
 * use:
 ***|
sub misc_combat_WAR(bool _debug)
	DEBUG \atmisc_combat_WAR\ax()

	ISMEDEAD
	/if (${target[isdead, TARGET]}) {
		/return FALSE
	}


	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

	/if (AGRO) /call get_agro_${Me.Class.ShortName}

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}


	/if (${is_ready[${_debug}, "${maChr.Find[stNoTime].Value}"]} && !${Me.Buff[${maChr.Find[stDichotomic].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stNoTime].Value}" 0 FALSE
	}

	| stShocking blocks Brace for Impact
	/if (${is_ready[${_debug}, "${maChr.Find[stShocking].Value}"]} && !${Me.Buff[Brace for Impact].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stShocking].Value}" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stRunes].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stRunes].Value}" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stDefense].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stDefense].Value}" 0 FALSE
	}


	/if (${is_ready[${_debug}, "${maChr.Find[stProvoke].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stProvoke].Value}" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stCommanding].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCommanding].Value}" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "Brace for Impact"]} && !${Me.Buff[${maChr.Find[stShocking].Value}].ID}) {
		/call voodoo ${_debug} "Brace for Impact" 0 FALSE
	}

	/if (${is_ready[${_debug}, "War Stomp"]} && !${Me.Song[Battle Leap].ID}) {
		/call voodoo ${_debug} "War Stomp" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Gut Punch"]}) {
		/call voodoo ${_debug} "Gut Punch" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stField].Value}"]} && !${Me.Song[${maChr.Find[stField].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stField].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stShield].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stShield].Value}" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "Imperator's Command"]}) {
		/call voodoo ${_debug} "Imperator's Command" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Knee Strike"]}) {
		/call voodoo ${_debug} "Knee Strike" TARGET FALSE
	}

	/call check_facing ${maDebug.Find[face].Value} TARGET FALSE

	| Duplicating these two sub calls here b/c don't want to lose agro or mitigation while firing off this other stuff.
	/if (AGRO) /call get_agro_${Me.Class.ShortName}

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}

	/if (AE# && ${SpawnCount[npc radius ENV targetable PS4]} >= AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stExpanse].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stExpanse].Value}" 0 FALSE
		}

		/if (${is_ready[${_debug}, "${maChr.Find[stWade].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stWade].Value}" 0 FALSE
		}

	}

	/if (${is_ready[${_debug}, "${maChr.Find[stPrecision].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stPrecision].Value}" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stReprisal].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stReprisal].Value}" 0 FALSE
	}


	/if (${is_ready[${_debug}, "Blade Guardian"]}) {
		/call voodoo ${_debug} "Blade Guardian" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Vehement Rage"]}) {
		/call voodoo ${_debug} "Vehement Rage" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Battle Leap"]} && !${Me.Song[Battle Leap Warcry].ID} && !${Me.Song[Group Bestial Alignment].ID}) {
		/call voodoo ${_debug} "Battle Leap" 0 FALSE
	}

/return



|***
 * note:
 * use:
 ***|
sub get_agro_WAR(bool _debug)
	DEBUG \atget_agro_WAR\ax()

	ISMEDEAD
	GETINPUT
	CHECKEXIT
	/if (${target[isdead, TARGET]}) {
		/return FALSE
	}



	| (60 seconds) and ae taunt
	/if (${SpawnCount[npc OVERLOS radius 30 targetable PS4]} >= AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stRallying].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stRallying].Value}" 0 FALSE

		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stProvoke].Value}"]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.CleanName}]}) {
		/call voodoo ${_debug} "${maChr.Find[stProvoke].Value}" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stShout].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stShout].Value}" 0 FALSE
	}


	/if (${is_ready[${_debug}, "${maChr.Find[stBazu].Value}"]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.CleanName}]}) {
		/call voodoo ${_debug} "${maChr.Find[stBazu].Value}" TARGET FALSE
	}

	/if (${Me.TargetOfTarget.Name.NotEqual[${Me.CleanName}]}) {
		/if (${is_ready[${_debug}, "Rage of the Forsaken"]}) {
			/call voodoo ${_debug} "Rage of the Forsaken" TARGET FALSE
			/if (${is_ready[${_debug}, "Blast of Anger"]}) {
				/call voodoo ${_debug} "Blast of Anger" TARGET FALSE
			}
		}
	}

	/if (AE# && ${SpawnCount[npc radius ENV targetable PS4]} >= AE#) {
		/if (${is_ready[${_debug}, "Area Taunt"]}) {
			/call voodoo ${_debug} "Area Taunt" 0 FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stWhirling].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stWhirling].Value}" TARGET FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stRallying].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stRallying].Value}" 0 FALSE
		}

	}

	| 20% agro boost AA with 2/10 uptime
	/if (${is_ready[${_debug}, "Warlord's Fury"]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.CleanName}]}) {
		/call voodoo ${_debug} "Warlord's Fury" TARGET FALSE
	}

	/if (${is_ready[${_debug}, "Blast of Anger"]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.CleanName}]}) {
		/call voodoo ${_debug} "Blast of Anger" TARGET FALSE
	}

/return



|***
 * note: Self heals
 * use: *see note*
 ***|
sub check_heal_WAR(bool _debug, string _type)

	ISMEDEAD
	CHECKTIE
	CHECKEXIT
	CHECKREZ

	SETHUD "heal ${_type}"

 	/if (!${get_hurt[FALSE, Self]} || ${Me.PctHPs} >= ${maHeal.Find[stHealPointWAR].Value}*.PCTHO) {
		SETHUD FALSE
 		/return FALSE
 	}

	| heal
	/call cast_list ${_debug} lsHealBaseLine ${Me.ID}

/return




|***
 * not: burn routines
 * use:
 ***|
sub burn_WAR(bool _debug)
	DEBUG burn_WAR(${_debug})

	ISMEDEAD
	/if (${target[isdead, TARGET]}) {
		/return FALSE
	}


	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE disc

	/if (!${is_defensive_active[${_debug}, BURN, SAFEENV]}) /call cast_defensive_cycle ${_debug}

	/if (AGRO && ${swBurnControl} && TARGET && (${NameList.Find[${Target.DisplayName}]} && !${swOverrideName})) {
		/if (${is_ready[${_debug}, "Fortitude Discipline"]}) {
			/stopdisc
			/delay 10 !${Me.ActiveDisc.ID}
			/call voodoo ${_debug} "Fortitude Discipline" 0 FALSE

		} else {
			/if (AGRO && ${is_ready[${_debug}, "${maChr.Find[stWarriors].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stWarriors].Value}" TARGET FALSE
			}
			/if (AGRO && ${is_ready[${_debug}, "Mark of the Mage Hunter"]}) {
				/call voodoo ${_debug} "Mark of the Mage Hunter" 0 FALSE
			}
		}
	}

	| Defensive burn routine (swAgro) (but see below re: mitigation).
	/if (AGRO && ${is_ready[${_debug}, "${maChr.Find[stEvasive].Value}"]} && !${Me.ActiveDisc.Name.Equal[Fortitude Discipline]} && (!${Me.Buff[${maChr.Find[stWarriors].Value}].ID} || ${SpawnCount[npc radius ENV targetable PS4]} >= ${maBurn.Find[stBurnCount].Value})) {
		/stopdisc
		/delay 10 !${Me.ActiveDisc.ID}
		/call voodoo ${_debug} "${maChr.Find[stEvasive].Value}" 0 FALSE

	} else {
		/if (AGRO && (${is_ready[${_debug}, "Fortitude Discipline"]} && ${SpawnCount[npc radius ENV targetable PS4]} >= ${maBurn.Find[stBurnCount].Value}) && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stEvasive].Value}]} && !${Me.Buff[${maChr.Find[stWarriors].Value}].ID}) {
			/stopdisc
			/delay 10 !${Me.ActiveDisc.ID}
			/call voodoo ${_debug} "Fortitude Discipline" 0 FALSE

		} else {
			/if (!${Me.ActiveDisc.Name.Equal[${maChr.Find[stEvasive].Value}]} && !${Me.ActiveDisc.Name.Equal[Fortitude Discipline]} && !${Me.Buff[${maChr.Find[stWarriors].Value}].ID}) {
				/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
			}
		}
	}

	| Offensive burn routine (!AGRO).
	/if (!AGRO && ${is_ready[${_debug}, "${maChr.Find[stMighty].Value}"]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOnslaught].Value}]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOffensive].Value}]}) {
		/stopdisc
		/delay 10 !${Me.ActiveDisc.ID}
		/call voodoo ${_debug} "${maChr.Find[stMighty].Value}" 0 FALSE

	} else {
		/if (!AGRO && ${is_ready[${_debug}, "${maChr.Find[stOnslaught].Value}"]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stMighty].Value}]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOffensive].Value}]}) {
			/stopdisc
			/delay 10 !${Me.ActiveDisc.ID}
			/call voodoo ${_debug} "${maChr.Find[stOnslaught].Value}" 0 FALSE

		} else {
			/if (!AGRO && ${is_ready[${_debug}, "${maChr.Find[stOffensive].Value}"]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stMighty].Value}]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOnslaught].Value}]}) {
				/stopdisc
				/delay 10 !${Me.ActiveDisc.ID}
				/call voodoo ${_debug} "${maChr.Find[stOffensive].Value}" 0 FALSE

			}
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAttention].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAttention].Value}" TARGET FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stPhantom].Value}"]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
		/call voodoo ${_debug} "${maChr.Find[stPhantom].Value}" TARGET FALSE
	}

	/if (${Me.AltAbilityReady[Resplendent Glory]} && !${Me.Song[Hold the Line].ID}) {
		/if (${is_ready[${_debug}, "Resplendent Glory"]}) {
			/call voodoo ${_debug} "Resplendent Glory" 0 FALSE
		}
	} else /if (${Me.AltAbilityReady[Hold the Line]} && !${Me.Song[Resplendent Glory].ID}) {
		/if (${is_ready[${_debug}, "Hold the Line"]}) {
			/call voodoo ${_debug} "Hold the Line" 0 FALSE
		}
	}
	/if (${is_ready[${_debug}, "Warlords Tenacity"]}) {
		/call voodoo ${_debug} "Warlords Tenacity" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Warlords Bravery"]}) {
		/call voodoo ${_debug} "Warlords Bravery" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Wars Sheol's Heroic Blade"]}) {
		/call voodoo ${_debug} "Wars Sheol's Heroic Blade" 0 FALSE
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/call cast_glyph ${maDebug.Find[glyph].Value}

	/if (${is_ready[${_debug}, "Rage of Rallos Zek"]}) {
		/call voodoo ${_debug} "Rage of Rallos Zek" 0 FALSE
	}

/return




|***
 * note: warrior controls
 * use:
 ***|
sub set_control_WAR(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_WAR\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr
		/return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return


	| shield flash
	} else /if (${_type.Equal[flash]}) {
		/if (!${set_control_num_range[${_debug}, stPctShieldFlash, "${_verbage}", 0, 99]}) /return

	| attraction
	} else /if (${_type.Equal[attraction]}) {
		/invoke ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return


