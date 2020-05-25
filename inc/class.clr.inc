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

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/call set_data ${_debug} maData stSpire															map outer "Spire of the Vicar"
	/call set_data ${_debug} maData stSynergy														map outer "Templar's Synergy"
	/call set_data ${_debug} maData stEpic1															map outer "Water Sprinkler of Nem Ankh"
	/call set_data ${_debug} maData stEpic15														map outer "Harmony of the Soul"
	/call set_data ${_debug} maData stEpic2															map outer "Aegis of Superior Divinity"
	/call set_data ${_debug} maData stFade															map outer "Divine Peace"
	/call set_data ${_debug} maData stCompleteHeal											map outer "Complete Heal"


	| group regen timer
	/invoke ${set_data_timer[${_debug}, Check_Regen, 3s]}

	| Veturika's Preserverance
	/call set_data_map ${_debug} maChr stPctVP													0															"${auxnum} % of manna to use veturikas perseverance"

  /call set_data_map ${_debug} maHeal swHealGroupOverTime							FALSE													"${auxsw} use group heal over time spells/aas" heal
  /call set_data_map ${_debug} maChr swCelestialRegen									FALSE													"${auxsw} use celestial regen aa"

	| single heal over time
	/call set_data_map ${_debug} maChr stCountIssuance									0															"${auxnum} number of mobs in area to use issuance"

	| count heal over time single
	/call set_data_map ${_debug} maHeal swHealOverTime									FALSE													"${auxsw} enable to use heal over time" heal

	/call set_data_map ${_debug} maHeal stCountHealGroup								0															"${auxnum} number in group hurt to use a group heal" heal

	/call set_data_map ${_debug} maChr stAlliance												PREGEN|1											"${auxna} alliance spell"
	/call set_data ${_debug} maData stAllianceSPA												map outer "481"

	| issuance
	/call set_data_map ${_debug} maChr stCountIssuance									0															"${auxnum} number of mobs in area to use issuance"
	/call set_data_map ${_debug} maChr stIssuance												PREGEN|1											"${auxna} issuance spell"

	| swarm pet
	/call set_data_map ${_debug} maChr stTempPet												PREGEN|1											"${auxna} temp hammer pet spell"

	| splash
	/call set_data_map ${_debug} maHeal stCountSplash										0															"${auxnum} number mobs in range to use a splash heal" heal

	| divine rez
	/call set_data_map ${_debug} maRez swDivineCleric										FALSE													"${auxsw} divine rez other clerics"

	| Rampage Tank
	/call set_data_map ${_debug} maHeal stRampageTank										FALSE													"${auxna} rampage tank to not heal (\a-wcompletely ignore\ax)" heal

	| arbitration
	/call set_data_map ${_debug} maChr stCountArbitrate									0															"${auxnum} num of group hurt to use arbitration "
	/call set_data_map ${_debug} maChr swArbitrateAA 										FALSE													"${auxsw} use aa for arbitration"
	/call set_data_map ${_debug} maChr swArbitrateEpic									FALSE													"${auxsw} use epic for arbitration"

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

	| temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon									EMPTY													"${auxli} list of toons to use your temp buff on"
	/call set_data_map ${_debug} maChr stTempBuff		 										FALSE													"${auxna} temp buff spell"

	| primary weapon
  /call set_data_map ${_debug} maChr swSummonPrimary									FALSE													"${auxsw} use summoned primary hammer"
  /call set_data_map ${_debug} maChr stSummonPrimary									PREGEN|1											"${auxna} name of primary hammer spell"

	| beacon of life aa
	/call set_data_map ${_debug} maChr stCountBeaconofLife							0																	"${auxnum} num of group hurt to use beacon of life aa"
	/call set_data_map ${_debug} maChr stAABeaconofLife									"Beacon of Life"									"${auxna} neacon of life aa"

	| AAs
	/call set_data_map ${_debug} maChr stAAUnity												"Saint's Unity"										"${auxna} class aa unity buff"
	/call set_data_map ${_debug} maChr stAADivineArbitration						"Divine Arbitration"							"${auxna} divine arbitration aa"
	/call set_data_map ${_debug} maChr stAACelestialRegeneration				"Celestial Regeneration"					"${auxna} celestial regeneration aa"
	/call set_data_map ${_debug} maChr stAAFocusedCelestialRegeneration	"Focused Celestial Regeneration"	"${auxna} focused celestial regeneration aa"
	/call set_data_map ${_debug} maChr stAABurstofLife									"Burst of Life"										"${auxna} burst of life aa"
	/call set_data_map ${_debug} maChr stAAForcefulRejuvenation					"Forceful Rejuvenation"						"${auxna} forceful rejuvenation aa"
	/call set_data_map ${_debug} maChr stAAChannelingtheDivine					"Channeling the Divine"						"${auxna} channeling the divine aa"
	/call set_data_map ${_debug} maChr stAADivineRetribution						"Divine Retribution"							"${auxna} divine retribution aa"
	/call set_data_map ${_debug} maChr stAASilentCasting								"Silent Casting"									"${auxna} cilent casting aa"
	/call set_data_map ${_debug} maChr stAAHealingFrenzy								"Healing Frenzy"									"${auxna} healing frenzy aa"
	/call set_data_map ${_debug} maChr stAAFlurryofLife									"Flurry of Life"									"${auxna} flurry of life aa"
	/call set_data_map ${_debug} maChr stAAExquisiteBenediction					"Exquisite Benediction"						"${auxna} exquisite benediction aa"
	/call set_data_map ${_debug} maChr stAABattleFrenzy									"Battle Frenzy"										"${auxna} battle frenzy aa"
	/call set_data_map ${_debug} maChr stAACelestialRapidity						"Celestial Rapidity"							"${auxna} celestial rapidity aa"
	/call set_data_map ${_debug} maChr stAATurnUndead										"Turn Undead"											"${auxna} turn undead aa"
	/call set_data_map ${_debug} maChr stAACelestialHammer							"Celestial Hammer"								"${auxna} celestial hammer aa"

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

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

	/while (${check_combat_status[${maDebug.Find[status].Value}]}) {
		GETINPUT clr_main_loop NA|NA
		/call check_heal_routines

		CHECKFADE
		/if (${maCC.Find[swAlliance].Value} && !${timer_Check_Alliance}) /call check_${Me.Class.ShortName}_alliance ${maDebug.Find[alliance].Value}
		/if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp ${maDebug.Find[yaulp].Value}
		/if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stWard lsWardToon
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stRetort lsRetortToon
			/call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
		}
		/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
		/if (BURN) /call burn_${Me.Class.ShortName} ${maDebug.Find[burn].Value} TARGET
		/if (${check_class_loop[${maDebug.Find[status].Value}]}) /return

		/if (VALID) {
			CHECKREZ
			/call set_combat FALSE
			/if (MELEE) /call do_melee FALSE TARGET
			/call misc_combat_${Me.Class.ShortName} ${maDebug.Find[misc].Value} TARGET
			/if (!${timer_Check_deBuffs}) {
				/call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
			}
			/if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
			/if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
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
	FLOW misc_combat_CLR

	ISMEDEAD
	/if (${target[isdead, ${_tmpID}]}) {
		/return FALSE
	}
	CHECKEXIT
	GETINPUT misc_combat_CLR NA|NA
	CHECKTIE

	CHECKDEFENSE

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

	/if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) {
		/call cast_yaulp ${maDebug.Find[yaulp].Value}
	}

	/if (SWARM) {
		/call cast_temp_pet ${maDebug.Find[temp].Value} ${_tmpID} "${maChr.Find[stTempPet].Value}"
	}

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
	FLOW cast_nut_smasher

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
	FLOW check_heal_CLR ${_type}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	CHECKREZ
	CHECKFADE
	GETINPUT clr_heal_start type|${_type}

	SETHUD "heal ${_type.Lower}"

	| /declare _count int local 0

	| Group Regen Heal
	/if (${maHeal.Find[swHealGroupOverTime].Value} && !${timer_Check_Regen}) {
		/call cast_cycle_heal_regen ${maDebug.Find[regen].Value} 0 healgrouphot
	}

	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

	/if (!${hurtCount} && ${maHeal.Find[swHealPromise].Value}) {
		/call cast_cycle_heal_promise ${_debug}
	}

	| if no one is hurt. back to the start
	/if (!${hurtCount}) {
		SETHUD EMPTY
		/return
	}

 	/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
 		/return FALSE
 	}

	/if (!${maEnv.Find[swZoneSafe].Value}) {

		| group healing
		/if (${Select[${_type},group]} && ${Group}) {

			| AA Beacon of life (3 min use)
		  /if (${maChr.Find[stCountBeaconofLife].Value} && ${hurtCount} >= ${maChr.Find[stCountBeaconofLife].Value}) {
			  /if (${is_ready[${_debug}, "${maChr.Find[stAABeaconofLife].Value}"]}) {
			  	/call voodoo ${_debug} "${maChr.Find[stAABeaconofLife].Value}" 0 FALSE
			  	/call get_hurt ${maDebug.Find[hurt].Value} ${_type}
			  }
			}

			| arbitration
			/if (${maChr.Find[stCountArbitrate].Value} && ${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) {
				/call cast_arbitration ${maDebug.Find[arb].Value}
			 	/if (${Macro.Return}) {
			 		/if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
			 			/return FALSE
			 		}
			 	}
			}

			| group heal
			/if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
				/call cast_cycle_heal ${_debug} 0 healgroup
 			}
		}


		| single heal over time
		/if (${maChr.Find[swHealOverTime].Value} && !${timer_Check_Regen}) {
			/if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
				/call cast_cycle_heal_regen ${maDebug.Find[regen].Value} ${hurtID} healhot
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
		/if (${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
			/if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
				/call Bind_command_splash ${maDebug.Find[splash].Value}
			}
		}

		| Panic AA single target heal
	  /if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.85*.PCTHO) {
			/if (${is_ready[${_debug}, "${maChr.Find[stAABurstofLife].Value}"]}) {
				/if (${Me.Casting.ID}) {
					/stopcast
				}
				/call voodoo ${_debug} "${maChr.Find[stAABurstofLife].Value}" ${hurtID} FALSE
			}
	  }

		/call get_hurt ${maDebug.Find[hurt].Value} ${_type}

		/if (${maChr.Find[stCountIssuance].Value}) {
			/if (${SpawnCount[npc radius ENV PS4]} >= ${maChr.Find[stCountIssuance].Value}) {
				/call Bind_command_issuance FALSE
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

	/call cast_cycle_heal ${_debug} ${hurtID} healsingle

/return	TRUE



|***
 * note: Cast the cleric floatie shield AE regen. Twin cast it if the AA is ready.
 * use: /issuance
 ***|
#bind command_issuance /issuance
sub Bind_command_issuance(bool _debug)
	DEBUG \atBind_command_issuance\ax()
	FLOW command_issuance

	/if (!${check_class[FALSE, |CLR]}) /return FALSE
	ISMEDEAD
	CHECKEXIT

	:castIssuance
	/if (!${is_ready[${_debug}, "${maChr.Find[stIssuance].Value}"]}) {
		/return FALSE
	}

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
	FLOW burn_CLR

	ISMEDEAD
	CHECKEXIT
	GETINPUT clr_burn NA|NA
	CHECKTIE

	/if (${Bool[${_tmpID}]}) {
		/if (${target[isdead, ${_tmpID}]}) {
			/return FALSE
		}

		CHECKDEFENSE
		/if (SWARM || PET) {
			/call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
		}
		/if (AUTO && MELEE) {
			/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
		}
	}

	/if (ENGINE2) {
		/call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
		/return TRUE
	}

	| Group AA Celestial Regeneration
 	/if (${maHeal.Find[swHealGroupOverTime].Value} && ${Group.Members} && !${Raid.Members}) {
 		/if (!${Bool[${Me.Song[${maChr.Find[stAACelestialRegeneration].Value}]}]} && !${Me.Song[${maChr.Find[stAAFocusedCelestialRegeneration].Value}].ID}) {
	 	 	/if (${is_ready[${_debug}, "${maChr.Find[stAACelestialRegeneration].Value}"]}) {
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
	/if (!${Me.Song[${maChr.Find[stAASilentCasting].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAABattleFrenzy].Value}"]}) {
    	/call voodoo ${_debug} "${maChr.Find[stAABattleFrenzy].Value}" 0 FALSE
    }
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  CHECKREZ

/return TRUE



|***
 * note: HP arbitration
 * use: /call cast_arbitration ${maDebug.Find[arb].Value}
 ***|
sub cast_arbitration(bool _debug)
	DEBUG \atcast_arbitration\ax()
	FLOW cast_arbitration

  | AA arb
  /if (${maChr.Find[swArbitrateAA].Value}) {
  	/if (${is_ready[${_debug}, "${maChr.Find[stAADivineArbitration].Value}"]}) {
	    /call voodoo ${_debug} "${maChr.Find[stAADivineArbitration].Value}" 0 FALSE
	    /call get_hurt ${maDebug.Find[hurt].Value} reset
	    /return TRUE
	  }
  }

  /if (${maChr.Find[swArbitrateEpic].Value}) {

		| Second epic shield
	  /if (${is_ready[${_debug}, "${maData.Find[stEpic2].Value}"]}) {
	    /call voodoo ${_debug} "${maData.Find[stEpic2].Value}" 0 FALSE
	    /call get_hurt ${maDebug.Find[hurt].Value} reset
	    /return TRUE
	  }

	  | First epic shield
	  /if (${is_ready[${_debug}, "${maData.Find[stEpic15].Value}"]}) {
	    /call voodoo ${_debug} "${maData.Find[stEpic15].Value}" 0 FALSE
	    /call get_hurt ${maDebug.Find[hurt].Value} reset
	    /return TRUE
	  }
	}

/return FALSE



|***
 * note: predictive combat preperation
 * use: /call prediction_${Me.Class.ShortName} ${maDebug.Find[pred].Value}
 ***|
sub prediction_CLR(bool _debug)


	| Group Regen Heal
	/if (${maHeal.Find[swHealGroupOverTime].Value}) {
		/call cast_cycle_heal_regen ${_debug} 0 healgrouphot
	}




/return tRUE









|***
 * note: check alliance and reapply if needed
 * use: /call cast_alliance_${Me.Class.ShortName} [DEBUG]
 ***|
sub check_CLR_alliance(bool _debug)
	DEBUG \atcheck_${Me.Class.ShortName}_alliance\ax()
 	FLOW cast_alliance_${Me.Class.ShortName} (1)

	CHECKEXIT
	GETINPUT

	| restart abuse timer
  /invoke ${set_data_timer[${_debug}, Check_Alliance, RESTART]}

	| this will probably rarely fail. alliance for healers is an instant recast
	/if (!${is_ready[${_debug}, "${maChr.Find[stAlliance].Value}"]}) {
		/return FALSE
	}

	| find out who the hell we should be assisting
	/declare _tmpID int local ${get_assist_info[${_debug}, id]}

	|
	/if (!${_tmpID}) {
		/return FALSE
	}

	| set our target. we need to check buffs
	/invoke ${target[set, ${_tmpID}]}

	/delay 5 ${Target.BuffsPopulated}


	/if (${Target.BuffsPopulated} && !${Target.Buff[${maChr.Find[stAlliance].Value}].ID}) {
		/call voodoo ${_debug} "${maChr.Find[stAlliance].Value}" ${_tmpID} FALSE
	}



/return TRUE















|***
 * note: cleric specific control
 * use: /chr
 ***|
sub set_control_CLR(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atset_control_CLR\ax(\a-wt:${_type}, v:"${_verbage}", s:${_silent}\ax)

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


	| arbitration (sheild AND AA)
	} else /if (${_type.Equal[arb]}) {
		/if (${_verbage.Equal[count]}) {
			/if (!${set_control_num_range[${_debug}, stCountArbitrate, ${_silent}, 0, 6]}) /return
		} else /if (${_verbage.Equal[aa]}) {
			/invoke ${set_switch_env[${_debug}, swArbitrateAA, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[epic]}) {
			/invoke ${set_switch_env[${_debug}, swArbitrateEpic, TRUE, ${_silent}]}
		}

		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[count,stCountArbitrate]}
		/invoke ${maControl.Add[aa,swArbitrateAA]}
		/invoke ${maControl.Add[epic,swArbitrateEpic]}
		/invoke ${set_control_output[${_debug}, maChr, "chr arb", maControl]}
		/return TRUE

	| use divine rez
	} else /if (${_type.Equal[divinerez]}) {
		/invoke ${set_switch_env[${_debug}, swDivineCleric, TRUE, ${_verbage}]}

	| defensive mob agro count
	} else /if (${_type.Equal[def]}) {
		/if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return

	| set ae on/off / count
	} else /if (${_type.Equal[ae]}) {
		/if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return

	| set beacon of life count
	} else /if (${_type.Equal[beacon]}) {
		/if (!${set_control_num_range[${_debug}, stCountBeaconofLife, "${_verbage}", 0, 6]}) /return

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


