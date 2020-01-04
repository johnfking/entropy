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
	/call set_data FALSE maData stSpire																	map outer "Spire of Chivalry"
	/call set_data FALSE maData stSynergy																map outer "Knight's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Fiery Defender"
	/call set_data FALSE maData stEpic15																map outer "Heartwood Blade"
	/call set_data FALSE maData stEpic2																	map outer "Nightbane, Sword of the Valiant"
	/call set_data FALSE maData stFade																	map outer "Balefire Burst"
	/call set_data_map FALSE maChr stAAUnity														"Divine Protector's Unity"

	| tank shield flash
	/call set_data_map FALSE maChr stPctShieldFlash											0															"${auxnum} % of HP to try to get shield flash running"
	/call set_data_map FALSE maChr stCountShieldFlash										0															"${auxnum} # of mobs to hit shield flash"

	| Rampage Tank 
	/call set_data_map FALSE maChr stRampageTank												FALSE													"${auxna} sets a rampage tank name for healing exclusion"

	| attraction AA used for pulling / agro
	/call set_data_map FALSE maChr swAttraction													FALSE													"${auxsw} use the attraction aa to help with agro/pulling"
 
 	/call set_data_map FALSE maChr stRest 															PREGEN|1											"${auxna} name of stamina rest disc"
 	/call set_data_map FALSE maChr stHiatus															PREGEN|1											"${auxna} name of hiatus stamina recovery disc"
 	/call set_data_map FALSE maChr swHiatus															FALSE													"${auxsw} use hiatus rest disc"

 	/call set_data_map FALSE maChr stAlliance 													PREGEN|1											"${auxna} alliance spell .. damn useless thing"

	| heals
 	/call set_data_list FALSE maChr lsHealPanic 												PREGEN|3											"${auxli} panic instant no target spells"
 	/call set_data_list FALSE maChr lsHealTouch 												PREGEN|3											"${auxli} touch line of spells"
 	/call set_data_list FALSE maChr lsHealBurst 												PREGEN|3											"${auxli} burst healing line of spells"
 	/call set_data_list FALSE maChr lsHealToT		 												PREGEN|3											"${auxli} targets target healing line of spells"
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

	/declare lsHealOrdertot list outer
	/varset dump ${lsHealOrderpassive.Delimiter["|"]}
	/varset dump ${lsHealOrderpassive.Append[lsHealToT]}

/return TRUE
 
 
 
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

		/if (${maChr.Find[stPctShieldFlash].Value} || ${maChr.Find[stCountShieldFlash].Value}) /call cast_shield_flash FALSE aa

		| aa LoH
		/if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
		  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
				/if (${is_ready[${_debug}, "Lay on Hands"]}) {
					/call voodoo ${_debug} "Lay on Hands" ${Me.ID} FALSE
				}
			}
		}	

		/if (!${timer_Check_Marrs_Gift} && ${maChr.Find[stPctMG].Value} && (${Me.PctMana} <= ${maChr.Find[stPctMG].Value} || ${Me.PctHPs} <= ${maChr.Find[stPctMG].Value})) {
			/call cast_AA_marrs_gift
		}
	}
		
	/while (${check_combat_status[FALSE]}) {
		/call check_heal_routines
		/if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp FALSE
		/if (${check_class_loop[FALSE]}) /return
		
		/if (VALID) {
			/call set_combat FALSE
			/if (AGRO) /call get_agro_${Me.Class.ShortName} FALSE TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
			/call check_heal_routines
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
			/if (MELEE) /call do_melee FALSE TARGET
			/if (DOT) /call cast_dot ${maDebug.Find[dot].Value} lsDoT TARGET
			/if (NUKE) /call cast_nuke ${maDebug.Find[nuke].Value} lsNuke TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
			/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			| /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
			/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) {
				/call check_harmonious
			}
		
			/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
				/call check_buff_cycle_temp_self ${maDebug.Find[temp].Value} stSteel 
				/call check_buff_cycle_temp_self ${maDebug.Find[temp].Value} stStance
			}			
			
		}

		/if (AUTO) {	
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			CHECKFADE
			/if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[rest].Value}
		}
	}

/return TRUE



|***
 * note: all other combat .. things
 * use: /call misc_combat_CLS DEBUG [TARGETID]
 ***|
