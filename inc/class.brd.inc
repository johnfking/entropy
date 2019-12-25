|***
 * entropy
 * class.brd.inc
 ***|
 


|***
 * note: primary initilization of all bard non shared data
 * use: god i hate bards...
 ***|
sub set_BRD(bool _debug)
	DEBUG \atset_BRD\ax()

	/call set_data FALSE maData stSpire																	map outer "Spire of the Minstrels"
	/call set_data FALSE maData stSynergy																map outer "Troubadour's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Singing Short Sword"
	/call set_data FALSE maData stEpic15																map outer "Prismatic Dragon Blade"
	/call set_data FALSE maData stEpic2																	map outer "Blade of Vesagran"
	/call set_data FALSE maData stFade																	map outer "Fading Memories"
	/call set_data_map FALSE maChr stAAUnity														FALSE													"${auxna} class aa unity buff"


	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance song"

	/call set_data_map FALSE maChr stNukeServant 												PREGEN|1											"${auxna} spam pet song"

	| map for mob control
	/declare maMobControl map outer



	/call set_data_map FALSE maChr swSongRest														FALSE													"${auxna} enable song while resting"
	/call set_data_map FALSE maChr stSongRest														FALSE													"${auxna} song to use while group resting"

	/call set_data_map FALSE maChr stSongRefresh												6															"${auxnum} remaining duration on a song to refresh. in seconds"


	| current song line
	/declare lsLineSong list outer
	/varset dump ${lsLineSong.Append[tank,caster,melee,mix,other,]}
	/call set_data_map FALSE maChr stCurrent														mix													"${auxna} current song line to play"
	/call set_data FALSE FALSE stLine																		string outer ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}


/return


sub main_BRD(bool _debug)
	DEBUG \atmain_BRD\ax()
	
	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ

		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/call control_routine FALSE
		/if (${check_class_loop[FALSE]}) /return FALSE
	
		/if (VALID) {
			/call control_routine FALSE
			/call set_combat FALSE
			/if (${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle ${maDebug.Find[def].Value}
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (!${timer_Check_deBuffs}) /call check_deBuffs FALSE TARGET
			/call control_routine FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (BURN) /call burn_${Me.Class.ShortName}
			/call control_routine FALSE
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/if (SWARM) /call cast_servant_swarm FALSE TARGET 
			/call control_routine FALSE
			| /if (${swAlliance}) /call trigger_alliance TARGET
			/if (DOT) /call cast_dot ${maDebug.Find[dot].Value} lsDoT TARGET
			/call pulse_line FALSE
			/call control_routine FALSE
			/if (NUKE) /call cast_nuke ${maDebug.Find[nuke].Value} lsNuke TARGET
			/call misc_combat_${Me.Class.ShortName} TARGET
			/call control_routine FALSE
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET	

		}
		
		/if (AUTO) {	
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			CHECKFADE
		}	
		
	}	

/return TRUE



|***
 *	DES: Combat skills, Disciplines and Alternate abilities.
 *	USE: Disc/aas and stuffs
 ***|
sub misc_combat_BRD(bool _debug, int _tmpID)
	ISMEDEAD

	/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) {
		/call clear_combat
		/call pulse_stop
		/return
	}
	
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}	
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	/call check_item_cycle ${maDebug.Find[item].Value} ${_tmpID}	

	/if (AUTO) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
		/call stick_to_target FALSE ${_tmpID}
	}	
	
	/if (${is_ready[${_debug}, "Fierce Eye"]}) {
		/call voodoo ${_debug} "Fierce Eye" 0 FALSE
	}
	
 	/if (${is_ready[${_debug}, "Reflexive Retort"]}) {
 		/call voodoo ${_debug} "Reflexive Retort" ${_tmpID} FALSE
 	}

 	/if (${is_ready[${_debug}, "Relfexive Rejoinder"]}) {
 		/call voodoo ${_debug} "Relfexive Rejoinder" ${_tmpID} FALSE
 	}	
	
 	/if (${is_ready[${_debug}, "Bladed Song"]}) {
 		/call voodoo ${_debug} "Bladed Song" 0 FALSE
 	}
 	
	/if (${is_ready[${_debug}, "Cacophony"]}) /call voodoo ${_debug} "Cacophony" ${_tmpID} FALSE
	/if (${Spawn[TARGET].PctHPs} > 30) {
		/if (${is_ready[${_debug}, "Boastful Bellow"]}) /call voodoo ${_debug} "Boastful Bellow" ${_tmpID} FALSE	
		/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) {
			/call pulse_stop
			/call clear_combat
			/return
		}
	}
	
/return TRUE



|***
 * note: self healing
 * use: /call check_heal__${Me.Class.ShortName}
 ***|
sub check_heal_BRD(bool _debug)
	DEBUG \atcheck_heal_BRD\ax()

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE
	/call cast_list ${_debug} lsHealBaseLine ${Me.ID}		
	
	SETHUD FALSE
	
/return TRUE





|***
 * note: rotates bard swarm pets as DPS fillers
 * use: /call cast_servant_swarm DEBUG [TARGETID]
 ***|
sub cast_servant_swarm(bool _debug, int _tmpID)
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) {
		/call clear_combat
		/return
	}

	/if (AUTO) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
		/call stick_to_target FALSE ${_tmpID}
	}	
	
	/if (${Target.ID}) /pet swarm

	/if (${is_ready[${_debug}, "Lyrical Prankster"]}) {
		/call voodoo ${_debug} "Lyrical Prankster" ${_tmpID} FALSE
		/return
	}

	/if (${is_ready[${_debug}, "Song of Stone"]}) {
		/call voodoo ${_debug} "Song of Stone" alt ${_tmpID} FALSE
		/return
	}

	/call cast_temp_pet FALSE ${_tmpID} "${maChr.Find[stNukeServant].Value}"	

/return TRUE



|***
 * note: sings the line of songs for group support
 * use: /call pulse_line debug 
 ***|
sub pulse_line(bool _debug)
	DEBUG \atpulse_line\ax()

	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) {
		/call clear_combat
		/call pulse_stop
		/return
	}

	/declare _skip bool local FALSE

	SETHUD "pulse"
	
	/declare _lipl listiterator local
	/vardata _lipl lsLine${stLine}.First.Clone
	/while (!${_lipl.IsEnd}) {

		/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) {
			/call clear_combat
			/call pulse_stop
			/return
		}
		
		/call control_routine FALSE
		
		/if (AUTO) {
			/call check_facing ${maDebug.Find[face].Value} TARGET FORCE
			/call stick_to_target FALSE TARGET
		}	

		| song not ready
		/if (!${is_ready[${_debug}, "${_lipl.Value}"]}) /varset _skip TRUE

		| song has time left
		/if (${Me.Song[${_lipl.Value}].Duration} > ${maChr.Find[stSongRefresh].Value}*10) /varset _skip TRUE

		/if (${_skip}) {
			/varset dump ${_lipl.Advance}
			/continue
		}

		/call voodoo ${_debug} "${_lipl.Value}" 0 FALSE
		/varset dump ${_lipl.Advance}
	}	

	SETHUD FALSE
	
/return TRUE



|***
 * note: would rather take a cheese grater to my nuts then listen to you
 * use: /call pulse_stop [useful tag as a marker for debug] DEBUG
 ***|
sub pulse_stop(string _marker, bool _debug)
	DEBUG \atpulse_stop\ax(marker:\aw${_marker}\aw)

	/while (${Me.Casting.ID} || ${Me.BardSongPlaying}) {
		/varset dump ${maData.Add[stCastReturn,NOTREADY]}
		/stopsong
		/stopcast
		/delay 10 !${Me.Casting.ID}
	}
	
/return 



|***
 * note: Bard AA version of Breather
 * use: /call cast_AA_rallying DEBUG
 *  
 *  AA - Rallying Solo
 *  Song - Rallying Solo
 ***|
sub cast_AA_rallying(bool _debug)
	DEBUG \atcast_AA_rallying\ax()

 	/if (SAFEZONE || !${Me.AltAbilityReady[Rallying Solo]}) /return
	ISMEDEAD
	GETINPUT
	
	/call pulse_stop FALSE

 	/declare _endurancePoint int local 30
 	/declare _manaPoint int local 30

 	/if (((${Me.PctEndurance} < ${_endurancePoint}) && (${Me.PctMana} < ${_manaPoint})) && ${Select[${Me.CombatState},resting]}) {
		/if (${is_ready[${_debug}, "Rallying Solo"]}) /call voodoo ${_debug} "Rallying Solo" ${Me.ID} FALSE
	}
/return TRUE



|***
 * note: Bard burn routines
 * use: establish all comp/trigger skills
 ***| 
sub burn_BRD(bool _debug)
	DEBUG \atburn_BRD\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) {
		/call clear_combat
		/call pulse_stop
		/return
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} TARGET	
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
	
	/call control_routine FALSE
	
 	/if (${is_ready[${_debug}, "Thousand Blades"]}) {
 		/call voodoo ${_debug} "Thousand Blades" 0 FALSE	
 	}
 	
	/call cast_AA_spire ${maDebug.Find[spire].Value}
	
	/if (${is_ready[${_debug}, "Funeral Dirge"]}) {
		/call voodoo ${_debug} "Funeral Dirge" 0 FALSE
	}
	
	/if (${is_ready[${_debug}, "Quick Time"]} && !${Me.Song[Ruaabri's Fury].ID}) {
		/call voodoo ${_debug} "Quick Time" 0 FALSE
	}
	
	| trigger Frenzied kicks AA for bards if target is debuffed from intimidate
	/if (${Target.Buff[Harbinger's Intimidation].ID} && ${is_ready[${_debug}, "Frenzied Kicks"]}) {
		/call voodoo ${_debug} "Frenzied Kicks" 0 FALSE
	}
	
	/call cast_glyph ${maDebug.Find[glyph].Value}
	
	/call check_item_cycle ${maDebug.Find[item].Value} TARGET	

/return TRUE



|***
 * note: issues the bard commands for running around and doing stupid shit
 * use: /dex BARDNAME /travel .. you can figure out the commands for eqbc yourself.
 ***|
#bind command_brd_travel /travel
sub Bind_command_brd_travel(bool _debug)

	/if (!${check_class[TRUE, |BRD]}) /return
	
	/if (${is_ready[${_debug}, "Selo's Sonata"]}) {
		/call voodoo ${_debug} "Selo's Sonata" 0 FALSE
	}
	/delay 1
	/if (${is_ready[${_debug}, "Shauri's Sonorous Clouding"]}) {
		/call voodoo ${_debug} "Shauri's Sonorous Clouding" alt 0 FALSE
	}
	
/return



|***
 * note: bard specific control
 * use: /chr
 ***|
sub set_control_BRD(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_BRD\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[refresh]}) {
		/if (!${set_control_num_range[${_debug}, stSongRefresh, "${_verbage}", 0, 18]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return


	} else /if (${_type.Equal[line]}) {
		/if (!${lsLineSong.Contains[${_verbage.Lower}]}) {
			OUT usage${sep}/chr line [\a-wtank\ax|\a-wcaster\ax|\a-wmelee\ax|\a-wmix\ax|\a-wother\ax]
			/return
		}
		/varset dump ${do_raw_edit[${_debug}, SILENT, stCurrent, "${_verbage.Lower}"]}
		/varset stLine ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}

	| use AA crystals
	} else /if (${_type.Equal[rest]}) {
		/varset dump ${set_switch_env[${_debug}, swSongRest, TRUE, ${_verbage}]}


	}
	
	/if (${_silent.Equal[SILENT]}) /return 
	/varset dump ${set_control_output[${_debug}, maChr, chr, maChrControl]}
	
/return


