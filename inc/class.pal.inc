|***
 * entropy
 * class.pal.inc
 ***|
 


|***
 * note: primary initilization of all paladin non shared data
 * use: are you a paladin?
 ***|
sub set_PAL(bool _debug)
	DEBUG \atset_PAL\ax()
	/call set_data FALSE maData stSpire1																map outer "Fundament: First Spire of Holiness"
	/call set_data FALSE maData stSpire2																map outer "Fundament: Second Spire of Holiness"
	/call set_data FALSE maData stSpire3																map outer "Fundament: Third Spire of Holiness"
	/call set_data FALSE maData stSynergy																map outer "Knight's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Fiery Defender"
	/call set_data FALSE maData stEpic15																map outer "Heartwood Blade"
	/call set_data FALSE maData stEpic2																	map outer "Nightbane, Sword of the Valiant"
	/call set_data FALSE maData stFade																	map outer "Balefire Burst"
	/call set_data_map FALSE maChr stAAUnity														"Divine Protector's Unity"

	| tank shield flash
	/call set_data_map FALSE maChr stPctShieldFlash											0															"${auxnum} % of HP to try to get shield flash running"

	| Rampage Tank 
	/call set_data_map FALSE maChr stRampageTank												FALSE													"${auxna} sets a rampage tank name for healing exclusion"

	| attraction AA used for pulling / agro
	/call set_data_map FALSE maChr swAttraction													FALSE													"${auxsw} use the attraction aa to help with agro/pulling"
 
 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxli} remedy spells to use"
 	/call set_data_map FALSE maChr stAlliance 													PREGEN|1											"${auxna} alliance spell .. damn useless thing"

	| heals
 	/call set_data_list FALSE maChr lsHealPanic 												PREGEN|3											"${auxli} panic instant no target spells"
 	/call set_data_list FALSE maChr lsHealTouch 												PREGEN|3											"${auxli} touch line of spells"
 	/call set_data_list FALSE maChr lsHealBurst 												PREGEN|3											"${auxli} burst healing line of spells"
 	/call set_data_map FALSE maChr stHealLight 													PREGEN|1											"${auxna} light heal spell"
 	/call set_data_map FALSE maChr stSplashPanic 												PREGEN|1											"${auxna} fast cast panic splash spell"
 	/call set_data_map FALSE maChr stSplashCure 												PREGEN|1											"${auxna} ground target ae cure/heal spell"
 	/call set_data_map FALSE maChr stSplash 														PREGEN|1											"${auxna} ground target ae heal spell"

	| stance
	/varset dump ${set_data_timer[FALSE, Check_Stance, 7.5m]}
	/call set_data_map FALSE maChr swStance 														FALSE													"${auxsw} use stance self buff"
 	/call set_data_map FALSE maChr stStance 														PREGEN|1											"${auxna} name of the self stance buff"

	| harmonions
	/varset dump ${set_data_timer[FALSE, Check_Harmonious, 1m]}
	/call set_data_map FALSE maChr swHarmonious 												FALSE													"${auxsw} use harmonious"
 	/call set_data_map FALSE maChr stHarmonious 												PREGEN|1											"${auxna} name of the harmonions spell"

	| steel
	/varset dump ${set_data_timer[FALSE, Check_Steel, 2m]}
 	/call set_data_map FALSE maChr swSteel 															FALSE													"${auxsw} use steel self buff"
 	/call set_data_map FALSE maChr stSteel 															PREGEN|1											"${auxna} name of the steel self buff"

	| Marrs Gift
	/varset dump ${set_data_timer[FALSE, Check_Marrs_Gift, 10s]}
	/call set_data_map FALSE maChr stPctMG															0															"${auxnum} % of mana to use mars gift aa"


 	/call set_data_map FALSE maChr stTempRune 													PREGEN|1											"${auxna} fast reuse disc rune"
 	/call set_data_map FALSE maChr stChallenge 													PREGEN|1											"${auxna} challenge spell for agro"
 	/call set_data_map FALSE maChr stValiant 														PREGEN|1											"${auxna} vlaiant spell for defensive"
 	/call set_data_map FALSE maChr stUnflinching 												PREGEN|1											"${auxna} name of unflinching disc to use for agro help"
 	/call set_data_map FALSE maChr stReflexive 													PREGEN|1											"${auxna} name of reflexive disc"
  /call set_data_map FALSE maChr stDichotomic 												PREGEN|1											"${auxna} dichotomic name"

 	| lay on hands
	/call set_data_map FALSE maChr stPctLoH															0															"${auxnum} % of mana to use aa lay on hands. requires self healing to be enabled"

	| yaulp
	/varset dump ${set_data_timer[FALSE, Check_Yaulp, 6s]}
	/call set_data_map FALSE maChr swYaulp 															FALSE													"${auxsw} use yaulp"
  /call set_data_map FALSE maChr stYaulp															PREGEN|1											"${auxna} name of yaulp to use"

	| aa stun
	/call set_data_map FALSE maChr swStun 															FALSE													"${auxsw} use AA stun"
  /call set_data_map FALSE maChr stStun																FALSE													"${auxna} name of AA stun to use"
	
 
 
	| set heal orders
	/declare lsHealOrderbase list outer
	/varset dump ${lsHealOrderbase.Delimiter["|"]}
	/varset dump ${lsHealOrderbase.Append[lsHealBaseLine]}

	/declare lsHealOrderaggressive list outer
	/varset dump ${lsHealOrderaggressive.Delimiter["|"]}
	/varset dump ${lsHealOrderaggressive.Append[lsHealTouch|lsHealBurst]}

	/declare lsHealOrderpassive list outer
	/varset dump ${lsHealOrderpassive.Delimiter["|"]}
	/varset dump ${lsHealOrderpassive.Append[lsHealTouch|lsHealBurst]}
 
/return 
 
 
 
|***
 * note: Main Paladin Routine
 * use: primarly for making unicorn jokes at. also useful for throwing at raid mobs as speed bumps.
 ***|
sub main_PAL(bool _debug)
	DEBUG \atmain_PAL\ax()

	GETINPUT
	ISMEDEAD
	CHECKTIE
	CHECKREZ
	
	/if (AUTO) {
	  /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
	    /call check_xtarget_build FALSE
		}

		/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa

		| aa LoH
		/if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
		  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
				/if (${is_ready[FALSE, "Lay on Hands"]}) {
					/call voodoo FALSE "Lay on Hands" ${Me.ID} FALSE
				}
			}
		}	

		/if (!${timer_Check_Marrs_Gift} && ${maChr.Find[stPctMG].Value} && (${Me.PctMana} <= ${maChr.Find[stPctMG].Value} || ${Me.PctHPs} <= ${maChr.Find[stPctMG].Value})) {
			/call cast_AA_marrs_gift
		}
	}
		
	/while (${check_combat_status[FALSE]}) {
		/if (${maHeal.Find[swHealSelf].Value} && !${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Self
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Group
		/if (${maHeal.Find[swHealXTarget].Value}) /call get_hurt_${Me.Class.ShortName} FALSE XTarget
		/if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) /call get_hurt_${Me.Class.ShortName} FALSE Pet
		/if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call cast_cure FALSE
		/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE		
		/if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp FALSE
		/if (${check_class_loop[FALSE]}) /return
		
		/if (VALID) {
			/call set_combat FALSE
			/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
			/if (AGRO) /call get_agro_${Me.Class.ShortName}
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (BURN) /call burn_${Me.Class.ShortName} FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_DOT FALSE lsDoT TARGET
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/call check_item_cycle FALSE TARGET
			| /call check_deBuffs FALSE TARGET
			| /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
			/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) {
				/call check_harmonious
			}
		
			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_self FALSE stSteel 
				/call check_buff_cycle_temp_self FALSE stStance
			}			
			
		}

		/if (AUTO) {	
			/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
			CHECKFADE
			| /if (${validate_class[FALSE, ${lstClassMelee}]}) /call cast_stamina_recovery
		}
	}

/return



|***
 * note: all other combat .. things
 * use: 
 ***|
sub misc_combat_PAL(bool _debug)
	DEBUG \atmisc_combat_PAL\ax()
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[FALSE, TARGET]}) /return FALSE
	CHECKTIE

	/call set_combat FALSE

	| spam rune
	/if (${is_ready[FALSE, "${maChr.Find[stTempRune].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stTempRune].Value}" 0 FALSE
	}

	/call check_facing FALSE TARGET FORCE
	
	| defensive	
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
		
	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa
	
	| AA inquisitors armor stacking control
	/if (${SpawnCount[npc OVERLOS radius 30 targetable]} >= ${maChr.Find[stCountDefensive].Value} && ${maChr.Find[stCountDefensive].Value}) {
		/if (!${Me.Song[Group Armor of the Inquisitor].ID}) { 
			/if (${is_ready[FALSE, "Armor of the Inquisitor"]}) {
				/call voodoo FALSE "Armor of the Inquisitor" 0 FALSE
			}

		| dont use group armor in raids
		} else /if (!${Me.Song[Armor of the Inquisitor].ID} && !${Raid.Members}) {
			/if (${is_ready[FALSE, "Group Armor of the Inquisitor"]}) {
				/call voodoo FALSE "Group Armor of the Inquisitor" 0 FALSE
			}
		}
	}

	| undead cripple
	/if (${Target.Body.Name.Equal[Undead]}) {
		/if (${is_ready[FALSE, "Vanquish the Fallen"]}) {
			/call voodoo FALSE "Vanquish the Fallen" TARGET FALSE
		}
	}
	
/return



|***
 * note: I'm more important then you are!
 * use: /call get_agro_PAL
 ***|
sub get_agro_PAL(bool _debug)
	DEBUG \atget_agro_PAL\ax()
	
	ISMEDEAD
	CHECKEXIT	  
	GETINPUT

	/if (${is_target_dead[FALSE, TARGET]}) /return FALSE
	/call set_combat FALSE
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE
	/if (${maChr.Find[stPctShieldFlash].Value} && ${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa
	
	| aa LoH
	/if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
	  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Lay on Hands"]}) {
				/call voodoo FALSE "Lay on Hands" ${Me.ID} FALSE
			}
		}
	}	

	| challenge
	/call cast_challenge ${_debug} TARGET 

	| chain stun
	/if (${is_ready[FALSE, "${maChr.Find[stStun].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stStun].Value}" TARGET FALSE
	}

	/declare _coutAE int local 0
	
	| AE Stuns
	/if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= AE# && AE#) {
		/varset _coutAE ${SpawnCount[npc OVERLOS radius 60 PS4 targetable]}
	
		/if (!${Raid.Members} || ${Raid.Members} && ${Raid.MainAssist.Name.Equal[${Me.DisplayName}]}) {
			/if (${_coutAE} > 1) {
				/if (${is_ready[FALSE, alt, "Hallowed Lodestar"]}) {
					/call voodoo FALSE "Hallowed Lodestar" TARGET FALSE
				}
			} else /if (${_coutAE} > 4) {
				/if (${is_ready[FALSE, alt, "Beacon of the Righteous"]}) {
					/call voodoo FALSE "Beacon of the Righteous" 0 FALSE
				}
			}
		}
	}

	/if (${Me.TargetOfTarget.Name.Equal[${Me.DisplayName}]}) /return
	/if (${is_target_dead[FALSE, TARGET]}) /return FALSE

	| AA pet hate (the little sword thingie)
	/if (${Me.PctAggro} < 80 && ${is_ready[FALSE, "Projection of Piety"]}) {
		/call voodoo FALSE "Projection of Piety" TARGET FALSE
	}

	| snap hate
	/if (!${Raid.Members}) {
		/if (${Me.PctAggro} < 60 && ${is_ready[FALSE, "Ageless Enmity"]}) {
			/call voodoo FALSE "Ageless Enmity" TARGET FALSE
		}
	}

	| named fake hate
	/if ((${Me.PctAggro} < 60 || ${lsZoneNamed.Contains[${Target.DisplayName}]}) && ${is_ready[FALSE, "${maChr.Find[stUnflinching].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stUnflinching].Value}" TARGET FALSE
	}

	| Valiant Defense line. agro+heal
	/if (${Me.PctAggro} < 95 && ${is_ready[FALSE, "${maChr.Find[stValiant].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stValiant].Value}" TARGET FALSE
	}
/return



|***
 * note: Palading group heal. centered more to splashing
 * use: 
 ***|
sub check_heal_PAL(bool _debug, string _type)
	DEBUG \atcheck_heal_group_PAL\ax(\a-w${_type}\ax)
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type}"
	
	/call get_hurt FALSE ${_type}
	
	/if (!${hurtCount}) {
		| SETHUD FALSE
		/return	
	}

	| Panic Splash
	/if (${Group.Members}) {
	  /if (${hurtPctHP} <= 45*.PCTHO) {
			/if (${is_ready[FALSE, "${maChr.Find[stSplashPanic].Value}"]}) {
				/call voodoo FALSE "${maChr.Find[stSplashPanic].Value}" 0 FALSE
			}
		}
	}
	
	/call get_hurt FALSE ${_type}

	| group only heals
	/if (${Select[${_type},group]} && ${maHeal.Find[swHealGroup].Value}) {
		| Group Splash
	  /if (${hurtCount} >= 1) {
	    /if (${is_ready[FALSE, "${maChr.Find[stSplash].Value}"]}) {
	    	/call voodoo FALSE "${splashGroup}" 0 FALSE
	    	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
	    }
	  } 
			
		| AA Splash
	  /if (${hurtCount} >= 1) {
	  	/if (${is_ready[FALSE, "Hand of Piety"]}) {
	  		/call voodoo FALSE "Hand of Piety" 0 FALSE
	  		/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
	  	}
	  }
	  
		| Hand of Piety AA Group heal	
		/if (${hurtCount} >= 1) {
			/if (${is_ready[FALSE, "Hand of Piety"]}) {
				/call voodoo FALSE "Hand of Piety" 0 FALSE
				/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
			}
		}
		

		| heal disc
		/if (${hurtCount} >= 1) {
			/if (${is_ready[FALSE, "${maChr.Find[stReflexive].Value}"]}) {
				/call voodoo FALSE "${maChr.Find[stReflexive].Value}" TARGET FALSE
				/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
			}
		}	
	  
	}
	
	
	| self only heals
	/if (${Select[${_type},self]} && ${maHeal.Find[swHealSelf].Value}) {
		| aa LoH
		/if (${maChr.Find[stPctLoH].Value}) {
		  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
				/if (${is_ready[FALSE, "Lay on Hands"]}) {
					/call voodoo FALSE "Lay on Hands" ${Me.ID} FALSE
				}
			}
		}	

		| self only panich heals
		/call cast_list FALSE lsHealPanic ${Me.ID}
		/call get_hurt ${_debug} ${_type} 
	
		/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		/call click_item_heal
	}	
	
	| AA Gift of Life -- not in raids
	/if (${Me.PctHPs} < ${maHeal.Find[stHealPointPAL].Value}*.30*.PCTHO && !${Raid.Members}) {
		/if (${is_ready[FALSE, "Gift of Life"]}) {
			/call voodoo FALSE "Gift of Life" 0 FALSE
		}
	}	

	
 	/if (!${get_hurt[FALSE, ${_type}]}) /return FALSE
	CHECKREZ
	CHECKFADE
		
	/declare _liHM listiterator local
	/vardata _liHM lsHealOrder${maHeal.Find[stHealMode].Value}.First.Clone
	/while (!${_liHM.IsEnd}) {
		/call cast_list FALSE "${_liHM.Value}" ${hurtID}
		/if (!${get_hurt[FALSE, ${_type}]}) /return FALSE
		/varset dump ${_liHM.Advance}
	}		
	
	SETHUD FALSE
	
/return



|***
 * note: Mar's Gift 30ish k HP/15k manna 
 * use: /call cast_AA_marrs_gift [DEBUG]
 ***|
sub cast_AA_marrs_gift(bool _debug)
	DEBUG \atcast_AA_marrs_gift\ax()
	ISMEDEAD
	CHECKEXIT
  GETINPUT

	/varset dump ${set_data_timer[FALSE, Check_Marrs_Gift, RESTART]}
	
	/if (!${is_ready[FALSE, "Marr's Gift"]}) /return FALSE
	
	/if (SAFEZONE || !${maHeal.Find[swHealGroup].Value}) /return FALSE
	
	/if ((${Me.PctMana} < ${maChr.Find[stPctMG].Value}*.PCTHO) || (${Me.PctHPs} < ${maChr.Find[stPctMG].Value}*.PCTHO)) {
		/call voodoo FALSE "Marr's Gift" 0 FALSE
	} 
/return	



|***
 * note: ksspe pally steel famly running
 * use: /call check_steel [DEBUG]
 ***|
sub check_steel(bool _debug)
	DEBUG \atcheck_steel\ax()
	
	/if (SAFEZONE) /return
	ISMEDEAD
	CHECKEXIT
  GETINPUT

	/varset dump ${set_data_timer[FALSE, Check_Steel, RESTART]}
	
	/if (${is_ready[${_debug}, "${maChr.Find[swSteel].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[swSteel].Value}" 0 FALSE
	}
/return



|***
 * note: Paladin burn routines (like pallys can do any DPS.!)
 * use: seriously why bother
 ***| 
sub burn_PAL(bool _debug)
	DEBUG \atburn_PAL\ax()

	/if (${is_target_dead[FALSE, TARGET]}) /return FALSE
	ISMEDEAD
	CHECKEXIT
  GETINPUT

	/call check_facing ${_debug} TARGET FORCE

	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE

	| deflection
	/if (!${Bool[${Me.ActiveDisc}]} && ${Me.PctHPs} < 50*.PCTHO) {
		/if (${is_ready[FALSE, "Deflection Discipline"]}) {
			/call voodoo FALSE "Deflection Discipline" 0 FALSE
			/return
		}
	}
	
	/if (${Me.PctHPs} <= ${maChr.Find[stPctShieldFlash].Value} && ${maChr.Find[stPctShieldFlash].Value}) /call cast_shield_flash FALSE aa

	| aa LoH
	/if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
	  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Lay on Hands"]}) {
				/call voodoo FALSE "Lay on Hands" ${Me.ID} FALSE
			}
		}
	}	

	/call cast_AA_spire FALSE

	/if (${is_ready[FALSE, "Inquisitor's Judgement"]}) {
		/call voodoo FALSE "Inquisitor's Judgement" 0 FALSE
	}
	/if (${is_ready[FALSE, "Valorous Rage"]]}) {
		/call voodoo FALSE "Valorous Rage" 0 FALSE
	}

	/call cast_glyph FALSE
	
/return



|***
 *	DES: Alliance (Holly Alliance|spell)
 *	USE: 
 *  NOTE: 6 casts to fulm.
 ***|
sub cast_alliance_PAL(int _tmpID, bool _debug)
	DEBUG cast_alliance_${Me.Class.ShortName()

  | check alliance group/raid
  /if (!${Bool[${setAlliance}]}) /return FALSE
	/if (${setAlliance.Equal[raid]} && !${Raid.Members}) /return FALSE  
	/if (${setAlliance.Equal[group]} && (!${Bool[${Group.Members]} || ${Raid.Members})) /return FALSE  
	| enough paladins to make it worth casting?
	/if (${SpawnCount[paladin radius ${setMobAgro} zradius ${setzradius}]} < 3) /return FALSE
	| have the alliance on it already?
	/if (${Target.Buff[${${Me.Class.ShortName}Alliance}].ID}) /return FALSE
	| spell ready?
	| /if (!${check_ready[spell, "${${Me.Class.ShortName}Alliance}"]}) /return FALSE
	| no target
	/if (${is_target_dead[FALSE, ${_tmpID}]}) /return FALSE

	ISMEDEAD
	CHECKEXIT
  GETINPUT

	| /if (${validate_cast[FALSE, spell, "${${Me.Class.ShortName}Alliance}", ${_tmpID}]}) /call voodoo FALSE "${${Me.Class.ShortName}Alliance}" alt ${_tmpID} FALSE

/return TRUE



|***
 * note: paladin controls
 * use: 
 ***|
sub set_control_PAL(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_PAL\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

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

	| to Yaulp or not		
	} else /if (${_type.Equal[yaulp]}) {
		/varset dump ${set_switch_env[${_debug}, swYaulp, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[FALSE, stCountHealGroup, "${_verbage}", 0, 6]}) /return

	| Stun Control
	} else /if (${_type.Equal[stun]}) {

		/declare _maStun map local
		/varset dump ${_maStun.Add[divine,Divine Stun]}
		/varset dump ${_maStun.Add[force,Force of Disruption]}
		/varset dump ${_maStun.Add[disruptive,Disruptive Persecution]}
		
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[FALSE, swStun, TRUE, ${_silent}]}
		} else /if (${_maStun.Contains[${_verbage}]}) {
			/varset dump ${do_raw_edit[${_debug}, SILENT, stStun, "${_maStun.Find[${_verbage}].Value}"]}
		} 
	
		/declare _mi mapiterator local
		/vardata _mi _maStun.First.Clone
		OUT /chr stun [\a-wuse\ax|\a-wdivine\ax|\a-wforce\ax|\a-wdisruptive\ax]
		/varset _out ${_out} \awuse\ax:${If[${maChr.Find[swStun].Value},${on},${off}]} ${dot}
		/while (!${_mi.IsEnd}) {
			/varset _out ${_out} ${If[${_maStun.First.Value.Equal[${_mi.Value}]},,${dot}]} ${If[${maChr.Find[stStun].Value.Equal[${_mi.Value}]},${good}${_mi.Value}\ax,${info}${_mi.Value}\ax]}
			/varset dump ${_mi.Advance}
		}	
		OUT /chr stun ${_out}
		/return

	| shield flash
	} else /if (${_type.Equal[flash]}) {
		/if (!${set_control_num_range[FALSE, stPctShieldFlash, "${_verbage}", 0, 99]}) /return

	| Lay on hands
	} else /if (${_type.Equal[loh]}) {
		/if (!${set_control_num_range[FALSE, stPctLoH, "${_verbage}", 0, 99]}) /return

	| Marr's Gift
	} else /if (${_type.Equal[mg]}) {
		/if (!${set_control_num_range[FALSE, stPctMG, "${_verbage}", 0, 99]}) /return

	| harmonious
	} else /if (${_type.Equal[harmonious]}) {
		/varset dump ${set_switch_env[${_debug}, swHarmonious, TRUE, ${_verbage}]}
	
	| attraction
	} else /if (${_type.Equal[attraction]}) {
		/varset dump ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}
	
	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset _out
	/vardata li lsChr.First.Clone
	/while (!${li.IsEnd}) {
		/varset _out ${_out} ${If[${lsChr.First.Value.Equal[${li.Value}]},,${dot}]} ${info}${li.Value}\ax
		/if (${li.Value.Equal[yaulp]}) /varset _out ${_out}:${If[${maChr.Find[swYaulp].Value},${on},${off}]}
		/if (${li.Value.Equal[stun]}) /varset _out ${_out}:${submenu}
		/if (${li.Value.Equal[manaclick]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaClick].Value},${num}${maChr.Find[stPctManaClick].Value}\ax,${r0}]}
		/if (${li.Value.Equal[fade]}) /varset _out ${_out}:${If[${maChr.Find[stPctFade].Value},${num}${maChr.Find[stPctFade].Value}\ax,${r0}]}
		/if (${li.Value.Equal[ae]}) /varset _out ${_out}:${If[${maChr.Find[stCountAE].Value},${num}${maChr.Find[stCountAE].Value}\ax,${r0}]}
		/if (${li.Value.Equal[def]}) /varset _out ${_out}:${If[${maChr.Find[stCountDefensive].Value},${num}${maChr.Find[stCountDefensive].Value}\ax,${r0}]}
		/if (${li.Value.Equal[spire]}) /varset _out ${_out}:${submenu}
		/if (${li.Value.Equal[groupheal]}) /varset _out ${_out}:${If[${maChr.Find[stCountHealGroup].Value},${num}${maChr.Find[stCountHealGroup].Value}\ax,${r0}]}
		/if (${li.Value.Equal[flash]}) /varset _out ${_out}:${If[${maChr.Find[stPctShieldFlash].Value},${num}${maChr.Find[stPctShieldFlash].Value}\ax,${r0}]}
		/if (${li.Value.Equal[loh]}) /varset _out ${_out}:${If[${maChr.Find[stPctLoH].Value},${num}${maChr.Find[stPctLoH].Value}\ax,${r0}]}
		/if (${li.Value.Equal[mg]}) /varset _out ${_out}:${If[${maChr.Find[stPctMG].Value},${num}${maChr.Find[stPctMG].Value}\ax,${r0}]}
		/if (${li.Value.Equal[attraction]}) /varset _out ${_out}:${If[${maChr.Find[swAttraction].Value},${on},${off}]}
		/if (${li.Value.Equal[harmonious]}) /varset _out ${_out}:${If[${maChr.Find[swHarmonious].Value},${on},${off}]}
		/if (${li.Value.Equal[vp]}) /varset _out ${_out}:${If[${maChr.Find[stPctVP].Value},${num}${maChr.Find[stPctVP].Value}\ax,${r0}]}
		/if (${li.Value.Equal[splash]}) /varset _out ${_out}:${If[${maChr.Find[stCountSplash].Value},${num}${maChr.Find[stCountSplash].Value}\ax,${r0}]}
		/varset dump ${li.Advance}
	}	
	OUT /chr ${_out}
/return