sub misc_combat_PAL(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_${Me.Class.ShortName\ax(\a-w${_tmpID}\ax)
	
	ISMEDEAD
	ISTMPIDDEAD

	CHECKEXIT
	GETINPUT
	CHECKTIE

	/call check_heal_routines
	/if (SWARM || PET) /call send_pet ${maDebug.Find[minion].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	| spam rune
	/if (${is_ready[${_debug}, "${maChr.Find[stTempRune].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stTempRune].Value}" 0 FALSE
	}

	ISTMPIDDEAD
	
	/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
		
	/if (${maChr.Find[stPctShieldFlash].Value} || ${maChr.Find[stCountShieldFlash].Value}) /call cast_shield_flash FALSE aa
	
	| AA inquisitors armor stacking control
	/if (${SpawnCount[npc OVERLOS radius 30 targetable]} >= ${maChr.Find[stCountDefensive].Value} && ${maChr.Find[stCountDefensive].Value}) {
		/if (!${Me.Song[Group Armor of the Inquisitor].ID}) { 
			/if (${is_ready[${_debug}, "Armor of the Inquisitor"]}) {
				/call voodoo ${_debug} "Armor of the Inquisitor" 0 FALSE
			}

		| dont use group armor in raids
		} else /if (!${Me.Song[Armor of the Inquisitor].ID} && !${Raid.Members}) {
			/if (${is_ready[${_debug}, "Group Armor of the Inquisitor"]}) {
				/call voodoo ${_debug} "Group Armor of the Inquisitor" 0 FALSE
			}
		}
	}
	
	ISTMPIDDEAD

	| undead cripple
	/if (${Target.Body.Name.Equal[Undead]}) {
		/if (${is_ready[${_debug}, "Vanquish the Fallen"]}) {
			/call voodoo ${_debug} "Vanquish the Fallen" ${_tmpID} FALSE
		}
	}
	
/return TRUE 



|***
 * note: I'm more important then you are!
 * use: /call get_agro_PAL DEBUG [TARRGETID]
 ***|
sub get_agro_PAL(bool _debug, int _tmpID)
	DEBUG \atget_agro_PAL\ax()
	
	ISMEDEAD
	CHECKEXIT	  
	GETINPUT

	ISTMPIDDEAD
	
	/call set_combat ${_debug}
	/call check_heal_routines
	/if (${maChr.Find[stPctShieldFlash].Value} || ${maChr.Find[stCountShieldFlash].Value}) /call cast_shield_flash FALSE aa
	
	| aa LoH
	/if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
	  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "Lay on Hands"]}) {
				/call voodoo ${_debug} "Lay on Hands" ${Me.ID} FALSE
			}
		}
	}	

	| challenge
	/call cast_challenge ${_debug} ${_tmpID} 

	| chain stun
	/if (${is_ready[${_debug}, "${maChr.Find[stStun].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stStun].Value}" ${_tmpID}  FALSE
	}

	/declare _coutAE int local 0
	
	| AE Stuns
	/if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= AE# && AE#) {
		/varset _coutAE ${SpawnCount[npc OVERLOS radius 60 PS4 targetable]}
	
		/if (!${Raid.Members} || ${Raid.Members} && ${Raid.MainAssist.Name.Equal[${Me.DisplayName}]}) {
			/if (${_coutAE} > 1) {
				/if (${is_ready[${_debug}, "Hallowed Lodestar"]}) {
					/call voodoo ${_debug} "Hallowed Lodestar" ${_tmpID} FALSE
				}
			} else /if (${_coutAE} > 4) {
				/if (${is_ready[${_debug}, "Beacon of the Righteous"]}) {
					/call voodoo ${_debug} "Beacon of the Righteous" 0 ${_tmpID} 
				}
			}
		}
	}
	ISTMPIDDEAD

	/if (${Me.TargetOfTarget.Name.Equal[${Me.DisplayName}]}) /return
	/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID} ]}) /return FALSE

	| AA pet hate (the little sword thingie)
	/if (${Me.PctAggro} < 80 && ${is_ready[${_debug}, "Projection of Piety"]}) {
		/call voodoo ${_debug} "Projection of Piety" ${_tmpID} FALSE
	}

	| snap hate
	/if (!${Raid.Members}) {
		/if (${Me.PctAggro} < 60 && ${is_ready[${_debug}, "Ageless Enmity"]}) {
			/call voodoo ${_debug} "Ageless Enmity" ${_tmpID} FALSE
		}
	}

	| named fake hate
	/if ((${Me.PctAggro} < 60 || ${lsZoneNamed.Contains[${Target.DisplayName}]}) && ${is_ready[${_debug}, "${maChr.Find[stUnflinching].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stUnflinching].Value}" ${_tmpID} FALSE
	}

	| Valiant Defense line. agro+heal
	/if (${Me.PctAggro} < 95 && ${is_ready[${_debug}, "${maChr.Find[stValiant].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stValiant].Value}" ${_tmpID} FALSE
	}
	
/return TRUE



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
	
	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}
	
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return	FALSE
	}

	| Panic Splash
	/if (${Group.Members}) {
	  /if (${hurtPctHP} <= 45*.PCTHO) {
			/if (${is_ready[${_debug}, "${maChr.Find[stSplashPanic].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stSplashPanic].Value}" 0 FALSE
			}
		}
	}
	
	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

	| ToT heals
	/if (${Select[${_type},tot]} && ${maHeal.Find[swHealToT].Value}) {

		/if (${hurtCount} >= 1)  {
			
			| aa LoH targets target
			/if (${maChr.Find[stPctLoH].Value}) {
			  /if (${Me.TargetOfTarget.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
					/if (${is_ready[${maDebug.Find[tot].Value}, "Lay on Hands"]}) {
						/call voodoo ${maDebug.Find[tot].Value} "Lay on Hands" ${Target.ID} FALSE
						/return TRUE
					}
				}
			}	
			
			/call cast_list ${maDebug.Find[tot].Value} lsHealToT ${hurtID}
			/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE			
		}
	}

	| group only heals
	/if (${Select[${_type},group]} && ${maHeal.Find[swHealGroup].Value}) {
		| Group Splash
	  /if (${hurtCount} >= 1) {
	    /if (${is_ready[${_debug}, "${maChr.Find[stSplash].Value}"]}) {
	    	/call voodoo ${_debug} "${splashGroup}" 0 FALSE
	    	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
	    }
	  } 
			
		| AA Splash
	  /if (${hurtCount} >= 1) {
	  	/if (${is_ready[${_debug}, "Hand of Piety"]}) {
	  		/call voodoo ${_debug} "Hand of Piety" 0 FALSE
	  		/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
	  	}
	  }
	  
		| Hand of Piety AA Group heal	
		/if (${hurtCount} >= 1) {
			/if (${is_ready[${_debug}, "Hand of Piety"]}) {
				/call voodoo ${_debug} "Hand of Piety" 0 FALSE
				/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
			}
		}

		| heal disc
		/if (${hurtCount} >= 1) {
			/if (${is_ready[${_debug}, "${maChr.Find[stReflexive].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stReflexive].Value}" TARGET FALSE
				/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
			}
		}	
	  
	}

	| self only heals
	/if (${Select[${_type},self]} && ${maHeal.Find[swHealSelf].Value}) {
		| aa LoH
		/if (${maChr.Find[stPctLoH].Value}) {
		  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
				/if (${is_ready[${_debug}, "Lay on Hands"]}) {
					/call voodoo ${_debug} "Lay on Hands" ${Me.ID} FALSE
				}
			}
		}	

		| self only panich heals
		/call cast_list FALSE lsHealPanic ${Me.ID}
		/call get_hurt ${maDebug.Find[hurt].Value} ${_type} 
	
		/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
	}	
	
	| AA Gift of Life -- not in raids
	/if (${Me.PctHPs} < ${maHeal.Find[stHealPointPAL].Value}*.30*.PCTHO && !${Raid.Members}) {
		/if (${is_ready[${_debug}, "Gift of Life"]}) {
			/call voodoo ${_debug} "Gift of Life" 0 FALSE
		}
	}	
	
 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
	CHECKREZ
	CHECKFADE
		
	/declare _liHM listiterator local
	/vardata _liHM lsHealOrder${maHeal.Find[stHealMode].Value}.First.Clone
	/while (!${_liHM.IsEnd}) {
		/call cast_list ${_debug} "${_liHM.Value}" ${hurtID}
		/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
		/varset dump ${_liHM.Advance}
	}		
	
	SETHUD FALSE
	
/return TRUE



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
	
	/if (!${is_ready[${_debug}, "Marr's Gift"]}) /return FALSE
	
	/if (SAFEZONE || !${maHeal.Find[swHealGroup].Value}) /return FALSE
	
	/if ((${Me.PctMana} < ${maChr.Find[stPctMG].Value}*.PCTHO) || (${Me.PctHPs} < ${maChr.Find[stPctMG].Value}*.PCTHO)) {
		/call voodoo ${_debug} "Marr's Gift" 0 FALSE
	} 
/return	



|***
 * note: Paladin burn routines (like pallys can do any DPS.!)
 * use: seriously why bother
 ***| 
sub burn_PAL(bool _debug, int _tmpID)
	DEBUG \atburn_PAL\ax()

	ISMEDEAD
	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return FALSE

	CHECKEXIT
	GETINPUT
	CHECKTIE
	
	/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) /call cast_defensive_cycle ${maDebug.Find[def].Value}
	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} TARGET	
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

	| deflection
	/if (!${Bool[${Me.ActiveDisc}]} && ${Me.PctHPs} < 50*.PCTHO) {
		/if (${is_ready[${_debug}, "Deflection Discipline"]}) {
			/call voodoo ${_debug} "Deflection Discipline" 0 FALSE
			/return
		}
	}
	
	/if (${maChr.Find[stPctShieldFlash].Value} || ${maChr.Find[stCountShieldFlash].Value}) /call cast_shield_flash FALSE aa

	| aa LoH
	/if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
	  /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "Lay on Hands"]}) {
				/call voodoo ${_debug} "Lay on Hands" ${Me.ID} FALSE
			}
		}
	}	

	ISTMPIDDEAD

	/call cast_AA_spire ${maDebug.Find[spire].Value}

	/if (${is_ready[${_debug}, "Inquisitor's Judgement"]}) {
		/call voodoo ${_debug} "Inquisitor's Judgement" 0 FALSE
	}
	/if (${is_ready[${_debug}, "Valorous Rage"]]}) {
		/call voodoo ${_debug} "Valorous Rage" 0 FALSE
	}

	/call cast_glyph ${maDebug.Find[glyph].Value}
	
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
	} else /if (${_type.Equal[manarecover]}) {
		/if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return
		
	| set spire to be used
	} else /if (${_type.Equal[spire]}) {
		/call set_control_chr_spire ${_debug} ${_verbage} ${_silent}
		/return

	| set AA fade use %	
	} else /if (${_type.Equal[fade]}) {
		/if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return

	| to Yaulp or not		
	} else /if (${_type.Equal[yaulp]}) {
		/varset dump ${set_switch_env[${_debug}, swYaulp, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return

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
	
		/vardata mi _maStun.First.Clone
		OUT /chr stun [\a-wuse\ax|\a-wdivine\ax|\a-wforce\ax|\a-wdisruptive\ax]
		/varset _out ${_out} \awuse\ax:${If[${maChr.Find[swStun].Value},${on},${off}]} ${dot}
		/while (!${mi.IsEnd}) {
			/varset _out ${_out} ${If[${_maStun.First.Value.Equal[${mi.Value}]},,${dot}]} ${If[${maChr.Find[stStun].Value.Equal[${mi.Value}]},${good}${mi.Value}\ax,${info}${mi.Value}\ax]}
			/varset dump ${mi.Advance}
		}	
		OUT /chr stun ${_out}
		/return

	| shield flash
	} else /if (${_type.Equal[flash]}) {
		
		/if (${_verbage.Equal[pct]}) {
			/if (!${set_control_num_range[${_debug}, stPctShieldFlash, "${_silent}", 0, 99]}) /return
		} else /if (${_verbage.Equal[count]}) {
			/if (!${set_control_num_range[${_debug}, stCountShieldFlash, "${_silent}", 0, 99]}) /return
		}

		OUT /chr flash ${dot} ${info}pct\ax:${If[${Bool[${maChr.Find[stPctShieldFlash].Value}]},${num}${maChr.Find[stPctShieldFlash].Value}\ax,${r0}]} ${dot} ${info}count\ax:${If[${Bool[${maChr.Find[stCountShieldFlash].Value}]},${num}${maChr.Find[stCountShieldFlash].Value}\ax,${r0}]}
		/return

	| Lay on hands
	} else /if (${_type.Equal[loh]}) {
		/if (!${set_control_num_range[${_debug}, stPctLoH, "${_verbage}", 0, 99]}) /return

	| Marr's Gift
	} else /if (${_type.Equal[mg]}) {
		/if (!${set_control_num_range[${_debug}, stPctMG, "${_verbage}", 0, 99]}) /return

	| harmonious
	} else /if (${_type.Equal[harmonious]}) {
		/varset dump ${set_switch_env[${_debug}, swHarmonious, TRUE, ${_verbage}]}
	
	| attraction
	} else /if (${_type.Equal[attraction]}) {
		/varset dump ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}

	| use hiatus disc
	} else /if (${_type.Equal[hiatus]}) {
		/varset dump ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}
	
	}

	/if (${_silent.Equal[SILENT]}) /return 
	/varset dump ${set_control_output[${_debug}, maChr, chr, maChrControl]}
	
/return TRUE
