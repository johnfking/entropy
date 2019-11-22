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
		
	/call set_data FALSE maData stSpire																	map outer "Spire of the Reavers"
	/call set_data FALSE maData stSynergy																map outer "Knight's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Innoruuk's Curse"
	/call set_data FALSE maData stEpic15																map outer "Innoruuk's Voice"
	/call set_data FALSE maData stEpic2																	map outer "Innoruuk's Dark Blessing"
	/call set_data FALSE maData stFade																	map outer "Death Peace"

	/call set_data_map FALSE maChr stAAUnity														"Dark Lord's Unity (Azia)"							"${auxna} class aa unity buff"

 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxli} rest disc to recover stamina"
 	/call set_data_map FALSE maChr stAlliance 													PREGEN|1											"${auxna} alliance spell .. damn useless thing"

	/varset dump ${set_data_timer[FALSE, Check_Harmonious, 1m]}
	/call set_data_map FALSE maChr swHarmonious 												FALSE													"${auxsw} use harmonious"
 	/call set_data_map FALSE maChr stHarmonious 												PREGEN|1											"${auxna} name of the harmonions spell"
	/varset dump ${set_data_timer[FALSE, Check_Stance, 7.5m]}
	/call set_data_map FALSE maChr swStance 														FALSE													"${auxsw} use stance self buff"
 	/call set_data_map FALSE maChr stStance 														PREGEN|1											"${auxna} name of the self stance buff"
	/varset dump ${set_data_timer[FALSE, Check_Skin, 2m]}
 	/call set_data_map FALSE maChr swSkin 															FALSE													"${auxsw} use steel self buff"
 	/call set_data_map FALSE maChr stSkin 															PREGEN|1											"${auxna} name of the steel self buff"
	/call set_data_map FALSE maChr swAttraction													FALSE													"${auxsw} use the attraction aa to help with agro/pulling"
	/call set_data_map FALSE maChr stPctShieldFlash											0															"${auxnum} % of HP to try to get shield flash running"
	/varset dump ${set_data_timer[FALSE, Check_VoT, 21m]}
	/call set_data_map FALSE maChr swVoT				 												FALSE													"${auxsw} use voice of thule aa"
 	/call set_data_map FALSE maChr stTempRune 													PREGEN|1											"${auxna} fast reuse disc rune"
 	/call set_data_map FALSE maChr stChallenge 													PREGEN|1											"${auxna} challenge spell for agro"
	/call set_data_map FALSE maChr stCrimsonBlade			 									PREGEN|1											"${auxna} blade melee dps"
	/call set_data_map FALSE maChr stAcrimony			 											PREGEN|1											"${auxna} hate attention"
	/call set_data_map FALSE maChr stRancor				 											PREGEN|1											"${auxna} blade disc tap"
	/call set_data_map FALSE maChr lsHate 															EMPTY													"${auxli} single target hate skills to use"
	/call set_data_map FALSE maChr lsAEHate 														EMPTY													"${auxli} ae hate skills to use"
	/call set_data_map FALSE maChr stAELifeTap													PREGEN|1											"${auxna} se lifetap nuke"
	/call set_data_map FALSE maChr stLeechcurse													PREGEN|1											"${auxna} leechcurse disc"
	
/return



|***
 * note: Main Shadowknight Routine
 * use: self absorbed asses.. think they can tank but in reality wet TP is tougher.
 ***|
sub main_SHD(bool _debug)
	DEBUG main_SHD()

	/if (AUTO) {
		/if (!${check_engaged[]} && PET) /call check_pet
	}	
	
	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa

	/while (${check_combat_status[FALSE]}) {
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call cast_cure FALSE
		/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE		
		/if (${check_class_loop[FALSE]}) /return
		
		/if (VALID) {
			/call set_combat FALSE
			/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
			/if (AGRO) /call get_agro_${Me.Class.ShortName}
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (BURN) /call burn_${Me.Class.ShortName} FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_DOT FALSE lsDoT TARGET
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/call check_item_cycle FALSE TARGET
			| /call check_deBuffs FALSE TARGET
			| /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
			/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) {
				/call check_harmonious
			}
		
			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_self FALSE stSkin
				/call check_buff_cycle_temp_self FALSE stStance
			}			
			
		}

		/if (AUTO) {	
			/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery
		}
	}

/return TRUE



|***
 * note: kill all the things!
 * use: this shits auto, learn to love it.
 ***|
sub misc_combat_SHD(bool _debug)
	DEBUG \atmisc_combat_SHD\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE
	
	/if (${is_target_dead[FALSE, TARGET]}) /return

	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
	
	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa

	| AA Voice of Thule AA
	/if (AGRO && !${timer_Check_VoT} && ${maChr.Find[swVoT].Value}) {
		/if (!${Me.Buff[Voice of Thule].ID}) {	
			/if (${is_ready[FALSE, "Voice of Thule"]}) /call voodoo FALSE "Voice of Thule" alt ${Me.ID} FALSE
			/varset dump ${set_data_timer[FALSE, Check_VoT, RESTART]}
		}
	} else /if ((!AGRO && ${Me.Buff[Voice of Thule].ID}) || !${maChr.Find[swVoT].Value}) {
			/invoke ${Me.Buff[Voice of Thule].Remove}
	}

	| get me sum...
	/if (AGRO) /call get_agro_${Me.Class.ShortName}

	| tap 'dat ass..
	/if (${maHeal.Find[swHealSelf].Value}) {
		/if (${is_ready[FALSE, "${maChr.Find[stRancor].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) /call voodoo FALSE "${maChr.Find[stRancor].Value}" TARGET FALSE
		/if (${is_ready[FALSE, "Leech Touch"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) /call voodoo FALSE "Leech Touch" TARGET FALSE
		/if (${SpawnCount[npc OVERLOS radius SAFEENV*2 targetable]} >= ${defCount}) {
			/if (${is_ready[FALSE, "${maChr.Find[stLeechcurs].Valuee}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) /call voodoo FALSE "${maChr.Find[stLeechcurse].Value}" 0 FALSE
		}
		/if (${is_ready[FALSE, "Vicious Bite of Chaos"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) /call voodoo FALSE "Vicious Bite of Chaos" TARGET FALSE
	}

	| swarm pets
	/if (SWARM) {
		/if (${is_ready[FALSE, "Chattering Bones"]}) /call voodoo FALSE "Chattering Bones" TARGET FALSE
	}
	
	/if (${is_ready[FALSE, "${maChr.Find[stTempRune].Value}"]}) /call voodoo FALSE "${maChr.Find[stTempRune].Value}" 0 FALSE
	/if (${is_ready[FALSE, "Thought Leech"]} &&  ${Me.PctMana} <= 60*.PCTHO) /call voodoo FALSE "Thought Leech" TARGET FALSE
	/if (${is_ready[FALSE, "${maChr.Find[stGougingBlade].Value}"]}) /call voodoo FALSE "${maChr.Find[stGougingBlade].Value}" FALSE

/return



|***
 * note: we are agro gods.
 * use: /call get_agro_${Me.Class.ShortName}
 ***|
sub get_agro_SHD(bool _debug)
	DEBUG get_agro_SHD()
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[FALSE, TARGET]}) /return
	/if (PET) /call send_pet FALSE TARGET
	
	/call cast_challenge TARGET
	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa
	
	| AA pet hate
	/if (${Me.PctAggro} < 80 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
		/if (${is_ready[FALSE, "Projection of Doom"]}) /call voodoo FALSE "Projection of Doom" TARGET FALSE
	}

	| single hate
	/vardata li lsHate.First.Clone
	/while (!${li.IsEnd}) {
		/if (${Me.PctAggro} > 90 || ${Me.TargetOfTarget.Name.Equal[${Me.DisplayName}]}) /break
		/if (${is_ready[FALSE, "${li.Value}"]}) {
			/call voodoo FALSE "${li.Value}" TARGET FALSE
		}
		/varset dump ${li.Advance}
	}	

	| AE Hate
	/if (AE# && ${SpawnCount[npc radius SAFEENV targetable PS4]} >= AE#) {
		/varset mobAECount ${SpawnCount[npc OVERLOS radius 50]}
		
		/if (${mobAECount} >= AE#) {
			/vardata li lsAEHate.First.Clone
			/while (!${li.IsEnd}) {
				/if (${is_ready[FALSE, "${li.Value}"]}) {
					/call voodoo FALSE "${li.Value}" TARGET FALSE
				}
				/varset dump ${li.Advance}
			}	
		} 
		
		| AE lifetap	
		/if (${mobAECount} >= AE# && !${SpawnCount[pc enchanter radius SAFEENV*2]}) {
			/if (${is_ready[FALSE, "${maChr.Find[stAELifeTap].Value}"]}) {
				/call voodoo FALSE "${maChr.Find[stAELifeTap].Value}" TARGET FALSE
			}
		}
	}
	
	| fake hate
	/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) { 
		/if (${is_ready[FALSE, "Ageless Enmity"]}) {
			/call voodoo FALSE "Ageless Enmity" TARGET FALSE
		}
	}
	| named fake hate
	/if (${lsZoneNamed.Contains[${Target.DisplayName}]}) {
		/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
			/if (${is_ready[FALSE, "Mindless Hatred"]} && ${Target.Level} <= 110) {
				/call voodoo FALSE "Mindless Hatred" TARGET FALSE
			}
		}
		/if (${Me.PctAggro} < 90 || ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) { 
			/if (${is_ready[FALSE, "${maChr.Find[stAcrimony].Value}"]}) {
				/call voodoo FALSE "${maChr.Find[stAcrimony].Value}" TARGET FALSE
		}
	}
/return



|***
 * note: keeps Shadowknigt Vizat's Skin
 * use: /call check_skin DEBUG
 ***|
sub check_skin(bool _debug)
	DEBUG \atcheck_skin\ax()

	ISMEDEAD
	SIMPLEEXIT
	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa
	GETINPUT
	/if (${is_target_dead[FALSE, TARGET]}) /return	
	/if (!${Me.Buff[${maChr.Find[stSkin].Value}].ID} && ${is_ready[FALSE, "${maChr.Find[stSkin].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stSkin].Value}" 0 FALSE
		/varset dump ${set_data_timer[FALSE, Check_Skin, RESTART]}
	}
	
/return



|***
 * note: burn it with fire!
 * use: 
 ***| 
sub burn_SHD(bool _debug)
	DEBUG \atburn_SHD\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[FALSE, TARGET]}) /return		
	
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
	
	| deflection
	/if (!${Me.ActiveDisc.ID} && ${Me.PctHPs} < 50*.PCTHO) {
		/if (${is_ready[FALSE, "Deflection Discipline"]}) {
			/call voodoo FALSE "Deflection Discipline" 0 FALSE
			/return
		}
	}	
	
	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa

	/if (${is_ready[FALSE, "T`Vyl's Resolve"]}) /call voodoo FALSE "T`Vyl's Resolve" TARGET FALSE
	/if (${is_ready[FALSE, "${stCrimsonBlade}"]}) /call voodoo FALSE "${stCrimsonBlade}" 0 FALSE
	/call cast_AA_spire FALSE
	/if (${is_ready[FALSE, "Veil of Darkness"]}) /call voodoo FALSE "Veil of Darkness" TARGET FALSE
	/if (${is_ready[FALSE, "Harm Touch"]}) /call voodoo FALSE "Harm Touch" TARGET FALSE
	/if (AGRO) {
		/if (${is_ready[FALSE, "Scourge Skin"]}) /call voodoo FALSE "Scourge Skin" 0 FALSE
	}

	| AA Visage of Decay/Death stacking issues
	/if (!${Me.Buff[Visage of Decay].ID}) {
		/if (${is_ready[FALSE, "Visage of Death"]}) /call voodoo FALSE "Visage of Death" 0 FALSE
	} else /if (!${Me.AltAbilityReady[Visage of Death]} && !${Me.Buff[Visage of Death].ID}) {
		/if (${is_ready[FALSE, "Visage of Decay"]})	/call voodoo FALSE "Visage of Decay" 0 FALSE
	}

	/call cast_glyph FALSE

/return



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
		/return

	| use mana click items
	} else /if (${_type.Equal[manaclick]}) {
		/if (!${set_control_num_range[FALSE, stPctManaClick, "${_verbage}", 0, 99]}) /return
		
	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire FALSE ${_verbage} ${_silent}
		/return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[FALSE, stPctFade, "${_verbage}", 0, 99]}) /return

	| voice of Thule
	} else /if (${_type.Equal[vot]}) {
		/varset dump ${set_switch_env[${_debug}, swVoT, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return

	| shield flash
	} else /if (${_type.Equal[flash]}) {
		/if (!${set_control_num_range[FALSE, stPctShieldFlash, "${_verbage}", 0, 99]}) /return

	| harmonious
	} else /if (${_type.Equal[harmonious]}) {
		/varset dump ${set_switch_env[${_debug}, swHarmonious, TRUE, ${_verbage}]}
	
	| attraction
	} else /if (${_type.Equal[attraction]}) {
		/varset dump ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}
	
	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset _out
	/vardata li lsChr.First.Clone
	/while (!${li.IsEnd}) {
		/varset _out ${_out} ${If[${lsChr.First.Value.Equal[${li.Value}]},,${dot}]} ${info}${li.Value}\ax
		/if (${li.Value.Equal[manaclick]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaClick].Value},${num}${maChr.Find[stPctManaClick].Value}\ax,${r0}]}
		/if (${li.Value.Equal[fade]}) /varset _out ${_out}:${If[${maChr.Find[stPctFade].Value},${num}${maChr.Find[stPctFade].Value}\ax,${r0}]}
		/if (${li.Value.Equal[ae]}) /varset _out ${_out}:${If[${maChr.Find[stCountAE].Value},${num}${maChr.Find[stCountAE].Value}\ax,${r0}]}
		/if (${li.Value.Equal[def]}) /varset _out ${_out}:${If[${maChr.Find[stCountDefensive].Value},${num}${maChr.Find[stCountDefensive].Value}\ax,${r0}]}
		/if (${li.Value.Equal[spire]}) /varset _out ${_out}:${submenu}
		/if (${li.Value.Equal[flash]}) /varset _out ${_out}:${If[${maChr.Find[stPctShieldFlash].Value},${num}${maChr.Find[stPctShieldFlash].Value}\ax,${r0}]}
		/if (${li.Value.Equal[vot]}) /varset _out ${_out}:${If[${maChr.Find[swVoT].Value},${on},${off}]}
		/if (${li.Value.Equal[attraction]}) /varset _out ${_out}:${If[${maChr.Find[swAttraction].Value},${on},${off}]}
		/if (${li.Value.Equal[harmonious]}) /varset _out ${_out}:${If[${maChr.Find[swHarmonious].Value},${on},${off}]}


		/varset dump ${li.Advance}
	}	
	OUT /chr ${_out}
/return

