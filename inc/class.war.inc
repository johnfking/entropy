
|***
 * entropy
 * class.war.inc
 ***|



|***
 * note: primary initilization of all class non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Warlord"
  /call set_data ${_debug} maData stSynergy                           map outer "Myrmidon's Synergy"
  /call set_data ${_debug} maData stFade                              map outer FALSE
  /call set_data_map ${_debug} maChr stAAUnity                        FALSE                         "${auxna} class aa unity buff"

  | attraction AA used for pulling / agro
  /call set_data_map ${_debug} maChr swAttraction                     FALSE                         "${auxsw} use the attraction aa to help with agro/pulling"

  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr stHiatus                         PREGEN|1                      "${auxna} name of hiatus stamina recovery disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"


  | /call set_data_map FALSE maChr stAlliance                         PREGEN|1                      "${auxna} "
  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} dichotomic disc"
  /call set_data_map ${_debug} maChr stRunes                          PREGEN|1                      "${auxna} Aura of Runes Discipline family disc"
  /call set_data_map ${_debug} maChr stKnuckle                        PREGEN|1                      "${auxna} Knuckle Smash family disc"
  /call set_data_map ${_debug} maChr stCommanding                     PREGEN|1                      "${auxna} Commanding Voice family disc"
  /call set_data_map ${_debug} maChr stReprisal                       PREGEN|1                      "${auxna} Determined Reprisal family disc"
  /call set_data_map ${_debug} maChr stEvasive                        PREGEN|1                      "${auxna} Evasive Discipline family disc"
  /call set_data_map ${_debug} maChr stProvoke                        PREGEN|1                      "${auxna} Provoke family disc"
  /call set_data_map ${_debug} maChr stWhirling                       PREGEN|1                      "${auxna} Whirling Blade family disc"
  /call set_data_map ${_debug} maChr stWade                           PREGEN|1                      "${auxna} Wade into Battle family disc"
  /call set_data_map ${_debug} maChr stRallying                       PREGEN|1                      "${auxna} Rallying Roar family disc"
  /call set_data_map ${_debug} maChr stShout                          PREGEN|1                      "${auxna} Harrassing Shout family disc"
  /call set_data_map ${_debug} maChr stDefense                        PREGEN|1                      "${auxna} Bracing Defense family disc"
  /call set_data_map ${_debug} maChr stField                          PREGEN|1                      "${auxna} Field Armorer family disc"

  /call set_data_map ${_debug} maChr stFlash                          PREGEN|1                      "${auxna} Flash of Anger family disc"

  /call set_data_map ${_debug} maChr stNoTime                         PREGEN|1                      "${auxna} No Time to Bleed family disc"
  /call set_data_map ${_debug} maChr stAttention                      PREGEN|1                      "${auxna} Undivided Attention family disc"
  /call set_data_map ${_debug} maChr stPhantom                        PREGEN|1                      "${auxna} Phantom Aggressor family disc"
  /call set_data_map ${_debug} maChr stShield                         PREGEN|1                      "${auxna} Shield Topple family disc"
  /call set_data_map ${_debug} maChr stExpanse                        PREGEN|1                      "${auxna} Harmonious Expanse family disc"
  /call set_data_map ${_debug} maChr stPrecision                      PREGEN|1                      "${auxna} Harmonious Precision family disc"
  /call set_data_map ${_debug} maChr stWarriors                       PREGEN|1                      "${auxna} Warrior's Auspice family disc"
  /call set_data_map ${_debug} maChr stBazu                           PREGEN|1                      "${auxna} Bazu Bellow family disc"
  /call set_data_map ${_debug} maChr stShocking                       PREGEN|1                      "${auxna} Shocking Defense Discipline family disc"
  /call set_data_map ${_debug} maChr stMighty                         PREGEN|1                      "${auxna} Mighty Strike Discipline family disc"
  /call set_data_map ${_debug} maChr stOnslaught                      PREGEN|1                      "${auxna} Savage Onslaught Discipline family disc"
  /call set_data_map ${_debug} maChr stOffensive                      PREGEN|1                      "${auxna} Offensive Discipline family disc"

  /call set_data_map ${_debug} maChr stFortitude                      PREGEN|1                      "${auxna} Fortitude Discipline family disc"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                EMPTY                                     "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 EMPTY                                     "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    taunt|melee|agro|burn|misc|dot|nuke|item|debuff|alliance      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                staminarecover                            "${auxli} class post combat"

  | AAs
  /call set_data_map ${_debug} maChr stAABlastofAnger                 PREGEN|1                      "${auxna} Blast of Anger AA"
  /call set_data_map ${_debug} maChr stAAWarlordsFury                 PREGEN|1                      "${auxna} Warlord's Fury AA"
  /call set_data_map ${_debug} maChr stAAAreaTaunt                    PREGEN|1                      "${auxna} Area Taunt AA"
  /call set_data_map ${_debug} maChr stAARageoftheForsaken            PREGEN|1                      "${auxna} Rage of the Forsaken AA"
  /call set_data_map ${_debug} maChr stAABraceforImpact               PREGEN|1                      "${auxna} Brace for Impact AA"
  /call set_data_map ${_debug} maChr stAAWarStomp                     PREGEN|1                      "${auxna} War Stomp AA"
  /call set_data_map ${_debug} maChr stAAGutPunch                     PREGEN|1                      "${auxna} Gut Punch AA"
  /call set_data_map ${_debug} maChr stAAImperatorsCommand            PREGEN|1                      "${auxna} Imperator's Command AA"
  /call set_data_map ${_debug} maChr stAAKneeStrike                   PREGEN|1                      "${auxna} Knee Strike AA"
  /call set_data_map ${_debug} maChr stAABladeGuardian                PREGEN|1                      "${auxna} Blade Guardian AA"
  /call set_data_map ${_debug} maChr stAAVehementRage                 PREGEN|1                      "${auxna} Vehement Rage AA"
  /call set_data_map ${_debug} maChr stAABattleLeap                   PREGEN|1                      "${auxna} Battle Leap AA"
  /call set_data_map ${_debug} maChr stAAResplendentGlory             PREGEN|1                      "${auxna} Resplendent Glory AA"
  /call set_data_map ${_debug} maChr stAAHoldtheLine                  PREGEN|1                      "${auxna} Hold the Line AA"
  /call set_data_map ${_debug} maChr stAAWarlordsTenacity             PREGEN|1                      "${auxna} Warlords Tenacity AA"
  /call set_data_map ${_debug} maChr stAAWarlordsBravery              PREGEN|1                      "${auxna} Warlords Bravery AA"
  /call set_data_map ${_debug} maChr stAARageofRallosZek              PREGEN|1                      "${auxna} Rage of Rallos Zek AA"
  /call set_data_map ${_debug} maChr stAAMarkoftheMageHunter          PREGEN|1                      "${auxna} Mark of the Mage Hunter AA"
  /call set_data_map ${_debug} maChr stAAWarsSheolsHeroicBlade        PREGEN|1                      "${auxna} Wars Sheol's Heroic Blade AA"

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return



|***
 * note: main warrior routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  GETINPUT
  ISMEDEAD
  CHECKTIE

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase
  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }
  }

  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    /call class_cycle lsOrderClassPre
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
/return



|***
 * note: the things we to to irritate chanters and clerics
 * use:
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT
  CHECKTIE
  CHECKDEFENSE
  BUFFNOW
  BUFFTEMP

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  CHECKDEFENSE

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[misc].Value} ${_spawn.ID}
    /return TRUE
  }

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} disc
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stNoTime].Value}"]} && !${Me.Buff[${maChr.Find[stDichotomic].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }

  | stShocking blocks Brace for Impact
  /if (${cast_data[${_debug}, "${maChr.Find[stShocking].Value}"]} && !${Me.Buff[${maChr.Find[stAABraceforImpact].Value}].ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stRunes].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stDefense].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stProvoke].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stCommanding].Value}"]} && !${Me.Song[${maChr.Find[stCommanding].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAABraceforImpact].Value}"]} && !${Me.ActiveDisc.Name.Equal[${Spell[${maChr.Find[stShocking].Value}].RankName}]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAWarStomp].Value}"]} && !${Me.Song[${maChr.Find[stAABattleLeap].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAGutPunch].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stField].Value}"]} && !${Me.Song[${maChr.Find[stField].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stShield].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAImperatorsCommand].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAKneeStrike].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /invoke ${spawn_face[${_spawn.ID}]}

  | Duplicating these two sub calls here b/c don't want to lose agro or mitigation while firing off this other stuff.
  /if (AGRO) {
    /call get_agro ${maDebug.Find[agro].Value} ${_spawn.ID}
  }

  CHECKDEFENSE

  /if (AE# && ${SpawnCount[npc radius ENV targetable PS4]} >= AE#) {
    /if (${cast_data[${_debug}, "${maChr.Find[stExpanse].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

    /if (${cast_data[${_debug}, "${maChr.Find[stWade].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

  }

  /if (${cast_data[${_debug}, "${maChr.Find[stPrecision].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stReprisal].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }


  /if (${cast_data[${_debug}, "${maChr.Find[stAABladeGuardian].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAVehementRage].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAABattleLeap].Value}"]} && !${Me.Song[${maChr.Find[stAABattleLeap].Value}].ID} && !${Me.Song[Group Bestial Alignment].ID}) {
    /call cast ${_debug} 0 FALSE
  }

/return



|***
 * note:
 * use:
 ***|
sub get_agro(bool _debug, spawn _spawn)
  DEBUG \atget_agro\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  GETINPUT
  CHECKEXIT
  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  /if (ENGINE2) {
    /call cast_cycle_agro ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  /if (AE#) {
    /if (${SpawnCount[npc radius ENV targetable PS4]} >= AE#) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAAreaTaunt].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
      /if (${cast_data[${_debug}, "${maChr.Find[stWhirling].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
      /if (${cast_data[${_debug}, "${maChr.Find[stRallying].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }
  }

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} disc
  }

  /if (${Me.PctAggro} >= ${maAgro.Find[stPctAgroHold].Value}) {
    /return FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stProvoke].Value}"]} && ${Me.TargetOfTarget.ID} != ${Me.ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stShout].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }


  /if (${cast_data[${_debug}, "${maChr.Find[stBazu].Value}"]} && ${Me.TargetOfTarget.ID} != ${Me.ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${Me.TargetOfTarget.Name.NotEqual[${Me.CleanName}]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAARageoftheForsaken].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
      /if (${cast_data[${_debug}, "${maChr.Find[stAABlastofAnger].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
    }
  }

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  | 20% agro boost AA with 2/10 uptime
  /if (${cast_data[${_debug}, "${maChr.Find[stAAWarlordsFury].Value}"]} && ${Me.TargetOfTarget.ID} != ${Me.ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAABlastofAnger].Value}"]} && ${Me.TargetOfTarget.ID} != ${Me.ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

/return



|***
 * note: Self heals
 * use: *see note*
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} disc
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  CHECKREZ
  CHECKFADE

  /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

  | if no one is hurt. back to the start
  /if (!${hurtCount}) {
    SETHUD
    /return TRUE
  }

  /if (!SAFEZONE) {

    | group healing
    /if (${Select[${_type},group]} && ${hurtCount}) {

      | group heal
      /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /call cast_cycle_heal ${_debug} 0 group
      }
    }

  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

  SETHUD

/return TRUE




|***
 * not: burn routines
 * use:
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\x(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT
  CHECKTIE
  CHECKDEFENSE

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

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} disc
  }

  CHECKDEFENSE

  /if (AGRO) {
    /if (${cast_data[${_debug}, "Fortitude Discipline"]}) {
      /stopdisc
      /delay 10 !${Me.ActiveDisc.ID}
      /call cast ${_debug} 0 FALSE

    } else {
      /if (${cast_data[${_debug}, "${maChr.Find[stWarriors].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
      /if (${cast_data[${_debug}, "${maChr.Find[stAAMarkoftheMageHunter].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }

    | Defensive burn routine (AGRO) (but see below re: mitigation).
    /if (${cast_data[${_debug}, "${maChr.Find[stEvasive].Value}"]} && !${Me.ActiveDisc.Name.Equal[Fortitude Discipline]} && (!${Me.Buff[${maChr.Find[stWarriors].Value}].ID} || ${SpawnCount[npc radius ENV targetable PS4]} >= ${maBurn.Find[stBurnCount].Value})) {
      /stopdisc
      /delay 10 !${Me.ActiveDisc.ID}
      /call cast ${_debug} 0 FALSE

    } else {
      /if ((${cast_data[${_debug}, "Fortitude Discipline"]} && ${SpawnCount[npc radius ENV targetable PS4]} >= ${maBurn.Find[stBurnCount].Value}) && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stEvasive].Value}]} && !${Me.Buff[${maChr.Find[stWarriors].Value}].ID}) {
        /stopdisc
        /delay 10 !${Me.ActiveDisc.ID}
        /call cast ${_debug} 0 FALSE

      } else {
        /if (!${Me.ActiveDisc.Name.Equal[${maChr.Find[stEvasive].Value}]} && !${Me.ActiveDisc.Name.Equal[Fortitude Discipline]} && !${Me.Buff[${maChr.Find[stWarriors].Value}].ID}) {
          CHECKDEFENSE
        }
      }
    }

  } else /if (!AGRO) {

    | Offensive burn routine (!AGRO).
    /if (${cast_data[${_debug}, "${maChr.Find[stMighty].Value}"]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOnslaught].Value}]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOffensive].Value}]}) {
      /stopdisc
      /delay 10 !${Me.ActiveDisc.ID}
      /call cast ${_debug} 0 FALSE

    } else {
      /if (${cast_data[${_debug}, "${maChr.Find[stOnslaught].Value}"]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stMighty].Value}]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOffensive].Value}]}) {
        /stopdisc
        /delay 10 !${Me.ActiveDisc.ID}
        /call cast ${_debug} 0 FALSE

      } else {
        /if (${cast_data[${_debug}, "${maChr.Find[stOffensive].Value}"]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stMighty].Value}]} && !${Me.ActiveDisc.Name.Equal[${maChr.Find[stOnslaught].Value}]}) {
          /stopdisc
          /delay 10 !${Me.ActiveDisc.ID}
          /call cast ${_debug} 0 FALSE

        }
      }
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAttention].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stPhantom].Value}"]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${Me.AltAbilityReady[${maChr.Find[stAAResplendentGlory].Value}]} && !${Me.Song[${maChr.Find[stAAHoldtheLine].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAResplendentGlory].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } else /if (${Me.AltAbilityReady[${maChr.Find[stAAHoldtheLine].Value}]} && !${Me.Song[${maChr.Find[stAAResplendentGlory].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAHoldtheLine].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAAWarlordsTenacity].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAAWarlordsBravery].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAWarsSheoolsHeroicBlade].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

  /if (${cast_data[${_debug}, "${maChr.Find[stAARageofRallosZek].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

/return TRUE




|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", "${_verbage2}"\ax)

  | attraction
  /if (${_type.Equal[attraction]}) {
    /invoke ${set_switch_env[${_debug}, swAttraction, TRUE, ${_verbage}]}

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE


