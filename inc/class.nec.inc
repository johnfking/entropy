|***
 * entropy
 * class.nec.inc
 ***|



|***
 * note: primary initilization of all necromancer non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data FALSE maData stSpire                                 map outer "Spire of Necromancy"
  /call set_data FALSE maData stSynergy                               map outer "Defiler's Synergy"
  /call set_data FALSE maData stFade                                  map outer "A Hole in Space"


  /call set_data_map ${_debug} maChr stNukeServant                    PREGEN|1                      "${auxna} spam nuke pet spell"
  /call set_data_list ${_debug} maChr lsHealPetMend                   PREGEN|3                      "${auxli} mending spells for pet"
  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell"
  /call set_data_map ${_debug} maChr stDichotomic                     PREGEN|1                      "${auxna} dichotomic spell"

  /invoke ${set_data_timer[${_debug}, Check_Blood, 8s]}
  /call set_data_map ${_debug} maChr swBlood                          FALSE                         "${auxsw} use  blood spell"
  /call set_data_map ${_debug} maChr stBlood                          PREGEN|1                      "${auxna} name of current call for blood spell"

  /call set_data_map ${_debug} maChr stPctBloom                       0                             "${auxnum} % of manna you want to use bloom at"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                checkminion|manarecover|gather                                                        "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 EMPTY                                                                                 "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    pet|melee|debuff|burn|swarm|dot|nuke|misc|item                                        "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|gather                                                                    "${auxli} class post combat "

  | AAs
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                    "${auxna} class aa unity buff"
  /call set_data_map ${_debug} maChr stAADyingGrasp                   PREGEN|1                    "${auxna} Dying Grasp AA"
  /call set_data_map ${_debug} maChr stAAMendCompanion                PREGEN|1                    "${auxna} Mend Companion AA"
  /call set_data_map ${_debug} maChr stAAWaketheDead                  PREGEN|1                    "${auxna} Wake the Dead AA"
  /call set_data_map ${_debug} maChr stAASwarmofDecay                 PREGEN|1                    "${auxna} Swarm of Decay AA"
  /call set_data_map ${_debug} maChr stAARiseofBones                  PREGEN|1                    "${auxna} Rise of Bones AA"
  /call set_data_map ${_debug} maChr stAAHandofDeath                  PREGEN|1                    "${auxna} Hand of Death AA"
  /call set_data_map ${_debug} maChr stAAHereticsTwincast             PREGEN|1                    "${auxna} Heretic's Twincast AA"
  /call set_data_map ${_debug} maChr stAAFocusofArcanum               PREGEN|1                    "${auxna} Focus of Arcanum AA"
  /call set_data_map ${_debug} maChr stAAMercurialTorment             PREGEN|1                    "${auxna} Mercurial Torment AA"
  /call set_data_map ${_debug} maChr stAAFuneralPyre                  PREGEN|1                    "${auxna} Funeral Pyre AA"
  /call set_data_map ${_debug} maChr stAAEmbalmersCarapace            PREGEN|1                    "${auxna} Embalmer's Carapace AA"
  /call set_data_map ${_debug} maChr stAALifeBurn                     PREGEN|1                    "${auxna} Life Burn AA"
  /call set_data_map ${_debug} maChr stAACompanionsFury               PREGEN|1                    "${auxna} Companion's Fury AA"
  /call set_data_map ${_debug} maChr stAADeathBloom                   PREGEN|1                    "${auxna} Death Bloom AA"


  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return



|***
 * note: Main Necro Routine
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
      /if (${maChr.Find[swBlood].Value} && !${timer_Check_Blood}) /call cast_blood ${maDebug.Find[blood].Value} GETASSIST
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
 * note: why are we healing the dead?
 * use:
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKEXIT

  /call spawn_hurt ${maDebug.Find[hurt].Value} ${_type}

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

      | aa Dying grasp
      /if (${Me.PctHPs} <= ${${Me.Class.ShortName}Point}*.PCTHO) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAADyingGrasp].Value}"]}) {
          /call cast ${_debug} ${_type} FALSE
        }
      }

    }

    /if (${Select[${_type},pet]} && ${hurtCount}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAMendCompanion].Value}"]}) {
        /call cast ${_debug} ${Pet.ID} FALSE
      }
    }
  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

  SETHUD EMPTY

