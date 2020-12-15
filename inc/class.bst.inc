|***
 * entropy
 * class.bst.inc
 ***|



|***
 * note: primary initilization of all beastlord non shared data
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
  /call set_data_map ${_debug} maChr stFury                           PREGEN|1                      "${auxna} furry disc (ruaabri)"
  /call set_data_map ${_debug} maChr stSavage                         PREGEN|1                      "${auxna} savagery disc"

  /call set_data_list ${_debug} maChr lsHealToT                       PREGEN|3                      "${auxli} targets target healing line of spells"

  /invoke ${set_data_timer[FALSE, Check_Para, 2s]}
  /call set_data_map ${_debug} maChr stPctParagonG                    0                             "${auxnum} pct mana to use group paragon aa"
  /call set_data_map ${_debug} maChr stPctParagonF                    0                             "${auxnum} pct mana to use focused paragon aa on you"
  /call set_data_map ${_debug} maChr stPctParagonFO                   0                             "${auxnum} pct mana to use focused paragon aa on someone else"
  /call set_data_map ${_debug} maChr stPctConsume                     0                             "${auxnum} pct mana to use consume aa"
  /call set_data_list ${_debug} maChr lsParagonClass                  EMPTY                         "${auxli} classes you will use single paragon on"


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
    /call check_mana
    /if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
    /if (!${timer_Check_Para}) /call check_mana ${maDebug.Find[para].Value}
  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }
  }

  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    GETINPUT
    CHECKREZ
    /if (!${timer_Check_Cure} && ${maChr.Find[swHealCure].Value}) /call check_cure ${maDebug.Find[cure].Value}
    /if (!${check_class_loop[${maDebug.Find[status].Value}]}) /return FALSE

    /if (VALID && !ENGINE3) {
      /call set_combat_control ${maDebug.Find[decision].Value} TARGET
      /if (MELEE) /call do_melee ${maDebug.Find[melee].Value} TARGET
      /call check_heal_routines
      /call check_item_cycle ${maDebug.Find[item].Value} TARGET epic
      /if (PET || SWARM) /call send_pet ${maDebug.Find[sic].Value} TARGET
      /if (!${timer_Check_deBuffs}) {
        /call check_deBuffs ${maDebug.Find[debuff].Value} TARGET
      }
      /call misc_combat ${maDebug.Find[misc].Value} TARGET
      /if (BURN) /call burn ${maDebug.Find[burn].Value} TARGET
      /if (SWARM) /call cast_temp_pet ${maDebug.Find[temp].Value} TARGET "${maChr.Find[stNukeServant].Value}"
      /if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} TARGET
      /if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} TARGET
      /call check_item_cycle ${maDebug.Find[item].Value} TARGET
      /if (${maCC.Find[swAlliance].Value}) /call cast_alliance_${Me.Class.ShortName} TARGET

    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

    /if (AUTO && !ENGINE3) {
      CHECKFADE
      /if (!${timer_Check_Para}) /call check_mana ${maDebug.Find[para].Value}
      /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
      /if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[rest].Value}
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
sub misc_combat(bool _debug, int _tmpID)
  DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKEXIT misc_combat
  GETINPUT
  CHECKTIE
  CHECKDEFENSE
  BUFFNOW
  BUFFTEMP
  /if (!PULL) {
    /call get_assist_target ${maDebug.Find[target].Value}
    /if (TARGET != ${_tmpID}) /varset _tmpID TARGET
  }

  /if (!PULL) {
    /call get_assist_target ${maDebug.Find[target].Value}
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

  | do we want the mobs attention?
  /if (AGRO) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAARoaringStrike].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAARoaringStrike].Value}" ${_tmpID} FALSE
    }

  } else /if (!AGRO) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAARoarofThunder].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAARoarofThunder].Value}" ${_tmpID} FALSE
    }
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stBestial].Value}"]} && !${Me.Song[${maChr.Find[stBestial].Value}].ID}) {
    /call voodoo ${_debug} "${maChr.Find[stBestial].Value}" 0 FALSE
  }

  /if (!AE#) {
    /if (${is_ready[${_debug}, "${maChr.Find[stClaws].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stClaws].Value}" ${_tmpID} FALSE
    }
  }

  /if (${is_ready[${_debug}, "Enduring Frenzy"]} && !${Target.Buff[Enduring Frenzy].ID}) {
    /call voodoo ${_debug} "Enduring Frenzy" ${_tmpID} FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stRake].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stRake].Value}" ${_tmpID} FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stRending].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stRending].Value}" ${_tmpID} FALSE
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (${Me.Pet.ID} && !${Me.Song[${maChr.Find[stGrowl].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stGrowl].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stGrowl].Value}" 0 FALSE
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

  ISMEDEAD
  CHECKEXIT check_mana_BST
  /declare _count int local 0

  | consumption of Spirits AA
  /if (${maChr.Find[stPctConsume].Value} && ${Pet.ID} && !SAFEZONE) {
    /if (${Me.PctMana} < ${maChr.Find[stPctConsume].Value}*.PCTHO) {
      /if (${is_ready[${_debug}, "${maChr.Find[atAAConsumptionofSpirit].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[atAAConsumptionofSpirit].Value}" 0 FALSE
        /return TUE
      }
    }
  }
  
  /if (${Me.Song[${maChr.Find[stAAParagonofSpirit].Value}].ID}) {
    DEBUG ${break}have${sep}${maChr.Find[stAAParagonofSpirit].Value} (1)
    /return FALSE
  } 
  
  /if (${is_ready[${_debug}, "${maChr.Find[stAAParagonofSpirit].Value}"]}) {
    DEBUG ${sep}checking${sep}\a-wparagon for group\ax
    /declare _below int local 0

    /for _count 0 to ${Group}
      DEBUG ${sep}${_count}${sep}\a-w${Group.Member[${_count}].DisplayName}\ax
      ISMEDEAD
      CHECKEXIT check_mana_BST2
      /if (!${Spawn[${Group.Member[${_count}].ID}].ID}) {
        DEBUG ${break}notinzone
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
      /call voodoo ${_debug} "${maChr.Find[stAAParagonofSpirit].Value}" 0 FALSE
      /return TRUE
    }
  }
  
  | stop here if no focused is ready
  /if (!${is_ready[${_debug}, "${maChr.Find[stAAFocusedParagonofSpirits].Value}"]}) {
    /return FALSE
  }
  
  
  | self paragon
  DEBUG ${sep}checking self${sep}\a-wfocused paragon\ax
  /if (${maChr.Find[stPctParagonF].Value} && !SAFEZONE && !${Me.Song[${maChr.Find[stAAFocusedParagonofSpirits].Value}].ID}) {
    /if (${Me.PctMana} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO || ${Me.PctEndurance} <= ${maChr.Find[stPctParagonF].Value}*.PCTHO) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAFocusedParagonofSpirits].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAFocusedParagonofSpirits].Value}" ${Me.ID} FALSE
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
      
      /if (!${Spawn[${Group.Member[${_count}].ID}].ID}) {
        DEBUG ${break}notinzone
      }
      
      /if (!${lsParagonClass.Contains[${Group.Member[${_count}].Class.ShortName}]}) {
        DEBUG ${break}Bad Class${sep}${Group.Member[${_count}].Class.ShortName}
      }

      /if (${Group.Member[${_count}].Type.NotEqual[CORPSE]}) {
        DEBUG ${break}dead
       /continue
      }

      /if (${Group.Member[${_count}].Distance} > ${Spell[${maChr.Find[stAAFocusedParagonofSpirits].Value}].MyRange}) {
        DEBUG ${break} > ${Spell[${maChr.Find[stAAFocusedParagonofSpirits].Value}].MyRange} range
        /continue
      }

      /if ((${Group.Member[${_count}].PctMana} || ${Group.Member[${_count}].PctEndurance}) <=  ${maChr.Find[stPctParagonFO].Value}*.PCTHO) {
        DEBUG ${sep} mana || stamina <= ${maChr.Find[stPctParagonFO].Value}*.PCTHO
        /varset _vound TRUE
      }

      /if (${_found}) {  
        /call voodoo ${_debug} "${maChr.Find[stAAFocusedParagonofSpirits].Value}" ${Group.Member[${_count}].ID} FALSE
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

    | ToT heals
    /if (${Select[${_type},tot]} && ${hurtCount}) {
      /if (${hurtCount} >= 1)  {
        /call cast_cycle_heal ${_debug} ${Target.ID} tot
        /if (!${get_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
      }
    }

    | PET HEALING
    /if (${Select[${_type},pet]} && ${hurtCount}) {
      /if (${hurtPctHP} <= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /if (${is_ready[${_debug}, "Mend Companion"]}) {
          /call voodoo ${_debug} "Mend Companion" ${Pet.ID} FALSE
        }
      }

      /call cast_cycle_heal ${_debug} ${Pet.ID} minion
    }

    | self healing
    /if (${Select[${_type},self]} && ${hurtCount}) {
      /if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /if (${is_ready[${_debug}, "Warder's Gift", ${hurtID}]}) {
          /call voodoo ${_debug} "Warder's Gift" 0 FALSE
        }
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
 * note: Beast burn routines
 * use:
 ***|
sub burn(bool _debug, int _tmpID)
  DEBUG \atburn\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }
  CHECKEXIT burn_BST
  GETINPUT
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
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_tmpID}
    /return TRUE
  }


  /if (${is_ready[${_debug}, "${maChr.Find[stAABestialAlignment].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAABestialAlignment].Value}" 0 FALSE
  }

| Group ???


  /if (${is_ready[${_debug}, "${maChr.Find[stAAFrenziedSwipes].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAFrenziedSwipes].Value}" 0 FALSE
  }
  /if (${is_ready[${_debug}, "${maChr.Find[stAAFerociousness].Value}"]} && !${Me.Song[${maChr.Find[stSavage].Value}].ID}) {
    /call voodoo ${_debug} "${maChr.Find[stAAFerociousness].Value}" 0 FALSE
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (SWARM) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAAAttackoftheWarders].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAAttackoftheWarders].Value}" ${_tmpID} FALSE
    }
  }

  /if (${Me.Pet.ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAACompanionsFury].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAACompanionsFury].Value}" 0 FALSE
    }
  }

  /if (MELEE || DOT) {
    /if (${is_ready[${_debug}, "${maChr.Find[stVindication].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stVindication].Value}" 0 FALSE
    }
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

  /if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stFury].Value}].RankName}]} && !${Me.Song[${maChr.Find[stAAFrenzyofSpirit].Value}].ID} && !${Me.Buff[${maChr.Find[stDichotomic].Value}].ID}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stFury].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stFury].Value}" 0 FALSE
    }
  } else {
    /if (${Me.Song[${maChr.Find[stFury].Value}].ID}) /return
    /if (${is_ready[${_debug}, "${maChr.Find[stAAFrenzyofSpirit].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAAFrenzyofSpirit].Value}" 0 FALSE
    }
    /if (${is_ready[${_debug}, "${maChr.Find[stAABloodLust].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAABloodLust].Value}" 0 FALSE
    }

    /if (${target[isdead, ${_tmpID}]}) {
      /return FALSE
    }

    /call cast_AA_spire ${maDebug.Find[spire].Value}

    /if (${is_ready[${_debug}, "${maChr.Find[stSavage].Value}"]} && !${Me.Song[${maChr.Find[stAABestialAlignment].Value}].ID}) {
      /call voodoo ${_debug} "${maChr.Find[stSavage].Value}" 0 FALSE
    }
    /if (${is_ready[${_debug}, "${maChr.Find[stAAGroupBestialAlignment].Value}"]} && !${Me.Buff[Ferociousness].ID}) {
      /call voodoo ${_debug} "${maChr.Find[stAAGroupBestialAlignment].Value}" 0 FALSE
    }
  }

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


