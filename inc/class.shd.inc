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

  | temp buff
  /call set_data_list ${_debug} maChr lsTempBuffToon                  EMPTY                         "${auxli} list of toons to use your temp buff on"
  /call set_data_map ${_debug} maChr stTempBuff                       FALSE                         "${auxna} temp buff spell"

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
    /if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle FALSE
  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }
  }

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
  }

  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    /if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
    /if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
      /call check_buff_cycle_temp_list ${maDebug.Find[temp].Value} stTempBuff lsTempBuffToon
    }
    /if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

    /if (VALID && !ENGINE3) {
      /call set_combat_control ${maDebug.Find[decision].Value} TARGET
      /invoke ${do_taunt[${_debug}, TARGET]}
      /if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
      /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
        /call cast_shield_flash ${_debug} aa
      }
      /if (AGRO) /call get_agro ${maDebug.Find[agro].Value} TARGET
      /call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
      /if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
      /call misc_combat ${maDebug.Find[misc].Value} TARGET
      /if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
      /if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
      /if (!${timer_Check_deBuffs}) {
        /call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
      }
      /call check_item_cycle ${maDebug.Find[item].Value} TARGET
      | /if (${swAlliance}) /call cast_alliance_${Me.Class.ShortName} TARGET
      /if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) {
        /call check_harmonious ${maDebug.Find[harmonious].Value}
      }

      /if (!${timer_Check_Buff_Temp} && ${maBuff.Find[swBuffTemp].Value}) {
        /call check_buff_cycle_temp_self FALSE stSkin
        /call check_buff_cycle_temp_self FALSE stStance
      }
    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }

    }

    /if (AUTO && !ENGINE3) {
      /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
      CHECKFADE
      /if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
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
  FLOW check_heal ${_type}

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  CHECKREZ
  CHECKFADE

  /call get_hurt ${maDebug.Find[hurt].Value} ${_type}

  | if no one is hurt. back to the start
  /if (!${hurtCount}) {
    SETHUD EMPTY
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

      /if (${is_ready[${_debug}, "${maChr.Find[stRancor].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
        /call voodoo ${_debug} "${maChr.Find[stRancor].Value}" TARGET FALSE
      }

      /if (${is_ready[${_debug}, "${maChr.Find[stAALeechTouch].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
        /call voodoo ${_debug} "${maChr.Find[stAALeechTouch].Value}" TARGET FALSE
      }

      /if (${SpawnCount[npc OVERLOS radius SAFEENV*2 targetable]} >= ${maChr.Find[stCountDefensive].Value}) {
        /if (${is_ready[${_debug}, "${maChr.Find[stLeechcurs].Value}"]} && ${Me.PctHPs} < ${maHeal.Find[stHealPointSHD].Value}*.PCTHO) {
          /call voodoo ${_debug} "${maChr.Find[stLeechcurse].Value}" 0 FALSE
        }
      }

      /if (${is_ready[${_debug}, "${maChr.Find[stAAViciousBiteofChaos].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAViciousBiteofChaos].Value}" TARGET FALSE
      }
    }
  }

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}
  /call cast_cycle_heal ${_debug} ${hurtID} single
  SETHUD EMPTY

/return TRUE



|***
 * note: kill all the things!
 * use: this shits auto, learn to love it.
 ***|
sub misc_combat(bool _debug, int _tmpID)
  DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)
  FLOW misc_combat

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT misc_combat_SHD _tmpID|${_tmpID}
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
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
    /return TRUE
  }

  /invoke ${do_taunt[${_debug}, ${_tmpID}]}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
  }

  | AA Voice of Thule AA
  /if (AGRO && !${timer_Check_VoT} && ${maChr.Find[swVoT].Value}) {
    /if (!${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].ID}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAVoiceofThule].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAVoiceofThule].Value}" ${Me.ID} FALSE
      }
      /invoke ${set_data_timer[FALSE, Check_VoT, RESTART]}
    }
  } else /if ((!AGRO && ${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].ID}) || !${maChr.Find[swVoT].Value}) {
      /invoke ${Me.Buff[${maChr.Find[stAAVoiceofThule].Value}].Remove}
  }

  | get me sum...
  /if (AGRO) {
    /call get_agro ${_debug} ${_tmpID}
  }

  | quickspear proc
  /if (${is_ready[${_debug}, "${maChr.Find[stAAGiftoftheQuickSpear].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAGiftoftheQuickSpear].Value}" 0 FALSE
  }

  | swarm pets
  /if (SWARM) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAChatteringBones].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAChatteringBones].Value}" ${_tmpID} FALSE
    }
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stTempRune].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stTempRune].Value}" 0 FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stAAThoughtLeech].Value}"]} &&  ${Me.PctMana} <= 60*.PCTHO) {
    /call voodoo ${_debug} "${maChr.Find[stAAThoughtLeech].Value}" ${_tmpID} FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stGougingBlade].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stGougingBlade].Value}" ${_tmpID} FALSE
  }

/return TRUE



|***
 * note: we are agro gods.
 * use: /call get_agro
 ***|
sub get_agro(bool _debug, int _tmpID)
  DEBUG \atget_agro\ax()
  FLOW get_agro

  ISMEDEAD
  CHECKEXIT
  GETINPUT get_agr _tmpID|${_tmpID}

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
  }

  /if (ENGINE2) {
    /call cast_cycle_agro ${maDebug.Find[agro].Value} ${_tmpID}
    /return TRUE
  }

  /invoke ${do_taunt[${_debug}, ${_tmpID}]}

  /call cast_challenge ${_debug} ${_tmpID}

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
  }

  | AE Hate
  /if (AE#) {
    /if (${SpawnCount[npc radius 50 PS4]} >= AE#) {
        /declare _liaeha listiterator local
        /vardata _liaeha lsAEHate.First.Clone
        /while (!${_liaeha.IsEnd}) {
          /if (${is_ready[${_debug}, "${_liaeha.Value}"]}) {
            /call voodoo ${_debug} "${_liaeha.Value}" 0 FALSE
            /break
          }
          /invoke ${_liaeha.Advance}
        }

      | AE lifetap
      /if (!${SpawnCount[pc enchanter radius SAFEENV*2]}) {
        /if (${is_ready[${_debug}, "${maChr.Find[stAELifeTap].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAELifeTap].Value}" 0 FALSE
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
    /if (${is_ready[${_debug}, "${_liha.Value}"]}) {
      /call voodoo ${_debug} "${_liha.Value}" ${_tmpID} FALSE
      /break
    }
    /invoke ${_liha.Advance}
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }
  /invoke ${do_taunt[${_debug}, ${_tmpID}]}

  /if (${Me.PctAggro} >= ${maAgro.Find[stPctAgroHold].Value}) {
    /return FALSE
  }

  | AA pet hate
  /if (${is_ready[${_debug}, "${maChr.Find[stAAProjectionofDoom].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAProjectionofDoom].Value}" ${_tmpID} FALSE
  }


  | fake hate
  /if (${is_ready[${_debug}, "${maChr.Find[stAAAgelessEnmity].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAAgelessEnmity].Value}" ${_tmpID} FALSE
  }

  | named fake hate
  /if (${lsZoneNamed.Contains[${Target.DisplayName}]}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAcrimony].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAcrimony].Value}" ${_tmpID} FALSE
    }
  }

/return TRUE



|***
 * note: don't touch me there...
 * use:
 ***|
sub burn(bool _debug, int _tmpID)
  DEBUG \atburn\ax()
  FLOW burn

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT burn_SHD _tmpID|${_tmpID}
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

  | deflection
  /if (!${Me.ActiveDisc.ID} && ${Me.PctHPs} < 50*.PCTHO) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAADeflectionDiscipline].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAADeflectionDiscipline].Value}" 0 FALSE
      /return TRUE
    }
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
    /return TRUE
  }

  /if (${maSash.Find[stPctShieldFlash].Value} || ${maSash.Find[stCountShieldFlash].Value}) {
    /call cast_shield_flash ${_debug} aa
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stAATVylsResolve].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAATVylsResolve].Value}" ${_tmpID} FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stCrimsonBlade].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stCrimsonBlade].Value}" 0 FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (${is_ready[${_debug}, "${maChr.Find[stAAVeilofDarkness].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAVeilofDarkness].Value}" ${_tmpID} FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stAAHarmTouch].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAHarmTouch].Value}" ${_tmpID} FALSE
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (AGRO) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAScourgeSkin].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAScourgeSkin].Value}" 0 FALSE
    }
  }

  | AA Visage of Decay/Death stacking issues
  /if (!${Me.Buff[${maChr.Find[stAAVisageofDecay].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAVisageofDeath].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAVisageofDeath].Value}" 0 FALSE
    }
  } else /if (!${Me.AltAbilityReady[${maChr.Find[stAAVisageofDeath].Value}]} && !${Me.Buff[${maChr.Find[stAAVisageofDeath].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAVisageofDecay].Value}"]})  {
      /call voodoo ${_debug} "${maChr.Find[stAAVisageofDecay].Value}" 0 FALSE
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

