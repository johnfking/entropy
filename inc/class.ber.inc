|***
 * entropy
 * class.clr.inc
 ***|



|***
 * note: primary initilization of all zerker non shared data
 * use: not for you
 ***|
sub set_BER(bool _debug)
	DEBUG \atset_BER\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of the Juggernaut"
	/call set_data FALSE maData stSynergy																map outer "Fury's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Kerasian Axe of Ire"
	/call set_data FALSE maData stEpic15																map outer "Raging Taelosian Alloy Axe"
	/call set_data FALSE maData stEpic2																	map outer "Vengeful Taelosian Blood Axe"
	/call set_data FALSE maData stFade																	map outer "Self Preservation"
	/call set_data_map FALSE maChr stAAUnity														FALSE													"${auxna} class aa unity buff"

 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"

	/call set_data_map FALSE maChr stRageVolley 												PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stCryOfChaos 												PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stInspiredAnger											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stOverpoweringFrenzy 								PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stUnsettlingScream 									PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stAxeThrow 													PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stDivertiveStrike 										PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stSharedBloodlust 										PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stBattleFrenzy												PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stCleavingRage												PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stKickintheShins											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stAxeofRalos													PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stVengefulFlurry											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stVengefulFlurry											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stViciousSpiral											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stCryHavoc														PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stAugmentedFrenzy										PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stFesteringRage 											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stResolve														PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stDichotomic													PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stDisconcerting											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stSappingStrikes											PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stPhantomAssailant										PREGEN|1											"${auxna} "
	/call set_data_map FALSE maChr stOpenWound													PREGEN|1											"${auxna} "

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: i have one purpose,  otherwise i'm useless
 * use:
 ***|
sub main_BER(bool _debug)
	DEBUG \atmain_BER\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ

	/if (AUTO && !ENGINE3) {
	} else /if (AUTO && ENGINE3) {
		/if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
			/call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
		}
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		CHECKDEFENSE
		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return

		/if (VALID && !ENGINE3) {
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			CHECKDEFENSE
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		} else /if (VALID && ENGINE3) {
			/if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
				/call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}

		/if (AUTO && !ENGINE3) {
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery
		} else /if (AUTO && !ENGINE3) {
			/if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
				/call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}
	}

/return TRUE



|***
 * note:
 * use:
 ***|
sub misc_combat_BER(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_BER\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat_BER

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT
	GETINPUT
	CHECKTIE

	CHECKDEFENSE

	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	}

	/if (ENGINE2) {
		/call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
		/return TRUE
	}

	/if (${Me.PctHPs} < 90) {
		/if (${is_ready[${_debug}, "${maChr.Find[stBattleFrenzy].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stBattleFrenzy].Value}" ${_tmpID} FALSE
		}

		/if (${is_ready[${_debug}, "${maChr.Find[stAugmentedFrenzy].Value}"]} ) {
			/call voodoo ${_debug} "${maChr.Find[stAugmentedFrenzy].Value}" 0 FALSE
		}

	}

	/if (${is_ready[${_debug}, "${maChr.Find[stSappingStrikes].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stSappingStrikes].Value}" 0 FALSE
	}

	/if (${Me.ActiveDisc[Frenzied Resolve].ID} && ${Spawn[id ${_tmpID}].Distance} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.8) {
		/if (${is_ready[${_debug}, "Drawn to Blood"]}) {
			/call voodoo ${_debug} "Drawn to Blood" 0 FALSE
		}
	}

	/if (!${Me.Song[Battle Leap].ID} && !${Me.Song[Group Bestial Alignment].ID}) {
		/if (${is_ready[${_debug}, "Battle Leap"]} && !${Me.Underwater}) {
			/call voodoo ${_debug} "Battle Leap" 0 FALSE
		}
	}

	/if (!BURN) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDisconcerting].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDisconcerting].Value}" 0 FALSE
		}
		/if (${is_ready[${_debug}, "Reckless Abandon"]}) {
			/call voodoo ${_debug} "Reckless Abandon" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "Binding Axe"]}) {
		/call voodoo ${_debug} "Binding Axe" ${_tmpID} FALSE
	}

	/if (AE#) {
		/if (${is_ready[${_debug}, "Rampage"]} && ${Me.Song[Strike of Savagery].ID}) {
			/call voodoo ${_debug} "Rampage" 0 FALSE
		}
	}

	/if (!${Me.Song[Reckless Abandon].ID}) {
		/if (${is_ready[${_debug}, "Vehement Rage"]}) {
			/call voodoo ${_debug} "Vehement Rage" 0 FALSE
		}
	}

	| to agro or not ?
	/if (!AGRO) {
		/if (${is_ready[${_debug}, "Distraction Attack"]} && ${Me.PctAggro} > 96) /call voodoo ${_debug} "Distraction Attack" 0 FALSE
		/if (${is_ready[${_debug}, "${maChr.Find[stDivertiveStrike].Value}"]} && ${Me.PctAggro} > 80) {
			/call voodoo ${_debug} "${maChr.Find[stDivertiveStrike].Value}" ${_tmpID} FALSE
		}
	}

	/if (${Me.PctHPs} < 90) {
		/if (${is_ready[${_debug}, "${maChr.Find[stOverpoweringFrenzy].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stOverpoweringFrenzy].Value}" ${_tmpID} FALSE
		}
	}

	| endurance regeneration
	/if (${Me.PctHPs} > 75 && ${Me.PctEndurance} < 50) {
		/if (${is_ready[${_debug}, "Communion of Blood"]}) {
			/call voodoo ${_debug} "Communion of Blood" 0 FALSE
		}
	}

	| this hits up to 8 targets and can cause fear.
	/if (${is_ready[${_debug}, "Braxi's Howl"]}) {
		/call voodoo ${_debug} "Braxi's Howl" 0 FALSE
	}

	| dicho
	/if (${is_ready[${_debug}, "${maChr.Find[stDichotomic].Value}"]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stDichotomic].Value}].EnduranceCost}*2) {
		/call voodoo ${_debug} "${maChr.Find[stDichotomic].Value}" ${_tmpID} FALSE
	}

	/if (AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stViciousSpiral].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stViciousSpiral].Value}" 0 FALSE
		}
	}

	/if (${Me.PctHPs} > 90 && !${Me.Song[${maChr.Find[stOpenWound].Value}].ID}) {
		/if (${is_ready[${_debug}, "Bloodfury"]}) /call voodoo ${_debug} "Bloodfury" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stCryHavoc].Value}"]} && !${Me.Buff[${maChr.Find[stCryHavoc].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stCryHavoc].Value}" 0 FALSE
	}
	/if (${is_ready[${_debug}, "${maChr.Find[stFesteringRage].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stFesteringRage].Value}" 0 FALSE
	}

	| swarm pet
	/if (SWARM) {
		/if (${is_ready[${_debug}, "${maChr.Find[stPhantomAssailant].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stPhantomAssailant].Value}" ${_tmpID} FALSE
		}
	}

	| axes
	/if (${is_ready[${_debug}, "${maChr.Find[stRageVolley].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stRageVolley].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAxeofRalos].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAxeofRalos].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAxeThrow].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAxeThrow].Value}" ${_tmpID} FALSE
	}

	/if (!${Me.Song[${maChr.Find[stSharedBloodlust].Value}].ID} && ${Me.TargetOfTarget.ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stSharedBloodlust].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stSharedBloodlust].Value}" 0 FALSE
		}
	}

/return TRUE



|***
 * note: if you are using this.. and people find out.. you will get laughed at. zerkers are glass cannons..
 * use: /heal self
 ***|
sub check_heal_BER(bool _debug, string _type)
	DEBUG \atcheck_heal_BER\ax(\a-w${_type}\ax)
	FLOW check_heal_BER ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) /return FALSE

	/call cast_cycle_heal ${_debug} ${hurtID} healsingle

	SETHUD EMPTY

/return TRUE



|***
 * note: Berzerker burn routines
 * use: if you're not burning. you are doing it wrong..
 ***|
sub burn_BER(bool _debug, int _tmpID)
	DEBUG \atburn_BER\ax(\aw${_tmpID}\ax)
	FLOW burn_BER

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
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


	/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) {
		:loopmakestop
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /stopdisc
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /goto :loopmakestop
	}

	/if (${is_ready[${_debug}, "Desperation"]} && ${Me.PctHPs} < 90) {
		/call voodoo ${_debug} "Desperation" 0 FALSE
	}

	/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stUnsettlingScream].Value}].RankName}]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stUnsettlingScream].Value}].EnduranceCost}) {
		/disc ${maChr.Find[stUnsettlingScream].Value}
		/call voodoo ${_debug} "${maChr.Find[stUnsettlingScream].Value}" ${_tmpID} FALSE
	}

	/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stCryOfChaos].Value}].RankName}]} && !${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
		/delay ${Math.Rand[8]}s ${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}
		/if (!${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
			/if (${Raid.Members}) {
				/if (${is_ready[${_debug}, "Cry of Battle"]}) {
					/call voodoo ${_debug} "Cry of Battle" 0 FALSE
				}
			}
			/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stCryOfChaos].Value}].RankName}]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stCryOfChaos].Value}].EnduranceCost}) {
				/disc ${maChr.Find[stCryOfChaos].Value}
			}
		}
	}

	| AA rampage choice
	/if (AE# && ${SpawnCount[npc group radius 50 PS4]} >= AE#) {
		/if (${is_ready[${_debug}, "Furious Rampage"]}) {
			/call voodoo ${_debug} "Furious Rampage" 0 FALSE
		}
	} else /if (!AE#) {
		/if (${is_ready[${_debug}, "Focused Furious Rampage"]}) {
			/call voodoo ${_debug} "Focused Furious Rampage" 0 FALSE
		}
		/if (${is_ready[${_debug}, "Silent Strikes"]}) {
			/call voodoo ${_debug} "Silent Strikes" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "Untamed Rage"]}) {
		/call voodoo ${_debug} "Untamed Rage" 0 FALSE
	}

	/if (${is_ready[${_debug}, "Blinding Fury"]}) {
		/call voodoo ${_debug} "Blinding Fury" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Juggernaut Surge"]}) {
		/call voodoo ${_debug} "Juggernaut Surge" 0 FALSE
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

	/if (!${Me.Song[Reckless Abandon].ID} && ${Raid.Members}) {
		/if (${is_ready[${_debug}, "Intensity of the Resolute"]}) {
			/call voodoo ${_debug} "Intensity of the Resolute" 0 FALSE
		}
	}

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "Blood Pact"]}) {
		/call voodoo ${_debug} "Blood Pact" 0 FALSE
	}

	/if (${Spawn[group PC Shaman].ID}) {
	 	/if (!${Me.Buff[Savage Spirit].ID} && !${Me.ActiveDisc.ID}) {
	 		/if (${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
	 			/call voodoo ${_debug} "${maChr.Find[stInspiredAnger].Value}" 0 FALSE
	 		}
	 	}
		/if (!${Me.ActiveDisc.ID} && ${Me.AltAbilityReady[Savage Spirit]}) {
		 /alt act 465
		 /delay 5
		 /return
		}
	} else {
		/if (!${Me.ActiveDisc.ID} && ${Me.AltAbilityReady[Savage Spirit]}) {
		 	/alt act 465
		 	/delay 5
		 	/return
		}
		/if (!${Me.Buff[Savage Spirit].ID} && !${Me.ActiveDisc.ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stInspiredAnger].Value}" 0 FALSE
			}
		}
	}

	/if (!${Me.Buff[Savage Spirit].ID} && !${Me.ActiveDisc.ID}) {

		/if (${Me.PctHPs} < 88) {
			/if (${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stResolve].Value}" 0 FALSE
			}
		}

		/if (!${Me.CombatAbilityReady[${Spell[${maChr.Find[stResolve].Value}].RankName}]}) {
			/disc ${maChr.Find[stVengefulFlurry].Value}
		}
		/if (!${Me.CombatAbilityReady[${Spell[${maChr.Find[stResolve].Value}].RankName}]} && !${Me.CombatAbilityReady[${Spell[${maChr.Find[stVengefulFlurry].Value}].RankName}]}) {
			/disc ${maChr.Find[stCleavingRage].Value}
		}
	}

/return TRUE



|***
 * note: rogue specific controls
 * use: /chr
 ***|
sub set_control_BER(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_BER\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

