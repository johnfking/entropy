|***
 * entropy
 * class.pal.inc
 ***|



|***
 * note: primary initilization of all paladin non shared data
 * use: are you a paladin?
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of Chivalry"
  /call set_data ${_debug} maData stSynergy                           map outer "Knight's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Balefire Burst"

  | attraction AA used for pulling / agro
  /call set_data_map ${_debug} maChr swAttraction                     FALSE                         "${auxsw} use the attraction aa to help with agro/pulling"

  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr stHiatus                         PREGEN|1                      "${auxna} name of hiatus stamina recovery disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"

  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell .. damn useless thing"

  | heals
  /call set_data_map ${_debug} maHeal stCountSplash                   0                             "${auxnum} number mobs in range to use a splash heal" heal

  | harmonions
  /invoke ${set_data_timer[${_debug}, Check_Harmonious, 1m]}
  /call set_data_map ${_debug} maChr swHarmonious                     FALSE                         "${auxsw} use harmonious"
  /call set_data_map ${_debug} maChr stHarmonious                     PREGEN|1                      "${auxna} name of the harmonions spell"

  | Marrs Gift
  /invoke ${set_data_timer[${_debug}, Check_Marrs_Gift, 10s]}
  /call set_data_map ${_debug} maChr stPctMG                          0                             "${auxnum} % of mana to use mars gift aa"

  | /call set_data_map ${_debug} maChr stTempRune                       PREGEN|1                      "${auxna} fast reuse disc rune"
  /call set_data_map ${_debug} maChr stChallenge                      PREGEN|1                      "${auxna} challenge spell for agro"
  /call set_data_map ${_debug} maChr stValiant                        PREGEN|1                      "${auxna} vlaiant spell for defensive"
  /call set_data_map ${_debug} maChr stUnflinching                    PREGEN|1                      "${auxna} name of unflinching disc to use for agro help"
  /call set_data_map ${_debug} maChr stReflexive                      PREGEN|1                      "${auxna} name of reflexive disc"
  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} dichotomic name"

  | lay on hands
  /call set_data_map FALSE maChr stPctLoH                             0                             "${auxnum} % of mana to use aa lay on hands. requires self healing to be enabled"

  | yaulp
  /invoke ${set_data_timer[${_debug}, Check_Yaulp, 6s]}
  /call set_data_map ${_debug} maChr swYaulp                          FALSE                         "${auxsw} use yaulp"
  /call set_data_map ${_debug} maChr stYaulp                          PREGEN|1                      "${auxna} name of yaulp to use"

  | aa stun
  /call set_data_map ${_debug} maChr swStun                           FALSE                         "${auxsw} use AA stun"
  /call set_data_map ${_debug} maChr stStun                           FALSE                         "${auxna} name of AA stun to use"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                xtarget                                                                       "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 heal|manarecover|staminarecovery                                              "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    taunt|stun|agro|melee|heal|stun|burn|misc|dot|nuke|item|taunt|stun|agro|debuff|alliance|harmonious      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|staminarecovery                                                   "${auxli} class post combat "

  | AAs
  /call set_data_map ${_debug} maChr stAAArmoroftheInquisitor         PREGEN|1                      "${auxna} Armor of the Inquisitor AA"
  /call set_data_map ${_debug} maChr stAAGroupArmoroftheInquisitor    PREGEN|1                      "${auxna} Group Armor of the Inquisitor AA"
  /call set_data_map ${_debug} maChr stAAVanquishtheFallen            PREGEN|1                      "${auxna} Vanquish the Fallen AA"
  /call set_data_map ${_debug} maChr stAALayonHands                   PREGEN|1                      "${auxna} Lay on Hands AA"
  /call set_data_map ${_debug} maChr stAAGiftofLife                   PREGEN|1                      "${auxna} Gift of Life AA"
  /call set_data_map ${_debug} maChr stAAHallowedLodestar             PREGEN|1                      "${auxna} Hallowed Lodestar AA"
  /call set_data_map ${_debug} maChr stAABeaconoftheRighteous         PREGEN|1                      "${auxna} Beacon of the Righteous AA"
  /call set_data_map ${_debug} maChr stAAProjectionofPiety            PREGEN|1                      "${auxna} Projection of Piety AA"
  /call set_data_map ${_debug} maChr stAAAgelessEnmity                PREGEN|1                      "${auxna} Ageless Enmity AA"
  /call set_data_map ${_debug} maChr stAAMarrsGift                    PREGEN|1                      "${auxna} Marr's Gift AA"
  /call set_data_map ${_debug} maChr stAAValorousRage                 PREGEN|1                      "${auxna} Valorous Rage AA"
  /call set_data_map ${_debug} maChr stAADeflectionDiscipline         PREGEN|1                      "${auxna} Deflection Discipline AA"
  /call set_data_map ${_debug} maChr stAAInquisitorsJudgmen           PREGEN|1                      "${auxna} Inquisitor's Judgment AA"
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                      "${auxna} Class Unity AA"
  /call set_data_map ${_debug} maChr stAAHandofPiety                  PREGEN|1                      "${auxna} Hand of Piety AA"


  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: Main Paladin Routine
 * use: primarly for making unicorn jokes at. also useful for throwing at raid mobs as speed bumps.
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase

    /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
      /call cast_shield_flash FALSE aa
    }

    | aa LoH
    /if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
      /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAALayonHands].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      }
    }

    /if (!${timer_Check_Marrs_Gift} && ${maChr.Find[stPctMG].Value} && ${Me.PctMana} <= ${maChr.Find[stPctMG].Value}) {
      /call cast_AA_marrs_gift
    }
  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }
  }

  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    /call class_cycle lsOrderClassPre
    /if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp FALSE
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
 * note: all other combat .. things
 * use: /call misc_combat DEBUG [spawn]
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD
  CHECKEXIT
  GETINPUT

  /call check_heal_routines
  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[minion].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  SPAWNDEAD

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash FALSE aa
  }

  | AA inquisitors armor stacking control
  /if (${SpawnCount[npc OVERLOS radius 30 targetable]} >= ${maDefense.Find[stCountDef].Value} && ${maDefense.Find[stCountDef].Value}) {
    /if (!${Me.Song[${maChr.Find[stAAGroupArmoroftheInquisitor].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAArmoroftheInquisitor].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }

    | dont use group armor in raids
    } else /if (!${Me.Song[${maChr.Find[stAAArmoroftheInquisitor].Value}].ID} && !${Raid.Members}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAGroupArmoroftheInquisitor].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }
  }

  SPAWNDEAD

  | undead cripple
  /if (${Target.Body.Name.Equal[Undead]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAVanquishtheFallen].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

/return TRUE



|***
 * note: I'm more important then you are!
 * use: /call get_agro DEBUG [spawn]
 ***|
sub get_agro(bool _debug, spawn _spawn)
  DEBUG \atget_agro\ax(\a-w${_spawn.ID}\ax)

  CHECKEXIT
  GETINPUT
  SPAWNDEAD

  /if (ENGINE2) {
    /call cast_cycle_agro ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  | /call set_combat ${_debug}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash FALSE aa
  }

  /call cast_challenge ${_debug} ${_spawn.ID}

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  | chain stun
  /if (${cast_data[${_debug}, "${maChr.Find[stStun].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  | Valiant Defense line. agro+heal
  /if (${cast_data[${_debug}, "${maChr.Find[stValiant].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }  

  | AE Stuns
  /if (AE#) {
    /if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= AE#) {
      /declare _coutAE int local 0
      /varset _coutAE ${SpawnCount[npc OVERLOS radius 60 PS4 targetable]}

      /if (!${Raid.Members} || ${Raid.Members} && ${Raid.MainAssist.Name.Equal[${Me.DisplayName}]}) {
        /if (${_coutAE} > 1) {
          /if (${cast_data[${_debug}, "${maChr.Find[stAAHallowedLodestar].Value}"]}) {
            /call cast ${_debug} ${_spawn.ID} FALSE
          }
        }
        /if (${_coutAE} > 4) {
          /if (${cast_data[${_debug}, "${maChr.Find[stAABeaconoftheRighteous].Value}"]}) {
            /call cast ${_debug} 0 ${_spawn.ID}
          }
        }
      }
    }
  }
  
  SPAWNDEAD

  /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
    /return
  }

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  /if (${Me.PctAggro} >= ${maAgro.Find[stPctAgroHold].Value}) {
    /return FALSE
  }

  | AA pet hate (the little sword thingie)
  /if (${cast_data[${_debug}, "${maChr.Find[stAAProjectionofPiety].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  | snap hate
  /if (!${Raid.Members}) {
    /if (${Me.PctAggro} < ${maAgro.Find[stPctAgroHold].Value} && ${cast_data[${_debug}, "${maChr.Find[stAAAgelessEnmity].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  | named fake hate
  /if ((${Me.PctAggro} < ${maAgro.Find[stPctAgroHold].Value} || ${lsZoneNamed.Contains[${Target.DisplayName}]}) && ${cast_data[${_debug}, "${maChr.Find[stUnflinching].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }


/return TRUE



|***
 * note: Palading group heal. centered more to splashing
 * use:
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal_group\ax(\a-w${_type}\ax)

  CHECKTIE
  CHECKEXIT

  SETHUD "heal ${_type}"

  /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

  /if (!${hurtCount}) {
    SETHUD
    /return FALSE
  }

  | splash heal
  /if (${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
    /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
      /call Bind_command_splash ${maDebug.Find[splash].Value}
    }
  }

  /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

  | ToT heals
  /if (${Select[${_type},tot]} && ${maHeal.Find[swHealToT].Value}) {

    /if (${hurtCount} >= 1)  {

      | aa LoH targets target
      /if (${maChr.Find[stPctLoH].Value}) {
        /if (${Me.TargetOfTarget.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
          /if (${cast_data[${maDebug.Find[tot].Value}, "${maChr.Find[stAALayonHands].Value}"]}) {
            /call cast ${maDebug.Find[tot].Value} ${Target.ID} FALSE
            /return TRUE
          }
        }
      }

      /call cast_cycle_heal ${_debug} ${Target.ID} tot

      /return FALSE
    }
  }

  | group only heals
  /if (${Select[${_type},group]} && ${Group}) {

    | splash heal
    /if (${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
      /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
        /call Bind_command_splash ${maDebug.Find[splash].Value}
      }
    }

    /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }

    | group heal
    /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
      /call cast_cycle_heal ${_debug} 0 group
    }

    | Hand of Piety AA Group heal
    /if (${hurtCount} >= 1 || BURN) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAHandofPiety].Value}"]}) {
        /call cast ${_debug} 0 FALSE
        /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
      }
    }

    /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }

    | heal disc
    /if (${hurtCount} >= 1) {
      /if (${cast_data[${_debug}, "${maChr.Find[stReflexive].Value}"]}) {
        /call cast ${_debug} TARGET FALSE
        /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
      }
    }

  }

  | self only heals
  /if (${Select[${_type},self]} && ${maHeal.Find[swHealSelf].Value}) {
    | aa LoH
    /if (${maChr.Find[stPctLoH].Value}) {
      /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAALayonHands].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      }
    }

    /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }
  }

  | AA Gift of Life -- not in raids
  /if (${Me.PctHPs} < ${maHeal.Find[stHealPointPAL].Value}*.30*.PCTHO && !${Raid.Members}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAGiftofLife].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

  SETHUD

/return TRUE



|***
 * note: Mar's Gift 30ish k HP/15k manna
 * use: /call cast_AA_marrs_gift [DEBUG]
 ***|
sub cast_AA_marrs_gift(bool _debug)
  DEBUG \atcast_AA_marrs_gift\ax()

  CHECKEXIT
  GETINPUT

  /if (SAFEZONE) {
    /return FALSE
  }

  /invoke ${set_data_timer[FALSE, Check_Marrs_Gift, RESTART]}

  /if (!${cast_data[${_debug}, "${maChr.Find[stAAMarrsGift].Value}"]}) {
    /return FALSE
  }

  /call cast ${_debug} 0 FALSE

/return



|***
 * note: Paladin burn routines (like pallys can do any DPS.!)
 * use: seriously why bother
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD
  CHECKEXIT
  GETINPUT

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

  | deflection
  /if (!${Bool[${Me.ActiveDisc}]} && ${Me.PctHPs} < 50*.PCTHO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAADeflectionDiscipline].Value}"]}) {
      /call cast ${_debug} 0 FALSE
      /return
    }
  }

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash FALSE aa
  }

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  | aa LoH
  /if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
    /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAALayonHands].Value}"]}) {
        /call cast ${_debug} ${Me.ID} FALSE
      }
    }
  }

  SPAWNDEAD

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (${cast_data[${_debug}, "${maChr.Find[stAAInquisitorsJudgemen].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAAValorousRage].Value}"]]}) {
    /call cast ${_debug} 0 FALSE
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 * note: AA stun
 * use: /chr stun to select
 ***|
sub cast_AA_stun(bool _debug, spawn _spawn)
  DEBUG \atcast_AA_stun\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD
  CHECKEXIT
  GETINPUT
  
  /if (${cast_data[${_debug}, "${maChr.Find[stStun].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", "${_verbage2}"\ax)

  | to Yaulp or not
  /if (${_type.Equal[yaulp]}) {
    /invoke ${set_switch_env[${_debug}, swYaulp, TRUE, ${_verbage}]}

  | Stun Control
  } else /if (${_type.Equal[stun]}) {

    /declare _maStun map local
    /invoke ${_maStun.Add[divine,Divine Stun]}
    /invoke ${_maStun.Add[force,Force of Disruption]}
    /invoke ${_maStun.Add[disruptive,Disruptive Persecution]}

    /if (${_verbage.Equal[use]}) {
      /invoke ${set_switch_env[FALSE, swStun, TRUE, ${_verbage2}]}
    } else /if (${_maStun.Contains[${_verbage}]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stStun, "${_maStun.Find[${_verbage}].Value}"]}
    }

    /declare _out string local
    /declare _mistun mapiterator local
    /vardata _mistun _maStun.First.Clone
    OUT /chr stun [\a-wuse\ax|\a-wdivine\ax|\a-wforce\ax|\a-wdisruptive\ax]
    /varset _out ${_out} \awuse\ax:${If[${maChr.Find[swStun].Value},${on},${off}]} ${dot}
    /while (!${_mistun.IsEnd}) {
      /varset _out ${_out} ${If[${_maStun.First.Value.Equal[${_mistun.Value}]},,${dot}]} ${If[${maChr.Find[stStun].Value.Equal[${_mistun.Value}]},${good}${_mistun.Value}\ax,${info}${_mistun.Value}\ax]}
      /invoke ${_mistun.Advance}
    }
    OUT /chr stun ${_out}
    /return

  | shield flash
  } else /if (${_type.Equal[flash]}) {

    /if (${_verbage.Equal[pct]}) {
      /if (!${set_control_num_range[${_debug}, stPctShieldFlash, "${_verbage2}", 0, 99]}) /return
    } else /if (${_verbage.Equal[count]}) {
      /if (!${set_control_num_range[${_debug}, stCountShieldFlash, "${_verbage2}", 0, 99]}) /return
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
    /invoke ${set_switch_env[${_debug}, swHarmonious, TRUE, ${_verbage}]}

  | attraction
  } else /if (${_type.Equal[attraction]}) {
    /invoke ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}

  /if (${Macro.Return.Equal[SKIP]}) /return FALSE
  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE
