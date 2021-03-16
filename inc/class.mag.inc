|***
 * entropy
 * class.mag.inc
 ***|



|***
 * note: primary initilization of all magician non shared data
 * use: i think we've been over this once already
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Elements"
  /call set_data ${_debug} maData stSynergy                           map outer "Conjurer's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Drape of Shadows"

  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} class Alliance spell"
  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} Dichotomic spell"
  /call set_data_map ${_debug} maChr stCoHGroup                       PREGEN|1                      "${auxna} group target call of the hero"
  /call set_data_map ${_debug} maChr stCoH                            PREGEN|1                      "${auxna} single target call of the hero"

  | pet element type
  /call set_data_map ${_debug} maChr stMinionElementType              FALSE                         "${auxna} type of pet; Air, Earth, Fire, Water"
  /call set_data ${_debug} FALSE lsElementType                        list outer Air|Earth|Fire|Water

  /invoke ${set_data_timer[${_debug}, Check_Gather, 3s]}
  /call set_data_map ${_debug} maChr stGather                         PREGEN|1                      "${auxna} spell or item to use to recover manna (not rods or aas)"
  /call set_data_map ${_debug} maChr stPctGather                      0                             "${auxnum} percent to start using gather tricks"

  /call set_data_map ${_debug} maChr stNukeServant                    PREGEN|1                      "${auxna} the little spam pets mages love to use so much."

  /call set_data_map ${_debug} maChr swCauldron                       FALSE                         "${auxsw} click cauldron to get prizes or not?"
  /call set_data_map ${_debug} maChr stCauldron                       FALSE                         "${auxna} name of the cauldron item you wish to use"

 
   | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                checkminion|manarecover|gather                                    "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 EMPTY                                                             "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|debuff|pet|swarm|dot|nuke|misc|item                         "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|gather                                                "${auxli} class post combat "

 | AAs
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                      "${auxna} Thaumaturge's Unity AA"
  /call set_data_map ${_debug} maChr stAAForceofElements              PREGEN|1                      "${auxna} Force of Elements AA"
  /call set_data_map ${_debug} maChr stAAMendCompanion                PREGEN|1                      "${auxna} Mend Companion AA"
  /call set_data_map ${_debug} maChr stAAFocusofArcanum               PREGEN|1                      "${auxna} Focus of Arcanum AA"
  /call set_data_map ${_debug} maChr stAAThaumaturgesFocus            PREGEN|1                      "${auxna} Thaumaturge's Focus AA"
  /call set_data_map ${_debug} maChr stAASilentCasting                PREGEN|1                      "${auxna} Silent Casting AA"
  /call set_data_map ${_debug} maChr stAAHeartofSkyfire               PREGEN|1                      "${auxna} Heart of Skyfire AA"
  /call set_data_map ${_debug} maChr stAAServantofRo                  PREGEN|1                      "${auxna} Servant of Ro AA"
  /call set_data_map ${_debug} maChr stAACompanionsFury               PREGEN|1                      "${auxna} Companion's Fury AA"
  /call set_data_map ${_debug} maChr stAAHostoftheElements            PREGEN|1                      "${auxna} Host of the Elements AA"
  /call set_data_map ${_debug} maChr stAAElementalConversion          PREGEN|1                      "${auxna} Elemental Conversion AA"
  /call set_data_map ${_debug} maChr stAAForcefulRejuvenation         PREGEN|1                      "${auxna} Forceful Rejuvenation AA"
  /call set_data_map ${_debug} maChr stAAHostintheShell               PREGEN|1                      "${auxna} Host in the Shell AA"

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE





|***
 * note: main magician routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase
    /if (${maChr.Find[swCauldron].Value}) /call check_cauldron  ${maDebug.Find[cauldron].Value}
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
/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat
 ***|
sub misc_combat(bool _debug, int _tmpID)
  DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)

  TMPIDDEAD
  CHECKEXIT
  GETINPUT

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

  | Forece of Elements AA
  /if (${cast_data[${_debug}, "${maChr.Find[stAAForceofElements].Value}"]}) {
    /call cast ${_debug} ${_tmpID} FALSE
  }

/return TRUE



|***
 * note: "One can never have enough socks." - dumbledor
 * use:  when you or your agro whore pet get hurt
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKEXIT

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

    /if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAAMendCompanion].Value}"]}) {
          /call cast ${_debug} ${Pet.ID} FALSE
        }
      }
      /call cast_cycle_heal ${_debug} ${Pet.ID} minion
    }
  }

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  /call cast_cycle_heal ${_debug} ${hurtID} single
  SETHUD EMPTY

/return TRUE



|***
 * note: we like fire...
 * use: establish all comp/trigger skills
 ***|
sub burn(bool _debug, int _tmpID)
  DEBUG \atburn\ax(\a-w${_tmpID}\ax)

  TMPIDDEAD
  CHECKEXIT
  GETINPUT

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

  /if (${Target.PctHPs} > 20) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusofArcanum].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  | direct damage bonus
  /if (${Target.PctHPs} > 10) {
    /if (${Me.Buff[${maChr.Find[stAAThaumaturgesFocus].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAHeartofSkyfire].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    } else /if (!${Me.Buff[${maChr.Find[stAAHeartofFlames].Value}].ID} && !${Me.AltAbilityReady[${maChr.Find[stAAHeartofSkyfire].Value}]}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAThaumaturgesFocus].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    } else /if (!${Me.AltAbilityReady[${maChr.Find[stAAHeartofSkyfire].Value}]} && !${Me.Buff[${maChr.Find[stAAHeartofSkyfire].Value}].ID} && !${Me.AltAbilityReady[${maChr.Find[stAAThaumaturgesFocus].Value}]} && !${Me.Buff[${maChr.Find[stAAThaumaturgesFocus].Value}].ID}) {
      /call cast_glyph ${maDebug.Find[glyph].Value}
      | Intensity of the Resolute AA 4 hour reuse
      /call cast_AA_intensity ${maDebug.Find[intensity].Value}

    }
  }



  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (${cast_data[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | pet AAs
  /if (PET) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAServantofRo].Value}"]}) {
      /call cast ${_debug} ${_tmpID} FALSE
    }
    /if (${cast_data[${_debug}, "${maChr.Find[stAACompanionsFury].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /if (${cast_data[${_debug}, "${maChr.Find[stAAHostintheShell].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }


  }

  | swarm pets
  /if (SWARM) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAHostoftheElements].Value}"]}) {
      /call cast ${_debug} ${_tmpID} FALSE
    }
  }

/return TRUE



|***
 * note: All that you buy, beg, borrow or steal.
 * use:
 ***|
sub check_gather(bool _debug)
  DEBUG \atcheck_gather\ax()

  /invoke ${set_data_timer[${_debug}, Check_Gather, RESTART]}

  /if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) {
    DEBUG ${sep} manna: ${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO
    /return FALSE
  }

  CHECKEXIT
  GETINPUT

  /if (${Me.Buff[Improved Twincast].ID} || ${Me.Buff[Twincast].ID}) {
    DEBUG ${sep}twincast running. get manna later
    /return FALSE
  }

  | pet consumption for manna
  /if (${Pet.ID} && ${Me.PctMana} <= 10 ) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAElementalConversion].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) {
    DEBUG ${sep} manna: ${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO
    /return FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stGather].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }


  /if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) {
    /return FALSE
  }

  /if (!${cast_data[${_debug}, "${maChr.Find[stGather].Value}"]} && !${Me.AltAbilityReady[${maChr.Find[stAAForcefulRejuvenation].Value}]}) {
    /return FALSE
  }

  | force rejuv if we can proc gather spell again
  /if (!${cast_data[${_debug}, "${maChr.Find[stGather].Value}"]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAForcefulRejuvenation].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stGather].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

/return TRUE



|***
 * note: mage execution for /coh command from other toons
 * use: /call_of_the_hero [group]
 ***|
#bind control_mag_coh /call_of_the_hero
sub Bind_control_mag_coh(string _sender, string _verbage, bool _debug)
  DEBUG \atBind_control_mag_coh\ax(\a-w${_sender}, ${_verbage}\ax)

  | output control map
  /if (${_sender.Equal[aux]}) {
    /call Bind_command_aux coh
    /return
  }

  /if (${_verbage.Equal[group]}) {
    /call set_spell_to ${_debug} TRUE "${maChr.Find[stCoHGroup].Value}" ${maEnv.Find[stBuffGem].Value} TRUE
    /makemevisible

    /if (${cast_data[${_debug}, "${maChr.Find[stCoHGroup].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } else {
    /delay 15s ${Me.AltAbilityReady[${maChr.Find[stCoH].Value}]}
    /makemevisible
    /if (${cast_data[${_debug}, "${maChr.Find[stCoH].Value}"]}) {
      /call cast ${_debug} ${Spawn[pc ${_sender}].ID} FALSE
    }
  }
/return TRUE



|***
 * note: summon cauldron item
 * use: auto on timer
 ***|
sub check_cauldron(bool _debug)
  DEBUG \atcheck_cauldron\ax()

  /if (!${Bool[${maChr.Find[stCauldron].Value}]}) {
    OUT You need to set up your cauldron if you want to use it.
    /invoke ${set_switch_env[${_debug}, swCauldron, TRUE, FALSE]}
    /return FALSE
  }

  /if (!${cast_data[${_debug}, "${maChr.Find[stCauldron].Value}"]}) {
    /invoke ${set_timer[${_debug}, Check_Cauldron, ${Math.Calc[${FindItem[${maChr.Find[stCauldron].Value}].Timer.TotalMinutes}]}m]}
    /return FALSE
  }

  CHECKEXIT
  CHECKTIE

  /call cast ${_debug} 0 FALSE

  /while (${Cursor.ID}) {
    /call check_cursor ${_debug} CLEAR
    /delay 5
  }

  /invoke ${set_timer[${_debug}, Check_Cauldron, ${Math.Calc[${FindItem[${maChr.Find[stCauldron].Value}].Timer.TotalMinutes} + 1]}m]}

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

  | minion type
  } else /if (${_type.Equal[pettype]}) {

    /if (!${Bool[${_verbage}]} || !${lsElementType.Contains[${_verbage.Left[1].Upper}${_verbage.Right[-1].Lower}]}) {
      /declare _out string local
      /declare _lipt listiterator local
      /vardata _lipt lsElementType.First.Clone
      /while (!${_lipt.IsEnd}) {
        /varset _out ${_out} ${If[${lsElementType.First.Value.Equal[${_lipt.Value}]},,${dot}]} ${If[${maChr.Find[stMinionElementType].Value.Equal[${_lipt.Value}]},${good}${_lipt.Value}\ax,${info}${_lipt.Value}\ax]}
        /invoke ${_lipt.Advance}
      }
      OUT /chr pettype ${dot} ${_out}
      /return FALSE
    } else {
      /invoke ${do_raw_edit[${_debug}, SILENT, stMinionElementType, "${_verbage.Left[1].Upper}${_verbage.Right[-1].Lower}"]}
    }

  | set ae on/off / count
  } else /if (${_type.Equal[cauldron]}) {
    /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swCauldron, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[item]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stCauldron, "${_verbage2}"]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swCauldron]}
    /invoke ${maControl.Add[item,stCauldron|br]}
    /invoke ${set_control_output[${_debug}, maChr, "chr cauldron", maControl]}
    /return TRUE

  | set gather use %
  } else /if (${_type.Equal[gather]}) {
    /if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return FALSE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}

  /if (${Macro.Return.Equal[SKIP]}) /return FALSE
  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

