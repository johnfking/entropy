|***
 * entropy
 * class.clr.inc
 ***|
 


|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub set_CLR(bool _debug)
	DEBUG \atset_CLR\ax()

	/call set_data FALSE maData stSpire																	map outer "Spire of the Vicar"
	/call set_data FALSE maData stSynergy																map outer "Templar's Synergy"
	/call set_data FALSE maData stEpic1																	map outer "Water Sprinkler of Nem Ankh"
	/call set_data FALSE maData stEpic15																map outer "Harmony of the Soul"
	/call set_data FALSE maData stEpic2																	map outer "Aegis of Superior Divinity"
	/call set_data FALSE maData stFade																	map outer "Divine Peace"
	/call set_data FALSE maData stCompleteHeal													map outer "Complete Heal"
	/call set_data_map FALSE maChr stAAUnity														"Saint's Unity"								"${auxna} class aa unity buff"

	| Veturika's Preserverance
	/call set_data_map FALSE maChr stPctVP															0															"${auxnum} % of manna to use veturikas perseverance"
	
	| healing
 	/call set_data_list FALSE maChr lsHealRemedy												PREGEN|3											"${auxli} remedy spells to use"
 	/call set_data_list FALSE maChr lsHealRenewal												PREGEN|3											"${auxli} renewal spells to use"
 	/call set_data_list FALSE maChr lsHealIntervention									PREGEN|3											"${auxli} intervention spells to use"
 	/call set_data_list FALSE maChr lsHealLight													PREGEN|3											"${auxli} light spells to use"
 	/call set_data_map FALSE maChr stHealPanic													PREGEN|1											"${auxna} oh shit spell"

  /call set_data_map FALSE maChr swHealGroupOverTime									FALSE													"${auxsw} use group heal over time spells/aas"
  /call set_data_map FALSE maChr swCelestialRegen											FALSE													"${auxsw} use celestial regen aa"
  /call set_data_map FALSE maChr stHealGroupOverTime									PREGEN|1											"${auxna} group heal over time spell"


	/call set_data_map FALSE maChr swHealOverTime												FALSE													"${auxsw} % of members health to use heal over time"
	/call set_data_map FALSE maChr stHealOverTime												PREGEN|1											"${auxna} heal over time spelll"

	/call set_data_map FALSE maChr stCountHealGroup											0															"${auxnum} number in group hurt to use a group heal"
 	/call set_data_list FALSE maChr lsHealGroup													PREGEN|2											"${auxli} group heal spells"

	/call set_data_map FALSE maChr stAlliance														PREGEN|1											"${auxna} alliance spell"

	| issuance
	/call set_data_map FALSE maChr stCountIssuance											0															"${auxnum} nuymber of mobs in area to use issuance"
	/call set_data_map ${_debug} maChr stIssuance												PREGEN|1											"${auxna} issuance spell"

	| swarm pet
	/call set_data_map FALSE maChr stTempPet														PREGEN|1											"${auxna} temp hammer pet spell"

	| splash
	/call set_data_map FALSE maChr stCountSplash												0															"${auxnum} number of hurt to use a splash heal"
	/call set_data_map FALSE maChr stSplash															PREGEN|1											"${auxna} splash heal"

	| divine rez
	/call set_data_map FALSE maRez swDivineCleric												FALSE													"${auxsw} divin rez other clerics"
	
	| Rampage Tank 
	/call set_data_map FALSE maChr stRampageTank												FALSE													"${auxna} rampage tank to not heal (\a-wcompletely ignore\ax)"
	
	| arbitration
	/call set_data_map FALSE maChr swArbitratetoHealGroup								FALSE													"${auxsw} heal group immediately after an arbitration"
	/call set_data_map FALSE maChr swArbitrate													FALSE													"${auxsw} use aa arbitrate and epic sheild"
	/call set_data_map FALSE maChr stCountArbitrate											0															"${auxnum} num of group hurt to use arbitration aa/epic"

	| yaulp
	/varset dump ${set_data_timer[FALSE, Check_Yaulp, 6s]}
	/call set_data_map FALSE maChr swYaulp 															FALSE													"${auxsw} use yaulp (\a-win combat, on cooldown\ax)"
  /call set_data_map FALSE maChr stYaulp															PREGEN|1											"${auxna} name of yaulp spell/aa/item"

	| temp buff: ward family
  /call set_data_list FALSE maChr lsWardToon	 												EMPTY													"${auxli} list of toons to use ward temp hp buff on"
	/call set_data_map FALSE maChr stWard			 													PREGEN|1											"${auxna} use ward temp buff"

	| temp buff: retort family
	/call set_data_list FALSE maChr lsRetortToon 												EMPTY													"${auxli} list of toons to use retort temp line on"
	/call set_data_map FALSE maChr stRetort															PREGEN|1											"${auxna} use retort temp buff"

	| primary weapon  
  /call set_data_map FALSE maChr swSummonPrimary											FALSE													"${auxsw} use summoned primary hammer"
  /call set_data_map FALSE maChr stSummonPrimary											PREGEN|1											"${auxna} name of primary hammer spell"

	| set heal orders
	/declare lsHealOrderbase list outer
	/varset dump ${lsHealOrderbase.Delimiter["|"]}
	/varset dump ${lsHealOrderbase.Append[lsHealBaseLine]}

	/declare lsHealOrderaggressive list outer
	/varset dump ${lsHealOrderaggressive.Delimiter["|"]}
	/varset dump ${lsHealOrderaggressive.Append[lsHealIntervention|lsHealRemedy|lsHealRenewal|lsHealLight]}

	/declare lsHealOrderpassive list outer
	/varset dump ${lsHealOrderpassive.Delimiter["|"]}
	/varset dump ${lsHealOrderpassive.Append[lsHealRemedy|lsHealIntervention|lsHealRenewal|lsHealLight]}

/return



|***
 * note: main cleric routine
 * use: 
 ***|
sub main_CLR(bool _debug)
	DEBUG \atmain_CLR\ax()
	
	| correct any and all XTarget healing spots
	/if (AUTO) {
	  /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
	    /call check_xtarget_build FALSE
		}
	
		| sort the summoned cleric primary and any mana regen from VP if needed
		/if (!${Me.Invis}) {
			/if (${maChr.Find[swSummonPrimary].Value}) {
				/if (!${FindItemCount[${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}]} && ${InvSlot[mainhand].Item.Name.NotEqual[${FindItem[${Spell[${Me.Gem[13]}].Base[1]}]}]} || !${Me.Primary}) /call cast_nut_smasher FALSE
				CHECKCURSOR
			}
			/if (!${check_exit[FALSE]} && !SAFEZONE && !${Me.Buff[Revival Sickness].ID} && ${maChr.Find[stPctVP].Value}) {
			 	/if (${Me.PctMana} < ${maChr.Find[stPctVP].Value}*.PCTHO) {
			 		/if (${is_ready[FALSE, "Veturika's Perseverance"]}) {
			 			/call voodoo FALSE "Veturika's Perseverance" 0 FALSE
			 		}
			 	}
			}
		}
		
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
		/if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp FALSE
		CHECKFADE
		| /if (${swAlliance}) /call trigger_alliance

		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list FALSE stWard lsWardToon
			/call check_buff_cycle_temp_list FALSE stRetort lsRetortToon
		}
		/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
		/if (BURN) /call burn_${Me.Class.ShortName} FALSE
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			CHECKREZ
			/call set_combat FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} FALSE
			/if (!${timer_Check_deBuffs}) /call check_deBuffs FALSE TARGET
			/if (DOT) /call cast_dot FALSE lsDot TARGET
			/if (NUKE) /call cast_nuke FALSE lsNuke TARGET
			/call check_item_cycle FALSE TARGET
		}

		/if (AUTO) {	
			/if (${maChr.Find[stPctManaClick].Value}) /call do_mana_recovery_item FALSE
			CHECKFADE
		}

	}

/return



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat_CLR(bool _debug)
	DEBUG \atmisc_combat_CLR\ax()
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKREZ
	/if (${is_target_dead[FALSE, TARGET]}) /return
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE

	/if (${maMinion.Find[swSwarm].Value} && !${Me.Pet.ID}) {
		/call cast_list ${_debug} lsTempPet TARGET
	}
	
	
	| pet AA hammer	
	/if (${maBurn.Find[swBurnForce].Value} || (${maBurn.Find[swBurnAuto].Value} && ${lsZoneNamed.Contains[${Target.DisplayName}]})) {
		/if (${is_ready[FALSE, "Celestial Hammer"]}) {
			/call voodoo ${maDebug.Find[cast].Value} "Celestial Hammer" TARGET FALSE
		}
	}
	
	| undead aa nuke
	/if (${Target.Body.Name.Equal[Undead]}) {
		/if (${is_ready[FALSE, "Turn Undead"]}) {
			/call voodo ${maDebug.Find[cast].Value} "Turn Undead" TARGET FALSE
		}
	}	
	
	/if (AUTO && ${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
		/call get_hurt FALSE Group
		/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
	}
	
/return



|***
 * note: This will summon the clerics awesome nut smasher hammer if you do not have a good primary weapon.
 * use: you hit the bad guy.. thats how you fucking use it
 ***|
sub cast_nut_smasher(bool _debug)
	DEBUG \atcast_nut_smasher\ax()
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKREZ
	
	/if (!${FindItem[${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}].ID}) {
		/call cast_item_summon FALSE stSummonPrimary
	}
	
	/delay 10s !${Me.Casting.ID}
	
	/if (${InvSlot[mainhand].Item.ID} != ${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}) {
		/call Bind_command_swap "${FindItem[${Spell[${maChr.Find[stSummonPrimary].Value}].RankName.Base[1]}].Name}" 13 ${_debug}
		/return	
	}	
	
/return



|***
 * note: heal sub
 * use: /call check_heal_CLR [DEBUG] [group|pet|xtarget]
 ***|
sub check_heal_CLR(bool _debug, string _type)
	DEBUG \atcheck_heal_CLR\ax(\a-w${_type}\ax)
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE

	SETHUD "heal ${_type.Lower}"
	
	/call get_hurt ${_debug} ${_type}
	/if (${Select[${_type},group]} && ${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE

	/if (${Select[${_type},group]} && ${hurtCount} >= ${maChr.Find[stCountHealGroup].Value} && ${maChr.Find[stCountHealGroup].Value}) /call cast_list ${_debug} lsHealGroup ${Me.ID}


	/declare _count int local 0
  	
	| Group Regen Heal 
 	/if (${maChr.Find[swHealGroupOverTime].Value} && !SAFEZONE && ${Group.Members} && COMBAT) {
	 	/if (${SpawnCount[npc radius ENV PS4 targetable]} >= ${maChr.Find[stCountAE].Value} && ${maChr.Find[stCountAE].Value}) {
	 		/declare _groupHOT string local ${set_rank[FALSE, ${maChr.Find[stHealGroupOverTime].Value.Arg[1,+]}]}
			/if (!${Me.Song[Focused Celestial Regeneration].ID} && !${Me.Song[${_groupHOT}].ID} && !${Me.Song[Shear of Renewal].ID} && !${Me.Song[Ghost of Renewal].ID} && !${Me.Song[Cloud of Renewal].ID}) {

				/declare _found bool TRUE
				| check regen SPA
				/if (${Spell[${_groupHOT}].HasSPA[374]}) {
					/for _count 1 to ${Spell[${_groupHOT}].NumEffects}
						/if (!${Spell[${Spell[${_groupHOT}].Base2[${_count}]}].Duration}) /continue
						/if (!${Me.Song[${Spell[${_groupHOT}].Base2[${_count}]}].ID}) /varset _found FALSE
						/if (!${Spell[${Spell[${_groupHOT}].Base2[${_count}]}].Stacks}) /varset _found TRUE
						/if (${_found}) /break
					/next _count
				} else {
					/if (!${Me.Song[${_groupHOT}].ID}) /varset _found FALSE
					/if (${Spell[${_groupHOT}].Stacks}) /varset _found FALSE
				}
				/if (!${_found} && ${is_ready[FALSE, ${_groupHOT}]}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maChr.Find[stHealGroupOverTime].Value}" 0 FALSE
				}
			}
		}
	}
	
	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return	
	} 
	
	/call get_hurt ${_debug} ${_type}

	/if (${Select[${_type},group]}) {
		/if (${maChr.Find[stCountArbitrate].Value}) {
			/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
		}
		/if (${maChr.Find[stCountHealGroup].Value}) {
			/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) /call cast_list ${_debug} lsHealGroup ${Me.ID}
		}
		/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
	}

	/if (!SAFEZONE) {
		| panic heal
		/if (${hurtPctHP} < ${maHeal.Find[stHealPointPanic].Value}*.PCTHO) {
			/if (${Me.Casting.ID}) /stopcast
			/if (${is_ready[FALSE, "${maChr.Find[stHealPanic].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stHealPanic].Value}" ${hurtID} FALSE
			}
			/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		}

		| Arbitration
		/if (${Select[${_type},group]}) {
			/if (${maChr.Find[stCountArbitrate].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
			}
			/if (${maChr.Find[stCountHealGroup].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) /call cast_list ${_debug} lsHealGroup ${Me.ID}
			}
			/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		}

		| Focused Celestial Regeneration - out of raid only
		/if (!${Raid.Members}) {
			/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO && !${Me.Song[Celestial Regeneration].ID}) { 
				/if (${is_ready[${_debug}, "Focused Celestial Regeneration"]}) {
					/call voodoo ${_debug} "Focused Celestial Regeneration" ${hurtID} FALSE
				}
			}
		}
		
		| GoM
		/if (${Me.Song[Gift of Mana].ID}) {
			/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) { 
				/call cast_list ${_debug} lsGoM ${hurtID}
			 	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		  }
		}
	  
		CHECKFADE
		CHECKREZ

		| Panic AA single target heal
	  /if (${hurtPctHP} <= ${maHeal.Find[stHealPointPanic].Value}*.PCTHO) {
			/if (${is_ready[FALSE, "Burst of Life"]}) {
				/if (${Me.Casting.ID}) /stopcast
				/call voodoo ${_debug} "Burst of Life" ${hurtID} FALSE
			 	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
			}
	  }
		
		/if (${Select[${_type},group]}) {
			/if (${maChr.Find[stCountArbitrate].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
			}
			/if (${maChr.Find[stCountHealGroup].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) /call cast_list ${_debug} lsHealGroup ${Me.ID}
			}
			/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		}	
	
		| panic group heal. AA Beacon of life (3 min use)
	  /if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value} && ${Group.Members} > 1 && ${maChr.Find[stCountHealGroup].Value}) {
		  /if (${is_ready[${_debug}, "Beacon of Life"]}) {
		  	/call voodoo ${_debug} "Beacon of Life" 0 FALSE
			 	/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		  }
		}		 

		/if (${Select[${_type},group]}) {
			/if (${maChr.Find[stCountArbitrate].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
			}
			/if (${maChr.Find[stCountHealGroup].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) /call cast_list ${_debug} lsHealGroup ${Me.ID}
			}
			/if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE
		}

		/if (${maChr.Find[stCountIssuance].Value}) {
			/if (${SpawnCount[npc radius ENV PS4]} >= ${maChr.Find[stCountIssuance].Value}) /call Bind_command_issuance FALSE
		}
		
		| splash heal
		/if (${maChr.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
			/if (${SpawnCount[npc radius ENV PS4]} >= ${maChr.Find[stCountSplash].Value}) /call Bind_command_splash
		}
		
	}

 	| single heal over time
	/if (${maChr.Find[swHealOverTime].Value}) {
		/if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
			/declare _stacks bool local FALSE
			/declare _has bool local FALSE
			/declare _spell string local ${maChr.Find[swHealOverTime].Value.Arg[1,+]}
			/dquery ${Spawn[${hurtID}].DisplayName} -q "Spell[${_spell}].Stacks" -o _stacks
			/dquery ${Spawn[${hurtID}].DisplayName} -q "Me.Song[${_spell}].Duration" -o _has
			/if (${_stacks} && !${_has}) {
				/if (${is_ready[${_debug}, "${_spell}"]}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maChr.Find[stHealOverTime}.Value}" ${hurtID} FALSE
				}
			}
		}
	}


 	/if (!${get_hurt[FALSE, ${_type}]}) {
 		/return FALSE
 	}

	ISMEDEAD
	CHECKEXIT
	CHECKFADE	
	CHECKREZ
		
	| yes. this is all there is to healing..
	/declare _liHM listiterator local
	/vardata _liHM lsHealOrder${maHeal.Find[stHealMode].Value}.First.Clone
	/while (!${_liHM.IsEnd}) {
		DEBUG ${sep}_liHM.Value${sep}\a-w${_liHM.Value}\ax
		/call cast_list FALSE "${_liHM.Value}" ${hurtID}
		/if (!AUTO) /return
		/call get_hurt FALSE ${_type} 
		/if (${Select[${_type},group]} && ${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) {
			/call do_arbitration FALSE
		} else /if (!${hurtCount} || !${maChr.Find[stCountArbitrate].Value}) {
			/return
		}
			
		/varset dump ${_liHM.Advance}
	}

/return	



|***
 * note: Use cleric divine arbitration AA or Epic shield click. then group heal if it is to be so
 * use: auto based on /chr arbitrate #
 ***|
sub do_arbitration(bool _debug)
	DEBUG \atdo_arbitration\ax()
	
	ISMEDEAD
	CHECKEXIT
	
	/declare _arbDone bool local FALSE
	
	| Second epic shield
  /if (${is_ready[${_debug}, "${maData.Find[stEpic2].Value}"]}) {    
    /call voodoo ${_debug} "${maData.Find[stEpic2].Value}" 0 FALSE
    /varset _arbDone TRUE
    
  | First epic shield  
  } else /if (${is_ready[${_debug}, "${maData.Find[stEpic15].Value}"]}) {
    /call voodoo ${_debug} "${maData.Find[stEpic15].Value}" 0 FALSE
    /varset _arbDone TRUE
    
  | AA arb
  } else /if (${is_ready[${_debug}, "Divine Arbitration"]}) {
    /call voodoo ${_debug} "Divine Arbitration" 0 FALSE
    /varset _arbDone TRUE
  }
  
  | should we group heal?
	/if ((${maChr.Find[swArbitratetoHealGroup].Value} && ${_arbDone}) || (${maChr.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maChr.Find[stCountHealGroup].Value})) {
		/varset dump ${out[3, "Arbitration done"]}
	  /call cast_list ${_debug} lsHealGroup ${Me.ID}
	}
	
	CHECKREZ
	
/return



|***
 * note: Cast the cleric floatie shield AE regen. Twin cast it if the AA is ready.
 * use: /issuance 
 ***|
#bind command_issuance /issuance
sub Bind_command_issuance(bool _debug)
	DEBUG \atBind_command_issuance\ax()

	/if (!${check_class[FALSE, |CLR]}) /return FALSE
	ISMEDEAD
	CHECKEXIT

	/if (!${is_ready[FALSE, "${maChr.Find[stIssuance].Value}"]}) /return FALSE
	
	:castIssuance
	/if (!${Target.ID} || ${Target.ID} == ${Me.ID} || ${Target.Distance} > 55) {
		/if (${NearestSpawn[2, pc radius 55].ID}) {
			/varset dump ${set_target[${NearestSpawn[2, pc radius 55].ID}]}
		} else /if (${NearestSpawn[1, mercenary radius 55].ID}) {
			/varset dump ${set_target[${NearestSpawn[1, mercenary radius 55].ID}]}
		} else /if (${NearestSpawn[1, npc radius 55].ID}) {
			/varset dump ${set_target[${NearestSpawn[1, npc radius 55].ID}]}
		}
	}

	/if (!${Spawn[${Target.ID}].CanSplashLand}) /return 
	/if (${Spawn[${Target.ID}].Distance} < 59) {
		/call voodoo ${_debug} "${maChr.Find[stIssuance].Value}" ${Target.ID} FALSE
		}
	} 
	
	/if (AUTO && ${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
		/call get_hurt FALSE Group
		/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
	}
	
	| should we recast?
	/if (BURN) {
		/if (${is_Ready[${_debug}, "Forceful Rejuvenation"]}) {
			/call voodoo ${_debug} "Forceful Rejuvenation" 0 FALSE
			/delay 4s !${Me.Casting.ID}
			/goto :castIssuance
		} 
	}
	
	CHECKREZ

/return	


 
|***
 * note: burn routines
 * use: meh..
 ***| 
sub burn_CLR(bool _debug)
	DEBUG \atburn_CLR\ax()
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKREZ
	
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]} != TRUE) /call cast_defensive_cycle FALSE	

	| Group AA Celestial Regeneration
 	/if (${maChr.Find[swHealGroupOverTime].Value} && ${Group}) {
 	 	/if (${is_ready[FALSE, "Celestial Regeneration"]} && !${Bool[${Me.Song[Celestial Regeneration]}]} && !${Me.Song[Focused Celestial Regeneration].ID}) {
 			/call voodoo ${maDebug.Find[cast].Value} "Celestial Regeneration" 0 FALSE
	  } 
	}

	| reflect spells (proc:Shield of Reverence, 18 sec duration 3 min use)
	/if (${is_ready[FALSE, alt, "Shield of Reverence", ${Me.ID}]}) {
		/call voodoo ${maDebug.Find[cast].Value} "Shield of Reverence" 0 FALSE
	}

		| stage 1
	/if (${Me.AltAbilityReady[Channeling the Divine]} && ${Me.AltAbilityReady[Celestial Rapidity]} && !${Me.Buff[Healing Frenzy].ID}) {
		
	  | Spire 1:30 min duration 7:30 min reuse
   	/call cast_AA_spire FALSE
	  
	 	| anti agro (pric Silent Casting 1min duration 9 min reuse)
		/if (!${Me.Buff[Divine Retribution].ID}) {
			/if (${is_ready[FALSE, "Silent Casting"]}) {
				/call voodoo ${maDebug.Find[cast].Value} "Silent Casting" 0 FALSE
			}
		}	
		
	 	| 50% chance to twincast (proc:Channeling of the Divine, 45 sec duration 10 min resuse)
	 	/if (${is_ready[FALSE, "Channeling the Divine"]}) {
	 		/call voodoo ${maDebug.Find[cast].Value} "Channeling the Divine"  0 FALSE
	 	}
	 	
		| - ~50% to cast times [proc:Celestial Rapidity, 1 min duration 10 min reuse]
		/if (${is_ready[FALSE, "Celestial Rapidity"]}) {
			/call voodoo ${maDebug.Find[cast].Value} "Celestial Rapidity" 0 FALSE
		}
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
			/call get_hurt FALSE Group
			/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
		}
		
	| stage 2
	} else /if (${Me.AltAbilityReady[Healing Frenzy]} && ${Me.AltAbilityReady[Flurry of Life]} && ${Me.Buff[${maData.Find[stSpire${maChr.Find[stSpireGroup].Value}].Value}].ID} && !${Me.Buff[${maData.Find[stSpire${maChr.Find[stSpireRaid].Value}].Value}].ID}) {

		| melee block anti agro (20 min reuse)
		/if (!${Me.Song[Silent Casting].ID}) {
			/if (${is_ready[FALSE, "Divine Retribution"]}) {
				/call voodoo ${maDebug.Find[cast].Value} "Divine Retribution" 0 FALSE
			}
		}

		| 100% exceptional heal (proc: Flurry of Life, 1:30 min duration 15 min reuse)
		/if (${is_ready[FALSE, "Healing Frenzy"]}) {
			/call voodoo ${maDebug.Find[cast].Value} "Healing Frenzy" 0 FALSE
		}

		| heal power bost: 35% (proc Healing Frenzy, 40sec duration 15 min reuse)
		/if (${is_ready[FALSE, "Flurry of Life"]}) {
			/call voodoo ${maDebug.Find[cast].Value} "Flurry of Life" 0 FALSE
		}
		
		/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
			/call get_hurt FALSE Group
			/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) /call do_arbitration FALSE
		}
	}

	| heal ward (20 min reuse)
	/if (${is_ready[FALSE, "Exquisite Benediction"]}) {
		/call voodoo ${maDebug.Find[cast].Value} "Exquisite Benediction" 0 FALSE
	}

	| direct damage boost if using aggressive healing
	/if (${maHeal.Find[stHealMode].Value.Equal[aggressive]} && !${Me.Song[Silent Casting].ID}) {
    /if (${is_ready[FALSE, "Battle Frenzy"]}) {
    	/call voodoo ${maDebug.Find[cast].Value} "Battle Frenzy" 0 FALSE
    }
  }
  
  /call cast_glyph FALSE
  CHECKREZ

/return



|***
 * note: cleric specific control
 * use: /chr
 ***|
sub set_control_CLR(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_CLR\ax(\a-w${_type}, "${_verbage}", ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maChr ${_verbage}
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

	| Veturika's Perseverance	
	} else /if (${_type.Equal[vp]}) {
		/if (!${set_control_num_range[FALSE, stPctVP, "${_verbage}", 0, 99]}) /return

	| use Issuance Shield 
	} else /if (${_type.Equal[issuance]}) {
		/if (!${set_control_num_range[FALSE, stCountIssuance, "${_verbage}", 0, 99]}) /return

	| use splashes
	} else /if (${_type.Equal[splash]}) {
		/if (!${set_control_num_range[FALSE, stCountSplash, ${_verbage}, 0, 99]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[FALSE, stCountHealGroup, "${_verbage}", 0, 6]}) /return

	| use group Heal over Time	
	} else /if (${_type.Equal[grouphot]}) {
		/varset dump ${set_switch_env[${_debug}, swHealGroupOverTime, TRUE, ${_verbage}]}

	| arbitration (sheild AND AA)	
	} else /if (${_type.Equal[arbitrate]}) {
		/if (!${set_control_num_range[FALSE, stCountArbitrate, ${_verbage}, 0, 6]}) /return

	| use divine rez
	} else /if (${_type.Equal[divinerez]}) {
		/varset dump ${set_switch_env[${_debug}, swDivineCleric, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[FALSE, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[FALSE, stCountAE, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[hot]}) {
		/varset dump ${set_switch_env[${_debug}, swHealOverTime, TRUE, ${_verbage}]}

	| cleric summoned weapon
	} else /if (${_type.Equal[primary]}) {

		| use the weapon.. seriously.. go camp something. put some effort into the class
		/if (${_verbage.Equal[use]}) {
			/echo here switch
			/varset dump ${set_switch_env[FALSE, swSummonPrimary, TRUE]}
			
		| spell to summon
		} else /if (${_verbage.Equal[spell]}) {
			/varset dump ${do_raw_edit[${_debug}, SILENT, stSummonPrimary, "${_silent}"]}
	
		}

		/declare _lsout list local
		/varset dump ${_lsout.Append[use,spell]}
		/vardata li _lsout.First.Clone
		/while (!${li.IsEnd}) {
			/varset _out ${_out} ${If[${li.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${li.Value}\ax
			/if (${li.Value.Equal[use]}) /varset _out ${_out}:${If[${maChr.Find[swSummonPrimary].Value},${on},${off}]}
			/if (${li.Value.Equal[spell]}) /varset _out ${_out}:[${If[${Bool[${maChr.Find[stSummonPrimary].Value}]},${num}${maChr.Find[stSummonPrimary].Value}\ax,${r0}]}]
			/varset dump ${li.Advance}
		}	
		OUT /chr primary ${_out}
		/return

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
		/if (${li.Value.Equal[vp]}) /varset _out ${_out}:${If[${maChr.Find[stPctVP].Value},${num}${maChr.Find[stPctVP].Value}\ax,${r0}]}
		/if (${li.Value.Equal[arbitrate]}) /varset _out ${_out}:${If[${maChr.Find[stCountArbitrate].Value},${num}${maChr.Find[stCountArbitrate].Value}\ax,${r0}]}
		/if (${li.Value.Equal[yaulp]}) /varset _out ${_out}:${If[${maChr.Find[swYaulp].Value},${on},${off}]}
		/if (${li.Value.Equal[divinerez]}) /varset _out ${_out}:${If[${maRez.Find[swDivineCleric].Value},${on},${off}]}
		/if (${li.Value.Equal[issuance]}) /varset _out ${_out}:${If[${maChr.Find[stCountIssuance].Value},${num}${maChr.Find[stCountIssuance].Value}\ax,${r0}]}
		/if (${li.Value.Equal[groupheal]}) /varset _out ${_out}:${If[${maChr.Find[stCountHealGroup].Value},${num}${maChr.Find[stCountHealGroup].Value}\ax,${r0}]}
		/if (${li.Value.Equal[grouphot]}) /varset _out ${_out}:${If[${maChr.Find[swHealGroupOverTime].Value},${on},${off}]}
		/if (${li.Value.Equal[hot]}) /varset _out ${_out}:${If[${maChr.Find[swHealOverTime].Value},${on},${off}]}
		/if (${li.Value.Equal[manaclick]}) /varset _out ${_out}:${If[${maChr.Find[stPctManaClick].Value},${num}${maChr.Find[stPctManaClick].Value}\ax,${r0}]}
		/if (${li.Value.Equal[splash]}) /varset _out ${_out}:${If[${maChr.Find[stCountSplash].Value},${num}${maChr.Find[stCountSplash].Value}\ax,${r0}]}
		/if (${li.Value.Equal[primary]}) /varset _out ${_out}:${submenu}
		/varset dump ${li.Advance}
	}	
	OUT /chr ${_out}

/return



