|***
 * entropy
 * class.bst.inc
 ***|



|***
 * note: primary initilization of all non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Savage Lord"
  /call set_data ${_debug} maData stSynergy                           map outer "Primalist's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Falsified Death"

  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr stHiatus                         PREGEN|1                      "${auxna} name of hiatus stamina recovery disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"
  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell"

  /call set_data_map ${_debug} maChr stNukeServant                    PREGEN|1                      "${auxna} spam nuke pet spell"
  /call set_data_map ${_debug} maChr stGrowl                          PREGEN|1                      "${auxna} growl buff spell"
  /call set_data_map ${_debug} maChr stFero                           PREGEN|1                      "${auxna} ferocity spell line"
  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} dichotomic spell"
  /call set_data_map ${_debug} maChr stBestial                        PREGEN|1                      "${auxna} bestial disc line"
  /call set_data_map ${_debug} maChr stRending                        PREGEN|1                      "${auxna} reflexive disc"
  /call set_data_map ${_debug} maChr stVindication                    PREGEN|1                      "${auxna} vindication disc"
  /call set_data_map ${_debug} maChr stRake                           PREGEN|1                      "${auxna} rake disc"
  /call set_data_map ${_debug} maChr stClaws                          PREGEN|1                      "${auxna} claws disc"
  /call set_data_map ${_debug} maChr stAEClaws                        PREGEN|1                      "${auxna} ae claws disc"
  /call set_data_map ${_debug} maChr stFury                           PREGEN|1                      "${auxna} furry disc (ruaabri)"
  /call set_data_map ${_debug} maChr stSavage                         PREGEN|1                      "${auxna} savagery disc"

  /call set_data_list ${_debug} maChr lsHealToT                       PREGEN|3                      "${auxli} targets target healing line of spells"

  /invoke ${set_data_timer[${_debug}, Check_Para, 2s]}
  /call set_data_map ${_debug} maChr stPctParagonG                    0                             "${auxnum} pct mana to use group paragon aa"
  /call set_data_map ${_debug} maChr stPctParagonF                    0                             "${auxnum} pct mana to use focused paragon aa on you"
  /call set_data_map ${_debug} maChr stPctParagonFO                   0                             "${auxnum} pct mana to use focused paragon aa on someone else"
  /call set_data_map ${_debug} maChr stPctConsume                     0                             "${auxnum} pct mana to use consume aa"
  /call set_data_list ${_debug} maChr lsParagonClass                  EMPTY                         "${auxli} classes you will use single paragon on"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                checkminion|manarecover|staminarecover                    "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 checkminion                                               "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|heal|pet|debuff|misc|burn|swarm|dot|nuke|item       "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|staminarecover                                "${auxli} class post combat "

  | AAs
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                      "${auxna} class aa unity buff"
  /call set_data_map ${_debug} maChr stAARoaringStrike                PREGEN|1                      "${auxna} Roaring Strike aa"
  /call set_data_map ${_debug} maChr stAAAttackoftheWarders           PREGEN|1                      "${auxna} Attack of the Warders aa"
  /call set_data_map ${_debug} maChr stAABestialAlignment             PREGEN|1                      "${auxna} Bestial Alignment aa"
  /call set_data_map ${_debug} maChr stAAFrenziedSwipes               PREGEN|1                      "${auxna} Frenzied Swipes aa"
  /call set_data_map ${_debug} maChr stAAFerociousness                PREGEN|1                      "${auxna} Ferociousness aa"
  /call set_data_map ${_debug} maChr stAACompanionsFury               PREGEN|1                      "${auxna} Companion's Fury aa"
  /call set_data_map ${_debug} maChr stAAParagonofSpirit              PREGEN|1                      "${auxna} Paragon of Spirit aa"
  /call set_data_map ${_debug} maChr stAAFocusedParagonofSpirits      PREGEN|1                      "${auxna} Focused Paragon of Spirits aa"
  /call set_data_map ${_debug} maChr stAAFrenzyofSpirit               PREGEN|1                      "${auxna} Frenzy of Spirit aa"
  /call set_data_map ${_debug} maChr stAAGroupBestialAlignment        PREGEN|1                      "${auxna} Group Bestial Alignment aa"
  /call set_data_map ${_debug} maChr stAABloodLust                    PREGEN|1                      "${auxna} BloodLust aa"
  /call set_data_map ${_debug} maChr stAARoarofThunder                PREGEN|1                      "${auxna} Roar of Thunder aa"
  /call set_data_map ${_debug} maChr atAAConsumptionofSpirit          PREGEN|1                      "${auxna} Consumption of Spirit aa"
  /call set_data_map ${_debug} maChr atAAEnduringFrenzy               pregen|1                      "${auxna} Enduring Frenzy aa"


  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: main beastlord routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase
    /if (!${timer_Check_Para}) /call check_mana ${maDebug.Find[para].Value}
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
      /if (!${timer_Check_Para}) /call check_mana ${maDebug.Find[para].Value}
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }
  }

/return TRUE



|***
 * note:
 * use: /call misc_combat DEBUG
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD

  CHECKEXIT misc_combat
  GETINPUT
  CHECKTIE

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }

  /if ((AUTO && MELEE) || MODE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }
  
  SPAWNDEAD

  | do we want the mobs attention?
  /if (AGRO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAARoaringStrike].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }

  } else /if (!AGRO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAARoarofThunder].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  SPAWNDEAD

  /if (${cast_data[${_debug}, "${maChr.Find[stBestial].Value}"]} && !${Me.Song[${maChr.Find[stBestial].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (!AE#) {
    /if (${cast_data[${_debug}, "${maChr.Find[stClaws].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  } else /if (AE#) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAEClaws].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }
  
  SPAWNDEAD

  /if (${cast_data[${_debug}, "${maChr.Find[atAAEnduringFrenzy].Value}"]} && !${Target.Buff[Enduring Frenzy].ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stRake].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stRending].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  SPAWNDEAD

  /if (${Me.Pet.ID} && !${Me.Song[${maChr.Find[stGrowl].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stGrowl].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

/return TRUE



|***
 * note: Focused Paragon. will maintain the bst
 * use: /call check_mana
 ***|
sub check_mana(bool _debug)
  DEBUG \atcheck_mana\ax()

  /invoke ${set_data_timer[${_debug}, Check_Para, RESTART]}

  | ISMEDEAD
  CHECKEXIT check_mana_BST
  /declare _count int local 0

  | consumption of Spirits AA
  /if (${maChr.Find[stPctConsume].Value} && ${Pet.ID} && !SAFEZONE) {
    /if (${Me.PctMana} < ${maChr.Find[stPctConsume].Value}*.PCTHO) {
      /if (${cast_data[${_debug}, "${maChr.Find[atAAConsumptionofSpirit].Value}"]}) {
        /call cast ${_debug} 0 FALSE
        /return TRUE
      }
    }
  }
  
  /if (${Me.Song[${maChr.Find[stAAParagonofSpirit].Value}].ID}) {
    DEBUG ${break}have${sep}${maChr.Find[stAAParagonofSpirit].Value} (1)
    /return FALSE
  } 
  
  /if (${cast_data[${_debug}, "${maChr.Find[stAAParagonofSpirit].Value}"]}) {
    DEBUG ${sep}checking${sep}\a-wparagon for group\ax
    /declare _below int local 0

    /for _count 0 to ${Group}
      DEBUG ${sep}${_count}${sep}\a-w${Group.Member[${_count}].DisplayName}\ax
      ISMEDEAD
      CHECKEXIT check_mana_BST2
      
      /if (!${Spawn[${Group.Member[${_count}].ID}].ID} || !${Group.Member[${_count}].Present}) {
        DEBUG ${break}notinzone
        /continue
      }

      /if (${Group.Member[${_count}].Type.Equal[corpse]}) {
        DEBUG ${break}corpse
        /continue
      }
      
      /if (${Group.Member[${_count}].Class.ShortName.Equal[BRD]}) {
        DEBUG ${break}bard
        /continue
      }

      /if (!${lsParagonClass.Contains[${Group.Member[${_count}].Class.ShortName}]}) {
        DEBUG ${break}Bad Class${sep}${Group.Member[${_count}].Class.ShortName}
        /continue
      }

      /if (${Group.Member[${_count}].PctMana} >= ${maChr.Find[stPctParagonG].Value}*.PCTHO) {
        DEBUG ${break}maChr.Find[stPctParagonG].Value
        /continue
      }
      
      /if (${Group.Member[${_count}].Distance} > ${Spell[${maChr.Find[stAAParagonofSpirit].Value}].AERange}) {
        DEBUG ${break}AERange${sep}${Spell[${maChr.Find[stAAParagonofSpirit].Value}].AERange}
        /continue
      }

      /if (${Group.Member[${_count}].ID} == ${Pet.ID}) {
        DEBUG ${break}pet
        /continue
      }

      /varcalc _below ${_below}+1
      /if (${_below}) {
        DEBUG ${break}we found someone
        /break
      }
    /next _count

    /if (${_below} && !SAFEZONE) {
      /call cast ${_debug} 0 FALSE
      /return TRUE
    }
  }
  
  | stop here if no focused is ready
  /if (!${cast_data[${_debug}, "${maChr.Find[stAAFocusedParagonofSpirits].Value}"]}) {
    /return FALSE
  }
  
  
  | self paragon
  DEBUG ${sep}checking self${sep}\a-wfocused paragon\ax
  /if (${maChr.Find[stPctParagonF].Value} && !SAFEZONE && !${Me.Song[${maChr.Find[stAAFocusedParagonofSpirits].Value}].ID}) {
    /if (${Me.PctMana} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO || ${Me.PctEndurance} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusedParagonofSpirits].Value}"]}) {
        /call cast ${_debug} ${Me.ID} FALSE
        /return TRUE
      }
    }
  }
  
  | Group member single Paragon
  DEBUG ${sep}checking group${sep}\a-wfocused paragon\ax
  /if (${Group} && ${maChr.Find[stPctParagonFO].Value} && !SAFEZONE) {
    /declare _found bool local FALSE
    /for _count 0 to ${Group}

      /if (${Group.Member[${_count}].ID} == ${Me.ID}) {
        /continue
      }
      
      /if (!${Spawn[${Group.Member[${_count}].ID}].ID} || !${Group.Member[${_count}].Present}) {
        DEBUG ${break}notinzone
        /continue
      }
      
      /if (!${lsParagonClass.Contains[${Group.Member[${_count}].Class.ShortName}]}) {
        DEBUG ${break}Bad Class${sep}${Group.Member[${_count}].Class.ShortName}
        /continue
      }

      /if (${Group.Member[${_count}].Type.NotEqual[CORPSE]}) {
        DEBUG ${break}dead
       /continue
      }

      /if (${Group.Member[${_count}].Distance} > ${Spell[${maChr.Find[stAAFocusedParagonofSpirits].Value}].MyRange}) {
        DEBUG ${break} > ${Spell[${maChr.Find[stAAFocusedParagonofSpirits].Value}].MyRange} range
        /continue
      }

      /if ((${Group.Member[${_count}].PctMana} <=  ${maChr.Find[stPctParagonFO].Value}*.PCTHO) || (${Group.Member[${_count}].PctEndurance} <= ${maChr.Find[stPctParagonFO].Value}*.PCTHO)) {
        DEBUG ${sep} mana || stamina <= ${maChr.Find[stPctParagonFO].Value}*.PCTHO
        /varset _found TRUE
      }

      /if (${_found}) {
        OUT ${maChr.Find[stAAFocusedParagonofSpirits].Value}${sep}\a-w${Group.Member[${_count}].DisplayName}\ax
        /call cast ${_debug} ${Group.Member[${_count}].ID} FALSE
        /varset _found FALSE
        /return TRUE
      }

    /next _count
  }  
   
/return TRUE




|***
 * note: healing for beastlord
 * use:  when you or your agro whore pet get hurt
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKTIE
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

    | ToT heals
    /if (${Select[${_type},tot]} && ${hurtCount}) {
      /if (${hurtCount} >= 1)  {
        /call cast_cycle_heal ${_debug} ${Target.ID} tot
        /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
      }
    }

    | PET HEALING
    /if (${Select[${_type},pet]} && ${hurtCount}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /if (${cast_data[${_debug}, "Mend Companion"]}) {
          /call cast ${_debug} ${Pet.ID} FALSE
        }
      }

      /call cast_cycle_heal ${_debug} ${Pet.ID} minion
    }

    | self healing
    /if (${Select[${_type},self]} && ${hurtCount}) {
      /if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /if (${cast_data[${_debug}, "Warder's Gift"]}) {
          /call cast ${_debug} 0 FALSE
        }
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
 * note: Beast burn routines
 * use:
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD
  CHECKEXIT burn_BST
  GETINPUT
  CHECKTIE

  /if (SWARM || PET) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  
  /if ((AUTO && MELEE) || MODE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_spawn.ID}
    /return TRUE
  }


  /if (${cast_data[${_debug}, "${maChr.Find[stAABestialAlignment].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

| Group ???


  /if (${cast_data[${_debug}, "${maChr.Find[stAAFrenziedSwipes].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAAFerociousness].Value}"]} && !${Me.Song[${maChr.Find[stSavage].Value}].ID}) {
    /call cast ${_debug} 0 FALSE
  }

  SPAWNDEAD

  /if (SWARM) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAAttackoftheWarders].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (${Me.Pet.ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAACompanionsFury].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /if (MELEE || DOT) {
    /if (${cast_data[${_debug}, "${maChr.Find[stVindication].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  SPAWNDEAD

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

  /if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stFury].Value}].RankName}]} && !${Me.Song[${maChr.Find[stAAFrenzyofSpirit].Value}].ID} && !${Me.Buff[${maChr.Find[stDichotomic].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stFury].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } else {
    /if (${Me.Song[${maChr.Find[stFury].Value}].ID}) /return
    /if (${cast_data[${_debug}, "${maChr.Find[stAAFrenzyofSpirit].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /if (${cast_data[${_debug}, "${maChr.Find[stAABloodLust].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

    SPAWNDEAD

    /call cast_AA_spire ${maDebug.Find[spire].Value}

    /if (${cast_data[${_debug}, "${maChr.Find[stSavage].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /if (${cast_data[${_debug}, "${maChr.Find[stAAGroupBestialAlignment].Value}"]} && !${Me.Buff[Ferociousness].ID}) {
      /call cast ${_debug} 0 FALSE
    }
  }

/return TRUE



|***
 * note: MGB if both AAs are ready
 * use:
 ***|
sub cast_AA_paragon(bool _debug)
  DEBUG \atcast_AA_paragon\ax()
  
  /if (${Me.Song[${maChr.Find[stAAParagonofSpirit].Value}].ID}) {
    /return FALSE
  }
  
  /if (${Me.Song[${maChr.Find[stAAFocusedParagonofSpirits].Value}].ID}) {
    /return FALSE
  }
  
  /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
    /return FALSE
  }

  /if (!${Me.AltAbilityReady[${maChr.Find[stAAParagonofSpirit].Value}]}) {
    /return FALSE
  }
  
  ISMEDEAD
  CHECKEXIT
  /call cast_data ${_debug} "Mass Group Buff"
  /call cast ${_debug} 0 FALSE
  /delay 5
  /call cast_data ${_debug} "${maChr.Find[stAAParagonofSpirit].Value}"
  /call cast ${_debug} 0 FALSE
  
/return TRUE



|***
 * note: beastlord specific control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, string _verbage3, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | consume spirit
  /if (${_type.Equal[consume]}) {
    /if (!${set_control_num_range[${_debug}, stPctConsume, "${_verbage}", 0, 99]}) /return

  | mgb
  } else /if (${_type.Equal[mpara]}) {
    /call cast_AA_paragon
    /return TRUE

  | paragon aa, usages, class
  } else /if (${_type.Equal[para]}) {
    /if (${_verbage.Equal[self]}) {
      /if (!${set_control_num_range[${_debug}, stPctParagonF, "${_verbage2}", 0, 99]}) /return
    } else /if (${_verbage.Equal[other]}) {
      /if (!${set_control_num_range[${_debug}, stPctParagonFO, "${_verbage2}", 0, 99]}) /return
    } else /if (${_verbage.Equal[group]}) {
      /if (!${set_control_num_range[${_debug}, stPctParagonG, "${_verbage2}", 0, 99]}) /return
     } else /if (${_verbage.Equal[class]}) {
      /call set_control_shortname ${_debug} lsParagonClass ${_verbage2}
      /invoke ${out[12, "/chr para class ${dot} ${Macro.Return}"]}
      /return TRUE
 
     }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[self,stPctParagonF]}
    /invoke ${maControl.Add[other,stPctParagonFO]}
    /invoke ${maControl.Add[group,stPctParagonG]}
    /invoke ${maControl.Add[class,submenu]}
    /invoke ${set_control_output[${_debug}, maChr, "chr para", maControl]}
    /return TRUE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE


