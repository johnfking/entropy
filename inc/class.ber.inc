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
		
	/call set_data FALSE maData stSpire1																map outer "Spire of lhe Juggernaut"
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

/return



|***
 * note: i have one purpose,  otherwise i'm useless
 * use: 
 ***|
sub main_BER(bool _debug)
	DEBUG main_BER()
	
	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ

	/if (AUTO) {
		| /if (!${timer_click_poison}) /call click_poison FALSE
		/if (${maEnv.Find[swSoS].Value}) /call set_sos FALSE
	}
	
	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call do_sneak_attack FALSE
			/call set_combat FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (MELEE && !${timer_Check_Behind}) /call stick_to_target FALSE TARGET 
			/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (BURN) /call burn_${Me.Class.ShortName} FALSE
			/call check_item_cycle FALSE TARGET

		}

		/if (AUTO) {	
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery
		}
	}

/return



|***
 * note: 
 * use: 
 ***|
sub misc_combat_BER(bool _debug)
	DEBUG misc_combat_BER()

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return
		
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE

	/if (${Me.PctHPs} < 90) {
		/if (${is_ready[FALSE, "${maChr.Find[stBattleFrenzy].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stBattleFrenzy].Value}" TARGET FALSE
		}
	}
	
	/if (${is_ready[FALSE, "${maChr.Find[stSappingStrikes].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stSappingStrikes].Value}" 0 FALSE
	}
	
	/if (${Me.Buff[Frenzied Resolve].ID} && ${Spawn[id TARGET].Distance} > ${Spawn[id TARGET].MaxRangeTo}*.8) {
		/if (${is_ready[FALSE, "Drawn to Blood"]}) {
			/call voodoo FALSE "Drawn to Blood" 0 FALSE
		}
	}
	
	/if (!${Me.Song[Battle Leap].ID}) {
		/if (${is_ready[FALSE, "Battle Leap"]} && !${Me.Underwater}) /call voodoo FALSE "Battle Leap" 0 FALSE
	}
	
	/if (!BURN) {
		/if (${is_ready[FALSE, "${maChr.Find[stDisconcerting].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stDisconcerting].Value}" 0 FALSE			
		}
		/if (${is_ready[FALSE, "Reckless Abandon"]}) {
			/call voodoo FALSE "Reckless Abandon" 0 FALSE
		}
	}
		
	/if (${is_ready[FALSE, "Binding Axe"]}) {
		/call voodoo FALSE "Binding Axe" 0 FALSE
	}
	
	/if (AE#) {
		/if (${is_ready[FALSE, "Rampage"]} && ${Me.Song[Strike of Savagery].ID}) {
			/call voodoo FALSE "Rampage" 0 FALSE
		}
	}
		
	/if (!${Me.Song[Reckless Abandon].ID}) {
		/if (${is_ready[FALSE, "Vehement Rage"]}) /call voodoo FALSE "Vehement Rage" 0 FALSE
	}

	| to agro or not ?
	/if (!AGRO) {
		/if (${is_ready[FALSE, "Distraction Attack"]} && ${Me.PctAggro} > 96) /call voodoo FALSE "Distraction Attack" 0 FALSE
		/if (${is_ready[FALSE, "${maChr.Find[stDivertiveStrike].Value}"]} && ${Me.PctAggro} > 80) /call voodoo FALSE "${maChr.Find[stDivertiveStrike].Value}" TARGET FALSE
	}

	/if (${Me.PctHPs} < 90) {
		/if (${is_ready[FALSE, "${maChr.Find[stOverpoweringFrenzy].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stOverpoweringFrenzy].Value}" TARGET FALSE
		}
	}
		
	| endurance regeneration
	/if (${Me.PctHPs} > 75 && ${Me.PctEndurance} < 50) {
		/if (${is_ready[FALSE, "Communion of Blood"]}) {
			/call voodoo FALSE "Communion of Blood" 0 FALSE
		}
	}
	
	| this hits up to 8 targets and can cause fear.
	/if (AE#) {
		/if (${is_ready[FALSE, "War Cry of the Braxi"]}) {
			/call voodoo FALSE "War Cry of the Braxi" 0 FALSE
		}
	}

	| dumb ass zerker has a dicho and a dicho ##. cant use the name to cast with..
	/if (${is_ready[FALSE, "${maChr.Find[stDichotomic].Value}"]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stDichotomic].Value}].EnduranceCost}*2) {
		/call voodoo FALSE "${maChr.Find[stDichotomic].Value}" TARGET FALSE
	}
	
	/if (AE#) {
		/if (${is_ready[FALSE, "${maChr.Find[stViciousSpiral].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stViciousSpiral].Value}" 0 FALSE
		}
	}

	/if (${Me.PctHPs} > 90 && !${Me.Song[${maChr.Find[stOpenWound].Value}].ID}) {
		/if (${is_ready[FALSE, "Bloodfury"]}) /call voodoo FALSE "Bloodfury" 0 FALSE
	}
	
	/if (${is_ready[FALSE, "${maChr.Find[stAugmentedFrenzy].Value}"]} && ${Me.PctHPs} < 90) {
		/call voodoo FALSE "${maChr.Find[stAugmentedFrenzy].Value}" 0 FALSE
	}
	/if (${is_ready[FALSE, "${maChr.Find[stCryHavoc].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stCryHavoc].Value}" 0 FALSE
	}
	/if (${is_ready[FALSE, "${maChr.Find[stFesteringRage].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stFesteringRage].Value}" 0 FALSE
	}
	
	| swarm pet
	/if (SWARM) {
		/if (${is_ready[FALSE, "${maChr.Find[stPhantomAssailant].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stPhantomAssailant].Value}" TARGET FALSE
		}
	}

	| axes
	/if (${is_ready[FALSE, "${maChr.Find[stRageVolley].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stRageVolley].Value}" TARGET FALSE
	}

	/if (${is_ready[FALSE, "${maChr.Find[stAxeofRalos].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stAxeofRalos].Value}" TARGET FALSE
	}

	/if (${is_ready[FALSE, "${maChr.Find[stAxeThrow].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stAxeThrow].Value}" TARGET FALSE
	}

	/if (!${Me.Song[${maChr.Find[stSharedBloodlust].Value}].ID} && ${Me.TargetOfTarget.ID}) {
		/if (${is_ready[FALSE, "${maChr.Find[stSharedBloodlust].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stSharedBloodlust].Value}" 0 FALSE
		}
	}
/return



|***
 * note: if you are using this.. and people find out.. you will get laughed at. zerkers are glass cannons..
 * use: /heal self
 ***|
sub check_heal_BER(bool _debug, string _type)
	DEBUG check_heal_BER(${_debug}, ${_type})

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

	/if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) /return FALSE

	/call cast_list FALSE lsHealBaseLine ${Me.ID}	
	
	SETHUD FALSE

/return



|***
 * note: Berzerker burn routines
 * use: if you're not burning. you are doing it wrong..
 ***| 
sub burn_BER(bool _debug)
	DEBUG burn_BER()

	CHECKEXIT
	GETINPUT
	
	/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) {
		:loopmakestop
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /stopdisc
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /goto :loopmakestop
	}
	
	/if (${is_ready[FALSE, "Desperation"]} && ${Me.PctHPs} < 90) /call voodoo FALSE "Desperation" 0 FALSE
	/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stUnsettlingScream].Value}].RankName}]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stUnsettlingScream].Value}].EnduranceCost}) {
		/disc ${maChr.Find[stUnsettlingScream].Value}
		/call voodoo FALSE "${maChr.Find[stUnsettlingScream].Value}" TARGET FALSE
	}
		
	/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stCryOfChaos].Value}].RankName}]} && !${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
		/delay ${Math.Rand[8]}s ${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}
		/if (!${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
			/if (${Raid.Members}) {
				/if (${is_ready[FALSE, "Cry of Battle"]}) /call voodoo FALSE "Cry of Battle" 0 FALSE
			}
			/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stCryOfChaos].Value}].RankName}]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stCryOfChaos].Value}].EnduranceCost}) /disc ${maChr.Find[stCryOfChaos].Value}
		}
	} 
	
	| AA rampage choice
	/if (AE# && ${SpawnCount[npc group radius 50 PS4]} >= AE#) {
		/if (${is_ready[FALSE, "Furious Rampage"]}) {
			/call voodoo FALSE "Furious Rampage" 0 FALSE
		}
	} else /if (!AE#) {
		/if (${is_ready[FALSE, "Focused Furious Rampage"]}) {
			/call voodoo FALSE "Focused Furious Rampage" 0 FALSE
		}
		/if (${is_ready[FALSE, "Silent Strikes"]}) {
			/call voodoo FALSE "Silent Strikes" 0 FALSE
		}
	}
	
	/if (${is_ready[FALSE, "Untamed Rage"]}) /call voodoo FALSE "Untamed Rage" 0 FALSE

	/if (${is_ready[FALSE, "Blinding Fury"]}) /call voodoo FALSE "Blinding Fury" 0 FALSE
	/if (${is_ready[FALSE, "Juggernaut Surge"]}) /call voodoo FALSE "Juggernaut Surge" 0 FALSE
	
	/call cast_glyph FALSE
	
	/if (!${Me.Song[Reckless Abandon].ID} && ${Raid.Members}) {
		/if (${is_ready[FALSE, "Intensity of the Resolute"]}) /call voodoo FALSE "Intensity of the Resolute" 0 FALSE
	}
	
	/call cast_AA_spire FALSE
	
	/if (${is_ready[FALSE, "Blood Pact"]}) /call voodoo FALSE "Blood Pact" 0 FALSE

	/if (${Spawn[group PC Shaman].ID}) {
	 	/if (!${Me.Buff[Savage Spirit].ID} && !${Me.ActiveDisc.ID}) {
	 		/if (${is_ready[FALSE, "${maChr.Find[stInspiredAnger].Value}}"]}) /call voodoo FALSE "${maChr.Find[stInspiredAnger].Value}}" 0 FALSE		
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
			/if (${is_ready[FALSE, "${maChr.Find[stInspiredAnger].Value}"]}) /call voodoo FALSE "${maChr.Find[stInspiredAnger].Value}" 0 FALSE		
		}
	}
	
	/if (!${Me.Buff[Savage Spirit].ID} && !${Me.ActiveDisc.ID}) {

		/if (${Me.PctHPs} < 88) {
			/if (${is_ready[FALSE, "${maChr.Find[stResolve].Value}"]}) /call voodoo FALSE "${maChr.Find[stResolve].Value}" 0 FALSE
		}

		/if (!${Me.CombatAbilityReady[${Spell[${maChr.Find[stResolve].Value}].RankName}]}) /disc ${maChr.Find[stVengefulFlurry].Value}
		/if (!${Me.CombatAbilityReady[${Spell[${maChr.Find[stResolve].Value}].RankName}]} && !${Me.CombatAbilityReady[${Spell[${maChr.Find[stVengefulFlurry].Value}].RankName}]}) /disc ${maChr.Find[stCleavingRage].Value}
	}

/return






/if (${li.Value.Equal[hiatus]}) /varset _out ${_out}:${If[${maChr.Find[swHiatus].Value},${on},${off}]}

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/varset dump ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}





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
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[FALSE, stPctFade, "${_verbage}", 0, 99]}) /return

	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire FALSE ${_verbage} ${_silent}
		/return

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/varset dump ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}
		
		
	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset _out
	/vardata li lsChr.First.Clone
	/while (!${li.IsEnd}) {
		/varset _out ${_out} ${If[${li.Value.Equal[${lsChr.First}]},,${dot}]} ${info}${li.Value}\ax
		/if (${li.Value.Equal[ae]}) /varset _out ${_out}:${If[${maChr.Find[stCountAE].Value},${num}${maChr.Find[stCountAE].Value}\ax,${r0}]}
		/if (${li.Value.Equal[def]}) /varset _out ${_out}:${If[${maChr.Find[stCountDefensive].Value},${num}${maChr.Find[stCountDefensive].Value}\ax,${r0}]}
		/if (${li.Value.Equal[fade]}) /varset _out ${_out}:${If[${maChr.Find[stPctFade].Value},${num}${maChr.Find[stPctFade].Value}\ax,${r0}]}
		/if (${li.Value.Equal[spire]}) /varset _out ${_out}:${submenu}
		/if (${li.Value.Equal[hiatus]}) /varset _out ${_out}:${If[${maChr.Find[swHiatus].Value},${on},${off}]}

		/varset dump ${li.Advance}
	}	
	OUT /chr ${_out}

/return