/return tRUE



|***
 * note: the extra stuff
 * use: normal routine
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

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
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  | aa wake the dead
  /if (SWARM) {
    | wake the dead
    /if (${SpawnCount[corpse radius ${Spell[${maChr.Find[stAAWaketheDead].Value}].MyRange}]} > 3) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAWaketheDead].Value}"]} && ${Target.PctHPs} > 15) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
    }
    | aa swarm of decay
    /if (${cast_data[${_debug}, "${maChr.Find[stAASwarmofDecay].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
    | aa Rise of bones
    /if (${cast_data[${_debug}, "${maChr.Find[stAARiseofBones].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAHereticsTwincast].Value}"]} && !${Me.Song[${maChr.Find[stAAFuneralPyre].Value}].ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

/return TRUE



|***
 * note: “You should never attack a necromancer in a cemetery; it’s like chasing Rambo into a building full of loaded guns." --Laurell K. Hamilton, Dead Ice
 * use: burn the dead
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

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

  | Pet Burnage
  /if (PET && ${Me.Pet.ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAACompanionsFury].Value}"]}) {
      /call cast ${_debug} ${Me.Pet.ID} FALSE
    }
  }

  /if (${Target.PctHPs} > 15 && ${Me.PctHPs} > 90) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAALifeBurn].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (!${Me.Buff[Spire of Necromancy].ID}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAEmbalmersCarapace].Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
    }
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAFuneralPyre].Value}"]} && !${Me.Song[${maChr.Find[stAAHereticsTwincast].Value}].ID}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAMercurialTorment].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusofArcanum].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAHereticsTwincast].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }


  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return TRUE



|***
 * note: casts call of blood line of spells
 * use: /call cast_blood DEBUG [ID]
 ***|
sub cast_blood(bool _debug, spawn _spawn)
  DEBUG \atcast_blood\ax(\a-w${_spawn.ID}\ax)

  CHECKEXIT
  GETINPUT

  /invoke ${set_data_timer[${_debug}, Check_Blood, RESTART]}
  /if (${Me.Song[Chaotic Power].ID}) /return FALSE

  SPAWNDEAD


  /if (${cast_data[${_debug}, "${maChr.Find[stBlood].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

/return TRUE



|***
 * note: We want Mana
 * use: /chr gather %##
 ***|
sub check_gather(bool _debug)
  DEBUG \atcheck_gather\ax()

  /if (${Zone.ID} == ${Me.BoundLocation[0].ID} || SAFEZONE) {
    /return FALSE
  }

  CHECKEXIT
  GETINPUT

  /if (${cast_data[${_debug}, "Death Bloom"]}) {
    /call cast ${_debug} 0 FALSE
  }

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | use blood nuke line
  /if (${_type.Equal[blood]}) {
    /invoke ${set_switch_env[${_debug}, swBlood, TRUE, ${_verbage}]}

    /if (${_verbage.Equal[name]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stBlood, "${_verbage2}"]}
    } else /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swBlood, TRUE, ${_verbage2}]}
    }
    /invoke ${maChrControl.Clear}
    /invoke ${maChrControl.Add[name,stBlood|br]}
    /invoke ${maChrControl.Add[active,swBlood]}
    /invoke ${set_control_output[${_debug}, maHeal, "chr blood", maChrControl]}
    /return TRUE

  | use bloom mana recovery
  } else /if (${_type.Equal[gather]}) {
    /if (!${set_control_num_range[${_debug}, stPctBloom, "${_verbage}", 0, 99]}) /return FALSE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE