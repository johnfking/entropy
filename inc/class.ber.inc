|***
 * entropy
 * class.clr.inc
 ***|



|***
 * note: primary initilization of all zerker non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
	DEBUG \atclass_init\ax()

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

	| AAs
	/call set_data_map ${_debug} maChr stAADrawntoBlood									PREGEN|1											"${auxna} Drawn to Blood aa"
	/call set_data_map ${_debug} maChr stAABattleLeap										PREGEN|1											"${auxna} Battle Leap aa"
	/call set_data_map ${_debug} maChr stAARecklessAbandon							PREGEN|1											"${auxna} Reckless Abandon aa"
	/call set_data_map ${_debug} maChr stAABindingAxe										PREGEN|1											"${auxna} Binding Axe aa"
	/call set_data_map ${_debug} maChr stAARampage											PREGEN|1											"${auxna} Rampage aa"
	/call set_data_map ${_debug} maChr stAAVehementRage									PREGEN|1											"${auxna} Vehement Rage aa"
	/call set_data_map ${_debug} maChr stAACommunionofBlood							PREGEN|1											"${auxna} Communion of Blood aa"
	/call set_data_map ${_debug} maChr stAABraxisHowl										PREGEN|1											"${auxna} Braxi's Howl aa"
	/call set_data_map ${_debug} maChr stAACryofBattle									PREGEN|1											"${auxna} Cry of Battle aa"
	/call set_data_map ${_debug} maChr stAAFuriousRampage								PREGEN|1											"${auxna} Furious Rampage aa"
	/call set_data_map ${_debug} maChr stAASilentStrikes								PREGEN|1											"${auxna} Silent Strikes aa"
	/call set_data_map ${_debug} maChr stAAFocusedFuriousRampage				PREGEN|1											"${auxna} Focused Furious Rampage aa"
	/call set_data_map ${_debug} maChr stAAJuggernautSurge							PREGEN|1											"${auxna} Juggernaut Surge aa"
	/call set_data_map ${_debug} maChr stAABlindingFury									PREGEN|1											"${auxna} Blinding Fury aa"
	/call set_data_map ${_debug} maChr stAABloodPact										PREGEN|1											"${auxna} Blood Pact aa"
	/call set_data_map ${_debug} maChr stAADesperation									PREGEN|1											"${auxna} Desperation aa"
	/call set_data_map ${_debug} maChr stAABloodfury										PREGEN|1											"${auxna} Bloodfury aa"
	/call set_data_map ${_debug} maChr stAAUntamedRage									PREGEN|1											"${auxna} Untamed Rage aa"
	/call set_data_map ${_debug} maChr stAARecklessAbandon							PREGEN|1											"${auxna} Reckless Abandon aa"
	/call set_data_map ${_debug} maChr DistractionAttack								PREGEN|1											"${auxna} Distraction Attack aa"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: i have one purpose,  otherwise i'm useless
 * use:
 ***|
sub class_main(bool _debug)
	DEBUG \atclass_main\ax()

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
		/if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

		/if (VALID && !ENGINE3) {
			/call do_sneak_attack ${maDebug.Find[class].Value} TARGET
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
			CHECKDEFENSE
			/if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
			/call misc_combat ${maDebug.Find[misc].Value} TARGET
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
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
		} else /if (AUTO && !ENGINE3) {
			/if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
				/call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
			}
		}
	}

/return TRUE



|***
 * note: suprize but slap?
 * use: designed for a first attack
 ***|
sub do_sneak_attack(bool _debug, int _tmpID)
	DEBUG \atdo_sneak_attack\ax(\a-w${_tmpID}\ax)
	FLOW do_sneak_attack

  /if (${Select[${Me.CombatState},active,debuffed]}) {
		/return FALSE
  }

	/if (!${is_ready[${_debug}, "${maChr.Find[stKickintheShins].Value}"]}) {
		/return FALSE
	}

	CHECKEXIT

	/if (${Target.Distance} <= 49 && ${Target.Distance} <= ENV) {
		/call voodoo ${_debug} "${maChr.Find[stKickintheShins].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note:
 * use:
 ***|
sub misc_combat(bool _debug, int _tmpID)
	DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat

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

	/if (!${Me.Song[${maChr.Find[stSharedBloodlust].Value}].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stSharedBloodlust].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stSharedBloodlust].Value}" 0 FALSE
		}
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

	/if (${Me.ActiveDisc[Frenzied Resolve].ID} && ${Spawn[id ${_tmpID}].Distance} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.8 && ${Spawn[id ${_tmpID}].Distance} < 60) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAADrawntoBlood].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAADrawntoBlood].Value}" 0 FALSE
		}
	}

	/if (!${Me.Song[${maChr.Find[stAABattleLeap].Value}].ID} && !${Me.Song[Group Bestial Alignment].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAABattleLeap].Value}"]} && !${Me.Underwater}) {
			/call voodoo ${_debug} "${maChr.Find[stAABattleLeap].Value}" 0 FALSE
		}
	}

	/if (!BURN) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDisconcerting].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDisconcerting].Value}" 0 FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stAARecklessAbandon].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAARecklessAbandon].Value}" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAABindingAxe].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAABindingAxe].Value}" ${_tmpID} FALSE
	}

	/if (AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAARampage].Value}"]} && ${Me.Song[Strike of Savagery].ID}) {
			/call voodoo ${_debug} "${maChr.Find[stAARampage].Value}" 0 FALSE
		}
	}

	/if (!${Me.Song[${maChr.Find[stAARecklessAbandon].Value}].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAVehementRage].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAVehementRage].Value}" 0 FALSE
		}
	}

	| to agro or not ?
	/if (!AGRO) {
		/if (${is_ready[${_debug}, "${maChr.Find[DistractionAttack].Value}"]} && ${Me.PctAggro} > 96) {
			/call voodoo ${_debug} "${maChr.Find[DistractionAttack].Value}" 0 FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stDivertiveStrike].Value}"]} && ${Me.PctAggro} > 80) {
			/call voodoo ${_debug} "${maChr.Find[stDivertiveStrike].Value}" ${_tmpID} FALSE
		}
	}

	/if (${Me.PctHPs} < 90) {
		/if (${is_ready[${_debug}, "${maChr.Find[stOverpoweringFrenzy].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stOverpoweringFrenzy].Value}" 0 FALSE
		}
	}

	| endurance regeneration
	/if (${Me.PctHPs} > 75 && ${Me.PctEndurance} < 50) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAACommunionofBlood].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAACommunionofBlood].Value}" 0 FALSE
		}
	}

	| this hits up to 8 targets and can cause fear.
	/if (${is_ready[${_debug}, "${maChr.Find[stAABraxisHowl].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAABraxisHowl].Value}" 0 FALSE
	}

	| serper dicho
	/if (${is_ready[${_debug}, "${maChr.Find[stDichotomic].Value}"]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stDichotomic].Value}].EnduranceCost}*2) {
		/call voodoo ${_debug} "${maChr.Find[stDichotomic].Value}" ${_tmpID} FALSE
	}

	/if (AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stViciousSpiral].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stViciousSpiral].Value}" 0 FALSE
		}
	}

	/if (${Me.PctHPs} > 90 && !${Me.Song[${maChr.Find[stOpenWound].Value}].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAABloodfury].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
		}
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


/return TRUE



|***
 * note: if you are using this.. and people find out.. you will get laughed at. zerkers are glass cannons..
 * use: /heal self
 ***|
sub check_heal(bool _debug, string _type)
	DEBUG \atcheck_heal\ax(\a-w${_type}\ax)
	FLOW check_heal ${_type}

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
 *  ramp selection
 *  savage spirit
 *  berserking
 *  jugg surge
 *  spire
 *  if no ancient blinding
 *  blood pact
 *  mashing skills
 *  no berserking use cleaving anger and alt act 3710
 *  no cleaving use vengful and glyph
 *  no vengful use frenzied and drawn
 ***|
sub burn(bool _debug, int _tmpID)
	DEBUG \atburn\ax(\aw${_tmpID}\ax)
	FLOW burn

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

	/if (!${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]} && !${Me.Buff[${maChr.Find[stResolve].Value}].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stDisconcerting].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stDisconcerting].Value}" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]} || ${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} || ${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} || ${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]}) {
		:loopmakestop
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /stopdisc
		/delay 5
		/if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stDisconcerting].Value}]}) /goto :loopmakestop
	}

  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

	/if (${is_ready[${_debug}, "${maChr.Find[stCryOfChaos].Value}"]} && !${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
		/delay ${Math.Rand[8]}s ${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}
		/if (!${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
			/if (${Raid.Members}) {
				/if (${is_ready[${_debug}, "${maChr.Find[stAACryofBattle].Value}"]}) {
					/call voodoo ${_debug} "${maChr.Find[stAACryofBattle].Value}" 0 FALSE
				}
			}
			/if (${is_ready[${_debug}, "${maChr.Find[stCryOfChaos].Value}"]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stCryOfChaos].Value}].EnduranceCost}) {
				/call voodoo ${_debug} "${maChr.Find[stCryOfChaos].Value}" 0 FALSE
			}
		}
	}

	| AA rampage choice
	/if (AE# && ${SpawnCount[npc group radius 50 PS4]} >= AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAFuriousRampage].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAFuriousRampage].Value}" 0 FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stAASilentStrikes].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAASilentStrikes].Value}" 0 FALSE
		}
	} else /if (!AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAAFocusedFuriousRampage].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAFocusedFuriousRampage].Value}" 0 FALSE
		}
		/if (${is_ready[${_debug}, "${maChr.Find[stAASilentStrikes].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAASilentStrikes].Value}" 0 FALSE
		}
	}

	| Brutal/berserking and savage spirit
	/if (${Spawn[group PC Shaman].ID}) {
	 	/if (!${Me.Buff[Savage Spirit].ID} && !${Me.ActiveDisc.ID}) {
	 		/if (${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
	 			/call voodoo ${_debug} "${maChr.Find[stInspiredAnger].Value}" 0 FALSE
	 		}
	 	}
		/if (${Me.ActiveDisc.ID} && ${Me.AltAbilityReady[Savage Spirit]}) {
		 /alt act 465
		 /delay 5
		 /return
		}
	}

	| juggernaught surge
	/if (${is_ready[${_debug}, "${maChr.Find[stAAJuggernautSurge].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAJuggernautSurge].Value}" 0 FALSE
	}

	| Spire control
	/call cast_AA_spire ${maDebug.Find[spire].Value}

	| Blinding
	/if (${is_ready[${_debug}, "${maChr.Find[stAABlindingFury].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAABlindingFury].Value}" 0 FALSE
	}

	| blood pact
	/if (${is_ready[${_debug}, "${maChr.Find[stAABloodPact].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAABloodPact].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAADesperation].Value}"]} && !${Me.Song[${maChr.Find[stCryOfChaos].Value}].ID}) {
		/if (${Me.PctHPs} > 90 && !${Me.Song[${maChr.Find[stOpenWound].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAABloodfury].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
			}
		}
		/if (${Me.PctHPs} < 90) {
			/call voodoo ${_debug} "${maChr.Find[stAADesperation].Value}" 0 FALSE
		}
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAAUntamedRage].Value}"]} && !${Spawn[group PC Bard].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stAAUntamedRage].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, ${maChr.Find[stUnsettlingScream].Value}]} && ${Me.CurrentEndurance} > ${Spell[${maChr.Find[stUnsettlingScream].Value}].EnduranceCost}) {
		/call voodoo ${_debug} "${maChr.Find[stUnsettlingScream].Value}" ${_tmpID} FALSE
	}

	|no berserking use cleaving anger and reckless abandon
	/if (${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} && !${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stCleavingRage].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAARecklessAbandon].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAARecklessAbandon].Value}" 0 FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stAAVehementRage].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAVehementRage].Value}" 0 FALSE
	}

	|no cleaving use vengful and glyph
	/if (${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} && !${Me.ActiveDisc.ID} && ${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stVengefulFlurry].Value}" 0 FALSE
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}

	|no vengful use frenzied and drawn
	/if (!${Me.ActiveDisc.ID} && !${is_ready[${_debug}, "${maChr.Find[stInspiredAnger].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stVengefulFlurry].Value}"]} && !${is_ready[${_debug}, "${maChr.Find[stCleavingRage].Value}"]}) {
		/if (${Me.PctHPs} > 90 && !${Me.Song[${maChr.Find[stOpenWound].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAABloodfury].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAABloodfury].Value}" 0 FALSE
			}
		}
		/if (${Me.PctHPs} < 90) {
			/if (${is_ready[${_debug}, "${maChr.Find[stResolve].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stResolve].Value}" 0 FALSE
			}
		}
	}


/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
	DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

	/call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
	/if (${Macro.Return.Equal[SKIP]}) /return FALSE

	/if (${_verbage2.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

