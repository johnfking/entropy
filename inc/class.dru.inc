|***
 * entropy
 * class.dru.inc
 ***|



|***
 * note: primary initilization of all druid non shared data
 * use:
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data FALSE maData stSpire                                 map outer "Spire of Nature"
  /call set_data FALSE maData stSynergy                               map outer "Preserver's Synergy"
  /call set_data FALSE maData stFade                                  map outer "Veil of the Underbrush"
  /call set_data_map FALSE maChr stAAUnity                            "Wildtender's Unity"          "${auxna} class aa unity buff"

  /call set_data_map FALSE maChr stAlliance                           PREGEN|1                      "${auxna} alliance spell"

  /call set_data_map FALSE maChr swWolfSpirit                         FALSE                         "${auxsw} enable spirit of the great wolf aa (self/group)"

  /call set_data_map FALSE maChr stTwincast                           PREGEN|1                      "${auxna} twincast spell"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                xtarget                                                                               "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 heal|alliance|manarecover|burn                                                        "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|heal|misc|debuff|dot|nuke|alliance|item                                         "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover                                                                           "${auxli} class post combat "

  | AAs
  /call set_data_map ${_debug} maChr stAASeasonsWrath                 PREGEN|1                      "${auxna} Season's Wrath AA"
  /call set_data_map ${_debug} maChr stAASpiritoftheGreatWolf         PREGEN|1                      "${auxna} Spirit of the Great Wolf AA"
  /call set_data_map ${_debug} maChr stAAGroupSpiritoftheGreatWolf    PREGEN|1                      "${auxna} Group Spirit of the Great Wolf AA"
  /call set_data_map ${_debug} maChr stAASwarmofFireflies             PREGEN|1                      "${auxna} Swarm of Fireflies AA"
  /call set_data_map ${_debug} maChr stAAConvergenceofSpirits         PREGEN|1                      "${auxna} Convergence of Spirits AA"
  /call set_data_map ${_debug} maChr stAAWildtendersSurvival          PREGEN|1                      "${auxna} Wildtender's Survival AA"
  /call set_data_map ${_debug} maChr stAASpiritoftheWood              PREGEN|1                      "${auxna} Spirit of the Wood AA"
  /call set_data_map ${_debug} maChr stAABlessingofTunare             PREGEN|1                      "${auxna} Blessing of Tunare AA"
  /call set_data_map ${_debug} maChr stAANaturesGuardian              PREGEN|1                      "${auxna} Nature's Guardian AA"
  /call set_data_map ${_debug} maChr stAAProtectionofDirewood         PREGEN|1                      "${auxna} Protection of Direwood AA"
  /call set_data_map ${_debug} maChr stAASilentCasting                PREGEN|1                      "${auxna} Silent Casting AA"
  /call set_data_map ${_debug} maChr stAADestructiveVortex            PREGEN|1                      "${auxna} Destructive Vortex AA"
  /call set_data_map ${_debug} maChr stAANaturesFury                  PREGEN|1                      "${auxna} Natures Fury AA"
  /call set_data_map ${_debug} maChr stAANaturesBlessing              PREGEN|1                      "${auxna} Nature's Blessing AA"
  /call set_data_map ${_debug} maChr stAANaturesBoon                  PREGEN|1                      "${auxna} Nature's Boon AA"
  /call set_data_map ${_debug} maChr stAAImprovedTwincast             PREGEN|1                      "${auxna} Improved Twincast AA"
  /call set_data_map ${_debug} maChr stAAFocusofArcanum               PREGEN|1                      "${auxna} Focus of Arcanum aa"
  

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return



|***
 * note: main druid routine
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
    } else /if (AUTO && !ENGINE3) {
      /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
        /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
      }
    }

  }

/return TRUE



|***
 * note: Combat skills, Disciplines and Alternate abilities.
 * use: pushmorebuttons!!
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  SPAWNDEAD
  CHECKEXIT

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

  /if (${cast_data[${_debug}, "${maChr.Find[stAASeasonsWrath].Value}"]} && ${madeBuff.Find[malo].Value}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAStormStrike].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  | great wolf self/group aa
  /if (${maChr.Find[swWolfSpirit].Value}) {
    /if (${Me.AltAbilityReady[${maChr.Find[stAASpiritoftheGreatWolf].Value}]} && !${Me.Buff[${maChr.Find[stAASpiritoftheGreatWolf].Value}].ID} && !${Me.Buff[${maChr.Find[stAAGroupSpiritoftheGreatWolf].Value}].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAASpiritoftheGreatWolf].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    } else /if (${Me.AltAbilityReady[${maChr.Find[stAAGroupSpiritoftheGreatWolf].Value}]} && !${Me.AltAbilityReady[${maChr.Find[stAASpiritoftheGreatWolf].Value}]} && !${Me.Buff[${maChr.Find[stAASpiritoftheGreatWolf].Value}].ID} && !${Me.Buff[Group Spirit of the Great Wolf].ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAGroupSpiritoftheGreatWolf].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }
  }


/return TRUE



|***
 * note: healing.. cause every druid likes healing
 * use: fuckers...
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  ISMEDEAD
  CHECKEXIT

  SETHUD "heal ${_type.Lower}"

  /call spawn_hurt ${_debug} ${_type}

  /if (!${hurtCount}) {
    SETHUD
    /return
  }

  /if (!SAFEZONE) {

    /if (${hurtData.PctHPs} < ${maHeal.Find[stHealPoint${hurtData.Class.ShortName}].Value}*.PCTHO) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAASwarmofFireflies].Value}"]}) {
        /call cast ${_debug} ${hurtData.ID} FALSE
      }
    }

    /if (${hurtData.PctHPs} < ${maHeal.Find[stHealPoint${hurtData.Class.ShortName}].Value}*.PCTHO) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAConvergenceofSpirits].Value}"]}) {
        /call cast ${_debug} ${hurtData.ID} FALSE
      }
    }
    

    | single heal over time
    /if (${maHeal.Find[swHealOverTime].Value}) {
      /if (${hurtData.PctHPs} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
        /call cast_cycle_heal_regen ${maDebug.Find[regen].Value} ${hurtData.ID} hot
      }
    }
    

  }

  /if (!${spawn_hurt[${_debug}, ${_type}]}) /return FALSE

  | Group Heal
  /if (${Select[${_type},group]} && ${maHeal.Find[stCountHealGroup].Value}) {
    /if (${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {

      /if (!SAFEZONE) {

        /if (${hurtData.PctHPs} < ${maHeal.Find[stHealPoint${hurtData.Class.ShortName}].Value}*.PCTHO) {
          /if (${cast_data[${_debug}, "${maChr.Find[stAAWildtendersSurvival].Value}"]}) {
            /call cast ${_debug} 0 FALSE
          }
        }

        /if (${cast_data[${_debug}, "${maChr.Find[stAASpiritoftheWood].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
          /return
        }

        /if (!${spawn_hurt[${_debug}, ${_type}]}) /return FALSE

        /if (${cast_data[${_debug}, "${maChr.Find[stAABlessingofTunare].Value}"]}) {
          /call cast ${_debug} ${Me.ID} FALSE
          /return
        }
      }

      /if (!${spawn_hurt[${_debug}, ${_type}]}) /return FALSE

      | group heal?
      /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /call cast_cycle_heal ${_debug} 0 group
      }

    }
  }

  /if (!${spawn_hurt[${_debug}, ${_type}]}) {
    /return FALSE
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

  SETHUD

/return TRUE



|***
 * note: Druid burn routines
 * use: dpsing the things
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

  | swarm pets
  /if (SWARM) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAANaturesGuardian].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  | defense
  /if (${cast_data[${_debug}, "${maChr.Find[stAAProtectionofDirewood].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusofArcanum].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | damage boost
  /if (${cast_data[${_debug}, "${maChr.Find[stAADestructiveVortex].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAANaturesFury].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | heal boost
  /if (${cast_data[${_debug}, "${maChr.Find[stAANaturesBlessing].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  | alternate twincast
  /if (!${Me.Buff[${maChr.Find[stTwincast].Value}].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAImprovedTwincast].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  } else /if (!${Me.Buff[Improved Twincast].ID} && !${Me.AltAbilityReady[Improved Twincast]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stTwincast].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  | AA AE heal
  /if (!${Raid.Members}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAANaturesBoon].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}

  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 * note: MGB if both AAs are ready
 * use:
 ***|
sub cast_AA_wood(bool _debug)
  DEBUG \atcast_AA_wood\ax()
  
  /if (${Me.Song[${maChr.Find[stAASpiritoftheWood].Value}].ID}) {
    /return FALSE
  }
  
  /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
    /return FALSE
  }

  /if (!${Me.AltAbilityReady[${maChr.Find[stAASpiritoftheWood].Value}]}) {
    /return FALSE
  }
  
  ISMEDEAD
  CHECKEXIT
  /call cast_data ${_debug} "Mass Group Buff"
  /call cast ${_debug} 0 FALSE
  /delay 5
  /call cast_data ${_debug} "${maChr.Find[stAASpiritoftheWood].Value}"
  /call cast ${_debug} 0 FALSE
  
/return TRUE



|***
 * note: class specific control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | set group Heals
  /if (${_type.Equal[groupheal]}) {
    /if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return FALSE

  | mgb
  } else /if (${_type.Equal[mwood]}) {
    /call cast_AA_wood
    /return TRUE

  | wolf spirit AAs
  } else /if (${_type.Equal[wolf]}) {
    /invoke ${set_switch_env[${_debug}, swWolfSpirit, TRUE, ${_verbage}]}

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE



