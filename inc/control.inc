|*********************************************************************
 * entropy
 * control.inc
 *
 ***|




|***
 * note: debug control
 * use: /debug []
 ***|
#bind control_debug /debug
sub Bind_control_debug(string _type, string _verbage, bool _debug)
  DEBUG \atBind_control_debug\ax(\a-w${_type}\ax)

  /if (${_type.Left[1].Equal[-]}) {
   /lua run HELP debug ${_type.Right[1]}
   /return TRUE
  }

  /if (${_type.Equal[ui]}) {
    /lua run ${MacroQuest.Path[macros]}\entropy\lua\debug.lua
    /return TRUE
  }

  /declare _count int local 0

  /if (${_type.Equal[stop]}) {
    /declare _ligdeb listiterator local
    /vardata _ligdeb lsDebuglist.First.Clone
    /declare _lideb listiterator local
    /while (!${_ligdeb.IsEnd}) {
      /vardata _lideb lsDebug${_ligdeb.Value}.First.Clone
      /while (!${_lideb.IsEnd}) {
        /invoke ${maDebug.Add[${_lideb.Value},FALSE]}
        /invoke ${_lideb.Advance}
      }
      /invoke ${_ligdeb.Advance}
    }
    /invoke ${out[12, "/debug \arSTOP\ax"]}
    /return TRUE
    
  } else /if (${lsDebuglist.Contains[${_type.Lower}]}) {

    /if (${Bool[${_verbage}]}) {
      /invoke ${maDebug.Add[${_verbage.Lower},${If[${maDebug.Find[${_verbage.Lower}].Value},FALSE,TRUE]}]}
    }

    /declare _libug listiterator local
    /vardata _libug lsDebug${_type.Lower}.First.Clone
    /declare _out string local

    /while (!${_libug.IsEnd}) {
      /varset _out ${_out} ${If[${_libug.Value.Equal[${lsDebug${_type.Lower}.First}]},,${dot}]} ${If[${maDebug.Find[${_libug.Value}].Value},${good}${_libug.Value}\ax,${info}${_libug.Value}\ax]}
      /invoke ${_libug.Advance}
    }
    /invoke ${out[12, "/debug ${_type.Lower} ${dot} ${_out}"]}

    /return TRUE
  }

  /declare _libug listiterator local
  /vardata _libug lsDebuglist.First.Clone
  /declare _out string local

  /while (!${_libug.IsEnd}) {
    /varset _out ${_out} ${If[${_libug.Value.Equal[${lsDebuglist.First}]},,${dot}]} ${info}${_libug.Value}\ax:${submenu}
    /invoke ${_libug.Advance}
  }
  /varset _out ${_out} ${dot} \ar<\ax${info}stop\ax\ar>\ax
  /invoke ${out[12, "/debug ${dot} ${_out}"]}

/return TRUE



|***
 * note: help
 * use: /-h option
 ***|
#Bind command_help /-h
sub Bind_command_help(string _type, bool _debug)

  /declare _lsHelp list local
  /invoke ${_lsHelp.Append[alto,beg,buff,build,burn,cc,crew,cursor,debuff,debug]}
  /invoke ${_lsHelp.Append[enter,cure,heal,home,invis,mode,minion,navto,off]}
  /invoke ${_lsHelp.Append[on,override,repeat,see,splash,take,tag,rez,tag,entropy]}
  /invoke ${_lsHelp.Append[agro,miscdps,song,area,sash,pred,mercenary,tie,tc]}
  /invoke ${_lsHelp.Append[coh,farm,ttl,defense,dot,nuke,clickitem,env,grab,grain]}
  /invoke ${_lsHelp.Append[gtfo,pull,cb,here,nomore,edit]}
  
  /if (${_lsHelp.Contains[${_type}]}) {
    ECHO
    /lua run HELP ${_type} u
    /delay 1
    /lua run HELP ${_type} h
    /invoke ${out[18, 0, 33]}
    /return TRUE
  }
  
  ECHO
  /sqlite query "${DBstatic}" dummy SELECT value FROM online WHERE env_var='discord'
  /if (!${sql_check[${_debug}, dummy, Bind_command_aux, SELECT]}) /endmacro
  /invoke ${out[0, "Discord${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}
  /declare _out string local
  /declare _liaux listiterator local
  /invoke ${out[0, "\aw/-h [\a-w option \ax]"]}
  /varset _out
  
  /declare _count int local 0
  /declare _liho listiterator local
  /vardata _liho _lsHelp.First.Clone
  /while (!${_liho.IsEnd}) {
    /varcalc _count ${_count} +1
    /varset _out ${_out} ${dot} ${info}${_liho.Value}\ax
    /if (${_count} == 10) {
      /invoke ${out[0, "${_out}"]}
      /varset _out
      /varset _count 0
    }
    /invoke ${_liho.Advance}
  }
  /invoke ${out[0, "${_out}"]}
  /invoke ${out[18, 0, 33]}

/return TRUE



|***
 * note: mode control
 * use: /mode []
 ***|
#bind control_mode /mode
sub Bind_control_mode(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atBind_control_mode\ax(\a-w${_type}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP mode ${_type.Right[1]}
    /return TRUE
  }

  /if (${_type.Equal[stop]}) {
    /declare _lisie listiterator local
    /vardata _lisie lsMode.First.Clone
    /while (!${_lisie.IsEnd}) {
      /invoke ${maMode.Add[${_lisie.Value},FALSE]}
      /invoke ${_lisie.Advance}
    }
    /invoke ${out[12, "/mode \arSTOP\ax"]}
    /return TRUE

  } else /if (${_type.Equal[data]}) {

    /if (${_verbage.Equal[target]} && ${Target.ID}) {
      /varset stModeData "${Target.DisplayName}"
    } else {

      /varset stModeData ${_verbage}
    }

  } else /if (${_type.Equal[farm]}) {

    /if (!${SubDefined[${_verbage}]}) {
      /invoke ${out[20, "Invalid farm${sep}\a-w${_verbage}\ax"]}
      /return FALSE

    } else /if (${SubDefined[${_verbage}]}) {
      /invoke ${out[20, "Starting farm${sep}\a-w${_verbage}\ax"]}
      /call ${_verbage} ${_verbage2}
      /return TRUE
    }

  } else /if (${maMode.Contains[${_type}]}) {

    /invoke ${maMode.Add[${_type},${If[${maMode.Find[${_type}].Value},FALSE,TRUE]}]}

    /if (${_type.Equal[petfarm]} && ${Bool[${_verbage}]}) {
      /call petfarm_route ${_verbage} ${_verbage2}
      /return
    }
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[forage,option]}
  /invoke ${maControl.Add[harvest,option]}
  /invoke ${maControl.Add[fish,option]}
  /invoke ${maControl.Add[farm,option]}
  /invoke ${maControl.Add[hunt,option]}
  /invoke ${maControl.Add[lush,option]}
  /invoke ${maControl.Add[summon,option]}
  /invoke ${maControl.Add[scribe,option]}
  /invoke ${maControl.Add[temppet,option]}
  /invoke ${maControl.Add[trainskill,option]}
  /invoke ${maControl.Add[trainspell,option]}
  /invoke ${maControl.Add[data,stModeData|br_no_c]}
  /invoke ${maControl.Add[petfarm,option]}
  /invoke ${maControl.Add[summon,option]}
  /invoke ${maControl.Add[drag,option]}

  /invoke ${maControl.Add[stop,option]}
  /invoke ${set_control_output[${_debug}, maMode, mode, maControl]}

/return TRUE



|***
 * note: administration control
 * use: /admin
 ***|
#bind control_admin /admin
sub Bind_control_admin(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
  DEBUG \atBind_control_admin\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

  | cehck for undeclared variables
  /if (${_type.Equal[checkvar]}) {
    OUT Checking for any undefined variables.
    /invoke ${Macro.Undeclared}
    /invoke ${out[18, 0, 33]}
    /return TRUE

  | pull target marking
  } else /if (${_type.Equal[pull]}) {

    /invoke ${maSD.Add[debug,TRUE]}
    /invoke ${maSD.Add[type,${_verbage}]}

    /call build_map_spawndata
    /call pull_mob_find_v2
    OUT
    OUT ${notice} make sure you issue a `/nav stop` command
    /return FALSE


  | check observer view
  } else /if (${_type.Equal[obs]}) { 
    OUT Observed${sep}\a-w${_verbage}\ax
    /declare _count int local 0
    /declare _obs string local
    /for _count 1 to ${DanNet[${_verbage}].Observe.Count[|]}
      /varset _obs ${DanNet[${_verbage}].Observe.Arg[${_count},|]}
    OUT ${_count}${sep}\a-w${_obs}\ax (\a-r${DanNet[${_verbage}].Observe["${_obs}"]}\ax)
    /next _count
    /invoke ${out[18, 0, 33]}
    /return TRUE
    
  } else /if (${_type.Equal[doloot]}) {
    OUT Forcing 1 cycle of the loot routine.
    /call do_loot ${maDebug.Find[loot].Value}
    /invoke ${out[18, 0, 33]}
    /return TRUE

  | reset dannet.. if you need too
  } else /if (${_type.Equal[reset]}) {
    /if (!${Bool[${_verbage}]}) {
      OUT This is for resetting plugins.
      OUT eg. \a-w/admin reset dannet\ax
      OUT will reset the mq2dannett plugin.
      /return FALSE
    }
    
    /if (!${Bool[${Plugin[mq2${_verbage.Lower}]}]}) {
      OUT Invalid or unloaded plugin${sep}\a-wMQ2${_verbage}\ax
      /return FALSE
    }

    /invoke ${out[0, "Resetting Plugin${sep}\a-wmq2${_verbage.Lower}\ax"]}
    /plugin mq2${_verbage.Lower} unload
    /delay 3s
    /plugin mq2${_verbage.Lower}
    /invoke ${out[18, 0, 33]}
    /return TRUE

  } else /if (${_type.Equal[pregen]}) {
    /invoke ${out[0, "Checking \aw${MacroQuest.Server.Replace[ ,_].Lower}_${Me.Class.ShortName}_${Me.DisplayName}\ax${sep}\awenvironment_character\ax table for updated skills/spells/AAs${sep}"]}
    /varset pregenUpdate TRUE
    /call class_init ${maDebug.Find[init].Value}
    /varset pregenUpdate FALSE
    /invoke ${out[18, 0, 33]}
    /return TRUE

  } else /if (${_type.Equal[sqlcheck]}) {
    /call sql_schema_stale_check ${maDebug.Find[sql].Value} TRUE
    /call sql_schema_shared ${maDebug.Find[sql].Value} TRUE
    /invoke ${out[18, 0, 33]}
    /return TRUE

  } else /if (${_type.Equal[sqldrop]}) {
    
    OUT This is for dropping sqlite tables. Probably shouldn't be doing ths.
    OUT \a-w/admin sqldrop TABLE FIELD\ax
    
    /if (${Bool[${_verbage}]} && ${Bool[${_verbage2}]}) {
      /call sql_drop ${_verbage} ${_verbage2} TRUE
    }
    /invoke ${out[18, 0, 33]}
    /return TRUE

  | see output resonses from database (strings)
  } else /if (${_type.Equal[out]}) {
    OUT Listing stored output strings.
    /declare _count int local 0 
    /sqlite query "${DBstatic}" dummycount SELECT Count(*) AS data FROM string;
    /for _count 1 to ${sqlite.Result[dummycount 1 data]}
      /sqlite query "${DBstatic}" dummy SELECT * FROM string WHERE ROWID=${_count};
      OUT ${_count}${sep}${Parse[1,${sqlite.Result[dummy 1 value]}]}
    /next _count
    /invoke ${out[18, 0, 33]}
    /return TRUE

  | look for a column in a table
  } else /if (${_type.Equal[sqlcol]}) {

    | testing afor a known column first
    /sqlite query "${DBzone}" dummy SELECT 1 FROM PRAGMA_TABLE_INFO('zone_control') WHERE name='element'
    /if (!${sql_check[${_debug}, dummy, Bind_control_admin, "SELECT 1 (PRAGMA_TABLE_INFO)"]}) /endmacro
    DEBUG SELECT 1 FROM PRAGMA_TABLE_INFO 'zone_control' \ay${sqlite.Status[dummy]}\ax
    DEBUG Rows${sep}\ap${sqlite.Rows[dummy]}\ax


    | testing unknown
    /sqlite query "${DBzone}" dummy SELECT 1 FROM PRAGMA_TABLE_INFO('zone_control') WHERE name='wp_name'
    DEBUG SELECT 1 FROM PRAGMA_TABLE_INFO 'zone_control' \ay${sqlite.Status[dummy]}\ax

    DEBUG Rows${sep}\ap${sqlite.Rows[dummy]}\ax
    /invoke ${out[18, 0, 33]}
    /return TRUE

  | delete entropy aliases
  } else /if (${_type.Equal[whitewash]}) {
    /call set_alias delete ${maDebug.Find[init].Value}
    /return TRUE

  | parse SPA data
  } else /if (${_type.Equal[spa]}) {
    ECHO

    /if (!${Bool[${_verbage}]}) {
      OUT Display the SPA data for a given spell (A)ffect
      OUT \a-w/admin spa "EXACT SPELLNAME with ranks"\ax or gem#
      /return FALSE
    }
  

    | reset gem as value if needed
    /if (${_verbage.Find[gem]} && ${_verbage.Length} < 6) {
      /varset _verbage ${Me.Gem[${_verbage.Right[-3]}]}
    } else /if (${_verbage.Find[buff]} && ${_verbage.Length} < 7) {
      /varset _verbage ${Me.Buff[${_verbage.Right[-4]}]}
    } else /if (${_verbage.Find[song]} && ${_verbage.Length} < 7) {
      /varset _verbage ${Me.Song[${_verbage.Right[-4]}]}
    }    
      
    /if (${Defined[spellData]}) {
      /deletevar spellData
    }

    /declare spellData spell outer
    /vardata spellData Spell[${_verbage}]    
    /declare _list string local
    /declare _count int local 0
    
    MQCLEAR
    OUT \aw${spellData.Name}\ax ${sep} ID\a-w ${spellData}\ax ${sep} Group \a-w${spellData.SpellGroup}\ax ${sep} lvl \a-w${spellData.Level}\ax
    OUT \awTarget Type ${sep} \a-w${spellData.TargetType}\ax
    OUT \awSub Category ${sep} \a-w${spellData.Subcategory}\ax
    OUT \awSPA Count\ax ${sep} \a-w${spellData.NumEffects}\ax

    /for _count 1 to ${spellData.NumEffects}
      OUT #\ay${_count}\ax
      /if (${Select[${spellData.Attrib[${_count}]},10]}) /continue
      /sqlite query "${DBstatic}" dummy SELECT * FROM eq_spa WHERE env_var='${spellData.Attrib[${_count}]}'
      OUT ${dot} .Attrib \a-w${spellData.Attrib[${_count}]}\ax ${dot} \a-g${sqlite.Result[dummy 1 value]}\ax
      OUT ${dot} .Trigger \a-w${spellData.Trigger[${_count}].Name}\ax ${dot} ID:\a-w${spellData.Base2[${_count}]}\ax\ax ()
      OUT ${dot} .Base \a-w${spellData.Base[${_count}]}\ax
      OUT ${dot} .Max \a-w${spellData.Max[${_count}]} \ax
      OUT ${dot} .Calc \a-w${spellData.Calc[${_count}]} \ax
      OUT ${dot} .Restrictions \a-w${spellData.Restrictions[${_count}]}\ax
      OUT ${dot} .Extra \a-w${spellData.Extra}\ax
      /varset _list ${_list} ${spellData.Attrib[${_count}]}
    /next _count
    OUT ${dot} ${dot} ${dot} ${dot}
    OUT \awSPA list\ax ${sep} \a-w${_list}\ax

    /deletevar spellData
    /invoke ${out[18, 0, 33]}
    /return TRUE

  } else /if (${_type.Equal[import]}) {

      OUT loot import is currently disabled
      /return TRUE
    
    /if (${_verbage.Equal[loot]}) {
  
      MQCLEAR
      | current core loot file
      /declare _lootini string local tc\\data\\core.loot.ini
      /declare _section string local
      /declare _key string local
      /declare _value string local
      /declare _countS int local 0
      /declare _loopKey int local 0
      /declare _totalCount int local 0

      /invoke ${out[12, "\awImporting Loot File from\ax\at...\ax\a-w${_lootini}\ax"]}
      /invoke ${out[12, "On large loot files this will take a second to build."]}

        /for _countS 0 to ${Ini[${_lootini}].Count[|]}

          /varset _section ${Ini[${_lootini}].Arg[${_countS},|]}

          /if (!${Ini[${_lootini},${_section}].Count[|]}) {
            /ini "${_lootini}" "${_section}" NULL NULL
            /continue
          }

          /invoke ${out[12, "\at..\ax\aw${_section}"]}

          | are there keys in the section?
          /while (${Ini[${_lootini},${_section}].Count[|]}) {

            | get the key
            /varset _key ${Ini[${_lootini},${_section}].Arg[0,|]}

            | delete key if NULL and go to next
            /if (${Select[${_key},NULL]}) {
              /ini "${_lootini}" "${_section}" "${_key}" NULL
              | /varcalc _loopKey ${_loopKey}+1
              /continue
            }

            | get the key value
            /varset _value ${Ini[${_lootini},${_section},${_key}].Arg[0,|]}

            | delete the key if the value is null and move to next
            /if (${Select[${_value},NULL]}) {
              /ini "${_lootini}" "${_section}" "${_key}" NULL
              /continue
            }

            | add key->value to DB and delete
            /sqlite query "${DBloot}" dummy INSERT INTO loot_control VALUES ("${_key}", "${_value}", 0, 0)
            /ini "${_lootini}" "${_section}" "${_key}" NULL

          }
          /next _countS

      }

      /invoke ${out[18, 0, 33]}
      /return TRUE

    } else /if (${_type.Equal[see]}) {
      /if (!${Bool[${_verbage}]}) {
        OUT lets you see an unfiltered map or list contents.
        OUT \a-w/admin see map|list\ax
        /return FALSE
        
      }

      | wtf.. list...
      /if (${_verbage.Left[2].Equal[ls]}) {
        /invoke ${out[12, "\aw${_verbage}\ax ${dot} \a-g${${_verbage}.Count}\ax ${If[${${_verbage}} > 1 || ${${_verbage}} == 0,items,item]}"]}
        /if (!${${_verbage}.Count}) /return
        /declare _lias listiterator local
        /vardata _lias ${_verbage}.First.Clone
        /while (!${_lias.IsEnd}) {
          /invoke ${out[12, "${dot}\a-w${_lias.Value}\ax"]}
          /invoke ${_lias.Advance}
        }

      | map
      } else /if (${_verbage.Left[2].Equal[ma]}) {
        
        /invoke ${out[12, "\aw${_verbage}\ax ${dot} \a-g${${_verbage}.Count}\ax ${If[${${_verbage}} > 1 || ${${_verbage}} == 0,items,item]}"]}
        /if (!${${_verbage}.Count}) /return

        /declare _mias mapiterator local
        /vardata _mias ${_verbage}.First.Clone

        /while (!${_mias.IsEnd}) {
          /invoke ${out[12, "${_mias.Key}${sep}\aw${Parse[1,${_mias.Value}]}\ax"]}

          /invoke ${_mias.Advance}
        }
      }
      /invoke ${out[18, 0, 33]}
      /return TRUE

    } else /if (${_type.Equal[keymap]}) {
      /if (!${Bool[${_verbage}]}) {
        OUT /admin keymap - finds the map location of a variable
        OUT \a-w/admin keymap VARIABLENAME\ax
        /return FALSE 
      }
      /call get_key_map TRUE ${_verbage} TRUE
      /invoke ${out[18, 0, 33]}
      /return TRUE

    | refresh zone data
    } else /if (${_type.Equal[zonerefresh]}) {
      OUT Refreshing current zone data.
      /call set_zone_lists ${maDebug.Find[area].Value}
      /if (${lsGrain.Contains[1]}) {
        OUT ${num}${Zone.Name}\ax${sep}\a-w${Zone.ShortName}\ax
      }
     /return TRUE
    }

    /if (${_verbage.Equal[zone]}) {
      MQCLEAR
      | current core loot file
      /declare _ini string local exspes\\zonedata.ini
      /declare _section string local
      /declare _key string local
      /declare _value string local
      /declare _pass string local
      /declare _countS int local 0
      /declare _countK int local 0
      /declare _countV int local 0

      ECHO \awImporting Named File from\ax\at...\ax\a-w${_ini}\ax
      ECHO \a-yINI sections\ax::\a-w${Ini[${_ini}].Count[|]}\ax

      /for _countS 1 to ${Ini[${_ini}].Count[|]}
        /varset _section ${Ini[${_ini}].Arg[${_countS},|]}
        /if (${Select[${_section},NULL]}) /continue
        /invoke ${out[12, "\at..\ax\aw${_section}\ax"]}
        /for _countK 1 to ${Ini[${_ini},${_section}].Count[|]}
          /varset _key ${Ini[${_ini},${_section}].Arg[${_countK},|]}
          /if (${Select[${_key},NULL]}) /continue

          /for _countV 1 to ${Ini[${_ini},${_section},${_key}].Count[|]}
            /varset _value ${Ini[${_ini},${_section},${_key}].Arg[${_countV},|]}
            /invoke ${out[12, "\at...\ax\a-w${_value}\ax"]}
            /sqlite query "${DBzone}" dummy INSERT OR IGNORE INTO zone_control VALUES ("${_section}", "${_key}", "${_value}")
          /next _countV
        /next _countK
      /next _countS
      /invoke ${out[18, 0, 33]}
      ECHO
      /return
    }


  /if (${_silent.Equal[SILENT]}) /return
  /invoke ${out[12, "PID${sep}\a-w${EverQuest.PID}\ax"]}
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[checkvar,out]}
  /invoke ${maControl.Add[spa,out]}
  /invoke ${maControl.Add[pull,out]}
  /invoke ${maControl.Add[obs,out]}
  /invoke ${maControl.Add[whitewash,option]}
  | /invoke ${maControl.Add[import,out]}
  /invoke ${maControl.Add[keymap,out]}
  /invoke ${maControl.Add[see,out]}
  /invoke ${maControl.Add[sqlcheck,option]}
  /invoke ${maControl.Add[sqldrop,out]}
  /invoke ${maControl.Add[out,out]}
  /invoke ${maControl.Add[zonerefresh,option]}
  /invoke ${maControl.Add[doloot,option]}
  /invoke ${set_control_output[${_debug}, maEnv, "admin", maControl]}

/return TRUE



|***
 * note: environment control
 * use: /env
 ***|
#bind control_env /env
sub Bind_control_env(string _type, string _verbage, string _silent, bool _debug)
  DEBUG \atBind_control_env\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP env ${_type.Right[1]}
    /return TRUE
  }

  SETHUD
  | macro auto/manual
  /if (${_type.Equal[auto]}) {
    /invoke ${set_switch_env[${_debug}, swAuto, FALSE, ${_verbage}]}
    /invoke ${out[12, "/env ${If[${maEnv.Find[swAuto].Value},${good}auto\ax,${bad}auto\ax]}"]}

    /if (!${maEnv.Find[swAuto].Value}) {
      /if (HOME) {
        /call Bind_control_home clear NULL NULL ${maDebug.Find[home].Value}
      }
      /invoke ${target[clear]}
    } else /if (${_type.Equal[auto]}) {
      /if ((${maPull.Find[swPullSetHome].Value} && PULL) || ${maHome.Find[swonAuto].Value}) {
        /if (!${swModeToken}) {
          /call Bind_control_home set NULL NULL ${maDebug.Find[home].Value}
        }
      }
    }
    /invoke ${out_popup["/env auto ${maEnv.Find[swAuto].Value}"]}
    /return TRUE

  | enable wasiting fireworks for those bastards with too much AA on their hands
  } else /if (${_type.Equal[fireworks]}) {
    /invoke ${set_switch_env[${_debug}, swAAFireworks, TRUE, ${_verbage}]}

  | enable bagging from mages for rods and pet gear
  } else /if (${_type.Equal[begmage]}) {
    /invoke ${set_switch_env[${_debug}, swBegMAG, TRUE, ${_verbage}]}

  | sometimes.. sometimes, i dont like people
  } else /if (${_type.Equal[dropinvis]}) {
    /invoke ${set_switch_env[${_debug}, swAlwaysDropInvisCauseThePeopleIGroupWithAreAssHats, TRUE, ${_verbage}]}

  | take raid invites
  } else /if (${_type.Equal[takeri]}) {
    /invoke ${set_switch_env[${_debug}, swTakeRaidInvite, TRUE, ${_verbage}]}

  | take group invites
  } else /if (${_type.Equal[takegi]}) {
    /invoke ${set_switch_env[${_debug}, swTakeGroupInvite, TRUE, ${_verbage}]}

  | exp settings
  } else /if (${_type.Equal[exp]}) {

    | exp max level
    /if (${_verbage.Equal[maintain]}) {
      /if (!${set_control_num_range[${_debug}, stLvlPct, "${_silent}", 0, 99]}) /return

    | exp maintain level
    } else /if (${_verbage.Equal[level]}) {
      /if (!${set_control_num_range[${_debug}, stLvlMax, "${_silent}", 0, 115]}) /return

    | exp auto adjust
    } else /if (${_verbage.Equal[adjust]}) {
      /invoke ${set_switch_env[FALSE, swCorrectEXPAA, TRUE, ${_silent}]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[adjust,swCorrectEXPAA]}
    /invoke ${maControl.Add[level,stLvlMax]}
    /invoke ${maControl.Add[maintain,stLvlPct]}
    /invoke ${set_control_output[${_debug}, maEnv, "env exp", maControl]}
    /return

  | food/drink
  } else /if (${_type.Equal[meal]}) {
    /if (${_verbage.Equal[food]}) {
      /invoke ${do_raw_edit[FALSE, SILENT, stFood, "${_silent}"]}
    } else /if (${_verbage.Equal[drink]}) {
      /invoke ${do_raw_edit[FALSE, SILENT, stDrink, "${_silent}"]}
    }

    /invoke ${out[12, "/env meal${sep}"]}
    /invoke ${out[12, "${dot} ${info}food\ax:[${If[${Bool[${maEnv.Find[stFood].Value}]},${num}${maEnv.Find[stFood].Value}\ax,${off}]}]"]}
    /invoke ${out[12, "${dot} ${info}drink\ax:[${If[${Bool[${maEnv.Find[stDrink].Value}]},${num}${maEnv.Find[stDrink].Value}\ax,${off}]}]"]}
    /return

  } else /if (${_type.Equal[moblvlmin]}) {
    /if (!${set_control_num_range[${_debug}, stMobLvlMin, "${_verbage}", 1, 500]}) /return

  | set mob maximum level for attacking
  } else /if (${_type.Equal[moblvlmax]}) {
    /if (!${set_control_num_range[${_debug}, stMobLvlMax, "${_verbage}", 1, 500]}) /return

  | loot modes
  } else /if (${_type.Equal[loot]}) {
    /if (${_verbage.Equal[off]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stLootMode, "off"]}
    } else /if (${_verbage.Equal[manual]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stLootMode, "manual"]}
    } else /if (${_verbage.Equal[advanced]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stLootMode, "advanced"]}
    }

    /declare _liL listiterator local
    /declare _out string local
    /vardata _liL lsLootMode.First.Clone
    /while (!${_liL.IsEnd}) {
      /varset _out ${_out} ${If[${_liL.Value.Equal[${lsLootMode.First}]},,${dot}]} ${If[${_liL.Value.Equal[${maEnv.Find[stLootMode].Value}]},${num}${_liL.Value}\ax,${info}${_liL.Value}\ax]}
      /invoke ${_liL.Advance}
    }
    /invoke ${out[12, "/env loot ${dot} ${_out}"]}
    /return TRUE    
    
    
  } else /if (${_type.Equal[autoinv]}) {
    /invoke ${set_switch_env[FALSE, swAutoInv, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[tribute]}) {
    /invoke ${set_switch_env[FALSE, swCheckTribute, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[glyphoutsideraid]}) {

  } else /if (${_type.Equal[safelistguild]}) {
    /invoke ${set_switch_env[FALSE, swSafeListGuild, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[buffgemhold]}) {
    /invoke ${set_switch_env[FALSE, swHoldBuffGem, TRUE, ${_verbage}]}

  | set buffgem
  } else /if (${_type.Equal[buffgem]}) {
    /if (!${lsClassCast.CountOf[${Me.Class.ShortName}]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stBuffGem, 0]}
      /invoke ${out[1, shade, 0]}
      /return FALSE
    }
    /if (${Range.Between[1,${Me.NumGems}:${Int[${_verbage}]}]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stBuffGem, "${_verbage}"]}
    } else {
      /invoke ${out[12, "/env buffgem(${num}${maEnv.Find[stBuffGem].Value}\ax) range ${info}1 - ${Me.NumGems}\ax"]}
      /return FALSE
    }

  | set fake safe radius
  } else /if (${_type.Equal[saferadius]}) {
    /if (!${set_control_num_range[${_debug}, stEnvSafeRadius, "${_verbage}", 0, 60]}) /return

  | set operation radius
  } else /if (${_type.Equal[rad]}) {
    /if (!${set_control_num_range[${_debug}, stEnvRadius, "${_verbage}", 0, 9999]}) /return

  } else /if (${_type.Equal[incharge]}) {
    /declare _count int local 0
    /declare _name string local FALSE

    | cycle all the toons you are boxing
    /for _count 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}
      /varset _name ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}

      | if i'm issuing the command. turn off tie and move along
      /if (${_name.Equal[${Me.DisplayName}]}) {
        /invoke ${do_raw_edit[${_debug}, SILENT, stTieToon, FALSE]}
        /continue
      }

      | if the toon is not in this zone, move along
      /if (!${Spawn[pc ${_name}].ID}) /continue

      | update the toons DB setting
      /dexecute ${_name} /squelch /target clear
      /dexecute ${_name} /tc toon ${Me.DisplayName}

    /next _count
    /invoke ${out[12, "Gator's bitches better be using jimmies!"]}
    /return

  | auto SOS... or not... ROGUES ONLY
  } else /if (${_type.Equal[sos]}) {
    /if (${Select[${Me.Class.ShortName},ROG]}) {
      /invoke ${set_switch_env[${_debug}, swSoS, TRUE, ${_verbage}]}
    } else {
      /invoke ${out[1, shade, 0]}
      /return FALSE
    }

  | shortcut for the build set routine
  } else /if (${_type.Equal[set]}) {
    /if (${Select[${_verbage},FALSE,off]} || !${Ini["${INIset}"].Find[${_verbage}]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stBuildSetCurrent, FALSE]}
    } else {
      /call Bind_control_build set ${_verbage}
      /return TRUE
    }

  }

  /if (${_silent.Equal[SILENT]}) /return
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[auto,swAuto|pr]}
  /invoke ${maControl.Add[rad,stEnvRadius]}
  | /invoke ${maControl.Add[rest,submenu]}
  /invoke ${maControl.Add[meal,submenu]}
  /invoke ${maControl.Add[exp,submenu]}
  /invoke ${maControl.Add[incharge,option]}
  /invoke ${maControl.Add[loot,submenu]}
  /invoke ${maControl.Add[autoinv,swAutoInv]}
  /invoke ${maControl.Add[buffgem,stBuffGem]}
  /invoke ${maControl.Add[buffgemhold,swHoldBuffGem]}
  /invoke ${maControl.Add[safelistguild,swSafeListGuild]}
  /invoke ${maControl.Add[moblvlmax,stMobLvlMax]}
  /invoke ${maControl.Add[moblvlmin,stMobLvlMin]}
  /invoke ${maControl.Add[begmage,swBegMAG]}
  /invoke ${maControl.Add[fireworks,swAAFireworks]}
  /invoke ${maControl.Add[takeri,swTakeRaidInvite]}
  /invoke ${maControl.Add[takegi,swTakeGroupInvite]}
  /invoke ${maControl.Add[saferadius,stEnvSafeRadius]}
  /invoke ${maControl.Add[tribute,swCheckTribute]}
  /invoke ${maControl.Add[dropinvis,swAlwaysDropInvisCauseThePeopleIGroupWithAreAssHats|pr]}

  /if (${Select[${Me.Class.ShortName},ROG]}) {
    /invoke ${maControl.Add[sos,swSoS]}
  }

  /invoke ${set_control_output[${_debug}, maEnv, env, maControl]}

/return TRUE



|***
 * note: class shortname bind conversion to /cls
 * use:
 ***|
#bind to_chr /chr
sub Bind_to_chr(string _type, string _verbage, string _silent, bool _debug)
  | DEBUG /call set_control _type:${_type} _verbage:${_verbage} _silent:${_silent} _debug:${_debug}

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP chr ${_type.Right[1]}
    /return TRUE
  }
  
  /call set_control ${_type} "${_verbage}" "${_silent}" "${_debug}"

/return TRUE



|***
 * note: force buff check routine
 * use: /cb
 ***|
#bind command_cb /cb
sub Bind_command_cb(string _type)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP cb ${_type.Right[1]}
    /return TRUE
  }
  
  /declare _list list local
  /invoke ${_list.Append[u,s,a,c,g,r,b,t,xx,robe]}

  /if (${Bool[${_type}]} && ${_list.Contains[${_type}]}) {
    /call Bind_control_nomore invis
    /call check_buff_cycle ${maDebug.Find[cb].Value} TRUE ${_type}
    /return TRUE
  } else /if (${Bool[${_type}]} && ${Select[${_type},m]}) {
    /call Bind_control_nomore invis
     /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
     /return TRUE
  }
  
  /invoke ${maControl.Clear}
  /declare _licb listiterator local
  /vardata _licb _list.First.Clone
  /while (!${_licb.IsEnd}) {
    /invoke ${maControl.Add[${_licb.Value},option]}

    /invoke ${_licb.Advance}
  }  
  /invoke ${maControl.Add[m,option]}
  /invoke ${set_control_output[FALSE, maBuff, cb, maControl]} 

/return TRUE



|***
 * note: watches for a spawn in zone
 * use: /watch [add|delete|reset] [current target]
 ***|
#bind control_watch /watch
sub Bind_control_watch(string _verbage, string _verbage2, bool _debug)
  DEBUG \atBind_control_watch\ax(\a-w${_verbage}, ${_verbage2}\ax)

  /if (${_verbage.Left[1].Equal[-]}) {
    /lua run HELP watch ${_verbage.Right[1]}
    /return TRUE
  }

  /if (${_verbage.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swWatch, TRUE, ${_verbage2}]}


  } else /if (${_verbage.Equal[beep]}) {
    /invoke ${set_switch_env[${_debug}, swWatchBeep, TRUE, ${_verbage2}]}

  } else /if (${_verbage.Equal[list]}) {
    OUT \awlsWatch\ax${sep}
    /call out_list lsWatch
    /return TRUE

  } else /if (${_verbage.Equal[add]}) {
    /if (${Target.ID}) {
      /call do_raw_edit ${_debug} FALSE lsWatch "${Target.DisplayName}" add
    } else /if (!${Target.ID}) {
      OUT /watch ${_verbage} requires a target.
      /return FALSE
    }

  } else /if (${_verbage.Equal[delete]}) {
    /if (${Target.ID}) {
      /call do_raw_edit ${_debug} FALSE lsWatch "${Target.DisplayName}" delete
    } else /if (!${Target.ID}) {
      OUT /watch ${_verbage} requires a target.
      /return FALSE
    }

  } else /if (${_verbage.Equal[clear]}) {
    /call do_raw_edit ${_debug} TRUE lsWatch clear

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[active,swWatch]}
  /invoke ${maControl.Add[add,option]}
  /invoke ${maControl.Add[beep,swWatchBeep]}
  /invoke ${maControl.Add[clear,option]}
  /invoke ${maControl.Add[delete,option]}
  
  /invoke ${set_control_output[${_debug}, maWatch, watch, maControl]}

/return TRUE



|***
 * note:control for /home
 * use:
 ***|
#Bind control_home /home
sub Bind_control_home(string _type, string _verbage, string _silent, bool _debug)
  DEBUG \atBind_control_home\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP home ${_type.Right[1]}
    /return TRUE
  }

  /declare _lsout list local
  /declare _out string local
  /declare _count int local 0

  | no mesh for zone
  /if (!${Navigation.MeshLoaded}) {
    /invoke ${out[1, shade, 0]}
    /invoke ${maHome.Add[swHomeMarker,FALSE]}
    /return
  }

  /if (${_type.Equal[campfire]}) {
    /invoke ${out[12, "Sorting Campfire"]}
    | need 3 people close
    /if (${SpawnCount[fellowship radius 50]} < 3) /return FALSE

    /if (!${Window[FellowshipWnd].Open}) /invoke ${Window[FellowshipWnd].DoOpen}
    /delay 10 !${Window[FellowshipWnd].Open}
    /nomodkey /notify FellowshipWnd FP_Subwindows tabselect 2
    /delay 1s

    | skip this if we dont have a fire
    /if (${Me.Fellowship.Campfire}) {
      /nomodkey /notify FellowshipWnd FP_DestroyCampsite leftmouseup
      /delay 5s ${Window[ConfirmationDialogBox].Open}
      /if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
      /delay 5s !${Me.Fellowship.Campfire}
    }

    /nomodkey /notify FellowshipWnd FP_RefreshList leftmouseup
    /delay 1s

    /if (!${FindItemCount[=Fellowship Campfire Materials]} || ${_verbage.Equal[base]}) {
      /nomodkey /notify FellowshipWnd FP_CampsiteKitList listselect 1
    } else /if (${FindItemCount[=Fellowship Campfire Materials]} || ${_verbage.Equal[advanced]}) {
      /nomodkey /notify FellowshipWnd FP_CampsiteKitList listselect 2
    }
    /delay 1s
    /nomodkey /notify FellowshipWnd FP_CreateCampsite leftmouseup
    /delay 5s ${Me.Fellowship.Campfire}
    /if (${Window[FellowshipWnd].Open}) /invoke ${Window[FellowshipWnd].DoClose}
    /return TRUE

  | position
  } else /if (${_type.Equal[position]}) {

    | stick wherever
    /if (${_verbage.Equal[free]}) {
      /call do_raw_edit ${_debug} SILENT stCombatPosition "free"
    | rear
    } else /if (${_verbage.Equal[rear]}) {
      /call do_raw_edit ${_debug} SILENT stCombatPosition "rear"
    | front
    } else /if (${_verbage.Equal[face]}) {
      /call do_raw_edit ${_debug} SILENT stCombatPosition "face"
    | right side
    } else /if (${_verbage.Equal[right]}) {
      /call do_raw_edit ${_debug} SILENT stCombatPosition "right"
    | left side
    } else /if (${_verbage.Equal[left]}) {
      /call do_raw_edit ${_debug} SILENT stCombatPosition "left"
    | random
    } else /if (${_verbage.Equal[random]}) {
      /call do_raw_edit ${_debug} SILENT stCombatPosition "random"
    | see the list
    } else /if (${_verbage.Equal[list]}) {
      OUT Random Options${sep}\a-wlsPosition\ax
      /call out_list lsPosition
      /return FALSE
    }

    /declare _liP listiterator local
    /vardata _liP lsCombatPosition.First.Clone
    /varset _out ${out_list} ${dot}
    /while (!${_liP.IsEnd}) {
      /varset _out ${_out} ${If[${_liP.Value.Equal[${lsCombatPosition.First}]},,${dot}]} ${If[${_liP.Value.Equal[${maHome.Find[stCombatPosition].Value}]},${num}${_liP.Value}\ax,${info}${_liP.Value}\ax]}
      /invoke ${_liP.Advance}
    }
    /invoke ${out[12, "/home position ${dot} ${_out}"]}
    /return TRUE

  } else /if (${_type.Equal[switch]}) {
    /invoke ${maHome.Add[swSetTie,FALSE]}
    /if (!${maHome.Find[swHomeMarker].Value}) {
      /invoke ${out[19, "Home${sep}Set ${dot} Y:${num}${Me.Y}\ax X:${num}${Me.X}\ax Z:${num}${Me.Z}\ax "]}
      /invoke ${maHome.Add[swHomeMarker,TRUE]}
      /invoke ${maData.Add[stCampX,${Me.X}]}
      /invoke ${maData.Add[stCampY,${Me.Y}]}
      /invoke ${maData.Add[stCampZ,${Me.Z}]}
      /invoke ${set_data_timer[FALSE, Check_Home, ${maHome.Find[stHomeVariance].Value}s, temp]}
      /return

    } else /if (${maHome.Find[swHomeMarker].Value}) {
      /if (${maHome.Find[swHomeMarker].Value}) {
        /invoke ${out[19, "Home${sep}Clear"]}
      }
      /invoke ${maHome.Add[swHomeMarker,FALSE]}
      /invoke ${maData.Add[stCampX,999999999]}
      /invoke ${maData.Add[stCampY,999999999]}
      /invoke ${maData.Add[stCampZ,999999999]}
      /invoke ${set_data_timer[FALSE, Check_Home, FALSE, delete]}
      /return
    }

  } else /if (${_type.Equal[set]}) {
    /invoke ${out[19, "Home${sep}Set ${dot} Y:${num}${Me.Y}\ax X:${num}${Me.X}\ax Z:${num}${Me.Z}\ax"]}
    /invoke ${maHome.Add[swSetTie,FALSE]}
    /invoke ${maHome.Add[swHomeMarker,TRUE]}
    /invoke ${maData.Add[stCampX,${Me.X}]}
    /invoke ${maData.Add[stCampY,${Me.Y}]}
    /invoke ${maData.Add[stCampZ,${Me.Z}]}
    /invoke ${set_data_timer[FALSE, Check_Home, ${maHome.Find[stHomeVariance].Value}s, temp]}
    /return

  } else /if (${_type.Equal[clear]}) {
    /if (${maHome.Find[swHomeMarker].Value}) {
      /invoke ${out[19, "Home${sep}Clear"]}
    }
    /invoke ${maHome.Add[swHomeMarker,FALSE]}
    /invoke ${maData.Add[stCampX,999999999]}
    /invoke ${maData.Add[stCampY,999999999]}
    /invoke ${maData.Add[stCampZ,999999999]}
    /invoke ${set_data_timer[FALSE, Check_Home, FALSE, delete]}
    /return

  } else /if (${_type.Equal[return]} && AUTO) {
    /if (${maHome.Find[stCampZ].Value} > 999999991) {
      /return FALSE
    } else /if (!${Navigation.PathExists[locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value}]}) {
      /invoke ${out[19, "${notice}${sep}no path home"]}
      /return FALSE
    }

    /declare _curVarDis float local ${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]}

    | check variance for home position
    /if (${_curVarDis} < ${maHome.Find[stHomeVariance].Value}) {
      DEBUG ${sep}within home${sep}\a-w${_curVarDis} < ${maHome.Find[stHomeVariance].Value}\ax
      /return TRUE
    }

    SETHUD "'goin home"

    /doevents flush
    /delay 2

    /if (${_curVarDis} > ${maHome.Find[stHomeVariance].Value} && ${_curVarDis} < SAFEENV && PULL) {
      DEBUG ${sep}move backwords${sep}\a-w${_curVarDis} > ${maHome.Find[stHomeVariance].Value}*2 && ${_curVarDis} < SAFEENV && PULL\ax
      /nav locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value} |dist=${maHome.Find[stHomeVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value} facing=backward
    } else {
      DEBUG ${sep}move normal
      /nav locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value} |dist=${maHome.Find[stHomeVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value}
    }

    /delay 2 ${Navigation.Active}
    /while (${Navigation.Active}) {
      CHECKDEAD
      /delay 2
    }
    SETHUD
    /return

  } else /if (${_type.Equal[arc]}) {
    /if (!${set_control_num_range[FALSE, stFaceArc, "${_verbage}", 0, 45]}) /return

  } else /if (${_type.Equal[variance]}) {
    /if (!${set_control_num_range[FALSE, stHomeVariance, "${_verbage}", 0, 15]}) /return

  } else /if (${_type.Equal[facefast]}) {
    /invoke ${set_switch_env[FALSE, swFaceFast, TRUE, ${_verbage}]}

 

  | when to clear home
  } else /if (${_type.Equal[stop]}) {

    /if (${_verbage.Equal[death]}) {
      /invoke ${set_switch_env[${_debug}, swMoveDeathClear, TRUE, ${_silent}]}

    } else /if (${_verbage.Equal[gather]}) {
      /invoke ${set_switch_env[${_debug}, swMoveGatherClear, TRUE, ${_silent}]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[death,swMoveDeathClear]}
    /invoke ${maControl.Add[gather,swMoveGatherClear]}
    /invoke ${set_control_output[${_debug}, maHome, "home stop", maControl]}
    /return FALSE

  } else /if (${_type.Equal[onauto]}) {
    /invoke ${set_switch_env[FALSE, swonAuto, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[incombat]}) {
    /invoke ${set_switch_env[FALSE, swinCombat, TRUE, ${_verbage}]}

  }

  /if (${_silent.Equal[SILENT]}) /return
  /if (${maHome.Find[swHomeMarker].Value}) {
    /invoke ${out[12, "Home${sep}Set ${dot} Y:${num}${maData.Find[stCampY].Value}\ax X:${num}${maData.Find[stCampX].Value}\ax Z:${num}${maData.Find[stCampZ].Value}\ax"]}
  } else /if (!${maHome.Find[swHomeMarker].Value}) {
    /invoke ${out[12, "Home${sep}Clear"]}
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[set,swHomeMarker|pr]}
  /invoke ${maControl.Add[variance,stHomeVariance]}
  /invoke ${maControl.Add[facefast,swFaceFast]}
  /invoke ${maControl.Add[arc,stFaceArc]}
  | /invoke ${maControl.Add[mount,stMount]}
  /invoke ${maControl.Add[stop,submenu]}
  /invoke ${maControl.Add[onauto,swonAuto]}
  /invoke ${maControl.Add[incombat,swinCombat]}
  /invoke ${maControl.Add[clear,option]}
  /invoke ${maControl.Add[campfire,option]}
  /invoke ${maControl.Add[position,submenu]}  
  /invoke ${set_control_output[${_debug}, maHome, home, maControl]}

/return TRUE



|***
 * note: control for /tc
 * use:
 ***|
#bind control_tc /tc
sub Bind_control_tc(string _type, string _verbage, string _silent, bool _debug)
  DEBUG \atBind_control_tc\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP tc ${_type.Right[1]}
    /return TRUE
  }

  /declare _lsout list local
  /declare _out string local


  /if (${_type.Equal[mode]}) {
    /invoke ${_lsout.Append[nav,stick]}
    /if (${_lsout.Contains[${_verbage}]}) {
      /if (!${Navigation.MeshLoaded}) {
        /invoke ${do_raw_edit[${_debug}, SILENT, stTieMode, stick]}
      } else /if (${Navigation.MeshLoaded}) {
        /invoke ${do_raw_edit[${_debug}, SILENT, stTieMode, "${_verbage}"]}
      }
    }

    /declare _litc listiterator local
    /vardata _litc _lsout.First.Clone
    /while (!${_litc.IsEnd}) {
      /varset _out ${_out} ${If[${_litc.Value.Equal[${_lsout.First}]},,${dot}]} ${If[${_litc.Value.Equal[${maTie.Find[stTieMode].Value}]},${good}${_litc.Value}\ax,${info}${_litc.Value}\ax]}
      /invoke ${_litc.Advance}
    }
    /invoke ${out[12, "/tc mode ${dot} ${_out}"]}
    /return

  } else /if (${_type.Equal[toon]}) {
    /invoke ${do_raw_edit[${_debug}, SILENT, stTieToon, "${_verbage}"]}

  | heal self
  } else /if (${_type.Equal[break]}) {
    /invoke ${set_switch_env[${_debug}, swTieBreak, TRUE, ${_verbage}]}


  } else /if (${_type.Equal[variance]}) {
    /if (!${set_control_num_range[${_debug}, stTieVariance, "${_verbage}", 0, 15]}) /return

  }

  /if (${_silent.Equal[SILENT]}) /return

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[toon,stTieToon|br]}
  /invoke ${maControl.Add[variance,stTieVariance]}
  /invoke ${maControl.Add[mode,submenu]}
  /invoke ${maControl.Add[break,swTieBreak]}
  /invoke ${set_control_output[${_debug}, maTie, tc, maControl]}

/return TRUE



|***
 * note: control for /tie
 * use: /tie [switch|on|off]
 ***|
#bind command_tie /tie
sub Bind_command_tie(string _type, bool _debug)
  DEBUG \atBind_command_tie\ax(\a-w${_type}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP tie ${_type.Right[1]}
    /return TRUE
  }

  | tie toon not in zone?
  /if (!${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}) {
    DEBUG ${sep}not in zone${sep}\a-w${maTie.Find[stTieToon].Value}\ax
    /invoke ${maTie.Add[swSetTie, FALSE]}
    /return FALSE
  }
  
  /if (HOME) {
    /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
  }  

  | set tie on and force to nav
  /if (${_type.Equal[nav]}) {
    DEBUG ${sep}setting tie${sep}${on} (\a-rnav\ax)
     /if (${maTie.Find[stTieMode].Value.NotEqual[nav]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stTieMode, "nav"]}
      OUT /tc mode \a-w${maTie.Find[stTieMode].Value}\ax
    }
    /if (${maTie.Find[swSetTie].Value}) /return
    /invoke ${maTie.Add[swSetTie, TRUE]}
  
  | set tie on and force to stick
  } else /if (${_type.Equal[stick]}) {
    DEBUG ${sep}setting tie${sep}${on} (\a-rstick\ax)
    /if (${maTie.Find[stTieMode].Value.NotEqual[stick]}) {
      /invoke ${do_raw_edit[${_debug}, SILENT, stTieMode, "stick"]}
      OUT /tc mode \a-w${maTie.Find[stTieMode].Value}\ax
    }
    /if (${maTie.Find[swSetTie].Value}) /return
    /invoke ${maTie.Add[swSetTie, TRUE]}
    
  } else /if (${_type.Equal[on]}) {
    DEBUG ${sep}setting tie${sep}${on}
    /if (${maTie.Find[swSetTie].Value}) /return
    | /if (${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}) 
    /invoke ${maTie.Add[swSetTie, TRUE]}
  } else /if (${_type.Equal[off]}) {
    DEBUG ${sep}setting tie${sep}${off}
    /if (!${maTie.Find[swSetTie].Value}) /return
    | /if (${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}) 
    /invoke ${maTie.Add[swSetTie, FALSE]}

  } else {
    DEBUG ${sep}setting tie${sep}[\a-yswap\ax]
    | /if (${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}) 
    /invoke ${maTie.Add[swSetTie,${If[${maTie.Find[swSetTie].Value},FALSE,TRUE]}]}
  }
  
  /if (!${maTie.Find[swSetTie].Value}) {
    /invoke ${maData.Add[swGetOverHere,FALSE]}
  }

  /invoke ${out[12, "/tie ${maTie.Find[stTieToon].Value}${sep}${If[${maTie.Find[swSetTie].Value},${on},${off}]}"]}

/return TRUE



|***
 * note: control for /area
 * use:
 ***|
#bind control_area /area
sub Bind_control_area(string _type, string _verbage, string _silent, bool _debug)

  /if (${maDebug.Find[area].Value}) {
    /varset _debug TRUE
  }
  DEBUG \atBind_control_area\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP area ${_type.Right[1]}
    /return TRUE
  }

  /if (${_type.Equal[list]}) {
    ECHO
    OUT ${num}${Zone.Name}\ax${sep}\a-w${Zone.ShortName}\ax
    /invoke ${out[12, "${sep}swZoneSafe${sep}\a-wSAFEZONE\ax"]}
    /declare _liwtf listiterator local
    /declare _liseez listiterator local
    /vardata _liseez lsZoneProperty.First.Clone
    /while (!${_liseez.IsEnd}) {
      OUT ${sep}${_liseez.Value}
        /vardata _liwtf ${_liseez}.First.Clone
        /while (!${_liwtf.IsEnd}) {
          OUT ${dot}${dot}${dot}${dot}\a-w${_liwtf.Value}\ax
          /invoke ${_liwtf.Advance}
        }
      /invoke ${_liseez.Advance}
    }
    /invoke ${out[18, 0, 33]}
    /return TRUE
  }


  | get the zone data
  | /sqlite query "${DBzone}" curZoneData SELECT * FROM zone WHERE ShortName="${Zone.ShortName.Lower}"
  | /if (!${sql_check[${_debug}, curZoneData, Bind_control_area, SELECT]}) /endmacro

  /declare _element string local FALSE
  /if (${_type.Equal[exclude]}) {
    /varset _element lsZoneExclude
  } else /if (${_type.Equal[named]}) {
    /varset _element lsZoneNamed
  } else /if (${_type.Equal[ranged]}) {
    /varset _element lsZoneRanged
  } else /if (${_type.Equal[charm]}) {
    /varset _element lsZoneCharm
  } else /if (${_type.Equal[hunt]}) {
    /varset _element lsZoneHunt
  } else /if (${_type.Equal[nocast]}) {
    /varset _element lsZoneNoCast
  } else /if (${_type.Equal[nomelee]}) {
    /varset _element lsZoneNoMelee
  } else /if (${_type.Equal[nomagic]}) {
    /varset _element lsZoneNoMagic
  } else /if (${_type.Equal[nodisease]}) {
    /varset _element lsZoneNoDisease
  } else /if (${_type.Equal[nofire]}) {
    /varset _element lsZoneNoFire
  } else /if (${_type.Equal[nopoison]}) {
    /varset _element lsZoneNoPoison
  } else /if (${_type.Equal[nocold]}) {
    /varset _element lsZoneNoCold
  } else /if (${_type.Equal[nocorruption]}) {
    /varset _element lsZoneNoCorruption
  } else /if (${_type.Equal[nopull]}) {
    /varset _element lsZoneNoPull


  | debuffs
  } else /if (${_type.Equal[nomez]}) {
    /varset _element lsZoneNoMez
  } else /if (${_type.Equal[nocripple]}) {
    /varset _element lsZoneNoCripple
  } else /if (${_type.Equal[noeradicate]}) {
    /varset _element lsZoneNoEradicate
  } else /if (${_type.Equal[nomalo]}) {
    /varset _element lsZoneNoMalo
  } else /if (${_type.Equal[noslow]}) {
    /varset _element lsZoneNoSlow
  } else /if (${_type.Equal[nosnare]}) {
    /varset _element lsZoneNoSnare
  } else /if (${_type.Equal[notash]}) {
    /varset _element lsZoneNoTash

  }

  DEBUG ${sep}_element${sep}\a-w${_element}\ax

  | check whats in the lists
  /if (${_type.NotEqual[rwp]}) {
    /if (!${Target.ID} && ${_verbage.Equal[list]}) {
      /call Bind_command_see ${_element}
      /return TRUE
    } else /if (${Target.ID} && ${_verbage.Equal[list]}) {
      /if (${${_element}.CountOf[${Target.DisplayName}]}) {
        OUT ${_type} list contains${sep}\a-w${Target.DisplayName}\ax
        /return TRUE
      } else {
        OUT ${_type} list does not contain${sep}\a-w${Target.DisplayName}\ax
        /return TRUE
      }
    }
  }

  | safezone toggle
  /if (${_type.Equal[safe]}) {
    DEBUG ${sep}safezone toggle


    /if (${maEnv.Find[swZoneSafe].Value} || ${_verbage.Equal[FALSE]}) {
      DEBUG FALSE
      DEBUG ${sep}sqlite query DBzone QZone UPDATE zone_control SET value='FALSE' WHERE zone_shortname='${Zone.ShortName}' AND element='swZoneSafe'
      /sqlite query "${DBzone}" QZone UPDATE zone_control SET value='FALSE' WHERE zone_shortname='${Zone.ShortName}' AND element='swZoneSafe'

      | /if (${Select[${sqlite.Resultcode[QZone]},19]}) {
      |   /sqlite query "${DBzone}" QZone REPLACE INTO zone_control(zone_shortname, element, value) VALUES("${Zone.ShortName}", "swZoneSafe", "FALSE");
      | }

      /if (!${sql_check[${_debug}, QZone, Bind_control_area, "UPDATE (zone_control:FALSE)"]}) /endmacro
      /invoke ${maEnv.Add[swZoneSafe,FALSE]}



    } else /if (!${maEnv.Find[swZoneSafe].Value} || ${_verbage.Equal[TRUE]}) {
      DEBUG TRUE
      DEBUG ${sep}sqlite query DBzone QZone UPDATE zone_control SET value='TRUE' WHERE zone_shortname='${Zone.ShortName}' AND element='swZoneSafe'
      /sqlite query "${DBzone}" QZone UPDATE zone_control SET value='TRUE' WHERE zone_shortname='${Zone.ShortName}' AND element='swZoneSafe'
      
      | /if (${Select[${sqlite.Resultcode[QZone]},19]}) {
      |   /sqlite query "${DBzone}" QZone INSERT OR REPLACE INTO zone_control(zone_shortname, element, value) VALUES("${Zone.ShortName}", "swZoneSafe", "TRUE");
      | }

      /if (!${sql_check[${_debug}, QZone, Bind_control_area, "UPDATE (zone_control:TRUE)"]}) /endmacro
      /invoke ${maEnv.Add[swZoneSafe,TRUE]}
    }





  | remove a mob from the current zone
  } else /if (${_type.Equal[delete]}) {
    /sqlite query "${DBzone}" QZone DELETE FROM zone_control WHERE zone_shortname='${Zone.ShortName}' AND value='${Target.DisplayName}'
    /if (!${sql_check[${_debug}, QZone, Bind_control_area, "DELETE FROM (zone_control)"]}) /endmacro
    /if (${lsGrain.Contains[12]}) {
      OUT /area delete ${dot} ${num}${Zone.ShortName}\ax${sep}\a-w${Target.DisplayName}\ax
    }

  | remaining list adjustments
  } else /if (${Select[${_type},exclude,named,charm,ranged,hunt,nomez,nocast,nomelee,nomagic,nodisease,nofire,nopoison,nocold,nocorruption,nosnare,nocripple,notash,nomalo,noeradicate,noslow,nopull]} && ${Target.ID}) {
    DEBUG ${sep}target${sep}\a-w${Target.DisplayName}\ax
    /if (!${${_element}.Contains[${Target.DisplayName}]}) {
      | Add a property to a mob
      /sqlite query "${DBzone}" QZone INSERT OR IGNORE INTO zone_control VALUES ("${Zone.ShortName}", "${_element}", "${Target.DisplayName}")
      /if (!${sql_check[${_debug}, QZone, Bind_control_area, "INSERT INTO (zone_control:insert)"]}) /endmacro
      /if (${lsGrain.Contains[12]}) {
        OUT /area ${_type} ${dot} ${num}${_element}\ax${sep}\a-w${Target.DisplayName}\ax
      }

    } else /if (${${_element}.Contains[${Target.DisplayName}]}) {
      OUT \aw${Target.DisplayName}\ax already exists in${sep}\a-w${_element}\ax
      /return TRUE
    }


  }

  | update others?
  /if (${Bool[${_type}]}) {
    DEBUG ${sep} sending zone updates to crew in zone
    /call set_zone_lists ${maDebug.Find[area].Value}
    /if (${lsGrain.Contains[1]}) {
      OUT ${num}${Zone.Name}\ax${sep}\a-w${Zone.ShortName}\ax
    }
    /dgzexecute /admin zonerefresh
    /return TRUE
  }


  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[safe,swZoneSafe]}
  /invoke ${maControl.Add[named,option]}
  /invoke ${maControl.Add[exclude,option]}
  /invoke ${maControl.Add[nopull,option]}
  /invoke ${maControl.Add[charm,option]}
  /invoke ${maControl.Add[ranged,option]}
  /invoke ${maControl.Add[nomez,option]}
  /invoke ${maControl.Add[nocast,option]}
  /invoke ${maControl.Add[nomelee,option]}
  /invoke ${maControl.Add[nofire,option]}
  /invoke ${maControl.Add[nopoison,option]}
  /invoke ${maControl.Add[nomagic,option]}
  /invoke ${maControl.Add[nocorruption,option]}
  /invoke ${maControl.Add[nocold,option]}
  /invoke ${maControl.Add[noslow,option]}
  /invoke ${maControl.Add[nodisease,option]}
  /invoke ${set_control_output[${_debug}, maEnv, area, maControl]}
  
/return TRUE



|***
 * note:/beg command
 * use: starts the buff begging process
 ***|
#Bind command_beg /beg
sub Bind_command_beg(string _buff, string _operate)

  /if (${_buff.Left[1].Equal[-]}) {
    /lua run HELP beg ${_buff.Right[1]}
    /return TRUE
  }

  /if (!${Bool[${_operate}]}) /varset _operate Add

  /dgzexecute /requestbuff ${Me.Name} Add "${_buff}"

/return TRUE



|***
 * note:sends the buff beg to all toons
 * use: this is not for player usage!
 ***|
#Bind command_requestbuff /requestbuff
sub Bind_command_requestbuff(string _toon, string _operate, string _buff)

  /if (!${Bool[${_toon}]}) {
    /varset _toon ${Me.Name}
  }

  /if (!${Bool[${_operate}]} || ${_operate.Equal[add]}) {
    /varset _operate Add
  }

  /invoke ${maBuffRequest.${_operate}[${_toon},"${_buff}"]}

/return TRUE



|***
 * note:control for /heal
 * use:
 ***|
#Bind control_heal /heal
sub Bind_control_heal(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_heal\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP heal ${_type.Right[1]}
    /return TRUE
  }
  
  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,50:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Heal 50 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  | set heal point
  /if (${_type.Equal[point]}) {
    /call set_control_heal_point ${_debug} ${_verbage} ${_verbage2}
    /return

  | heal self
  } else /if (${_type.Equal[self]}) {
    /invoke ${set_switch_env[${_debug}, swHealSelf, TRUE, ${_verbage}]}

  | twinheal attempts
  } else /if (${_type.Equal[twin]}) {
    /invoke ${set_switch_env[${_debug}, swTwinHeal, TRUE, ${_verbage}]}

  | heal targets target
  } else /if (${_type.Equal[tot]}) {
    /if (${lsClassHealToT.Contains[${Me.Class.ShortName}]}) {
      /invoke ${set_switch_env[${_debug}, swHealToT, TRUE, ${_verbage}]}
    } else {
      /invoke ${out[1, shade, 0]}
    }

  | heal xtarget
  } else /if (${_type.Equal[xt]}) {
    | will force xtarget list clear
    /if (${_verbage.Equal[clear]}) {
      /call set_xtarget_clear FALSE
      /return
    }

    | force a build of xtarget list for.. whatever the fuck you want
    /if (${_verbage.Equal[build]}) {
      /call check_xtarget_build FALSE FORCE
      /return
    }

    /invoke ${set_switch_env[${_debug}, swHealXTarget, TRUE, ${_verbage}]}

    /if (!${maHeal.Find[swHealXTarget].Value}) {
      /call set_xtarget_clear
    }

  | heal pets
  } else /if (${_type.Equal[pet]}) {
    /invoke ${set_switch_env[${_debug}, swHealPet, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[adj]}) {
    /if (!${set_control_num_range[${_debug}, stHealAdjust, "${_verbage}", 0, 99]}) /return


  | set the XTarget class to heal
  } else /if (${_type.Equal[xtclass]}) {
    /call set_control_shortname ${_debug} lsHealXTClass ${_verbage}
    /invoke ${out[12, "/heal xtclass ${dot} ${Macro.Return}"]}
    /return

  | set rampage tank toon
  } else /if (${_type.Equal[rampage]}) {
    /if (${Bool[${_verbage}]} && ${_verbage.NotEqual[TRUE]} && ${_verbage.NotEqual[ON]} && ${_verbage.NotEqual[OFF]}) {
      /call do_raw_edit ${_debug} SILENT stRampageTank ${proper_case[name, "${_verbage}"]}
    } else /if (!${Bool[${_verbage}]} || ${_verbage.Equal[OFF]}) {
      /invoke ${set_switch_env[${_debug}, stRampageTank, TRUE, FALSE]}
    }

  | use group Heal over Time
  } else /if (${_type.Equal[grouphot]}) {
    /invoke ${set_switch_env[${_debug}, swHealGroupOverTime, TRUE, ${_verbage}]}

  | use weighted HP healing
  } else /if (${_type.Equal[weight]}) {
    /invoke ${set_switch_env[${_debug}, swHealWeighted, TRUE, ${_verbage}]}

  | count of hurt to use a surge (shaman)
  } else /if (${_type.Equal[surge]} && ${Select[${Me.Class.ShortName},SHM]}) {
    /if (!${set_control_num_range[${_debug}, stCountSurge, "${_verbage}", 0, ${Math.Calc[${Me.XTargetSlots}+6]}]}) /return


  | use promise spells
  } else /if (${_type.Equal[promise]} && ${Select[${Me.Class.ShortName},CLR]}) {
    /invoke ${set_switch_env[${_debug}, swHealPromise, TRUE, ${_verbage}]}

  | use complete heals
  } else /if (${_type.Equal[ch]} && ${Select[${Me.Class.ShortName},CLR]}) {

    /if (${_verbage.Equal[pct]}) {
      /call do_raw_edit ${_debug} SILENT stPctCH "${_verbage2}"
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[pct,stPctCH]}
    /invoke ${set_control_output[${_debug}, maHeal, "heal ch", maControl]}
    /return TRUE

  | use splashes
  } else /if (${_type.Equal[splash]} && ${Select[${Me.Class.ShortName},CLR,PAL]}) {
    /if (!${set_control_num_range[${_debug}, stCountSplash, "${_verbage}", 0, 6]}) /return

  | heal group
  } else /if (${_type.Equal[group]}) {
    /invoke ${set_switch_env[${_debug}, swHealGroup, TRUE, ${_verbage}]}

  | set group heals count
  } else /if (${_type.Equal[groupcount]}) {
    /if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return

  | enable break heal on %
  } else /if (${_type.Equal[break]}) {
    /invoke ${set_switch_env[${_debug}, swBreakHealPCT, TRUE, ${_verbage}]}

  | heal with dannet
  } else /if (${_type.Equal[dannet]}) {
    /invoke ${set_switch_env[${_debug}, swHealDanNet, TRUE, ${_verbage}]}

  | set single hot
  } else /if (${_type.Equal[hot]}) {
    /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swHealOverTime, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[pct]}) {
      /if (!${set_control_num_range[${_debug}, stHealPointHoT, "${_verbage2}", 0, 99]}) /return
    }
    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swHealOverTime]}
    /invoke ${maControl.Add[pct,stHealPointHoT]}
    /invoke ${set_control_output[${_debug}, maHeal, "heal hot", maControl]}
    /return TRUE

  | chain CH
  } else /if (${_type.Equal[chain]}) {

    /if (!${Select[${Me.Class.ShortName},CLR]}) {
      OUT You are not a cleric. gtfo
      /return FALSE
    }

    /if (${_verbage.Equal[list]}) {
      OUT Tank Lineup${sep}\a-wlsChain\ax
      /call out_list lsChain
      /return TRUE
    }

    | listen for CH Chain rotation calls
    /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swChain, TRUE, ${_verbage2}]}

    | % to start the chain cast
    } else /if (${_verbage.Equal[pct]}) {
      /if (!${set_control_num_range[${_debug}, stPctChain, "${_verbage2}", 0, 99]}) /return

    | % to start the chain cast
    } else /if (${_verbage.Equal[pause]}) {
      /if (!${set_control_num_range[${_debug}, stChainPause, "${_verbage2}", 0, 15]}) /return

    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[list,list]}
    /invoke ${maControl.Add[pct,stPctChain]}
    /invoke ${maControl.Add[active,swChain]}
    /invoke ${maControl.Add[pause,stChainPause]}
    /invoke ${set_control_output[${_debug}, maHeal, "heal chain", maControl]}
    /return TRUE
       
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[point,submenu]}
  /invoke ${maControl.Add[self,swHealSelf]}
  /if (${lsClassHealToT.Contains[${Me.Class.ShortName}]}) {
    /invoke ${maControl.Add[tot,swHealToT]}
  }

  /invoke ${maControl.Add[break,swBreakHealPCT]}
  /invoke ${maControl.Add[dannet,swHealDanNet]}
  
  /if (${Select[${Me.Class.ShortName},SHM,CLR]}) {
    /invoke ${maControl.Add[grouphot,swHealGroupOverTime]}
  }
  
  /invoke ${maControl.Add[group,swHealGroup]}
  /invoke ${maControl.Add[groupcount,stCountHealGroup]}
  /invoke ${maControl.Add[xt,swHealXTarget]}
  /invoke ${maControl.Add[xtclass,submenu]}

  /invoke ${maControl.Add[pet,swHealPet]}
  /invoke ${maControl.Add[adj,stHealAdjust]}
  /invoke ${maControl.Add[weight,swHealWeighted]}

  /if (${Select[${Me.Class.ShortName},SHM]}) {
    /invoke ${maControl.Add[surge,stCountSurge]}
  }

  /if (${Select[${Me.Class.ShortName},CLR]}) {
    /invoke ${maControl.Add[hot,submenu]}
    /invoke ${maControl.Add[ch,submenu]}
    /invoke ${maControl.Add[chain,submenu]}
    /invoke ${maControl.Add[promise,swHealPromise]}
  }

  /if (${Select[${Me.Class.ShortName},CLR,PAL]}) {
    /invoke ${maControl.Add[splash,stCountSplash]}
    /invoke ${maControl.Add[twin,swTwinHeal]}
  }

  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[rampage,stRampageTank|br]}
  /invoke ${set_control_output[${_debug}, maHeal, heal, maControl]}

/return TRUE



|***
 * note: control for /entropy
 * use:
 ***|
#Bind control_entropy /entropy
sub Bind_control_entropy(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
  DEBUG \atcontrol_entropy\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP entropy ${_type.Right[1]}
    /return TRUE
  }

  /if (${_type.Equal[maintenance]}) {
    /invoke ${set_switch_env[FALSE, swMaintenance, TRUE, ${_verbage}]}
    /invoke ${out[12, "/entropy ${If[${maEntropy.Find[swMaintenance].Value},${good}maintenance\ax,${bad}maintenance\ax]}"]}
    /invoke ${out[12, "${warning} ${dot} ENDING MACRO${dot}${dot}${dot}please restart"]}
    /endmacro

  | use concolors for targets
  } else /if (${_type.Equal[concolor]}) {
    /invoke ${set_switch_env[${_debug}, swUseConColor, TRUE, ${_verbage}]}

  | enable/disable using events.inc file
  } else /if (${_type.Equal[events]}) {
    /invoke ${set_switch_env[${_debug}, swEventsInc, TRUE, ${_verbage}]}

  | autoinvnetory while in manual
  } else /if (${_type.Equal[manautoinv]}) {
    /invoke ${set_switch_env[${_debug}, swAutoInvManual, TRUE, ${_verbage}]}

  | enable buff cooldown waits
  } else /if (${_type.Equal[buffcooldownwait]}) {
    /invoke ${set_switch_env[${_debug}, swBuffCooldownWait, TRUE, ${_verbage}]}

  | enable sub hook checking
  } else /if (${_type.Equal[hook]}) {
    /invoke ${set_switch_env[${_debug}, swHookSub, TRUE, ${_verbage}]}

    /if (!${maHook.Find[swHookSub].Value}) {
      /invoke ${maHook.Clear}
    }

  | number of fizzle retrys
  } else /if (${_type.Equal[fizzles]}) {
    /if (!${set_control_num_range[${_debug}, stCountFizzleRetry, "${_verbage}", 0, 999]}) /return


  } else /if (${_type.Equal[loopdelay]}) {
    /if (!${set_control_num_range[${_debug}, stMainLoopDelay, "${_verbage}", 0, 9]}) /return

   } else /if (${_type.Equal[buffrefresh]}) {
    /if (!${set_control_num_range[${_debug}, stBuffRefresh, "${_verbage}", 0, 36]}) /return

  | verbosity level
  } else /if (${_type.Equal[verb]}) {
    /if (!${set_control_num_range[${_debug}, stVerb, "${_verbage}", 1, 10]}) /return

  | remember the set build at mac start
  } else /if (${_type.Equal[rememberset]}) {
    /invoke ${set_switch_env[${_debug}, swBuildSetRemember, TRUE, ${_verbage}]}

  | enable auto adding mobs to lists
  } else /if (${_type.Equal[autolist]}) {
    /invoke ${set_switch_env[${_debug}, swAutoList, TRUE, ${_verbage}]}

  | count of cast loop attempts
  } else /if (${_type.Equal[castloops]}) {
    /invoke ${set_switch_env[${_debug}, stCastLoop, TRUE, ${_verbage}]}

  | code engine
  } else /if (${_type.Equal[engine]}) {
    /if (!${set_control_num_range[${_debug}, stEngine, "${_verbage}", 1, 3]}) /return

  | relay tells
  } else /if (${_type.Equal[relaytell]}) {
    /invoke ${set_switch_env[${_debug}, swRelayTell, TRUE, ${_verbage}]}

  | tlp player
  } else /if (${_type.Equal[tlp]}) {
    /invoke ${set_switch_env[${_debug}, swTLP, TRUE, ${_verbage}]}


  | hud adjustments
  } else /if (${_type.Equal[hud]}) {

    /if (${_verbage.Equal[start]}) {
      /lua run ${MacroQuest.Path[macros]}\entropy\lua\hud.lua 
      
    } else /if (${_verbage.Equal[stop]}) {   
      /lua stop hud.lua
      
    } else /if (${_verbage.Equal[auto]}) {   
      /invoke ${set_switch_env[${_debug}, swHUDAuto, TRUE, ${_verbage2}]}

    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[auto,swHUDAuto]}
    /invoke ${maControl.Add[start,option]}
    /invoke ${maControl.Add[stop,option]}
    /invoke ${set_control_output[${_debug}, maEntropy, "entropy hud", maControl]}
    /return TRUE
    
     
  | randomization switches
  } else /if (${_type.Equal[random]}) {

    | character engage
    /if (${_verbage.Equal[engage]}) {
      /if (!${set_control_num_range[${_debug}, stRNDEngage, "${_verbage2}", 0, 100]}) /return

    | pet engage
    } else /if (${_verbage.Equal[pet]}) {
      /if (!${set_control_num_range[${_debug}, stRNDEngagePet, "${_verbage2}", 0, 100]}) /return

    | swarm engage
    } else /if (${_verbage.Equal[swarm]}) {
      /if (!${set_control_num_range[${_debug}, stRNDEngageSwarm, "${_verbage2}", 0, 100]}) /return

    | burn engage
    } else /if (${_verbage.Equal[burn]}) {
      /if (!${set_control_num_range[${_debug}, stRNDEngageBurn, "${_verbage2}", 0, 100]}) /return

    | burn engage
    } else /if (${_verbage.Equal[rez]}) {
      /if (!${set_control_num_range[${_debug}, stRNDRezTake, "${_verbage2}", 0, 100]}) /return

    | repeat delay
    } else /if (${_verbage.Equal[repeat]}) {
      /if (!${set_control_num_range[${_debug}, stRNDRepeat, "${_verbage2}", 0, 100]}) /return

    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[engage,stRNDEngage]}
    /invoke ${maControl.Add[pet,stRNDEngagePet]}
    /invoke ${maControl.Add[swarm,stRNDEngageSwarm]}
    /invoke ${maControl.Add[burn,stRNDEngageBurn]}
    /invoke ${maControl.Add[rez,stRNDRezTake]}
    /invoke ${maControl.Add[repeat,stRNDRepeat]}
    /invoke ${set_control_output[${_debug}, maEntropy, "entropy random", maControl]}
    /return TRUE    
    

  | dannet stuff
  } else /if (${_type.Equal[dannet]}) {

    /if (${_verbage.Equal[nettimeout]}) {
      /if (!${set_control_num_range[${_debug}, stNetworkTimeout, "${_verbage2}", 0, 50]}) /return
      /dnet timeout ${_verbage2}

    } else /if (${_verbage.Equal[querydelay]}) {
      /if (!${set_control_num_range[${_debug}, stDanNetQueryDelay, "${_verbage2}", 0, 50]}) /return

    } else /if (${_verbage.Equal[evasive]}) {
      /if (!${set_control_num_range[${_debug}, stNetworkEvasive, "${_verbage2}", 1000, 30000]}) /return
      /dnet evasive ${_verbage2}

    } else /if (${_verbage.Equal[evasiverefresh]}) {
      /invoke ${set_switch_env[${_debug}, swEvasiveRefresh, TRUE, ${_verbage2}]}
      /dnet evasiverefresh ${_verbage2}

    } else /if (${_verbage.Equal[expired]}) {
      /if (!${set_control_num_range[${_debug}, stNetworkExpired, "${_verbage2}", 15000, 60000]}) /return
      /dnet expired ${_verbage2}

    } else /if (${_verbage.Equal[frontdelim]}) {
      /invoke ${set_switch_env[${_debug}, swFrontDelim, TRUE, ${_verbage2}]}
      /dnet frontdelim ${_verbage2}

    } else /if (${_verbage.Equal[commandecho]}) {
      /invoke ${set_switch_env[${_debug}, swCommandEcho, TRUE, ${_verbage2}]}
      /dnet commandecho ${_verbage2}

    } else /if (${_verbage.Equal[localecho]}) {
      /invoke ${set_switch_env[${_debug}, swLocalEcho, TRUE, ${_verbage2}]}
      /dnet localecho ${_verbage2}

    } else /if (${_verbage.Equal[fullname]}) {
      /invoke ${set_switch_env[${_debug}, swFullNames, TRUE, ${_verbage2}]}
      /dnet fullnames ${_verbage2}

    } else /if (${_verbage.Equal[maingroup]}) {
      /call do_raw_edit ${_debug} SILENT stEntropyGroup_all "${_verbage2}"
      /djoin ${_verbage2}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[frontdelim,swFrontDelim]}
    /invoke ${maControl.Add[commandecho,swCommandEcho]}
    /invoke ${maControl.Add[localecho,swLocalEcho]}
    /invoke ${maControl.Add[fullname,swFullNames]}
    /invoke ${maControl.Add[maingroup,stEntropyGroup_all|br]}
    /invoke ${maControl.Add[nettimeout,stNetworkTimeout]}
    /invoke ${maControl.Add[evasive,stNetworkEvasive]}
    /invoke ${maControl.Add[evasiverefresh,swEvasiveRefresh]}
    /invoke ${maControl.Add[expired,stNetworkExpired]}
    /invoke ${maControl.Add[querydelay,stDanNetQueryDelay]}
    /invoke ${set_control_output[${_debug}, maEntropy, "entropy dannet", maControl]}
    /return TRUE

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[autolist,swAutoList]}
  /invoke ${maControl.Add[fizzles,stCountFizzleRetry]}
  /invoke ${maControl.Add[concolor,swUseConColor]}
  /invoke ${maControl.Add[random,submenu]}
  /invoke ${maControl.Add[engine,stEngine]}
  /invoke ${maControl.Add[castloops,stCastLoop]}
  /invoke ${maControl.Add[events,swEventsInc]}
  /invoke ${maControl.Add[tlp,swTLP]}
  /invoke ${maControl.Add[hook,swHookSub]}
  /invoke ${maControl.Add[hud,submenu]}
  /invoke ${maControl.Add[dannet,submenu]}
  /invoke ${maControl.Add[loopdelay,stMainLoopDelay]}
  /invoke ${maControl.Add[manautoinv,swAutoInvManual]}
  /invoke ${maControl.Add[maintenance,swMaintenance]}
  /invoke ${maControl.Add[rememberset,swBuildSetRemember]}
  /invoke ${maControl.Add[buffcooldownwait,swBuffCooldownWait]}
  /invoke ${maControl.Add[relaytell,swRelayTell]}

  /invoke ${set_control_output[${_debug}, maEntropy, entropy, maControl]}

/return TRUE



|***
 * note: control for rez
 * use:
 ***|
#Bind control_rez /rez
sub Bind_control_rez(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_rez\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP rez ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]} && !${Select[${_verbage},see,tag]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Rez 8 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}

    /if (${Select[${_type},list]}) {
      /call Bind_command_see lsRezIC
      /call Bind_command_see lsRezOOC
    }
    /return TRUE
  }

  | take rez
  /if (${_type.Equal[take]}) {
    /invoke ${set_switch_env[${_debug}, swRezTake, TRUE, ${_verbage}]}

  | rez in combat
  } else /if (${_type.Equal[ic]}) {
    /invoke ${set_switch_env[${_debug}, swRezIC, TRUE, ${_verbage}]}

  | rez OOC
  } else /if (${_type.Equal[ooc]}) {
    /invoke ${set_switch_env[${_debug}, swRezOOC, TRUE, ${_verbage}]}

  | rez everyone
  } else /if (${_type.Equal[dannet]}) {
    /invoke ${set_switch_env[${_debug}, swRezDanNet, TRUE, ${_verbage}]}

  | take calls
  } else /if (${_type.Equal[takecall]}) {
    /invoke ${set_switch_env[${_debug}, swRezTakeCall, TRUE, ${_verbage}]}

  | rez everyone
  } else /if (${_type.Equal[everyone]}) {
    /invoke ${set_switch_env[${_debug}, swRezEveryone, TRUE, ${_verbage}]}

  | use rez tokens
  } else /if (${_type.Equal[token]}) {
    /invoke ${set_switch_env[${_debug}, swRezToken, TRUE, ${_verbage}]}

  | rez radius
  } else /if (${_type.Equal[rad]}) {
    /if (!${set_control_num_range[${_debug}, stMaxRezRange, "${_verbage}", 0, 200]}) /return

  | use divine rez
  } else /if (${_type.Equal[divine]} && ${Select[${Me.Class.ShortName},CLR]}) {
    /invoke ${set_switch_env[${_debug}, swDivineCleric, TRUE, ${_verbage}]}


  | set rez accept percent
  } else /if (${_type.Equal[pct]}) {
    /if (${Range.Between[10,96:${Int[${_verbage}]}]}) {
      /if (${lsRezPct.Contains[${_verbage}]}) /call do_raw_edit ${_debug} SILENT stPctMinRez "${_verbage}"
    }

    /declare _out string local
    /declare _lirezpct listiterator local
    /vardata _lirezpct lsRezPct.First.Clone
    /while (!${_lirezpct.IsEnd}) {
      /varset _out ${_out} ${If[${_lirezpct.Value.Equal[${lsRezPct.First}]},,${dot}]} ${If[${maRez.Find[stPctMinRez].Value} == ${_lirezpct.Value},${good}${_lirezpct.Value}\ax,${info}${_lirezpct.Value}\ax]}
      /invoke ${_lirezpct.Advance}
    }
    /invoke ${out[12, "/rez pct ${dot} ${_out}"]}
    /return TRUE

  }

  /if (${_verbage2.Equal[SILENT]}) /return
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[pct,stPctMinRez]}
  /invoke ${maControl.Add[rad,stMaxRezRange]}
  /invoke ${maControl.Add[token,swRezToken]}
  /invoke ${maControl.Add[takecall,swRezTakeCall]}
  /invoke ${maControl.Add[everyone,swRezEveryone]}
  /invoke ${maControl.Add[take,swRezTake]}
  /invoke ${maControl.Add[ic,swRezIC]}
  /invoke ${maControl.Add[ooc,swRezOOC]}
  /invoke ${maControl.Add[dannet,swRezDanNet]}
  /if (${Select[${Me.Class.ShortName},CLR]}) {
    /invoke ${maControl.Add[divine,swDivineCleric]}
  }
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maRez, rez, maControl, maControl]}

/return TRUE



|***
 * note: your mama so fat, each ass cheek has a different zip code
 * use: /buff
 ***|
#Bind control_buff /buff
sub Bind_control_buff(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atBind_control_buff\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP buff ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list]} || ${Range.Between[1,50:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Buff 50 "${_type}" "${_verbage}" "${_verbage2}" "${_add_delete}"
    /return TRUE
  }

  | master buff switch
  /if (${_type.Equal[master]}) {
    /invoke ${set_switch_env[${_debug}, swBuffMaster, FALSE, ${_verbage}]}

  } else /if (${_type.Equal[ps]}) {
    | /invoke ${set_switch_env[${_debug}, swBuffPowersource, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[self]}) {
    /invoke ${set_switch_env[${_debug}, swBuffSelf, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[crew]}) {
    /invoke ${set_switch_env[${_debug}, swBuffCrew, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[now]}) {
    /invoke ${set_switch_env[${_debug}, swBuffNow, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[raid]}) {
    /invoke ${set_switch_env[${_debug}, swBuffRaid, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[group]}) {
    /invoke ${set_switch_env[${_debug}, swBuffGroup, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[ammo]}) {
    /invoke ${set_switch_env[${_debug}, swBuffAmmo, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[beg]}) {

    | output control map
    /if (${_verbage.Equal[see]}) {
      /call Bind_command_see maBuffRequest ${_silent}
      /return TRUE
    }

    /invoke ${set_switch_env[${_debug}, swBuffBeg, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[unity]}) {
    /invoke ${set_switch_env[${_debug}, swBuffUnity, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[aura]}) {
    /invoke ${set_switch_env[${_debug}, swBuffAura, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[temp]}) {
    /invoke ${set_switch_env[${_debug}, swBuffTemp, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[drop]}) {
    /invoke ${set_switch_env[${_debug}, swDropBuffs, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[shrink]}) {
    /if (${_verbage.Equal[item]}) {
      /call do_raw_edit ${_debug} SILENT stShrinkItem "${_verbage2}"
    } else /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swBuffShrink, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[height]}) {
      /call do_raw_edit ${_debug} SILENT stShrinkHeight "${_verbage2}"      
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swBuffShrink]}
    /invoke ${maControl.Add[height,stShrinkHeight]}
    /invoke ${maControl.Add[item,stShrinkItem|br]}
    /invoke ${set_control_output[${_debug}, maBuff, "buff shrink", maControl]}
    /return

  | TLP keep the mount and the buff
  } else /if (${_type.Equal[keepmount]}) {
    /invoke ${set_switch_env[${_debug}, swBuffKeepMount, TRUE, ${_verbage}]}


  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[ammo,swBuffAmmo]}
  /invoke ${maControl.Add[aura,swBuffAura]}
  /invoke ${maControl.Add[beg,swBuffBeg]}
  /invoke ${maControl.Add[drop,swDropBuffs]}
  /invoke ${maControl.Add[crew,swBuffCrew]}
  /invoke ${maControl.Add[group,swBuffGroup]}
  /invoke ${maControl.Add[keepmount,swBuffKeepMount]}
  /invoke ${maControl.Add[master,swBuffMaster|pr]}
  /invoke ${maControl.Add[now,swBuffNow]}
  | /invoke ${maControl.Add[ps,swBuffPowersource]}
  /invoke ${maControl.Add[raid,swBuffRaid]}
  /invoke ${maControl.Add[self,swBuffSelf]}
  /invoke ${maControl.Add[shrink,submenu]}
  /invoke ${maControl.Add[temp,swBuffTemp]}
  /invoke ${maControl.Add[unity,swBuffUnity]}
  /invoke ${maControl.Add[list,list]}
  

  /invoke ${set_control_output[${_debug}, maBuff, buff, maControl]}

/return TRUE



|***
 * note: control for overrides
 * use:
 ***|
#Bind control_over /override
sub Bind_control_over(string _type, string _verbage, string _silent, bool _debug)
  DEBUG \atBind_control_over\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP override ${_type.Right[1]}
    /return TRUE
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[los,swOverLOS]}
  /invoke ${maControl.Add[engage,swOverPctEngage]}
  /invoke ${maControl.Add[loot,swOverLoot]}
  /invoke ${maControl.Add[safenames,swOverSafeNames]}
  /invoke ${maControl.Add[trainspell,swOverTrainSpell]}
  /invoke ${maControl.Add[dscheck,swOverDSCheck]}
  /invoke ${maControl.Add[envauto,swOverEnvAuto]}
  /invoke ${maControl.Add[navcheck,swOverNavCheck]}
  /invoke ${maControl.Add[groupass,swOverGroupAss]}
  /invoke ${maControl.Add[splashhurt,swOverSplashHurt]}
  /invoke ${maControl.Add[targetclear,swTargetClear]}
  /invoke ${maControl.Add[tmpengage,option]}

  /if (${_type.Equal[startcheck]}) {
    /declare _out string local
    /declare _count int local 0
    /declare _mios mapiterator local
    /vardata _mios maControl.First.Clone
    /while (!${_mios.IsEnd}) {
      /if (${maOver.Find[${_mios.Value}].Value}) {
        /varset _out ${_out} ${If[!${_count},,${dot}]} ${_mios.Key}
        /varcalc _count ${_count} + 1
      }
      /invoke ${_mios.Advance}
    }  
    /if (${Bool[${_out}]}) {
      OUT \a-tOverrides Enabled\ax${sep}\a-w${_out}\ax
    }
    /return TRUE

  } else /if (${_type.Equal[los]}) {
    /invoke ${set_switch_env[${_debug}, swOverLOS, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[engage]}) {
    /invoke ${set_switch_env[${_debug}, swOverPctEngage, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[loot]}) {
    /invoke ${set_switch_env[${_debug}, swOverLoot, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[safenames]}) {
    /invoke ${set_switch_env[${_debug}, swOverSafeNames, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[trainspell]}) {
    /invoke ${set_switch_env[${_debug}, swOverTrainSpell, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[envauto]}) {
    /invoke ${set_switch_env[${_debug}, swOverEnvAuto, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[dscheck]}) {
    /invoke ${set_switch_env[${_debug}, swOverDSCheck, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[navcheck]}) {
    /invoke ${set_switch_env[${_debug}, swOverNavCheck, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[groupass]}) {
    /invoke ${set_switch_env[${_debug}, swOverGroupAss, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[splashhurt]}) {
    /invoke ${set_switch_env[${_debug}, swOverSplashHurt, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[targetclear]}) {
    /invoke ${set_switch_env[${_debug}, swTargetClear, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[tmpengage]}) {
    /invoke ${set_switch_env[${_debug}, swOverPctEngage, FALSE, TRUE]}
    OUT /override engage \agENABLED\ax . Disabling in 60 seconds!
    /timed 600 /invoke ${maOver.Add[swOverPctEngage,FALSE]}
    /return TRUE
  }

  /if (${_silent.Equal[SILENT]}) /return
  /invoke ${set_control_output[${_debug}, maOver, override, maControl]}

/return TRUE



|***
 * note: shorthand for raw db editing
 * use: mostly for breaking shit
 ***|
#bind_noparse raw_edit /edit
sub Bind_noparse_raw_edit(string _variable, string _value, string _add_delete, string _verbage)

  /if (${_variable.Left[1].Equal[-]}) {
    /lua run HELP edit ${_variable.Right[1]}
    /return TRUE
  }

  /call do_raw_edit ${maDebug.Find[edit].Value} FALSE "${_variable}" "${Parse[1,${_value}]}" ${_add_delete} ${_verbage}

/return TRUE



|***
 * note: shorthand for raw db editing from lua
 * use: still mostly for breaking shit
 ***|
#bind_noparse lua_raw_edit /luaedit
sub Bind_noparse_lua_raw_edit(string _variable, string _value, string _add_delete, string _verbage)
  /call do_raw_edit FALSE SILENT "${_variable}" "${Parse[1,${_value}]}" "${_add_delete}"

/return TRUE



|***
 * note: lets see what we can see
 * use: /see [a thing]
 ***|
#bind command_see /see
sub Bind_command_see(string _variable, string _verbage, string _silent)
  /declare _debug bool local FALSE

  DEBUG \atBind_control_see\ax(\a-w${_variable}, ${_verbage}, ${_silent}\ax)
  DEBUG .Left[2]${sep}\a-w${_variable.Left[2]}\ax

  /if (${_variable.Left[1].Equal[-]}) {
    /lua run HELP see ${_variable.Right[1]}
    /return TRUE
  }

  /if (!${Defined[${_variable}]} && !${Select[${_variable},${lsControls},spawns,target]}) {
    /invoke ${out[0, "Crack says your input sucks${sep}\a-w${_variable}\ax"]}
    /return FALSE
  }

  /declare _count int local 0
  /declare _DBLOC string local
  /declare _tmpAux string local

  | shows detailed target info
  /if (${_variable.Equal[target]}) {
    /if (!${Target.ID}) {
      /invoke ${out[0, shade, 0]}
      /return
    }
    
    /declare _spawn spawn local 
    /vardata _spawn Target.ID

    ECHO
    OUT Target Info${sep}
    OUT name:${con_color[${_debug}, ${_spawn.ID}]}${sep}\a-w${_spawn.Name}\ax${sep}\ay${_spawn.ID}\ax
    OUT lvl${sep}\a-w${_spawn.Level}\ax
    OUT named${sep}\a-w\ax

    /sqlite query "${DBstatic}" dummy SELECT value FROM animation WHERE env_var='${_spawn.Animation}'
    /if (!${sql_check[${_debug}, dummy, Bind_command_see, SELECT]}) /endmacro
    /declare _animationresult string local UNKNOWN ANIMATION
    /if (${sqlite.Rows[dummy]}) {
      /varset _animationresult ${sqlite.Result[dummy 1 value]}
    }
    OUT animation${sep}\a-w${_spawn.Animation}\ax${sep}${_animationresult}
    OUT body${sep}\a-w${_spawn.Body}\ax

    OUT dis${sep}\a-w${_spawn.Distance}\ax
    OUT LOC yxz Y${sep}\a-w${_spawn.Y}\ax X${sep}\a-w${_spawn.X}\ax Z${sep}\a-w${_spawn.Z}\ax
    OUT NAV Path${sep}\a-w${Navigation.PathExists[id ${_spawn.ID}]}\ax
    OUT NAV Length${sep}\a-w${Navigation.PathLength[id ${_spawn.ID}]}\ax

    /invoke ${out[18, 0, 33]}
    /return FALSE
    
  } else /if (${_variable.Equal[spawns]}) {
    MQCLEAR
    OUT Nearest \aw5\ax spawns
    /declare _spawn spawn
    /declare _animationresult string local UNKNOWN ANIMATION
    
    /if (!${Bool[${_verbage}]}) {
      /varset _verbage 5      
    }

    /for _count 1 to ${_verbage}
      /vardata _spawn NearestSpawn[${_count}]
      OUT #\ag${_count}\ax${sep}name:\a-w${_spawn.Name}\ax${sep}${con_color[${_debug}, ${_spawn}]}
      OUT ${sep}Dis(\a-r${_spawn.Distance}\ax) ${dot} Y(\a-r${_spawn.Y}\ax)X(\a-r${_spawn.X}\ax)

      /sqlite query "${DBstatic}" dummy SELECT value FROM animation WHERE env_var='${_spawn.Animation}'
      /if (!${sql_check[${_debug}, dummy, control_debug, "SELECT (value)"]}) /endmacro
      /varset _animationresult string local UNKNOWN ANIMATION
      /if (${sqlite.Rows[dummy]}) {
        /varset _animationresult ${sqlite.Result[dummy 1 value]}
      }
      OUT ${sep}Animation(\a-r${_spawn.Animation}\ax${sep}\a-w${_animationresult}\ax) ${dot} Body(\a-r${_spawn.Body}\ax) ${dot} ID(\a-r${_spawn.ID}\ax) ${dot} Type(\a-r${_spawn.Type}\ax)
      OUT ${sep}Master(\a-r${_spawn.Master}\ax) ${dot} Owner(\a-r${_spawn.Owner}\ax)
      ECHO
    /next _count
    /invoke ${out[18, 0, 33]}
    /return FALSE

  | see what we are observing
  } else /if (${_variable.Equal[observed]}) {
    /for _count 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}
      /if (!${Defined[lsObserved_${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}]}) /continue
      /invoke ${out[12, "Observed on${sep}\at${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|].Upper}\ax"]}
      /declare _liob listiterator local
      /vardata _liob lsObserved_${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}.First
      /while (!${_liob.IsEnd}) {
        /invoke ${out[12, "${sep}\aw${_liob.Value}\ax"]}
        /invoke ${_liob.Advance}
      }
    /next _count

  | wtf.. list...
  } else /if (${_variable.Left[2].Equal[ls]} && ${${_variable}.Count}) {
    /invoke ${out[12, "\aw${_variable}\ax ${dot} \a-g${${_variable}.Count}\ax ${If[${${_variable}} > 1 || ${${_variable}} == 0,items,item]}"]}
    /declare _liwtf listiterator local
    /vardata _liwtf ${_variable}.First.Clone
    /while (!${_liwtf.IsEnd}) {
      /invoke ${out[12, "${dot}\a-w${_liwtf.Value}\ax"]}
      /invoke ${_liwtf.Advance}
    }

  | map
  } else /if (${_variable.Left[2].Equal[ma]}) {
    /invoke ${out[12, "\aw${_variable}\ax ${dot} \a-g${${_variable}.Count}\ax ${If[${${_variable}} > 1 || ${${_variable}} == 0,items,item]}"]}
    /if (!${${_variable}.Count}) /return
    /declare _misee mapiterator local
    /vardata _misee ${_variable}.First.Clone
    /while (!${_misee.IsEnd}) {
      /if (${_verbage.Equal[-h]}) {
        /varset _DBLOC ${get_key_DBLOC[${_debug}, ${_misee.Key}]}
        /sqlite query "${DBcharacter}" dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_misee.Key}'
        /if (${sqlite.Status[dummy].Equal[SUCCESS]} && ${sqlite.Result[dummy 1 aux].Length} > 6) {
          /varset _tmpAux ${dot} \a-g${sqlite.Result[dummy 1 aux]}\ax
        } else {
          /varset _tmpAux
        }
      }
      OUT ${_misee.Key}${sep}\aw${Parse[1,${_misee.Value}]}\ax ${If[${_verbage.Equal[-h]},${_tmpAux},]}
      /invoke ${_misee.Advance}
    }

  | set
  } else /if (${_variable.Left[2].Equal[se]} && ${${_variable}.Count}) {
    /invoke ${out[12, "\aw${_variable}\ax ${dot} \a-g${${_variable}.Count}\ax ${If[${${_variable}} > 1 || ${${_variable}} == 0,items,item]}"]}
    /vardata si ${_variable}.First.Clone
    /while (!${si.IsEnd}) {
      /invoke ${out[12, "${si.Value}"]}
      /invoke ${si.Advance}
    }

  | does it exist
  } else /if (!${Defined[${_variable}]}) {
    /invoke ${out[0, "${notice}${sep}\aw${_variable}\ax is not defined. Nothing to see."]}
    /return

  } else {

    /if (${${_variable}.Count} == 0) {
      /invoke ${out[0, "\a-w${dot} ${_variable} is empty\ax"]}
      /return FALSE
    } else {
      /invoke ${out[0, "\a-w${dot} you spelled something wrong\ax"]}
      /return FALSE
    }
  }

  /if (${_silent.NotEqual[silent]}) {
    /invoke ${out[18, 0, 33]}
  }

/return TRUE



|***
 * note: control for burn
 * use: /burn []
 ***|
#bind control_burn /burn
sub Bind_control_burn(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atBind_control_burn\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP burn ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]}) {

    /if (ENGINE1) {
      /invoke ${out[0, "/burn engine is only available with engine 2/3 enabled."]}
      /return FALSE
    }
    /call control_base_edit ${maDebug.Find[edit].Value} Burn 20 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }
  
  | trigger 10 min burns
  | requires a sub in custom.inc
  | sub burn_CLANAME_10()
  | build your routine there
  /if (${_type.Equal[ten]}) {
    /if (${SubDefined[burn_${Me.Class.ShortName}_10]}) {
      /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}    
      /call burn_${Me.Class.ShortName}_10
    } else {
      OUT ${notice} ${sep} sub burn_${Me.Class.ShortName}_10 has not been created in custom.inc
      /return FALSE
    }
    /return TRUE
  }

  | Burn Automatic on/off
  /if (${_type.Equal[auto]}) {
    /invoke ${set_switch_env[${_debug}, swBurnAuto, TRUE, ${_verbage}]}

  | Burn in Raids
  } else /if (${_type.Equal[raid]}) {
    /invoke ${set_switch_env[${_debug}, swBurnRaid, TRUE, ${_verbage}]}

  | Mob count #
  } else /if (${_type.Equal[count]}) {
    /if (!${set_control_num_range[${_debug}, stBurnCount, "${_verbage}", 0, 99]}) /return

  | Burn Engage at %
  } else /if (${_type.Equal[engage]}) {
    /if (!${set_control_num_range[${_debug}, stPctBurnEngage, "${_verbage}", 0, 99]}) /return

  | force constant burning
  } else /if (${_type.Equal[force]}) {
    /invoke ${set_switch_env[${_debug}, swBurnForce, TRUE, ${_verbage}]}
    /if (${maBurn.Find[swBurnForce].Value}) {
      /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
    } else /if (!${maBurn.Find[swBurnForce].Value}) {
      /invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
    }

  | spin up some burns before combat
  } else /if (${_type.Equal[spinup]}) {
    OUT Starting the burns I can get away with ${dot}${dot}
    /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
    /invoke ${maBurn.Add[swBurnSpinUp,TRUE]}
    /call burn ${maDebug.Find[burn].Value} 0
    /delay 5
    /invoke ${maBurn.Add[swBurnSpinUp,FALSE]}
    /return TRUE

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[auto,swBurnAuto]}
  /invoke ${maControl.Add[count,stBurnCount]}
  /invoke ${maControl.Add[engage,stPctBurnEngage]}
  /invoke ${maControl.Add[force,swBurnForce]}
  /invoke ${maControl.Add[raid,swBurnRaid]}
  /invoke ${maControl.Add[spinup,option]}
  /invoke ${maControl.Add[list,list]}
  /if (${SubDefined[burn_${Me.Class.ShortName}_10]}) {
    /invoke ${maControl.Add[ten,option]}
  }    
  /invoke ${set_control_output[${_debug}, maBurn, burn, maControl]}

/return TRUE



|***
 * note: set various debuff things
 * use: /debuff []
 ***|
#bind control_debuff /debuff
sub Bind_control_debuff(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atBind_control_debuff\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP debuff ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,12:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} deBuff 12 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    
    /if (${_type.Equal[list]}) {
      OUT \awdeBuff Order\aw${sep}\a-wlsdeBuffOrder\ax
      /call out_list lsdeBuffOrder
    }
    /return TRUE
  }

  /if (${_type.Equal[mez]}) {
    /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swMez, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[resistcount]}) {
      /if (!${set_control_num_range[${_debug}, stCountMezResist, "${_verbage2}", 0, 10]}) /return
    }
    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swMez]}
    /invoke ${maControl.Add[resistcount,stCountMezResist]}
    /invoke ${set_control_output[${_debug}, madeBuff, "debuff mez", maControl]}
    /return TRUE

  } else /if (${_type.Equal[malo]}) {
    /invoke ${set_switch_env[${_debug}, swMalo, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[tash]}) {
    /invoke ${set_switch_env[${_debug}, swTash, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[slow]}) {
    /invoke ${set_switch_env[${_debug}, swSlow, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[snare]}) {
    /invoke ${set_switch_env[${_debug}, swSnare, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[cripple]}) {
    /invoke ${set_switch_env[${_debug}, swCripple, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[eradicate]}) {
    /invoke ${set_switch_env[${_debug}, swEradicate, TRUE, ${_verbage}]}

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[eradicate,swEradicate]}
  /invoke ${maControl.Add[malo,swMalo]}
  /invoke ${maControl.Add[mez,swMez|sm]}
  /invoke ${maControl.Add[slow,swSlow]}
  /invoke ${maControl.Add[cripple,swCripple]}
  /invoke ${maControl.Add[snare,swSnare]}
  /invoke ${maControl.Add[tash,swTash]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, madeBuff, debuff, maControl]}

/return TRUE



|***
 * note: control for /cc
 * use:
 ***|
#Bind control_cc /cc
sub Bind_control_cc(string _type, string _verbage, string _verbage2, string _verbage3, bool _debug)
  DEBUG \atBind_control_cc\ax(\a-wt:${_type}, v:${_verbage}, v2:${_verbage2}, v3:${_verbage3}\ax)

  /declare _out string local

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP cc ${_type.Right[1]}
    /return TRUE
  }

  | set assist
  /if (${_type.Equal[ass]}) {
    
    /if (${_verbage.Equal[list]}) {
      
      /if (${Select[${_verbage2},clear]}) {
        /call do_raw_edit ${_debug} SILENT lsAss clear
      } else /if (${Select[${_verbage3},add,delete]}) {
        /call do_raw_edit ${_debug} SILENT lsAss "${_verbage2}" ${_verbage3}
      }
      
      OUT Assist List${sep}\a-wlsAss\ax
      /call out_list lsAss
      /return TRUE

    } else /if (${_verbage.Equal[ma]}) {
      /invoke ${set_switch_env[${_debug}, swManualAssist, TRUE, ${_verbage2}]}

    } else /if (${_verbage.Equal[smart]}) {
      /if (${Select[${_verbage2},g,1,2,3,m1,m2,m3]}) {
        /call do_raw_edit ${_debug} SILENT stAssistMode ${_verbage2}

      } else  /if (${Select[${_verbage2},off,FALSE]}) {
        /call do_raw_edit ${_debug} SILENT stAssistMode FALSE
      } else {
        /lua run HELP cc ccasserror
        /return FALSE
      }
      
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[list,list]}
    /invoke ${maControl.Add[smart,stAssistMode|br]}
    /invoke ${maControl.Add[ma,swManualAssist]}
    /invoke ${set_control_output[${_debug}, maCC, "cc ass", maControl]}   
      
    /return TRUE

  | glyphs
  } else /if (${_type.Equal[glyph]}) {
    /if (${_verbage.Equal[group]}) {
      /invoke ${set_switch_env[${_debug}, swGlyphGroup, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[raid]}) {
      /invoke ${set_switch_env[${_debug}, swGlyphRaid, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[name]}) {
      /invoke ${do_raw_edit[FALSE, SILENT, stGlyph, "${_verbage2}"]}
    }
    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[group,swGlyphGroup]}
    /invoke ${maControl.Add[name,stGlyph|br]}
    /invoke ${maControl.Add[raid,swGlyphRaid]}
    /invoke ${set_control_output[${_debug}, maCC, "cc glyph", maControl]}
    /return TRUE

  | set combat mode
  } else /if (${_type.Equal[mode]}) {

    | enable combat: melee
    /if (${_verbage.Equal[melee]}) {
      /invoke ${set_switch_env[${_debug}, swCombatMelee, TRUE, ${_verbage2}]}

    | enable combat: range
    } else /if (${_verbage.Equal[range]}) {
      /invoke ${set_switch_env[${_debug}, swCombatRange, TRUE, ${_verbage2}]}

    | enable combat: nuke
    } else /if (${_verbage.Equal[nuke]}) {
      /invoke ${set_switch_env[${_debug}, swCombatNuke, TRUE, ${_verbage2}]}

    | enable combat: dot
    } else /if (${_verbage.Equal[dot]}) {
      /invoke ${set_switch_env[${_debug}, swCombatDoT, TRUE, ${_verbage2}]}

    | enable combat: Heal
    } else /if (${_verbage.Equal[heal]}) {
      /invoke ${set_switch_env[${_debug}, swCombatHeal, TRUE, ${_verbage2}]}

    | enable combat: Heal
    } else /if (${_verbage.Equal[cc]}) {
      /if (!${Select[${Me.Class.ShortName},ENC,BRD]}) {
        /return FALSE
      }
      /invoke ${set_switch_env[${_debug}, swCombatControl, TRUE, ${_verbage2}]}

    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[range,swCombatRange]}
    /invoke ${maControl.Add[dot,swCombatDoT]}
    /invoke ${maControl.Add[melee,swCombatMelee]}
    /invoke ${maControl.Add[nuke,swCombatNuke]}
    /invoke ${maControl.Add[heal,swCombatHeal]}
    /if (${Select[${Me.Class.ShortName},ENC,BRD]}) {
      /invoke ${maControl.Add[cc,swCombatControl]}
    }

    /invoke ${set_control_output[${_debug}, maCC, "cc mode", maControl]}
    /return TRUE

  | mob engage %
  } else /if (${_type.Equal[engage]}) {
    /if (!${set_control_num_range[${_debug}, stPctEngage, "${_verbage}", 0, 100]}) /return

  | set Z radius
  } else /if (${_type.Equal[zradius]}) {
    /if (!${set_control_num_range[${_debug}, stZRadius, "${_verbage}", 0, 300]}) /return

  | use alliance
  } else /if (${_type.Equal[alliance]}) {
    | /invoke ${set_switch_env[${_debug}, swAlliance, TRUE, ${_verbage}]}

  | stop on target DS switch
  } else /if (${_type.Equal[stoponds]}) {
    /invoke ${set_switch_env[${_debug}, swStoponDS, TRUE, ${_verbage}]}

  | force manual combat routines
  } else /if (${_type.Equal[force]}) {
    /if (!${Target.ID} || (${Target.ID} == ${Me.ID}) || (${Target.ID} == ${Me.Pet.ID})) {
      /return FALSE
    }

    /varset stAssTarID ${Target.ID}
    /vardata targetData Target.ID
    /if (${Me.Invis}) {
      /call Bind_control_nomore invis ${_debug}
    }

    /invoke ${maCC.Add[swValidAssTarID,TRUE]}
    /invoke ${maCC.Add[swSetCombat,TRUE]}
    /invoke ${maCC.Add[swSetForced,TRUE]}
    /if (${swModeToken}) {
      /invoke ${out[12, "Hunting ${dot} ${con_color[${_debug}, ${Target.ID}]}"]}
    } else {
      /invoke ${out[12, "/cc force ${dot} ${con_color[${_debug}, ${Target.ID}]}"]}
    }
    /if (${maMinion.Find[swonForce].Value}) {
      /delay 1
      /pet attack
    }

    /if (FALSE) {
      /echo swValidAssTarID${sep}\a-w${maCC.Find[swValidAssTarID].Value}\ax
      /echo swSetCombat${sep}\a-w${maCC.Find[swSetCombat].Value}\ax
      /echo swSetForced${sep}\a-w${maCC.Find[swSetForced].Value}\ax
      /echo stAssTarID${sep}\a-w${stAssTarID}\ax
    }

    /return TRUE

  | shortcut to broadcast opverride engage pct to force instant engage
  } else /if (${_type.Equal[kill]}) {

    /if (${_type.Equal[all]}) {
      /dgexecute all /invoke ${maOver.Add[swOverPctEngage,TRUE]}
    } else {
      /dggexecute /invoke ${maOver.Add[swOverPctEngage,TRUE]}
    }

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[ass,submenu]}
  /invoke ${maControl.Add[force,option]}
  /invoke ${maControl.Add[kill,option]}
  /invoke ${maControl.Add[mode,submenu]}
  /invoke ${maControl.Add[glyph,submenu]}
  /invoke ${maControl.Add[engage,stPctEngage]}
  /invoke ${maControl.Add[zradius,stZRadius]}
  /invoke ${maControl.Add[stoponds,swStoponDS]}
  | /invoke ${maControl.Add[alliance,swAlliance]}
  /invoke ${set_control_output[${_debug}, maCC, cc, maControl]}

/return TRUE



|***
 * note: help text
 * use:
 ***|
#Bind command_aux /aux
sub Bind_command_aux(string _type, bool _debug)


  OUT IN Macro Help is currently Disabled /sadpanda
  /return FALSE

  /declare _count int local 0
  /declare _lsCompleteAux list local
  
  /declare _list1 string local alto,beg,buff,build,burn,cc,crew,cursor,debuff,debug
  /declare _list2 string local edit,enter,cure,heal,home,invis,mode,minion,
  /declare _list3 string local override,see,splash,tag,rez,tag,entropy
  /declare _list4 string local miscdps,song,line,area,sash,pred,mercenary,tie,tc
  /declare _list5 string local coh,farm,env,grab,grain
  /declare _list6 string local gtfo,pull

  /for _count 1 to 6
    /declare _lsout${_count} list local
    /invoke ${_lsout${_count}.Append[${_list${_count}}]}
    /invoke ${_lsCompleteAux.Append[${_list${_count}},]}
  /next _count    

  /if (${_lsCompleteAux.Contains[${_type}]}) {
    /sqlite query "${DBstatic}" dummy SELECT * FROM help WHERE help_cat='${_type}' ORDER BY ROWID
    /if (!${sql_check[${_debug}, dummy, Bind_command_aux, SELECT]}) /endmacro
    DEBUG dummy SELECT * FROM help WHERE help_cat='${_type}' ORDER BY help_var
    MQCLEAR
    ECHO
    /invoke ${out[0, "\aw/aux \ax\a-w${_type.Lower}\ax"]}
    /for _count 1 to ${sqlite.Rows[dummy]}
      /invoke ${out[0,${If[${sqlite.Result[dummy ${_count} help_var].Find[${_type}]},,${sqlite.Result[dummy ${_count} help_var]}${sep}]}\a-g${Parse[1,${sqlite.Result[dummy ${_count} value]}]}\ax]}
    /next _count
    /invoke ${out[18, 0, 33]}
    /return TRUE
  }

  ECHO
  /sqlite query "${DBstatic}" dummy SELECT value FROM online WHERE env_var='discord'
  /if (!${sql_check[${_debug}, dummy, Bind_command_aux, SELECT]}) /endmacro
  /invoke ${out[0, "Discord${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}
  /invoke ${out[0, "Available Control Information${sep}"]}
  /declare _out string local
  /declare _liaux listiterator local
  /invoke ${out[0, "/aux${sep}"]}
  /for _count 1 to 6
    /varset _out
    /vardata _liaux _lsout${_count}.First.Clone
    /while (!${_liaux.IsEnd}) {
      /varset _out ${_out} ${If[${_liaux.Value.Equal[${_lsout${_count}.First}]},,${dot}]} ${info}${_liaux.Value}\ax
      /invoke ${_liaux.Advance}
    }
    /invoke ${out[0, "${_out}"]}
  /next _count

  /return TRUE

/return TRUE



|***
 * note: swap items. cause fuck MQ2Exchange
 * use: /swap "Item Name" slot [DEBUG] || /call Bind_command_swap "Item Name" slot [DEBUG]
 ***|
#Bind command_swap /swap
sub Bind_command_swap(string _item, int _toslot, bool _debug)
  DEBUG \atBind_command_swap\ax(\aw${_item}, ${_toslot}\ax)

  | skip if its already there?
  /if (${InvSlot[${_toslot}].Item.Name.Equal[${_item}]}) {
    /return TRUE
  }

  | reasons to not do this thing!!
  /if (!${FindItemCount[${_item}]}) {
    /return FALSE
  }

  /declare _swapslot string local NULL
  /declare _pack int local 0
  /declare _slot int local 0

  | find the item we need moved
  | _swapslot markes the location we take it from.
  | in a bag
  /if (${InvSlot[pack${Select[${FindItem[=${_item}].ItemSlot},23,24,25,26,27,28,29,30,31,32]}].Item.Container}) {
    /varset _pack ${Int[${Math.Calc[${FindItem[=${_item}].ItemSlot}-22]}]}
    /varset _slot ${Int[${Math.Calc[${FindItem[=${_item}].ItemSlot2}+1]}]}
    /varset _swapslot |INBAG|Pack${_pack}|${_slot}
    DEBUG ${sep}\a-gINBAG\ax _swapslot ${_swapslot}

  | in a top slot
  } else /if (${FindItem[=${_item}].InvSlot}) {
    /varset _slot ${FindItem[=${_item}].InvSlot}
    /varset  _swapslot |TOPSLOT|${_slot}
    DEBUG ${sep}\a-gTOPSLOT\ax _swapslot ${_swapslot}

  } else /if (${Cursor.Name.Equal[=${_item}]}) {
    /varset _slot NA
    /varset  _swapslot |CURSOR|${_slot}
    DEBUG ${sep}\a-gCURSOR\ax _swapslot ${_swapslot}

  | this is G7s fault
  } else /if (!${FindItemCount[=${_item}]} && !${Cursor.Name.Equal[${_item}]}) {
    | /invoke ${out[0, "Cannot Find${sep}\a-w${_item}\ax"]}
    /squelch /autoinventory
    /return FALSE
  }

  | get item to move on your cursor
  DEBUG ${sep}\aoPICK UP TEM TO MOVE\ax
  /if (${_swapslot.Arg[1,|].Equal[INBAG]}) {
    DEBUG ${sep}\aoFROM BAG\ax /itemnotify in pack${_pack} ${_slot} leftmouseup
    /squelch /nomodkey /shiftkey /itemnotify in pack${_pack} ${_slot} leftmouseup

  } else /if (${_swapslot.Arg[1,|].Equal[TOPSLOT]}) {
    DEBUG ${sep}\aoFROM TOPSLOT\ax /itemnotify ${_slot} leftmouseup
    /squelch /nomodkey /shiftkey /itemnotify ${_slot} leftmouseup

  } else /if (${_swapslot.Arg[1,|].Equal[CURSOR]}) {

  }
  
  /delay 1s ${Cursor.ID}

  | put item in slot
  /if (${Range.Between[1,32:${_toslot}]}) {
    DEBUG ${sep}\awNEWITEM Home\ax /itemnotify ${Int[${Math.Calc[${_toslot}]}]} leftmouseup
    | /squelch /nomodkey /shiftkey /itemnotify ${_toslot} leftmouseup
    /nomodkey /shiftkey /itemnotify ${_toslot} leftmouseup
  }

| /delay 1s ${Cursor.ID}

  | place the swaped item back where you found the other one
  DEBUG ${sep}\a-yRETURNING SWAPPED ITEM TO\ax
  /if (${Cursor.ID}) {
    /if (${_swapslot.Arg[1,|].Equal[INBAG]}) {
      DEBUG ${sep}\a-yINBAG\ax /itemnotify in ${_swapslot.Arg[2,|]} ${_swapslot.Arg[3,|]} leftmouseup
      /squelch /nomodkey /shiftkey /itemnotify in ${_swapslot.Arg[2,|]} ${_swapslot.Arg[3,|]} leftmouseup
    } else /if (${_swapslot.Arg[1,|].Equal[TOPSLOT]}) {
      DEBUG ${sep}\a-yTOPSLOT\ax /itemnotify ${_swapslot.Arg[2,|]} leftmouseup
      /squelch /nomodkey /shiftkey /itemnotify ${_swapslot.Arg[2,|]} leftmouseup

    } else /if (${_swapslot.Arg[1,|].Equal[CURSOR]}) {
      DEBUG ${sep}\a-yANYWHERE\ax /autoinventory
      /squelch /autoinventory

    }

  }

/return TRUE



|***
 * note: complete stop.. of all your dumb shit
 * use: /alto
 ***|
#bind command_alto /alto
sub Bind_command_alto(string _type, string _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP alto ${_type.Right[1]}
    /return TRUE
  }
  
  /if (${Me.AutoFire}) {
    /squelch /autofire
  }
  /if (${Me.Combat}) {
    /squelch /attack off
  }

  /call set_stop ${maDebug.Find[stop].Value}
  /invoke ${target[release]}
  /invoke ${maData.Add[swGetOverHere,FALSE]}
  /invoke ${maData.Add[stCastReturn,RESET]}
  
  /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}

/return TRUE



|***
 * note: navigates to name/target/locyxz/wp
 * use: /navto [NAME|current target|locyxz|wp] [nickname]
 ***|
#bind command_navto /navto
sub Bind_command_navto(string _verbage, string _loc, string _opt, bool _debug)
  DEBUG \atBind_command_navto\ax(\a-w${_verbage}, "${_loc}"\ax)

  /if (${_verbage.Left[1].Equal[-]}) {
   /lua run HELP navto ${_verbage.Right[1]}
   /return TRUE
  }

  | add/update a waypoint
  /if (${_verbage.Equal[wpadd]}) {
    /sqlite query "${DBnav}" dummy SELECT * FROM waypoint WHERE zone_shortname="${Zone.ShortName}" AND nickname="${_loc}"
    /if (!${sqlite.Rows[dummy]}) {
      /invoke ${out[19, "Waypoint add${sep}\a-w${_loc}\ax ${dot} Y:${num}${Me.Y}\ax X:${num}${Me.X}\ax Z:${num}${Me.Z}\ax"]}
      /sqlite query "${DBnav}" dummy INSERT INTO waypoint VALUES ("${Zone.ShortName}", "${_loc}", "${Me.Y}", "${Me.X}", "${Me.Z}")
    } else /if (${sqlite.Rows[dummy]}) {
      /invoke ${out[19, "Waypoint update${sep}\a-w${_loc}\ax ${dot} Y:${num}${Me.Y}\ax X:${num}${Me.X}\ax Z:${num}${Me.Z}\ax"]}
      /sqlite query "${DBnav}" dummy UPDATE waypoint SET locY="${Me.Y}", locX="${Me.X}", locZ="${Me.Z}" WHERE zone_shortname="${Zone.ShortName}" AND nickname="${_loc}"
    }

    /return

  | delete a waypoint
  } else /if (${_verbage.Equal[wpdelete]}) {

  | list the waypoints
  } else /if (${_verbage.Equal[wplist]}) {

    /sqlite query "${DBnav}" wpdummy SELECT COUNT(*) AS data FROM waypoint WHERE zone_shortname='${Zone.ShortName}'
    DEBUG ${sep}wpdummy SELECT count(*) as data from zone_control where zone_shortname  \ay${sqlite.Status[dummy]}\ax resutl:${sqlite.Result[wpdummy 1 data]}
    /if (${sqlite.Status[wpdummy].Find[no such table:]}) {
      ECHO fail
    }
    /declare _count int local 0

    /for _count 1 to ${sqlite.Result[wpdummy 1 data]}
      ECHO ${sqlite.Result[wpdummy 1{_count} nickname]}

    /next count

  | moveto a waypoint
  } else /if (${_verbage.Equal[wp]}) {
    /sqlite query "${DBnav}" dummy SELECT * FROM waypoint WHERE zone_shortname="${Zone.ShortName}" AND nickname="${_loc}"
    /if (!${sqlite.Rows[dummy]}) {
      /invoke ${out[1, shade, 0]}
      /return FALSE
    }
    /invoke ${out[19, "Waypoint move${sep}\a-w${_loc}\ax ${dot} Y:${num}${Me.Y}\ax X:${num}${Me.X}\ax Z:${num}${Me.Z}\ax"]}
    /nav locyxz ${sqlite.Result[dummy 1 locY]} ${sqlite.Result[dummy 1 locX]} ${sqlite.Result[dummy 1 locZ]} |dist=${maHome.Find[stHomeVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value}
    /return TRUE

  }

  | input notice
  /if (!${Target.ID} && !${Bool[${_verbage}]}) {
    OUT ${notice}
    OUT ${sep}need a name, target, loc, or waypoint options
    /return FALSE
  }

  /declare _tmpHomeMarker bool local FALSE
  /declare _tmpPullMarker bool local FALSE

  /if (${maHome.Find[swHomeMarker].Value}) {
    /varset _tmpHomeMarker TRUE
    /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
  }

  /if (${maPull.Find[swPull].Value}) {
    /varset _tmpPullMarker TRUE
    /invoke ${maPull.Add[swPull,FALSE]}
  }

  /if (${_opt.Equal[invis]} && !${Me.Invis}) {
    /call check_invis_single ${_debug}
  }

  | location given
  /if (${_verbage.Equal[loc]}) {
    DEBUG navto${sep}\a-wloc\ax
    /squelch /target clear
    /delay 1
    /invoke ${out[22, "/navto \a-w${_loc}\ax"]}
    /nav locyxz ${_loc} |dist=${maHome.Find[stHomeVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value}

  | nav to current target
  } else /if (${Target.ID} && !${Bool[${_verbage}]} && ${_verbage.NotEqual[loc]}) {
    DEBUG navto${sep}\a-wtarget\ax
    /invoke ${out[22, "/navto ${con_color[${_debug}, ${Target.ID}]}"]}
    /nav id ${Target.ID} |dist=${maHome.Find[stHomeVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value}

  | nav to closest spawn matching name
  } else /if (${Bool[${_verbage}]} && ${_verbage.NotEqual[loc]}) {
    /if (!${Spawn[${_verbage}].ID}) {
      /invoke ${out[22, "not in zone${sep}\a-w${_verbage}\ax"]}
      /return FALSE
    }
    DEBUG navto${sep}\a-wspawn\ax
    /invoke ${out[22, "/navto ${con_color[${_debug}, ${Spawn[${_verbage}].ID}]}"]}
    /nav id ${Spawn[${_verbage}].ID} |dist=${maHome.Find[stHomeVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value}

  }

  /if (${_opt.Equal[invis]} && !${Me.Invis}) {
    /call check_invis_single ${_debug}
  }

  /delay 5 ${Me.Moving}

  DEBUG ${sep}ok we're moving. right this way now..
  /while (${Me.Moving}) {
    /if (${_opt.Equal[invis]} && !${Me.Invis}) {
      /call check_invis_single ${_debug}
    }
    /delay 2
  }

  | reset home and pull if they were being used
  /if (${_tmpHomeMarker}) {
    /call Bind_control_home set NULL NULL ${maDebug.Find[home].Value}
  }
  /if (${_tmpPullMarker}) {
    /invoke ${maPull.Add[swPull,TRUE]}
  }

/return TRUE



|***
 * note: have everyone get your target and repeate what you say.
 * use: /repeat "things to say"
 ***|
#bind command_repeat /repeat
sub Bind_command_repeat(string _verbage, string _sender, int _tmpID, bool _debug)
  DEBUG \atBind_command_repeat\ax(\a-w${_verbage}, ${_sender}, ${_tmpID}\ax)


  /if (${_verbage.Left[1].Equal[-]}) {
    /lua run HELP repeat ${_verbage.Right[1]}
    /return TRUE
  }

  /call Bind_control_nomore invis ${_debug}
  /invoke ${out[21, "${broadcast}${sep}\a-w/repeat ${_verbage}\ax"]}
  | sender
  /if (!${Bool[${_sender}]}) {
    | need a target
    /if (!${Target.ID}) /return FALSE

    | speak you peace
    /say ${_verbage}

    | tell everyone else to speak up
    /dgexecute /repeat "${_verbage}" ${Me.DisplayName} ${Target.ID}
    /return TRUE
  }

  | everyone else
  /if (${_sender.NotEqual[${Me.DisplayName}]}) {
    /if (${Spawn[pc ${_sender} radius 60].ID}) {
      /call target set ${_tmpID}
      /if (${Target.ID} == ${_tmpID}) {
        /declare _tmpInt int local ${Math.Calc[${maEntropy.Find[stRNDRepeat].Value} + 10]}
        /delay ${Math.Rand[${_tmpInt}]}
        /say ${_verbage}
        /delay 1s
      }
    }
  }

/return TRUE



|***
 * note: make the toon stop its shit and splash at its feet
 * use: /dex NAME /splash | /splash
 ***|
#bind command_splash /splash
sub Bind_command_splash(bool _debug)
  DEBUG \atBind_command_splash\ax()

  /if (!${check_class[${_debug}, |PAL|CLR]}) {
    /return FALSE
  }
  
  | is splash ready?
  | if the user has not set the line order, set it to a default for now
  /if (${lsLineHeal.Contains[EMPTY]} && ${lsLineHeal.Count} == 1) {
    DEBUG ${sep}resetting the empty heal line
    /invoke ${lsLineHeal.Clear}
    /invoke ${lsLineHeal.Append[${stCount50}]}
  }

  /declare _switch bool local FALSE
  /declare _tmpSplash int local 0
  /declare _liHeal listiterator local
  /vardata _liHeal lsLineHeal.First.Clone
  /while (!${_liHeal.IsEnd}) {
    DEBUG ${sep}heal${sep}\a-w${maHeal.Find[stHeal${_liHeal.Value}].Value}${sep}${maHeal.Find[stHeal${_liHeal.Value}]}\ax
    GETINPUT cast_cycle_heal NA|NA
    ISMEDEAD
    GETOVERHERE
    CHECKEXIT cast_cycle_heal

    | check the switch
    /if (!${maHeal.Find[swHeal${_liHeal.Value}].Value}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maHeal.Find[stHeal${_liHeal.Value}].Value}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag for failure
    /if (!${lsHeal${_liHeal.Value}Tag.Contains[splash]}) {
      DEBUG ${break}no tag${sep}\a-wsplash\ax
      /invoke ${_liHeal.Advance}
      /continue
    }

    | ready?
    /if (${cast_data[${_debug}, "${maHeal.Find[stHeal${_liHeal.Value}].Value}"]}) {
      DEBUG ${sep}Spalsh is ready${sep}\aw${maHeal.Find[stHeal${_liHeal.Value}].Value}\ax
      /varset _tmpSplash ${_liHeal.Value}
      /varset _switch TRUE
      /break
    }

    /delay 1
    /invoke ${_liHeal.Advance}
  }

  | switch failure || spell not ready to be cast
  /if (!${_switch}) {
    DEBUG ${break}\arSplash is not ready\ax
    /return FALSE
  }

  /if (${maHeal.Find[swTwinHeal].Value}) {
    | /call cast_cooldown_wait ${_debug}
    /call cast_cycle_proc_twinheal ${_debug}  
    /if (${Select[${Macro.Return},1]}) {
      /call cast_cooldown_wait ${_debug}
    }
  }


  | correct the cast data back to the splash
  /invoke ${cast_data[${_debug}, "${maHeal.Find[stHeal${_tmpSplash}].Value}"]}

  
  | paladin
  /if (${Me.Class.ShortName.Equal[PAL]}) {
    DEBUG ${sep}paladin splash
    /call cast ${_debug} 0 FALSE
  }

  | cleric
  /if (${Me.Class.ShortName.Equal[CLR]}) {
    DEBUG ${sep}cleric splash
    /call cast ${_debug} ${Me.ID} FALSE
  }

/return TRUE



|***
 * note: clicks a door or lever to open or zone
 * use: /enter
 ***|
#bind command_enter /enter
sub Bind_command_enter(string _type, string _verbage, bool _debug)
  DEBUG \atBind_command_enter\ax()

  /call control_broadcast_command enter ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE

  ISMEDEAD

  /invoke ${set_data_timer[${_debug}, Try_Door, RESTART]}
  /squelch /target /clear

  /while (TRUE) {
    /if (!${timer_Try_Door}) /return FALSE
    /squelch /doortarget
    /delay 1
    /if (${Switch.ID}) /break
  }

  /if (!${timer_Try_Door}) /return FALSE

  /if (!${Switch.Open}) {
    /squelch /click left door
    /delay 1s
  }

/return TRUE



|***
 * note: control to trigger group invis
 * use: /invis
 ***|
#bind command_invis /invis
sub Bind_command_invis(string _type, string _verbage, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP invis ${_type.Right[1]}
    /return TRUE
  }

  /call control_broadcast_command invis ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE

  | fucking bards
  /if (${Select[${Me.Class.ShortName},BRD]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stInvisGroup].Value}"]}) {
      /call pulse_stop
      /delay ${maChr.Find[stInvisDelay].Value}
      /call cast ${_debug} 0 ignoreall
      /return TRUE
    }
  }

  | Invis vs Normies casters
  /if (${Select[${Me.Class.ShortName},WIZ,MAG,ENC,SHM,RNG,DRU]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stInvisGroup].Value}"]}) {
      /delay ${maChr.Find[stInvisDelay].Value}
      /call cast ${_debug} 0 ignoreall
      /return TRUE
    }
  }

  | Invis Vs undead casters
  /if (${Select[${Me.Class.ShortName},CLR,SHD,NEC,PAL]}) {
    /if (${cast_data[${_debug}, "${maChr.Find[stInvisGroupIVU].Value}"]} ) {
      /delay ${maChr.Find[stInvisDelay].Value}
      /call cast ${_debug} 0 ignoreall
      /return TRUE
    }
  }

/return TRUE



|***
 * note: displays the port and builds the command to the porting class
 * use: /port
 ***|
#bind control_port /port
sub Bind_control_port(string _type, string _nick1, string _nick2, bool _debug)
  DEBUG \atBind_control_port\ax(\a-wtype:${_type}, nick1:${_nick1}, nick2:${_nick2}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP port ${_type.Right[1]}
    /return TRUE
  }

  /declare _count int local 0
  /declare _portName string local FALSE
  /declare _list list local
  /declare _out string local
  /declare _tmpcount int local 0

  /if (${_type.Equal[see]} || !${Bool[${_type}]}) {
    /if (!${Bool[${_nick1}]}) {
      OUT Port categories${sep}\a-wwiz, dru, guild, item, vendor, aa\ax
      OUT \a-w/port see item\ax
      /return FALSE
    }
       
    /if (${Select[${_nick1},wiz,dru,guild,item,vendor,aa]}) {
      /sqlite query "${DBstatic}" dummycount SELECT Count(*) AS data FROM port WHERE type='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',]};
      DEBUG Count(${sqlite.Result[dummycount 1 data]})
      
      /declare _itemReady bool local TRUE

      /for _count 1 to ${sqlite.Result[dummycount 1 data]}
        /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',]};
        /varset _itemReady TRUE
        
        | have it, or the components??
        /if (${Select[${_nick1},item]}) {
          /if (!${FindItem[=${sqlite.Result[dummy ${_count} name1]}].ID}) {
            /continue
          }
          /if (${Bool[${sqlite.Result[dummy ${_count} component]}]} && !${FindItem[=${sqlite.Result[dummy ${_count} component]}].ID}) {
            /continue
          }
          /if (!${Me.ItemReady[=${sqlite.Result[dummy ${_count} name1]}]}) {
            /varset _itemReady FALSE
          }
        } else /if (${Select[${_nick1},guild]}) {
          /if (!${Ground.Search[${sqlite.Result[dummy ${_count} name1]}].ID}) {
            /continue
          }
        } else /if (${Select[${_nick1},wiz,dru]}) {
        } else /if (${Select[${_nick1},vendor]}) {
        } else /if (${Select[${_nick1},aa]}) {
          /if (!${Me.AltAbilityReady[${sqlite.Result[dummy ${_count} name1]}]}) {
            /continue
          }
          
        }     
        
        | build string
        | nickname1
        /varset _out ${sep}\aw${sqlite.Result[dummy ${_count} nick1]}\ax
        | if nickname2, add
        /varset _out ${_out}${If[${Bool[${sqlite.Result[dummy ${_count} nick2]}]},${sep}\aw${sqlite.Result[dummy ${_count} nick2]}\ax,]}
        
        | name1
        /varset _out ${_out}${If[${Bool[${sqlite.Result[dummy ${_count} name1]}]},${sep}\a-w${sqlite.Result[dummy ${_count} name1]}\ax,]}
      
        | if component
        /varset _out ${_out}${If[${Bool[${sqlite.Result[dummy ${_count} component]}]}, (\a-r${sqlite.Result[dummy ${_count} component]}\ax${sep}${FindItemCount[${sqlite.Result[dummy ${_count} component]}]}),]} ${If[!${_itemReady},\a#4183D7cooldown\ax,]}

        OUT ${_out}
        /varset _out
      /next _count    
    

      | /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='${_nick1}' ${If[${Bool[${_nick1}]},AND nick1='${_nick2}',]}

      | ECHO \awList\ax${sep}\a-w${sqlite.Rows[dummy]}\ax
      | /for _count 1 to ${sqlite.Rows[dummy]}
      | /next _count
      
      /invoke ${out[18, 0, 33]}
    }
    /return TRUE

  } 

  | /if (${Select[${_type},wiz,dru,guild,item,vendor,aa]}) {
  |   SDEBUG SELECT * FROM port WHERE type='${_type}' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',AND nick2='FALSE']} LIMIT 1
  |   /sqlite query "${DBstatic}" portreturn SELECT * FROM port WHERE type='${_type}' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',AND nick2='FALSE']} LIMIT 1
  |   /varset _portName ${sqlite.Result[portreturn 1 name1]}
  | }

  | wizard ports
  /if (${Select[${_type},wiz]}) {

  | druid ports
  } else /if (${Select[${_type},dru]}) {


  | guild placable ports
  } else /if (${Select[${_type},guild]}) {
    
    /if (!${Zone.ShortName.Find[guild]}) {
      OUT Not in a guild hall
      /return FALSE
    }
    
    | need a mesh
    /if (!${Navigation.MeshLoaded}) {
      /invoke ${out[0, "${notice} ${dot} NO Nav Mesh for this zone."]}
      /return FALSE
    }
    
    /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='guild' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',]} LIMIT 1;

    /invoke ${Ground.Search[${sqlite.Result[dummy 1 name1]}]}
    /declare _item ground local

    /invoke ${Ground.Search[${_item.ID}]}
    /nav item |dist=20 log=${maEntropy.Find[swNavLogSpew].Value}
    /delay 1s

    /while (${Me.Moving}) {
      DEBUG ${sep}whileloop${sep}\a-wMe.Moving\ax
      /if (${_item.Distance3D} <= 15 || !${Navigation.Active} || !${Me.Moving}) {
        DEBUG ${sep}distance${sep}\a-w${_item.Distance3D}\ax
        /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
        /break
      }
      /delay 2
    }

    | target the item
    /itemtarget "${sqlite.Result[dummy 1 name1]}"
    /delay 5
    | right click it to get a menu
    /click right item
    /delay 5
    | select the menu to port
    /invoke ${Menu.Select[${sqlite.Result[dummy 1 menu]}]}


  | item ports
  } else /if (${Select[${_type},item]}) {
    /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='item';
    /for _count 1 to ${sqlite.Rows[dummy]}
      /invoke ${_list.Append[${sqlite.Result[dummy ${_count} nick1]}]}
    /next _count

    | TBL port lamp
    /if (${_nick1.Equal[lamp]}) {
      /if (${_nick2.Equal[Zephyr]}) {
        /call get_item_convert FALSE "Wishing Lamp" "Wishing Lamp: Zephyr's Flight"
        /varset _portName Wishing Lamp: Zephyr's Flight
      } else /if (${_nick2.Equal[Palace]}) {
        /call get_item_convert FALSE "Wishing Lamp" "Wishing Lamp: Palace of Embers"
        /varset _portName Wishing Lamp: Palace of Embers
      } else /if (${_nick2.Equal[Stone]}) {
        /call get_item_convert FALSE "Wishing Lamp" "Wishing Lamp: The Stone Demesne"
        /varset _portName Wishing Lamp: The Stone Demesne
      }

    | zueria slide
    } else /if (${_nick1.Equal[slide]}) {
      /if (${_nick2.Equal[Stonebrunt]}) {
        /call get_item_convert FALSE "Zueria Slide" "Zueria Slide: Stonebrunt"
        /varset _portName Zueria Slide: Stonebrunt
      } else /if (${_nick2.Equal[GreatDivide]}) {
        /call get_item_convert FALSE "Zueria Slide" "Zueria Slide: Great Divide"
        /varset _portName Zueria Slide: Great Divide
      } else /if (${_nick2.Equal[Ro]}) {
        /call get_item_convert FALSE "Zueria Slide" "Zueria Slide: North Ro"
        /varset _portName Zueria Slide: North Ro
      } else /if (${_nick2.Equal[Nektulos]}) {
        /call get_item_convert FALSE "Zueria Slide" "Zueria Slide: Nektulos"
        /varset _portName Zueria Slide: Nektulos
      } else /if (${_nick2.Equal[Dreadlands]}) {
        /call get_item_convert FALSE "Zueria Slide" "Zueria Slide: Dreadlands"
        /varset _portName Zueria Slide: Dreadlands
      } else /if (${_nick2.Equal[Skyfire]}) {
        /call get_item_convert FALSE "Zueria Slide" "Zueria Slide: Skyfire"
        /varset _portName Zueria Slide: Skyfire
      }

    } else /if (${_list.Contains[${_nick1}]}) {
      /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='item' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',]};
      /varset _portName ${sqlite.Result[dummy 1 name1]}

    } else {
      /invoke ${out[0, "not a valid item port"]}
    }

    /if (${Bool[${_portName}]}) {
      /call Bind_control_nomore invis ${_debug}
      /if (${cast_data[${_debug}, "${_portName}"]}) {
        /call cast ${_debug} 0 FALSE
      }
    }


  | vendor ports
  } else /if (${Select[${_type},vendor]}) {

    | need a mesh
    /if (!${Navigation.MeshLoaded}) {
      /invoke ${out[0, "${notice} ${dot} NO Nav Mesh for this zone."]}
      /return FALSE
    }
    
    | set teleport merchant
    /declare _portSpawn spawn local
    /if (${Spawn[npc Zeflmin].ID}) {
      /vardata _portSpawn Spawn[npc Zeflmin]
    } else /if (${Spawn[npc Teleportation Assistan].ID}) {
      /vardata _portSpawn Spawn[npc Teleportation Assistan]
    } else {
      /invoke ${out[0, "${notice} ${dot} No Teleportation Merchant in this hall."]}
      /return FALSE
    }

    DEBUG ${sep}sqlite: dummy SELECT * FROM port WHERE type='vendor' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',]} LIMIT 1;
    /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='vendor' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',]} LIMIT 1;

    /if (!${sqlite.Rows[dummy]}) {
      /invoke ${out[0, "${notice} ${dot} Invalid Request: ${_nick1}"]}
      /return FALSE
    }

    DEBUG ${sep}results:
    DEBUG ${sep}${sep}component result: ${sqlite.Result[dummy 1 component]}]}
    DEBUG ${sep}${sep}component count: ${FindItemCount[=${sqlite.Result[dummy 1 component]}]}
    
    /while (!${FindItemCount[=${sqlite.Result[dummy 1 component]}]} && ${_tmpcount} < 5) {
      
      /if (${FindItemCount[=${sqlite.Result[dummy 1 component]}]}) {
        /break
      }
       
      /nav id ${_portSpawn.ID} |log=${maEntropy.Find[swNavLogSpew].Value} dist=8

      /if (!${Window[MerchantWnd].Open}) {
        /delay 10s !${Navigation.Active}
        /delay 1s
        /squelch /target id ${_portSpawn.ID}
        /delay 1s
        /nomodkey /click right target
        /delay 5s ${Window[MerchantWnd].Open}
      }

      /delay 1s
      /call vendor_buy ${maDebug.Find[vendor].Value} "${sqlite.Result[dummy 1 component]}" 1

      /if (${Window[MerchantWnd].Open}) {
        /nomodkey /notify MerchantWnd MW_Done_Button LeftMouseUp
      }
      /varcalc _tmpcount ${_tmpcount} + 1
    }
     
    | get stone on cursor
    /itemnotify "${sqlite.Result[dummy 1 component]}" leftmouseup
    /delay 5

    /if (!${Window[GiveWnd].Open}) {
      /squelch /target id ${_portSpawn.ID}
      /delay 5
      /nomodkey /click left target
      /delay 5
      /delay 5s ${Window[GiveWnd].Open}
    }

    /if (${Window[GiveWnd].Open}) {
      /notify GiveWnd GVW_Give_Button leftmouseup
      /delay 3s
    }

    /if (${Window[InventoryWindow].Open}) {
      /nomodkey /notify InventoryWindow IW_DoneButton LeftMouseUp
      /delay 5
    }

    /if (${Window[InventoryWindow].Open}) /nomodkey /notify InventoryWindow IW_DoneButton LeftMouseUp
    /delay 5

    /squelch /target clear


  | aa ports
  } else /if (${Select[${_type},aa]}) {
    /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='aa'
    /for _count 1 to ${sqlite.Rows[dummy]}
      /invoke ${_list.Append[${sqlite.Result[dummy ${_count} nick1]}]}
    /next _count

    /if (${_list.Contains[${_nick1}]}) {
      DEBUG ${sep}AA
      /sqlite query "${DBstatic}" dummy SELECT * FROM port WHERE type='aa' AND nick1="${_nick1}"
      /varset _portName ${sqlite.Result[dummy 1 name1]}

    } else {
      /invoke ${out[0, "not a valid AA port"]}
    }
    
    /if (${Bool[${_portName}]}) {
      /call Bind_control_nomore invis ${_debug}
      /if (${cast_data[${_debug}, "${_portName}"]}) {
        /call cast ${_debug} 0 skiptargetcheck
        /return TRUE
      }
    }    
    
  }


/return TRUE



|***
 * note: turns everyone off, get your tricks somewhere else...
 * use: /off
 ***|
#bind command_off /command_off
sub Bind_command_off(string _type, string _verbage, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP off ${_type.Right[1]}
    /return TRUE
  }
  
  /call control_broadcast_command off ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE
  /return TRUE

  /if (!${Bool[${_type}]} || ${Select[${_type},all,zone,group,raid]}) {
    /if (!${Bool[${_type}]} || ${_type.Equal[group]}) {
      /if (!${Bool[${_type}]}) {
        /invoke ${out[21, "${broadcast}${sep}\a-w/off\ax (\a-rdefault group\ax)"]}
      } else /if (${_type.Equal[group]}) {
        /if (!${Group.Members}) {
          /invoke ${out[21, "${broadcast}${sep}\a-w/off\ax (\a-rgroup\ax) (\a-rFail: not in a group\ax)"]}
          /return FALSE
        } else {
          /invoke ${out[21, "${broadcast}${sep}\a-w/off\ax (\a-rgroup\ax)"]}
        }
      }
      /dggexecute /env auto off

    } else /if (${_type.Equal[zone]}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/off\ax (\a-rzone\ax)"]}
      /dgzexecute /env auto off

    } else /if (${_type.Equal[all]}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rall\ax)"]}
      /dgexecute ${maEntropy.Find[stEntropyGroup_all].Value} /env auto off

    } else /if (${_type.Equal[raid]}) {
      /if (!${Raid.Members}) {
        /invoke ${out[21, "${broadcast}${sep}\a-w/off\ax (\a-rraid\ax) (\a-rFail: not in a raid\ax)"]}
        /return FALSE
      }
      /invoke ${out[21, "${broadcast}${sep}\a-w/off\ax (\a-rraid\ax)"]}
      /dgrexecute /env auto off
    }
    /return FALSE
  } 

/return TRUE



|***
 * note: turns everyone on.. who doesn't like a little foreplay
 * use: /on
 ***|
#bind command_on /command_on
sub Bind_command_on(string _type, string _verbage, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP on ${_type.Right[1]}
    /return TRUE
  }

  /call control_broadcast_command on ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE
  /return TRUE
    
  /if (!${Bool[${_type}]} || ${Select[${_type},all,zone,group,raid]}) {
    /if (!${Bool[${_type}]} || ${_type.Equal[group]}) {
      /if (!${Bool[${_type}]}) {
        /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rdefault group\ax)"]}
      } else /if (${_type.Equal[group]}) {
        /if (!${Group.Members}) {
          /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rgroup\ax) (\a-rFail: not in a group\ax)"]}
          /return FALSE
        } else {
          /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rgroup\ax)"]}
        }
      }
      /dggexecute /env auto on

    } else /if (${_type.Equal[zone]}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rzone\ax)"]}
      /dgzexecute /env auto on

    } else /if (${_type.Equal[all]}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rall\ax)"]}
      /dgexecute ${maEntropy.Find[stEntropyGroup_all].Value} /env auto on

    } else /if (${_type.Equal[raid]}) {
      /if (!${Raid.Members}) {
        /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rraid\ax) (\a-rFail: not in a raid\ax)"]}
        /return FALSE
      }
      /invoke ${out[21, "${broadcast}${sep}\a-w/on\ax (\a-rraid\ax)"]}
      /dgrexecute /env auto on
    }
    /return FALSE
  } 

/return TRUE



|***
 * note: there is shit everywhere....
 * use: /cls
 ***|
#bind command_cls /cls
sub Bind_command_cls(string _type, string _verbage, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP cls ${_type.Right[1]}
    /return TRUE
  }
  
  /call control_broadcast_command cls ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE

  /hidecorpse NPC
  /hidecorpse LOOTED

/return TRUE



|***
 * note: fuck, what did the bard do this time?
 * use: /here
 ***|
#bind command_here /here
sub Bind_command_here(string _type, string _verbage, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP here ${_type.Right[1]}
    /return TRUE
  }

  /call control_broadcast_command here ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE

  ISMEDEAD

  | disable Tie
  /call Bind_command_tie off ${maDebug.Find[tie].Value}
  | sort movements
  /if (${Stick.Active}) /squelch /stick off
  | stop mq2nav
  /if (${Navigation.Active}) /squelch /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}

  | enable auto
  /invoke ${set_switch_env[${_debug}, swAuto, FALSE, TRUE]}

  /call Bind_control_nomore invis ${_debug}
  /stopcast

  | reduce pull range if in a dense area
  /if (${maPull.Find[swPull].Value}  && ${SpawnCount[NPC radius PULLENV PS4 targetable]} >= AE#) {
    /if (PULLENV > 60) {
      /call Bind_control_cc pull rad 60
    }
  }

  | set home?
  /if (${maHome.Find[swonAuto].Value}) {
    /call Bind_control_home set NULL NULL ${maDebug.Find[home].Value}
  }

  | reset timers
  /declare lsHereTimers list local
  /invoke ${lsHereTimers.Append[Check_Buff_Aura,Check_Buff_Beg,Check_Buff_Crew,Check_Buff_Self,Check_Buff_Shrink,Check_Buff_Unity]}
  /invoke ${lsHereTimers.Append[Check_Minion_Cycle]}
  /declare _lihere listiterator local
  /vardata _lihere lsHereTimers.First.Clone
  /while (!${_lihere.IsEnd}) {
    /invoke ${set_data_timer[FALSE, ${_lihere.Value}, 1m]}
    /invoke ${_lihere.Advance}
  }

  | stop moving
  /if (${Me.Moving}) {
    /keypress forward
    /keypress back
  }

  | clear the current target
  /if (${Target.ID}) {
    /squelch /target clear
  }

  /if (PET) {
    /if (${Me.Pet.Target.ID} && ${Me.Pet.ID}) /pet back off
  }

/return TRUE



|***
 * note: clicks yes on a window option for some form of travel or trade
 * use: /take 
 ***|
#bind command_take /take
sub Bind_command_take(string _type, string _verbage, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
   /lua run HELP take ${_type.Right[1]}
   /return TRUE
  }

  /call control_broadcast_command take ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE

  ISMEDEAD
  
 
  | take ports
  /if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find[${Spawn[${Me}].Guild}]} || ${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["Guild Hall"]}) {
    /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
  }
  
  /if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["be translocated by"]}) {
    /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
  }
  
  /if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["be translocated to your bind point"]}) {
    /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
  }

  /if (${Window[LargeDialogWindow].Child[LDW_TextBox].Text.Find["Would you like to travel"]}) {
    /nomodkey /notify LargeDialogWindow LDW_YesButton leftmouseup
  }

  | take taskadds/missions/HA
  /if (${Window[LargeDialogWindow].Child[LDW_TextBox].Text.Find["You have a heroic adventure"]}) {
    /nomodkey /notify LargeDialogWindow LDW_YesButton leftmouseup
  }
  
  /if (${Window[LargeDialogWindow].Child[LDW_TextBox].Text.Find["Would you like to take on the challenge"]}) {
    /nomodkey /notify LargeDialogWindow LDW_YesButton leftmouseup
  }
  
  /if (${Window[ConfirmationDialogBox].Child[CD_TextOutPut].Text.Find["Join the expedition"]}) {
    /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
  }
  
  /if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["has asked you to join the shared task"]}) {
    /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
  }

  | accept highlighted task
  /if (${Window[TaskSelectWnd]}) {
    /nomodkey /notify TaskSelectWnd TSEL_AcceptButton leftmouseup
  }

  | rez
  /if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["Blessing of Resurrection"]}) {
    /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
  }

  | trade window
  /if (${Window[Tradewnd]}) {
    /nomodkey /notify Tradewnd TRDW_Trade_Button leftmouseup
  }

  | give trade (inc NPC)
  /if (${Window[GiveWnd]}) /squelch /notify GiveWnd GVW_Give_Button leftmouseup

/return TRUE



|***
 * note: grabs a ground spawn. parital names accepted
 * use: /grab NAME [SILENT]
 ***|
#bind command_grab /grab
sub Bind_command_grab(string _name, string _silent, bool _debug)
  DEBUG Bind_command_grab(\a-w${_name}\ax)

  /if (${_name.Left[1].Equal[-]}) {
    /lua run HELP grab ${_name.Right[1]}
    /return TRUE
  }
  
  | typed something wrong
  /if (${_name.Equal[NULL]} || !${Bool[${_name}]} || ${_name.Equal[info]} ) {
    /lua run HELP grab h
    /return
  }

  | trash pick up.
  /if (${Ground[${_name}].Distance3D} > ${maHard.Find[stRadGrab].Value}) {
    /if (${_silent.NotEqual[silent]}) {
      OUT ${info}${_name}\ax is ${Ground[${_name}].Distance3D}. Needs to be within${sep}\a-w${maHard.Find[stRadGrab].Value}\ax
    }
    /return
  } else /if (${Ground[${_name}].Distance3D} <= ${maHard.Find[stRadGrab].Value}) {
    /invoke ${Ground.Search[${_name}].Grab}
  }

/return TRUE



|***
 * note: lesson of the devoted
 * use: /lesson
 ***|
#bind command_lesson /lesson
sub Bind_command_lesson(string _type, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP lesson ${_type.Right[1]}
    /return TRUE
  }

  /if (!${cast_data[${_debug}, "Lesson of the Devoted"]}) {
    /return FALSE
  }

  /call cast ${_debug} 0 FALSE

/return TRUE



|***
 * note: Request Call of the Hero [aa|group]
 * use: /coh [group]
 ***|
#bind command_coh /coh
sub Bind_command_coh(string _type, bool _debug)
  DEBUG /atBind_command_coh/ax(/a-w${_type}/ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP coh ${_type.Right[1]}
    /return TRUE
  }

  /if (${_type.Equal[group]}) {
    /dex ${get_resident[${_debug}, |mag, 99999]} /call_of_the_hero "${Me.DisplayName}" "${_type}"
    /return TRUE
  } else {
    /dex ${get_resident[${_debug}, |mag, 99999]} /call_of_the_hero "${Me.DisplayName}"
    /return TRUE
  }

/return FALSE



|***
 * note: allows for custom builds
 * use: /build [save|load|set] [nickname] [all] [DEBUG]
 ***|
#bind control_build /build
sub Bind_control_build(string _build, string _nickname, string _verbage, bool _debug)

  /if (${_build.Equal[ui]}) {
    /lua run ${MacroQuest.Path[macros]}\entropy\lua\build.lua
    /return TRUE
  }

  /if (${_build.Left[1].Equal[-]}) {
    /lua run HELP build ${_build.Right[1]}
    /return TRUE
  }

  /if (!${Bool[${_build}]} || !${Bool[${_nickname}]}) {
    /lua run HELP build u
    /delay 1
    /lua run HELP build h
    /return
  }
  
  /declare _count int local 0

  | set file name
  /declare _fn string local build_${Me.Class.ShortName.Lower}_${_nickname}.ini

  | set fully qualified MQ name
  /declare _fqn string local ${MacroQuest.Path}\\macros\\entropy\\data\\${_fn}

  | set stuff to not export/import
  /declare _skipMap list local
  /invoke ${_skipMap.Append[maDebug,maTimer,Gems,maMode,maDeez,maPREGEN,maRezControl,maWatchspawn]}
  /invoke ${_skipMap.Append[maBuffRequest,maHook,maData,maManaRecovery,maDeez,maTTL,Header,Evals,maPred]}

  /declare _skipKey list local
  /invoke ${_skipKey.Append[swBuffMaster,swIsMeDead,swSetCombat,swValidAssTarID,swSnapStuck,swHomeMarker,verbosity,swZoneSafe,swAuto]}
  /invoke ${_skipKey.Append[stCurrentZoneID,stCastReturn,build,swSetTie,swBurnConditionMet,swBurnSpinUp,repo,schema,stPredID]}
  /invoke ${_skipKey.Append[swWatch,stWatchCount,swSetForced]}

  | obfuscation
  /invoke ${_skipKey.Append[lsProtectOutsideGroup,stTieToon,lsChain,lsAss,lsSafeNames,lsNoHealToon,stRampageTank]}

  | export to INI routine
  /if (${_build.Equal[save]}) {
    OUT /build save${sep}\a-w${_fn}\ax
    OUT \awExporting\ax${sep}

    /ini "${_fqn}" "Header" "Macro" "Entropy"
    /ini "${_fqn}" "Header" "Build" "${maEnv.Find[build].Value}"
    /ini "${_fqn}" "Header" "Schema" "${maEnv.Find[schema].Value}"
    /ini "${_fqn}" "Header" "Hash" "MD5"
    /ini "${_fqn}" "Header" "NickName" "${_nickname}"
    /ini "${_fqn}" "Header" "Date" "${Time.Date}"
    /ini "${_fqn}" "Header" "Class" "${Me.Class.ShortName}"
    /ini "${_fqn}" "Header" "Level" "${Me.Level}"
    /ini "${_fqn}" "Header" "Race" "${Me.Race}"
    /ini "${_fqn}" "Header" "MQBuild" "${MacroQuest.BuildDate}"

    /declare _mi mapiterator local
    /declare _limaps listiterator local
    /vardata _limaps lsMaps.First.Clone
    /while (!${_limaps.IsEnd}) {

      | skip the maps we dont want to export
      /if (${_skipMap.Contains[${_limaps.Value}]}) {
        /invoke ${_limaps.Advance}
        /continue
      }

      ECHO \at...\ax\a-w${_limaps.Value}\ax
        /vardata _mi ${_limaps.Value}.First.Clone
        /while (!${_mi.IsEnd}) {
          /if (${_skipKey.Contains[${_mi.Key}]}) {
            /invoke ${_mi.Advance}
            /continue
          }

          | skip empty/false values
          /if (${_verbage.NotEqual[all]}) {
            /if (${_mi.Value.Equal[FALSE]} || ${_mi.Value.Equal[EMPTY]}) {
              /invoke ${_mi.Advance}
              /continue
            }
          }
          
          | character name protection
          /if (${_mi.Value.Find[temp]}) {
            /invoke ${_mi.Advance}
            /continue
          }

          | OUTPUT TO THE INI
          /ini "${_fqn}" ${_limaps.Value} "${_mi.Key}" "${Parse[1,${_mi.Value}]}"
          /invoke ${_mi.Advance}
        }
      /invoke ${_limaps.Advance}
    }

    | output gems
    /if (${Me.NumGems} && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
      ECHO \at...\ax\a-wgem\ax
      /for _count 1 to ${Me.NumGems}
        /ini "${_fqn}" Gems "Gem${_count}" "${Spell[${Me.Gem[${_count}]}].BaseName}"
      /next _count
    }

    | output tag/eval
    ECHO \at...\ax\a-weval\ax
    /sqlite query "${DBshared}" dummy SELECT tag_group.grouping, tag.name, tag_validate.created_by, tag_validate.eval, tag_validate.aux FROM tag_validate LEFT JOIN tag ON tag_validate.tag = tag.id LEFT JOIN tag_group ON tag_validate.grouping = tag_group.id ORDER BY tag_group.grouping, tag.name
    /for _count 1 to ${sqlite.Rows[dummy]}
      /if (${sqlite.Result[dummy ${_count} created_by].Equal[2]} && ${sqlite.Result[dummy ${_count} eval].NotEqual[marker name]}) {
        /ini "${_fqn}" "Evals" "${sqlite.Result[dummy ${_count} grouping]}::${sqlite.Result[dummy ${_count} name]}" "${Parse[2,${sqlite.Result[dummy ${_count} eval]}]}"
      }
    /next _count

    | output md5
    ECHO \at...\ax\a-wmd5\ax
    /squelch /exec powershell "(get-content ${MacroQuest.Path}\macros\entropy\data\${_fn}).Replace('MD5',(Get-FileHash ${MacroQuest.Path}\macros\entropy\data\${_fn} -Algorithm MD5).Hash) | Set-Content ${MacroQuest.Path}\macros\entropy\data\${_fn}" bg
    /invoke ${out[18, 0, 33]}
    ECHO
    /return TRUE

  | import INI routine
  } else /if (${_build.Equal[load]}) {
    /declare _section string local
    /declare _key string local
    /declare _value string local
    /declare _countS int local 0
    /declare _countK int local 0

    OUT /build load${sep}\a-w${_fn}\ax
    OUT \awLoading\ax${sep}

    /for _countS 1 to ${Ini[${_fqn}].Count[|]}
      /varset _section ${Ini[${_fqn}].Arg[${_countS},|]}
      /if (${_skipMap.Contains[${_section}]}) /continue
      /if (${Select[${_section},,NULL,Header,Gems,Evals]}) /continue
      ECHO \at...\ax\a-w${_section}\ax
      /for _countK 1 to ${Ini[${_fqn},${_section}].Count[|]}
        /varset _key ${Ini[${_fqn},${_section} ].Arg[${_countK},|]}
        /if (${Select[${_key},NULL]}) /continue
        /if (${_skipKey.Contains[${_key}]}) /continue
        DEBUG ${_key}=${Ini[${_fqn},${_section},${Parse[1,${_key}]},noparse]}
        /call do_raw_edit FALSE SILENT ${_key} "${Ini[${_fqn},${_section},${Parse[1,${_key}]},noparse]}" overwrite
      /next _countK
    /next _countS


    | load gems
    /if (${Me.NumGems} && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
      ECHO \at...\ax\a-wgem\ax
      /if (!${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoOpen}
      /for _count 1 to ${Me.NumGems}
        /call set_spell_to ${_debug} TRUE "${Spell[${Ini[${_fqn},Gems,Gem${_count}]}].RankName}" ${_count} FALSE
      /next _count
      /if (${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoClose}
    }

    | load evals
    ECHO \at...\ax\a-weval\ax
    /for _countK 1 to ${Ini[${_fqn},Evals].Count[|]}
      /varset _key ${Ini[${_fqn},Evals].Arg[${_countK},|]}
      /if (${Select[${_key},NULL]}) /continue

      DEBUG ${sep}${_key.Arg[1,::]} ${_key.Arg[2,::]} "${Ini[${_fqn},Evals,${Parse[1,${_key}]},noparse]}"
      /call Bind_noparse_tag ${_key.Arg[1,::]} ${_key.Arg[2,::]} "${Ini[${_fqn},Evals,${Parse[1,${_key}]},noparse]}" TRUE

    /next _countK

    /invoke ${out[18, 0, 33]}
    ECHO
    /return TRUE


  } else /if (${_build.Equal[set]}) {
    DEBUG ${sep}set${sep}\a-w${_nickname}\ax
    /declare _loadtimestart int local ${MacroQuest.Running}
    /declare _lsset list local
    /invoke ${_lsset.Delimiter["|"]}
    /invoke ${_lsset.Append[${Ini["${INIset}",${_nickname}]}]}
    /declare _liset listiterator local
    /vardata _liset _lsset.First.Clone
    /invoke ${do_raw_edit[${_debug}, SILENT, stBuildSetCurrent, "${_nickname}"]}
    /while (!${_liset.IsEnd}) {
      /if (${Bool[${_liset.Value}]}) {
        /if (${lsBuildSet.Contains[${_liset.Value}]}) {
          /if (${_liset.Value.Equal[primary]}) {
            /call Bind_command_swap "${Ini["${INIset}",${_nickname},${_liset.Value}]}" 13 FALSE
          } else /if (${_liset.Value.Equal[secondary]}) {
            /call Bind_command_swap "${Ini["${INIset}",${_nickname},${_liset.Value}]}" 14 FALSE
          } else /if (${_liset.Value.Equal[range]}) {
            /call Bind_command_swap "${Ini["${INIset}",${_nickname},${_liset.Value}]}" 11 FALSE
          } else /if (${_liset.Value.Equal[spellset]}) {
            /squelch /memspellset ${Ini["${INIset}",${_nickname},${_liset.Value}]}
            /delay 1
          }
        } else {
          /call do_raw_edit ${_debug} SILENT ${_liset.Value} ${Ini["${INIset}",${_nickname},${_liset.Value}]}
        }
      }
      /invoke ${_liset.Advance}
    }
    SETHUD
    OUT Set${sep}\a-w${_nickname}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_loadtimestart}) / 10) / 60]}s\ax
    /return TRUE

  }

/return TRUE



|***
 * note: cursor control
 * use: /cursor
 ***|
#bind control_cursor /cursor
sub Bind_control_cursor(string _type, string _verbage, bool _debug)
  DEBUG \atBind_control_cursor\ax(\a-w${_type}, ${_verbage}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP cursor ${_type.Right[1]}
    /return TRUE
  }


  | output control map
  /if (${_type.Equal[list]}) {
    /if (!${Cursor.ID} && !${Bool[${_verbage}]}) {
      /invoke ${out[1, shade, 0]}
    } else {
      /sqlite query "${DBloot}" lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}"
      /if (!${sqlite.Rows[lootquery]}) {
        /invoke ${out[0, "settings for${sep}\a-w${Cursor.Name}\ax"]}
        /invoke ${out[0, "${notice}${sep}\a-wnot in loot database\ax"]}
        /invoke ${out[18, 0, 33]}
        /return FALSE
      }
      /invoke ${out[0, "settings for${sep}\a-w${sqlite.Result[lootquery 1 item]}\ax"]}
      /invoke ${out[0, "action${sep}\a-w${sqlite.Result[lootquery 1 action]}\ax"]}
      | /invoke ${out[0, "pass${sep}\a-w${sqlite.Result[lootquery 1 pass]}\ax"]}
      /declare _count int local 0
      /varcalc _count ${FindItemCount[${Cursor.ID}]} + ${FindItemBankCount[${Cursor.ID}]}
      /invoke ${out[0, "count${sep}\a-w${_count}\ax"]}
      /invoke ${out[18, 0, 33]}
    }
    /return TRUE

  } 

  | see if item is in the DB first?
  /sqlite query "${DBloot}" lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}"
  /if (${sqlite.Status[lootquery].NotEqual[Success]} || !${sqlite.Rows[lootquery]}) {
    /if (${FindItem[=${Cursor.Name}].NoDrop} || ${FindItem[=${Cursor.Name}].Lore} || ${FindItemBank[${Cursor.Name}].Lore}) {
      /sqlite query "${DBloot}" lootquery INSERT INTO loot_control(item,action)  VALUES("${Cursor.Name}", "leave")
    } else {
      /sqlite query "${DBloot}" lootquery INSERT INTO loot_control(item,action) VALUES("${Cursor.Name}", "keep")
    }
  }

  /if (${Select[${_type},destroy,leave,keep,ignore]}) {
    /declare _item string local

    /if (${Cursor.ID}) {
      /varset _item ${Cursor.Name}
    } else /if (!${Cursor.ID} && ${Bool[${_verbage}]}) {
      /varset _item ${_verbage}
    } else /if (!${Cursor.ID} && !${Bool[${_verbage}]}) {
      /invoke ${out[1, shade, 0]}
    }

    /sqlite query "${DBloot}" lootquery UPDATE loot_control SET action="${_type.Lower}" WHERE item="${_item}"
    DEBUG ${sep}lootquery UPDATE loot_control SET action="${_type.Lower}" WHERE item="${_item}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
    OUT Cursor${sep}\aw${_item}\ax${sep}\a-w${_type.Lower}\ax
    /return TRUE

  } else /if (${_type.Equal[pass]}) {
    /sqlite query "${DBloot}" lootquery UPDATE loot_control SET pass="${_verbage}" WHERE item="${Cursor.Name}"
    DEBUG ${sep}lootquery UPDATE loot_control SET pass="${_verbage}" WHERE item="${Cursor.Name}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
    OUT Cursor${sep}\aw${_verbage}\ax${sep}\a-wpass\ax
    /return TRUE
  } 

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[destroy,out]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[ignore,out]}
  /invoke ${maControl.Add[keep,out]}
  /invoke ${maControl.Add[leave,out]}
  | /invoke ${maControl.Add[pass,out]}
  /invoke ${set_control_output[${_debug}, maData, cursor, maControl]}

/return TRUE



|***
 * note: control for minion (pets)
 * use: /minion []
 ***|
#bind control_minion /minion
sub Bind_control_minion(string _type, string _verbage, string _silent, bool _debug)
  DEBUG \atBind_control_minion\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP minion ${_type.Right[1]}
    /return TRUE
  }

  /declare _out string local
  /declare _lsout list local

  | force a minion build/check
  /if (${_type.Equal[build]}) {
    /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
    /return

  } else /if (${_type.Equal[engage]}) {
    /if (!${set_control_num_range[${_debug}, stPctPetEngage, "${_verbage}", 0, 99]}) /return

  | illusion for pet
  } else /if (${_type.Equal[illusion]}) {
    /invoke ${do_raw_edit[FALSE, SILENT, stPetIllusion, "${_verbage}"]}

  | enable pet use
  } else /if (${_type.Equal[pet]}) {
    /invoke ${set_switch_env[${_debug}, swPet, TRUE, ${_verbage}]}

    | turn off pet charming
    /if (${maMinion.Find[swPet].Value} && ${Select[${Me.Class.ShortName},ENC,DRU,MAG,NEC]}) {
      /if (${maMinion.Find[swPetCharm].Value}) {
        /invoke ${set_switch_env[${_debug}, swPetCharm, TRUE, FALSE]}
      }
    }

  | enable charm pet use
  } else /if (${_type.Equal[charm]} && ${Select[${Me.Class.ShortName},ENC,DRU,MAG,NEC]}) {
    /invoke ${set_switch_env[${_debug}, swPetCharm, TRUE, ${_verbage}]}

    | turn off pet charming
    /if (${maMinion.Find[swPetCharm].Value}) {
      /if (${maMinion.Find[swPet].Value}) {
        /invoke ${set_switch_env[${_debug}, swPet, TRUE, FALSE]}
      }
    }

  | enable swarm pets
  } else /if (${_type.Equal[swarm]}) {
    /invoke ${set_switch_env[${_debug}, swSwarm, TRUE, ${_verbage}]}

  | set swarm pet engage
  } else /if (${_type.Equal[sengage]}) {
    /if (!${set_control_num_range[${_debug}, stPctSwarmEngage, "${_verbage}", 0, 99]}) /return

  | use swarm pets on mobs with a DS?
  } else /if (${_type.Equal[ds]}) {
    /invoke ${set_switch_env[${_debug}, swNoSwarmonDS, TRUE, ${_verbage}]}

  | pet gear
  } else /if (${_type.Equal[gear]}) {
    /if (${_verbage.Equal[weapon]}) {
      /call do_raw_edit ${_debug} SILENT stPetWeapon "${_silent}"
    } else /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swUsePetGear, TRUE, ${_silent}]}
    } else /if (${_verbage.Equal[begalias]}) {
      /call do_raw_edit ${_debug} SILENT stBegAlias "${_silent}"
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swUsePetGear]}
    /invoke ${maControl.Add[weapon,stPetWeapon|br]}
    /invoke ${maControl.Add[begalias,stBegAlias|br]}
    /invoke ${set_control_output[${_debug}, maMinion, "minion gear", maControl]}    
    
    /return TRUE


  | send pets with a force command
  } else /if (${_type.Equal[onforce]}) {
    /invoke ${set_switch_env[${_debug}, swonForce, TRUE, ${_verbage}]}

  | buff your pet?
  } else /if (${_type.Equal[buff]}) {
    /invoke ${set_switch_env[${_debug}, swPetBuff, TRUE, ${_verbage}]}

  | pet shrinks
  } else /if (${_type.Equal[shrink]}) {
    /if (${_verbage.Equal[item]}) {
      /call do_raw_edit ${_debug} SILENT stPetShrink "${_silent}"
    } else /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${_debug}, swPetShrink, TRUE, ${_silent}]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swPetShrink]}
    /invoke ${maControl.Add[item,stPetShrink|br]}
    /invoke ${set_control_output[${_debug}, maMinion, "minion shrink", maControl]}
    /return TRUE

  }

  /if (${_silent.Equal[SILENT]}) /return

  /invoke ${maControl.Clear}
  | /invoke ${maControl.Add[type,stPetType|br]}
  /invoke ${maControl.Add[illusion,stPetIllusion|br]}
  /invoke ${maControl.Add[pet,swPet]}
  /if (${Select[${Me.Class.ShortName},ENC,DRU,MAG,NEC]}) {
    /invoke ${maControl.Add[charm,swPetCharm]}
  }
  /invoke ${maControl.Add[buff,swPetBuff]}
  /invoke ${maControl.Add[build,option]}
  /invoke ${maControl.Add[shrink,submenu]}
  /invoke ${maControl.Add[engage,stPctPetEngage]}
  /invoke ${maControl.Add[swarm,swSwarm]}
  /invoke ${maControl.Add[sengage,stPctSwarmEngage]}
  /invoke ${maControl.Add[ds,swNoSwarmonDS]}
  /invoke ${maControl.Add[gear,submenu]}
  /invoke ${maControl.Add[onforce,swonForce]}
  /invoke ${set_control_output[${_debug}, maMinion, minion, maControl]}

/return TRUE



|***
 * note: shortcut to force sub cast in a manual instance
 * use: /entcast [NAME] [current target] DEBUG
 ***|
#bind command_entcast /entcast
sub Bind_command_entcast(string _name, string _target, bool _debug)
  DEBUG \atBind_command_entcast\ax(\a-w${_name}, ${_target}\ax)

  /call cast_data ${_debug} "${_name}"
  /call cast ${_debug} ${_target} FALSE

/return TRUE



|***
 * note: forces an intervention spell on that target
 * use: /advancedpullint [spawn] DEBUG
 ***|
#bind command_pull_int /advancedpullint
sub Bind_command_pull_int(spawn _spawn, bool _debug)
  DEBUG \atBind_command_pull_int\ax(\a-w${_spawn.ID}\ax)

  /if (!${_spawn.ID}) {
    /invoke ${out[9, "${notice}${sep}target required"]}
    /return FALSE
  }
  
  OUT Intervention pull requested for target${sep}\a-w${_spawn.DisplayName}\ax (\a-r${_spawn.ID}\ax)
  /call cast_cycle_heal ${_debug} ${_spawn.ID} interventionpull

/return FALSE



|***
 * nav: forces a nav pull on that target
 * use: /advancedpullnav [targetID] DEBUG
 ***|
#bind command_pull_nav /advancedpullnav
sub Bind_command_pull_nav(spawn _spawn, bool _debug)
  DEBUG \atBind_command_pull_nav\ax(\a-w${_tmpID}\ax)

  /call get_target_navpull ${_debug} ${_spawn.ID}

/return FALSE



|***
 * note: forces a pet pull on that target
 * use: /advancedpullpet [TargetID] DEBUG
 ***|
#bind command_pull_pet /advancedpullpet
sub Bind_command_pull_pet(spawn _spawn, bool _debug)
  DEBUG \atBind_command_pull_pet\ax(\a-w${_tmpID}\ax)

  /call get_target_petpull ${_debug} ${_spawn.ID}

/return FALSE



|***
 * note: control for output granulatiry
 * use: /grain #
 ***|
#bind control_grain /grain
sub Bind_control_grain(string _type, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP grain ${_type.Right[1]}
    /return TRUE
  }

  /declare _count int local 0
  
  /sqlite query "${DBstatic}" dummy SELECT COUNT(env_var) AS data FROM output
  /declare _shadeCount int local ${sqlite.Result[dummy 1 data]}
  
  /if (${_type.Equal[list]}) {
    /sqlite query "${DBstatic}" grainout SELECT value FROM output
    /for _count 1 to ${_shadeCount}
      OUT ${sep} \aw${_count}\ax:${If[${lsGrain.Contains[${_count}]},${on},${off}]}${sep}\a-w${sqlite.Result[grainout ${_count} value]}
    /next _count
    /sqlite clear grainout
    /invoke ${out[18, 0, 33]}
    /return TRUE
  }

  | remove a grain if it exists
  /if (${lsGrain.Contains[${_type}]} && ${Range.Between[1,${_shadeCount}:${_type}]}) {
    /call do_raw_edit ${_debug} silent lsGrain ${_type} delete

  | add a grain if it oes not exist
  } else /if (!${lsGrain.Contains[${_type}]} && ${Range.Between[1,${_shadeCount}:${_type}]}) {
    /call do_raw_edit ${_debug} silent lsGrain ${_type} add

  | idiot proofing
  } else /if (${Bool[${_type}]} && !${Range.Between[1,${_shadeCount}:${_type}]}) {
    /invoke ${out[1, shade, 0]}
  }

  /declare _out string local
  
  /varset _out ${dot} ${out_list}

  /for _count 1 to ${_shadeCount}
    /varset _out ${_out} ${dot} \aw${_count}\ax:${If[${lsGrain.Contains[${_count}]},${on},${off}]}
  /next _count

  /invoke ${out[12, "/grain ${_out}"]}

/return FALSE



|***
 * note: clickitem editing
 * use: /clickitem
 ***|
#bind control_clickitem /clickitem
sub Bind_control_clickitem(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atBind_control_clickitem\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP clickitem ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Item 20 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maItem, clickitem, maControl]}

/return TRUE



|***
 * note: defense editing / control
 * use: /defense
 ***|
#bind control_defense /defense
sub Bind_control_defense(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_defense\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP defense ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Defense 20 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  /if (${_type.Equal[count]}) {
    /if (!${set_control_num_range[${_debug}, stCountDef, "${_verbage}", 0, 99]}) /return

  | pct of health to activate defensive
  } else /if (${_type.Equal[pct]}) {
    /if (!${set_control_num_range[${_debug}, stPctDef, "${_verbage}", 0, 99]}) /return

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[count,stCountDef]}
  /invoke ${maControl.Add[pct,stPctDef]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maDefense, defense, maControl]}

/return TRUE



|***
 * note: control for /ttl
 * use:
 ***|
#bind control_TTL /ttl
sub Bind_control_TTL(string _type, string _verbage, string _silent, bool _debug)
  DEBUG \atBind_control_TTL\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP ttl ${_type.Right[1]}
    /return TRUE
  }

  | set the switch to use
  /if (${_type.Equal[logic]}) {
    /invoke ${set_switch_env[${_debug}, swTTL, TRUE, ${_verbage}]}

  | clear the TTL statiestics
  } else /if (${_type.Equal[clear]}) {
    /sqlite query "${DBcharacter}" dummy SELECT swTTL FROM environment_character
    /invoke ${maTTL.Clear}
    /invoke ${maTTL.Add[swTTL,${sqlite.Result[dummy 1 swTTL]}]}
    /invoke ${maTTL.Add[stTTLTimeStart,0]}
  }

  /if (${_silent.Equal[SILENT]}) /return
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[logic,swTTL]}
  /invoke ${maControl.Add[cleat,option]}
  /invoke ${set_control_output[${_debug}, maTTL, ttl, maControl]}

/return TRUE



|***
 * note:control for /nuke
 * use:
 ***|
#Bind control_nuke /nuke
sub Bind_control_nuke(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_nuke\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP nuke ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Nuke 20 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  | set stop nuke %
  /if (${_type.Equal[pctstop]}) {
    /if (!${set_control_num_range[${_debug}, stPctStopNuke, "${_verbage}", 0, 99]}) /return

  | force a nuke loop till all are casted before exiting
  } else /if (${_type.Equal[loop]}) {
    /invoke ${set_switch_env[FALSE, swNukeLoop, TRUE, ${_verbage}]}

  | force a nuke loop till all are casted before exiting
  } else /if (${_type.Equal[sit]}) {
    /invoke ${set_switch_env[FALSE, swNukeWhackAMole, TRUE, ${_verbage}]}

  | delay in nuking
  } else /if (${_type.Equal[delay]}) {
    /if (!${set_control_num_range[${_debug}, stNukeDelay, "${_verbage}", 0, 100]}) /return
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[pctstop,stPctStopNuke]}
  /invoke ${maControl.Add[delay,stNukeDelay]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[loop,swNukeLoop]}
  /invoke ${maControl.Add[sit,swNukeWhackAMole]}
  /invoke ${set_control_output[${_debug}, maNuke, nuke, maControl]}

/return TRUE



|***
 * note:control for /dot
 * use:
 ***|
#Bind control_dot /dot
sub Bind_control_dot(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_dot\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP dot ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} DoT 20 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  | set stop nuke %
  /if (${_type.Equal[pctstop]}) {
    /if (!${set_control_num_range[${_debug}, stPctStopDoT, "${_verbage}", 0, 99]}) /return

  | set dot refresh time in seconds
  } else /if (${_type.Equal[refresh]}) {
    /if (!${set_control_num_range[${_debug}, stDotRefresh, "${_verbage}", 0, 99]}) /return

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[pctstop,stPctStopDoT]}
  /invoke ${maControl.Add[refresh,stDotRefresh]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maDoT, dot, maControl]}

/return TRUE



|***
 * note:control for /agro
 * use:
 ***|
#Bind control_agro /agro
sub Bind_control_agro(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_agro\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP agro ${_type.Right[1]}
    /return TRUE
  }

  | /if (ENGINE1 && ${Select[${_type},list]}) {
    | /invoke ${out[0, "/agro list is only available with engine 2/3 enabled."]}
    | /return FALSE
  | }

  | /if (ENGINE2 || ENGINE3) {
    /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]}) {
      /call control_base_edit ${maDebug.Find[edit].Value} Agro 20 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
      /return TRUE
    }
  | }

  /if (${_type.Equal[taunt]} && ${lsMeleeSkill.Contains[taunt]}) {
    /if (!${set_control_num_range[${_debug}, stPctTaunt, "${_verbage}", 0, 99]}) /return FALSE

  | hold
  } else /if (${_type.Equal[hold]}) {
    /if (!${set_control_num_range[${_debug}, stPctAgroHold, "${_verbage}", 0, 99]}) /return FALSE

  | agro
  } else /if (${_type.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swAgro, TRUE, ${_verbage}]}
    /call set_AA_agro ${_debug}
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[active,swAgro]}
  /if (${lsMeleeSkill.Contains[taunt]}) {
    /invoke ${maControl.Add[taunt,stPctTaunt]}
    /invoke ${maControl.Add[hold,stPctAgroHold]}
  }
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maAgro, agro, maControl]}

/return TRUE



|***
 * note:control for /miscdps
 * use:
 ***|
#Bind control_miscdps /miscdps
sub Bind_control_miscdps(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_miscdps\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (ENGINE1) {
    /invoke ${out[0, "/miscdps is only available with engine 2/3 enabled."]}
    /return FALSE
  }

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP miscdps ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,50:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} MiscDPS 50 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maMiscDPS, "miscdps", maControl]}

/return TRUE



|***
 * note:control for /miscdps
 * use:
 ***|
#Bind control_song /song
sub Bind_control_song(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_song\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (!${Select[${Me.Class.ShortName},BRD]}) {
    /invoke ${out[0, "You are not a bard.. Might think you are.. but.."]}
    /return FALSE
  }

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP song ${_type.Right[1]}
    /return TRUE
  }
  
  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,20:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Song 20 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maSong, "song", maControl]}

/return TRUE



|***
 * note:control for /sash
 * use:
 ***|
#Bind control_sash /sash
sub Bind_control_sash(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_sash\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP sash ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,12:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Sash 12 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  | procing weapon swaps
  /if (${_type.Equal[proc]}) {
    /if (${_verbage.Equal[active]}) {
      /invoke ${set_switch_env[${maDebug.Find[edit].Value}, swSashProc, TRUE, ${_verbage2}]}
    } else /if (${_verbage.Equal[name]}) {
      /invoke ${do_raw_edit[${maDebug.Find[edit].Value}, SILENT, stSashProc, "${_verbage2}"]}
    }
    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[active,swSashProc]}
    /invoke ${maControl.Add[name,stSashProc|br]}
    /invoke ${set_control_output[${_debug}, maSash, "sash proc", maControl]}
    /return TRUE

  } else /if (${_type.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swSash, TRUE, ${_verbage}]}


  } else /if (${_type.Equal[startwith]}) {
    /invoke ${do_raw_edit[${_debug}, SILENT, stSashStart, "${_verbage}"]}

  | shield flash
  } else /if (${_type.Equal[flash]}) {
    /if (${_verbage.Equal[pct]}) {
      /if (!${set_control_num_range[${_debug}, stPctShieldFlash, "${_verbage2}", 0, 99]}) /return
    } else /if (${_verbage.Equal[count]}) {
      /if (!${set_control_num_range[${_debug}, stCountShieldFlash, "${_verbage2}", 0, 99]}) /return
    | } else /if (${_verbage.Equal[name]}) {
    |   /invoke ${do_raw_edit[${_debug}, SILENT, stSashShieldFlash, "${_verbage2}"]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[pct,stPctShieldFlash]}
    /invoke ${maControl.Add[count,stCountShieldFlash]}
    | /invoke ${maControl.Add[name,stSashShieldFlash|br]}
    /invoke ${set_control_output[${_debug}, maSash, "sash flash", maControl]}
    /return TRUE

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[active,swSash]}
  | /invoke ${maControl.Add[startwith,stSashStart|br]}
  | /invoke ${maControl.Add[proc,submenu]}
  /if (${Select[${Me.Class.ShortName},PAL,WAR,SHD]}) {
    /invoke ${maControl.Add[flash,submenu]}
  }

  /invoke ${set_control_output[${_debug}, maSash, sash, maControl]}

/return TRUE



|***
 * note:control for /cure
 * use: /cure control
 ***|
#Bind control_cure /cure
sub Bind_control_cure(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_cure\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP cure ${_type.Right[1]}
    /return TRUE
  }
  
  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,12:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Cure 12 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  /if (${_type.Equal[self]}) {
    /invoke ${set_switch_env[${_debug}, swCureSelf, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[group]}) {
    /invoke ${set_switch_env[${_debug}, swCureGroup, TRUE, ${_verbage}]}
  } else /if (${_type.Equal[crew]}) {
    /invoke ${set_switch_env[${_debug}, swCureCrew, TRUE, ${_verbage}]}
  }

  | set observers
  /if (${maCure.Find[swCureGroup].Value} || ${maCure.Find[swCureCrew].Value}) {
    /call observer_build ${maDebug.Find[observe].Value} obs_cure add
  } else /if (!${maCure.Find[swCureSelf].Value} && !${maCure.Find[swCureGroup].Value} && !${maCure.Find[swCureCrew].Value}) {
    /call observer_build ${maDebug.Find[observe].Value} obs_cure drop
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[self,swCureSelf]}
  /invoke ${maControl.Add[group,swCureGroup]}
  /invoke ${maControl.Add[crew,swCureCrew]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maCure, cure, maControl]}

/return TRUE



|***
 * note: control for /pred
 * use: predictive combat
 ***|
#bind control_pred /pred
sub Bind_control_pred(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
  DEBUG \atBind_control_TTL\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP pred ${_type.Right[1]}
    /return TRUE
  }

  | set the switch to use
  /if (${_type.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swPred, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[class]}) {
    /if (!${set_control_num_range[${_debug}, stPredCountPrepClass, "${_verbage}", 0, 99]}) /return

  } else /if (${_type.Equal[burn]}) {
    /if (!${set_control_num_range[${_debug}, stPredCountPrepBurn, "${_verbage}", 0, 99]}) /return

  | } else /if (${_type.Equal[delay]}) {
  |   /if (!${set_control_num_range[${_debug}, stPredDelay, "${_verbage}", 1, 99]}) /return

  } else /if (${_type.Equal[state]}) {

    /if (!${lsPlayerstate.Contains[${_verbage}]}) {
      OUT /pred state [\a-w0|1|2|4|8|16|32|64|128\ax]
      /return FALSE
    }

    /invoke ${do_raw_edit[${_debug}, SILENT, stPredPlayerstate, "${_verbage}"]}

    /declare _out string local
    /declare _listate listiterator local
    /vardata _listate lsPlayerstate.First.Clone
    /while (!${_listate.IsEnd}) {
      /varset _out ${_out} ${If[${_listate.Value.Equal[${lsPlayerstate.First}]},,${dot}]} ${If[${maPred.Find[stPredPlayerstate].Value} == ${_listate.Value},${good}${_listate.Value}\ax,${info}${_listate.Value}\ax]}
      /invoke ${_listate.Advance}
    }
    /invoke ${out[12, "/pred state ${dot} ${_out}"]}
    /return TRUE

  | predictive radius
  } else /if (${_type.Equal[rad]}) {
    /if (!${set_control_num_range[${_debug}, stPredRad, "${_verbage}", 1, 80]}) /return

  }

  /if (${_silent.Equal[SILENT]}) /return
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[active,swPred]}
  /invoke ${maControl.Add[rad,stPredRad]}
  /invoke ${maControl.Add[class,stPredCountPrepClass]}
  /invoke ${maControl.Add[burn,stPredCountPrepBurn]}
  /invoke ${maControl.Add[state,stPredPlayerstate|show]}

  /invoke ${set_control_output[${_debug}, maPred, pred, maControl]}

  /call out_list lsPredPullers


/return TRUE


|***
 * note: predictive combat
 * use: predictive combat
 ***|
sub control_pred(bool _debug)
  DEBUG \atcontrol_pred\ax()

  | nothing to check
  /if (!${Bool[${maPred.Find[stPredID].Value}]}) {
    /return FALSE
  }

  | set spawn area ount
  /declare _spawncount ${SpawnCount[loc ${Spawn[${Target.ID}].X} ${Spawn[${Target.ID}].Y} ${Spawn[${Target.ID}].Z} NPC radius ${maPred.Find[stPredRad].Value}]}

  | set burn if area count is met and on
  /if (${maPred.Find[swPredPrepBurn].Value}) {
    /if (${maBurn.Find[stBurnCount].Value}) {
      /if (${_spawncount} >= ${maBurn.Find[stBurnCount].Value}) {
        /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
      }

    | set burn based off name in area of pulled target
    } else /if (${maBurn.Find[swBurnAuto].Value}) {
      /declare _count int local 0
      /declare _mobID int local 0
      /for _count 1 to ${_spawncount}
        /varset _mobID ${NearestSpawn[${_count}, loc ${Spawn[${Target.ID}].X} ${Spawn[${Target.ID}].Y} ${Spawn[${Target.ID}].Z} NPC radius ${maPred.Find[stPredRad].Value}].ID}
        /if (${lsZoneNamed.Contains[${Spawn[ID ${_mobID}].DisplayName}]} && !${maOver.Find[swOverName].Value}) {
          /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
          /return TRUE
        }
      /next _count
    }
  }

  | start class predictions
  /if (${maPred.Find[swPredPrepClass].Value} && ${SubDefined[prediction]}) {
    /call prediction ${maDebug.Find[pred].Value}
  }

/return TRUE



|***
 * note: control for /mercenary
 * use: mercenary controls
 ***|
#bind control_mercenary /mercenary
sub Bind_control_mercenary(string _type, string _verbage, string _silent, bool _debug)
  DEBUG \atBind_control_mercenary\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)
  
  OUT DISABLED FOR NOW....
  /return FALSE

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP mercenary ${_type.Right[1]}
    /return TRUE
  }

  | set the switch to use
  /if (${_type.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swMercenary, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[class]}) {

    | fix user entry
    /if (!${Bool[${_verbage}]} && ${lsClassMercenary.Contains[${Target.Class.ShortName}]}) {
      /call do_raw_edit ${_debug} SILENT stMercenaryClass ${Target.Class.ShortName}
    } else /if (${Bool[${_verbage}]} && ${lsClassMercenary.Contains[${_verbage.Upper}]}) {
      /call do_raw_edit ${_debug} SILENT stMercenaryClass ${_verbage.Upper}
    } else {
      /invoke ${out[0, "\ayusage\ax /mercenary class [\a-wTarget\ax|\a-wWAR\ax|\a-wWIZ\ax|\a-wCLR\ax|\a-wROG\ax]"]}
      /return FALSE
    }

  }

  /if (${_silent.Equal[SILENT]}) /return
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[active,swMercenary]}
  /invoke ${maControl.Add[class,stMercenaryClass|br]}
  /invoke ${set_control_output[${_debug}, maMercenary, mercenary, maControl]}

/return TRUE



|***
 * note: tag editor
 * use:
 ***|
