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

	/call set_data ${_debug} maData stSpire															map outer "Spire of the Vicar"
	/call set_data ${_debug} maData stSynergy														map outer "Templar's Synergy"
	/call set_data ${_debug} maData stEpic1															map outer "Water Sprinkler of Nem Ankh"
	/call set_data ${_debug} maData stEpic15														map outer "Harmony of the Soul"
	/call set_data ${_debug} maData stEpic2															map outer "Aegis of Superior Divinity"
	/call set_data ${_debug} maData stFade															map outer "Divine Peace"
	/call set_data ${_debug} maData stCompleteHeal											map outer "Complete Heal"

	| Veturika's Preserverance
	/call set_data_map ${_debug} maChr stPctVP													0															"${auxnum} % of manna to use veturikas perseverance"

	| healing
 	/call set_data_list ${_debug} maChr lsHealRemedy										PREGEN|3											"${auxli} remedy spells to use"
 	/call set_data_list ${_debug} maChr lsHealRenewal										PREGEN|3											"${auxli} renewal spells to use"
 	/call set_data_list ${_debug} maChr lsHealIntervention							PREGEN|3											"${auxli} intervention spells to use"
 	/call set_data_list ${_debug} maChr lsHealLight											PREGEN|3											"${auxli} light spells to use"

  /call set_data_map ${_debug} maChr swHealGroupOverTime							FALSE													"${auxsw} use group heal over time spells/aas"
  /call set_data_map ${_debug} maChr swCelestialRegen									FALSE													"${auxsw} use celestial regen aa"
  /call set_data_map ${_debug} maChr stHealGroupOverTime							PREGEN|1											"${auxna} group heal over time spell"


	/call set_data_map ${_debug} maChr swHealOverTime										FALSE													"${auxsw} % of members health to use heal over time"
	/call set_data_map ${_debug} maChr stHealOverTime										PREGEN|1											"${auxna} heal over time spelll"

	/call set_data_map ${_debug} maChr stCountHealGroup									0															"${auxnum} number in group hurt to use a group heal"
 	/call set_data_list ${_debug} maChr lsHealGroup											PREGEN|2											"${auxli} group heal spells"

	/call set_data_map ${_debug} maChr stAlliance												PREGEN|1											"${auxna} alliance spell"

	| issuance
	/call set_data_map ${_debug} maChr stCountIssuance									0															"${auxnum} number of mobs in area to use issuance"
	/call set_data_map ${_debug} maChr stIssuance												PREGEN|1											"${auxna} issuance spell"

	| swarm pet
	/call set_data_map ${_debug} maChr stTempPet												PREGEN|1											"${auxna} temp hammer pet spell"

	| splash
	/call set_data_map ${_debug} maChr stCountSplash										0															"${auxnum} number of hurt to use a splash heal"
	/call set_data_map ${_debug} maChr stSplash													PREGEN|1											"${auxna} splash heal"

	| divine rez
	/call set_data_map ${_debug} maRez swDivineCleric										FALSE													"${auxsw} divine rez other clerics"

	| Rampage Tank
	/call set_data_map ${_debug} maChr stRampageTank										FALSE													"${auxna} rampage tank to not heal (\a-wcompletely ignore\ax)"

	| arbitration

	| /call set_data_map ${_debug} maChr swArbitratetoHealGroup						FALSE													"${auxsw} heal group immediately after an arbitration"
	| /call set_data_map ${_debug} maChr swArbitrate											FALSE													"${auxsw} use aa arbitrate and epic sheild"
	/call set_data_map ${_debug} maChr stCountArbitrate									0															"${auxnum} num of group hurt to use arbitration aa/epic"

	| yaulp
	/invoke ${set_data_timer[${_debug}, Check_Yaulp, 6s]}
	/call set_data_map ${_debug} maChr swYaulp 													FALSE													"${auxsw} use yaulp (\a-win combat, on cooldown\ax)"
  /call set_data_map ${_debug} maChr stYaulp													PREGEN|1											"${auxna} name of yaulp spell/aa/item"

	| temp buff: ward family
  /call set_data_list ${_debug} maChr lsWardToon	 										EMPTY													"${auxli} list of toons to use ward temp hp buff on"
	/call set_data_map ${_debug} maChr stWard			 											PREGEN|1											"${auxna} use ward temp buff"

	| temp buff: retort family
	/call set_data_list ${_debug} maChr lsRetortToon 										EMPTY													"${auxli} list of toons to use retort temp line on"
	/call set_data_map ${_debug} maChr stRetort													PREGEN|1											"${auxna} use retort temp buff"

	| primary weapon
  /call set_data_map ${_debug} maChr swSummonPrimary									FALSE													"${auxsw} use summoned primary hammer"
  /call set_data_map ${_debug} maChr stSummonPrimary									PREGEN|1											"${auxna} name of primary hammer spell"

	| AAs
	/call set_data_map ${_debug} maChr stAAUnity												"Saint's Unity"										"${auxna} class aa unity buff"
	/call set_data_map ${_debug} maChr stAADivineArbitration						"Divine Arbitration"
	/call set_data_map ${_debug} maChr stAACelestialRegeneration				"Celestial Regeneration"
	/call set_data_map ${_debug} maChr stAAFocusedCelestialRegeneration	"Focused Celestial Regeneration"
	/call set_data_map ${_debug} maChr stAABurstofLife									"Burst of Life"
	/call set_data_map ${_debug} maChr stAABeaconofLife									"Beacon of Life"
	/call set_data_map ${_debug} maChr stAAForcefulRejuvenation					"Forceful Rejuvenation"
	/call set_data_map ${_debug} maChr stAAChannelingtheDivine					"Channeling the Divine"
	/call set_data_map ${_debug} maChr stAADivineRetribution						"Divine Retribution"
	/call set_data_map ${_debug} maChr stAASilentCasting								"Silent Casting"
	/call set_data_map ${_debug} maChr stAAHealingFrenzy								"Healing Frenzy"
	/call set_data_map ${_debug} maChr stAAFlurryofLife									"Flurry of Life"
	/call set_data_map ${_debug} maChr stAAExquisiteBenediction					"Exquisite Benediction"
	/call set_data_map ${_debug} maChr stAABattleFrenzy									"Battle Frenzy"
	/call set_data_map ${_debug} maChr stAACelestialRapidity						"Celestial Rapidity"
	/call set_data_map ${_debug} maChr stAATurnUndead										"Turn Undead"
	/call set_data_map ${_debug} maChr stAACelestialHammer							"Celestial Hammer"
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA
	/call set_data_map ${_debug} maChr stAA



	| set heal orders
	/declare lsHealOrderbase list outer
	/invoke ${lsHealOrderbase.Delimiter["|"]}
	/invoke ${lsHealOrderbase.Append[lsHealBaseLine]}

	/declare lsHealOrderaggressive list outer
	/invoke ${lsHealOrderaggressive.Delimiter["|"]}
	/invoke ${lsHealOrderaggressive.Append[lsHealIntervention|lsHealRemedy|lsHealRenewal|lsHealLight]}

	/declare lsHealOrderpassive list outer
	/invoke ${lsHealOrderpassive.Delimiter["|"]}
	/invoke ${lsHealOrderpassive.Append[lsHealRemedy|lsHealIntervention|lsHealRenewal|lsHealLight]}

	/declare lsHealOrderheavy list outer
	/invoke ${lsHealOrderheavy.Delimiter["|"]}
	/invoke ${lsHealOrderheavy.Append[lsHealRenewal|lsHealIntervention|lsHealLight|lsHealRemedy]}

