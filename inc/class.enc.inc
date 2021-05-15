|***
 * entropy
 * class.enc.inc
 ***|



|***
 * note: primary initilization of all enchanter non shared data
 * use: not for you
 ***|
sub class_init(bool _debug)
  DEBUG \atclass_init\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of Enchantment"
  /call set_data ${_debug} maData stSynergy                           map outer "Begulier's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Self Stasis"

  /call set_data_map ${_debug} maChr stPctGather                      0                             "${auxnum} % of your mana to usin gather mana AA and crystal if enabled"

  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance spell"

  /call set_data_map ${_debug} maChr swAABoS                          FALSE                         "${auxsw} use aa Beam of Slumber in your mez routines"

  | single target charm
  /call set_data_map ${_debug} maChr stCharm                          PREGEN|1                      "${auxna} name of your charm spell"
  /call set_data_map ${_debug} maChr swCharm                          FALSE                         "${auxsw} enable using charm"

  /call set_data_map ${_debug} maChr stPctGather                      0                             "${auxnum} % of manna you want to use gather at"

  | aa crystals
  /invoke ${set_data_timer[${_debug}, Check_Crystals, 10s]}
  /call set_data_map ${_debug} maChr swAACrystals                     FALSE                         "${auxsw} use aa crystals:\a-wAzure Mind Crystal, Sanguine Mind Crystal/ax"

  | PBAE stun
  /call set_data_map ${_debug} maChr stAEStun                         PREGEN|1                      "${auxna} name of your point blank ae stun"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                gather|checkminion                        "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 cc|manarecover                            "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    pet|cc|melee|debuff|cc|heal|burn|misc|cc|dot|cc|nuke|cc|alliance|item      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover|gather                        "${auxli} class post combat "

  | AAs
  /call set_data_map ${_debug} maChr stAABeamofSlumber                PREGEN|1                      "${auxna} Beam of Slumber AA"
  /call set_data_map ${_debug} maChr stAAPhantasmalOpponent           PREGEN|1                      "${auxna} Phantasmal Opponent AA"
  /call set_data_map ${_debug} maChr stAAChromaticHaze                PREGEN|1                      "${auxna} Chromatic Haze AA"
  /call set_data_map ${_debug} maChr stAAAzureMindCrystal             PREGEN|1                      "${auxna} Azure Mind Crystal AA"
  /call set_data_map ${_debug} maChr stAASanguineMindCrystal          PREGEN|1                      "${auxna} Sanguine Mind Crystal AA"
  /call set_data_map ${_debug} maChr stAAGatherMana                   PREGEN|1                      "${auxna} Gather Mana AA"
  /call set_data_map ${_debug} maChr stAAIllusionsofGrandeur          PREGEN|1                      "${auxna} Illusions of Grandeur AA"
  /call set_data_map ${_debug} maChr stAAFocusofArcanum               PREGEN|1                      "${auxna} Focus of Arcanum AA"
  /call set_data_map ${_debug} maChr stAADimensionalShield            PREGEN|1                      "${auxna} Dimensional Shield AA"
  /call set_data_map ${_debug} maChr stAASilentCasting                PREGEN|1                      "${auxna} Silent Casting AA"
  /call set_data_map ${_debug} maChr stAAImprovedTwincast             PREGEN|1                      "${auxna} Improved Twincast AA"
  /call set_data_map ${_debug} maChr stAAMentalContortion             PREGEN|1                      "${auxna} Mental Contortion AA"
  /call set_data_map ${_debug} maChr stAADoppelganger                 PREGEN|1                      "${auxna} Doppelganger AA"
  /call set_data_map ${_debug} maChr stAACalculatedInsanity           PREGEN|1                      "${auxna} Calculated Insanity AA"
  /call set_data_map ${_debug} maChr stAAUnity                        PREGEN|1                      "${auxna} class aa unity buff"

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE




|***
 * note: main enchanter routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()


  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase
    
    /if (${maChr.Find[swAACrystals].Value} && !${timer_Check_Crystals}) {
      /call check_crystals ${maDebug.Find[crystal].Value}
    }
    /if (${maChr.Find[stPctGather].Value}) {
      /call check_gather ${maDebug.Find[gather].Value}
    }
    /if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) {
      /call check_minion_cycle ${maDebug.Find[petbuild].Value}
    }
  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_pre_${Me.Class.ShortName}]}) {
      /call e3_pre_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }
  }

  /while (${check_combat_status[${maDebug.Find[status].Value}]}) {
    GETINPUT
    CHECKREZ
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
  }

  /if (AUTO && !ENGINE3) {
    /if (${maChr.Find[swAACrystals].Value} && !${timer_Check_Crystals}) {
      /call check_crystals ${maDebug.Find[crystal].Value}
    }
    /call class_cycle lsOrderClassPost

  } else /if (AUTO && ENGINE3) {
    /if (${SubDefined[e3_post_${Me.Class.ShortName}]}) {
      /call e3_post_${Me.Class.ShortName} ${maDebug.Find[e3].Value}
    }
  }

/return TRUE




|***
 * note: basic DPS discs
 * use:
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  /call target assist ${_spawn.ID}

  CHECKEXIT misc_combat
  GETINPUT
  CHECKTIE
  CHECKDEFENSE
  BUFFNOW
  BUFFTEMP

  /if (SWARM || PET || ${maMinion.Find[swPetCharm].Value}) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  | use AA beam of slumber?
  /if (${maChr.Find[swAABoS].Value} && ${madeBuff.Find[swMez].Value}) {
    /if (${_spawn.NearestSpawn[3,npc radius 30].ID} && ${_spawn.Distance3D} <= 200 && ${Spawn[TARGET].Distance3D} >= ENV*.5) {
      /if (${Me.AltAbilityReady[${maChr.Find[stAABeamofSlumber].Value}]}) {
        /if (${Me.Casting.ID}) /stopcast
        /face ID ${_spawn.ID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
        /if (${cast_data[${_debug}, "${maChr.Find[stAABeamofSlumber].Value}"]}) {
          /call cast ${_debug} 0 FALSE
        }
      }
    }
  }

  SPAWNDEAD

  | swarm pet
  /if (SWARM && AGRO) {
    /if (${Target.PctHPs} <= ${maMinion.Find[stPctSwarmEngage].Value}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAAPhantasmalOpponent].Value}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }
  }

  | group DPS bonus
  /if (${cast_data[${_debug}, "${maChr.Find[stAAChromaticHaze].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

/return TRUE



|***
 * note: Keep the chanter stocked with self AA crystals
 * use: /call check_crystals DEBUG
 ***|
sub check_crystals(bool _debug)
  DEBUG \atcheck_crystals\ax()

  /invoke ${set_data_timer[FALSE, Check_Crystals, RESTART]}

  ISMEDEAD
  CHECKEXIT check_crystals
  GETINPUT
  CHECKTIE

  | mana crystal
  /if (!${FindItemCount[${Spell[${maChr.Find[stAAAzureMindCrystal].Value}].Base[1]}]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAAAzureMindCrystal].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /autoinventory
    CHECKCURSOR FALSE clear
  }

  | hp crystal
  /if (!${FindItemCount[${Spell[${maChr.Find[stAASanguineMindCrystal].Value}].Base[1]}]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAASanguineMindCrystal].Value}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /autoinventory
    CHECKCURSOR FALSE clear
  }

/return TRUE



|***
 * note: needs manna!!!
 * use: /call check_gather DEBUG
 ***|
sub check_gather(bool _debug)
  DEBUG \atcheck_gather\ax()

  /if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) /return FALSE

  ISMEDEAD
  CHECKEXIT check_gather_ENC
  GETINPUT
  CHECKTIE

  | AA manna crystal
  /if (${maChr.Find[swAACrystals].Value}) {
    /if (${cast_data[${_debug}, "${FindItem[${Spell[Azure Mind Crystal].Base[1]}].Name}"]} && ${Spell[${FindItem[Azure Mind Crystal].Spell}].Base[1]} < ${Math.Calc[${Me.MaxMana}-${Me.CurrentMana}].Int} && ${Me.CurrentMana} != ${Me.MaxMana}) {
      /call cast ${_debug} 0 FALSE
    }
  }

  /if (${Me.PctMana} >= ${maChr.Find[stPctGather].Value}*.PCTHO) /return FALSE

  | aa Gather Mana
  /if (${cast_data[${_debug}, "${maChr.Find[stAAGatherMana].Value}"]} && ${AltAbility[${maChr.Find[stAAGatherMana].Value}].Spell.Base[1]} < ${Math.Calc[${Me.MaxMana}-${Me.CurrentMana}].Int} && ${Me.CurrentMana} != ${Me.MaxMana}) {
    /call cast ${_debug} 0 FALSE
  }

/return TRUE



|***
 * note: chanter self healing? like this is really gonna work. wtf
 * use: /call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

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
  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }


  /if (${Select[${_type},self]}) {
    /if (${Me.PctHPs} > ${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO) {

      | AA Sanguine mind crystal summoned item 5 min
      /if (${maChr.Find[swAACrystals].Value}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAASanguineMindCrystal].Value}"]}) {
          /call cast ${_debug} 0 FALSE
        }
      }

      | rune and anti hate 20 min
      /if (${cast_data[${_debug}, "${maChr.Find[stAADimensionalShield].Value}"]}) {
        /call cast ${_debug} 0 FALSE
        /return
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
 * note: toast should not be burnt
 * use: establish all comp/trigger skills
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  SPAWNDEAD

  CHECKEXIT
  GETINPUT
  CHECKTIE

  /if (SWARM || PET || ${maMinion.Find[swPetCharm].Value}) {
    /call send_pet ${maDebug.Find[sic].Value} ${_spawn.ID}
  }
  /if (AUTO && MELEE) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /if (ENGINE2) {
    /call cast_cycle_burn ${maDebug.Find[burn].Value} ${_spawn.ID}
    /return TRUE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAIllusionsofGrandeur].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAMentalContortion].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  SPAWNDEAD

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (${cast_data[${_debug}, "${maChr.Find[stAAImprovedTwincast].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | # DD spell damage bonus
  /if (${cast_data[${_debug}, "${maChr.Find[stAACalculatedInsanity].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  SPAWNDEAD

  /if (${cast_data[${_debug}, "${maChr.Find[stAASilentCasting].Value}"]}) {
    /call cast ${_debug}  0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAADimensionalShield].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | resist mod
  /if (${cast_data[${_debug}, "${maChr.Find[stAAFocusofArcanum].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  | Group DD Bonus
  /if (${cast_data[${_debug}, "${maChr.Find[stAAChromaticHaze].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  SPAWNDEAD

  /call cast_glyph ${maDebug.Find[glyph].Value}
  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

/return



|***
 * note: do people still really do this shit?
 * use: /call find_mob_to_charm()
 ***
sub find_mob_to_charm(bool _debug)
  DEBUG \a-pfind_mob_to_charm\ax()

  /if (${Me.Pet.ID}) {
    /return TRUE
  }


  /declare _count int local 0
  /declare _spawns int local 0
  /declare _spawn.ID int local 0
  /declare _maxlvl int local ${Spell[${maChr.Find[stCharm].Value}].Max[1]}

  /delay 1

  /if (${SpawnCount[npc los Range 1 ${_maxlvl} radius ENV targetable]}) {
    /varset _spawns ${SpawnCount[npc los Range 1 ${_maxlvl} radius ENV targetable]}
    /for _count 1 to ${_spawns}
      /varset _spawn.ID ${NearestSpawn[${_count}, npc los Range 1 ${_maxlvl} radius ENV targetable].ID}
      /if (${_spawn.ID} == TARGET) /continue
      /if (${lsZoneCharm.Contains[${Spawn[id ${_spawn.ID}].DisplayName}]} && !${Spawn[${_spawn.ID}].Master.Type.Equal[Pet]} && !${Spawn[${_spawn.ID}].Master.Type.Equal[Corpse]}) {

        /squelch /target ID ${_spawn.ID}
        /if (!${Target.Tashed.Duration}) {
          /call voodoo ${_debug} {madeBuff.Find[stTash].Value} ${_spawn.ID} FALSE

          /while (${Me.SpellInCooldown}) {
            CHECKEXIT
            ISMEDEAD
            /if (!${Me.SpellInCooldown}) /break
            /delay 5
          }

        | /if (${cast_data[${_debug}, spell, "${spellpbaeMez}", ${Me.ID}]}) /call voodoo ${_debug} "${spellpbaeMez}" 0 check_for_adds

        /if (${Target.Level} <= 46) {
          /if (${cast_data[${_debug}, "Dire Charm"]}) {
            /call voodoo ${_debug} "Dire Charm" ${_spawn.ID} FALSE
          }
        } else {
          /if (${cast_data[${_debug}, "${maChr.Find[stCharm].Value}"]}) {
            /call voodoo ${_debug} "${maChr.Find[stCharm].Value}" ${_spawn.ID} FALSE
          }
        }

        /break
      }
    /next _count
  }
/return
**|


|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | set gather use %
  /if (${_type.Equal[gather]}) {
    /if (!${set_control_num_range[${_debug}, stPctGather, "${_verbage}", 0, 99]}) /return FALSE

  | use AA crystals
  } else /if (${_type.Equal[crystal]}) {
    /invoke ${set_switch_env[${_debug}, swAACrystals, TRUE, ${_verbage}]}

  | use AA Beam of Slumber
  } else /if (${_type.Equal[bos]}) {
    /invoke ${set_switch_env[${_debug}, swAABoS, TRUE, ${_verbage}]}

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE



