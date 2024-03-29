|***
 * entropy
 * class.rng.inc
 ***|




|***
 * note: primary initilization of all ranger non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Pathfinders"
  /call set_data ${_debug} maData stSynergy                            map outer "Outrider's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Cover Tracks"
  /call set_data_map ${_debug} maChr stAAUnity                        "Wildstalker's Unity (Azia)"  "${auxna} class aa unity buff"

  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell"

  | spam summer nuke
  /invoke ${set_data_timer[${_debug}, Summer_Nuke, 1s]}
  /call set_data_map ${_debug} maChr stSummer                         PREGEN|1                      "${auxna} summer line nuke spell"
  /call set_data_map ${_debug} maChr stCountSummer                    0                             "${auxnum} number of mobs in environment to start using summer nuke line"

  | rest disc
  /call set_data_map ${_debug} maChr stRest                           PREGEN|1                      "${auxna} name of stamina rest disc"
  /call set_data_map ${_debug} maChr stHiatus                         PREGEN|1                      "${auxna} name of hiatus stamina recovery disc"
  /call set_data_map ${_debug} maChr swHiatus                         FALSE                         "${auxsw} use hiatus rest disc"

  | heals
  /call set_data_map ${_debug} maChr stDiscHeal                       PREGEN|1                      "${auxna} group heal disc"

  | bow disc
  /call set_data_map ${_debug} maChr stDiscRange                      PREGEN|1                      "${auxna} ranged DPS disc"

  | melee disc
  /call set_data_map ${_debug} maChr stDiscMelee                      PREGEN|1                      "${auxna} melee DPS disc"

  | agro/anti agro kicks
  /call set_data_map ${_debug} maChr stDiscJolt                       PREGEN|1                      "${auxna} hate reduction disc"
  /call set_data_map ${_debug} maChr stDiscEnrage                     PREGEN|1                      "${auxna} hate increase disc"

  | ae melee disc
  /call set_data_map ${_debug} maChr stDiscMeleeAE                    PREGEN|1                      "${auxna} ae melee disc"

  | focused melee disc
  /call set_data_map ${_debug} maChr stDiscMeleeSingle                PREGEN|1                      "${auxna} single melee disc"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                EMPTY                                     "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 EMPTY                                     "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|heal|debuff|item|burn|misc|dot|nuke "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|staminarecover                "${auxli} class post combat"

  | AAs
  /call set_data_map ${_debug} maChr stAAImbuedFerocity               PREGEN|1                      "${auxna} Imbued Ferocity aa"
  /call set_data_map ${_debug} maChr stAAEmpoweredBlades              PREGEN|1                      "${auxna} Empowered Blades aa"
  /call set_data_map ${_debug} maChr stAAOutridersAccuracy            PREGEN|1                      "${auxna} Outrider's Accuracy aa"
  /call set_data_map ${_debug} maChr stAAOutridersAttack              PREGEN|1                      "${auxna} Outrider's Attack aa"
  /call set_data_map ${_debug} maChr stAAGroupGuardianoftheForest     PREGEN|1                      "${auxna} Group Guardian of the Forest aa"
  /call set_data_map ${_debug} maChr stAAGuardianoftheForest          PREGEN|1                      "${auxna} Guardian of the Forest aa"
  /call set_data_map ${_debug} maChr stAAPackHunt                     PREGEN|1                      "${auxna} Pack Hunt aa"
  /call set_data_map ${_debug} maChr stAAElementalArrow               PREGEN|1                      "${auxna} Elemental Arrow aa"
  /call set_data_map ${_debug} maChr stAAAuspiceoftheHunter           PREGEN|1                      "${auxna} Auspice of the Hunter aa"
  /call set_data_map ${_debug} maChr stAAScarletCheetahsFang          PREGEN|1                      "${auxna} Scarlet Cheetah's Fang aa"

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: Main Ranger Routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  GETINPUT
  ISMEDEAD
  CHECKTIE
  CHECKREZ

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
      /call set_combat_control ${maDebug.Find[decision].Value} GETASSIST
      /call class_cycle lsOrderClass
    } else /if (VALID && ENGINE3) {
      /if (${SubDefined[e3_combat_${Me.Class.ShortName}]}) {
        /call e3_combat_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

    /if (AUTO && !ENGINE3) {
      /call class_cycle lsOrderClassPost
      /if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) /call cast_stamina_recovery ${maDebug.Find[staminarecover].Value}
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

  }

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: /call misc_combat DEBUG TARGETID
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  CHECKEXIT
  GETINPUT
  SPAWNDEAD

  /call check_heal_routines

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

  /if (${maChr.Find[stCountSummer].Value} && !${timer_Summer_Nuke}) {
    /call cast_nuke_summer ${maDebug.Find[summer].Value} ${_spawn.ID}
  }

  | agro control
  /if (AGRO) {

    /invoke ${do_taunt[${_debug}, ${_spawn.ID}]}

    /if (${Me.PctAggro} < ${maAgro.Find[stPctAgroHold].Value}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stDiscEnrage].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
    }
  }

  /if (!AGRO) {
    /if (${cast_data[${_debug}, "${maChr.Find[stDiscJolt].Value}", ready]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  | elemental arrows rotation
  /if (!${Target.Buff[Elemental Arrow].ID}) {
    /if (${cast_data[${_debug}, "Elemental Arrow"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (!AE#) {
    /if (${cast_data[${_debug}, "${maChr.Find[stDiscMeleeSingle].Value}", ready]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  } else /if (${SpawnCount[npc OVERLOS radius 60 PS4 targetable]} >= ${maChr.Find[stCountAE].Value} && ${maChr.Find[stCountAE].Value}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stDiscMeleeAE].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

/return TRUE



|***
 * note: healing
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

    | ToT heals
    /if (${Select[${_type},tot]} && ${hurtCount}) {
      /if (${hurtCount})  {
        /call cast_cycle_heal ${_debug} ${hurtData.ID} tot
        /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) /return FALSE
      }
    }


    /if (!${spawn_hurt[${_debug}, ${_type}]}) /return FALSE

    | Reflexive Healing Disc
    /if (${hurtCount} && ${Select[${_type},self,group,tot]}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
        /call cast ${_debug} TARGET FALSE
      }
    }
  }


  /if (${Select[${_type},tot]}) {
   /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
      /return FALSE
    }

    /call cast_cycle_heal ${_debug} ${Target.ID} tot
  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

  SETHUD

/return TRUE



|***
 * note: Control for Spam Agro Nuke
 * use: /call cast_nuke_summer DEBUG [spawnID]
 ***|
sub cast_nuke_summer(bool _debug, spawn _spawn)
  DEBUG \atcast_nuke_summer\ax(\a-w${_spawn.ID}\ax)

  SPAWNDEAD

  /if (!${maChr.Find[stCountSummer].Value}) {
    /return FALSE
  }

  /if (${SpawnCount[npc radius ENV PS4]} < ${maChr.Find[stCountSummer].Value}) {
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT
  /invoke ${set_data_timer[${_debug}, Summer_Nuke, RESTART]}

  /if (${cast_data[${_debug}, "${maChr.Find[stSummer].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

/return TRUE



|***
 * note: really should be starting fires
 * use:
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

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  | cheeta
  /if (${cast_data[${_debug}, "${maChr.Find[stAAScarletCheetahsFang].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | auspice
  /if (${cast_data[${_debug}, "${maChr.Find[stAAAuspiceoftheHunter].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | MELEE
  /if (MELEE && !RANGE || MELEE && ${Spawn[${_spawn.ID}].Distance} <= ${Spawn[${_spawn.ID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {

    | weapon proc
    /if (${cast_data[${_debug}, "${maChr.Find[stAAImbuedFerocity].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

    /if (!${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAEmpoweredBlades].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }

    } else /if (!${Me.ActiveDisc.ID} && !${Me.Song[${maChr.Find[stAAEmpoweredBlades].Value}].ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stDiscMele].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }

    /if (!${Me.ActiveDisc.ID} && !${cast_data[${_debug}, "${maChr.Find[stDiscMele].Value}"]} && !${Me.AltAbilityReady[${maChr.Find[stAAEmpoweredBlades].Value}]} && !${Me.Song[${maChr.Find[stAAEmpoweredBlades].Value}].ID}) {
      /call cast_glyph ${maDebug.Find[glyph].Value}
      | Intensity of the Resolute AA 4 hour reuse
      /call cast_AA_intensity ${maDebug.Find[intensity].Value}

    }
  }


  | RANGE
  /if (RANGE && ${Target.Distance} >= ${maHard.Find[stMinBowDist].Value}) {
    /if (!${Me.ActiveDisc.ID} && !${Me.Buff[${maCC.Find[stGlyph].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stDiscRange].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }

    /if (!${Me.ActiveDisc.ID} && !${cast_data[${_debug}, "${maChr.Find[stDiscRange].Value}"]}) {
      /call cast_glyph ${maDebug.Find[glyph].Value}
      | Intensity of the Resolute AA 4 hour reuse
      /call cast_AA_intensity ${maDebug.Find[intensity].Value}
    }
  }


  | attack / dmg AA
  /if (${cast_data[${_debug}, "${maChr.Find[stAAOutridersAccuracy].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  /if (${cast_data[${_debug}, "${maChr.Find[stAAOutridersAttack].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | guardian buff (group first)
  /if (!${Me.Song[${maChr.Find[stAAGuardianoftheForest].Value}].ID} && ${Me.AltAbilityReady[${maChr.Find[stAAGroupGuardianoftheForest].Value}]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAGroupGuardianoftheForest].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }

  } else /if (!${Me.Song[${maChr.Find[stAAGroupGuardianoftheForest].Value}].ID} && ${Me.AltAbilityReady[${maChr.Find[stAAGuardianoftheForest].Value}]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAGuardianoftheForest].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  | swarm pets
  /if (SWARM) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAPackHunt].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  | ae regen
  /if (${cast_data[${_debug}, "${maChr.Find[stDiscHeal].Value}"]} && !${Me.Song[Bladespur Reflection].ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

 /return TRUE



|***
 * note: MGB if both AAs are ready
 * use:
 ***|
sub cast_AA_auspice(bool _debug)
  DEBUG \atcast_AA_auspice\ax()
 
  /if (${Me.Song[${maChr.Find[stAAAuspiceoftheHunter].Value}].ID}) {
    /return FALSE
  }
  
  /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
    /return FALSE
  }

  /if (!${Me.AltAbilityReady[${maChr.Find[stAAAuspiceoftheHunter].Value}]}) {
    /return FALSE
  }
  
  ISMEDEAD
  CHECKEXIT
  /call cast_data ${_debug} "Mass Group Buff"
  /call cast ${_debug} 0 FALSE
  /delay 5
  /call cast_data ${_debug} "${maChr.Find[stAAAuspiceoftheHunter].Value}"
  /call cast ${_debug} 0 FALSE
  
/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | summer switch
  /if (${_type.Equal[summer]}) {
    /if (!${set_control_num_range[${_debug}, stCountSummer, "${_verbage}", 0, 100]}) /return
  
  | mgb
  } else /if (${_type.Equal[mspice]}) {
    /call cast_AA_auspice
    /return TRUE
  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

