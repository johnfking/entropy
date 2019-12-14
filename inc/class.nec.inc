|***
 * entropy
 * class.nec.inc
 ***|
 


|***
 * note: primary initilization of all necromancer non shared data
 * use: not for you
 ***|
sub set_NEC(bool _debug)
	DEBUG \atset_NEC\ax()

	/call set_data FALSE maData stSpire																	map outer "Spire of Necromancy"
	/call set_data FALSE maData stSynergy																map outer "Defiler's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Scythe of the Shadowed Soul"
	/call set_data FALSE maData stEpic15																map outer "Soulwhisper"
	/call set_data FALSE maData stEpic2																	map outer "Deathwhisper"
	/call set_data FALSE maData stFade																	map outer "A Hole in Space"
	/call set_data FALSE maData stCompleteHeal													map outer "Complete Heal"
	/call set_data_map ${_debug} maChr stAAUnity												"Mortifier's Unity"					"${auxna} class aa unity buff"


	/call set_data_map ${_debug} maChr stNukeServant 										PREGEN|1											"${auxna} spam nuke pet spell"
 	/call set_data_list ${_debug} maChr lsHealPetMend										PREGEN|3											"${auxli} mending spells for pet"
 	/call set_data_map ${_debug} maChr stAlliance 											PREGEN|1											"${auxna} alliance spell"
  /call set_data_map ${_debug} maChr stDichotomic 										PREGEN|1											"${auxna} dichotomic spell"


	/varset dump ${set_data_timer[${_debug}, Check_Blood, 8s]}
	/call set_data_map ${_debug} maChr swblood													FALSE													"${auxsw} use  blood spell"
	/call set_data_map ${_debug} maChr stBlood													PREGEN|1											"${auxna} name of current call for blood spell"		

	/call set_data_map ${_debug} maChr stPctBloom				 								0															"${auxnum} % of manna you want to use bloom at"

/return



|***
 * note: Main Necro Routine
 * use: 
 ***|
sub main_NEC(bool _debug)
	DEBUG \atmain_NEC\ax()

	/if (AUTO) {
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
		/if (PET || SWARM) /call send_pet ${maDebug.Find[minion].Value} TARGET
	}

		GETINPUT
		CHECKREZ
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} ${maDebug.Find[heal].Value} Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
		/if (${check_class_loop[FALSE]}) /return
	
		/if (VALID) {
			/if (PET || SWARM) /call send_pet ${maDebug.Find[minion].Value} TARGET
			/call set_combat FALSE
			/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
			/if (!${timer_Check_deBuffs}) /call check_deBuffs /call check_cure ${maDebug.Find[debuff].Value} TARGET
			/if (BURN) /call burn_${Me.Class.ShortName} /call check_cure ${maDebug.Find[burn].Value}
			/if (SWARM) /call cast_temp_pet ${maDebug.Find[minion].Value} TARGET "${maChr.Find[stNukeServant].Value}"
			/if (${maCC.Find[swAlliance].Value}) /call trigger_alliance TARGET
			/if (${maChr.Find[swblood].Value} && ${timer_Check_Blood}) /call cast_blood ${maDebuf.Find[blood].Value} TARGET
			/if (DOT) /call cast_dot ${maDebug.Find[castdot].Value} lsDoT TARGET
			/if (NUKE) /call cast_nuke ${maDebug.Find[castnuke].Value} lsNuke TARGET
			/if (MELEE) /call use_skill_melee TARGET
			/call misc_combat_${Me.Class.ShortName}
			/call check_item_cycle /call check_cure ${maDebug.Find[castitem].Value} TARGET

		}
		
		/if (AUTO) {	
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value]}
			/if (${Me.PctMana} <= ${maChr.Find[stPctBloom].Value}*.PCTHO) /call cast_bloom ${maDebug.Find[mannarecover].Value]}
			CHECKFADE
		}	
	}	
	
		
/return



|***
 *	DES: 
 *	USE: 
 *  NOTE: 
 ***|
sub check_heal_NEC(bool _debug, string _type)
	DEBUG \atcheck_heal_NEC\ax()
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"
	
 	/if (!${get_hurt[${_debug}, ${_type}]}) {
 		/return FALSE
 	}
 	  	
	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return	FALSE
	}  	

	/if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value}) {
		/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
			/call cast_list ${_debug} lsHealPetMend ${Pet.ID}
		}
		
		/if (${is_ready[${_debug}, "Mend Companion"]}) {
			/call voodoo ${_debug} "Mend Companion" ${Pet.ID} FALSE
		}		

	}

 	/if (!${get_hurt[${_debug}, ${_type}]}) {
 		/return FALSE
 	}
 	
	| heal anything spell
	/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
		/call cast_list ${_debug} lsHealBaseLine ${hurtID}
	}

	| aa Dying grasp
	/if (${Me.PctHPs} <= ${${Me.Class.ShortName}Point}*.PCTHO} && ${validate_cast[${_debug}, "Dying Grasp"]}) {
		/call voodoo ${_debug} "Dying Grasp" TARGET FALSE
	}

	SETHUD FALSE

/return



|***
 * note: the extra stuff 
 * use: normal routine 
 ***|
sub misc_combat_NEC(bool _debug)
	DEBUG \atmisc_combat_NEC\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

	| aa wake the dead
	/if (SWARM) {
		| wake the dead
		/if (${SpawnCount[corpse radius ${Spell[Wake the Dead].MyRange}]} > 3) { 
			/if (${validate_cast[${_debug}, "Wake the Dead"]} && ${Target.PctHPs} > 15) /call voodoo ${_debug} "Wake the Dead" TARGET FALSE
		}
		| aa swarm of decay
		/if (${validate_cast[${_debug}, "Swarm of Decay"]}) /call voodoo ${_debug} "Swarm of Decay" TARGET FALSE
		| aa Rise of bones
		/if (${validate_cast[${_debug}, "Rise of Bones"]}) /call voodoo ${_debug} "Rise of Bones" TARGET FALSE
	}
	
	/if (${validate_cast[${_debug}, "Hand of Death"]}) /call voodoo ${_debug} "Hand of Death" TARGET FALSE

/return TRUE



|***
 * note: �You should never attack a necromancer in a cemetery; it�s like chasing Rambo into a building full of loaded guns." --Laurell K. Hamilton, Dead Ice
 * use: burn the dead
 ***| 
sub burn_NEC(bool _debug)
	DEBUG \atburn_NEC\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
	
	| /if (${Me.Song[Gift of Deathly Resolve].ID}) /call cast_DOT FALSE TARGET
	
	| /if (!${Me.Song[Gift of Deathly Resolve].ID} && ${Me.Buff[Deathly Resolve].ID}) /call cast_nuke FALSE TARGET

	/call cast_AA_spire ${_debug}


	| Pet Burnage
	/if (PET && ${Me.Pet.ID}) {
		/if (${validate_cast[${_debug}, "Companion's Fury"]}) /call voodoo ${_debug} "Companion's Fury" ${Me.Pet.ID} FALSE
	}   

	/if (${Target.PctHPs} > 15 && ${Me.PctHPs} > 90) {
		/if (${validate_cast[${_debug}, "Life Burn"]}) /call voodoo ${_debug} "Life Burn" TARGET FALSE
	}

	/if (!${Me.Buff[Spire of Necromancy].ID}) {
		/if (${validate_cast[${_debug}, "Embalmer's Carapace"]}) /call voodoo ${_debug} "Embalmer's Carapace" TARGET FALSE
	}		
	
	/if (${validate_cast[${_debug}, "Funeral Pyre"]}) /call voodoo ${_debug} "Funeral Pyre" TARGET FALSE

	/if (${validate_cast[${_debug}, "Gift of Deathly Resolve"]}) /call voodoo ${_debug} "Gift of Deathly Resolve" 0 FALSE

	/if (${validate_cast[${_debug}, "Mercurial Torment"]}) /call voodoo ${_debug} "Mercurial Torment" 0 FALSE

	/if (${validate_cast[${_debug}, "Focus of Arcanum"]}) /call voodoo ${_debug} "Focus of Arcanum" 0 FALSE        

	/call cast_glyph ${_debug}

/return TRUE



|***
 * note: casts call of blood line of spells
 * use: /call cast_blood DEBUG [ID]
 ***|
sub cast_blood(bool _debug, int _tmpID)
	DEBUG \atcast_blood\ax(\a-w${_tmpID}\ax)
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	
	/varset dump ${set_data_timer[${_debug}, Check_Blood, RESTART]}
	/if (${Me.Song[Chaotic Power].ID}) /return FALSE
	
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return
	
	/if (${validate_cast[FALSE, "${maChr.Find[stBlood].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stBlood].Value}" ${_tmpID} FALSE
	}
	
/return TRUE


 
|***
 * note: We want Mana
 * use: /chr bloom #
 ***|
sub cast_bloom(bool _debug)
	DEBUG \atcast_bloom\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (${validate_cast[FALSE, "Death Bloom"]}) {
		/call voodoo ${_debug} "Death Bloom" 0 FALSE
	}
	
/return TRUE
 
 
 
 
 
 
 
|***
 * note: Class controls
 * use: /chr []
 ***|
sub set_control_NEC(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_NEC\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return

	| use blood nuke line
	} else /if (${_type.Equal[blood]}) {
		/varset dump ${set_switch_env[${_debug}, swblood, TRUE, ${_verbage}]}
		
	| use bloom mana recovery
	} else /if (${_type.Equal[bloom]}) {
		/varset dump ${set_switch_env[${_debug}, stPctBloom, TRUE, ${_verbage}]}
		

	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset dump ${set_control_output[${_debug}, maChr, chr, maChrControl]}
	
/return



 
 
 