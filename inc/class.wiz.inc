|***
 * entropy
 * class.wiz.inc
 ***|



|***
 * note: primary initilization of all wizard non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of Arcanum"
  /call set_data ${_debug} maData stSynergy                           map outer "Evoker's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "A Hole in Space"

  /call set_data_map ${_debug} maChr stPctGather                      0                             "${auxnum} % of your mana to start using gather spells and AAs"
  /call set_data_map ${_debug} maChr stGather                         PREGEN|1                      "${auxna} name of mana gather spell (\a-wNOT THE AA\ax)"
  /call set_data_map ${_debug} maChr stTempPet                        PREGEN|1                      "${auxna} temp flame'ie pig sticker pet spell"

  /call set_data_map ${_debug} maChr swManaBurn                       FALSE                         "${auxsw} use aa manaburn during burns"

  /call set_data_map ${_debug} maChr stConcussion                     PREGEN|1                      "${auxna} name of anti agro spell cause you nuke to damn much and dont let tank get agro"
  /call set_data_map ${_debug} maChr stPctConcussion                  0                             "${auxnum} % of your agro you want to try and cover your ass at"

  | current nuke line
  /call set_data ${_debug} FALSE lsNukeLine                           list outer base|fire|ice|magic
  /call set_data_map ${_debug} maChr stCurrent                        base
  /call set_data ${_debug} FALSE stLine                               string outer ${maChr.Find[stCurrent].Value.Left[1].Upper}${maChr.Find[stCurrent].Value.Right[-1].Lower}

  /invoke ${set_data_timer[FALSE, Force_TC, 6s]}
  /call set_data_map ${_debug} maChr stTC                             PREGEN|1                      "${auxna} whatever the hell you want to use for twincast"
  /call set_data_map ${_debug} maChr swForceTC                        FALSE                         "${auxsw} force using twincast aa/spells as much as possible"
  /call set_data_map ${_debug} maChr stNukeTCFire                     PREGEN|1                      "${auxna} fire line twincast spell"
  /call set_data_map ${_debug} maChr stNukeTCIce                      PREGEN|1                      "${auxna} ice line twincast spell"
  /call set_data_map ${_debug} maChr stNukeTCMagic                    PREGEN|1                      "${auxna} magic line twincast spell"
  /call set_data_map ${_debug} maChr stNukeTCBase                     FALSE                         "${auxna} base line twincast spell"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                EMPTY                                     "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 EMPTY                                     "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|debuff|burn|pet|swarm|alliance|nuke|misc|item      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|gather                        "${auxli} class post combat"

  | AAs
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                      "${auxna} Class Unity aa"
  /call set_data_map ${_debug} maChr stAAArcaneWhisper                PREGEN|1                      "${auxna} Arcane Whisper aa"
  /call set_data_map ${_debug} maChr stAAMindCrash                    PREGEN|1                      "${auxna} Mind Crash aa"
  /call set_data_map ${_debug} maChr stAAConcussiveIntuition          PREGEN|1                      "${auxna} Concussive Intuition aa"
  /call set_data_map ${_debug} maChr stAAHarvestofDruzzil             PREGEN|1                      "${auxna} Harvest of Druzzil aa"
  /call set_data_map ${_debug} maChr stAASilentCasting                PREGEN|1                      "${auxna} Silent Casting aa"
  /call set_data_map ${_debug} maChr stAAFrenziedDevastation          PREGEN|1                      "${auxna} Frenzied Devastation aa"
  /call set_data_map ${_debug} maChr stAAFocusofArcanum               PREGEN|1                      "${auxna} Focus of Arcanum aa"
  /call set_data_map ${_debug} maChr stAAImprovedTwincast             PREGEN|1                      "${auxna} Improved Twincast aa"
  /call set_data_map ${_debug} maChr stAAFuryoftheGods                PREGEN|1                      "${auxna} Fury of the Gods aa"
  /call set_data_map ${_debug} maChr stAAArcaneDestruction            PREGEN|1                      "${auxna} Arcane Destruction aa"
  /call set_data_map ${_debug} maChr stAAArcaneFury                   PREGEN|1                      "${auxna} Arcane Fury aa"
  /call set_data_map ${_debug} maChr stAACallofXuzl                   PREGEN|1                      "${auxna} Call of Xuzl aa"
  /call set_data_map ${_debug} maChr stAAManaBurn                     PREGEN|1                      "${auxna} Mana Burn aa"


  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: main wizzeurd routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

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
 * use: /call misc_combat_${Me.Class.ShortName} DEBUG TARGETID
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

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[misc].Value} ${_spawn.ID}
    /return TRUE
  }

  | agro control
  /if (AGRO) {
  } else /if (!AGRO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAArcaneWhisper].Value}"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
    /if (${cast_data[${_debug}, "${maChr.Find[stAAMindCrash].Value}"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value} && ${lsZoneNamed.Contains[${Spawn[TARGET].DisplayName}]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
    /if (${cast_data[${_debug}, "${maChr.Find[stAAConcussiveIntuition].Value}"]} && ${Me.PctAggro} > ${maChr.Find[stPctFade].Value}) {
      call cast ${_debug} ${_spawn.ID} FALSE
    }
    /if (${cast_data[${_debug}, "${maChr.Find[stConcussion].Value}"]} && ${Me.PctAggro} > ${maChr.Find[stPctConcussion].Value}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

/return TRUE



|***
 * note: But you say I'm a dog when I leave you fucked and deserted.
 * use: /call check_harvest DEBUG
 ***|
sub check_gather(bool _debug)
  DEBUG \atcheck_gather\ax()

  CHECKEXIT
  GETINPUT
  CHECKTIE

  /if (${Me.Buff[Improved Twincast].ID}) {
    /return FALSE
  }

  /if (${Bool[${maChr.Find[stTC].Value}]} && ${Me.Buff[${maChr.Find[stTC].Value}].ID}) {
    /return FALSE
  }

  /if (!${cast_data[${_debug}, "${maChr.Find[stGather].Value}"]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAForcefulRejuvenation].Value}"]}) {
      /call cast ${_debug} 0 FALSE
      /delay 5
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stGather].Value}"]} && ${Me.PctMana} <= ${maChr.Find[stPctGather].Value}) {
    /if (${Spell[${Spell[${maChr.Find[stGather].Value}].RankName}].Base[1]} <= ${Math.Calc[${Me.MaxMana}-${Me.CurrentMana}].Int} && ${Me.CurrentMana} != ${Me.MaxMana}) {
      /call cast ${_debug} 0 FALSE
      /return TRUE
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAHarvestofDruzzil].Value}"]} && ${Me.PctMana} <= ${maChr.Find[stPctGather].Value}) {
    /if (${AltAbility[${maChr.Find[stAAHarvestofDruzzil].Value}].Spell.Base[1]} < ${Math.Calc[${Me.MaxMana}-${Me.CurrentMana}].Int} && ${Me.CurrentMana} != ${Me.MaxMana}) {
      /call cast ${_debug} 0 FALSE
      /return TRUE
    }
  }

/return TRUE



|***
 * note: healing. no. you dont get any. suck it up butercup
 * use: /call check_heal
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

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
 * note: Wizard burn routines
 * use: bug guns?
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

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

  | shhhhh..
  /if (${cast_data[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | next 45 spells +51% crit rate
  /if (${cast_data[${_debug}, "${maChr.Find[stAAFrenziedDevastation].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  | increase chance to land
  /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusofArcanum].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }


  | alternate twincast
  /if (!${Me.Buff[${maChr.Find[stTC].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAImprovedTwincast].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.AltAbilityReady[Improved Twincast]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stTC].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  | boost spell type damage
  /if (${cast_data[${_debug}, "${maChr.Find[stAAFuryoftheGods].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAArcaneDestruction].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | increase damage
  /if (${cast_data[${_debug}, "${maChr.Find[stAAArcaneFury].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | aww baby
  /if (${maChr.Find[swManaBurn].Value}) {
    /call target set ${_spawn.ID}
    /if (!${Target.Buff[Mana Burn].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAManaBurn].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
    }
  }

  | swarm pet
  /if (SWARM) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAACallofXuzl].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 * note: Control for getting Twincast either spell or Proc
 * use: /call force_twincast DEBUG
 ***|
sub force_twincast(bool _debug, spawn _spawn)
  DEBUG \atforce_twincast\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  CHECKEXIT
  GETINPUT

  /invoke ${set_data_timer[${_debug}, Force_TC, 6s]}

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }


  /if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[Twincast].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stTC].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /return

  } else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.Buff[${maChr.Find[stTC].Value}].ID} && !${Me.Buff[Twincast].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stNukeTC${stLine}].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | set concussion use %
  /if (${_type.Equal[concussion]}) {
    /if (!${set_control_num_range[${_debug}, stPctConcussion, "${_verbage}", 0, 99]}) /return FALSE

  | defensive mob agro count
  } else /if (${_type.Equal[def]}) {
    /if (!${set_control_num_range[${_debug}, stCountDefensive, "${_verbage}", 0, 99]}) /return FALSE

  | set gather use %
  } else /if (${_type.Equal[gather]}) {
    /if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return FALSE

  | use manaburn
  } else /if (${_type.Equal[mb]}) {
    /invoke ${set_switch_env[${_debug}, swManaBurn, TRUE, ${_verbage}]}

  | force twincast attempts
  } else /if (${_type.Equal[forcetc]}) {
    /invoke ${set_switch_env[${_debug}, swForceTC, TRUE, ${_verbage}]}

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE




