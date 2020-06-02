|***
 * entropy
 * class.shd.inc
 ***|



|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub set_SHD(bool _debug)
	DEBUG \atset_SHD\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of the Reavers"
	/call set_data FALSE maData stSynergy																map outer "Knight's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Innoruuk's Curse"
	/call set_data FALSE maData stEpic15																map outer "Innoruuk's Voice"
	/call set_data FALSE maData stEpic2																	map outer "Innoruuk's Dark Blessing"
	/call set_data FALSE maData stFade																	map outer "Death Peace"

	/call set_data_map FALSE maChr stAAUnity														"Dark Lord's Unity (Azia)"							"${auxna} class aa unity buff"

 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr stHiatus															PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"

 	/call set_data_map FALSE maChr stAlliance 													PREGEN|1											"${auxna} alliance spell .. damn useless thing"

	/invoke ${set_data_timer[FALSE, Check_Harmonious, 1m]}
	/call set_data_map FALSE maChr swHarmonious 												FALSE													"${auxsw} use harmonious"
 	/call set_data_map FALSE maChr stHarmonious 												PREGEN|1											"${auxna} name of the harmonions spell"
	/invoke ${set_data_timer[FALSE, Check_Stance, 7.5m]}
	/call set_data_map FALSE maChr swStance 														FALSE													"${auxsw} use stance self buff"
 	/call set_data_map FALSE maChr stStance 														PREGEN|1											"${auxna} name of the self stance buff"
	/invoke ${set_data_timer[FALSE, Check_Skin, 2m]}
 	/call set_data_map FALSE maChr swSkin 															FALSE													"${auxsw} use steel self buff"
 	/call set_data_map FALSE maChr stSkin 															PREGEN|1											"${auxna} name of the steel self buff"
	/call set_data_map FALSE maChr swAttraction													FALSE													"${auxsw} use the attraction aa to help with agro/pulling"

	/invoke ${set_data_timer[FALSE, Check_VoT, 21m]}
	/call set_data_map FALSE maChr swVoT				 												FALSE													"${auxsw} use voice of thule aa"
 	/call set_data_map FALSE maChr stTempRune 													PREGEN|1											"${auxna} fast reuse disc rune"
 	/call set_data_map FALSE maChr stChallenge 													PREGEN|1											"${auxna} challenge spell for agro"
	/call set_data_map FALSE maChr stCrimsonBlade			 									PREGEN|1											"${auxna} blade melee dps"
	/call set_data_map FALSE maChr stAcrimony			 											PREGEN|1											"${auxna} hate attention"
	/call set_data_map FALSE maChr stRancor				 											PREGEN|1											"${auxna} blade disc tap"
	/call set_data_list FALSE maChr lsHate 															EMPTY													"${auxli} single target hate skills to use"
	/call set_data_list FALSE maChr lsAEHate 														EMPTY													"${auxli} ae hate skills to use"
	/call set_data_map FALSE maChr stAELifeTap													PREGEN|1											"${auxna} se lifetap nuke"
	/call set_data_map FALSE maChr stLeechcurse													PREGEN|1											"${auxna} leechcurse disc"
	/call set_data_map FALSE maChr stGougingBlade												PREGEN|1											"${auxna} gouging blade family disc"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: Main Shadowknight Routine
 * use: self absorbed asses.. think they can tank but in reality wet TP is tougher.
 ***|
sub main_SHD(bool _debug)
	DEBUG \atmain_SHD\ax()

	/if (AUTO) {
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle FALSE
	}

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash FALSE aa
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return

		/if (VALID) {
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
				/call cast_shield_flash FALSE disc
			}
			/if (AGRO) /call get_agro_${Me.Class.ShortName} ${maDebug.Find[agro].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
			| /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
			/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) {
				/call check_harmonious ${maDebug.Find[harmonious].Value}
			}

			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_self FALSE stSkin
				/call check_buff_cycle_temp_self FALSE stStance
			}

		}

		/if (AUTO) {
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
		}
	}

/return TRUE



|***
 * note: healing
 * use: /call check_heal_self_${Me.Class.ShortName}
 ***|
sub check_heal_SHD(bool _debug, string _type)
	DEBUG \atcheck_heal_${Me.Class.ShortName}\ax(\a-w${_type}\ax)
	FLOW check_heal_SHD ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE
	GETINPUT check_heal_SHD NA|NA

	SETHUD "heal ${_type}"

	| tap 'dat ass..
	/if (${maHeal.Find[swHealSelf].Value}) {

		/if (${is_ready[${_debug}, "${maChr.Find[stRancor].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
			/call voodoo ${_debug} "${maChr.Find[stRancor].Value}" TARGET FALSE
		}

		/if (${is_ready[${_debug}, "Leech Touch"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
			/call voodoo ${_debug} "Leech Touch" TARGET FALSE
		}

		/if (${SpawnCount[npc OVERLOS radius SAFEENV*2 targetable]} >= ${maChr.Find[stCountDefensive].Value}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stLeechcurs].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
				/call voodoo ${_debug} "${maChr.Find[stLeechcurse].Value}" 0 FALSE
			}
		}
		/if (${is_ready[${_debug}, "Vicious Bite of Chaos"]}) {
			/call voodoo ${_debug} "Vicious Bite of Chaos" TARGET FALSE
		}
	}

	/call get_hurt FALSE ${_type}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return	FALSE
	}

	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

	/call cast_cycle_heal ${_debug} ${hurtID} healsingle

	SETHUD EMPTY

/return TRUE



|***
 * note: kill all the things!
 * use: this shits auto, learn to love it.
 ***|
sub misc_combat_SHD(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_SHD\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat_SHD

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT misc_combat_SHD _tmpID|${_tmpID}
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

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash FALSE aa
	}

	| AA Voice of Thule AA
	/if (AGRO && !${timer_Check_VoT} && ${maChr.Find[swVoT].Value}) {
		/if (!${Me.Buff[Voice of Thule].ID}) {
			/if (${is_ready[${_debug}, "Voice of Thule"]}) /call voodoo ${_debug} "Voice of Thule" ${Me.ID} FALSE
			/invoke ${set_data_timer[FALSE, Check_VoT, RESTART]}
		}
	} else /if ((!AGRO && ${Me.Buff[Voice of Thule].ID}) || !${maChr.Find[swVoT].Value}) {
			/invoke ${Me.Buff[Voice of Thule].Remove}
	}

	| get me sum...
	/if (AGRO) {
		/call get_agro_${Me.Class.ShortName} ${_debug} ${_tmpID}
	}

	| swarm pets
	/if (SWARM) {
		/if (${is_ready[${_debug}, "Chattering Bones"]}) {
			/call voodoo ${_debug} "Chattering Bones" ${_tmpID} FALSE
		}
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stTempRune].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stTempRune].Value}" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Thought Leech"]} &&  ${Me.PctMana} <= 60*.PCTHO) {
		/call voodoo ${_debug} "Thought Leech" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stGougingBlade].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stGougingBlade].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: we are agro gods.
 * use: /call get_agro_${Me.Class.ShortName}
 ***|
sub get_agro_SHD(bool _debug, int _tmpID)
	DEBUG \atget_agro_SHD\ax()
	FLOW get_agro_SHD

	ISMEDEAD
	CHECKEXIT
	GETINPUT get_agro_SHD _tmpID|${_tmpID}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}

	/if (ENGINE2) {
		/call cast_cycle_agro ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	/call cast_challenge FALSE ${_tmpID}

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash FALSE aa
	}

	| AA pet hate
	/if (${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
		/if (${is_ready[${_debug}, "Projection of Doom"]}) {
			/call voodoo ${_debug} "Projection of Doom" ${_tmpID} FALSE
		}
	}

	| single hate
	/declare _liha listiterator local
	/vardata _liha lsHate.First.Clone
	/while (!${_liha.IsEnd}) {
		| /if (${Me.PctAggro} > 90 || ${Me.TargetOfTarget.Name.Equal[${Me.DisplayName}]}) /break
		/if (${is_ready[${_debug}, "${_liha.Value}"]}) {
			/call voodoo ${_debug} "${_liha.Value}" ${_tmpID} FALSE
			/break
		}
		/invoke ${_liha.Advance}
	}

	| AE Hate
	/if (AE# && ${SpawnCount[npc radius 50 PS4]} >= AE#) {
			/declare _liaeha listiterator local
			/vardata _liaeha lsAEHate.First.Clone
			/while (!${_liaeha.IsEnd}) {
				/if (${is_ready[${_debug}, "${_liaeha.Value}"]}) {
					/call voodoo ${_debug} "${_liaeha.Value}" 0 FALSE
					/break
				}
				/invoke ${_liaeha.Advance}
			}

		| AE lifetap
		/if (!${SpawnCount[pc enchanter radius SAFEENV*2]}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAELifeTap].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAELifeTap].Value}" 0 FALSE
			}
		}
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	| fake hate
	/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
		/if (${is_ready[${_debug}, "Ageless Enmity"]}) {
			/call voodoo ${_debug} "Ageless Enmity" ${_tmpID} FALSE
		}
	}
	| named fake hate
	/if (${lsZoneNamed.Contains[${Target.DisplayName}]}) {
		/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
			/if (${is_ready[${_debug}, "Mindless Hatred"]} && ${Target.Level} <= ${Me.Level}) {
				/call voodoo ${_debug} "Mindless Hatred" ${_tmpID} FALSE
			}
		}
		/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAcrimony].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAcrimony].Value}" ${_tmpID} FALSE
			}
		}
	}

/return TRUE



|***
 * note: keeps Shadowknigt Vizat's Skin
 * use: /call check_skin DEBUG
 ***|
sub check_skin(bool _debug)
	DEBUG \atcheck_skin\ax()
	FLOW check_skin

	ISMEDEAD
	SIMPLEEXIT
	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash FALSE aa
	}

	GETINPUT check_skin NA|NA

	/if (!${Me.Buff[${maChr.Find[stSkin].Value}].ID} && ${is_ready[${_debug}, "${maChr.Find[stSkin].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stSkin].Value}" 0 FALSE
		/invoke ${set_data_timer[FALSE, Check_Skin, RESTART]}
	}

/return TRUE



|***
 * note: don't touch me there...
 * use:
 ***|
sub burn_SHD(bool _debug, int _tmpID)
	DEBUG \atburn_SHD\ax()
	FLOW burn_SHD

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT burn_SHD _tmpID|${_tmpID}
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	| deflection
	/if (!${Me.ActiveDisc.ID} && ${Me.PctHPs} < 50*.PCTHO) {
		/if (${is_ready[${_debug}, "Deflection Discipline"]}) {
			/call voodoo ${_debug} "Deflection Discipline" 0 FALSE
			/return TRUE
		}
	}

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}

	/if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
		/call cast_shield_flash FALSE aa
	}

	/if (${is_ready[${_debug}, "T`Vyl's Resolve"]}) {
		/call voodoo ${_debug} "T`Vyl's Resolve" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stCrimsonBlade].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCrimsonBlade].Value}" 0 FALSE
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "Veil of Darkness"]}) {
		/call voodoo ${_debug} "Veil of Darkness" ${_tmpID} FALSE
	}
	/if (${is_ready[${_debug}, "Harm Touch"]}) {
		/call voodoo ${_debug} "Harm Touch" ${_tmpID} FALSE
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (AGRO) {
		/if (${is_ready[${_debug}, "Scourge Skin"]}) {
			/call voodoo ${_debug} "Scourge Skin" 0 FALSE
		}
	}

	| AA Visage of Decay/Death stacking issues
	/if (!${Me.Buff[Visage of Decay].ID}) {
		/if (${is_ready[${_debug}, "Visage of Death"]}) {
			/call voodoo ${_debug} "Visage of Death" 0 FALSE
		}
	} else /if (!${Me.AltAbilityReady[Visage of Death]} && !${Me.Buff[Visage of Death].ID}) {
		/if (${is_ready[${_debug}, "Visage of Decay"]})	{
			/call voodoo ${_debug} "Visage of Decay" 0 FALSE
		}
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}

/return TRUE



|***
 * note: class controls
 * use:
 ***|
sub set_control_SHD(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_SHD\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr
		/return TRUE

	| use mana click items
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return TRUE

	| set AA fade use %
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| voice of Thule
	} else /if (${_type.Equal[vot]}) {
		/invoke ${set_switch_env[${_debug}, swVoT, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| harmonious
	} else /if (${_type.Equal[harmonious]}) {
		/invoke ${set_switch_env[${_debug}, swHarmonious, TRUE, ${_verbage}]}

	| attraction
	} else /if (${_type.Equal[attraction]}) {
		/invoke ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	| use skin disc
	} else /if (${_type.Equal[skin]}) {
		/invoke ${set_switch_env[${_debug}, swSkin, TRUE, ${_verbage}]}

	| use hiatus disc
	} else /if (${_type.Equal[stance]}) {
		/invoke ${set_switch_env[${_debug}, swStance, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

