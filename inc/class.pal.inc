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

  | temp combat buffs
  /call set_data_map ${_debug} maChr stStance                         PREGEN|1                      "${auxna} name of the self stance buff"
  /call set_data_map ${_debug} maChr stSteel                          PREGEN|1                      "${auxna} name of the steel self buff"

  | harmonions
  /invoke ${set_data_timer[${_debug}, Check_Harmonious, 1m]}
  /call set_data_map ${_debug} maChr swHarmonious                     FALSE                         "${auxsw} use harmonious"
  /call set_data_map ${_debug} maChr stHarmonious                     PREGEN|1                      "${auxna} name of the harmonions spell"

  | Marrs Gift
  /invoke ${set_data_timer[${_debug}, Check_Marrs_Gift, 10s]}
  /call set_data_map ${_debug} maChr stPctMG                          0                             "${auxnum} % of mana to use mars gift aa"

  /call set_data_map ${_debug} maChr stTempRune                       PREGEN|1                      "${auxna} fast reuse disc rune"
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

  GETINPUT
  ISMEDEAD
  CHECKTIE
  CHECKREZ

  /if (AUTO && !ENGINE3) {
    /if (${maHeal.Find[swHealXTarget].Value} && !${timer_Check_Xtarget_Build}) {
      /call check_xtarget_build FALSE
    }

    /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
      /call cast_shield_flash FALSE aa
    }

    | aa LoH
    /if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
      /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
        /if (${is_ready[${_debug}, "${maChr.Find[stAALayonHands].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAALayonHands].Value}" ${Me.ID} FALSE
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
    /call check_heal_routines
    /if (!${timer_Check_Yaulp} && ${maChr.Find[swYaulp].Value}) /call cast_yaulp FALSE
    /if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

    /if (VALID && !ENGINE3) {
      /call set_combat_control ${maDebug.Find[decision].Value} TARGET
      /invoke ${do_taunt[${_debug}, TARGET]}
      /if (MELEE) /call cast_cycle_melee ${maDebug.Find[melee].Value} TARGET
      /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
        /call cast_shield_flash FALSE aa
      }
      /if (AGRO) /call get_agro ${maDebug.Find[agro].Value} TARGET
      /call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
      /call check_heal_routines
      /if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
      /call misc_combat ${maDebug.Find[misc].Value} TARGET
      /if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
      /if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
      /call check_item_cycle ${maDebug.Find[item].Value} TARGET
      /if (!${timer_Check_deBuffs}) /call check_debuff_routines TARGET
      | /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
      /if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) {
        /call check_harmonious ${maDebug.Find[harmonious].Value}
      }

      /if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
        /call check_buff_cycle_temp_self ${maDebug.Find[temp].Value} stSteel
        /call check_buff_cycle_temp_self ${maDebug.Find[temp].Value} stStance
      }

    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

    /if (AUTO && !ENGINE3) {
      /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
      CHECKFADE
      /if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[rest].Value}
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }
  }

/return TRUE



|***
 * note: all other combat .. things
 * use: /call misc_combat DEBUG [TARGETID]
 ***|
sub misc_combat(bool _debug, int _tmpID)
  DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)

  ISMEDEAD

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT
  CHECKTIE
  CHECKDEFENSE
  BUFFNOW
  BUFFTEMP
  /if (!PULL) {
    /call get_assist_target ${maDebug.Find[target].Value}
    /if (TARGET != ${_tmpID}) /varset _tmpID TARGET
  }

  /call check_heal_routines
  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[minion].Value} ${_tmpID}
  }
  /if (AUTO && MELEE) {
    /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
  }

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
    /return TRUE
  }

  | spam rune
  /if (${is_ready[${_debug}, "${maChr.Find[stTempRune].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stTempRune].Value}" 0 FALSE
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /invoke ${do_taunt[${_debug}, ${_tmpID}]}

  /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash FALSE aa
  }

  | AA inquisitors armor stacking control
  /if (${SpawnCount[npc OVERLOS radius 30 targetable]} >= ${maDefense.Find[stCountDef].Value} && ${maDefense.Find[stCountDef].Value}) {
    /if (!${Me.Song[${maChr.Find[stAAGroupArmoroftheInquisitor].Value}].ID}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAArmoroftheInquisitor].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAArmoroftheInquisitor].Value}" 0 FALSE
      }

    | dont use group armor in raids
    } else /if (!${Me.Song[${maChr.Find[stAAArmoroftheInquisitor].Value}].ID} && !${Raid.Members}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAGroupArmoroftheInquisitor].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAGroupArmoroftheInquisitor].Value}" 0 FALSE
      }
    }
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  | undead cripple
  /if (${Target.Body.Name.Equal[Undead]}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAVanquishtheFallen].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAVanquishtheFallen].Value}" ${_tmpID} FALSE
    }
  }

/return TRUE



|***
 * note: I'm more important then you are!
 * use: /call get_agro DEBUG [TARRGETID]
 ***|
sub get_agro(bool _debug, int _tmpID)
  DEBUG \atget_agro\ax()

  ISMEDEAD
  CHECKEXIT
  GETINPUT
  CHECKDEFENSE

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (ENGINE2) {
    /call cast_cycle_agro ${maDebug.Find[agro].Value} ${_tmpID}
    /return TRUE
  }

  /call set_combat ${_debug}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash FALSE aa
  }

  /call cast_challenge ${_debug} ${_tmpID}

  /invoke ${do_taunt[${_debug}, ${_tmpID}]}

  | chain stun
  /if (${is_ready[${_debug}, "${maChr.Find[stStun].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stStun].Value}" ${_tmpID} FALSE
  }


  | AE Stuns
  /if (AE#) {
    /if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= AE#) {
      /declare _coutAE int local 0
      /varset _coutAE ${SpawnCount[npc OVERLOS radius 60 PS4 targetable]}

      /if (!${Raid.Members} || ${Raid.Members} && ${Raid.MainAssist.Name.Equal[${Me.DisplayName}]}) {
        /if (${_coutAE} > 1) {
          /if (${is_ready[${_debug}, "${maChr.Find[stAAHallowedLodestar].Value}"]}) {
            /call voodoo ${_debug} "${maChr.Find[stAAHallowedLodestar].Value}" ${_tmpID} FALSE
          }
        }
        /if (${_coutAE} > 4) {
          /if (${is_ready[${_debug}, "${maChr.Find[stAABeaconoftheRighteous].Value}"]}) {
            /call voodoo ${_debug} "${maChr.Find[stAABeaconoftheRighteous].Value}" 0 ${_tmpID}
          }
        }
      }
    }
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
    /return
  }

  /invoke ${do_taunt[${_debug}, ${_tmpID}]}

  /if (${Me.PctAggro} >= ${maAgro.Find[stPctAgroHold].Value}) {
    /return FALSE
  }

  | AA pet hate (the little sword thingie)
  /if (${is_ready[${_debug}, "${maChr.Find[stAAProjectionofPiety].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAProjectionofPiety].Value}" ${_tmpID} FALSE
  }

  | snap hate
  /if (!${Raid.Members}) {
    /if (${Me.PctAggro} < ${maAgro.Find[stPctAgroHold].Value} && ${is_ready[${_debug}, "${maChr.Find[stAAAgelessEnmity].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAAgelessEnmity].Value}" ${_tmpID} FALSE
    }
  }

  | named fake hate
  /if ((${Me.PctAggro} < ${maAgro.Find[stPctAgroHold].Value} || ${lsZoneNamed.Contains[${Target.DisplayName}]}) && ${is_ready[${_debug}, "${maChr.Find[stUnflinching].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stUnflinching].Value}" ${_tmpID} FALSE
  }

  | Valiant Defense line. agro+heal
  /if (${Me.PctAggro} < 98 && ${is_ready[${_debug}, "${maChr.Find[stValiant].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stValiant].Value}" ${_tmpID} FALSE
  }

/return TRUE



|***
 * note: Palading group heal. centered more to splashing
 * use:
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal_group\ax(\a-w${_type}\ax)

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  CHECKREZ
  CHECKDEFENSE

  SETHUD "heal ${_type}"

  /call get_hurt ${maDebug.Find[hurt].Value} ${_type}

  /if (!${hurtCount}) {
    SETHUD EMPTY
    /return FALSE
  }

  | splash heal
  /if (${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
    /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
      /call Bind_command_splash ${maDebug.Find[splash].Value}
    }
  }

  /call get_hurt ${maDebug.Find[hurt].Value} ${_type}

  | ToT heals
  /if (${Select[${_type},tot]} && ${maHeal.Find[swHealToT].Value}) {

    /if (${hurtCount} >= 1)  {

      | aa LoH targets target
      /if (${maChr.Find[stPctLoH].Value}) {
        /if (${Me.TargetOfTarget.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
          /if (${is_ready[${maDebug.Find[tot].Value}, "${maChr.Find[stAALayonHands].Value}"]}) {
            /call voodoo ${maDebug.Find[tot].Value} "${maChr.Find[stAALayonHands].Value}" ${Target.ID} FALSE
            /return TRUE
          }
        }
      }

      /call cast_cycle_heal ${_debug} ${Target.ID} tot

      /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
    }
  }

  | group only heals
  /if (${Select[${_type},group]} && ${Group}) {
    | Group Splash

    | splash heal
    /if (${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
      /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
        /call Bind_command_splash ${maDebug.Find[splash].Value}
      }
    }

    /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }

    | group heal
    /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
      /call cast_cycle_heal ${_debug} 0 group
    }

    | Hand of Piety AA Group heal
    /if (${hurtCount} >= 1 || BURN) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAHandofPiety].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAHandofPiety].Value}" 0 FALSE
        /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
      }
    }

    /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
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
        /if (${is_ready[${_debug}, "${maChr.Find[stAALayonHands].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAALayonHands].Value}" ${Me.ID} FALSE
        }
      }
    }

    /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }
  }

  | AA Gift of Life -- not in raids
  /if (${Me.PctHPs} < ${maHeal.Find[stHealPointPAL].Value}*.30*.PCTHO && !${Raid.Members}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAGiftofLife].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAGiftofLife].Value}" 0 FALSE
    }
  }

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  CHECKREZ
  CHECKFADE

  /call cast_cycle_heal ${_debug} ${hurtID} single

  SETHUD EMPTY

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

  /if (SAFEZONE) {
    /return FALSE
  }

  /invoke ${set_data_timer[FALSE, Check_Marrs_Gift, RESTART]}

  /if (!${is_ready[${_debug}, "${maChr.Find[stAAMarrsGift].Value}"]}) {
    /return FALSE
  }

  /call voodoo ${_debug} "${maChr.Find[stAAMarrsGift].Value}" 0 FALSE

/return



|***
 * note: Paladin burn routines (like pallys can do any DPS.!)
 * use: seriously why bother
 ***|
sub burn(bool _debug, int _tmpID)
  DEBUG \atburn\ax(\a-w${_tmpID}\ax)

  ISMEDEAD

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT
  CHECKTIE
  CHECKDEFENSE
  /if (!PULL) {
    /call get_assist_target ${maDebug.Find[target].Value}
    /if (TARGET != ${_tmpID}) /varset _tmpID TARGET
  }

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} TARGET
  }
  /if (AUTO && MELEE) {
    /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
    /return TRUE
  }


  | deflection
  /if (!${Bool[${Me.ActiveDisc}]} && ${Me.PctHPs} < 50*.PCTHO) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAADeflectionDiscipline].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAADeflectionDiscipline].Value}" 0 FALSE
      /return
    }
  }

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash FALSE aa
  }

  /invoke ${do_taunt[${_debug}, ${_tmpID}]}

  | aa LoH
  /if (${maHeal.Find[swHealSelf].Value} && ${maChr.Find[stPctLoH].Value}) {
    /if (${Me.PctHPs} <= ${maChr.Find[stPctLoH].Value}*.PCTHO) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAALayonHands].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAALayonHands].Value}" ${Me.ID} FALSE
      }
    }
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (${is_ready[${_debug}, "${maChr.Find[stAAInquisitorsJudgemen].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAInquisitorsJudgemen].Value}" 0 FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stAAValorousRage].Value}"]]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAValorousRage].Value}" 0 FALSE
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

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