/return TRUE




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
			/if (${maChr.Find[swSummonPrimary].Value} && !${Me.Inventory[mainhand].ID}) {
				/if (!${FindItemCount[${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}]} && ${InvSlot[mainhand].Item.Name.NotEqual[${FindItem[${Spell[${Me.Gem[13]}].Base[1]}]}]}) /call cast_nut_smasher FALSE
				CHECKCURSOR
			}
			/if (!${check_exit[FALSE]} && !SAFEZONE && !${Me.Buff[Revival Sickness].ID} && ${maChr.Find[stPctVP].Value}) {
			 	/if (${Me.PctMana} < ${maChr.Find[stPctVP].Value}*.PCTHO) {
			 		/if (${is_ready[${_debug}, "Veturika's Perseverance"]}) {
			 			/call voodoo ${_debug} "Veturika's Perseverance" 0 FALSE
			 		}
			 	}
			}
		}

	}

	/while (${check_combat_status[FALSE]}) {
		GETINPUT clr_main_loop NA|NA
		/call check_heal_routines

		CHECKFADE
		| /if (${swAlliance}) /call trigger_alliance

		/if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp FALSE
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stWard lsWardToon
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stRetort lsRetortToon
		}
		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
		/if (${check_class_loop[FALSE]}) /return

		/if (VALID) {
			CHECKREZ
			/call set_combat FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (DOT) /call cast_dot ${maDebug.Find[dot].Value} lsDoT TARGET
			/if (NUKE) /call cast_nuke ${maDebug.Find[nuke].Value} lsNuke TARGET
			/call check_item_cycle ${maDebug.Find[item].Value} TARGET
		}

		/if (AUTO) {
			/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
			CHECKFADE
		}

	}

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat_CLR(bool _debug, int _tmpID)
	DEBUG \atmisc_combat_CLR\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	/if (!${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}
	CHECKEXIT
	GETINPUT misc_combat_CLR NA|NA
	CHECKTIE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) {
		/call cast_defensive_cycle ${maDebug.Find[def].Value}
	}

	/if (SWARM || PET) /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
	/if (AUTO && MELEE) /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	/if (!${is_defensive_active[${maDebug.Find[def].Value}, FALSE, SAFEENV]}) {
		/call cast_defensive_cycle ${maDebug.Find[def].Value}
	}

	/if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp FALSE

	/if (SWARM) /call cast_temp_pet ${maDebug.Find[temp].Value} ${_tmpID} "${maChr.Find[stTempPet].Value}"


	| pet AA hammer
	/if (${maBurn.Find[swBurnForce].Value} || (${maBurn.Find[swBurnAuto].Value} && ${lsZoneNamed.Contains[${Target.DisplayName}]})) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAACelestialHammer].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAACelestialHammer].Value}" ${_tmpID} FALSE
		}
	}

	| undead aa dot
	/if (${Target.Body.Name.Equal[Undead]} && DOT) {
		/if (${is_ready[${_debug}, "${maChr.Find[stAATurnUndead].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAATurnUndead].Value}" ${_tmpID} FALSE
		}
	}

/return TRUE



|***
 * note: This will summon the clerics awesome nut smasher hammer if you do not have a good primary weapon.
 * use: you hit the bad guy.. thats how you fucking use it
 ***|
sub cast_nut_smasher(bool _debug)
	DEBUG \atcast_nut_smasher\ax()

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	GETINPUT clr_summon_primary stSummonPrimary|${maChr.Find[stSummonPrimary].Value}
	CHECKREZ

	/if (!${FindItem[${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}].ID}) {
		/call cast_item_summon FALSE stSummonPrimary
	}

	/delay 10s !${Me.Casting.ID}

	/if (${InvSlot[mainhand].Item.ID} != ${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}) {
		/call Bind_command_swap "${FindItem[${Spell[${maChr.Find[stSummonPrimary].Value}].RankName.Base[1]}].Name}" 13 ${_debug}
	}

/return TRUE



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
	GETINPUT clr_heal_start type|${_type}

	SETHUD "heal ${_type.Lower}"

	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

	/declare _count int local 0

	| Group Regen Heal
 	/if (${maChr.Find[swHealGroupOverTime].Value} && !SAFEZONE && ${Group.Members} && COMBAT && ${maChr.Find[stCountAE].Value}) {
	 	/if (${SpawnCount[npc radius ENV PS4 targetable]} >= ${maChr.Find[stCountAE].Value}) {
	 		/declare _groupHOT string local ${Set_Rank[False, ${maChr.Find[stHealGroupOverTime].Value.Arg[1,+]}]}
			/if (!${Me.Song[${_groupHOT}].ID} && !${Me.Song[Shear of Renewal].ID} && !${Me.Song[Ghost of Renewal].ID} && !${Me.Song[Cloud of Renewal].ID}) {
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
				/if (!${_found} && ${is_ready[${_debug}, ${_groupHOT}]}) {
					/call voodoo ${_debug} "${maChr.Find[stHealGroupOverTime].Value}" 0 FALSE
				}
			}
		}
	}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD FALSE
		/return
	}

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	/if (!SAFEZONE) {

		| arbitrate / group heal
		/if (${Select[${_type},group]}) {
			/if (${maChr.Find[stCountArbitrate].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) {
					| /call do_arbitration ${maDebug.Find[arbitrate].Value}

				  | AA arb
				  /if (${is_ready[${_debug}, "${maChr.Find[stAADivineArbitration].Value}"]}) {
				    /call voodoo ${_debug} "${maChr.Find[stAADivineArbitration].Value}" 0 FALSE
					| Second epic shield
				  } else /if (${is_ready[${_debug}, "${maData.Find[stEpic2].Value}"]}) {
				    /call voodoo ${_debug} "${maData.Find[stEpic2].Value}" 0 FALSE
				  | First epic shield
				  } else /if (${is_ready[${_debug}, "${maData.Find[stEpic15].Value}"]}) {
				    /call voodoo ${_debug} "${maData.Find[stEpic15].Value}" 0 FALSE
				  }
					/call get_hurt ${maDebug.Find[hurt].Value} ${_type}
				}
			}
			/if (${maChr.Find[stCountHealGroup].Value}) {
				/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) {
					/call cast_list ${_debug} lsHealGroup ${Me.ID}
				}
			}
		}

		| Focused Celestial Regeneration - out of raid only
		/if (!${Raid.Members}) {
			/if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO && !${Me.Song[${maChr.Find[stAACelestialRegeneration].Value}].ID}) {
				/if (${is_ready[${_debug}, "${maChr.Find[stAAFocusedCelestialRegeneration].Value}"]}) {
					/call voodoo ${_debug} "${maChr.Find[stAAFocusedCelestialRegeneration].Value}" ${hurtID} FALSE
				}
			}
		}

		| GoM
		/if (${Me.Song[Gift of Mana].ID}) {
			/if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
				/call cast_list ${_debug} lsGoM ${hurtID}
		  }
		}

		/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

		CHECKTIE
		ISMEDEAD
		CHECKEXIT
		CHECKREZ
		CHECKFADE
		GETINPUT clr_heal_mid type|${_type}

		| splash heal
		/if (${maChr.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
			/if (${SpawnCount[npc radius ENV]} >= ${maChr.Find[stCountSplash].Value}) {
				/call Bind_command_splash ${maDebug.Find[splash].Value}
			}
		}


		| Panic AA single target heal
	  /if (${hurtPctHP} <= ${maHeal.Find[stHealPointPanic].Value}*.PCTHO) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAABurstofLife].Value}"]}) {
				/if (${Me.Casting.ID}) {
					/stopcast
				}
				/call voodoo ${_debug} "${maChr.Find[stAABurstofLife].Value}" ${hurtID} FALSE
			}
	  }

		| panic group heal. AA Beacon of life (3 min use)
	  /if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value} && ${Group.Members} > 1 && ${maChr.Find[stCountHealGroup].Value}) {
		  /if (${is_ready[${_debug}, "${maChr.Find[stAABeaconofLife].Value}"]}) {
		  	/call voodoo ${_debug} "${maChr.Find[stAABeaconofLife].Value}" 0 FALSE
		  }
		}

		/call get_hurt ${maDebug.Find[hurt].Value} ${_type}


		/if (${maChr.Find[stCountIssuance].Value}) {
			/if (${SpawnCount[npc radius ENV PS4]} >= ${maChr.Find[stCountIssuance].Value}) {
				/call Bind_command_issuance FALSE
			}
		}

	}

 	| single heal over time
	/if (${maChr.Find[swHealOverTime].Value}) {
		/if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
			/declare _spell string local ${maChr.Find[swHealOverTime].Value.Arg[1,+]}
			/dquery ${Spawn[${hurtID}].DisplayName} -q "Spell[${_spell}].Stacks" -o DNQReturn
			/dquery ${Spawn[${hurtID}].DisplayName} -q "Me.Song[${_spell}].Duration" -o DNQReturn2
			/if (${DNQReturn} && !${DNQReturn2}) {
				/if (${is_ready[${_debug}, "${_spell}"]}) {
					/call voodoo ${_debug} "${maChr.Find[stHealOverTime}.Value}" ${hurtID} FALSE
				}
			}
		}
	}


 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE
	GETINPUT clr_heal_end type|${_type}

	| yes. this is all there is to healing..
	/declare _liHM listiterator local
	/vardata _liHM lsHealOrder${maHeal.Find[stHealMode].Value}.First.Clone
	/while (!${_liHM.IsEnd}) {
		DEBUG ${sep}_liHM.Value${sep}\a-w${_liHM.Value}\ax
		/call cast_list ${_debug} "${_liHM.Value}" ${hurtID}
		/if (!AUTO) /return FALSE
		/call get_hurt ${maDebug.Find[hurt].Value} ${_type}
		/if (!${hurtCount}) {
			/return FALSE
		}
		/invoke ${_liHM.Advance}
	}

/return	TRUE



|***
 * note: Use cleric divine arbitration AA or Epic shield click. then group heal if it is to be so
 * use: auto based on /chr arbitrate #
 ***|
sub do_arbitration(bool _debug)
	DEBUG \atdo_arbitration\ax()

	ISMEDEAD
	CHECKEXIT
	GETINPUT clr_arbitrate NA|NA

	| /declare _arbDone bool local FALSE

  | AA arb
  /if (${is_ready[${_debug}, "${maChr.Find[stAADivineArbitration].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAADivineArbitration].Value}" 0 FALSE
    | /varset _arbDone TRUE

	| Second epic shield
  } else /if (${is_ready[${_debug}, "${maData.Find[stEpic2].Value}"]}) {
    /call voodoo ${_debug} "${maData.Find[stEpic2].Value}" 0 FALSE
    | /varset _arbDone TRUE

  | First epic shield
  } else /if (${is_ready[${_debug}, "${maData.Find[stEpic15].Value}"]}) {
    /call voodoo ${_debug} "${maData.Find[stEpic15].Value}" 0 FALSE
    | /varset _arbDone TRUE

  }

	| /if (${_arbDone}) {
		| /invoke ${out[3, "arbitration done"]}
	| 	/delay 1
	| }

/return TRUE



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

	/if (!${is_ready[${_debug}, "${maChr.Find[stIssuance].Value}"]}) /return FALSE

	:castIssuance
	/if (!${Target.ID} || ${Target.ID} == ${Me.ID} || ${Target.Distance} > 55) {
		/if (${NearestSpawn[2, pc radius 55].ID}) {
			/invoke ${set_target[${NearestSpawn[2, pc radius 55].ID}]}
		} else /if (${NearestSpawn[1, mercenary radius 55].ID}) {
			/invoke ${set_target[${NearestSpawn[1, mercenary radius 55].ID}]}
		} else /if (${NearestSpawn[1, npc radius 55].ID}) {
			/invoke ${set_target[${NearestSpawn[1, npc radius 55].ID}]}
		}
	}

	/if (!${Spawn[${Target.ID}].CanSplashLand}) {
		/return FALSE
	}

	/if (${Spawn[${Target.ID}].Distance} < 59) {
		/call voodoo ${_debug} "${maChr.Find[stIssuance].Value}" ${Target.ID} FALSE
	}

	| should we recast?
	/if (BURN) {
		/if (${is_Ready[${_debug}, "${maChr.Find[stAAForcefulRejuvenation].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAForcefulRejuvenation].Value}" 0 FALSE
			/delay 4s !${Me.Casting.ID}
			/goto :castIssuance
		}
	}

	CHECKREZ

/return	TRUE



|***
 * note: burn routines
 * "What day is today?" asked Pooh
 * "It's the day we burn this mother fucker to the ground." squeaked Piglet
 * "My favorite day." said Pooh
 ***|
sub burn_CLR(bool _debug, int _tmpID)
	DEBUG \atburn_CLR\ax(\a-w${_tmpID}\ax)

	ISMEDEAD
	CHECKEXIT
	GETINPUT clr_burn NA|NA
	CHECKTIE

	/if (${Bool[${_tmpID}]}) {
		/if (${target[isdead, ${_tmpID}]}) {
			/return FALSE
		}

		/if (!${is_defensive_active[${maDebug.Find[def].Value}, BURN, SAFEENV]}) {
			/call cast_defensive_cycle ${maDebug.Find[def].Value}
		}
		/if (SWARM || PET) {
			/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
		}
		/if (AUTO && MELEE) {
			/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
		}
	}


	| Group AA Celestial Regeneration
 	/if (${maChr.Find[swHealGroupOverTime].Value} && ${Group}) {
 	 	/if (${is_ready[${_debug}, "${maChr.Find[stAACelestialRegeneration].Value}"]}) {
 	 		/if (!${Bool[${Me.Song[${maChr.Find[stAACelestialRegeneration].Value}]}]} && !${Me.Song[${maChr.Find[stAAFocusedCelestialRegeneration].Value}].ID}) {
 				/call voodoo ${_debug} "${maChr.Find[stAACelestialRegeneration].Value}" 0 FALSE
 			}
	  }
	}

	| stage 1
	/if ((${Me.AltAbilityReady[${maChr.Find[stAAChannelingtheDivine].Value}]} || ${Me.AltAbilityReady[${maChr.Find[stAACelestialRapidity].Value}]}) && !${Me.Buff[${maChr.Find[stAAHealingFrenzy].Value}].ID}) {

	  | Spire 1:30 min duration 7:30 min reuse
   	/call cast_AA_spire ${maDebug.Find[spire].Value}

	 	| anti agro (Silent Casting 1min duration 9 min reuse)
		/if (!${Me.Buff[${maChr.Find[stAADivineRetribution].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAASilentCasting].Value}" 0 FALSE
			}
		}

	 	| 50% chance to twincast (proc:Channeling of the Divine, 45 sec duration 10 min resuse)
	 	/if (${is_ready[${_debug}, "${maChr.Find[stAAChannelingtheDivine].Value}"]}) {
	 		/call voodoo ${_debug} "${maChr.Find[stAAChannelingtheDivine].Value}" 0 FALSE
	 	}

		| - ~50% to cast times [proc:Celestial Rapidity, 1 min duration 10 min reuse]
		/if (${is_ready[${_debug}, "${maChr.Find[stAACelestialRapidity].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAACelestialRapidity].Value}" 0 FALSE
		}

	| stage 2
	} else /if ((${Me.AltAbilityReady[${maChr.Find[stAAHealingFrenzy].Value}]} || ${Me.AltAbilityReady[${maChr.Find[stAAFlurryofLife].Value}]}) && !${Me.Buff[${maData.Find[stSpire].Value}].ID}) {

		| melee block anti agro (20 min reuse)
		/if (!${Me.Song[${maChr.Find[stAASilentCasting].Value}].ID}) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAADivineRetribution].Value}"]}) {
				/call voodoo ${_debug} "${maChr.Find[stAADivineRetribution].Value}" 0 FALSE
			}
		}

		| heal power bost: 35% (proc Healing Frenzy, 40sec duration 15 min reuse)
		/if (${is_ready[${_debug}, "${maChr.Find[stAAHealingFrenzy].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAHealingFrenzy].Value}" 0 FALSE
		}

		| 100% exceptional heal (proc: Flurry of Life, 1:30 min duration 15 min reuse)
		/if (${is_ready[${_debug}, "${maChr.Find[stAAFlurryofLife].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stAAFlurryofLife].Value}" 0 FALSE
		}

	}

	| heal ward (20 min reuse)
	/if (${is_ready[${_debug}, "${maChr.Find[stAAExquisiteBenediction].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stAAExquisiteBenediction].Value}" 0 FALSE
	}

	| direct damage boost if using aggressive healing
	/if (${maHeal.Find[stHealMode].Value.Equal[aggressive]} && !${Me.Song[${maChr.Find[stAASilentCasting].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAABattleFrenzy].Value}"]}) {
    	/call voodoo ${_debug} "${maChr.Find[stAABattleFrenzy].Value}" 0 FALSE
    }
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  CHECKREZ

/return TRUE



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
		/invoke ${set_switch_env[${_debug}, swYaulp, TRUE, ${_verbage}]}

	| Veturika's Perseverance
	} else /if (${_type.Equal[vp]}) {
		/if (!${set_control_num_range[${_debug}, stPctVP, "${_verbage}", 0, 99]}) /return

	| use Issuance Shield
	} else /if (${_type.Equal[issuance]}) {
		/if (!${set_control_num_range[${_debug}, stCountIssuance, "${_verbage}", 0, 99]}) /return

	| use splashes
	} else /if (${_type.Equal[splash]}) {
		/if (!${set_control_num_range[${_debug}, stCountSplash, ${_verbage}, 0, 99]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return

	| use group Heal over Time
	} else /if (${_type.Equal[grouphot]}) {
		/invoke ${set_switch_env[${_debug}, swHealGroupOverTime, TRUE, ${_verbage}]}

	| arbitration (sheild AND AA)
	} else /if (${_type.Equal[arbitrate]}) {
		/if (!${set_control_num_range[${_debug}, stCountArbitrate, ${_verbage}, 0, 6]}) /return

	| use divine rez
	} else /if (${_type.Equal[divinerez]}) {
		/invoke ${set_switch_env[${_debug}, swDivineCleric, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[hot]}) {
		/invoke ${set_switch_env[${_debug}, swHealOverTime, TRUE, ${_verbage}]}

	| cleric summoned weapon
	} else /if (${_type.Equal[primary]}) {

		| use the weapon.. seriously.. go camp something. put some effort into the class
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[FALSE, swSummonPrimary, TRUE]}

		| spell to summon
		} else /if (${_verbage.Equal[spell]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stSummonPrimary, "${_silent}"]}

		}

		/declare _lsout list local
		/declare _lichrs listiterator local
		/invoke ${_lsout.Append[use,spell]}
		/vardata _lichrs _lsout.First.Clone
		/while (!${_lichrs.IsEnd}) {
			/varset _out ${_out} ${If[${_lichrs.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${_lichrs.Value}\ax
			/if (${_lichrs.Value.Equal[use]}) /varset _out ${_out}:${If[${maChr.Find[swSummonPrimary].Value},${on},${off}]}
			/if (${_lichrs.Value.Equal[spell]}) /varset _out ${_out}:[${If[${Bool[${maChr.Find[stSummonPrimary].Value}]},${num}${maChr.Find[stSummonPrimary].Value}\ax,${r0}]}]
			/invoke ${_lichrs.Advance}
		}
		OUT /chr primary ${_out}
		/return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return
