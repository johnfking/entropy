|***
 * entropy
 * class.enc.inc
 ***|



|***
 * note: primary initilization of all enchanter non shared data
 * use: not for you
 ***|
sub set_ENC(bool _debug)
	DEBUG \atset_ENC\ax()

	/call set_data FALSE maData stSpire																	map outer "Spire of Enchantment"
	/call set_data FALSE maData stSynergy																map outer "Begulier's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Staff of the Serpent"
	/call set_data FALSE maData stEpic15																map outer "Oculus of Persuasion"
	/call set_data FALSE maData stEpic2																	map outer "Staff of Eternal Eloquence"
	/call set_data FALSE maData stFade																	map outer "Self Stasis"
	/call set_data_map FALSE maChr stAAUnity														"Orator's Unity"								"${auxna} class aa unity buff"

	| map for mob control
	/declare maMobControl map outer

	/call set_data_map FALSE maChr stPctGather													0															"${auxnum} % of your mana to usin gather mana AA and crystal if enabled"

	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance spell"

	/call set_data_map FALSE maChr swAABoS															FALSE													"${auxsw} use aa Beam of Slumber in your mez routines"

	| single target charm
	/call set_data_map FALSE maChr stCharm															PREGEN|1											"${auxna} name of your charm spell"
	/call set_data_map FALSE maChr swCharm															FALSE													"${auxsw} enable using charm"

  | nights endless terror
  /call set_data_map FALSE maChr stTerror															PREGEN|1											"${auxna} name of terror spell"
  /call set_data_list FALSE maChr lsTerrorToon												EMPTY													"${auxli} lsit of toons you want to keep terror buff on"

	/call set_data_map FALSE maChr stPctGather				 									0															"${auxnum} % of manna you want to use gather at"

	| aa crystals
  /invoke ${set_data_timer[FALSE, Check_Crystals, 10s]}
	/call set_data_map FALSE maChr swAACrystals													FALSE													"${auxsw} use aa crystals:\a-wAzure Mind Crystal, Sanguine Mind Crystal/ax"

	| PBAE stun
	/call set_data_map FALSE maChr stAEStun															PREGEN|1											"${auxna} name of your point blank ae stun"

/return TRUE



|***
 * note: main enchanter routine
 * use:
 ***|
sub main_ENC(bool _debug)
	DEBUG \atmain_ENC\ax()


	/if (AUTO) {
		/if (${maChr.Find[swAACrystals].Value} && !${timer_Check_Crystals}) {
			/call check_crystals ${maDebug.Find[crystal].Value}
		}
		/if (${maChr.Find[stPctGather].Value}) {
			/call check_gather_${Me.Class.ShortName} ${maDebug.Find[gather].Value}
		}
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) {
			/call check_minion_cycle ${maDebug.Find[petbuild].Value}
		}
	}


	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		/call control_routine ${maDebug.Find[mez].Value}
		/call check_heal_routines
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/call set_combat FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) {
				/call cast_defensive_cycle ${maDebug.Find[def].Value}
			}
			/if (!${timer_Check_deBuffs}) /call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			/call control_routine ${maDebug.Find[mez].Value}

			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET

			/call control_routine ${maDebug.Find[mez].Value}

			/if (DOT) /call cast_dot ${maDebug.Find[dot].Value} lsDoT TARGET
			/call control_routine ${maDebug.Find[mez].Value}
			/if (NUKE) /call cast_nuke ${maDebug.Find[nuke].Value} lsNuke TARGET
			/call control_routine ${maDebug.Find[mez].Value}
			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTerror lsTerrorToon
			}
			/call control_routine ${maDebug.Find[mez].Value}
			| /if (${swAlliance}) /call trigger_alliance TARGET

			/call check_item_cycle FALSE ${maDebug.Find[item].Value}

		}

	}

	/if (AUTO) {
		/if (${maChr.Find[swAACrystals].Value} && !${timer_Check_Crystals}) {
			/call check_crystals ${maDebug.Find[crystal].Value}
		}
		/if (${maChr.Find[stPctManaRecovery].Value}) {
			/call do_mana_recovery ${maDebug.Find[manarecover].Value}
		}
		/if (${maChr.Find[stPctGather].Value}) {
			/call check_gather_${Me.Class.ShortName} ${maDebug.Find[gather].Value}
		}
		| /if (${switchCharmPet} && ${swPet} && !${Me.Pet.ID}) {
		| 	/call find_mob_to_charm
		| }
	}

/return TRUE




|***
 * note: basic DPS discs
 * use:
 ***|
sub misc_combat_ENC(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_${Me.Class.ShortName\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) {
		/call cast_defensive_cycle ${maDebug.Find[def].Value}
	}


	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	| use AA beam of slumber?
	/if (${maChr.Find[swAABoS].Value} && ${madeBuff.Find[swMez].Value}) {
		/if (${Spawn[${_tmpID}].NearestSpawn[3,npc radius 30].ID} && ${Spawn[${_tmpID}].Distance3D} <= 200 && ${Spawn[TARGET].Distance3D} >= ENV*.5) {
			/if (${Me.AltAbilityReady[Beam of Slumber]}) {
				/if (${Me.Casting.ID}) /stopcast
				/face ID ${_tmpID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
				/if (${is_ready[${_debug}, "Beam of Slumber"]}) /call voodoo ${_debug} "Beam of Slumber" 0 FALSE
			}
		}
	}

	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	| swarm pet
	/if (SWARM && AGRO) {
		/if (${is_ready[${_debug}, "Phantasmal Opponent"]} && ${Target.PctHPs} <= ${maMinion.Find[stPctSwarmEngage].Value}) {
			/call voodoo ${_debug} "Phantasmal Opponent" 0 FALSE
		}
	}

	| group DPS bonus
	/if (${is_ready[${_debug}, "Chromatic Haze"]}) {
		/call voodoo ${_debug} "Chromatic Haze" 0 FALSE
	}

/return TRUE



|***
 * note: Keep the chanter stocked with self AA crystals
 * use: /call check_crystals DEBUG
 ***|
sub check_crystals(bool _debug)
	DEBUG \atcheck_crystals\ax()

	/invoke ${set_data_timer[FALSE, Check_Crystals, RESTART]}

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE

	| mana crystal
	/if (!${FindItemCount[${Spell[Azure Mind Crystal].Base[1]}]}) {
		/if (${is_ready[${_debug}, "Azure Mind Crystal"]}) {
			/call voodoo ${_debug} "Azure Mind Crystal" 0 FALSE
		}
		/autoinventory
		| /call check_cursor ${_debug} CLEAR
	}

	| hp crystal
	/if (!${FindItemCount[${Spell[Sanguine Mind Crystal].Base[1]}]}) {
		/if (${is_ready[${_debug}, "Sanguine Mind Crystal"]}) {
			/call voodoo ${_debug} "Sanguine Mind Crystal" 0 FALSE
		}
		/autoinventory
		| /call check_cursor ${_debug} CLEAR
	}

	/if (!${target_reset[ ${_debug}, TARGET, all]}) /return FALSE

/return TRUE



|***
 * note: needs manna!!!
 * use: /call check_gather DEBUG
 ***|
sub check_gather_ENC(bool _debug)
	DEBUG \atcheck_gather_${Me.Class.ShortName}\ax()

	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) /return FALSE

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE

	| AA manna crystal
	/if (${maChr.Find[swAACrystals].Value}) {
		/if (${is_ready[${_debug}, "${FindItem[${Spell[Azure Mind Crystal].Base[1]}].Name}"]}) {
			/call voodoo ${_debug} "${FindItem[${Spell[Azure Mind Crystal].Base[1]}].Name}" 0 FALSE
		}
	}

	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) /return FALSE

	| aa Gather Mana
	/if (${is_ready[${_debug}, "Gather Mana"]}) {
		/call voodoo ${_debug} "Gather Mana" 0 FALSE
	}

	/if (!${target_reset[ ${_debug}, TARGET, all]}) /return FALSE

/return TRUE



|***
 * note: chanter self healing? like this is really gonna work. wtf
 * use: /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
 ***|
sub check_heal_ENC(bool _debug)
	DEBUG \atcheck_heal_${Me.Class.ShortName}\ax()
	/if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) /return FALSE

	ISMEDEAD
	CHECKEXIT
	CHECKTIE

	| AA Sanguine mind crystal summoned item 5 min
	/if (${maChr.Find[swAACrystals].Value}) {
		/if (${is_ready[${_debug}, "${healCrystal}"]}) {
			/call voodoo ${_debug} "${healCrystal}" 0 FALSE
			/return
		}
	}

	| 3 decoys 10 min
	/if (${is_ready[${_debug}, "Doppelganger"]}) {
		/call voodoo ${_debug} "Doppelganger" ${Me.ID} FALSE
		/return
	}

	| rune and anti hate 20 min
	/if (${is_ready[${_debug}, "Dimensional Shield"]}) {
		/call voodoo ${_debug} "Dimensional Shield" 0 FALSE
		/return
	}

/return TRUE



|***
 * note: toast should not be burnt
 * use: establish all comp/trigger skills
 ***|
sub burn_ENC(bool _debug, int _tmpID)
	DEBUG \atburn_${Me.Class.ShortName\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	/if (${is_ready[${_debug}, "Illusions of Grandeur"]}) {
		/call voodoo ${_debug} "Illusions of Grandeur" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Mental Contortion"]}) {
		/call voodoo ${_debug} "Mental Contortion" ${_tmpID} FALSE
	}

	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "Improved Twincast"]}) {
		/call voodoo ${_debug} "Improved Twincast" 0 FALSE
	}

	| # DD spell damage bonus
	/if (${is_ready[${_debug}, "Calculated Insanity"]}) {
		/call voodoo ${_debug} "Calculated Insanity" 0 FALSE
	}

	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	/if (${is_ready[${_debug}, "Silent Casting"]}) {
		/call voodoo ${_debug} "Silent Casting" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Dimensional Shield"]}) {
		/call voodoo ${_debug} "Dimensional Shield" 0 FALSE
	}

	| resist mod
	/if (${is_ready[${_debug}, "Focus of Arcanum"]}) /call voodoo ${_debug} "Focus of Arcanum" 0 FALSE

	| Group DD Bonus
	/if (${is_ready[${_debug}, "Chromatic Haze"]}) /call voodoo ${_debug} "Chromatic Haze" 0 FALSE

	/if (!${target_reset[ ${_debug}, ${_tmpID}, all]}) /return FALSE

	/call cast_glyph ${maDebug.Find[glyph].Value}

/return



|***
 *	DES: Charm Mobs. cause fucking charmed mobs and shits
 *	USE: /call find_mob_to_charm()
 *	NOTE:

sub find_mob_to_charm(bool _debug)
	DEBUG \a-pfind_mob_to_charm\ax()

	/declare _count int local 0
	/declare _spawns int local 0
	/declare _tmpID int local 0
	/declare _maxlvl int local ${Int[${Math.Calc[${Me.Level}-1]}]}

	/delay 1

	/if (${SpawnCount[npc los Range 1 ${_maxlvl} radius ENV targetable]}) {
		/varset _spawns ${SpawnCount[npc los Range 1 ${_maxlvl} radius ENV targetable]}
		/for _count 1 to ${_spawns}
			/varset _tmpID ${NearestSpawn[${_count}, npc los Range 1 ${_maxlvl} radius ENV targetable].ID}
			/if (${CharmMobList.Find[${Spawn[id ${_tmpID}].DisplayName}]} && !${Spawn[${_tmpID}].Master.Type.Equal[Pet]} && !${Spawn[${_tmpID}].Master.Type.Equal[Corpse]}) {

				/squelch /target ID ${_tmpID}
				/if (!${Target.Tashed.Duration}) /call cast_tash ${_tmpID}
				/if (${is_ready[${_debug}, spell, "${spellpbaeMez}", ${Me.ID}]}) /call voodoo ${_debug} "${spellpbaeMez}" 0 0 check_for_adds

				/if (${Target.Level} <= 46) {
					/if (${is_ready[${_debug}, alt, "Dire Charm", ${_tmpID}]}) /call voodoo ${_debug} "Dire Charm" alt ${_tmpID} FALSE
				} else {
					/if (${is_ready[${_debug}, spell, "${spellCharm}", ${_tmpID}]}) /call voodoo ${_debug} "${spellCharm}" 0 ${_tmpID} FALSE
				}

				/if (${Defined[timer_mez_ID${_tmpID}]}) /deletevar timer_mez_ID${_tmpID}
				/break
			}
		/next _count
	}
/return
 ***|







|***
 * note: class specific control
 * use: /chr
 ***|
sub set_control_ENC(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_${Me.Class.ShortName}\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
		/return

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

	| use mana click items
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return

	| set gather use %
	} else /if (${_type.Equal[gather]}) {
		/if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return

	| use AA crystals
	} else /if (${_type.Equal[crystal]}) {
		/invoke ${set_switch_env[${_debug}, swAACrystals, TRUE, ${_verbage}]}

	| use AA Beam of Slumber
	} else /if (${_type.Equal[bos]}) {
		/invoke ${set_switch_env[${_debug}, swAABoS, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE



