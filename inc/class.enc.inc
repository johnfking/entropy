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
	/call set_data_map FALSE maChr swAACrystals													FALSE													"${auxsw} use aa crystals:\a-wAzure Mind Crystal, Sanguine Mind Crystal/ax"
	
	| PBAE stun
	/call set_data_map FALSE maChr stAEStun															PREGEN|1											"${auxna} name of your point blank ae stun"

/return
  


|***
 * note: main enchanter routine
 * use: 
 ***|
sub main_ENC(bool _debug)
	DEBUG \atmain_ENC\ax()
	
			
	/if (AUTO) {
		/if (${maChr.Find[swAACrystals].Value}) /call check_crystals FALSE
		/if (${maChr.Find[stPctGather].Value}) /call check_gather FALSE
		/if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value}
	}

	
	/while (${check_combat_status[FALSE]}) {
		GETINPUT
		CHECKREZ
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure FALSE
		/call control_routine FALSE
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			/call set_combat FALSE
			/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
			/if (!${timer_Check_deBuffs}) /call check_deBuffs FALSE TARGET
			/call control_routine FALSE

			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value}
			/if (PET || SWARM) /call send_pet FALSE TARGET
			/if (MELEE) /call use_skill_melee TARGET
			/call misc_combat_${Me.Class.ShortName} FALSE

			/call control_routine FALSE

			/if (DOT) /call cast_dot FALSE lsDot TARGET
			/call control_routine FALSE
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/call control_routine FALSE
			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_list FALSE stTerror lsTerrorToon
			}
			/call control_routine FALSE
			| /if (${swAlliance}) /call trigger_alliance TARGET

			/call check_item_cycle FALSE TARGET
			
		}

	}

	/if (AUTO) {	
		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
		/if (${maChr.Find[stPctGather].Value}) /call check_gather FALSE
		| /if (${switchCharmPet} && ${swPet} && !${Me.Pet.ID}) /call find_mob_to_charm
	}

/return TRUE




|***
 * note: basic DPS discs
 * use: 
 ***|
sub misc_combat_ENC(bool _debug)
	DEBUG \atmisc_combat_ENC\ax()
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE
	
	/if (!${is_defensive_active[${_debug}, FALSE, SAFEENV]}) /call cast_defensive_cycle ${_debug}

	
	| use AA beam of slumber?
	/if (${maChr.Find[swAABoS].Value} && ${madeBuff.Find[swMez].Value}) {
		/if (${Spawn[TARGET].NearestSpawn[3,npc radius 30].ID} && ${Spawn[TARGET].Distance3D} <= 200 && ${Spawn[TARGET].Distance3D} >= ${setMobAgro}*.5) {
			/if (${Me.AltAbilityReady[Beam of Slumber]}) {
				/if (${Me.Casting.ID}) /stopcast
				/face ID TARGET ${If[${maHome.Find[swFaceFast].Value},fast,]}
				/if (${is_ready[FALSE, "Beam of Slumber"]}) /call voodoo FALSE "Beam of Slumber" 0 FALSE
			} 
		}
	}	
	
	
	

	| swarm pet
	/if (SWARM && AGRO) {
		/if (${is_ready[FALSE, "Phantasmal Opponent"]} && ${Target.PctHPs} <= ${maMinion.Find[stPctSwarmEngage].Value}) {
			/call voodoo FALSE "Phantasmal Opponent" 0 FALSE
		}
	}

	| group DPS bonus
	/if (${is_ready[FALSE, "Chromatic Haze"]}) {
		/call voodoo FALSE "Chromatic Haze" 0 FALSE
	}
	
/return



|***
 * note: Keep the chanter stocked with self AA crystals
 * use: /call check_crystals DEBUG
 ***|
sub check_crystals(bool _debug)
	DEBUG \atcheck_crystals\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE

	/if (!${FindItemCount[${Spell[Azure Mind Crystal].Base[1]}]}) {
		/if (${is_ready[FALSE, "Azure Mind Crystal"]}) {
			/call voodoo FALSE "Azure Mind Crystal" 0 FALSE
		}
	}

	/if (!${FindItemCount[${Spell[Sanguine Mind Crystal].Base[1]}]}) {
		/if (${is_ready[FALSE, "Sanguine Mind Crystal"]}) {
			/call voodoo FALSE "Sanguine Mind Crystal" 0 FALSE
		}
	}

	/if (${maChr.Find[stPctGather].Value}) { 

	}

/return



|***
 * note: needs manna!!!
 * use: /call check_gather DEBUG
 ***|
sub check_gather(bool _debug)
	DEBUG \atcheck_gather\ax()
	
	/if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) /return FALSE
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE

	| AA manna crystal
	/if (${maChr.Find[swAACrystals].Value}) {
		/if (${is_ready[FALSE, "${Spell[Sanguine Mind Crystal].Base[1]}"]}) {
			/call voodoo FALSE "${Spell[Sanguine Mind Crystal].Base[1]}" item 0 FALSE
			/return
		}
	}

	| aa Gather Mana 
	/if (${is_ready[FALSE, "Gather Mana"]}) {
		/call voodoo FALSE "Gather Mana" 0 FALSE
		/return
	}

/return




|***
 * note: chanter self healing? like this is really gonna work. wtf 
 * use: /call check_heal_${Me.Class.ShortName} FALSE Self
 ***|
sub check_heal_ENC(bool _debug)
	DEBUG \atcheck_heal_ENC\ax()
	/if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) /return FALSE

	ISMEDEAD
	CHECKEXIT
	CHECKTIE
	
	| AA Sanguine mind crystal summoned item 5 min
	/if (${maChr.Find[swAACrystals].Value}) {
		/if (${is_ready[FALSE, "${healCrystal}"]}) {
			/call voodoo FALSE "${healCrystal}" 0 FALSE
			/return
		}
	}	
	
	| 3 decoys 10 min
	/if (${is_ready[FALSE, "Doppelganger"]}) {
		/call voodoo FALSE "Doppelganger" ${Me.ID} FALSE
		/return
	}
	
	| rune and anti hate 20 min
	/if (${is_ready[FALSE, "Dimensional Shield"]}) {
		/call voodoo FALSE "Dimensional Shield" 0 FALSE
		/return
	}
	
/return



|***
 * note: toast should not be burnt
 * use: establish all comp/trigger skills
 ***| 
sub burn_ENC(bool _debug)
	DEBUG \atburn_ENC\ax()
	
	CHECKEXIT
	GETINPUT
	
	
	/if (!${is_defensive_active[${_debug}, BURN, SAFEENV]}) /call cast_defensive_cycle ${_debug}
	
	/if (${is_ready[FALSE, "Illusions of Grandeur"]}) {
		/call voodoo ${_debug} "Illusions of Grandeur" 0 FALSE
	}

	/if (${is_ready[FALSE, "Mental Contortion"]}) {
		/call voodoo ${_debug} "Mental Contortion" TARGET FALSE
	}

	/call cast_AA_spire ${_debug}

	/if (${is_ready[FALSE, "Improved Twincast"]}) {
		/call voodoo ${_debug} "Improved Twincast" 0 FALSE
	}

	| # DD spell damage bonus
	/if (${is_ready[FALSE, "Calculated Insanity"]}) {
		/call voodoo ${_debug} "Calculated Insanity" 0 FALSE
	}

	
	/if (${is_ready[FALSE, "Silent Casting"]}) {
		/call voodoo ${_debug} "Silent Casting" 0 FALSE
	} 
	
	/if (${is_ready[FALSE, "Dimensional Shield"]}) {
		/call voodoo ${_debug} "Dimensional Shield" 0 FALSE
	}

	| resist mod
	/if (${is_ready[FALSE, "Focus of Arcanum"]}) /call voodoo ${_debug} "Focus of Arcanum" 0 FALSE

	| Group DD Bonus
	/if (${is_ready[FALSE, "Chromatic Haze"]}) /call voodoo ${_debug} "Chromatic Haze" 0 FALSE
	
	/call cast_glyph ${_debug}

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
				/if (${is_ready[FALSE, spell, "${spellpbaeMez}", ${Me.ID}]}) /call voodoo FALSE "${spellpbaeMez}" 0 0 check_for_adds

				/if (${Target.Level} <= 46) {
					/if (${is_ready[FALSE, alt, "Dire Charm", ${_tmpID}]}) /call voodoo FALSE "Dire Charm" alt ${_tmpID} FALSE					
				} else {
					/if (${is_ready[FALSE, spell, "${spellCharm}", ${_tmpID}]}) /call voodoo FALSE "${spellCharm}" 0 ${_tmpID} FALSE
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
	DEBUG \atset_control_ENC\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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
		
	| use mana click items
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[FALSE, stPctManaRecovery, "${_verbage}", 0, 99]}) /return
		
	| use AA crystals
	} else /if (${_type.Equal[crystal]}) {
		/varset dump ${set_switch_env[${_debug}, swAACrystals, TRUE, ${_verbage}]}
		
	| use AA Beam of Slumber
	} else /if (${_type.Equal[bos]}) {
		/varset dump ${set_switch_env[${_debug}, swAABoS, TRUE, ${_verbage}]}
		
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
		/if (${li.Value.Equal[manarecover]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaRecovery].Value},${num}${maChr.Find[stPctManaRecovery].Value}\ax,${r0}]}
		/if (${li.Value.Equal[crystal]}) /varset _out ${_out}:${If[${maChr.Find[swAACrystals].Value},${on},${off}]}
		/if (${li.Value.Equal[bos]}) /varset _out ${_out}:${If[${maChr.Find[swAABoS].Value},${on},${off}]}

		/varset dump ${li.Advance}
	}	
	/varset dump ${out[14, "${_out}"]}

/return






