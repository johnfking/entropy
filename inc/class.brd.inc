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

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

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
	/call set_data_list ${_debug} maSong lsLineSong											EMPTY												"${auxli} list of song numbers to sing"  song

	| aa boastful bellow switch
	/call set_data_map FALSE maChr swAABellow														FALSE													"${auxsw} enable/disable using AA Boastful Bellow"

	| sing OOC
	/call set_data_map FALSE maChr swPulseOOC														FALSE													"${auxna} sing while out of combat"


	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE


sub main_BRD(bool _debug)
	DEBUG \atmain_BRD\ax()

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/call control_routine FALSE

		/if (${maChr.Find[swPulseOOC].Value}) {
			/call pulse_line ${maDebug.Find[pulse].Value} 0
		}
		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

		/if (VALID) {
			| /call control_routine FALSE
			/call set_combat FALSE
			CHECKDEFENSE
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			| /call control_routine FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			| /call control_routine FALSE
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/if (SWARM) /call cast_servant_swarm FALSE TARGET
			| /call control_routine FALSE
			| /if (${swAlliance}) /call trigger_alliance TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/call pulse_line ${maDebug.Find[pulse].Value} TARGET
			| /call control_routine FALSE
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			| /call control_routine FALSE
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
	DEBUG \atmisc_combat_BRD\ax()
	FLOW misc_combat_BRD

	ISMEDEAD

	/if (${target[isdead, ${_tmpID}]}) {
		/call clear_combat
		/call pulse_stop
		/return
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
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

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

	/if (${is_ready[${_debug}, "Cacophony"]}) {
		/call voodoo ${_debug} "Cacophony" ${_tmpID} FALSE
	}
	/if (${Spawn[TARGET].PctHPs} > 30 && ${maChr.Find[swAABellow].Value}) {
		/if (${is_ready[${_debug}, "Boastful Bellow"]}) {
			/call voodoo ${_debug} "Boastful Bellow" ${_tmpID} FALSE
		}
		/if (${target[isdead, ${_tmpID}]}) {
			/call pulse_stop
			/call clear_combat
			/return FALSE
		}
	}

/return TRUE



|***
 * note: self healing
 * use: /call check_heal__${Me.Class.ShortName}
 ***|
sub check_heal_BRD(bool _debug, string _type)
	DEBUG \atcheck_heal_BRD\ax(\a-w${_type}\ax)
	FLOW check_heal_BRD ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE
	/call cast_cycle_heal ${_debug} ${hurtID}

	SETHUD EMPTY

/return TRUE





|***
 * note: rotates bard swarm pets as DPS fillers
 * use: /call cast_servant_swarm DEBUG [TARGETID]
 ***|
sub cast_servant_swarm(bool _debug, int _tmpID)
	DEBUG \atcast_servant_swarm\ax(\a-w${_tmpID}\ax)
	FLOW cast_servant_swarm

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${target[isdead, ${_tmpID}]}) {
		/call pulse_stop
		/call clear_combat
		/return FALSE
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
 * use: /call pulse_line debug [TARGET]
 ***|
sub pulse_line(bool _debug, _tmpID)
	DEBUG \atpulse_line\ax(\a-w${_tmpID}\ax)
	FLOW pulse_line

	SETHUD "pulse"

	| if the user has not set the line order, set it to a default for now
	/if (${lsLineSong.Contains[EMPTY]} && ${lsLineSong.Count} == 1) {
		DEBUG ${sep}resetting the empty line
		/invoke ${lsLineSong.Clear}
		/invoke ${lsLineSong.Append[1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20]}
	}

	/declare _lipl listiterator local
	/vardata _lipl lsLineSong.First.Clone
	/while (!${_lipl.IsEnd}) {

		/if (${_tmpID}) {
			/if (${target[isdead, ${_tmpID}]}) {
				/call pulse_stop
				/call clear_combat
				/return FALSE
			}
		}

		| /call control_routine FALSE

		| is the song on?
		| check the switch
		/if (!${Bool[${maSong.Find[swSong${_lipl.Value}].Value}]}) {
			DEBUG ${sep}switch off${sep}\a-w${maSong.Find[stSong${_lipl.Value}].Value}\ax
			/invoke ${_lipl.Advance}
			/continue
		}

		| check the name
		/if (!${Bool[${maSong.Find[stSong${_lipl.Value}].Value}]}) {
			DEBUG ${sep}no name${sep}\a-w${maSong.Find[stSong${_lipl.Value}].Value}\ax
			/invoke ${_lipl.Advance}
			/continue
		}

		/if (!${is_ready[${_debug}, "${maSong.Find[stSong${_lipl.Value}].Value}"]}) {
			DEBUG ${sep}not ready${sep}\a-w${maSong.Find[stSong${_lipl.Value}].Value}\ax
			/invoke ${_lipl.Advance}
			/continue
		}

		| apply any conditions
		/if (!${check_condition[${_debug}, ${maSong.Find[stSong${_lipl.Value}Con].Value}]}) {
			DEBUG ${sep}condition fail${sep}\a-w${maSong.Find[stSong${_lipl.Value}].Value}\ax
			/invoke ${_lipl.Advance}
			/continue
		}

		/call check_basic_tags ${_debug} Song${_lipl.Value} FALSE "${maSong.Find[stSong${_lipl.Value}].Value}"
		/if (!${Macro.Return}) {
			DEBUG ${sep}tag fail${sep}\a-w${maSong.Find[stSong${_lipl.Value}].Value}\ax
			/invoke ${_lipl.Advance}
			/continue
		}

		| song has time left
		/if (${Me.Song[${Spell[${maSong.Find[stSong${_lipl.Value}].Value}].RankName}].Duration} > ${maChr.Find[stSongRefresh].Value}*10) {
			/invoke ${_lipl.Advance}
			/continue
		}

		/if (AUTO && ${_tmpID}) {
			/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
			/call stick_to_target FALSE ${_tmpID}
		}

		/call voodoo ${_debug} "${maSong.Find[stSong${_lipl.Value}].Value}" 0 FALSE

		/invoke ${_lipl.Advance}

	}

	SETHUD EMPTY

/return TRUE



|***
 * note: would rather take a cheese grater to my nuts then listen to you
 * use: /call pulse_stop [useful tag as a marker for debug] DEBUG
 ***|
sub pulse_stop(string _marker, bool _debug)
	DEBUG \atpulse_stop\ax(marker:\aw${_marker}\aw)
	FLOW pulse_stop

	/while (${Me.Casting.ID} || ${Me.BardSongPlaying}) {
		/invoke ${maData.Add[stCastReturn,NOTREADY]}
		/stopsong
		/stopcast
		/delay 10 !${Me.Casting.ID}
	}

/return TRUE



|***
 * note: Bard AA version of Breather
 * use: /call cast_AA_rallying DEBUG
 *
 *  AA - Rallying Solo
 *  Song - Rallying Solo
 ***|
sub cast_AA_rallying(bool _debug)
	DEBUG \atcast_AA_rallying\ax()
	FLOW cast_AA_rallying

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
sub burn_BRD(bool _debug, int _tmpID)
	DEBUG \atburn_BRD\ax(\a-w${_tmpID}\ax)
	FLOW burn_BRD

	ISMEDEAD
		/if (${target[isdead, ${_tmpID}]}) {
			/call pulse_stop
			/call clear_combat
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

	| /call control_routine FALSE

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

	/call check_item_cycle ${maDebug.Find[item].Value} ${_tmpID}

/return TRUE



|***
 * note: issues the bard commands for running around and doing stupid shit
 * use: /dex BARDNAME /travel .. you can figure out the commands for eqbc yourself.
 ***|
#bind command_brd_travel /travel
sub Bind_command_brd_travel(bool _debug)
	DEBUG \atBind_command_brd_travel\ax
	FLOW Bind_command_brd_travel

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

	| use rest song
	} else /if (${_type.Equal[rest]}) {
		/invoke ${set_switch_env[${_debug}, swSongRest, TRUE, ${_verbage}]}

	| use aa bellow
	} else /if (${_type.Equal[bellow]}) {
		/invoke ${set_switch_env[${_debug}, swAABellow, TRUE, ${_verbage}]}

	| sing ooc
	} else /if (${_type.Equal[ooc]}) {
		/invoke ${set_switch_env[${_debug}, swPulseOOC, TRUE, ${_verbage}]}


	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE


