|***
 *	entropy
 *	class.rng.inc
 ***|
 



|***
 * note: primary initilization of all ranger non shared data
 * use: not for you
 ***|
sub set_RNG(bool _debug)
	DEBUG \atset_RNG\ax()
		
	/call set_data FALSE maData stSpire																	map outer "Spire of the Pathfinders"
	/call set_data FALSE maData stSynergy																map outer "Outrider's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Earthcaller"
	/call set_data FALSE maData stEpic15																map outer "Heartwood Blade"
	/call set_data FALSE maData stEpic2																	map outer "Aurora, the Heartwood Blade"
	/call set_data FALSE maData stFade																	map outer "Cover Tracks"
	/call set_data_map FALSE maChr stAAUnity														"Wildstalker's Unity (Azia)"	"${auxna} class aa unity buff"

	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance spell"

	| spam summer nuke
	/varset dump ${set_data_timer[FALSE, Summer_Nuke, 1s]}
	/call set_data_map FALSE maChr stSummer		 													PREGEN|1											"${auxna} summer line nuke spell"
	/call set_data_map FALSE maChr swSummer															FALSE													"${auxsw} use summer nuke line spell"

	| rest disc
 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr stHiatus															PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"

	| fast heal
  /call set_data_map FALSE maChr stHealDesperate											PREGEN|1											"${auxna} fast cast heal"

	| bow disc
	/call set_data_map FALSE maChr stDiscRange			 										PREGEN|1											"${auxna} ranged DPS disc"

	| melee disc
	/call set_data_map FALSE maChr stDiscMele			 											PREGEN|1											"${auxna} melee DPS disc"

	| agro/anti agro kicks
	/call set_data_map FALSE maChr stDiscJolt			 											PREGEN|1											"${auxna} hate reduction disc"
	/call set_data_map FALSE maChr stDiscEnrage		 											PREGEN|1											"${auxna} hate increase disc"

	| AE group heal disc
	/call set_data_map FALSE maChr stDiscHeal		 												PREGEN|1											"${auxna} group heal disc"

	| ae melee disc
	/call set_data_map FALSE maChr stDiscMeleeAE		 										PREGEN|1											"${auxna} ae melee disc"

	| focused melee disc
	/call set_data_map FALSE maChr stDiscMeleeSingle			 							PREGEN|1											"${auxna} single melee disc"

/return
                                     		




|***
 * note: Main Ranger Routine
 * use: 
 ***|
sub main_RNG(bool _debug)
	DEBUG \atmain_RNG\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ

	/while (${check_combat_status[FALSE]}) {
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Group
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call cast_cure FALSE
		/if (${maHeal.Find[swHealXTarget].Value}) /call get_hurt_${Me.Class.ShortName} FALSE XTarget
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call get_hurt_${Me.Class.ShortName} FALSE Pet
		/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
		/if (${check_class_loop[FALSE]}) /return FALSE
		/if (VALID) {
			/call set_combat FALSE
			/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (BURN) /call burn_${Me.Class.ShortName} FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_dot FALSE lsDoT TARGET
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/if (${maChr.Find[swSummer].Value} && !${timer_Summer_Nuke}) /call cast_nuke_summer FALSE TARGET
			/call check_item_cycle FALSE TARGET
			| /if (${swAlliance}) /call trigger_alliance TARGET

		}
	
		/if (AUTO) {	
			CHECKFADE
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
			| /if (${validate_class[FALSE, ${lstClassMelee}]}) /call cast_stamina_recovery
		}
		
	}
	
/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat_RNG(bool _debug)
	DEBUG \atmisc_combat_RNG\ax()
	
	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
		
	| agro control
	/if (AGRO) {
		/if (${is_ready[FALSE, "${maChr.Find[stDiscEnrage].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stDiscEnrage].Value}" TARGET FALSE
		}
	} else /if (!AGRO) {
		/if (${is_ready[FALSE, "${maChr.Find[stDiscJolt].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stDiscJolt].Value}" TARGET FALSE
		}
	}	

	| elemental arrows rotation
	/if (!${Target.Buff[Elemental Arrow].ID}) {
		/if (${is_ready[FALSE, "Elemental Arrow"]}) {
			/call voodoo FALSE "Elemental Arrow" TARGET FALSE
		}
	}

	
	/if (!${maChr.Find[stCountAE].Value}) {
		/if (${is_ready[FALSE, "${maChr.Find[stDiscMeleeSingle].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stDiscMeleeSingle].Value}" TARGET FALSE
		}
	} else /if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= ${maChr.Find[stCountAE].Value} && ${maChr.Find[stCountAE].Value}) {	
		/if (${is_ready[FALSE, "${maChr.Find[stDiscMeleeAE].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stDiscMeleeAE].Value}" TARGET FALSE
		}
	}
	
/return TRUE



|***
 * note: MGB ranger Auspice if both AAs are ready
 * use: 
 ***|
sub cast_AA_auspice(bool _debug)
	DEBUG \atcast_AA_auspice\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[Auspice of the Hunter]} && !${Bool[${Me.Song[Auspice of the Hunter]}]}) {
		/if (${is_ready[FALSE, "Mass Group Buff"]}) {
			/call voodoo FALSE "Mass Group Buff" 0 FALSE
		}
		/if (${is_ready[FALSE, "Auspice of the Hunter"]}) {
			/call voodoo FALSE "Auspice of the Hunter" 0 FALSE
		}
	} else /if (${Me.Song[Auspice of the Hunter].ID}) {
		OUT ${bad}QUIT\ax trying to cast ${sp}Auspice of the Hunter\ax while its running.	
	}		
/return TRUE



|***
 * note: healing
 * use: /call check_heal_self_${Me.Class.ShortName}
 ***|
sub check_heal_RNG(bool _debug, string _type)
	DEBUG \atcheck_heal_RNG\ax(\a-w${_type}\ax)
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "HEAL ${_type}"
	
	/call check_hurt FALSE ${_type}
  	
	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		| SETHUD FALSE
		/return	
	}  	
	
	/if (!${check_hurt[${_debug}, ${_type}]}) /return FALSE

	| Reflexive Healing Disc
	/if (${hurtCount} && ${Select[${_type},self,group]}) {
		/if (${is_ready[FALSE, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
			/call voodoo FALSE "${maChr.Find[stDiscHeal].Value}" TARGET FALSE
		}
	}
	
	
	/if (!${get_hurt[FALSE, ${_type}]}) /return FALSE
	CHECKREZ
	CHECKFADE
		
	/call cast_list FALSE "lsHealBaseLine" ${hurtID}
	
	SETHUD FALSE

/return TRUE



|***
 * note: Control for Spam Agro Nuke
 * use: /call cast_nuke_summer DEBUG [TARGETID]
 ***|
sub cast_nuke_summer(bool _debug, int _tmpID)
	DEBUG \atcast_nuke_summer\ax(\a-w${_tmpID}\ax)

	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return
	ISMEDEAD
	CHECKEXIT
	/varset dump ${set_data_timer[FALSE, Summer_Nuke, RESTART]}
	
	/if (${is_ready[FALSE, "${maChr.Find[stSummer].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stSummer].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: really should be starting fires
 * use: 
 ***| 
sub burn_RNG(bool _debug)
	DEBUG \atburn_RNG\ax()

	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} TARGET
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE	

	/call cast_AA_spire ${maDebug.Find[spire].Value}
	
	| cheeta
	/if (${is_ready[FALSE, "Scarlet Cheeta's Fang"]}) {
		/call voodoo FALSE "Scarlet Cheeta's Fang" 0 FALSE
	}
	
	| auspice
	/if (${is_ready[FALSE, "Auspice of the Hunter"]}) {
		/call voodoo FALSE "Auspice of the Hunter" 0 FALSE
	}
	
	|**
	| MELEE
	/if (MELEE) {
		/if (${Target.Distance} <= ${Spawn[TARGET].MaxRangeTo}*.9) {

			| weapon proc
			/if (${is_ready[FALSE, "Imbued Ferocity"]}) {
				/call voodoo FALSE "Imbued Ferocity" 0 FALSE
			}

			/if (!${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
				/if (${is_ready[FALSE, "Empowered Blades"]}) {
					/call voodoo FALSE "Empowered Blades" 0 FALSE
				}
				
			} else /if (!${Me.ActiveDisc.ID} && !${Me.Song[Empowered Blades].ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
				/if (${is_ready[FALSE, "${maChr.Find[stDiscMele].Value}"]}) {
					/call voodoo FALSE "${maChr.Find[stDiscMele].Value}" 0 FALSE
				}
				
			} else /if (!${Me.ActiveDisc.ID} && !${is_ready[FALSE, "${maChr.Find[stDiscMele].Value}"]} && !${Me.AltAbilityReady[Empowered Blades]} && !${Me.Song[Empowered Blades].ID}) {
				/call cast_glyph ${maDebug.Find[glyph].Value}
			} 
		}


	}
	
	| RANGE
	/if (RANGE) {
		/if (${Target.Distance} >= 31) {
			/if (${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
				/if (${is_ready[FALSE, "${maChr.Find[stDiscRange].Value}"]}) {
					/call voodoo FALSE "${maChr.Find[stDiscRange].Value}" 0 FALSE
				}
			} else /if (!${Me.ActiveDisc.ID} && !(${is_ready[FALSE, "${maChr.Find[stDiscRange].Value}"]}) {
				/call cast_glyph ${maDebug.Find[glyph].Value}
			}
		}
	
	}
	**|
	
	| attack / dmg AA
	/if (${is_ready[FALSE, "Outrider's Accuracy"]}) {
		/call voodoo FALSE "Outrider's Accuracy" 0 FALSE
	}
	/if (${is_ready[FALSE, "Outrider's Attack"]}) {
		/call voodoo FALSE "Outrider's Attack" 0 FALSE
	}
	
	| guardian buff (group first)
	/if (!${Me.Song[Guardian of the Forest].ID} && ${Me.AltAbilityReady[Group Guardian of the Forest]}) {
		/if (${is_ready[FALSE, "Group Guardian of the Forest", ${Me.ID}]}) {
			/call voodoo FALSE "Group Guardian of the Forest" 0 FALSE
		}
		
	} else /if (!${Me.Song[Group Guardian of the Forest].ID} && ${Me.AltAbilityReady[Guardian of the Forest]}) {  
		/if (${is_ready[FALSE, "Guardian of the Forest", ${Me.ID}]}) {
			/call voodoo FALSE "Guardian of the Forest" 0 FALSE
		}
	}
		
	| swarm pets
	/if (SWARM) {
		/if (${is_ready[FALSE, "Pack Hunt"]}) {
			/call voodoo FALSE "Pack Hunt" TARGET FALSE   
		}
	}
	
	| ae regen 
	/if (${is_ready[FALSE, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
		/call voodoo FALSE "${maChr.Find[stDiscHeal].Value}" TARGET FALSE
	}
			
 /return TRUE
 	


|***
 * note: ranger specific control
 * use: /chr
 ***|
sub set_control_RNG(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_RNG\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local
  
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
		/return

	| use mana click items
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[FALSE, stPctManaRecovery, "${_verbage}", 0, 99]}) /return
		
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

	| summer switch	
	} else /if (${_type.Equal[summer]}) {
		/varset dump ${set_switch_env[${_debug}, swSummer, TRUE, ${_verbage}]}

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/varset dump ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset dump ${set_control_output[${_debug}, maChr, chr, maChrControl]}
	
/return


