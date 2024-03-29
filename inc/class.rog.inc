|***
 * entropy
 * class.rog.inc
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

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Rake"
  /call set_data ${_debug} maData stSynergy                           map outer "Blackguard's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Escape"

  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell"
  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr stHiatus                         PREGEN|1                      "${auxna} name of hiatus stamina recovery disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"

  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} Dichotomic disc family"
  /call set_data_map ${_debug} maChr stAggro                          PREGEN|1                      "${auxna} Misdirection disc family"
  /call set_data_map ${_debug} maChr stAssault                        PREGEN|1                      "${auxna} Assault disc family"
  /call set_data_map ${_debug} maChr stBleed                          PREGEN|1                      "${auxna} Bleed disc family"
  /call set_data_map ${_debug} maChr stPlay                           PREGEN|1                      "${auxna} Knifeplay disc family"
  /call set_data_map ${_debug} maChr stPhantom                        PREGEN|1                      "${auxna} Phantom Assassin disc family"
  /call set_data_map ${_debug} maChr stHack                           PREGEN|1                      "${auxna} Jugular Slash disc family"
  /call set_data_map ${_debug} maChr stHiddenBlade                    PREGEN|1                      "${auxna} Hidden Blade disc family"
  /call set_data_map ${_debug} maChr stBladePoison                    PREGEN|1                      "${auxna} Toxic Blade disc family"
  /call set_data_map ${_debug} maChr stSneakAttack                    PREGEN|1                      "${auxna} Sneak Attack disc family"
  /call set_data_map ${_debug} maChr stShadowHunter                   PREGEN|1                      "${auxna} Shadow-Hunter's Dagger disc family"
  /call set_data_map ${_debug} maChr stDisassociative                 PREGEN|1                      "${auxna} Disassociative Puncture disc family"
  /call set_data_map ${_debug} maChr stAspBleeder                     PREGEN|1                      "${auxna} Aspbleeder disc family"
  /call set_data_map ${_debug} maChr stPinPoint                       PREGEN|1                      "${auxna} Pinpoint Vulnerability disc family"
  /call set_data_map ${_debug} maChr stMark                           PREGEN|1                      "${auxna} Easy Mark disc family"
  /call set_data_map ${_debug} maChr stEradicator                     PREGEN|1                      "${auxna} Duelist disc family"
  /call set_data_map ${_debug} maChr stRazor                          PREGEN|1                      "${auxna} Razor's Edge disc family"
  /call set_data_map ${_debug} maChr stFrenzy                         PREGEN|1                      "${auxna} Frenzied Stabbing disc family"
  /call set_data_map ${_debug} maChr stTwistedChance                  PREGEN|1                      "${auxna} Twisted Chance disc family"
  /call set_data_map ${_debug} maChr stVision                         PREGEN|1                      "${auxna} Thief's Eyes disc family"

  /call set_data_map ${_debug} maChr swBlinding                       FALSE                         "${auxsw} use blinding disc.. or not."
  /call set_data_map ${_debug} maChr stBlinding                       PREGEN|1                      "${auxna} Blinding Flash disc family"

  /call set_data_map ${_debug} maChr stLegPoison                      0                             "${auxna} number of summon leg poisons to keep. dont forget the buff tag (\a-wsummonpoison\ax) for legs"

  | no sneak while tie is active?
  /call set_data_map ${_debug} maChr swNoSneakinTie                   FALSE                         "${auxsw} turn off sneaking while /tie is active"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                EMPTY                                   "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 EMPTY                                   "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|burn|misc|debuff|alliance|item|nuke|dot    "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                staminarecover                          "${auxli} class post combat"

  | AAs
  /call set_data_map ${_debug} maChr stAATwistedShank                 PREGEN|1                      "${auxna} Twisted Shank aa"
  /call set_data_map ${_debug} maChr stAAShadowsFlanking              PREGEN|1                      "${auxna} Shadow's Flanking AA"
  /call set_data_map ${_debug} maChr stAAFocusedRakesRampage          PREGEN|1                      "${auxna} Focused Rake's Rampage AA"
  /call set_data_map ${_debug} maChr stAARakesRampage                 PREGEN|1                      "${auxna} Rake's Rampage AA"
  /call set_data_map ${_debug} maChr stAARoguesFury                   PREGEN|1                      "${auxna} Rogue's Fury AA"
  /call set_data_map ${_debug} maChr stAATumble                       PREGEN|1                      "${auxna} Tumble AA"


  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE


|***
 * note: stabb'em in the ass
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  GETINPUT
  ISMEDEAD
  CHECKTIE
  CHECKREZ

  /if (AUTO && !ENGINE3) {
    /if (${maEnv.Find[swSoS].Value}) /call set_sos FALSE
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
      /call do_sneak_attack ${maDebug.Find[class].Value} GETASSIST
      /if (${Select[${Me.Class.ShortName},ROG]} && ${maEnv.Find[swSoS].Value}) {
        /call Bind_control_nomore sos ${_debug}
      }
      /call set_combat_control ${maDebug.Find[decision].Value} GETASSIST
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
 * note: suprize but seks?
 * use: designed for a first attack
 ***|
sub do_sneak_attack(bool _debug, spawn _spawn)
  DEBUG \atdo_sneak_attack\ax(\a-w${_spawn.ID}\ax)
  ISMEDEAD

  /if (!${Me.Sneaking} || ${Me.ActiveDisc.ID}) {
    /return FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stSneakAttack].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE

    /if (${maCC.Find[swBackstab].Value} && !${Skill[Backstab].Auto}) {
      /doability backstab
    }
  }

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat_ROG DEBUG
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
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
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }
  
  /if (${cast_data[${_debug}, "${maChr.Find[stAssault].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  SPAWNDEAD
  
  /call set_combat_melee ${_debug} ${_spawn.ID}
  
  /if (${cast_data[${_debug}, "${maChr.Find[stAAShadowsFlanking].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stVision].Value}"]} && !${Me.Song[${maChr.Find[stVision].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAATwistedShank].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stShadowHunter].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stDisassociative].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stBleed].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  SPAWNDEAD

  /if (${cast_data[${_debug}, "${maChr.Find[stPlay].Value}"]}) {
    /if (!${lsZoneNamed.Contains[${_spawn.DisplayName}]} || !${cast_data[${_debug}, "${maChr.Find[stFrenzy].Value}"]}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stPlay].Value}"]} && !${Bool[${Me.ActiveDisc}]}) {
        /call cast ${_debug} 0 FALSE
      }
    }
  }

  /if (!${lsZoneNamed.Contains[${Target.DisplayName}]} && !${cast_data[${_debug}, "${maChr.Find[stFrenzy].Value}"]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stPlay].Value}"]} && !${Bool[${Me.ActiveDisc}]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  SPAWNDEAD

  /if (${Target.ID} && ${cast_data[${_debug}, "${maChr.Find[stPhantom].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stHiddenBlade].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stHack].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stBladePoison].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  SPAWNDEAD

  /if (!AGRO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAggro].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (${maChr.Find[swBlinding].Value} && ${Target.Level} <= ${Me.Level}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stBlinding].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

/return TRUE



|***
 * note: healing
 * use: auto
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  CHECKREZ
  CHECKFADE

  SETHUD "heal ${_type}"

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

    /if (${Select[${_type},self]} && ${hurtCount}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAATumble].Value}"]}) {
        /call cast ${_debug} 0 FALSE
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
 * note: sets the rogue to invis
 * use: auto
 ***|
sub set_sos(bool _debug)
  DEBUG \atset_sos\ax()

  /if (!${maEnv.Find[swSoS].Value}) /return FALSE
  /if (${Me.Combat} || ${Me.Stunned} || ${Me.State.Equal[FEIGN]}) /return FALSE
  /if (${Me.Casting.ID} || ${Me.Mount.ID}) /return FALSE

  ISMEDEAD
  CHECKTIE

  /declare _sos bool local FALSE

  /if (${Me.Invis} && ${Me.Sneaking} && !${Me.AbilityReady[hide]} && !${Me.AbilityReady[sneak]}) {
    /return
  }

  /if (!${Me.Invis} && ${Me.AbilityReady[hide]} && ${Me.Sneaking} && !${Me.AbilityReady[sneak]} ) {
    /if (${Me.AbilityReady[hide]}) /doability hide
    /return
  } else /if (!${Me.Invis} && ${Me.Sneaking} && !${Me.AbilityReady[sneak]}) {
    /if (${Me.AbilityReady[sneak]}) /doability sneak
    /return
  }

  /if (!${_sos} && ${Me.AbilityReady[hide]} && ${Me.AbilityReady[sneak]}) {
    /doability hide
    /delay 1
    /doability sneak
  }

/return TRUE



|***
 * note: burn routines
 * use: seriously. if you are not buning constantly, you are doing something wrong. wtf you thinking.
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
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

  /if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stPlay].Value}]} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stFrenzy].Value}].RankName}]}) {
    :loopmakestopburn_ROG
    /delay 5
    /if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stPlay].Value}]} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stFrenzy].Value}].RankName}]}) /stopdisc
    /delay 5
    /if (${Me.ActiveDisc.Name.Equal[${maChr.Find[stPlay].Value}]} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stFrenzy].Value}].RankName}]}) /goto :loopmakestopburn_ROG
  }

  SPAWNDEAD

  /if (${cast_data[${_debug}, "${maChr.Find[stDichotomic].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  
  /if (!AE#) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusedRakesRampage].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } else /if (AE#) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAARakesRampage].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stPinPoint].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAARoguesFury].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stMark].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  SPAWNDEAD
  
  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (!${Me.ActiveDisc.ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stFrenzy].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /delay 2
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stTwistedChance].Value}"]} && !${Me.ActiveDisc.ID}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stEradicator].Value}"]} && !${Me.ActiveDisc.ID}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stRazor].Value}"]} && !${Me.ActiveDisc.ID}) {
    /call cast ${_debug} 0 FALSE
  }

  SPAWNDEAD
  
  /if (${cast_data[${_debug}, "${maChr.Find[stAspBleeder].Value}"]} && !${Me.ActiveDisc.ID}) {
    /call cast ${_debug} 0 FALSE
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
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | use blinding disc
  /if (${_type.Equal[blind]}) {
    /invoke ${set_switch_env[${_debug}, swBlinding, TRUE, ${_verbage}]}

  | sneak while tie is active switch
  } else /if (${_type.Equal[nosneaktie]}) {
    /invoke ${set_switch_env[${_debug}, swNoSneakinTie, TRUE, ${_verbage}]}

  | leg poisons to keep
  } else /if (${_type.Equal[poison]}) {
    /if (!${set_control_num_range[${_debug}, stLegPoison, "${_verbage}", 0, 99]}) /return FALSE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE






            