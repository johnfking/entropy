|***
 * entropy
 * class.clr.inc
 ***|



|***
 * note: primary initilization of all beastlord non shared data
 * use: not for you
 ***|
sub set_BST(bool _debug)
	DEBUG \atset_BST\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data FALSE maData stSpire																	map outer "Spire of the Savage Lord"
	/call set_data FALSE maData stSynergy																map outer "Primalist's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Claw of the Savage Spirit"
	/call set_data FALSE maData stEpic15																map outer "Savage Lord's Totem"
	/call set_data FALSE maData stEpic2																	map outer "Spiritcaller Totem of the Feral"
	/call set_data FALSE maData stFade																	map outer "Playing Possum"
	/call set_data_map FALSE maChr stAAUnity														"Feralist's Unity"						"${auxna} class aa unity buff"

 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr stHiatus															PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"
 	/call set_data_map FALSE maChr stAlliance 													PREGEN|1											"${auxna} alliance spell"

	/call set_data_map FALSE maChr stNukeServant 												PREGEN|1											"${auxna} spam nuke pet spell"
	/call set_data_map FALSE maChr stGrowl 															PREGEN|1											"${auxna} growl buff spell"
  /call set_data_map FALSE maChr stFero 															PREGEN|1											"${auxna} ferocity spell line"
  /call set_data_map FALSE maChr stDichotomic 												PREGEN|1											"${auxna} dichotomic spell"
	/call set_data_map FALSE maChr stBestial			 											PREGEN|1											"${auxna} bestial disc line"
  /call set_data_map FALSE maChr stRending														PREGEN|1											"${auxna} reflexive disc"
  /call set_data_map FALSE maChr stVindication 												PREGEN|1											"${auxna} vindication disc"
  /call set_data_map FALSE maChr stRake 															PREGEN|1											"${auxna} rake disc"
  /call set_data_map FALSE maChr stClaws 															PREGEN|1											"${auxna} claws disc"
  /call set_data_map FALSE maChr stFury 															PREGEN|1											"${auxna} furry disc (ruaabri)"
  /call set_data_map FALSE maChr stSavage 														PREGEN|1											"${auxna} savagery disc"

	/invoke ${set_data_timer[FALSE, Check_Para, 2s]}
 	/call set_data_map FALSE maChr stPctParagonG 												0															"${auxnum} pct mana to use group paragon aa"
	/call set_data_map FALSE maChr stPctParagonF 												0															"${auxnum} pct mana to use focused paragon aa"
	/call set_data_map FALSE maChr stPctConsume 												0															"${auxnum} pct mana to use consume aa"
 	/call set_data_list FALSE maChr lsHealToT		 												PREGEN|3											"${auxli} targets target healing line of spells"

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: main beastlord routine
 * use:
 ***|
sub main_BST(bool _debug)
	DEBUG \atmain_BST\ax()
	FLOW main_BST

	/if (AUTO) {
		/call check_mana_${Me.Class.ShortName}
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
	}

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT
		CHECKREZ
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}

		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}

		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return

		/if (VALID) {
			FLOW main_BST VALID
			/call set_combat_control ${maDebug.Find[decision].Value} TARGET
			/call check_heal_routines
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[temp].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
			/if (${maCC.Find[swAlliance].Value}) /call cast_alliance_${Me.Class.ShortName} TARGET

		}

		/if (AUTO) {
			CHECKFADE
			/if (!${timer_Check_Para}) /call check_mana_BST ${maDebug.Find[para].Value}
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[rest].Value}
		}
	}

/return TRUE



|***
 * note:
 * use: /call misc_combat_BST DEBUG
 ***|
sub misc_combat_BST(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_BST\ax(\a-w${_tmpID}\ax)
	FLOW misc_combat_BST

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	CHECKEXIT misc_combat_BST
	GETINPUT
	CHECKTIE

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

	| do we want the mobs attention?
	/if (AGRO) {
		/if (${is_ready[${_debug}, "Roaring Strike"]}) {
			/call voodoo ${_debug} "Roaring Strike" ${_tmpID} FALSE
		}

	} else /if (!AGRO) {
		| /if (${is_ready[${_debug}, "Chameleon Strike"]}) {
		| 	/call voodoo ${_debug} "Chameleon Strike" ${_tmpID} FALSE
		| }
		/if (${is_ready[${_debug}, "Roar of Thunder"]}) {
			/call voodoo ${_debug} "Roar of Thunder" ${_tmpID} FALSE
		}
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stBestial].Value}"]} && !${Me.Song[${maChr.Find[stBestial].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stBestial].Value}" 0 FALSE
	}

	/if (!AE#) {
		/if (${is_ready[${_debug}, "${maChr.Find[stClaws].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stClaws].Value}" ${_tmpID} FALSE
		}
	}

	/if (${is_ready[${_debug}, "Enduring Frenzy"]} && !${Target.Buff[Enduring Frenzy].ID}) {
		/call voodoo ${_debug} "Enduring Frenzy" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stRake].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stRake].Value}" ${_tmpID} FALSE
	}

	/if (${is_ready[${_debug}, "${maChr.Find[stRending].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stRending].Value}" ${_tmpID} FALSE
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (${Me.Pet.ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stGrowl].Value}"]} && !${Me.Song[${maChr.Find[stGrowl].Value}].ID}) {
			/call voodoo ${_debug} "${maChr.Find[stGrowl].Value}" 0 FALSE
		}
	}

/return TRUE



|***
 * note: Focused Paragon. will maintain the bst
 * use: /call check_mana_${Me.Class.ShortName}
 ***|
sub check_mana_BST(bool _debug)
	DEBUG \atcheck_mana_BST\ax()
	FLOW check_mana_BST


	/invoke ${set_data_timer[${_debug}, Check_Para, RESTART]}

	ISMEDEAD
	CHECKEXIT check_mana_BST

	| consumption of Spirits AA
	/if (${maChr.Find[stPctConsume].Value}) {
		/if (${Me.PctMana} < ${maChr.Find[stPctConsume].Value}*.PCTHO) {
		 	/if (${is_ready[${_debug}, "Consumption of Spirit"]}) {
		 		/call voodoo ${_debug} "Consumption of Spirit" 0 FALSE
		 	}
		}
	}

	/if (${Me.Song[Paragon of Spirit].ID}) /return FALSE
	/if (${Me.Song[Focused Paragon of Spirit].ID}) /return FALSE

	| self paragon
	DEBUG ${sep}checking${sep}\a-wfocused paragon\ax
	/if (${maChr.Find[stPctParagonF].Value}) {
		/if (${Me.PctMana} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO || ${Me.PctEndurance} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "Focused Paragon of Spirits"]}) {
				/call voodoo ${_debug} "Focused Paragon of Spirits" ${Me.ID} FALSE
			}
		}
	}

	/if (${Me.Song[Paragon of Spirit].ID}) /return FALSE
	/if (${Me.Song[Focused Paragon of Spirit].ID}) /return FALSE

	| Paragon Group Members	NOT IN RAID.... if 2+ need stamina or mana
	DEBUG ${sep}checking${sep}\a-wparagon\ax
	| /if (!${Group.Members} || ${Raid.Members} || !${Me.AltAbilityReady[Paragon of Spirit]}) /return TRUE

	/declare _below int local 0
	/declare _liC6 listiterator local
	/vardata _liC6 lsCount6.First.Clone

	/while (!${_liC6.IsEnd}) {
		DEBUG ${sep}${_liC6.Value}${sep}\a-w${Group.Member[${_liC6.Value}].DisplayName}\ax
		ISMEDEAD
		CHECKEXIT check_mana_BST2
		/if (${Group.Member[${_liC6.Value}].Class.ShortName.Equal[BRD]}) {
			/invoke ${_liC6.Advance}
			/continue
		}
		/if (${Group.Member[${_liC6.Value}].PctMana} >= ${maChr.Find[stPctParagonG].Value}*.PCTHO) {
			/invoke ${_liC6.Advance}
			/continue
		}
		/if (${Group.Member[${_liC6.Value}].Distance} > 100) {
			/invoke ${_liC6.Advance}
			/continue
		}
		/if (${Group.Member[${_liC6.Value}].Type.Equal[CORPSE]}) {
			/invoke ${_liC6.Advance}
			/continue
		}

		/varcalc _below ${_below}+1
		/invoke ${_liC6.Advance}
	}

	/if (${_below} >= 1) {
		/call voodoo ${_debug} "Paragon of Spirit" 0 FALSE
	}

/return TRUE



|***
 * note: healing for beastlord
 * use:  when you or your agro whore pet get hurt
 ***|
sub check_heal_BST(bool _debug, string _type)
	DEBUG \atcheck_heal_BST\ax(\a-w${_type}\ax)
	FLOW check_heal_BST ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT check_heal_BST
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return
	}

	| ToT heals
	/if (${Select[${_type},tot]} && ${maHeal.Find[swHealToT].Value}) {
		/if (${hurtCount} >= 1)  {
			/call cast_list ${maDebug.Find[tot].Value} lsHealToT ${hurtID}
			/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
		}
	}

	| PET HEALING
	/if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value}) {
		/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "Mend Companion"]}) {
				/call voodoo ${_debug} "Mend Companion" ${Pet.ID} FALSE
			}
		}

		/call cast_list ${_debug} lsHealPetLine ${Pet.ID}
	}

	| self healing
	/if (${Select[${_type},self]} && ${maHeal.Find[swHealSelf].Value}) {
	 	/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
	 		/if (${is_ready[${_debug}, "Warder's Gift", ${hurtID}]}) {
	 			/call voodoo ${_debug} "Warder's Gift" 0 FALSE
	 		}
		}
	}

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}
	| heal anything spell
	/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
		/call cast_cycle_heal ${_debug} ${hurtID}
	}

	SETHUD EMPTY

/return FALSE



|***
 * note: MGB beast Paragon if both AAs are ready
 *use: /paragon
 ***|
#bind cast_AA_paragon /paragon
sub Bind_cast_AA_paragon(bool _debug)
	/if (${Select[${Me.Class.ShortName},BST]}) {
		/call cast_AA_paragon
	}
/return


sub cast_AA_paragon(bool _debug)
	DEBUG \atcast_AA_paragon\ax()
	FLOW cast_AA_paragon

	ISMEDEAD
	CHECKEXIT Bind_cast_AA_paragon
	GETINPUT

	/if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[Paragon of Spirit]} && !${Me.Song[Paragon of Spirit].ID}) {
		/if (${is_ready[${_debug}, "Mass Group Buff"]}) {
			/call voodoo ${_debug} "Mass Group Buff" 0 FALSE
		}
		/if (${is_ready[${_debug}, "Paragon of Spirit"]}) {
			/call voodoo ${_debug} "Paragon of Spirit" 0 FALSE
		}
	} else /if (${Bool[${Me.Song[Paragon of Spirit]}]}) {
		OUT ${owarning} trying to cast ${csp}Paragon of Spirit\ax while its running.
	}

/return TRUE



|***
 * note: Beast burn routines
 * use: establish all comp/trigger skills
 ***|
sub burn_BST(bool _debug, int _tmpID)
	DEBUG \atburn_BST\ax(\a-w${_tmpID}\ax)
	FLOW burn_BST

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}
	CHECKEXIT burn_BST
	GETINPUT
	CHECKTIE

	CHECKDEFENSE
	/if (SWARM || PET) {
		/call send_pet ${maDebug.Find[minion].Value} ${_tmpID}
	}
	/if (AUTO && MELEE) {
		/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
	}

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}


	/if (${is_ready[${_debug}, "Bestial Alignment"]}) {
		/call voodoo ${_debug} "Bestial Alignment" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Frenzied Swipes"]}) {
		/call voodoo ${_debug} "Frenzied Swipes" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Ferociousness"]} && !${Me.Song[${maChr.Find[stSavage].Value}].ID}) {
		/call voodoo ${_debug} "Ferociousness" 0 FALSE
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/if (SWARM) {
		/if (${is_ready[${_debug}, "Attack of the Warders"]}) {
			/call voodoo ${_debug} "Attack of the Warders" ${_tmpID} FALSE
		}
	}

	/if (${Me.Pet.ID}) {
		/if (${is_ready[${_debug}, "Companion's Fury"]}) {
			/call voodoo ${_debug} "Companion's Fury" 0 FALSE
		}
	}

	/if (MELEE || DOT) {
		/if (${is_ready[${_debug}, "${maChr.Find[stVindication].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stVindication].Value}" 0 FALSE
		}
	}

	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}

	/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stFury].Value}].RankName}]} && !${Me.Song[Frenzy of Spirit].ID} && !${Me.Buff[${maChr.Find[stDichotomic].Value}].ID}) {
		/if (${is_ready[${_debug}, "${maChr.Find[stFury].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stFury].Value}" 0 FALSE
		}
	} else {
		/if (${Me.Song[${maChr.Find[stFury].Value}].ID}) /return
		/if (${is_ready[${_debug}, "Frenzy of Spirit"]}) {
			/call voodoo ${_debug} "Frenzy of Spirit" 0 FALSE
		}
		/if (${is_ready[${_debug}, "BloodLust"]}) {
			/call voodoo ${_debug} "BloodLust" 0 FALSE
		}

		/if (${target[isdead, ${_tmpID}]}) {
			/return FALSE
		}

		/call cast_AA_spire ${maDebug.Find[spire].Value}

		/if (${is_ready[${_debug}, "${maChr.Find[stSavage].Value}"]} && !${Me.Song[Bestial Alignment].ID}) {
			/call voodoo ${_debug} "${maChr.Find[stSavage].Value}" 0 FALSE
		}
		/if (${is_ready[${_debug}, "Group Bestial Alignment"]} && !${Me.Buff[Ferociousness].ID}) {
			/call voodoo ${_debug} "Group Bestial Alignment" 0 FALSE
		}
	}

/return TRUE



|***
 * note: beastlord specific control
 * use: /chr
 ***|
sub set_control_BST(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_BST\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr
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

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| consume spirit
	} else /if (${_type.Equal[consume]}) {
		/if (!${set_control_num_range[${_debug}, stPctConsume, "${_verbage}", 0, 99]}) /return

	| focused paragon
	} else /if (${_type.Equal[parafocus]}) {
		/if (!${set_control_num_range[${_debug}, stPctParagonF, "${_verbage}", 0, 99]}) /return

	| group paragon
	} else /if (${_type.Equal[paragroup]}) {
		/if (!${set_control_num_range[${_debug}, stPctParagonG, "${_verbage}", 0, 99]}) /return

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE




