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

		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/call control_routine FALSE
		/if (${check_class_loop[FALSE]}) /return FALSE
	
		/if (VALID) {
			/call control_routine FALSE

			/call set_combat FALSE
			/if (${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle ${maDebug.Find[def].Value}
			/if (!${timer_Check_deBuffs}) /call check_deBuffs FALSE TARGET
			/call control_routine FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (BURN) /call burn_${Me.Class.ShortName}
			/call control_routine FALSE
			/if (PET || SWARM) /call send_pet FALSE TARGET
			/if (SWARM) /call cast_servant_swarm FALSE TARGET 
			/call control_routine FALSE
			| /if (${swAlliance}) /call trigger_alliance TARGET
			/if (DOT) /call cast_dot FALSE lsDoT TARGET
			/call pulse_line FALSE
			/call control_routine FALSE
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/call misc_combat_${Me.Class.ShortName}
			/call control_routine FALSE
			/call check_item_cycle FALSE TARGET

		}
		
		/if (AUTO) {	
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
			CHECKFADE
		}	
		
	}	

/return



|***
 *	DES: Combat skills, Disciplines and Alternate abilities.
 *	USE: Disc/aas and stuffs
 ***|
sub misc_combat_BRD(bool _debug)
	ISMEDEAD

	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) {
		/call clear_combat
		/call pulse_stop
		/return
	}
	
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

	/if (AUTO) {
		/call check_facing ${maDebug.Find[face].Value} TARGET FORCE
		/call stick_to_target FALSE TARGET
	}	
	
	/if (${is_ready[FALSE, "Fierce Eye"]}) {
		/call voodoo FALSE "Fierce Eye" 0 FALSE
	}
	
 	/if (${is_ready[FALSE, "Reflexive Retort"]}) {
 		/call voodoo FALSE "Reflexive Retort" TARGET FALSE
 	}

 	/if (${is_ready[FALSE, "Relfexive Rejoinder"]}) {
 		/call voodoo FALSE "Relfexive Rejoinder" TARGET FALSE
 	}	
	
 	/if (${is_ready[FALSE, "Bladed Song"]}) {
 		/call voodoo FALSE "Bladed Song" 0 FALSE
 	}
 	
	/if (${is_ready[FALSE, "Cacophony"]}) /call voodoo FALSE "Cacophony" TARGET FALSE
	/if (${Spawn[TARGET].PctHPs} > 30) {
		/if (${is_ready[FALSE, "Boastful Bellow"]}) /call voodoo FALSE "Boastful Bellow" TARGET FALSE	
		/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) {
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
	/call cast_list FALSE lsHealBaseLine ${Me.ID}	
	
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

	/if (${is_ready[FALSE, "Lyrical Prankster"]}) {
		/call voodoo FALSE "Lyrical Prankster" ${_tmpID} FALSE
		/return
	}

	/if (${is_ready[FALSE, "Song of Stone"]}) {
		/call voodoo FALSE "Song of Stone" alt ${_tmpID} FALSE
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
		/if (!${is_ready[FALSE, "${_lipl.Value}"]}) /varset _skip TRUE

		| song has time left
		/if (${Me.Song[${_lipl.Value}].Duration} > ${maChr.Find[stSongRefresh].Value}*10) /varset _skip TRUE

		/if (${_skip}) {
			/varset dump ${_lipl.Advance}
			/continue
		}

		/call voodoo FALSE "${_lipl.Value}" 0 FALSE
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
		/if (${is_ready[FALSE, "Rallying Solo"]}) /call voodoo FALSE "Rallying Solo" ${Me.ID} FALSE
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
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
	
	/call control_routine FALSE
	
 	/if (${is_ready[FALSE, "Thousand Blades"]}) {
 		/call voodoo FALSE "Thousand Blades" 0 FALSE	
 	}
 	
	/call cast_AA_spire ${maDebug.Value[spire].Value}
	
	/if (${is_ready[FALSE, "Funeral Dirge"]}) {
		/call voodoo FALSE "Funeral Dirge" 0 FALSE
	}
	
	/if (${is_ready[FALSE, "Quick Time"]} && !${Me.Song[Ruaabri's Fury].ID}) {
		/call voodoo FALSE "Quick Time" 0 FALSE
	}
	
	| trigger Frenzied kicks AA for bards if target is debuffed from intimidate
	/if (${Target.Buff[Harbinger's Intimidation].ID} && ${is_ready[FALSE, "Frenzied Kicks"]}) {
		/call voodoo FALSE "Frenzied Kicks" 0 FALSE
	}
	
	/call cast_glyph ${maDebug.Value[glyph].Value}
	
	/call check_item_cycle FALSE TARGET

/return TRUE



|***
 * note: issues the bard commands for running around and doing stupid shit
 * use: /dex BARDNAME /travel .. you can figure out the commands for eqbc yourself.
 ***|
#bind command_brd_travel /travel
sub Bind_command_brd_travel(bool _debug)

	/if (!${check_class[TRUE, |BRD]}) /return
	
	/if (${is_ready[FALSE, "Selo's Sonata"]}) {
		/call voodoo FALSE "Selo's Sonata" 0 FALSE
	}
	/delay 1
	/if (${is_ready[FALSE, "Shauri's Sonorous Clouding"]}) {
		/call voodoo FALSE "Shauri's Sonorous Clouding" alt 0 FALSE
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
		/if (!${set_control_num_range[FALSE, stPctManaRecovery, "${_verbage}", 0, 99]}) /return
		
	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire FALSE ${_verbage} ${_silent}
		/return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[FALSE, stPctFade, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[refresh]}) {
		/if (!${set_control_num_range[FALSE, stSongRefresh, "${_verbage}", 0, 18]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return


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


