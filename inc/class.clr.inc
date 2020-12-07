|***
 * entropy
 * class.clr.inc
 ***|



|***
 * note: primary initilization of all cleric non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Vicar"
  /call set_data ${_debug} maData stSynergy                           map outer "Templar's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Divine Peace"

  | group regen timer
  /invoke ${set_data_timer[${_debug}, Check_Regen, 3s]}

  | beacon/burst timer slowdown
  /invoke ${set_timer[${_debug}, timer_ofLife_pause, 3s]}

  /invoke ${set_timer[${_debug}, timer_Check_Mana, 18s]}
  | Veturika's Preserverance
  /call set_data_map ${_debug} maChr stPctVP                          0                             "${auxnum} % of manna to use veturikas perseverance"
  | Quiet Prayer
  /call set_data_map ${_debug} maChr stPctQP                          0                             "${auxnum} % of group members mana to use QP on them"
  /call set_data_list ${_debug} maChr lsQPClass                       EMPTY                         "${auxli} classes you will use QP on"


  /call set_data_map ${_debug} maHeal swHealGroupOverTime             FALSE                         "${auxsw} use group heal over time spells/aas"              heal
  /call set_data_map ${_debug} maChr swCelestialRegen                 FALSE                         "${auxsw} use celestial regen aa"

  | single heal over time
  /call set_data_map ${_debug} maChr stCountIssuance                  0                             "${auxnum} number of mobs in area to use issuance"

  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell"
  /call set_data ${_debug} maData stAllianceSPA                       map outer "481"

  | issuance
  /call set_data_map ${_debug} maChr stCountIssuance                  0                             "${auxnum} number of mobs in area to use issuance"
  /call set_data_map ${_debug} maChr stIssuance                       PREGEN|1                      "${auxna} issuance spell"

  | swarm pet
  /call set_data_map ${_debug} maChr stTempPet                        PREGEN|1                      "${auxna} temp hammer pet spell"

  | splash
  /call set_data_map ${_debug} maHeal stCountSplash                   0                             "${auxnum} number mobs in range to use a splash heal"       heal

  | divine rez
  /call set_data_map ${_debug} maRez swDivineCleric                   FALSE                         "${auxsw} divine rez other clerics"                         rez

  | arbitration
  /call set_data_map ${_debug} maChr stCountArbitrate                 0                             "${auxnum} num of group hurt to use arbitration"
  /call set_data_map ${_debug} maChr stArbitratePause                 12                            "${auxnum} delay in seconds between arbitration uses"
  /call set_data_map ${_debug} maChr swArbitrateAA                    FALSE                         "${auxsw} use aa for arbitration"
  /call set_data_map ${_debug} maChr swArbitrateEpic                  FALSE                         "${auxsw} use epic for arbitration"
  /invoke ${set_timer[${_debug}, timer_arbitration_pause, ${maChr.Find[stArbitratePause].Value}s]}

  | yaulp
  /invoke ${set_data_timer[${_debug}, Check_Yaulp, 6s]}
  /call set_data_map ${_debug} maChr swYaulp                          FALSE                         "${auxsw} use yaulp (\a-win combat\ax)"
  /call set_data_map ${_debug} maChr stYaulp                          PREGEN|1                      "${auxna} name of yaulp spell/aa/item"

  | primary weapon
  /call set_data_map ${_debug} maChr swSummonPrimary                  FALSE                         "${auxsw} use summoned primary hammer"
  /call set_data_map ${_debug} maChr stSummonPrimary                  PREGEN|1                      "${auxna} name of primary hammer spell"

  | beacon of life aa count trigger
  /call set_data_map ${_debug} maChr stCountBeaconofLife              0                             "${auxnum} num of group hurt to use beacon of life aa"

  | complete heal
  /call set_data_map ${_debug} maHeal stPctCH                         0                             "${auxnum} complete heal %"                                 heal

  | AAs
  /call set_data_map ${_debug} maRez stAADivineResurrection           PREGEN|1                      "${auxna} Divine Resurrection AA"                           rez
  /call set_data_map ${_debug} maChr stAABeaconofLife                 PREGEN|1                      "${auxna} Beacon of Life aa"
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                      "${auxna} class aa unity buff"
  /call set_data_map ${_debug} maChr stAADivineArbitration            PREGEN|1                      "${auxna} divine arbitration aa"
  /call set_data_map ${_debug} maChr stAACelestialRegeneration        PREGEN|1                      "${auxna} celestial regeneration aa"
  /call set_data_map ${_debug} maChr stAAFocusedCelestialRegeneration PREGEN|1                      "${auxna} focused celestial regeneration aa"
  /call set_data_map ${_debug} maChr stAABurstofLife                  PREGEN|1                      "${auxna} burst of life aa"
  /call set_data_map ${_debug} maChr stAAForcefulRejuvenation         PREGEN|1                      "${auxna} forceful rejuvenation aa"
  /call set_data_map ${_debug} maChr stAAChannelingtheDivine          PREGEN|1                      "${auxna} channeling the divine aa"
  /call set_data_map ${_debug} maChr stAADivineRetribution            PREGEN|1                      "${auxna} divine retribution aa"
  /call set_data_map ${_debug} maChr stAASilentCasting                PREGEN|1                      "${auxna} cilent casting aa"
  /call set_data_map ${_debug} maChr stAAHealingFrenzy                PREGEN|1                      "${auxna} healing frenzy aa"
  /call set_data_map ${_debug} maChr stAAFlurryofLife                 PREGEN|1                      "${auxna} flurry of life aa"
  /call set_data_map ${_debug} maChr stAAExquisiteBenediction         PREGEN|1                      "${auxna} exquisite benediction aa"
  /call set_data_map ${_debug} maChr stAABattleFrenzy                 PREGEN|1                      "${auxna} battle frenzy aa"
  /call set_data_map ${_debug} maChr stAACelestialRapidity            PREGEN|1                      "${auxna} celestial rapidity aa"
  /call set_data_map ${_debug} maChr stAATurnUndead                   PREGEN|1                      "${auxna} turn undead aa"
  /call set_data_map ${_debug} maChr stAACelestialHammer              PREGEN|1                      "${auxna} celestial hammer aa"
  /call set_data_map ${_debug} maChr stAAVeturikasPerseverance        PREGEN|1                      "${auxna} Veturika's Perseverance AA"
  /call set_data_map ${_debug} maChr stAAQuietPrayer                  PREGEN|1                      "${auxna} Quiet Prayer AA"
  /call set_data_map ${_debug} maChr stAASanctuary                    PREGEN|1                      "${auxna} Sanctuary AA"


  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: main class routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  | correct all XTarget healing spots
  /if (AUTO && !ENGINE3) {
    /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
      /call check_xtarget_build FALSE
    }

    | sort the summoned cleric primary and any mana regen from VP if needed
    /if (!${Me.Invis}) {
      /if (${maChr.Find[swSummonPrimary].Value} && !${Me.Inventory[mainhand].ID}) {
        /if (!${FindItemCount[${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}]} && ${InvSlot[mainhand].Item.Name.NotEqual[${FindItem[${Spell[${Me.Gem[13]}].Base[1]}]}]}) /call cast_nut_smasher FALSE
        CHECKCURSOR
      }
      /if (!${timer_Check_Mana}) /call check_mana ${maDebug.Find[checkmana].Value}
      |**
      /if (!${check_exit[FALSE]} && !SAFEZONE && !${Me.Buff[Revival Sickness].ID} && ${maChr.Find[stPctVP].Value}) {
        /if (${Me.PctMana} < ${maChr.Find[stPctVP].Value}*.PCTHO) {
          /if (${is_ready[${_debug}, "${maChr.Find[stAAVeturikasPerseverance].Value}"]}) {
            /call voodoo ${_debug} "${maChr.Find[stAAVeturikasPerseverance].Value}" 0 FALSE
          }
        }
      }
      **|
    }
    BUFFNOW
  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }

  }

  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    GETINPUT class_main_CLR NA|NA
    /call check_heal_routines

    CHECKFADE
    /if (${maCC.Find[swAlliance].Value} && !${timer_Check_Alliance}) /call check_alliance ${maDebug.Find[alliance].Value}
    /if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp ${maDebug.Find[yaulp].Value}
    BUFFTEMP

    /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
    /if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
    /if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

    /if (VALID && !ENGINE3) {
      CHECKREZ
      /call set_combat_control ${maDebug.Find[decision].Value} TARGET
      /if (MELEE) /call do_melee FALSE TARGET
      /call misc_combat ${maDebug.Find[misc].Value} TARGET
      /if (!${timer_Check_deBuffs}) {
        /call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
      }
      /if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
      /if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
      /call check_item_cycle ${maDebug.Find[item].Value} TARGET
    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

    /if (AUTO && !ENGINE3) {
      BUFFNOW
      /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
      CHECKFADE
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

  }

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName}
 ***|
sub misc_combat(bool _debug, int _tmpID)
  DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }
  CHECKEXIT
  GETINPUT misc_combat_CLR NA|NA
  CHECKTIE
  CHECKDEFENSE
  BUFFNOW
  BUFFTEMP
  /if (!PULL) {
    /call get_assist_target ${maDebug.Find[target].Value}
    /if (TARGET != ${_tmpID}) /varset _tmpID TARGET
  }

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

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  GETINPUT clr_summon_primary stSummonPrimary|${maChr.Find[stSummonPrimary].Value}

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
 * use: /call check_heal [DEBUG] [group|pet|xtarget]
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  CHECKFADE
  GETINPUT clr_heal_start type|${_type}


  | Group Regen Heal
  /if (${maHeal.Find[swHealGroupOverTime].Value} && !${timer_Check_Regen}) {
    /call cast_cycle_heal_regen ${maDebug.Find[regen].Value} 0 grouphot
  }
  
  /call get_hurt ${maDebug.Find[hurt].Value} ${_type}

  | forcing of splash  
  /if (!${hurtCount} && ${maOver.Find[swOverSplashHurt].Value} && ${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
    /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
      /call Bind_command_splash ${maDebug.Find[splash].Value}
    }
  }

  /if (!${hurtCount} && ${maHeal.Find[swHealPromise].Value}) {
    /call cast_cycle_heal_promise ${_debug}
  }

  | if no one is hurt. back to the start
  /if (!${hurtCount}) {
    CHECKREZ
    SETHUD EMPTY
    /delay 1
    /return
  }

  SETHUD "heal ${_type.Lower}"

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  /if (!SAFEZONE) {

    | Complete Heal
    | requires group roll Main Tank set to function
    /if (${Select[${_type},ch]} && ${hurtCount}) {
      /call cast_cycle_heal ${_debug} ${Group.MainTank.ID} complete
      /return FALSE
    }

    | group healing
    /if (${Select[${_type},group]} && ${hurtCount}) {

      | AA Beacon of life (3 min use)
      /if (!${timer_ofLife_pause} && ${maChr.Find[stCountBeaconofLife].Value} && ${hurtCount} >= ${maChr.Find[stCountBeaconofLife].Value}) {
        /if (${is_ready[${_debug}, "${maChr.Find[stAABeaconofLife].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAABeaconofLife].Value}" 0 FALSE
          /invoke ${set_timer[${_debug}, timer_ofLife_pause, 3s]}
          /call get_hurt ${maDebug.Find[hurt].Value} ${_type}
        }
      }

      | arbitration
      /if (${maChr.Find[stCountArbitrate].Value} && ${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) {
        /call cast_arbitration ${maDebug.Find[arb].Value}
        /if (${Macro.Return}) {
          /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}, TRUE]}) {
            /return FALSE
          }
        }
      }

      | group heal
      /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /call cast_cycle_heal ${_debug} 0 group
      }
    }

    | single heal over time
    /if (${maHeal.Find[swHealOverTime].Value} && !${timer_Check_Regen}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
        /call cast_cycle_heal_regen ${maDebug.Find[regen].Value} ${hurtID} hot
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
    /if (!${timer_ofLife_pause} && ${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.85*.PCTHO) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAABurstofLife].Value}"]}) {
        /if (${Me.Casting.ID}) {
          /stopcast
        }
        /call voodoo ${_debug} "${maChr.Find[stAABurstofLife].Value}" ${hurtID} FALSE
        /invoke ${set_timer[${_debug}, timer_ofLife_pause, 3s]}
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

  /call cast_cycle_heal ${_debug} ${hurtID} single

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
    }
  }

  CHECKREZ

/return TRUE



|***
 * note: burn routines
 * "What day is today?" asked Pooh
 * "It's the day we burn this mother fucker to the ground." squeaked Piglet
 * "My favorite day." said Pooh
 ***|
sub burn(bool _debug, int _tmpID)
  DEBUG \atburn\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  CHECKEXIT
  GETINPUT clr_burn NA|NA
  CHECKTIE
  CHECKDEFENSE
  /if (!PULL) {
    /call get_assist_target ${maDebug.Find[target].Value}
    /if (TARGET != ${_tmpID}) /varset _tmpID TARGET
  }

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
  }
  /if (AUTO && MELEE) {
    /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
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
  /if ((${Me.AltAbilityReady[${maChr.Find[stAAHealingFrenzy].Value}]} || ${Me.AltAbilityReady[${maChr.Find[stAACelestialRapidity].Value}]}) && !${Me.Buff[${maChr.Find[stAAChannelingtheDivine].Value}].ID} && !${Me.Buff[${maChr.Find[stAAFlurryofLife].Value}].ID}) {

    | Spire 1:30 min duration 7:30 min reuse
    /call cast_AA_spire ${maDebug.Find[spire].Value}

    | anti agro (Silent Casting 1min duration 9 min reuse)
    /if (!${Me.Buff[${maChr.Find[stAADivineRetribution].Value}].ID}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAASilentCasting].Value}" 0 FALSE
      }
    }

    | heal power bost: 35% (proc Healing Frenzy, 40sec duration 15 min reuse)
    /if (${is_ready[${_debug}, "${maChr.Find[stAAHealingFrenzy].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAHealingFrenzy].Value}" 0 FALSE
    }

    | - ~50% to cast times [proc:Celestial Rapidity, 1 min duration 10 min reuse]
    /if (${is_ready[${_debug}, "${maChr.Find[stAACelestialRapidity].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAACelestialRapidity].Value}" 0 FALSE
    }

  | stage 2
  } else /if ((${Me.AltAbilityReady[${maChr.Find[stAAChannelingtheDivine].Value}]} || ${Me.AltAbilityReady[${maChr.Find[stAAFlurryofLife].Value}]}) && !${Me.Buff[${maChr.Find[stAAHealingFrenzy].Value}].ID} && !${Me.Buff[${maChr.Find[stAACelestialRapidity].Value}].ID}) {

    | melee block anti agro (20 min reuse)
    /if (!${Me.Song[${maChr.Find[stAASilentCasting].Value}].ID}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAADivineRetribution].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAADivineRetribution].Value}" 0 FALSE
      }
    }

    | 50% chance to twincast (proc:Channeling of the Divine, 45 sec duration 10 min resuse)
    /if (${is_ready[${_debug}, "${maChr.Find[stAAChannelingtheDivine].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAChannelingtheDivine].Value}" 0 FALSE
    }

    | 100% exceptional heal (proc: Flurry of Life, 1:30 min duration 15 min reuse)
    /if (${is_ready[${_debug}, "${maChr.Find[stAAFlurryofLife].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAFlurryofLife].Value}" 0 FALSE
    }

  }

  | heal ward pet (20 min reuse)
  /if (${is_ready[${_debug}, "${maChr.Find[stAAExquisiteBenediction].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAExquisiteBenediction].Value}" 0 FALSE
  }

  | direct damage boost
  /if (!${Me.Song[${maChr.Find[stAASilentCasting].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAABattleFrenzy].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAABattleFrenzy].Value}" 0 FALSE
    }
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

  CHECKREZ

/return TRUE



|***
 * note: HP arbitration
 * use: /call cast_arbitration ${maDebug.Find[arb].Value}
 ***|
sub cast_arbitration(bool _debug)
  DEBUG \atcast_arbitration\ax()

  /if (${timer_arbitration_pause}) {
    /return FALSE
  }

  | AA arb
  /if (${maChr.Find[swArbitrateAA].Value}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAADivineArbitration].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAADivineArbitration].Value}" 0 FALSE
      /invoke ${set_timer[${_debug}, timer_arbitration_pause, ${maChr.Find[stArbitratePause].Value}s]}
      /return TRUE
    }
  }

  /if (${maChr.Find[swArbitrateEpic].Value} && !${timer_arbitration_pause}) {

    | Second epic shield 2.0
    /if (${is_ready[${_debug}, "${sqlite.Result[epic${Me.Class.ShortName} 3 name]}"]}) {
      /call voodoo ${_debug} "${sqlite.Result[epic${Me.Class.ShortName} 3 name]}" 0 FALSE
      /invoke ${set_timer[${_debug}, timer_arbitration_pause, ${maChr.Find[stArbitratePause].Value}s]}
      /return TRUE
    }

    | First epic shield 1.5
    /if (${is_ready[${_debug}, "${sqlite.Result[epic${Me.Class.ShortName} 2 name]}"]}) {
      /call voodoo ${_debug} "${sqlite.Result[epic${Me.Class.ShortName} 2 name]}" 0 FALSE
      /invoke ${set_timer[${_debug}, timer_arbitration_pause, ${maChr.Find[stArbitratePause].Value}s]}
      /return TRUE
    }

  }

/return FALSE



|***
 * note: predictive combat preperation
 * use: /call prediction ${maDebug.Find[pred].Value}
 ***|
sub prediction(bool _debug)
  DEBUG \atprediction\ax()

  | Group Regen Heal
  /if (${maHeal.Find[swHealGroupOverTime].Value}) {
    /call cast_cycle_heal_regen ${_debug} 0 healgrouphot
  }

/return TRUE



|***
 * note: check alliance and reapply if needed
 * use: /call cast_alliance_${Me.Class.ShortName} [DEBUG]
 ***|
sub check_alliance(bool _debug)
  DEBUG \atcheck_alliance\ax()

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
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-wt:${_type}, v:"${_verbage}", v2:"${_verbage2}"\ax)

  | to Yaulp or not
  /if (${_type.Equal[yaulp]}) {
    /invoke ${set_switch_env[${_debug}, swYaulp, TRUE, ${_verbage}]}

  | Veturika's Perseverance
  } else /if (${_type.Equal[vp]}) {
    /if (!${set_control_num_range[${_debug}, stPctVP, "${_verbage}", 0, 99]}) /return FALSE

  | use Issuance Shield
  } else /if (${_type.Equal[issuance]}) {
    /if (!${set_control_num_range[${_debug}, stCountIssuance, "${_verbage}", 0, 99]}) /return FALSE


  | arbitration (sheild AND AA)
  } else /if (${_type.Equal[arb]}) {
    /if (${_verbage.Equal[count]}) {
      /if (!${set_control_num_range[${_debug}, stCountArbitrate, ${_verbage2}, 0, 6]}) /return FALSE
    } else /if (${_verbage.Equal[pause]}) {
      /if (!${set_control_num_range[${_debug}, stArbitratePause, ${_verbage2}, 0, 999]}) /return FALSE
    } else /if (${_verbage.Equal[aa]}) {
      /invoke ${set_switch_env[${_debug}, swArbitrateAA, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[epic]}) {
      /invoke ${set_switch_env[${_debug}, swArbitrateEpic, TRUE, ${_verbage2}]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[pause,stArbitratePause]}
    /invoke ${maControl.Add[count,stCountArbitrate]}
    /invoke ${maControl.Add[aa,swArbitrateAA]}
    /invoke ${maControl.Add[epic,swArbitrateEpic]}
    /invoke ${set_control_output[${_debug}, maChr, "chr arb", maControl]}
    /return TRUE

  | set beacon of life count
  } else /if (${_type.Equal[beacon]}) {
    /if (!${set_control_num_range[${_debug}, stCountBeaconofLife, "${_verbage}", 0, 6]}) /return FALSE

  | cleric summoned weapon
  } else /if (${_type.Equal[primary]}) {

    | use the weapon.. seriously.. go camp something. put some effort into the class
    /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swSummonPrimary, TRUE]}
    | spell to summon
    } else /if (${_verbage.Equal[spell]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stSummonPrimary, "${_verbage2}"]}
    }
    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swSummonPrimary]}
    /invoke ${maControl.Add[spell,stSummonPrimary|br]}
    /invoke ${set_control_output[${_debug}, maChr, "chr primary", maControl]}
    /return TRUE

  | cleric summoned weapon
  } else /if (${_type.Equal[qp]}) {
    /if (${_verbage.Equal[pct]}) {
      /if (!${set_control_num_range[${_debug}, stPctQP, "${_verbage2}", 0, 99]}) /return FALSE
    } else /if (${_verbage.Equal[class]}) {
      /call set_control_shortname ${_debug} lsQPClass ${_verbage2}
      /invoke ${out[12, "/chr qp class ${dot} ${Macro.Return}"]}
      /return TRUE
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[pct,stPctQP]}
    /invoke ${maControl.Add[class,submenu]}
    /invoke ${set_control_output[${_debug}, maChr, "chr qp", maControl]}
    /return TRUE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE








|**
#bind setCHeal /cheal
sub Bind_setCHeal(string _type, string _verbage, bool _silent, bool _debug)
  DEBUG \a-p[Bind_setCHeal\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /declare _sep bool local
  /declare _listout string local
  /declare _count int local 0
  /declare _bind string local cheal
  /declare _lstClass string local |chpct|heal|listen|pause|tank

  | listen for CH Chain rotation calls
  /if (${_type.Equal[listen]}) {
    /call set_switch ${chrID} swCHChainListen ${_verbage}

  | sets the CH rotation data
  } else /if (${_type.Equal[chchain]}) {
    OUT /clr chchain \aw|timeout|tank1|tank2|tank3|tank4\ax
    /if (${Bool[${_verbage}]}) {
      /varset setCHPassedReqs |${Me.DisplayName}${_verbage}
    }
    /return

  | Complete Heal
  } else /if (${_type.Equal[chpct]}) {
    /if (${Range.Between[0,99:${Int[${_verbage}]}]}) {
      /call sql_update FALSE ${chrID} CompleteHealpct ${_verbage}
    } else /if (!${Range.Between[0,99:${Int[${_verbage}]}]}) {
      VOUT /${_bind} ch(${cnum}${CompleteHealpct}\ax) allowable range ${sep} ${cinfo}0 - 99\ax
      /return
    }

  | set the Heal to be used
  } else /if (${_type.Equal[heal]}) {
    /call sql_update FALSE ${chrID} setCompleteHeal ${_verbage}


  | set the pause time to be used
  } else /if (${_type.Equal[pause]}) {
    /if (${_silent}) {
      /varset setCHPause ${_verbage}
    } else {
      /dgaexecute CLR /varset setCHPause ${_verbage}
    }
    /delay 5

  | set the tank to be healed
  } else /if (${_type.Equal[tank]}) {
    /if (${_silent}) {
      /varset setCHTank ${_verbage}
    } else {
      /dgaexecute CLR /varset setCHTank ${_verbage}
    }
    /delay 5

  }

  /for _count 1 to ${_lstClass.Count[|]}
    /varset _listout ${_listout} ${If[${_sep},${sep},]} ${info}${_lstClass.Arg[${_count},|]}\ax
    /if (${_lstClass.Arg[${_count},|].Equal[chpct]}) /varset _listout ${_listout}:${If[${CompleteHealpct},${cnum}${CompleteHealpct}\ax,${off}]}
    /if (${_lstClass.Arg[${_count},|].Equal[listen]}) /varset _listout ${_listout}:${If[${swCHChainListen},${oon},${ooff}]}
    /if (${_lstClass.Arg[${_count},|].Equal[heal]}) /varset _listout ${_listout}:${num}${setCompleteHeal}\ax
    /if (${_lstClass.Arg[${_count},|].Equal[pause]}) /varset _listout ${_listout}:${If[${Bool[${setCHPause}]},${cnum}${setCHPause}\ax,${off}]}
    /if (${_lstClass.Arg[${_count},|].Equal[tank]}) /varset _listout ${_listout}:${If[${Bool[${setCHTank}]},${cnum}${setCHTank}\ax,${off}]}
  /varset _sep TRUE
  /next _count
  VOUT /${_bind} ${_listout}
/return


**|




sub check_mana(bool _debug)
  DEBUG \atcheck_mana\ax()

  /invoke ${set_data_timer[${_debug}, Check_Mana, RESTART]}

  CHECKEXIT check_mana NA|NA
  ISMEDEAD
  /if (SAFEZONE) {
    /return FALSE
  }

  | check self for VP first
  /if (${maChr.Find[stPctVP].Value}) {
    /if (${Me.PctMana} < ${maChr.Find[stPctVP].Value}*.PCTHO) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAVeturikasPerseverance].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAVeturikasPerseverance].Value}" 0 FALSE
        /return TRUE
      }
    }
  }

  | check group for QP
  /if (!${Group}) {
    /return FALSE
  }

  /if (!${maChr.Find[stPctQP].Value} || !${is_ready[${_debug}, "${maChr.Find[stAAQuietPrayer].Value}"]}) {
    /return FALSE
  }

  | cycle the group. skipping yourself
  /declare _found bool local FALSE
  /declare _count int local 0
  /for _count 0 to ${Group}

    /if (${Group.Member[${_count}].ID} == ${Me.ID}) {
      /continue
    }
    
    /if (!${Spawn[${Group.Member[${_count}].ID}].ID}) {
      DEBUG ${break}notinzone
    }
    
    /if (!${lsQPClass.Contains[${Group.Member[${_count}].Class.ShortName}]}) {
      DEBUG ${break}Bad Class${sep}${Group.Member[${_count}].Class.ShortName}
      /continue
    }

    /if (${Group.Member[${_count}].Type.NotEqual[CORPSE]}) {
      DEBUG ${break}dead
     /continue
    }

    /if (${Group.Member[${_count}].Distance} > ${Spell[${maChr.Find[stAAQuietPrayer].Value}].MyRange}) {
      DEBUG ${break} > ${Spell[${maChr.Find[stAAQuietPrayer].Value}].MyRange} range
      /continue
    }

    /if (${Group.Member[${_count}].PctMana} <=  ${maChr.Find[stPctVP].Value}*.PCTHO) {
      DEBUG ${sep} mana || stamina <= ${maChr.Find[stPctVP].Value}*.PCTHO
      /varset _vound TRUE
    }

    /if (${_found}) {  
      /call voodoo ${_debug} "${maChr.Find[stAAQuietPrayer].Value}" ${Group.Member[${_count}].ID} FALSE
      /return TRUE
    }

  /next _count
  

/return TRUE



