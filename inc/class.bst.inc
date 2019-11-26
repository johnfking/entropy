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
	
	/call set_data FALSE maData stSpire																	map outer "Spire of the Savage Lord"
	/call set_data FALSE maData stSynergy																map outer "Primalist's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Claw of the Savage Spirit"
	/call set_data FALSE maData stEpic15																map outer "Savage Lord's Totem"
	/call set_data FALSE maData stEpic2																	map outer "Spiritcaller Totem of the Feral"
	/call set_data FALSE maData stFade																	map outer "Playing Possum"
	/call set_data_map FALSE maChr stAAUnity														"Feralist's Unity"						"${auxna} class aa unity buff"
	
	

 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"
 	/call set_data_map FALSE maChr stAlliance 													PREGEN|1											"${auxna} alliance spell"

	/call set_data_map FALSE maChr stNukeServant 												PREGEN|1											"${auxna} spam nuke spam spell"
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

 	/call set_data_map FALSE maChr stPctParagonG 												0															"${auxnum} pct mana to use group paragon aa" 										
	/call set_data_map FALSE maChr stPctParagonF 												0															"${auxnum} pct mana to use focused paragon aa"
	/call set_data_map FALSE maChr stPctConsume 												0															"${auxnum} pct mana to use consume aa"
   
	/call set_data_map FALSE maCC stMeleeKick														kick													"${auxna} current combat kick method"
	/declare lsMeleePunch list outer
	/varset dump ${lsMeleePunch.Append[dragon punch,eagle strike,tiger claw]}

	/call set_data_map FALSE maCC stMeleePunch   												"dragon punch"								"${auxna} current combat punch method"
	/declare lsMeleeKick list outer
	/varset dump ${lsMeleeKick.Append[kick,flying kick,round kick]}
   
                                          			
/return




|***
 * note: main beastlord routine
 * use: 
 ***|
sub main_BST(bool _debug)
	DEBUG \atmain_BST\ax()

	/if (AUTO) {
		/call check_mana_${Me.Class.ShortName}
		/if (PET || SWARM) /call send_pet FALSE TARGET
	}

	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Group
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/if (${maHeal.Find[swHealXTarget].Value}) /call check_heal_${Me.Class.ShortName} FALSE XTarget
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Pet
		/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
		/if (${check_class_loop[FALSE]}) /return		

		/if (VALID) {
			/call set_combat FALSE
			/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
			/if (AGRO) /call get_agro_${Me.Class.ShortName}
			/if (!${timer_Check_deBuffs}) /call check_deBuffs FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (BURN) /call burn_${Me.Class.ShortName} FALSE
			/if (PET || SWARM) /call send_pet FALSE TARGET
			/if (SWARM) /call cast_temp_pet FALSE TARGET "${maChr.Find[stNukeServant].Value}"
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_dot FALSE lsDoT TARGET
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/call check_item_cycle FALSE TARGET

			| /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
		
			| /if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				| /call check_buff_cycle_temp_self FALSE stSteel 
				| /call check_buff_cycle_temp_self FALSE stStance
			| }			
		}

		/if (AUTO) {	
			CHECKFADE
			/call check_mana_BST FALSE
			/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
			| /if (${validate_class[FALSE, ${lstClassMelee}]}) /call cast_stamina_recovery
		}	
	}
	
/return



|***
 * note: 
 * use: /call misc_combat_BST DEBUG
 ***|
sub misc_combat_BST(bool _debug)
	DEBUG \atmisc_combat_BST\ax()

	/if (${Me.PctEndurance} < 2) /return
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/if (${is_target_dead[FALSE, TARGET]}) /return
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
	/if (SWARM || PET) /call send_pet FALSE TARGET
	
	/if (${is_ready[FALSE, "${maChr.Find[stBestial].Value}", ${Me.ID}]} && !${Me.Song[${maChr.Find[stBestial].Value}].ID}) {
		/call voodoo FALSE "${maChr.Find[stBestial].Value}" disc 0 FALSE
	}
	
	/if (${is_ready[FALSE, "${maChr.Find[stClaws].Value}", TARGET]}) {
		/call voodoo FALSE "${maChr.Find[stClaws].Value}" TARGET FALSE
	}
	
	/if (${is_ready[FALSE, "Enduring Frenzy", TARGET]} && !${Target.Buff[Enduring Frenzy].ID}) {
		/call voodoo FALSE "Enduring Frenzy" TARGET FALSE
	}

	/if (${is_ready[FALSE, "Chameleon Strike", TARGET]}) {
		/call voodoo FALSE "Chameleon Strike" TARGET FALSE
	}

	/if (${is_ready[FALSE, "${maChr.Find[stRake].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stRake].Value}" TARGET FALSE
	}

	/if (${is_ready[FALSE, "${maChr.Find[stRending].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stRending].Value}" TARGET FALSE
	}
			
	| do we want the mobs attention?
	/if (AGRO) {
		/if (${is_ready[FALSE, "Roaring Strike"]} && ${Me.PctAggro} < 80) {
			/call voodoo FALSE "Roaring Strike" TARGET FALSE
		}

	} else /if (!AGRO) {
		/if (${is_ready[FALSE, "Roar of Thunder", TARGET]} && ${Me.PctAggro} > 80) {
			/call voodoo FALSE "Roar of Thunder" TARGET FALSE
		}		
		
	}

	/if (${Me.Pet.ID}) {
		| /if (!${Me.Song[Growl].ID} && !${Me.Song[${maChr.Find[stGrowl].Value}].ID} && !${Me.PetBuff[Savage Rage]} && !${Me.Song[${maChr.Find[stSavage].Value}].ID}) {
			/if (${is_ready[FALSE, "${maChr.Find[stGrowl].Value}"]}) {
				/call voodoo FALSE "${maChr.Find[stGrowl].Value}" 0 FALSE
			}
		| }
	}
	
/return



|***
 * note: Focused Paragon. will maintain the bst
 * use: /call check_mana_${Me.Class.ShortName}
 ***|
sub check_mana_BST(bool _debug)
	DEBUG \atcheck_mana_BST\ax()

	ISMEDEAD
	CHECKEXIT
	
	| consumption of Spirits AA
	/if (${maChr.Find[stPctConsume].Value}) {
		/if (${Me.PctMana} < ${maChr.Find[stPctConsume].Value}*.PCTHO) {
		 	/if (${is_ready[FALSE, "Consumption of Spirit"]}) {
		 		/call voodoo FALSE "Consumption of Spirit" 0 FALSE
		 		/return
		 	}
		}
	}
	
	| Paragon Group Members	NOT IN RAID.... if 2+ need stamina or mana
	/if (${Group.Members} && !${Raid.Members} && ${Me.AltAbilityReady[Paragon of Spirit]}) {
		/declare _count int local 0
		/declare _countHurt int local 0

		/for _count 0 to ${Group.Members}
			ISMEDEAD
			CHECKEXIT
			/if (${Group.Member[${_count}].Name.Equal[${Me.DisplayName}]}) /continue
			/if (${Group.Member[${_count}].PctMana} >= ${maChr.Find[stPctParagonG].Value}*.PCTHO && ${Group.Member[${_count}].PctEndurance} >= ${maChr.Find[stPctParagonG].Value}*.PCTHO) /continue
			/if (${Group.Member[${_count}].Distance} > 100) /continue
			/if (${Group.Member[${_count}].Type.Equal[CORPSE]}) /continue
			/if (${Group.Member[${_count}].Class.ShortName.Equal[BRD]}) /continue
			

			/varcalc _countHurt ${_countHurt}+1
		/next _count				
		
		/if (${_countHurt} > 1) {
			/call voodoo FALSE "Paragon of Spirit" 0 FALSE
			/delay 2s
		}
	}
	
	/if (${Me.Song[Paragon of Spirit].ID}) /return FALSE
	/if (${Me.Song[Focused Paragon of Spirit].ID}) /return FALSE
		
	| self paragon
	/if (${maChr.Find[stPctParagonF].Value}) {
		/if (${Me.PctMana} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO || ${Me.PctEndurance} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Focused Paragon of Spirits"]}) {
				/call voodoo FALSE "Focused Paragon of Spirits" ${Me.ID} FALSE
				/delay 2s
			}
		}
	}
		
/return


 
|***
 * note: healing for beastlord
 * use:  when you or your agro whore pet get hurt
 ***|
sub check_heal_BST(bool _debug, string _type)
	DEBUG \atcheck_heal_BST\ax(\a-w${_type}\ax)
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"
	
 	/if (!${get_hurt[FALSE, ${_type}]}) {
 		/return FALSE
 	}
 	  	
	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return	
	}  	

	/if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value}) {
		/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Mend Companion"]}) {
				/call voodoo FALSE "Mend Companion" ${Pet.ID} FALSE
			}
		}

		/call cast_list FALSE lsHealPetLine ${Pet.ID}
	}


	| self healing
	/if (${Select[${_type},self]} && ${maHeal.Find[swHealSelf].Value}) {
	 	/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
	 		/if (${is_ready[FALSE, "Warder's Gift", ${hurtID}]}) {
	 			/call voodoo FALSE "Warder's Gift" 0 FALSE
	 		}
		}
	}

 	/if (!${get_hurt[FALSE, ${_type}]}) {
 		/return FALSE
 	}
	| heal anything spell
	/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
		/call cast_list FALSE lsHealBaseLine ${hurtID}
	}

	SETHUD FALSE

/return FALSE



|***
 *	DES: MGB beast Paragon if both AAs are ready
 *	USE: /paragon
 *  NOTE: 
 ***|
#bind cast_AA_paragon /paragon
sub Bind_cast_AA_paragon(bool _debug)
	/if (${Select[${Me.Class.ShortName},BST]}) /call cast_AA_paragon
/return


sub cast_AA_paragon(bool _debug)
	DEBUG \atcast_AA_paragon\ax()
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[Paragon of Spirit]} && !${Me.Song[Paragon of Spirit].ID}) {
		/if (${is_ready[FALSE, "Mass Group Buff"]}) {
			/call voodoo FALSE "Mass Group Buff" 0 FALSE
		}
		/if (${is_ready[FALSE, "Paragon of Spirit"]}) {
			/call voodoo FALSE "Paragon of Spirit" 0 FALSE
		}
	} else /if (${Bool[${Me.Song[Paragon of Spirit]}]}) {
		OUT ${owarning} trying to cast ${csp}Paragon of Spirit\ax while its running.	
	}		
/return



|***
 *	DES: DPS Clickies
 *	USE: this shits auto, learn to love it.
 *  NOTE: 
 ***|
sub click_DPS_BST(bool _debug)
	DEBUG click_DPS_BST(${_debug})

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[FALSE, TARGET]}) /return
	/call click_horn_of_unity 	

/return



 |***
 * note: Beast burn routines
 * use: establish all comp/trigger skills
 ***| 
sub burn_BST(bool _debug)
	DEBUG burn_BST(${_debug})

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[FALSE, TARGET]}) /return
	/if (SWARM || PET) /call send_pet FALSE TARGET
	
	/if (${is_ready[FALSE, "Bestial Alignment"]}) {
		/call voodoo FALSE "Bestial Alignment" 0 FALSE
	}
	/if (${is_ready[FALSE, "Frenzied Swipes"]}) {
		/call voodoo FALSE "Frenzied Swipes" 0 FALSE
	}
	/if (${is_ready[FALSE, "Ferociousness"]} && !${Me.Song[${maChr.Find[stSavage].Value}].ID}) {
		/call voodoo FALSE "Ferociousness" 0 FALSE
	}


	/if (SWARM) {
		/if (${is_ready[FALSE, "Attack of the Warders"]}) {
			/call voodoo FALSE "Attack of the Warders" TARGET FALSE
		}
	}

	/if (${Me.Pet.ID}) {
		/if (${is_ready[FALSE, "Companion's Fury"]}) {
			/call voodoo FALSE "Companion's Fury" 0 FALSE
		}
	}
	
	/if (MELEE || DOT) {
		/if (${is_ready[FALSE, "${maChr.Find[stVindication].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stVindication].Value}" 0 FALSE
		}
	}

	/call cast_glyph FALSE

	/if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stFury].Value}].RankName}]} && !${Me.Song[Frenzy of Spirit].ID} && !${Me.Buff[stDichotomic Fury].ID}) {
		/if (${is_ready[FALSE, "${maChr.Find[stFury].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stFury].Value}" 0 FALSE
		}
	} else {
		/if (${Me.Song[${maChr.Find[stFury].Value}].ID}) /return
		/if (${is_ready[FALSE, "Frenzy of Spirit"]}) {
			/call voodoo FALSE "Frenzy of Spirit" 0 FALSE
		}
		/if (${is_ready[FALSE, "BloodLust"]}) {
			/call voodoo FALSE "BloodLust" 0 FALSE
		}
		/call cast_AA_spire FALSE
		/if (${is_ready[FALSE, "${maChr.Find[stSavage].Value}"]} && !${Me.Song[Bestial Alignment].ID}) {
			/call voodoo FALSE "${maChr.Find[stSavage].Value}" 0 FALSE
		}
		/if (${is_ready[FALSE, "Group Bestial Alignment"]} && !${Me.Buff[Ferociousness].ID}) {
			/call voodoo FALSE "Group Bestial Alignment" 0 FALSE
		}
	}
	
/return



|
|***
 * note: beastlord specific control
 * use: /chr
 ***|
sub set_control_BST(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_BST\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

	/declare _li listiterator local
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

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return


	| consume spirit
	} else /if (${_type.Equal[consume]}) {
		/if (!${set_control_num_range[FALSE, stPctConsume, "${_verbage}", 0, 99]}) /return

	| focused paragon
	} else /if (${_type.Equal[parafocus]}) {
		/if (!${set_control_num_range[FALSE, stPctParagonF, "${_verbage}", 0, 99]}) /return

	| group paragon
	} else /if (${_type.Equal[paragroup]}) {
		/if (!${set_control_num_range[FALSE, stPctParagonG, "${_verbage}", 0, 99]}) /return

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
		/if (${li.Value.Equal[manaclick]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaClick].Value},${num}${maChr.Find[stPctManaClick].Value}\ax,${r0}]}
		/if (${li.Value.Equal[consume]}) /varset _out ${_out}:${If[${maChr.Find[stPctConsume].Value},${num}${maChr.Find[stPctConsume].Value}\ax,${r0}]}
		/if (${li.Value.Equal[parafocus]}) /varset _out ${_out}:${If[${maChr.Find[stPctParagonF].Value},${num}${maChr.Find[stPctParagonF].Value}\ax,${r0}]}
		/if (${li.Value.Equal[paragroup]}) /varset _out ${_out}:${If[${maChr.Find[stPctParagonG].Value},${num}${maChr.Find[stPctParagonG].Value}\ax,${r0}]}
		/if (${li.Value.Equal[hiatus]}) /varset _out ${_out}:${If[${maChr.Find[swHiatus].Value},${on},${off}]}
		/varset dump ${li.Advance}
	}	
	OUT /chr ${_out}

/return

		
