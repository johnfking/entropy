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
  /call set_data_map ${_debug} maHeal stPctCH                         0                             "${auxnum} complete heal %"                                   heal

  | chain complete heal 
  /call set_data_map ${_debug} maHeal stPctChain                       0                            "${auxnum} % of tanks health to start the ch. 0=instantly"    heal                     
  /call set_data_map ${_debug} maHeal swChain                          FALSE                        "${auxsw} actively listen for complete heal chain rotation"   heal
  /call set_data_list ${_debug} maHeal lsChain                         EMPTY                        "${auxli} list of tanks to heal"                              heal
  /call set_data ${_debug} maHeal swChainToken                         map outer FALSE
  /call set_data_map ${_debug} maHeal stChainPause                     0                            "${auxnum}duration to pause (in seconds) once getting tolken to start casting"    heal                     

  | try to twinheal
  /call set_data_map ${_debug} maHeal swTwinHeal                       FALSE                        "${auxsw} allow the code to try to force twinheal buffs before healing"   heal


  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                xtarget                                   "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 heal|alliance|manarecover|burn            "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|heal|misc|debuff|dot|nuke|item      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover                               "${auxli} class post combat"


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
  /call set_data_map ${_debug} maChr stAAVeturikasPerseverance        PREGEN|1                      "${auxna} veturika's verseverance AA"
  /call set_data_map ${_debug} maChr stAAQuietPrayer                  PREGEN|1                      "${auxna} quiet vrayer AA"
  /call set_data_map ${_debug} maChr stAASanctuary                    PREGEN|1                      "${auxna} sanctuary AA"
  
  
  | test db entry 
  | /call set_data_map ${_debug} maChr stDeleteMe                    FALSE                            "DELETE THIS ENTRY"
  

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

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase

    | sort the summoned cleric primary and any mana regen from VP if needed
    /if (!${Me.Invis}) {
      /if (${maChr.Find[swSummonPrimary].Value} && !${Me.Inventory[mainhand].ID}) {
        /if (!${FindItemCount[${Spell[${maChr.Find[stSummonPrimary].Value}].Base[1]}]} && ${InvSlot[mainhand].Item.Name.NotEqual[${FindItem[${Spell[${Me.Gem[13]}].Base[1]}]}]}) /call cast_nut_smasher FALSE
        CHECKCURSOR
      }
      /if (!${timer_Check_Mana}) {
        /call check_mana ${maDebug.Find[checkmana].Value}
      }
    }

  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }

  }
  
  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    GETINPUT class_main_CLR NA|NA

    /if (${maHeal.Find[swChain].Value}) {
      /call find_chain_token
      /if (${maHeal.Find[swChainToken].Value}) {
        /call completeheal_chain ${maDebug.Find[chain].Value}
      }
    }
    /call class_cycle lsOrderClassPre
    /if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp ${maDebug.Find[yaulp].Value}

    /if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

    /if (VALID && !ENGINE3) {
      /call class_cycle lsOrderClass
    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

    /if (AUTO && !ENGINE3) {
      /call class_cycle lsOrderClassPost
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

  }

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_${Me.Class.ShortName} spawnID
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD
  CHECKEXIT
  GETINPUT misc_combat_CLR NA|NA

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  /if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) {
    /call cast_yaulp ${maDebug.Find[yaulp].Value}
  }

  /if (${maHeal.Find[swChain].Value} && ${maHeal.Find[swChainToken].Value}) {
    /call completeheal_chain ${maDebug.Find[chain].Value}
  }  

  /if (SWARM) {
    /call cast_temp_pet ${maDebug.Find[temp].Value} ${_spawn.ID} "${maChr.Find[stTempPet].Value}"
  }

  | pet AA hammer
  /if (${maBurn.Find[swBurnForce].Value} || (${maBurn.Find[swBurnAuto].Value} && ${lsZoneNamed.Contains[${Target.DisplayName}]})) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAACelestialHammer].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  | undead aa dot
  /if (${_spawn.Body.Name.Equal[Undead]} && DOT) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAATurnUndead].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
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
    /call cast_item_summon ${_debug} stSummonPrimary
  }

  /delay 15s !${Me.Casting.ID}

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

  CHECKEXIT
  GETINPUT clr_heal_start type|${_type}
  
  /if (${maHeal.Find[swChain].Value} && ${maHeal.Find[swChainToken].Value}) {
    /call completeheal_chain ${maDebug.Find[chain].Value}
  }
  
  | Complete Heal
  /if (${maHeal.Find[stPctCH].Value}) {
    /if (${Group.MainTank.PctHPs} <= ${maHeal.Find[stPctCH].Value}) {
      SETHUD "heal complete"
      /call cast_cycle_heal ${_debug} ${Group.MainTank.ID} complete
      SETHUD FALSE
      /return FALSE
    }
  }  

  /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

  | Group Regen Heal
  /if (${maHeal.Find[swHealGroupOverTime].Value} && !${timer_Check_Regen}) {
    /if (AE# && ${SpawnCount[npc radius ENV]} >= AE#) {
      /call cast_cycle_heal_regen ${maDebug.Find[regen].Value} ${Me.ID} grouphot
    }
  }
  
  | forcing of splash  
  /if (!${hurtCount} && ${maOver.Find[swOverSplashHurt].Value} && ${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
    /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
      /call Bind_command_splash ${maDebug.Find[splash].Value}
    }
  }

  /if (!${hurtCount} && ${maHeal.Find[swHealPromise].Value}) {
    /call cast_cycle_heal_promise ${_debug}
  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    SETHUD
    /delay 1
    /return FALSE
  }

  SETHUD "heal ${_type.Lower}"
  
  /if (!SAFEZONE) {

    | group healing
    /if (${Select[${_type},group]} && ${hurtCount}) {

      | AA Beacon of life (3 min use)
      /if (!${timer_ofLife_pause} && ${maChr.Find[stCountBeaconofLife].Value} && ${hurtCount} >= ${maChr.Find[stCountBeaconofLife].Value}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAABeaconofLife].Value}"]}) {
          /call cast ${_debug} 0 FALSE
          /invoke ${set_timer[${_debug}, timer_ofLife_pause, 3s]}
          /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}
        }
      }

      | arbitration
      /if (${maChr.Find[stCountArbitrate].Value} && ${hurtCount} >= ${maChr.Find[stCountArbitrate].Value}) {
        /call cast_arbitration ${maDebug.Find[arb].Value}
        /if (${Macro.Return}) {
          /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}, TRUE]}) {
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
    /if (${maHeal.Find[swHealOverTime].Value}) {
      /if (${hurtData.PctHPs} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
        /call cast_cycle_heal_regen ${maDebug.Find[regen].Value} ${hurtData.ID} hot
      }
    }

    | Focused Celestial Regeneration - out of raid only
    /if (!${Raid.Members}) {
      /if (${hurtData.PctHPs} <= ${maHeal.Find[stHealPoint${hurtData.Class.ShortName}].Value}*.PCTHO && !${Me.Song[${maChr.Find[stAACelestialRegeneration].Value}].ID}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusedCelestialRegeneration].Value}"]}) {
          /call cast ${_debug} ${hurtData.ID} FALSE
        }
      }
    }

    | GoM
    /if (${Me.Song[Gift of Mana].ID}) {
      /if (${hurtData.PctHPs} < ${maHeal.Find[stHealPoint${hurtData.Class.ShortName}].Value}*.PCTHO) {
        /call cast_list ${_debug} lsGoM ${hurtData.ID}
      }
    }

    /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

    CHECKEXIT
    GETINPUT clr_heal_mid type|${_type}

    | splash heal
    /if (${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
      /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
        /call Bind_command_splash ${maDebug.Find[splash].Value}
      }
    }

    | Panic AA single target heal
    /if (!${timer_ofLife_pause} && ${hurtData.PctHPs} <= ${maHeal.Find[stHealPoint${hurtData.Class.ShortName}].Value}*.85*.PCTHO) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAABurstofLife].Value}"]}) {
        /if (${Me.Casting.ID}) {
          /stopcast
        }
        /call cast ${_debug} ${hurtData.ID} FALSE
        /invoke ${set_timer[${_debug}, timer_ofLife_pause, 3s]}
      }
    }

    /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

    /if (${maChr.Find[stCountIssuance].Value}) {
      /if (${SpawnCount[npc radius ENV PS4]} >= ${maChr.Find[stCountIssuance].Value}) {
        /call Bind_command_issuance FALSE
      }
    }

  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT clr_heal_end type|${_type}

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

/return TRUE



|***
 * note: Cast the cleric floatie shield AE regen. Twin cast it if the AA is ready.
 * use: /issuance
 ***|
#bind command_issuance /issuance
sub Bind_command_issuance(bool _debug)
  DEBUG \atBind_command_issuance\ax()

  /if (!${check_class[FALSE, |CLR]}) {
    /return FALSE
  }
  
  ISMEDEAD
  CHECKEXIT

  /if (!${cast_data[${_debug}, "${maChr.Find[stIssuance].Value}"]}) {
    /return FALSE
  }
  
  /declare _spawn spawn local
  /if (${NearestSpawn[2, pc radius 55].ID}) {
    DEBUG ${sep}using pc radius 55
    /vardata _spawn NearestSpawn[2, pc radius 55]
  } else /if (${NearestSpawn[1, mercenary radius 55].ID}) {
    DEBUG ${sep}using mercenary radius 55
    /vardata _spawn NearestSpawn[1, mercenary radius 55]
  } else /if (${NearestSpawn[1, npc radius 55].ID}) {
    DEBUG ${sep}using pc radius 55
    /vardata _spawn NearestSpawn[1, npc radius 55]
  }
  
  /if (!${_spawn.ID}) {
    DEBUG ${break}no spawns usable
    /return FALSE
  }

  /if (!${_spawn.CanSplashLand}) {
    DEBUG ${break}splash cannot land
    /return FALSE
  }
  
  /if (${Target.ID} == ${Me.ID}) {
    /invoke ${target[clear]}
  }

  /if (${_spawn.Distance} < 59) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  | should we recast?
  /if (BURN) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAForcefulRejuvenation].Value}"]}) {
      /call cast ${_debug} 0 FALSE
      /delay 4s !${Me.Casting.ID}
    }
  }

/return TRUE



|***
 * note: burn routines
 * "What day is today?" asked Pooh
 * "It's the day we burn this mother fucker to the ground." squeaked Piglet
 * "My favorite day." said Pooh
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

  CHECKEXIT
  GETINPUT clr_burn NA|NA

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_spawn.ID}
    /return TRUE
  }
  
  /if (${maHeal.Find[swChain].Value} && ${maHeal.Find[swChainToken].Value}) {
    /call completeheal_chain ${maDebug.Find[chain].Value}
  }  

  | Group AA Celestial Regeneration
  /if (${maHeal.Find[swHealGroupOverTime].Value} && ${Group.Members} && !${Raid.Members}) {
    /if (!${Bool[${Me.Song[${maChr.Find[stAACelestialRegeneration].Value}]}]} && !${Me.Song[${maChr.Find[stAAFocusedCelestialRegeneration].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAACelestialRegeneration].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }
  }

  | stage 1
  /if ((${Me.AltAbilityReady[${maChr.Find[stAAHealingFrenzy].Value}]} || ${Me.AltAbilityReady[${maChr.Find[stAACelestialRapidity].Value}]}) && !${Me.Buff[${maChr.Find[stAAChannelingtheDivine].Value}].ID} && !${Me.Buff[${maChr.Find[stAAFlurryofLife].Value}].ID}) {

    | Spire 1:30 min duration 7:30 min reuse
    /call cast_AA_spire ${maDebug.Find[spire].Value}

    | anti agro (Silent Casting 1min duration 9 min reuse)
    /if (!${Me.Buff[${maChr.Find[stAADivineRetribution].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }

    | heal power bost: 35% (proc Healing Frenzy, 40sec duration 15 min reuse)
    /if (${cast_data[${_debug}, "${maChr.Find[stAAHealingFrenzy].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

    | - ~50% to cast times [proc:Celestial Rapidity, 1 min duration 10 min reuse]
    /if (${cast_data[${_debug}, "${maChr.Find[stAACelestialRapidity].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

  | stage 2
  } else /if ((${Me.AltAbilityReady[${maChr.Find[stAAChannelingtheDivine].Value}]} || ${Me.AltAbilityReady[${maChr.Find[stAAFlurryofLife].Value}]}) && !${Me.Buff[${maChr.Find[stAAHealingFrenzy].Value}].ID} && !${Me.Buff[${maChr.Find[stAACelestialRapidity].Value}].ID}) {

    | melee block anti agro (20 min reuse)
    /if (!${Me.Song[${maChr.Find[stAASilentCasting].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAADivineRetribution].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }

    | 50% chance to twincast (proc:Channeling of the Divine, 45 sec duration 10 min resuse)
    /if (${cast_data[${_debug}, "${maChr.Find[stAAChannelingtheDivine].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

    | 100% exceptional heal (proc: Flurry of Life, 1:30 min duration 15 min reuse)
    /if (${cast_data[${_debug}, "${maChr.Find[stAAFlurryofLife].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

  }

  | heal ward pet (20 min reuse)
  /if (${cast_data[${_debug}, "${maChr.Find[stAAExquisiteBenediction].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | direct damage boost
  /if (!${Me.Song[${maChr.Find[stAASilentCasting].Value}].ID} && (NUKE || DOT)) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAABattleFrenzy].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

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
    /if (${cast_data[${_debug}, "${maChr.Find[stAADivineArbitration].Value}"]}) {
      /call cast ${_debug} 0 FALSE
      /invoke ${set_timer[${_debug}, timer_arbitration_pause, ${maChr.Find[stArbitratePause].Value}s]}
      /return TRUE
    }
  }

  /if (${maChr.Find[swArbitrateEpic].Value} && !${timer_arbitration_pause}) {

    | Second epic shield 2.0
    /if (${cast_data[${_debug}, "${sqlite.Result[epic${Me.Class.ShortName} 3 name]}"]}) {
      /call cast ${_debug} 0 FALSE
      /invoke ${set_timer[${_debug}, timer_arbitration_pause, ${maChr.Find[stArbitratePause].Value}s]}
      /return TRUE
    }

    | First epic shield 1.5
    /if (${cast_data[${_debug}, "${sqlite.Result[epic${Me.Class.ShortName} 2 name]}"]}) {
      /call cast ${_debug} 0 FALSE
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
    /call cast_cycle_heal_regen ${_debug} ${Me.ID} grouphot
  }

/return TRUE



|***
* note: Complete Heal Chain
* use:
***|
sub completeheal_chain(bool _debug)
  DEBUG \atcompleteheal_chain\ax()

  /if (!AUTO) /return FALSE
  /if (!${maHeal.Find[swChain].Value}) /return FALSE
  /if (!${Me.Gem[Complete Heal]}) /return FALSE
  /if (!${maHeal.Find[swChainToken].Value}) /return FALSE

  DEBUG ${sep}swChainToken${sep}\a-w${maHeal.Find[swChainToken].Value}\ax

  /declare _count int local 0
  /declare _nexthasCH bool local FALSE
  /declare _countMe int local 0
  /declare _countNext int local 0

  | get my position in the line
  /for _count 1 to ${DanNet.PeerCount[clr]}
    /if (${DanNet.Peers[CLR].Arg[${_count},|].Equal[${Me.DisplayName}]}) {
      /varset _countMe ${_count}
      /break
    }
  /next _count

  DEBUG ${sep}_countMe position${sep}\a-w${_countMe}\ax

  | set next cleric in the line. this is just a starting point from me, to move on
  /if (${_countMe} == ${DanNet.PeerCount[clr]}) {
    /varset _countNext 1
  } else {
    /varcalc _countNext ${_countMe} + 1
  }

  | does next cleric in the line have a CH memed?
  | is the cleric in zone?
  | if not, keep the line moving
  /while (${_nexthasCH}) {
    /dquery ${setCHPassedReqs.Arg[${_countNext},|]} -q "Me.Gem[Complete Heal]" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    /if (${Bool[${DNQReturn}]} && ${Spawn[PC ${DanNet.Peers[clr].Arg[${_countNext},|]}].ID}) {
      /varset _nexthasCH TRUE
      /break
    } else {
      | am i last cleric in the line?
      /if (${_countNext} == ${DanNet.PeerCount[clr]}) {
        /varset _countNext 1
      | set to next cleric in the line
      } else {
        /varcalc _countNext ${_countNext} + 1
      }
    }

  }

  | find the bitch to heal
  /declare _spawn spawn local
  /declare _lich listiterator local
  /vardata _lich lsChain.First.Clone
  /while (!${_lich.IsEnd}) {	
    /vardata _spawn Spawn[pc ${_lich.Value}].ID

    DEBUG ${sep}Tank${sep}\aw${_count}\ax${sep}\a-w${_spawn.ID}\ax
    /if (!AUTO || !${maHeal.Find[swChain].Value}) {
      /return FALSE
    }

    | does the tank have a corpse and is there a live version here. ass-u-me it has rez effects and move on
    /if (${_spawn.Dead} || !${_spawn.ID}) {
      /invoke ${_lich.Advance}
      /continue
    }
  }

  | hold the script for whatever time is set in the pause
  /if (${maHeal.Find[stChainPause].Value}) OUT HOLDING${sep}(${num}${maHeal.Find[stChainPause].Value}s\ax) to start casting.
  /delay ${maHeal.Find[stChainPause].Value}s ${_spawn.PctHPs} >= ${maHeal.Find[stPctChain].Value}
  
  | emergency break to release to heal if HP drop below a specific point
  | tell the next cleric to continue CH while you try and patch
  /if (${_spawn.PctHPs} <= ${maHeal.Find[stPctChain].Value}) {
    /invoke ${maHeal.Add[swChainToken,FALSE]}
    /dexecute ${DanNet.Peers[clr].Arg[${_countNext},|]} /invoke ${maHeal.Add[swChainToken,TRUE]}
    /call cast_cycle_heal ${_debug} ${_spawn.ID} single
    /return FALSE
  }
  
  CHECKINPUTS
  CHECKEXIT
  CHECKDEAD

  | set the next cleric in the line to start their heal process
  /dexecute ${DanNet.Peers[clr].Arg[${_countNext},|]} /invoke ${maHeal.Add[swChainToken,TRUE]}
  DEBUG ${sep}\aw${DanNet.Peers[clr].Arg[${_countNext},|]}\ax has next

  | do not want to leave a cleric locked in this any more then needed
  /invoke ${maHeal.Add[swChainToken,FALSE]}
  
  SPAWNDEAD

  | cast the heal
  /call cast_cycle_heal ${_debug} ${_spawn.ID} complete

/return TRUE



|***
 * note: will VP the cleric or QP other people
 * use: 
 ***|
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
      /if (${cast_data[${_debug}, "${maChr.Find[stAAVeturikasPerseverance].Value}"]}) {
        /call cast ${_debug} 0 FALSE
        /return TRUE
      }
    }
  }

  | check group for QP
  /if (!${Group.Members}) {
    /return FALSE
  }

  /if (!${maChr.Find[stPctQP].Value} || !${cast_data[${_debug}, "${maChr.Find[stAAQuietPrayer].Value}"]}) {
    /return FALSE
  }

  | cycle the group. skipping yourself
  /declare _found bool local FALSE
  /declare _count int local 0
  /for _count 0 to ${Group}

    /if (${Group.Member[${_count}].ID} == ${Me.ID}) {
      /continue
    }
    
    /if (!${Group.Member[${_count}].Present}) {
      DEBUG ${break}not present
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

    /if (${Group.Member[${_count}].PctMana} <= ${maChr.Find[stPctVP].Value}*.PCTHO) {
      DEBUG ${sep} mana <= ${maChr.Find[stPctVP].Value}*.PCTHO
      /varset _found TRUE
    }

    /if (${_found}) {  
      /call cast ${_debug} ${Group.Member[${_count}].ID} FALSE
      /return TRUE
    }

  /next _count
  

/return TRUE



|***
 * note: MGB if both AAs are ready
 * use:
 ***|
sub cast_AA_celestialregeneration(bool _debug)
  DEBUG \atcast_AA_celestialregeneration\ax()
 
  /if (${Me.Song[${maChr.Find[stAACelestialRegeneration].Value}].ID}) {
    /return FALSE
  }

  /if (${Me.Song[${maChr.Find[stAAFocusedCelestialRegeneration].Value}].ID}) {
    /return FALSE
  }
    
  /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
    /return FALSE
  }

  /if (!${Me.AltAbilityReady[${maChr.Find[stAACelestialRegeneration].Value}]}) {
    /return FALSE
  }
  
  ISMEDEAD
  CHECKEXIT
  /call cast_data ${_debug} "Mass Group Buff"
  /call cast ${_debug} 0 FALSE
  /delay 5
  /call cast_data ${_debug} "${maChr.Find[stAACelestialRegeneration].Value}"
  /call cast ${_debug} 0 FALSE
  
/return TRUE



|***
 * note: alliance
 * use:
 ***|
sub alliance(bool _debug)
  DEBUG \atalliance\ax()

  /invoke ${set_data_timer[${_debug}, Check_Alliance, RESTART]}
  /declare _tmpSpell string local ${get_best_in_Alliance[${_debug}, "Single", "Heals", "Fc_Cast_Spell_On_Land"]}
  /if (!${cast_data[${_debug}, "${_tmpSpell}"]}) {
    /return FALSE
  }
  
  /if (!${assistData.ID}) {
    /return FALSE
  }

  /call target set ${assistData.ID}

  /delay 2s ${Target.BuffsPopulated}
  /if (${Target.BuffsPopulated}) {
    /if (${Target.Buff[${_tmpSpell}].ID}) /return FALSE
  } else /if (!${Target.BuffsPopulated}) {
    /return FALSE
  }

  /call cast ${_debug} ${assistData.ID} FALSE

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

  | mgb
  } else /if (${_type.Equal[mcr]}) {
    /call cast_AA_celestialregeneration
    /return TRUE

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






#bind control_ch_chain /chain
sub Bind_control_ch_chain(string _type, string _verbage, bool _silent, bool _debug)
  DEBUG \atBind_control_ch_chain\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (!${Select[${Me.Class.ShortName},CLR]}) {
    OUT You are not a cleric. gtfo
    /return FALSE
  }
  
  /if (${_type.Equal[list]}) {
    OUT Tank Lineup${sep}\a-wlsChain\ax
    /call out_list lsChain
    /return TRUE
  }

  | listen for CH Chain rotation calls
  /if (${_type.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swChain, TRUE, ${_verbage}]}

  | % to start the chain cast
  } else /if (${_type.Equal[pct]}) {
    /if (!${set_control_num_range[${_debug}, stPctChain, "${_verbage}", 0, 99]}) /return

  | % to start the chain cast
  } else /if (${_type.Equal[pause]}) {
    /if (!${set_control_num_range[${_debug}, stChainPause, "${_verbage}", 0, 15]}) /return

  }


  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[pct,stPctChain]}
  /invoke ${maControl.Add[active,swChain]}
  /invoke ${maControl.Add[pause,stChainPause]}
  /invoke ${set_control_output[${_debug}, maHeal, chain, maControl]}

/return





|***
 * note: the goal here is to find out who has the fucking token for complete heal
 *   so, if we have it, we should be starting our ch rot if the rest of the conditions 
 *   are met, if not, we are possibly free to do other things.
 * use: dont beat a dead horses...
 ***|
sub find_chain_token()

  /declare _count int local 0
  /declare _toon string local FALSE

  /for _count 1 to ${DanNet.PeerCount[clr]}
    /varset _toon ${DanNet.Peers[CLR].Arg[${_count},|]}
    /if (${_toon.Equal[${Me.DisplayName}]}) {
      
      | if i have the token, we are done here
      /if (${maHeal.Find[swChainToken].Value}) {
        /return ${Me.DisplayName}
      }
    }
    /if (!{Spawn[${_toon}].ID}) {
      /continue
    }

    /dquery ${_toon} -q "maHeal.Find[swChainToken].Value" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    /if (${Bool[${DNQReturn}]}) {
      /return ${_toon}
    }

  /next _count
  
  | no one has it, settin it true
  /invoke ${maHeal.Add[swChainToken,TRUE]}

/return ${Me.DisplayName}









