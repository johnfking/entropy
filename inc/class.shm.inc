|***
 * entropy
 * class.shm.inc
 ***|



|***
 * note: primary initilization of all shaman non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /call set_data ${_debug} maData stSpire                             map outer "Spire of Ancestors"
  /call set_data ${_debug} maData stSynergy                           map outer "Luminary's Synergy"
  /call set_data ${_debug} maData stFade                              map outer FALSE
  /call set_data_map ${_debug} maChr stAAUnity                        "Visionary's Unity"           "${auxna} class aa unity buff"

  | group regen timer
  /invoke ${set_data_timer[${_debug}, Check_Regen, 3s]}

  /invoke ${set_data_timer[${_debug}, Check_Talisman, 30s]}
  /call set_data_map ${_debug} maChr stTalisman                       PREGEN|1                      "${auxna} talisman spell"
  /call set_data_map ${_debug} maChr swTalisman                       FALSE                         "${auxsw} use talisman spell"

  /call set_data_map ${_debug} maChr stCannibalization                PREGEN|1                      "${auxna} canni spell"

  /call set_data_map ${_debug} maChr stPctCanniHP                     60                            "${auxnum} HP level to stop using canni"
  /call set_data_map ${_debug} maChr stPctCanniManna                  60                            "${auxnum} mana level to start using canni"

  /call set_data_map ${_debug} maHeal stCountSurge                    0                             "${auxnum} number of people hurt to use surge healing"

  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} dichotomic spell"
  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell"

  /call set_data_map ${_debug} maHeal swHealGroupOverTime             FALSE                         "${auxsw} use group heal over time spells/aas"            heal

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                xtarget|checkminion                                                 "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 heal|manarecovery                                                   "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|pet|misc|debuff|dot|nuke|alliance|swarm|item                  "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover                                                         "${auxli} class post combat "

  | AAs
  /call set_data_map ${_debug} maChr stAACalloftheAncients            PREGEN|1                      "${auxna} Call of the Ancients aa"
  /call set_data_map ${_debug} maChr stAASpiritCall                   PREGEN|1                      "${auxna} Spirit Call aa"
  /call set_data_map ${_debug} maChr stAADampenResistance             PREGEN|1                      "${auxna} Dampen Resistance aa"
  /call set_data_map ${_debug} maChr stAAFleetingSpirit               PREGEN|1                      "${auxna} Fleeting Spirit aa"
  /call set_data_map ${_debug} maChr stAASpiritualBlessing            PREGEN|1                      "${auxna} Spiritual Blessing aa"
  /call set_data_map ${_debug} maChr stAARabidBear                    PREGEN|1                      "${auxna} Rabid Bear aa"
  /call set_data_map ${_debug} maChr stAACannibalization              PREGEN|1                      "${auxna} Cannibalization aa"
  /call set_data_map ${_debug} maChr stAAAncestralAid                 PREGEN|1                      "${auxna} Ancestral Aid aa"

/return



|***
 * note: main shaman routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase
    /call check_cani ${maDebug.Find[canni].Value}
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
      /if (${maChr.Find[swTalisman].Value}) /call cast_talisman ${maDebug.Find[talisman].Value}
    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }
    /if (AUTO && !ENGINE3) {
      /call class_cycle lsOrderClassPost
      /call check_cani ${maDebug.Find[canni].Value}
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }
  }

/return


|***
 * noote: Combat skills, Disciplines and Alternate abilities.
 * use: yes, there are a lot here... lies..
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

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
  }
  /if (AUTO && MELEE) {
    /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
  }

  /call check_cani ${maDebug.Find[canni].Value}

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_tmpID}
    /return TRUE
  }

/return TRUE



|***
 * note:
 * use:
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKTIE
  | ISMEDEAD
  CHECKEXIT
  | CHECKREZ
  | CHECKFADE

  SETHUD "heal ${_type}"

  /if (!SAFEZONE) {

    | Group Regen Heal
    /if (${maHeal.Find[swHealGroupOverTime].Value} && !${timer_Check_Regen} && AE#) {
      /if (${SpawnCount[npc radius ENV]} >= AE#) {
        /call cast_cycle_heal_regen ${maDebug.Find[regen].Value} 0 grouphot
      }
    }

    | forcing surge  
    /if (${maOver.Find[swOverSplashHurt].Value} && ${maHeal.Find[stCountSplash].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
      /if (${SpawnCount[npc radius ENV]} >= ${maHeal.Find[stCountSplash].Value}) {
        /call cast_cycle_heal_surge ${maDebug.Find[surge].Value}
      }
    }

    /call get_hurt ${maDebug.Find[hurt].Value} ${_type}


    | check for surge
    /if (${maHeal.Find[stCountSurge].Value} && (${Group.Members} > 1 || ${Raid.Members})) {
      /if (${hurtCount} >= ${maHeal.Find[stCountSurge].Value}) {
        /call cast_cycle_heal_surge ${maDebug.Find[surge].Value}
      }
    }

    | group healing
    /if (${Select[${_type},group]} && ${Group}) {

      | group heal
      /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /call cast_cycle_heal ${_debug} 0 group
      }

      /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
        /return FALSE
      }

      | stationary ward heal
      /if (${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /if (${is_ready[${_debug}, "${maChr.Find[stAACalloftheAncients].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAACalloftheAncients].Value}" ${Me.ID} FALSE
        }
      }

    }

    | PET HEALING
    /if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value} && ${Pet.ID}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /call cast_cycle_heal ${_debug} ${Pet.ID} minion
      }
    }

    /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }

    | single heal over time
    /if (${maHeal.Find[swHealOverTime].Value} && !${timer_Check_Regen}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
        /call cast_cycle_heal_regen ${_debug} ${hurtID} hot
      }
    }

  }

  /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT
  | CHECKFADE
  | CHECKREZ

  /call cast_cycle_heal ${_debug} ${hurtID} single

  SETHUD EMPTY

/return



|***
 * note: surge heal
 * use: part of heal routine
 ***|
sub cast_cycle_heal_surge(bool _debug)
  DEBUG \atcast_cycle_heal_surge\ax()

  CHECKEXIT
  ISMEDEAD
  GETINPUT

  | get the main assisst ID
  /declare _tmpID int local ${get_assist_info[${_debug}, ID]}
  DEBUG ${sep}assist ID${sep}\a-w${_tmpID}\ax

  /if (!${Spawn[${_tmpID}].LineOfSight}) {
    DEBUG ${break}.LineOfSight
    /return FALSE
  }

  | get our twincast proc
  /if (!${Me.Song[Healing Twincast].ID}) {
    /call cast_cycle_nuke ${_debug} ${_tmpID} twincast
  }

  | /if (!${Me.Song[Healing Twincast].ID}) {
  |   DEBUG ${break}no twincast song
  |   /return FALSE
  | }

  | get our surge spell

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineHeal.Contains[EMPTY]} && ${lsLineHeal.Count} == 1) {
    DEBUG ${sep}resetting the empty heal line
    /invoke ${lsLineHeal.Clear}
    /invoke ${lsLineHeal.Append[${stCount50}]}
  }

  /declare _myHeal string local FALSE
  /declare _liHeal listiterator local
  /vardata _liHeal lsLineHeal.First.Clone
  /while (!${_liHeal.IsEnd}) {
    DEBUG ${sep}heal${sep}\a-w${maHeal.Find[stHeal${_liHeal.Value}].Value}${sep}${maHeal.Find[stHeal${_liHeal.Value}]}\ax
    GETINPUT cast_cycle_heal_surge NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_heal_surge

    | check the switch
    /if (!${Bool[${maHeal.Find[swHeal${_liHeal.Value}].Value}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maHeal.Find[stHeal${_liHeal.Value}].Value}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag for failure
    /if (!${lsHeal${_liHeal.Value}Tag.Contains[surge]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag pass
    /if (${lsHeal${_liHeal.Value}Tag.Contains[surge]}) {
      /varset _myHeal ${maHeal.Find[stHeal${_liHeal.Value}].Value}
    }

    | ready?.. or continue
    /if (!${is_ready[${_debug}, "${_myHeal}"]}) {
      /varset _myHeal FALSE
      /invoke ${_liHeal.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Heal ${_liHeal.Value} ${Spawn[${_tmpID}].DisplayName} "${maHeal.Find[stHeal${_liHeal.Value}Con].Value}" cast_cycle_heal_surge
    /if (!${Macro.Return}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    /call voodoo ${_debug} "${maHeal.Find[stHeal${_liHeal.Value}].Value}" ${_tmpID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
      /return TRUE
    }

    /invoke ${_liHeal.Advance}
  }

/return TRUE



|***
 * note: Shaman burn routines
 * use: normal burn stuf
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

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_tmpID}
  }

  /if (AUTO && MELEE) {
    /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
  }

  | swarm aa pet
  /if (SWARM && ${Target.ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAASpiritCall].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAASpiritCall].Value}" ${_tmpID} FALSE
    }
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
    /return TRUE
  }

  | Lower spell resists
  /if (${is_ready[${_debug}, "${maChr.Find[stAADampenResistance].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAADampenResistance].Value}" ${_tmpID} FALSE
  }

  | Twincast next 6 DOTs/HOTs
  /if (${is_ready[${_debug}, "${maChr.Find[stAAFleetingSpirit].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAFleetingSpirit].Value}" 0 FALSE
  }

  | Increase spell cost by 14% for 100% exceptional heals on instant heals for 18 seconds
  /if (${is_ready[${_debug}, "${maChr.Find[stAASpiritualBlessing].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAASpiritualBlessing].Value}" 0 FALSE
  }

  | Mega bear crap
  /if (AUTO && MELEE && ${is_ready[${_debug}, "${maChr.Find[stAARabidBear].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAARabidBear].Value}" ${_tmpID} FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 *  DES: Control for Talisman Buff
 *  USE: /call cast_talisman DEBUG
 ***|
sub cast_talisman(bool _debug)
  DEBUG \atcast_talisman\ax()

  /invoke ${set_data_timer[${_debug}, Check_Talisman, RESTART]}

  /if (${Me.Buff[${maChr.Find[stTalisman].Value}].ID}) {
    /return FALSE
  }

  /if (${Me.Buff[${maChr.Find[stDichotomic].Value}].ID}) {
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT
  GETINPUT

  /if (${is_ready[${_debug}, "${maChr.Find[stTalisman].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stTalisman].Value}" ${Me.ID} FALSE
  }

/return TRUE



|***
 * note: We want Mana
 * use: /chr
 ***|
sub check_cani(bool _debug)
  DEBUG \atcheck_cani\ax()

  /if (!${maChr.Find[stPctCanniManna].Value}) {
    DEBUG ${break}canni disabled
    /return TRUE
  }

  /if (${Me.PctMana} >= ${maChr.Find[stPctCanniManna].Value}) {
    DEBUG ${break}returning(1) - too much mana${sep}\a-w${Me.PctMana} >= ${maChr.Find[stPctCanniManna].Value}\ax
    /return FALSE
  }
  /if (${Me.PctHPs} <= ${maChr.Find[stPctCanniHP].Value}) {
    DEBUG ${break}returning(1) - HP too low${sep}\a-w${Me.PctHPs} <= ${maChr.Find[stPctCanniHP].Value}\ax
    /return FALSE
  }


  CHECKEXIT
  GETINPUT
  ISMEDEAD

  | use the aa cani
  /if (${AltAbility[${maChr.Find[stAACannibalization].Value}].Spell.Base[2]} < ${Math.Calc[${Me.MaxMana}-${Me.CurrentMana}].Int} && ${Me.CurrentMana} != ${Me.MaxMana} && ${Me.CurrentHPs} > ${AltAbility[${maChr.Find[stAACannibalization].Value}].Spell.Base[1]}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAACannibalization].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAACannibalization].Value}" 0 FALSE
    }
  } 

  | if canni spell is not ready bail..
  /if (!${is_ready[${_debug}, "${maChr.Find[stCannibalization].Value}"]}) {
    DEBUG ${break}not ready${sep}\a-w${maChr.Find[stCannibalization].Value}\ax
    /return FALSE
  }
  
  /if (${Me.PctMana} >= ${maChr.Find[stPctCanniManna].Value}) {
    DEBUG ${break}returning(2) - too much mana${sep}\a-w${Me.PctMana} >= ${maChr.Find[stPctCanniManna].Value}\ax
    /return FALSE
  }
  /if (${Me.PctHPs} <= ${maChr.Find[stPctCanniHP].Value}) {
    DEBUG ${break}returning(2) - HP too low${sep}\a-w${Me.PctHPs} <= ${maChr.Find[stPctCanniHP].Value}\ax
    /return FALSE
  }

  | check HP requirements
  
  /if (${Spell[${Spell[${maChr.Find[stCannibalization].Value}].RankName}].Base[2]} >= ${Math.Calc[${Me.MaxMana}-${Me.CurrentMana}].Int} || ${Me.CurrentMana} == ${Me.MaxMana} || ${Me.CurrentHPs} < ${Spell[${Spell[${maChr.Find[stCannibalization].Value}].RankName}].Base[1]}) {
    DEBUG ${break}bad time to cani!!
    /return FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stCannibalization].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stCannibalization].Value}" 0 FALSE
  }

/return TRUE



|***
 * note: check/cast shaman alliance
 * use: yahh... not really sure this even works anymore
 ***|
sub cast_alliance(bool _debug)
  DEBUG \atcast_alliance\ax()

  CHECKEXIT
  GETINPUT

  /if (!${is_ready[${_debug}, "${maChr.Find[stAlliance].Value}"]}) /return FALSE

  /declare _name string local ${get_assist_info[${_debug}, name]}
  /declare _tmpID int local ${get_assist_info[${_debug}, id]}

  /call target set ${_tmpID}

  /delay 5 ${Target.BuffsPopulated}
  /if (${Target.BuffsPopulated} && !${Target.Buff[${maChr.Find[stAlliance].Value}].ID}) {
    /call voodoo ${_debug} "${maChr.Find[stAlliance].Value}" ${_tmpID} FALSE
  }

/return TRUE



|***
 * note: MGB if both AAs are ready
 * use:
 ***|
sub cast_AA_aid(bool _debug)
  DEBUG \atcast_AA_aid\ax()
  
  /if (${Me.Song[${maChr.Find[stAAAncestralAid].Value}].ID}) {
    /return FALSE
  }
  
  /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
    /return FALSE
  }

  /if (!${Me.AltAbilityReady[${maChr.Find[stAAAncestralAid].Value}]}) {
    /return FALSE
  }
  
  ISMEDEAD
  CHECKEXIT
  /call voodoo ${_debug} "Mass Group Buff" 0 FALSE
  /delay 5
  /call voodoo ${_debug} "${maChr.Find[stAAAncestralAid].Value}" 0 FALSE
  
/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | % of HP to canni at
  /if (${_type.Equal[cannihps]}) {
    /if (!${set_control_num_range[${_debug}, stPctCanniHP, "${_verbage}", 0, 99]}) /return FALSE

  | mgb
  } else /if (${_type.Equal[maid]}) {
    /call cast_AA_aid
    /return TRUE

  } else /if (${_type.Equal[cannimanna]}) {
    /if (!${set_control_num_range[${_debug}, stPctCanniManna, "${_verbage}", 0, 99]}) /return FALSE

  | use talisman
  } else /if (${_type.Equal[talisman]}) {
    /invoke ${set_switch_env[${_debug}, swTalisman, TRUE, ${_verbage}]}

  | arbitration (sheild AND AA)
  } else /if (${_type.Equal[canni]}) {
    /if (${_verbage.Equal[mana]}) {
      /if (!${set_control_num_range[${_debug}, stPctCanniManna, ${_verbage2}, 0, 99]}) /return FALSE
    } else /if (${_verbage.Equal[hp]}) {
      /if (!${set_control_num_range[${_debug}, stPctCanniHP, ${_verbage2}, 0, 99]}) /return FALSE
    } else /if (${_verbage.Equal[aa]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stAACannibalization, "${_verbage2}"]}
    } else /if (${_verbage.Equal[spell]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stCannibalization, "${_verbage2}"]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[mana,stPctCanniManna]}
    /invoke ${maControl.Add[hp,stPctCanniHP]}
    /invoke ${maControl.Add[aa,stAACannibalization|br]}
    /invoke ${maControl.Add[spell,stCannibalization|br]}
    /invoke ${set_control_output[${_debug}, maChr, "chr canni", maControl]}
    /return TRUE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE


