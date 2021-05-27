|***
 *  modes.inc
 *  Mode Specific Functions
 *  for doing strange stuff....
 *
 *
 ***|




|***
 *  DES: stop all non standard modes
 *  USE: /call switch_clear_mode
 *  INI: NA
 *  NOTE:

sub switch_clear_mode(bool _debug)
/return


  /declare _count int local 0
  /declare _list string local ${sql_return[FALSE, lstTypesMode]}

  /if (${swModeToken}) {
    /varset swModeToken FALSE
  }

  /for _count 1 to ${_list.Count[|]}
    /if (${swMode${_list.Arg[${_count},|]}}) {
      /varset swMode${_list.Arg[${_count},|]} FALSE
    }
  /next _count

  /varset swOverrideLOS ${sql_return[FALSE, swOverrideLOS]}
  /varset swOverrideEngagePct ${sql_return[FALSE, swOverrideEngagePct]}
  /varset swAutoCursor ${sql_return[FALSE, swAutoCursor]}
  /varset swModeTrainspellSPC FALSE

/return TRUE
 ***|


|***
 *  DES: central get target and kill for hunting modes
 *  USE: /call mode_get_mob
 *  NOTE:

sub mode_get_mob(bool _debug)
  :movetomob
  GETINPUT
  ISMEDEAD

  /call check_cursor ${_debug} clear

  /if (!${Spawn[id TARGET].ID} || ${Spawn[id TARGET].Dead} || ${Target.Dead}) {
    /call set_stop ${_debug} SILENT
    /call clear_combat ${_debug}
    /return FALSE
  }

  /call get_target_navpull ${_debug} TARGET

  /if (${Spawn[TARGET].ID} && ${Spawn[TARGET].Distance} > ${setCombatDistance}) /goto :movetomob

/return
 ***|


|***
 * note: Train non spell skills. Best to use during a duel with a pet.
 * use: /mode trainskill
 ***|
sub mode_trainskill(bool _debug)
  DEBUG \atmode_train\ax()

  ISMEDEAD
  SETHUD "trainskill"
  /makemevisible
  /delay 1

  /if (${Me.Skill[Taunt]} < ${Skill[Taunt].SkillCap} && ${Me.AbilityReady[Taunt]}) {
    DEBUG ${sep}taunt
    /doability Taunt
  }

  /if (${Me.Skill[Disarm]} < ${Skill[Disarm].SkillCap} && ${Me.AbilityReady[Disarm]}) {
    DEBUG ${sep}disarm
    /doability Disarm
  }

  /if (${Me.Skill[Bash]} < ${Skill[Bash].SkillCap} && ${Me.AbilityReady[Bash]}) {
    DEBUG ${sep}bash
    /doability Bash
  }

  /if (${Me.Skill[Kick]} < ${Skill[Kick].SkillCap} && ${Me.AbilityReady[Kick]}) {
    DEBUG ${sep}kick
    /doability Kick
  }

  /if (${Me.Skill[Sneak]} < ${Skill[Sneak].SkillCap} && ${Me.AbilityReady[Sneak]}) {
    DEBUG ${sep}sneak
    /doability sneak
  }

  /if (${Target.ID} && ${Target.ID} != ${Me.ID}) {
    /if (${Me.Skill[Begging]} < ${Skill[Begging].SkillCap} && ${Me.AbilityReady[Begging]}) /doability begging
  }

  | Bind Wounds
  /if (${Me.AbilityReady[Bind Wound]} && ${FindItem[=Bandages].ID}) {
    /if (${Me.Skill[Bind Wound]} < ${Skill[Bind Wound].SkillCap} && ${Me.PctHPs} < 99*.PCTHO) {
      DEBUG ${sep}bind
      /call target set ${Me.ID}
      /doability "Bind Wound"
      /delay 14s
    }
  }

  SETHUD

/return TRUE



|***
 * note: everyone needs spare sandals
 * use: /mode fish
 ***|
sub mode_fish(bool _debug)

  ISMEDEAD
  GETINPUT

  SETHUD "fish"

  /if (!${Me.Standing}) /stand

  /call check_cursor ${_debug} clear

  /if (${Me.ItemReady[Fisherman's Companion]} && !${Me.ItemReady[Brell's Fishin' Pole]} && !${Me.Inventory[mainhand].ID}) {
    /if (${Me.Invis}) /docommand /makemevisable
    /if (${cast_data[${_debug}, "Fisherman's Companion"]}) {
      /call cast ${_debug} ${Me.ID} FALSE
    }
  }

  /call check_cursor ${_debug} clear

  /if (${Me.AbilityReady[fishing]}) {
    DEBUG ${sep}fish
    /doability fishing
  }

  /call check_cursor ${_debug} clear

  SETHUD

/return TRUE



|***
 * note: you got shit under your finger nails
 * use: /mode forage
 ***|
sub mode_forage(bool _debug)
  DEBUG \atmode_forage\ax()

  /if (${Select[${Me.CombatState},combat]}) {
    /return FALSE
  }

  /if (!${Me.AbilityReady[forage]}) {
    /return FALSE
  }

  ISMEDEAD
  SETHUD "forage"

  /if (!${Me.Standing}) {
    /stand
  }

  DEBUG ${sep}forage
  /doability forage
  /delay 1s

  /while (${Cursor.ID}) {
    GETINPUT
    ISMEDEAD

    /call check_cursor ${_debug} clear
    /if (!${Cursor.ID}) /break
    /delay 2
  }

  SETHUD

/return TRUE



|***
 * note: Kill people then drag them around like trophys!!! Win Win...
 * use: /mode drag
 ***|
sub mode_drag(bool _debug)
  DEBUG \atmode_drag\ax()

  /declare _count     int local
  /declare _corpseID  int local

  SETHUD "drag"

  GETINPUT
  ISMEDEAD

  /delay 1
  /if (${SpawnCount[pccorpse radius 110]}) {
    /for _count 1 to ${SpawnCount[pccorpse radius 110]}
      ISMEDEAD

      /varset _corpseID ${NearestSpawn[${_count},pccorpse radius 110].ID}
      /if (${Spawn[${_corpseID}].Distance} > 15) {
        /squelch /target id ${_corpseID}
        /delay 5 ${Target.ID} == ${_corpseID}
        /corpse
      }
    /next _count
  }

  SETHUD

/return






|***
 * note: Pet Farm
 * use: /mode petfarm
 ***|
sub mode_petfarm(bool _debug)
  DEBUG \atmode_petfarm\ax()

  /if (!${lsClassPet.Contains[${Me.Class.ShortName}]} || SAFEZONE) {
    /invoke ${maMode.Add[petfarm,FALSE]}
    OUT WTF makes you think you are a pet class. Turning off /mode petfarm.
    /return FALSE
  }

  ISMEDEAD

  /invoke ${maOver.Add[swOverPctEngage,TRUE]}
  /invoke ${maOver.Add[swOverLOS,TRUE]}
  /varset swModeToken TRUE

  SETHUD "petfarm"

  GETINPUT

  /if (AUTO) {
    /if (!${check_engaged[FALSE]} && PET && !${timer_Check_Minion_Cycle}) {
      /call check_minion_cycle ${maDebug.Find[minion].Value} FALSE
    }
    /if (${maHeal.Find[swHealPet].Value} && ${Group.Members}) {
      /call check_heal ${maDebug.Find[minion].Value} Pet
    }
  }

  | check loot
  /if (${maEnv.Find[swADVLoot].Value} && !${timer_Check_Loot}) {
    /if (${Me.UseAdvancedLooting} && !${AdvLoot.LootInProgress}) /call do_loot ${maDebug.Find[loot].Value}
  }

  | find a mob
  /call pull_mob_find ${_debug} 0 petfarm
  DEBUG ${sep}pull_mob_find.return${sep}\a-w${Macro.Return}\ax

  /if (!${targetData.ID}) {
    DEBUG \arNO\ax targetData
    SETHUD
    GETINPUT
    /if (${Pet.Distance3D} > ENV) {
      /call where_the_fuck_is_my_pet ${_debug} FORCE
    }
    /return FALSE
  }

  CHECKTIE
  /doevents
  /call target set TARGET

  /pet attack

  CHECKTIE

  /invoke ${set_timer[${_debug}, timer_mob_get, 20s]}

  /while (TARGET) {
    GETINPUT
    /delay 5

    /if (${targetData.Type.Equal[CORPSE]} || !${targetData.ID}) /break
    /if (${Pet.Target.ID} != ${targetData.ID}) /break
    /if (${targetData.Distance} > PULLENV) /break
    DEBUG ${sep}here${sep}\a-w${timer_mob_get}\ax \ag${targetData.ID}\ax
    /if (!${timer_mob_get}) {
      /break
    }

  }

  /invoke ${maOver.Add[swOverPctEngage,FALSE]}
  /invoke ${maOver.Add[swOverLOS,FALSE]}
  /varset swModeToken FALSE
  /if (${Pet.Distance} > ENV) {
    /call where_the_fuck_is_my_pet ${_debug} FORCE
  }
  
  /call clear_combat

  SETHUD

/return




|***
 * note: Pet Farm for temp pet casts [CLR|WIZ]
 * use: /mode temppet
 ***|
sub mode_temppet(bool _debug)
  DEBUG \atmode_temppet\ax()

  /if (!${Select[${Me.Class.ShortName},CLR,WIZ]} || SAFEZONE) {
    /invoke ${maMode.Add[temppet,FALSE]}
    OUT This is for Clerics and Wizards ONLY. Turning off /mode temppet
    /return FALSE
  }

  SETHUD "temppet"
  
  /if (${Pet.ID}) {
    /delay 1s
    /return TRUE
  }

  /declare _tmpPullRad int local 0
  /if (PULLENV > 200) {
    /varset _tmpPullRad 200
  } else {
    /varset _tmpPullRad PULLENV
  }
  
  | check for things to torment
  /if (!${SpawnCount[npc radius ${_tmpPullRad}]}) {
    /delay 1s
    SETHUD
    /return FALSE
  }

  | get our pet spell
  /declare _petSpell string local ${get_best_in_spell[${_debug}, "Self", "Sum: Animation", "Spawn NPC"]}
  
  | get us a target•
  /declare _tmpID int local ${NearestSpawn[1, npc radius ${_tmpPullRad}].ID}
  /delay 5
  /call target set ${_tmpID}
    
  | send in the clowns
  /call cast_data ${_debug} "${_petSpell}"
  /call cast ${_debug} ${_tmpID} FALSE

  SETHUD


/return TRUE



|***
 *  DES: runs around and picks up shit
 *  USE: /mode harvest
 ***|
sub mode_harvest(bool _debug)
  DEBUG \atmode_harvest\ax()

  ISMEDEAD
  GETINPUT

  SETHUD "harvest"

  /declare _lsCollect list local
  /invoke ${_lsCollect.Delimiter[|]}
  /invoke ${_lsCollect.Append[${stModeData}]}
  /declare _licol listiterator local
  /vardata _licol _lsCollect.First.Clone

  /while (!${_licol.IsEnd}) {
    /if (${GroundItemCount[${_licol.Value}]}) {
      /call get_ground_item ${_debug} "${_licol.Value}"
    }
    /invoke ${_licol.Advance}
  }

  /delay 1s
  SETHUD

/return



|***
 *  DES: moves to the ground item and picks it up
 *  USE: /call get_ground_harvest DEBUG [ITEM NAME]
 *  NOTE:
 ***|
sub get_ground_item(bool _debug, string _tmpItem)
  DEBUG \atget_ground_item\ax(\a-w"${_tmpItem}"\ax)

  /if (${Cursor.ID}) {
    /call check_cursor ${_debug} clear
  }

  /if (!${GroundItemCount[${_tmpItem}]}) {
    DEBUG ${sep}no ground item count
    /return FALSE
    /delay 2
  }

  /if (!${Navigation.MeshLoaded}) {
    /invoke ${maMode.Add[harvest,FALSE]}
    /invoke ${out[0, "${notice} ${dot} NO Nav Mesh for this zone."]}
    /return FALSE
  }

  ISMEDEAD

  /invoke ${Ground.Search[${_tmpItem}]}
  /declare _item ground local
  /vardata _item Ground.First
  /declare _distfinal int local 999999
  /declare _validdis int local 0
  /declare _validpath bool local FALSE
  /declare _finalGround ground local

  /while (${_item.ID}) {
    /varset _validdis ${Navigation.PathLength[item ${_item.ID}]}
    /varset _validpath ${Navigation.PathExists[item ${_item.ID}]}
    DEBUG ID:\a-w${_item.ID}\ax .. Dis:${If[${_validdis} > 0,\a-w${_validdis}\ax,\arinvalid\ax]}

    /if (${_validdis} <= 0 || !${_validpath}) {
      /vardata _item Ground.Next
      /continue
    }

    /if (${_validdis} < ${_distfinal}) {
      /varset _distfinal ${_validdis}
      /vardata _finalGround Ground
    }
    /vardata _item Ground.Next
  }

  DEBUG FINAL ID:${_finalGround.ID}
  /invoke ${Ground.Search[${_finalGround.ID}]}
  /nav item |dist=10 log=${maEntropy.Find[swNavLogSpew].Value}
  /delay 1s

  /while (${Me.Moving} || ${_finalGround.Distance3D} <= 19) {
    DEBUG ${sep}whileloop${sep}\a-wMe.Moving\ax

    /if (${Cursor.ID}) {
      /call check_cursor ${_debug} clear
    }

    /if (${_finalGround.Distance3D} <= 19) {
      DEBUG ${sep}distance${sep}\a-w${_finalGround.Distance3D}\ax
      /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
      /call Bind_command_grab "${_tmpItem}" ${_debug}
      /if (${Cursor.ID}) {
        /call check_cursor ${_debug} clear
      }
      /break
    }

    /if (!${Navigation.Active} || !${Me.Moving}) {
      /break
    }

    /delay 2
  }

/return TRUE



|***
 *  DES: Alcohol Tolerance
 *  USE: /call mode_get_waisted
 *  NOTE:
 ***|
sub mode_get_waisted(bool _debug)
  /declare _ale string  local Summoned: Ale
  /declare _aleCount int local 20
  /declare _pole string local Brell's Fishin' Pole
  /declare _box string local Fisherman's Companion

  | no fishermans companion. end this mess
  /if (!${FindItem[=${_box}].ID}) {
    VOUT ${onotice} ${sep} No ${_box}
    VOUT This Mode uses Fisherman's Companion.
    VOUT Go get one.
    /varset swModeLush FALSE
    /return
  }

  SETHUD "Drink"
  /if (!${swAutoCursor}) /varset swAutoCursor TRUE
  | check max stats too ?

  | need booze! keep a stock on hand
  /if (${FindItemCount[${_ale}]} < ${_aleCount} && ${Me.ItemReady[${_pole}]} && !${Me.Drunk}) {
    /if (${validate_cast[FALSE, item, "${_pole}", ${Me.ID}]}) /call core_cast2 "${_pole}" item ${Me.ID} FALSE
    CLEARCURSOR TRUE
  }

  | lets get lit
  /if (${FindItemCount[${_ale}]} && ${Me.Drunk} < ${Skill[Alcohol Tolerance].SkillCap}) {
    VOUT Drinking ${sep} ${cinfo}${_ale}\ax
    /useitem ${_ale}
  }
  SETHUD
/return



|***
 * note: alcohol tolerance
 * use: /call mode_lush
 ***|
sub mode_lush(bool _debug)
  DEBUG \atmode_lush\ax()

  /if (!${FindItem[=${stModeData}].ID} || !${Bool[${stModeData}]}) {
    /varset swModeDrink FALSE
    /return
  }

  SETHUD "lush"

  | start drinking.
  /while (${Me.Skill[Alcohol Tolerance]} < ${Skill[Alcohol Tolerance].SkillCap}) {
    /if (!${Me.ItemReady[=${stModeData}]}) /break
    /if (${Me.Drunk} < 200) {
      /call cast_data ${_debug} "${stModeData}"
      /call cast ${_debug} 0 FALSE
    }
    /delay 2
    /delay 2s ${Me.Drunk} <  190
  }

  | maxed skill turn this off
  /if (${Me.Skill[Alcohol Tolerance]} == ${Skill[Alcohol Tolerance].SkillCap}) {
    OUT Alcohol Tolerance Maxed ${sep} (${num}${Me.Skill[Alcohol Tolerance]}\ax)
    /return TRUE
  }

  SETHUD

/return TRUE



|***
 *  DES: train spells
 *  USE: /call mode_trainspell | /mode trainspell
 *  NOTE:
 ***|
sub mode_trainspell(string _type, bool _debug)
  ISMEDEAD
  SETHUD "trainspell"

  | /if (${_type.Equal[specialize]} && !${swModeTrainspellSPC}) {
  |   /varset swModeTrainspellSPC TRUE
  | }

  /declare _skill string local |Alteration|Abjuration|Conjuration|Divination|Evocation
  /declare _count int local 0
  /declare _gem[5] bool local FALSE

  | figure out what we want to cast
  /for _count 1 to ${Me.NumGems}

    /call do_rest ${maDebug.Find[rest].Value}

    | validate skill caps and spell. if cap is maxed, move on
    | /if (!${maOver.Find[swOverTrainSpell].Value}) {
    |   /if (${Me.Skill[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]} == ${Me.SkillCap[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]}) /continue
    | }

    | if the skill spell is set to FALSE, move on
    | /if (!${Bool[${_skill.Arg[${_count},|]}]}) /continue

    | if the spell is FALSE, move on
    | /if (!${Bool[${Spell[${${_skill.Arg[${_count},|]}}].Skill}]}) /continue

    | if I already have the spell memed, move on
    | /if (!${Me.Gem[${_count}].Name.Equal[${${_skill.Arg[${_count},|]}}]}) {
    |   /call mem_spell TRUE "${${_skill.Arg[${_count},|]}}" ${_count} FALSE
    | }

    | /varset _gem[${_count}] TRUE


    /if (!${Me.Gem[${_count}].ID}) {
      /delay 1
      /continue
    }

    | empty hands
    /if (${Cursor.ID}) {
      /call check_cursor ${_debug} clear
    }

    | cast the spell
    /if (${cast_data[${_debug}, "${Me.Gem[${_count}].Name}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /delay 5s !${Me.Casting.ID}

    | seriously theres an apocalypse going on, shoud wash your hands
    /if (${Cursor.ID}) {
      /call check_cursor ${_debug} clear
    }


  /next _count


  SETHUD

/return


${Me.Gem[${_count}].Name}



  ISMEDEAD
  SETHUD "trainspell"

  /declare swTrainAlteration bool local TRUE

  | /if (${_type.Equal[specialize]} && !${swModeTrainspellSPC}) {
  |   /varset swModeTrainspellSPC TRUE
  | }

  /declare _skill string local |Alteration|Abjuration|Conjuration|Divination|Evocation
  /declare _count int local 0
  /declare _gem[5] bool local FALSE

  | figure out what we want to cast
  /for _count 1 to ${_skill.Count[|]}

    | validate skill caps and spell. if cap is maxed, move on
    /if (!${swOverrideTrainSpell}) {
      /if (${Me.Skill[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]} == ${Me.SkillCap[${If[${swModeTrainspellSPC},specialize ,]}${_skill.Arg[${_count},|]}]}) /continue
    }

    | if the skill spell is set to FALSE, move on
    /if (!${Bool[${_skill.Arg[${_count},|]}]}) /continue

    | if the spell is FALSE, move on
    /if (!${Bool[${Spell[${${_skill.Arg[${_count},|]}}].Skill}]}) /continue

    | if I already have the spell memed, move on
    /if (!${Me.Gem[${_count}].Name.Equal[${${_skill.Arg[${_count},|]}}]}) {
      /call mem_spell TRUE "${${_skill.Arg[${_count},|]}}" ${_count} FALSE
    }

    /varset _gem[${_count}] TRUE
  /next _count

  | start getting things done
  /for _count 1 to ${_skill.Count[|]}
    /if (!${_gem[${_count}]}) /continue
    /if (${cast_ready[${_debug}, "${${_skill.Arg[${_count},|]}}"]}) {
      /call cast ${_debug} 0 FALSE
    }
    /call sort_cursor TRUE
  /next _count


  SETHUD



|***
 *  DES: open something
 *  USE: /call mode_open | /mode open
 *  NOTE:
 ***|
sub mode_open(string _type, bool _debug)
  ISMEDEAD
  SETHUD "Open"

  /call timer_set timer_mode_open ${tMode_Open}
  /declare _tmpID int local

  /if (${SpawnCount[${modeOpenItem} npc radius 20]}) {
    /varset _tmpID ${Spawn[${modeOpenItem} npc radius 20].ID}
    /squelch /target ID ${_tmpID}
    /if (${Target.Dead}) /return
    /delay 2s ${Target.ID} == ${_tmpID}
    /open
    /delay 1s
    /return
  }

  SETHUD
/return



|***
 * note: summons a corpse in lobby
 * use: /mode summon DEBUG
 ***|
sub mode_summon(bool _debug)
  DEBUG \atmode_summon\ax()

  | in the lobby?
  /if (${Zone.ShortName.NotEqual[GuildLobby]}) {
    /invoke ${maMode.Add[summon,FALSE]}
    /return FALSE
  }

  SETHUD "summon"

  | set stone to buy
  /declare _soulstone string local
  /declare _count int local 0
  /sqlite query "${DBstatic}" dummy SELECT * FROM soulstone;
  /for _count 1 to ${sqlite.Rows[dummy]}
    /if (${Me.Level} <= ${sqlite.Result[dummy ${_count} env_var].Arg[1,|]} && ${Me.Level} >= ${sqlite.Result[dummy ${_count} env_var].Arg[2,|]}) {
      /varset _soulstone ${sqlite.Result[dummy ${_count} value]}
      /break
    }
  /next _count
  
  :summonstart
  GETINPUT
  ISMEDEAD

  | do we need to buy a stone?
  /if (!${FindItemCount[=${_soulstone}]}) {
    /nav spawn npc A Disciple of Luclin |log=${maEntropy.Find[swNavLogSpew].Value}

    :priestinventory
      /delay 10s !${Navigation.Active}
      /delay 1s
      /squelch /target A Disciple of Luclin
      /delay 1s
      /nomodkey /click right target
      /delay 5s ${Window[MerchantWnd].Open}
      /if (!${Window[MerchantWnd].Open}) /goto :priestinventory
    /delay 1s
    /call vendor_buy ${maDebug.Find[vendor].Value} "${_soulstone}" 1

    :closepriestinventory
      /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
      /delay 10
      /if (${Window[MerchantWnd].Open}) /goto :closepriestinventory
  }
  /if (!${FindItemCount[=${_soulstone}]}) /goto :summonstart

  | move to rezer
  /nav spawn npc A Priest of Luclin |log=${maEntropy.Find[swNavLogSpew].Value} distance=6
  /delay 10s !${Navigation.Active}

  | get stone on cursor
  /itemnotify "${_soulstone}" leftmouseup
  /delay 5

  :givetopriest
    /squelch /target A Priest of Luclin
    /delay 5
    /nomodkey /click left target
    /delay 5
    /delay 5s ${Window[GiveWnd].Open}
    /if (!${Window[GiveWnd].Open}) /goto :givetopriest
    /if (${Window[GiveWnd].Open}) {
      /notify GiveWnd GVW_Give_Button leftmouseup
      /delay 3s
      /if (${Window[GiveWnd].Open}) /goto :GiveSoulStone
    }

  :CloseInvWnd
    /if (${Window[InventoryWindow].Open}) {
      /nomodkey /notify InventoryWindow IW_DoneButton LeftMouseUp
      /delay 5
      /if (${Window[InventoryWindow].Open}) /goto :CloseInvWnd
    }

  /if (${Window[InventoryWindow].Open}) /nomodkey /notify InventoryWindow IW_DoneButton LeftMouseUp
  /delay 5

  /squelch /target clear
  /invoke ${maMode.Add[summon,FALSE]}
  /varset swIsMeDead TRUE

/return FALSE



|***
 * note: Scorched Earth Mode
 * use: /mode hunt
 ***|
sub mode_hunt(bool _debug)
  DEBUG \atmode_hunt\ax(\a-w\ax)

  /invoke ${maOver.Add[swOverPctEngage,TRUE]}
  /invoke ${maOver.Add[swOverLOS,TRUE]}
  /varset swModeToken TRUE
  /call clear_combat ${_debug}
  /declare _count int local 0
  /declare _lsHunt list local
  /declare _lihunt listiterator local


  SETHUD "hunt"

  :checkpath
  GETINPUT

  /if (${Bool[${stModeData}]}) {
    /invoke ${_lsHunt.Delimiter[|]}
    /invoke ${_lsHunt.Append[${stModeData}]}
    /vardata _lihunt _lsHunt.First.Clone
    /while (!${_lihunt.IsEnd}) {
      /if (${Spawn[npc ${_lihunt.Value} radius PULLENV targetable].ID}) {
        /varset stAssTarID ${NearestSpawn[npc ${_lihunt.Value}].ID}
        /if (${lsZoneExclude.Contains[${Spawn[${stAssTarID}].DisplayName}]}) {
          /invoke ${_lihunt.Advance}
          /continue
        }
        /break
      }
      /invoke ${_lihunt.Advance}
    }

  } else /if (!${Bool[${stModeData}]}) {
    /call pull_mob_find ${_debug} ${_count}
    DEBUG ${sep}pull_mob_find.return${sep}\a-w${Macro.Return}\ax
  }

  /if (!TARGET) {
    DEBUG \arNO\ax stAssTarID
    /call clear_combat ${_debug}
    /return FALSE
  }

  /if (!${Navigation.PathExists[locyxz ${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]}) {
    GETINPUT
    /invoke ${out[0, "${notice}${sep}No Nav Path${sep}\a-w${Spawn[TARGET].DisplayName}\ax"]}
    /varcalc _count ${_count}+1
    /if (${_count} > 10) /return FALSE
    /delay 1s
    /goto :checkpath
   }

  | get to the mob
  /call pull_mob_move_to ${_debug} TARGET ${Math.Calc[${Spawn[id TARGET].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} on

  /if (${Spawn[id TARGET].Dead}) /return TRUE


  /invoke ${maCC.Add[swSetCombat,TRUE]}
  /call target set TARGET

  | /call check_facing ${_debug} TARGET FORCE

  /call Bind_control_cc FORCE
  /call class_main
  /call clear_combat

  /invoke ${maOver.Add[swOverPctEngage,FALSE]}
  /invoke ${maOver.Add[swOverLOS,FALSE]}
  /varset swModeToken FALSE

  SETHUD

/return TRUE








quest giver: Missionary Ada
quest Name: The Way Back Home
quest phrases: go look

|*********************
farms
********************|



sub tov_ew_ada(bool _debug)
  DEBUG \attov_ew_ada\ax()

  /if (${Target.ID}) /squelch /target clear

  /declare _quest string local The Way Back Home
  /declare _count int local 0

  /if (!${Task[${_quest}].ID}) {
    /call task_get ${_debug} "${_quest}"
    /if (!${Macro.Return}) {
      /invoke ${out[23, "Failed to get${sep}\a-w${_quest}\ax"]}
    } else /if (${Macro.Return}) {
      /invoke ${out[23, "Quest started${sep}\a-w${_quest}\ax"]}
    }
  }

  /if (${Me.Sitting}) /stand
  /squelch /hidecorpse always

  /call Bind_control_cc pull rad 45
  /call Bind_control_cc pull active on
  /call Bind_control_cc agro on

  | how big is the quest?
  /sqlite query "${DBstatic}" farmcount SELECT count(*) as data from farm where task="${_quest}"
  DEBUG ${sep}count${sep}\a-w${sqlite.Result[farmcount 1 data]}\ax

  | get the data we need for this step
  /for _count 0 to ${Math.Calc[${sqlite.Result[farmcount 1 data]} -1]}
    /sqlite query "${DBstatic}" farmquery${_count} SELECT * FROM farm WHERE task="${_quest}" AND stepn="${_count}"
    /if (!${_count}) /continue
    OUT ${dot}loading${sep}\aw${_count}\ax${sep}\a-w${sqlite.Result[farmquery${_count} 1 step]}\ax
  /next _count
  /echo


  | step 1
    /invoke ${out[23, "\agSTEP 1\ax${sep}\a-w${sqlite.Result[farmquery1 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status}\ax)"]}
    :tov_ew_ada_step1
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status.NotEqual[Done]}) {
      /call task_output ${_debug} 1
      DEBUG ${sep}step${sep}${sqlite.Result[farmquery1 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status}\ax)
      GETINPUT
      ISMEDEAD
      /if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery1 1 ivu]}]} && ${cast_data[${_debug}, "Group Perfected Invisibility to Undead"]}) {
        /call cast ${_debug} 0 FALSE
      }
      /call Bind_command_navto loc "${sqlite.Result[farmquery1 1 locyxz]}" FALSE
      /delay 2s
    }
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step1


  | step 2
    /invoke ${out[23, "\agSTEP 2\ax${sep}\a-w${sqlite.Result[farmquery2 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status}\ax)"]}
    :tov_ew_ada_step2
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual[Done]}) {
      /call task_output ${_debug} 3
      DEBUG ${sep}step${sep}${sqlite.Result[farmquery2 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status}\ax)
      GETINPUT
      ISMEDEAD
      /if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery2 1 ivu]}]} && ${cast_data[${_debug}, "Group Perfected Invisibility to Undead"]}) {
        /call cast ${_debug} 0 FALSE
      }
      /call Bind_command_navto loc "${sqlite.Result[farmquery2 1 locyxz]}" FALSE
      /delay 2s
    }
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step2


  | step 3
    /invoke ${out[23, "\agSTEP 3\ax${sep}\a-w${sqlite.Result[farmquery3 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status}\ax)"]}
    :tov_ew_ada_step3
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status.NotEqual[Done]}) {
      /call task_output ${_debug} 2
      DEBUG ${sep}step${sep}${sqlite.Result[farmquery3 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status}\ax)
      GETINPUT
      ISMEDEAD
      /if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery3 1 ivu]}]} && ${cast_data[${_debug}, "Group Perfected Invisibility to Undead"]}) {
        /call cast ${_debug} 0 FALSE
      }
      /call Bind_command_navto loc "${sqlite.Result[farmquery3 1 locyxz]}" FALSE
      /delay 2s
    }
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step3


  | step 4
    /invoke ${out[23, "\agSTEP 5\ax${sep}\a-w${sqlite.Result[farmquery4 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status}\ax)"]}
    :tov_ew_ada_step4
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status.NotEqual[Done]}) {
      /call task_output ${_debug} 4
      DEBUG ${sep}step${sep}${sqlite.Result[farmquery4 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status}\ax)
      GETINPUT
      ISMEDEAD
      /if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery4 1 ivu]}]} && ${cast_data[${_debug}, "Group Perfected Invisibility to Undead"]}) {
        /call cast ${_debug} 0 FALSE
      }
      /call Bind_command_navto loc "${sqlite.Result[farmquery4 1 locyxz]}" FALSE
      /delay 2s
    }
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery4 1 step]}].Status.NotEqual[done]}) /goto :tov_ew_ada_step4


  | step 5/6
    /declare _tmpStep int local 0
    /declare _tmpMob string local
    /declare _tmpObj string local ${Task[${_quest}].Step}
    /if (${_tmpObj.Find[huge]}) {
      /varset _tmpStep 6
    } else /if (${_tmpObj.Find[small]}) {
      /varset _tmpStep 5
    }
    | set mob name for use
    /varset _tmpMob ${sqlite.Result[farmquery${_tmpStep} 1 questmob]}


    /invoke ${out[23, "\agSTEP ${_tmpStep}\ax${sep}\a-w${sqlite.Result[farmquery${_tmpStep} 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery${_tmpStep} 1 step]}].Status}\ax)"]}
    :tov_ew_ada_step5_6
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery${_tmpStep} 1 step]}].Status.NotEqual[Done]}) {
      /call task_output ${_debug} 4
      DEBUG ${sep}step${sep}${sqlite.Result[farmquery${_tmpStep} 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery${_tmpStep} 1 step]}].Status}\ax)
      GETINPUT
      ISMEDEAD
      /if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery${_tmpStep} 1 ivu]}]} && ${cast_data[${_debug}, "Group Perfected Invisibility to Undead"]}) {
        /call cast ${_debug} 0 FALSE
      }
      /call Bind_command_navto loc "${sqlite.Result[farmquery${_tmpStep} 1 locyxz]}" FALSE

    }

    :tov_ew_ada_step5_6_wait
    SDEBUG waiting for${sep}\a-w${_tmpMob}\ax

    | kill spawn
    /if (!${Spawn[npc radius 45 ${_tmpMob}].ID}) {
      /delay 1s
      /goto :tov_ew_ada_step5_6
    } else /if (${Spawn[npc radius 45 ${_tmpMob}].ID}) {
      /dismount
      SDEBUG ${dot}found${sep}\a-w${_tmpMob}\ax
      /call target set ${Spawn[${_tmpMob}].ID}
      /delay 1s ${Target.ID} == ${Spawn[${_tmpMob}].ID}
    }

    :tov_ew_ada_step5_6_killing
      /face fast nolook
      /squelch /stick id moveback ${Math.Calc[${Spawn[id ${Target.ID}].MaxRangeTo}*.6]}
      /call task_combatloop ${Spawn[${_tmpMob}].ID}
      /delay 5
      /if (${sqlite.Result[farmquery${_tmpStep} 1 step].NotEqual[done]}) /goto :tov_ew_ada_step5_6_killing


  | step 7
    /invoke ${out[23, "\agSTEP 6\ax${sep}\a-w${sqlite.Result[farmquery7 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery7 1 step]}].Status}\ax)"]}
    /delay 5
    /advloot shared "Simple Velium Pendant" giveto ${Me.Name}
    /delay 5
    /advloot personal "Simple Velium Pendant" loot
    /delay 5
    /if (${Window[ConfirmationDialogBox].Open}) {
      /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
    }
    /delay 5

  | step 8

/return TRUE



|***
 * note: gets a task
 * use: /call get_task DEBUG [name of task/quest]
 ***|
sub task_get(bool _debug, string _verbage)
  DEBUG \attask_get\ax(\a-w${_verbage}\ax)

  | get the dat a we need to get quest started
  /sqlite query "${DBstatic}" farmquery SELECT * FROM farm WHERE step="start" AND task="${_verbage}"

  | are we in the right zone?
  /if (${Zone.ShortName.NotEqual[${sqlite.Result[farmquery 1 zonesn]}]}) {
    DEBUG ${sep}wrong zone${sep}\a-w${sqlite.Result[farmquery 1 zonesn]}\ax
    /return FALSE
  }

  | target mob up
  /if (!${Spawn[${sqlite.Result[farmquery 1 questmob]}].ID}) {
    DEBUG ${sep}mob not up${sep}\a-w${Spawn[${sqlite.Result[farmquery 1 questmob]}].DisplayName}\ax
    /return FALSE
  }

  | get to quest target we should be kinda close for a say
  /while (${Spawn[${sqlite.Result[farmquery 1 questmob]}].Distance} > 60) {
    /if (${sqlite.Result[farmquery 1 invis]}) {
      /call check_invis_single ${_debug}
    }
    /if (!${Me.Moving} || !${Navigation.Active}) {
      /call Bind_command_navto "${Spawn[${sqlite.Result[farmquery 1 questmob]}].ID}"
    }
    /delay 5
  }

  /call target set ${Spawn[${sqlite.Result[farmquery 1 questmob]}].ID}

  /makemevisible
  /delay 5
  /say ${sqlite.Result[farmquery 1 phrase]}

  | give quest window time to populate
  /delay 5s ${Task[${_verbage}].Index}

  /if (!${Task[${_verbage}].Index}) /return FALSE

/return TRUE



|***
 * note: outputs task details
 * use: /call task_output DEBUG [query#]
 ***|
sub task_output(bool _debug, string _verbage)
  DEBUG \attask_output\ax(\a-w${_verbage}\ax)

  /if (${Bool[${sqlite.Result[farmquery${_verbage} 1 questmob]}]}) {
    /invoke ${out[23, "mob${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 questmob]}\ax"]}
  }
  /if (${Bool[${sqlite.Result[farmquery${_verbage} 1 locyxz]}]}) {
    /invoke ${out[23, "locyxz${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 locyxz]}"]}
  }
  /if (${Bool[${sqlite.Result[farmquery${_verbage} 1 invis]}]} || ${Bool[${sqlite.Result[farmquery${_verbage} 1 ivu]}]} || ${Bool[${sqlite.Result[farmquery${_verbage} 1 sos]}]}) {
    /invoke ${out[23, "IV${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 invis]}\ax IVU${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 ivu]}\ax SoS${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 sos]}\ax"]}
  }
  /if (${Bool[${sqlite.Result[farmquery${_verbage} 1 phrase]}]}) {
    /invoke ${out[23, "phrase${sep}\a-w${sqlite.Result[farmquery${_verbage} 1 phrase]}\ax"]}
  }

/return TRUE



|***
 * note: task specific combat loops
 * use: /call task_combatloop TARGETID DEBUG
 ***|
sub task_combatloop(int _tmpID, bool _debug)

  SETHUD "killing target"
  /while (${_tmpID}) {

    /invoke ${maCC.Add[swSetForced,TRUE]}
    /varset swModeToken TRUE

    /call check_heal_routines
    /call set_combat_control ${maDebug.Find[decision].Value} ${_tmpID}

    /if (MELEE) /call cast_cycle_melee ${maDebug.Find[melee].Value} ${_tmpID}

    /if (${maChr.Find[swYaulp].Value}) {
      /if (!${timer_Check_Yaulp}) /call cast_yaulp FALSE
    }
    /if (${Defined[get_agro]}) {
      /if (AGRO) /call get_agro FALSE ${_tmpID}
    }
    /if (${maChr.Find[stPctShieldFlash].Value} || ${maChr.Find[stCountShieldFlash].Value}) /call cast_shield_flash FALSE aa
    /call set_combat_control ${maDebug.Find[decision].Value} ${_tmpID}

    /call check_item_cycle ${maDebug.Find[item].Value} ${_tmpID} epic
    /call misc_combat ${maDebug.Find[misc].Value} ${_tmpID}
    /if (BURN) /call burn ${maDebug.Find[burn].Value} ${_tmpID}
    /if (MELEE) /call cast_cycle_melee ${maDebug.Find[melee].Value} ${_tmpID}
    /if (DOT) /call cast_cycle_dot ${maDebug.Find[dot].Value} ${_tmpID}
    /if (NUKE) /call cast_cycle_nuke ${maDebug.Find[nuke].Value} ${_tmpID}
    /call check_item_cycle ${maDebug.Find[item].Value} ${_tmpID}
    /call check_heal_routines
    /call check_deBuffs ${maDebug.Find[debuff].Value} ${_tmpID}
      /if (!${timer_Check_deBuffs}) {
        /call check_deBuffs ${maDebug.Find[debuff].Value} ${_tmpID}
      }

    | readjust if needed
    /if (!${Spawn[id ${_tmpID}].Dead}) {
      /call task_mob_move_to ${_debug} ${_tmpID} ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
    }

    /if (${maChr.Find[stPctManaRecovery].Value}) {
      /call do_mana_recovery ${maDebug.Find[manarecover].Value}
    }
    /if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) {
      /call cast_stamina_recovery ${maDebug.Find[rest].Value}
    }

    /if (${target[isdead, ${_tmpID}]}) {
      SETHUD
      /return FALSE
    }

  }

/return TRUE



|***
 * note: quest farm: Fear Begins at Home
 *  This will do the quest forever
 *  You better have a LOT (read hundreds!!) of Cloudy Potions if you plan to leave this on auto for a while.
 *  Will not eat the shards, just collects them.
 * use: /mode farm rof_shard_bodhe [wait]
 *  Adding the option wait to the command execution will cause this farm routine to pause outside the entrance
 *  to the Alarian temple. The toon will wait there until the 10/10 count for step 2 has been reached. It will then
 *  go claim the quest reward, get it again, and return to wait.
 *  This is here so you can run many toons at once through this stupid leveling of the tear, and only need 1 or 2
 *  toons to actually do the killing.
 ***|
sub rof_shard_bodhe(string _opt, bool _debug)
  | /varset _debug TRUE
  DEBUG \atrof_shard_bodhe\ax(\a-w${_opt}\ax)
  /declare _timestart int local 0
  /varset swModeToken TRUE

  /declare _quest string local Fear Begins at Home
  /declare _count int local 0
  /declare _mobinc int local 0
  /declare _spcount int local 0
  /declare _pullrad int local 200

  /declare _huntlocs int local 5
  /declare _curpos int local 0
  /declare _loc1 string local |61|2331|-329
  /declare _loc2 string local |-215|2282|-329
  /declare _loc3 string local |-591|2212|-399
  /declare _loc4 string local |-823|1741|-498
  /declare _loc5 string local |-674|1436|-539

  :start_rof_shard_bodhe
  /varset _timestart ${MacroQuest.Running}
  /invoke ${target[clear]}
  /if (!${Task[${_quest}].ID}) {
    /call task_get ${_debug} "${_quest}"
    /if (!${Macro.Return}) {
      /invoke ${out[23, "Failed to get${sep}\a-w${_quest}\ax"]}
    } else /if (${Macro.Return}) {
      /invoke ${out[23, "Quest started${sep}\a-w${_quest}\ax"]}
    }
  }

  SETHUD "starting task"

  /if (${Window[TaskWnd].Open}) /keypress esc

  /if (${Me.Sitting}) /stand
  /squelch /hidecorpse always

  /if (AUTO) {
    /call Bind_control_env auto off
  }
  /call Bind_control_cc pull rad ${_pullrad}
  /if (!AGRO) {
    /call Bind_control_agro active on
  }

  | set nav pulling
  /call Bind_control_cc pull mode nav

  /if (${Select[${Me.Class.ShortName},ROG]}) {
    /invoke ${set_switch_env[${_debug}, swMoveBehind, TRUE, FALSE]}
  }


  | how big is the quest?
  /sqlite query "${DBstatic}" farmcount SELECT count(*) AS data FROM farm WHERE task="${_quest}"
  DEBUG ${sep}count${sep}\a-w${sqlite.Result[farmcount 1 data]}\ax

  | get the data we need for this step
  /for _count 0 to ${Math.Calc[${sqlite.Result[farmcount 1 data]} -1]}
    /sqlite query "${DBstatic}" farmquery${_count} SELECT * FROM farm WHERE task="${_quest}" AND stepn="${_count}"
    /if (!${_count}) /continue
    OUT ${dot}loading${sep}\aw${_count}\ax${sep}\a-w${sqlite.Result[farmquery${_count} 1 step]}\ax
  /next _count
  /echo

  | step 1
    /invoke ${out[23, "\agSTEP 1\ax${sep}\a-w${sqlite.Result[farmquery1 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status}\ax)"]}
    :rof_shard_bodhe_step1
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status.NotEqual[Done]}) {
      DEBUG ${sep}step${sep}${sqlite.Result[farmquery1 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status}\ax)
      GETINPUT
      ISMEDEAD
      /if (!${Me.Invis} && ${sqlite.Result[farmquery1 1 invis]}) {
        /call check_invis_single ${_debug}
      }

      SETHUD "navto marker"
      /if (${_opt.Equal[wait]}) {
        /call Bind_command_navto loc "660 2093 -295" invis ${_debug}
      } else {
        /call Bind_command_navto loc "${sqlite.Result[farmquery1 1 locyxz]}" invis ${_debug}
      }
    }

    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery1 1 step]}].Status.NotEqual[done]}) {
      /goto :rof_shard_bodhe_step1
    }
    
    /call do_rest ${maDebug.Find[rest].Value}

  | step 2
    /invoke ${out[23, "\agSTEP 2\ax${sep}\a-w${sqlite.Result[farmquery2 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status}\ax)"]}
    :rof_shard_bodhe_step2

    /if (${_opt.Equal[wait]}) {
      /invoke ${out[23, "\agSTEP 2\ax${sep}\a-wWaiting for kill count\ax"]}
      SETHUD "waiting"
      /while (${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual[Done]}) {
        DEBUG ${sep}step${sep}${sqlite.Result[farmquery2 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status}\ax)
        GETINPUT
        ISMEDEAD

        | find us something to kill
        /while (${Task[Fear Begins at Home].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual["Done"]}) {
          /delay 1s
          GETINPUT
        }
      }

    } else {
      /while (${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual[Done]}) {
        DEBUG ${sep}step${sep}${sqlite.Result[farmquery2 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status}\ax)
        GETINPUT
        ISMEDEAD

        | find us something to kill
        /if (${Task[Fear Begins at Home].Objective[${sqlite.Result[farmquery2 1 step]}].Status.NotEqual["Done"]}) {

          SETHUD "finding location"
          /for _count 1 to ${_huntlocs}
            /varset _curpos ${_count}
            /if (${SpawnCount[npc loc ${_loc${_curpos}.Arg[2,|]}  ${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[3,|]} radius ${_pullrad}]}) {
              DEBUG _count${sep}\a-w${_curpos}\ax .. SpawnCount[npc loc ${_loc${_curpos}.Arg[2,|]}  ${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[3,|]} radius ${_pullrad}]
              OUT location ${_curpos}${sep}\a-w${SpawnCount[npc loc ${_loc${_curpos}.Arg[2,|]}  ${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[3,|]} radius ${_pullrad}]} mobs\ax
              SETHUD "navto marker"
              /call Bind_command_navto loc "${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[2,|]} ${_loc${_curpos}.Arg[3,|]}" FALSE ${_debug}
              /if (${_curpos} == 1) {
                /call check_buff_cycle ${maDebug.Find[cb].Value} TRUE a
                /call check_buff_cycle ${maDebug.Find[cb].Value} TRUE s
                /call check_buff_cycle ${maDebug.Find[cb].Value} TRUE u
                /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
                /if (${SubDefined[check_mana]}) /call check_mana
                /call do_rest ${maDebug.Find[rest].Value}

              }
              /call check_cursor ${_debug} clear
              DEBUG _count${sep}\a-w${_count}\ax
              /goto :kill_rof_shard_bodhe
            }
          /next _count
          /delay 2
          SETHUD
        }
        /call where_the_fuck_is_my_pet DEBUG TRUE
        :kill_rof_shard_bodhe
        /while (${SpawnCount[npc loc ${_loc${_curpos}.Arg[2,|]}  ${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[3,|]} radius ${_pullrad}]}) {
          DEBUG ${sep}spawncount loc${_curpos}${sep}\a-w${SpawnCount[npc loc ${_loc${_curpos}.Arg[2,|]}  ${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[3,|]} radius ${_pullrad}]}\ax

          SETHUD "find target"
          /call task_mob_find ${_debug}

          /if (!TARGET) {
            /goto :rof_shard_bodhe_step2
          }

          /call target set TARGET
          SETHUD "navto target"
          /call task_mob_move_to ${_debug} TARGET ${Math.Calc[${Spawn[id TARGET].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
 
          /if (${maMinion.Find[swonForce].Value}) /pet attack
          /invoke ${maCC.Add[swValidAssTarID,TRUE]}
          /invoke ${maCC.Add[swSetCombat,TRUE]}
          /invoke ${maCC.Add[swSetForced,TRUE]}   
          /call check_facing ${_debug} TARGET FORCE    
          /call stick_to_target ${_debug} TARGET  
          /call class_main
          /if (${SpawnCount[npc PS4 radius SAFEENV]} && ${Me.XTarget}) /goto :kill_rof_shard_bodhe

          | finished.. go kill more
          /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery2 1 step]}].Status.Equal[done]}) {
            /break
          }

          | move to start if we get too far away
          /if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[2,|]} ${_loc${_curpos}.Arg[3,|]}]} > 80 || !${SpawnCount[npc radius ${_pullrad}]}) {
            /call Bind_command_navto loc "${_loc${_curpos}.Arg[1,|]} ${_loc${_curpos}.Arg[2,|]} ${_loc${_curpos}.Arg[3,|]}" FALSE ${_debug}
          }

          | check loot
          /if (${maEnv.Find[swADVLoot].Value} && !${timer_Check_Loot}) {
            /if (${Me.UseAdvancedLooting} && !${AdvLoot.LootInProgress}) /call do_loot ${maDebug.Find[loot].Value}
          }

        }
      }
    }

  | step 3
    :wait_step_3

    | check loot
    /if (${maEnv.Find[swADVLoot].Value} && !${timer_Check_Loot}) {
      /if (${Me.UseAdvancedLooting} && !${AdvLoot.LootInProgress}) /call do_loot ${maDebug.Find[loot].Value}
    }

    /invoke ${out[23, "\agSTEP 3\ax${sep}\a-w${sqlite.Result[farmquery3 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status}\ax)"]}
    :rof_shard_bodhe_step3
    /delay ${Math.Rand[1,30]}
    /if (${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status.NotEqual[Done]}) {
      SETHUD "collecting"
      DEBUG ${sep}step${sep}${sqlite.Result[farmquery3 1 step]}\ax (\a-r${Task[${_quest}].Objective[${sqlite.Result[farmquery3 1 step]}].Status}\ax)
      GETINPUT
      ISMEDEAD
      /if (!${Me.Invis} && ${Bool[${sqlite.Result[farmquery3 1 invis]}]}) {
        /call check_invis_single ${_debug}
      }
      /invoke ${target[clear]}
      /call Bind_command_navto "${sqlite.Result[farmquery3 1 questmob]}" invis ${_debug}
      /delay 1s
    }

    /if (${Spawn[npc ${sqlite.Result[farmquery3 1 questmob]}].Distance} <= 20) {
      /call target set ${Spawn[npc ${sqlite.Result[farmquery3 1 questmob]}].ID}
      /delay 5
      /makemevisible
      /delay 5
      /keypress hail
    }

    /delay 5s ${Window[ConfirmationDialogBox].Open}

    /if (${Window[ConfirmationDialogBox].Open}) {
      /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
    }
    /delay 5

    GETINPUT

    /delay ${Math.Rand[1,30]}
    /if (${Window[RewardSelectionWnd].Open}) /nomodkey /notify RewardSelectionWnd RewardSelectionChooseButton leftmouseup
    /keypress esc
    /invoke ${out[0, "\agTask Completion Time\ax${sep}\a-w${Math.Calc[(((${MacroQuest.Running}-${_timestart}) / 10) / 60) /60]}m\ax", 0]}
    /invoke ${out[18, 0, 33]}
    /goto :start_rof_shard_bodhe

/return TRUE



|***
 * note: addition to petfarm for location route,
 * use: /mode petfarm ROUTENAME
 ***|
sub petfarm_route(string _route, int _start)
  /declare _debug bool local ${maDebug.Find[petfarm].Value}
  | /varset _debug TRUE

  /if (!${Pet.ID}) {
    DEBUG ${break}no pet
    /invoke ${maMode.Add[petfarm,FALSE]}
    OUT need a pet
    /return FALSE
  }

  /invoke ${maMode.Add[petfarm,TRUE]}
  /declare _count int local 0

  /while (TRUE) {
    DEBUG ${sep}initial loop start
    CHECKDEAD
    GETINPUT

    /for _count ${If[${_start},${_start},1]} to ${Ini["${INIfarmroute}",${_route},stops]}
      DEBUG ${sep}_count${sep}${_count}${sep}${Ini["${INIfarmroute}",${_route},stops]} stops
      DEBUG ${sep}navto ${dot} ${Ini["${INIfarmroute}",${_route},loc${_count}]}
      /call Bind_command_navto loc "${Ini["${INIfarmroute}",${_route},loc${_count}]}" ${_debug}
      /invoke ${set_timer[FALSE, _timerWait, ${Ini["${INIfarmroute}",${_route},loc${_count}wait]}]}
      OUT Location${sep}\a-w${_count}\ax/\a-w${Ini["${INIfarmroute}",${_route},stops]}\ax (\a-r${Ini["${INIfarmroute}",${_route},loc${_count}wait]}\ax)
      /while (${_timerWait}) {
        /if (!${timer_Check_Mode}) {
          /call check_mode ${maDebug.Find[mode].Value}
        }
        GETINPUT
        /delay 5
      }
    /next _count

    /varset _start 1
    /call check_buff_cycle_self ${_debug}
    /call check_minion_cycle ${_debug}

  }
  /invoke ${maMode.Add[petfarm,FALSE]}

/return



|***
 * note: addition to petfarm for location route,
 * use: /mode petfarm ROUTENAME

sub petfarm_route(string _route, int _start)
  /declare _debug bool local ${maDebug.Find[petfarm].Value}

  /if (!${Pet.ID}) {
    DEBUG ${break}no pet
    /invoke ${maMode.Add[petfarm,FALSE]}
    OUT need a pet
    /return FALSE
  }

  /invoke ${maMode.Add[petfarm,TRUE]}
  /declare _count int local 0

  /while (TRUE) {
    DEBUG ${sep}initial loop start
    CHECKDEAD
    GETINPUT

    /for _count ${If[${_start},${_start},1]} to ${Ini["${INIfarmroute}",${_route},stops]}
      DEBUG ${sep}_count${sep}${_count}${sep}${Ini["${INIfarmroute}",${_route},stops]}
      DEBUG ${sep}navto ${dot} ${Ini["${INIfarmroute}",${_route},loc${_count}]}
      /call Bind_command_navto loc "${Ini["${INIfarmroute}",${_route},loc${_count}]}" ${_debug}
      /invoke ${set_timer[FALSE, _timerWait, ${Ini["${INIfarmroute}",${_route},loc${_count}wait]}]}
      OUT Location${sep}\a-w${_count}\ax (\a-r${Ini["${INIfarmroute}",${_route},loc${_count}wait]}\ax)
      /while (${_timerWait}) {
        /if (!${timer_Check_Mode}) {
          /call check_mode ${maDebug.Find[mode].Value}
        }
        GETINPUT
        /delay 5
      }
    /next _count

    /varset _start 1
    /call check_buff_cycle_self ${_debug}
    /call check_minion_cycle ${_debug}

  }
  /invoke ${maMode.Add[petfarm,FALSE]}

/return
 ***|


|***
 * note: moves to an established stAssTarID
 * use: /call task_mob_move_to DEBUG [TARGETID] [distance to stop at]
 ***|
sub task_mob_move_to(bool _debug, int _tmpID, int _dist)
  DEBUG \attask_mob_move_to\ax(\a-w${_tmpID}, ${_dist}\ax)

  /nav id ${_tmpID} | distance=${_dist} log=${maEntropy.Find[swNavLogSpew].Value}
  /delay 1s ${Navigation.Active}

  DEBUG ${sep}moving to${sep}\a-w${_tmpID}\ax
  /while (${Navigation.Active}) {
    
    GETINPUT

    | am I dead on the move
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) {
      DEBUG ${sep}/while \a-gis_me_dead\ax
      /invoke ${target[clear]}
      /return FALSE
    }

    | is the target still alive?
    /if (!${_tmpID} || ${Spawn[${_tmpID}].Dead}) {
      DEBUG ${sep}/while \a-gbreak_NO_target\ax
      /invoke ${target[clear]}
      /break
    }

    | are we there yet???
    /if (${Spawn[${_tmpID}].Distance} > ${_dist} && !${Spawn[${_tmpID}].LineOfSight}) {
      | DEBUG ${sep}dis check${sep}\a-w${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${Spawn[${_tmpID}].Y} ${Spawn[${_tmpID}].X} ${Spawn[${_tmpID}].Z}]} > ${_dist} && !\arLOS\ax
      DEBUG ${sep}dis check${sep}\a-w${Spawn[${_tmpID}].Distance} > ${_dist} && !\arLOS\ax
      /delay 5
    }

    | hell yah.. were here
    /if (${Spawn[${_tmpID}].Distance} < ${_dist} && ${Spawn[${_tmpID}].LineOfSight}) {
      | DEBUG ${sep}dis \arHALT\ax${sep}\a-w${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${Spawn[${_tmpID}].Y} ${Spawn[${_tmpID}].X} ${Spawn[${_tmpID}].Z}]} < \ag${_dist}\ax && \agLOS\a-w (\a-r${_tmpID}\ax)
      DEBUG ${sep}dis \arHALT\ax${sep}\a-w${Spawn[${_tmpID}].Distance} < \ag${_dist}\ax && \agLOS\a-w (\a-r${_tmpID}\ax)
      /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
      /return TRUE
    }

    | mobs in our personal space. fuck those things. no respect
    /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} & ${NearestSpawn[1, npc radius SAFEENV PS4 targetable].ID} != ${_tmpID}) {
      DEBUG ${break}mob in environment space
      /invoke ${out[9, 0, 26]}
      /invoke ${target[clear]}
      /varset _tmpID ${NearestSpawn[1, npc radius SAFEENV PS4 targetable].ID}
      /return TRUE
    }

    /delay 5
  }

/return TRUE



|***
 * note: Finds and sets stAssTarID for tasks
 * use: /call task_mob_find DEBUG [# to force SpawnCount increment]
 ***|
sub task_mob_find(bool _debug, int _spCount, string _tag)
  DEBUG \attask_mob_find\ax(\a-w${_spCount}\ax)

  /if (!${Bool[${_spCount}]}) {
    /varset _spCount 0
  }

  /if (!${swModeToken}) {
    /if (HOME && !${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
      DEBUG ${break}no token for mode. stopping
      /call Bind_control_home return NULL NULL ${maDebug.Find[home].Value}
    }
  }

  /if (${_spCount}) {
    /varset stAssTarID 0
    /varset targetData 0
  }

  | we have an existign target and not using a mode
  /if (TARGET && !${_spCount} && !${swModeToken}) {
    /return TARGET
  }

  /declare _tmpID int local
  /declare _tmpAgroRange int local ${Int[${Math.Calc[PULLENV/5]}]}
  /declare _count int local 0
  /declare _mobHP int local 999
  /declare _tmpHPAdj int local 0
  /declare _los string local
  /declare _closestNAV int local PULLENV
  /declare _closestID int local 0
  /declare _navRange int local 0
  /declare _mobcount int local 0

  | /declare _tmpCombatRange int local ${maHard.Find[stMinBowDist].Value}

  | adjust LOS?
  /if (${maOver.Find[swOverLOS].Value} || ${Select[${maCC.Find[stPullMode].Value},nav,int,pet,calm,multi]}) {
    DEBUG ${sep} LOS overridden
    /varset _los
  }

  | time to step the agro range out so we get mobs closer first
  DEBUG ${sep}range${sep}\aw${maEnv.Find[stMobLvlMin].Value}\ax-\aw${maEnv.Find[stMobLvlMax].Value}\ax zradius${sep}\a-w${maPull.Find[stPullZRadius].Value}\ax

  /while (${_tmpAgroRange} <= PULLENV && !TARGET) {
    /delay 1

    /if (${_tmpAgroRange} <= SAFEENV) {
      /varset _mobcount ${SpawnCount[npc ${_los} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
    } else {
      /varset _mobcount ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
    }

    | search local ENV range in steps of 10 for mob acquisition
  DEBUG ${sep}SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}\ax radius${sep}\ay${_tmpAgroRange}
    DEBUG ${sep}count:\aw${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}\ax radius${sep}\ay${_tmpAgroRange}\ax

    /if (${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}) {

      /for _count 1 to ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}

        ISMEDEAD

        /varset _tmpID ${NearestSpawn[${If[${_spCount},${Math.Calc[${_count}+${_spCount}]},${_count}]}, npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value}].ID}
        DEBUG ${sep}_tmpID${sep}\a-w${_tmpID}\ax navlength${sep}\a-w${Navigation.PathLength[id ${_tmpID}]}\ax

        | excluded mobs
        /if (${lsZoneExclude.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
          DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsZoneExclude\ax (\a-r${Spawn[${_tmpID}].DisplayName}\ax)
          /continue
        }

        | animation checks
        | /if (${lsAnimationNoTarget.Contains[${Spawn[id ${_tmpID}].Animation}]}) {
        |   DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsAnimationNoTarget\ax (\a-r${Spawn[id ${_tmpID}].Animation}\ax)
        |   /varset _spCount 1
        |   /continue
        | }

        | fish are friends, not food
        /if (${lsNoAttackBodyType.Contains[${Spawn[id ${_tmpID}].Body}]}) {
          DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsNoAttackBodyType\ax (\a-r${Spawn[id ${_tmpID}].Body}\ax)
          /varset _spCount 1
          /continue
        }

        | can i come into the out now?
        /if (${Spawn[id ${_tmpID}].Type.Equal[NPC]}) {
          /if (${lsNoTargetNPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
            DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetNPC\ax (\a-r${Spawn[${_tmpID}].DisplayName}\ax)
            /varset _spCount 1
            /continue
          }
        }

        | PCs to not consider
        /if (${Spawn[id ${_tmpID}].Type.Equal[PC]}) {
          /if (${lsNoTargetPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
            DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetPC\ax (\a-r${Spawn[${_tmpID}].DisplayName}\ax)
            /varset _spCount 1
            /continue
          }
        }

        | magic creatures to not consider
        /if (${Spawn[id ${_tmpID}].Type.Equal[Magic]}) {
          /if (${lsNoTargetMagic.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
            DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetMagic\ax (\a-r${Spawn[${_tmpID}].DisplayName}\ax)
            /varset _spCount 1
            /continue
          }
        }

        /if (${_tmpID} == ${Me.Pet.ID}) {
          /continue
        }

        | CYA for shit MQ will pick up in the environment that does not have a name
        /if (${Spawn[ID ${_tmpID}].DisplayName.Length} < 1) {
          DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wDisplayName.Length < 1\ax
          /varset _spCount 1
          /continue
        }

        | check navigation path validity
        /if (!${Navigation.PathExists[id ${_tmpID}]} && !${maOver.Find[swOverNavCheck].Value}) {
          DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wNavigation.PathExists[\a-yid ${_tmpID}\ax]\ax (\a-r${Spawn[id ${_tmpID}].DisplayName}\ax)
          /varset _spCount 1
          /continue
        }

        | pet tagging mezed mobs only (should be skipped. pets cannot break mez)
        | skip this if we are the group main assist
        /if (${Group.MainAssist.Name.NotEqual[${Me.DisplayName}]}) {
          /if (${maCC.Find[stPullMode].Value.Equal[pet]} && ${lsAnimationMezed.Contains[${Spawn[id ${_tmpID}].Animation}]}) {
            /continue
          }
        }

        |** check nav length
        /if (${Navigation.PathLength[id ${_tmpID}]} > -1) {
          /varset _navRange ${Navigation.PathLength[id ${_tmpID}]}

          | sort by closest nav path length
          /if (${_navRange} <= ${_closestNAV}) {
            /varset _closestNAV ${_navRange}
          } else /if (${_navRange} > ${_closestNAV}) {
            /continue
          }
        } else /if (${Navigation.PathLength[id ${_tmpID}]} <= 0) {
          /continue
        }

        **|

        | added because MQ sucks ass for getting accurate HP values returned over a mouse dick in length.
        /if (${Spawn[id ${_tmpID}].PctHPs} > 100 && ${_tmpAgroRange} > 200) {
          /varset _tmpHPAdj 100
        } else {
          /varset _tmpHPAdj ${Spawn[id ${_tmpID}].PctHPs}
        }

        | get the lowest HP mob if we can
        /if (${_tmpHPAdj} <= ${_mobHP}) {
          /varset _mobHP ${Spawn[id ${_tmpID}].PctHPs}
          /varset stAssTarID ${_tmpID}
          /invoke ${maCC.Add[swSetCombat,TRUE]}
          DEBUG ${sep}T2K${sep}\awTARGET\aw${sep}\ay${Spawn[TARGET].DisplayName}\ay ${sep} ${If[${Spawn[TARGET].LineOfSight},\ag${Spawn[TARGET].Distance}\ax,\ar${Spawn[TARGET].Distance}\ax]}
        }

        | is this a priority mob?
        /if (${lsZoneNoMez.Contains[${Spawn[ID ${_tmpID}].DisplayName}]} || ${lsZoneNamed.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
          DEBUG ${sep}Priority Mob${sep}\a-w${Spawn[id ${_tmpID}].DisplayName}\ax
          | off tanks have to sort by a different position
          /if (TARGET && ${Me.XTarget[1].ID} == ${_tmpID}) {
            DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wMe.XTarget[1].ID != _tmpID\ax
            /continue
          }
          /return TARGET
        }

      /next _count

    }

    /if (TARGET) /return TARGET
    /varcalc _tmpAgroRange ${_tmpAgroRange}+${Math.Calc[PULLENV/5]}
  }

  /if (!TARGET) {
    /delay 5
  }

/return TARGET