#bind_noparse tag /tag
sub Bind_noparse_tag(string _grouping, string _tagname, string _eval, bool _silent)
  /declare _debug bool local FALSE
  DEBUG \atBind_noparse_tag\ax(\a-w${_grouping}, ${_tagname}, ${Parse[1,${_eval}]}\ax)

  /if (${_grouping.Left[1].Equal[-]}) {
    /lua run HELP tag ${_grouping.Right[1]}
    /return TRUE
  }
  
  /declare _count int local 0

  | delete a tag or a validate
  /if (${_eval.Equal[delete]} || ${_tagname.Equal[delete]}) {

    | delete the tag_validate
    /if (${_eval.Equal[delete]}) {
      DEBUG ${sep}delete tag_validate${sep}\a-w${_grouping}, ${_tagname}\ax

      | see if it is even there?
      /sqlite query "${DBshared}" dummy SELECT * FROM tag_validate LEFT JOIN tag ON tag_validate.tag = tag.id LEFT JOIN tag_group ON tag_validate.grouping = tag_group.id WHERE tag_group.grouping = "${_grouping}" AND tag.name = "${_tagname}" AND tag_validate.created_by <> 1;

      /if (!${sqlite.Rows[dummy]}) {
        OUT Something is invalid${sep}\a-r${_grouping}\ax, \a-r${_tagname}\ax
        /return FALSE
      }

      | delete the bastard
      /sqlite query "${DBshared}" dummy DELETE FROM tag_validate WHERE tag IN (SELECT tag.id FROM tag WHERE tag.name = "${_tagname}") AND grouping = (SELECT tag_group.id FROM tag_group WHERE tag_group.grouping = "${_grouping}") AND created_by <> 1;
      OUT Evaluation Deleted${sep}\aw${_grouping}\ax${sep}\a-t${_tagname}\ax
      /return TRUE

    }

    | delete the tag
    /if (${_tagname.Equal[delete]}) {
      DEBUG ${sep}delete tag${sep}\a-t${_grouping}\ax

      | see if the tag is there
      /sqlite query "${DBshared}" dummy SELECT * FROM tag WHERE tag.name = "${_grouping}" AND created_by <> 1;
      /if (!${sqlite.Rows[dummy]}) {
        OUT Something is invalid${sep}\a-w${_grouping}\ax
        /return FALSE
      }

      | make sure the tag isn't being used somewhere?!
      /sqlite query "${DBshared}" dummy SELECT created_by FROM tag_validate WHERE tag=(SELECT id FROM tag WHERE name="${_grouping}") AND created_by <> 1;

      /if (${sqlite.Rows[dummy]}) {
        OUT Tag is currently attached to an evaluation${sep}\a-w${_grouping}\ax
        OUT Delete all associated evaluations before deleting a tag name.
        /return FALSE
      }

      | delete it, now that we are passed the checks
      /sqlite query "${DBshared}" dummy DELETE FROM tag WHERE tag.name = "${_grouping}" AND created_by <> 1;
      OUT Tag Deleted${sep}\a-t${_grouping}\ax

      /return TRUE
    }
  }

  | help / lists
  /sqlite query "${DBshared}" dummy SELECT grouping FROM tag_group WHERE grouping="${_grouping}";

  /if (!${sqlite.Rows[dummy]} && !${Select[${_grouping},list,see]}) {
    ECHO
    OUT To create a tag with an evaluation${sep}\a-w/tag groupname tagname "evaluation"\ax
    OUT \arDO NOT\ax use all uppercase in tagname or groupname
    OUT To delete an evaluation${sep}\a-w/tag groupname tagname delete\ax
    OUT To delete a tag${sep}\a-w/tag tagname delete\ax
    /declare _out string local
    /sqlite query "${DBshared}" dummy SELECT grouping FROM tag_group;
    /for _count 1 to ${sqlite.Rows[dummy]}
      /varset _out ${_out} ${dot} \a-w${sqlite.Result[dummy ${_count} grouping]}\ax
    /next _count
    OUT Available Groups ${_out}
    /return FALSE
  }


  /if (!${sqlite.Rows[dummy]} || ${Select[${_grouping},list,see]}) {
    /sqlite query "${DBshared}" dummy SELECT tag_group.grouping, tag.name, tag_validate.created_by, tag_validate.eval, tag_validate.aux FROM tag_validate LEFT JOIN tag ON tag_validate.tag = tag.id LEFT JOIN tag_group ON tag_validate.grouping = tag_group.id ORDER BY tag_group.grouping, tag.name
    /if (${Select[${_grouping},list,see]} && !${Bool[${_tagname}]}) {
      /for _count 1 to ${sqlite.Rows[dummy]}
        /if (${Parse[99,${sqlite.Result[dummy ${_count} eval].Find[::]}]}) {
          OUT \aw${sqlite.Result[dummy ${_count} grouping]}\ax${sep}\a-o${sqlite.Result[dummy ${_count} name]\ax${sep}\a-w${Parse[2,${sqlite.Result[dummy ${_count} aux]}]}\ax
        } else {
          OUT \aw${sqlite.Result[dummy ${_count} grouping]}\ax${sep}${If[${sqlite.Result[dummy ${_count} created_by].Equal[1]},\a-g${sqlite.Result[dummy ${_count} name]}\ax,\at${sqlite.Result[dummy ${_count} name]}\ax]}${sep}\a-w${Parse[2,${sqlite.Result[dummy ${_count} eval]}]}\ax
        }
      /next _count
      /invoke ${out[18, 0, 33]}
      /return FALSE

    } else /if (${Select[${_grouping},list]} && ${Range.Between[1,2:${Int[${_tagname}]}]})  {
      /for _count 1 to ${sqlite.Rows[dummy]}
        /if (${sqlite.Result[dummy ${_count} created_by].Equal[${_tagname}]}) {
          OUT \aw${sqlite.Result[dummy ${_count} grouping]}\ax${sep}${If[${sqlite.Result[dummy ${_count} created_by].Equal[1]},\a-g${sqlite.Result[dummy ${_count} name]}\ax,\at${sqlite.Result[dummy ${_count} name]}\ax]}${sep}\a-w${Parse[2,${sqlite.Result[dummy ${_count} eval]}]}\ax
        }
      /next _count
      /invoke ${out[18, 0, 33]}
      /return FALSE

    } else /if (${Select[${_grouping},list]} && ${_tagname.Equal[marker]})  {
      /for _count 1 to ${sqlite.Rows[dummy]}
        /if (${Parse[99,${sqlite.Result[dummy ${_count} eval].Find[::MARKER]}]}) {
          OUT \aw${sqlite.Result[dummy ${_count} grouping]}\ax${sep}${If[${sqlite.Result[dummy ${_count} created_by].Equal[1]},\a-o${sqlite.Result[dummy ${_count} name]}\ax,\at${sqlite.Result[dummy ${_count} name]}\ax]}${sep}\a-w${sqlite.Result[dummy ${_count} aux]}\ax
        }
      /next _count
      /invoke ${out[18, 0, 33]}
      /return FALSE

    } else /if (${Select[${_grouping},list,see]} && ${Bool[${_tagname}]}) {
      /for _count 1 to ${sqlite.Rows[dummy]}
        /if (${sqlite.Result[dummy ${_count} grouping].NotEqual[${_tagname}]}) /continue

        /if (${Parse[99,${sqlite.Result[dummy ${_count} eval].Find[::]}]}) {
          OUT \aw${sqlite.Result[dummy ${_count} grouping]}\ax${sep}\a-o${Parse[2,${sqlite.Result[dummy ${_count} name]}]}\ax${sep}\a-w${Parse[2,${sqlite.Result[dummy ${_count} aux]}]}\ax
        } else {
          OUT \aw${sqlite.Result[dummy ${_count} grouping]}\ax${sep}${If[${sqlite.Result[dummy ${_count} created_by].Equal[1]},\a-g${sqlite.Result[dummy ${_count} name]}\ax,\at${sqlite.Result[dummy ${_count} name]}\ax]}${sep}\a-w${Parse[2,${sqlite.Result[dummy ${_count} eval]}]}\ax
        }
      /next _count
      /invoke ${out[18, 0, 33]}
      /return FALSE
    }
    /return FALSE
  }

  | check the tag first. do not let them make tags that already exist
  /sqlite query "${DBshared}" dummy SELECT name, created_by FROM tag WHERE created_by = '1' AND name = '${_tagname}';

  | create the tag if it does not exist
  /sqlite query "${DBshared}" dummy INSERT INTO tag (name, created_by) VALUES ("${_tagname}", 2);

  | check if what they want to do is trying to overwrite the defaults?
  /sqlite query "${DBshared}" dummy SELECT created_by, eval FROM tag_validate WHERE grouping=(SELECT id FROM tag_group WHERE grouping='${_grouping}') AND tag=(SELECT id FROM tag WHERE name='${_tagname}') AND created_by = '1';
  /if (!${sql_check[FALSE, dummy, Bind_noparse_tag, "SELECT (created_by)"]}) /endmacro

  /if (${sqlite.Rows[dummy]}) {
    /if (${Select[${sqlite.Result[dummy 1 created_by]},1]}) {
      OUT Tag Exists${sep}\aw${_grouping}\ax${sep}\ag${_tagname}\ax (\arDo not edit default tags\ax)
      /return FALSE
    }
  }

  | see if what we are trying to make is there?
  /sqlite query "${DBshared}" dummy SELECT created_by, eval FROM tag_validate WHERE grouping=(SELECT id FROM tag_group WHERE grouping='${_grouping}') AND tag=(SELECT id FROM tag WHERE name='${_tagname}') AND created_by = '2';

  | if it is there, we update the current one
  /if (${sqlite.Rows[dummy]}) {
    | tag exists, update
    /sqlite query "${DBshared}" dummy UPDATE tag_validate SET eval="${Parse[1,${_eval}]}" WHERE grouping=(SELECT id FROM tag_group WHERE grouping="${_grouping}") AND tag=(SELECT id FROM tag WHERE name="${_tagname}");
    /if (!${sql_check[${_debug}, dummy, Bind_noparse_tag, "UPDATE (tag_validate)"]}) /endmacro

    /sqlite query "${DBshared}" dummy SELECT created_by, eval FROM tag_validate WHERE grouping=(SELECT id FROM tag_group WHERE grouping='${_grouping}') AND tag=(SELECT id FROM tag WHERE name='${_tagname}') AND created_by = '2';

    | clear it from memory so it will reget the new version
    /sqlite clear q${_grouping.Left[1].Upper}${_grouping.Right[-1].Lower}_${_tagname}

    | /if (!${_silent}) {
      OUT Evaluation Updated${sep}\aw${_grouping}\ax${sep}${If[${sqlite.Result[dummy 1 created_by].Equal[1]},\aw${_tagname}\ax,\at${_tagname}\ax]}${sep}\a-w${Parse[1,${_eval}]}\ax
      DEBUG ${sep}clear q${_grouping.Left[1].Upper}${_grouping.Right[-1].Lower}_${_tagname}
      /sqlite clear q${_grouping.Left[1].Upper}${_grouping.Right[-1].Lower}_${_tagname}
    | }
    /return TRUE

  | if its not there, we create/insert a new on
  } else /if (!${sqlite.Rows[dummy]}) {
    | insert
    /sqlite query "${DBshared}" dummy INSERT INTO tag_validate (tag, grouping, eval, created_by) SELECT tag.id, tag_group.id, "${Parse[1,${_eval}]}", 2 FROM tag CROSS JOIN tag_group WHERE tag.name="${_tagname}" AND tag_group.grouping="${_grouping}" LIMIT 1;
    /if (!${sql_check[${_debug}, dummy, Bind_noparse_tag, "INSERT INTO (tag_validate)"]}) /endmacro
    | check it
    /sqlite query "${DBshared}" dummy SELECT created_by, eval FROM tag_validate WHERE grouping=(SELECT id FROM tag_group WHERE grouping='${_grouping}') AND tag=(SELECT id FROM tag WHERE name='${_tagname}') AND created_by = '2';
    /if (${sqlite.Rows[dummy]})  {
    | /if (${sqlite.Rows[dummy]} && !${_silent})  {
      OUT Tag and Evaluation Created${sep}\aw${_grouping}\ax${sep}\aw${If[${sqlite.Result[dummy 1 created_by].Equal[2]},\at${_tagname}\ax,\aw${_tagname}\ax]}\ax${sep}\a-w${Parse[1,${_eval}]}\ax
    }

  }

/return TRUE



|***
 * note: gttfo editing / control
 * use: /gtfo
 ***|
#bind control_gtfo /gtfo
sub Bind_control_gtfo(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_gtfo\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)


  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP gtfo ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_type},force]}) {
    OUT Getting out of here.
    /call check_gtfo FALSE ${_debug}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,12:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} GTFO 12 "${_type}" "${_verbage}" "${_verbage2}" ${_add_delete}
    /return TRUE
  }

  /if (${_type.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swGTFOActive, TRUE, ${_verbage}]}

  | number of group dead
  } else /if (${_type.Equal[dead]}) {
    /if (!${set_control_num_range[${_debug}, stCountGTFODead, "${_verbage}", 0, 5]}) /return

  | number of group dead
  } else /if (${_type.Equal[spawns]}) {
    /if (!${set_control_num_range[${_debug}, stCountGTFOSpawns, "${_verbage}", 0, 99]}) /return

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[force,option]}
  /invoke ${maControl.Add[active,swGTFOActive]}
  /invoke ${maControl.Add[dead,stCountGTFODead]}
  /invoke ${maControl.Add[spawns,stCountGTFOSpawns]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[${_debug}, maGTFO, gtfo, maControl]}

/return TRUE



|***
 * note: pulling controls
 * use: /pull [no, dont pull that]
 ***|
#bind control_pull /pull
sub Bind_control_pull(string _type, string _verbage, string _verbage2, string _silent)
  /declare _debug bool local FALSE
  DEBUG \atBind_control_pulll\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP pull ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,12:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Pull 12 "${_type}" "${_verbage}" "${_verbage2}" ${_silent}
    /return TRUE
  }

  | turn pull on or off
  /if (${_type.Equal[active]}) {
    /invoke ${set_switch_env[${_debug}, swPull, TRUE, ${_verbage}]}

  | force intervention pull
  } else /if (${_type.Equal[one]}) {
    
    | get us a target
    /if (${Target.ID}) {
      /vardata targetData Target.ID
    } else {
      /call pull_mob_find FALSE
      /if (${targetData.ID}) {
        /invoke ${out[9, "No Targets in${sep}\a-wPULLENV\ax"]}
        /return
      }
    }
    
    | intervention pulling
    /if (${_verbage.Equal[int]}) {
      /call get_resident ${_debug} |clr|shm 200 notme
      /declare _name string local ${Macro.Return}
      /if (${Bool[${_name}]}) {
        /invoke ${out[9, "Pull Intervention${sep}\a-w${Macro.Return.Lower}\ax ${submenu} \a-w${targetData.DisplayName}\ax"]} 
        /dexecute ${_name} /advancedpullint ${Me.ID}
      } else {
        /invoke ${out[9, "NO Intervention pullers in range (\a-rclr|shm\ax)."]}
      }
      /return TRUE

    | pet pulling
    } else /if (${_verbage.Equal[pet]}) {
      /call get_resident ${_debug} |mag|bst|nec|enc|shm|dru ENV notme
      /if (${Bool[${Macro.Return}]}) {
        /dex ${Macro.Return} /advancedpullnpet TARGET
      } else {
        /invoke ${out[9, "NO pet pullers in range (\a-rmag|bst|nec|enc|shm|dru\ax)."]}
      }

    | nav pulling
    } else /if (${_verbage.Equal[nav]}) {
      /call get_resident ${_debug} |tank|melee ENV notme
      /if (${Bool[${Macro.Return}]}) {
        /dex ${Macro.Return} /advancedpullnav TARGET
      } else {
        /invoke ${out[9, "NO melee nav pullers in range (\a-rtank|melee\ax)."]}
      }

    | lul pulling
    } else /if (${_verbage.Equal[lul]}) {
      | to be built still
    }    
    
    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[nav,option]}
    /invoke ${maControl.Add[pet,option]}
    /invoke ${maControl.Add[int,option]}
    /invoke ${set_control_output[${_debug}, maPull, "pull one", maControl]}
    /return TRUE    
    
  | pull radius
  } else /if (${_type.Equal[rad]}) {
    /invoke ${do_raw_edit[${_debug}, SILENT, stPullRadius, "${_verbage}"]}

  | pull arc
  } else /if (${_type.Equal[arc]}) {
    DEBUG ${sep}pull arc${sep}\a-w${_verbage}\ax

    /if (${_verbage.Equal[reset]}) {
      DEBUG ${sep}pull arc${sep}\a-wclear\ax
      /invoke ${maPull.Add[stPullArcWidth,0]}
      /varset stPullArcLeft 0
      /varset stPullArcRight 0
      /varset stPullArcCenter 0
      /return TRUE
    }

    /if (${Range.Between[0,360:${_verbage}]}) {
      /invoke ${maPull.Add[stPullArcWidth,${_verbage}]}
    } else {
      /invoke ${maPull.Add[stPullArcWidth,0]}
      /return FALSE
    }

    /varset stPullArcCenter ${Me.Heading.Degrees}

    | left
    /if (${Math.Calc[${stPullArcCenter}-(${_verbage}*.5)]} < 0) {
      /varcalc stPullArcLeft 360-((${_verbage}*.5)-${_face})
    } else {
      /varcalc stPullArcLeft ${stPullArcCenter}-(${_verbage}*.5)
    }

    | right
    /if (${Math.Calc[${stPullArcCenter} + (${_verbage}*.5)]} > 360) {
      /varcalc stPullArcRight ((${_verbage}*.5) + ${stPullArcCenter} - 360)
    } else {
      /varcalc stPullArcRight ${stPullArcCenter}+(${_verbage}*.5)
    }

  | pull zradius
  } else /if (${_type.Equal[zrad]}) {
    /invoke ${do_raw_edit[${_debug}, SILENT, stPullZRadius, "${_verbage}"]}

  | nav stop distance
  } else /if (${_type.Equal[navstop]}) {
    /if (!${set_control_num_range[${_debug}, stPullNavStopDistance, "${_verbage}", 31, 250]}) /return

  | nav % variance
  } else /if (${_type.Equal[navvar]}) {
    /if (!${set_control_num_range[${_debug}, stPullNavVariance, "${_verbage}", 0, 99]}) /return

  | radius to watch for pets  
  } else /if (${_type.Equal[petwatch]}) {
    /if (!${set_control_num_range[${_debug}, stPullPetWatch, "${_verbage}", 0, 60]}) /return

  | chain pull #
  } else /if (${_type.Equal[chain]}) {
    /if (!${set_control_num_range[${_debug}, stCountChainPull, "${_verbage}", 0, 99]}) /return


  | enable/disable heal check
  } else /if (${_type.Equal[healcheck]}) {
    /invoke ${set_switch_env[${_debug}, swPullHealCheck, TRUE, ${_verbage}]}

  | outrun range
  } else /if (${_type.Equal[outrun]}) {
    /if (!${set_control_num_range[${_debug}, stPullOutrunRange, "${_verbage}", 0, 1000]}) /return

  | enable/disable setting home marker when pulling
  } else /if (${_type.Equal[sethome]}) {
    /invoke ${set_switch_env[${_debug}, swPullSetHome, TRUE, ${_verbage}]}

  | enable/disable using nav pull length to find mobs
  } else /if (${_type.Equal[pathlogic]}) {
    /invoke ${set_switch_env[${_debug}, swNavPathLogic, TRUE, ${_verbage}]}

  | enable/disable using step calculations to find valid mobs
  } else /if (${_type.Equal[step]}) {
    /if (!${set_control_num_range[${_debug}, stPullStep, "${_verbage}", 0, 10]}) /return
    


  | pull modes
  } else /if (${_type.Equal[mode]}) {
    | pet pulling
    /if (${_verbage.Equal[pet]}) {
      /if (!${Select[${Me.Class.ShortName},MAG,ENC,DRU,SHM,SHD,NEC,BST]}) {
        /invoke ${out[1, shade, 0]}
        /return TRUE
      }
      /call do_raw_edit ${_debug} SILENT stPullMode "pet"
      
    | others?
    } else /if (${Select[${_verbage},nav,multi,base,watch]}) {
      /call do_raw_edit ${_debug} SILENT stPullMode "${_verbage.Lower}"
    | calm pulling
    } else /if (${_verbage.Equal[calm]}) {
      OUT Non functional. go away...
      | /call do_raw_edit ${_debug} SILENT stPullMode "calm"
      /return FALSE
    }

    /declare _liP listiterator local
    /declare _out string local
    /vardata _liP lsPullMode.First.Clone
    /while (!${_liP.IsEnd}) {
      /varset _out ${_out} ${If[${_liP.Value.Equal[${lsPullMode.First}]},,${dot}]} ${If[${_liP.Value.Equal[${maPull.Find[stPullMode].Value}]},${num}${_liP.Value}\ax,${info}${_liP.Value}\ax]}
      /invoke ${_liP.Advance}
    }
    /invoke ${out[12, "/pull mode ${dot} ${_out}"]}
    /return TRUE

  | pull stop reasons
  } else /if (${_type.Equal[stop]}) {


    | hard stop duration
    /if (${_verbage.Equal[time]}) {
      /if (!${set_control_num_range[${_debug}, stHardStopDuration, "${_verbage2}", 0, 6000]}) /return

    | hard stop pct
    } else /if (${_verbage.Equal[pct]}) {
      /if (!${set_control_num_range[${_debug}, stPctHardStop, "${_verbage2}", 0, 99]}) /return

    | hard stop pct
    } else /if (${_verbage.Equal[rez]}) {
      /invoke ${set_switch_env[${_debug}, swHardStopRez, TRUE, ${_verbage2}]}
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[time,stHardStopDuration]}
    /invoke ${maControl.Add[pct,stPctHardStop]}
    /invoke ${maControl.Add[rez,swHardStopRez]}
    /invoke ${set_control_output[${_debug}, maPull, "pull stop", maControl]}
    /return TRUE

  | enable/disable needing LoS for nav pulling
  } else /if (${_type.Equal[navlos]}) {
    /invoke ${set_switch_env[${_debug}, swPullNavLoS, TRUE, ${_verbage}]}

  | pull modes
  } else /if (${_type.Equal[with]}) {

    | range
    /if (${_verbage.Equal[range]}) {
      /call do_raw_edit ${_debug} SILENT stPullWith "range"
    | melee
    } else /if (${_verbage.Equal[melee]}) {
      /call do_raw_edit ${_debug} SILENT stPullWith "melee"
    | other
    } else /if (${_verbage.Equal[other]}) {
      /call do_raw_edit ${_debug} SILENT stPullWith "other"
    } else /if (${_verbage.Equal[bandolier]}) {
      /call do_raw_edit ${_debug} SILENT stRangeBandolier "${_verbage2}"

    }

    /declare _liP listiterator local
    /declare _out string local
    /vardata _liP lsPullWith.First.Clone
    /while (!${_liP.IsEnd}) {
      /varset _out ${_out} ${If[${_liP.Value.Equal[${lsPullWith.First}]},,${dot}]} ${If[${_liP.Value.Equal[${maPull.Find[stPullWith].Value}]},${num}${_liP.Value}\ax,${info}${_liP.Value}\ax]}
      /invoke ${_liP.Advance}
    }
    /varset _out ${_out} ${dot} ${info}bandolier\ax:[${If[${Bool[${maPull.Find[stRangeBandolier].Value}]},${num}${maPull.Find[stRangeBandolier].Value}\ax,${off}]}]
    /invoke ${out[12, "/pull with ${dot} ${_out}"]}
    /return TRUE

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[active,swPull]}
  | /invoke ${maControl.Add[arc,stPullArcWidth|pr]}
  /invoke ${maControl.Add[rad,stPullRadius]}
  /invoke ${maControl.Add[navlos,swPullNavLoS]}
  /invoke ${maControl.Add[zrad,stPullZRadius]}
  /invoke ${maControl.Add[pathlogic,swNavPathLogic]}
  /invoke ${maControl.Add[step,stPullStep]}
  /invoke ${maControl.Add[one,submenu]}
  /invoke ${maControl.Add[stop,submenu]}
  /invoke ${maControl.Add[mode,submenu]}
  /invoke ${maControl.Add[petwatch,stPullPetWatch]}
  /invoke ${maControl.Add[healcheck,swPullHealCheck]}
  /invoke ${maControl.Add[with,submenu]}
  /invoke ${maControl.Add[outrun,stPullOutrunRange]}
  /invoke ${maControl.Add[sethome,swPullSetHome]}
  /invoke ${maControl.Add[navstop,stPullNavStopDistance]}
  /invoke ${maControl.Add[navvar,stPullNavVariance]}
  /invoke ${maControl.Add[chain,stCountChainPull]}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[FALSE, maPull, "pull", maControl]}

/return TRUE



|***
 * note: crew controls
 * use: /crew
 ***|
#bind control_crew /crew
sub Bind_control_crew(string _type, string _verbage, string _verbage2, string _verbage3, string _verbage4, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP crew ${_type.Right[1]}
    /return TRUE
  }

  /declare _count int local 0

  DEBUG ${sep}_type${sep}\a-w${_type}\ax
  DEBUG ${sep}_verbage${sep}\a-w${_verbage}\ax
  DEBUG ${sep}_verbage2${sep}\a-w${_verbage2}\ax
  DEBUG ${sep}_verbage3${sep}\a-w${_verbage3}\ax
  DEBUG ${sep}_verbage4${sep}\a-w${_verbage4}\ax

  /if (${_type.Equal[add]}) {
    
    | add a character
    /if (${_verbage.Equal[character]} && ${Bool[${_verbage2}]}) {
  
      | fix the character name case
      /varset _verbage2 ${proper_case[name, "${_verbage2}"]}
      
      | add default current server it not added
      /if (!${Bool[${_verbage3}]}) {
        /varset _verbage3 ${MacroQuest.Server.Replace[ ,_].Lower}
      }
      
      /sqlite query "${DBshared}" dummy SELECT * FROM characters WHERE charactername='${_verbage2}' AND server='${_verbage3}';
      /if (!${sqlite.Rows[dummy]}) {
        /sqlite query "${DBshared}" dummy INSERT INTO characters(charactername, server) VALUES ("${_verbage2}", "${_verbage3}");

        /if (${sqlite.Resultcode[dummy]} == 0) {  
          OUT /crew add character${sep}\a-w${_verbage3} ${_verbage2}\ax (\a-r${sqlite.Status[dummy]}\ax)
          /return TRUE
        } else {
          OUT /crew add character${sep}\a-w${_verbage3} ${_verbage2}\ax (\a-rFail\ax)
          /return FALSE
        }
  
      } else {
        OUT /crew add character${sep}\a-w${_verbage3} ${_verbage2}\ax (\a-rExists\ax)
        /return TRUE
      }
    
    | add a crew name
    } else /if (${_verbage.Equal[crewname]} && ${Bool[${_verbage2}]}) {
      /sqlite query "${DBshared}" dummy SELECT * FROM crews WHERE crewname='${_verbage2}';
      /if (!${sqlite.Rows[dummy]}) {
        /sqlite query "${DBshared}" dummy INSERT INTO crews(crewname) VALUES ("${_verbage2}")
      
        /if (${sqlite.Resultcode[dummy]} == 0) {  
          OUT /crew add crewname${sep}\a-w${_verbage2}\ax (\a-r${sqlite.Status[dummy]}\ax)
          /return TRUE
        } else {
          OUT /crew add crewname${sep}\a-w${_verbage2}\ax (\a-rFail\ax)
          /return FALSE
        }
      } else {
        OUT /crew add crewname${sep}\a-w${_verbage2}\ax (\a-rExists\ax)
        /return TRUE
      }
      
    | add toon to crew  
    } else /if (${_verbage.Equal[member]} && ${Bool[${_verbage2}]} && ${Bool[${_verbage3}]}) {

      | fix the character name case
      /varset _verbage2 ${proper_case[name, "${_verbage2}"]}

      | character exist in DB?
      /if (!${crew_check_character[${_debug}, "${_verbage2}", FALSE, "add member"]}) {
        /return FALSE
      }

      | crew exist?
      /sqlite query "${DBshared}" qcrewname SELECT id FROM crews WHERE crewname='${_verbage3}';
      /if (!${sqlite.Rows[qcrewname]}) {
        OUT /crew add member${sep}\a-w${_verbage2} ${_verbage3}\ax (\a-rFail\ax) (\a-rInvalid Crewname\ax)
        /return FALSE
      }
 
      | member in the crew already?
      /sqlite query "${DBshared}" increw SELECT crewmembers.id FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE characters.charactername = '${_verbage2}' AND crews.crewname = '${_verbage3}';
      DEBUG ${sep}.Rows (\a-rincrew\ax)${sep}\a-w${sqlite.Rows[increw]}\ax

      /if (${sqlite.Rows[increw]}) {
        OUT /crew add member${sep}\a-w${_verbage2} ${_verbage3}\ax (\a-rExists\ax)
        /return FALSE
      } 

      | get group count
      /sqlite query "${DBshared}" qmembercount SELECT COUNT(crewmembers.id) AS data FROM crewmembers LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage3}';
      DEBUG ${sep}COUNT(crewmembers.id)${sep}\a-w${sqlite.Result[qmembercount 1 data]}\ax
      /if (${sqlite.Result[qmembercount 1 data]} >= 6) {
        OUT /crew add member${sep}\a-w${_verbage2} ${_verbage3}\ax (\a-rFail\ax) (\a-r${sqlite.Result[qmembercount 1 data]}\ax/\a-r6\ax)
        /return FALSE
      } 
      
      | passed all error checking; add the toon to the crew
      /sqlite query "${DBshared}" qcrewname INSERT INTO crewmembers(crewid, characterid) VALUES (${sqlite.Result[qcrewname 1 id]}, ${sqlite.Result[qucheckcharacter 1 id]});
      /if (${sqlite.Resultcode[qcrewname]} == 0) {  
        OUT /crew add member${sep}\a-w${_verbage2} ${_verbage3}\ax (\a-r${sqlite.Status[qcrewname]}\ax)
        /sqlite clear qcrewname
        /return TRUE
      } else {
        OUT /crew add member${sep}\a-w${_verbage2} ${_verbage3}\ax (\a-rFail\ax)
        /return FALSE
      }  
      /return TRUE
      
    | add toon to a roll (with butter for knightly)
    } else /if (${_verbage.Equal[role]} && ${Bool[${_verbage2}]} && ${Bool[${_verbage3}]} && ${Bool[${_verbage4}]}) {
      /varset _verbage2 ${proper_case[name, "${_verbage2}"]}

      | character exist in DB?
      /if (!${crew_check_character[${_debug}, "${_verbage2}", FALSE, "add role"]}) {
        /return FALSE
      } 
      
      | fix the role case
      /varset _verbage3 ${proper_case[role, "${_verbage3}"]}
      
      /sqlite query "${DBshared}" dummy SELECT name FROM roles WHERE name = '${_verbage3}'
      /if (!${sqlite.Rows[dummy]}) {
        /sqlite query "${DBshared}" dummy SELECT name FROM roles;
        OUT Valid roles${sep}
        /for _count 1 to ${sqlite.Rows[dummy]}
          OUT ${sep}\a-w${sqlite.Result[dummy ${_count} name]}\ax
        /next _count 
        /invoke ${out[18, 0, 33]}
        /sqlite clear temp
        /return FALSE
      }


      | crew exist?
      /sqlite query "${DBshared}" qucrewname SELECT id FROM crews WHERE crewname='${_verbage4}';
      /if (!${sqlite.Rows[qucrewname]}) {
        OUT Crewname not in DB${sep}\a-w${_verbage4}\ax
      }

      | check if toon has that role for that crew
      /sqlite query "${DBshared}" quhasrole SELECT crewmemberroles.id FROM crewmemberroles WHERE crewmemberroles.role = '${_verbage3}' AND crewmemberroles.crewmemberid IN (SELECT crewmembers.id FROM crewmembers LEFT JOIN characters on characters.id = crewmembers.characterid WHERE characters.charactername = '${_verbage2}');
      DEBUG ${sep}.Rows (\a-rquhasrole\ax)${sep}\a-w${sqlite.Rows[quhasrole]}\ax
      /if (${sqlite.Rows[quhasrole]}) {
        OUT Character${sep}\a-w${_verbage2}\ax has role${sep}\a-w${_verbage3}\ax for crew${sep}\a-w${_verbage4}\ax
        /return FALSE
      }

      | find the id of the toon in its crew
      /sqlite query "${DBshared}" qucrewmemberid SELECT crewmembers.id FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE characters.charactername = '${_verbage2}' AND characters.server = '${MacroQuest.Server.Replace[ ,_].Lower}' AND crews.crewname = '${_verbage4}';
      DEBUG ${sep}.Result (\a-rqucrewmemberid\ax)${sep}\a-w${sqlite.Result[qucrewmemberid 1 id]}\ax
      
      | add the toon as that role in crewmemberroles
      /sqlite query "${DBshared}" dummy INSERT INTO crewmemberroles(crewmemberid, role) VALUES (${sqlite.Result[qucrewmemberid 1 id]}, '${_verbage3}');
      DEBUG ${sep}.Result (\a-rqdummy\ax)${sep}\a-w${sqlite.Rows[dummy]}\ax
      
      /if (${sqlite.Resultcode[dummy]} == 0) {
        OUT /crew add role${sep}\a-w${_verbage2} ${_verbage3} ${_verbage4}\ax (\a-r${sqlite.Status[dummy]}\ax)
        /sqlite clear qcrewname
        /return TRUE
      } else {
        OUT Character${sep}\a-w${_verbage2}\ax NOT added as role${sep}\a-w${_verbage3}\aax. Something bad happened.
      }

    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[character,out]}
    /invoke ${maControl.Add[crewname,out]}
    /invoke ${maControl.Add[member,out]}
    /invoke ${maControl.Add[role,out]}
    /invoke ${set_control_output[FALSE, maChr, "crew add", maControl]}
    /return FALSE

  | list some stuff
  } else /if (${_type.Equal[list]}) {
    
    | list members in a crew
    /if (${Bool[${_verbage}]}) {
  
      /sqlite query "${DBshared}" increw SELECT characters.charactername FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage}'
      /sqlite query "${DBshared}" iscrew SELECT id FROM crews WHERE crewname = '${_verbage}'
      /sqlite query "${DBshared}" dummy SELECT COUNT(crewmembers.id) AS data FROM crewmembers LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage}';
      ECHO
      OUT Crewname List${sep}\a-w${_verbage}\ax ${If[${sqlite.Rows[iscrew]},(\a-r${sqlite.Result[dummy 1 data]}\ax/\a-r6\ax),(\a-rFail\ax) (\a-rInvalid Crewname\ax)]}   
      /if (!${sqlite.Rows[increw]}) {
        /return FALSE
      }
      OUT Members${sep}
      /if (${sqlite.Rows[increw]}) {
        /for _count 1 to ${sqlite.Rows[increw]}
          OUT ${sep}${sqlite.Result[increw ${_count} charactername]}
        /next _count
      }

      OUT Roles${sep}
      /sqlite query "${DBshared}" dummy SELECT characters.charactername, crewmemberroles.role FROM crewmemberroles LEFT JOIN crewmembers ON crewmemberroles.crewmemberid = crewmembers.id LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage}'
      /if (${sqlite.Rows[dummy]}) {
        /for _count 1 to ${sqlite.Rows[dummy]}
          OUT ${sep}${sqlite.Result[dummy ${_count} role]}${sep}\a-w${sqlite.Result[dummy ${_count} charactername]}\ax
        /next _count
      }

      
      /invoke ${out[18, 0, 33]}
      /sqlite clear increw
      /return FALSE     
    
    | list crews
    } else /if (!${Bool[${_verbage}]}) {

      ECHO
      OUT Crewname List${sep}\a-wall\ax
      
       /sqlite query "${DBshared}" crewlist SELECT crewname FROM crews
       /if (!${sqlite.Rows[crewlist]}) {
        /invoke ${out[18, 0, 33]}
        /sqlite clear increw
        /return FALSE
      }
      
      /for _count 1 to ${sqlite.Rows[crewlist]}
      /sqlite query "${DBshared}" dummy SELECT COUNT(crewmembers.id) AS data FROM crewmembers LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${sqlite.Result[crewlist ${_count} crewname]}';
      OUT ${sqlite.Result[crewlist ${_count} crewname]} (\a-r${sqlite.Result[dummy 1 data]}\ax/\a-r6\ax)
      /next _count
      /invoke ${out[18, 0, 33]}
      /sqlite clear increw
      /sqlite clear iscrew
      /return FALSE

    }
    
    
  | build your current group design
  } else /if (${_type.Equal[buildas]}) {
    
    /if (!${Bool[${_verbage}]}) {
      /invoke ${out[1, shade, 0]}
      /return FALSE
    }
    
    OUT ${notice}${sep}/crew buildas  - will only build a NEW group from your current group design.
    
    /if (!${Group.Members}) { 
      OUT Try being in a group for fucks sake.
      /return FALSE
    }

    | crew exist?
    /sqlite query "${DBshared}" qcrewname SELECT id FROM crews WHERE crewname='${_verbage}';
    /if (${sqlite.Rows[qcrewname]}) {
      OUT /crew buildas${sep}\a-w${_verbage}\ax (\a-rFail\ax) (\a-rCrewname Exists\ax)
      /sqlite clear qcrewname
      /return FALSE
    }
 
    /declare _count2 int local 0
 
    | make the crewname
    /sqlite query "${DBshared}" dummy INSERT INTO crews(crewname) VALUES ("${_verbage}")
    OUT /crew buildas${sep}\a-w${_verbage}\ax (\a-r${sqlite.Status[dummy]}\ax)
 
    /for _count 0 to ${Group.Members}
      DEBUG _count${sep}${_count}${sep}\a-w${Group.Member[${_count}]}\ax
      
      | add to character
      /sqlite query "${DBshared}" dummy SELECT * FROM characters WHERE charactername='${Group.Member[${_count}]}' AND server='${MacroQuest.Server.Replace[ ,_].Lower}';
      /if (!${sqlite.Rows[dummy]}) {
        /sqlite query "${DBshared}" dummy INSERT INTO characters(charactername, server) VALUES ("${Group.Member[${_count}]}", "${MacroQuest.Server.Replace[ ,_].Lower}");
        /if (${sqlite.Resultcode[dummy]} == 0) {  
          OUT /crew buildas${sep}\a-w${MacroQuest.Server.Replace[ ,_].Lower} ${Group.Member[${_count}]}\ax (\a-r${sqlite.Status[dummy]}\ax)
        }
      }
      
      | character exist in DB?
      /sqlite query "${DBshared}" qucheckcharacter SELECT id FROM characters WHERE charactername='${Group.Member[${_count}]}';
      /sqlite query "${DBshared}" qucrewname SELECT id FROM crews WHERE crewname='${_verbage}';

      | passed all error checking; add the toon to the crew
      /sqlite query "${DBshared}" qcrewname INSERT INTO crewmembers(crewid, characterid) VALUES (${sqlite.Result[qucrewname 1 id]}, ${sqlite.Result[qucheckcharacter 1 id]});
      OUT /crew buildas${sep}\a-w${Group.Member[${_count}]} ${_verbage}\ax (\a-r${sqlite.Status[qcrewname]}\ax)
   
      | add roles
      /sqlite query "${DBshared}" qucrewmemberid SELECT crewmembers.id FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE characters.charactername = '${Group.Member[${_count}]}' AND characters.server = '${MacroQuest.Server.Replace[ ,_].Lower}' AND crews.crewname = '${_verbage}';
      DEBUG ${sep}.Result (\a-rqucrewmemberid\ax)${sep}\a-w${sqlite.Result[qucrewmemberid 1 id]}\ax

      | set roles
      /sqlite query "${DBshared}" temp SELECT name FROM roles;
      /for _count2 1 to ${sqlite.Rows[temp]}
        DEBUG ${sep}checking${sep}\a-w.${sqlite.Result[temp ${_count2} name]}\ax${sep}${Group.${sqlite.Result[temp ${_count2} name]}}
        /if (${Group.${sqlite.Result[temp ${_count2} name]}.Name.Equal[${Group.Member[${_count}]}]}) {
          /sqlite query "${DBshared}" dummy INSERT INTO crewmemberroles(crewmemberid, role) VALUES (${sqlite.Result[qucrewmemberid 1 id]}, '${sqlite.Result[temp ${_count2} name]}');
          OUT /crew buildas${sep}\a-w${Group.Member[${_count}]} ${_verbage} ${sqlite.Result[temp ${_count2} name]}\ax (\a-r${sqlite.Status[dummy]}\ax)
        }
      /next _count2 
      /sqlite clear temp

    /next _count 
    /invoke ${out[18, 0, 33]}
    /sqlite clear qucrewmemberid
    /sqlite clear qucheckcharacter
    /sqlite clear qucrewname
    /return TRUE


  | delete crews
  } else /if (${_type.Equal[delete]}) {
    
    | delete a role
    /if (${_verbage.Equal[role]} && ${Bool[${_verbage2}]} && ${Bool[${_verbage3}]} && ${Bool[${_verbage4}]}) {
     
      | fix the name case
      /varset _verbage2 ${proper_case[name, "${_verbage2}"]}

      | fix the role case
      /varset _verbage3 ${proper_case[role, "${_verbage3}"]}

      | /cecho /sqlite query "${DBshared}" qucrewmemberid SELECT crewmembers.id FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE characters.charactername = '${_verbage2}' AND characters.server = '${MacroQuest.Server.Replace[ ,_].Lower}' AND crews.crewname = '${_verbage4}';
      /sqlite query "${DBshared}" qucrewmemberid SELECT crewmembers.id FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE characters.charactername = '${_verbage2}' AND characters.server = '${MacroQuest.Server.Replace[ ,_].Lower}' AND crews.crewname = '${_verbage4}';
      DEBUG ${sep}.Result (\a-rqucrewmemberid\ax)${sep}\a-w${sqlite.Result[qucrewmemberid 1 id]}\ax
      
      | add the toon as that role in crewmemberroles
      /sqlite query "${DBshared}" dummy DELETE FROM crewmemberroles WHERE crewmemberid = '${sqlite.Result[qucrewmemberid 1 id]}' AND role = '${_verbage3}';
      DEBUG ${sep}DELETE (\a-rqdummy\ax)${sep}\a-w${sqlite.Rows[dummy]}\ax
      OUT /crew delete role${sep}\a-w${_verbage2} ${_verbage3} ${_verbage4}\ax
      /return TRUE


    | delete a crewname
    } else /if (${_verbage.Equal[crewname]} && ${Bool[${_verbage2}]}) {

      /sqlite query "${DBshared}" dummy SELECT * FROM crews WHERE crewname='${_verbage2}';
      /if (!${sqlite.Rows[dummy]}) {
        OUT /crew delete crewname${sep}\a-w${_verbage2}\ax (\a-rDoes Not Exist\ax)
        /return FALSE
      }

      /sqlite query "${DBshared}" dummy SELECT characters.charactername, crewmemberroles.role FROM crewmemberroles LEFT JOIN crewmembers ON crewmemberroles.crewmemberid = crewmembers.id LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage2}';
      /if (${sqlite.Rows[dummy]}) {
        /for _count 1 to ${sqlite.Rows[dummy]}
          | get role id info        
          /sqlite query "${DBshared}" qucrewmemberid SELECT crewmembers.id FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE characters.charactername = '${sqlite.Result[dummy ${_count} charactername]}' AND characters.server = '${MacroQuest.Server.Replace[ ,_].Lower}' AND crews.crewname = '${_verbage2}';
          | delete roles
          /sqlite query "${DBshared}" tempdelete DELETE FROM crewmemberroles WHERE crewmemberid = '${sqlite.Result[qucrewmemberid 1 id]}' AND role = '${sqlite.Result[dummy ${_count} role]}';
          /sqlite clear tempdelete
          /sqlite clear qucrewmemberid
        /next _count

      }

      | get data for crewmembers
      /sqlite query "${DBshared}" qcrewname SELECT id FROM crews WHERE crewname='${_verbage2}';
      | delete crewmember relation to crews
      /sqlite query "${DBshared}" tempdelete DELETE FROM crewmembers WHERE crewid = '${sqlite.Result[qcrewname 1 id]}';
      
      /sqlite query "${DBshared}" dummy DELETE FROM crews WHERE crewname="${_verbage2}";
      /if (${sqlite.Resultcode[dummy]} == 0) {
        OUT /crew delete crewname${sep}\a-w${_verbage2}\ax (\a-r${sqlite.Status[dummy]}\ax)
        /return TRUE
      } else {
        OUT /crew delete crewname${sep}\a-w${_verbage2}\ax (\a-rDoes Not Exists\ax)
        /return TRUE
      }

    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[crewname,out]}
    /invoke ${maControl.Add[role,out]}
    /invoke ${set_control_output[FALSE, maChr, "crew delete", maControl]}
    /return FALSE   


  | form a crew
  } else /if (${_type.Equal[form]}) {

    | find the peoples and invite them
    /sqlite query "${DBshared}" increw SELECT characters.charactername FROM crewmembers LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage}'
    /sqlite query "${DBshared}" iscrew SELECT id FROM crews WHERE crewname = '${_verbage}'
    /sqlite query "${DBshared}" dummy SELECT COUNT(crewmembers.id) AS data FROM crewmembers LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage}';
    OUT /crew form${sep}\a-w${_verbage}\ax ${If[${sqlite.Rows[iscrew]},(\a-r${sqlite.Result[dummy 1 data]}\ax/\a-r6\ax),(\a-rFail\ax) (\a-rInvalid Crewname\ax)]}   
    /if (!${sqlite.Rows[increw]}) {
      /return FALSE
    }

    /if (${sqlite.Rows[increw]}) {
      /for _count 1 to ${sqlite.Rows[increw]}
        /if (${Group.Member[${sqlite.Result[increw ${_count} charactername]}].ID} || ${Spawn[pc ${sqlite.Result[increw ${_count} charactername]}].ID} == ${Me.ID}) {
          /continue
        }
        /invite ${sqlite.Result[increw ${_count} charactername]}
        /delay 4s ${Group.Member[${sqlite.Result[increw ${_count} charactername]}].ID}
        OUT ${sep}${sqlite.Result[increw ${_count} charactername]} (\a-r${Group.Member[${sqlite.Result[increw ${_count} charactername]}].ID}\ax)
      /next _count
    }

    | /grouproles set [NAME] [#]
    | 1=MainTank
    | 2=MainAssist           
    | 3=Puller
    | 4=MarkNpc
    | 5=MasterLooter         
    | /makeleader [NAME]

    | find the roles and set them
    /declare _leader sring local FALSE
    /sqlite query "${DBshared}" dummy SELECT characters.charactername, crewmemberroles.role FROM crewmemberroles LEFT JOIN crewmembers ON crewmemberroles.crewmemberid = crewmembers.id LEFT JOIN characters ON characters.id = crewmembers.characterid LEFT JOIN crews ON crews.id = crewmembers.crewid WHERE crews.crewname = '${_verbage}'
    /if (${sqlite.Rows[dummy]}) {
      /for _count 1 to ${sqlite.Rows[dummy]}
        
        | pull out leader
        /if (${sqlite.Result[dummy ${_count} role].Equal[Leader]}) {
          /varset _leader ${sqlite.Result[dummy ${_count} charactername]}
          /continue
        } else /if (${sqlite.Result[dummy ${_count} role].Equal[MainTank]}) {
          /grouproles set ${sqlite.Result[dummy ${_count} charactername]} 1
        } else /if (${sqlite.Result[dummy ${_count} role].Equal[MainAssist]}) {
          /grouproles set ${sqlite.Result[dummy ${_count} charactername]} 2
        } else /if (${sqlite.Result[dummy ${_count} role].Equal[Puller]}) {
          /grouproles set ${sqlite.Result[dummy ${_count} charactername]} 3
        } else /if (${sqlite.Result[dummy ${_count} role].Equal[MarkNpc]}) {
          /grouproles set ${sqlite.Result[dummy ${_count} charactername]} 4
        } else /if (${sqlite.Result[dummy ${_count} role].Equal[MasterLooter]}) {
          /grouproles set ${sqlite.Result[dummy ${_count} charactername]} 5
        }
        OUT ${sep}${sqlite.Result[dummy ${_count} role]}${sep}\a-w${sqlite.Result[dummy ${_count} charactername]}\ax
        /delay 1
      /next _count
    }
      
    /if (${Bool[${_leader}]}) {
      /makeleader ${_leader}        
      OUT ${sep}Leader${sep}\a-w${_leader}\ax
    }
      
    /invoke ${out[18, 0, 33]}
    /sqlite clear increw
    /sqlite clear iscrew
    /return TRUE
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}  
  /invoke ${maControl.Add[add,submenu]}
  /invoke ${maControl.Add[delete,submenu]}
  /invoke ${maControl.Add[buildas,out]}
  /invoke ${maControl.Add[form,out]}
  /invoke ${set_control_output[FALSE, maChr, crew, maControl]}

/return TRUE



|***
 * note: gets various states of other toons you are boxing
 * use: /state
 ***|
#bind command_state /state
sub Bind_command_state(string _type, string _verbage, bool _debug
  DEBUG \atBind_command_state\ax(\a-w${_verbage}\ax)
 
  /call control_broadcast_command state ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE
  
  ISMEDEAD

  /declare maState map local
  /invoke ${maState.Add[auto,maEnv|swAuto|bool]}
  /invoke ${maState.Add[agro,maAgro|swAgro|bool]}
  /invoke ${maState.Add[home,maHome|swHomeMarker|bool]}
  /invoke ${maState.Add[pull,maPull|swPull|bool]}
  /invoke ${maState.Add[env,maEnv|stEnvRadius|int]}
  /invoke ${maState.Add[tie,maTie|swSetTie|bool]}

  /declare _mist mapiterator local
  /declare _count int local 0
  /declare _out string local
  /declare _final string local
  /declare _toon string local

  | all group broadcast
  /if (${_type.Equal[group]} || !${Bool[${_type}]} && ${maEntropy.Find[stBroadcastState].Value.Equal[group]}) {
    /if (!${Group.Members}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/state\ax (\a-rgroup\ax) (\a-rFail: not in a group\ax)"]}
      /return FALSE
    } 
    /for _count 0 to ${Group.Members}    
      /if (${Group.Member[${_count}].ID} == ${Me.ID}) {
        /continue
      }
      /vardata _mist maState.First.Clone
      /while (!${_mist.IsEnd}) {
        /if (!${Defined[_${_mist.Key}]}) /declare _${_mist.Key} string local
        /dquery ${Group.Member[${_count}]} -q "${_mist.Value.Arg[1,|]}.Find[${_mist.Value.Arg[2,|]}].Value" -o _${_mist.Key} -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        /if (${_mist.Value.Arg[3,|].Equal[bool]}) {
          /varset _final ${If[${_${_mist.Key}},${on},${off}]}
        } else {
          /varset _final ${_${_mist.Key}}
        }
        /varset _out ${_out} ${dot} \aw${_mist.Key}\ax:${_final}
        /invoke ${_mist.Advance}
      }
      OUT ${sep}${Group.Member[${_count}]} ${_out}
      /varset _out
    /next _count

  | zone broadcasts
  } else /if (${_type.Equal[zone]} || !${Bool[${_type}]} && ${maEntropy.Find[stBroadcastState].Value.Equal[zone]}) {

    /for _count 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}
      /varset _toon ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}  
      /if (${_toon.Equal[${Me.DisplayName}]} || !${Spawn[${_toon}].ID}) {
        /continue
      }
      /vardata _mist maState.First.Clone
      /while (!${_mist.IsEnd}) {
        /if (!${Defined[_${_mist.Key}]}) /declare _${_mist.Key} string local
        /dquery ${_toon} -q "${_mist.Value.Arg[1,|]}.Find[${_mist.Value.Arg[2,|]}].Value" -o _${_mist.Key} -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        /if (${_mist.Value.Arg[3,|].Equal[bool]}) {
          /varset _final ${If[${_${_mist.Key}},${on},${off}]}
        } else {
          /varset _final ${_${_mist.Key}}
        }
        /deletevar _${_mist.Key}

        /varset _out ${_out} ${dot} \aw${_mist.Key}\ax:${_final}
        /invoke ${_mist.Advance}
      }
      OUT ${sep}${_toon} ${_out}
      /varset _out
    /next _count


  | all entropy broadcasts
  } else /if (${_type.Equal[all]} || !${Bool[${_type}]} && ${maEntropy.Find[stBroadcastState].Value.Equal[all]}) {
    /for _count 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}
      /varset _toon ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}  
      /if (${_toon.Equal[${Me.DisplayName}]}) {
        /continue
      }
      /vardata _mist maState.First.Clone
      /while (!${_mist.IsEnd}) {
        /if (!${Defined[_${_mist.Key}]}) /declare _${_mist.Key} string local
        /dquery ${_toon} -q "${_mist.Value.Arg[1,|]}.Find[${_mist.Value.Arg[2,|]}].Value" -o _${_mist.Key} -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        /if (${_mist.Value.Arg[3,|].Equal[bool]}) {
          /varset _final ${If[${_${_mist.Key}},${on},${off}]}
        } else {
          /varset _final ${_${_mist.Key}}
        }
        /varset _out ${_out} ${dot} \aw${_mist.Key}\ax:${_final}
        /invoke ${_mist.Advance}
      }
      OUT ${sep}${_toon} ${_out}
      /varset _out
    /next _count


  | all raid broadcast
  } else /if (${_type.Equal[raid]} || !${Bool[${_type}]} && ${maEntropy.Find[stBroadcastState].Value.Equal[raid]}) {
    /if (!${Raid.Members}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/state\ax (\a-rraid\ax) (\a-rFail: not in a raid\ax)"]}
      /return break
    }

    /for _count 0 to ${Raid.Members}    
      /if (${Raid.Member[${_count}].ID} == ${Me.ID} || !${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Find[${Raid.Member[${_count}]}]} ) {
        /continue
      }
      /vardata _mist maState.First.Clone
      /while (!${_mist.IsEnd}) {
        /if (!${Defined[_${_mist.Key}]}) /declare _${_mist.Key} string local
        /dquery ${Raid.Member[${_count}]} -q "${_mist.Value.Arg[1,|]}.Find[${_mist.Value.Arg[2,|]}].Value" -o _${_mist.Key} -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        /if (${_mist.Value.Arg[3,|].Equal[bool]}) {
          /varset _final ${If[${_${_mist.Key}},${on},${off}]}
        } else {
          /varset _final ${_${_mist.Key}}
        }
        /varset _out ${_out} ${dot} \aw${_mist.Key}\ax:${_final}
        /invoke ${_mist.Advance}
      }
      OUT ${sep}${Raid.Member[${_count}]} ${_out}
      /varset _out
    /next _count

  }
  /invoke ${out[18, 0, 33]}

/return TRUE



|***
 * note: melee control
 * use: /melee
 ***|
#bind control_melee /melee
sub Bind_control_melee(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
  DEBUG \atBind_control_melee\ax(\a-w${_type}, ${_verbage}, ${_verbage2}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP melee ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_verbage},see,tag,alias,clear]} || ${Select[${_type},list,line]} || ${Range.Between[1,12:${Int[${_type}]}]}) {
    /call control_base_edit ${maDebug.Find[edit].Value} Melee 12 "${_type}" "${_verbage}" "${_verbage2}" ${_silent}
    /return TRUE
  }
 
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${set_control_output[FALSE, maMelee, "melee", maControl]}

/return TRUE



|***
 * note: remove a buff type
 * use: /nomore [levi|invis|SPA type|list|sos] DEBUG
 ***|
#bind control_nomore /nomore
sub Bind_control_nomore(string _type, bool _debug)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP nomore ${_type.Right[1]}
    /return TRUE
  }

  | list spa types (long as list....)
  /if (${Select[${_type},list]}) {
    
    /sqlite query "${DBstatic}" dummy SELECT * FROM eq_spa;
    /declare _count int local 0
    
    /for _count 1 to ${sqlite.Rows[dummy]}
      OUT ${sep}\a-w${sqlite.Result[dummy ${_count} env_var]}\ax ${submenu} \a-w${sqlite.Result[dummy ${_count} value]}\ax
    /next _count
    /invoke ${out[18, 0, 33]}
    /return FALSE

  | remove levi
  } else /if (${Select[${_type},levi]}) {
    /call remove_spa ${_debug} 57
    /return TRUE

  | remove ALLLL!!! damage Shields
  } else /if (${Select[${_type},ds]}) {
    /call remove_spa ${_debug} 59
    /return TRUE

  | remove t3e3 event from ToL
  } else /if (${Select[${_type},t3e3]}) {
    | levi
    /call remove_spa ${_debug} 57
    | DS
    /call remove_spa ${_debug} 59
    | movement
    /call remove_spa ${_debug} 3

    /return TRUE
  
  | remove invis
  } else /if (${Select[${_type},invis]}) {
    /call remove_spa ${_debug} 12
    /call remove_spa ${_debug} 28
    /call remove_spa ${_debug} 29
    /call remove_spa ${_debug} 314
    /call remove_spa ${_debug} 315
    /call remove_spa ${_debug} 316
    
    
    /if (${maEnv.Find[swSoS].Value}) {
      /if (!${Me.AbilityReady[sneak]}) {
        /delay ${Me.AbilityReady[sneak]}
        OUT Removing${sep}\a-wSneak\ax
        /doability sneak
      }
      /if (!${Me.AbilityReady[hide]}) {
        /delay ${Me.AbilityReady[hide]}
        OUT Removing${sep}\a-wHide\ax
        /doability hide
      }
    }
    
    
    /return TRUE
  | remove invis
  } else /if (${Select[${_type},da]}) {
    /call remove_spa ${_debug} 40
    /return TRUE

  | remove invis
  } else /if (${Select[${_type},sos]}) {
    |**
    /if (!${Me.AbilityReady[sneak]}) {
      /delay ${Me.AbilityReady[sneak]}
      OUT Removing${sep}\a-wSneak\ax
      /doability sneak
    }
    /if (!${Me.AbilityReady[hide]}) {
      /delay ${Me.AbilityReady[hide]}
      OUT Removing${sep}\a-wHide\ax
      /doability hide
    }
    **|
    /makemevisible 
    /return TRUE

  | remove all buffs
  } else /if (${Select[${_type},removeall]}) {
    /declare _count int local 0
    /while (${_count} <= ${Me.MaxBuffSlots}) {
      /varcalc _count ${_count} +1
      /invoke ${Me.Buff[${_count}].Remove}
      /delay 1
      /if (${_count} > 30) /continue
      /invoke ${Me.Song[${_count}].Remove}
      /delay 1
    }
        
  | remove spa by type (name)   
  } else /if (${Range.Between[1,999:${Int[${_type}]}]}) {
    
    /sqlite query "${DBstatic}" dummy SELECT * FROM eq_spa WHERE env_var='${_type}';
    
    /if (!${sqlite.Rows[dummy]}) {
      OUT Not a valid SPA type${sep}\a-w${_type}\ax
      /return FALSE
    }
    /call remove_spa ${_debug} ${sqlite.Result[dummy 1 env_var]}   
    /return 
  }
  
  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[invis,option]}
  /invoke ${maControl.Add[levi,option]}
  /invoke ${maControl.Add[da,option]}
  /invoke ${maControl.Add[spa##,option]}
  /invoke ${maControl.Add[removeall,option]}
  /invoke ${set_control_output[FALSE, maMelee, "nomore", maControl]}  

/return TRUE



|***
 * note: break all actions and force a movement (not that kind of movement)
 * use: /getoverhere
 ***|
#bind command_getoverhere /getoverhere
sub Bind_command_getoverhere(string _type, string _verbage, bool _debug)

  /call control_broadcast_command getoverhere ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE

  /if (!${maTie.Find[swTieBreak].Value}) {
    /return FALSE
  }

  /call set_stop ${_debug} SILENT
  /call target clear
  /invoke ${set_data_timer[${_debug}, Check_Tie, 0]}
  /invoke ${maTie.Add[swSetTie,TRUE]}
  /invoke ${maData.Add[swGetOverHere,TRUE]}
  
/return TRUE



|***
 * note: merchant tools
 * use: /merchant
 ***|
#bind control_merchant /merchant
sub Bind_control_merchant(string _type, string _verbage, _verbage2, bool _debug)

  | buy
  /if (${_type.Equal[buy]}) {
    OUT Buying${seo}\a-w${_verbage}\ax
  	/declare _quantity int local
  	/declare _current int local
  	/declare _lagmonster int local
 
    /invoke ${Merchant.OpenWindow}
  	/delay 10s ${Merchant.ItemsReceived}
 
  	/if (${_verbage2}) {
    	/if (${_verbage2} <= ${FindItemCount[=${_verbage}]}) {
        OUT you have more than enough ${_verbage}
        /return TRUE
      }
    }
  	
  	/varcalc _lagmonster ${Math.Calc[${MacroQuest.Ping} / 10]}
    
  	/if (${FindItemCount[=${_verbage}]}) {
  		/varcalc _quantity ${Int[${Math.Calc[${_verbage2}-${FindItemCount[=${_verbage}]}]}]}
  	} else {
  		/varset _quantity ${_verbage2}
  	}
  	
  	/varcalc _current ${Int[${Math.Calc[${_verbage2}+${FindItemCount[=${_verbage}]}]}]}
  :Filling
  	/delay 5s ${Merchant.ItemsReceived}

  	/if (${FindItemCount[=${_verbage}]}>= ${_verbage2}) {
  		/return
  	}
  	/if (${Merchant.Item[=${_verbage}].ID}) {
  		/invoke ${Merchant.SelectItem[=${_verbage}]}
  		/delay 2
  		
  	} else {
  		/if (${Window[MerchantWnd].Child[MW_UsableButton].Checked}) {
  			/notify MerchantWnd MW_UsableButton leftmouseup
  			/goto :Filling
  		}
  		OUT Couldn't find ${_verbage} on this Merchant
  		/return FALSE
  	}
  	OUT Buying ${_verbage} until I get ${_verbage2}


  	/while (${_quantity} > 0) {
  		/invoke ${Merchant.SelectItem[=${_verbage}]}
  		OUT Testing only will remove later
  		/delay 5
  		/invoke ${Merchant.Buy[${_quantity}]}
  		/delay 5
  		/delay 5
  		/varcalc _quantity ${Int[${Math.Calc[${_verbage2}-${FindItemCount[=${_verbage}]}]}]}
  		OUT amount i have ${_current}==${FindItemCount[=${_verbage}]}
  		/delay 10s ${_current}==${FindItemCount[=${_verbage}]}
  		OUT I have ${FindItemCount[=${_verbage}]} ${_verbage}
  		/if (${_quantity} <= ${FindItemCount[=${_verbage}]}) {
  			| /popup Woot we did it!!
  			/break
  		}
  		| /popup Well lag is too bad so activating the lag monster
  		| /delay ${_lagmonster}
  	}
    
  	/delay 5
  	/varcalc _quantity ${Int[${Math.Calc[${_verbage2}-${FindItemCount[=${_verbage}]}]}]}
  	/if (${_quantity} <= 0) /return
  	/return
  }
  
  | Sell
  /if (${_type.Equal[sell]}) {
    OUT Selling ${_verbage2} ${_verbage}
    OUT i have ${FindItemCount[=${_verbage}]} ${_verbage}
    /invoke ${Merchant.OpenWindow}
    /delay 10s ${Merchant.ItemsReceived}

    /selectitem "${_verbage}"
    /delay 5
    /invoke ${Merchant.Sell[${_verbage2}]}
    /delay 5
    OUT i now have ${FindItemCount[=${_verbage}]} ${_verbage}

    /return

  }


/return



|***
 * note: you are the black hole of suck....
 * use: /gather
 ***|
#bind command_gather /gather
sub Bind_command_gather(string _type, string _verbage, bool _debug)
  DEBUG \atBind_command_gather\ax()

  /call control_broadcast_command gather ${_type} ${_verbage}
  /if (${Macro.Return.Equal[break]}) /return TRUE

  ISMEDEAD
  
  /if (!${Select[${_type},all,group,raid,zone]}) {
    /varset _type ${maEntropy.Find[stBroadcastGather].Value}
  } 

  /invoke ${out[12, "Gathering${sep}\a-w${_type.Lower}\ax"]}

  /if (${_type.Equal[group]} && ${Group.Members}) {
    /dggexecute /squelch /target clear
    /dggexecute /delay 1s ${Parse[0,!${Target.ID}]}
    /dggexecute /navto ${Me.ID}
    
  } else /if (${_type.Equal[raid]} && ${Raid.Member}) {
    /dgrexecute /squelch /target clear
    /dgrexecute /delay 1s ${Parse[0,!${Target.ID}]}
    /dgrexecute /navto ${Me.ID}
    
  } else /if (${_type.Equal[zone]}) {
    /dgzexecute /squelch /target clear
    /dgzexecute /delay 1s ${Parse[0,!${Target.ID}]}
    /dgzexecute /navto ${Me.ID}
    
  } else /if (${_type.Equal[all]}) {
    /dgexecute ${maEntropy.Find[stEntropyGroup_all].Value} /squelch /target clear
    /dgexecute ${maEntropy.Find[stEntropyGroup_all].Value} /delay 1s ${Parse[0,!${Target.ID}]}
    /dgexecute ${maEntropy.Find[stEntropyGroup_all].Value} /navto ${Me.ID}
    
  }

/return TRUE



|***
 * note: control for rest
 * use: /rest [options]
 ***|
#Bind control_rest /rest
sub Bind_control_rest(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)
  DEBUG \atcontrol_rest\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)

  /if (${_type.Left[1].Equal[-]}) {
    /lua run HELP rest ${_type.Right[1]}
    /return TRUE
  }

  /if (${Select[${_type},list]}) {
   OUT Spawn Search${sep}\awstRestSpawnSearch\ax 
   OUT ${sep}${maRest.Find[stRestSpawnSearch].Value}
   /return TRUE    

  | set rest %
  } else /if (${_type.Equal[pct]}) {
    /if (!${set_control_num_range[${_debug}, stPctRest, "${_verbage}", 0, 99]}) /return

  | rest radius
  } else /if (${_type.Equal[rad]}) {
    /if (!${set_control_num_range[${_debug}, stRestRadius, "${_verbage}", 1, 100]}) /return

  | rest to full
  } else /if (${_type.Equal[full]}) {
    /invoke ${set_switch_env[${_debug}, swRestFull, TRUE, ${_verbage}]}

  | rest to full
  } else /if (${_type.Equal[health]}) {
    /invoke ${set_switch_env[${_debug}, swRestCheckHealth, TRUE, ${_verbage}]}

  | rest while in a raid?
  } else /if (${_type.Equal[inraid]}) {
    /invoke ${set_switch_env[${_debug}, swRestInRaid, TRUE, ${_verbage}]}

  | rest through combat
  } else /if (${_type.Equal[combat]}) {
    /invoke ${set_switch_env[${_debug}, swRestCombat, TRUE, ${_verbage}]}

  | mod rods in rest
  } else /if (${_type.Equal[modrod]}) {
    /invoke ${set_switch_env[${_debug}, swRestModRod, TRUE, ${_verbage}]}

  | check buffs in rest
  } else /if (${_type.Equal[buff]}) {
    /invoke ${set_switch_env[${_debug}, swRestBuff, TRUE, ${_verbage}]}

  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[list,list]}
  /invoke ${maControl.Add[pct,stPctRest]}
  /invoke ${maControl.Add[rad,stRestRadius]}
  /invoke ${maControl.Add[full,swRestFull]}
  /invoke ${maControl.Add[health,swRestCheckHealth]}
  /invoke ${maControl.Add[combat,swRestCombat]}
  /invoke ${maControl.Add[inraid,swRestInRaid]}
  /invoke ${maControl.Add[modrod,swRestModRod]}
  /invoke ${maControl.Add[buff,swRestBuff]}
  /invoke ${set_control_output[${_debug}, maRest, "rest", maControl]}

/return TRUE


