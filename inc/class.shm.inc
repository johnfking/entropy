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
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|pet|heal|misc|debuff|dot|nuke|alliance|heal|swarm|item        "${auxli} class active combat order"
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

  /call check_cani ${maDebug.Find[canni].Value}

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
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
  CHECKEXIT

  SETHUD "heal ${_type}"


  | Group Regen Heal
  /if (${maHeal.Find[swHealGroupOverTime].Value} && !${timer_Check_Regen}) {
    /if (AE# && ${SpawnCount[npc radius ENV]} >= AE#) {
      /call cast_cycle_heal_regen ${maDebug.Find[regen].Value} 0 grouphot
    }
  }

  /if (!SAFEZONE) {

    /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

    | check for surge
    /if (${maHeal.Find[stCountSurge].Value} && (${Group.Members} > 1 || ${Raid.Members}) || ${Me.Song[Healing Twincast].ID}) {
      /if (${SpawnCount[npc radius ENV PS4]} && ${Select[${Me.TargetOfTarget.Type},npc]}) {
        /call cast_cycle_heal_surge ${maDebug.Find[surge].Value}
      }
    }

    | group healing
    /if (${Select[${_type},group]} && ${Group}) {

      | group heal
      /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /call cast_cycle_heal ${_debug} 0 group
      }

      /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
        /return FALSE
      }

      | stationary ward heal
      /if (${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAACalloftheAncients].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
        }
      }

    }

    | PET HEALING
    /if (${Select[${_type},pet]} && ${maMinion.Find[swPet].Value} && ${Pet.ID}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /call cast_cycle_heal ${_debug} ${Pet.ID} minion
      }
    }

    /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }

    | single heal over time
    /if (${maHeal.Find[swHealOverTime].Value} && !${timer_Check_Regen}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
        /call cast_cycle_heal_regen ${_debug} ${hurtData.ID} hot
      }
    }

  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

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
  | /declare _spawn.ID int local ${get_assist_info[${_debug}, ID]}
  | DEBUG ${sep}assist ID${sep}\a-w${_spawn.ID}\ax

  /if (!${assistData.LineOfSight}) {
    DEBUG ${break}.LineOfSight
    /return FALSE
  }
  
  | get our twincast proc
  /if (${lsLineNuke.Contains[EMPTY]} && ${lsLineNuke.Count} == 1) {
    DEBUG ${sep}resetting the empty nuke line
    /invoke ${lsLineNuke.Clear}
    /invoke ${lsLineNuke.Append[${stCount20}]}
  }

  /declare _liNukeTC listiterator local
  /vardata _liNukeTC lsLineNuke.First.Clone
  /while (!${_liNukeTC.IsEnd}) {

    | check the switch
    /if (!${Bool[${maNuke.Find[swNuke${_liNukeTC.Value}].Value}]}) {
      DEBUG ${break}switched off${sep}\a-w${maNuke.Find[stNuke${_liNukeTC.Value}].Value}\ax
      /invoke ${_liNukeTC.Advance}
      /continue
    }

    | skip non twincast nukes
    /if (!${lsNuke${_liNukeTC.Value}Tag.Contains[twincast]}) {
      DEBUG ${break}invalid twincast check (\a-r${maNuke.Find[stNuke${_liNukeTC.Value}].Value}\ax)
      /invoke ${_liNukeTC.Advance}
      /continue
    }
    
    /if (!${cast_data[${_debug}, "${maNuke.Find[stNuke${_liNukeTC.Value}].Value}"]}) {
      /invoke ${_liNukeTC.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Nuke ${_liNukeTC.Value} ${Me.DisplayName} "${maNuke.Find[stNuke${_liNukeTC.Value}].Value}" cast_cycle_heal_surge
    /if (!${Macro.Return}) {
      /invoke ${_liNukeTC.Advance}
      /continue
    }

    /call cast ${_debug} ${assistData.ID} FALSE
    /delay 10s !${Me.Casting.ID}
    /delay 2.5s
    
    /if (${Me.Song[Healing Twincast].ID}) {
      /break
    }
    
    /invoke ${_liNukeTC.Advance}
  }

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

    | ready?.. or continue
    /if (!${cast_data[${_debug}, "${maHeal.Find[stHeal${_liHeal.Value}].Value}"]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Heal ${_liHeal.Value} ${Spawn[${_spawn.ID}].DisplayName} "${maHeal.Find[stHeal${_liHeal.Value}Con].Value}" cast_cycle_heal_surge
    /if (!${Macro.Return}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    /call cast ${_debug} ${assistData.ID} FALSE

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

  | swarm aa pet
  /if (SWARM && ${Target.ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAASpiritCall].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_spawn.ID}
    /return TRUE
  }

  | Lower spell resists
  /if (${cast_data[${_debug}, "${maChr.Find[stAADampenResistance].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  | Twincast next 6 DOTs/HOTs
  /if (${cast_data[${_debug}, "${maChr.Find[stAAFleetingSpirit].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | Increase spell cost by 14% for 100% exceptional heals on instant heals for 18 seconds
  /if (${cast_data[${_debug}, "${maChr.Find[stAASpiritualBlessing].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | Mega bear crap
  /if (AUTO && MELEE && ${cast_data[${_debug}, "${maChr.Find[stAARabidBear].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
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

  /if (${cast_data[${_debug}, "${maChr.Find[stTalisman].Value}"]}) {
    /call cast ${_debug} ${Me.ID} FALSE
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
    /if (${cast_data[${_debug}, "${maChr.Find[stAACannibalization].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } 

  | if canni spell is not ready bail..
  /if (!${cast_data[${_debug}, "${maChr.Find[stCannibalization].Value}"]}) {
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

  /if (${cast_data[${_debug}, "${maChr.Find[stCannibalization].Value}"]}) {
    /call cast ${_debug} 0 FALSE
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

  /if (!${cast_data[${_debug}, "${maChr.Find[stAlliance].Value}"]}) /return FALSE

  /declare _name string local ${get_assist_info[${_debug}, name]}
  /declare _spawn.ID int local ${get_assist_info[${_debug}, id]}

  /call target set ${_spawn.ID}

  /delay 5 ${Target.BuffsPopulated}
  /if (${Target.BuffsPopulated} && !${Target.Buff[${maChr.Find[stAlliance].Value}].ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
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
  /call cast_data ${_debug} "Mass Group Buff" 
  /call cast ${_debug}0 FALSE
  /delay 5
  /call cast_data ${_debug} "${maChr.Find[stAAAncestralAid].Value}" 
  /call cast ${_debug} 0 FALSE
  
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


