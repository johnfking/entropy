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
  /call set_data_list ${_debug} maChr lsOrderClass                    melee|heal|misc|debuff|dot|nuke|item                                                  "${auxli} class active combat order"
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
sub misc_combat(bool _debug, int _tmpID)
  DEBUG \atmisc_combat\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  TMPIDDEAD
  CHECKEXIT

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

  /if (${is_ready[${_debug}, "${maChr.Find[stAASeasonsWrath].Value}"]} && ${madeBuff.Find[malo].Value}) {
    /call voodoo ${_debug} "${maChr.Find[stAASeasonsWrath].Value}" ${_tmpID} FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stAAStormStrike].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAStormStrike].Value}" ${_tmpID} FALSE
  }

  | great wolf self/group aa
  /if (${maChr.Find[swWolfSpirit].Value}) {
    /if (${Me.AltAbilityReady[${maChr.Find[stAASpiritoftheGreatWolf].Value}]} && !${Me.Buff[${maChr.Find[stAASpiritoftheGreatWolf].Value}].ID} && !${Me.Buff[${maChr.Find[stAAGroupSpiritoftheGreatWolf].Value}].ID}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAASpiritoftheGreatWolf].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAASpiritoftheGreatWolf].Value}" 0 FALSE
      }
    } else /if (${Me.AltAbilityReady[${maChr.Find[stAAGroupSpiritoftheGreatWolf].Value}]} && !${Me.AltAbilityReady[${maChr.Find[stAASpiritoftheGreatWolf].Value}]} && !${Me.Buff[${maChr.Find[stAASpiritoftheGreatWolf].Value}].ID} && !${Me.Buff[Group Spirit of the Great Wolf].ID}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAGroupSpiritoftheGreatWolf].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAGroupSpiritoftheGreatWolf].Value}" 0 FALSE
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

  SETHUD "heal ${_type}"

  /call get_hurt ${_debug} ${_type}

  /if (!${hurtCount}) {
    SETHUD EMPTY
    /return
  }

  /if (!SAFEZONE) {

    /if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAASwarmofFireflies].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAASwarmofFireflies].Value}" ${hurtID} FALSE
      }
    }

    /if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAConvergenceofSpirits].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAConvergenceofSpirits].Value}" ${hurtID} FALSE
      }
    }

  }

  /if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

  | Group Heal
  /if (${Select[${_type},group]} && ${maHeal.Find[stCountHealGroup].Value}) {
    /if (${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {

      /if (!SAFEZONE) {

      /if (${hurtPctHP} < ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
        /if (${is_ready[${_debug}, "${maChr.Find[stAAWildtendersSurvival].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAAWildtendersSurvival].Value}" 0 FALSE
        }
      }

        /if (${is_ready[${_debug}, "${maChr.Find[stAAPeacefulSpiritoftheWood].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAAPeacefulSpiritoftheWood].Value}" ${Me.ID} FALSE
          /return
        }

        /if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

        /if (${is_ready[${_debug}, "${maChr.Find[stAABlessingofTunare].Value}"]}) {
          /call voodoo ${_debug} "${maChr.Find[stAABlessingofTunare].Value}" 0 FALSE
          /return
        }
      }

      /if (!${get_hurt[${_debug}, ${_type}]}) /return FALSE

      | group heal?
      /if (${maHeal.Find[stCountHealGroup].Value} && ${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
        /call cast_cycle_heal ${_debug} 0 group
      }

    }
  }

  /if (!${get_hurt[${_debug}, ${_type}]}) {
    /return FALSE
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT
  CHECKREZ
  CHECKFADE
  GETINPUT dru_heal_end type|${_type}

  /call cast_cycle_heal ${_debug} ${hurtID} single

  SETHUD EMPTY

/return TRUE



|***
 * note: Druid burn routines
 * use: dpsing the things
 ***|
sub burn(bool _debug, int _tmpID)
  DEBUG \atburn\ax()

  ISMEDEAD
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

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

  | swarm pets
  /if (SWARM) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAANaturesGuardian].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAANaturesGuardian].Value}" ${_tmpID} FALSE
    }
  }

  | defense
  /if (${is_ready[${_debug}, "${maChr.Find[stAAProtectionofDirewood].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAAProtectionofDirewood].Value}" 0 FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAASilentCasting].Value}" 0 FALSE
  }

  | damage boost
  /if (${is_ready[${_debug}, "${maChr.Find[stAADestructiveVortex].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAADestructiveVortex].Value}" 0 FALSE
  }

  /if (${is_ready[${_debug}, "${maChr.Find[stAANaturesFury].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAANaturesFury].Value}" 0 FALSE
  }

  | heal boost
  /if (${is_ready[${_debug}, "${maChr.Find[stAANaturesBlessing].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[stAANaturesBlessing].Value}" 0 FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  | AA AE heal
  /if (!${Raid.Members}) {
    /if (${is_ready[${_debug}, "${maChr.Find[stAANaturesBoon].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stAANaturesBoon].Value}" 0 FALSE
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
  /call voodoo ${_debug} "Mass Group Buff" 0 FALSE
  /delay 5
  /call voodoo ${_debug} "${maChr.Find[stAASpiritoftheWood].Value}" 0 FALSE
  
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



