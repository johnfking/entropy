|***
 * entropy
 * class.brd.inc
 ***|



|***
 * note: primary initilization of all non shared data
 * use: god i hate bards...
 ***|
sub class_init(bool _debug)
  DEBUG \atset_BRD\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /call set_data ${_debug} maData stSpire                             map outer "Spire of the Minstrels"
  /call set_data ${_debug} maData stSynergy                           map outer "Troubadour's Synergy"
  /call set_data ${_debug} maData stFade                              map outer "Fading Memories"
  /call set_data_map ${_debug} maChr stAAUnity                        FALSE                         "${auxna} class aa unity buff"
  /call set_data_map ${_debug} maChr stAlliance                       PREGEN|1                      "${auxna} alliance song"
  /call set_data_map ${_debug} maChr stNukeServant                    PREGEN|1                      "${auxna} spam pet song"

  | map for mob control
  /if (!${Defined[maMobControl]}) {
    /declare maMobControl map outer
  }

  /call set_data_map ${_debug} maChr swSongRest                       FALSE                         "${auxna} enable song while resting"
  /call set_data_map ${_debug} maChr stSongRest                       FALSE                         "${auxna} song to use while group resting"
  /call set_data_map ${_debug} maChr stSongRefresh                    6                             "${auxnum} remaining duration on a song to refresh. in seconds"

  | remez time
  /call set_data_map ${_debug} maChr streMezTime                      180                           "${auxnum} min mez time in seconds left to remez"

  | current song line
  /call set_data_list ${_debug} maSong lsLineSong                     EMPTY                         "${auxli} list of song numbers to sing"  song

  | aa boastful bellow switch
  /call set_data_map ${_debug} maChr swAABellow                       FALSE                         "${auxsw} enable/disable using AA Boastful Bellow"
  /call set_data_map ${_debug} maChr swVainglorious                   FALSE                         "${auxsw} enable/disable using Vainglorious"

  | sing OOC
  /call set_data_map ${_debug} maChr swPulseOOC                       FALSE                         "${auxna} sing while out of combat"

  | swap instruments
  /call set_data_map ${_debug} maChr swInstrumentSwap                 FALSE                         "${auxsw} enable swapping instruments"
  /call set_data_map ${_debug} maChr stInstrumentSecondary            FALSE                         "${auxna} name of secondary weapon to return from swap"
  /call set_data_map ${_debug} maChr stInstrumentPrimary              FALSE                         "${auxna} name of primary weapon to return from swap"

  | reflexive
  /call set_data_map ${_debug} maChr stReflexive                      PREGEN|1                      "${auxna} Reflexive disc line"
  /call set_data_map ${_debug} maChr stThousandBlades				          PREGEN|1						          "${auxna} Thousand Blades disc"

  | class order lists
  /call set_data_list ${_debug} maChr lsOrderClassBase                EMPTY                                    "${auxli} class base loop start"
  /call set_data_list ${_debug} maChr lsOrderClassPre                 cc                                       "${auxli} class pre combat"
  /call set_data_list ${_debug} maChr lsOrderClass                    cc|melee|debuff|burn|misc|pet|swarm|alliance|dot|pulseic|nuke|item      "${auxli} class active combat order"
  /call set_data_list ${_debug} maChr lsOrderClassPost                manarecover                              "${auxli} class post combat "


  | AAs
  /call set_data_map ${_debug} maChr stAAFierceEye                    PREGEN|1                      "${auxna} Fierce Eye aa"
  /call set_data_map ${_debug} maChr stAABladedSong                   PREGEN|1                      "${auxna} Bladed Song aa"
  /call set_data_map ${_debug} maChr stAACacophony                    PREGEN|1                      "${auxna} Cacophony aa"
  /call set_data_map ${_debug} maChr stAABoastfulBellow               PREGEN|1                      "${auxna} Boastful Bellow aa"
  /call set_data_map ${_debug} maChr stAALyricalPrankster             PREGEN|1                      "${auxna} Lyrical Prankster aa"
  /call set_data_map ${_debug} maChr stAASongofStone                  PREGEN|1                      "${auxna} Song of Stone aa"
  /call set_data_map ${_debug} maChr stAARallyingSolo                 PREGEN|1                      "${auxna} Rallying Solo aa"
  /call set_data_map ${_debug} maChr stAAFuneralDirge                 PREGEN|1                      "${auxna} Funeral Dirge aa"
  /call set_data_map ${_debug} maChr stAAQuickTime                    PREGEN|1                      "${auxna} Quick Time aa"
  /call set_data_map ${_debug} maChr stAAFrenziedKicks                PREGEN|1                      "${auxna} Frenzied Kicks aa"
  /call set_data_map ${_debug} maChr stAASelosSonata                  PREGEN|1                      "${auxna} Selo's Sonata aa"
  /call set_data_map ${_debug} maChr stAAShaurisSonorousClouding      PREGEN|1                      "${auxna} Shauri's Sonorous Clouding aa"
  /call set_data_map ${_debug} maChr stAAFlurryofNotes				        PREGEN|1						          "${auxna} Flurry of Notes aa"
  /call set_data_map ${_debug} maChr stAADanceofBlades				        PREGEN|1						          "${auxna} Dance of Blades aa"
  /call set_data_map ${_debug} maChr stAAVaingloriousShout  	        PREGEN|1						          "${auxna} Vainglorious Shout aa"

  | set base instrument focuses
  /if (!${Defined[maBardFocus]}) {
    /declare maBardFocus map outer
  }
  /call scan_focus FALSE ${maDebug.Find[scan].Value}

  /if (${maDebug.Find[init].Value}) {
    OUT \agset_${Me.Class.ShortName}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE




|***
 * note: main class routine
 * use:
 ***|
sub class_main(bool _debug)
  DEBUG \atclass_main\ax()

  /if (AUTO && !ENGINE3) {
    /call class_cycle lsOrderClassBase
    /if (${maChr.Find[swPulseOOC].Value} && !${Select[${Me.CombatState},combat]}) {
      /call pulse_cycle ${maDebug.Find[pulse].Value} 0 pulseooc
    }
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
 *  DES: Combat skills, Disciplines and Alternate abilities.
 *  USE: Disc/aas and stuffs
 ***|
sub misc_combat(bool _debug, spawn _spawn)
  DEBUG \atmisc_combat\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD

  /if (${target[isdead, ${_spawn.ID}]}) {
    /call clear_combat
    /call pulse_stop
    /return
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

  /if (ENGINE2) {
    /call cast_cycle_miscdps ${maDebug.Find[agro].Value} ${_spawn.ID}
    /return TRUE
  }

  /if (AUTO) {
    /invoke ${spawn_face[${_spawn.ID}]}
    /call spawn_stick ${_debug} ${_spawn.ID}
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAAFierceEye].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stReflexive].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAABladedSong].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAACacophony].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

  /if (${maChr.Find[swAABellow].Value} && ${Target.BuffsPopulated}) {
    /if (${_spawn.PctHPs} > 30 && ${Me.PctEndurance} > 30 && !${Target.MyBuff[Boastful Bellow].ID} && !${Target.Mezzed.ID}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stAABoastfulBellow].Value}"]}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
      }
    }
  }
  
  /if (${Bool[${maChr.Find[stAAVaingloriousShout].Value}]} && ${maChr.Find[swVainglorious].Value} && ${Target.BuffsPopulated}) {
    /if (${madeBuff.Find[swMez].Value} && AE#) {
      | output a message warning that you have mezzing enabled and an AE count and vainglorious will hit them all
      /if (${_spawn.PctHPs} > 3 && ${Me.CurrentEndurance} > 10500 && !${Target.MyBuff[Vainglorious Shout].ID}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAAVaingloriousShout].Value}"]}) {
          /call cast ${_debug} ${_spawn.ID} FALSE
        }
      }
    } else /if (!${madeBuff.Find[swMez].Value} && AE#) {
      /if (${_spawn.PctHPs} > 3 && ${Me.CurrentEndurance} > 10500 && !${Target.MyBuff[Vainglorious Shout].ID}) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAAVaingloriousShout].Value}"]}) {
          /call cast ${_debug} ${_spawn.ID} FALSE
        }
      }
    }
  }


  /if (${target[isdead, ${_spawn.ID}]}) {
    /call pulse_stop
    /call clear_combat
    /return FALSE
  }  
  
/return TRUE



|***
 * note: self healing
 * use: /call check_heal
 ***|
sub check_heal(bool _debug, string _type)
  DEBUG \atcheck_heal\ax(\a-w${_type}\ax)

  CHECKTIE
  ISMEDEAD
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

  }

  /if (!${spawn_hurt[${maDebug.Find[hurt].Value}, ${_type}]}) {
    /return FALSE
  }

  GETINPUT ${Me.Class.ShortName}_heal_end type|${_type}

  /call cast_cycle_heal ${_debug} ${hurtData.ID} single

  SETHUD

/return TRUE



|***
 * note: rotates bard swarm pets as DPS fillers
 * use: /call cast_servant_swarm DEBUG [spawn]
 ***|
sub cast_servant_swarm(bool _debug, spawn _spawn)
  DEBUG \atcast_servant_swarm\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  CHECKEXIT
  GETINPUT
  /if (${target[isdead, ${_spawn.ID}]}) {
    /call pulse_stop
    /call clear_combat
    /return FALSE
  }

  /if (AUTO) {
    /invoke ${spawn_face[${_spawn.ID}]}
    /call spawn_stick ${_debug} ${_spawn.ID}
  }

  /if (${Target.ID}) /pet swarm

  /if (${cast_data[${_debug}, "${maChr.Find[stAALyricalPrankster].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
    /return
  }

  /if (${cast_data[${_debug}, "${maChr.Find[stAASongofStone].Value}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
    /return
  }

  /call cast_temp_pet FALSE ${_spawn.ID} "${maChr.Find[stNukeServant].Value}"

/return TRUE



|***
 * note: sings the line of songs for group support
 * use: /call pulse_cycle DEBUG [spawn] tag
 ***|
sub pulse_cycle(bool _debug, spawn _spawn, string _tag)
  DEBUG \atpulse_cycle\ax(\a-w${_spawn.ID}, ${_tag}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineSong.Contains[EMPTY]} && ${lsLineSong.Count} == 1) {
    DEBUG ${sep}resetting the empty line
    /invoke ${lsLineSong.Clear}
    /invoke ${lsLineSong.Append[${stCount20}]}
  }

  /declare _lipl listiterator local
  /vardata _lipl lsLineSong.First.Clone
  /while (!${_lipl.IsEnd}) {

    | check the switch
    /if (!${maSong.Find[swSong${_lipl.Value}].Value}) {
      DEBUG ${sep}switch off${sep}\a-w${maSong.Find[swSong${_lipl.Value}].Value}\ax
      /invoke ${_lipl.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maSong.Find[stSong${_lipl.Value}].Value}]}) {
      DEBUG ${sep}no name${sep}\a-w${maSong.Find[stSong${_lipl.Value}].Value}\ax
      /invoke ${_lipl.Advance}
      /continue
    }

    | check the tag passed
    /if (${Bool[${_tag}]}) {
      /if (!${lsSong${_lipl.Value}Tag.Contains[${_tag}]}) {
        /invoke ${_lipl.Advance}
        /continue
      }
    }

    | buff has time left
    /if (${Me.Buff[${Spell[${maSong.Find[stSong${_lipl.Value}].Value}].RankName}].Duration.TotalSeconds} > ${maChr.Find[stSongRefresh].Value}) {
      DEBUG ${break}${Me.Buff[${Spell[${maSong.Find[stSong${_lipl.Value}].Value}].RankName}].Duration.TotalSeconds} > ${maChr.Find[stSongRefresh].Value} (refresh: buff)
      /invoke ${_lipl.Advance}
      /continue
    }

    | song has time left
    /if (${Me.Song[${Spell[${maSong.Find[stSong${_lipl.Value}].Value}].RankName}].Duration.TotalSeconds} > ${maChr.Find[stSongRefresh].Value}) {
      DEBUG ${break}${Me.Song[${Spell[${maSong.Find[stSong${_lipl.Value}].Value}].RankName}].Duration.TotalSeconds} > ${maChr.Find[stSongRefresh].Value} (refresh: song)
      /invoke ${_lipl.Advance}
      /continue
    }

    | ready
    /if (!${cast_data[${_debug}, "${maSong.Find[stSong${_lipl.Value}].Value}"]}) {
      DEBUG ${sep}not ready${sep}\a-w${maSong.Find[stSong${_lipl.Value}].Value}\ax
      /invoke ${_lipl.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Song ${_lipl.Value} ${Me.DisplayName} "${maSong.Find[stSong${_lipl.Value}].Value}" pulse_cycle
    /if (!${Macro.Return}) {
      DEBUG ${break}condition${sep}Buff${_lipl.Value}
      /invoke ${_lipl.Advance}
      /continue
    }

    /if (${_spawn.ID}) {

      /if (MELEE) {
         /call set_combat_control ${maDebug.Find[decision].Value} ${_spawn.ID}
      }

      /if (AUTO && ${_spawn.ID}) {
        /invoke ${spawn_face[${_spawn.ID}]}
        /call spawn_stick ${_debug} ${_spawn.ID}
      }
    }    

    SETHUD "pulse"

    /call cast ${_debug} 0 ignoremovement
    
    /if (${Macro.Return}) {
      /return TRUE
    }

    | /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,RESISTED,IMMUNE]}) {
    |   /return TRUE
    | }

    /invoke ${_lipl.Advance}

  }

  SETHUD

/return TRUE



|***
 * note: would rather take a cheese grater to my nuts then listen to you
 * use: /call pulse_stop [useful tag as a marker for debug] DEBUG
 ***|
sub pulse_stop(string _marker, bool _debug)
  DEBUG \atpulse_stop\ax(marker:\aw${_marker}\aw)

  /while (${Me.Casting.ID} || ${Me.BardSongPlaying}) {
    /invoke ${maData.Add[stCastReturn,FALSE]}
    /stopsong
    /stopcast
    /delay 1 !${Me.Casting.ID}
  }

/return TRUE



|***
 * note: Bard AA version of Breather
 * use: /call cast_AA_rallying DEBUG
 *
 *  AA - Rallying Solo
 *  Song - Rallying Solo
 ***|
sub cast_AA_rallying(bool _debug)
  DEBUG \atcast_AA_rallying\ax()

  /if (SAFEZONE || !${Me.AltAbilityReady[${maChr.Find[stAARallyingSolo].Value}]}) {
    /return FALSE
  }

  ISMEDEAD
  GETINPUT

  /call pulse_stop FALSE

  /declare _endurancePoint int local 30
  /declare _manaPoint int local 30

  /if (((${Me.PctEndurance} < ${_endurancePoint}) && (${Me.PctMana} < ${_manaPoint})) && ${Select[${Me.CombatState},resting]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stAARallyingSolo].Value}"]}) {
      /call cast ${_debug} ${Me.ID} FALSE
    }
  }

/return TRUE



|***
 * note: Bard burn routines
 * use: establish all comp/trigger skills
 ***|
sub burn(bool _debug, spawn _spawn)
  DEBUG \atburn\ax(\a-w${_spawn.ID}\ax)

  ISMEDEAD
  /if (${target[isdead, ${_spawn.ID}]}) {
    /call pulse_stop
    /call clear_combat
    /return FALSE
  }

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

  /if (${cast_data[${_debug}, "${maChr.Find[stThousandBlades].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /call cast_AA_spire ${maDebug.Find[spire].Value}

  /if (${cast_data[${_debug}, "${maChr.Find[stAAFuneralDirge].Value}"]} && !${Target.Buff[Funeral Dirge].ID}) {
    /call cast ${_debug}  0 FALSE
  }

  | this is disabled until the stuck gem bug is fixed
  | /if (${cast_data[${_debug}, "${maChr.Find[stAAQuickTime].Value}"]} && !${Me.Song[Ruaabri's Fury].ID}) {
  |   /call cast ${_debug} 0 FALSE
  | }

  | trigger Frenzied kicks AA for bards if target is debuffed from intimidate
  /if (${Target.Buff[Harbinger's Intimidation].ID} && ${cast_data[${_debug}, "${maChr.Find[stAAFrenziedKicks].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  
   | Dance of Blades
  /if (${cast_data[${_debug}, "${maChr.Find[stAADanceofBlades].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }
  
    | Flurry of Notes
  /if (${cast_data[${_debug}, "${maChr.Find[stAAFlurryofNotes].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /call cast_glyph ${maDebug.Find[glyph].Value}

  | Intensity of the Resolute AA 4 hour reuse
  /call cast_AA_intensity ${maDebug.Find[intensity].Value}

  /call check_item_cycle ${maDebug.Find[item].Value} ${_spawn.ID}

/return TRUE



|***
 * note: issues the bard commands for running around and doing stupid shit
 * use: /dex BARDNAME /travel .. you can figure out the commands for eqbc yourself.
 ***|
#bind command_brd_travel /travel
sub Bind_command_brd_travel(bool _debug)
  DEBUG \atBind_command_brd_travel\ax

  /if (${cast_data[${_debug}, "${maChr.Find[stAASelosSonata].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

  /delay 1

  /if (${cast_data[${_debug}, "${maChr.Find[stAAShaurisSonorousClouding].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

/return



|***
 * note: scans the toons inventory to find the best instrument focuses
 * use: /call scan_focus DEBUG
 ***|
sub scan_focus(string _type, bool _debug)
  DEBUG \atscan_focus\ax(\a-w${_type}\ax)

  /declare _invItem         int local 0
  /declare _invItemAug      int local 0
  /declare _containerItem   int local 0
  /declare _tmpItemID       int local 0
  /declare _tmpUsable       int local 0
  /declare _count           int local 0
  /declare _found           bool local FALSE

  | stringed
  /invoke ${maBardFocus.Add[49,|0|0|0]}
  | percussion
  /invoke ${maBardFocus.Add[70,|0|0|0]}
  | brass
  /invoke ${maBardFocus.Add[12,|0|0|0]}
  | wind
  /invoke ${maBardFocus.Add[54,|0|0|0]}
  | singing
  /invoke ${maBardFocus.Add[41,|0|0|0]}

  | loop all inventory items
  /for _invItem 1 to 32

    /varset _tmpUsable 0

    | skip enpty slots
    /if (!${InvSlot[${_invItem}].Item.ID}) /continue

    DEBUG ${sep}\ay${_invItem}\ax${sep}${InvSlot[${_invItem}].Item} :: ${InvSlot[${_invItem}].Item.ID}

    | if the item is a container, look inside
    /if (${InvSlot[${_invItem}].Item.Container}) {

      | loop every item in the container
      /for _containerItem 1 to ${InvSlot[${_invItem}].Item.Container}

        | skip empty slots in bags
        /if (!${Me.Inventory[${_invItem}].Item[${_containerItem}].ID}) /continue

        | set the item
        /varset _tmpItemID ${Me.Inventory[${_invItem}].Item[${_containerItem}].ID}

        | is there a focus on it?
        /if (!${FindItem[${_tmpItemID}].Focus2.Spell.ID}) /continue
        
        | can i use it?
        /if (${Me.Level} < ${FindItem[${_tmpItemID}].RequiredLevel}) /continue

        | bard focus??
        /if (!${Select[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]},49,70,12,54,41]}) /continue

        | usable in primary/secondary?
        /if (${FindItem[${_tmpItemID}].WornSlot[offhand]} || ${FindItem[${_tmpItemID}].WornSlot[mainhand]}) {
          /varset _tmpUsable 1
        }

        | make sure its bard usable
        /for _count 1 to ${FindItem[${_tmpItemID}].Classes}
          /if (${Select[${FindItem[${_tmpItemID}].Class[${_count}]},Bard]}) {
            /varset _found TRUE
          }
        /next _count
        /if (!${_found}) /continue
     

        | if the current iteration is > then the set one. replace it
        DEBUG ${sep}${sep}${FindItem[${_tmpItemID}].Focus2.Spell.Base[1]} > ${maBardFocus.Find[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]}].Value.Arg[2,|]}
        /if (${FindItem[${_tmpItemID}].Focus2.Spell.Base[1]} > ${maBardFocus.Find[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]}].Value.Arg[2,|]}) {
          DEBUG ${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]},|${_tmpItemID}|${FindItem[${_tmpItemID}].Focus2.Spell.Base[1]}|${_tmpUsable}
          /invoke ${maBardFocus.Add[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]},|${_tmpItemID}|${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]}|${_tmpUsable}]}
        }

      /next _containerItem

    | if its not a container? top level inv/gear
    } else {

      /if (${Me.Inventory[${_invItem}].AugSlot[2].Type} == 17) { 
        /varset _tmpItemID ${Me.Inventory[${_invItem}].AugSlot[2].Item.ID}
      } else {
        /varset _tmpItemID ${Me.Inventory[${_invItem}].ID}
      }

      /if (!${FindItem[${_tmpItemID}].Focus2.Spell.ID}) /continue
      /if (${Me.Level} < ${FindItem[${_tmpItemID}].RequiredLevel}) /continue
      /if (!${Select[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]},49,70,12,54,41]}) /continue
      /if (${FindItem[${_tmpItemID}].WornSlot[offhand]} || ${FindItem[${_tmpItemID}].WornSlot[mainhand]}) {
        /varset _tmpUsable 1
      }
      DEBUG ${sep}${sep}${FindItem[${_tmpItemID}].Focus2.Spell.Base[1]} >= ${maBardFocus.Find[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]}].Value.Arg[2,|]}
      /if (${FindItem[${_tmpItemID}].Focus2.Spell.Base[1]} >= ${maBardFocus.Find[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]}].Value.Arg[2,|]}) {
        DEBUG ${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]},|${_tmpItemID}|${FindItem[${_tmpItemID}].Focus2.Spell.Base[1]}|${_tmpUsable}
        /invoke ${maBardFocus.Add[${FindItem[${_tmpItemID}].Focus2.Spell.Base[3]},|${_tmpItemID}|${FindItem[${_tmpItemID}].Focus2.Spell.Base[1]}|${_tmpUsable}]}
      }

    }

  /next _invItem
  
  
  | check augs on worn slots for type 17s 
  | some expansions put the bard focus on augs
  |** fuck these devs...
  /for _invItem 2 to 19
    
    /for _invItemAug 0 to ${Me.Inventory[${_invItem}].Augs}
      | /echo ${x} - ${y} - ${Me.Inventory[${x}].AugSlot[${y}].Item}
    
      /if (${Me.Inventory[${_invItem}].AugSlot[${_invItemAug}].Type} != 17) /continue
      
      | /echo ${Me.Inventory[${x}].Name} == ${Me.Inventory[${x}].AugSlot[${y}].Item.Focus2.Spell.Base[1]}
      
    /next _invItemAug
     
  /next _invItem
  **|

  /if (${_type.Equal[see]}) {
    /call Bind_command_see maBardFocus
  }

/return TRUE



|***
 * note: class control
 * use: /chr
 ***|
sub set_control(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control\ax(\a-w${_type}, "${_verbage}", ${_verbage2}\ax)

  | song refresh
  /if (${_type.Equal[refresh]}) {
    /if (!${set_control_num_range[${_debug}, stSongRefresh, "${_verbage}", 0, 18]}) /return FALSE

  | use rest song
  } else /if (${_type.Equal[rest]}) {
    /invoke ${set_switch_env[${_debug}, swSongRest, TRUE, ${_verbage}]}

  | use aa bellow
  } else /if (${_type.Equal[bellow]}) {
    /invoke ${set_switch_env[${_debug}, swAABellow, TRUE, ${_verbage}]}

  | use Vainglorious
  } else /if (${_type.Equal[vainglorious]}) {
    /invoke ${set_switch_env[${_debug}, swVainglorious, TRUE, ${_verbage}]}

  | sing ooc
  } else /if (${_type.Equal[ooc]}) {
    /invoke ${set_switch_env[${_debug}, swPulseOOC, TRUE, ${_verbage}]}

  | swap
  } else /if (${_type.Equal[swap]}) {

    /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swInstrumentSwap, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[secondary]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stInstrumentSecondary, "${_verbage2}"]}
    } else /if (${_verbage.Equal[primary]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stInstrumentPrimary, "${_verbage2}"]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swInstrumentSwap]}
    /invoke ${maControl.Add[secondary,stInstrumentSecondary|br]}
    /invoke ${maControl.Add[primary,stInstrumentPrimary|br]}
    /invoke ${set_control_output[${_debug}, maChr, "chr swap", maControl]}
    /return TRUE

  | scan for new singing focus effects
  } else /if (${_type.Equal[scan]}) {
    /call scan_focus see ${maDebug.Find[scan].Value}
    /return TRUE

  }

  /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
  /if (${Macro.Return.Equal[SKIP]}) /return FALSE

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maChr, chr, maChrControl]}

/return TRUE

