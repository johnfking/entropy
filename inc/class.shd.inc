|***
 * entropy
 * class.shd.inc
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

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Reavers"
  /call set_data ${_debug} maData stSynergy                           map outer "Knight's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Death Peace"

  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr stHiatus                         PREGEN|1                      "${auxna} name of hiatus stamina recovery disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"

  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell .. damn useless thing"

  /invoke ${set_data_timer[${_debug}, Check_Harmonious, 1m]}
  /call set_data_map ${_debug} maChr swHarmonious                     FALSE                         "${auxsw} use harmonious"
  /call set_data_map ${_debug} maChr stHarmonious                     PREGEN|1                      "${auxna} name of the harmonions spell"

  | temp combat buffs (self only)
  /call set_data_map ${_debug} maChr stStance                         PREGEN|1                      "${auxna} name of the self stance buff"
  /call set_data_map ${_debug} maChr stSkin                           PREGEN|1                      "${auxna} name of the steel self buff"

  /call set_data_map ${_debug} maChr swAttraction                     FALSE                         "${auxsw} use the attraction aa to help with agro/pulling"

  /invoke ${set_data_timer[${_debug}, Check_VoT, 21m]}
  /call set_data_map ${_debug} maChr swVoT                            FALSE                         "${auxsw} use voice of thule aa"
  /call set_data_map ${_debug} maChr stTempRune                       PREGEN|1                      "${auxna} fast reuse disc rune"
  /call set_data_map ${_debug} maChr stChallenge                      PREGEN|1                      "${auxna} challenge spell for agro"
  /call set_data_map ${_debug} maChr stCrimsonBlade                   PREGEN|1                      "${auxna} blade melee dps"
  /call set_data_map ${_debug} maChr stAcrimony                       PREGEN|1                      "${auxna} hate attention"
  /call set_data_map ${_debug} maChr stRancor                         PREGEN|1                      "${auxna} blade disc tap"
  /call set_data_list ${_debug} maChr lsHate                          EMPTY                         "${auxli} single target hate skills to use"
  /call set_data_list ${_debug} maChr lsAEHate                        EMPTY                         "${auxli} ae hate skills to use"
  /call set_data_map ${_debug} maChr stAELifeTap                      PREGEN|1                      "${auxna} se lifetap nuke"
  /call set_data_map ${_debug} maChr stLeechcurse                     PREGEN|1                      "${auxna} leechcurse disc"
  /call set_data_map ${_debug} maChr stGougingBlade                   PREGEN|1                      "${auxna} gouging blade family disc"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                checkminion                                                                   "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 manarecover|staminarecovery                                                   "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    taunt|melee|agro|heal|burn|misc|dot|nuke|item|debuff|alliance|harmonious      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|staminarecovery                                                   "${auxli} class post combat "

  | AAs
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                      "${auxna} class aa unity buff"
  /call set_data_map ${_debug} maChr stAAVoiceofThule                 PREGEN|1                      "${auxna} Voice of Thule AA"
  /call set_data_map ${_debug} maChr stAALeechTouch                   PREGEN|1                      "${auxna} Leech Touch AA"
  /call set_data_map ${_debug} maChr stAAViciousBiteofChaos           PREGEN|1                      "${auxna} Vicious Bite of Chaos AA"
  /call set_data_map ${_debug} maChr stAAChatteringBones              PREGEN|1                      "${auxna} Chattering Bones AA"
  /call set_data_map ${_debug} maChr stAAProjectionofDoom             PREGEN|1                      "${auxna} Projection of Doom AA"
  /call set_data_map ${_debug} maChr stAAThoughtLeech                 PREGEN|1                      "${auxna} Thought Leech AA"
  /call set_data_map ${_debug} maChr stAAAgelessEnmity                PREGEN|1                      "${auxna} Ageless Enmity AA"
  /call set_data_map ${_debug} maChr stAADeflectionDiscipline         PREGEN|1                      "${auxna} Deflection Discipline AA"
  /call set_data_map ${_debug} maChr stAATVylsResolve                 PREGEN|1                      "${auxna} T`Vyl's Resolve AA"
  /call set_data_map ${_debug} maChr stAAVeilofDarkness               PREGEN|1                      "${auxna} Veil of Darkness AA"
  /call set_data_map ${_debug} maChr stAAHarmTouch                    PREGEN|1                      "${auxna} Harm Touch AA"
  /call set_data_map ${_debug} maChr stAAScourgeSkin                  PREGEN|1                      "${auxna} Scourge Skin AA"
  /call set_data_map ${_debug} maChr stAAVisageofDeath                PREGEN|1                      "${auxna} Visage of Death AA"
  /call set_data_map ${_debug} maChr stAAVisageofDecay                PREGEN|1                      "${auxna} Visage of Decay AA"
  /call set_data_map ${_debug} maChr stAAGiftoftheQuickSpear          PREGEN|1                      "${auxna} Gift of the Quick Spear AA"

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: Main Shadowknight Routine
 * use: self absorbed asses.. think they can tank but in reality wet TP is tougher.
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

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
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

/return TRUE



|***
 * note: healing
 * use: /call check_heal_self_${Me.Class.ShortName}
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKTIE
  ISMEDEAD
  CHECKEXIT

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

      /if (${cast_data[${_debug}, "${maChr.Find[stRancor].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
        /call cast ${_debug} TARGET FALSE
      }

      /if (${cast_data[${_debug}, "${maChr.Find[stAALeechTouch].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
        /call cast ${_debug} TARGET FALSE
      }

      /if (${SpawnCount[npc OVERLOS radius SAFEENV*2 targetable]} >= ${maChr.Find[stCountDefensive].Value}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stLeechcurs].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
          /call cast ${_debug} 0 FALSE
        }
      }

      /if (${cast_data[${_debug}, "${maChr.Find[stAAViciousBiteofChaos].Value}"]}) {
        /call cast ${_debug} TARGET FALSE
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
 * note: kill all the things!
 * use: this shits auto, learn to love it.
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT misc_combat_SHD _spawn.ID|${_spawn.ID}
  CHECKTIE
  CHECKDEFENSE
  BUFFNOW

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

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
  }

  | AA Voice of Thule AA
  /if (AGRO && !${timer_Check_VoT} && ${maChr.Find[swVoT].Value}) {
    /if (!${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAVoiceofThule].Value}"]}) {
        /call cast ${_debug} ${Me.ID} FALSE
      }
      /invoke ${set_data_timer[FALSE, Check_VoT, RESTART]}
    }
  } else /if ((!AGRO && ${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].ID}) || !${maChr.Find[swVoT].Value}) {
      /invoke ${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].Remove}
  }

  | get me sum...
  /if (AGRO) {
    /call get_agro ${_debug} ${_spawn.ID}
  }

  | quickspear proc
  /if (${cast_data[${_debug}, "${maChr.Find[stAAGiftoftheQuickSpear].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | swarm pets
  /if (SWARM) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAChatteringBones].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stTempRune].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAAThoughtLeech].Value}"]} &&  ${Me.PctMana} <= 60*.PCTHO) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stGougingBlade].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

/return TRUE



|***
 * note: we are agro gods.
 * use: /call get_agro
 ***|
sub get_agro(bool _debug, spawn _spawn)
  DEBUG \atget_agro\ax()

  ISMEDEAD
  CHECKEXIT
  GETINPUT get_agro _spawn.ID|${_spawn.ID}

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  /if (PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }

  /if (ENGINE2) {
    /call cast_cycle_agro ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  /call cast_challenge ${_debug} ${_spawn.ID}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
  }

  | AE Hate
  /if (AE#) {
    /if (${SpawnCount[npc radius 50 PS4]} >= AE#) {
        /declare _liaeha listiterator local
        /vardata _liaeha lsAEHate.First.Clone
        /while (!${_liaeha.IsEnd}) {
          /if (${cast_data[${_debug}, "${_liaeha.Value}"]}) {
            /call cast ${_debug} 0 FALSE
            /break
          }
          /invoke ${_liaeha.Advance}
        }

      | AE lifetap
      /if (!${SpawnCount[pc enchanter radius SAFEENV*2]}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAELifeTap].Value}"]}) {
          /call cast ${_debug} 0 FALSE
        }
      }
    }
  }

  /if (${Me.PctAggro} >= ${maAgro.Find[stPctAgroHold].Value}) {
    /return FALSE
  }

  | single hate
  /declare _liha listiterator local
  /vardata _liha lsHate.First.Clone
  /while (!${_liha.IsEnd}) {
    /if (${cast_data[${_debug}, "${_liha.Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
      /break
    }
    /invoke ${_liha.Advance}
  }

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }
  /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

  /if (${Me.PctAggro} >= ${maAgro.Find[stPctAgroHold].Value}) {
    /return FALSE
  }

  | AA pet hate
  /if (${cast_data[${_debug}, "${maChr.Find[stAAProjectionofDoom].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }


  | fake hate
  /if (${cast_data[${_debug}, "${maChr.Find[stAAAgelessEnmity].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  | named fake hate
  /if (${lsZoneNamed.Contains[${Target.DisplayName}]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAcrimony].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

/return TRUE



|***
 * note: don't touch me there...
 * use:
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax()

  ISMEDEAD
  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT burn_SHD _spawn.ID|${_spawn.ID}
  CHECKTIE
  CHECKDEFENSE

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  | deflection
  /if (!${Me.ActiveDisc.ID} && ${Me.PctHPs} < 50*.PCTHO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAADeflectionDiscipline].Value}"]}) {
      /call cast ${_debug} 0 FALSE
      /return TRUE
    }
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_spawn.ID}
    /return TRUE
  }

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAATVylsResolve].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stCrimsonBlade].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (${cast_data[${_debug}, "${maChr.Find[stAAVeilofDarkness].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAAHarmTouch].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  /if (AGRO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAScourgeSkin].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  | AA Visage of Decay/Death stacking issues
  /if (!${Me.Buff[${maChr.Find[stAAVisageofDecay].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAVisageofDeath].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } else /if (!${Me.AltAbilityReady[${maChr.Find[stAAVisageofDeath].Value}]} && !${Me.Buff[${maChr.Find[stAAVisageofDeath].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAVisageofDecay].Value}"]})  {
      /call cast ${_debug} 0 FALSE
    }
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 * note: class controls
 * use:
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | voice of Thule
  /if (${_type.Equal[vot]}) {
    /invoke ${set_switch_env[${_debug}, swVoT, TRUE, ${_verbage}]}

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

