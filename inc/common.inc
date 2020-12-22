5|*********************************************************************
 * entropy
 * common.inc
 *
 ***|





|***
 * note: sets memory resident data values/containers/strings/so on
 * use: /call set_data DEBUG [container|FALSE] [variable/key name] [col|map|list|set|string|int|bool] [outer|local|global] ["value"]
 ***|
sub set_data(bool _debug, string _container, string _name, string _type, string _scope, string _value)
  /if (${_debug}) ECHO \atset_data\ax(\a-w${_container}, ${_name}, ${_type}, ${_scope}, "${_value}"\ax)

  | default MQ2 variables
  /if (${Select[${_type},bool,string,int,float]}) {
    | define the variable if it does not exist
    /if (!${Defined[${_name}]}) {
      /declare ${_name} ${_type} ${_scope}
    }

    | populate the variable
    /varset ${_name} ${_value}
    /return
  }

  | maps
  /if (${Select[${_type},col,map]}) {
    | make sure map container exists
    /if (!${Defined[${_container}]}) {
      /declare ${_container} map ${_scope}
    }

    | add to map
    /invoke ${${_container}.Add[${_name},"${_value}"]}
    /if (${_debug}) ECHO \a-p::\axmap contents\a-p::\ax${${_container}.Find[${_name}]}
    /if (${_debug}) ECHO \a-p::\axmap Success\a-p::\ax count\a-p::\ax${${_container}.Count}
  }

  | sets
  /if (${Select[${_type},set]}) {
    | make sure set container exists
    /if (!${Defined[${_container}]}) {
      /declare ${_container} set ${_scope}
    }

    | add to set
    /invoke ${${_container}.Add[${_value}]}
    /if (${_debug}) ECHO \a-p::\axset Success\a-p:: count\a-p::\ax${${_container}.Count}
  }

  | lists
  /if (${Select[${_type},col,list]}) {
    | make sure list container exists
    /if (!${Defined[${_name}]}) {
      /declare ${_name} list ${_scope}
      /invoke ${${_name}.Delimiter["|"]}
    }

    | add to list
    /if (${Bool[${_value}]}) {
      /invoke ${${_name}.Append["${_value}"]}
      /if (${_debug}) ECHO \a-p::\axlist Success\a-p::\ax
    }
  }

/return TRUE



|***
 * note: generate/restart timers
 * use: /call set_data_timer DEBUG [timer variable] [value|RESTART] [Temp|delete] || ${set_data_timer[DEBUG, Variable, value|RESTART, temp|delete]}
 ***|
sub set_data_timer(string _debug, string _env_timer, string _value, string _verbage)
  DEBUG \atset_data_timer\ax(\a-w${_env_timer}, ${_value}, ${_verbage}\ax)

  | timer restart
  /if (${_value.Equal[RESTART]}) {
    DEBUG ${sep}\a-yRESTART\ax for \aw${_env_timer}\ax${sep}${maTimer.Find[ti${_env_timer}].Value}\ax
    /varset timer_${_env_timer} ${maTimer.Find[ti${_env_timer}].Value}
    /return
  }

  | temporary timer
  /if (${_verbage.Equal[temp]}) {
    DEBUG ${sep}temp${sep}timer_${_env_timer}${sep}\a-w${_value}\ax
    /if (!${Defined[timer_${_env_timer}]}) {
      /declare timer_${_env_timer} timer outer
    }
    /varset timer_${_env_timer} ${_value}
    /return TRUE
  }

  | delete timer
  /if (${_verbage.Equal[delete]}) {
    /if (!${Defined[timer_${_env_timer}]}) {
      /deletevar timer_${_env_timer}
    }
  }

  | create map
  /if (!${Defined[maTimer]}) {
    /declare maTimer map outer
  }

  | is the data there already?
  /sqlite query "${DBcharacter}" dummy SELECT * FROM environment_timer WHERE env_timer='ti${_env_timer}';
  /if (!${sql_check[${_debug}, dummy, set_data_timer, "SELECT *"]}) /endmacro

  | data exists. get from DB and call good
  /if (${sqlite.Rows[dummy]}) {
    DEBUG ${sep}Timer \agdoes\ax exists. map it
    /invoke ${maTimer.Add[ti${_env_timer},${sqlite.Result[dummy 1 value]}]}

  } else /if (!${sqlite.Rows[dummy]}) {
    DEBUG ${sep}Timer does \arNOT\ax exists. add to DB and map it
    /sqlite query "${DBcharacter}" dummy INSERT INTO environment_timer(env_timer, value) VALUES('ti${_env_timer}', '${_value}');
    /if (!${sql_check[${_debug}, dummy, set_data_timer, "INSERT INTO environment_timer"]}) /endmacro
    /invoke ${maTimer.Add[ti${_env_timer},${_value}]}
  }

  | make timer
  | all timers start at 1 so the code will trigger the associated sub on the first cycle. from there we get into a normal routine
  /if (!${Defined[timer_${_env_timer}]}) {
    /declare timer_${_env_timer} timer outer
    /varset timer_${_env_timer} 1
  } else {
    /varset timer_${_env_timer} ${_value}
  }

/return TRUE



|***
 * note: generate initial environment sql and map
 * use: /call set_data_map DEBUG [map name] [variable] [value] <aux> <table>
 ***|
sub set_data_map(bool _debug, string _map, string _env_var, string _value, _aux, _table)
  DEBUG \atset_data_map\ax(\a-w${_map}, ${_env_var}, "${_value}", "${_aux}", ${_table}\ax)

  | make sure map container exists
  /if (!${Defined[${_map}]}) {
    /declare ${_map} map outer
  }

  | set the default database suffix
  /if (!${Bool[${_table}]}) {
    /varset _table character
  }

  | is the data there already?
  /sqlite query "${DBcharacter}" dummy SELECT * FROM environment_${_table} WHERE env_var="${_env_var}";
  /if (!${sql_check[${_debug}, dummy, set_data_map, "SELECT (*)"]}) /endmacro

  | data exists. get from DB and call it good
  /if (${sqlite.Rows[dummy]} && !${pregenUpdate}) {
    DEBUG ${sep}sqlite.Rows[dummy]${sep}\a-w${sqlite.Rows[dummy]}\ax
    /if (${maEnv.Find[schema].Value.Equal[${maData.Find[stPreviousSchema].Value}]}) {
      /invoke ${${_map}.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
      DEBUG ${sep}${_map}.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]
      /return TRUE

    | data exists but versions are different
    } else /if (${maEnv.Find[schema].Value.NotEqual[${maData.Find[stPreviousSchema].Value}]}) {
      DEBUG ${sep}DATA \agdoes exists VERSIONS \arDO NOT\ax MATCH\ax
      /invoke ${${_map}.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
      DEBUG ${sep}${_map}${${_map}.Find[${sqlite.Result[dummy 1 env_var]}]}
      /declare _DBLOC string local ${get_key_DBLOC[${_debug}, ${_env_var}]}
      /sqlite query "${DBcharacter}" dummy UPDATE ${_DBLOC.Arg[1,|]} SET 'aux'="${Parse[1,${_aux}]}" WHERE env_var="${_env_var}";
      /if (!${sql_check[${_debug}, dummy, set_data_map, "UPDATE ${_DBLOC.Arg[1,|]}"]}) /endmacro
      /return TRUE

    }

  | data does not exist
  } else /if (!${sqlite.Rows[dummy]} || ${pregenUpdate}) {
    DEBUG ${sep}DATA \ardoes NOT exists || Forced rebuild\ax

    /if (${_value.Find[PREGEN]}) {
      DEBUG \agPREGEN DATA\ax
      /declare _count int local 1
      /declare _tmpValue string local FALSE
      /declare _previousValue string local

      /sqlite query "${DBstatic}" dummy SELECT * FROM env_${Me.Class.ShortName.Lower} WHERE env_var="${_env_var}";
      /if (!${sql_check[${_debug}, dummy, set_data_map, "SELECT (*)"]}) /endmacro

      DEBUG .... ${sqlite.Result[dummy 1 value].Count[|]} ..${sqlite.Result[dummy 1 value]}
      
      | get current data for output change notice
      | /if (${pregenUpdate}) {
        /varset _previousValue ${${_map}.Find[${_env_var}].Value}
      | }

      | how many do we want to find?
      /if (${sqlite.Result[dummy 1 value].Count[|]}) {
        /while (${_count} <= ${sqlite.Result[dummy 1 value].Count[|]}) {
          DEBUG ${sep}_count list${sep}\a-w${_count}/${sqlite.Result[dummy 1 value].Count[|]}\ax ${dot} ${sqlite.Result[dummy 1 value].Arg[${_count},|]}
          /if (${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.AltAbility[${sqlite.Result[dummy 1 value].Arg[${_count},|]}]}) {
            /varset _tmpValue ${sqlite.Result[dummy 1 value].Arg[${_count},|]}
            DEBUG ${sep}_tmpValue${sep}\a-w${sqlite.Result[dummy 1 value].Arg[${_count},|]}\ax
            /break
          }
          /varcalc _count ${_count}+1
        }

      } else /if (!${sqlite.Result[dummy 1 value].Count[|]}) {
        /varset _tmpValue FALSE
        DEBUG ${sep}_tmpValue${sep}\a-wFALSE (nothing there)\ax
      }


      | adjust our running value for the database entry
      /varset _value ${_tmpValue}
      DEBUG ${sep}_value${sep}\a-w${_tmpValue}\ax
      
    /if (${pregenUpdate}) {
      OUT ${_env_var}${sep}\a-w${_previousValue}\ax ${submenu} \a-w${_value}\ax
    }
  
      
      
    }

    /sqlite query "${DBcharacter}" dummy INSERT OR IGNORE INTO environment_${_table}(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${_aux}");
    /if (!${sql_check[${_debug}, dummy, set_data_map, "INSERT OR IGNORE INTO (environment_${_table})"]}) /endmacro
    /invoke ${${_map}.Add[${_env_var},"${_value}"]}
    
   
    

  }

/return TRUE



|***
 * note: generate initial environment sql and list
 * use: /call set_data_list DEBUG [list name] [variable] [value] [aux]
 ***|
sub set_data_list(bool _debug, string _map, string _env_var, string _value, string _aux, string _table)
  DEBUG \atset_data_list\ax(\a-w${_map}, ${_env_var}, "${_value}", "${_aux}", ${_table}\ax)

  | make sure list container exists
  /if (!${Defined[${_env_var}]}) {
    /declare ${_env_var} list outer
    /invoke ${${_env_var}.Delimiter["|"]}
  }

  | make sure map container exists
  /if (!${Defined[${_map}]}) {
    /declare ${_map} map outer
  }

  | set the default database suffix
  /if (!${Bool[${_table}]}) {
    /varset _table character
  }

  | is the data there already?
  /sqlite query "${DBcharacter}" dummy SELECT * FROM environment_${_table} WHERE env_var="${_env_var}";
  /if (!${sql_check[${_debug}, dummy, set_data_list, "SELECT *"]})  /endmacro

  | data exists. get from DB and call it good
  /if (${sqlite.Rows[dummy]}) {
    /if (${maEnv.Find[schema].Value.Equal[${maData.Find[stPreviousSchema].Value}]}) {
      DEBUG ${sep}DATA \agdoes exists VERSIONS MATCH\ax

      /if (${Bool[${sqlite.Result[dummy 1 value]}]}) {
        /declare _tmpValue string local ${sqlite.Result[dummy 1 value]}
        DEBUG ${sep}_tmpValue${sep}${_tmpValue}

        /invoke ${${_env_var}.Append["${_tmpValue}"]}
         DEBUG ${sep}${_map}.Add[${_env_var},"${_tmpValue}"]
        /invoke ${${_map}.Add[${_env_var},"${_tmpValue}"]}
      }
      /return TRUE

    | data exists but versions are different
    } else /if (${maEnv.Find[schema].Value.NotEqual[${maData.Find[stPreviousSchema].Value}]}) {
      DEBUG ${sep}DATA \agdoes exists VERSIONS \arNO NOT\ax MATCH\ax

      /if (${Bool[${sqlite.Result[dummy 1 value]}]}) {
        /declare _tmpValue string local ${sqlite.Result[dummy 1 value]}
        DEBUG ${sep}_tmpValue${sep}${_tmpValue}

        /invoke ${${_env_var}.Append["${_tmpValue}"]}
        DEBUG ${sep}${_map}.Add[${_env_var},"${_tmpValue}"]
        /invoke ${${_map}.Add[${_env_var},"${_tmpValue}"]}
      }
      /declare _DBLOC string local ${get_key_DBLOC[${_debug}, ${_env_var}]}
      /sqlite query "${DBcharacter}" dummy UPDATE ${_DBLOC.Arg[1,|]} SET 'aux'='${Parse[1,${_aux}]}' WHERE env_var="${_env_var}";
      /if (!${sql_check[${_debug}, dummy, set_data_list, "UPDATE ${_DBLOC.Arg[1,|]}"]}) /endmacro
      /return TRUE

    }

  | if data doesnt exist??
  } else /if (!${sqlite.Rows[dummy]}) {
    DEBUG ${sep}DATA \ardoes NOT exists\ax
    /declare _count int local 1
    /declare _tmpList string local

    /if (${_value.Find[PREGEN]}) {
      DEBUG PREGEN DATA
      /sqlite query "${DBstatic}" dummy SELECT * FROM env_${Me.Class.ShortName.Lower} WHERE env_var="${_env_var}";
      /if (!${sql_check[${_debug}, dummy, set_data_list, "SELECT *"]})  /endmacro

      | how many do we want to find?

      /while (${_count} <= ${_value.Arg[2,|]}) {
        DEBUG ++ ${sqlite.Result[dummy 1 value].Arg[${_count},|]}
        | is it in the book?
        DEBUG --- ${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${sqlite.Result[dummy 1 value].Arg[${_count},|]}]} || ${Me.AltAbility[${sqlite.Result[dummy 1 value].Arg[${_count},|]}]} || ${sqlite.Result[dummy 1 value].Arg[${_count},|].Equal[EMPTY]}
        /if (${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.AltAbility[${sqlite.Result[dummy 1 value].Arg[${_count},|]}]} || ${sqlite.Result[dummy 1 value].Arg[${_count},|].Equal[EMPTY]}) {
          /if (${Bool[${_tmpList}]}) /varset _tmpList ${_tmpList}|
          /varset _tmpList ${_tmpList}${sqlite.Result[dummy 1 value].Arg[${_count},|]}
          /varcalc _count ${_count}+1
        } else /if (!${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} && !${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} && !${Me.AltAbility[${sqlite.Result[dummy 1 value].Arg[${_count},|]}]}) {
          /varcalc _count ${_count}+1
          /continue
        }
      }

      | adjust our running value for the database entry
      /varset _value ${_tmpList}
      DEBUG ${sep} _value ${_tmpList}
      DEBUG ${sep} _tmpList ${_tmpList}
    }

    | put shit into the database.. took us this long to get here. may as well do something
    /sqlite query "${DBcharacter}" dummy INSERT OR IGNORE INTO environment_${_table}(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${Parse[1,${_aux}]}");
    /if (!${sql_check[${_debug}, dummy, set_data_list,  "INSERT OR IGNORE INTO environment_${_table}"]})  /endmacro

  }

  | create our list for the class to use
  /if (!${_value.Find[PREGEN]}) {
    DEBUG ${sep} _map${sep}\a-w${_map}\ax ${dot} _env_var${sep}\a-w${_env_var}\ax ${dot} _value${sep}\a-w${_value}\ax
    /invoke ${${_env_var}.Append[${_value}]}
    /invoke ${${_map}.Add[${_env_var},"${_value}"]}
  }

/return TRUE



|***
 * note: enforced a range of numbers from a user enterd value
 * use: ${set_control_num_range[DEBUG, VARIABLE, "VALUE", LOW, HIGH]}
 ***|
sub set_control_num_range(bool _debug, string _variable, int _value, int _low, int _high)
  DEBUG \atset_control_num_range\ax(\a-w${_variable}, ${_value}, ${_low}, ${_high}\ax)

  /if ((${Bool[${_value}]} || ${_value} == 0) && ${Range.Between[${_low},${_high}:${Int[${_value}]}]}) {
    /call do_raw_edit ${_debug} SILENT "${_variable}" "${_value}"
    /return TRUE
  } else {
    /invoke ${out[1, shade, 0]}
    /if (${_low} == 0) {
      /invoke ${out[0, "${notice}${sep}Control Range ${dot} ${info}${_low}\ax(\aroff\ax)${info} - ${_high}\ax"]}
      /return FALSE
    } else {
      /invoke ${out[0, "${notice}${sep}Control Range ${dot} ${info}${_low} - ${_high}\ax"]}
      /return FALSE
    }
  }

/return FALSE



|***
 * note: DEFINE call to forde event and bind queue checking
 * use: GETINPUT [SUBTOCREATE] [VARIABLETOMAKEACCESSABLE]
 ***|
sub get_input(string _sub, string _variable)

  | check the event queue
  /doevents

  /if (!${maEntropy.Find[swHookSub].Value}) {
    /return TRUE
  }

  /if (${Bool[${_sub}]}) {
    /call register_hook ${_sub} ${_variable}
  }

/return TRUE



|***
 * note: check that a plugin is loaded
 * use:
 ***|
sub check_plugin(bool _debug, bool _error, string _plugin)
  DEBUG \atcheck_plugin\ax(\a-w${_error}, ${_plugin}\ax)

  /if (!${Bool[${_plugin}]}) {
    /invoke ${out[0, 0, 60]}
    /endmacro
  }
  /declare _plugcount int local 0
  /declare _count int local 0

  /for _count 1 to ${_plugin.Count[|]}
    /if (${Bool[${Plugin[${_plugin.Arg[${_count},|]}]}]}) {
      /varcalc _plugcount ${_plugcount}+1
    } else /if (!${Bool[${Plugin[${_plugin.Arg[${_count},|]}]}]} && ${_error}) {
       ECHO \arWARNING\ax\a-p::\ax\a-w${_plugin.Arg[${_count},|]}\ax is not loaded
    }
  /next _count

  /if (${_plugcount} == ${_plugin.Count[|]}) {
    /return TRUE
  } else /if (${_plugcount} != ${_plugin.Count[|]}) {
    /return FALSE
  }

/return FALSE



|***
 * note: sets all blocked buffs into a list
 * use: set_buffs_blocked
 ***|
sub set_buffs_blocked(bool _debug, string _silent)
  DEBUG \atset_buffs_blocked\ax(\a-w${_silent}\ax)

  /declare _count int local 0

  /if (!${Defined[lsBlockedBuffsMe]}) {
    DEBUG ${sep}setting ${sep}\a-wlsBlockedBuffsMe\ax
    /declare lsBlockedBuffsMe string outer
  }

  | cycle the blocked buffs and add them to a string
  /for _count 1 to 40
    | skip empty ones
    /if (!${Me.BlockedBuff[${_count}].ID}) {
      /continue
    }
    /if (!${lsBlockedBuffsMe.Find[${Me.BlockedBuff[${_count}].ID}]}) {
      DEBUG ${sep}adding${sep}${Me.BlockedBuff[${_count}].ID}${sep}\a-w${Spell[${Me.BlockedBuff[${_count}].ID}].Name}\ax
      /varset lsBlockedBuffsMe ${lsBlockedBuffsMe}|${Me.BlockedBuff[${_count}].ID}
    }

  /next _count

  /if (${_silent.Equal[SILENT]}) {
    /return TRUE
  }
  /invoke ${out[0, "Marking Blocked Buffs"]}

/return



|***
 * note: get free ingame cash
 * use: /call check_free_ingame_cash DEBUG
 ***|
sub check_free_ingame_cash(bool _debug)
  DEBUG \atcheck_free_ingame_cash\ax()

  GETINPUT check_free_ingame_cash FALSE|FALSE

  /if (${Window[MKPW_ClaimWindow].Child[MKPW_ClaimClickHereBtn].Text.EqualCS[Claimed]}) {
    /return TRUE
  }

  /declare _current_funds string local 0

  /if (${Window[MKPW_ClaimWindow].Child[MKPW_ClaimClickHereBtn].Text.EqualCS[Claim]}) {
    /if (!${Window[MarketplaceWnd]}) /market
    /varset _current_funds ${Window[MarketplaceWnd].Child[MKPW_AvailableFundsUpper].Text}
    /notify MKPW_ClaimWindow MKPW_ClaimClickHereBtn leftmouseup
    /delay 1s
    /delay 10s ${Window[MarketplaceWnd].Child[MKPW_AvailableFundsUpper].Text.NotEqual[${_current_funds}]}
    /varset _current_funds ${Window[MarketplaceWnd].Child[MKPW_AvailableFundsUpper].Text}
    /invoke ${out[0, "In Game Cash${sep}${num}${_current_funds}\ax"]}

    /if (${Window[MarketplaceWnd]}) /market

  }

/return TRUE



|***
 * note: checks whos hurt, returns a count. sets most hurt: ID, Class, pct hurt
 * use: /call get_hurt DEBUG [group|pet|mypet|xtarget|tot|CH|reset] || ${get_hurt[DEBUG, group|pet|mypet|xtarget|tot|ch|reset]}
 ***|
sub get_hurt(bool _debug, string _type, bool _breakonGroup)
  DEBUG \atget_hurt\ax(\a-w${_type}\ax)
  DEBUG ${sep}using weighted HP check${sep}\a-w${maHeal.Find[swHealWeighted].Value}\ax

  /declare _count int local 0
  /varset hurtID ${Me.ID}
  /varset hurtPctHP 100
  /varset hurtCLS ${Me.Class.ShortName}
  /varset hurtCount 0
  /varset hurtDead 0
  /declare _hurtWeight float local 99.01
  /declare _tmpWeight float local 99.02

  | we just take defaults of no one hurt and exit.
  /if (${_type.Equal[reset]}) {
    /return 0

  | check group hurt
  } else /if (${_type.Equal[group]}) {
    DEBUG \a-w{_type.Equal[group]}\ax

    /for _count 0 to ${Group.Members}
      DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}${Group.Member[${_count}].Name}${sep}${Group.Member[${_count}].ID}\ax
      /if (${Group.Member[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}*.PCTHO) /continue
      /if (${Group.Member[${_count}].Distance} > ENV) /continue
      /if (!${Group.Member[${_count}].Present}) /continue
      /if (${Group.Member[${_count}].Dead}) {
        /varcalc hurtDead ${hurtDead} + 1
        /continue
      }
      /if (${Group.Member[${_count}].DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) /continue
      /if (${lsNoHealToon.Contains[${Group.Member[${_count}].DisplayName}]}) /continue

      | group hurt running count
      /if (${Group.Member[${_count}].PctHPs} <= ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}*.PCTHO) {
        /varcalc hurtCount ${hurtCount} + 1
        /if (${maHeal.Find[stCountHealGroup].Value} && ${_breakonGroup}) {
          DEBUG ${sep}break on${sep}\a-w${hurtCount}\ax \aw>=\ax \a-w${maHeal.Find[stCountHealGroup].Value}\ax
          /if (${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) {
            /return ${hurtCount}
          }
        }
      }

      /if (${maHeal.Find[swHealWeighted].Value}) {
        /varset _tmpWeight ${Float[${Math.Calc[(${Group.Member[${_count}].PctHPs} - .${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}) / ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}].Value}]}].Precision[5]}
        DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}_tmpWeight${sep}\a-w${_tmpWeight}\ax
        /if (${Float[${_tmpWeight}].Precision[5]} < ${Float[${_hurtWeight}].Precision[5]}) {
          /varset _hurtWeight ${Float[${_tmpWeight}].Precision[5]}
          /varset hurtPctHP ${Group.Member[${_count}].PctHPs}
          /varset hurtID ${Group.Member[${_count}].ID}
          /varset hurtCLS ${Group.Member[${_count}].Class.ShortName}
        }
      } else /if (!${maHeal.Find[swHealWeighted].Value}) {
        /varset hurtPctHP ${Group.Member[${_count}].PctHPs}
        /varset hurtID ${Group.Member[${_count}].ID}
        /varset hurtCLS ${Group.Member[${_count}].Class.ShortName}
      }

    /next _count
    /return ${hurtCount}


  | check xtarget hurt
  } else /if (${_type.Equal[xtarget]}) {
    DEBUG \a-w{_type.Equal[xtarget]}\ax
    /for _count 1 to ${Me.XTargetSlots}
      /if (!${Me.XTarget[${_count}].ID}) /continue
      /if (${Me.XTarget[${_count}].Dead}) /continue
      /if (${Me.XTarget[${_count}].PctHPs} > ${hurtPctHP}) /continue
      /if (${Me.XTarget[${_count}].Distance} > ENV) /continue
      /if (${Me.XTarget[${_count}].DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) /continue
      /if (${lsNoHealToon.Contains[${Me.XTarget[${_count}].DisplayName}]}) /continue
      /if (!${Me.XTarget[${_count}].Type.Equal[PC]} && !${lsProtectOutsideGroup.Contains[${Me.XTarget[${_count}].DisplayName}]}) /continue
      /if (${Me.XTarget[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}*.PCTHO) /continue
      /varset _tmpWeight ${Float[${Math.Calc[(${Me.XTarget[${_count}].PctHPs} - .${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}) / ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}].Value}]}].Precision[5]}

      /if (${Float[${_tmpWeight}].Precision[5]} < ${Float[${_hurtWeight}].Precision[5]}) {
        /varset _hurtWeight ${Float[${_tmpWeight}].Precision[5]}
        /varset hurtID ${Me.XTarget[${_count}].ID}
        /varset hurtPctHP ${Me.XTarget[${_count}].PctHPs}
        /varset hurtCLS ${Me.XTarget[${_count}].Class.ShortName}
        /varset hurtCount 1
      }

    /next _count
    /return ${hurtCount}


  | check pet hurt
  } else /if (${_type.Equal[pet]}) {
    /for _count 0 to ${Group}
      /if (!${Group.Member[${_count}].Pet.ID}) /continue
      /if (${Group.Member[${_count}].Pet.PctHPs} > ${maHeal.Find[stHealPoint${Group.Member[${_count}].Pet.Class.ShortName}].Value}) /continue
      /if (${Group.Member[${_count}].Pet.Distance} > ENV) /continue
      /if (${Group.Member[${_count}].Pet.PctHPs} >= ${hurtPctHP}) /continue

      /varset hurtCount 1
      /varset hurtID ${Group.Member[${_count}].Pet.ID}
      /varset hurtPctHP ${Group.Member[${_count}].Pet.PctHPs}
      /varset hurtCLS ${Group.Member[${_count}].Pet.Class.ShortName}
    /next _count
    /return ${hurtCount}


  | self only pet hurt
  } else /if (${_type.Equal[mypet]}) {
    /if (!${Me.Pet.ID}) /return FALSE
    /if (${Me.Pet.PctHPs} >= ${maHeal.Find[stHealPoint${Me.Pet.Class.ShortName}].Value}) {
      /return 0
    }
    /if (${Me.Pet.Distance} > ENV) {
      /return 0
    }

    /varset hurtID ${Me.Pet.ID}
    /varset hurtPctHP ${Me.Pet.PctHPs}
    /varset hurtCLS Pet
    /if (${hurtPctHP} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /return 0
    }
    /varset hurtCount 1
    /return 1

  | set self hurt
  } else /if (${_type.Equal[self]}) {
    /varset hurtID ${Me.ID}
    /varset hurtPctHP ${Me.PctHPs}
    /varset hurtCLS ${Me.Class.ShortName}

    /if (${hurtPctHP} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /return 0
    }
    /varset hurtCount 1
    /return 1


  | target of target hurt
  } else /if (${_type.Equal[tot]}) {
    /varset hurtID ${Me.TargetOfTarget.ID}
    /varset hurtPctHP ${Me.TargetOfTarget.PctHPs}
    /varset hurtCLS ${Me.TargetOfTarget.Class.ShortName}
    DEBUG ${sep}hurtID:\a-w${hurtID}\ax ${dot} hurtPctHP:\a-w${hurtPctHP}\ax ${dot} hurtCLS:\a-w${hurtCLS}\ax

    DEBUG ${hurtPctHP} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO
    /if (${hurtPctHP} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /return 0
    }
    /varset hurtID ${Target.ID}
    /varset hurtCount 1

    DEBUG ${sep}hurtID:\a-w${hurtID}\ax ${dot} hurtPctHP:\a-w${hurtPctHP}\ax ${dot} hurtCLS:\a-w${hurtCLS}\ax

    /return 1


  | Cleric Complete Heal hurt check
  } else /if (${_type.Equal[complete]}) {
    DEBUG \a-w{_type.Equal[complete]}\ax

    DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}${Group.MainTank.Name}${sep}${Group.MainTank.ID}\ax
    /if (${Group.MainTank.PctHPs} > ${maHeal.Find[stPctCH].Value}*.PCTHO) /return 0
    /if (${Group.MainTank.Distance} > ENV) /return 0
    /if (!${Group.MainTank.Present} || ${Group.MainTank.Dead}) /return 0

    /return 1


  | dannet healing
  } else /if (${_type.Equal[DanNet]}) {

    /declare _toonName string local

    | each toon
    /for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}

      | get the name to something usable
      /varset _toonName ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
      DEBUG ${sep}_toonName${sep}\ay${_toonName}\ax

      | is the observer set
      /if (!${Defined[${_toonName}_PctHPs]}) {
        DEBUG ${break}defined${sep}!\a-w${_toonName}_PctHPs\ax
        | /varcalc _countToon ${_countToon}+1
        /continue
      }

      DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}${Spawn[pc ${_toonName}].DisplayName}${sep}${Spawn[pc ${_toonName}].ID}\ax
      /if (${Spawn[pc ${_toonName}].PctHPs} > ${maHeal.Find[stHealPoint${Spawn[pc ${_toonName}].Class.ShortName}].Value}*.PCTHO) /continue
      /if (${Spawn[pc ${_toonName}].Distance} > ENV) /continue
      /if (${Spawn[pc ${_toonName}].Dead}) {
        /varcalc hurtDead ${hurtDead} + 1
        /continue
      }
      /if (!${Spawn[pc ${_toonName}].ID}) /continue

      /if (${Spawn[pc ${_toonName}].DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) /continue

      | group hurt running count
      /if (${Spawn[pc ${_toonName}].PctHPs} <= ${maHeal.Find[stHealPoint${Spawn[pc ${_toonName}].Class.ShortName}].Value}*.PCTHO) {
        /varcalc hurtCount ${hurtCount} + 1
      }

      /if (${maHeal.Find[swHealWeighted].Value}) {
        DEBUG ${sep}_count${sep}\a-w${_toonName}\ax${sep}\a-o(${Spawn[pc ${_toonName}].PctHPs} / ${maHeal.Find[stHealPoint${Spawn[pc ${_toonName}].Class.ShortName}].Value}) - .${maHeal.Find[stHealPoint${Spawn[pc ${_toonName}].Class.ShortName}].Value}\ax
        /varset _tmpWeight ${Float[${Math.Calc[(${Spawn[pc ${_toonName}].PctHPs} / ${maHeal.Find[stHealPoint${Spawn[pc ${_toonName}].Class.ShortName}].Value}) - .${maHeal.Find[stHealPoint${Spawn[pc ${_toonName}].Class.ShortName}].Value}]}].Precision[5]}
        DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}_tmpWeight${sep}\a-w${_tmpWeight}\ax
        /if (${Float[${_tmpWeight}].Precision[5]} < ${Float[${_hurtWeight}].Precision[5]}) {
          /varset _hurtWeight ${Float[${_tmpWeight}].Precision[5]}
          /varset hurtPctHP ${Spawn[pc ${_toonName}].PctHPs}
          /varset hurtID ${Spawn[pc ${_toonName}].ID}
          /varset hurtCLS ${Spawn[pc ${_toonName}].Class.ShortName}
        }
      } else /if (!${maHeal.Find[swHealWeighted].Value}) {
        /varset hurtPctHP ${Spawn[pc ${_toonName}].PctHPs}
        /varset hurtID ${Spawn[pc ${_toonName}].ID}
        /varset hurtCLS ${Spawn[pc ${_toonName}].Class.ShortName}
      }

    /next _count
    /return ${hurtCount}


  }

/return 0



|***
 * note: checks facing of a target
 * use: /call check_facing DEBUG [TARGET ID] [adjust position FORCE/FALSE]
 ***|
sub check_facing(bool _debug, int _tmpID, string _adjust)
  DEBUG \atcheck_facing\ax(\a-w${_tmpID}, ${_adjust}\ax)

  | no target == bail
  /if (!${_tmpID}) {
    DEBUG ${break}._tmpID
    /return FALSE
  }

  /if (!AUTO && !${swModeToken}) /return FALSE

  | /if (${Stick.Active}) /return FALSE

  /if (!${Range.Between[0,${maHome.Find[stFaceArc].Value}:${Math.Abs[${Math.Calc[${Target.HeadingTo.Degrees}-${Me.Heading.Degrees}]}]}]}) {
    /squelch /face id ${_tmpID} nolook ${If[${maHome.Find[swFaceFast].Value},fast,]}
  }

  /if (${_adjust.Equal[FORCE]}) {
    /declare _mobdist float local ${Spawn[id ${_tmpID}].Distance}

    /if ((${_mobdist} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value} && ${_mobdist} < ${Spawn[id ${_tmpID}].MaxRangeTo}+1) || ${_mobdist} < ${Spawn[id ${_tmpID}].MaxRangeTo}*.2) {
      /squelch /stick moveback ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
    }
  }

/return TRUE



|***
 * note: is something ready to be used? i dont GAF what it is, is it ready?
 * use: ${is_ready[DEBUG, "Thingtocheck"]}
 ***|
sub is_ready(bool _debug, string _verbage)
  DEBUG \atis_ready\ax(\a-w"${_verbage}"\ax)

  /if (!${Bool[${_verbage}]} || ${_verbage.Equal[NULL]}) {
    /return FALSE
  }

  | alt ability
  /if (${Me.AltAbilityReady[${_verbage}]}) {
    DEBUG ${sep}\a-yAA READY\ax${sep}\a-w${_verbage}\ax
    /return TRUE

  | spell (song)
  } else /if (${Me.Book[${Spell[${_verbage}].RankName}]}) {

    | dont fail buffs so they can be memed when needed
    /if (!${Me.SpellReady[${Spell[${_verbage}].RankName}]} && !${Spell[${Spell[${_verbage}].RankName}].Category.Equal[Heals]}) {
      /if (!${lsFreePassSpells.Contains[${Spell[${Spell[${_verbage}].RankName}].SpellType}]}) /return FALSE
    }

    /if (!${Me.SpellReady[${Spell[${_verbage}].RankName}]}) /return FALSE
    DEBUG ${sep}\a-ySPELL READY\ax${sep}\a-w${Spell[${_verbage}].RankName}\ax
    /return TRUE

  | disc
  } else /if (${Me.CombatAbilityReady[${Me.CombatAbility[${Spell[${_verbage}].RankName}]}]}) {
    DEBUG ${sep}\a-yDISC READY\ax${sep}\a-w${Spell[${_verbage}].RankName}\ax
    /return TRUE

  | ability
  } else /if (${Me.AbilityReady[${_verbage}]}) {
    DEBUG ${sep}\a-ABILITY READY\ax${sep}\a-w${_verbage}\ax
    /return TRUE

  | item
  } else /if (${Me.ItemReady[${_verbage}]} && !${FindItem[=${_verbage}].Timer}) {
    DEBUG ${sep}\a-yITEM READY\ax${sep}\a-w${_verbage}\ax
    /return TRUE

  }

  DEBUG ${sep}\a-rNOT READY\ax

| blanket fail
/return FALSE



|***
 * note: finds youre current assisst and returns their info Name, ID, or both
 * use: ${get_assist_info[DEBUG, name|id|both]}
 ***|
sub get_assist_info(bool _debug, string _type)
  DEBUG \atget_assist_info\ax(\a-w${_type}\ax)

  /declare _assist local string FALSE

  | healers or CC that may not have an assst set
  /if (${maCC.Find[stAssistMode].Value.Equal[FALSE]} && (${maCC.Find[swCombatHeal].Value} || ${maCC.Find[swCombatControl].Value})) {

    /if (${Raid}) {
      DEBUG ${sep}assist found${sep}\a-w${Spawn[pc ${Raid.MainAssist}].DisplayName}\ax
      /varset _assist ${Spawn[pc ${Raid.MainAssist}].DisplayName}
    } else /if (!${Raid}) {
      DEBUG ${sep}assist found${sep}\a-w${Spawn[pc ${Group.MainAssist}].DisplayName}\ax
      /varset _assist ${Spawn[pc ${Group.MainAssist}].DisplayName}
    }
  }

  | wtf are we assisting
  /if (${maCC.Find[stAssistMode].Value.NotEqual[FALSE]}) {

    /if (${Group.MainAssist.Name.Equal[${Me.DisplayName}]}) {
      DEBUG ${sep}assist found${sep}\a-w${Me.DisplayName}\ax
      /varset _assist ${Me.DisplayName}
    } else /if (${maCC.Find[stAssistMode].Value.Equal[G]} && ${Spawn[${Group.MainAssist.ID}].ID}) {
      DEBUG ${sep}assist found${sep}\a-w${Spawn[pc ${Group.MainAssist}].DisplayName}\ax
      /varset _assist ${Spawn[pc ${Group.MainAssist}].DisplayName}
    } else /if (${maCC.Find[stAssistMode].Value.Equal[1]}) {
      DEBUG ${sep}assist found${sep}\a-w${Spawn[pc ${Raid.MainAssist}].DisplayName}\ax
      /varset _assist ${Spawn[pc ${Raid.MainAssist}].DisplayName}
    } else /if (${maCC.Find[stAssistMode].Value.Equal[2]}) {
      DEBUG ${sep}assist found${sep}\a-w\ax
    } else /if (${maCC.Find[stAssistMode].Value.Equal[3]}) {
      DEBUG ${sep}assist found${sep}\a-w\ax
    } else /if (${Bool[${maCC.Find[stManualAssist1].Value}]} && ${Spawn[pc ${maCC.Find[stManualAssist1].Value}].ID} && !${Spawn[${maCC.Find[stManualAssist1].Value}].Dead}) {
      DEBUG ${sep}assist found${sep}\a-w${Spawn[${maCC.Find[stManualAssist1].Value}].DisplayName}\ax
      /varset _assist ${Spawn[pc ${maCC.Find[stManualAssist1].Value}].DisplayName}
    } else /if (${Bool[${maCC.Find[stManualAssist2].Value}]} && ${Spawn[${maCC.Find[stManualAssist2].Value}].ID} && !${Spawn[${maCC.Find[stManualAssist2].Value}].Dead}) {
      DEBUG ${sep}assist found${sep}\a-w${Spawn[${maCC.Find[stManualAssist2].Value}].DisplayName}\ax
      /varset _assist ${Spawn[${maCC.Find[stManualAssist2].Value}]}
    }
  }

  | return the assist info requested
  /if (${_type.Equal[name]}) {
    DEBUG ${sep}returning assist${sep}\a-w${_assist}\ax
    /return ${_assist}
  } else /if (${_type.Equal[id]}) {
    DEBUG ${sep}returning assist${sep}\a-w${Spawn[PC ${_assist}].ID}\ax
    /return ${Spawn[PC ${_assist}].ID}
  } else /if (${_type.Equal[both]}) {
    DEBUG ${sep}returning assist${sep}\a-w|${_assist}|${Spawn[PC ${_assist}].ID}\ax
    /return |${_assist}|${Spawn[PC ${_assist}].ID}
  } else {
    DEBUG ${sep}returning assist${sep}\arFALSE\ax
    /invoke ${out[0, 0, 61]}
    /return FALSE
  }

/return FALSE



|***
 * note: creates observers
 * use: /call set_observers DEBUG [map of things to observe] [add|drop] [silent T/F]
 ***|
sub set_observers(bool _debug, string _map, string _action, bool _silent)
  DEBUG \atset_observers\ax(\a-w${_map}, ${_action}, ${_silent}\ax)

  /declare _count           int     local 1
  /declare _actionO         string  local FALSE
  /declare _toon            string  local FALSE
  /declare _timestart       int     local 0
  /declare _timeend         int     local 0
  /declare _tlo             string  local ${${_map}.Find[tlo].Value}
  /declare _miso            mapiterator local

  /invoke ${set_data_timer[${_debug}, Check_Observers, RESTART]}

  | correct/check the action variable
  /if (${_action.Equal[drop]}) {
    /varset _actionO -drop
  } else /if (${_action.Equal[add]}) {
    /varset _actionO
  } else {
    /invoke ${out[0, "_action \arMISSING\ax for set_observer(${_map}) call"]}
    /return FALSE
  }

  | do all the peoples!
  /while (${_count} <= ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}) {
    /varset _timestart ${MacroQuest.Running}

    | skip toons that are not here
    | /if (!${Spawn[${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}].ID}) {
    |   DEBUG ${break}nospawn
    |   /varcalc _count ${_count}+1
    |   /continue
    | }

    | cure group only
    | /if (${_map.Equal[maCure]} && ${maHeal.Find[swHealCureGroup].Value} && !${maHeal.Find[swHealCureCrew].Value}) {
    |   /if (!${Group.Member[${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}].ID}) /continue
    | }

    | were going to use the dannet default all group to get names from. can be whatever you like tho
    /varset _toon ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}

    | start the list of things we will be observing
    /if (!${Defined[lsObserved_${_toon}]}) {
      /declare lsObserved_${_toon} list outer
      /invoke ${lsObserved_${_toon}.Delimiter["|"]}
    }
    DEBUG ${sep}_toon\aw ${_count}\ax::\at${_toon}\ax

    | start our trip throught the map
    /vardata _miso ${_map}.First.Clone
    /while (!${_miso.IsEnd}) {
      | we're skipping the TLO for monitoring
      /if (${_miso.Key.Equal[tlo]}) {
        /invoke ${_miso.Advance}
        /continue
      }

      DEBUG ${sep}map(key,value)${sep}${_map}(\a-w${_miso.Key},${_miso.Value}\ax)

      | define or delete the variables as we need
      /if (${_action.Equal[drop]}) {
        DEBUG ${sep}/invoke lsObserved_${_toon}.Remove[${_miso.Value}]
        /if (${Defined[${_toon}_${_miso.Value}]}) {
          /deletevar ${_toon}_${_miso.Value}
        }
        /invoke ${lsObserved_${_toon}.Remove[${_miso.Value}]}

      } else /if (${_action.Equal[add]}) {
        | if what we are observing isnt in the charcters list, add it
        /if (!${lsObserved_${_toon}.Contains[${_miso.Value}]}) {
          /invoke ${lsObserved_${_toon}.Append[${_miso.Value}]}
        }
        | if the observer variable does not exist create it
        /if (!${Defined[${_toon}_${_miso.Value}]}) {
          /declare ${_toon}_${_miso.Value} string outer
        }
      }

      | set observer or drop it. depending on your choice
      DEBUG ${sep}\a-t/dobserve\ax ${_toon} -q \a-w"${_tlo}.${_miso.Value}"\ax -o ${_toon}_${_miso.Value} ${_actionO}
      /dobserve ${_toon} -q "${_tlo}.${_miso.Value}" -o ${_toon}_${_miso.Value} ${_actionO}

      | increment to the next item in the map and start loop over if theres more to do
      /invoke ${_miso.Advance}

    }
    | set an end time for our test for that characer
    /varset _timeend ${MacroQuest.Running}
    | give us some response so we know that something is happening
    /if (!${_silent}) {
      /invoke ${out[17, "Observed(${If[${_action.Equal[add]},\agadd\ax,\ardrop\ax]})${sep}${_map}${sep}\aw${_toon}\ax${sep}\a-w${Math.Calc[((${_timeend}-${_timestart}) / 10) / 60]}s\ax"]}
    }

    | moving to the next toon in the dannet all default channel
    /varcalc _count ${_count}+1
  }

/return



|***
 * note: basic 3-way switch control for environment settings for TRUE/FALSE
 * use: /call set_switch_env DEBUG [switchname] [SQL] [value] || ${set_switch_env[DEBUG, switchname, SQL T/F, value]}
 ***|
sub set_switch_env(bool _debug, string _switch, bool _sql, string _verbage)
  DEBUG \atset_switch_env\ax(\a-w${_switch}, ${_sql}, ${_verbage}\ax)

  /declare _map string local ${get_key_map[${_debug}, ${_switch}]}

  DEBUG ${sep}_map${sep}${_map}

  | set switch TRUE
  /if (${_verbage.Equal[on]} || ${_verbage.Equal[TRUE]}) {
    DEBUG ${sep}on
    /invoke ${${_map}.Add[${_switch},TRUE]}

  | set switch FALSE
  } else /if (${_verbage.Equal[off]} || ${_verbage.Equal[FALSE]}) {
    DEBUG ${sep}off
    /invoke ${${_map}.Add[${_switch},FALSE]}

  | swap the switch status
  } else /if (!${_verbage.Length} || (!${Bool[${_verbage}]} && ${_verbage.NotEqual[NULL]})) {
    DEBUG ${sep}swapping${sep}\a-w{${_map}.Add[${_switch},${If[${${_map}.Find[${_switch}].Value},FALSE,TRUE]}]}\ax
    /invoke ${${_map}.Add[${_switch},${If[${${_map}.Find[${_switch}].Value},FALSE,TRUE]}]}

  }

  | update database?
  /if (${_sql}) {
    /declare _DBLOC string local FALSE
    /varset _DBLOC ${get_key_DBLOC[${_debug}, ${_switch}]}
    DEBUG ${sep}dummy UPDATE ${_DBLOC.Arg[1,|]} SET value='${${_map}.Find[${_switch}].Value}' WHERE ${_DBLOC.Arg[2,|]}='${_switch}'
    /sqlite query "${DBcharacter}" dummy UPDATE ${_DBLOC.Arg[1,|]} SET value='${${_map}.Find[${_switch}].Value}' WHERE ${_DBLOC.Arg[2,|]}='${_switch}';
    /if (!${sql_check[${_debug}, dummy, set_switch_env, "UPDATE ${_DBLOC.Arg[1,|]}"]}) /endmacro
  }

/return ${${_map}.Find[${_switch}].Value}



|***
 * note: sorts the list of classes, if one doesnt match. its out...
 * use: ${check_class[DEBUG, |CLS|CLS]}
 ***|
sub check_class(bool _debug, string _class)
  /declare _pass bool local FALSE
  /declare _count int local 0

  /for _count 1 to ${_class.Count[|]}
    /if (${Me.Class.ShortName.Upper.Equal[${_class.Arg[${_count},|]}]}) {
      /varset _pass TRUE
      /break
    }
  /next _count

/return ${_pass}



|***
 * note: this is for waisting AAs. it does nothing more. if you expect it to do more, go away.
 * use: /call cast_AA_fireworks()
 ***|
sub cast_AA_fireworks(bool _debug)
  DEBUG \atcast_AA_fireworks\ax()

  /invoke ${set_data_timer[FALSE, Waste_Fireworks, RESTART]}

  /if (!${maEnv.Find[swAAFireworks].Value} || ${Raid.Members}) /return
  /if (${Me.AAPoints} < 20) /return

  /if (${Me.AAPoints} >= 120 && !${Me.AltAbilityReady[Glyph of Fireworks II]}) {
    /alt buy 897
    /invoke ${set_data_timer[FALSE, Waste_Fireworks, 1m]}
    /return
  } else /if (${Me.AltAbilityReady[Glyph of Fireworks II]}) {
    /invoke ${out[1, 0, 62]}
    /call voodoo ${_debug} "Glyph of Fireworks II" 0 FALSE
  }

/return



|***
 * note: watches for the rez box if we have been marked as dead or called.
 * use: /call check_for_rezbox()
 ***|
sub check_for_rezbox(bool _debug)
  DEBUG \atcheck_for_rezbox\ax()

  /doevents REZ_ACCEPT

  GETINPUT rez_box_found swIsMeDead|${swIsMeDead}

  | dont rezz me
  /if (!${maRez.Find[swRezTake].Value}) {
    /return FALSE
  }

  | not dead? GO AWAY
  /if (!${swIsMeDead}) {
    /return FALSE
  }

  | bail if no rez box to check
  /if (!${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["Do you wish this?"]}]}) {
    /return FALSE
  }

  | is there a rez window?
  /if (${Window[ConfirmationDialogBox].Open}) {

    /declare _stRezWindowPct string local FALSE

    | get rez window % to something we can use
    /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["Do you wish this?"]}]}) {

      /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(0 percent)"]}]}) {
        /varset _stRezWindowPct 0
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(10 percent)"]}]}) {
        /varset _stRezWindowPct 10
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(20 percent)"]}]}) {
        /varset _stRezWindowPct 20
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(35 percent)"]}]}) {
        /varset _stRezWindowPct 35
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(50 percent)"]}]}) {
        /varset _stRezWindowPct 50
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(60 percent)"]}]}) {
        /varset _stRezWindowPct 60
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(75 percent)"]}]}) {
        /varset _stRezWindowPct 75
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(90 percent)"]}]}) {
        /varset _stRezWindowPct 90
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(96 percent)"]}]}) {
        /varset _stRezWindowPct 96
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(100 percent)"]}]}) {
        /varset _stRezWindowPct 100
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["you will still be able to get a resurrection later"]}]}) {
        /if (${maRez.Find[swRezTakeCall].Value}) {
          /varset _stRezWindowPct 100
        } else {
          /varset _stRezWindowPct -1
        }
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["${Me.DisplayName}"]}]}) {
        /varset _stRezWindowPct 100
      }
    }

    DEBUG  ${sep}_stRezWindowPct ${_stRezWindowPct}

    /declare _swValidRezFound bool local FALSE

    /if (${maRez.Find[stPctMinRez].Value} <= ${_stRezWindowPct}) {
      /varset _swValidRezFound TRUE
    }

    DEBUG ${sep}_swValidRezFound${sep}\a-w${_swValidRezFound}\ax

    | more waiting
    /if (${_swValidRezFound}) {
      /varset swIsMeDead FALSE
      /delay 1s
      /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
      /delay 5
      /if (${Window[RespawnWnd].Open}) {
        /if (${Zone.ShortName.Equal[Powar]}) {
          /nomodkey /notify RespawnWnd RW_OptionsList listselect 1
        } else {
          /nomodkey /notify RespawnWnd RW_OptionsList listselect 2
        }
        /delay 1s
        /nomodkey /notify RespawnWnd RW_SelectButton leftmouseup
      }

      | Clear ALL THE MARKERS!
      /varset stAssTarID 0
      /invoke ${maCC.Add[swValidAssTarID,FALSE]}
      /invoke ${maCC.Add[swSetCombat,FALSE]}
      /invoke ${maCC.Add[swSetForced,FALSE]}
      /invoke ${set_data_timer[FALSE, Check_Behind, 0]}

      /varset swIsMeDead FALSE
      /varset _swValidRezFound FALSE
      /doevents REZ_ACCEPT

      /call set_stop ${maDebug.Find[stop].Value} SILENT

      /if (${maHome.Find[swHomeMarker].Value}) {
        /call Bind_control_home clear NULL NULL ${maDebug.Find[home].Value}
      }

      /return TRUE
    } else /if (!${_swValidRezFound}) {
      /nomodkey /notify ConfirmationDialogBox No_Button leftmouseup
      /delay 5
      /return FALSE
    }
  }

/return FALSE



|***
 * note: what we have here is one large ass
 * use: /call check_buff_cycle_shrink DEBUG
 ***|
sub check_buff_cycle_shrink(bool _debug)
  DEBUG \atcheck_buff_cycle_shrink\ax()

  /if (!${maBuff.Find[swBuffShrink].Value}) {
    /return FALSE
  }


  SETHUD "buff shrink"

  /if (AUTO) {
    /invoke ${set_data_timer[FALSE, Check_Buff_Shrink, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_shrink

  GETINPUT check_buff_cycle_shrink NA|NA

  /if (!${is_ready[${_debug}, "${maBuff.Find[stShrinkItem].Value}"]}) {
    /return FALSE
  }

  /declare _count int local 0

  | me shrinks
  /while (${Me.Height} > ${maBuff.Find[stShrinkHeight].Value} && ${_count} < 3) {
    /call voodoo ${_debug} "${maBuff.Find[stShrinkItem].Value}" ${Me.ID} FALSE
    /varcalc _count ${_count}+1
    /delay 5
  }

  SETHUD EMPTY

/return TRUE



|***
 * note: this casts things. it doesn't care what it casts, but it will cast it.
 * note: ORDER of selections: ALT, SPELL, DISC, ITEM, SONG
 * note:
 * note: best to wrap or prep the cast in an ${is_ready[]}
 * note:
 * use: /call voodoo DEBUG r [TARGETID] [sub to check on the way] [ENV override]
 ***|
sub voodoo(bool _debug, string _verbage, int _tmpID, string _sub, int _rangeOverride)
  DEBUG \atvoodoo\ax(\a-w"${_verbage}", ${_tmpID}, ${_sub}, ${_rangeOverride}\ax)
  ISMEDEAD

  | critical errors
  /if (!${Bool[${_verbage}]}) {
    /invoke ${out[0, "${error}${sep}voodoo${sep}\ar_verbage\ax${sep}\a-w${_verbage}, ${_tmpID}, ${_sub}\ax)"]}
    /delay 5
    /return FALSE
  }

  | generic validations
  /if (${Me.Invis} && ${_sub.NotEqual[ignoreall]}) {
    /return FALSE
  }

  | /if (AUTO && ${swIsMeDead}) {
  |   /return FALSE
  | } else 
  /if (!AUTO && ${swIsMeDead}) {
    /varset swIsMeDead FALSE
    DEBUG ${sep}passed swIsMeDead
  }


  | are we moving? and not a bard?
  /if (!${Select[${Me.Class.ShortName},BRD]} && ${Me.Moving} && ${_sub.NotEqual[ignoremovement]}) {
    DEBUG ${sep}break voodoo.moving
    /if (${Me.Moving}) /return FALSE
  }

  | correct the range if needed
  /declare _range int local
  /if (${Range.Between[1,9999999:${_rangeOverride}]}) {
    DEBUG ${sep}setting max range to${sep}\a-w${_rangeOverride}\ax
    /varset _range ${_rangeOverride}
  } else {
    /varset _range ENV
  }

  DEBUG ${sep}range${sep}\a-w${_range}\ax

  | is the issued target still a thing???
  /if (${_tmpID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
    /if (!${Spawn[${_tmpID}].ID} || ${Spawn[${_tmpID}].Dead}) /return FALSE
    /if (${Spawn[${_tmpID}].Distance} > ${_range}) /return FALSE
  }

  DEBUG ${sep}type select

  /declare _type string local FALSE

      | what are we dealing with ???
      | Alternate ability
      /if (${Me.AltAbilityReady[${_verbage}]}) {
        DEBUG _type${sep}\a-wALT\ax
        /varset _type ALT

      | bard song
      } else /if (${Select[${Me.Class.ShortName},BRD]} && ${Me.Book[${Spell[${_verbage}].RankName}]}) {
        DEBUG _type${sep}\a-wPULSE\ax
        /varset _type PULSE

      | spell
      } else /if (${Me.Book[${Spell[${_verbage}].RankName}]}) {
        DEBUG _type${sep}\a-wSPELL\ax
        /varset _type SPELL

      | melee disc
      } else /if (${Me.CombatAbilityReady[${Me.CombatAbility[${Spell[${_verbage}].RankName}]}]}) {
        DEBUG _type${sep}\a-wDISC\ax
        /varset _type DISC

      | click item
      } else /if (${Me.ItemReady[=${_verbage}]}) {
        DEBUG _type${sep}\a-wITEM\ax
        /varset _type ITEM

      | we have no idea what this is!
      } else {
        DEBUG ${sep}FIND${sep}\arFAIL\ax${sep}${_verbage}${sep}\a-wnot found or not ready\ax
        /return FALSE
      }

      DEBUG ${sep}found ready${sep}\ay${_type}\ax${sep}\aw${_verbage}\ax

  /if (${Select[${_type},SPELL,DISC,PULSE]}) {
    /varset _verbage ${Spell[${_verbage}].RankName}
  }

  | set number of cast trys
  /declare _castLoop int local ${maEntropy.Find[stCastLoop].Value}


  /if (!${Defined[_castTime]}) {
    /declare _castTime int local 0
  }

  | alt abilities
  /if (${_type.Equal[ALT]}) {
    DEBUG ${sep}alt${sep}${_verbage}

    /if (${Me.CurrentMana} < ${Spell[${_verbage}].Mana}) {
      /return FALSE
    }

    | /if (${Spell[${_verbage}].SpellType.Equal[Beneficial]} && ${_tmpID} == 0) {
    |   /if (!${Spell[${AltAbility[${_verbage}].Spell.ID}].Stacks}) /return FALSE
    | }

    /if (${Me.Buff[${_verbage}].ID} && ${_sub.NotEqual[ignoreall]}) {
      /return FALSE
    }
    /if (${Me.Song[${_verbage}].ID} && ${_sub.NotEqual[ignoreall]}) {
      /return FALSE
    }

    /delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_tmpID}) {
      /invoke ${target[set, ${_tmpID}]}
    }

    /if (${_tmpID} == 0) {
      /invoke ${out[2, "AA${sep}${sp}${_verbage}\ax"]}
    } else /if (${_tmpID} != 0 && ${Target.ID}) {
      /invoke ${out[2, "AA${sep}${sp}${_verbage}\ax ${dot} ${con_color[FALSE, ${_tmpID}]}\ax"]}
    }

    /while (${_castLoop} > 0) {
      DEBUG ${sep}altCastLoop${sep}${_verbage}

      /if (${_tmpID} && ${Target.ID} != ${_tmpID}) {
        /break
      }

      /alt act ${Me.AltAbility[${_verbage}].ID}
      /delay 1s ${Me.Casting.ID}

        | set delays for casting. GTFO out of this as fast as possible
        | /delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}
        /varset _castTime ${Math.Calc[${Spell[${_verbage}].MyCastTime}+50]}
        DEBUG ${sep}_castTime${sep}\a-w${_castTime}\ax

        /invoke ${set_timer[${_debug}, _castTime, ${_castTime}]}

        /while (${_castTime} > 0) {
          /if (${_tmpID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
            /if (${Spawn[id ${_tmpID}].Dead} || !${Spawn[id ${_tmpID}].ID}) {
              DEBUG ${sep}spell.target dead.break
              /stopcast
              /return FALSE
            }
          }

          /if (!${Me.Casting.ID}) {
            /break
          }
          /delay 3

        }

        | delay for longer casts if there is any, cooldowns or open spell cast windows
        DEBUG ${sep}altloop.casting,window,cooldown
        /while (${Me.Casting.ID} || ${Window[CastingWindow].Open} || ${Me.SpellInCooldown}) {
          /if (!${Me.Casting.ID}) /break
          /if (!${Window[CastingWindow].Open}) /break
          /if (${Select[${Me.Class.ShortName},BRD]} && ${Me.SpellInCooldown} && !${Me.Casting.ID}) {
            /break
          }

          /delay 3
        }

      /doevents

      /if (${_sub.Equal[ignoreall]}) {
        /return TRUE
      }

      DEBUG ${sep}cast return${sep}\ay${maData.Find[stCastReturn].Value}\ax

      | ECHO maData.Find[stCastReturn].Value == ${maData.Find[stCastReturn].Value}


      /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,TAKEHOLD,DISTRACTED,RESISTED]}) {
        /varset _castLoop 0
        /if (${Select[${Me.Class.ShortName},BRD]}) {
          /call pulse_stop
        }
        /break
      }

      /if (${Select[${maData.Find[stCastReturn].Value},OVERWRITTEN]}) {
        /varset _castLoop 0
        /if (${Select[${Me.Class.ShortName},BRD]}) {
          /call pulse_stop
        }
        /break
      }

      /if (${Select[${maData.Find[stCastReturn].Value},NOTREADY]}) {
        /varcalc _castLoop ${_castLoop}-1
        /delay 2
      }

    }
    /return TRUE

  | spells
  } else /if (${_type.Equal[SPELL]}) {
    DEBUG ${sep}\agSPELL\ax${sep}${_verbage}

    | buff on me i shouldnt cast with?
    | /if (${check_no_cast_buff[FALSE]}) /return FALSE

    | am i casting a spell?
    /if (${Me.Casting.ID}) {
      /return FALSE
    }

    | global cooldown
    /if (${Me.SpellInCooldown}) {
      /return FALSE
    }

    | correct rank name
    /varset _verbage ${Spell[${_verbage}].RankName}

    | have it?
    /if (!${Bool[${Me.Book[${_verbage}]}]}) {
      /return FALSE
    }

    | gem in cooldown?
    /if (${Me.GemTimer[${_verbage}]}) {
      /return FALSE
    }

    | enough manna to cast?
    /if (${Me.CurrentMana} < ${Spell[${_verbage}].Mana}) {
      /return FALSE
    }

    | dont fail buffs so they can be memed when needed
    /if (!${Me.SpellReady[${_verbage}]} && !${Spell[${_verbage}].Category.Equal[Heals]}) {
      /declare _pass bool local FALSE
      /if (${Spell[${_verbage}].SpellType.Equal[Beneficial]}) /varset _pass TRUE
      /if (${Spell[${_verbage}].SpellType.Equal[Pet]}) /varset _pass TRUE
      /if (${Spell[${_verbage}].SpellType.Equal[aura]}) /varset _pass TRUE

      /if (!${_pass}) {
        DEBUG ${sep}FREE mem pass \arfailure\ax
        /return FALSE
      }
    }

    | check cast range
    /if (${Spell[${_verbage}].Range}) {
      /if (${Spawn[${_tmpID}].Distance} > ${Spell[${_verbage}].MyRange}) {
        /return FALSE
      }
    }
    | check AE range
    /if (${Spell[${_verbage}].TargetType.Equal[Group V2]}) {
      /if (${Spawn[${_tmpID}].Distance} > ${Spell[${_verbage}].AERange}) {
        /return FALSE
      }
    }

    | check for stacking issues
    | DEBUG ${sep}${Spell[${_verbage}].SpellType.Equal[Beneficial]} && !${Spell[${_verbage}].Category.Equal[Heals]} && !${Select[${Spell[${_verbage}].Category},Heals,Combat Inates]}
    /if (${_tmpID} == ${Me.ID} || ${_tmpID} == 0) {
      /if (${Spell[${_verbage}].SpellType.Equal[Beneficial]} && !${Select[${Spell[${_verbage}].Category},Heals,Combat Inates]}) {
        /if (!${Spell[${_verbage}].Stacks}) {
          /return FALSE
        }
        /if (${Me.Buff[${_verbage}].ID}) {
          /return FALSE
        }
        /if (${Me.Song[${_verbage}].ID}) {
          /return FALSE
        }
      }
    }


    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_tmpID}) {
      /invoke ${target[set, ${_tmpID}]}
    }

    | types vs zone control
    /if (!${Spell[${_verbage}].SpellType.Equal[Beneficial]}) {
      /if (${lsZoneNoDisease.Contains[${Target.DisplayName}]} && ${Spell[${_verbage}].ResistType.Equal[Disease]}) /return FALSE
      /if (${lsZoneNoMagic.Contains[${Target.DisplayName}]} && ${Spell[${_verbage}].ResistType.Equal[Magic]}) /return FALSE
      /if (${lsZoneNoPoison.Contains[${Target.DisplayName}]} && ${Spell[${_verbage}].ResistType.Equal[Poison]}) /return FALSE
      /if (${lsZoneNoFire.Contains[${Target.DisplayName}]} && ${Spell[${_verbage}].ResistType.Equal[Fire]}) /return FALSE
      /if (${lsZoneNoCold.Contains[${Target.DisplayName}]} && ${Spell[${_verbage}].ResistType.Equal[Cold]}) /return FALSE
      /if (${lsZoneNoCorruption.Contains[${Target.DisplayName}]} && ${Spell[${_verbage}].ResistType.Equal[Corruption]}) /return FALSE
    }

    | check any subs? yes.. no.. maybe??
    /if (${_sub.Equal[breakonhealpct]} && ${maHeal.Find[swBreakHealPCT].Value}) {
      DEBUG ${sep}${_sub}${sep}\a-wbreakonhealpct\ax
      /if (${Spawn[id ${_tmpID}].PctHPs} >= ${maHeal.Find[stHealPoint${Spawn[id ${_tmpID}].Class.ShortName}].Value}*.PCTHO) {
        /stopcast
        /return TURE
      }
    }

    | if a gom procs and a spell is ready, we hijack the existing cast and use the GOM spell in its place
    /if (${_tmpID} != 0 && ${Target.ID} && ${Me.Song[Gift of Mana].ID} && !${Select[${Me.Class.ShortName},CLR]} && COMBAT) {

      /declare _ligom listiterator local
      /vardata _ligom lsGoM.First.Clone
        /while (!${_ligom.IsEnd}) {
          /if (${_ligom.Value.Equal[empty]}) /break
          /if (${is_ready[${_debug}, "${_ligom.Value}"]}) {
            /varset _verbage ${Spell[${_ligom.Value}].RankName}
            /break
          }
        /invoke ${_ligom.Advance}
      }
    }

    /if (${Window[SpellBookWnd].Open}) {
      /invoke ${Window[SpellBookWnd].DoClose}
    }

    | mem spell if needed??
    /if (!${Me.Gem[${_verbage}]}) {
      /call set_spell_to ${_debug} TRUE "${_verbage}" ${maEnv.Find[stBuffGem].Value} TRUE
    }

    /if (${Spawn[id ${_tmpID}].Dead} && ${_sub.NotEqual[skiptargetcheck]}) {
      /return FALSE
    }
    
    | is the spell memed
    /if (${Me.Gem[${_verbage}]}) {

      | echos
      /if (${_tmpID} == 0) {
        /invoke ${out[3, "Cast${sep}${sp}${_verbage}\ax"]}
      } else /if (${_tmpID} != 0 && ${Target.ID} && ${Target.ID} == ${_tmpID}) {
        /invoke ${out[3, "Cast${sep}${sp}${_verbage}\ax ${dot} ${con_color[FALSE, ${_tmpID}]}\ax"]}
      }

      /while (${_castLoop} > 0) {

        /if (${Me.Invis} && ${_sub.NotEqual[ignoreall]}) {
          DEBUG ${sep}spell.cr:DISTRACTED(invis).break
          /varset _castLoop 0
          /invoke ${maData.Add[stCastReturn,DISTRACTED]}
          /break
        }

        /cast ${Me.Gem[${_verbage}]}
        /delay 1s ${Me.Casting.ID}

        | set delays for casting. GTFO out of this as fast as possible
        | /delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}
        /varset _castTime ${Math.Calc[${Spell[${_verbage}].MyCastTime}+50]}
        DEBUG ${sep}_castTime${sep}\a-w${_castTime}\ax

        /invoke ${set_timer[${_debug}, _castTime, ${_castTime}]}

        /while (${_castTime} > 0) {
          /if (${_tmpID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
            /if (${Spawn[id ${_tmpID}].Dead} || !${Spawn[id ${_tmpID}].ID}) {
              DEBUG ${sep}spell.target dead.break
              /stopcast
              /return FALSE
            }

            | break on heal % check
            /if (${_sub.Equal[breakonhealpct]} && ${maHeal.Find[swBreakHealPCT].Value}) {
              DEBUG ${sep}${_sub}${sep}\a-wbreakonhealpct\ax
              /if (${Spawn[id ${_tmpID}].PctHPs} >= ${maHeal.Find[stHealPoint${Spawn[id ${_tmpID}].Class.ShortName}].Value}*.PCTHO) {
                /stopcast
                /return TURE
              }
            }
          }

          /if (!${Me.Casting.ID}) {
            /break
          }
          /delay 3

        }

        | delay for longer casts if there is any, cooldowns or open spell cast windows
        DEBUG ${sep}castloop.casting,window,cooldown
        /while (${Me.Casting.ID} || ${Window[CastingWindow].Open} || ${Me.SpellInCooldown}) {
          /if (!${Me.Casting.ID}) /break
          /if (!${Window[CastingWindow].Open}) /break
          /if (!${Me.SpellInCooldown}) /break
          /delay 3
        }

        /doevents
        DEBUG ${sep}cast return${sep}\ay${maData.Find[stCastReturn].Value}\ax

        /if (${_sub.Equal[ignoreall]}) {
          /return TRUE
        }

        | notready/ cant cast
        /if (${Select[${maData.Find[stCastReturn].Value},DISTRACTED,NOTREADY,CANNOTSEE,OUTOFMANA,INTERRUPTED]}) {
          DEBUG ${sep}spell.cr:DISTRACTED,NOTREADY,CANNOTSEEOUTOFMANA,INTERRUPTED.\arbreak\ax
          /varset _castLoop 0
          /if (${Select[${Me.Class.ShortName},BRD]}) {
            /call pulse_stop
          }
          /break

        | good
        } else /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,BLOCKED,RESISTED,IMMUNE,FALSE]}) {
          DEBUG ${sep}spell.cr:SUCCESS,BLOCKED,RESISTED,IMMUNE.\agbreak\ax
          /varset _castLoop 0
          /if (${Select[${Me.Class.ShortName},BRD]}) {
            /call pulse_stop
          }
          /break

        }

      }

    }
    /return TRUE


  | discs
  } else /if (${_type.Equal[DISC]}) {
    DEBUG ${sep}\agDISC\ax${sep}${_verbage}
    /delay 1

    /if (${Me.CurrentEndurance} < ${Spell[${_verbage}].EnduranceCost}) /return FALSE
    /if (${Spawn[id ${_tmpID}].Distance} > ${Spell[${_verbage}].MyRange}) /return FALSE
    /if (${Bool[${Me.Song[${_verbage}].Duration}]}) /return FALSE
    /if (${Bool[${Me.Buff[${_verbage}].Duration}]}) /return FALSE

    | stacking
    /if (!${Spell[${_verbage}].Stacks}) /return FALSE

    | CHECK DISK DURATION VS ACTIVE DISC FOR STACKING ISSUES/ BLOCKED DISCS
    /if (${Me.ActiveDisc.Name.Equal[${_verbage}]}) /return FALSE

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_tmpID}) {
      /invoke ${target[set, ${_tmpID}]}
    }

    /if (${_tmpID} == 0) {
      /invoke ${out[4, "Disc${sep}${sp}${_verbage}\ax"]}
    } else /if (${_tmpID} != 0 && ${Target.ID}) {
      /invoke ${out[4, "Disc${sep}${sp}${_verbage}\ax ${dot} ${con_color[FALSE, ${_tmpID}]}\ax"]}
    }
    /disc ${Spell[${_verbage}].ID}

    /delay 5
    /while (${Me.Casting.ID} && ${Window[CastingWindow].Open}) {
      /delay 5
    }

    /if (${Select[${Me.Class.ShortName},BRD]}) {
      /call pulse_stop
    }
    /return TRUE


  | items
  } else /if (${_type.Equal[ITEM]}) {
    | /varset _debug TRUE
    DEBUG ${sep}\agITEM\ax${sep}${_verbage}

    /if (${_tmpID} == 0 || ${_tmpID} == ${Me.ID}) {
      | illusion proc check
      /if (${Me.Buff[${Spell[${FindItem[=${_verbage}].Illusion.SpellID}].Name}].ID}) /return FALSE
      | mount proc check
      /if (${Me.Buff[${Spell[${FindItem[=${_verbage}].Mount.SpellID}].Name}].ID}) /return FALSE
      | familiar proc check
      /if (${Me.Buff[${Spell[${FindItem[=${_verbage}].Familiar.SpellID}].Name}].ID}) /return FALSE
      | clickitem buffs
      /if (${Me.Buff[${Spell[${FindItem[=${_verbage}].Clicky.SpellID}].Name}].ID}) /return FALSE
      /if (${Me.Buff[${FindItem[=${_verbage}].Spell}].ID}) /return FALSE
      /if (${Me.Song[${FindItem[=${_verbage}].Spell}].ID}) /return FALSE
      /if (${Spawn[${_tmpID}].Distance} > ${Spell[${FindItem[=${_verbage}].Spell}].MyRange} && ${Spell[${FindItem[=${_verbage}].Spell}].Range} != 0) /return FALSE
      | /if (${Spell[${FindItem[=${_verbage}].Spell}].SpellType.Equal[Beneficial]} && !${Spell[${FindItem[=${_verbage}].Spell}].Stacks}) /return FALSE
    }

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_tmpID}) {
      /invoke ${target[set, ${_tmpID}]}
    }

    /if (${_tmpID} == 0 || ${_tmpID} == ${Me.ID}) {
      /if (${lsGrain.Contains[7]}) {
        OUT Item${sep}${sp}${_verbage}\ax
      }
    } else /if (${_tmpID} != 0 && ${Target.ID}) {
      /if (${lsGrain.Contains[7]}) {
        OUT Item${sep}${sp}${_verbage}\ax ${dot} ${con_color[FALSE, ${_tmpID}]}
      }
    }

    /while (${_castLoop}) {
      | DEBUG While
      /useitem "${_verbage}"
      /delay 2

      /if (${_tmpID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
        /if (${Spawn[id ${_tmpID}].Dead} || !${Spawn[id ${_tmpID}].ID}) {
          DEBUG ${sep}item.target dead.break
          /stopcast
          /return FALSE
        }

        | break on heal % check
        /if (${_sub.Equal[breakonhealpct]} && ${maHeal.Find[swBreakHealPCT].Value}) {
          DEBUG ${sep}${_sub}${sep}\a-wbreakonhealpct\ax
          /if (${Spawn[id ${_tmpID}].PctHPs} >= ${maHeal.Find[stHealPoint${Spawn[id ${_tmpID}].Class.ShortName}].Value}*.PCTHO) {
            /stopcast
            /return TURE
          }
        }
      }
      
      /if (!${Me.ItemReady[=${_verbage}]}) {
        /return TRUE
      }

      | get out if its a no cast click item
      /if (!${FindItem[=${_verbage}].CastTime}) {
        | DEBUG NO CAST TIME
        /if (${Select[${Me.Class.ShortName},BRD]}) /call pulse_stop
        /invoke ${maData.Add[stCastReturn,SUCCESS]}
        /return


      } else {
        | DEBUG CAST TIME
        /delay 8
        /delay ${Math.Calc[${FindItem[${_verbage}].CastTime.Float}+5]}s !${Me.Casting.ID}
        
        /while (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
          /if (!${Me.Casting.ID}) /break
          /if (!${Window[CastingWindow].Open}) /break
          /delay 3
        }       
        
        GETINPUT voodoo_item NA|NA
        /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,RESISTED]}) {
          /varset _castLoop 0
          /if (${Select[${Me.Class.ShortName},BRD]}) /call pulse_stop
          /break
        }
        /if (${Select[${maData.Find[stCastReturn].Value},NOTREADY]}) {
          /varcalc _castLoop ${_castLoop}-1
          /delay 2
        }
      }
      /return TRUE
    }


  | pulses
  } else /if (${_type.Equal[PULSE]}) {
    DEBUG ${sep}\agPULSE\ax${sep}${_verbage}

    /if (${Me.CurrentMana} < ${Spell[${_verbage}].Mana}) /return FALSE

    /if (${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoClose}

    | mem spell if needed??
    /if (!${Me.Gem[${_verbage}]}) {
      /call set_spell_to ${_debug} TRUE "${_verbage}" ${maEnv.Find[stBuffGem].Value} TRUE
    }

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_tmpID}) {
      /invoke ${target[set, ${_tmpID}]}
    }

    /if (${_tmpID} == 0) {
      /invoke ${out[5, "Play${sep}${sp}${_verbage}\ax"]}
    } else /if (${_tmpID} != 0 && ${Target.ID}) {
      /invoke ${out[5, "Pulse${sep}${sp}${_verbage}\ax ${dot} ${con_color[FALSE, ${_tmpID}]}\ax"]}
    }

    /while (${_castLoop}) {
      /cast ${Me.Gem[${_verbage}]}
      /delay 1

      |** old loop duration control. leave for now.
      | adjust delay for Metronome AA (fuck bards)
      /if (${Me.AltAbility[Metronome]}) {
      /delay ${Spell[${_verbage}].CastTime.Seconds}s
      } else {
        /delay ${Math.Calc[${Spell[${_verbage}].CastTime.Seconds}+${Math.Calc[${Spell[${_verbage}].CastTime.Seconds}*.05]}]}s
      }
      **|

      | bard swap mechanics
      /while (${Window[CastingWindow].Open}) {
        /if (${maChr.Find[swInstrumentSwap].Value}) {
          /if (${Me.CastTimeLeft} < ${maHard.Find[stBardSwapRemain].Value}) {
            /if (${maBardFocus.Find[${Skill[${Spell[${_verbage}].Skill}].ID}].Value.Arg[3,|]}) {
              /if (${FindItem[${maBardFocus.Find[${Skill[${Spell[${_verbage}].Skill}].ID}].Value.Arg[1,|]}].WornSlot[offhand]}) {
                /call Bind_command_swap "${FindItem[${maBardFocus.Find[${Skill[${Spell[${Spell[${_verbage}].ID}].Skill}].ID}].Value.Arg[1,|]}].Name}" 14 FALSE
              } else /if (${FindItem[${maBardFocus.Find[${Skill[${Spell[${_verbage}].Skill}].ID}].Value.Arg[1,|]}].WornSlot[mainhand]}) {
                /call Bind_command_swap "${FindItem[${maBardFocus.Find[${Skill[${Spell[${Spell[${_verbage}].ID}].Skill}].ID}].Value.Arg[1,|]}].Name}" 13 FALSE
              }
            }
          }
        }
        /delay 3
      }
      /delay 3
      /doevents

      /if (${_sub.Equal[ignoreall]}) {
        /return TRUE
      }

      /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,RESISTED,IMMUNE]}) {
        /varset _castLoop 0
        /call pulse_stop
        /break
      }
      /if (${Select[${maData.Find[stCastReturn].Value},NOTREADY,DISTRACTED]}) {
        /delay 2
      }
      /varcalc _castLoop ${_castLoop}-1
    }

    /if (${maChr.Find[swInstrumentSwap].Value}) {
      /call Bind_command_swap "${maChr.Find[stInstrumentSecondary].Value}" 14 FALSE
      /call Bind_command_swap "${maChr.Find[stInstrumentPrimary].Value}" 13 FALSE
    }

    /return TRUE
  }

/return FALSE



|***
 * note: stops: casting, movement
 * use: /call set_stop DEBUG [SILENT]
 ***|
sub set_stop(bool _debug, string _silent)
  DEBUG \atset_stop\ax(\a-w${_silent}\ax)

  SETHUD "stop"
  GETINPUT set_stop swAuto|${maEnv.Find[swAuto].Value}

  /if (${Select[${Me.Class.ShortName},BRD]} && (${Me.Casting.ID} || ${Me.BardSongPlaying} || ${Window[CastingWindow].Open})) {
    DEBUG ${sep}stop${sep}\a-wbard bs\ax
    /while (${Me.Casting.ID} || ${Me.BardSongPlaying} || ${Window[CastingWindow].Open}) {
      /stopsong
      /stopcast
      /delay 10 !${Me.Casting.ID}
    }

  } else {
    /if (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
      DEBUG ${sep}stop${sep}\a-wcasting\ax
      /stopcast
    }
  }


  | resets
  /invoke ${maPred.Add[stPredID,0]}

  | pull wait timer
  /invoke ${set_timer[${_debug}, wait_for_mob, 0]}

  /if (${_silent.NotEqual[SILENT]}) {
    /invoke ${out[0, 0, 7]}
  }

  /if (${maHome.Find[swSnapStuck].Value}) {
    DEBUG ${sep}stop${sep}\a-wreset stuck\ax
    /invoke ${maHome.Add[swSnapStuck,FALSE]}
  }

  /if (TIE) {
    DEBUG ${sep}stop${sep}\a-wreset tie\ax
    /invoke ${maTie.Add[swSetTie,FALSE]}
  }

  /if (${Me.Pet.ID} && ${Me.Pet.Target.ID}) {
    DEBUG ${sep}stop${sep}\a-wreset pet\ax
    /if (!${Me.Pet.ReGroup}) {
      /pet regroup
    }
    /if (!${Me.Pet.Hold}) {
      /pet hold on
    }
    /pet back off
  }

  | stop movement
  /declare _breakmove int local 1
  /while (${Me.Moving} || ${Navigation.Active}) {

    /if (${Stick.Active}) {
      DEBUG ${sep}stop${sep}\a-wstick\ax (\a-r${_breakmove}\ax)
      /squelch /stick off
    }

    /if (${Navigation.Active}) {
      DEBUG ${sep}stop${sep}\a-wnavigation\ax (\a-r${_breakmove}\ax)
      /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
    }

    /varcalc _breakmove ${_breakmove}+1
    /if (${Me.Moving} || ${_breakmove} > 4) /break

  }

  SETHUD EMPTY

/return TRUE




|***
 * note: stops: movement
 * use: /call set_stop_movement DEBUG
 ***|
sub set_stop_movement(bool _debug)
  DEBUG \atset_stop_movement\ax(\a-w\ax)

  | stop movement
  /declare _breakmove int local 1
  /while (${Me.Moving} || ${Navigation.Active}) {

    /if (${Stick.Active}) {
      DEBUG ${sep}stop${sep}\a-wstick\ax (\a-r${_breakmove}\ax)
      /squelch /stick off
    }

    /if (${Navigation.Active}) {
      DEBUG ${sep}stop${sep}\a-wnavigation\ax (\a-r${_breakmove}\ax)
      /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
    }

    /varcalc _breakmove ${_breakmove}+1
    /if (${Me.Moving} || ${_breakmove} > 4) /break

  }

/return TRUE



|***
 * note: moves the toon to a valid/LoS location of a target id with a given distance
 * use : /call moveto_range [ID] [DISTANCE]
 ***|
sub moveto_range(bool _debug), int _tmpID, int _range)
/varset _debug TRUE
  DEBUG \atmoveto_range\ax(\a-w${_tmpID}, ${_range}\ax)

  /declare _i int local 0
  /declare _locX float local 0
  /declare _locY float local 0
  /declare _locZ float local 0
  /declare _tmpX float local 0
  /declare _tmpY float local 0
  /declare _dist float local ${Math.Calc[${If[${_range}==0,1,${_range}]} / ${If[${Spawn[${_tmpID}].Distance}==0,1,${Spawn[${_tmpID}].Distance}]}]}

  DEBUG ${sep}{Math.Calc[${If[${_range}==0,1,${_range}]} / ${If[${Spawn[${_tmpID}].Distance}==0,1,${Spawn[${_tmpID}].Distance}]}]}
  DEBUG ${sep}_dist${sep}\a-w${_dist}\ax

  /varcalc _locX ${_dist}*${Me.X} + (1-${_dist})*${Spawn[${_tmpID}].X}
  /varcalc _locY ${_dist}*${Me.Y} + (1-${_dist})*${Spawn[${_tmpID}].Y}
  /if (${Navigation.PathExists[locyxz ${_locY} ${_locX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_locX} ${_locY} ${Me.Z}]} && ${LineOfSight[${_locY},${_locX},${Me.Z}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X},${Spawn[${_tmpID}].Z}]}) {
    DEBUG {Navigation.PathExists[locyxz ${_locY} ${_locX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_locX} ${_locY} ${Me.Z}]} && ${LineOfSight[${_locY},${_locX},${Me.Z}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X},${Spawn[${_tmpID}].Z}]
    DEBUG ${sep}(\a-r0\ax) locyxz ${_locY} ${_locX} ${Me.Z}
    /nav locyxz ${_locY} ${_locX} ${Me.Z} |log=${maEntropy.Find[swNavLogSpew].Value}
    /delay 5 ${Navigation.Active}
    /delay 5s !${Navigation.Active}
    /return TRUE
  }

  /for _i 1 to 20
    /varcalc _tmpX ${_locX}*${cos[${_i}]} - ${_locY}*${sin[${_i}]}
    /varcalc _tmpY ${_locX}*${cos[${_i}]} + ${_locY}*${sin[${_i}]}
    /if (${Navigation.PathExists[locyxz ${_tmpY} ${_tmpX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_tmpX} ${_tmpY} ${Me.Z}]} && ${LineOfSight[${_tmpY},${_tmpX},${Me.Z}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X},${Spawn[${_tmpID}].Z}]}) {
      DEBUG ${sep}(\a-r-${_i}\ax) locyxz ${_locY} ${_locX} ${Me.Z}
      /nav locyxz ${_tmpY} ${_tmpX} ${Me.Z} |log=${maEntropy.Find[swNavLogSpew].Value}
      /delay 5 ${Navigation.Active}
      /delay 5s !${Navigation.Active}
      /return TRUE
    }

    /varcalc _tmpX ${_locX}*${cos[${_i}]} + ${_locY}*${sin[${_i}]}
    /varcalc _tmpY ${_locX}*${cos[${_i}]} - ${_locY}*${sin[${_i}]}
    /if (${Navigation.PathExists[locyxz ${_tmpY} ${_tmpX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_tmpX} ${_tmpY} ${Me.Z}]} && ${LineOfSight[${_tmpY},${_tmpX},${Me.Z}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X},${Spawn[${_tmpID}].Z}]}) {
      DEBUG ${sep}(\a-r+${_i}\ax) locyxz ${_locY} ${_locX} ${Me.Z}
      /nav locyxz ${_tmpY} ${_tmpX} ${Me.Z} |log=${maEntropy.Find[swNavLogSpew].Value}
      /delay 5 ${Navigation.Active}
      /delay 5s !${Navigation.Active}
      /return TRUE
    }

  /next _i

/return TRUE



|***
 * note: just clears the xtarget list
 * use: /call set_xtarget_clear DEBUG
 ***|
sub set_xtarget_clear(bool _debug)
  DEBUG \atset_xtarget_clear\ax()

  /declare _count int local 0
  /for _count 1 to ${Me.XTargetSlots}
    /xtarget set ${_count} Autohater
  /next _count

/return



|***
 * note: extended target controls
 * use: /call check_xtarget_build DEBUG [FORCED]
 ***|
sub check_xtarget_build(bool _debug, string _forced)
  DEBUG \atcheck_xtarget_build\ax(\a-w${_forced}\ax)

  /invoke ${set_data_timer[FALSE, Check_Xtarget_Build, RESTART]}

  /if (!${Me.XTargetSlots}) /return
  /if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
    /invoke ${out[1, shade, 0]}
    /return FALSE
  }

  /invoke ${out[0, "Adjusting XTargets${sep}${If[${Bool[${_forced}]},\a-wForced\ax,Timer \a-w${maTimer.Find[tiCheck_Xtarget_Build].Value}\ax]}"]}
  /declare _x         int     local
  /declare _b         int     local
  /declare _count     int     local 0
  /declare _name      string  local

  | reset extended target slots
  /call set_xtarget_clear FALSE

  | observe any assists for xtarget 1
  /if (${Select[${maCC.Find[stAssistMode].Value},G,1,2,3]} || ${Bool[${maCC.Find[stManualAssist1].Value}]} || ${Bool[${maCC.Find[stManualAssist2].Value}]}) {
    /varset _b 1
  }

  | protect specific people outside your group
  /if (${lsProtectOutsideGroup.Count}) {

    /declare _lipcxt listiterator local
    /vardata _lipcxt lsProtectOutsideGroup.First.Clone
    /while (!${_lipcxt.IsEnd}) {

      /varset _name ${Spawn[${_lipcxt.Value}].DisplayName}

      | i hate people
      /if (${lsNoHealToon.Contains[${_name}]}) {
        /invoke ${_lipcxt.Advance}
        /continue
      }

      | in our range of operaions?
      /if (${Spawn[${_name}].Distance} > ENV || !${Bool[${Spawn[${_name}]}]}) {
        /invoke ${_lipcxt.Advance}
        /continue
      }

      | are they already in our group?
      /if (${Group.Member[${_name}].ID}) {
        /invoke ${_lipcxt.Advance}
        /continue
      }

      | out of xtarget slots?
      /if (${_b} > ${Me.XTargetSlots}) {
        /break
      }

      /if (${Bool[${Me.XTarget[${_name}]}]}) {
        /varcalc _b ${_b}+1
      } else /if (${Bool[${Spawn[${_name}]}]}) {
        /varcalc _b ${_b}+1
        /xtarget set ${_b} ${Spawn[${_name}].Name}
      }

      /invoke ${_lipcxt.Advance}
    }
  }


  | set raid member classes to protect
  /if (${Raid.Members}) {
    /declare _lixt listiterator local
    /vardata _lixt lsHealXTClass.First.Clone
    /while (!${_lixt.IsEnd}) {

      | no class in range
      /if (!${SpawnCount[pc ${_lixt.Value} raid radius ENV]}) {
        /invoke ${_lixt.Advance}
        /continue
      }

      | cycle the class and add to slots
      /for _x 1 to ${SpawnCount[pc ${_lixt.Value} raid radius ENV]}

        /if (${_b} > ${Me.XTargetSlots}) {
          /break
        }

        | yes.. i still hate people
        /if (${lsNoHealToon.Contains[${NearestSpawn[${_x}, pc ${_lixt.Value} raid].DisplayName}]}) {
          /continue
        }

        | are they a group member?
        /if (${Group.Member[${NearestSpawn[${_x}, pc ${_lixt.Value} raid]}].ID}) {
          /continue
        }

        | on the xtarget
        /if (${Bool[${Me.XTarget[${NearestSpawn[${_x}, pc ${_lixt.Value} raid].Name}]}]}) {
          /continue
        }
        
        | skip lsProtectOutsideGroup list. it is already covered
        /if (${lsProtectOutsideGroup.Contains[${Me.XTarget[${NearestSpawn[${_x}, pc ${_lixt.Value} raid].Name}]}]}) {
          /continue
        }

        | me?
        /if (${NearestSpawn[${_x}, pc ${_lixt.Value} raid radius ENV].DisplayName.Equal[${Me.DisplayName}]}) {
          /continue
        }

        /varcalc _b ${_b}+1
        /xtarget set ${_b} ${NearestSpawn[${_x}, pc ${_lixt.Value} raid radius ENV]}
        /delay 1
      /next _x

      /invoke ${_lixt.Advance}
    }

  }

/return TRUE



|***
 * note: Sticks you to the target
 * note: for the love of whatever you find important, don't touch this ever again...
 * use: /call stick_to_target DEBUG [target ID]
 ***|
sub stick_to_target(bool _debug, int _tmpID)
  DEBUG \atstick_to_target\ax(\a-w${_tmpID}\ax)

  /if (${timer_check_stick}) /return FALSE

  /invoke ${set_timer[${_debug}, timer_check_stick, 1s]}

  /if (!AUTO && !${swModeToken}) /return FALSE

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  ISMEDEAD
  GETINPUT stick_to_target tmpID|${_tmpID}

  /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FALSE


  | if the stick behind switch is on. we do that over ranged decision combat
  /if (${Target.ID} && ${Me.TargetOfTarget.ID} != ${Me.ID}&& !${timer_Check_Behind}) {
    /if (${maHome.Find[swMoveBehind].Value} && !${Stick.Behind} && ${Target.Distance} <= ENV ) {
      DEBUG ${sep}stick${sep}\a-wbehind\ax

      /squelch /stick id ${_tmpID} ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} snaproll
      /invoke ${set_data_timer[${_debug}, Check_Behind, RESTART]}
      /delay 1s ${Me.Moving}
      /delay 5s !${Me.Moving}

      | if we get stuck, get out and give it a sec.
      /if (${MoveUtils.Stuck}) {
        /delay 1
        /squelch /stick off
        /invoke ${set_data_timer[${_debug}, Check_Behind, RESTART]}
      }
    }
  } else  /if (${Target.ID} && ${Me.TargetOfTarget.ID} == ${Me.ID} && !${timer_Check_Behind}) {
    /invoke ${set_data_timer[${_debug}, Check_Behind, RESTART]}
  }

  /if (${Spawn[id ${_tmpID}].Distance} <= ENV && ${Spawn[id ${_tmpID}].Distance} >= ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
    DEBUG ${sep}stick

    /if (!${Stick.Active} && ${Target.ID} && VALID && ${Spawn[id ${_tmpID}].Distance} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
      /squelch /stick id ${_tmpID} ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
      /delay 1s ${Me.Moving}
      /delay 5s !${Me.Moving}
    }

    /delay 1
    /if (${Spawn[id ${_tmpID}].Distance} <= ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) /squelch /stick off
  }

  /squelch /stick off

/return TRUE



|***
 * note: Seriously, do you need to ask?
 * use: ${is_target_dead[DEBUG, TARGETID]}
 ***|
sub is_target_dead(bool _debug, int _tmpID)
  DEBUG \atis_target_dead\ax(\a-w${_tmpID}\ax)

  ISMEDEAD

  /declare _dead bool local FALSE

  /if (!AUTO) {
    /if (!${Bool[${_tmpID}]} && ${Target.ID}) {
      DEBUG ${sep}!auto !target.id clear_combat
      /varset _tmpID ${Target.ID}
    }
    /if (!${Bool[${_tmpID}]}) {
      DEBUG ${sep}!auto !_tmpID clear_combat
      /invoke ${target[clear]}
      /return TRUE
    }
  }

  | reasons to consider something dead
  /if (${_tmpID} == 0 || !${Bool[${_tmpID}]}) {
    DEBUG ${sep}!_tmpID
    /varset _dead TRUE
  } else /if (!${Target.ID}) {
    DEBUG ${sep}!target-id
    /varset _dead TRUE
  } else /if (${Target.Dead}) {
    DEBUG ${sep}target-dead
    /varset _dead TRUE
  } else /if (${Target.Type.Equal[Corpse]}) {
    DEBUG ${sep}taget-corpse
    /varset _dead TRUE
  } else /if (${Spawn[${_tmpID}].Dead}) {
    DEBUG ${sep}spawn-dead
    /varset _dead TRUE
  } else /if (!${Spawn[${_tmpID}].ID}) {
    DEBUG ${sep}!spawn-id
    /varset _dead TRUE
  }

  /if (${_dead}) {
    /invoke ${target[clear]}
  }

/return ${_dead}



|***
 * note: Clear all combat variables
 * use: /call clear_combat DEBUG
 ***|
sub clear_combat(bool _debug)
  DEBUG \atclear_combat\ax()

  GETINPUT clear_combat NA|NA

  /varset stAssTarID 0
  /invoke ${maCC.Add[swValidAssTarID,FALSE]}
  /invoke ${maCC.Add[swSetCombat,FALSE]}
  /invoke ${maCC.Add[swSetForced,FALSE]}
  /invoke ${set_data_timer[FALSE, Check_Behind, 0]}

  /if (${Me.AutoFire}) {
    /squelch /autofire
  }

  /if (${Me.Combat}) {
    /squelch /attack off
  }

  /if (!${Select[${Me.Class.ShortName},BRD]}) {
    /if (AUTO && ${Me.Casting.ID}) {
      /if (!${Select[${Spell[${Me.Casting.ID}].Category},Transport,Heals]}) {
        /stopcast
      }
    }
  }

  /invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
  /invoke ${maBurn.Add[swBurnSpinUp,FALSE]}

  /if (${Stick.Active}) {
    /squelch /stick off
  }
  /if (AUTO && ${Target.ID}) {
    /squelch /target clear
  }

/return TRUE



|***
 * note: Clears all defined timers for a specific ID
 * use: /call clear_timer DEBUG [TARGETID]
 ***|
sub clear_timer(bool _debug, int _tmpID)
  DEBUG \atclear_timer\ax(\a-w${_tmpID}\ax)

  /if (${Defined[timer_${_tmpID}]}) {
    /deletevar timer_${_tmpID}
  }

/return



|***
 * note: Make sure that what ever we are targeting is something we want to kill
 * note: in manual mode, anything is a valid target.
 * use: ${is_target_valid[DEBUG, TARGETID]}
**|
sub is_target_valid(bool _debug, int _tmpID)
  DEBUG \atis_target_valid\ax(\a-w${_tmpID}\ax)

  /if (!AUTO) {
    /return TRUE
  }

  ISMEDEAD
  GETINPUT is_target_valid _tmpid|${_tmpID}

  /if (!${Target.ID}) /return FALSE
  /if (!${Bool[${_tmpID}]}) /return FALSE
  /if (SAFEZONE) /return FALSE

  | check smart assists
  /if (${Target.ID}) {
    /if (${Select[${maCC.Find[stAssistMode].Value},1,2,3]} && ${Spawn[${_tmpID}].ID} != ${Me.XTarget[1].ID}) {
      /invoke ${target[clear]}
      /return FALSE
    } else /if (${Select[${maCC.Find[stAssistMode].Value},g]} && ${Group.MainAssist.ID} && !${Bool[${Spawn[${Group.MainAssist}].Dead}]} && ${Spawn[${_tmpID}].ID} != ${Me.XTarget[1].ID}) {
      /invoke ${target[clear]}
      /return FALSE
    }
  }

  /if (!${maOver.Find[swOverPctEngage].Value} && !PULL) {
    /if (${Spawn[${_tmpID}].PctHPs} > ${maCC.Find[stPctEngage].Value} && !${Select[${maCC.Find[stAssistMode].Value},1,2,3,g]}) {
      | /delay 1
      /return FALSE
    }
  }

  | shit we really shouldn't be attacking
  /if (${lsNoAttackBodyType.Contains[${Spawn[${_tmpID}].Type}]}) /return FALSE
  /if (${lsNoTargetNPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /return FALSE
  /if (${lsNoTargetPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /return FALSE
  /if (${lsNoTargetMagic.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /return FALSE
  /if (${lsAnimationNoTarget.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /return FALSE
  /if (${lsZoneExclude.Contains[${Spawn[${_tmpID}].DisplayName}]}) /return FALSE


  /if (!PULL) {
    /if (${Target.ID} && ${Target.Mezzed.Duration}) /return FALSE
    /if ((!${Spawn[${_tmpID}].LineOfSight} && !${maOver.Find[swOverLOS].Value}) || ${Spawn[${_tmpID}].Distance} > ENV) /return FALSE
  } else /if (PULL) {
    /if (${Spawn[${_tmpID}].Distance} > PULLENV) /return FALSE
  }
  /if (${Spawn[${_tmpID}].Type.Equal[PC]}) /return FALSE
  /if (${Spawn[${_tmpID}].Master.Type.Equal[PC]}) /return FALSE
  /if (${Target.Type.Equal[PC]}) /invoke ${maCC.Add[swValidAssTarID,FALSE]}
  /if (${_tmpID} == ${Me.ID} || ${_tmpID} == ${Me.Pet.ID}) /return FALSE

  /if (${Select[${Me.Class.ShortName},ROG]}) {
    /if (${maEnv.Find[swSoS].Value}) /makemevisible
  }

/return TRUE



|***
 * note: Check if we need to engage burn routines. /combat heal/dual are treated differently
 * use: ${is_burn_valid[DEBUG]}
 ***|
sub is_burn_valid(bool _debug)
  DEBUG \atis_burn_valid\ax()

  | dont do shit in safe zone..
  /if (SAFEZONE) {
    DEBUG ${break}safezone${sep}\a-wreturning FALSE\ax
    /invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
    /return FALSE
  }

  | no raid burning if turned off
  /if (!${maBurn.Find[swBurnRaid].Value} && ${Raid.Members}) {
    DEBUG ${break}burn in raids OFF${sep}\a-wreturning FALSE\ax
    /invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
    /return FALSE
  }

  | if always set to burn, always burn while in combat
  /if (${maBurn.Find[swBurnForce].Value} && (${Me.CombatState.Equal[combat]} || ${Target.DisplayName.Find[combat dummy]})) {
    DEBUG ${break}FORCED burn mode${sep}\a-wreturning TRUE\ax
    /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
    /return TRUE
  }

  | remainder requires automated modes
  /if (!AUTO) {
    DEBUG ${break}in manual${sep}\a-wis_burn_valid() returning FALSE\ax
    /return FALSE
  }

  | no mobs around
  /if (!${SpawnCount[npc radius ENV PS4]}) {
    DEBUG ${break}no mobs in radius to check${sep}\a-wreturning FALSE\ax
    /invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
    /return FALSE
  }

  | if the current target is a namer
  /if (${maBurn.Find[swBurnAuto].Value} && TARGET && ${Spawn[TARGET].PctHPs} <= ${maBurn.Find[stPctBurnEngage].Value} && (${lsZoneNamed.Contains[${Target.DisplayName}]} && !${maOver.Find[swOverName].Value})) {
    DEBUG ${break}target is named${sep}\a-wreturning TRUE\ax
    /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
    /return TRUE
  }

  | if number of mobs in the local area reaches the /burn count ## threashold
  /if (${maBurn.Find[stBurnCount].Value}) {
    /if (${SpawnCount[npc radius ENV targetable PS4]} >= ${maBurn.Find[stBurnCount].Value}) {
      DEBUG ${break}spawncount in radius${sep}\a-wreturning TRUE\ax
      /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
      /return TRUE
    }
  }

  | healers and off combat routines simply check for a named in the local area to start a burn. no direct target of a named is required. just be in the area.
  /if (!HEAL) {
    DEBUG ${break}not a healer. stopping here${sep}\a-wreturning FALSE\ax
    /invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
    /return FALSE
  }

  /if (COMBAT) {
    DEBUG ${sep}checking named radius
    /declare _count int local 0
    /declare _mobID int local 0

    /for _count 1 to ${SpawnCount[npc radius ENV PS4]}
      /varset _mobID ${NearestSpawn[${_count}, npc radius ENV].ID}
      /if (${maBurn.Find[swBurnAuto].Value} && ${lsZoneNamed.Contains[${Spawn[id ${_mobID} radius ENV].DisplayName}]} && !${maOver.Find[swOverName].Value}) {
        DEBUG ${break}named found in radius${sep}\a-wreturning TRUE\ax
        /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
        /return TRUE
      }
    /next _count
  }

/return FALSE



|***
 * note: do we want to exit the class while loop. assume no exit (FALSE)
 * use: ${check_class_loop[DEBUG]}
 ***|
sub check_class_loop(bool _debug)
  DEBUG \atcheck_class_loop\ax()

  /if (AUTO) {
    | no target.. were done
    /if (!${Spawn[TARGET].ID}) {
      DEBUG ${sep}no target
      /return FALSE
    }

    /if (${target[isdead, TARGET]}) {
      /return FALSE
    }

    | engage point met?
    /if (!${maOver.Find[swOverPctEngage].Value} && !PULL) {
      /if (${Spawn[TARGET].PctHPs} > ${maCC.Find[stPctEngage].Value} && ${Select[${maCC.Find[stAssistMode].Value},1,2,3,g]}) {
        /invoke ${maCC.Add[swValidAssTarID,FALSE]}
        /invoke ${maCC.Add[swSetCombat,FALSE]}
        /return FALSE
      }
    }

  | manual mode
  } else /if (!AUTO) {
    | no target. go away
    /if (!${Target.ID}) /return FALSE
    | forced combat in manual
    /if (COMBAT) {
      | target dead. end
      /if (${target[isdead, ${Target.ID}]}) {
        /return FALSE
      }
    }
  }

/return TRUE



|***
 * note: prep sub for all calsses to decide if they are going to be in combat and if its time to burn
 * use: /call check_combat_status DEBUG || ${check_combat_status[DEBUG]}
 **|
sub check_combat_status(bool _debug)
  DEBUG \atcheck_combat_status\ax()

  CHECKCURSOR

  | manual mode
  /if (!AUTO) {
    DEBUG ${sep}Manual combat check
    /if (HEAL) {
      DEBUG ${sep}heal
      /invoke ${maCC.Add[swSetCombat,FALSE]}
    }
    /if (!COMBAT) {
      DEBUG ${break}manual${sep}no combat
      | /delay 1
      /return FALSE
    }

    /if (${Target.ID}) {
      /if (${Target.ID} == ${Me.ID}) {
        DEBUG ${break}target is me
        /return FALSE
      }
      /if (COMBAT) {
        DEBUG ${sep}check for burn
        /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}
      }
      /return TRUE

    } else /if (!${Target.ID}) {
      /if (COMBAT || VALID) {
        /invoke ${target[clear]}
      }
      /return FALSE
    }

  | combat healing/crowd control/bard ooc song
  } else /if (AUTO && (HEAL || (${madeBuff.Find[swMez].Value} && ${maCC.Find[swCombatControl].Value}))) {
    DEBUG ${sep}auto-heal/CC combat check

    /if (${maCC.Find[swCombatControl].Value} || NUKE || DOT || MELEE) {
      /if (${is_target_valid[${_debug}, TARGET]}) {
        /invoke ${maCC.Add[swValidAssTarID,TRUE]}
      }
    }
    /invoke ${maCC.Add[swSetCombat,TRUE]}
    /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

    /return TRUE

  | automatic mode
  } else /if (AUTO) {
    DEBUG ${sep}auto combat check

    /if (${Me.Invis} && ${Me.Class.ShortName.NotEqual[ROG]}) {
      /return FALSE
    }
    /if (${Me.Invis} && ${Me.Class.ShortName.Equal[ROG]} && !${maEnv.Find[swSoS].Value}) {
      /return FALSE
    }

    /if (${Me.Class.ShortName.Equal[BRD]}) {
      /if (${Me.Song[Rallying Solo].ID}) /return FALSE
    }


    /if (${is_target_valid[${_debug}, TARGET]}) {
      /invoke ${maCC.Add[swValidAssTarID,TRUE]}
    }

    /if (!VALID || !${Spawn[TARGET].ID}) {
      /invoke ${target[clear]}
      /return FALSE

    } else /if (VALID && ${Spawn[TARGET].ID}) {
      DEBUG ${sep}\ayelse /if (VALID && ${Spawn[TARGET].ID})\ax
      /invoke ${maCC.Add[swSetCombat,TRUE]}
      /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}
      /return TRUE

    }
  }

/return FALSE



|***
 *  DES: preps toon for combat modes
 *  USE: /call set_combat DEBUG
 *  NOTE:
 ***|
sub set_combat(bool _debug)
  DEBUG \atset_combat\ax()

  /if (${target[isdead, TARGET]}) {
    /return FALSE
  }

  | home check
  /if (HOME && ${maHome.Find[swinCombat].Value}) {
    /declare _curVarDis float local ${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]}
    /if (${_curVarDis} > SAFEENV && PULL) {
      /call Bind_control_home return NULL NULL ${maDebug.Find[home].Value}
    }
  }

  CHECKCURSOR

  | out of range?
  /if (${Target.Distance} > ENV && !${maCC.Find[swSetForced].Value}) {
    /return FALSE
  }

  /if (${Me.State.NotEqual[stand]}) /stand
  /if (AUTO && ${Me.State.Equal[feign]}) /squelch /stand
  /if (!AUTO && ${Me.Invis}) /makemevisible

  | not within ${combatradius} and far enought to range?
  /if (${Target.Distance} > 30 && ${Target.Distance} > ${Spawn[id TARGET].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {

    | range mobs if enabled
    /if (RANGE) {

      /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

      | no need to move were are at a good distance
      /if (${Stick.Active}) {
        /squelch /stick off
      }

      /if (${lsZoneRange.Contains[${Target.DisplayName}]}) {
        /if (${Me.AutoFire}) /squelch /autofire
      } else {
        /if (${Spawn[TARGET].LineOfSight} && !${Me.AutoFire}) {
          /squelch /autofire
        }
      }

    | cannot range, so melee if in agro range?
    } else /if (!RANGE && MELEE) {
      /if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${maCC.Find[swStoponDS].Value} && ${Target.DSed.ID} && !AGRO)) {
        /squelch /attack off
        /return
      } else {
        /if (${Me.AutoFire}) /squelch /autofire
        /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
        /if ((AUTO || ${swModeToken}) && !${timer_check_stick}) {
          /call set_combat_control ${maDebug.Find[decision].Value} TARGET
        }
      }

    }
  }

  | mob too close for range
  /if (${Target.Distance} <= ${maHard.Find[stMinBowDist].Value}) {

    | adjust range if melee is diabled to keep close to target. give them some range to.. range again
    /if (${Target.Distance} < ${maHard.Find[stMinBowDist].Value} && RANGE && !MELEE) {
      /if (AUTO && !${Stick.Active} && ${Me.TargetOfTarget.ID} != ${Me.ID}) {
        /call moveto_range TARGET ${maHard.Find[stMinMovetoBow].Value}
      }

    | can we get to the melee already?
    } else /if (MELEE) {
      /if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${maCC.Find[swStoponDS].Value} && ${Target.DSed.ID} && !AGRO)) {
        /squelch /attack off
        /return FALSE
      } else {
        /if (${Me.AutoFire}) {
          /squelch /autofire
        }
        /call check_facing ${maDebug.Find[face].Value} TARGET FORCE

        /if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
          /call check_sash_proc ${_debug}
        } else {
          /call cycle_sash ${maDebug.Find[sash].Value}
        }

        /if (MELEE) {
          /squelch /attack on
        }
        /if (AUTO && !${timer_check_stick}) {
          /call set_combat_control ${maDebug.Find[decision].Value} TARGET
        }
      }
    }
  }

/return



|***
 * note: decision tree for what kind of combat to do
 * use: /call set_combat_control DEBUG [TARGETID]
 ***|

sub set_combat_control(bool _debug, int _tmpID)
  DEBUG \atset_combat_control\ax(\a-w${_tmpID}\ax)

  ISMEDEAD

  /if (!AUTO && ${maCC.Find[swSetForced].Value}) {
    /varset _tmpID ${Target.ID}
  }

  | validate the target
  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  | out of range?
  /if (${Target.Distance} > ENV && !${maCC.Find[swSetForced].Value}) {
    /return FALSE
  }

  /if (${Me.State.NotEqual[stand]}) {
    /squelch /stand
  }
  /if (AUTO && ${Me.State.Equal[feign]}) {
    /squelch /stand
  }
  /if (!AUTO && ${Me.Invis}) {
    /makemevisible
  }

  /if (${Target.ID} && (MELEE || RANGE) && ${Spawn[id ${_tmpID}].LineOfSight}) {
    /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  | set the max range so we dont have to keep calculating it over and over
  | /declare _maxrange float local ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}

  | range or melee, priority is range!
  /if (MELEE && RANGE) {
    DEBUG ${sep}\agmelee\ax${sep}\ayor\ax${sep}\agrange\ax

    /if (${lsZoneRange.Contains[${Target.DisplayName}]} || ${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && ${maCC.Find[swStoponDS].Value} && !AGRO)) {
      /call set_combat_range ${_debug} ${_tmpID}
      /return TRUE
    }

    /if (${Target.Distance} >= ${maHard.Find[stMinBowDist].Value}) {
      DEBUG ${sep}...range${sep}\a-w${Target.Distance}\ax
      /call set_combat_range ${_debug} ${_tmpID}
    } else /if (${Target.Distance} < ${maHard.Find[stMinBowDist].Value}) {
      DEBUG ${sep}...melee${sep}\a-w${Target.Distance}\ax
      /call set_combat_melee ${_debug} ${_tmpID}
    }
    /return TRUE

  | melee combat only
  } else /if (MELEE && !RANGE) {
    DEBUG ${sep}\agmelee\ax${sep}\arrange\ax

    /if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && ${maCC.Find[swStoponDS].Value} && !AGRO)) {
      /call set_combat_range ${_debug} ${_tmpID}
      /return TRUE
    }

    /call set_combat_melee ${_debug} ${_tmpID}
    /return TRUE

  | range combat only
  } else /if (RANGE && !MELEE) {
    DEBUG ${sep}\agrange\ax${sep}\armelee\ax

    /call set_combat_range ${_debug} ${_tmpID}

    /return TRUE

  }

/return TRUE



|***
 * note: sets range combat
 * use: /call set_combat_range DEBUG [TARGETID]
 ***|
sub set_combat_range(bool _debug, int _tmpID)
  DEBUG \atset_combat_range\ax(\a-w${_tmpID}\ax)

  /if (${Stick.Active}) {
    /squelch /stick off
  }

  /if (AUTO && !${Stick.Active} && ${Me.TargetOfTarget.ID} != ${Me.ID}) {
    /if (${Spawn[${_tmpID}].Distance} < ${maHard.Find[stMinBowDist].Value} || !${Spawn[${_tmpID}].LineOfSight}) {
      /call moveto_range ${_debug} ${_tmpID} 35
    }
  }

  /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

  /if (${Spawn[${_tmpID}].LineOfSight} && !${Me.AutoFire}) {
    /squelch /autofire
  }

/return TRUE



|***
 * note: sets melee combat
 * use: /call set_combat_melee DEBUG [TARGETID]
 ***|
sub set_combat_melee(bool _debug, int _tmpID)
  DEBUG \atset_combat_melee\ax(\a-w${_tmpID}\ax)

  /if (${Me.AutoFire}) {
    /squelch /autofire
  }

  /if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
    /call check_sash_proc ${_debug}
  }

  /if (MELEE) {
    /squelch /attack on
  }

  /if (${timer_check_stick}) {
    /return FALSE
  }

  /invoke ${set_timer[${_debug}, timer_check_stick, 1s]}

  /if (!AUTO && !${swModeToken}) {
    /return FALSE
  }

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  ISMEDEAD
  GETINPUT set_combat_melee NA|NA

  /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FALSE

  | if the stick behind switch is on. we do that over ranged decision combat
  /if (${maHome.Find[swMoveBehind].Value} && !${Stick.Behind} && ${Target.Distance} <= ENV && !${timer_Check_Behind}) {
    DEBUG ${sep}stick${sep}\a-wbehind\ax

    /squelch /stick id ${_tmpID} ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} snaproll
    /invoke ${set_data_timer[FALSE, Check_Behind, RESTART]}
    /delay 1s ${Me.Moving}
    /delay 5s !${Me.Moving}

    | if we get stuck, get out and give it a sec.
    /if (${MoveUtils.Stuck}) {
      /delay 1
      /squelch /stick off
      /invoke ${set_data_timer[FALSE, Check_Behind, RESTART]}
    }
  }

  /if (${Target.Distance} <= ENV && ${Spawn[id ${_tmpID}].Distance} >= ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
    DEBUG ${sep}stick

    /if (!${Stick.Active} && ${Target.ID} && VALID && ${Spawn[id ${_tmpID}].Distance} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
      /squelch /stick id ${_tmpID} ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
      /delay 1s ${Me.Moving}
      /delay 5s !${Me.Moving}
    }

    /delay 1
    /if (${Spawn[id ${_tmpID}].Distance} <= ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
      /squelch /stick off
    }
  }

  /squelch /stick off

/return TRUE



|***
 * note: finds a buff you cast on the current target
 * note: response is in duration (in seconds) left on the buff
 * use: ${get_target_buff_time_left[DEBUG, [BUFF NAME]]}
 ***|
sub get_target_buff_time_left(bool _debug, string _buff)
  DEBUG \atget_target_buff_time_left\ax(\a-w${_buff}\ax)

  /if (!${Target.ID} || ${Target.Dead}) {
    /return 0
  }

  /declare _count int local 0
  /for _count 1 to ${Target.BuffCount}

    /if (!${Target.ID} || ${Target.Dead}) {
      /return 0
    }

    /if (!${Target.Buff[${_count}].Name.Find[${_buff}]}) {
      /continue
    }

    /if (${Target.Buff[${_count}].Caster.Equal[${Me.DisplayName}]}) {
      DEBUG ${sep}FOUND${sep}\a-w${_buff}\ax (\a-r${Target.BuffDuration[${_count}].TotalSeconds}\ax)
      /break
    }

  /next _count

/return ${Target.BuffDuration[${_count}].TotalSeconds}



|***
 * note: need something to kill
 * note: a target ID is not needed. only used for off pull methods
 * use: /call get_target_basepull DEBUG [TARGETID]
 ***|
sub get_target_basepull(bool _debug, int _tmpID)
  DEBUG \atget_target_basepull\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  GETINPUT get_target_basepull NA|NA
  CHECKTIE

  /if (TARGET && ${Spawn[TARGET].ID}) {
    DEBUG ${sep}we already have a target${sep}\a-wTARGET\ax
    /return TRUE
  }

  SETHUD "pull base"

  | A naked blonde walks into a bar with a poodle under one arm and a two-foot salami under the other.
  | She lays the poodle on the table. Bartender says,'I suppose you won't be needing a drink.'
  | Naked lady says...
  /if (${_tmpID}) {
    /varset stAssTarID ${_tmpID}
  } else {
    /call pull_mob_find ${_debug}
  }

  DEBUG ${sep}target${sep}\a-wTARGET\ax

  | When you grow up, your heart dies.
  /if (!TARGET) {
    SETHUD EMPTY
    /if (HOME && !${SpawnCount[npc radius 15 PS4 targetable]}) {
      /call Bind_control_home return NULL NULL ${maDebug.Find[home].Value}
    }
    /return FALSE
  }

  | Could you describe the ruckus, sir?
  /if (TARGET) {
    /if (${Target.ID} != TARGET) {
      /invoke ${target[set, TARGET]}
    }
  }

  | Screws fall out all the time, the world is an imperfect place.
  /call pull_mob_prep ${_debug} TARGET
  /if (!${Macro.Return}) {
    DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
    /invoke ${target[clear]}
    /return FALSE
  }

  /call pull_mob_tag ${_debug} TARGET
  /if (${Macro.Return.Equal[tagfailure]}) {
    DEBUG ${sep}\arNAVpull FAILURE\ax${sep}resetting/returning to camp
    /invoke ${target[clear]}
    /call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
    /return FALSE
  }

  /if (MELEE) {
    /attack on
  }

  | wait for mob to get to camp
  DEBUG ${sep}loop \a-wwaitformob\ax

  /declare _waittime int local ${Math.Calc[(PULLENV + ${Spawn[${_tmpID}].Distance})*.08]}
  /call get_target_wait ${maDebug.Find[wait].Value} TARGET ${_waittime}s

  DEBUG ${sep}\arCLOSING\ax${sep}\atget_target_basepull\ax (\a-rTARGET\ax)
  SETHUD EMPTY

/return



|***
 * note: wait for mobs to get to you from a pull
 * use: /call get_target_wait DEBUG [TARGETID] [WAITTIME]
 ***|
sub get_target_wait(bool _debug, int _tmpID, string _waittime)
  | /varset _debug TRUE
  DEBUG \atget_target_wait\ax(\a-w${_tmpID}, ${_waittime}\ax)

  | too close for comfort
  /if (${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
    /return TRUE
  }

  | wait for mob to get to camp
  DEBUG ${sep}loop \a-wwait for mob\ax${sep}\a-w${_tmpID}\ax
  | /if (!${SpawnCount[npc radius ENV PS4 targetable]}) {
    DEBUG ${sep}_wait${sep}\ay${_waittime}\ax
    /invoke ${set_timer[${_debug}, wait_for_mob, ${_waittime}]}

    /if (!${Spawn[${_tmpID}].Dead}) {
      /invoke ${out[9, "Waiting on${sep}${con_color[${_debug}, ${_tmpID}]}${sep}\a-w${_waittime}\ax"]}
    }

    /while (${wait_for_mob}) {
 
      /if (${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
        DEBUG ${break}/while \a-ghostile mob in safeenv rada\ax
        /invoke ${target[clear]}
        /break
      }
          
      DEBUG ${sep}\awTARGET\ax.distance${sep}\a-w${Spawn[${_tmpID}].Distance}\ax${sep}\a-w${wait_for_mob}\ax
      GETINPUT get_target_basepull_wait NA|NA
      /delay 1

      | check for ammo if needed
      /if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID}) {
        /call check_buff_cycle_ammo ${maDebug.Find[ammo].Value} TRUE
      }

      | am I dead on the wait?
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) {
        DEBUG ${sep}/while \a-gis_me_dead\ax
        /invoke ${set_timer[${_debug}, wait_for_mob, 0]}
        /invoke ${target[clear]}
        /break
      }

      | is the target still alive?
      /if (!${_tmpID} || ${Spawn[${_tmpID}].Dead} || !${Spawn[${_tmpID}].ID}) {
        DEBUG ${sep}/while \a-gbreak_NO_target\ax
        /invoke ${target[clear]}
        /break
      }

      | mobs in our personal space. fuck those things. no respect
      /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} && ${NearestSpawn[1, npc radius SAFEENV].ID} != ${_tmpID}) {
      | /if (${SpawnCount[npc radius ENV PS4 targetable]}) {
        DEBUG ${break}mob in environment space
        /invoke ${out[9, 0, 26]}
        /invoke ${target[clear]}
        /break
      }

      | target LOS & within tag range
      | /if (${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < ENV && ${Spawn[${_tmpID}].Distance} < 100 && RANGE) {
      /if (${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < ENV) {
        DEBUG ${sep}/while \a-gbreak_tag\ax
        /break
      }

      | target within melee
      /if (${Spawn[${_tmpID}].Distance} <= ${Spawn[${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value} && MELEE) {
        DEBUG ${sep}/while \a-gbreak_MaxRangeTo\ax
        /break
      }

      | target in stupid range out of meele less then range.. hate this spot
      /if (${Spawn[${_tmpID}].Distance} > ${Spawn[${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value} && ${Spawn[${_tmpID}].Distance} < ${maHard.Find[stMinBowDist].Value}) {
        DEBUG ${sep}/while \a-gbreak_MaxRangeTo <> ${maHard.Find[stMinBowDist].Value}\ax
        /break
      }

      | LoS time reduction
      /if (!${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < ENV) {
        /delay 1
      }

      | tank attraction skills
      /if (${maChr.Find[swAttraction].Value} && (${Me.TargetOfTarget.ID} != ${Me.ID}) && ${Spawn[${_tmpID}].LineOfSight}) {
        /call pull_mob_attraction ${_debug} ${_tmpID} FALSE
      }

      | pet loss?
      /if (${maPull.Find[stPullMode].Value.Equal[pet]} && !${Pet.ID} && PET) {
        /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
      }

      | timer ran out
      /if (!${wait_for_mob} || !AUTO) {
        /invoke ${out[9, "Sick of waiting on${sep}${con_color[${_debug}, ${_tmpID}]}"]}
        /invoke ${target[clear]}
        /break
      }
    }

/return TRUE



|***
 * note: various conditions for the target in question
 * use: /call pull_mob_prep DEBUG [TARGETID]
 ***|
sub pull_mob_prep(bool _debug, int _tmpID)
  DEBUG \atpull_mob_prep\ax(\a-w${_tmpID}\ax)

  /declare _count int local 0
  /declare _los string local los

  | advanced pulling does not require LoS, clear the reqs for anything except base pulling
  /if (${maOver.Find[swOverLOS].Value} || ${Select[${maPull.Find[stPullMode].Value},int,pet,calm,multi]}) {
    /varset _los
  }

  | exclude list pulls. clear these targets.
  /if (${lsZoneExclude.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
    DEBUG ${sep}Pull Excludelist${sep}\a-w${lsZoneExclude.Contains[${Spawn[id ${_tmpID}].DisplayName}]}\ax \arreset\ax
    /varset stAssTarID 0
    /varset _tmpID 0
    /return FALSE
  }

  | get the mob to us for non pet classes..
  DEBUG ${sep}pull_mob_prep${sep}\awLoop.Start\ax${sep}${_tmpID}
  /if (!${target[isdead, ${_tmpID}]} && !${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) {

    /if (!AUTO) {
      /invoke ${target[clear]}
      /return FALSE
    }

    | keep tabs on targets range if they are the puller. advanced pullers fetching we do not care about yet
    /if (${Spawn[${_tmpID}].Distance} > PULLENV) {
      /invoke ${target[clear]}
      /return FALSE
    }

    | CHECKCURSOR TRUE
    GETINPUT pull_mob_prep _tmpID|${_tmpID}
    ISMEDEAD

    | stand the fuck up. seriously...
    /if (${SpawnCount[npc ${_los} radius PULLENV targetable]} && ${Me.State.NotEqual[stand]}) {
      /stand
    }
    | ain't got no time for that
    /if (${Me.Stunned}) {
      /return FALSE
    }

    | keep us facing the target just enough...
    /if (${Target.ID} && (MELEE || RANGE) && ${Spawn[id ${_tmpID}].LineOfSight}) {
      /call check_facing ${_debug} ${_tmpID} FORCE
    }

    | /delay 2
    /if (${target[isdead, ${_tmpID}]}) {
      /return FALSE
    }

    | get out if mob is even remotely close
    /if (${Spawn[id ${_tmpID}].Distance} <= ENV && ${Spawn[id ${_tmpID}].LineOfSight}) {
      DEBUG ${sep}prep${sep}in sight < ENV
      /return TRUE
    }
  }

  | pet auto send to kill
  /if (${Spawn[id ${_tmpID}].Distance} <= PULLENV && ${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) {
    DEBUG ${sep}Pet Range${sep}${_tmpID}
    /if (${Target.ID} != ${_tmpID}) {
      /invoke ${target[set, ${_tmpID}]}
    }
    /if (${Target.ID} && !${Bool[${Me.Pet.Target}]}) {
      /squelch /pet attack
    }
    /return TRUE
  }

/return TRUE


|***
 * note: hard stop for pulling based on crew hp/stamina
 * use: /call pull_hardstop DEBUG
 ***|
sub pull_hardstop(bool _debug)
  DEBUG \atpull_hardstop\ax()

 | if duration is turned off, we dont come back to check for 30 seconds..
  /if (!${Bool[${maPull.Find[stHardStopDuration].Value}]}) {
    /invoke ${set_data_timer[${_debug}, Check_Pull_Hardstop_Delay, 30s]}
    /return FALSE
  }

  /declare _countToon int local 1
  /declare _toonName string local FALSE
  /declare _found bool local FALSE

  | each toon
  /while (${_countToon} <= ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}) {

    | get the name to something usable
    /varset _toonName ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}
    DEBUG ${sep}_toonName${sep}\ay${_toonName}\ax

    | skip this person?
    /if (!${Spawn[pc ${_toonName}].ID} || ${Spawn[pc ${_toonName}].Dead} || ${Spawn[pc ${_toonName}].Distance} > ENV) {
      DEBUG ${sep}skipping${sep}\a-w${_toonName}\ax ${dot} dead or beyond ENV
      /varcalc _countToon ${_countToon}+1
      /continue
    }

    | check mana and stamina
    /if (${Bool[${maPull.Find[stPctHardStop].Value}]}) {
      /if (${Select[${Spawn[pc ${_toonName}].Class.ShortName},PAL,SHD,RNG,BST,WIZ,MAG,ENC,NEC,CLR,DRU,SHM]}) {
        /dquery ${_toonName} -q "Me.PctMana" -o DNQHardStop -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}.PctMana \at${DNQHardStop}\ax
        /if (${Range.Between[1,101:${DNQHardStop}]}) {
          /if (${DNQHardStop} < ${maPull.Find[stPctHardStop].Value}) {
            /varset _found TRUE
          }
        }
      }

      | stamina
      /if (${Select[${Spawn[pc ${_toonName}].Class.ShortName},WAR,PAL,SHD,BRD,RNG,BST,BER,MNK,ROG]}) {
        /dquery ${_toonName} -q "Me.PctEndurance" -o DNQHardStop -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}.PctEndurance \at${DNQHardStop}\ax
        /if (${Range.Between[1,101:${DNQHardStop}]}) {
          /if (${DNQHardStop} < ${maPull.Find[stPctHardStop].Value}) {
            /varset _found TRUE
          }
        }
      }
    }

    | rez efects
    /if (${Bool[${maPull.Find[swHardStopRez].Value}]}) {
    /dquery ${_toonName} -q "Me.Buff[Revival Sickness].ID" -o DNQHardStop -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${sep}${sep}Revival Sickness \at${DNQHardStop}\ax
      /if (${Bool[${DNQHardStop}]}) {
        /varset _found TRUE
      }

      /dquery ${_toonName} -q "Me.Buff[Resurrection Sickness].ID" -o DNQHardStop -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${sep}${sep}Resurrection Sickness \at${DNQHardStop}\ax
      /if (${Bool[${DNQHardStop}]}) {
        /varset _found TRUE
      }
    }

    /if (${_found} && !${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
      /invoke ${set_data_timer[FALSE, Check_Pull_Hardstop, ${maPull.Find[stHardStopDuration].Value}s]}
      /return TRUE
    }

    /varcalc _countToon ${_countToon}+1
  }

/return FALSE



|***
 * note: tea anyone?
 * use: /call pull_mob_tag DEBUG [TARGETID]
 ***|
sub pull_mob_tag(bool _debug, int _tmpID)
  DEBUG \atpull_mob_tag\ax(\a-w${_tmpID}\ax)

  /declare _count int local 0

  /if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID} && ${maBuff.Find[swBuffAmmo].Value}) {
    /call check_buff_cycle_ammo ${_debug} TRUE
  }

  | loop cycle start
  /declare _loopbreak int local 1

  /if (${Spawn[id ${_tmpID}].LineOfSight}) {
    DEBUG ${sep}Target${sep}\agLoS\ax

    | get our target
    /if (${Target.ID} != ${_tmpID}) {
      /invoke ${target[set, ${_tmpID}]}
      /delay 5 ${Target.ID} == ${_tmpID}
    }


    | melee tag
    | we are using a % of the targets max range to melee just to make sure we get close enough.
    | if we are not in this range, we try and get closer.
    /if (${maPull.Find[stPullWith].Value.Equal[melee]} && ${Spawn[id ${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} || ${Spawn[id ${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      DEBUG ${sep}Tag${sep}Melee${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax
      GETINPUT pull_mob_tag_melee NA|NA

      /attack on
      | make sure we are stopped
      /if (!${Select[${Me.Class.ShortName},BRD]}) {
        /declare _breakmove int local 1
        /while (${Me.Moving} || ${Navigation.Active}) {
          DEBUG ${sep}stop tag${sep}\a-wnavigation\ax (\a-r${_breakmove}\ax)
          /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
          /varcalc _breakmove ${_breakmove}+1
          /if ((!${Me.Moving} && !${Navigation.Active}) || ${_breakmove} > 4) /break
          /delay 5 !${Me.Moving} && !${Navigation.Active}
        }
      }

      DEBUG ${sep}TargetOfTarget.Melee
      /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FALSE
      /varset _loopbreak 1

      DEBUG ${sep}Entering _loopbreak Melee
      /while (${_loopbreak} <= ${maHard.Find[stPullMobTagTry].Value}) {

        /delay 5
        GETINPUT
        CHECKDEAD

        | have the targets attention. can go back
        /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
          DEBUG ${break}pull_mob_tag${sep}tot\ag==\axme (\a-r${_loopbreak}\ax)
          /break
        }

        | break if we have agro, don't panic, we can get more
        /if (${Me.PctAggro} >= ${maAgro.Find[stPctAgroHold].Value}) {
          DEBUG ${break}pull_mob_tag${sep}have agro hold %
          /break
        }

        | target is stunned. call it good
        /if (${Spawn[id ${_tmpID} playerstate 32].ID}) {
          DEBUG ${break}pull_mob_tag${sep}stunned
          /break
        }

        | break if mob is close enough to bash to a bloody pulp
        /if (${Spawn[id ${_tmpID}].Distance} <= ${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
          DEBUG ${break}pull_mob_tag${sep}target is in melee range
          /break
        }

        | i'm hurt more then i should be
        | /if (${Me.PctHPs} <= ${Math.Calc[${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO]}) {
        |   DEBUG ${break}pull_mob_tag${sep}I'm hurt wtf?
        |   /break
        | }
        
        /if (${NearestSpawn[1, npc radius SAFEENV PS4].ID} != ${_tmpID}) {
          DEBUG ${break}/while \a-ghostile mob in safeenv rad, breaking (\a-rpull_mob_tag(${_tmpID})\ax) to alter target\ax
          /invoke ${target[clear]}
          /break
        }        

        /varcalc _loopbreak ${_loopbreak}+1
        /if (${_loopbreak} > ${maHard.Find[stPullMobTagTry].Value}) {
          DEBUG ${break}mob_tag${sep}\a-wloop:${_loopbreak}\ax
          /attack off
          /return tagfailure
        }

      }

      /return TRUE
    }

    | aa/spell/disc/item tag
    /if (${maPull.Find[stPullWith].Value.Equal[other]}) {
      | /if (${is_ready[${_debug}, "${maCC.Find[swRangeOther].Value}"]}) {
        DEBUG ${sep}Tag${sep}Other${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax

|**
        | get the range for whatever it is we are using
        /declare _range int local

        /if (${Me.AltAbilityReady[${maCC.Find[swRangeOther].Value}]}) {
          DEBUG ${sep}\a-wALT\ax
          /varset _range ${Spell[${maCC.Find[swRangeOther].Value}].MyRange}

        | spell/song
        } else /if (${Me.Book[${Spell[${maCC.Find[swRangeOther].Value}].RankName}]}) {
          DEBUG ${sep}SPELL
          /varset _range ${Spell[${maCC.Find[swRangeOther].Value}].MyRange}

        | disc
        } else /if (${Me.CombatAbilityReady[${Me.CombatAbility[${Spell[${maCC.Find[swRangeOther].Value}].RankName}]}]}) {
          DEBUG ${sep}DISC
          /varset _range ${Spell[${maCC.Find[swRangeOther].Value}].MyRange}

        | item
        } else /if (${Me.ItemReady[=${maCC.Find[swRangeOther].Value}]}) {
          DEBUG ${sep}ITEM${sep}\a-w${Spell[${FindItem[=${maCC.Find[swRangeOther].Value}].Spell}].MyRange}\ax
          /varset _range ${Spell[${FindItem[=${maCC.Find[swRangeOther].Value}].Spell}].MyRange}

        | we have no idea what this is!
        } else {
          /invoke ${out[0, 0, 8]}
          /invoke ${out[0, 0, 9]}

          /varset _range 0
        }
**|

        | make sure we are stopped
        /if (!${Select[${Me.Class.ShortName},BRD]}) {
          /declare _breakmove int local 1
          /while (${Me.Moving} || ${Navigation.Active}) {
            DEBUG ${sep}stop tag${sep}\a-wnavigation\ax (\a-r${_breakmove}\ax)
            /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
            /varcalc _breakmove ${_breakmove}+1
            /if ((!${Me.Moving} && !${Navigation.Active}) || ${_breakmove} > 4) {
              /break
            }
            /delay 1s !${Me.Moving} && !${Navigation.Active}
          }
        }

        | out of range failure
        | /if (${Spawn[${_tmpID}].Distance} > ${_range}) {
        |   DEBUG ${break}pull_mob_tag${sep}cast \arfailure\ax. target is out of cast range
        |   /return tagfailure
        | }

        DEBUG ${sep}Entering _loopbreak OTHER
        /while (${_loopbreak} <= ${maHard.Find[stPullMobTagTry].Value}) {

          | get its attention
          /call pull_get_other_type ${_debug} ${_tmpID}
          /delay 5
          GETINPUT

          | have the targets attention. can go back
          /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
            DEBUG ${break}pull_mob_tag${sep}tot\ag==\axme (\a-r${_loopbreak}\ax)
            /break
          }

          | i'm hurt more then i should be
          | /if (${Me.PctHPs} <= ${Math.Calc[${maHeal.Find[stHealPoint${Me.Class.ShortName}].Value}*.PCTHO]}) {
          |   DEBUG ${break}pull_mob_tag${sep}I'm hurt wtf?
          |   /break
          | }

          /if (${Select[${maData.Find[stCastReturn].Value},DISTRACTED,NOTREADY,CANNOTSEE]}) {
            DEBUG ${break}pull_mob_tag${sep}cast \arfailure\ax. reacquire through game play
            /return tagfailure
          }

          /varcalc _loopbreak ${_loopbreak}+1
          /if (${_loopbreak} > ${maHard.Find[stPullMobTagTry].Value}) {
            DEBUG ${break}mob_tag${sep}\a-wloop:${_loopbreak}\ax
            /return tagfailure
          }

        }

      /return TRUE
    }

    | range tag
    | there are some base limitations to combat range tagging. we are going to limit to 250 for range/thrown items
    | yes i know some classes can go a lot farther..
    /if (${maPull.Find[stPullWith].Value.Equal[range]} && ${Spawn[id ${_tmpID}].Distance} > ${maHard.Find[stMinBowDist].Value} && ${Spawn[id ${_tmpID}].Distance} < ${maHard.Find[stMaxBowDist].Value}) {
      DEBUG ${sep}Tag${sep}Range${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax${sep}\a-w${Spawn[id ${_tmpID}].Distance}\ax
      GETINPUT pull_mob_tag_range NA|NA

      | swap bandoliers? and mark what we have
      /if (${Bool[${maPull.Find[stRangeBandolier].Value}]} && !${Me.Bandolier[${maPull.Find[stRangeBandolier].Value}].Active}) {
        /invoke ${maData.Add[stMainhand,${Me.Inventory[mainhand].Name}]}
        /invoke ${maData.Add[stOffhand,${Me.Inventory[offhand].Name}]}
        /invoke ${maData.Add[stRanged,${Me.Inventory[ranged].Name}]}
        /invoke ${maData.Add[stAmmo,${Me.Inventory[ammo].Name}]}
        /delay 1
        /invoke ${Me.Bandolier[${maPull.Find[stRangeBandolier].Value}].Activate}
      }

      | make sure we are stopped
      /if (!${Select[${Me.Class.ShortName},BRD]}) {
        /declare _breakmove int local 1
        /while (${Me.Moving} || ${Navigation.Active}) {
          DEBUG ${sep}stop tag${sep}\a-wnavigation\ax (\a-r${_breakmove}\ax)
          /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
          /varcalc _breakmove ${_breakmove}+1
          /if ((!${Me.Moving} && !${Navigation.Active}) || ${_breakmove} > 4) /break
          /delay 5 !${Me.Moving} && !${Navigation.Active}
        }
      }

      | check for ammo if needed
      /if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID}) {
        /call check_buff_cycle_ammo ${maDebug.Find[ammo].Value} TRUE
      }

      /if (${Target.ID}) {
        /range
      }

      /while (${Target.AggroHolder.Name.NotEqual[${Me.DisplayName}]} || ${_count} < 4) {
        /if (${Target.AggroHolder.Name.Equal[${Me.DisplayName}]}) /break
        /if (${Target.ID} && ${Spawn[id ${_tmpID}].Distance} > ${maHard.Find[stMinBowDist].Value}) {
          /range
          /delay 5
          /varcalc _count ${_count}+1
        } else {
          /return FALSE
        }
      }

      | return the previous gear for the swap
      /if (${Bool[${maPull.Find[stRangeBandolier].Value}]} && ${Me.Bandolier[${maPull.Find[stRangeBandolier].Value}].Active}) {
        /if (${Bool[${maData.Find[stMainhand].Value}]}) {
          /call Bind_command_swap "${maData.Find[stMainhand].Value}" 13 ${_debug}
        }
        /if (${Bool[${maData.Find[stOffhand].Value}]}) {
          /call Bind_command_swap "${maData.Find[stOffhand].Value}" 14 ${_debug}
        }
        /if (${Bool[${maData.Find[stRanged].Value}]}) {
          /call Bind_command_swap "${maData.Find[stRanged].Value}" 11 ${_debug}
        }
        /if (${Bool[${maData.Find[stAmmo].Value}]}) {
          /call Bind_command_swap "${maData.Find[stAmmo].Value}" 22 ${_debug}
        }
      }

      /return TRUE
    }

    | inbetween melee and range distance.. you know that fucked up mo mans land spot...
    /if (MELEE && ${Spawn[id ${_tmpID}].Distance} > ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} && ${Spawn[id ${_tmpID}].Distance} <= ENV) {
      DEBUG ${sep}Tag${sep}Melee${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax
      GETINPUT pull_mob_tag_both NA|NA
      /attack on

      DEBUG ${sep}${Spawn[id ${_tmpID}].Distance} > ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
      DEBUG ${sep}Tag${sep}Melee${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax \agNAVTO\ax
      /nav id ${_tmpID} |dist=${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} log=${maEntropy.Find[swNavLogSpew].Value}
      /delay 5 ${Me.Moving}
      /return TRUE
    }

  } else /if (!${Spawn[id ${_tmpID}].LineOfSight} && !${maOver.Find[swOverLOS].Value}) {
    DEBUG ${sep}Target${sep}\arLOS\ax\a-w${_tmpID}\ax
    /return tagfailure

  }

  /if (${Spawn[id ${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} && MELEE && !RANGE) {
    DEBUG ${sep}Tag${sep}Melee NO RANGE${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax
    GETINPUT pull_mob_tag_fail NA|NA
    /attack on
    /return TRUE

  }

/return FALSE



|***
 * note: Finds and sets stAssTarID
 * use: /call pull_mob_find DEBUG [# to force SpawnCount increment]
 ***|
sub pull_mob_find(bool _debug, int _spCount, string _tag)
  DEBUG \atpull_mob_find\ax(\a-w${_spCount}\ax)

  /if (!${Bool[${_spCount}]}) {
    /varset _spCount 0
  }

  /if (!${swModeToken}) {
    /if (HOME && !${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
      /call Bind_control_home return NULL NULL ${maDebug.Find[home].Value}
    }
  }

  /if (${_spCount}) {
    /varset stAssTarID 0
  }

  | we have an existign target and not using a mode
  /if (TARGET && !${_spCount} && !${swModeToken}) {
    /return TARGET
  }

  /declare _tmpID int local
  /declare _tmpAgroRange int local ${Int[${Math.Calc[PULLENV / ${maHard.Find[stPullMobFindStep].Value}]}]}
  /declare _count int local 0
  /declare _mobHP int local 999
  /declare _tmpHPAdj int local 0
  /declare _los string local los
  /declare _closestNAV int local PULLENV
  /declare _closestID int local 0
  /declare _navRange int local 0
  /declare _mobcount int local 0

  | /declare _tmpCombatRange int local ${maHard.Find[stMinBowDist].Value}

  | adjust LOS?
  /if (${maOver.Find[swOverLOS].Value} || ${Select[${maPull.Find[stPullMode].Value},nav,int,pet,calm,multi]}) {
    DEBUG ${sep} LOS overridden
    /varset _los
  }

  | time to step the agro range out so we get mobs closer first
  DEBUG ${sep}rad${sep}\aw PULLENV\ax zradius${sep}\a-w${maPull.Find[stPullZRadius].Value}\ax

  /while (${_tmpAgroRange} <= PULLENV && !TARGET) {
    /delay 1

    /if (${_tmpAgroRange} <= SAFEENV) {
      /varset _mobcount ${SpawnCount[npc ${_los} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
    } else {
      /varset _mobcount ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
    }

    | search local ENV range in steps for mob acquisition
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
        /if (${lsAnimationNoTarget.Contains[${Spawn[id ${_tmpID}].Animation}]}) {
          DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsAnimationNoTarget\ax (\a-r${Spawn[id ${_tmpID}].Animation}\ax)
          /varset _spCount 1
          /continue
        }

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
        /if (${Spawn[id ${_tmpID}].DisplayName.Length} < 1) {
          DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wDisplayName.Length < 1\ax
          /varset _spCount 1
          /continue
        }

        | bishes suffering from tunnel vision
        /if (${maPull.Find[stPullArcWidth].Value}) {
          |**
          /if (!${Defined[_tmpDegree]}) {
            /declare _tmpDegree float local 0
          }

          /varset _tmpDegree ${Spawn[${_tmpID}].HeadingTo[${maData.Find[stCampY].Value},${maData.Find[stCampX].Value}].Degrees}

          /echo 0:(${stPullArcLeft} >= ${stPullArcRight})
          /if (${stPullArcLeft} >= ${stPullArcRight}) {
              /echo 1:(${_tmpDegree} < ${stPullArcLeft} && ${_tmpDegree} > ${stPullArcRight}) return 0
              /if (${_tmpDegree} < ${stPullArcLeft} && ${_tmpDegree} > ${stPullArcRight}) /return 0
          } else {
            /echo 2:(${_tmpDegree} < ${stPullArcLeft} || ${_tmpDegree} > ${stPullArcRight}) return 0
              /if (${_tmpDegree} < ${stPullArcLeft} || ${_tmpDegree} > ${stPullArcRight}) /return 0
          }
          **|
        }

        | check navigation path validity for all but base pulling
        /if (!${Navigation.PathExists[id ${_tmpID}]} && !${maOver.Find[swOverNavCheck].Value} && !${Select[${maPull.Find[stPullMode].Value},base]}) {
          DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wNavigation.PathExists[\a-yid ${_tmpID}\ax]\ax (\a-r${Spawn[id ${_tmpID}].DisplayName}\ax)
          /varset _spCount 1
          /continue
        }

        | pet tagging mezed mobs only (should be skipped. pets cannot break mez)
        | skip this if we are the group main assist
        /if (${Group.MainAssist.Name.NotEqual[${Me.DisplayName}]}) {
          /if (${maPull.Find[stPullMode].Value.Equal[pet]} && ${lsAnimationMezed.Contains[${Spawn[id ${_tmpID}].Animation}]}) {
            /continue
          }
        }

        | check nav path length
        /if (${maPull.Find[swNavPathLogic].Value}) {
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
        }

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
        /if (${lsZoneNoMez.Contains[${Spawn[id ${_tmpID}].DisplayName}]} || ${lsZoneNamed.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
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

    /if (!AUTO && !${Select[${_tag},petfarm]}) {
      /return 0
    }

    /if (TARGET) {
      /return TARGET
    }

    | no target, try next step
    /varcalc _tmpAgroRange ${_tmpAgroRange}+${Int[${Math.Calc[PULLENV / ${maHard.Find[stPullMobFindStep].Value}]}]}
  }

  /if (!TARGET) {
    /delay 5
  }

/return TARGET



|***
 * note: memorize a spell/song to a specific gem if not memed
 * use: /call set_spell_to DEBUG [echo memming T/F] ["spell name"] [gem to mem it in] [wait for recycle to cast T/F]
 ***|
sub set_spell_to(bool _debug, bool _echo, string _spellname, string _spellgem, bool _wait)
  DEBUG \atset_spell_to\ax(\a-w${_echo}, ${_spellname}, ${_spellgem}, ${_wait}\ax)

  /if (${Me.Gem[${_spellname}]}) {
    /return TRUE
  }

  ISMEDEAD
  GETINPUT set_spell_to NA|NA

  /declare _ready bool FALSE

  | cant open a book with shit in hands
  /if (${Cursor.ID}) {
    /call check_cursor ${_debug} clear
  }

  | set timer for.. whatever
  /if (${_wait}) {
    /declare _breakTimer timer local 30s
  } else {
    /declare _breakTimer timer local 5s
  }

  DEBUG _breakTimer${sep}\a-w${_breakTimer}\ax

  | do we even have the spell?
  /if (!${Me.Book[${Spell[${_spellname}].RankName}]}) {
    /invoke ${out[0, "${notice}${sep}Bad Spell Name ${dot} ${_spellname}"]}
    /return FALSE
  }

  | just the facts ma'am
  /call get_gem_id FALSE "${_spellgem}"
  /declare _gem int local ${Macro.Return}

  /while (${Me.State.NotEqual[sit]} && !${Me.Mount.ID}) {
    /sit
    /delay 1s ${Me.State.Equal[sit]}
  }

  /if (${lsGrain.Contains[11]} && ${_echo}) {
    OUT Gem${_gem}${sep}\a-w${_spellname}\ax
  }

  /while (!${_ready}) {
    /memspell ${_gem} "${_spellname}"
    /delay 5
    /delay 30s ${Me.Gem[${_gem}].Name.Equal[${_spellname}]}

    /if (${Me.Gem[${_gem}].Name.Equal[${_spellname}]}) {
      /varset _ready TRUE
    }
  }

  | wait for spell to be ready
  /while (${_wait}) {
    DEBUG Waiting as requested${sep}\a-w${Math.Calc[${_breakTimer} / 10]}s\ax
    /if (!${_breakTimer}) /return FALSE
    /if (${Me.SpellReady[${_spellname}]}) /break
    /delay 2
  }

  /delay 5

/return TRUE



|***
 * note: corrects the Gem## and returns the # only #
 * use: /call get_gem_id DEBUG [gem##] | ${get_gem_id[DEBUG, gem##]}
 ***|
sub get_gem_id(bool _debug, string _spellGem)
  DEBUG \atget_gem_id\ax(\a-w${_spellGem}\ax)

  /declare _gem int local 0
  /if (${_spellGem.Length} == 1 || ${_spellGem.Length} == 4) {
    /varset _gem ${_spellGem.Right[1]}
  } else /if (${_spellGem.Length} == 2 || ${_spellGem.Length} == 5) {
    /varset _gem ${_spellGem.Right[2]}
  }

/return ${_gem}



|***
 * note: If the followng criteria are met, the calling function will be skipped.
 * use: ${check_exit[DEBUG]}
 ***|
sub check_exit(bool _debug, string _call)
  DEBUG \atcheck_exit\ax(\a-w${_call}\ax)

  | invis
  /if (${Me.Invis} && !${Select[${Me.Class.ShortName},ROG]}) /return TRUE

  | movement
  /if (${Me.Moving} && !${Select[${Me.Class.ShortName},BRD]}) /return TRUE

  | charm
  /if (${Me.SPA[22]}) /return TRUE

  | inhibit: 96=spell 191=skill
  /if (${Me.SPA[96]} || ${Me.SPA[191]}) /return TRUE

  | SELF STATES
  /if (${Me.Invulnerable.ID}) /return TRUE
  /if (${Me.State.Equal[FEIGN]}) /return TRUE
  /if (${Me.Ducking}) /return TRUE
  /if (${Me.Stunned}) /return TRUE
  /if (${Me.Charmed.ID}) /return TRUE
  /if (${Me.Mezzed.ID}) /return TRUE
  /if (${Me.Dead}) /return TRUE

  | check no cast list and mezz?
  /if (AUTO && ${Target.ID}) {
    /if (${lsZoneNoCast.Contains[${Target.DisplayName}]}) /return TRUE
    /if (${Target.Mezzed.ID} && !PULL) /return TRUE
  }

/return FALSE



|***
 * note: Check to see if toon is engaged in any way.
 * use: /call check_engaged DEBUG || ${check_engagedDEBUG}
 ***|
sub check_engaged(bool _debug)
  DEBUG \atcheck_engaged\ax()

  GETINPUT check_engaged NA|NA

  /if (${Select[${Me.Class.ShortName},ROG]}) {
    /if (AUTO && ${maEnv.Find[swSoS].Value}) /return FALSE
  }

  /if (${Me.Invis}) {
    /return TRUE
  }

  /if (${Me.Casting.ID}) {
    /return TRUE
  }

  | movement
  /if (${Me.Moving} && !${Select[${Me.Class.ShortName},BRD]}) {
    /return TRUE
  }

  | combat
  /if (${Me.Combat}) {
    /return TRUE
  }
  /if (${Me.AutoFire}) {
    /return TRUE
  }

  | am i the puller or main agro with mobs in range?
  /if (PULL && AGRO) {
    /if (${SpawnCount[npc OVERLOS radius 15*2 PS4]}) {
      /return TRUE
    }
  }
  /if (${Group.MainAssist.Name.Equal[${Me.DisplayName}]}) {
    /if (${SpawnCount[npc OVERLOS radius SAFEENV*2 PS4]}) {
      /return TRUE
    }
  }

/return FALSE



|***
 * note: rest/not rest routines.
 * use: /call do_rest
 ***|
sub do_rest(bool _debug)
  DEBUG \atdo_rest\ax()

  | the dead dont need to rest
  /if (!AUTO || ${swIsMeDead}) {
    /return FALSE
  }

  | dont rest while in tie
  /if (TIE) {
    /return FALSE
  }

  | clerics and pallys wait 'till yaulp gone
  /if (${Me.Buff[Yaulp].ID}) {
    /return FALSE
  }
  
  GETINPUT do_rest NA|NA

  /declare _needrest bool local FALSE
  /declare _resting bool local FALSE

  :loopneedmorerest
  DEBUG ${sep}loop${sep}\a-wloopneedmorerest\ax start
  /varset _needrest FALSE
  CHECKREZ
  /if (((!${maEnv.Find[swRestCombat].Value} || ${Me.CombatState.Equal[combat]}) && ${SpawnCount[npc OVERLOS radius ENV PS4]}) || !AUTO) {
    /varset _needrest FALSE
    /if (${Me.State.NotEqual[STAND]}) /stand
    /if (${Select[${Me.Class.ShortName},BRD]} && ${Me.BardSongPlaying}) {
      /call set_stop ${maDebug.Find[stop].Value} SILENT
    } else /if (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
      /stopcast
    }
    DEBUG ${sep}dont need rest
    /return FALSE
  }

  ISMEDEAD
  CHECKCURSOR
  CHECKTIE
  GETINPUT
  | check self click buffs
  /if (!${Me.Invis} && !${Select[${Me.Class.ShortName},ROG]} && ${maEnv.Find[swRestBuff].Value}) {
    /if (!${timer_Check_Buff_Cycle}) {
      /call check_buff_cycle ${maDebug.Find[buff].Value}
    }
  }

  /if (${maEnv.Find[swADVLoot].Value} && !${timer_Check_Loot}) {
    /if (AUTO && (${Group.Members} && ${Group.MasterLooter.ID} == ${Me.ID}) && ${Me.UseAdvancedLooting} && !${AdvLoot.LootInProgress} && !${Stick.Active}) /call do_loot ${maDebug.Find[loot].Value}
  }

  | bard rest song
  /if (${Me.Class.ShortName.Equal[BRD]} && !${Me.Invis}) {
    /if (${maChr.Find[swSongRest].Value} && !${Me.Song[${maChr.Find[stSongRest].Value}].ID}) {
      /invoke ${out[5, 0, 64]}
      /call set_spell_to FALSE FALSE "${Spell[${maChr.Find[stSongRest].Value}].RankName}" ${maEnv.Find[stBuffGem].Value} TRUE
      /cast ${Me.Gem[${Spell[${maChr.Find[stSongRest].Value}].RankName}]}
      /delay 3
      /if (${Me.AltAbility[Metronome]}) {
        /delay ${Spell[${Spell[${maChr.Find[stSongRest].Value}].RankName}].CastTime.Seconds}s
      } else {
        /delay ${Math.Calc[${Spell[${Spell[${maChr.Find[stSongRest].Value}].RankName}].CastTime.Seconds}+${Math.Calc[${Spell[${Spell[${maChr.Find[stSongRest].Value}].RankName}].CastTime.Seconds}*.05]}]}s
      }

      /while (${Window[CastingWindow].Open}) {
        /delay 3
      }
      /delay 5

    }
  }

  /if (PCTREST && !${Me.Moving} && ((${maEnv.Find[swRestCombat].Value} && ${Me.CombatState.NotEqual[combat]}) || !${SpawnCount[npc OVERLOS radius ${maEnv.Find[stRestRadius].Value} PS4]})) {

    | Hybrid: RNG,SHD,BST,PAL,BRD
    /if ((${lsClassMelee.CountOf[${Me.Class.ShortName}]} && ${lsClassCast.CountOf[${Me.Class.ShortName}]}) || ${Me.Class.ShortName.Equal[BRD]}) {

      | rest to full requirement
      /if (${maEnv.Find[swRestFull].Value} && ${_resting}) {
        /if (${Me.PctEndurance} < 99*.PCTHO) /varset _needrest TRUE
        /if (${Me.PctMana} < 99*.PCTHO) /varset _needrest TRUE
      } else {
        /if (${Me.PctEndurance} < PCTREST*.PCTHO) /varset _needrest TRUE
        /if (${Me.PctMana} < PCTREST*.PCTHO) /varset _needrest TRUE
      }

      DEBUG ${sep}swRestFull${sep}\a-w${_needrest}\ax

      | bards suck...
      /if (${Me.Class.ShortName.Equal[BRD]}) {
        /call cast_AA_rallying FALSE
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
        /if (${Me.PctEndurance} < PCTREST*.PCTHO) /varset _needrest TRUE
        /if (${Me.PctMana} < PCTREST*.PCTHO) /varset _needrest TRUE
        /if (${maChr.Find[swSongRest].Value}) /varset _needrest TRUE

      } else {
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
        /call cast_stamina_recovery FALSE
      }
      /if (${Select[${Me.Class.ShortName},PAL]}) {
        /if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) /call check_harmonious
      } else /if (${Select[${Me.Class.ShortName},SHD]}) {
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
        /if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) /call check_harmonious

      } else /if (${Select[${Me.Class.ShortName},RNG]}) {
      } else /if (${Select[${Me.Class.ShortName},BST]}) {
        /if (!${timer_Check_Para}) /call check_mana ${maDebug.Find[para].Value}
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
      }

      DEBUG ${sep}hybrid _needrest${sep}\a-w${_needrest}\ax${sep}

    | Melee: WAR,MNK,ROG,BER
    } else /if (${lsClassMelee.CountOf[${Me.Class.ShortName}]} && !${lsClassCast.CountOf[${Me.Class.ShortName}]}) {
      /if (${Me.PctEndurance} < PCTREST*.PCTHO) /varset _needrest TRUE

      | rest to full requirement
      /if (${maEnv.Find[swRestFull].Value}) {
        /if (${Me.PctEndurance} < 99*.PCTHO) /varset _needrest TRUE
      }

      DEBUG ${sep}melee _needrest${sep}\a-w${_needrest}\ax

      /call cast_stamina_recovery
      /if (${Select[${Me.Class.ShortName},WAR]}) {
      } else /if (${Select[${Me.Class.ShortName},MNK]}) {
      } else /if (${Select[${Me.Class.ShortName},ROG]}) {
        /if (AUTO && ${maEnv.Find[swSoS].Value}) /call set_sos FALSE
      } else /if (${Select[${Me.Class.ShortName},BER]}) {
      }

    | Caster: CLR,DRU,SHM,ENC,WIZ,NEC,MAG
    } else /if (!${lsClassMelee.CountOf[${Me.Class.ShortName}]} && ${lsClassCast.CountOf[${Me.Class.ShortName}]}) {
      /if (${Me.PctMana} < PCTREST*.PCTHO) {
        /varset _needrest TRUE
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
      }

      | rest to full requirement
      /if (${maEnv.Find[swRestFull].Value}) {
        /if (${Me.PctMana} < 99*.PCTHO) /varset _needrest TRUE
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery FALSE
      }

      /if (${Select[${Me.Class.ShortName},CLR]}) {
        /if (${maCC.Find[swCombatHeal].Value} && ${SpawnCount[npc radius ENV PS4]}) {
          /if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
            /call check_heal FALSE Group
          }
          /if (${maHeal.Find[swHealXTarget].Value}) /call check_heal FALSE XTarget
        }
      } else /if (${Select[${Me.Class.ShortName},DRU]}) {
        /if (${maCC.Find[swCombatHeal].Value} && ${SpawnCount[npc radius ENV PS4]}) {
          /if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
            /call check_heal FALSE Group
          }
          /if (${maHeal.Find[swHealXTarget].Value}) /call check_heal FALSE XTarget
        }

      } else /if (${Select[${Me.Class.ShortName},SHM]}) {
        /call check_cani ${maDebug.Find[canni].Value}
        /if (${maCC.Find[swCombatHeal].Value} && ${SpawnCount[npc radius ENV PS4]}) {
          /if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
            /call check_heal FALSE Group
          }
          /if (${maHeal.Find[swHealXTarget].Value}) /call check_heal FALSE XTarget
        }
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
      } else /if (${Select[${Me.Class.ShortName},ENC]}) {
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
      } else /if (${Select[${Me.Class.ShortName},WIZ]}) {
      } else /if (${Select[${Me.Class.ShortName},NEC]}) {
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
      } else /if (${Select[${Me.Class.ShortName},MAG]}) {
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
        /if (${maChr.Find[stPctGather].Value} && !${timer_Check_Gather}) /call check_gather ${maDebug.Find[gather].Value}
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
      }

      DEBUG ${sep}caster _needrest${sep}\a-w${_needrest}\ax

    }
  }

  /if (${_needrest}) {
    SETHUD "rest"
    /if (${Me.State.NotEqual[sit]} && !${Me.Mount.ID}) /sit
    /varset _resting TRUE
    /delay 2
    CHECKTIE
    ISMEDEAD
    GETINPUT do_rest_loop NA|NA
    /goto :loopneedmorerest
  }

  SETHUD EMPTY

/return TRUE



|***
 * note: Acquire target, get bitches. This will establish ${stAssTarID}
 * use: /call get_assist_target DEBUG
 ***|
sub get_assist_target(bool _debug)
  DEBUG \atget_assist_target\ax()

  /if (!AUTO) {
    /return
  }

  CHECKDEAD
  GETINPUT get_assist_target NA|NA

  | if were not assisting.. don't assist
  /if (!${Bool[MA1]} && !${Bool[MA2]} && !${Select[${maCC.Find[stAssistMode].Value},g,1,2,3]}) {
    DEBUG ${break}\arNO\ax assist
    /delay 1
    /return FALSE
  }

  | cannnot assist yourself
  /if (${Group.MainAssist.ID} == ${Me.ID} && !${maOver.Find[swOverGroupAss].Value}) {
    DEBUG ${break}cannot assist self
    /return FALSE
  }

  /if (${Spawn[MA1].ID} == ${Me.ID} || ${Spawn[=${Raid.MainAssist}].ID} == ${Me.ID}) {
    /invoke ${do_raw_edit[${_debug}, TRUE, stManualAssist1, FALSE]}
  }
  /if (${Spawn[MA2].ID} == ${Me.ID} || ${Spawn[=${Raid.MainAssist}].ID} == ${Me.ID}) {
    /invoke ${do_raw_edit[${_debug}, TRUE, stManualAssist2, FALSE]}
  }

  | if we dont have xtargetsd, skip ALL smartassist go directly to jail.
  /if (!${Me.XTargetSlots}) {
    /goto :skippingtoMAtarget
  }

  | tar acquisition when using smart assist. set XTarget positions.
  /if (${Select[${maCC.Find[stAssistMode].Value},g,1,2,3]} && ${Group.MainAssist.ID}) {
    DEBUG ${sep}Smart Assist${sep}\a-w${maCC.Find[stAssistMode].Value}\ax

    | skip corpses and me
    /if (${Spawn[id ${stAssTarID}].Dead} || ${stAssTarID} == ${Me.ID}) {
      DEBUG ${break} bad target${sep}\a-wcorpse or self\ax
      /varset stAssTarID 0
      /return FALSE
    }

    | if the raid/group roles are not set or dead, move on tho stManualAssist1/2
    /if (${Select[${maCC.Find[stAssistMode].Value},g]}) {
      /if (!${Group.MainAssist.ID}) /goto :skippingtoMAtarget
      /if (${Bool[${Spawn[${Group.MainAssist}].Dead}]}) /goto :skippingtoMAtarget
    } else /if (${Select[${maCC.Find[stAssistMode].Value},1,2,3]}) {
    }

    | set the 1st xtarget slot to the assists target
    /if (${Select[${maCC.Find[stAssistMode].Value},g]}) {
      DEBUG ${sep}set Xtarget${sep}\a-wg\ax
      /if (${Me.XTarget[1].TargetType.NotEqual[Group Assist Target]} && ${Group.Members}) {
        /xtarget set 1 groupassisttarget
      }
    } else /if (${Select[${maCC.Find[stAssistMode].Value},1]}) {
      DEBUG ${sep}set Xtarget${sep}\a-w1\ax
      /if (${Me.XTarget[1].TargetType.NotEqual[Raid Assist 1 Target]} && ${Raid.Members}) {
        /xtarget set 1 raidassist1target
      }
    } else /if (${Select[${maCC.Find[stAssistMode].Value},2]}) {
      DEBUG ${sep}set Xtarget${sep}\a-w2\ax
      /if (${Me.XTarget[1].TargetType.NotEqual[Raid Assist 2 Target]} && ${Raid.Members}) {
        /xtarget set 1 raidassist2target
      }
    } else /if (${Select[${maCC.Find[stAssistMode].Value},3]}) {
      DEBUG ${sep}set Xtarget${sep}\a-w3\ax
      /if (${Me.XTarget[1].TargetType.NotEqual[Raid Assist 3 Target]} && ${Raid.Members}) {
        /xtarget set 1 raidassist3target
      }
    }

    | smart assist group
    DEBUG ${sep}GET Target

    | set main target variable
    /if (${Me.XTarget[1].ID} && TARGET != ${Me.XTarget[1].ID}) {
      DEBUG ${sep}setting${sep}stAssTarID ${Me.XTarget[1].ID}
      /varset stAssTarID ${Me.XTarget[1].ID}
    }

    | set actual pc target
    /if (${Spawn[id ${stAssTarID}].Distance} <= ENV && ${stAssTarID}) {
      /if (MELEE || RANGE || NUKE || DOT) {
        /if (${Me.XTarget[1].PctHPs} <= ${maCC.Find[stPctEngage].Value} || ${maOver.Find[swOverPctEngage].Value}) {
          DEBUG targeting${sep}stAssTarID${sep}${maCC.Find[stPctEngage].Value}
          /if (${Target.ID} != ${stAssTarID} && !${Spawn[${stAssTarID}].Dead}) {
            /squelch /target id ${stAssTarID}
            /delay 1s ${Target.ID} == ${stAssTarID}
          }
        }
      }
    }

    /return TRUE
  }


  | target acquisition when using manual assist.
  :skippingtoMAtarget
  /if ((${Bool[MA1]} || ${Bool[MA2]}) || !${Bool[${Group.MainAssist}]}) {
  DEBUG ${sep}Manual Assist${sep}\a-w${maCC.Find[stAssistMode].Value}\ax

    | assist ma1
    /if (${Spawn[MA1].ID} && !${Spawn[MA1].Dead}) {
      DEBUG ${sep}assisting${sep}\a-wMA1\ax
      /if ((MELEE || RANGE || NUKE || DOT)) {
        /squelch /assist MA1
      }

    | assist ma2
    } else /if (${Spawn[MA2].ID} && !${Spawn[MA2].Dead} && (!${Spawn[MA1].ID} || ${Spawn[MA1].Dead})) {
      DEBUG ${sep}assisting${sep}\a-wMA2\ax
      /if ((MELEE || RANGE || NUKE || DOT)) {
        /squelch /assist MA2
      }
    }

    | /delay 2s ${Me.AssistComplete}
    /if (!TARGET && ${Target.ID}) {
      /varset stAssTarID ${Target.ID}
    } else /if (TARGET != ${Target.ID} && TARGET != 0 && ${Target.ID} && ${Spawn[${Target}].Type.Equal[npc]}) {
      /delay 1
      /varset stAssTarID 0
      /return TRUE
    }

  }


  CHECKDEAD
  GETINPUT
  /if (TARGET == ${Target.ID}) {
    /if (${Target.PctHPs} > ${maCC.Find[stPctEngage].Value} && !${maOver.Find[swOverPctEngage].Value}) {
      /delay 5
      /goto :skippingtoMAtarget
    }
  | } else {
  |   /if (${Target.ID}) {
  |     /squelch /target clear
  |   }
  |   /return TRUE
  }

/return



|***
 * note: creates hooks for data\custom.inc
 * use: GETINPUT [sub] [variable]
 ***|
sub register_hook(string _sub, string _variable)

  /if (!${maEntropy.Find[swHookSub].Value}) {
    /return FALSE
  }

  /if (${maHook.Contains[hook_${_sub}]}) {
    /if (${maHook.Find[hook_${_sub}].Value.Arg[2,|].NotEqual[${_variable.Arg[2,|]}]}) {
      /invoke ${maHook.Add[hook_${_sub},${_variable}]}
    }

    /if (${SubDefined[hook_${_sub}]}) {
      /call hook_${_sub} ${_variable}
      /return ${Macro.Return}
    }

  } else {
    /invoke ${maHook.Add[hook_${_sub},${_variable}]}
  }

/return FALSE



|***
 * note: updates working zone lists
 * use: /call set_zone_lists DEBUG
 ***|
sub set_zone_lists(bool _debug)
  DEBUG \atset_zone_lists\ax()

  /if (${maDebug.Find[area].Value}) {
    /varset _debug TRUE
  }

  | check zone exists
  /while (TRUE) {
    /sqlite query "${DBzone}" QZone SELECT count(*) as data from zone_control where zone_shortname='${Zone.ShortName}';
    /if (!${sql_check[${_debug}, QZone, set_zone_lists, "SELECT count(*)"]}) /endmacro

    | if it doesnt exist, make it
    /if (!${sqlite.Rows[QZone]}) {
      /sqlite query "${DBzone}" QZone INSERT INTO zone_control VALUES ('${Zone.ShortName}', 'swZoneSafe', 'FALSE');
      | /if (!${sql_check[${_debug}, QZone, set_zone_lists, "INSERT INTO (zone_control)"]}) /endmacro
    }
    /if (${sqlite.Resultcode[QZone]} != 0) {
      /delay 1
    } else /if (${sqlite.Resultcode[QZone]} == 0) {
      /break
    }
  }

  | get safe zone
  /sqlite query "${DBzone}" QZone SELECT value FROM zone_control WHERE zone_shortname='${Zone.ShortName}' AND element='swZoneSafe';

  /if (!${sqlite.Rows[QZone]}) {
    /sqlite query "${DBzone}" QZone INSERT INTO zone_control VALUES ('${Zone.ShortName}', 'swZoneSafe', 'FALSE');
    /invoke ${maEnv.Add[swZoneSafe,FALSE]}
  } else {
    /invoke ${maEnv.Add[swZoneSafe,${sqlite.Result[QZone 1 value]}]}
  }

  | load zone properties
  /declare _lizl listiterator local
  /vardata _lizl lsZoneProperty.First.Clone
  /while (!${_lizl.IsEnd}) {
    /invoke ${${_lizl.Value}.Clear}

    /while (TRUE) {
      /sqlite query "${DBzone}" QZone SELECT count(*) AS data FROM zone_control WHERE zone_shortname='${Zone.ShortName}' and element='${_lizl.Value}';
      /if (${sqlite.Resultcode[QZone]} != 0) {
        /delay 1
      } else /if (${sqlite.Resultcode[QZone]} == 0) {
        /break
      }
    }

    /if (${sqlite.Result[QZone 1 data]} > 0) {
      /sqlite query "${DBzone}" QZone SELECT group_concat(value,'|') AS data FROM zone_control WHERE zone_shortname = '${Zone.ShortName}' and element = '${_lizl.Value}' group by element;
      /invoke ${${_lizl.Value}.Append[${sqlite.Result[QZone 1 data]}]}
    }

    /invoke ${_lizl.Advance}
  }

/return TRUE



|***
 * note: find a variables mapping
 * use: ${get_key_map[DEBUG, variable, validate t/f]} returns the mapname
 ***|
sub get_key_map(bool _debug, string _key, bool _validate)
  DEBUG\atget_key_map\ax(\a-w${_key}\ax)

  /declare _liglm listiterator local
  /vardata _liglm lsMaps.First.Clone
  /while (!${_liglm.IsEnd}) {
    DEBUG ${sep}lsMaps${sep}\a-w${_liglm.Value}\ax${sep}contains(\aw${_key}\ax)${If[${${_liglm}.Contains[${_key}]}, ${dot} \agFound\ax,]}
    /if (${${_liglm.Value}.Contains[${_key}]}) {
      DEBUG ${sep}lsMaps${sep}\agReturning\ax${sep}\a-w${_liglm.Value}\ax
      /return ${_liglm.Value}
    }
    /invoke ${_liglm.Advance}
  }
  DEBUG ${sep}Map Returning${sep}\arNO MAP FOUND\ax

/return FALSE



|***
 * note: find a variables DB table
 * use: ${get_key_table[DEBUG, variable]} returns the table name
 ***|
sub get_key_DBLOC(bool _debug, string _key)
  DEBUG \atget_key_DBLOC\ax(\a-w${_key}\ax)

  /declare _mikey mapiterator local
  /vardata _mikey maDBTables.First.Clone
  /while (!${_mikey.IsEnd}) {
    /sqlite query "${DBcharacter}" dummy SELECT * FROM '${_mikey.Key}' WHERE ${_mikey.Value}='${_key}';
    /if (${sqlite.Rows[dummy]}) {
      DEBUG ${sep}DBLOC Returning${sep}\a-w|${_mikey.Key}|${_mikey.Value}\ax
      /return |${_mikey.Key}|${_mikey.Value}
    }
    /invoke ${_mikey.Advance}
  }
  DEBUG ${sep}DBLOC Returning${sep}\a-wNOT FOUND IN DB\ax

/return FALSE



|***
 * note: user controls
 * use: hey look squirrels...
 * use: /call do_raw_edit DEBUG [silent T/F] [variable to change] [value to change it too|gem#|cursor] [add|delete|clear (for lists)] [not used yet]
 ***|
sub do_raw_edit(bool _debug, string _silent, string _variable, string _value, string _add_delete, string _verbage)
  DEBUG \atdo_raw_edit\ax(\aw${_silent}, ${_variable}, ${Parse[1,"${_value}"]}, ${_add_delete}, ${_verbage}\ax)

  | ** error control ** |

  | is it really a DB variable?
  /declare _DBLOC string local ${get_key_DBLOC[${_debug}, ${_variable}]}
  /declare _originalValue string local ${Parse[1,${_value}]}

  | variable error instructions
  /if ((!${_variable.Length} || ${_DBLOC.Equal[FALSE]}) && ${_value.NotEqual[clear]}) {
    /invoke ${out[16, "${notice}${sep}\ar/edit\ax${sep}invalid DB variable${sep}\a-w${_variable}\ax"]}
    /return FALSE
  }

  | error for add/delete
  /if (${_variable.Left[2].Equal[ls]} && (!${Select[${_add_delete},add,delete,override]} && ${_value.NotEqual[clear]})) {
    /invoke ${out[16, 0, 10]}
    /return FALSE
  }

  | errors for values
  /if (!${_value.Length}) {
    /invoke ${out[16, 0, 11]}
    /return FALSE
  }

  | ** end of error control .. start of actually getting something fucking done. ** |

  | does it have a ?
  /declare _map string local ${get_key_map[${_debug}, ${_variable}]}
  /declare _list bool local FALSE
  /declare _found bool local FALSE


  | environment_var edits. not for users...
  /if (${_DBLOC.Arg[1,|].Equal[environment_var]}) {

    /sqlite query "${DBcharacter}" dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}";
    /if (!${sql_check[${_debug}, dummy, do_raw_edit, UPDATE]}) /endmacro
    /varset ${_variable} ${_value}
    /return TRUE

  | set value as a gem
  } else /if (${_value.Find[gem]} && ${_value.Length} < 6) {

    /declare _gemN int local ${get_gem_id[FALSE, ${_value}]}

    /if (!${Range.Between[1,${Me.NumGems}:${_gemN}]}) {
      /invoke ${out[16, 0, 65]}
      /invoke ${out[16, 0, 66]}
      /return FALSE
    }

    | adjust to the loaded spell
    /varset _value ${Me.Gem[${_gemN}].BaseName}

  | set value as cursor
  } else /if (${_value.Equal[cursor]}) {
    /if (!${Cursor.ID}) {
      /invoke ${out[16, ENTROPY, 12]}
      /return FALSE
    }
    /varset _value ${Cursor.Name}

  | return to default settings
  } else /if (${_value.Equal[pregen]}) {

    DEBUG ${sep}DATA \agdoes exists\ax \arWE ARE RESETTING IT\ax
    /declare _count int local 1
    /declare _tmpList string local

    DEBUG GET PREGEN DATA
    /sqlite query "${DBstatic}" dummy SELECT * FROM env_${Me.Class.ShortName.Lower} WHERE env_var='${_env_var}';
    /if (!${sql_check[${_debug}, dummy, do_raw_edit, SELECT]}) /endmacro

    | how many do we want to find?
    /if (${maPREGEN.Find[${_value}].Value}) {
    /while (${_count} <= ${maPREGEN.Find[${_value}].Value}) {
      DEBUG ${sep}${sqlite.Result[dummy 1 value].Arg[${_count},|]}
      | is it in the book?
      /if (${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]}) {
        /if (${Bool[${_tmpList}]}) /varset _tmpList ${_tmpList}|
        /varset _tmpList ${_tmpList}${sqlite.Result[dummy 1 value].Arg[${_count},|]}
        /varcalc _count ${_count} + 1
      }
    }

    | adjust our running value for the database entry
    DEBUG ${sep} _tmpList ${_tmpList}
    /varset _value ${_tmpList}
    DEBUG ${sep} _value ${_tmpList}

    | not a list or a single entry
    } else {
      /varset _value ${sqlite.Result[dummy 1 value]}
      DEBUG ${sep} _value ${sqlite.Result[dummy 1 value]}
    }

  }

  | clear/reset an existing list
  /if (${_variable.Left[2].Equal[ls]} && ${_value.Equal[clear]}) {
    DEBUG ${sep}\ayclear a list\ax

    /declare _tmpList list local
    /invoke ${_tmpList.Delimiter["|"]}

    /sqlite query "${DBcharacter}" dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_variable}';
    /if (!${sql_check[${_debug}, dummy, do_raw_edit, SELECT]}) /endmacro
    /invoke ${_tmpList.Append[${sqlite.Result[dummy 1 value]}]}
    DEBUG ${sep}DB current count${sep}\aw${_tmpList.Count}\ax

    /varset _value EMPTY
    /varset _list TRUE

  | override switch for build control
  | this is not for normal users. keep your fucking mits off this.
  } else /if (${_add_delete.Equal[override]}) {
    /varset _value ${_value}
    /if (${_variable.Left[2].Equal[ls]}) {
      /varset _list TRUE
    } else /if (${_variable.Left[2].Equal[ma]}) {
      /varset _map TRUE
    } else {
      /varset _found TRUE
    }

  | add to an existig or default value
  } else /if (${_add_delete.Equal[add]}) {
    DEBUG ${sep}\ayadd\ax
    /declare _tmpList list local
    /invoke ${_tmpList.Delimiter["|"]}

    /sqlite query "${DBcharacter}" dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var="${_variable}";
    /if (!${sql_check[${_debug}, dummy, do_raw_edit, SELECT]}) /endmacro
    /invoke ${_tmpList.Append[${sqlite.Result[dummy 1 value]}]}
    DEBUG ${sep}DB current count${sep}\aw${_tmpList.Count}\ax

    /if (!${_tmpList.Contains["${Parse[2,${_value}]}"]}) {
      DEBUG ${sep}String add does not exists${sep}\a-w"${Parse[1,${_value}]}"\ax
      /if (${_tmpList.Contains[EMPTY]} && ${_tmpList.Count} == 1) {
        DEBUG clearing \arEMPTY\ax list
        /invoke ${_tmpList.Clear}
      }
      /invoke ${_tmpList.Append["${Parse[1,${_value}]}"]}
      DEBUG ${sep}DB New count${sep}\aw${_tmpList.Count}\ax
      /declare _tmpString string local
      /declare _li listiterator local
      /vardata _li _tmpList.First.Clone
      /while (!${_li.IsEnd}) {
        DEBUG ${sep}${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}\a-w${_li.Value}\ax
        /varset _tmpString ${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}${_li.Value}
        /invoke ${_li.Advance}
      }
      DEBUG ${sep}_tmpString${sep}\ay${_tmpString}\ax
      /varset _value ${_tmpString}
      /varset _list TRUE

    }

  | delete an existing value
  } else /if (${_add_delete.Equal[delete]}) {
    DEBUG ${sep}\aydelete\ax
    /declare _tmpList list local
    /invoke ${_tmpList.Delimiter["|"]}

    /sqlite query "${DBcharacter}" dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_variable}';
    /if (!${sql_check[${_debug}, dummy, do_raw_edit, SELECT]}) /endmacro
    /invoke ${_tmpList.Append[${sqlite.Result[dummy 1 value]}]}
    DEBUG ${sep}DB current count${sep}\aw${_tmpList.Count}\ax

    /if (${_tmpList.Contains["${Parse[1,${_value}]}"]}) {
      DEBUG ${sep}String add exists${sep}\a-w"${Parse[1,${_value}]}"\ax
      /invoke ${_tmpList.Remove["${Parse[1,${_value}]}"]}
      /if (${_tmpList.Count} == 0) {
        DEBUG populating \arEMPTY\ax list
        /invoke ${_tmpList.Append[EMPTY]}
      }
      DEBUG ${sep}DB New count${sep}\aw${_tmpList.Count}\ax
      /declare _tmpString string local
      /declare _li listiterator local
      /vardata _li _tmpList.First.Clone
      /while (!${_li.IsEnd}) {
        DEBUG ${sep}${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}\a-w${_li.Value}\ax
        /varset _tmpString ${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}${_li.Value}
        /invoke ${_li.Advance}
      }
      DEBUG ${sep}_tmpString${sep}\ay${_tmpString}\ax
      /varset _value ${_tmpString}
      /varset _list TRUE

    }

  | everything else, if its a real variable, edit it..
  }

  | update the _value into the DB
  /if (${Bool[${_map}]} || ${_list} || ${_found}) {
    /sqlite query "${DBcharacter}" dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}";
    /if (!${sql_check[${_debug}, dummy, do_raw_edit, UPDATE]}) /endmacro

    | reload data
    /sqlite query "${DBcharacter}" dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}";
    /if (!${sql_check[${_debug}, dummy, do_raw_edit, SELECT]}) /endmacro
    DEBUG valuerow:${sqlite.Result[dummy 1 value]}
  }

  | repopulate the containers
  /if (${Bool[${_map}]}) {
    DEBUG MAP ADD
    DEBUG /invoke ${${_map}.Add[${_variable},"${sqlite.Result[dummy 1 value]}"]}
    /invoke ${${_map}.Add[${_variable},"${sqlite.Result[dummy 1 value]}"]}
  }

  /if (${_list}) {
    DEBUG LIST Append
    /invoke ${${_variable}.Clear}
    /invoke ${${_variable}.Append["${Parse[1,${_value}]}"]}
  }

  | outputs ???
  /if (${sqlite.Status[dummy].Equal[Success]} && ${_silent.NotEqual[SILENT]}) {
    /if (${_list}) {
      DEBUG LIST output
      /invoke ${out[16, "DB${sep}${_variable}\ax${sep}\a-w${Parse[1,${_originalValue}]}\ax ${dot} \a-y${_add_delete}\ax"]}
      /invoke ${out[16, "List Updated${sep}\a-w${_variable}\ax Count${sep}\a-w${${_variable}.Count}\ax"]}
    } else /if (${Bool[${_map}]}) {
      DEBUG MAP output
      /if (${lsGrain.Contains[24]}) {
        OUT /edit ${_variable}${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
      }

    } else /if (${_found}) {
      DEBUG FOUND output
      /if (${lsGrain.Contains[24]}) {
        OUT /edit ${_variable}\ax${sep}${num}${Parse[0,"${_value}"]}\ax
      }
    } else {

    }

  } else /if (${sqlite.Status[dummy].NotEqual[Success]}) {
    ECHO ${warning} \arERROR\ax${sep}\atdo_raw_edit\ax(\aw${_silent}, ${_variable}, ${Parse[1,${_value}]}, ${_add_delete}, ${_verbage}\ax)
  }

/return TRUE



|***
 * note: sets where a spire will be used
 * use: /call set_control_chr_spire [debug] [group|raid] [NOTUSED]
 ***|
sub set_control_chr_spire(bool _debug, string _type, string _verbage)
  DEBUG \atset_control_chr_spire\ax(\a-w${_type}\ax)

  /if (${_type.Equal[group]}) {
    /invoke ${set_switch_env[${_debug}, swSpireGroup, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[raid]}) {
    /invoke ${set_switch_env[${_debug}, swSpireRaid, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[con]}) {
    /call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSpireCon" "${_verbage}"
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[group,swSpireGroup]}
  /invoke ${maControl.Add[raid,swSpireRaid]}
  /invoke ${set_control_output[${_debug}, maChr, "chr spire", maControl]}

/return TRUE



|***
 * note: sets /chr invis
 * use: /call set_control_chr_invis [debug] [type] [NAME]
 ***|
sub set_control_chr_invis(bool _debug, string _type, string _verbage)
  DEBUG \atset_control_chr_spire\ax(\a-w"${_verbage}"\ax)

  /if (${_type.Equal[self]}) {
    /call do_raw_edit ${_debug} SILENT stInvisSingle "${_verbage}"
  } else /if (${_type.Equal[group]}) {
    /call do_raw_edit ${_debug} SILENT stInvisGroup "${_verbage}"
  } else /if (${_type.Equal[selfivu]}) {
    /call do_raw_edit ${_debug} SILENT stInvisSingleIVU "${_verbage}"
  } else /if (${_type.Equal[groupivu]}) {
    /call do_raw_edit ${_debug} SILENT stInvisGroupIVU "${_verbage}"
  } else /if (${_type.Equal[delay]}) {
    /call do_raw_edit ${_debug} SILENT stInvisDelay "${_verbage}"
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[delay,stInvisDelay]}
  /invoke ${maControl.Add[self,stInvisSingle|br]}
  /invoke ${maControl.Add[group,stInvisGroup|Br]}
  /invoke ${maControl.Add[selfivu,stInvisSingleIVU|br]}
  /invoke ${maControl.Add[groupivu,stInvisGroupIVU|br]}
  /invoke ${set_control_output[${_debug}, maChr, "chr invis", maControl]}

/return TRUE



|***
 * note: sets where intensity will be used
 * use: /call set_control_chr_intensity [debug] [group|raid] [NOTUSED]
 ***|
sub set_control_chr_intensity(bool _debug, string _type, string _verbage)
  DEBUG \atset_control_chr_intensity\ax(\a-w${_type}\ax)

  /declare _out string local

  /if (${_type.Equal[group]}) {
    /invoke ${set_switch_env[${_debug}, swIntensityGroup, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[raid]}) {
    /invoke ${set_switch_env[${_debug}, swIntensityRaid, TRUE, ${_verbage}]}

  } else /if (${_type.Equal[con]}) {
    /call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stAAIntensity" "${_verbage}"
  }

  /invoke ${maControl.Clear}
  /invoke ${maControl.Add[group,swIntensityGroup]}
  /invoke ${maControl.Add[raid,swIntensityRaid]}
  /invoke ${set_control_output[${_debug}, maChr, "chr intensity", maControl]}

/return TRUE



|***
 * note: sets healpoints
 * use: /call set_control_heal_point [debug] [%## heal at] [cls]
 ***|
sub set_control_heal_point(bool _debug, int _verbage, string _verbage2)
  DEBUG \atset_control_heal_point\ax(\a-w${_verbage}\ax)

  /if (${Bool[${_verbage}]}) {

    /declare _target string local FALSE
    /if (${lsClassAll.Contains[${_verbage2.Upper}]}) {
      /varset _target ${_verbage2.Upper}
    } else /if (${Target.ID}) {
      /varset _target ${Target.Class.ShortName}
    } else {
      OUT Usage${sep} /heal point ## [target|class shortname]
      /return FALSE
    }

    /invoke ${set_control_num_range[${_debug}, stHealPoint${_target}, "${_verbage}", 1, 99]}
  }

  /declare _out string local
  /declare _lichp listiterator local
  /vardata _lichp lsClassAll.First.Clone
  /while (!${_lichp.IsEnd}) {
    /varset _out ${_out} ${If[${_lichp.Value.Equal[${lsClassAll.First}]},,${dot}]} ${info}${_lichp.Value}\ax:${num}${maHeal.Find[stHealPoint${_lichp.Value}].Value}\ax
    /invoke ${_lichp.Advance}
  }

  /invoke ${out[12, "/heal point ${dot} ${_out}"]}

/return FALSE



|***
 * note: sets the XTarget Heal classes a character will use
 * use: /call set_character_XTHealClass [debug] [class short name|TARGET]
 ***|
sub set_control_chr_xtclass(bool _debug, string _verbage)
  DEBUG \atset_control_chr_xtclass\ax(\a-w${_verbage}\ax)

  | fix user entry
  /if (!${Bool[${_verbage}]} && ${lsClassAll.Contains[${Target.Class.ShortName}]}) {
    /varset _verbage ${Target.Class.ShortName}
  } else /if (${Bool[${_verbage}]} && ${lsClassAll.Contains[${_verbage.Upper}]}) {
    /varset _verbage ${_verbage.Upper}
  } else {
    /invoke ${out[0, 0, 13]}
  }

  /if (${lsClassAll.Contains[${_verbage}]}) {
    | if its there, delete
    /if (${lsHealXTClass.Contains[${_verbage}]}) {
      /call do_raw_edit ${_debug} SILENT lsHealXTClass "${_verbage}" delete

    | if its not there, add
    } else /if (!${lsHealXTClass.Contains[${_verbage}]}) {
      /call do_raw_edit ${_debug} SILENT lsHealXTClass "${_verbage}" add
    }
  }
  /declare _out string local
  /declare _lisccx listiterator local
  /vardata _lisccx lsClassAll.First.Clone
  /while (!${_lisccx.IsEnd}) {
    /varset _out ${_out} ${If[${_lisccx.Value.Equal[${lsClassAll.First}]},,${dot}]} ${info}${_lisccx.Value}\ax
    /if (${lsHealXTClass.Contains[${_lisccx.Value}]}) /varset _out ${_out}:${on}
    /if (!${lsHealXTClass.Contains[${_lisccx.Value}]}) /varset _out ${_out}:${off}
    /invoke ${_lisccx.Advance}
  }
  /invoke ${out[12, "/heal xtclass ${dot} ${_out}"]}

/return TRUE



|***
 * note: make sure you have the correct zone db info loaded
 * use: /call check_zone_id DEBUG [force T/F]
 ***|
sub check_zone_id(bool _debug, bool _force)
  DEBUG \atcheck_zone_id\ax(\a-w${_force}\ax)

  GETINPUT check_zone_id stCurrentZoneID|${maEnv.Find[stCurrentZoneID].Value}

  /if (${timer_Check_Zone_ID} && !${_force}) {
    /return FALSE
  }

  /invoke ${set_data_timer[${_debug}, Check_Zone_ID, RESTART]}
  /declare _zoned bool local FALSE

  | zone matches. nothing to see here. move on
  /if (${maEnv.Find[stCurrentZoneID].Value} == ${Zone.ID} && !${_force}) {
    /return TRUE
  }

  | zone does not match. wtf..
  /if (${maEnv.Find[stCurrentZoneID].Value} != ${Zone.ID} || ${_force}) {
    /if (${lsGrain.Contains[1]}) {
      OUT ${num}${Zone.Name}\ax${sep}\a-w${Zone.ShortName}\ax
    }

    /call set_zone_lists ${maDebug.Find[area].Value}
    /varset swIsMeDead FALSE
    /varset _zoned TRUE
    /doevents flush

    | check for mesh
    /if (!${Navigation.MeshLoaded}) {
      /invoke ${out[1, 0, 14]}
    }

    | make it right with the world
    /invoke ${maEnv.Add[stCurrentZoneID,${Zone.ID}]}

  }

/return



|***
 * note: keep exp at a specified level then max aa. set lvl to 0 to disable
 * use: /call check_expaa_adjust DEBUG
 ***|
sub check_expaa_adjust(bool _debug)
  DEBUG \atcheck_expaa_adjust\ax()

  /invoke ${set_data_timer[${_debug}, Check_EXP_Adjust, RESTART]}

  | set EXP cause we deaded too much
  /if (${Me.Level} <= ${maEnv.Find[stLvlMax].Value} && ${Me.PctExp} <= ${maEnv.Find[stLvlPct].Value}) {
    /invoke ${out[13, 0, 15]}
    /alternateadv off

  | set to AA cause the group healer is awesom
  } else /if (${Me.Level} == ${maEnv.Find[stLvlMax].Value} && ${Me.PctExp} >= ${maEnv.Find[stLvlPct].Value} && ${Me.AAPoints} < ${Float[${Math.Calc[${Me.Level}*2]}].Int}) {
    /alternateadv on 100

  | set back to exp cause nothing else to buy
  } else /if (${Me.Level} == ${maEnv.Find[stLvlMax].Value} && ${Me.PctExp} >= ${maEnv.Find[stLvlPct].Value} && (${Me.AAPoints} == ${Int[${Math.Calc[${Me.Level}*2]}]})) {
    /alternateadv off
  }

/return TRUE



|***
 * note: food coma time
 * use: /call last_supper
 ***|
sub check_last_supper(bool _debug)
  DEBUG \atcheck_last_supper\ax()

  /invoke ${set_data_timer[FALSE, Check_FoodDrink, RESTART]}

  | for a frame of reference hunger and thirst seem to go to ~10k
  | one bite of black bread from a cleric is 500
  | one drink of a kadim constitution is almost 6k

  | eat, drink and be merry
  /if (${FindItemCount[=${maEnv.Find[stFood].Value}]} && ${Me.Hunger} < ${maHard.Find[stFoodMaintain].Value}) {
    /invoke ${out[0, 0, 16]}
    /useitem "${maEnv.Find[stFood].Value}"
  }
  /if (${FindItemCount[=${maEnv.Find[stDrink].Value}]} && ${Me.Thirst} < ${maHard.Find[stDrinkMaintain].Value}) {
    /invoke ${out[0, 0, 17]}
    /useitem "${maEnv.Find[stDrink].Value}"
  }

/return TRUE



|***
 * note: will loosly tie to a specific toon. will move close to the toon when it moves. set to timer [Timers] -> CheckTieTime
 * use: /tie [on/off toggle] requires /tc name [NAME] and /tc variance ##
 **|
sub set_tie(bool _debug)
  DEBUG \atset_tie\ax()

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Tie, RESTART]}
  }

  /if (!${Bool[${maTie.Find[stTieToon].Value}]}) {
    /return FALSE
  }

  /declare _tmpID int local
  /varset _tmpID ${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}

  DEBUG _tmpID${sep}\a-w${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}\ax

  /if (HOME) {
    /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
  }

  ISMEDEAD
  GETINPUT set_tie NA|NA

  | reasons to not tie to a toon
  /if (${maTie.Find[swSetTie].Value} && ${_tmpID} && ${Spawn[id ${_tmpID}].Dead}) {
    /return
  }

  /if (${Me.Combat}) {
    /return
  }

  /if (${Spawn[pc ${maTie.Find[stTieToon].Value}].Distance} <= ${maTie.Find[stTieVariance].Value}) {
    /return TRUE
  }

  | disable sneak for pre full movement speed AAs SoS
  /if (${Select[${Me.Class.ShortName},ROG]}) {
    /if (${maChr.Find[swNoSneakinTie].Value} && ${Me.Sneaking}) {
      /doability sneak
    }
  }

  | MQ2Nav mode
  /if (${maTie.Find[stTieMode].Value.Equal[nav]}) {
    DEBUG ${sep}mode${sep}\a-wnav\ax
    /if (${Navigation.MeshLoaded}) {
      /if (!${Navigation.Active} && ${Spawn[id ${_tmpID}].Distance} > ${maTie.Find[stTieVariance].Value}) {
        DEBUG get closer
        /if (${Target.ID}) /squelch /target clear
        /squelch /face id ${_tmpID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
        /nav id ${_tmpID} |dist=${maTie.Find[stTieVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value}

      } else /if (${Spawn[id ${_tmpID}].Distance} <= ${maTie.Find[stTieVariance].Value}) {
        /if (${Navigation.Active}) {
          /nav stop log=${maEntropy.Find[swNavLogSpew].Value}
        }
      }

    } else /if (!${Navigation.MeshLoaded}) {
      /invoke ${out[0, 0, 18]}
      /call Bind_control_tc mode stick
    }

  | MQ2MoveUtils mode
  } else /if (${maTie.Find[stTieMode].Value.Equal[stick]}) {
    DEBUG ${sep}mode${sep}\a-wstick\ax
    | all present and or accounted for SIR!
    /if (${Spawn[id ${_tmpID}].Distance} > ${maTie.Find[stTieVariance].Value}) {
      /if (${Target.ID}) {
        /squelch /target clear
      }

      /squelch /face id ${_tmpID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
      /squelch /stick id ${_tmpID} ${maTie.Find[stTieVariance].Value} UW
    }

    :okweresticking
    ISMEDEAD
    GETINPUT set_tie_stick NA|NA

    /if (${Spawn[id ${_tmpID}].Distance} <= ${maTie.Find[stTieVariance].Value}) {
      /squelch /stick off
      /return
    }

    /if (!${Stick.Stopped} && ${_tmpID}) {
      /goto :okweresticking
    } else {
      /if (${Stick.Active}) /squelch /stick off
      /if (${Me.Moving}) {
        /keypress forward
        /keypress back
      }
    }

  }

/return TRUE



|***
 * note: cast classes AA fade
 * use: /call cast_AA_fade
 ***|
sub cast_AA_fade(bool _debug)
  DEBUG \a-tcast_AA_fade\as()

  /if (!${Bool[${maChr.Find[stPctFade].Value}]}) {
    /return FALSE
  }

  ISMEDEAD
  /if (${Me.PctHPs} > ${maChr.Find[stPctFade].Value}) {
    /return FALSE
  }
  /if (!${SpawnCount[npc radius ENV PS4]}) {
    /return FALSE
  }

  | made it this far. may as well see if the shit works
  /if (${is_ready[${_debug}, "${maData.Find[stFade].Value}"]}) {
    /call voodoo ${_debug} "${maData.Find[stFade].Value}" 0 FALSE
  }

/return TRUE



|***
 * note: checks and casts whats int lsBuffNow
 * use: /call check_buff_cycle_now DEBUG
 ***|
sub check_buff_cycle_now(bool _debug)
  DEBUG \atcheck_buff_cycle_now\ax()

  /invoke ${set_data_timer[${_debug}, Check_Buff_Now, RESTART]}

  /if (AUTO) {
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_now
  GETINPUT check_buff_cycle_now NA|NA

  /declare _spell string local
  /declare _targetID int local 0
  /declare _targetIDorg string local NULL
  /declare _licountself listiterator local
  /vardata _licountself lsCount50.First.Clone
  /while (!${_licountself.IsEnd}) {

    | alwaysup tag needed
    /if (!${lsBuff${_licountself.Value}Tag.Contains[now]}) {
      DEBUG ${break} no alwaysup tag
      /invoke ${_licountself.Advance}
      /continue
    }

    | no name set?
    /if (!${Bool[${maBuff.Find[stBuff${_licountself.Value}].Value}]}) {
      DEBUG ${break}no name set:${_licountself.Value}
      /invoke ${_licountself.Advance}
      /continue
    }

    | buff on/off switch
    /if (!${maBuff.Find[swBuff${_licountself.Value}].Value}) {
      DEBUG ${break}switched off:${_licountself.Value}
      /invoke ${_licountself.Advance}
      /continue
    }

    | is it ready
    /if (!${is_ready[${_debug}, "${maBuff.Find[stBuff${_licountself.Value}].Value}"]}) {
      /invoke ${_licountself.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_licountself.Value} ${Me.DisplayName} "${maBuff.Find[stBuff${_licountself.Value}].Value}" check_buff_cycle_now
    /if (!${Macro.Return}) {
      DEBUG ${break}condition${sep}Buff${_licountself.Value}
      /invoke ${_licountself.Advance}
      /continue
    }

    DEBUG ${sep}Buff#: ${_licountself.Value}${sep}\a-w${maBuff.Find[stBuff${_licountself.Value}].Value}\ax
    /varset _spell ${maBuff.Find[stBuff${_licountself.Value}].Value}

    /if (${Me.Book[${Spell[${_spell}].RankName}]} || ${Me.CombatAbilityReady[${Me.CombatAbility[${Spell[${_spell}].RankName}]}]}) {

      DEBUG ${sep}_spell # effects${sep}\a-w${Spell[${_spell}].NumEffects}\ax

      /if (${Spell[${_spell}].NumEffects} == 1)  {
        /varset _spell ${Spell[${Spell[${_spell}].Trigger[1]}].RankName}
        DEBUG ${sep}_spell${sep}\a-w${Spell[${Spell[${_spell}].Trigger[1]}].RankName}\ax
      }

    } else /if (${Me.ItemReady[=${_spell}]}) {
      /varset _spell ${FindItem[${_spell}].Clicky}

    } else /if (${Me.AltAbilityReady[${_spell}]}) {
      /varset _spell ${maBuff.Find[stBuff${_licountself.Value}].Value}
    }

    DEBUG ${sep}${_spell} :: ${Me.Buff[${_spell}].ID} || ${Me.Song[${_spell}].ID}

    | do we have it, now that we know what it is?
    DEBUG (${Me.Buff[${_spell}].ID} || ${Me.Song[${_spell}].ID})
    /if (${Me.Buff[${_spell}].ID} || ${Me.Song[${_spell}].ID}) {
      DEBUG ${break}have buff/song
      /invoke ${_licountself.Advance}
      /continue
    }

    /if (${Spell[${_spell}].TargetType.Equal[single]}) {
      /varset _targetID ${Me.ID}
    } else {
      /varset _targetID 0
    }

    /call voodoo ${_debug} "${maBuff.Find[stBuff${_licountself.Value}].Value}" ${_targetID} FALSE

    /if (${lsBuff${_licountself.Value}Tag.Contains[autoinventory]}) {
      /delay 2s ${Cursor.ID}
      /call check_cursor ${_debug} clear
    }

    DEBUG ${sep}next iteration
    /invoke ${_licountself.Advance}
  }

/return TRUE



|***
 * note: casts an in order value from a list at passed target ID
 * note: this should not be used anywhere....
 * use: /call cast_list DEBUG [lsNAME] [TARGETID]
 ***|
sub cast_list(bool _debug, string _lsList, int _tmpID)
  DEBUG \atcast_list\ax(\a-w${_lsList}, ${_tmpID}\ax)

  /declare _liCL listiterator local
  /vardata _liCL ${_lsList}.First.Clone
  /while (!${_liCL.IsEnd}) {
    /if (!${maCC.Find[swSetCombat].Value}) {
      /return FALSE
    }

    DEBUG ${sep}_liCL.Value${sep}\a-w${_liCL.Value}\ax
    /if (${is_ready[${_debug}, "${_liCL.Value}"]}) {
      /call voodoo ${_debug} "${_liCL.Value}" ${_tmpID} FALSE
      /return TRUE
    }
    /invoke ${_liCL.Advance}
  }

/return TRUE



|***
 * note: check characters unity
 * use: /call check_buff_unity
 ***|
sub check_buff_cycle_unity(bool _debug)
  DEBUG \atcheck_buff_cycle_unity\ax()

  SETHUD "buff unity"

  /if (AUTO) {
    /invoke ${set_data_timer[FALSE, Check_Buff_Unity, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_unity
  GETINPUT check_buff_cycle_unity NA|NA

  /if (!${is_ready[${_debug}, "${maChr.Find[stAAUnity].Value}"]}) {
    /return FALSE
  }

  /declare _count int local 0
  /declare _found bool local TRUE
  /declare _spell string local ${maChr.Find[stAAUnity].Value}
  DEBUG ${sep}_spell${sep}\a-w${_spell}\ax
  DEBUG ${sep}effects${sep}\a-w${Spell[${_spell}].NumEffects}\ax

  /for _count 1 to ${Spell[${_spell}].NumEffects}
    DEBUG ${sep}_count${sep}\aw${_count}\ax${sep}\a-w${Spell[${_spell}].Trigger[${_count}]}\ax

    | trigger
    /if (${Me.Buff[${Spell[${_spell}].Trigger[${_count}].BaseName}].ID}) /continue

    | stacks
    /if (!${Spell[${_spell}].Trigger[${_count}].Stacks}) /continue

    | do not have
    /if (!${Me.Buff[${Spell[${_spell}].Trigger[${_count}]}].ID}) /varset _found FALSE
    DEBUG ${sep}_found${sep}\a-w${_found}\ax

    /if (!${_found}) /break
  /next _count

  /if (!${_found}) {
    /call voodoo ${_debug} "${_spell}" 0 FALSE
  }

  SETHUD EMPTY

/return TRUE



|***
 * note: do all the damn buffs
 * use: /call check_buff_cycle DEBUG <FORCE>
 ***|
sub check_buff_cycle(bool _debug, bool _force, string _verbage)
  DEBUG \atcheck_buff_cycle\ax(\a-wforce:${_force}, verb:${_verbage}\ax)

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Buff_Cycle, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  /if (${_force}) {
    DEBUG ${sep}forced /cb${sep}\ao${_verbage}\ax
    /declare _buffcycletimestart int local ${MacroQuest.Running}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /invoke ${out[0, 0, 19]}
    }
  }

  /declare _tmpSpell string local FALSE

  | remember buff slot?
  /if (${maEnv.Find[swHoldBuffGem].Value}) {
    /if (${Me.Gem[${maEnv.Find[stBuffGem].Value}].ID})  {
      DEBUG ${sep}hold buff gem${sep}\a-w${Me.Gem[${maEnv.Find[stBuffGem].Value}]}\ax
      /varset _tmpSpell ${Me.Gem[${maEnv.Find[stBuffGem].Value}]}
    }
  }

  | unity
  /if ((${maBuff.Find[swBuffUnity].Value} && AUTO && !${timer_Check_Buff_Unity}) || (${_verbage.Equal[u]} && ${_force})) {
    /call check_buff_cycle_unity ${If[${_force},${_debug},${maDebug.Find[unity].Value}]}
  }

  | aura
  /if ((${maBuff.Find[swBuffAura].Value} && AUTO && !${timer_Check_Buff_Aura}) || (${_verbage.Equal[a]} && ${_force})) {
    /call check_buff_cycle_aura ${If[${_force},${maDebug.Find[aura].Value},${maDebug.Find[aura].Value}]}
  }

  | shrink
  /if ((${maBuff.Find[swBuffShrink].Value} && AUTO && !${timer_Check_Buff_Shrink}) || (${_verbage.Equal[s]} && ${_force})) {
    /call check_buff_cycle_shrink ${If[${_force},${_debug},${maDebug.Find[shrink].Value}]}
  }

  | self
  /if ((${maBuff.Find[swBuffSelf].Value} && AUTO && !${timer_Check_Buff_Self}) || (${_verbage.Equal[s]} && ${_force})) {
    /call check_buff_cycle_self ${If[${_force},${_debug},${maDebug.Find[buffself].Value}]}
  }

  | crew
  /if ((${maBuff.Find[swBuffCrew].Value} && AUTO && !${timer_Check_Buff_Crew}) || (${_verbage.Equal[c]} && ${_force})) {
    /call check_buff_cycle_crew ${If[${_force},${_debug},${maDebug.Find[crew].Value}]}
  }

  | group
  /if ((${maBuff.Find[swBuffGroup].Value} && AUTO && !${timer_Check_Buff_Group}) || (${_verbage.Equal[g]} && ${_force})) {
    /call check_buff_cycle_others ${If[${_force},${_debug},${maDebug.Find[buffgroup].Value}]} Group
  }

  | raid
  /if ((${maBuff.Find[swBuffRaid].Value} && AUTO && !${timer_Check_Buff_Raid}) || (${_verbage.Equal[r]} && ${_force})) {
    /call check_buff_cycle_others ${If[${_force},${_debug},${maDebug.Find[buffraid].Value}]} Raid
  }

  | beg
  /if ((${maBuff.Find[swBuffBeg].Value} && AUTO && !${timer_Check_Buff_Beg}) || (${_verbage.Equal[b]} && ${_force})) {
    /call check_buff_cycle_beg ${If[${_force},${_debug},${maDebug.Find[beg].Value}]}
  }

  | powersource
  /if (${maBuff.Find[swBuffPowersource].Value} && ((AUTO && !${timer_Check_Buff_Powersource}) || ${_force})) {
    | /call check_buff_cycle_powersource ${If[${_force},${_debug},${maDebug.Find[power].Value}]}
  }

  | ranged ammo
  /if ((${maBuff.Find[swBuffAmmo].Value} && AUTO && !${timer_Check_Ammo}) || (${_verbage.Equal[s]} && ${_force})) {
    /call check_buff_cycle_ammo ${If[${_force},${_debug},${maDebug.Find[ammo].Value}]}
  }

  | class specific buffs. buff self required
  /if ((${maBuff.Find[swBuffSelf].Value} && !${timer_Check_Buff_Self}) || (${_verbage.Equal[s]} && ${_force})) {
    /if (${Select[${Me.Class.ShortName},BER]}) {
    } else /if (${Select[${Me.Class.ShortName},BRD]}) {
    } else /if (${Select[${Me.Class.ShortName},BST]}) {
    } else /if (${Select[${Me.Class.ShortName},CLR]}) {
    } else /if (${Select[${Me.Class.ShortName},DRU]}) {
    } else /if (${Select[${Me.Class.ShortName},ENC]}) {
      | chanter hp/manna crystals
      /if (${maChr.Find[swAACrystals].Value}) {
        /call check_crystals ${If[${_force},${_debug},${maDebug.Find[crystal].Value}]}
      }

    } else /if (${Select[${Me.Class.ShortName},MAG]}) {
    } else /if (${Select[${Me.Class.ShortName},MNK]}) {
    } else /if (${Select[${Me.Class.ShortName},NEC]}) {
    } else /if (${Select[${Me.Class.ShortName},PAL]}) {
    } else /if (${Select[${Me.Class.ShortName},RNG]}) {
    } else /if (${Select[${Me.Class.ShortName},ROG]}) {
      | summon leg poison
      /if (${maChr.Find[stLegPoison].Value} && ((AUTO && !${timer_Check_Poison}) || ${_force})) {
        /call check_buff_cycle_leg_poison ${If[${_force},${_debug},${maDebug.Find[poison].Value}]}
      }

    } else /if (${Select[${Me.Class.ShortName},SHD]}) {
    } else /if (${Select[${Me.Class.ShortName},SHM]}) {
    } else /if (${Select[${Me.Class.ShortName},WAR]}) {
    } else /if (${Select[${Me.Class.ShortName},WIZ]}) {
    }
  }

  | reapply buff slot?
  /if (${maEnv.Find[swHoldBuffGem].Value}) {
    /if (${Bool[${_tmpSpell}]}) {
      DEBUG ${sep}reset buff gem${sep}\a-w${Me.Gem[${maEnv.Find[stBuffGem].Value}]}\ax
      /call set_spell_to ${_debug} TRUE "${_tmpSpell}" ${maEnv.Find[stBuffGem].Value} FALSE
    }
  }

  /if (${_force}) {
    /if (${maBuff.Find[swBuffMaster].Value}) {
      /invoke ${out[0, "Buff Cycle${sep}\a-wCompleted\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_buffcycletimestart}) / 10) / 60]}s\ax"]}
    }
  }

/return TRUE



|***
 * note: power sources
 * use: /call check_buff_cycle_powersource DEBUG
 ***|
sub check_buff_cycle_powersource(bool _debug)
  DEBUG \atcheck_buff_cycle_powersource\ax()

  /invoke ${set_data_timer[${_debug}, Check_Buff_Powersource, RESTART]}

  | /if (${Me.Inventory[powersource].Power}) /return FALSE

  /declare _psName string local FALSE

  /declare _count int local 0
  /for _count 1 to 50
    /if (!${lsBuff${_count}Tag.Contains[buffps]}) {
      /continue
    } else /if (!${lsBuff${_count}Tag.Contains[powersource]}) {
      DEBUG buff${_count}${sep}\a-w${maBuff.Find[stBuff${_count}].Value}\ax
      /varset _psName ${maBuff.Find[stBuff${_count}].Value}
      /break
    }
  /next _count

  /if (!${Bool[${_psName}]}) /return FALSE

  | delete current empty one
  /if (${Me.Inventory[powersource].ID} && ${Me.Inventory[powersource].Power} == 0) {
    /declare _currentPSSlot string local ${Me.Inventory[powersource].Name}
    /nomodkey /itemnotify "${_currentPSSlot}" leftmouseup
    /delay 1s ${Cursor.ID}
    /if (${Cursor.Name.Equal[${_currentPSSlot}]}) /destroy
    /delay 1s ${Cursor.Equal[Null]}
  }

  | powersources that are not sealed
  /if (!${Spell[${FindItem[${_psName}].Clicky.SpellID}].ID}) {

    | get a new one
    /if (${FindItem[${_psName}].ID} && !${Me.Inventory[powersource].ID}) {
      /delay 5
      /call Bind_command_swap "${_psName}" 21 FALSE
    } else /if (!${FindItem[${_psName}].ID}) {
      /return FALSE
    }

  | powersources that are sealed
  } else /if (${Spell[${FindItem[${_psName}].Clicky.SpellID}].ID}) {

    | unwrap a new on
    /if (${FindItem[${_psName}].ID} && !${Me.Inventory[powersource].ID}) {
      /call voodoo ${_debug} "${_psName}" 0 FALSE
      /delay 5
      /autoinventory
    } else /if (!${FindItem[${_psName}].ID}) {
      /return FALSE
    }

  } else {
    /return FALSE
  }

/return TRUE



|***
 * note: Mana recovery items; rods, shards, crack pipes
 * use: /call do_mana_recovery DEBUG
 ***|
sub do_mana_recovery(bool _debug)
  DEBUG \atdo_mana_recovery\ax()


  | turned off? or at max mana?
  /if (!${maChr.Find[stPctManaRecovery].Value} || ${Me.CurrentMana} == ${Me.MaxMana}) {
    DEBUG ${break}${off} || ${Me.CurrentMana} == ${Me.MaxMana}
    /return FALSE
  }

  /if (${Me.PctMana} >= ${maChr.Find[stPctManaRecovery].Value}) {
    DEBUG ${break}more manna${sep}\a-w${Me.PctMana} >= ${maChr.Find[stPctManaRecovery].Value}\ax
    /return FALSE
  }

  /if (SAFEZONE) {
    /return FALSE
  }

  /if (!${lsClassCast.Contains[${Me.Class.ShortName}]}) {
    DEBUG ${break}not a caster
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT
  GETINPUT do_mana_recovery NA|NA

  | get mana recovery items
  /if (!${timer_Check_Inventory_ManaClick}) {
    /call scan_inventory_for ${maDebug.Find[inventoryscan].Value} ManaClick 2 15
    /invoke ${set_data_timer[${_debug}, Check_Inventory_ManaClick, RESTART]}
  }

  /declare _tmpID int local 0
  /declare _lidmc listiterator local
  /vardata _lidmc lsInventoryManaClick.First.Clone
  /while (!${_lidmc.IsEnd}) {

    DEBUG ${sep}\a-w${_lidmc.Value}\ax

    /if (!${is_ready[${_debug}, "${_lidmc.Value}"]}) {
      /invoke ${_lidmc.Advance}
      /continue
    }

    | worth the mana or gonna kill us?
    /if (${FindItem[=${_lidmc.Value}].Spell.Base[2]} < ${Math.Calc[${Me.MaxMana} - ${Me.CurrentMana}].Int} && ${Me.CurrentHPs} > ${FindItem[=${_lidmc.Value}].Spell.Base[1]}) {
      DEBUG ${sep}You can live and get mana too!
    } else /if (${FindItem[=${_lidmc.Value}].Spell.Base[2]} > ${Math.Calc[${Me.MaxMana} - ${Me.CurrentMana}].Int}) {
      DEBUG ${sep}Giving too much mana back dont be a waste.....
      /invoke ${_lidmc.Advance}
      /continue
    } else /if (${Me.CurrentHPs} > ${FindItem[=${_lidmc.Value}].Spell.Base[1]}) {
      DEBUG ${sep}Come back when you have more HP slacker
      /invoke ${_lidmc.Advance}
      /continue
    }

    | if its a targetable click, change target ID to self
    /if (${Select[${Spell[${_lidmc.Value}].RankName.TargetType},Self,AE PC v2]}) {
      /varset _tmpID 0
    } else /if (${Select[${Spell[${_lidmc.Value}].RankName.TargetType},Single]}) {
      /varset _tmpID ${Me.ID}
    }

    /call voodoo ${_debug} "${_lidmc.Value}" ${_tmpID} FALSE

    /invoke ${_lidmc.Advance}
  }

/return TRUE



|***
 * note: Cleric/Paladin AA Yaulp
 * use: /call cast_yaulp
 ***|
sub cast_yaulp(bool _debug)
  DEBUG \atcast_yaulp\ax()

  /invoke ${set_data_timer[${_debug}, Check_Yaulp, RESTART]}

  /if (!${maBuff.Find[swBuffMaster].Value}) {
    DEBUG ${break}
    /return FALSE
  }

  /if (${Me.PctMana} > 99*.PCTHO) {
    DEBUG ${break}mana above${sep}\a-w${Math.Calc[99*.PCTHO]}\ax
    /return FALSE
  }

  /if (${Me.Sitting} || ${Me.Mount.ID}) {
    DEBUG ${break}sitting or mounted
    /return FALSE
  }

  /if (${Me.Buff[Lesser Yaulp].ID} || ${Me.Buff[Yaulp].ID}) {
    DEBUG ${break}have yaulp buff
    /return FALSE
  }

  /if (!${is_ready[${_debug}, "${maChr.Find[stYaulp].Value}"]}) {
    /return FALSE
  }

  /if (!${Select[${Me.CombatState},combat]}) {
    DEBUG ${break}not in combat
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT cast_yaulp
  /call voodoo ${_debug} "${maChr.Find[stYaulp].Value}" 0 FALSE

/return TRUE



|***
 * note: sets your target.. nothing more.. nothing less..
 * use: ${set_target[ID]} | /call set_target [ID]
 ***|
sub set_target(int _tmpID)

  /if (!${Bool[${_tmpID}]}) {
    /return TRUE
  }

  /if (${Spawn[npc corpse ${_tmpID}].ID}) {
    /return FALSE
  }

  /if (${Target.ID} == ${_tmpID}) {
    /return TRUE
  }

  /squelch /target id ${_tmpID}
  | /delay 1s ${Target.ID} == ${_tmpID}

  /if (${Target.ID} != ${_tmpID}) {
    /return FALSE
  }

/return TRUE



|***
 * note: used to refresh ammo for ranged attacks
 * use: /call check_buff_cycle_ammo DEBUG
 ***|
sub check_buff_cycle_ammo(bool _debug)
  DEBUG \atcheck_buff_cycle_ammo\ax()

  SETHUD "buff ammo"

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Ammo, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_ammo
  GETINPUT check_buff_cycle_ammo NA|NA

  /declare _count int local 0
  /for _count 1 to 50
    /if (!${maBuff.Find[swBuff${_count}].Value}) /continue
    /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) /continue
    /if (!${lsBuff${_count}Tag.Contains[summonrange]} && !${lsBuff${_count}Tag.Contains[summonaxe]}) /continue

    DEBUG ${sep}_count${sep}\a-wBuff${_count}${sep}${maBuff.Find[stBuff${_count}].Value}

    | summon zerker axes
    /if (${lsBuff${_count}Tag.Contains[summonaxe]}) {
      DEBUG ${sep}Tag${sep}summonaxe
      /if (!${FindItemCount[=${Spell[${maBuff.Find[stBuff${_count}].Value}]}]}) {
        /if (${is_ready[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
          /call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" 0 FALSE
        }
      }

    | all other ranged item summoning
    } else /if (${lsBuff${_count}Tag.Contains[summonrange]}) {
      DEBUG ${sep}Tag${sep}summonitem -- ${FindItemCount[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}

      | any spares in bags?
      /if (${Me.Inventory[22].ID} != ${Spell[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]} && ${FindItemCount[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}) {
        /call Bind_command_swap "${FindItem[${Spell[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}" 22 ${_debug}
        /return TRUE
      }


      /if (!${FindItemCount[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}) {
        /if (${is_ready[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
          /call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" 0 FALSE
        }
        /delay 5
        | /autoinventory
        /if (${Me.Inventory[22].Name.NotEqual[${Spell[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}) {
          /call Bind_command_swap "${FindItem[${Spell[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}" 22 ${_debug}
        }
      } else /if (${FindItemCount[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}) {
        /if (${Me.Inventory[22].Name.NotEqual[${Spell[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}) {
          /call Bind_command_swap "${FindItem[=${Spell[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}" 22 ${_debug}
        }
      }
    }

    | autoinventory?
    /if (${lsBuff${_count}Tag.Contains[autoinventory]}) {
      /delay 2s ${Cursor.ID}
      /call check_cursor ${_debug} clear
    }

  /next _count

/return TRUE



|***
 * note: poison clickie things
 * use: /call check_buff_cycle_poison DEBUG
 ***|
sub check_buff_cycle_leg_poison(bool _debug)
  DEBUG \atcheck_buff_cycle_leg_poison\ax()

  SETHUD "buff leg poison"

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Poison, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  ISMEDEAD
  GETINPUT check_buff_cycle_leg_poison NA|NA

  /declare _count int local 0
  /for _count 1 to 50
    /if (!${maBuff.Find[swBuff${_count}].Value}) {
      /continue
    }
    /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
      /continue
    }
    /if (!${lsBuff${_count}Tag.Contains[summonpoison]}) {
      /continue
    }

    | do we have enough summoned
    /if (${FindItemCount[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Base[1]}]} >= ${maChr.Find[stLegPoison].Value}) {
      /return FALSE
    }

    /if (!${SpawnCount[radius ENV PS4 targetable]}) {
      /makemevisible
    }

    /if (${is_ready[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
      /call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" 0 FALSE
      /delay 5 ${Me.Casting.ID}
      /delay 3s !${Me.Casting.ID}
    }

    /if (${lsBuff${_count}Tag.Contains[autoinventory]}) {
      /delay 2s ${Cursor.ID}
      /autoinventory
    }

    /if (${Cursor.Name.Equal[${FindItem[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}]}) {
      /autoinventory
    }

  /next _count

  SETHUD EMPTY

/return TRUE



|***
 * note: watches buff beg map for aliases it can cast
 * use: /call check_buff_cycle_beg DEBUG
 ***|
sub check_buff_cycle_beg(bool _debug)
  DEBUG \atcheck_buff_cycle_beg\ax()

  /if (AUTO) {
    /invoke ${set_data_timer[FALSE, Check_Buff_Beg, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  /if (!${maBuffRequest}) {
    DEBUG ${break}no buff requests
    /return TRUE
  }

  SETHUD "buff beg"

  /declare _remove bool local FALSE

  /declare _mibb mapiterator local
  /vardata _mibb maBuffRequest.First.Clone
  /declare _licountbeg listiterator local
  /while (!${_mibb.IsEnd}) {

    CHECKTIE
    ISMEDEAD
    CHECKEXIT check_buff_cycle_beg
    GETINPUT check_buff_cycle_beg NA|NA

    /if (!${is_lsSafeNames[${maDebug.Find[safe].Value}, ${_mibb.Key}]}) {
      /invoke ${maBuffRequest.Remove[${_mibb.Key}]}
      /return FALSE
    }

    /vardata _licountbeg lsCount50.First.Clone
    /while (!${_licountbeg.IsEnd}) {

      /if (!${maBuff.Find[swBuff${_licountbeg.Value}].Value}) {
        /invoke ${_licountbeg.Advance}
        /continue
      }
      /if (!${Bool[${maBuff.Find[stBuff${_licountbeg.Value}].Value}]}) {
        /invoke ${_licountbeg.Advance}
        /continue
      }
      /if (!${Bool[${maBuff.Find[stBuff${_licountbeg.Value}Alias].Value}]}) {
        /invoke ${_licountbeg.Advance}
        /continue
      }

      /if (${maBuff.Find[stBuff${_licountbeg.Value}Alias].Value.Find[${_mibb.Value}]}) {
        DEBUG ${sep}BEG \ap${_mibb.Value}\ax found${sep}\aw${_licountbeg.Value}\ax${sep}\a-w${maBuff.Find[stBuff${_licountbeg.Value}].Value}\ax for \a-y${_mibb.Key}\ax
        /call voodoo ${_debug} "${maBuff.Find[stBuff${_licountbeg.Value}].Value}" ${Spawn[pc ${_mibb.Key}].ID} FALSE
        /delay 1s ${Me.Casting.ID}
        /delay 5s !${Me.Casting.ID}
        /varset _remove TRUE
        /break
      }

      /invoke ${_licountbeg.Advance}
    }

    /if (${_remove}) {
      /invoke ${maBuffRequest.Remove[${_mibb.Key}]}
      /break
    }

    /invoke ${_mibb.Advance}
  }

  SETHUD EMPTY

/return



|***
 * note: buffs all your toons
 * use: part of standard buff cycle if enabled
 ***|
sub check_buff_cycle_crew(bool _debug)
  DEBUG \atcheck_buff_cycle_crew\ax()

  /if (AUTO) {
    /invoke ${set_data_timer[FALSE, Check_Buff_Crew, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  SETHUD "buff crew"

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_crew
  GETINPUT check_buff_cycle_crew NA|NA

  /declare _spell string local FALSE
  /declare _spellTrigger string local FALSE
  /declare _count int local 0
  /declare _countToon int local 1
  /declare _target bool local FALSE
  /declare _toonName string local FALSE
  /declare _found bool TRUE
  /declare _i int local 0
  /declare _base2 string local\
  /declare _tmpBuff string local FALSE

  DEBUG ${sep}Buff${sep}\a-wCrew\ax

  | each toon
  /while (${_countToon} <= ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}) {

    | get the name to something usable
    /varset _toonName ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}
    DEBUG ${sep}_toonName${sep}\ay${_toonName}\ax

    | skip this person?
    /if (${_toonName.Equal[${Me.DisplayName}]} || !${Spawn[pc ${_toonName}].ID} || ${Spawn[pc ${_toonName}].Dead}) {
      DEBUG ${sep}skipping${sep}\a-w${_toonName}\ax
      /varcalc _countToon ${_countToon}+1
      /continue
    }

    | any buff slots available?
    /dquery ${_toonName} -q "Me.FreeBuffSlots" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    /if (${DNQReturn.Equal[0]}) {
      /varcalc _countToon ${_countToon}+1
      /continue
    }

    | each buff
    /for _count 1 to 50
      
      /varset _tmpBuff 

      | buff has no name
      /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
        /continue
      | buff is turned off
      } else /if (!${maBuff.Find[swBuff${_count}].Value}) {
        /continue
      }
      
      | not a crew tag buff
      /if (!${lsBuff${_count}Tag.Contains[crew]}) {
        /continue
      }
      
      | start redirect for names
      | /varset _tmpBuff ${maBuff.Find[stBuff${_count}].Value}
      
      | buffs with SDPA parsing to be done
      /if (${maBuff.Find[stBuff${_count}].Value.Count[|]} == 3) {
        /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_count}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[3,|]}"
        /invoke ${maData.Add[tmpName,${Macro.Return}]}

      | get item clickie spells
      } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
        DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
        /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
  
      | just the spell/aa/whatever
      } else {
        DEBUG ${sep}spell${sep}${maBuff.Find[stBuff${_count}].Value}
        /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
      }

      | skip self only
      /if (${Select[${Spell[${maData.Find[tmpName].Value}].RankName.TargetType},self]}) {
        DEBUG ${sep}skip self only tag for crew cycle
        /continue
      }

      | skip tags
      /if (${lsBuff${_count}Tag.Contains[aura]}) {
        DEBUG ${sep}skip saura tag for crew cycle
        /continue
      }

      | skip summon ranged items/zerker axes
      /if (${lsBuff${_count}Tag.Contains[summonrange]} || ${lsBuff${_count}Tag.Contains[summonaxe]}) {
        DEBUG ${sep}skip range type tags for crew cycle
        /continue
      }

      | cycle the tags
      /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_toonName} "${maData.Find[tmpName].Value}" check_buff_cycle_crew
      /if (!${Macro.Return}) {
        DEBUG ${sep}skip${sep}failed tag cycle check
        /continue
      }

      DEBUG ${sep}\agBUFF\ax#${sep}\a-w${_count}\ax \a-p---------------------\ax

      | one of your crew, pretend they are not a target until they qualify
      /varset _target FALSE

      | is this an acceptable target for casting
      /if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_toonName}]}) {
        DEBUG ${sep}crew member${sep}\a-w${_toonName}\ax (\a-rvalid target\ax)
        /varset _target TRUE
      }

      /varset _spell ${set_rank[${_debug}, "${maData.Find[tmpName].Value}"]}
      DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax

      | distance check
      /if (${Select[${Spell[${_spell}].TargetType},Group v2]}) {
        DEBUG ${sep}distance check${sep}\a-wGroup v2\ax
        /if (${Spawn[pc ${_toonName}].Distance} > ${Spell[${_spell}].AERange}) /continue
      } else {
        DEBUG ${sep}distance check${sep}\a-wtargeted ?? ${Spell[${_spell}].MyRange}\ax
        /if (${Spell[${_spell}].MyRange}) {
          /if (${Spawn[pc ${_toonName}].Distance} > ${Spell[${_spell}].MyRange} && !${Select[${Spell[${_spell}].TargetType},self]}) /continue
        }
      }

      DEBUG ${sep}\aw470\ax${sep}\a-w${Spell[${_spell}].HasSPA[470]}\ax
      DEBUG ${sep}\aw374\ax${sep}\a-w${Spell[${_spell}].HasSPA[374]}\ax
      DEBUG ${sep}\aw340\ax${sep}\a-w${Spell[${_spell}].HasSPA[340]}\ax
      /if (${Spell[${_spell}].HasSPA[470]} || ${Spell[${_spell}].HasSPA[374]} || ${Spell[${_spell}].HasSPA[340]}) {
        DEBUG ${sep}multiple landing spell

        /for _i 1 to ${Spell[${_spell}].NumEffects}
          /varset _found TRUE

          | skip any secondary procs with no duration
          /if (!${Bool[${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Duration}]}) /continue

          /varset _spellTrigger ${Spell[${maData.Find[tmpName].Value}].Trigger[${_i}]}
          DEBUG ${sep}${sep}\awTrigger\ax${sep}\a-w${_spellTrigger}\ax ID:\aw${Spell[${_spellTrigger}].ID}\ax

          | is it buff blocked
          /dquery ${_toonName} -q "lsBlockedBuffsMe.Find[${Spell[${_spellTrigger}].ID}]" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
          DEBUG ${sep}${sep}BLOCKED .Trigger.Blocked Buff \at${Bool[${DNQReturn}]}\ax ${dot} ${_spellTrigger}
          /if (${Bool[${DNQReturn}]}) {
            /continue
          }

          | does it stack
          /dquery ${_toonName} -q "Spell[${_spellTrigger}].Stacks" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
          DEBUG ${sep}${sep}STACKS: .Trigger.Stacks \at${DNQReturn}\ax ${dot} ${_spellTrigger}
          /if (!${Bool[${DNQReturn}]}) {
            /continue
          }

          | do we have the triggered buff?
          /dquery ${_toonName} -q "Me.Buff[${_spellTrigger}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
          DEBUG ${sep}trigger${sep}\ay${_spellTrigger}\ax
          DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}
          /if (!${Bool[${DNQReturn}]}) {
            /varset _found FALSE
            /break
          }

        /next _i

        DEBUG ${sep}${sep}\ay****\ax Found:${_found} .. Target:${_target}
        /if (!${_found} && ${_target}) {
          /call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Spawn[pc ${_toonName}].ID} FALSE
          /call cast_cooldown_wait ${_debug}
          /continue
        }

      } else {
        DEBUG ${sep}single landing spell

        /varset _spellTrigger ${Spell[${maData.Find[tmpName].Value}].RankName}
        DEBUG ${sep}\awTrigger\ax${sep}\a-w${_spellTrigger}\ax ID:${Spell[${_spellTrigger}].ID}

        | does it stack
        /dquery ${_toonName} -q "Spell[${_spellTrigger}].Stacks" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}STACKS: .Buff.Stacks \at${DNQReturn}\ax ${dot} ${_spellTrigger}
        /if (${DNQReturn.Equal[FALSE]}) {
          /continue
        }

        | is it buff blocked
        /dquery ${_toonName} -q "lsBlockedBuffsMe.Find[${Spell[${_spellTrigger}].ID}]" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}BLOCKED .Buff.Blocked \at${DNQReturn}\ax ${dot} ${_spellTrigger}
        /if (${Bool[${DNQReturn}]}) /continue

        | do we have the triggered buff?
        /dquery ${_toonName} -q "Me.Buff[${_spellTrigger}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}HAVE: .Buff.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}
        /if (${Bool[${DNQReturn}]}) {
          /continue
        }

        DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}

        /if (${_target}) {
          /call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Spawn[pc ${_toonName}].ID} FALSE
          /call cast_cooldown_wait ${_debug}
          /continue
        }

      }

    /next _count
    /varcalc _countToon ${_countToon}+1

  }

  SETHUD EMPTY

/return TRUE



|***
 * note: buffs others in group or raids that are not part of your crew
 * use: part of standard buff cycle if enabled
 ***|
sub check_buff_cycle_others(bool _debug, string _type)
  DEBUG \atcheck_buff_cycle_others\ax(\a-w${_type}\ax)

  | if we are in a raid, we are done here
  /if (!${Raid.Members} && !${Group.Members}) {
    /return FALSE
  }

  /if (${_type.Equal[group]}) {
    /if (AUTO) {
      /invoke ${set_data_timer[FALSE, Check_Buff_Group, RESTART]}
      /if (!${maBuff.Find[swBuffMaster].Value}) {
        /return FALSE
      }
    }
    /varset _type Group

  } else /if (${_type.Equal[raid]}) {

    /if (AUTO) {
      /invoke ${set_data_timer[FALSE, Check_Buff_Raid, RESTART]}
      /if (!${maBuff.Find[swBuffMaster].Value}) {
        /return FALSE
      }
    }
    /varset _type Raid

  } else {
    /invoke ${out[0, "${warning} group/raid type error in buff others"]}
    /return FALSE
  }

  SETHUD "buff others (${_type})"

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_others
  GETINPUT check_buff_cycle_others NA|NA

  | i'd like to go on the recored and let everyone know i fucking hate mercs. more then bards
  | they are a bitch to code around and they kill the group content of this game
  | ok, i feel better now. moving on..

  DEBUG ${sep}Buff${sep}\a-w${_type}\ax

  /declare _countToon int 0
  /declare _count int 0
  /declare _toonName string local FALSE
  /declare _spell string local FALSE
  /declare _found bool local TRUE
  /declare _i int local 0
  /declare _spellTrigger string local FALSE

  | non boxed people/mercs (fuck your pets)
  /for _countToon 1 to ${${_type}.Members}

    | "A girl has no name."
    /varset _toonName ${${_type}.Member[${_countToon}].DisplayName}
    DEBUG ${sep}_toonName${sep}\ay${_toonName}\ax

    | one of your crew
    /if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_toonName}]}) /continue


    | skip this person?
    /if (${Spawn[pc ${_toonName}].Dead}) {
      DEBUG ${sep}skipping${sep}\a-w${_toonName}\ax
      /continue
    }

    | maybe 50 buffs was a bit much.
    /for _count 1 to 50

      /if (!${maBuff.Find[swBuff${_count}].Value}) /continue
      /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) /continue

      | skip self only
      /if (${lsBuff${_count}Tag.Count} == 1 && (${lsBuff${_count}Tag.Contains[self]} || ${lsBuff${_count}Tag.Contains[minion]})) /continue

      | make sure base tags are there?
      /if (${_type.Equal[group]}) {
        /if (!${lsBuff${_count}Tag.Contains[group]}) /continue
      } else /if (${_type.Equal[raid]}) {
        /if (!${lsBuff${_count}Tag.Contains[raid]}) /continue
      }

      DEBUG ${sep}_count${sep}\a-w${_count}\ax

      | cycle the tags
      /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_toonName} "${maBuff.Find[stBuff${_count}].Value}" check_buff_cycle_others
      /if (!${Macro.Return}) {
        /continue
      }
      
      | buffs with SDPA parsing to be done
      /if (${maBuff.Find[stBuff${_count}].Value.Count[|]} == 3) {
        /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_count}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[3,|]}"
        /invoke ${maData.Add[tmpName,${Macro.Return}]}

      | get item clickie spells
      } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
        DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
        /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
      }
      
      /varset _spell ${set_rank[${_debug}, "${maData.Find[tmpName].Value}"]}
      DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax

      | distance check
      /if (${Select[${Spell[${_spell}].TargetType},Group v2]}) {
        DEBUG ${sep}distance check${sep}\a-wGroup v2\ax
        /if (${Spawn[pc ${_toonName}].Distance} > ${Spell[${_spell}].AERange}) /continue
      } else {
        DEBUG ${sep}distance check${sep}\a-wtargeted ?? ${Spell[${_spell}].MyRange}\ax
        /if (${Spawn[pc ${_toonName}].Distance} > ${Spell[${_spell}].MyRange} && !${Select[${Spell[${_spell}].TargetType},self]}) /continue
      }

      | target the toon
      /invoke ${target[set, ${${_type}.Member[${_toonName}].ID}]}

      | give the game a second to populate the targets buffs
      /delay 1s ${Target.BuffsPopulated}

      | start sorting the buff triggers
      /if (${Spell[${_spell}].NumEffects}) {
        /varset _found TRUE

        /for _i 1 to ${Spell[${_spell}].NumEffects}
          /varset _spellTrigger ${Spell[${_spell}].Trigger[${_i}]}
          DEBUG _spellTrigger ${Spell[${_spell}].Trigger[${_i}]}

          | stacks
          DEBUG stackstarget.../if (!${Spell[${_spellTrigger}].StacksTarget}) /continue
          /if (!${Spell[${_spellTrigger}].StacksTarget}) /continue

          | target has the buff?
          DEBUG has..../if (!${Target.Buff[${_spellTrigger}].ID}) /varset _found FALSE
          /if (!${Target.Buff[${_spellTrigger}].ID}) /varset _found FALSE

          /if (!${_found}) /break

        /next _i

        /if (!${_found} && ${Target.ID} == ${${_type}.Member[${_toonName}].ID}) {
          /call voodoo ${_debug} "${maData.Find[tmpName].Value}" ${${_type}.Member[${_toonName}].ID} FALSE
          /continue
        }

      | buffs with only 1 trigger
      } else {

        /varset _found TRUE

        /varset _spellTrigger ${Spell[${_spell}].Trigger}
        DEBUG ${sep}Trigger${sep}\a-w${_spellTrigger}\ax

        | stacks
        /if (!${Spell[${_spellTrigger}].StacksTarget}) /continue

        | target has the buff?
        /if (!${Target.Buff[${_spellTrigger}].ID}) /varset _found FALSE

        /if (!${_found} && ${Target.ID} == ${${_type}.Member[${_toonName}].ID}) {
          /call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${${_type}.Member[${_toonName}].ID} FALSE
          /continue
        }
      }

    /next _count

    /if (${Target.ID}) {
      /squelch /target clear
    }
  /next _countToon

  SETHUD EMPTY

/return TRUE



|***
 * note: we dont trust anyone
 * use: ${is_lsSafeNames[DEBUG, NAME]}
 ***|
sub is_lsSafeNames(bool _debug, string _name)
  DEBUG \atis_lsSafeNames\ax(\a-w${_name}\ax)

  | override and accept everyone
  /if (${maOver.Find[swOverSafeNames].Value}) {
    DEBUG ${sep}safe${sep}\a-wswOverSafeNames\ax
    /return TRUE
  }

  | auto accept ${DanNetMacroChannel} channel toons
  /if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_name.Lower}]}) {
    DEBUG ${sep}safe${sep}\a-wstEntropyGroup_all\ax
    /return TRUE
  }

  | basic safe list check
  /if (${lsSafeNames.Contains[${_name.Lower}]}) {
    DEBUG ${sep}safe${sep}\a-wlsSafeNames\ax
    /return TRUE
  }

  | guild safe list check
  /if (${maEnv.Find[swSafelistGuild].Value} && ${Bool[${Me.Guild}]}) {
    /if (${Spawn[PC ${_name}].Guild.Equal[${Me.Guild}]}) {
      DEBUG ${sep}safe${sep}\a-wswSafelistGuild\ax
      /return TRUE
    }
  }

/return FALSE



|***
 * note: incombat self buffs
 * use: /call check_buff_cycle_temp_self DEBUG [spell]
 ***|
sub check_buff_cycle_temp_self(bool _debug, string _spell)
  DEBUG \atcheck_buff_cycle_temp_self\ax(\a-w${_spell}\ax)

  /if (${target[isdead, TARGET]}) {
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT check_buff_cycle_temp_self
  GETINPUT check_buff_cycle_temp_self _spell|${_spell}
  CHECKTIE

  /invoke ${set_data_timer[FALSE, Check_Buff_Temp, RESTART]}

  /if (${is_ready[${_debug}, "${maChr.Find[${_spell}].Value}"]}) {
    /call voodoo ${_debug} "${maChr.Find[${_spell}].Value}" 0 FALSE
  }

/return TRUE



|***
 * node: checks and casts short term temp buffs that have a toon list
 * node: designed for fast cast in combat buffs
 * use: /call check_buff_cycle_temp_list DEBUG [spell var] [toon list var]
 ***|
sub check_buff_cycle_temp_list(bool _debug, string _spell, string _toonList)
  DEBUG \atcheck_buff_cycle_temp\ax(\a-w${_spell}, ${_toonList}\ax)

  /if (AUTO) {
    /invoke ${set_data_timer[FALSE, Check_Buff_Temp, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  /if (!${maBuff.Find[swBuffTemp].Value}) {
    /return FALSE
  }

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_temp_list
  GETINPUT check_buff_cycle_temp_list NA|NA

  | ready?
  /if (!${is_ready[${_debug}, "${maChr.Find[${_spell}].Value}"]}) {
    /return FALSE
  }

  | me?
  /if (${maBuff.Find[swBuffSelf].Value}) {
    /if (${${_toonList}.Contains[${Me.DisplayName}]} && !${Me.Buff[${_spell}].ID}) {
      /call voodoo ${_debug} "${maChr.Find[${_spell}].Value}" ${Me.ID} FALSE
      /return TRUE
    }
  }

  SETHUD "buff temp"

  /declare _castSpell bool local FALSE

  /declare _libt listiterator local
  /vardata _libt ${_toonList}.First.Clone
    /while (!${_libt.IsEnd}) {
      DEBUG ${sep}_libt.Value${sep}\a-w${_libt.Value}\ax
      /varset _castSpell FALSE

      /if (${${_toonList}.Contains[EMPTY]} && ${${_toonList}.Count} == 1 || !${_libt.Value.Length}) {
        /invoke ${_libt.Advance}
        /continue
      }

      /if (!${Spawn[pc ${_libt.Value}].ID} || ${Spawn[pc ${_libt.Value}].Dead}) {
        /invoke ${_libt.Advance}
        /continue
      }

      /dquery ${_libt.Value} -q "Spell[${maChr.Find[${_spell}].Value}].Stacks" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${DNQReturn}
      /if (${Bool[${DNQReturn}]}) /varset _castSpell TRUE

      /dquery ${_libt.Value} -q "Me.Buff[${maChr.Find[${_spell}].Value}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${DNQReturn}
      /if (${Bool[${DNQReturn}]}) /varset _castSpell FALSE

      /if (${_castSpell}) {
        /call voodoo ${_debug} "${maChr.Find[${_spell}].Value}" ${Spawn[=${_libt.Value} pc].ID} FALSE
      }
      /invoke ${_libt.Advance}
    }

  SETHUD EMPTY

/return TRUE



|***
 * note: get an item from a spell
 * use:  /call cast_item_summon DEBUG [ITEMVARIABLE]
 ***|
sub cast_item_summon(bool _debug, string _var)
  DEBUG \atcast_item_summon\ax()

  CHECKTIE
  ISMEDEAD
  CHECKEXIT cast_item_summon
  GETINPUT cast_item_summon NA|NA

  /if (${Bool[${_var}]}) {
    /declare _spellc string local ${${get_key_map[${_debug}, ${_var}]}.Find[${_var}].Value}
    /declare _spell string local ${_spellc.Arg[1,+]}
    /declare _condition string local ${_spellc.Arg[2,+]}
    /varset _spell ${set_rank[${_debug}, "${_spell}"]}
  } else {
    /invoke ${out[0, "cast_item_summon${sep}\arERROR\ax"]}
  }

  /if (!${FindItem[${Spell[${_spell}].Base[1]}].ID}) {
    /call voodoo ${_debug} "${_spellc}" ${Me.ID} FALSE
    /delay 5
    /delay 15s !${Me.Casting.ID}

    | need autoinventory stuff here!!

  }

/return TRUE



|***
 * note: challenge line for knights
 * use: /call cast_challenge DEBUG [TARGETID]
 ***|
sub cast_challenge(bool _debug, int _tmpID)
  DEBUG \atcast_challenge\ax(\a-w${_tmpID}\ax)
  ISMEDEAD

  /if (${Target.BuffDuration[${maChr.Find[stChallenge].Value}].TotalSeconds}) {
    DEBUG ${sep}Target.BuffDuration${sep}\a-w${Target.BuffDuration[${maChr.Find[stChallenge].Value}].TotalSeconds}\ax
    /return TRUE
  }

  CHECKEXIT cast_challenge
  GETINPUT cast_challenge NA|NA

  /if (!${is_Ready[${_debug}, "${maChr.Find[stChallenge].Value}"]}) {
    /return FALSE
  }

  /call voodoo ${_debug} "${maChr.Find[stChallenge].Value}" ${_tmpID} FALSE

/return TRUE



|***
 * note: cast nukes lists
 * use: /call cast_nuke DEBUG [line] [targetid]

sub cast_nuke(bool _debug, string _line, int _tmpID)
  DEBUG \atcast_nuke\ax(\a-w${_line}, ${_tmpID}\ax)

  /declare _linuke listiterator local
  /vardata _linuke ${_line}.First.Clone

  /if (${Spawn[${_tmpID}].Dead}) {
    /return FALSE
  }

  /while (!${_linuke.IsEnd}) {
    DEBUG ${sep}_linuke.Value${sep}\a-w${_linuke.Value}\ax
    /delay 1

    ISMEDEAD
    ISMEDEAD
    CHECKEXIT cast_nuke
    GETINPUT cast_nuke NA|NA

    /if (${Me.PctMana} < 2) {
      /return FALSE
    }

    /if (!${is_ready[${_debug}, "${_linuke.Value}"]}) {
      /invoke ${_linuke.Advance}
      /continue
    }

    /if (${target[isdead, ${_tmpID}]}) {
      /return FALSE
    }

    /invoke ${target[set, ${_tmpID}]}

    /call voodoo ${_debug} "${_linuke.Value}" ${_tmpID} FALSE

    | forced delay
    /if (${maCC.Find[stNukeDelay].Value}) {
      /invoke ${set_data_timer[${_debug}, Nuke_Delay, RESTART]}
    }

    | maintain the loop of nukes till all are cycled.
    /if (${maCC.Find[stNukeDelay].Value}) {
      /delay 10s !${Me.Casting.ID}
      /delay 2.5s
      /delay ${timer_Nuke_Delay}
    }

    /invoke ${_linuke.Advance}
  }

/return TRUE
 ***|


|***
 * nore: use base melee discs
 * use: /call do_melee DEBUG [TARGETID]
 ***|
sub do_melee(bool _debug, int _tmpID)
  DEBUG \atdo_melee\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  CHECKEXIT do_melee
  GETINPUT do_melee NA|NA

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKDEFENSE

  /invoke ${target[set, ${_tmpID}]}

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /if (AUTO || ${swModeToken}) {
    /if (!${timer_check_stick}) {
      /call set_combat_control ${maDebug.Find[decision].Value} ${_tmpID}
    }
  }

  | melee bandolier swap for proc weapons
  /if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
    /call check_sash_proc ${_debug}
  }

  | disarm
  /if (${maCC.Find[swDisarm].Value} && !${Skill[Disarm].Auto} && AGRO) {
    /if (${Me.AbilityReady[Disarm]} && ${Spawn[${_tmpID}].Distance} <= 14.5) {
      /invoke ${out[8, "Melee${sep}${sp}Disarm\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
      /doability Disarm
    }
  }

  | kick
  /if (${maCC.Find[swKick].Value} && !${Skill[Kick].Auto} && !${Select[${Me.Class.ShortName},MNK]}) {
    /if (${Me.AbilityReady[Kick]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /invoke ${out[8, "Melee${sep}${sp}Kick\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
      /doability Kick
    }
  }

  | bash
  /if (${maCC.Find[swBash].Value} && !${Skill[Bash].Auto}) {
    /if (${Me.AbilityReady[Bash]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /invoke ${out[8, "Melee${sep}${sp}Bash\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
      /doability Bash
    }
  }

  | Intimidation
  /if (${check_class[FALSE, |BRD|ROG|BER|MNK]}) {
    /if (${maCC.Find[swIntimidation].Value}) {
      /if (${Me.AbilityReady[Intimidation]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
        /invoke ${out[8, "Melee${sep}${sp}Intimidation\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}"]}
        /doability Intimidation
      }
    }
  }

  | Zerker only skill
  /if (${check_class[FALSE, |BER]}) {
    /if (${maCC.Find[swFrenzy].Value} && !${Skill[Frenzy].Auto}) {
      /if (${Me.AbilityReady[Frenzy]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /invoke ${out[8, "Melee${sep}${sp}Frenzy\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}"]}
        /doability Frenzy
      }
    }
  }

  | rogue only skill
  /if (${check_class[FALSE, |ROG]}) {
    /if (${maCC.Find[swBackstab].Value} && !${Skill[Backstab].Auto}) {
      /if (${Me.AbilityReady[Backstab]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /invoke ${out[8, "Melee${sep}${sp}Backstab\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}"]}
        /doability Backstab
      }
    }
  }

  | monk/bst only skill
  /if (${check_class[FALSE, |MNK|BST]}) {
      /if (${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      | fist
      /if (${maCC.Find[swDragonpunch].Value} && !${Skill[Dragon Punch].Auto} && ${Me.AbilityReady[Dragon Punch]}) {
        /invoke ${out[8, "Melee${sep}${sp}Dragon Punch\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}"]}
        /doability "Dragon Punch"
      } else /if (${maCC.Find[swEaglestrike].Value} && !${Skill[Eagle Strike].Auto} && ${Me.AbilityReady[Eagle Strike]}) {
        /invoke ${out[8, "Melee${sep}${sp}Eagle Strike\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
        /doability "Eagle Strike"
      } else /if (${maCC.Find[swTigerclaw].Value} && !${Skill[Tiger Claw].Auto} && ${Me.AbilityReady[Tiger Claw]}) {
      /invoke ${out[8, "Melee${sep}${sp}Tiger Claw\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
        /doability "Tiger Claw"
      }
      | kick
      /if (${maCC.Find[swKick].Value} && !${Skill[Kick].Auto} && ${Me.AbilityReady[Kick]}) {
        /invoke ${out[8, "Melee${sep}${sp}Kick\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
        /doability Kick
      } else /if (${maCC.Find[swFlyingkick].Value} && !${Skill[Flying Kick].Auto} && ${Me.AbilityReady[Flying Kick]}) {
        /invoke ${out[8, "Melee${sep}${sp}Flying Kick\ax ${con_color[${_debug}, ${_tmpID}]}\ax"]}
        /doability "Flying Kick"
      } else /if (${maCC.Find[swRoundkick].Value} && !${Skill[Round Kick].Auto} && ${Me.AbilityReady[Round Kick]}) {
        /invoke ${out[8, "Melee${sep}${sp}Round Kick\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
        /doability "Round Kick"
      }
    }
  }

/return TRUE



|***
 * note: uses melee rest disc
 * use: /call cast_stamina_recovery DEBUG
 ***|
sub cast_stamina_recovery(bool _debug)
  DEBUG \atcast_stamina_recovery\ax()

  /if (${Me.PctEndurance} > 34) {
    /return FALSE
  }

  /if (SAFEZONE) {
    /return FALSE
  }
  
  ISMEDEAD
  GETINPUT cast_stamina_recovery NA|NA

  /declare _endurancePoint int local 0

  | haitus switch
  /if (${maChr.Find[swHiatus].Value} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stHiatus].Value}].RankName}]} && ${Me.PctEndurance} < 20) {
    DEBUG rest${sep}\a-whiatus\ax
    /if (${Me.CombatAbility[${maChr.Find[stHiatus].Value} Rk. III]} && (${Me.CurrentEndurance} < 102500 && ${Me.PctEndurance} < 34)) /varset _endurancePoint 34
    /if (${Me.CombatAbility[${maChr.Find[stHiatus].Value} Rk. II]} && (${Me.CurrentEndurance} < 97500 && ${Me.PctEndurance} < 32)) /varset _endurancePoint 32
    /if (${Me.CombatAbility[${maChr.Find[stHiatus].Value}]} && (${Me.CurrentEndurance} < 92500 && ${Me.PctEndurance} < 29)) /varset _endurancePoint 29

    /if (${Me.PctEndurance} < ${_endurancePoint}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stHiatus].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stHiatus].Value}" 0 FALSE
      }
    }
    /return TRUE

  | rest disc
  } else /if (${Me.CombatAbilityReady[${Spell[${maChr.Find[stRest].Value}].RankName}]} && !${Select[${Me.CombatState},active]}) {
    DEBUG rest${sep}\a-w${maChr.Find[stRest].Value}\ax
    /if (${Me.CombatAbility[${maChr.Find[stRest].Value} Rk. III]}) /varset _endurancePoint 29
    /if (${Me.CombatAbility[${maChr.Find[stRest].Value} Rk. II]}) /varset _endurancePoint 25
    /if (${Me.CombatAbility[${maChr.Find[stRest].Value}]}) /varset _endurancePoint 21

    /if (${Me.PctEndurance} < ${_endurancePoint}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stRest].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stRest].Value}" 0 FALSE
      }
    }
    /return TRUE

  }

/return FALSE



|***
 * note: keeps SK/PAL harmonious up.. always..
 * use: /call check_harmonious
 ***|
sub check_harmonious(bool _debug)
  DEBUG \atcheck_harmonious\ax()

  ISMEDEAD
  CHECKEXIT check_harmonious
  GETINPUT check_harmonious NA|NA

  /invoke ${set_data_timer[${_debug}, Check_Harmonious, RESTART]}

  /if (!${Me.Buff[${maChr.Find[stHarmonious].Value}].ID} && ${Me.PctMana} > 2) {
    /if (${is_ready[${_debug}, "${maChr.Find[stHarmonious].Value}"]}) {
      /call voodoo ${_debug} "${maChr.Find[stHarmonious].Value}" 0 FALSE
    }
  }

/return TRUE



|***
 * note: checks the tag "summonitem" for a count of the item to be summoned on the toon. returns the count
 * use: ${check_basic_tags_summonitem[DEBUG, BUFF##]}
 ***|
sub check_basic_tags_summonitem(bool _debug, string _buff)
  DEBUG \atcheck_basic_tags_summonitem\ax(\a-w${_buff}\ax)

  /declare _count int local
  /varset _count ${FindItemCount[${Spell[${Spell[${maBuff.Find[st${_buff}].Value}].RankName}].Base[1]}]}

/return ${_count}



|***
 * note: converts a click item
 * use: /call get_item_convert DEBUG [item to find] [full name]
 ***|
sub get_item_convert(bool _debug, string _item, string _fullname)
  DEBUG \atget_item_convert\ax(\a-w${_item}, ${_fullname}\ax)

  /while (!${FindItem[${_fullname}].ID}) {
    GETINPUT get_item_convert NA|NA
    ISMEDEAD
    /convertitem ${_item}
    /delay 1s
  }

/return TRUE



|***
 * note: sets/updates a non permanent timer
 * use: ${set_timer[DEBUG, [TIMER VARIABLE NAME], [DURATION]]}
 ***|
sub set_timer(bool _debug, string _my_timer, string _my_duration)
  DEBUG \atset_timer\ax(\a-w${_my_timer}, ${_my_duration}\ax)

  /if (!${Defined[${_my_timer}]}) {
    /declare ${_my_timer} timer outer
  }
  /varset ${_my_timer} ${_my_duration}

/return TRUE



|***
 *  DES: loot routine(s)
 *  USE: auto
 *  NOTE: set a person in the group as master looter. Or, as the raid master looter.
 *  NOTE2: The event in this sectction is designed to handle lore items the toon already posseses. as of this, MQ has no property .Member value for LORE in ${AdvLoot

#Event forcedloreleave "#*#already has #1# and it is lore#*#"
#Event forcedloreleave "#*#does not want #1#. It is either on their never list or they have selected No#*#"
sub Event_forcedloreleave(string line, string _itemName, bool _debug)
  /if (!${maEnv.Find[swADVLoot].Value}) /return
  /varset _itemName ${AdvLoot.SList[1].Name}
  /invoke ${out[14, "Cursor ${sep} ${cinfo}${_itemName}\ax -> ${oleave}"]}
  /ini "${INILoot}" "${_itemName.Left[1]}" "${_itemName}" Leave
  /doevents flush
/return
 ***|
sub do_loot(bool _debug)
  DEBUG \atdo_loot\ax()

  ISMEDEAD
  GETINPUT do_loot NA|NA

  /invoke ${set_data_timer[FALSE, Check_Loot, RESTART]}

  | skip if MQ2AutoLoot is being used
  /if (${Bool[${Plugin[MQ2Autoloot]}]}) {
    /return
  }

  /if (!${Me.FreeInventory}) {
    OUT Setting loot${sep}\a-woff\ax
    OUT No more inventory space.
    /invoke ${do_raw_edit[${_debug}, SILENT, stLootMode, "off"]}
    /return FALSE
  }

  SETHUD "loot whoring"

  | manual looting
  /if (${maEnv.Find[stLootMode].Value.Equal[manual]}) {


  | advanced loot
  } else /if (${maEnv.Find[stLootMode].Value.Equal[advanced]}) {
    
    /if (${Group.Members} && ${Group.MasterLooter.ID} != ${Me.ID}) {
      DEBUG ${break}i'm not the looter
      /return FALSE
    }

    /if (!${AdvLoot.SCount} && !${AdvLoot.PCount}) {
      DEBUG ${break}loot empty
      /return FALSE
    }

    /if (!${Me.UseAdvancedLooting} || ${AdvLoot.LootInProgress}) {
      DEBUG ${break}loot in progress
      /return FALSE
    }

    | what the fuck is going on here?
    /if (!${maOver.Find[swOverLoot].Value}) {
      /if (${Raid.Members}) {
        /if (${Raid.MasterLooter.ID} != ${Me.ID} && ${Group.MasterLooter.ID} != ${Me.ID} ) {
          /return FALSE
        }
      } else /if (!${Raid.Members}) {
        /if (${Group.Members} && ${Group.MasterLooter.ID} != ${Me.ID}) {
        /return FALSE
      }
    }

    /declare _scount int local 0
    /declare _pcount int local 0
    /declare _notfound bool local FALSE

    | Item To Process Name (the thingie in the list of advloot)
    /declare _advlItem string local

    | _advlChoice is what is listed to do with the items: Leave,Destroy,Keep,Sell,Announce
    /declare _advlChoice string local

    | if we want to give it to someone else
    /declare _advlPerson string local

    | Shared List Sorting
    /if ((${Raid.Members} || ${Group.Members}) && ${AdvLoot.SCount}) {

      /while (${AdvLoot.SCount}) {

        CHECKDEAD

        /if (${AdvLoot.SList[1].Name.Equal[NULL]}) {
          /continue
        }

        | advloot window checkboxes will override
        /delay 5s !${AdvLoot.LootInProgress}
        /if (${AdvLoot.SList[1].Need}) {
          /advloot shared 1 giveto ${Me.Name}
          /continue
        } else /if (${AdvLoot.SList[1].Greed}) {
          /advloot shared 1 giveto ${Me.Name}
          /continue
        } else /if (${AdvLoot.SList[1].No}) {
          /advloot shared 1 leave
          /continue
        } else /if (${AdvLoot.SList[1].AlwaysGreed}) {
          /advloot shared 1 giveto ${Me.Name}
          /continue
        } else /if (${AdvLoot.SList[1].AlwaysNeed}) {
          /advloot shared 1 giveto ${Me.Name}
          /continue
        } else /if (${AdvLoot.SList[1].Never}) {
          /advloot shared 1 leave
          /continue
        }

        | get all the data for that item from loot file
        /sqlite query "${DBloot}" slootquery SELECT * FROM loot_control WHERE item="${AdvLoot.SList[1].Name}";
        /if (!${sql_check[${_debug}, slootquery, loot_control, "SELECT (loot_control)"]}) /endmacro

        | if we did not find the item?
        /if (!${sqlite.Rows[slootquery]}) {

          /if (${lsGrain.Contains[14]}) {
            OUT Loot${sep}${sep}\a-w${AdvLoot.SList[1].Name}\ax (\a-rnot in database\ax)
          }
          /if (${AdvLoot.SList[1].NoDrop} || ${FindItem[=${AdvLoot.SList[1].Name}].Lore} || ${FindItemBank[${AdvLoot.SList[1].Name}].Lore}) {
            /if (${lsGrain.Contains[14]}) {
              OUT Loot${sep}${add}${sep}\a-w${AdvLoot.SList[1].Name}\ax (\a-r${leave} [no drop/lore]\ax)
            }
            /sqlite query "${DBloot}" slootquery INSERT OR REPLACE INTO loot_control(item,action) VALUES("${AdvLoot.SList[1].Name}", "leave");
            /if (!${sql_check[${_debug}, slootquery, loot_control, "INSERT OR REPLACE INTO (loot_control)"]}) /endmacro
              /delay 1s !${AdvLoot.LootInProgress}
              /if (${AdvLoot.SList[1].ID}) {
                DEBUG ${sep}Leaving${sep}\a-w${AdvLoot.SList[1].Name}\ax
                /advloot shared 1 leave
                /delay 1
              }

          } else {
            /if (${lsGrain.Contains[14]}) {
              OUT Loot${sep}${add}${sep}\a-w${AdvLoot.SList[1].Name}\ax (\a-r${keep} [default]\ax)
            }
            | set item as keep
            /sqlite query "${DBloot}" slootquery INSERT OR REPLACE INTO loot_control(item,action) VALUES("${AdvLoot.SList[1].Name}", "keep");
            /if (!${sql_check[${_debug}, slootquery, loot_control, "INSERT OR REPLACE INTO (loot_control)"]}) /endmacro
          }

          | reload lootquery
          /sqlite query "${DBloot}" slootquery SELECT * FROM loot_control WHERE item="${AdvLoot.SList[1].Name}";
          /if (!${sql_check[${_debug}, slootquery, loot_control, "SELECT (loot_control)"]}) /endmacro

        }

        | set our actions
        /varset _advlChoice ${sqlite.Result[slootquery 1 action]}
        /varset _advlPerson ${sqlite.Result[slootquery 1 pass]}

        | now we do something with the item.
        | if we are going to leave the item or if it is lore AND we have one already. Conversly, we loot if we intend to keep or destroy the item
        | pass the item off if there is a person listed and in group
        /if (${Bool[${_advlPerson}]} && ${Group.Member[${_advlPerson}].ID}) {
          /delay 1s !${AdvLoot.LootInProgress}
          /if (${AdvLoot.SList[1].ID}) {
            DEBUG ${sep}Passing to${sep}\a-w${_advlPerson}\ax
            /delay 1
            /advloot shared 1 giveto ${_advlPerson}
          }

        | ditch the item if we need too
        } else /if (${Select[${_advlChoice},leave]} || ${FindItem[=${AdvLoot.SList[1].Name}].Lore} || ${FindItemBank[${AdvLoot.SList[1].Name}].Lore}) {

          /if (${Select[${_advlChoice},Announce]} && ${lsGrain.Contains[14]}) {
            OUT Loot${sep}\ayLeaving\ax${sep}\a-w${AdvLoot.SList[1].Name}
          }

          /delay 1s !${AdvLoot.LootInProgress}
          /if (${AdvLoot.SList[1].ID}) {
            DEBUG ${sep}eaving${sep}\a-w${AdvLoot.SList[1].Name}\ax
            /advloot shared 1 leave
            /delay 1
            | /return TRUE
          }

        | or we keep it.
        } else /if (${AdvLoot.SList[1].ID}) {
          /if (${Select[${_advlChoice},keep,destroy,sell]}) {
            /delay 1s !${AdvLoot.LootInProgress}
            /if (${AdvLoot.SList[1].ID}) {
              DEBUG ${sep}Moving to PList${sep}\a-w${AdvLoot.SList[1].Name}\ax
              /delay 1
              /advloot shared 1 giveto ${Me.Name}
              /delay 1s !${AdvLoot.LootInProgress}
            }
          }
        }
      }
    }

    | Personal List sorting. ALSO for solo killing
    /if (${AdvLoot.PCount}) {
      DEBUG ${sep}PList loot ${submenu}
      /declare _advlChoiceP string local
      /declare _tmpName string local FALSE

      /while (${AdvLoot.PCount}) {

        CHECKDEAD

        | advloot window checkboxes will override
        /delay 1s !${AdvLoot.LootInProgress}
        /if (${AdvLoot.PList[1].Need}) {
          /advloot personal 1 loot
          /continue
        } else /if (${AdvLoot.PList[1].Greed}) {
          /advloot personal 1 loot
          /continue
        } else /if (${AdvLoot.PList[1].No}) {
          /advloot personal 1 leave
          /continue
        } else /if (${AdvLoot.PList[1].AlwaysGreed}) {
          /advloot personal 1 loot
          /continue
        } else /if (${AdvLoot.PList[1].AlwaysNeed}) {
          /advloot personal 1 loot
          /continue
        } else /if (${AdvLoot.PList[1].Never}) {
          /advloot personal 1 leave
          /continue
        }

        DEBUG ${sep}personal${sep}1${sep}\a-e${AdvLoot.PList[1].Name}\ax

        /sqlite query "${DBloot}" plootquery SELECT * FROM loot_control WHERE item="${AdvLoot.PList[1].Name}";
        /if (!${sql_check[${_debug}, plootquery, loot_control, "SELECT (loot_control 1)"]}) /endmacro

        /varset _tmpName ${AdvLoot.PList[1].Name}

        /if (!${sqlite.Rows[plootquery]}) {
          /if (${lsGrain.Contains[14]}) {
            OUT Loot${sep}\a-w${AdvLoot.PList[1].Name}\ax (\a-rnot in database\ax)
          }

          /if (${AdvLoot.PList[1].NoDrop} || ${FindItem[=${AdvLoot.PList[1].Name}].Lore} || ${FindItemBank[1].Lore}) {
            /if (${lsGrain.Contains[14]}) {
              OUT Loot${sep}${add}${sep}\a-w${AdvLoot.PList[1].Name}\ax (\a-r${leave} [no drop/lore]\ax)
            }
            /sqlite query "${DBloot}" plootquery INSERT OR REPLACE INTO loot_control(item,action) VALUES("${AdvLoot.PList[1].Name}", "leave");
            /if (!${sql_check[${_debug}, plootquery, loot_control, "INSERT OR REPLACE INTO (loot_control 2)"]}) /endmacro

            /if (${AdvLoot.PList[1].ID}) {
              DEBUG ${sep}\ayLeaving\ax${sep}\a-w${AdvLoot.PList[1].Name}\ax
              /delay 1
              /advloot personal 1 leave
              /delay 1s !${AdvLoot.LootInProgress}
            }

          } else {
            /if (${lsGrain.Contains[14]}) {
              OUT Loot${sep}${add}${sep}\a-w${AdvLoot.PList[1].Name}\ax (\a-r${keep} [default]\ax)
            }
            /sqlite query "${DBloot}" plootquery INSERT OR REPLACE INTO loot_control(item,action) VALUES("${AdvLoot.PList[1].Name}", "keep");
            /if (!${sql_check[${_debug}, plootquery, loot_control, "INSERT OR REPLACE INTO (loot_control)"]}) /endmacro

          }
          /delay 5
          | reload lootquery
          /sqlite query "${DBloot}" plootquery SELECT * FROM loot_control WHERE item="${AdvLoot.PList[1].Name}";
          /if (!${sql_check[${_debug}, plootquery, loot_control, "SELECT (loot_control)"]}) /endmacro

        }

        /varset _advlChoiceP ${sqlite.Result[plootquery 1 action]}
        DEBUG ${sep}_advlChoiceP${sep}\a-w${sqlite.Result[plootquery 1 action]}\ax

        /if (${AdvLoot.PList[1].ID}) {
          DEBUG ${sep}${AdvLoot.PList[1].ID}
          /delay 2s !${AdvLoot.LootInProgress}
          /if (${Select[${_advlChoiceP},ignore,leave]} || ${FindItem[=${AdvLoot.PList[1].Name}].Lore} || ${FindItemBank[${AdvLoot.PList[1].Name}].Lore}) {
            /if (${lsGrain.Contains[14]}) {
              OUT Loot${sep}\a-yLeaving\ax${sep}\a-w${AdvLoot.PList[1].Name}\ax
            }
            /if (${AdvLoot.PList[1].ID}) {
              DEBUG ${sep}Leaving${sep}\a-w${AdvLoot.PList[1].Name}\ax
              /delay 1
              /advloot personal 1 leave
              /delay 1s !${AdvLoot.LootInProgress}
            }

          } else /if (${Select[${_advlChoiceP},keep,sell]}) {
            DEBUG ${sep}Moving to Inventory${sep}\a-w${AdvLoot.PList[1].Name}\ax
            /if (${AdvLoot.PList[1].ID}) {
              /delay 1
              /advloot personal 1 loot
              /delay 1s !${AdvLoot.LootInProgress}
            }
            /delay 5
            /if (${Window[ConfirmationDialogBox].Open}) {
              /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
              /delay 1
            }

          } else /if (${Select[${_advlChoiceP},destroy]}) {
            /if (${AdvLoot.PList[1].ID}) {
              DEBUG ${sep}Moving to Inventory to destroy${sep}\a-w${AdvLoot.PList[1].Name}\ax
              /delay 1
              /advloot personal 1 loot
              /delay 1s !${AdvLoot.LootInProgress}
            }

            /delay 5
            /if (${FindItem[="${_tmpName}"].ID}) {


  | I'M SERIOUSLY SICK OF FIXING THIS delete code.
              /while (${FindItem[="${_tmpName}"].ID}) {
                CHECKDEAD
                /delay 5
                /nomodkey /shift /itemnotify in Pack${Math.Calc[${FindItem[=${_tmpName}].ItemSlot}-22]} ${Math.Calc[${FindItem[=${_tmpName}].ItemSlot2}+1]} leftmouseup

                /if (${lsGrain.Contains[14]}) {
                  OUT Loot${sep}\arDestroying\ax${sep}\a-w${_tmpName}\ax
                }
                /nomodkey /destroy

                /call check_cursor ${_debug} clear
                /if (${Window[ConfirmationDialogBox].Open}) {
                  /delay 1
                  /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
                }
                /delay 3 !${Cursor.ID}
              }
  | evil delete code end...

            }
          }
        }
      }
    }
  }
}

  SETHUD EMPTY

/return TRUE



|***
 * note: Cures/removes debuffs, we hope...
 * use:
 ***|
sub check_cure(bool _debug)
  DEBUG \atcheck_cure\ax()

  /invoke ${set_data_timer[${_debug}, Check_Cure, RESTART]}

  /if (!AUTO) /return FALSE

  | dont take time from mobs attention
  /if ((PULL || AGRO) && ${Me.CombatState.Equal[combat]}) {
    /return FALSE
  }

  SETHUD "check cure"

  ISMEDEAD
  GETINPUT check_cure NA|NA

  | My daughter said you have issues.. She's usually right.
  /if (${Me.TotalCounters} && ${maHeal.Find[swHealCureSelf].Value}) {
    DEBUG ${sep}\aw${Me.DisplayName.Lower}\ax needs a medic!

    | Monk
    /if (${Select[${Me.Class.ShortName},MNK]} && ${Me.AltAbilityReady[Purify Body]}) {
      /call voodooo ${_debug} "Purify Body" 0 FALSE
      /delay 5
      /invoke ${set_data_timer[${_debug}, Check_Cure, 1]}
      /return

    | Paladin, Cleric, Shaman, Druid
    } else /if (${Select[${Me.Class.ShortName},PAL,CLR,SHM,DRU]} && ${Me.AltAbilityReady[Radiant Cure]} && (${Me.CountersCurse} || ${Me.CountersPoison} || ${Me.CountersDisease})) {
      /call voodoo ${_debug} "Radiant Cure" 0 FALSE
      /delay 5
      /invoke ${set_data_timer[${_debug}, Check_Cure, 1]}
      /return

    | Necromancer
    } else /if (${Select[${Me.Class.ShortName},NEC]} && ${Me.AltAbilityReady[Embrace the Decay]} && (${Me.CountersCurse} || ${Me.CountersPoison} || ${Me.CountersDisease} || ${Me.CountersCorruption})) {
      /call voodoo ${_debug} "Embrace the Decay" 0 FALSE
      /delay 5
      /invoke ${set_data_timer[${_debug}, Check_Cure, 1]}
      /return
    }

    | cure with items self only
    /if (${Me.CountersCurse} && ${Me.ItemReady[${maHeal.Find[stCureCurseItem].Value}]}) {
      /call voodoo ${_debug} "${maHeal.Find[stCureCurseItem].Value}" ${Me.ID} FALSE
      /delay 5
    }

    /if (${Me.CountersCorruption} && ${Me.ItemReady[${maHeal.Find[stCureCorruptionItem].Value}]}) {
      /call voodoo ${_debug} "${maHeal.Find[stCureCorruptionItem].Value}" ${Me.ID} FALSE
      /delay 5
    }

    /if (${Me.CountersPoison} && ${Me.ItemReady[${maHeal.Find[stCurePoisonItem].Value}]}) {
      /call voodoo ${_debug} "${maHeal.Find[stCurePoisonItem].Value}" ${Me.ID} FALSE
      /delay 5
    }

    /if (${Me.CountersDisease} && ${Me.ItemReady[${maHeal.Find[stCureDiseaseItem].Value}]}) {
      /call voodoo ${_debug} "${maHeal.Find[stCureDiseaseItem].Value}" ${Me.ID} FALSE
      /delay 5
    }
  } else /if (!${Me.TotalCounters} && ${maHeal.Find[swHealCureSelf].Value}) {
    DEBUG ${sep}\aw${Me.DisplayName.Lower}\ax is just fine
  }

  | bail now if only self curing
  /if (!${maHeal.Find[swHealCureGroup].Value} && !${maHeal.Find[swHealCureCrew].Value}) {
    /return TRUE
  }

  | time to check the others
  /declare _count int local 0
  /declare _name string local

  | each toon
  /for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}

    | get the name to something usable
    /varset _name ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
    DEBUG ${sep}checking${sep}\aw${_name}\ax (\a-r${maComm.Find[stEntropyGroup_all].Value}\ax)

    | are they here?
    /if (!${Spawn[pc ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}].ID}) /continue


    | group only
    /if (${maHeal.Find[swHealCureGroup].Value} && !${maHeal.Find[swHealCureCrew].Value}) {
      /if (!${Group.Member[${_name}].ID}) {
        /continue
      }
    }

    | if the observer hasnt "observerd" the toon yet. go no further!
    /if (!${Defined[${_name}_TotalCounters]}) {
      DEBUG ${sep}\aw${_name}\ax is not being observerd.. \archeck observers\ax...
      /continue
    }

    | are they close?. if not. fuck'em
    /if (${Spawn[pc ${_name}].Distance} > ${maHard.Find[stMaxCureRad].Value}) /continue

    | toon dead?
    /if (${Spawn[pc ${_name}].Dead}) /continue

    | are they debuffed at all?
    /if (!${Bool[${${_name}_TotalCounters}]} && !${Bool[${${_name}_Snared}]} && !${Bool[${${_name}_Mezzed}]}) {
      DEBUG ${sep}\aw${_name}\ax is just fine
      /continue
    } else /if (${Bool[${${_name}_TotalCounters}]}) {
      OUT Medic${sep}\a-w${_name}\ax (\a-r${${_name}_TotalCounters}\ax)
    }

    ISMEDEAD

    | healer class AA curing
    /if (${Select[${Me.Class.ShortName},PAL,CLR,SHM,DRU]}) {

      | AA Purify Soul
      | 200' 5 min
      /if (${Select[${Me.Class.ShortName},PAL,CLR,DRU]} && ${Me.AltAbilityReady[Purify Soul]} && ${Spawn[pc ${_name}].Distance} < 200) {
        /call voodoo ${_debug} "Purify Soul" ${Spawn[pc ${_name}].ID} FALSE
        /delay 5
        /if (!${Bool[${${_name}_TotalCounters}]}) {
          /invoke ${set_data_timer[FALSE, Check_Cure, 1s]}
          /return
        }
      }

      | AA Radient Cure
      | 100' 1 min
      /if (${Select[${Me.Class.ShortName},PAL,CLR,SHM,DRU]} && ${Me.AltAbilityReady[Radiant Cure]} && ${Spawn[pc ${_name}].Distance} < 100 && ${Group.Member[${_name}].ID}) {
        /call voodoo ${_debug} "Radiant Cure" 0 FALSE
        /delay 5
        /if (!${Bool[${${_name}_TotalCounters}]}) {
          /invoke ${set_data_timer[FALSE, Check_Cure, 1s]}
          /return
        }
      }

      | AA Group Purify Soul . were in deep shit if we've gotten this far.
      | 0/100' 15 min
      /if (${Select[${Me.Class.ShortName},CLR,DRU]} && ${Me.AltAbilityReady[Group Purify Soul]} && ${Spawn[pc ${_name}].Distance} < 100 && ${Group.Member[${_name}].ID}) {
        /call voodoo ${_debug} "Group Purify Soul" 0 FALSE
        /delay 5
        /if (!${Bool[${${_name}_TotalCounters}]}) {
          /invoke ${set_data_timer[FALSE, Check_Cure, 1s]}
          /return
        }
      }

      | paladin splash 50' range
      /if (${Select[${Me.Class.ShortName},PAL]} && ${Spawn[pc ${_name}].Distance} < 50) {
        /call Bind_command_splash FALSE
        /delay 5
        /if (!${Bool[${${_name}_TotalCounters}]}) {
          /invoke ${set_data_timer[FALSE, Check_Cure, 1s]}
          /return
        }
      }
    }

    | are they debuffed?
    DEBUG ${sep}\aw${_name}\aw_TotalCounters${sep}\a-w${${_name}_TotalCounters}\ax
    /if (!${Bool[${${_name}_TotalCounters}]}) {
      /continue
    }

    | is the DB set with the following cures to continue?
    /if (!${Bool[${maHeal.Find[stCureCurse].Value}]} && !${Bool[${maHeal.Find[stCureCorruption].Value}]} && !${Bool[${maHeal.Find[stCurePoison].Value}]} && !${Bool[${maHeal.Find[stCureDisease].Value}]}) {
      /continue
    }

    | cure with spell
    /if (${Bool[${maHeal.Find[stCureCurse].Value}]} && ${Range.Between[1,100:${${_name}_CountersCurse}]}) {
      DEBUG ${sep}${_name} is CURSED... oh fuck..
      /delay 3s !${Me.SpellInCooldown}
      /if (${is_ready[${_debug}, "${maHeal.Find[stCureCurse].Value}"]}) {
        /call voodoo ${_debug} "${maHeal.Find[stCureCurse].Value}" ${Spawn[pc ${_name}].ID} FALSE
        /delay 5
        /if (!${Bool[${${_name}_CountersCurse}]}) {
          /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
          /return
        }
      }
    }

    /if (${Bool[${maHeal.Find[stCureCorruption].Value}]} && ${Range.Between[1,100:${${_name}_CountersCorruption}]}) {
      DEBUG ${sep}${_name} is CORRUPTED... can't trust that bitch..
      /delay 3s !${Me.SpellInCooldown}
      /if (${is_ready[${_debug}, "${maHeal.Find[stCureCorruption].Value}"]}) {
        /call voodoo ${_debug} "${maHeal.Find[stCureCorruption].Value}" ${Spawn[pc ${_name}].ID} FALSE
        /delay 5
        /if (!${Bool[${${_name}_CountersCorruption}]}) {
          /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
          /return
        }
      }
    }

    /if (${Bool[${maHeal.Find[stCurePoison].Value}]} && ${Range.Between[1,100:${${_name}_CountersPoison}]}) {
      DEBUG ${sep}${_name} is POISONED... teach you to play with snakes
      /delay 3s !${Me.SpellInCooldown}
      /if (${is_ready[${_debug}, "${maHeal.Find[stCurePoison].Value}"]}) {
        /call voodoo ${_debug} "${maHeal.Find[stCurePoison].Value}" ${Spawn[pc ${_name}].ID} FALSE
        /delay 5
        /if (!${Bool[${${_name}_CountersPoison}]}) {
          /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
          /return
        }
      }
    }

    /if (${Bool[${maHeal.Find[stCureDisease].Value}]} && ${Range.Between[1,100:${${_name}_CountersDisease}]}) {
      DEBUG ${sep}${_name} is DISEASED...Burn'em with FIRE!!!
      /delay 3s !${Me.SpellInCooldown}
      /if (${is_ready[${_debug}, "${maHeal.Find[stCureDisease].Value}"]}) {
        /call voodoo ${_debug} "${maHeal.Find[stCureDisease].Value}" ${Spawn[pc ${_name}].ID} FALSE
        /delay 5
        /if (!${Bool[${${_name}_CountersDisease}]}) {
          /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
          /return
        }
      }
    }

  /next _count

  SETHUD EMPTY

/return



|***
 * note: dumb fucks always gotta put that red bouncy ball on my cursor
 * use: /call check_cursor DEBUG [clear]
 ***|
sub check_cursor(bool _debug, string _verbage)
  DEBUG \atcheck_cursor\ax(\a-w${_verbage}\ax)

  | skip if MQ2AutoLoot is being used
  /if (${Bool[${Plugin[MQ2Autoloot]}]}) {
    /return
  }

  SETHUD "cursor"

  /if (!${Me.FreeInventory}) {
    DEBUG ${break}no free inventorpy
    /return FALSE
  }

  | absolute end all of clearing the cursor
  /if (${_verbage.Equal[getoffthedamncursor]} && ${Cursor.ID}) {
    /declare _cursorbail int local 1
    /while (${Cursor.ID} && ${_cursorbail} < 31 && ${Me.FreeInventory}) {
      CHECKDEAD
      /autoinventory
      /delay 2
      /varcalc _cursorbail ${_cursorbail} +1
    }

  }

  | force clearing the cursor and GTFO
  /if (${_verbage.Equal[clear]}) {
    DEBUG ${sep}Force Inventory${sep}\a-w${Cursor.Name}\ax
    /invoke ${set_timer[${_debug}, Check_Cursor, 0]}
  }

  /if (${timer_Check_Cursor}) /return FALSE
  ISMEDEAD

  | please bow your heads..  lsAutoInventory has entered the room
  /if (${lsAutoInventory.Contains[${Cursor.Name}]}) {
    DEBUG ${sep}lsAutoInventory${sep}\a-w${Cursor.Name}\ax
    /autoinventory
    /return TRUE
  }

  /invoke ${set_data_timer[${_debug}, Check_Cursor, 5s]}

  | /delay 2

  /if (!${Cursor.ID}) {
    /return FALSE
  }

  | check DB for the item
  /call set_loot_item ${_debug} "${Cursor.Name}"

  | reload data
  /sqlite query "${DBloot}" lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}" LIMIT 1;
  /if (!${sql_check[${_debug}, lootquery, check_cursor, SELECT]}) /endmacro

  | correct the ignore vs. leave spam issue for lore items in the db, then reset
  /if (${Cursor.Lore} && ${Select[${sqlite.Result[lootquery 1 action]},leave]}) {
    /sqlite query "${DBloot}" lootquery UPDATE loot_control SET 'action'="ignore"  WHERE item="${Cursor.Name}";
    /if (!${sql_check[${_debug}, lootquery, check_cursor, UPDATE]}) /endmacro
    /invoke ${set_data_timer[${_debug}, Check_Cursor, 5]}
    /return TRUE
  }

  | wtf we gotta do with the item?
  | do the things that we do best
  /if (${Select[${sqlite.Result[lootquery 1 action]},keep]}) {
    | /invoke ${out[14, 0, 37]}
    /invoke ${out[14, "Cursor${sep}\agkeep\ax${sep}\a-w${Cursor.Name}\ax${sep}\a-w${FindItemCount[=${Cursor.Name}]}\ax"]}
    /nomodkey /autoinventory
  } else /if (${Select[${sqlite.Result[lootquery 1 action]},sell]}) {
    /invoke ${out[14, 0, 38]}
    /nomodkey /autoinventory
  } else /if (${Select[${sqlite.Result[lootquery 1 action]},destroy]}) {
    /invoke ${out[14, 0, 39]}
    /nomodkey /destroy
  } else /if (${Select[${sqlite.Result[lootquery 1 action]},leave]}) {
    | /invoke ${out[14, 0, 40]}
    /invoke ${out[14, "Cursor${sep}\ayleave\ax${sep}\a-w${Cursor.Name}\ax"]}
    /nomodkey /drop
  } else /if (${Select[${sqlite.Result[lootquery 1 action]},ignore]}) {
    
    | force auto inventroy if we have too
    /if (${_verbage.Equal[clear]}) {
      /nomodkey /autoinventory
    }
    
    | do nothing.. seriously.. what do you think ignore means?
  
  }

  | /delay 1

  | drop the timer so we dont have to wait again to check. get that shit off the cursor
  /if (${Cursor.ID}) {
    /invoke ${set_data_timer[${_debug}, Check_Cursor, 5]}
  }

  SETHUD EMPTY

/return TRUE



|***
 * note: will cast ePen pet against all raid leader instructions. because my DPS parse matters, not yours!
 * use: /call cast_temp_pet DEBUG [MOBID] [Temp Spell Name]
 ***|
sub cast_temp_pet(bool _debug, int _tmpID, string _tmpSpell)
  DEBUG \atcast_temp_pet\ax(\a-w${_tmpID}, ${_tmpSpell}\ax)

  | these classes can only have one pet at a time (not counting  AA and spell combo. but i'n not gonna sort that shit)
  /if (${Select[${Me.Class.ShortName},CLR,WIZ]}) {
    /if (${Me.Pet.ID}) /return FALSE
  }

  ISMEDEAD
  CHECKEXIT cast_temp_pet
  GETINPUT cast_temp_pet NA|NA

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  /if (${Target.PctHPs} > ${maMinion.Find[stPctSwarmEngage].Value}) {
    /return FALSE
  }

  /if (${is_ready[${_debug}, "${_tmpSpell}"]}) {
    /call voodoo ${_debug} "${_tmpSpell}" ${_tmpID} FALSE
  }

/return TRUE



|***
 * note: chopper.. sic balls!
 * use: /call send_pet ${maDebug.Find[sic].Value} [TARGETID]
 ***|
sub send_pet(bool _debug, int _tmpID)
  DEBUG \atsend_pet\ax(\a-w${_tmpID}\ax)

  | no target == bail
  /if (!${_tmpID}) {
    DEBUG ${break}._tmpID
    /return FALSE
  }

  | out of env range
  /if (${Spawn[id ${_tmpID}].Distance} > ENV) {
    DEBUG ${break}>env
    /return FALSE
  }

  | pet has the passed target
  /if (${Me.Pet.Target.ID} == ${_tmpID}) {
    DEBUG ${break}ID${sep}\a-w${Me.Pet.Target.ID} == ${_tmpID}\ax
    /return TRUE
  }

  | pets target does not match the passed id
  /if (${Me.Pet.Target.ID} != ${_tmpID}) {
    DEBUG ${break}ID${sep}\a-w${Me.Pet.Target.ID} != ${_tmpID}\ax
    /pet back off
  }

  | manual peoples...
  /if (!AUTO && !${maMinion.Find[swonForce].Value}) {
    DEBUG ${break}manual NO swonForce
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT send_pet
  GETINPUT send_pet NA|NA

  /if (PET) {
    /if (${Spawn[id ${_tmpID}].PctHPs} < ${maMinion.Find[stPctPetEngage].Value}) {
      /invoke ${target[set, ${_tmpID}]}
      /pet attack
    }
  }

  | send in the swarm
  /if (SWARM) {
    /if (${Spawn[id ${_tmpID}].PctHPs}  < ${maMinion.Find[stPctSwarmEngage].Value}) {
      /invoke ${target[set, ${_tmpID}]}
      /pet swarm
    }
  }

/return TRUE



|***
 * note: cycles all debuff routines. (NOT mezing).. look at eradicate for walkthrough cause your dumb ass will forget
 * use: /call check_deBuffs DEBUG [TARGETID]
 ***|
sub check_deBuffs(bool _debug, int _tmpID)
  DEBUG \atcheck_deBuffs\ax(\a-w${_tmpID}\ax)

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  | check distance or bail early
  /if (${Spawn[${_tmpID}].Distance} > ENV) {
    /invoke ${set_data_timer[FALSE, Check_deBuffs, RESTART]}
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT check_deBuffs
  CHECKTIE
  GETINPUT check_deBuffs _tmpID|${_tmpID}

  /invoke ${target[set, ${_tmpID}]}

  /declare _count int local 0
  /declare _mobCount int local 0
  /declare _tmpMobID int local
  /declare _tmpSpell string local
  /declare _lideb listiterator local

  | eradicate (dispell thingies)
  /if (${madeBuff.Find[swEradicate].Value}) {
    DEBUG ${sep}{madeBuff.Find[swEradicate].Value}${sep}\a-w${madeBuff.Find[swEradicate].Value}\ax

    | does this mob have a timer associated with it?
    | if there is no timer, create one with 1/10 second
    /if (!${Defined[timer_eradicate_ID${_tmpID}]}) {
      /invoke ${set_timer[${_debug}, timer_eradicate_ID${_tmpID}, 1]}
    }

    | if there is not timer left on the target, get to work
    | first check if there is an ae component for this debuff, if there is
    | and the character has an ae count that is = < then mobs around the target
    | fire the AE and add all those in that AE range into the list
    | otherwise just do singles
    /if (!${timer_eradicate_ID${_tmpID}} || !${lsdeBuffEradicate.Contains[${_tmpID}]}) {
      DEBUG NO timer or not in list

      /if (${Bool[${madeBuff.Find[stAEEradicate].Value}]} && AE#) {
        /varset _mobCount ${SpawnCount[npc loc ${Spawn[id ${_tmpID}].X} ${Spawn[id ${_tmpID}].Y} radius ${Spell[${madeBuff.Find[stAEEradicate].Value}].AERange}]}
      } else {
        /varset _mobCount 0
      }

      DEBUG _mobCount${sep}\a-w${_mobCount}\ax

      | single mob debuff
      /if ((!AE# || (AE# && !${is_ready[${_debug}, "${madeBuff.Find[stAEEradicate].Value}"]})) && ${is_ready[${_debug}, "${madeBuff.Find[stEradicate].Value}"]}) {
        DEBUG ${sep}{madeBuff.Find[swEradicate].Value}${sep}\a-wSINGLE\ax

        | try to eradicate it
        /call voodoo ${_debug} "${madeBuff.Find[stEradicate].Value}" ${_tmpID} FALSE

        | was the debuff cast successful?
        /if (${maData.Find[stCastReturn].Value.Equal[SUCCESS]}) {
          | throw it in out list if not there
          /if (!${lsdeBuffEradicate.Contains[${_tmpID}]}) {
            /invoke ${lsdeBuffEradicate.Append[${_tmpID}]}
          }

          | set the re-check timer
          /invoke ${set_timer[${_debug}, timer_eradicate_ID${_tmpID}, ${madeBuff.Find[stTimerEradicate].Value}]}

        | cast failure we try again in one tick
        } else /if (${maData.Find[stCastReturn].Value.NotEqual[SUCCESS]}) {
          /invoke ${set_timer[FALSE, timer_eradicate_ID${_tmpID}, 6s]}
        }

        /while (${Me.SpellInCooldown}) {
          CHECKEXIT
          ISMEDEAD
          /if (!${Me.SpellInCooldown}) /break
          /delay 5
        }

      }

      | multi mob debuff

      /if (AE# && (${_mobCount} >= AE#)) {
        /if (${is_ready[${_debug}, "${madeBuff.Find[stAEEradicate].Value}"]}) {
          DEBUG ${sep}{madeBuff.Find[swEradicate].Value}${sep}\a-wMULTI\ax

          | try to eradicate it
          /call voodoo ${_debug} "${madeBuff.Find[stAEEradicate].Value}" ${_tmpID} FALSE

          | was the debuff cast successful?
          /if (${maData.Find[stCastReturn].Value.Equal[SUCCESS]}) {

            | throw it in out list if not there
            /if (!${lsdeBuffEradicate.Contains[${_tmpID}]}) {
              /invoke ${lsdeBuffEradicate.Append[${_tmpID}]}
            }

            | set the re-check timer
            /invoke ${set_timer[${_debug}, timer_eradicate_ID${_tmpID}, ${madeBuff.Find[stTimerEradicate].Value}]}

            | get all his fucking friends too
            /for _count 1 to ${_mobCount}
              /varset _tmpMobID ${Spawn[${_tmpID}].NearestSpawn[${_count} ,npc radius ${Spell[${madeBuff.Find[stAEEradicate].Value}].AERange}].ID}
              /if (${_tmpMobID} == 0) /break

              /if (!${lsdeBuffEradicate.Contains[${_tmpMobID}]}) {
                /invoke ${lsdeBuffEradicate.Append[${_tmpMobID}]}
              }

              | bitches need timers
              /invoke ${set_timer[${_debug}, timer_eradicate_ID${_tmpMobID}, ${madeBuff.Find[stTimerEradicate].Value}]}

            /next _count

          | cast failure we try again in one tick
          } else /if (${maData.Find[stCastReturn].Value.NotEqual[SUCCESS]}) {
            /invoke ${set_timer[FALSE, timer_eradicate_ID${_tmpID}, 6s]}
          }

          /while (${Me.SpellInCooldown}) {
            CHECKEXIT
            ISMEDEAD
            /if (!${Me.SpellInCooldown}) /break
            /delay 5
          }
        }
      }

    }
  | end of eradicate debuff checks
  }

  |**
    documenting this for my fucking sanity
    debuff cycle:
    loop through: Tash,Malo,Slow,Snare,Cripple
    for each cycle a mob id list is maintained in the form of: lsdeBuff[TYPE]
    a timer is maintained for each IT in the list in the form of: timer_[TYPE]_ID[MOBID]
    the timer is based off the debuf duration less 6 seconds.

    if a timer exists and the ID is in the list, we ignore the mob.
    if a timer does not exist and it is debuffed, it is added to that list, a timer remaining is created and it is added to the list
    if a timer does exist and it is not in hte list, add to the list and move on (should not happen)
    if no debuff, no timer, and no id in list. debuff the shit

    this code does not check spells or sub procs of spells/ass/items. it works only off target debuff member checking.
    what this means: you may not get the best slow or cripple. the code accepts whatever is there as a valid debuff and moves on.
  **|

  /declare _lsout list local
  /invoke ${_lsout.Append[Tash,Malo,Slow,Snare,Cripple]}
  /vardata _lideb _lsout.First.Clone
  /declare _tmpTLO string local

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  | make sure we have a target and the buff list is current
  /invoke ${target[set, ${_tmpID}]}
  /delay 1s ${Target.BuffsPopulated}

  | start the debuff cycle
  /while (!${_lideb.IsEnd}) {
    DEBUG ${sep}checking${sep}\a-w${_lideb.Value}\ax (${If[${madeBuff.Find[sw${_lideb.Value}].Value},${on},${off}]})

    CHECKDEAD

    /if (${_lideb.Value.Equal[Slow]}) {
      /varset _tmpTLO Slowed
    } else /if (${_lideb.Value.Equal[Snare]}) {
      /varset _tmpTLO Snared
    } else /if (${_lideb.Value.Equal[Tash]}) {
      /varset _tmpTLO Tashed
    } else /if (${_lideb.Value.Equal[Cripple]}) {
      /varset _tmpTLO Crippled
    } else /if (${_lideb.Value.Equal[Malo]}) {
      /varset _tmpTLO Maloed
    }

    | is the debuf turned on
    /if (${madeBuff.Find[sw${_lideb.Value}].Value}) {

      | check if there is a debff timer for this debuff/mob. if not, create it
      /if (!${Defined[timer_${_lideb.Value.Lower}_ID${_tmpID}]}) {
        /invoke ${set_timer[${_debug}, timer_${_lideb.Value.Lower}_ID${_tmpID}, 0]}
      }

        /if (${_tmpTLO.Equal[Slowed]} && ${Select[${Target.Slowed},Tendon Slice,Helix of the Undying,Crippling Snare]}) {
          /goto :ignorecrapslow
        }

      | add if already there?
      DEBUG ${sep}checking:
      DEBUG \ar....\ax ${_tmpTLO}${sep}\a-w${Target.${_tmpTLO}}\ax(\a-r${Target.${_tmpTLO}.ID}\ax)
      | DEBUG \ar....\ax single debuff${sep}\a-w${Target.Buff[${madeBuff.Find[st${_lideb.Value}].Value}].ID}\ax
      | DEBUG \ar....\ax ae debuff${sep}\a-w${Target.Buff[${madeBuff.Find[stAE${_lideb.Value}].Value}].ID}\ax
      DEBUG \ar....\ax timer set${sep}\a-w${timer_${_lideb.Value.Lower}_ID${_tmpID}}\ax
      DEBUG \ar....\ax in list${sep}\a-w${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]}\ax
      | /if ((${Target.${_tmpTLO}.ID} || ${Target.Buff[${madeBuff.Find[st${_lideb.Value}].Value}].ID} || ${Target.Buff[${madeBuff.Find[stAE${_lideb.Value}].Value}].ID}) && (!${timer_${_lideb.Value.Lower}_ID${_tmpID}} || !${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]})) {

      | our target is debuffed for this cycle. make sure it is marked and move on to next check
      /if (${Target.${_tmpTLO}.ID} || ${Target.Buff[${madeBuff.Find[st${_lideb.Value}].Value}].ID} || ${Target.Buff[${madeBuff.Find[stAE${_lideb.Value}].Value}].ID}) {

        | in the list
        /if (!${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]}) {
          DEBUG ${sep}adding ID:\a-r${_tmpID}\ax to \a-wlsdeBuff${_lideb.Value}\ax
          /invoke ${lsdeBuff${_lideb.Value}.Append[${_tmpID}]}
        }

        | timer set?
        /if (${Target.${_tmpTLO}.Duration.TotalSeconds}) {
          DEBUG ${sep}adjusting${sep}\a-wtimer_${_lideb.Value.Lower}_ID${_tmpID}\ax
          /invoke ${set_timer[${_debug}, timer_${_lideb.Value.Lower}_ID${_tmpID}, ${Math.Calc[${Target.${_tmpTLO}.Duration.TotalSeconds}-6]}]}
        }

        | it is now marked and in the debuf list
        /invoke ${_lideb.Advance}
        /continue
      }

      :ignorecrapslow
      | if we've made it this far, cause shits not dealt with
      /if (!${timer_${_lideb.Value.Lower}_ID${_tmpID}} || !${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]}) {
        DEBUG ${sep}\arNO\ax timer or not in list${sep}\a-wlsdeBuff${_lideb.Value}\ax
        /if (${Bool[${madeBuff.Find[stAE${_lideb.Value}].Value}]} && AE#) {
          /varset _mobCount ${SpawnCount[npc loc ${Spawn[id ${_tmpID}].X} ${Spawn[id ${_tmpID}].Y} radius ${Spell[${madeBuff.Find[stAE${_lideb.Value}].Value}].AERange}]}
        } else {
          /varset _mobCount 0
        }
        DEBUG ${sep}_mobCount${sep}\a-w${_mobCount}\ax

        | /if (${is_ready[${_debug}, "${madeBuff.Find[st${_lideb.Value}].Value}"]}) {
        /if (!AE# || (AE# && ${_mobCount} < 2 && ${is_ready[${_debug}, "${madeBuff.Find[st${_lideb.Value}].Value}"]}) || (AE# && !${is_ready[${_debug}, "${madeBuff.Find[stAE${_lideb.Value}].Value}"]})) {
          DEBUG ${sep}sw${_lideb.Value}${sep}\a-wSINGLE\ax

          | cast the damn thing already
          /call voodoo ${_debug} "${madeBuff.Find[st${_lideb.Value}].Value}" ${_tmpID} FALSE

          | sort our returns and act
          /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
            /if (!${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]}) {
              /invoke ${lsdeBuff${_lideb.Value}.Append[${_tmpID}]}
            }
            /invoke ${set_timer[${_debug}, timer_${_lideb.Value.Lower}_ID${_tmpID}, ${Spell[${madeBuff.Find[st${_lideb.Value}].Value}].Duration.TotalSeconds}s]}

          | fizzle, we try again shortly
          } else /if (${Select[${maData.Find[stCastReturn].Value},RESISTED,NOTREADY,FIZZLE,DISTRACTED]}) {
            /invoke ${set_timer[FALSE, timer_${_lideb.Value.Lower}_ID${_tmpID}, 6s]}

          | immune, mark the mob as no touch. (long timer)
          } else /if (${Select[${maData.Find[stCastReturn].Value},IMMUNE,TAKEHOLD]}) {
            /if (!${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]}) {
              /invoke ${lsdeBuff${_lideb.Value}.Append[${_tmpID}]}
            }
            /invoke ${set_timer[${_debug}, timer_${_lideb.Value.Lower}_ID${_tmpID}, 10m]}
          }

          /while (${Me.SpellInCooldown}) {
            CHECKEXIT
            ISMEDEAD
            /if (!${Me.SpellInCooldown}) /break
            /delay 5
          }
        }

        /if (AE# && ${_mobCount} >= AE#) {
          /if (${is_ready[${_debug}, "${madeBuff.Find[stAE${_lideb.Value}].Value}"]}) {
            DEBUG ${sep}sw${_lideb.Value}${sep}\a-wMULTI\ax
            /call voodoo ${_debug} "${madeBuff.Find[stAE${_lideb.Value}].Value}" ${_tmpID} FALSE
            /if (${maData.Find[stCastReturn].Value.Equal[SUCCESS]}) {
              /if (!${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]}) {
                /invoke ${lsdeBuff${_lideb.Value}.Append[${_tmpID}]}
              }
              /invoke ${set_timer[${_debug}, timer_${_lideb.Value.Lower}_ID${_tmpID}, ${Spell[${madeBuff.Find[stAE${_lideb.Value}].Value}].Duration.TotalSeconds}s]}
              /for _count 1 to ${_mobCount}
                /varset _tmpMobID ${Spawn[${_tmpID}].NearestSpawn[${_count} ,npc radius ${Spell[${madeBuff.Find[stAE${_lideb.Value}].Value}].AERange}].ID}
                /if (${_tmpMobID} == 0) /break
                /if (!${lsdeBuff${_lideb.Value}.Contains[${_tmpMobID}]}) {
                  /invoke ${lsdeBuff${_lideb.Value}.Append[${_tmpMobID}]}
                }
                /invoke ${set_timer[${_debug}, timer_${_lideb.Value.Lower}_ID${_tmpMobID}, ${madeBuff.Find[stTimerSnare].Value}]}
              /next _count
            } else /if (${Select[${maData.Find[stCastReturn].Value},RESISTED,NOTREADY,FIZZLE,DISTRACTED]}) {
              /invoke ${set_timer[FALSE, timer_${_lideb.Value.Lower}_ID${_tmpID}, 6s]}
            } else /if (${Select[${maData.Find[stCastReturn].Value},IMMUNE,TAKEHOLD]}) {
              /if (!${lsdeBuff${_lideb.Value}.Contains[${_tmpID}]}) {
                /invoke ${lsdeBuff${_lideb.Value}.Append[${_tmpID}]}
              }
              /invoke ${set_timer[${_debug}, timer_${_lideb.Value.Lower}_ID${_tmpID}, 10m]}
            }

            /while (${Me.SpellInCooldown}) {
              CHECKEXIT
              ISMEDEAD
              /if (!${Me.SpellInCooldown}) /break
              /delay 5
            }

          }
        }

      }
    }

    /invoke ${_lideb.Advance}
  }

/return TRUE



|***
 * note: pets all sorts of fucked up again..
 * use: /call check_minion_cycle DEBUG <FORCE T/F>
 ***|
sub check_minion_cycle(bool _debug, bool _force)
  DEBUG \atcheck_minion_cycle\ax(\a-w${_force}\ax)

  /if (!PET) {
    /invoke ${out[0, 0, 41]}
    /return FALSE
  }

  /if (AUTO && !${_force}) {
    DEBUG ${sep}minion cycle${sep}\a-wminion cycle, not forced\ax
    /invoke ${set_data_timer[${_debug}, Check_Minion_Cycle, RESTART]}

  | reset timers to force shit
  } else /if (${_force}) {
    DEBUG ${sep}minion cycle${sep}\arFORCED\ax
    /declare _buffcycletimestart int local ${MacroQuest.Running}
    /invoke ${set_data_timer[${_debug}, Check_Minion_Buff, 0]}
    /invoke ${set_data_timer[${_debug}, Check_Minion_Shrink, 0]}
    /invoke ${out[0, 0, 42]}
  }

  | summon minion
  DEBUG ${sep}minion cycle${sep}\a-wsummon minion\ax (\a-r\ax)
  /if (!${Pet.ID}) {
    /call check_minion_cycle_summon ${_debug}
  }

  | buffs
  DEBUG ${sep}minion cycle${sep}\a-wbuffs\ax (\a-r${timer_Check_Minion_Buff}\ax)
  /if (${maMinion.Find[swPetBuff].Value} && (!${timer_Check_Minion_Buff} || ${_force})) {
    /call check_minion_cycle_buff ${_debug}
  }

  | illusions
  DEBUG ${sep}minion cycle${sep}\a-willusions\ax
  /if (${Pet.ID} && ${Bool[${maMinion.Find[stPetIllusion].Value}]}) {
    /call check_minion_cycle_Illusion ${_debug}
  }

  | get weapons?
  DEBUG ${sep}minion cycle${sep}\a-wget weapons?\ax
  /if (${maMinion.Find[swUsePetGear].Value}) {
    /call check_minion_cycle_weapons ${_debug}
  }

  | shrink
  DEBUG ${sep}minion cycle${sep}\a-wshrink\ax (\a-r${timer_Check_Minion_Shrink}\ax)
  /if (${maMinion.Find[swPetShrink].Value} && (!${timer_Check_Minion_Shrink} || ${_force})) {
    /call check_minion_cycle_shrink ${_debug}
  }

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Minion_Cycle, RESTART]}
  }

  /if (${_force}) {
    /if (${maMinion.Find[swPet].Value}) {
      /invoke ${out[0, "Minion Cycle${sep}\a-wCompleted\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_buffcycletimestart}) / 10) / 60]}s\ax"]}
    }
  }

/return TRUE



|***
 * note: minime??
 * use: /call check_minion_cycle_shrink DEBUG
 ***|
sub check_minion_cycle_shrink(bool _debug)
  DEBUG \atcheck_minion_cycle_shrink\ax()

  ISMEDEAD
  CHECKEXIT check_minion_cycle_shrink
  GETINPUT check_minion_cycle_shrink NA|NA

  /invoke ${set_data_timer[FALSE, Check_Minion_Shrink, RESTART]}

  /if (${Pet.Height} < 2) /return TRUE

  /if (!${is_ready[${_debug}, "${maMinion.Find[stPetShrink].Value}"]}) /return FALSE

  /declare _count int local 0

  | me shrinks
  /while (${Pet.Height} > 2 && ${_count} < 2) {
    CHECKDEAD
    /call voodoo ${_debug} "${maMinion.Find[stPetShrink].Value}" ${Pet.ID} FALSE
    /varcalc _count ${_count}+1
  }

/return TRUE



|***
 * note: buffs your pet
 * use: part of standard minion cycle if enabled
 ***|
sub check_minion_cycle_buff(bool _debug)
  DEBUG \atcheck_minion_cycle_buff\ax()

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Minion_Buff, RESTART]}
  }

  /if (!${Pet.ID}) /return FALSE

  /declare _spell string local FALSE
  /declare _count int local 0

  DEBUG ${sep}Buff${sep}\a-wMinion\ax

  | maybe 50 buffs was a bit much.
  /for _count 1 to 50

    /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
      /continue
    } else /if (!${maBuff.Find[swBuff${_count}].Value}) {
      /continue
    } else /if (!${lsBuff${_count}Tag.Contains[minion]}) {
      /continue
    } else /if (${maBuff.Find[stBuff${_count}].Value.Count[|]} == 3) {
      /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_count}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[3,|]}"
      /invoke ${maData.Add[tmpName,${Macro.Return}]}
    } else {
      /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${Me.DisplayName} "${maData.Find[tmpName].Value}" check_minion_cycle_buff
    /if (!${Macro.Return}) {
      /continue
    }

    DEBUG _count${sep}\a-w${_count}

    | distance check
    /if (${Pet.Distance} > ${Spell[${maData.Find[tmpName].Value}].MyRange}) /continue

    | have the buff?
    /if (${Pet.Buff[${maData.Find[tmpName].Value}]}) /continue

    /call voodoo ${_debug} "${maData.Find[tmpName].Value}" ${Pet.ID} FALSE
    /call cast_cooldown_wait ${_debug}

  /next _count

  /if (${Target.ID}) {
    /squelch /target clear
  }

/return TRUE



|***
 * note: summons your girl friend..
 * use: /call check_minion_cycle_summon DEBUG
 ***|
sub check_minion_cycle_summon(bool _debug)
  DEBUG \atcheck_minion_cycle_summon\ax()

  ISMEDEAD
  CHECKEXIT check_minion_cycle_summon
  GETINPUT check_minion_cycle_summon NA|NA

  /if (PET && AUTO && !${Pet.ID}) {
    /if (${is_ready[${_debug}, "Companion's Suspension"]}) {
      /call voodoo ${_debug} "Companion's Suspension" 0 FALSE
    }
  }

  | component check. cause `yo ho needs an allowance
  /if (PET && !${Pet.ID} && !${Select[${Me.Class.ShortName},BST]}) {
    DEBUG ${sep}checking pet components
    /declare _error bool local FALSE
    /if (${Select[${Me.Class.ShortName},ENC]} && !${FindItemCount[=Tiny Dagger]}) /varset _error TRUE
    /if (${Select[${Me.Class.ShortName},MAG]} && !${FindItemCount[=Malachite]}) /varset _error TRUE
    /if (${Select[${Me.Class.ShortName},NEC,SHD]} && !${FindItemCount[=Bone Chips]}) /varset _error TRUE

    /if (${_error}) {
      DEBUG ${sep}error summon comonent not found${sep}
      /invoke ${out[0, 0, 43]}
      /invoke ${out[0, 0, 44]}
      /invoke ${out[0, 0, 45]}
      /invoke ${set_switch_env[${_debug}, swPet, TRUE, FALSE]}
      /return FALSE
    }
  }

  /declare _minion string local ${get_best_in_pet[${_debug}]}
  /varset _minion ${set_rank[${_debug}, "${_minion}"]}

  | /invoke ${out[0, 0, 46]}
  /call set_spell_to ${_debug} TRUE "${_minion}" ${maEnv.Find[stBuffGem].Value} TRUE
  /call voodoo ${_debug} "${_minion}" 0 FALSE
  /delay 15s !${Me.Casting.ID}
  /pet hold on
  /pet ghold on

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Minion_Buff, 1]}
  }

/return FALSE



|***
 * note: pig stickers for your ho
 * use: /call check_minion_cycle_weapons DEBUG
 ***|
sub check_minion_cycle_weapons(bool _debug)
  DEBUG \atcheck_minion_cycle_weapons\ax()

  | no pets!!
  /if (!PET || !${Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) {
    DEBUG ${break}weapons are off
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT check_minion_cycle_weapons
  GETINPUT check_minion_cycle_weapons NA|NA

  | pet has weapons
  /if ((${Pet.Equipment[primary]} && ${Me.Pet.Equipment[primary]} != 10855) && ${Pet.Equipment[offhand]}) {
    DEBUG ${break}pet has weapons
    /return TRUE
  }

  | if you are not a mage, you should not be here
  /if (${Select[${Me.Class.ShortName},MAG]}) {

    /if (!${FindItemCount[=${get_MAG_folded_pack[${_debug}]}]} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {

      /declare _count int local 0
      /for _count 1 to 50
        /if (!${maBuff.Find[swBuff${_count}].Value}) /continue
        /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) /continue
        /if (!${lsBuff${_count}Tag.Contains[summonweapons]}) /continue
        DEBUG _count${sep}${_count}

        | /if (${FindItem[=${Spell[${maBuff.Find[stBuff${_count}].Value}].Base[1]}].ID}) /return TRUE

        /call cast_cooldown_wait ${_debug}

        /call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Me.ID} FALSE
        /delay 10s !${Me.Casting.ID}

        /if (${Cursor.Name.Equal[${FindItem[${Spell[${maBuff.Find[stBuff${_count}].Value}].Base[1]}].Name}]}) {
          /autoinventory
          /break
        }

      /next _count
    }

  } else /if (!${Select[${Me.Class.ShortName},MAG]}) {
    /if (!${FindItemCount[=${get_MAG_folded_pack[${_debug}]}]} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {
      /call Bind_command_beg ${maMinion.Find[stBegAlias].Value}
      /return TRUE
    }
  }

  /if (${Pet.ID}) {
    /call get_minion_summon_weapons ${_debug}
  }

/return TRUE



|***
 * note: lipstick on a pig is still a fucking pig
 * use: /call check_minion_cycle_Illusion DEBUG
 ***|
sub check_minion_cycle_Illusion(bool _debug)
  DEBUG \atcheck_minion_cycle_Illusion\ax()

  ISMEDEAD
  CHECKEXIT check_minion_cycle_Illusion
  GETINPUT check_minion_cycle_Illusion NA|NA

  | pet has illusion
  /if (${Pet.Buff[${Spell[${FindItem[${maMinion.Find[stPetIllusion].Value}].Clicky.SpellID}].Name}]}) {
    /return TRUE
  }

  | chekck for pet illusion
  /if (${is_ready[${_debug}, "${maMinion.Find[stPetIllusion].Value}"]}) {
    /call voodoo ${_debug} "${maMinion.Find[stPetIllusion].Value}" ${Pet.ID} FALSE
  }

/return TRUE



|***
 * note: returns a non container inventory slot ##, 0 on full bag inventory, you can pass an optional name to find an item in a top slot
 * use: ${get_openPackSlot[DEBUG, [itemname]]}
 ***|
sub get_openPackSlot(bool _debug, string _orFind)
  DEBUG \atget_openPackSlot\ax(\a-w${_orFind}\ax)

  | we are going to look for any passed item first. if we find it, return that bag slot ID
  | if we do not find it, we are going to look for an open slot
  | if we cant find that. then look for a slot with just a single item in it.

  /declare _count int local 0

  /declare _start int local 24
  /declare _stop int local 33

  | do we see the item
  /if (${Bool[${_orFind}]}) {
    /for _count ${_start} to ${_stop}
      /if (${InvSlot[${_count}].Item.Name.Equal[${_orFind}]}) {
        /return ${_count}
      }
    /next _count
  }

  /for _count ${_start} to ${_stop}
    /if (${InvSlot[${_count}].Item.Name.Equal[NULL]}) {
      /return ${_count}
    }
  /next _count


  /for _count ${_start} to ${_stop}
    /if (${InvSlot[${_count}].Item.Container} == 0) {
      /return ${_count}
    }
  /next _count

/return 0



|***
 * note: give weapons to pet
 * use: /call give_to_pet "ITEMNAME"
 ***|
sub give_to_pet(string _item, bool _debug)
  DEBUG \atgive_to_pet\ax(\a-w${_item}\ax)

  /invoke ${target[set, ${Pet.ID}]}

  ISMEDEAD
  GETINPUT give_to_pet NA|NA
  /if (${Cursor.ID} && ${Me.Pet.ID}) {
    /if (${Cursor.Name.Equal[${_item}]}) {
      /click left target
      /delay 5s !${Bool[${Cursor.ID}]}
    } else {
      /call check_cursor ${_debug} clear
    }
  }

/return TRUE



|***
 * note: equip pet weapons
 * use: /call get_minion_summon_weapons DEBUG
 ***|
sub get_minion_summon_weapons(bool _debug)
| /varset _debug TRUE
  DEBUG \atget_minion_summon_weapons\ax()

  /if (!PET || !${Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) /return FALSE

  /if (!${Me.FreeInventory}) {
    /invoke ${out[0, 0, 34]}
    /return FALSE
  }

  CHECKTIE
  ISMEDEAD
  GETINPUT get_minion_summon_weapons NA|NA

  /declare _count int local 0
  /declare _openPackSlot int local 0

  | get everything out of your hands, including your dick
  /call check_cursor ${_debug} clear

  | we have the weapons we can rebuild her better then before
  /if (${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]} > 1) /goto :havepetweapons
  :havepouchandnoweapons

  | need a non bag inventory slot or ur fuckled!
  /varset _openPackSlot ${get_openPackSlot[${_debug}, "Pouch of Quellious"]}
  DEBUG _openPackSlot ${_openPackSlot}
  /if (!${_openPackSlot} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {
    /invoke ${out[0, 0, 35]}
    /invoke ${out[0, 0, 36]}
    /return FALSE
  }

  | still have an old bag. it needs to be gone
  /if (${FindItemCount[=Pouch of Quellious]} && ${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]} < 2) {
    /call item_delete FALSE "Pouch of Quellious"
  }

  | how much for a dime?
  /if (!${FindItemCount[=${get_MAG_folded_pack[]}]} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {
    DEBUG ${break}no bag or weapons found. noting to get for pet
    /return FALSE
  }

  | have bag/not opened
  /call check_cursor ${_debug} clear

  /if (${FindItemCount[=${get_MAG_folded_pack[]}]}) {
    DEBUG ${sep} :havemageclosedbag
    /if (${Select[${FindItem[=${get_MAG_folded_pack[]}].InvSlot},23,24,25,26,27,28,29,30,31,32]} != 0) {
      DEBUG ${sep}\agSWAP\ax ${get_MAG_folded_pack[]} to slot \aw${_openPackSlot}\ax
      /call Bind_command_swap "${get_MAG_folded_pack[]}" ${_openPackSlot} ${_debug}
    }

    /call check_cursor ${_debug} clear

    | convert the folded pack
    /if (${is_ready[${_debug}, "${get_MAG_folded_pack[]}"]}) {
      /call voodoo ${_debug} "${get_MAG_folded_pack[]}" 0 FALSE
    }

    /call check_cursor ${_debug} clear

  }

  /call check_cursor ${_debug} getoffthedamncursor

  | have 2 blades
  :havepetweapons
  DEBUG ${sep} :havepetweapons
  /for _count 1 to 2
    /call check_cursor ${_debug} getoffthedamncursor
    /delay 5
    DEBUG \agWEAPON\ax\aw${_count}\ax Pack${Int[${Math.Calc[${FindItem[Pouch of Quellious].ItemSlot}-22]}]} slot-${Int[${Math.Calc[${FindItem[=${maMinion.Find[stPetWeapon].Value}].ItemSlot2}+1]}]}
    /nomodkey /itemnotify in Pack${Int[${Math.Calc[${FindItem[Pouch of Quellious].ItemSlot}-22]}]} ${Int[${Math.Calc[${FindItem[=${maMinion.Find[stPetWeapon].Value}].ItemSlot2}+1]}]} leftmouseup

    /delay 3s ${Cursor.ID}
    /delay 5
    /if (${Cursor.Name.NotEqual[${maMinion.Find[stPetWeapon].Value}]}) /goto :havepetweapons
    /call give_to_pet "${Cursor.Name}"
    /delay 3s !${Cursor.ID}
    /if (${Select[${Me.Class.ShortName},DRU]}) /break
  /next _count

  | close the give to window
  /if (${Window[GiveWnd].Open}) {
    DEBUG close \atGiveWnd\ax window
    /nomodkey /notify GiveWnd GVW_Give_Button leftmouseup
    /delay 5
    /invoke ${Window[GiveWnd].DoClose}
  }

  /call check_cursor ${_debug} clear

  :havepouchandnoweapons
  DEBUG ${sep} :havepouchandnoweapons
  /if (${FindItemCount[=Pouch of Quellious]} && ${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]} < 2) {
    /nomodkey /itemnotify ${FindItem[Pouch of Quellious].InvSlot} leftmouseup
    /delay 1
    /call item_delete FALSE "Pouch of Quellious"
  }

  /if (${Cursor.ID}) /call check_cursor ${_debug} clear

  /if (${Window[InventoryWindow].Open}) {
    /invoke ${Window[InventoryWindow].DoClose}
  }

/return



|***
 * note: deletes an item..
 * use: /call item_delete DEBUG ["ITEM NAME"]
 ***|
sub item_delete(bool _debug, string _item)
  DEBUG \atitem_delete\ax(\a-w${_item}\ax)

  /if (!${FindItem[=${_item}].ID}) /return FALSE

  /declare _swapslot string local ${item_find[FALSE, "${_item}"]}

  | get item to move on your cursor
  /if (${_swapslot.Arg[1,|].Equal[INBAG]}) {
    DEBUG ${sep}GRAB ITEM TO DELETE FROM BAG pack${_swapslot.Arg[2,|]} ${_swapslot.Arg[3,|]} leftmouseup
    /squelch /nomodkey /shiftkey /itemnotify in pack${_swapslot.Arg[2,|]} ${_swapslot.Arg[3,|]} leftmouseup
  } else /if (${_swapslot.Arg[1,|].Equal[TOPSLOT]}) {
    DEBUG ${sep}GRAB ITEM TO DELETE FROM TOPSLOT ${_swapslot.Arg[2,|]} leftmouseup
    /squelch /nomodkey /shiftkey /itemnotify ${_swapslot.Arg[2,|]} leftmouseup
  }

  /delay 1s ${Cursor.Name.Equal[${_item}]}
  /if (${Cursor.Name.Equal[${_item}]}) {
    /invoke ${out[14, "Destroying ${sep} ${tar}${_item}\ax"]}
    /destroy
  }

/return



|***
 * note: will find something returning its location
 * use: ${item_find[DEBUG, "ITEM NAME"]}
 ***|
sub item_find(bool _debug, string _item)
  DEBUG \atitem_find\ax(\a-w${_item}\ax)

  /if (!${FindItem[=${_item}].ID}) /return FALSE

  /declare _pack int local 0
  /declare _slot int local 0

  | in a bag
  /if (${InvSlot[pack${Select[${FindItem[=${_item}].ItemSlot},23,24,25,26,27,28,29,30,31,32]}].Item.Container}) {
    /varset _pack ${Int[${Math.Calc[${FindItem[=${_item}].ItemSlot}-22]}]}
    /varset _slot ${Int[${Math.Calc[${FindItem[=${_item}].ItemSlot2}+1]}]}
    DEBUG ${sep}ITEM FOUND INBAG \a-w|INBAG|${_pack}|${_slot}\ax
    /return |INBAG|${_pack}|${_slot}

  | in a top slot
  } else /if (${FindItem[=${_item}].InvSlot}) {
    /varset _slot ${FindItem[=${_item}].InvSlot}
    DEBUG ${sep}ITEM FOUND TOPSLOT \a-w|TOPSLOT|${_slot}\ax
    /return |TOPSLOT|${_slot}

  }

/return FALSE



|***
 * note: finds a folded mage weapon pack in your inventory. returns its name
 * use: ${get_MAG_folded_pack[DEBUG]}
 ***|
sub get_MAG_folded_pack(bool _debug)
  DEBUG \atget_MAG_folded_pack\ax()

  /declare _lsout list local
  /invoke ${_lsout.Append[Wirn's,Thassis',Frightforged,Manaforged,Spectral]}
  /declare _ligmfp listiterator local
  /vardata _ligmfp _lsout.First.Clone
  /while (!${_ligmfp.IsEnd}) {
    CHECKDEAD
    /if (${FindItemCount[Folded Pack of ${_ligmfp.Value} Armaments]}) {
      DEBUG ${sep}found mage pack${sep}\a-wFolded Pack of ${_ligmfp.Value} Armaments\ax
      /return Folded Pack of ${_ligmfp.Value} Armaments
    }
    /invoke ${_ligmfp.Advance}
  }

  DEBUG ${sep}no mage packs found

/return 0



|***
 * note: active Shield Flash when HP reach a specific % or a number of mobs in immediate range
 * use: /call cast_shield_flash DEBUG [aa|disc]
 ***|
sub cast_shield_flash(bool _debug, string _type)
  DEBUG \atcast_shield_flash\ax(\a-w${_type}\ax)

  /if (!AUTO) {
    /return FALSE
  }

  | health low enough || enough mobs in range
  /if (${Me.PctHPs} > ${maSash.Find[stPctShieldFlash].Value} && ${SpawnCount[npc radius SAFEENV PS4 targetable]} < ${maSash.Find[stCountShieldFlash].Value}) {
    /return FALSE
  }

  | is the skill available
  /if (${_type.Equal[aa]}) {
    /if (!${is_ready[${_debug}, "Shield Flash"]}) {
      /return FALSE
    }
  } else /if (${_type.Equal[disc]}) {
    /if (!${is_ready[${_debug}, "${maSash.Find[stFlash].Value}"]}) {
      /return FALSE
    }
  }

  CHECKEXIT

  | mark current bandolier and swap to flash
  /declare _Bandolier string local FALSE

  | take note of current Bandolier
  /if (!${Me.Bandolier[${maSash.Find[stSashShieldFlash].Value}].Active}) {
    /declare _count int local 0

    /for _count 1 to 12
      CHECKDEAD
      /if (${Me.Bandolier[${_count}].Active}) {
        /varset _Bandolier ${Me.Bandolier[${_count}]}
        /break
      }
    /next _count
  }

  | pally / shadowknight
  /if (${_type.Equal[aa]}) {
    /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
    /invoke ${Me.Bandolier[${maSash.Find[stSashShieldFlash].Value}].Activate}
    /call voodoo ${_debug} "Shield Flash" 0 FALSE

  | warior
  } else /if (${_type.Equal[disc]}) {
    /call check_facing ${maDebug.Find[face].Value} TARGET FORCE
    /invoke ${Me.Bandolier[${maSash.Find[stSashShieldFlash].Value}].Activate}
    /call voodoo ${_debug} "${maChr.Find[stFlash].Value}" 0 FALSE
  }

  | reset to previous bandolier
  /if (${Me.Bandolier[${maSash.Find[stSashShieldFlash].Value}].Name.NotEqual[${_Bandolier}]}) {
    /docommand /timed 70 /bandolier Activate ${_Bandolier}
  }

/return TRUE



|***
 * note: swaps bandolier
 * use: /call set_bandolier DEBUG [1h|2h|range|no|avatar]
 ***|
sub set_bandolier(bool _debug)
  DEBUG \atset_bandolier\ax()

  /if (${bandolierSwap.Equal[${bandolier${_band}}]}) /return TRUE
  /if (!${maCC.Find[swBandolier].Value}) /return FALSE
  /if (!${Bool[${bandolierNormal}]}) /return FALSE
  /if (!${Bool[${bandolier${_band}}]}) /return FALSE
  /delay 1
  /bandolier Activate ${bandolier${_band}}
  /varset bandolierSwap ${bandolier${_band}}

/return TRUE



|***
 * note: will use a glyph and buy it if you have the aa
 * use: /call cast_glyph DEBUG
 ***|
sub cast_glyph(bool _debug)
  DEBUG \atcast_glyph\ax()

  /if (${maCC.Find[stGlyph].Value.Equal[FALSE]}) /return FALSE

  /if (${Raid.Members}) {
    /if (!${Bool[${maCC.Find[swGlyphRaid].Value}]}) {
      /return FALSE
    }
  } else /if (${Group.Members}) {
    /if (!${Bool[${maCC.Find[swGlyphGroup].Value}]}) {
      /return FALSE
    }
  }

  ISMEDEAD
  GETINPUT cast_glyph NA|NA

  | buy the AA
  /if (!${Me.AltAbility[${maCC.Find[stGlyph].Value}].ID} && ${Me.AAPoints} >= ${AltAbility[${maCC.Find[stGlyph].Value}].Cost} && ${AltAbility[${maCC.Find[stGlyph].Value}].CanTrain}) {
    DEBUG${sep}Buying${sep}\a-w${maCC.Find[stGlyph].Value}\ax
    /alt buy ${AltAbility[${maCC.Find[stGlyph].Value}].ID}
  }

  /if (${is_ready[${_debug}, "${maCC.Find[stGlyph].Value}"]}) {
    /call voodoo ${_debug} "${maCC.Find[stGlyph].Value}" 0 FALSE
  }

  | buy it again if you can
  /if (!${Me.AltAbility[${maCC.Find[stGlyph].Value}].ID} && ${Me.AAPoints} >= ${AltAbility[${maCC.Find[stGlyph].Value}].Cost} && ${AltAbility[${maCC.Find[stGlyph].Value}].CanTrain}) {
    DEBUG${sep}Buying${sep}\a-w${maCC.Find[stGlyph].Value}\ax (if we used)
    /alt buy ${AltAbility[${maCC.Find[stGlyph].Value}].ID}
  }

/return TRUE



|***
 * note: uses the aa Spire you have set
 * use: /call cast_AA_spire DEBUG
 ***|
sub cast_AA_spire(bool _debug)
  DEBUG \atcast_AA_spire\aw()

  ISMEDEAD
  CHECKEXIT cast_AA_spire
  GETINPUT cast_AA_spire NA|NA

  /if (${Raid.Members} && ${maChr.Find[swSpireRaid].Value}) {
    DEBUG spire${sep}Raid
    /if (!${Me.Buff[${maData.Find[stSpire].Value}].ID} && ${Spell[${maData.Find[stSpire].Value}].Stacks}) {
      /if (${is_ready[${_debug}, "${maData.Find[stSpire].Value}"]}) {
        /call voodoo ${_debug} "${maData.Find[stSpire].Value}" 0 FALSE
        /return TRUE
      }
    }
  } else /if (!${Raid.Members} && ${maChr.Find[swSpireGroup].Value}) {
    DEBUG spire${sep}Group
    /if (!${Me.Buff[${maData.Find[stSpire].Value}].ID} && ${Spell[${maData.Find[stSpire].Value}].Stacks}) {
      /if (${is_ready[${_debug}, "${maData.Find[stSpire].Value}"]}) {
        /call voodoo ${_debug} "${maData.Find[stSpire].Value}" 0 FALSE
        /return TRUE
      }
    }
  }

/return FALSE



|***
 * note: uses the aa Intensity of the Resolute
 * use: /call cast_AA_intensity DEBUG
 ***|
sub cast_AA_intensity(bool _debug)
  DEBUG \atcast_AA_intensity\aw()

  ISMEDEAD
  CHECKEXIT cast_AA_intensity
  GETINPUT cast_AA_intensity NA|NA

  /if (${Raid.Members} && ${maChr.Find[swSpireRaid].Value}) {
    DEBUG intensity${sep}\-wRaid\ax
    /if (!${Me.Song[${maChr.Find[stSpire].Value}].ID} && ${Spell[${maChr.Find[stAAIntensity].Value}].Stacks}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAIntensity].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAIntensity].Value}" 0 FALSE
        /return TRUE
      }
    }
  } else /if (!${Raid.Members} && ${maChr.Find[swSpireGroup].Value}) {
    DEBUG intensity${sep}\a-wGroup\ax
    /if (!${Me.Song[${maChr.Find[stAAIntensity].Value}].ID} && ${Spell[${maChr.Find[stAAIntensity].Value}].Stacks}) {
      /if (${is_ready[${_debug}, "${maChr.Find[stAAIntensity].Value}"]}) {
        /call voodoo ${_debug} "${maChr.Find[stAAIntensity].Value}" 0 FALSE
        /return TRUE
      }
    }
  }

/return FALSE



|***
 * note: set hud varaibles
 * use: not for your use
 ***|
sub set_hud_data(string _mode)

  /varset h_build ${maEnv.Find[build].Value}

  /if (${Bool[${_mode}]}) {
    /varset h_mode ${_mode}
  } else {
    /varset h_mode
  }

  /varset h_time          ${maData.Find[swHUDTime].Value}

  | engages
  /varset h_engage        ${maCC.Find[stPctEngage].Value}
  /varset h_engagepet     ${maMinion.Find[stPctPetEngage].Value}
  /varset h_engageswarm   ${maMinion.Find[stPctSwarmEngage].Value}
  /varset h_swpet         ${maMinion.Find[swPet].Value}
  /varset h_swswarm       ${maMinion.Find[swSwarm].Value}

  | cc modes
  /varset h_auto          ${maEnv.Find[swAuto].Value}
  /varset h_melee         ${maCC.Find[swCombatMelee].Value}
  /varset h_range         ${maCC.Find[swCombatRange].Value}
  /varset h_nuke          ${maCC.Find[swCombatNuke].Value}
  /varset h_dot           ${maCC.Find[swCombatDoT].Value}
  /varset h_heal          ${maCC.Find[swCombatHeal].Value}
  /varset h_control       ${maCC.Find[swCombatControl].Value}
  /varset h_env           ${maEnv.Find[stEnvRadius].Value}

  | healing
  | /varset h_healmode      ${maHeal.Find[stHealMode].Value}
  /varset h_override      ${maHeal.Find[stHealAdjust].Value}
  /varset h_self          ${maHeal.Find[swHealSelf].Value}
  /varset h_group         ${maHeal.Find[swHealGroup].Value}
  /varset h_xt            ${maHeal.Find[swHealXTarget].Value}
  /varset h_pet           ${maHeal.Find[swHealPet].Value}
  /varset h_cure          ${maHeal.Find[swHealCure].Value}
  /varset h_tot           ${maHeal.Find[swHealToT].Value}
  /varset h_dan           ${maHeal.Find[swHealDanNet].Value}

  | buff
  /varset h_bself         ${maBuff.Find[swBuffSelf].Value}
  /varset h_bcrew         ${maBuff.Find[swBuffCrew].Value}
  /varset h_braid         ${maBuff.Find[swBuffRaid].Value}
  /varset h_bbeg          ${maBuff.Find[swBuffBeg].Value}
  /varset h_bunity        ${maBuff.Find[swBuffUnity].Value}
  /varset h_baura         ${maBuff.Find[swBuffAura].Value}
  /varset h_btemp         ${maBuff.Find[swBuffTemp].Value}
  /varset h_bnow          ${maBuff.Find[swBuffNow].Value}
  /varset h_bmaster       ${maBuff.Find[swBuffMaster].Value}
  /varset h_bpet          ${maMinion.Find[swPetBuff].Value}
  /varset h_bgroup        ${maBuff.Find[swBuffGroup].Value}

  | pull / agro
  /varset h_agro          ${maAgro.Find[swAgro].Value}
  /varset h_pull          ${maPull.Find[swPull].Value}
  /varset h_pullarea      ${maPull.Find[stPullRadius].Value}
  /varset h_pullmode      ${maPull.Find[stPullMode].Value}

  | tie
  /varset h_tie           ${maTie.Find[swSetTie].Value}
  /varset h_tietoon       ${maTie.Find[stTieToon].Value}
  /varset h_tiemode       ${maTie.Find[stTieMode].Value}

  | burn
  /varset h_buauto        ${maBurn.Find[swBurnAuto].Value}
  /varset h_bupct         ${maBurn.Find[stPctBurnEngage].Value}
  /varset h_bucount       ${maBurn.Find[stBurnCount].Value}
  /varset h_buraid        ${maBurn.Find[swBurnRaid].Value}

  | rest
  /varset h_rpct          ${maEnv.Find[stPctRest].Value}
  /varset h_rfull         ${maEnv.Find[swRestFull].Value}
  /varset h_rrod          ${maEnv.Find[swRestModRod].Value}
  /varset h_rbuff         ${maEnv.Find[swRestBuff].Value}
  /varset h_rcombat       ${maEnv.Find[swRestCombat].Value}

  | rez
  /varset h_zooc          ${maRez.Find[swRezOOC].Value}
  /varset h_ztake         ${maRez.Find[swRezTake].Value}
  /varset h_zic           ${maRez.Find[swRezIC].Value}
  /varset h_zeveryone     ${maRez.Find[swRezEveryone].Value}
  /varset h_ztoken        ${maRez.Find[swRezToken].Value}
  /varset h_zdan          ${maRez.Find[swRezDanNet].Value}
  /varset h_zrad          ${maRez.Find[stMaxRezRange].Value}

  | character build
  /varset h_eset          ${maEnv.Find[stBuildSetCurrent].Value}

  | build dev variables
  /varset h_dcombat         ${maCC.Find[swSetCombat].Value}
  /varset h_dburn           ${maBurn.Find[swBurnConditionMet].Value}
  /varset h_denv            ${maEnv.Find[stEnvRadius].Value}
  /varset h_dsafe           ${maEnv.Find[stEnvSafeRadius].Value}
  /varset h_dpull           ${maPull.Find[stPullRadius].Value}
  /varset h_dcaststatus     ${maData.Find[stCastReturn].Value}


/return TRUE



|***
 * note: passive toggle AA Agro switches
 * use: auto based on /cc agro switch
 ***|
sub set_AA_agro(bool _debug)
  DEBUG \atset_AA_agro\ax()

  ISMEDEAD
  GETINPUT set_AA_agro NA|NA

  | turn agro ON
  /if (AGRO) {

    | adjust bold attacks AA
    /if (${Me.AltAbilityReady[Bold Attacks: Disabled]}) {
      /alt act ${Me.AltAbility[Bold Attacks: Disabled].ID}
      /invoke ${out[2, 0, 47]}
    }

    | SHD Soul Flay
    /if (${Select[${Me.Class.ShortName},SHD]}) {
      /if (${Me.AltAbilityReady[Soul Flay: Disabled]}) {
        /alt act ${Me.AltAbility[Soul Flay: Disabled].ID}
        /invoke ${out[2, 0, 48]}
      }

    | BST Combat Subtlety
    } else /if (${Select[${Me.Class.ShortName},BST]}) {
      /if (${Me.AltAbilityReady[Combat Subtlety: Enabled]}) {
        /alt act ${Me.AltAbility[Combat Subtlety: Enabled].ID}
        /invoke ${out[2, 0, 50]}
      }

    | ranger ferocious kicks
    } else /if (${Select[${Me.Class.ShortName},RNG]}) {
      /if (${Me.AltAbilityReady[Ferocious Kick Subtlety: Enabled]}) {
        /alt act ${Me.AltAbility[Ferocious Kick Subtlety: Enabled].ID}
        /invoke ${out[2, 0, 49]}
      }
    }

  | turn agro OFF
  } else /if (!AGRO) {

    | adjust bold attacks AA
    /if (${Me.AltAbilityReady[Bold Attacks: Enabled]}) {
      /alt act ${Me.AltAbility[Bold Attacks: Enabled].ID}
      /invoke ${out[2, 0, 51]}
    }

    | SHD Soul Flay
    /if (${Select[${Me.Class.ShortName},SHD]}) {
      /if (${Me.AltAbilityReady[Soul Flay: Enabled]}) {
        /alt act ${Me.AltAbility[Soul Flay: Enabled].ID}
        /invoke ${out[2, 0, 52]}
      }

    | BST Combat Subtlety
    } else /if (${Select[${Me.Class.ShortName},BST]}) {
      /if (${Me.AltAbilityReady[Combat Subtlety: Disabled]}) {
        /alt act ${Me.AltAbility[Combat Subtlety: Disabled].ID}
        /invoke ${out[2, 0, 53]}
      }

    | ranger ferocious kicks
    } else /if (${Select[${Me.Class.ShortName},RNG]}) {
      /if (${Me.AltAbilityReady[Ferocious Kick Subtlety: Disabled]}) {
        /alt act ${Me.AltAbility[Ferocious Kick Subtlety: Disabled].ID}
        /invoke ${out[2, 0, 54]}
      }
    }

  }

/return TRUE



|***
 * note: pulls NON los mobs to you using a healers [SHM/CLR] intervention spell
 * use: /call get_target_intpull DEBUG [targetID]
 ***|
sub get_target_intpull(bool _debug, int _tmpID)
  DEBUG \atget_target_intpull\ax(\a-w${_tmpID}\ax)

  /if (!AUTO || !PULL) {
    /return FALSE
  }

  ISMEDEAD
  GETINPUT get_target_intpull NA|NA
  CHECKTIE

  /if (${_tmpID}) {
    /varset stAssTarID ${_tmpID}
  } else {
    /call pull_mob_find FALSE
  }

  /if (!TARGET) {
    SETHUD EMPTY
    /return FALSE
  }

  /invoke ${target[set, TARGET]}

  /delay 1s
  /dex ${get_resident[${_debug}, |clr, 60]} /advancedpullint ${Me.ID} ${_debug}
  /delay 1s

  /call pull_mob_prep ${_debug} TARGET
  /if (!${Macro.Return}) {
    DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
    /invoke ${target[clear]}
    /return FALSE
  }

  SETHUD EMPTY

/return TRUE



|***
 * note: moves to an established stAssTarID
 * use: /call pull_mob_move_to DEBUG [TARGETID] [distance to stop at] [Line Of Sight on|off]
 ***|
sub pull_mob_move_to(bool _debug, int _tmpID, int _dist, string _LoS)
  DEBUG \atpull_mob_move_to\ax(\a-wID:${_tmpID}, Dis:${_dist}, LoS:${_LoS}\ax)

  /if (!AUTO && !${swModeToken}) {
    /return FALSE
  }

  /nav ID ${_tmpID} |distance=${_dist} lineofsight=${_LoS} log=${maEntropy.Find[swNavLogSpew].Value}
  /delay 1s ${Navigation.Active}

  DEBUG ${sep}moving to${sep}\a-w${_tmpID}\ax
  /while (${Navigation.Active}) {
    CHECKDEAD

    /if (!AUTO) {
      DEBUG ${sep}/while \a-gmanual\ax
      /invoke ${target[clear]}
      /return FALSE
    }

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

    | are we there yet??? (no los required)
    /if (!${Bool[${_LoS}]}) {
      /if (${Spawn[${_tmpID}].Distance} <= ${_dist} && !${Spawn[${_tmpID}].LineOfSight}) {
        DEBUG ${sep}dis check${sep}\a-w${Spawn[${_tmpID}].Distance} <= ${_dist} && !\arLOS\ax
        /delay 2
        /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
        /return TRUE
      }
      
    | LOS required
    } else /if (${Bool[${_LoS}]}) {
      /if (${Spawn[${_tmpID}].Distance} <= ${_dist} && ${Spawn[${_tmpID}].LineOfSight}) {
        DEBUG ${sep}dis \arHALT\ax${sep}\a-w${Spawn[${_tmpID}].Distance} <= \ag${_dist}\ax && \agLOS\a-w (\a-r${_tmpID}\ax)
        /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
        /return TRUE
      }
    }

    | range from camp check
    /if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} > PULLENV && !${swModeToken}) {
      DEBUG Mob exceeded setMobAGro range
      /varset _tmpID 0
      /return TRUE
    }

    | mobs in our personal space. fuck those things. no respect
    |** /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} & ${NearestSpawn[1, npc radius SAFEENV PS4 targetable].ID} != ${_tmpID}) {
    /if (${SpawnCount[npc radius ENV PS4 targetable]} & ${NearestSpawn[1, npc radius ENV PS4 targetable].ID} != ${_tmpID}) {
      DEBUG ${break}mob in environment space
      /invoke ${out[9, 0, 26]}
      /invoke ${target[clear]}
      /varset _tmpID ${NearestSpawn[1, npc radius ENV PS4 targetable].ID}
      /return TRUE
    }
    **|

    /delay 1
  }

/return TRUE



|***
 * note: uses a pet class to get mobs to camp
 * use: /call get_target_petpull DEBUG [targetID] [adjusted find counter]
 ***|
sub get_target_petpull(bool _debug, int _tmpID, int _spCount)
  DEBUG \atget_target_petpull\ax(\a-w${_tmpID}, ${_spCount}\ax)

  ISMEDEAD
  GETINPUT get_target_petpull _tmpID|${_tmpID}
  CHECKTIE

  | fix the pet
  /if (!${Pet.ID} && PET) {
    /call cast_cooldown_wait ${_debug}
    /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
  }

  | give it treats and hope it behaves
  /if (!${Me.Pet.ReGroup}) /pet regroup
  /if (!${Me.Pet.Hold}) /pet hold on
  /pet back off



  /if (!PULL) {
    /call set_pull_observer ${_debug}
  } else {

    | if we pass an increment adjustment, set it, otherwise, 0
    /if (!${Bool[${_spCount}]}) {
      /varset _spCount 0
    }
    
    | find us a mob
    /call pull_mob_find ${_debug} ${_spCount}
  }

  | if we dont find a mob, restart the cycke
  /if (PULL && !TARGET) {
    DEBUG ${sep}\arNO\ax stAssTarID
    SETHUD EMPTY
    /return FALSE
  } else {
    /if (!${_tmpID}) {
      /varset _tmpID TARGET
    }
  }

  | target the mob
  /invoke ${target[set, ${_tmpID}]}

  | if the target is mezed, we need to look again, increment the spawncount
  /if (${Target.Mezzed.ID}) {
    /invoke ${target[clear]}
    /call pull_mob_find ${_debug} ${Int[${Math.Calc[${_spCount} + 1]}]}
  }

  DEBUG ${sep}target${sep}\ay${_tmpID}\ax${sep}\aw${Spawn[${_tmpID}].DisplayName}\ax ${sep} dist:\a-w${Spawn[${_tmpID}].Distance}\ax

  /if (${Me.Pet.Taunt}) /pet taunt
  /if (${Me.Pet.Stance.NotEqual[follow]}) /pet follow
  /if (!HOME) /call Bind_control_home set FALSE silent ${maDebug.Find[home].Value}

  | check for ammo if needed
  /if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID}) {
    /call check_buff_cycle_ammo ${maDebug.Find[ammo].Value} TRUE
  }

  /invoke ${out[9, "Fetching${sep}${con_color[${_debug}, ${_tmpID}]}\ax (\a-r${Spawn[${_tmpID}].Distance}\ax)"]}

  /if (!${Pet.ID} && PET) {
    /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
  }

  /declare _targetmelee float local ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}

  | get closer if we need too
  /if (${Spawn[${_tmpID}].Distance} > ${maHard.Find[stMaxRadPet].Value}) {
    /call pull_mob_move_to ${_debug} ${_tmpID} ${maPull.Find[stPullNavStopDistance].Value} off
  }
  
  /while (${Me.Moving}) {
    /delay 1
  }

  | target the mob
  /invoke ${target[set, ${_tmpID}]}

  | we set the pet to mob timer based off total distance you have to run to the mob + a buffer good good measure.
  | pet pathing is shit and for longer pulls need the longer wait time
  /declare _waitforstupidpet int local ${Math.Calc[(${Math.Distance[${Spawn[${_tmpID}].Y}, ${Spawn[${_tmpID}].X}, ${Spawn[${_tmpID}].Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} /10) + 10]}
  /invoke ${set_timer[${_debug}, timer_waitforstupidpet, ${_waitforstupidpet}s]}

  /pet attack

  /while (TRUE) {

    CHECKDEAD
    GETINPUT get_target_petpull_range NA|NA

    /if (!${Spawn[id ${_tmpID}].ID} || ${Spawn[id ${_tmpID}].Dead}) {
      /invoke ${out[9, 0, 27]}
      /break
    }

    | once pet has agro get out
    /if (${Target.AggroHolder.Name.Equal[${Me.Pet.Name}]}) {
      /invoke ${out[9, 0, 28]}
      /break
    }

    | break if more then 1 mob is near you or you have agro
    /if (${NearestSpawn[2, npc radius SAFEENV PS4].ID} || ${Me.PctAggro} > 90) {
      /invoke ${out[9, 0, 29]}
      /break
    }
    
    | wait for pet to get to meele range of the mob
    /delay 5 ${Math.Distance[${Me.Pet.Y}, ${Me.Pet.X}, ${Me.Pet.Z} : ${Spawn[${_tmpID}].Y}, ${Spawn[${_tmpID}].X}, ${Spawn[${_tmpID}].Z}]} < ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
    DEBUG ${sep}waiting on pet to get to target${sep}\a-w${Math.Distance[${Me.Pet.Y},${Me.Pet.X}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X}]} range\ax
    /delay 5

    | NO PET?
    /if (!${Me.Pet.ID}) {
      /invoke ${out[9, 0, 30]}
      /break
    }

    /if (${Me.XTHaterCount} && ${Target.AggroHolder.Name.NotEqual[${Me.Pet.Name}]}) {
      /invoke ${out[9, 0, 31]}
      /break
    }

    /if (!${timer_waitforstupidpet}) {
      /invoke ${out[9, 0, 32]}
      /break
    }

  }
    
  /if (!${timer_waitforstupidpet}) {
    /invoke ${target[clear]}
    /call where_the_fuck_is_my_pet ${maDebug.Find[lost].Value} TRUE
    SETHUD EMPTY
    /if (HOME) {
      /call pull_home_move_to ${_debug} ${_tmpID}
    }
    /return FALSE
  }

  /call where_the_fuck_is_my_pet ${maDebug.Find[lost].Value} FALSE

  /if (HOME) {
    /call pull_home_move_to ${_debug} ${_tmpID}
  }

  | wait for mob to get to camp
  /call get_target_wait ${maDebug.Find[wait].Value} ${_tmpID} ${_waitforstupidpet}s
  SETHUD EMPTY

/return



|***
 * note: mechanics for nav pulling
 * note: /call get_target_navpull DEBUG [targetID]
 ***|
sub get_target_navpull(bool _debug, int _tmpID)
  DEBUG \atget_target_navpull\ax(\a-w${_tmpID}\ax)

  ISMEDEAD
  GETINPUT get_target_navpull _tmpID|${_tmpID}
  CHECKTIE

  /declare _count int local 0

  /if (!PULL) {
    /call set_pull_observer ${_debug}
  } else {
    /call pull_mob_find ${_debug}
  }

  /if (PULL && !TARGET) {
    DEBUG ${sep}\arNO\ax${sep}\a-wstAssTarID\ax
    SETHUD EMPTY
    /return FALSE
  } else {
    /if (!${_tmpID}) /varset _tmpID TARGET
  }

  | we have a target, get us within range to pull
  /if (!${Navigation.PathExists[id ${_tmpID}]} && !${maOver.Find[swOverNavCheck].Value}) {
    GETINPUT get_target_navpull_wait _tmpID|${_tmpID}
    /invoke ${out[9, "${notice}${sep}No Nav Path ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
    /varcalc _count ${_count}+1
    /if (${_count} > 10) /return FALSE
    /delay 1s
    /call pull_mob_find ${_debug} ${_count}
  }

  DEBUG ${sep}Target ${sep} ${_tmpID} ${sep} \aw${Spawn[${_tmpID}].DisplayName}\ax

  | force home even if not using it
  /if (${maHome.Find[swinCombat].Value} && ${maPull.Find[swPullSetHome].Value} && !HOME) {
    /call Bind_control_home set FALSE silent ${maDebug.Find[home].Value}
  }

  | set the target
  /invoke ${out[9, "Fetching${sep}${con_color[${_debug}, ${_tmpID}]}\ax"]}
  /invoke ${target[set, ${_tmpID}]}

  | if the mob is kinda close??
  /if (${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < ENV) {
    DEBUG ${sep}navpull${sep}in sight < ENV
    /call pull_mob_prep ${_debug} ${_tmpID}
    /if (!${Macro.Return}) {
      DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
      /invoke ${target[clear]}
      /return FALSE
    }

    /if (MELEE && ${Spawn[${_tmpID}].Distance} > ${Math.Calc[${Spawn[${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /call pull_mob_move_to ${_debug} ${_tmpID} ${maPull.Find[stPullNavStopDistance].Value} on
    }

    /call pull_mob_tag ${_debug} ${_tmpID}
    /if (${Macro.Return.Equal[tagfailure]}) {
      DEBUG ${sep}\arNAVpull FAILURE\ax${sep}resetting/returning to camp
      /invoke ${target[clear]}
      /call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
      /return FALSE
    }

    /delay 2
    /return

  | get closer if we need too
  } else {
    DEBUG ${sep}Sight:${Spawn[${_tmpID}].LineOfSight} ${dot} Distance:${Spawn[${_tmpID}].Distance} ?? ${maPull.Find[stPullNavStopDistance].Value}

    | melee pulls get up the mobs ass
    | all other methods obey the stop distance settings
    /if (${maPull.Find[stPullWith].Value.Equal[melee]}) {
      /call pull_mob_move_to ${_debug} ${_tmpID} ${Math.Calc[${Spawn[${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} on
    } else {
      /call pull_mob_move_to ${_debug} ${_tmpID} ${maPull.Find[stPullNavStopDistance].Value} on
    }

    /while (${Me.Moving}) {
      CHECKDEAD
      /if (${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < ${maPull.Find[stPullNavStopDistance].Value}) {
        /break
      }
      /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
        /break
      }
      /delay 5
    }
    /call pull_mob_prep ${_debug} ${_tmpID}
    /if (!${Macro.Return}) {
      DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
      /invoke ${target[clear]}
      /return FALSE
    }
  }

  /delay 1s !${Me.Moving}
  /call pull_mob_tag ${_debug} ${_tmpID}
  /delay 2

  | failure on tag
  /if (${Macro.Return.Equal[tagfailure]}) {
    DEBUG ${sep}\arNAVpull FAILURE\ax${sep}resetting/returning to camp
    /invoke ${target[clear]}
    /call pull_home_move_to ${_debug} ${Me.ID}    
    /call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
    /return FALSE
  }

  | move home
  
  /if (HOME) {
    /call pull_home_move_to ${_debug} ${_tmpID}
  }
  
  |**  
  /if (${maPull.Find[swPullSetHome].Value} && ${maPull.Find[swPullSetHome].Value} && HOME) {
    /if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} > SAFEENV && PULL) {
      /call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
    }
  }

  /while (${Me.Moving}) {
    GETINPUT get_target_navpull_moving _tmpID|${_tmpID}
    CHECKDEAD
    /delay 5
  }
  **|

  /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FALSE

  /declare _waittime int local ${Math.Calc[(PULLENV + ${Spawn[${_tmpID}].Distance})*.08]}
  /call get_target_wait ${maDebug.Find[wait].Value} ${_tmpID} ${_waittime}s

  SETHUD EMPTY

/return TRUE



|***
 * note: still need to build this
 * use: /call get_target_calmpull DEBUG [targetID]
 ***|
sub get_target_calmpull(bool _debug, int _tmpID)
  DEBUG \atget_target_calmpull\ax(\a-w${_tmpID}\ax)
| find a target

| find out how many are around it.

| lul more then 2 targets

| tag extra

| bring it back to camp

/return



|***
 * note: mechanics for multipull choice and execution
 * use: /call get_target_multipull DEBUG
 ***|
sub get_target_multipull(bool _debug)
  DEBUG \atget_target_multipull\ax()

  /declare _count int local 0
  /declare _multiTimer timer local 30s

  | We need ${Target2Kkill}
  /call pull_mob_find ${_debug}

  /if (!TARGET) {
    DEBUG ${sep}\arNO\ax stAssTarID
    SETHUD EMPTY
    /return FALSE
  }

  | maintain home marker
  /if (!HOME) {
    /call Bind_control_home set NULL NULL ${maDebug.Find[home].Value}
  }

  | how the hell do we want to kill it.?

  | pull with a normal range if the mob is los and within 100. multi puller keeps local camp under control.
  /if (${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} < 100) {
    DEBUG ${sep}Spawn is LOS & < \ag100\ax
    /invoke ${out[9, 0, 55]}
    /target ID TARGET
    /delay 1s ${Target.ID} == TARGET

    | get us a mob
    /call pull_mob_prep ${_debug} TARGET
    /if (!${Macro.Return}) {
      DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
      /invoke ${target[clear]}
      /return FALSE
    }

    /call pull_mob_tag ${_debug} TARGET
    /if (${Macro.Return.Equal[tagfailure]}) {
      DEBUG ${sep}\arNAVpull FAILURE\ax${sep}resetting/returning to camp
      /invoke ${target[clear]}
      /call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
      /return FALSE
    }

  | the mob is not close enough to risk abandoning the camp or it is not in our direct line of sight. send some poor other gimp to get us some loots.
  } else /if (!${Spawn[TARGET].LineOfSight} || (${Spawn[TARGET].Distance} > 100 && ${Spawn[TARGET].Distance} < PULLENV)) {
    DEBUG Spawn is > 100  & < \ayPULLENV\ax

    | is there a usable nav path to this pos?
    /if (${Navigation.PathExists[id TARGET]}) {
      DEBUG ${sep}NAV Path \aggood\ax

      | we try melee first for nav to target then we try pet pulling

      | see if we have any melee classes avaialble?
      /call get_name_multipull ${_debug} |tank|melee

      /if (${Bool[${Macro.Return}]}) {
        /dex ${Macro.Return} /advancedpullnav TARGET ${_debug}
        /goto :waitformobmp
      }

      | no melee available get a pet puller?
      /call get_name_multipull FALSE |MAG|BST|NEC|ENC|SHM|DRU
      /if (${Bool[${Macro.Return}]}) {
        /dex ${Macro.Return} /advancedpullpet TARGET FALSE
        /goto :waitformobmp
      }

    | no path exists to get to target, "God Save the Queen!"
    } else /if (!${Navigation.PathExists[id TARGET]}) {
      DEBUG ${sep}NAV Path \arbad\ax
      :intpull
      /call get_target_intpull ${_debug} TARGET
      /goto :waitformobmp

    }

  }

  | wait for mob to get to camp
  :waitformobmp
  /if (!${SpawnCount[npc radius ${maHard.Find[stMinBowDist].Value} PS4 targetable]}) {

    /invoke ${set_timer[${_debug}, wait_for_mob, 30s]}
    /invoke ${out[9, "Waiting on ${dot} ${con_color[${_debug}, ${_tmpID}]}${sep}\a-w30s\ax"]}
    /while (TARGET && ${Spawn[TARGET].Distance} > ${maHard.Find[stMinBowDist].Value}) {
      GETINPUT get_target_multipull_wait NA|NA
      CHECKDEAD
      /delay 2

      | target gone.. wtf?
      /if (!TARGET) {
        DEBUG ${sep}/while \a-gbreak_NOTtarget\ax
        /break
      }

      | mobs in range not our target
      /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} && ${Spawn[TARGET].Distance} > ${maHard.Find[stMinBowDist].Value}) {
        DEBUG ${sep}/while \a-gbreak_TARGET/RANGE\ax
        /break
      }
      | target LOS & within tag range
      /if (${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} < PULLENV &&  && ${Spawn[TARGET].Distance} < 100) {
        DEBUG ${sep}/while \a-gbreak_tag\ax
        /break
      }
      | target within melee
      /if (${Spawn[TARGET].Distance} <= ${Spawn[TARGET].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
        DEBUG ${sep}/while \a-gbreak_MaxRangeTo\ax
        /break
      }

      | timer ran out
      /if (!${wait_for_mob}) {
        DEBUG ${sep}/while \a-gbreak_Timer\ax
        /break
      }
    }
  }

/return



|***
 * note: preps all advanced pulling
 * note: entry point
 * use: /call pull_prep_advanced DEBUG
 ***|
sub pull_prep_advanced(bool _debug)
  DEBUG \atprep_advanced_pulling\ax()
  
  GETINPUT pull_prep_advanced NA|NA

  SETHUD "pull ${maPull.Find[stPullMode].Value}"

/return



|***
 * note: finds a priest (clr/shm)
 * use: important for later. DO NOT DELETE!!
 ***|
sub find_priest(bool _debug, int _distance)
  DEBUG \atfind_priest\ax()

  /declare _count int local 0
  /declare _toonName string local

  /for _count 1 to ${DanNet.PeerCount[priest]}
    /varset _toonName ${DanNet.Peers[priest].Arg[${_count},|]}

    /if (!${Bool[pc ${_toonName}]}) /continue
    /if (!${Spawn[pc ${_toonName}].ID}) /continue
    /if (${Spawn[pc ${_toonName}].Dead}) /continue
    /if (${Spawn[pc ${_toonName}].Distance} > ${_distance}) /continue
    /break

    | /dex ECHO $\{Me.SpellReady[$\{Spell[$\{healIntervention${_count}}].RankName}]}}
    | /dquery ${_toonName} -q "Me.SpellReady[$\{Spell[$\{healIntervention1}].RankName}]" -o DNQReturn -t 1
    | /if (${Bool[${DNQReturn}]}) /break

  /next _count

/return ${_toonName}



|***
 * note: gets your pet back to you.. if alive
 * note: will pull it back if it's hurt too.
 * sidenote: you should probably consider geting a tougher pet cause urs sucks
 * use: /call where_the_fuck_is_my_pet DEBUG [FORCE RETURN T/F]
 ***|
sub where_the_fuck_is_my_pet(bool _debug, bool _force)
  DEBUG \atwhere_the_fuck_is_my_pet\ax(\a-w${_force}\ax)

  /if (!${Me.Pet.ID}) /return

  /if (${Me.Pet.PctHPs} < 50 || ${_force}) {
    /if (${is_ready[${_debug}, "Summon Companion"]}) {
      /call voodoo ${_debug} "Summon Companion" 0 FALSE
    }
  }

  /if (!${Me.Pet.ReGroup}) /pet regroup
  /if (!${Me.Pet.Hold}) /pet hold on
  /pet back off

/return TRUE



|***
 * note: this sets up observers for the puller to know what the MA is doing.
 * note: not needed for base pulling.
 * use: /call set_pull_observer DEBUG [not used option]
 ***|
sub set_pull_observer(bool _debug, string _verbage)
  DEBUG \atset_pull_observer\ax(${_verbage})

  | try to remember to move this after testing...
  /return TRUE

  /declare _assist string local FALSE
  /call setDeclare MainAssistAgro string outer
  /call setDeclare MainAssistCombat string outer
  /call setDeclare MainAssistMacro string outer

  /call get_assist_name FALSE NAME
  /varset _assist ${Macro.Return.Lower}

  | check the toons running macro
  /if (!${DanNet[${_assist}].ObserveSet["Macro.Name"]}) /dobserve ${_assist} -q "Macro.Name" -o MainAssistMacro
  DEBUG ${sep}MainAssistMacro set:\a-w${MainAssistMacro}\ax

  | no ma or forced delete
  /if (${_verbage.Equal[kill]}) {
    /dobserve ${_assist} -q "setMobAgro" -drop
    /dobserve ${_assist} -q "combat" -drop
    /dobserve ${_assist} -q "Macro.Name" -drop
    /deletevar MainAssistAgro
    /deletevar MainAssistCombat
    /deletevar MainAssistMacro
    /return FALSE
  }

  | set MA agro range
  /if (!${DanNet[${_assist}].ObserveSet["setMobAgro"]}) /dobserve ${_assist} -q "setMobAgro" -o MainAssistAgro
  DEBUG ${sep}MainAssistAgro set:\a-w${MainAssistAgro}\ax

  | set MA combat status
  /if (!${DanNet[${_assist}].ObserveSet["combat"]}) /dobserve ${_assist} -q "combat" -o MainAssistCombat
  DEBUG ${sep}MainAssistCombat set:\a-w${MainAssistCombat}\ax

/return TRUE



|***
 * note: outputs based by a verbosity lvl
 * use: ${out[reqlvl#, [string to output|EMPTY/0|SHADE|FORCED], DB ref##]}
 * _verbage options
 * forced = forces the output no matter what
 * shade = snarky replys. grain is aplied
 * empty/0 = only use the DB ## for the output
  ***|
sub out(int _lvl, string _verbage, int _strNum)

  /declare _debug bool local FALSE

  | forced output, no validation for grain
  /if (${_verbage.Equal[forced]}) {
    /sqlite query "${DBstatic}" getstring SELECT value FROM string WHERE env_var='${_strNum}';
    /if (!${sql_check[${_debug}, getstring, out, SELECT]}) /endmacro
    ECHO ${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}${sqlite.Result[getstring 1 value]}

  | shade output. grain validation
  } else /if (${_verbage.Equal[shade]} && (${lsGrain.Contains[${_lvl}]} || ${_lvl} == 0)) {
    /sqlite query "${DBstatic}" getshade SELECT * FROM shade ORDER BY RANDOM() LIMIT 1;
    /if (!${sql_check[${_debug}, getshade, out, SELECT]}) /endmacro
    ECHO ${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}${sqlite.Result[getshade 1 value]}

  | output grain validation
  } else /if (${Range.Between[1,99999:${Int[${_strNum}]}]} && (${lsGrain.Contains[${_lvl}]} || ${_lvl} == 0)) {
    /sqlite query "${DBstatic}" getstring SELECT value FROM string WHERE env_var='${_strNum}';
    /if (!${sql_check[${_debug}, getstring, out, SELECT]}) /endmacro
    ECHO ${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}${sqlite.Result[getstring 1 value]}

  | output grain validation with passed verbage
  } else /if (!${Range.Between[1,99999:${Int[${_strNum}]}]} && (${lsGrain.Contains[${_lvl}]} || ${_lvl} == 0)) {
    ECHO ${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}${_verbage}

  }

/return FALSE



|***
 * note: checks and corrects tribute and trophy usafe during a raid
 * use:
 ***|
sub check_tribute(bool _debug)
  DEBUG \atcheck_tribute\aw()

  /invoke ${set_data_timer[FALSE, Check_Tribute, RESTART]}

  /if (!${Raid.Members}) {
    /if (${Window[TributeBenefitWnd].Child[TBWG_ActivateButton].Text.Equal[Activate]}) {
      /tribute personal off
    }
    /if (${Window[TributeBenefitWnd].Child[TBWGT_ActivateButton].Text.Equal[Activate]}) {
      /trophy personal off
    }
    /return FALSE
  }

  | Personal tribute is off & Guild is on --> turn on personal
  /if (${Window[TributeBenefitWnd].Child[TBWP_ActivateButton].Text.Equal[Activate]} && ${Window[TributeBenefitWnd].Child[TBWG_ActivateButton].Text.Equal[Deactivate]}) {
    /tribute personal on
  }

  | Personal tribute is on & Guild is off --> turn off personal
  /if (${Window[TributeBenefitWnd].Child[TBWP_ActivateButton].Text.Equal[Deactivate]} && ${Window[TributeBenefitWnd].Child[TBWG_ActivateButton].Text.Equal[Activate]}) {
    /tribute personal off
  }


  | Personal trophy is off & Guild is on --> turn on personal
  /if (${Window[TributeBenefitWnd].Child[TBWT_ActivateButton].Text.Equal[Activate]} && ${Window[TributeBenefitWnd].Child[TBWGT_ActivateButton].Text.Equal[Deactivate]}) {
    /trophy personal on
  }

  | Personal trophy is on & Guild is off --> turn off personal
  /if (${Window[TributeBenefitWnd].Child[TBWT_ActivateButton].Text.Equal[Deactivate]} && ${Window[TributeBenefitWnd].Child[TBWGT_ActivateButton].Text.Equal[Activate]}) {
    /trophy personal off
  }

/return TRUE



|***
 * note: checks if a mode needs to be executed
 * use: /call check_mode DEBUG [RESET]
 ***|
sub check_mode(bool _debug, string _verbage)
  DEBUG \atcheck_mode\ax()

  /invoke ${set_data_timer[FALSE, Check_Mode, RESTART]}

  /declare _out string local
  /declare _mimode mapiterator local
  /vardata _mimode maMode.First.Clone

  | reset used for zoning, and other stuffs
  /if (${_verbage.Equal[reset]}) {
    /while (!${_mimode.IsEnd}) {
      CHECKDEAD
      /if (${_mimode.Value}) {
        /invoke ${maMode.Add[${_mimode.Key},FALSE]}
      }
      /invoke ${_mimode.Advance}
    }
    /return TRUE
  }

  | normal check routine
  /while (!${_mimode.IsEnd}) {
    CHECKDEAD
    /if (${_mimode.Value}) {
      DEBUG mode_${_mimode.Key}

      | skip switches for training flags
      /if (${_mimode.Key.Left[7].Equal[swTrain]}) {
        /invoke ${_mimode.Advance}
        /continue
      }

      /call mode_${_mimode.Key} ${maDebug.Find[${_mimode.Key}].Value}
    }
    /invoke ${_mimode.Advance}
  }

/return TRUE



|***
 * note: checks the passed item name against the DB
 * use: /call set_loot_item DEBUG [NAME]
 ***|
sub set_loot_item(bool _debug, string _item)
  DEBUG \atset_loot_item\ax(\a-w${_item}\ax)

  /if (!${Bool[${_item}]}) {
    /return FALSE
  }

  | check for the item in the DB
  /sqlite query "${DBloot}" lootquery SELECT * FROM loot_control WHERE item="${_item}";
  /if (!${sql_check[${_debug}, lootquery, set_loot_item, SELKECT]}) /endmacro

  | we did not find it?
  /if (${sqlite.Status[lootquery].NotEqual[Success]} || !${sqlite.Rows[lootquery]}) {
    DEBUG ${sep}${_item}${sep}\a-w not in database\ax

    | item not in database or lore and have
    /if (${AdvLoot.SList[${_item}].NoDrop} || ${AdvLoot.SList[${_item}].NoDrop} || ${FindItem[=${_item}].Lore} || ${FindItemBank[=${_item}].Lore}) {
      /invoke ${out[0, "${add} ${dot} ${_item} = ${leave} [no drop/lore]"]}
      /sqlite query "${DBloot}" lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "leave");
      /if (!${sql_check[${_debug}, lootquery, set_loot_item, "INSERT INTO"]}) /endmacro

    | by default add everything as keep.
    } else {
      /invoke ${out[0, "${add} ${dot} ${_item} = ${keep} [default]"]}
      /sqlite query "${DBloot}" lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "keep");
      /if (!${sql_check[${_debug}, lootquery, set_loot_item, "INSERT INTO"]}) /endmacro
    }
  }

/return TRUE



|***
 * note: buy items from a vendor
 * use : /call vendor_buy DEBUG [item] [#]
 ***|
sub vendor_buy( bool _debug, string _itemtoBuy, int _buyAmount)
  DEBUG \atvendor_buy\ax(\a-w${_itemtoBuy}, ${_buyAmount}\ax)

  /if (!${Select[${Target.Type},npc,pet]} || !${Target.ID} || ${Target.Type.Equal[pet]} && !${Target.DisplayName.Find[familiar]}) /return FALSE
  /declare _count int local 0
  /declare _qty int local 0
  /declare _listItem int local 0

  /varcalc _qty ${_buyAmount}-${FindItemCount[=${_itemtoBuy}]}
  /delay 3
  /if (${FindItemCount[=${_itemtoBuy}]} >= ${_buyAmount}) {
    OUT I already have ${_itemtoBuy} ${sep} (${num}${FindItemCount[=${_itemtoBuy}]}\ax)
    /return
  }

  | set item in the list window
  /if (${Target.ID} == ${Spawn[npc alerynril].ID}) {
    /varset _listItem ${Window[NewPointMerchantWnd].Child[NewPointMerchant_ItemList].List[=${_itemtoBuy}]}
  } else {
    /varset _listItem ${Window[MerchantWnd].Child[ItemList].List[=${_itemtoBuy},2]}}
  }

  /if (!${_listItem}) {
    OUT ${caution}${sep}\a-wCouldn't find ${_itemtoBuy}\ax
    /return
  } else {
    /if (${Target.ID} == ${Spawn[npc alerynril].ID}) {
      /notify NewPointMerchantWnd NewPointMerchant_ItemList listselect ${_listItem}
    } else {
      /notify MerchantWnd ItemList listselect ${_listItem}
    }
    /delay 5
  }

  OUT Buying${sep}\a-w${_itemtoBuy} 'till I get ${_buyAmount}\ax

  :loopbuyitems
  GETINPUT vendor_buy NA|NA

  /if (${_qty} > 0) {
    /if (${_qty} > 19) {
      /if (${Target.ID} == ${Spawn[npc alerynril].ID}) {
        /shiftkey /notify NewPointMerchantWnd NewPointMerchant_PurchaseButton leftmouseup
      } else {
        /shiftkey /notify merchantwnd MW_Buy_Button leftmouseup
      }
      /delay 5 ${FindItemCount[=${_itemtoBuy}]} >= ${_qty}
      /varcalc _qty ${_buyAmount}-${FindItemCount[=${_itemtoBuy}]}
      /if (${_qty} <= 0) /return
      }
      /delay 5
      /if (${_qty} > 19) /goto :loopbuyitems
    }
    /if (${_qty} > 0 && ${_qty} < 20) {
      /for _count 1 to ${_qty}
        /if (${Target.ID} == ${Spawn[npc alerynril].ID}) {
          /ctrlkey /notify NewPointMerchantWnd NewPointMerchant_PurchaseButton leftmouseup
        } else {
          /ctrlkey /notify merchantwnd MW_Buy_Button leftmouseup
        }
        /delay 1s ${FindItemCount[=${_itemtoBuy}]} >= ${Math.Calc[${FindItemCount[=${_itemtoBuy}]}+${_count}]}
        ECHO Buying ${_itemtoBuy} ${_count} of ${_qty}
        /if (${_count} >= ${_qty}) /return
      /next _count
      /varcalc _qty ${_buyAmount}-${FindItemCount[=${_itemtoBuy}]}

    }
  }

/return TRUE



|***
 * note: this is used to cycle the control map and handle the IDs in it. final return will be an unmezzed mob ID, if any
 * use: /call check_control_routine DEBUG
 ***|
sub control_routine(bool _debug)
  DEBUG \atcontrol_routine\ax()

  /if (!AUTO || SAFEZONE) /return FALSE
  /if (!${madeBuff.Find[swMez].Value}) /return FALSE
  /if (!${maCC.Find[swCombatControl].Value}) /return FALSE

  GETINPUT control_routine NA|NA
  CHECKEXIT

  | slow this shit down so we can read it...
  /if (${_debug}) {
    /delay 5
  }

  | ID for unmezzed/short mob
  /declare _mezzID int local 0
  | count of uncontrolled mobs in area
  /declare _countArea int local 0

  | ae mez marker
  /declare _aeMezDone bool local FALSE

  /declare _count int local 0

  /if (!${Defined[maMobControl]}) {
    /declare maMobControl map outer
  }

  | check control map for any mobs dead or gone
  /invoke ${control_map[${_debug}, dead]}

  | find unaccounted mobs in my area
  | returns the # of mobs that do not have a current mez timer associated
  /varset _countArea ${control_area[${_debug}]}
  | DEBUG _countArea${sep}\ay${_countArea}\ax

  | if no mobs are returned we bail
  /if (!${_countArea}) {
    DEBUG ${break}\arNo\ax mez targets
    /return TRUE
  }

  | if ${control_area returns more then the toons AE setting, hit the AE routines
  /if (${_countArea} >= AE# && ${Bool[${madeBuff.Find[stAEMez].Value}]}) {
    DEBUG ${sep}ae mez
    /call control_mezae ${_debug}
    /varset _aeMezDone TRUE
  }

  | if we have more then one mob in camp,
  /if (${Bool[${madeBuff.Find[stMez].Value}]}) {
    /if ((((${_countArea} && ${_countArea} < AE#) || (${_countArea} && !${Bool[${madeBuff.Find[stAEMez].Value}]}))) || ${_aeMezDone}) {
      DEBUG ${sep}single target mez

      | mez something for christs sake...
      /declare _mzcheck mapiterator local
      /vardata _mzcheck maMobControl.First.Clone
      /while (!${_mzcheck.IsEnd}) {

        /if (${_mzcheck.Key} == TARGET) {
          /invoke ${_mzcheck.Advance}
          /continue
        }

        CHECKDEAD
        CHECKEXIT
        /call control_mez ${_debug} ${_mzcheck.Key}
        /invoke ${_mzcheck.Advance}
      }
      /varset _aeMezDone FALSE
      /delay 1
    }
  }

  /if (!PULL) {
    /call get_assist_target ${maDebug.Find[target].Value}
  }

/return TRUE



|***
 * note: this is used to check our current ENV and find mobs that we do not know about that are pissed at us
 * use: ${control_area[DEBUG]}
 ***|
sub control_area(bool _debug)
  DEBUG \atcontrol_area\ax()

  /declare _agroCount int local 0
  /declare _count int local 0
  /declare _tmpID int local
  GETINPUT control_area NA|NA

  /for _count 1 to ${SpawnCount[npc los range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ENV zradius ${maCC.Find[stZRadius].Value} PS4]}
    | get the first mob in our discovery
    /varset _tmpID ${NearestSpawn[${_count}, npc los range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ENV zradius ${maCC.Find[stZRadius].Value} PS4].ID}
    DEBUG ${sep}control _tmpID${sep}\a-w${_tmpID}\ax

    GETINPUT

    | if this mob is the current assists target, remove it from our need to control
    /if (${_tmpID} == TARGET) {
      /if (${maMobControl.Contains[${_tmpID}]}) {
        DEBUG ${sep}removing ma target from maMobControl map${sep}\a-w${_tmpID}\ax
        /invoke ${maMobControl.Remove[${_tmpID}]}

        | delete the existing mez timer if any?
        /if (${Defined[timer_mez_ID${_tmpID}]}) {
          DEBUG ${sep} deleting timer for${sep}\a-wtimer_mez_ID${_tmpID}\ax
          /deletevar timer_mez_ID${_tmpID}
        }

      }
      /continue
    }

    | build our counter to return
    DEBUG ${sep}incrent aecounter${sep}\a-w${_agroCount} + 1\ax
    /varcalc _agroCount ${_agroCount} + 1

    | do we have the mob in the mez list ??
    /if (${maMobControl.Contains[${_tmpID}]}) {
      DEBUG ${sep}mob already in maMobControl map${sep}\a-w${_tmpID}\ax
      /continue
    }

    | mob is not in mez list and not in agro list. fresh meat...
    /if (!${maMobControl.Contains[${_tmpID}]}) {
      DEBUG ${sep}mob \arNOT\ax in maMobControl map${sep}\a-w${_tmpID}\ax

      GETINPUT

      | identify mobs we dont touch
      /if (${lsZoneNoMez.Contains[${Spawn[${_tmpID}].DisplayName}]} || ${lsZoneExclude.Contains[${Spawn[${_tmpID}].DisplayName}]}) {
        DEBUG ${sep}lsZoneNoMez${sep}\a-w${_tmpID}\ax
        /invoke ${maMobControl.Add[${_tmpID},EXEMPT]}

      | mobs that we are to charm
      } else /if (${lsZoneCharm.Contains[${Spawn[${_tmpID}].DisplayName}]} && ${maChr.Find[swCharm].Value}) {
        DEBUG ${sep}lsZoneCharm${sep}\a-w${_tmpID}\ax
        /invoke ${maMobControl.Add[${_tmpID},CHARM]}

      | mobs that we are to mez
      | create the mez timer with the ID
      } else {
        /if (!${Defined[timer_mez_ID${_tmpID}]}) {
          /invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, 1]}
        }
        /invoke ${maMobControl.Add[${_tmpID},MEZ]}
      }

    }

  /next _count

/return ${_agroCount}



|***
 * note: check the IDs in the passed list. remove them from the list if they are dead/gone
 * use: ${control_map[DEBUG, THINGIETOCHECK??]}
 *
 * THINGIETOCHECK:
 * dead = check for dead or no more spawns. removes them from the control map
 * mez = find mobs that are not mezed. returns ID of nonmezed or short duration remaining mezed mob
 ***|
sub control_map(bool _debug, string _type)
  DEBUG \atcontrol_map\ax(\a-w${_type}\ax)

  /declare _returnCount int local 0
  GETINPUT control_map

  | check list for dead mobs and get them gone
  /if (${_type.Equal[dead]}) {
    GETINPUT

    /declare maRemovals list
    /declare _remcount int local 0
    /declare _count int local 0

    /declare _mirem mapiterator local
    /vardata _mirem maMobControl.First.Clone
    /invoke ${maRemovals.Clear}
    /while (!${_mirem.IsEnd}) {
      GETINPUT
      DEBUG ${sep}_mirem.Key--.Value${sep}\a-w${_mirem.Key} -- ${_mirem.Value}\ax${sep}${If[!${Spawn[${_mirem.Key}].ID} || ${Spawn[${_mirem.Key}].Dead},\ardead\ax,\agalive\ax]}
      /if (!${Spawn[${_mirem.Key}].ID} || ${Spawn[${_mirem.Key}].Dead}) {
        DEBUG ${sep}removing${sep}\a-w${_mirem.Key}\ax

        | delete the existing mez timer if any?
        /if (${Defined[timer_mez_ID${_mirem.Key}]}) {
          /deletevar timer_mez_ID${_mirem.Key}
        }

        | Add the key to the list of items to remove.
        /invoke ${maRemovals.Append[${_mirem.Key}]}
      }
      /invoke ${_mirem.Advance}
    }

    /varcalc _remcount ${maRemovals.Count}

    /for _count 1 to ${_remcount}
      /invoke ${maMobControl.Remove[${maRemovals.Head}]}
    /next _count

  }

  | find a non mezed mob or mob with a short duration mez left. return its ID if any for cc
  /if (${_type.Equal[mez]}) {
    DEBUG ${sep}cc${sep}
    /declare _mimez mapiterator local
    /vardata _mimez maMobControl.First.Clone
    /while (!${_mimez.IsEnd}) {
      DEBUG ${sep}_mimez.Key--.Value${sep}\a-w${_mimez.Key} -- ${_mimez.Value}\ax
      CHECKDEAD
      /if (${Bool[${maCC.Find[stAssistMode].Value}]} && ${_mimez.Key} == ${Me.XTarget[1].ID}) {
        /invoke ${_mimez.Advance}
        /continue
      }

      /if (${_mimez.Value.Equal[MEZ]} && ${Defined[timer_mez_ID${_mimez.Key}]}) {

        /if (!${Defined[timer_mez_ID${_mimez.Key}]}) {
          /invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, 1]}
        }

        | if the timer is < 6 seconds. remez
        /if (${timer_mez_ID${_mimez.Key}} < 60) {
          /return ${_mimez.Key}
        }

      }
      /invoke ${_mimez.Advance}
    }
    /return 0
  }

/return FALSE



|***
 * note: AE mez routine
 * use: rwq > 1 mob
 ***|
sub control_mezae(bool _debug)
  DEBUG \atcontrol_mezae\ax()

  ISMEDEAD
  GETINPUT control_mezae NA|NA
  /declare _tmpID int local

  /if (!${is_ready[${_debug}, "${madeBuff.Find[stAEMez].Value}"]}) {
    /return FALSE
  }

  /if (${maMobControl.Count} >= AE#) {
    DEBUG ${sep}entering AE mez
    /vardata _tmpID maMobControl.First

    /if (AE# && ${SpawnCount[npc los radius ${Spell[${madeBuff.Find[stAEMez].Value}].AERange} targetable]} >= AE#) {
      DEBUG ${sep}AE# && ${SpawnCount[npc los radius ${Spell[${madeBuff.Find[stAEMez].Value}].AERange} targetable]} >= AE#

      /if (${Me.Gem[${Spell[${maChr.Find[stAEStun].Value}].RankName}]}) {
        /if (${Me.Casting.Name.NotEqual[${Spell[${maChr.Find[stAEStun].Value}].RankName}]}) /stopcast
        /delay 2.25s ${Me.SpellReady[${Spell[${maChr.Find[stAEStun].Value}].RankName}]}
        /call voodoo ${_debug} "${maChr.Find[stAEStun].Value}" 0 FALSE
      }

      /if (${Me.Gem[${Spell[${madeBuff.Find[stAEMez].Value}].RankName}]}) {
        /if (${Me.Casting.Name.NotEqual[${Spell[${madeBuff.Find[stAEMez].Value}].RankName}]}) /stopcast
        /delay 2.25s ${Me.SpellReady[${Spell[${madeBuff.Find[stAEMez].Value}].RankName}]}
        /call voodoo ${_debug} "${madeBuff.Find[stAEMez].Value}" 0 FALSE
      }

    }

    | use AA beam of slumber?
    /if (${maChr.Find[swAABoS].Value}) {
      /if (${Spawn[${_tmpID}].NearestSpawn[3,npc radius 30].ID} && ${Spawn[${_tmpID}].Distance3D} <= 200 && ${Spawn[${_tmpID}].Distance3D} >= ENV) {
        /if (${is_ready[${_debug}, "${maChr.Find[stAABeamofSlumber].Value}"]}) {
          /if (${Me.Casting.ID}) /stopcast
          /face ID ${_tmpID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
          /call voodoo ${_debug} "${maChr.Find[stAABeamofSlumber].Value}" 0 FALSE
        }
      }
    }
  }

/return



|***
 * note: single mez the target
 * use: /call control_mez DEBUG [TARGETID]
 ***|
sub control_mez(bool _debug, int _tmpID)
  DEBUG \atcontrol_mez\ax(${_tmpID})

  /if (!${_tmpID}) /return FALSE

  | if theres no timer.. make it
  /if (!${Defined[timer_mez_ID${_tmpID}]}) {
    /invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, 1]}
  }

  | if the current timer is greater then 6 seconds, move on
  /if (${timer_mez_ID${_tmpID}} > 60) {
    /return FALSE
  }

  /declare _resistCount int local 0
  GETINPUT control_mez NA|NA

  | try to mez more then once if there is a fail
  /while (${_resistCount} < ${madeBuff.Find[stCountMezResist].Value}) {

    /invoke ${target[set, ${_tmpID}]}
    /if (${_tmpID} == TARGET || ${_tmpID} == ${Me.XTarget[1].ID}) {
      /invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, 30s]}
      /return FALSE
    }

    ISMEDEAD
    GETINPUT

    /if (!${Target.LineOfSight}) {
      /varcalc _resistCount ${_resistCount} + 1
      /continue
    }

    /if (${Target.Mezzed.Duration} && ${Spell[${Target.Mezzed}].Name.Find[${madeBuff.Find[stMez].Value}]}) {
      /invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, ${Target.Mezzed.Duration.TotalSeconds}s]}
      /return TRUE
    }

    | mez ready?
    /delay 2.25s ${Me.SpellReady[${Spell[${madeBuff.Find[stMez].Value}].RankName}]}
    /call voodoo ${_debug} "${madeBuff.Find[stMez].Value}" ${_tmpID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,IMMUNE,TAKEHOLD]}) {

      /if (${Select[${maData.Find[stCastReturn].Value},IMMUNE,TAKEHOLD]}) {
        | auto add to no mez list for zone
        /if (${maEntropy.Find[swAutoList].Value} && ${maData.Find[stCastReturn].Value.Equal[IMMUNE]}) {
          /sqlite query "${DBzone}" dummy INSERT INTO zone_control VALUES ("${Zone.ShortName}", "lsZoneNoMez", "${Target.DisplayName}");
          /if (!${sql_check[${_debug}, dummy, control_mez, "INSERT INTO"]}) /endmacro
          /invoke ${out[15, 0, 58]}
          /call set_zone_lists ${maDebug.Find[area].Value}
        }

        | were not going to try this target again, clear its current control settings
        /invoke ${maMobControl.Add[${_tmpID},EXEMPT]}
        /if (${Defined[timer_mez_ID${_tmpID}]}) {
          /deletevar timer_mez_ID${_tmpID}
          /return FALSE
        }

      }

      /invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, ${Spell[${madeBuff.Find[stMez].Value}].Duration.Seconds}s]}
      /return TRUE

    } else /if (${Select[${maData.Find[stCastReturn].Value},RESISTED,RESISTEDYOU]}) {
      /varcalc _resistCount ${_resistCount} + 1
      /if (${_resistCount} >= 1) {
        /if (${is_ready[${_debug}, "${madeBuff.Find[stTash].Value}"]}) {
          /call voodoo ${_debug} "${madeBuff.Find[stTash].Value}" ${_tmpID} FALSE
        }
      }
    }
  }

/return



|***
 * note: builds the outputs for various / commands
 * use:

 - submenu = marker pointing to a deeper menu
 - option - the command is just an option no associated variables
 - br = square brackets surround the variable [variable]
 - out = simply display the variable with no off status
 - sw - an ON/OFF gree/red switch
 - pr - surround the on/off switch in a ( )
 ***|
sub set_control_output(bool _debug, string _map, string _outname, string _control)
  DEBUG \atset_control_output\ax(\a-w${_map}, "${_outname}", ${_control}\ax)

  /declare _out string local
  /declare _value string local
  /vardata cmi ${_control}.First.Clone
  
  | start the control with the list option if there is one.
  /if (${${_control}.Contains[list]}) {
    /varset _out ${_out} ${out_list}
    /if (${${_control}.Count} > 1) {
      /varset _out ${_out} ${dot}
    }
  }
    
  /while (!${cmi.IsEnd}) {
    DEBUG ${sep}left ${sep}\a-w${cmi.Value.Left[2]}\ax
    DEBUG ${sep}${cmi.Key}${sep}\a-w${cmi.Value}\ax${sep}\a-y${${_map}.Find[${cmi.Value}].Value}\ax

    /if (${cmi.Value.Count[|]}) {
      /varset _value ${cmi.Value.Arg[1,|]}
    } else {
      /varset _value ${cmi.Value}
    }

    DEBUG ${sep}cmi${sep}\a-w${cmi.Value}\ax

    | submenu only
    /if (${cmi.Value.Equal[submenu]}) {
      /varset _out ${_out} ${info}${cmi.Key}\ax:${submenu}
      
    | no variables just one shot option
    } else /if (${cmi.Value.Equal[list]}) {
      /invoke ${cmi.Advance}
      /continue   
      
    | no variables just one shot option
    } else /if (${cmi.Value.Equal[option]}) {
      /varset _out ${_out} \a#1E90FF${cmi.Key}\ax

    | no variables
    } else /if (${cmi.Value.Equal[out]}) {
      /varset _out ${_out} \aw${cmi.Key}\ax

    | no variables just an option
    } else /if (${cmi.Value.Arg[2,|].Equal[show]}) {
      /varset _out ${_out} ${info}${cmi.Key}\ax:${num}${${_map}.Find[${_value}].Value}\ax

    | string in brackets
    } else /if (${cmi.Value.Left[2].Equal[st]} && ${cmi.Value.Arg[2,|].Equal[br]}) {
      /varset _out ${_out} ${info}${cmi.Key}\ax:[${If[${Bool[${${_map}.Find[${_value}].Value}]},${num}${${_map}.Find[${_value}].Value}\ax,${off}]}]

    | string in persentasies
    } else /if (${cmi.Value.Left[2].Equal[st]} && ${cmi.Value.Arg[2,|].Equal[pr]}) {
      /varset _out ${_out} ${info}${cmi.Key}\ax:(${If[${Bool[${${_map}.Find[${_value}].Value}]},\ag${${_map}.Find[${_value}].Value}\ax,${r0}]})

    | straight string
    } else /if (${cmi.Value.Left[2].Equal[st]}) {
      /varset _out ${_out} ${info}${cmi.Key}\ax:${If[${Bool[${${_map}.Find[${_value}].Value}]},${num}${${_map}.Find[${_value}].Value}\ax,${r0}]}

    | switch in persentasies
    } else /if (${cmi.Value.Left[2].Equal[sw]} && ${cmi.Value.Arg[2,|].Equal[pr]}) {
      /varset _out ${_out} ${info}${cmi.Key}\ax:(${If[${${_map}.Find[${_value}].Value},${on},${off}]})

    | straight switch (optional submenu)
    } else /if (${cmi.Value.Left[2].Equal[sw]}) {
      /varset _out ${_out} ${info}${cmi.Key}\ax:${If[${${_map}.Find[${_value}].Value},${on},${off}]}${If[${cmi.Value.Arg[2,|].Equal[sm]},${submenu},]}
    }

    /invoke ${cmi.Advance}
    /if (!${cmi.IsEnd}) /varset _out ${_out} ${dot}
  }
  /invoke ${out[12, "/${_outname} ${dot} ${_out}"]}

/return



|***
 * note: checks heal routines by class
 * use: /call check_heal_routines
 ***|
sub check_heal_routines()

  | heal: curing
  /if (!${timer_Check_Cure} && (${maHeal.Find[swHealCureSelf].Value} || ${maHeal.Find[swHealCureGroup].Value} || ${maHeal.Find[swHealCureCrew].Value})) {
    /call check_cure ${maDebug.Find[cure].Value}
  }

  | heal: target of target
  /if (${lsClassHealToT.Contains[${Me.Class.ShortName}]} && ${maHeal.Find[swHealToT].Value}) {
    /call check_heal ${maDebug.Find[tot].Value} ToT
  }

  | heal: self
  /if (${maHeal.Find[swHealSelf].Value}) {
    /call check_heal ${maDebug.Find[self].Value} Self
  }

  | heal: DanNet
  /if (${maHeal.Find[swHealDanNet].Value}) {
    /call check_heal ${maDebug.Find[dannet].Value} DanNet
  }

  | heal: ch (CLERIC ONLY)
  /if (${Select[${Me.Class.ShortName},CLR]} && ${maHeal.Find[stPctCH].Value} && ${Group.Members}) {
    /call check_heal ${maDebug.Find[group].Value} ch
  }

  | heal: group
  /if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
    /call check_heal ${maDebug.Find[group].Value} Group
  }

  | heal: extended target lists
  /if (${maHeal.Find[swHealXTarget].Value}) {
    /call check_heal ${maDebug.Find[xt].Value} XTarget
  }

  | heal: pets
  /if (${maHeal.Find[swHealPet].Value}) {
    /call check_heal ${maDebug.Find[pet].Value} Pet
  }


  GETINPUT check_heal_routines_end NA|NA
  CHECKREZ

/return TRUE



|***
 * note: checks/corrects a/the a target
 * note: returns FALSE if a target is dead or does not exist, returns TRUE otherwise
 * use: /call target [assist|isdead|clear|set] [TARGETID]
 * use: ${target[assist|isdead|clear|set, TARGETID]}
 ***|
sub target(string _type, int _tmpID)

  /declare _debug bool local FALSE

  /if (${maDebug.Find[target].Value}) {
    /varset _debug TRUE
    DEBUG \attarget\ax(\a-w${_type}, ${_tmpID}\ax)
  }

  | set a default target id if none passed
  /if (!${_tmpID} && ${Bool[${maCC.Find[stAssistMode].Value}]} && !${Select[${_type},clear]} && !${maCC.Find[swSetForced].Value}) {
    /varset _tmpID TARGET
  }


  | check assist XTarget[1] vs. _tmpID
  /if (!PULL && AUTO) {
    /if (${Select[${_type},assist]}) {
      /if (${Me.XTarget[1].ID} != ${_tmpID}) {
        /call get_assist_target ${_debug}
      }
      /return TRUE
    }
  }

  /declare _isdead bool local FALSE

  | set a target
  /if (${Select[${_type},set]}) {

    /if (${Spawn[${_tmpID}].ID} && ${Spawn[${_tmpID}].Type.NotEqual[Corpse]}) {

      /if (!${Bool[${_tmpID}]}) {
        DEBUG ${sep}\a-y${_type}\ax${sep}${_tmpID}
        /return TRUE
      }

      /if (${Spawn[npc corpse ${_tmpID}].ID}) {
        DEBUG ${sep}\a-y${_type}\ax${sep}corpse
        /varset _isdead TRUE
      }

      /if (${Target.ID} == ${_tmpID}) {
        DEBUG ${sep}\a-y${_type}\ax${sep}good
        /return TRUE
      }

      DEBUG ${sep}\a-y${_type}\ax${sep}setting
      /squelch /target id ${_tmpID}
      | /delay 1s ${Target.ID} == ${_tmpID}

      /if ((${Target.ID} == ${_tmpID}) && !${_isdead}) {
        DEBUG ${sep}\a-y${_type}\ax${sep}isset
        /return TRUE
      }
    } else /if (!${Spawn[${_tmpID}].ID} || ${Spawn[${_tmpID}].Type.NotEqual[Corpse]}) {
      /varset _isdead TRUE
    }

  }

  | is the target  ..  dead?
  | return TRUE if dead otherwise return FALSE
  /if (${Select[${_type},isdead]} || ${_isdead}) {
    DEBUG ${sep}\a-y${_type}\ax${sep}

    | burn spinup command short circut
    /if (${maBurn.Find[swBurnSpinUp].Value}) {
      /return FALSE
    }

    /if (!AUTO) {
      /if (!${Bool[${_tmpID}]} && ${Target.ID}) {
        DEBUG ${sep}!auto !target.id clear_combat
        /varset _tmpID ${Target.ID}
      }
      /if (!${Bool[${_tmpID}]}) {
        DEBUG ${sep}!auto !_tmpID clear_combat
        /varset _isdead TRUE
      }
    }

    | reasons to consider something dead
    /if (${_tmpID} == 0 || !${Bool[${_tmpID}]}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}!_tmpID
      /varset _isdead TRUE
    } else /if (!${Target.ID}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}!target-id
      /varset _isdead TRUE
    } else /if (${Target.Dead}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}target-dead
      /varset _isdead TRUE
    } else /if (${Target.Type.Equal[Corpse]}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}target-corpse
      /varset _isdead TRUE
    } else /if (${Spawn[${_tmpID}].Dead}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}spawn-dead
      /varset _isdead TRUE
    } else /if (!${Spawn[${_tmpID}].ID}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}!spawn-id
      /varset _isdead TRUE
    }

    /if (!${_isdead}) {
      /return FALSE
    }

  }

  | clear/reset all combat
  /if (${Select[${_type},clear]} || ${_isdead}) {
    DEBUG ${sep}\a-y${_type}\ax${sep}
    GETINPUT target_clear NA|NA

    /varset stAssTarID 0
    /invoke ${maCC.Add[swValidAssTarID,FALSE]}
    /invoke ${maCC.Add[swSetCombat,FALSE]}
    /invoke ${maCC.Add[swSetForced,FALSE]}
    /invoke ${set_data_timer[FALSE, Check_Behind, 0]}

    /if (${Me.AutoFire}) {
      /squelch /autofire
    }

    /if (${Me.Combat}) {
      /squelch /attack off
    }

    /if (!${Select[${Me.Class.ShortName},BRD]}) {
      /if (AUTO && ${Me.Casting.ID}) {
        /if (!${Select[${Spell[${Me.Casting.ID}].Category},Transport,Heals]}) {
          /stopcast
        }
      }
    }

    /invoke ${maBurn.Add[swBurnConditionMet,FALSE]}

    /if (${Stick.Active}) {
      /squelch /stick off
    }
    /if (${Target.ID}) {
      /squelch /target clear
    }
    /return TRUE
  }

/return NULL




|***
 * note: attraction pulls for tanks
 * use: /call pull_mob_attraction DEBUG [TARGETID] [FORCE T/F]
 ***|
sub pull_mob_attraction(bool _debug, int _tmpID, bool _force)
  DEBUG \atpull_mob_attraction\ax(\a-w${_tmpID}, ${_force}\ax)

  | /if (!${maChr.Find[swAttraction].Value} && !${_force}) /return FALSE
  /if (${Spawn[id ${_tmpID}].Distance} > PULLENV) /return FALSE
  | /if (${Spawn[id ${_tmpID}].Distance} < ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) /return FALSE
  /if (${Spawn[id ${_tmpID}].Distance} < ${maHard.Find[stMinBowDist].Value}) /return FALSE
  /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
    /return FALSE
  }

  /declare _attraction string local FALSE
  /if (${Select[${Me.Class.ShortName},SHD]}) /varset _attraction Hate's Attraction
  /if (${Select[${Me.Class.ShortName},WAR]}) /varset _attraction Warlord's Grasp
  /if (${Select[${Me.Class.ShortName},PAL]}) /varset _attraction Divine Call
  /if (${Select[${Me.Class.ShortName},MON]}) /varset _attraction Moving Mountains

  DEBUG ${sep}${Me.Class.ShortName} _attraction${sep}\a-w${_attraction}\ax

  /if (!${is_ready[${_debug}, "${_attraction}"]}) {
    /return FALSE
  }

  /call voodoo ${_debug} "${_attraction}" ${_tmpID} FALSE PULLENV

/return TRUE



|***
 * note: cast or trigger alliance on target
 * use: /call trigger_alliance TARGETID

sub trigger_alliance(bool _debug, int _tmpID)
  DEBUG \attrigger_alliance\ax(\a-w${_tmpID}\ax)
  ISMEDEAD
  GETINPUT

  | early GTFO if not ready
  /if (!${Me.CombatAbilityReady[${Spell[${${Me.Class.ShortName}Alliance}].RankName}]} && ${Me.CombatAbility[${Spell[${${Me.Class.ShortName}Alliance}].RankName}]}) /return
  /if (!${Me.SpellReady[${Spell[${${Me.Class.ShortName}Alliance}].RankName}]} && ${Me.Book[${Spell[${${Me.Class.ShortName}Alliance}].RankName}]}) /return

  | correct healer targets to the assists ID, not the mobs..
  /if (${Select[${Me.Class.ShortName},CLR,SHM,DRU]}) {
    /if (${Raid.Members}) {
      /varset _tmpID ${Spawn[pc ${Raid.MainAssist}].ID}
    } else /if (!${Raid.Members} && ${Group}) {
      /varset _tmpID ${Spawn[pc ${Group.MainAssist}].ID}
    } else /if (${Bool[${setMA1}]} && ${Bool[${Spawn[pc ${setMA1}].ID}]} && !${Bool[${Spawn[pc ${setMA1}].Dead}]}) {
      /varset _tmpID ${Spawn[pc ${setMA1}].ID}
    } else /if (${Bool[${setMA2}]} && ${Bool[${Spawn[pc ${setMA2}].ID}]} && !${Bool[${Spawn[pc ${setMA2}].Dead}]}) {
      /varset _tmpID ${Spawn[pc ${setMA2}].ID}
    }
  }

  | is the spawn still alive?
  /if (${is_target_dead[${_tmpID}]}) /return FALSE

  | adjust our current target
  /varset dump ${set_Target[${_tmpID}]}

  | are we there yet?
  /delay 5 ${Target.BuffsPopulated}

  | have the alliance on it already?
  /if (${Target.BuffsPopulated}) {
    /if (${Target.Buff[${${Me.Class.ShortName}Alliance}].ID}) /return FALSE
    /if (${Bool[${Target.Buff[${${Me.Class.ShortName}Alliance}]}]}) /return FALSE
    /if ((${Spawn[${Target.Buff[${${Me.Class.ShortName}Alliance}].Caster}].ID} == ${Me.ID})) /return FALSE
  } else /if (!${Target.BuffsPopulated}) {
    /return FALSE
  }

  | alliance time
  /if (!${Target.BuffDuration[${${Me.Class.ShortName}Alliance}].TotalSeconds}) {
    /if (${validate_cast[FALSE, FIND, "${Spell[${${Me.Class.ShortName}Alliance}].RankName}", ${_tmpID}]}) /call core_cast2 "${Spell[${${Me.Class.ShortName}Alliance}].RankName}" FIND ${_tmpID} FALSE
    /return
  }

  | if theres no forced casted trigger, gtfo
  /if (!${Bool[${${Me.Class.ShortName}AllianceTrigger}]}) /return

  /declare _count int local 0

  | is one of the trigger spells ready?
  /for _count 1 to ${setAllianceTrigger.Count[|]}
    /if (${Me.SpellReady[${Spell[${setAllianceTrigger.Arg[${_count},|]}].RankName}]}) {
      /if (${validate_cast[FALSE, FIND, "${Spell[${setAllianceTrigger.Arg[${_count},|]}].RankName}", ${_tmpID}]}) /call core_cast2 "${Spell[${setAllianceTrigger.Arg[${_count},|]}].RankName}" FIND ${_tmpID} FALSE
      /continue
    }
  /next _count

/return
 ***|



|***
 * note: swap to a bandolier for a proc
 * use: /sash proc [NAME]
 ***|
sub check_sash_proc(bool _debug)
  DEBUG \atcheck_sash_proc\ax()

  | make sure the settings we need are there
  /if (!${maSash.Find[swSashProc].Value}) {
    /invoke ${set_data_timer[${_debug}, Check_Proc_Sash, 30s]}
    /return FALSE
  }

  /invoke ${set_data_timer[${_debug}, Check_Proc_Sash, RESTART]}

  | set the buff we need to look for
  /declare _buff string local ${FindItem[${Me.Bandolier[${maSash.Find[stSashProc].Value}].Item[1]}].Spell}

  |
  /if ((${Me.Buff[${_buff}].ID} || ${Me.Song[${_buff}].ID} || ${Target.Buff[${_buff}].ID}) && ${Me.Bandolier[${maSash.Find[stSashProc].Value}].Active}) {
    DEBUG ${sep}buff found${sep}\a-w${_buff}\ax

    | primary
    /if (${Bool[${maData.Find[stMainhand].Value}]}) {
      /call Bind_command_swap "${maData.Find[stMainhand].Value}" 13 ${_debug}
    }

    | secondary
    /if (${Bool[${maData.Find[stOffhand].Value}]}) {
      /call Bind_command_swap "${maData.Find[stOffhand].Value}" 14 ${_debug}
    }

    | ranged
    /if (${Bool[${maData.Find[stRanged].Value}]}) {
      /call Bind_command_swap "${maData.Find[stRanged].Value}" 11 ${_debug}
    }

    | ammo
    /if (${Bool[${maData.Find[stAmmo].Value}]}) {
      /call Bind_command_swap "${maData.Find[stAmmo].Value}" 22 ${_debug}
    }
    /return TRUE
  }

  /if (!${Me.Buff[${_buff}].ID} && !${Me.Song[${_buff}].ID} && !${Target.Buff[${_buff}].ID} && !${Me.Bandolier[${maSash.Find[stSashProc].Value}].Active} && ${Spell[${_buff}].Stacks}) {
    /invoke ${maData.Add[stMainhand,${Me.Inventory[mainhand].Name}]}
    /invoke ${maData.Add[stOffhand,${Me.Inventory[offhand].Name}]}
    /invoke ${maData.Add[stRanged,${Me.Inventory[ranged].Name}]}
    /invoke ${maData.Add[stAmmo,${Me.Inventory[ammo].Name}]}
    /delay 1
    /invoke ${Me.Bandolier[${maSash.Find[stSashProc].Value}].Activate}
  }

/return TRUE



|***
 * note: Time To Live (TTL) stores the average kill time for a mob by name
 * use: /call control_ttl DEBUG [control] [mob display name]
 ***|
sub control_ttl(bool _debug, string _verbage, string _mob)
  DEBUG \atcontrol_ttl\ax(\a-w${_verbage}, "${_mob}"\ax)


  /if ((!${Target.ID} || ${Target.Dead}) && ${_verbage.NotEqual[end]}) {
    /doevents flush TTLstart
    /return TRUE
  }

  | start the things for the things that need things
  /if (${_verbage.Equal[start]}) {
    /if (${maTTL.Find[stTTLTimeStart].Value} > 0) {
      DEBUG ${sep}TTL timer${sep}\a-wbreaking\ax
      /return TRUE
    }
    | set the initial timer
    /invoke ${maTTL.Add[stTTLTimeStart,${Time.SecondsSinceMidnight}]}

    | sort out the mob count
    /if (${maTTL.Contains[${_mob}]}) {
      /declare _mobcount int local ${Math.Calc[${maTTL.Find[${_mob}].Value.Arg[1,|]} + 1]}

      /if (${_mobcount} > 0) {
        /declare _lasttime int local ${maTTL.Find[${_mob}].Value.Arg[2,|]}
      } else {
        /declare _lasttime int local 0
      }

      /invoke ${maTTL.Add[${_mob},${_mobcount}|${_lasttime}]}
    } else {
      /declare _mobcount int local 1
      /invoke ${maTTL.Add[${_mob},${_mobcount}|1]}
    }
    /return TRUE

  | set the things to the things for all of the things that the things need
  } else /if (${_verbage.Equal[end]} && ${maTTL.Contains[${_mob}]}) {
    | time from the last kill
    /declare _lasttime int local ${maTTL.Find[${_mob}].Value.Arg[2,|]}
    DEBUG ${sep}_lasttime${sep}\a-w${_lasttime}\ax :: ${maTTL.Find[${_mob}].Value.Arg[2,|]}
    | time from, this kill
    /declare _killtime int local ${Math.Calc[${Time.SecondsSinceMidnight}-${maTTL.Find[stTTLTimeStart].Value}].Int}
    DEBUG ${sep}_killtime${sep}\a-w${_killtime}\ax
    | get mob count
    /declare _mobcount int local ${maTTL.Find[${_mob}].Value.Arg[1,|]}

    | put all the shit back together
    /if (${_mobcount} > 1) {
      /invoke ${maTTL.Add[${_mob},${_mobcount}|${Math.Calc[(${_lasttime} + ${_killtime}) / 2].Int}]}
    } else {
      /invoke ${maTTL.Add[${_mob},${_mobcount}|${_killtime}]}
    }

    | reset the running time
    /invoke ${maTTL.Add[stTTLTimeStart,0]}
    /return TRUE
  }

/return



|***
 * note: defensive things
 * use: /call check_cycle_defense DEBUG
 ***|
sub cast_cycle_defense(bool _debug, int _tmpID)
  DEBUG \atcast_cycle_defense\ax(\a-w${_tmpID}\ax)

  DEBUG ${sep}spawncount${sep}\a-w${SpawnCount[NPC radius SAFEENV PS4]} < ${maDefense.Find[stCountDef].Value}\ax
  DEBUG ${sep}HP%${sep}\a-w${Me.PctHPs} > ${maDefense.Find[stPctDef].Value}\ax

  /if ((${SpawnCount[NPC radius SAFEENV PS4]} < ${maDefense.Find[stCountDef].Value}) && (${Me.PctHPs} > ${maDefense.Find[stPctDef].Value})) {
    DEBUG ${sep}not in danger
    /return FALSE
  }

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineDefense.Contains[EMPTY]} && ${lsLineDefense.Count} == 1) {
    DEBUG ${sep}resetting the empty defense line
    /invoke ${lsLineDefense.Clear}
    /invoke ${lsLineDefense.Append[${stCount20}]}
  }

  /declare _liC20 listiterator local
  /vardata _liC20 lsCount20.First.Clone
  /while (!${_liC20.IsEnd}) {

    GETINPUT cast_cycle_defense NA|NA
    ISMEDEAD

    | check the switch
    /if (!${Bool[${maDefense.Find[swDefense${_liC20.Value}].Value}]}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maDefense.Find[stDefense${_liC20.Value}].Value}]}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    | defens thing is ready?
    /if (!${is_ready[${_debug}, "${maDefense.Find[stDefense${_liC20.Value}].Value}"]}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Defense ${_liC20.Value} ${Me.DisplayName} "${maDefense.Find[stDefense${_liC20.Value}].Value}" cast_cycle_defense
    /if (!${Macro.Return}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    DEBUG ${sep}_liC20.Value${sep}\a-w${_liC20.Value}\ax${sep}passed tags

    | seff targetable defnsive "things"
    /if (${lsDefense${_liC20.Value}Tag.Contains[self]}) {
      /varset _tmpID ${Me.ID}
    } else {
      /varset _tmpID 0
    }

    ISMEDEAD

    /call voodoo ${_debug} "${maDefense.Find[stDefense${_liC20.Value}].Value}" ${_tmpID} FALSE

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: clicks stuff
 * use: /call check_item_cycle DEBUG [TARGETID]
 ***|
sub check_item_cycle(bool _debug, int _tmpID, string _tag)
  DEBUG \atcheck_item_cycle\ax(\a-w${_tmpID}, ${_tag}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineItem.Contains[EMPTY]} && ${lsLineItem.Count} == 1) {
    DEBUG ${sep}resetting the empty clickitem line
    /invoke ${lsLineItem.Clear}
    /invoke ${lsLineItem.Append[${stCount20}]}
  }

  /declare _tmpName string local
  /declare _liC20 listiterator local
  /vardata _liC20 lsLineItem.First.Clone
  /while (!${_liC20.IsEnd}) {

    GETINPUT check_item_cycle NA|NA
    ISMEDEAD

    | check the switch
    /if (!${Bool[${maItem.Find[swItem${_liC20.Value}].Value}]}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maItem.Find[stItem${_liC20.Value}].Value}]}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    | simplify the name
    /varset _tmpName ${maItem.Find[stItem${_liC20.Value}].Value}

    | detour item name to epic if needed
    | class epic 1
    /if (${maItem.Find[stItem${_liC20.Value}].Value.Equal[epic1]}) {
      /varset _tmpName ${sqlite.Result[epic${Me.Class.ShortName} 1 name]}
    | class epic 1.5
    } else /if (${maItem.Find[stItem${_liC20.Value}].Value.Equal[epic15]}) {
      /varset _tmpName ${sqlite.Result[epic${Me.Class.ShortName} 2 name]}
    | class epic 2
    } else /if (${maItem.Find[stItem${_liC20.Value}].Value.Equal[epic2]}) {
      /varset _tmpName ${sqlite.Result[epic${Me.Class.ShortName} 3 name]}
    }
    DEBUG _tmpName${sep}\a-w${_tmpName}\ax

    /call is_ready ${_debug} "${_tmpName}"
    /if (!${Macro.Return}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Item ${_liC20.Value} ${Me.DisplayName} "${_tmpName}" check_item_cycle
    /if (!${Macro.Return}) {
      DEBUG ${break}item failed tag
      /invoke ${_liC20.Advance}
      /continue
    }

    DEBUG ${sep}_liC20.Value${sep}\a-w${_liC20.Value}\ax${sep}passed tags
    /delay 1
    /if (${lsItem${_liC20.Value}Tag.Contains[self]}) {
      /if (!${Me.Buff[${_tmpName}].ID}]}) {
        /varset _tmpID ${Me.ID}
      }
    }

    /if (${FindItem[${_tmpName}].Spell.Duration.TotalSeconds} && ${FindItem[${_tmpName}].Spell.Range}) {
      /invoke ${target[set, ${_tmpID}]}
      /if (${Target.BuffDuration[${_tmpName}].TotalSeconds}) {
        DEBUG ${break}target has buff
        /invoke ${_liC20.Advance}
        /continue
      }
    }

    /if (${target[isdead, ${_tmpID}]}) {
      /return FALSE
    }

    /call voodoo ${_debug} "${_tmpName}" ${_tmpID} FALSE

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: casts nukes from maNuke
 * use: /call cast_cycle_nuke DEBUG [TARGETID] [TAG]
 ***|
sub cast_cycle_nuke(bool _debug, int _tmpID, string _tag)
  DEBUG \atcast_cycle_nuke\ax(\a-w${_tmpID}, ${_tag}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineNuke.Contains[EMPTY]} && ${lsLineNuke.Count} == 1) {
    DEBUG ${sep}resetting the empty nuke line
    /invoke ${lsLineNuke.Clear}
    /invoke ${lsLineNuke.Append[${stCount20}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}
  
  | redirect to pb ae nukes if conditions met
  /if (AE#) {
    /if (${SpawnCount[npc radius ENV PS4]} >= AE#) {
      /declare _liae listiterator local
      /vardata _liae lsLineNuke.First.Clone
      /while (!${_liae.IsEnd}) {

        | check for pbaenuke
        /if (!${Bool[${maNuke.Find[swNuke${_liae.Value}].Value}]} || !${Bool[${maNuke.Find[stNuke${_liae.Value}].Value}]} || !${lsNuke${_liae.Value}Tag.Contains[pbaenuke]}) {
          /invoke ${_liae.Advance}
          /continue
        }

        | if we find an available ae nuke, and it is ready, temp reset the line to just that spell to save the next loop cycle
        /if (${is_ready[${_debug}, "${maNuke.Find[stNuke${_liae.Value}].Value}"]}) {
          /invoke ${lsLineNuke.Clear}
          /invoke ${lsLineNuke.Append[${_liae.Value}]}
          /varset _tag pbaenuke
          /varset _tmpID 0
          /break
        }    
        /invoke ${_liae.Advance}
      }
    }
  }

  /declare _count int local 0
  /declare _match bool local FALSE
  /declare _liNuke listiterator local
  /vardata _liNuke lsLineNuke.First.Clone
  /while (!${_liNuke.IsEnd}) {

   /if (${Me.PctMana} < 2 && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
      /return FALSE
    }
    
    GETINPUT cast_cycle_nuke NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_nuke

    /if (${target[isdead, ${_tmpID}]}) {
      /return FALSE
    }

    | stopnuke %
    /if (${maNuke.Find[stPctStopNuke].Value}) {
      /if (${Spawn[${_tmpID}].PctHPs} <= ${maNuke.Find[stPctStopNuke].Value}) {
        /return FALSE
      }
    }

    /invoke ${target[set, ${_tmpID}]}

    | check the switch
    /if (!${Bool[${maNuke.Find[swNuke${_liNuke.Value}].Value}]}) {
      /invoke ${_liNuke.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maNuke.Find[stNuke${_liNuke.Value}].Value}]}) {
      /invoke ${_liNuke.Advance}
      /continue
    }

    | check matching tag for failure
    /if (${Bool[${_tag}]}) {
      /if (!${lsNuke${_liNuke.Value}Tag.Contains[${_tag}]}) {
        /invoke ${_liNuke.Advance}
        /continue
      }
    }

    /if (!${is_ready[${_debug}, "${maNuke.Find[stNuke${_liNuke.Value}].Value}"]}) {
      /invoke ${_liNuke.Advance}
      /continue
    }

    | skip twincast nukes (they are accessed a different way)
    /if (${lsNuke${_liNuke.Value}Tag.Contains[twincast]} && ${_tag.NotEqual[twincast]}) {
      /invoke ${_liNuke.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Nuke ${_liNuke.Value} ${Me.DisplayName} "${maNuke.Find[stNuke${_liNuke.Value}].Value}" cast_cycle_nuke
    /if (!${Macro.Return}) {
      /invoke ${_liNuke.Advance}
      /continue
    }

    | totem type nukes?
    /if (${lsNuke${_liNuke.Value}Tag.Contains[totem]}) {
      | necro ae dps aura spawns
      /if (${Select[${Me.Class.ShortName},NEC]}) {
        /if (${Me.Aura[1].ID}) {
          /invoke ${_liNuke.Advance}
          /continue
        }

      | shaman totems
      } else /if (${Select[${Me.Class.ShortName},SHM]}) {
        /for _count 1 to ${SpawnCount[radius ENV]}
          /if (${NearestSpawn[${_count}].DisplayName.Equal[Soul Idol]} || ${NearestSpawn[${_count}].DisplayName.Equal[Spirit Idol]}) {
            /varset _match TRUE
            /break
          }
        /next _count

        /if (${_match}) {
          /varset _match FALSE
          /invoke ${_liNuke.Advance}
          /continue
        }

      } else /if (${Select[${Me.Class.ShortName},ENC]}) {

      }
    }

    /call voodoo ${_debug} "${maNuke.Find[stNuke${_liNuke.Value}].Value}" ${_tmpID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},OUTOFMANA,STUNNED]} && !${maNuke.Find[stNukeDelay].Value}) {
      /break
    }

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED]} && !${maNuke.Find[stNukeDelay].Value}) {
      /break
    }

    | forced delay
    /if (${maNuke.Find[stNukeDelay].Value}) {
      /invoke ${set_data_timer[${_debug}, Nuke_Delay, RESTART]}
    }

    | maintain the loop of nukes till all are cycled.
    /if (${maNuke.Find[stNukeDelay].Value}) {
      /delay 10s !${Me.Casting.ID}
      /delay 2.5s
      /delay ${timer_Nuke_Delay}
    }

    /invoke ${_liNuke.Advance}
  }

/return TRUE



|***
 * note: casts dots from maDoT
 * use: /call cast_cycle_dot DEBUG [TARGETID]
 ***|
sub cast_cycle_dot(bool _debug, int _tmpID)
  DEBUG \atcast_cycle_dot\ax(\a-w${_tmpID}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineDoT.Contains[EMPTY]} && ${lsLineDoT.Count} == 1) {
    DEBUG ${sep}resetting the empty dot line
    /invoke ${lsLineDoT.Clear}
    /invoke ${lsLineDoT.Append[${stCount20}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /declare _liC20 listiterator local
  /vardata _liC20 lsLineDoT.First.Clone
  /while (!${_liC20.IsEnd}) {

    /if (${Me.PctMana} < 2) {
      DEBUG ${sep}<2%manna
      /return FALSE
    }

    /if (${target[isdead, ${_tmpID}]}) {
      DEBUG ${sep}target dead
      /return FALSE
    }

    | stopdot %
    /if (${maDoT.Find[stPctStopDoT].Value}) {
      /if (${Spawn[${_tmpID}].PctHPs} <= ${maDoT.Find[stPctStopDoT].Value}) {
        DEBUG ${sep}${Spawn[${_tmpID}].PctHPs} <= ${maDoT.Find[stPctStopDoT].Value}
        /return FALSE
      }
    }

    | TTL evaluations
    /if (${maTTL.Find[swTTL].Value}) {
      /if (${maTTL.Contains[${Spawn[${_tmpID}].DisplayName}]}) {
        DEBUG ${sep}ttl
        | check overall durations of the average kill +2 tics over the spell duration time
        /if (${Spell[${Spell[${maDoT.Find[stDoT${_liC20.Value}].Value}].RankName}].Duration} > ${Math.Calc[${maTTL.Find[${Spawn[${_tmpID}].DisplayName}].Value.Arg[2,|]} + (${maTTL.Find[${Spawn[${_tmpID}].DisplayName}].Value.Arg[2,|]}*.5)]}) {
          /invoke ${_liC20.Advance}
          /continue
        }
        | if mob is less then 50% hp and spell duration is > 1/2 your average kill time
        /if (${Spawn[${_tmpID}].PctHPs} < 50 && ${Spell[${Spell[${maDoT.Find[stDoT${_liC20.Value}].Value}].RankName}].Duration} > ${Math.Calc[${maTTL.Find[${Spawn[${_tmpID}].DisplayName}].Value.Arg[2,|]} / 2]}) {
          /invoke ${_liC20.Advance}
          /continue
        }
      }
    }

    GETINPUT cast_cycle_dot NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_dot

    /if (${target[isdead, ${_tmpID}]}) {
      DEBUG ${sep}dead2
      /return FALSE
    }

    /invoke ${target[set, ${_tmpID}]}

    /if (${Target.ID} == ${_tmpID}) {
      /if (${Target.BuffDuration[${maDoT.Find[stDoT${_liC20.Value}].Value}].TotalSeconds} >= ${maDoT.Find[stDotRefresh].Value}) {
        DEBUG ${sep}dot refresh fail
        /invoke ${_liC20.Advance}
        /continue
      }
    } else {
      /return FALSE
    }

    | check the switch
    /if (!${Bool[${maDoT.Find[swDoT${_liC20.Value}].Value}]}) {
      DEBUG ${sep}switch off${sep}\a-w${maDoT.Find[stDoT${_liC20.Value}].Value}\ax
      /invoke ${_liC20.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maDoT.Find[stDoT${_liC20.Value}].Value}]}) {
      DEBUG ${sep}no name${sep}\a-w${maDoT.Find[stDoT${_liC20.Value}].Value}\ax
      /invoke ${_liC20.Advance}
      /continue
    }

    /if (!${is_ready[${_debug}, "${maDoT.Find[stDoT${_liC20.Value}].Value}"]}) {
      DEBUG ${sep}not ready${sep}\a-w${maDoT.Find[stDoT${_liC20.Value}].Value}\ax
      /invoke ${_liC20.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} DoT ${_liC20.Value} ${Me.DisplayName} "${maDoT.Find[stDoT${_liC20.Value}].Value}" cast_cycle_dot
    /if (!${Macro.Return}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    /call voodoo ${_debug} "${maDoT.Find[stDoT${_liC20.Value}].Value}" ${_tmpID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
      /break
    }

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: casts skills from maAgro for engine2
 * use: /call cast_cycle_agro DEBUG [TARGETID]
 ***|
sub cast_cycle_agro(bool _debug, int _tmpID)
  DEBUG \atcast_cycle_agro\ax(\a-w${_tmpID}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineAgro.Contains[EMPTY]} && ${lsLineAgro.Count} == 1) {
    DEBUG ${sep}resetting the empty agro line
    /invoke ${lsLineAgro.Clear}
    /invoke ${lsLineAgro.Append[${stCount20}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /declare _liC20 listiterator local
  /vardata _liC20 lsLineAgro.First.Clone
  /while (!${_liC20.IsEnd}) {

    /if (${target[isdead, ${_tmpID}]}) {
      DEBUG ${sep}target dead
      /return FALSE
    }

    GETINPUT cast_cycle_agro NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_agro

    /invoke ${target[set, ${_tmpID}]}

    | check the switch
    /if (!${Bool[${maAgro.Find[swAgro${_liC20.Value}].Value}]}) {
      DEBUG ${sep}switch off${sep}\a-w${maAgro.Find[stAgro${_liC20.Value}].Value}\ax
      /invoke ${_liC20.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maAgro.Find[stAgro${_liC20.Value}].Value}]}) {
      DEBUG ${sep}no name${sep}\a-w${maAgro.Find[stAgro${_liC20.Value}].Value}\ax
      /invoke ${_liC20.Advance}
      /continue
    }

    /if (!${is_ready[${_debug}, "${maAgro.Find[stAgro${_liC20.Value}].Value}"]}) {
      DEBUG ${sep}not ready${sep}\a-w${maAgro.Find[stAgro${_liC20.Value}].Value}\ax
      /invoke ${_liC20.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Agro ${_liC20.Value} ${Me.DisplayName} "${maAgro.Find[stAgro${_liC20.Value}].Value}" cast_cycle_agro
    /if (!${Macro.Return}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    /call voodoo ${_debug} "${maAgro.Find[stAgro${_liC20.Value}].Value}" ${_tmpID} FALSE

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: mash dps alternative Engine 2/3
 * use: /call cast_cycle_miscdps DEBUG [TARGETID]
 ***|
sub cast_cycle_miscdps(bool _debug, int _tmpID)
  DEBUG \atcast_cycle_miscdps\ax(\a-w${_tmpID}\ax)


  | if the user has not set the line order, set it to a default for now
  /if (${lsLineMiscdps.Contains[EMPTY]} && ${lsLineMiscdps.Count} == 1) {
    DEBUG ${sep}resetting the empty MiscDPS line
    /invoke ${lsLineMiscdps.Clear}
    /invoke ${lsLineMiscdps.Append[${stCount50}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /declare _flagtmpID bool local FALSE
  /declare _liC50 listiterator local
  /vardata _liC50 lsLineMiscdps.First.Clone
  /while (!${_liC50.IsEnd}) {

    /if (${target[isdead, ${_tmpID}]}) {
      DEBUG ${sep}target dead
      /return FALSE
    }

    GETINPUT cast_cycle_miscdps NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_miscdps

    /invoke ${target[set, ${_tmpID}]}

    | check the switch
    /if (!${Bool[${maMiscDPS.Find[swMiscDPS${_liC50.Value}].Value}]}) {
      DEBUG ${sep}switch off${sep}\a-w${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}\ax
      /invoke ${_liC50.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}]}) {
      DEBUG ${sep}no name${sep}\a-w${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}\ax
      /invoke ${_liC50.Advance}
      /continue
    }

    /if (!${is_ready[${_debug}, "${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}"]}) {
      DEBUG ${sep}not ready${sep}\a-w${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}\ax
      /invoke ${_liC50.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} MiscDPS ${_liC50.Value} ${Me.DisplayName} "${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}" cast_cycle_miscdps
    /if (!${Macro.Return}) {
      /invoke ${_liC50.Advance}
      /continue
    }

    /call voodoo ${_debug} "${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}" ${_tmpID} FALSE

    | reset _tmpID
    /if (${_flagtmpID}) {
      /call get_assist_target
      /varset _tmpID TARGET
      /varset _flagtmpID FALSE
    }

    /invoke ${_liC50.Advance}
  }

/return TRUE



|***
 * note: casts burns from maBurn
 * use: /call cast_cycle_burn DEBUG [TARGETID]
 ***|
sub cast_cycle_burn(bool _debug, int _tmpID)
  DEBUG \atcast_cycle_burn\ax(\a-w${_tmpID}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineBurn.Contains[EMPTY]} && ${lsLineBurn.Count} == 1) {
    DEBUG ${sep}resetting the empty burn line
    /invoke ${lsLineBurn.Clear}
    /invoke ${lsLineBurn.Append[${stCount20}]}
  }

  /declare _tmpName string local
  /declare _liC20 listiterator local
  /vardata _liC20 lsLineBurn.First.Clone
  /while (!${_liC20.IsEnd}) {

    GETINPUT cast_cycle_burn NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_burn

    /if (${target[isdead, ${_tmpID}]}) {
      /return FALSE
    }

    /invoke ${target[set, ${_tmpID}]}

    | check the switch
    /if (!${Bool[${maBurn.Find[swBurn${_liC20.Value}].Value}]}) {
      DEBUG ${break}#${_liC20.Value}${sep}${maBurn.Find[swBurn${_liC20.Value}].Value
      /invoke ${_liC20.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maBurn.Find[stBurn${_liC20.Value}].Value}]}) {
      DEBUG ${break}BAD NAME (no name)
      /invoke ${_liC20.Advance}
      /continue
    }

    | simplify the name
    /varset _tmpName ${maBurn.Find[stBurn${_liC20.Value}].Value}

    | detour item name to epic if needed
    | class epic 1
    /if (${maBurn.Find[stBurn${_liC20.Value}].Value.Equal[epic1]}) {
      /varset _tmpName ${sqlite.Result[epic${Me.Class.ShortName} 1 name]}
    | class epic 1.5
    } else /if (${maBurn.Find[stBurn${_liC20.Value}].Value.Equal[epic15]}) {
      /varset _tmpName ${sqlite.Result[epic${Me.Class.ShortName} 2 name]}
    | class epic 2
    } else /if (${maBurn.Find[stBurn${_liC20.Value}].Value.Equal[epic2]}) {
      /varset _tmpName ${sqlite.Result[epic${Me.Class.ShortName} 3 name]}
    }
    DEBUG _tmpName${sep}\a-w${_tmpName}\ax

    /if (!${is_ready[${_debug}, "${_tmpName}"]}) {
      DEBUG ${break}\a-w${_tmpName}\ax${sep}NOT READY
      /invoke ${_liC20.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Burn ${_liC20.Value} ${Me.DisplayName} "${_tmpName}" cast_cycle_burn
    /if (!${Macro.Return}) {
      /invoke ${_liC20.Advance}
      /continue
    }

    /call voodoo ${_debug} "${_tmpName}" ${_tmpID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},OUTOFMANA,STUNNED,SUCCESS,DISTRACTED]} && !${maBurn.Find[stBurnDelay].Value}) {
      DEBUG ${break}CR${sep}${maData.Find[stCastReturn].Value}
      /break
    }

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: cycle bandoliers from maBandolier
 * use: /call cycle_sash DEBUG
 ***|
sub cycle_sash(bool _debug)
  DEBUG \atcycle_sash\ax()

  /if (!${maSash.Find[swSash].Value}) {
    /return FALSE
  }

  | dont screw up the proc sash swap
  /if (${maSash.Find[swSashProc].Value} && ${Me.Bandolier[${maSash.Find[stSashProc].Value}].Active}) {
    /declare _buff string local ${FindItem[${Me.Bandolier[${maSash.Find[stSashProc].Value}].Item[1]}].Spell}
    /if (!${Me.Buff[${_buff}].ID} && !${Me.Song[${_buff}].ID} && !${Target.Buff[${_buff}].ID}) {
      /return FALSE
    }
  }

  /declare _condition bool local FALSE
  /declare _tag bool local FALSE

  /declare _liC12 listiterator local
  /vardata _liC12 lsCount12.First.Clone
  /while (!${_liC12.IsEnd}) {

    GETINPUT cast_cycle_bandolier NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_bandolier

    DEBUG ${sep}checking-${_liC12.Value}${sep}\a-wswSash${_liC12.Value}\ax${sep}${maSash.Find[swSash${_liC12.Value}].Value}

    | check the switch
    /if (!${maSash.Find[swSash${_liC12.Value}].Value}) {
      DEBUG ${sep}swSash${_liC12.Value} (${maSash.Find[swSash${_liC12.Value}].Value})${sep}\a-w${maSash.Find[swSash${_liC12.Value}].Value}\ax
      /invoke ${_liC12.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maSash.Find[stSash${_liC12.Value}].Value}]}) {
      DEBUG ${sep}stSash${_liC12.Value}.Value${sep}\a-w${Bool[${maSash.Find[stSash${_liC12.Value}].Value}]}\ax
      /invoke ${_liC12.Advance}
      /continue
    }

    /varset _condition FALSE
    /varset _tag FALSE

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Sash ${_liC12.Value} ${Me.DisplayName} "${maSash.Find[stSash${_liC12.Value}].Value}" cycle_sash
    /if (!${Macro.Return}) {
      /continue
    }

    /if (!${_condition} && ${_tag} && !${Me.Bandolier[${maSash.Find[stSash${_liC12.Value}].Value}].Active}) {
      DEBUG ${sep}${maSash.Find[stSash${_liC12.Value}].Value}${sep}\a-wactivating\ax
      /invoke ${Me.Bandolier[${maSash.Find[stSash${_liC12.Value}].Value}].Activate}
      /return TRUE
    } else /if (${_condition} && ${_tag} && ${Me.Bandolier[${maSash.Find[stSash${_liC12.Value}].Value}].Active}) {
      DEBUG ${sep}${maSash.Find[stSash${_liC12.Value}].Value}${sep}\a-walready active\ax
      /return TRUE
    }

    /invoke ${_liC12.Advance}
  }

/return TRUE



|***
 * note: watches to see if you are deaded. if so.. theres a couple things we need to do.
 * use: ${is_me_dead[DEBUG]}
 ***|
sub is_me_dead(bool _debug)

  /doevents REZ_ACCEPT

  | dont check in manual
  /if (!AUTO) {
    /return FALSE
  }

  | GETINPUT is_me_dead swIsMeDead|${swIsMeDead}

  /if (${swIsMeDead}) {
    /return TRUE
  }

  | check death states
  /if (${Me.State.Equal[HOVER]} || ${Me.Hovering}) {
    /varset swIsMeDead TRUE
  }

  | did I just die and get sent to bind ?
  /if ((${Me.Buff[Resurrection Sickness].ID} || ${Me.Buff[Revival Sickness].ID}) && ${Zone.ID} == ${Me.ZoneBound.ID}) {
    /if (${Me.BoundLocation[0].Y} == ${Me.Y} && ${Me.BoundLocation[0].X} == ${Me.X} && ${Me.BoundLocation[0].Z} == ${Me.Z}) {
      /varset swIsMeDead TRUE
    }
  }

  | if we died, shut shit down
  /if (${swIsMeDead}) {

    | map my death
    /call control_rez ${maDebug.Find[rez].Value} add ${Me.ID} dead
    | /dgzexecute /invoke ${maControlRez.Add[${Me.ID},dead]}

    /varset stAssTarID 0
    /invoke ${maCC.Add[swValidAssTarID,FALSE]}
    /invoke ${maCC.Add[swSetCombat,FALSE]}
    /invoke ${maCC.Add[swSetForced,FALSE]}
    /invoke ${set_data_timer[${_debug}, Check_Behind, 0]}

    /doevents flush

    | stop all the things(s)
    /if (${maHome.Find[swMoveDeathClear].Value}) {
      /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
    }

    | /call set_stop ${maDebug.Find[stop].Value} SILENT
    /invoke ${target[clear]}
    /if (${Me.Casting.ID}) {
      /stopcast
    }

  }

/return ${swIsMeDead}



|***
 * note: looks for dead fuckers. if we find dead fuckers, and they fall under dead fucker res status, we rez said dead fuckers
 * use: /call check_for_corpse DEBUG
 ***|
sub check_for_corpse(bool _debug)
  DEBUG \atcheck_for_corpse\ax()

  /if (${_debug}) /delay 1

  /if (${Me.State.Equal[HOVER]} || ${Me.Hovering} || ${Me.Dead}) {
    DEBUG ${break}i'm fucking dead too.
    /return FALSE
  }

  | fuck your corpse
  /if (!${maRez.Find[swRezIC].Value} && !${maRez.Find[swRezOOC].Value}) {
    DEBUG ${break}no rezing enabled
    /return FALSE
  }

  | no one dead
  /if (!${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}) {
    DEBUG ${break}no one is dead
    /return FALSE
  }

  | no combat rez
  /if (${Select[${Me.CombatState},combat]}) {
    /if (!${maRez.Find[swRezIC].Value}) {
      DEBUG ${break}no rezing in combat
      /return FALSE
    }
  }

  | no out of combat combat rez
  /if (!${Select[${Me.CombatState},combat]}) {
    /if (!${maRez.Find[swRezOOC].Value}) {
      DEBUG ${break}no rezing out of combat combat
      /return FALSE
    }
  }

  | not enough manna for a rez
  /if (${Me.PctMana} < 2 && !${maRez.Find[swRezToken].Value}) {
    DEBUG ${break}manna too low
    /return FALSE
  }

  | no emergency tokens AND dont use them when a cleric is around
  /if (${maRez.Find[swRezToken].Value}) {
    /if (!${Me.ItemReady[Token of Resurrection]}) {
      /return FALSE
    }
    /if (${Spawn[pc CLR radius ${maRez.Find[stMaxRezRange].Value}].ID}) {
      /return FALSE
    }

    | disable rezemergency if you dont have tokens
    /if (!${FindItemCount[=Token of Resurrection]}) {
      DEBUG ${sep}shutting down token rezing
      /invoke ${do_raw_edit[FALSE, SILENT, swRezToken, FALSE]}
      /invoke ${out[0, 0, 59]}
    }
  }

  /declare _rezReady bool local FALSE
  /declare _tokenReady bool local FALSE
  /declare _rez string local

  | get thing to rez with in combat
  /if (${Select[${Me.CombatState},combat]}) {
    /declare _liic listiterator local
    /vardata _liic lsRezIC.First.Clone
    /while (!${_liic.IsEnd}) {
      /if (${is_ready[${_debug}, "${_liic.Value}"]}) {
        /varset _rez ${_liic.Value}
        /varset _rezReady TRUE
        /break
      }
      /invoke ${_liic.Advance}
    }

  | something to rez with out of combat? should let the corpses rot...0
  } else /if (!${Select[${Me.CombatState},combat]}) {
    /declare _liooc listiterator local
    /vardata _liooc lsRezOOC.First.Clone
    /while (!${_liooc.IsEnd}) {
      /if (${is_ready[${_debug}, "${_liooc.Value}"]}) {
        /varset _rez ${_liooc.Value}
        /varset _rezReady TRUE
        /break
      }
      /invoke ${_liooc.Advance}
    }

  }

  /if (${maRez.Find[swRezToken].Value}) {
    /declare _rezEmergency string local "Token of Resurrection"
    /if (${is_ready[${_debug}, "${_rezEmergency}"]}) {
      /varset _tokenReady TRUE
    }
  }

  | still good???
  /if (!${_rezReady} && !${_tokenReady}) {
    DEBUG ${break}fail${sep}\a-wno rez type available\ax
    /return FALSE
  }

  SETHUD "resurrect"

  /declare _tmpID int local 0
  /declare _increment int local 1
  /declare _found bool local FALSE
  /declare _liC8 listiterator local
  /vardata _liC8 lsLineRez.First.Clone

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineRez.Contains[EMPTY]} && ${lsLineRez.Count} == 1) {
    DEBUG ${sep}resetting the empty dot line
    /invoke ${lsLineRez.Clear}
    /invoke ${lsLineRez.Append[1|2|3|4|5|6|7|8]}
  }

  | check priorities
  /while (!${_liC8.IsEnd}) {
    DEBUG ${sep}_liC8.Value${sep}${_liC8.Value}${sep}\a-w${maRez.Find[stRez${_liC8.Value}].Value}\ax

    | check the switch
    /if (!${maRez.Find[swRez${_liC8.Value}].Value}) {
      DEBUG ${sep}switch off${sep}\a-w${maRez.Find[swRez${_liC8.Value}].Value}\ax
      /invoke ${_liC8.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maRez.Find[stRez${_liC8.Value}].Value}]}) {
      DEBUG ${sep}no name${sep}\a-w${swRez.Find[stDoT${_liC8.Value}].Value}\ax
      /invoke ${_liC8.Advance}
      /continue
    }

    | any corpses matching this priority? if not, we move to the next priority, else, tag the closest toon and get to work
    /if (!${SpawnCount[${maRez.Find[stRez${_liC8.Value}].Value} radius ${maRez.Find[stMaxRezRange].Value}]}) {
      /invoke ${_liC8.Advance}
      /continue
    } else {

      | find us a corpse!
      /while (!${_found}) {
        /varset _tmpID ${NearestSpawn[${_increment},${maRez.Find[stRez${_liC8.Value}].Value} radius ${maRez.Find[stMaxRezRange].Value}].ID}
        DEBUG ${sep}_tmpID${sep}\a-w${NearestSpawn[${_increment},${maRez.Find[stRez${_liC8.Value}].Value} radius ${maRez.Find[stMaxRezRange].Value}].ID}\ax (\a-r${maRez.Find[stRez${_liC8.Value}].Value}\ax)

        | theres an error, get out
        /if (!${_tmpID}) {
          /break
        }

        | fix any timer
        /if (!${Defined[timer_corpse_${_tmpID}]}) {
          /declare timer_corpse_${_tmpID} timer outer 1
          /delay 1
        }

        | for the people that suck ass (not in a good way)
        /if (${lsNoRezToon.Contains[${Spawn[${_tmpID}].DisplayName}]}) {
          /varset timer_corpse_${_tmpID} ${maHard.Find[stRezIgnoreFucktards].Value}
          /varset _tmpID 0
        }

        | if there is no id, or the current corpse has a marker on it, move on
        /if (!${_tmpID} || ${timer_corpse_${_tmpID}}) {
          DEBUG ${sep}badID or timer set. next corpse
          /varcalc _increment ${_increment} + 1
        } else {
          DEBUG found good corpse
          /varset _found TRUE
        }
      }
    }

    | cleric rezing
    /if (${Select[${Me.Class.ShortName},CLR]}) {

      | divine rez another cleric?
      /if (${maRez.Find[swDivineCleric].Value} && ${Select[${Spawn[id ${_tmpID}].Class.ShortName},CLR]}) {
        /if (${Me.AltAbilityReady[${maRez.Find[stAADivineResurrection].Value}]} || ${Me.ItemReady[${maRez.Find[stAADivineResurrection].Value}]}) {
          /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${maRez.Find[stAADivineResurrection].Value}
          /return TRUE
        }
      | normal rez
      } else {
        /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} "${_rez}"
        /return TRUE
      }

    | other rez classes
    } else /if (${Select[${Me.Class.ShortName},DRU,PAL,SHM,NEC]}) {
      /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} "${_rez}"
      /return TRUE

    | token rezing
    } else /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
      /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} "${_rezEmergency}"
      /return TRUE
    }

    /invoke ${_liC8.Advance}
  }

  | end of priority rezing

  | dannet members
  /if (${maRez.Find[swRezDanNet].Value}) {
    DEBUG ${sep}swRezDanNet rez
    /declare _countD int local 0
    /for _countD 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}

      /if (!${Spawn[${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}].Dead}) {
        DEBUG ${sep}${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} dead${sep}\a-w${Spawn[${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}].Dead}\ax
        /continue
      } else /if (${Spawn[${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}].Dead}) {
        /varset _tmpID ${Spawn[${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}].ID}
      }

      | mark the toon if it isnt already
      /if (!${Defined[timer_corpse_${_tmpID}]}) {
        DEBUG ${sep}timer_corpse_${_tmpID}
        /declare timer_corpse_${_tmpID} timer outer 0
        /delay 1
      } else /if (${timer_corpse_${_tmpID}}) {
      DEBUG ${sep}timer_corpse_${_tmpID} == ${timer_corpse_${_tmpID}}
        /continue
      }

      /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
        /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} "${_rezEmergency}"
        /break
      } else {
        /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} "${_rez}"
        /break
      }
    /next _countD
  }

  | fuck it.. rez everyone!
  | needs /rez everyone switch to be true
  /if (${maRez.Find[swRezEveryone].Value}) {
    DEBUG ${sep}swRezEveryone rez
    /declare _countE int local 0
    /if (${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}) {
      /for _countE 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}
      /varset _tmpID ${NearestSpawn[${_countE}, pccorpse radius ${maRez.Find[stMaxRezRange].Value}].ID}

      /if (!${Defined[timer_corpse_${_tmpID}]}) {
        DEBUG ${sep}timer_corpse_${_tmpID}
        /declare timer_corpse_${_tmpID} timer outer 0
        /delay 1
      } else /if (${timer_corpse_${_tmpID}}) {
        /continue
      }

      /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
        /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rezEmergency}
        /break
      } else {
        /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} "${_rez}"
        /break
      }
      /next _countE
    }
  }

  SETHUD EMPTY

/return TRUE



|***
 * note: casts trez thingies and sets any needed timers for dead fuckers
 * use: /call cast_rez DEBUG [ID] [rez aa|token]
 ***|
sub cast_rez(bool _debug, int _tmpID, string _rez)
  DEBUG \atcast_rez\ax(\a-w${_tmpID}, ${_rez}\ax)

  /if (${Me.Dead}) {
    DEBUG ${break}i'm dead.. wtf..
    /return FALSE
  }
  /if (!${Defined[timer_corpse_${_tmpID}]}) {
    DEBUG ${break}no timer for${sep}\a-wtimer_corpse_${_tmpID}\ax
    /return FALSE
  }
  /if (${timer_corpse_${_tmpID}}) {
    DEBUG ${break}timer not empty${sep}\a-wtimer_corpse_${_tmpID}\ax
    /return FALSE
  }

  ISMEDEAD
  CHECKEXIT cast_rez ${_tmpID}|${_rez}

  SETHUD "resurrect"

  | ignore the people we dont like
  /if (${lsNoRezToon.Contains[${Spawn[pccorpse ${_tmpID}].DisplayName}]}) {
    DEBUG ${break}on go fuck yourself list${sep}\a-w${Spawn[pccorpse ${_tmpID}].DisplayName}]}\ax
    /invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezIgnoreFucktards].Value}]}
    /return FALSE
  }

  | get the corpse close to us
  | /if (${Spawn[pccorpse ${_tmpID}].Distance} <= ${maHard.Find[stRezSummonRange].Value}) {
  |   DEBUG ${sep}/corpse${sep}\a-wid ${_tmpID}\ax
  |   /target id ${_tmpID}
  |   /delay 1
  |   /squelch /corpse
  | }

  | can we AA res or token, maybe spell?
  /if (${Me.AltAbilityReady[${_rez}]} || (${Me.ItemReady[Token of Resurrection]} && ${maRez.Find[swRezToken].Value}) || ${Me.SpellReady[${_rez}]} || ${Me.ItemReady[${_rez}]}) {

    | not token rez rez
    /if (!${maRez.Find[swRezToken].Value}) {
      DEBUG ${sep}casting${sep}\a-w${_rez}\ax
      /delay 5s !${Me.Casting.ID}

      /squelch /target id ${_tmpID}
      /delay 1s ${Target.ID} == ${_tmpID}

      /call voodoo ${_debug} "${_rez}" ${_tmpID} skiptargetcheck
      /delay 5
      /delay 5s !${Me.Casting.ID}

    | token rez
    } else /if (${maRez.Find[swRezToken].Value}) {
      DEBUG ${sep}casting${sep}\a-wclicking token\ax
      /delay 5s !${Me.Casting.ID}

      /squelch /target id ${_tmpID}
      /delay 1s ${Target.ID} == ${_tmpID}

      /varset _rez "Token of Resurrection"
      /call voodoo ${_debug} "Token of Resurrection" ${_tmpID} skiptargetcheck
      /delay 5
      /delay 5s !${Me.Casting.ID}

    }

    /doevents

    | if cast was good, set the timers.
    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
      /invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezSuccess].Value}]}
      /invoke ${out[0, "Rez Success${sep}${con_color[${_debug}, ${_tmpID}]}"]}
    } else {
      /if (${Select[${Me.Class.ShortName},CLR,DRU,SHM,PAL,NEC]}) {
        /invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezFail].Value}]}
      } else {
        /invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezFailToken].Value}]}
      }
    }

  }

  SETHUD EMPTY

/return TRUE



|***
 * note: rez control system. to ensure only 1 person trys to rez a corpse
 * use:
 ***|
sub control_rez(bool _debug, string _action, int _ID, string _flag)
  DEBUG\atcontrol_rez\ax(\a-w\ax)


  /if (${_action.Equal[add]}) {

    /dgzexecute /invoke ${maControlRez.Add[${_ID},${_flag}]}

  } else /if (${_action.Equal[delete]}) {
    /dgzexecute /invoke ${maControlRez.Remove[${_ID}]}

  }

/return



|***
 * note: watches for spawns in the zone
 * use:
 ***|
sub check_watch_spawn(bool _debug)
  DEBUG \atcheck_watch_spawn\ax()

  | /declare _correctWatchCount[${lsWatch.Count}] int local
  | /declare _count int local 1
  /declare _liwa listiterator local
  /vardata _liwa lsWatch.First.Clone

  /while (!${_liwa.IsEnd}) {

    | /varset _correctWatchCount[${_count}] ${SpawnCount[npc ${_liwa.Value}]}

    |** count mismatch
    /if (${maWatch.Find[stWatchCount].Value} != ${_correctWatchCount[${_count}]}) {
      /invoke ${maWatch.Add[watchSpawnCount,${_correctWatchCount}]}
      /if (${maWatch.Find[swWatchBeep].Value}) {
        /beep
      }
      /squelch /mapshow ${_liwa.Value}
    }

    **|
    /squelch /mapshow ${_liwa.Value}
    | /varcalc _count ${_count} +1
    /invoke ${_liwa.Advance}
  }

  /invoke ${set_data_timer[${_debug}, Check_Watch, RESTART]}

/return TRUE



|***
 * note: regeneration spells (not the AAs)
 * use: /call cast_cycle_heal_regen DEBUG [healgrouphot|healhot]
 ***|
sub cast_cycle_heal_regen(bool _debug, int _tmpID, string _tag)
  DEBUG \atcast_cycle_heal_regen\ax(\a-w${_tmpID}, ${_tag}\ax)

  /invoke ${set_data_timer[${_debug}, Check_Regen, RESTART]}

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineHeal.Contains[EMPTY]} && ${lsLineHeal.Count} == 1) {
    DEBUG ${sep}resetting the empty heal line
    /invoke ${lsLineHeal.Clear}
    /invoke ${lsLineHeal.Append[${stCount50}]}
  }

  /declare _myHoT string local FALSE
  /declare _liHoT listiterator local
  /vardata _liHoT lsLineHeal.First.Clone

  | group heal over time
  /if (${_tag.Equal[grouphot]}) {
    DEBUG ${sep}tag.Found

    | find my hot
    /while (!${_liHoT.IsEnd}) {

      | break on no mobs in area
      /if (AE# && ${SpawnCount[npc radius ENV PS4 targetable]} < AE#) {
        DEBUG ${break}not enough mobs in env rad
        /return FALSE
      }
      
      /if (TIE) {
        DEBUG ${break}tie called
        /return FALSE        
      }

      CHECKEXIT
      CHECKDEAD
      
      | check the switch
      /if (!${maHeal.Find[swHeal${_liHoT.Value}].Value}) {
        /invoke ${_liHoT.Advance}
        /continue
      }

      | check the item name
      /if (!${Bool[${maHeal.Find[stHeal${_liHoT.Value}].Value}]}) {
        /invoke ${_liHoT.Advance}
        /continue
      }

      | check matching tag
      /if (!${lsHeal${_liHoT.Value}Tag.Contains[grouphot]}) {
        DEBUG ${break}wrongtag
        /invoke ${_liHoT.Advance}
        /continue
      }

      | check matching tag
      /if (${lsHeal${_liHoT.Value}Tag.Contains[grouphot]}) {
        DEBUG ${sep}setting _myHoT${sep}\a-w${maHeal.Find[stHeal${_liHoT.Value}].Value}\ax
        /varset _myHoT ${maHeal.Find[stHeal${_liHoT.Value}].Value}
      }

      | ready?.. or continue
      /if (!${is_ready[${_debug}, "${_myHoT}"]}) {
        DEBUG ${break}notready
        /varset _myHoT FALSE
        /invoke ${_liHoT.Advance}
        /continue
      }

      | cycle the tags
      /call tag_evaluation ${maDebug.Find[tag].Value} Heal ${_liHoT.Value} ${Me.DisplayName} "${maHeal.Find[stHeal${_liHoT.Value}].Value}" cast_cycle_heal_regen
      /if (!${Macro.Return}) {
        /varset _myHoT FALSE
        /invoke ${_liHoT.Value}
        /continue
      }

      /invoke ${_liHoT.Advance}
    }

    | bail if user doesnt have a grouphot tag set.. no one reads the damn changeloggs...
    /if (!${Bool[${_myHoT}]}) {
      DEBUG ${sep}fail, no groupregen set
      /return FALSE
    }

    /declare _HoTFound bool local FALSE
    /declare _HoTBase int local 0
    /declare _liHot listiterator local
    /vardata _liHot lsGroupRegens.First.Clone
    | look for a current HoT
    /while (!${_liHot.IsEnd}) {
      /if (${Me.Song[${_liHot.Value}].ID}) {
        /varset _HoTFound TRUE
        /varset _HoTBase ${Spell[${_liHot.Value}].Base[1]}
        /break
      }
      /invoke ${_liHot.Advance}
    }

    | my regen better then theirs?
    /if (${_HoTBase} >= ${Spell[${_myHoT}].Base[1]}) {
      DEBUG ${sep} base hot${sep}\a-w${_HoTBase} >= ${Spell[${_myHoT}].Base[1]}\ax
      /return FALSE
    }

    /declare _found bool TRUE
    | check regen SPA
    /if (${Spell[${_myHoT}].HasSPA[374]}) {
      /for _count 1 to ${Spell[${_myHoT}].NumEffects}
        /if (!${Spell[${Spell[${_myHoT}].Base2[${_count}]}].Duration}) /continue
        /if (!${Me.Song[${Spell[${_myHoT}].Base2[${_count}]}].ID}) /varset _found FALSE
        /if (!${Spell[${Spell[${_myHoT}].Base2[${_count}]}].Stacks}) /varset _found TRUE
        /if (${_found}) {
          /break
        }
      /next _count
    } else {
      /if (!${Me.Song[${_myHoT}].ID}) /varset _found FALSE
      /if (${Spell[${_myHoT}].Stacks}) /varset _found FALSE
    }
    /call voodoo ${_debug} "${_myHoT}" 0 FALSE
    /return TRUE

  | single heal over time
  } else /if (${_tag.Equal[hot]}) {

    | find my hot
    /while (!${_liHoT.IsEnd}) {
      
      | check the switch
      /if (!${maHeal.Find[swHeal${_liHoT.Value}].Value}) {
        DEBUG ${break}swHeal${_liHoT.Value}${sep}\a-w${maHeal.Find[stHeal${_liHoT.Value}].Value}\ax off
        /invoke ${_liHoT.Advance}
        /continue
      }
      | check the item name
      /if (!${Bool[${maHeal.Find[stHeal${_liHoT.Value}].Value}]}) {
        DEBUG ${break}swHeal${_liHoT.Value}${sep}\a-w${maHeal.Find[stHeal${_liHoT.Value}].Value}\ax no name
        /invoke ${_liHoT.Advance}
        /continue
      }
      | check matching tag
      /if (!${lsHeal${_liHoT.Value}Tag.Contains[hot]}) {
        DEBUG ${break}heal does not contain tag${sep}\a-whot\ax
        /invoke ${_liHoT.Advance}
        /continue
      }
      
      | good tag, set a name
      /if (${lsHeal${_liHoT.Value}Tag.Contains[hot]}) {
        DEBUG ${sep}setting _myHoT${sep}\a-w${maHeal.Find[stHeal${_liHoT.Value}].Value}\ax
        /varset _myHoT ${maHeal.Find[stHeal${_liHoT.Value}].Value}
        /break
      }
      /invoke ${_liHoT.Advance}
    }

    | could not find an available spell/item/whatever
    /if (!${Bool[${_myHoT}]}) {
      DEBUG ${break}fail, no single regen set
      /return FALSE
    }

    /dquery ${Spawn[id ${hurtID}].DisplayName} -q "Spell[${Spell[${_myHoT}].RankName}].Stacks" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG DNQReturn.Stacks${sep}\a-w${DNQReturn}\ax (\a-r${Spawn[id ${hurtID}].DisplayName}\ax)
    /dquery ${Spawn[id ${hurtID}].DisplayName} -q "Me.Song[${Spell[${_myHoT}].RankName}].Duration" -o DNQReturn2 -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG DNQReturn2.Duration${sep}\a-w${DNQReturn2}\ax (\a-r${Spawn[id ${hurtID}].DisplayName}\ax)
    /if (${DNQReturn} && !${DNQReturn2}) {
      /call voodoo ${_debug} "${_myHoT}" ${hurtID} FALSE
    }

    /return TRUE

  }

/return FALSE



|***
 * note: casts heals from maHeal
 * use: /call cast_cycle_heal DEBUG [TARGETID] [tag]
 ***|
sub cast_cycle_heal(bool _debug, int _tmpID, string _tag)
  DEBUG \atcast_cycle_heal\ax(\a-w${_tmpID}, ${_tag}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineHeal.Contains[EMPTY]} && ${lsLineHeal.Count} == 1) {
    DEBUG ${sep}resetting the empty heal line
    /invoke ${lsLineHeal.Clear}
    /invoke ${lsLineHeal.Append[${stCount50}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /declare _myHeal string local FALSE
  /declare _liHeal listiterator local
  /vardata _liHeal lsLineHeal.First.Clone
  /while (!${_liHeal.IsEnd}) {
    DEBUG ${sep}heal${sep}\a-w${maHeal.Find[stHeal${_liHeal.Value}].Value}${sep}${maHeal.Find[stHeal${_liHeal.Value}]}\ax
    GETINPUT cast_cycle_heal NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_heal

    | check the switch
    /if (!${Bool[${maHeal.Find[swHeal${_liHeal.Value}].Value}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maHeal.Find[stHeal${_liHeal.Value}].Value}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag for failure
    /if (!${lsHeal${_liHeal.Value}Tag.Contains[${_tag}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag pass
    /if (${lsHeal${_liHeal.Value}Tag.Contains[${_tag}]}) {
      /varset _myHeal ${maHeal.Find[stHeal${_liHeal.Value}].Value}
    }

    | ready?.. or continue
    /if (!${is_ready[${_debug}, "${_myHeal}"]}) {
      /varset _myHeal FALSE
      /invoke ${_liHeal.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Heal ${_liHeal.Value} ${Me.DisplayName} "${maHeal.Find[stHeal${_liHeal.Value}].Value}" cast_cycle_heal
    /if (!${Macro.Return}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    /call voodoo ${_debug} "${maHeal.Find[stHeal${_liHeal.Value}].Value}" ${_tmpID} breakonhealpct

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
      /break
    }

    /invoke ${_liHeal.Advance}
  }

/return TRUE



|***
 * note: cast promise heals from maHeal
 * use: /call cast_cycle_heal_promise DEBUG
 ***|
sub cast_cycle_heal_promise(bool _debug)
/return
  DEBUG \atcast_cycle_heal_promise\ax(\a-w\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineHeal.Contains[EMPTY]} && ${lsLineHeal.Count} == 1) {
    DEBUG ${sep}resetting the empty heal line
    /invoke ${lsLineHeal.Clear}
    /invoke ${lsLineHeal.Append[${stCount50}]}
  }

  /declare _myHeal string local FALSE
  /declare _liHeal listiterator local
  /vardata _liHeal lsLineHeal.First.Clone
  /while (!${_liHeal.IsEnd}) {
    DEBUG ${sep}heal${sep}\a-w${maHeal.Find[stHeal${_liHeal.Value}].Value}${sep}${maHeal.Find[stHeal${_liHeal.Value}]}\ax
    GETINPUT cast_cycle_heal_promise NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_heal_promise

    | check the switch
    /if (!${Bool[${maHeal.Find[swHeal${_liHeal.Value}].Value}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maHeal.Find[stHeal${_liHeal.Value}].Value}]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag for failure
    /if (!${lsHeal${_liHeal.Value}Tag.Contains[promise]}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag pass
    /if (${lsHeal${_liHeal.Value}Tag.Contains[promise]}) {
      /varset _myHeal ${maHeal.Find[stHeal${_liHeal.Value}].Value}
    }

    | ready?.. or continue
    /if (!${is_ready[${_debug}, "${_myHeal}"]}) {
      /varset _myHeal FALSE
      /invoke ${_liHeal.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Heal ${_count} ${Me.DisplayName} "${maHeal.Find[stHeal${_count}].Value}" cast_cycle_heal_promise
    /if (!${Macro.Return}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    /declare _tmpName string local ${get_assist_info[${_debug}, ID]}

    | set the spell
    /varset _spell ${Spell[${maHeal.Find[stHeal${_liHeal.Value}Con].Value}].RankName}
    DEBUG ${sep}\awSpell\ax${sep}\a-w${_spell}\ax ID:${Spell[${_spell}].ID}

    | does it stack
    /dquery ${_tmpName} -q "Spell[${_spell}].Stacks" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG ${sep}${sep}STACKS: .Stacks \at${DNQReturn}\ax ${dot} ${_spell}
    /if (${DNQReturn.Equal[FALSE]}) /continue

    | is it buff blocked
    /dquery ${_tmpName} -q "lsBlockedBuffsMe.Find[${Spell[${_spell}].ID}]" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG ${sep}${sep}BLOCKED .Buff.Blocked \at${DNQReturn}\ax ${dot} ${_spell}
    /if (${Bool[${DNQReturn}]}) {
      /continue
    }

    | do we have the triggered buff?
    /dquery ${_tmpName} -q "Me.Buff[${_spell}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG ${sep}${sep}HAVE: .Buff.ID \at${DNQReturn}\ax ${dot} ${_spell}
    /if (${Bool[${DNQReturn}]}) {
      /continue
    }

    /declare _tmpID string local ${get_assist_info[${_debug}, ID]}
    /delay 1

    /call voodoo ${_debug} "${maHeal.Find[stHeal${_liHeal.Value}].Value}" ${_tmpID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
      /break
    }

    /invoke ${_liHeal.Advance}
  }

/return TRUE



|***
 * note: moves you behind a target
 * use: /call moveto_behind DEBUG TARGETID
 ***|
sub moveto_behind(bool _debug, int _tmpID)
  DEBUG \atmoveto_behind\ax()
  /declare _dist float local ${Math.Calc[${Target.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
  /declare _tmpY float local ${Math.Calc[${Target.Y}-${Math.Calc[(${_dist}*${Math.Cos[${Target.Heading.DegreesCCW}]}]}]}
  /declare _tmpX float local ${Math.Calc[${Target.X}-${Math.Calc[(${_dist}*${Math.Sin[${Target.Heading.DegreesCCW}]}]}]}
  /declare _tmpZ float local ${Target.Z}

  DEBUG ${sep}locyxz${sep}\a-w${_tmpY} ${_tmpX} ${_tmpZ} dist=${_dist}\ax

  /if (${Navigation.PathExists[locyxz ${_tmpY} ${_tmpX} ${_tmpZ}]}) {
    /nav locyxz ${_tmpY} ${_tmpX} ${_tmpZ} |log=${maEntropy.Find[swNavLogSpew].Value} dist=${_dist}
    /delay 6 ${Me.Moving}
    /delay 30s !${Me.Moving}
  }

  /delay 2

/return TRUE



|***
 * note: casts heals from maBuff
 * use: /call check_buff_cycle_aura DEBUG [TARGETID]
 ***|
sub check_buff_cycle_aura(bool _debug, int _tmpID, string _tag)
  DEBUG \atcheck_buff_cycle_aura\ax(\a-w${_tmpID}, ${_tag}\ax)

  /declare _liBuff listiterator local
  /vardata _liBuff lsCount50.First.Clone
  /while (!${_liBuff.IsEnd}) {
    GETINPUT check_buff_cycle_aura NA|NA
    ISMEDEAD
    CHECKEXIT check_buff_cycle_aura

    | check the switch
    /if (!${maBuff.Find[swBuff${_liBuff.Value}].Value}) {
      /invoke ${_liBuff.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maBuff.Find[stBuff${_liBuff.Value}].Value}]}) {
      /invoke ${_liBuff.Advance}
      /continue
    }

    | check matching tag for failure
    /if (!${lsBuff${_liBuff.Value}Tag.Contains[aura]}) {
      DEBUG ${break}(\a-r${_liBuff.Value}\ax) no aura tag
      /invoke ${_liBuff.Advance}
      /continue
    }
    
    | start name redirects
    /if (${maBuff.Find[stBuff${_liBuff}].Value.Count[|]} == 3) {
      DEBUG ${sep}get best in
      /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_liBuff}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_liBuff}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_liBuff}].Value.Arg[3,|]}"
      /invoke ${maData.Add[tmpName,${Macro.Return}]}
      /invoke ${maData.Add[tmpBuff,${Macro.Return}]}
      
    | get item clickie name
    } else /if (${FindItem[=${maBuff.Find[stBuff${_liBuff}].Value}].ID}) {
      DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_liBuff}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_liBuff}].Value}].Clicky.SpellID}].Name}\ax
      /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_liBuff}].Value}].Clicky.SpellID}].Name}]}
      /invoke ${maData.Add[tmpBuff,${maBuff.Find[stBuff${_liBuff}].Value}]}

    | stick with buff name
    } else {
      DEBUG ${sep}defaultname${sep}${maBuff.Find[stBuff${_liBuff}].Value}\ax
      /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_liBuff}].Value}]}
      /invoke ${maData.Add[tmpBuff,${maBuff.Find[stBuff${_liBuff}].Value}]}
    }
   
   
    | have the auras?
    /if (${Me.Aura[1].Name.Equal[${Spell[${maData.Find[tmpName].Value}].RankName}]} || ${Me.Aura[1].Name.Find[${maData.Find[tmpName].Value}]} || ${Me.Aura[1].Name.Find[${maData.Find[tmpName].Value}]}) {
      DEBUG ${break}have aura1${sep}\a-w${maData.Find[tmpName].Value}\ax
      /invoke ${_liBuff.Advance}
      /continue
    } else /if (${Me.Aura[2].Name.Equal[${Spell[${maData.Find[tmpName].Value}].RankName}]} || ${Me.Aura[2].Name.Find[${maData.Find[tmpName].Value}]} || ${Me.Aura[2].Name.Find[${maData.Find[tmpName].Value}]}) {
      DEBUG ${break}have aura2${sep}\a-w${maData.Find[tmpName].Value}\ax
      /invoke ${_liBuff.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_liBuff.Value} ${Me.DisplayName} "${maData.Find[tmpName].Value}" check_buff_cycle_aura
    /if (!${Macro.Return}) {
      DEBUG ${break}condition${sep}Buff${_liBuff.Value}
      /invoke ${_liBuff.Advance}
      /continue
    }

    /delay 5
    /call voodoo ${_debug} "${maData.Find[tmpName].Value}" 0 FALSE
    /call cast_cooldown_wait ${_debug}

    /invoke ${_liBuff.Advance}
  }

/return TRUE



|***
 *  DES: Engages a merc based off curent control scructure
 *  USE: /call merc_control
 *  NOTE:

 ${Mercenary.StateID}'s
 SUSPENDED (ID:1)
 ACTIVE (ID:5)
 DEAD (ID: )
 COMBAT (ID: )

/invoke ${set_data_timer[FALSE, Check_Alliance, 6s]}
and
to restart the timer weould be
/invoke ${set_data_timer[FALSE, Check_Alliance, RESTART]}

 ${Mercenary.Stance}'s
 Rogue   : Passive, Balanced, Burn
 Warrior : Passive, Assist, Aggressive
 Cleric  : Passive, Balanced, Reactive, Effecient
 Wizard  : Passive, Balanced, Burn, Burn AE
${maMercenary.Find[stMercenaryClass].Value}
 ***|
sub merc_summon(bool _debug)
  DEBUG merc_summon()

  /declare _tempnum int local 0

  /if (!MERCENARY || ${Raid.Members}) /return FALSE

  /if (${Group.GroupSize} > 5) {
    /nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
  }

  /if (${maMercenary.Find[swMercenary].Value} && ${Select[${Mercenary.State},nomerc]}) {
    /invoke ${out[1, shade, 0]}
    /return
  }

  /if (${Select[${maMercenary.Find[stMercenaryClass].Value},CLR,ROG,WIZ,WAR]} && ${Mercenary.State.Equal[suspended]} && (${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Revive]} || ${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Unsuspend]})) {
    /if (${maMercenary.Find[stMercenaryClass].Value.Equal[CLR]}) {
      /varset _tempnum ${Me.MercListInfo[healer]}
      /invoke ${out[26, 0, 25]}
      /notify MMGW_ManageWnd MMGW_OwnedListBox listselect ${_tempnum}
      /nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
    } else /if (${maMercenary.Find[stMercenaryClass].Value.Equal[ROG]}) {
      /varset _tempnum ${Me.MercListInfo[damage melee]}
      /invoke ${out[26, 0, 22]}
      /notify MMGW_ManageWnd MMGW_OwnedListBox listselect ${_tempnum}
      /nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
    } else /if (${maMercenary.Find[stMercenaryClass].Value.Equal[WIZ]}) {
      /varset _tempnum ${Me.MercListInfo[damage caster]}
      /invoke ${out[26, 0, 23]}
      /notify MMGW_ManageWnd MMGW_OwnedListBox listselect ${_tempnum}
      /nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
    } else /if (${maMercenary.Find[stMercenaryClass].Value.Equal[WAR]}) {
      /varset _tempnum ${Me.MercListInfo[tank]}
      /invoke ${out[26, 0, 24]}
      /notify MMGW_ManageWnd MMGW_OwnedListBox listselect ${_tempnum}
      /nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
    }
  }

  /if (MERCENARY && ${Select[${Mercenary.State},dead,suspended]}) {
    /if (${Mercenary.State.Equal[SUSPENDED]}) {
      /if (!${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Revive]} && ${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Unsuspend]}) {
        /return FALSE
      }
      /nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
    } else /if (${Mercenary.State.Equal[DEAD]}) {
      /if (${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Revive]} && !${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Unsuspend]}) {
        /return FALSE
      }
      /nomodkey /notify MMGW_ManageWnd MMGW_SuspendButton leftmouseup
    } else /if (!${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Revive]} && !${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Unsuspend]}) {
      ECHO Merc timer need to wait
      /invoke ${set_timer[FALSE, Check_Merc, ${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text}]}
    }
  }

  /if (!COMBAT && !${Target.ID} && ${Me.MercenaryStance.NotEqual[Passive]}) {
    /stance Passive
    /return FALSE
  }

/return TRUE






sub merc_control(bool _debug)
  DEBUG merc_control()
  | manual mode
  /if (!AUTO) {
    /if (!COMBAT && !${Target.ID} && ${Me.MercenaryStance.NotEqual[Passive]}) {
      /stance Passive
      /return FALSE
    }
    /if (${Target.ID}) {
      /if (${Target.ID} == ${_tmpID} && ${Me.MercenaryStance.NotEqual[Passive]}) {
        /stance Passive
        /return FALSE
      }
      /if (COMBAT) {
        /if (BURN) {
          /if (${Select[${Mercenary.Class.ShortName},ROG,WIZ]}) {
            /if (${Me.MercenaryStance.NotEqual[Burn]}) /stance Burn
          } else /if (${Select[${Mercenary.Class.ShortName},WAR]}) {
            /if (${Me.MercenaryStance.NotEqual[Aggressive]}) /stance Aggressive
          } else /if (${Select[${Mercenary.Class.ShortName},CLR]}) {
            /if (${Me.MercenaryStance.NotEqual[Reactive]}) /stance Reactive
          }
        } else {
          /if (${Select[${Mercenary.Class.ShortName},ROG,WIZ,CLR]}) {
            /if (${Me.MercenaryStance.NotEqual[Balanced]}) /stance Balanced
          } else /if (${Select[${Mercenary.Class.ShortName},WAR]}) {
            /if (${Me.MercenaryStance.NotEqual[Assist]}) /stance Assist
          }
        }
      }
      /return TRUE
    } else /if (!${Target.ID}) {
      /if ((COMBAT || VALID) && ${Me.MercenaryStance.NotEqual[Passive]}) /stance Passive
      /return FALSE
    }

  | automatic mode
  } else /if (AUTO) {
    /if (!VALID) {
      /if (${Me.MercenaryStance.NotEqual[Passive]}) /stance Passive
      /return FALSE
    } else /if (VALID) {
      /if (BURN) {
        /if (${Select[${Mercenary.Class.ShortName},ROG,WIZ]}) {
          /if (${Me.MercenaryStance.NotEqual[Burn]}) /stance Burn
        } else /if (${Select[${Mercenary.Class.ShortName},WAR]}) {
          /if (${Me.MercenaryStance.NotEqual[Aggressive]}) /stance Aggressive
        } else /if (${Select[${Mercenary.Class.ShortName},CLR]}) {
          /if (${Me.MercenaryStance.NotEqual[Reactive]}) /stance Reactive
        }
      } else {
        /if (${Select[${Mercenary.Class.ShortName},ROG,WIZ,CLR]}) {
          /if (${Me.MercenaryStance.NotEqual[Balanced]}) /stance Balanced
        } else /if (${Select[${Mercenary.Class.ShortName},WAR]}) {
          /if (${Me.MercenaryStance.NotEqual[Assist]}) /stance Assist
        }
      }
    }
  }

  /if (SAFEZONE) {
    /if (${Me.MercenaryStance.NotEqual[Passive]}) /stance Passive
    /return FALSE
  }

/return TRUE



|***
 * note: check user input for valid stuff
 * use: /call validate_input DEBUG [TYPE] [check]
 ***|
sub validate_input(bool _debug, string _type, string _verbage)
  DEBUG \a-t\ax(\a-w${_type}, ${_verbage}\ax)

  | default passes
  /if (${_verbage.Find[gem]} || ${_verbage.Count[|]} == 3) {
    /return TRUE
  }

  | default passes
  /if (${Select[${_verbage},cursor]}) {
    /return TRUE
  }

  | checks ?
  /if (${_type.Equal[control]}) {

    | alt ability?
    /if (${Me.AltAbility[${_verbage}]}) {
      /return TRUE

    | spell
    } else /if (${Me.Book[${Spell[${_verbage}].RankName}]}) {
      /return TRUE

    | melee disc
    } else /if (${Me.CombatAbility[${Spell[${_verbage}].RankName}]}) {
      /return TRUE

    | click item
    } else /if (${FindItem[=${_verbage}].ID}) {
      /return TRUE

    | click item
    } else /if (${_verbage.Find[epic]}) {
      /return TRUE

    | skill ready
    } else /if (${Me.Ability[${_verbage}]}) {
      /return TRUE
    }

    OUT ${notice} this is not a thing${sep}\a-w${_verbage}\ax

  }

  | blanket fail
/return FALSE



|***
 * note: removes any unwanted buffs or songs
 * use: /call check_drop_buffs DEBUG
 ***|
sub check_drop_buffs(bool _debug)
  DEBUG \atcheck_drop_buffs\ax(\a-w\ax)

  /invoke ${set_data_timer[${_debug}, Check_Drop_Buffs, RESTART]}

  /if (${lsDropBuffs.Contains[EMPTY]} && ${lsDropBuffs.Count} == 1) {
    /return FALSE
  }

  /declare _lidb listiterator local
  /vardata _lidb lsDropBuffs.First.Clone
  /while (!${_lidb.IsEnd}) {

    /if (${Me.Buff[="${_lidb.Value}"].ID} || ${Me.Song[="${_lidb.Value}"].ID})  {
      OUT Drop${sep}${info}${_lidb.Value}\ax
      /invoke ${Me.Buff[="${_lidb.Value}"].Remove}
    }
    /invoke ${_lidb.Advance}
  }

/return TRUE



|***
 * note: popups
 * use: ${out_popup["STRING"]}
 ***|
sub out_popup(string _verbage)

  /if (!${lsGrain.Contains[27]}) {
    /return FALSE
  }

  /popup ${_verbage}

/return TRUE



|***
 * note: check self buffs
 * use: /call check_buff_cycle_self DEBUG
 ***|
sub check_buff_cycle_self(bool _debug)
  DEBUG \atcheck_buff_cycle_self\ax()

  SETHUD "buff self"

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Buff_Self, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  ISMEDEAD
  GETINPUT check_buff_cycle_self NA|NA

  /declare _count int local 0
  /declare _countD int local 0
  /declare _i int local 0
  /declare _found bool local

  /for _count 1 to 50
    
    

    /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
      /continue
    } else /if (!${maBuff.Find[swBuff${_count}].Value}) {
      /continue
    } else /if (!${lsBuff${_count}Tag.Contains[self]}) {
      /continue
    }

    | start name redirects
    /if (${maBuff.Find[stBuff${_count}].Value.Count[|]} == 3) {
      /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_count}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[3,|]}"
      /invoke ${maData.Add[tmpName,${Macro.Return}]}
      /invoke ${maData.Add[tmpBuff,${Macro.Return}]}

    | get item clickie name
    } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
      DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
      /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
      /invoke ${maData.Add[tmpBuff,${maBuff.Find[stBuff${_count}].Value}]}

    | stick with buff name
    } else {
      /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
      /invoke ${maData.Add[tmpBuff,${maBuff.Find[stBuff${_count}].Value}]}
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${Me.DisplayName} "${maData.Find[tmpName].Value}" check_buff_cycle_self
    /if (!${Macro.Return}) {
      /continue
    }

    /varset _found TRUE
    DEBUG ${sep}_count${sep}\a-w${_count}\ax
    

    | trigger swap AAs... fuck DBG and their shit..
    /if (${Spell[${maData.Find[tmpName].Value}].NumEffects} == 1 && ${Me.AltAbility[${maData.Find[tmpName].Value}]})  {
      DEBUG ${sep}\arSWAP\ax to trigger _spell\aw==\ax\a-w${Spell[${maData.Find[tmpName].Value}].Trigger[1]}\ax
      /invoke ${maData.Add[tmpName,${Spell[${Spell[${maData.Find[tmpName].Value}].Trigger[1]}].RankName}]}
    }

    | stacking / exist ?
    /if (${Spell[${maData.Find[tmpName].Value}].HasSPA[374]} || ${Spell[${maData.Find[tmpName].Value}].HasSPA[340]} || ${Spell[${maData.Find[tmpName].Value}].HasSPA[470]}) {
      DEBUG ${sep}\aw470\ax${sep}\a-w${Spell[${maData.Find[tmpName].Value}].HasSPA[470]}\ax ${dot} \aw374\ax${sep}\a-w${Spell[${maData.Find[tmpName].Value}].HasSPA[374]}\ax ${dot} \aw340\ax${sep}\a-w${Spell[${maData.Find[tmpName].Value}].HasSPA[340]}\ax
      DEBUG ${sep}\aweffects\ax${sep}\a-w${Spell[${maData.Find[tmpName].Value}].NumEffects}\ax

      /for _i 1 to ${Spell[${maData.Find[tmpName].Value}].NumEffects}
        DEBUG ${sep}\a-t${_i}/${Spell[${maData.Find[tmpName].Value}].NumEffects}\ax \aw::\ax ${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name} \aw::\ax ${Me.Buff[${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name}].ID} \aw::\ax D(\a-w${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Duration}\ax)

        | SPA has no duration, skip it
        /if (!${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Duration}) {
          DEBUG ${sep}\aw.Duration\ax break (${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name})
          /continue
        }

        | Does the SPA stack? if not, move on
        /if (!${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Stacks}) {
          DEBUG ${sep}\aw.Stacks\ax break (${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name})
          /continue
        }

        | blocked?
        /if (${lsBlockedBuffsMe.Find[${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].ID}]}) {
          DEBUG ${sep}\aw.Blocked\ax break (${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name})
          /continue
        }

        | do we have the SPA
        /if (!${Me.Buff[${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name}].ID}) {
          DEBUG ${sep}have name${sep}\a-wMe.Buff ${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name} .ID\ax
          /varset _found FALSE
        }

      /next _i

    } else /if (${FindItem[${maData.Find[tmpName].Value}].ID}) {

      | rogue poison exceptions
      /if (${lsBuff${_count}Tag.Contains[summonpoison]}) {

        | number of poisons?
        DEBUG ${sep}${FindItemCount[${Spell[${FindItem[=${maData.Find[tmpName].Value}].Clicky.SpellID}].Base[1]}]} \ay<=\ax ${maChr.Find[stLegPoison].Value}
        /if (${FindItemCount[${Spell[${FindItem[=${maData.Find[tmpName].Value}].Clicky.SpellID}].Base[1]}]} <= ${maChr.Find[stLegPoison].Value}) {
          /varset _found FALSE
        }

        | pants ready?
        /if (!${is_ready[${_debug}, "${maData.Find[tmpName].Value}"]}) {
          /continue
        }

      } else {
        DEBUG ${sep}Found Item ID${sep}\a-w${maData.Find[tmpName].Value}\ax${sep}${Spell[${FindItem[=${maData.Find[tmpName].Value}].Clicky.SpellID}].Name}
        /if (!${Me.Buff[${Spell[${FindItem[=${maData.Find[tmpName].Value}].Clicky.SpellID}].Name}].ID}) /varset _found FALSE
      }

    } else {
      DEBUG ${sep}\a-t${_count}\ax${sep}${maData.Find[tmpName].Value}${sep}${Me.Buff[${maData.Find[tmpName].Value}].ID}

      | stacks?
      /if (!${Spell[${maData.Find[tmpName].Value}].NewStacks}) /continue

      | do we have the buff?
      /if (!${Me.Buff[${maData.Find[tmpName].Value}].ID}) /varset _found FALSE

    }

    DEBUG ${sep}_found${sep}\a-w${_found}\ax
    /if (${_found}) {
      /continue
    }

    /if (${lsBuff${_count}Tag.Contains[dropinvis]}) {
      DEBUG ${sep}tag${sep}\a-wdropinvis\ax
      /makemevisible
    }
    
    /call voodoo ${_debug} "${maData.Find[tmpBuff].Value}" ${Me.ID} FALSE
    /call cast_cooldown_wait ${_debug}

    /if (${lsBuff${_count}Tag.Contains[autoinventory]}) {
      /delay 2s ${Cursor.ID}
      /autoinventory
    }

    | drop illusions/mounts
    /if (${lsBuff${_count}Tag.Contains[drop]}) {
      DEBUG ${sep}tag-\a-ydrop\ax
      /delay 5s !${Me.Casting.ID}
      /for _countD 1 to ${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.NumEffects}
        /if (${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.Trigger[${_countD}].Name.Find[Illusion:]} || ${Me.Buff[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].ID}) {
          | mounts
          /if (${Me.Buff[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].ID}) {
            /invoke ${out[0, "Removing${sep}\a-w${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}\ax"]}
            /removebuff ${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}
            /delay 5
          }
          | illusions
          /if (${Me.Buff[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.Trigger[${_countD}]}].ID}) {
            /invoke ${out[0, "Removing${sep}\a-w${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}\ax"]}
            /removebuff ${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.Trigger[${_countD}]}
            /delay 5
          }
        }
      /next _countD
    }

  /next _count

  SETHUD EMPTY

/return TRUE



|***
 * note: checks tags for stuff and things..
 * use: /call tag_evaluation type, ##, toonname, spellname, calling routine
 ***|
sub tag_evaluation(bool _debug, string _type, int _num, string _name, string _spell, string _sub)
  DEBUG \attag_evaluation\ax(\a-w${_type}, ${_num}, ${_name}, ${_spell}, ${_sub}\ax)

  | obey no tags. only use the aliase for this .. thing..
  /if (${ls${_type}${_num}Tag.Contains[aliasonly]}) {
    DEBUG ${break}tag${sep}\a-waliasonly\ax
    /return FALSE
  }

  | start cycling the tags
  /declare _litag listiterator local
  /vardata _litag ls${_type}${_num}Tag.First.Clone
  /declare _containsclass bool local FALSE
  /declare _licls listiterator local

  /while (!${_litag.IsEnd}) {

    | is the tag list empty?
    /if (${_litag.Value.Equal[EMPTY]}) {
      /invoke ${_litag.Advance}
      /continue
    }

    | does our query exist?
    /if (${sqlite.Status[q${_type}_${_litag.Value}].NotEqual[success]}) {
      DEBUG ${sep}\agpulling\ax sql data for \a-oq${_type}_${_litag.Value}\ax
      /while (TRUE) {
        /sqlite query "${DBshared}" q${_type}_${_litag.Value} SELECT tag_group.grouping, tag.name, tag_validate.created_by, tag_validate.eval, tag_validate.aux FROM tag_validate LEFT JOIN tag  ON tag_validate.tag = tag.id LEFT JOIN tag_group ON tag_validate.grouping = tag_group.id WHERE tag_group.grouping = "${_type.Lower}" AND tag.name = "${_litag.Value}";
        /if (!${sql_check[${_debug}, q${_type}_${_litag.Value}, sql_schema_shared, "SELECT (tag eval)"]}) /endmacro
        /if (${sqlite.Resultcode[q${_type}_${_litag.Value} == 0]}) /break
      }
    }

    | bad tag?
    /if (!${sqlite.Rows[q${_type}_${_litag.Value}]}) {
      OUT ${warning} \arInvalid\ax (\a-r${_sub}\ax)${sep}ls${_type}${_num}Tag${sep}\at${_litag.Value}\ax (\a-yq${_type}_${_litag.Value}\ax)
      /invoke ${_litag.Advance}
      /continue
    }

    | we have something to work with
    /if (${sqlite.Rows[q${_type}_${_litag.Value}]} == 1) {
      DEBUG ${sep}cycling tag(\a-oq${_type}_${_litag.Value}\ax)${sep}\a-w${_type}${_num}${sep}${_litag.Value}\ax${sep}\a-r${Parse[2,${sqlite.Result[q${_type}_${_litag.Value} 1 eval]}]}\ax

      | control markers we skip. will already be checked
      /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Equal[CONTROL::MARKER]}]}) {
        DEBUG ${skip}evaluation${sep}
        | do jack all here... just ignore the shit

      | name marker
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Equal[marker name]}]}) {
        /if (${_name.NotEqual[${_litag.Value}]}) {
          /invoke ${_litag.Advance}
          /continue
        }

      | execute sub here
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Find[SUB::]}]}) {
        DEBUG ${sep}releasing to sub${sep}

      | action tags here
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Find[ACTION::]}]}) {
        DEBUG ${sep}doing a thing for tag${sep}

        | delay1s
        /if (${_litag.Value.Equal[delay1s]}) {
          DEBUG ${sep}tag${sep}\a-wdelay1s\ax
          /delay 1s
        }

        | dropdisc
        /if (${_litag.Value.Equal[dropdisc]}) {
          /if (${Me.ActiveDisc.ID} && ${Me.CombatAbilityReady[${Spell[${_spell}].RankName}]}) {
            DEBUG ${sep}tag${sep}\a-wdropdisc\ax (\a-r${Me.ActiveDisc.ID}\ax)
            /stopdisc
            /delay 5
          }
        }
        
        | dropinvis
        | this is a very specific use tag.

        | autoinventory
        /if (${_litag.Value.Equal[autoinventory]}) {
          /call check_cursor ${maDebug.Find[loot].Value} CLEAR
        }

        | drop
        | this is a very specific use tag.

 
      | target setting
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Find[settarget]}]}) {
 
        /if (${_litag.Value.Equal[forcetarget]}) {
          /squelch /target id ${Parse[2,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Arg[2,|]}]}
          /delay 1s ${Target.ID} == ${Parse[2,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Arg[2,|]}]}
        } 
 
      | evaluate the tag eval here
      } else {

        | checks to match a class shortname
        /varset _containsclass FALSE
        /vardata _licls lsClassAll.First.Clone

        | cycle all 16 classes to see if there is a class in there at all
        | if we find even one class, we mark found and get out
        /while (!${_licls.IsEnd}) {
          /if (${ls${_type}${_num}Tag.Contains[${_licls.Value}]}) {
            /varset _containsclass TRUE
            /break
          }
          /invoke ${_licls.Advance}
        }

        | if we do find a class in the tag list, any class, we see if our class is one of them
        | if our class is not, we fail the check
        /if (${_containsclass} && ${_sub.NotEqual[check_buff_cycle_self]}) {
          /if (!${ls${_type}${_num}Tag.Contains[${Spawn[pc ${_name}].Class.ShortName}]}) {
            DEBUG ${sep}evaluation${sep}${_litag.Value}${sep}${Spawn[pc ${_name}].Class.ShortName}
            /return FALSE
          }

        | if theres not class in the tag check, we evaluate whatever it is..
        } else /if (!${_containsclass}) {
          DEBUG ${sep}evaluation${sep}${_litag.Value}${sep}\a-o${sqlite.Result[q${_type}_${_litag.Value} 1 eval]}\ax
          /if (!${Math.Calc[${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval]}]}]}) {
            DEBUG ${break}Returning${sep}\a-wFALSE\ax${sep}${_litag.Value}
            /return FALSE
          }
        }

      }
    }

    /invoke ${_litag.Advance}
  }

  DEBUG ${sep}tag_evaluation${sep}\agPASSED\ax
/return TRUE



|***
 * note: visual output for control options
 * use: /call control_base_edit [WHATTOCHECK] [rowcount] yadah yadah
 ***|
sub control_base_edit(bool _debug, string _chk, int _loopnum, string _type, string _verbage, string _verbage2, string _add_delete)
  DEBUG\atcontrol_base_edit\ax(\a-w${_chk}, ${_loopnum}, ${_type}, ${_verbage}, ${_verbage2}, ${_add_delete}\ax)
  /declare _count int local 0
  /declare _tmpName string local FALSE

  | list control contents
  /if (${_type.Equal[list]}) {
    ECHO
    /invoke ${out[12, "\aw${_chk} List\ax${sep}"]}

    | output any lines?
    /if (${lsControlLine.Contains[${_chk.Lower}]}) {
      /sqlite query "${DBcharacter}" dummy SELECT value FROM environment_${_chk.Lower} WHERE env_var="lsLine${_chk}";
      /invoke ${out[12, "Line${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}
    }

    | output the list
    /for _count 1 to ${_loopnum}
      /if (${ma${_chk}.Find[st${_chk}${_count}].Value.NotEqual[FALSE]}) {

        /if (${ma${_chk}.Find[st${_chk}${_count}].Value.Count[|]}) {
          /call get_best_in_spell ${_debug} "${ma${_chk}.Find[st${_chk}${_count}].Value.Arg[1,|]}" "${ma${_chk}.Find[st${_chk}${_count}].Value.Arg[2,|]}" "${ma${_chk}.Find[st${_chk}${_count}].Value.Arg[3,|]}"
          /varset _tmpName "${Macro.Return}"
        }
        /invoke ${out[12, "\awst${_chk}${_count}\ax${sep}${If[${Skill[${ma${_chk}.Find[st${_chk}${_count}].Value}].Auto},\apauto\ax,${If[${ma${_chk}.Find[sw${_chk}${_count}].Value},${on},${off}]}]}${sep}\a-w${ma${_chk}.Find[st${_chk}${_count}].Value}\ax${If[${Bool[${_tmpName}]}, ${dot} (\a-r${_tmpName}\ax),]}"]}
        /varset _tmpName FALSE
      }
    /next _count
    /invoke ${out[18, 0, 33]}
    /return TRUE
  }

  | help?
  /if (${_type.Equal[aux]}) {
    /call Bind_command_aux ${_chk.Lower}
    /return TRUE
  }

  /if (${_type.Equal[see]}) {
    /call Bind_command_see ma${_chk}
    /return TRUE
  }

  | check specific #
  /if (${Range.Between[1,${_loopnum}:${Int[${_type}]}]}) {

    | see item
    /if ((!${Bool[${_verbage}]} || ${Select[${_verbage},on,off]}) && ${_verbage.NotEqual[FALSE]}) {
      /invoke ${set_switch_env[${_debug}, sw${_chk}${_type}, TRUE, ${_verbage}]}
      /invoke ${out[12, "\aw${_chk}${_type}\ax${sep}${If[${ma${_chk}.Find[sw${_chk}${_type}].Value},${on},${off}]}${sep}\a-w${ma${_chk}.Find[st${_chk}${_type}].Value}\ax"]}
      /return TRUE

    | output map
    } else /if (${_verbage.Equal[see]}) {
      /invoke ${out[12, "\aw${_chk} #${_type}\ax${sep}${If[${Skill[${ma${_chk}.Find[st${_chk}${_type}].Value}].Auto},\apauto\ax,${If[${ma${_chk}.Find[sw${_chk}${_type}].Value},${on},${off}]}]}"]}
      /if (${ma${_chk}.Find[st${_chk}${_type}].Value.Count[|]}) {
        /call get_best_in_spell ${_debug} "${ma${_chk}.Find[st${_chk}${_type}].Value.Arg[1,|]}" "${ma${_chk}.Find[st${_chk}${_type}].Value.Arg[2,|]}" "${ma${_chk}.Find[st${_chk}${_type}].Value.Arg[3,|]}"
        /varset _tmpName "${Macro.Return}"
      }

      /invoke ${out[12, "st${_chk}\aw${_type}\ax${sep}\a-w${ma${_chk}.Find[st${_chk}${_type}].Value}\ax${If[${Bool[${_tmpName}]}, ${dot} (\a-r${_tmpName}\ax),]}"]}
      /if (${Select[${_chk},buff]}) {
        /invoke ${out[12, "st${_chk}\aw${_type}\axAlias${sep}\a-w${ma${_chk}.Find[st${_chk}${_type}Alias].Value}\ax"]}
      }
      /invoke ${out[12, "ls${_chk}\aw${_type}\axTag"]}
      /declare _lisee listiterator local
      /vardata _lisee ls${_chk}${_type}Tag.First.Clone
      /while (!${_lisee.IsEnd}) {

        | skip empty tag lists
        /if (${_lisee.Value.Equal[EMPTY]}) {
          OUT ${dot}\a-wEMPTY\ax
          /break
        }

        /sqlite query "${DBshared}" dummy SELECT tag_group.grouping, tag.name, tag_validate.created_by, tag_validate.eval, tag_validate.aux FROM tag_validate LEFT JOIN tag ON tag_validate.tag = tag.id LEFT JOIN tag_group ON tag_validate.grouping = tag_group.id WHERE tag_group.grouping = "${_chk.Lower}" AND tag.name = "${_lisee.Value}";
        /if (!${sqlite.Rows[dummy]}) {
          OUT ${dot} ${_chk.Lower}${sep}\at${_lisee.Value}\ax${sep}${warning} \arInvalid\ax
        } else /if (${Parse[99,${sqlite.Result[dummy 1 eval].Find[::]}]}) {
          OUT ${dot} ${sqlite.Result[dummy 1 grouping]}${sep}\a-o${sqlite.Result[dummy 1 name]}\ax${sep}\a-w${Parse[2,${sqlite.Result[dummy 1 aux]}]}\ax
        } else {
          OUT ${dot} ${sqlite.Result[dummy 1 grouping]}${sep}${If[${sqlite.Result[dummy 1 created_by].Equal[1]},\a-g${sqlite.Result[dummy 1 name]}\ax,\at${sqlite.Result[dummy 1 name]}\ax]}${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 eval]}]}\ax
        }
        /invoke ${_lisee.Advance}
      }
      /invoke ${out[18, 0, 33]}
      /return TRUE

    | reset control item to default
    } else /if (${Select[${_verbage},clear]}) {
      DEBUG ${sep}clear/reset
      /call do_raw_edit ${maDebug.Find[edit].Value} SILENT "sw${_chk}${_type}" FALSE
      /if (${Select[${_chk},buff]}) {
        /call do_raw_edit ${maDebug.Find[edit].Value} SILENT "st${_chk}${_type}Alias" FALSE
      }
      /call do_raw_edit ${maDebug.Find[edit].Value} SILENT "st${_chk}${_type}" FALSE
      /call do_raw_edit ${maDebug.Find[edit].Value} SILENT "ls${_chk}${_type}Tag" clear
      /invoke ${out[12, "${_chk}${_type}${sep}\a-wRESET\ax"]}
    } else /if (${Select[${_verbage},alias]}) {
      /call do_raw_edit ${maDebug.Find[edit].Value} FALSE "st${_chk}${_type}Alias" "${_verbage2}"
    } else /if (${Select[${_verbage},tag]}) {
      /call do_raw_edit ${maDebug.Find[edit].Value} FALSE "ls${_chk}${_type}Tag" "${_verbage2}" ${_add_delete} ${_verbage}
    } else /if (${Bool[${_verbage}]} && !${Select[${_verbage},tag,alias,clear,see]}) {
      /if (!${validate_input[${maDebug.Find[edit].Value}, control, "${_verbage}"]}) /return FALSE
      /call do_raw_edit ${maDebug.Find[edit].Value} FALSE "st${_chk}${_type}" "${_verbage}"
    }

  }

  | edit the line
  /if (${_type.Equal[line]} && ${lsControlLine.Contains[${_chk.Lower}]}) {
    DEBUG ${sep}line${sep}\a-w${_verbage}\ax

    | adjust to proper case
    /if (${Select[${_chk},dot]}) {
      /varset _chk DoT
    }

    | clear the current line data
    /invoke ${lsLine${_chk}.Clear}
    /sqlite query "${DBcharacter}" dummy UPDATE environment_${_chk.Lower} SET value="EMPTY" WHERE env_var="lsLine${_chk}";

    | if clearing. we stop here
    /if (${Select[${_verbage},clear,empty]}) {
      /invoke ${lsLine${_chk}.Append[EMPTY]}
      /invoke ${out[12, "Line ${_chk}${sep}\a-wEMPTY\ax"]}
      /return TRUE
    }

    | add the new data
    /call do_raw_edit ${maDebug.Find[edit].Value} SILENT lsLine${_chk} ${_verbage} add
    /invoke ${out[12, "Line ${_chk}${sep}\a-w${_verbage}\ax"]}
    /return TRUE
  }

/return TRUE



|***
 * note: returns 1/0 for class do's and dont's
 * note: markers::HealToT HealGroup Heal Rez Pet Eradicate Cure Snare Cast Melee HealSelf HealXT HealCure HealPet
 * use: ${marker[check]}
 ***|
sub marker(string _verbage)
  DEBUG \atmarker\ax(\a-w${_verbage}\ax)

  /if (${sqlite.Status[qMarker${Me.Class.ShortName}].NotEqual[success]}) {
    DEBUG ${sep}Pulling data for (\a-rqMarker${Me.Class.ShortName}\ax)${sep}\a-w${_verbage}\ax
    /sqlite query "${DBstatic}" qMarker${Me.Class.ShortName} SELECT * FROM markers WHERE shortname = '${Me.Class.ShortName}';
  }

  /if (${sqlite.Result[qMarker${Me.Class.ShortName} 1 ${_verbage}]} == 1) {
    /return 1
  }

/return 0



|***
 * note: forces a wait for spell gem cooldowns
 * use: /call cast_cooldown_wait DEBUG
 ***|
sub cast_cooldown_wait(bool _debug)
  DEBUG \atcast_cooldown_wait\ax()

  /if (!${Me.SpellInCooldown}) /return TRUE

  /if (!${maEntropy.Find[swBuffCooldownWait].Value}) {
    /return TRUE
  }

  /while (${Me.SpellInCooldown}) {
    CHECKEXIT
    ISMEDEAD
    /if (!${Me.SpellInCooldown}) {
      /break
    }
    /delay 5
  }

/return TRUE



|***
 * note: check/cast single instant cast invis only
 * use: /call
 ***|
sub check_invis_single(bool _debug)
  DEBUG \atcheck_invis_single\ax()

  /if (${Me.Invis}) {
    /return TRUE
  }

  /if (!${is_ready[${_debug}, "${maChr.Find[stInvisSingle].Value}"]}) {
    /return FALSE
  }

  /if (!${Spell[${maChr.Find[stInvisSingle].Value}].MyCastTime}) {
    /call voodoo ${_debug} "${maChr.Find[stInvisSingle].Value}" 0 ignoremovement
  } else {
    /call set_stop_movement ${_debug}
    /call voodoo ${_debug} "${maChr.Find[stInvisSingle].Value}" 0 FALSE
    /delay 2
    /delay 2s !${Me.Casting.ID}

  }

  /if (!${Me.Invis}) {
    /return FALSE
  }

/return TRUE



|***
 * note: shared class controls
 * use: /call set_control_shared ${_type} "${_verbage}" "${_verbage2}" ${_debug}
 ***|
sub set_control_shared(string _type, string _verbage, string _verbage2, bool _debug)
  DEBUG \atset_control_shared\ax(\a-wt:${_type}, v:"${_verbage}", s2:"${_verbage2}"\ax)

  | output control map
  /if (${_type.Equal[see]}) {
    /call Bind_command_see maChr ${_verbage}
    /return SKIP

  | set spire to be used
  } else /if (${_type.Equal[spire]}) {
    /call set_control_chr_spire ${_debug} "${_verbage}" "${_verbage2}"
    /return SKIP

  | set invis
  } else /if (${_type.Equal[invis]}) {
    /call set_control_chr_invis ${_debug} "${_verbage}" "${_verbage2}"
    /return SKIP

  | set ae on/off / count
  } else /if (${_type.Equal[ae]}) {
    /if (!${set_control_num_range[${_debug}, stCountAE, "${_verbage}", 0, 99]}) /return FALSE

  | set intensity AA
  } else /if (${_type.Equal[intensity]}) {
    /call set_control_chr_intensity ${_debug} ${_verbage} ${_verbage2}
    /return SKIP

  | set AA fade use %
  } else /if (${_type.Equal[fade]}) {
    /if (!${set_control_num_range[${_debug}, stPctFade, "${_verbage}", 0, 99]}) /return FALSE

  | use mana click items
  } else /if (${_type.Equal[manarecover]} && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
    /if (!${set_control_num_range[${_debug}, stPctManaRecovery, "${_verbage}", 0, 99]}) /return FALSE

  | use hiatus disc
  } else /if (${_type.Equal[hiatus]} && ${lsClassMelee.Contains[${Me.Class.ShortName}]}) {
    /invoke ${set_switch_env[${_debug}, swHiatus, TRUE, ${_verbage}]}

  }

/return TRUE



|***
 * note: returns the spawn ID name in its con color
 * use: ${con_color[DEBUG, ID]}
 ***|
sub con_color(bool _debug, int _tmpID)
  DEBUG \atcon_color\ax(\a-w${_tmpID}\ax)

  /if (!${maEntropy.Find[swUseConColor].Value}) {
    /return ${tar}${Spawn[id ${_tmpID}].DisplayName}\ax
  } else /if (${Spawn[id ${_tmpID}].ConColor.Equal[RED]}) {
    /return \a#FF0000${Spawn[id ${_tmpID}].DisplayName}\ax
  } else /if (${Spawn[id ${_tmpID}].ConColor.Equal[YELLOW]}) {
    /return \a#FFFF00${Spawn[id ${_tmpID}].DisplayName}\ax
  } else /if (${Spawn[id ${_tmpID}].ConColor.Equal[WHITE]}) {
    /return \a#FFFFFF${Spawn[id ${_tmpID}].DisplayName}\ax
  } else /if (${Spawn[id ${_tmpID}].ConColor.Equal[BLUE]}) {
    /return \a#0000FF${Spawn[id ${_tmpID}].DisplayName}\ax
  } else /if (${Spawn[id ${_tmpID}].ConColor.Equal[LIGHT BLUE]}) {
    /return \a#4FC3F7${Spawn[id ${_tmpID}].DisplayName}\ax
  } else /if (${Spawn[id ${_tmpID}].ConColor.Equal[GREEN]}) {
    /return \a#008000${Spawn[id ${_tmpID}].DisplayName}\ax
  } else /if (${Spawn[id ${_tmpID}].ConColor.Equal[GREY]}) {
    /return \a#808080${Spawn[id ${_tmpID}].DisplayName}\ax
  }

/return \arError\ax${sep}con_colorID



|***
 * note: taunts a mob, but only if needed
 * use: ${do_taunt[DEBUG, ID]}
 ***|
sub do_taunt(bool _debug, int _tmpID)
  DEBUG \atdo_taunt\ax(\a-w${_tmpID}\ax)

  CHECKDEAD
  CHECKEXIT

  /if (!${maAgro.Find[stPctTaunt].Value} || !AGRO) {
    /return FALSE
  }

  /if (!${Me.AbilityReady[Taunt]}) {
    /return FALSE
  }

  /if (!${Target.ID}) {
    /return FALSE
  }

  /if (${Me.PctAggro} > ${maCC.Find[stPctTaunt].Value} && ${Me.TargetOfTarget.ID} == ${Me.ID}) {
    /return FALSE
  }

  /if (${Spawn[${_tmpID}].Distance} > ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
    /return FALSE
  }

  /invoke ${out[8, "Melee${sep}${sp}Taunt\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
  /doability Taunt

/return TRUE



|***
 * note: find the best spell you have with given conditions
 * use: ${get_best_in_spell[DEBUG, TARGETTYPE, SUBCATEGORY, ATRIBUTENAME]}
 ***|
sub get_best_in_spell(bool _debug, string _targettype, string _subcat, string _attrib)
  DEBUG \atget_best_in_spell\ax(\a-w${_targettype}, ${_subcat}, ${_attrib}\ax)

  /declare _running         int local 0
  /declare _max             int local 0
  /declare _maxName         string local FALSE
  /declare _countNumEff     int local 0
  /declare _countSPA        int local 0
  /declare _count           int local 0

  /while (TRUE) {
    /sqlite query "${DBstatic}" qBestSpell SELECT * FROM eq_spa WHERE value="${_attrib}";
    /if (${sqlite.Resultcode[qBestSpell]} == 0) /break
  }

  /if (!${sqlite.Rows[qBestSpell]}) /return FALSE

  /for _count 1 to ${maHard.Find[stSpellBookSize].Value}
    /if (!${Me.Book[${_count}].ID}) /continue
    /if (${Me.Book[${_count}].Subcategory.NotEqual[${_subcat}]}) /continue
    /if (${Me.Book[${_count}].TargetType.NotEqual[${_targettype}]}) /continue
    /if (${Me.Book[${_count}].HasSPA[470]} || ${Me.Book[${_count}].HasSPA[374]} || ${Me.Book[${_count}].HasSPA[340]}) {
      /for _countNumEff 1 to ${Me.Book[${_count}].NumEffects}
        /if (!${Me.Book[${_count}].Trigger[${_countNumEff}].HasSPA[${sqlite.Result[qBestSpell 1 env_var]}]}) /continue
        /for _countSPA 1 to ${Me.Book[${_count}].Trigger[${_countNumEff}].NumEffects}
          /if (${Me.Book[${_count}].Trigger[${_countNumEff}].Attrib[${_countSPA}]} != ${sqlite.Result[qBestSpell 1 env_var]}) /continue
          /varset _running ${Me.Book[${_count}].Trigger[${_countNumEff}].Base[${_countSPA}]}
          /if (${_running} < -1) {
            /if (${_running} <= ${_max}) {
              /varset _max ${_running}
              /varset _maxName ${Me.Book[${_count}]}
            }
          } else {
            /if (${_running} >= ${_max}) {
              /varset _max ${_running}
              /varset _maxName ${Me.Book[${_count}]}
            }
          }
        /next _countSPA
      /next _countNumEff
    } else /if (${Me.Book[${_count}].SPA}) {
      /for _countSPA 1 to ${Me.Book[${_count}].NumEffects}
        /if (${Me.Book[${_count}].Attrib[${_countSPA}]} != ${sqlite.Result[qBestSpell 1 env_var]}) /continue
        /varset _running ${Me.Book[${_count}].Base[${_countSPA}]}
        /if (${_running} < -1) {
          /if (${_running} <= ${_max}) {
            /varset _max ${_running}
            /varset _maxName ${Me.Book[${_count}]}
          }
        } else {
          /if (${_running} >= ${_max}) {
            /varset _max ${_running}
            /varset _maxName ${Me.Book[${_count}]}
          }
        }
      /next _countSPA
    }
  /next _count
  DEBUG ${sep}Returning${sep}\a-w${_maxName}\ax

/return ${_maxName}



|***
 * note: returns a rank if there is one
 * use: ${set_rank[DEBUG, NAME]}
 ***|
sub set_rank(bool _debug, string _spell)
  DEBUG \atset_rank\ac(\a-w${_spell}\ax)

  /if (${Me.Book[${Spell[${_spell}].RankName}]}) {
    /if (!${Select[${Me.Subscription},GOLD]}) {
      DEBUG ${sep} returning basename${sep}\a-w${Spell[${_spell}].BaseName}\ax
      /return ${Spell[${_spell}].BaseName}
    } else {
      DEBUG ${sep} returning rankname${sep}\a-w${Spell[${_spell}].BaseName}\ax
      /return ${Spell[${_spell}].RankName}
    }
  }

  DEBUG ${sep} returning${sep}\a-w${Spell[${_spell}].BaseName}\ax

/return ${_spell}



|***
 * note: find the best pet you have in your book
 * use: ${get_best_in_pet[DEBUG]}
 ***|
sub get_best_in_pet(bool _debug)
  DEBUG \atget_best_in_pet\ax()

  /declare _running         int local 0
  /declare _max             int local 0
  /declare _maxName         string local FALSE
  /declare _subcat          string local FALSE
  /declare _count           int local 0

  /if (${Select[${Me.Class.ShortName},ENC]}) {
    DEBUG ${sep}found ENC${sep}\a-wSum: Animation\ax
    /varset _subcat Sum: Animation
  } else /if (${Select[${Me.Class.ShortName},BST,SHM]}) {
    DEBUG ${sep}found BST,SHM${sep}\a-wSum: Warder\ax
    /varset _subcat Sum: Warder
  } else /if (${Select[${Me.Class.ShortName},NEC,SHD]}) {
    DEBUG ${sep}found NEC,SDH${sep}\a-wSum: Undead\ax
    /varset _subcat Sum: Undead
  } else /if (${Select[${Me.Class.ShortName},MAG]}) {
    DEBUG ${sep}found MAG${sep}\a-wSum: ${maChr.Find[stMinionElementType].Value}\ax
    /varset _subcat Sum: ${maChr.Find[stMinionElementType].Value}
  }

  /for _count 1 to ${maHard.Find[stSpellBookSize].Value}
    /if (!${Me.Book[${_count}].ID}) /continue
    /if (${Me.Book[${_count}].Subcategory.NotEqual[${_subcat}]}) /continue
    /if (${Me.Book[${_count}].TargetType.NotEqual[Self]}) /continue
    
    DEBUG ${sep}found pet spell${sep}\a-w${Me.Book[${_count}].ID}${submenu}${Me.Book[${_count}].Name}\ax (\a-rlvl ${Me.Book[${_count}].Level}\ax)

    /varset _running ${Me.Book[${_count}].Level}
    /if (${_running} >= ${_max}) {
      /varset _max ${_running}
      /varset _maxName ${Me.Book[${_count}]}
    }

  /next _count

  DEBUG ${sep}Using pet spell${sep}\a-w${_maxName}\ax

/return ${_maxName}



|***
 * note: I can see the fucking future!
 * use:
 ***|
sub get_prediction(bool _debug)
  DEBUG \atget_prediction\ax()

  | on/off control
  /if (!${maPred.Find[stPredCountPrepBurn].Value} && !${maPred.Find[stPredCountPrepClass].Value}) {
    /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
    DEBUG ${break}turn some shit on.. seriously
    /return FALSE
  }

  /declare _tmpID int local ${get_assist_info[${_debug}, id]}

  | no assist
  /if (!${_tmpID} && !HEAL) {
    /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
    DEBUG ${break}NO ASSIST found/set
    /return FALSE
  } else /if (!${_tmpID} && HEAL) {
    /if (${Group.MainAssist.ID}) {
      /varset _tmpID ${Group.MainAssist.ID}
    } else {
      /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
      DEBUG ${break}NO ASSIST found/set
      /return FALSE
    }
  }

  | get spawncount around the assist
  DEBUG SpawnCount[npc loc ${Spawn[id ${_tmpID}].X} ${Spawn[id ${_tmpID}].Y} ${Spawn[id ${_tmpID}].Z} radius ${maPred.Find[stPredRad].Value} PS4]${sep}\a-w${SpawnCount[npc loc ${Spawn[id ${_tmpID}].X} ${Spawn[id ${_tmpID}].Y} ${Spawn[id ${_tmpID}].Z} radius ${maPred.Find[stPredRad].Value} playerstate ${maPred.Find[stPredPlayerstate].Value}]}\ax
  /declare _countMob ${SpawnCount[npc loc ${Spawn[id ${_tmpID}].X} ${Spawn[id ${_tmpID}].Y} ${Spawn[id ${_tmpID}].Z} radius ${maPred.Find[stPredRad].Value} playerstate ${maPred.Find[stPredPlayerstate].Value}]}

  | no mo bs
  /if (!${_countMob}) {
    /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
    DEBUG ${break}NO mob count
    /return FALSE
  }

  | more reasons to not do this??

  | assist is still relatively close
  /if (${Spawn[id ${_tmpID}].Distance} < SAFEENV) {
    DEBUG ${break}too close, dont bother
    /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
    /return FALSE
  }

  OUT Predicting${sep}\a-w${_countMob}\ax

  | give us a small break before checking again. predicting the future is hard work
  /invoke ${set_data_timer[${_debug}, Check_Prediction, ${maPred.Find[stPredDelay].Value}s]}

  /call get_assist_target ${_debug}

  | class specific subs set up for prediction
  /if (${maPred.Find[stPredCountPrepClass].Value}) {
    /if (${SubDefined[prediction]}) {
      /call prediction ${maDebug.Find[pred].Value}
    }
  }

  | start some class stuff
  /if (${maPred.Find[stPredCountPrepClass].Value} && TARGET) {
    /if (${maPred.Find[stPredCountPrepClass].Value} >= ${_countMob}) {
      /call misc_combat ${maDebug.Find[misc].Value} TARGET
    }
  }

  | do what burns can be done w/o a target.
  | will varry considerably by class
  /if (${maPred.Find[stPredCountPrepBurn].Value}) {
    /if (${maPred.Find[stPredCountPrepBurn].Value} >= ${_countMob}) {
      /call Bind_control_burn spinup
    }
  }

/return TRUE



|***
 * note: use GTFO oout or not
 * use: /call control_GTFO [option]
 ***|
sub control_GTFO(string _verbage, bool _debug)
  DEBUG \atcontrol_GTFO\ax(\a-w${_verbage}\ax)

  /invoke ${set_data_timer[${_debug}, Check_GTFO, RESTART]}
  
  /if (SAFEZONE) /return FALSE
  
  ISMEDEAD

  | forced?
  /if (${_verbage.Equal[force]}) {
    /call check_gtfo FALSE ${_debug}
    /return TRUE
  }

  /if (!${maGTFO.Find[swGTFOActive].Value}) {
    /return FALSE
  }

  | enough people dead in party?
  /if (${maGTFO.Find[stCountGTFODead].Value}) {
    /if (${Group.Injured[0]} >= ${maGTFO.Find[stCountGTFODead].Value}) {
      /call check_gtfo FALSE ${_debug}
      /return TRUE
    }
  }
  
  | too many mobs in area?
  /if (${maGTFO.Find[stCountGTFOSpawns].Value}) {
    /if (${SpawnCount[npc radius ENV PS4]} >= ${maGTFO.Find[stCountGTFOSpawns].Value}) {
      /call check_gtfo FALSE ${_debug}
      /return TRUE
    }
  }

/return TRUE



|***
 * note: defensive things
 * use: /call check_gtfo DEBUG
 ***|
sub check_gtfo(bool _debug)
  DEBUG \atcheck_gtfo\ax(\a-w\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineGTFO.Contains[EMPTY]} && ${lsLineGTFO.Count} == 1) {
    DEBUG ${sep}resetting the empty gtfo line
    /invoke ${lsLineGTFO.Clear}
    /invoke ${lsLineGTFO.Append[${stCount12}]}
  }

  /declare _liC12 listiterator local
  /vardata _liC12 lsCount12.First.Clone
  /while (!${_liC12.IsEnd}) {

    | check the switch
    /if (!${maGTFO.Find[swGTFO${_liC12.Value}].Value}) {
      /invoke ${_liC12.Advance}
      /continue
    }

    | check the item name
    /if (!${Bool[${maGTFO.Find[stGTFO${_liC12.Value}].Value}]}) {
      /invoke ${_liC12.Advance}
      /continue
    }

    | ready?
    /if (!${is_ready[${_debug}, "${maGTFO.Find[stGTFO${_liC12.Value}].Value}"]}) {
      /invoke ${_liC12.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} GTFO ${_liC12.Value} ${Me.DisplayName} "${maGTFO.Find[stGTFO${_liC12.Value}].Value}" check_gtfo
    /if (!${Macro.Return}) {
      /invoke ${_liC12.Advance}
      /continue
    }

    DEBUG ${sep}_liC12.Value${sep}\a-w${_liC12.Value}\ax${sep}passed tags

    ISMEDEAD

    /call voodoo ${_debug} "${maGTFO.Find[stGTFO${_liC12.Value}].Value}" 0 FALSE
    /return TRUE

    /invoke ${_liC12.Advance}
  }

/return TRUE



|***
 * note: casts skills from maPull for engine2
 * use: /call cast_cycle_agro DEBUG TEMPID
 ***|
sub pull_get_other_type(bool _debug, int _tmpID)
  DEBUG \atpull_get_other_type\ax(\a-w${_tmpID}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLinePull.Contains[EMPTY]} && ${lsLinePull.Count} == 1) {
    DEBUG ${sep}resetting the empty pull line
    /invoke ${lsLinePull.Clear}
    /invoke ${lsLinePull.Append[${stCount12}]}
  }

  /declare _liC12 listiterator local
  /vardata _liC12 lsLinePull.First.Clone
  /while (!${_liC12.IsEnd}) {

    /if (${target[isdead, ${_tmpID}]}) {
      DEBUG ${break}target dead
      /return FALSE
    }

    GETINPUT cast_cycle_agro NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_agro

    /invoke ${target[set, ${_tmpID}]}

    DEBUG ${sep}${_liC12}${sep}\a-w${maPull.Find[stPull${_liC12}].Value}\ax

    | check the switch
    /if (!${maPull.Find[swPull${_liC12}].Value}) {
      DEBUG ${break}switch off${sep}\a-w${maPull.Find[swPull${_liC12}].Value}\ax
      /invoke ${_liC12.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maPull.Find[stPull${_liC12}].Value}]}) {
      DEBUG ${break}no name${sep}\a-w${maPull.Find[stPull${_liC12}].Value}\ax
      /invoke ${_liC12.Advance}
      /continue
    }

    /if (!${is_ready[${_debug}, "${maPull.Find[stPull${_liC12}].Value}"]}) {
      DEBUG ${sep}not ready${sep}\a-w${maPull.Find[stPull${_liC12}].Value}\ax
      /invoke ${_liC12.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Pull ${_liC12.Value} ${Me.DisplayName} "${maPull.Find[stPull${_liC12}].Value}" pull_get_other_type
    /if (!${Macro.Return}) {
      /invoke ${_liC12.Advance}
      /continue
    }

    /call voodoo ${_debug} "${maPull.Find[stPull${_liC12}].Value}" ${_tmpID} FALSE PULLENV
    
    | good cast, get out...
    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,BLOCKED,RESISTED,IMMUNE]}) {
      DEBUG ${break}good use${sep}\a-w${maPull.Find[stPull${_liC12}].Value}\ax
      /return TRUE
    }
      
    /invoke ${_liC12.Advance}
  }

/return TRUE



|***
 * note: scans the toons inventory to find something and make a list of them
 * note: the end list will be lsInventoryNAME
 * use: /call scan_inventory_for DEBUG NAME BASE ATTRIB
 ***|
sub scan_inventory_for(bool _debug, string _name, int _base, int _attrib)
  DEBUG \atscan_inventory_for\ax(\a-w${_name}, ${_base}, ${_attrib}\ax)

  /declare _invItem         int local 0
  /declare _containerItem   int local 0
  /declare _tmpItemID       int local 0

  /if (!${Defined[lsInventory${_name}]}) {
    /declare lsInventory${_name} list outer
    /invoke ${lsInventory${_name}.Delimiter["|"]}
  } else {
    /invoke ${lsInventory${_name}.Clear}
  }

  | loop all inventory items
  /for _invItem 1 to 32

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
        DEBUG ${sep}\ay${_invItem}\ax${sep}${Me.Inventory[${_invItem}].Item[${_containerItem}].Name} :: ${Me.Inventory[${_invItem}].Item[${_containerItem}].ID}

        | is the attrib on it that we are looking for
        /if (${FindItem[${_tmpItemID}].Spell.Attrib[${_base}]} != ${_attrib}) /continue

        | can i use it?
        /if (${Me.Level} < ${FindItem[${_tmpItemID}].RequiredLevel}) /continue

        | passed out checks, add to the list and move on
        /invoke ${lsInventory${_name}.Append[${FindItem[${_tmpItemID}].Name}]}

      /next _containerItem

    | if its not a container? top level inv/gear
    } else {

      /varset _tmpItemID ${Me.Inventory[${_invItem}].ID}
      /if (${FindItem[${_tmpItemID}].Spell.Attrib[${_base}]} != ${_attrib}) /continue
      /if (${Me.Level} < ${FindItem[${_tmpItemID}].RequiredLevel}) /continue
      /invoke ${lsInventory${_name}.Append[${FindItem[${_tmpItemID}].Name}]}

    }

  /next _invItem

  | did we find something
  /if (!${lsInventory${_name}.Count}) {
    /return FALSE
  }

/return TRUE











|***
 * note: 
 * use: 
 ***|
sub do_loot_corpse(bool _debug)
  DEBUG \atdo_loot_corpse\ax()

  /if (!${Target.ID} || !${Target.Dead}) {
    DEBUG ${break}NO TARGET
    /return FALSE
  }

  /declare _item int local 0
  /declare _itemName string local
  /declare _itemSlot int local
  /declare _corpseCount int local ${Corpse.Items}
  /declare _advlItem string local
  /declare _advlChoice string local

  /squelch /loot
  /delay 5

  /delay 1s ${Window[LootWnd].Open}
  /delay 1s ${Corpse.Items}

  /if (!${Corpse.Items}) {
    /if (${Corpse.Open}) {
      /notify LootWnd DoneButton leftmouseup
      /delay 1s !${Window[LootWnd].Open}
    }
  }
  
  DEBUG ${sep}_corpseCount${sep}\a-w${Corpse.Items}\ax

  /for _item 1 to ${_corpseCount}

    /if (!${Me.FreeInventory}) {
      DEBUG ${break}Me.FreeInventory
      OUT Setting loot${sep}\a-woff\ax
      OUT No more inventory space.
      /invoke ${do_raw_edit[${_debug}, SILENT, stLootMode, "off"]}
      /return FALSE
    }

    /delay 1s ${Corpse.Item[${_item}].ID}

    /varset _itemName ${Corpse.Item[${_item}].Name}
    /varset _itemSlot ${Corpse.Item[=${_itemName}].ItemSlot}
    DEBUG ${sep}_itemName${sep}\a-w${_itemName}\ax \a-p.............\ax \aw${_itemSlot}\ax
    
    /sqlite query "${DBloot}" slootquery SELECT * FROM loot_control WHERE item="${_itemName}";
    /if (!${sql_check[${_debug}, slootquery, loot_control, "SELECT (loot_control)"]}) /endmacro

    | if we did not find the item?
    /if (!${sqlite.Rows[slootquery]}) {

      /if (${lsGrain.Contains[14]}) {
        OUT Loot${sep}${sep}\a-w${_itemName}\ax (\a-rnot in database\ax)
      }
      /if (${Corpse.Item[${_itemName}].NoDrop} || ${FindItem[=${_itemName}].Lore} || ${FindItemBank[=${_itemName}].Lore}) {
        /if (${lsGrain.Contains[14]}) {
          OUT Loot${sep}${add}${sep}\a-w${_itemName}\ax (\a-r${leave} [no drop/lore]\ax)
        }
        /sqlite query "${DBloot}" slootquery INSERT OR REPLACE INTO loot_control(item,action) VALUES("${_itemName}", "leave");
        /if (!${sql_check[${_debug}, slootquery, loot_control, "INSERT OR REPLACE INTO (loot_control)"]}) /endmacro
        DEBUG ${sep}Leaving${sep}\a-w${_itemName}\ax
        /continue

      } else {
        /if (${lsGrain.Contains[14]}) {
          OUT Loot${sep}${add}${sep}\a-w${_itemName}\ax (\a-r${keep} [default]\ax)
        }
        | set item as keep
        /sqlite query "${DBloot}" slootquery INSERT OR REPLACE INTO loot_control(item,action) VALUES("${_itemName}", "keep");
        /if (!${sql_check[${_debug}, slootquery, loot_control, "INSERT OR REPLACE INTO (loot_control)"]}) /endmacro
      }

      | reload lootquery
      /sqlite query "${DBloot}" slootquery SELECT * FROM loot_control WHERE item="${_itemName}";
      /if (!${sql_check[${_debug}, slootquery, loot_control, "SELECT (loot_control)"]}) /endmacro

    }

    | set our actions
    /varset _advlChoice ${sqlite.Result[slootquery 1 action]}
    DEBUG ${sep}_advlChoice${sep}\a-w${_advlChoice}\ax

    | now we do something with the item.
    | ditch the item if we need too
    /if (${Select[${_advlChoice},leave]} || ${FindItem[=${_itemName}].Lore} || ${FindItemBank[=${_itemName}].Lore}) {

      /if (${Select[${_advlChoice},Announce]} && ${lsGrain.Contains[14]}) {
        DEBUG ${sep}eaving${sep}\a-w${_itemName}\ax
        OUT Loot${sep}\ayLeaving\ax${sep}\a-w${_itemName}\ax
        /continue
      }
      
    | or we keep it.
    } else /if (${Select[${_advlChoice},keep,destroy,sell]}) {
      DEBUG ${sep}keep,destroy,sell${sep}\a-w${_itemName}\ax ${sep} ${_itemSlot}
      /squelch /shift /itemnotify loot${_item} leftmouseup
      /delay 1s ${Cursor.ID}

      /while (${Cursor.ID}) {
        /if (${Select[${_advlChoice},keep,sell]}) {
          DEBUG ${sep}keep,sell${sep}\a-w${_itemName}\ax
          /autoinventory
        } else /if (${Select[${_advlChoice},destroy]}) {
          DEBUG ${sep}destroy${sep}\a-w${_itemName}\ax
          /if (${Cursor.Name.Equal[${_itemName}]}) {
            /destroy
          }
        }
        /delay 1
      }

    }

  /next _item

  /delay 1

  /if (${Corpse.Open}) {
    /notify LootWnd DoneButton leftmouseup
    /delay 1s !${Window[LootWnd].Open}
  }

/return TRUE



|***
 * note: temp buffs for toon
 * use: part of standard buff cycle if enabled
 ***|
sub check_buff_cycle_temp(bool _debug)
  DEBUG \atcheck_buff_cycle_temp\ax()

  /if (AUTO) {
    /invoke ${set_data_timer[${_debug}, Check_Buff_Temp, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  SETHUD "buff temp"

  CHECKTIE
  ISMEDEAD
  CHECKEXIT check_buff_cycle_temp
  GETINPUT check_buff_cycle_temp NA|NA

  /declare _spell string local FALSE
  /declare _spellRank string local FALSE
  /declare _count int local 0
  /declare _countToon int local 1
  /declare _target bool local FALSE
  /declare _toonName string local FALSE
  /declare _found bool TRUE
  /declare _i int local 0
  /declare _tmpBuff string local FALSE
  /declare _tmpID int local 0

  DEBUG ${sep}Buff${sep}\a-wTemp\ax

  | each toon
  /while (${_countToon} <= ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}) {
    DEBUG ${sep}${_countToon} <= ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}

    | get the name to something usable and proper case
    /varset _toonName ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|].Left[1].Upper}${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|].Right[-1].Lower}
    DEBUG ${sep}_toonName${sep}\ay${_toonName}\ax

    | skip this person?
    /if (!${Spawn[pc ${_toonName}].ID} || ${Spawn[pc ${_toonName}].Dead}) {
      DEBUG ${sep}skipping${sep}\a-w${_toonName}\ax
      /varcalc _countToon ${_countToon}+1
      /continue
    }

    | any buff slots available?
    /dquery ${_toonName} -q "Me.FreeBuffSlots" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    /if (${DNQReturn.Equal[0]}) {
      /varcalc _countToon ${_countToon}+1
      /continue
    }

    | each buff
    /for _count 1 to 50
    
      /varset _tmpBuff 

      | buff has no name
      /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
        /continue
      | buff is turned off
      } else /if (!${maBuff.Find[swBuff${_count}].Value}) {
        /continue
      | not a temp tag buff
      } else /if (!${lsBuff${_count}Tag.Contains[temp]}) {
        /continue
      | bitch isnt on the list
      } else /if (!${lsBuff${_count}Tag.Contains[${_toonName}]}) {
        /continue
      }
      
      | buffs with SDPA parsing to be done
      /if (${maBuff.Find[stBuff${_count}].Value.Count[|]} == 3) {
        /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_count}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[3,|]}"
        /invoke ${maData.Add[tmpName,${Macro.Return}]}

      | get item clickie spells
      } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
        DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
        /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
    
      | just the spell/aa/whatever
      } else {
        DEBUG ${sep}spell${sep}${maBuff.Find[stBuff${_count}].Value}
        /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
      }

      | cycle the tags
      /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_toonName} "${maData.Find[tmpName].Value}" check_buff_cycle_temp
      /if (!${Macro.Return}) {
        DEBUG ${sep}skip${sep}failed tag cycle check
        /continue
      }

      DEBUG ${sep}\agBUFF TEMP\ax#${sep}\a-w${_count}\ax \a-p---------------------\ax

      | pretend they are not a target until they qualify
      /varset _target FALSE

      | is this an acceptable target for casting
      /if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_toonName}]}) {
        DEBUG ${sep}target${sep}\a-w${_toonName}\ax (\a-rvalid\ax)
        /varset _target TRUE
      }

      /varset _spell ${set_rank[${_debug}, "${maData.Find[tmpName].Value}"]}
      DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax

      | distance check
      /if (${Select[${Spell[${_spell}].TargetType},Group v2]}) {
        DEBUG ${sep}distance check${sep}\a-wGroup v2\ax
        /if (${Spawn[pc ${_toonName}].Distance} > ${Spell[${_spell}].AERange}) /continue
      } else {
        DEBUG ${sep}distance check${sep}\a-wtargeted ?? ${Spell[${_spell}].MyRange}\ax
        /if (${Spell[${_spell}].MyRange}) {
          /if (${Spawn[pc ${_toonName}].Distance} > ${Spell[${_spell}].MyRange} && !${Select[${Spell[${_spell}].TargetType},self]}) /continue
        }
      }

      /varset _spellRank ${Spell[${maData.Find[tmpName].Value}].RankName}
      DEBUG ${sep}\awTrigger\ax${sep}\a-w${_spellRank}\ax ID:${Spell[${_spellRank}].ID}

      | does it stack
      /dquery ${_toonName} -q "Spell[${_spellRank}].Stacks" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${sep}${sep}STACKS: .Buff.Stacks \at${DNQReturn}\ax ${dot} ${_spellRank}
      /if (${DNQReturn.Equal[FALSE]}) {
        /continue
      }

      | do they have the buff?
      /dquery ${_toonName} -q "Me.Buff[${maData.Find[tmpName].Value}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${sep}${sep}HAVE: .Buff.ID \at${DNQReturn}\ax ${dot} ${maData.Find[tmpName].Value}
      /if (${Bool[${DNQReturn}]}) {
        /continue
      }

      DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${maData.Find[tmpName].Value}

      /if (${_target}) {
        
        | self only redirect
        /if (!${Spell[${_spell}].TargetType.Equal[self]}) {
          /varset _tmpID ${Spawn[nopet ${_toonName}].ID}
        } else {
          /varset _tmpID 0
        }
        
        /call voodoo ${_debug} "${maData.Find[tmpName].Value}" ${_tmpID} FALSE
        /return TRUE
      }


    /next _count
    /varcalc _countToon ${_countToon}+1

  }

  SETHUD EMPTY

/return TRUE
 
 
 
|***
 * note: change the case of something
 * use: ${proper_case[type, "what to change"]}
 ***| 
sub proper_case(string _type, string _verbage) 

  | proper name
  /if (${_type.Equal[name]}) {
    /return ${_verbage.Left[1].Upper}${_verbage.Right[-1].Lower}
  
  | roles for group/raid
  } /if (${_type.Equal[roles]}) {
    /if (${_verbage.Equal[Leader]}) /return Leader
    /if (${_verbage.Equal[MainTank]}) /return MainTank
    /if (${_verbage.Equal[MainAssist]}) /return MainAssist
    /if (${_verbage.Equal[MasterLooter]}) /return MasterLooter
    /if (${_verbage.Equal[MarkNPC]}) /return MarkNPC
    /if (${_verbage.Equal[Puller]}) /return Puller
    
  }

/return FALSE



|***
 * note: basic list output
 * use: /call out_list LISTNAME
 ***|    
sub out_list(string _list)

  /declare _liwtf listiterator local
  /vardata _liwtf ${_list}.First.Clone
  /while (!${_liwtf.IsEnd}) {
    /invoke ${out[12, "${dot}\a-w${_liwtf.Value}\ax"]}
    /invoke ${_liwtf.Advance}
  }
  /invoke ${out[18, 0, 33]}

/return TRUE



|***
 * note: shared / command control
 * use: 
 ***|
sub control_broadcast_command(string _command, string _option, string _verbage, bool _debug)
  DEBUG \atcontrol_broadcast_command\ax(\a-wc:${_command} o:${_option} v:${_verbage}\ax)

  /varset _command ${proper_case[name, ${_command}]}

  /if (${_option.Equal[aux]}) {
    /call Bind_command_aux ${_command}
    /return break
  } else /if (${_option.Equal[-h]}) {
    OUT /${_command.Lower} [self|group|zone|raid|all]
    /return break
  } else /if (${Select[${_option},-o,broadcast]}) {
    /if (${lsBroadcastTypes.Contains[${_verbage}]}) {
      /call do_raw_edit FALSE SILENT stBroadcast${_command} "${_verbage.Lower}"
    } else /if (${Bool[${_verbage}]} && !${lsBroadcastTypes.Contains[${_verbage}]}) {
      OUT Options${sep}\a-w/${_command.Lower} broadcast\ax
      /call out_list lsBroadcastTypes
      /return break
    }

    /invoke ${maControl.Clear}
    /invoke ${maControl.Add[broadcast,stBroadcast${_command}]}
    /invoke ${set_control_output[FALSE, maEntropy, ${_command.Lower}, maControl]}
    /return break
  }
   
  
  | self only command 
  /if (${_option.Equal[self]}) {   
    /invoke ${out[21, "/${_command.Lower} (\a-rself\ax)"]}
  
    /if (${_command.Equal[on]}) {
      /invoke ${maEnv.Add[swAuto,TRUE]}
    } else /if (${_command.Equal[off]}) {
      /invoke ${maEnv.Add[swAuto,FALSE]}
    }  
    /return TRUE

  | all group broadcast
  } else /if (${_option.Equal[group]} || !${Bool[${_option}]} && ${maEntropy.Find[stBroadcast${_command}].Value.Equal[group]}) {
    /if (!${Group.Members}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/${_command.Lower}\ax (\a-rgroup\ax) (\a-rFail: not in a group\ax)"]}
      /return break
    } 

    /invoke ${out[21, "${broadcast}${sep}\a-w/${_command.Lower}\ax (\a-rgroup\ax)"]}

    /if (${Select[${_command},on,off]}) {
      /dggexecute /command_${_command} self
    } else {
      /dggexecute /${_command} self
    }

  | zone broadcasts
  } else /if (${_option.Equal[zone]} || !${Bool[${_option}]} && ${maEntropy.Find[stBroadcast${_command}].Value.Equal[zone]}) {
    /invoke ${out[21, "${broadcast}${sep}\a-w/${_command.Lower}\ax (\a-rzone\ax)"]}

    /if (${Select[${_command},on,off]}) {
      /dgzexecute /command_${_command} self
    } else {
      /dgzexecute /${_command} self
    }

  | all entropy broadcasts
  } else /if (${_option.Equal[all]} || !${Bool[${_option}]} && ${maEntropy.Find[stBroadcast${_command}].Value.Equal[all]}) {
    /invoke ${out[21, "${broadcast}${sep}\a-w/${_command.Lower}\ax (\a-rall\ax)"]}
 
    /if (${Select[${_command},on,off]}) {
      /dgexecute ${maComm.Find[stEntropyGroup_all].Value} /command_${_command} self
    } else {
      /dgexecute ${maComm.Find[stEntropyGroup_all].Value} /${_command} self
    }

  | all raid broadcast
  } else /if (${_option.Equal[raid]} || !${Bool[${_option}]} && ${maEntropy.Find[stBroadcast${_command}].Value.Equal[raid]}) {
    /if (!${Raid.Members}) {
      /invoke ${out[21, "${broadcast}${sep}\a-w/${_command.Lower}\ax (\a-rraid\ax) (\a-rFail: not in a raid\ax)"]}
      /return break
    }
    /invoke ${out[21, "${broadcast}${sep}\a-w/${_command.Lower}\ax (\a-rraid\ax)"]}

    /if (${Select[${_command},on,off]}) {
      /dgrexecute /command_${_command} self
    } else {
      /dgrexecute /${_command} self
    }
  }
  
  /if (${Select[${_command},here,on,off]}) {
    /return break
  }
  
/return TRUE



|***
 * note: sets the needed observers for non pullers to know what the pullers are on whte way back with
 * use: /call set_observer_prediction DEBUG [not used option]
 ***|
sub set_observer_prediction(bool _debug, string _verbage)
  DEBUG \atset_observer_prediction\ax(${_verbage})


 | maPull.Find[swPull].Value
 | maPull.Find[stPullRadius].Value
 | stPullMode == no base


  /declare _assist string local FALSE
  /call setDeclare MainAssistAgro string outer
  /call setDeclare MainAssistCombat string outer
  /call setDeclare MainAssistMacro string outer


  | check the toons running macro
  /if (!${DanNet[${_assist}].ObserveSet["Macro.Name"]}) /dobserve ${_assist} -q "Macro.Name" -o MainAssistMacro
  DEBUG ${sep}MainAssistMacro set:\a-w${MainAssistMacro}\ax

  | no ma or forced delete
  /if (${_verbage.Equal[kill]}) {
    /dobserve ${_assist} -q "setMobAgro" -drop
    /dobserve ${_assist} -q "combat" -drop
    /dobserve ${_assist} -q "Macro.Name" -drop
    /deletevar MainAssistAgro
    /deletevar MainAssistCombat
    /deletevar MainAssistMacro
    /return FALSE
  }

  | set MA agro range
  /if (!${DanNet[${_assist}].ObserveSet["setMobAgro"]}) /dobserve ${_assist} -q "setMobAgro" -o MainAssistAgro
  DEBUG ${sep}MainAssistAgro set:\a-w${MainAssistAgro}\ax

  | set MA combat status
  /if (!${DanNet[${_assist}].ObserveSet["combat"]}) /dobserve ${_assist} -q "combat" -o MainAssistCombat
  DEBUG ${sep}MainAssistCombat set:\a-w${MainAssistCombat}\ax

/return TRUE



|***
 * note: sets a class shortname on or off for varioius controls
 * use: /call set_control_shorntame DEBUG LIST CLASSSHORTNAME
 ***|
sub set_control_shortname(bool _debug, string _list, string _cls)
  DEBUG \atset_control_shortname\ax(\a-w${_list}, ${_cls}\ax)
  
  /if (!${Bool[${_cls}]} && ${lsClassAll.Contains[${Target.Class.ShortName}]}) {
    /varset _cls ${Target.Class.ShortName}
  } else /if (${Bool[${_cls}]} && ${lsClassAll.Contains[${_cls.Upper}]}) {
    /varset _cls ${_cls.Upper}
  } 

  /if (${lsClassAll.Contains[${_cls}]}) {
    /if (${${_list}.Contains[${_cls}]}) {
      /call do_raw_edit ${_debug} SILENT ${_list} "${_cls}" delete
    } else /if (!${${_list}.Contains[${_cls}]}) {
      /call do_raw_edit ${_debug} SILENT ${_list} "${_cls}" add
    }
  }

  /declare _out string local
  /declare _lisccsn listiterator local
  /vardata _lisccsn lsClassAll.First.Clone
  /while (!${_lisccsn.IsEnd}) {
    /varset _out ${_out} ${If[${_lisccsn.Value.Equal[${lsClassAll.First}]},,${dot}]} ${info}${_lisccsn.Value}\ax
    /if (${${_list}.Contains[${_lisccsn.Value}]}) /varset _out ${_out}:${on}
    /if (!${${_list}.Contains[${_lisccsn.Value}]}) /varset _out ${_out}:${off}
    /invoke ${_lisccsn.Advance}
  }

/return ${_out}



|***
 * note: check if a character exists in the crew shared DB
 * use: 
 ***|
sub crew_check_character(bool _debug, string _verbage, bool _forceadd, string _verbage2)
  DEBUG \atcrew_check_character\ax(\a-w${_verbage}\ax)
  
  | fix the character name case
  /varset _verbage ${proper_case[name, "${_verbage}"]}

  /sqlite query "${DBshared}" qucheckcharacter SELECT id FROM characters WHERE charactername='${_verbage}';
  DEBUG ${sep}.Rows (\a-rqucheckcharacter\ax)${sep}\a-w${sqlite.Rows[qucheckcharacter]}\ax

  /if (!${sqlite.Rows[qucheckcharacter]} && !${_forceadd}) {
    OUT /crew ${_verbage2}${sep}\a-w${_verbage}\ax (\a-rFail\ax)
    /sqlite clear qucheckcharacter
    /return FALSE
  }    

  /if (!${sqlite.Rows[qucheckcharacter]} && ${_forceadd}) {
    /sqlite query "${DBshared}" dummy INSERT INTO characters(charactername, server) VALUES ("${_verbage}", "${MacroQuest.Server.Replace[ ,_].Lower}")
    /return FALSE
  } 

/return TRUE



|***
 * note: cycles melee skills from maMelee
 * use: /call cast_cycle_melee DEBUG [TARGETID] [TAG]
 ***|
sub cast_cycle_melee(bool _debug, int _tmpID, string _tag)
  DEBUG \atcast_cycle_melee\ax(\a-w${_tmpID}, ${_tag}\ax)

  /if (${target[isdead, ${_tmpID}]}) {
    /return FALSE
  }

  CHECKDEFENSE

  /invoke ${target[set, ${_tmpID}]}

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /if (AUTO || ${swModeToken}) {
    /if (!${timer_check_stick}) {
      /call set_combat_control ${maDebug.Find[decision].Value} ${_tmpID}
    }
  }

  | melee bandolier swap for proc weapons
  /if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
    /call check_sash_proc ${_debug}
  }

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineMelee.Contains[EMPTY]} && ${lsLineMelee.Count} == 1) {
    DEBUG ${sep}resetting the empty melee line
    /invoke ${lsLineMelee.Clear}
    /invoke ${lsLineMelee.Append[${stCount12}]}
  }
  
  /declare _count int local 0
  /declare _match bool local FALSE
  /declare _liMelee listiterator local
  /vardata _liMelee lsLineMelee.First.Clone
  /while (!${_liMelee.IsEnd}) {

    GETINPUT cast_cycle_melee NA|NA
    ISMEDEAD
    CHECKEXIT cast_cycle_melee

    /if (${target[isdead, ${_tmpID}]}) {
      /return FALSE
    }

    /invoke ${target[set, ${_tmpID}]}

    | check the switch
    /if (!${Bool[${maMelee.Find[swMelee${_liMelee.Value}].Value}]}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maMelee.Find[stMelee${_liMelee.Value}].Value}]}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    | skip autoskills
    /if (${Skill[${maMelee.Find[stMelee${_liMelee.Value}].Value}].Auto}) {
      /invoke ${_liMelee.Advance}
      /continue
    }
    
    | check matching tag for failure
    /if (${Bool[${_tag}]}) {
      /if (!${lsMelee${_liMelee.Value}Tag.Contains[${_tag}]}) {
        /invoke ${_liMelee.Advance}
        /continue
      }
    }

    /if (!${is_ready[${_debug}, "${maMelee.Find[stMelee${_liMelee.Value}].Value}"]}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    | minimum range
    /if (${Spawn[${_tmpID}].Distance} > ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Melee ${_liMelee.Value} ${Me.DisplayName} "${maMelee.Find[stMelee${_liMelee.Value}].Value}" cast_cycle_melee
    /if (!${Macro.Return}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    /invoke ${out[8, "Melee${sep}${sp}${maMelee.Find[stMelee${_liMelee.Value}].Value}\ax ${dot} ${con_color[${_debug}, ${_tmpID}]}\ax"]}
    /doability "${maMelee.Find[stMelee${_liMelee.Value}].Value}"
    /invoke ${_liMelee.Advance}
  }

/return TRUE



|***
 * note: delete any conflicting aliases
 * use: see the note....
 ***|
sub delete_conflict_alias(bool _debug)

  /declare lsControls string outer buff,list,condition,aux,item,target,defense,heal,dot,nuke,agro,miscdps,song,burn,sash,out,rez,pred

  /declare _liAlias list local
  /invoke ${_liAlias.Append[debug,${lsControls}]}

  /declare _lial listiterator local
  /vardata _lial _liAlias.First.Clone
  /while (!${_lial.IsEnd}) {
    /squelch /alias /${_lial.Value} delete
    /invoke ${_lial.Advance}
  }

/return TRUE



|***
 * note: macro and dannet integrity check routine
 * use:
 ***|
sub check_housekeeping(bool _debug)
  DEBUG \atcheck_housekeeping\ax()

  /invoke ${set_data_timer[${_debug}, Check_Housekeeping, RESTART]}

  /if (${Select[${Me.CombatState},combat]}) {
    /return FALSE
  }

  | correct assist vs puller issues for group MA
  /if (PULL && (${Group.MainAssist.ID} == ${Me.ID}) && ${Bool[${maCC.Find[stAssistMode].Value}]}) {
    /call do_raw_edit ${_debug} SILENT stAssistMode FALSE
  }

  | set blocked buffs
  /call set_buffs_blocked FALSE SILENT

  | make sure we are not in the core_communications group. does not play well with others
  /if (${DanNet.Peers[core_communications].Find[${Me.DisplayName}]}) {
    /squelch /dleave core_communications
  }

  | error control
  /if (${maCC.Find[swCombatHeal].Value} && !${Select[${Me.Class.ShortName},CLR,SHM,DRU]}) {
    /invoke ${set_switch_env[${_debug}, swCombatHeal, TRUE, FALSE]}
  }
  /if (${maCC.Find[swCombatControl].Value} && !${Select[${Me.Class.ShortName},ENC,BRD,NEC,MAG]}) {
    /invoke ${set_switch_env[${_debug}, swCombatControl, TRUE, FALSE]}
  }

  DEBUG ${sep}housekeeping${sep}\a-wDONE\ax

/return TRUE



|***
 * note: return home from a mob pull
 * use:
 ***|
sub pull_home_move_to(bool _debug, int _tmpID)
  DEBUG \atpull_home_move_to\ax(\a-w${_tmpID}\ax)

  /if (!AUTO && !${swModeToken}) {
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

  /delay 5 ${Navigation.Active}
  /while (${Navigation.Active}) {
    CHECKDEAD

    | do we slow down if we start to outrun a mob?
    /if (${maPull.Find[stPullOutrunRange].Value} && !${Me.Mount.ID}) {
      DEBUG ${sep}stPullOutrunRange${sep}\a-w${maPull.Find[stPullOutrunRange].Value}\ax  ..  ${Spawn[${_tmpID}].Distance}
      | set walk
      /if (${Spawn[${_tmpID}].Distance} > ${maPull.Find[stPullOutrunRange].Value} && ${Me.Running}) {
        DEBUG ${sep}movement${sep}walk
        /keypress RUN_WALK
      }
      /delay 1 ${Spawn[${_tmpID}].Distance} <= ${maPull.Find[stPullOutrunRange].Value}
      | return to run
      /if (${Spawn[${_tmpID}].Distance} <= ${maPull.Find[stPullOutrunRange].Value} && !${Me.Running}) {
        DEBUG ${sep}movement${sep}run
        /keypress RUN_WALK
      }     
      
    }
    /delay 2

  }

  | make sure run is set back...
  /if (!${Me.Running}) {
    DEBUG ${sep}movement${sep}run
    /keypress RUN_WALK
  }

/return TRUE





|***
 * note: decide on pull or assist
 * use: 
 ***|
sub pull_assist_decision(bool _debug)
  DEBUG \atpull_assist_decision\ax

  DEBUG ${submenu}pullstate (\a-rassist/pull check\ax)

  /if (!PULL) {

    | prediction
    /if (${maPred.Find[swPred].Value} && !${timer_Check_Prediction}) {
      /call get_prediction ${maDebug.Find[prediction].Value}
    }

    DEBUG ${submenu}pullstate (\a-rget assist\ax)
    /call get_assist_target ${maDebug.Find[assist].Value}

  } else /if (PULL) {
    
    | alternate pullers
    /if (${Group.MainAssist.ID} != ${Me.ID} && ${Group.Members}) {
      DEBUG ${submenu}pullstate (\a-ralternate puller\ax)

      | chain pulling
      /if (${maPull.Find[stCountChainPull].Value} && ${maPull.Find[stPullMode].Value.NotEqual[base]} && ${SpawnCount[NPC los radius SAFEENV PS4 targetable]} <= ${maPull.Find[stCountChainPull].Value}) {
        DEBUG ${submenu}pullstate (\a-rchain pull\ax)
          
          | hardstop check
          /if (!${SpawnCount[NPC radius ENV PS4 targetable]}) {
            DEBUG ${submenu}pullstate (\a-rhardstop: no mobs\ax)
            /if (!${timer_Check_Pull_Hardstop}) {
              /call pull_hardstop ${maDebug.Find[hardstop].Value}
              /if (${Macro.Return}) {
                /return FALSE
              }
            } else /if (${timer_Check_Pull_Hardstop}) {
              DEBUG ${submenu}pullstate (\a-rhardstop Timer${sep}${timer_Check_Pull_Hardstop}\ax)
              /delay 1
              /return FALSE
            }
          }

          DEBUG ${submenu}pullstate (\a-radvanced: prep chain\ax)
          /call pull_prep_advanced ${maDebug.Find[pull].Value}
          DEBUG ${submenu}pullstate (\a-radvanced: ${maPull.Find[stPullMode].Value} chain\ax)
          /call get_target_${maPull.Find[stPullMode].Value}pull ${maDebug.Find[pull].Value} 0 ${Int[${Math.Calc[${maPull.Find[stCountChainPull].Value} + 1]}]}
   
      }
      
      | break pulling if a mob is in ENV AND LOS and go back to assist
      /if (${SpawnCount[npc los radius ENV PS4]}) {
        DEBUG ${submenu}pullstate (\a-rswapping to assist for los proximity agro\ax)
        /call get_assist_target ${maDebug.Find[assist].Value}
        /return FALSE
      }
    }
   

    | pulling when there is no established target    
    /if (!TARGET) {
      DEBUG ${submenu}pullstate (\a-rpull check\ax)

      | hardstop check
      /if (!${SpawnCount[NPC radius ENV PS4 targetable]}) {
        DEBUG ${submenu}pullstate (\a-rhardstop: no mobs\ax)
        /if (!${timer_Check_Pull_Hardstop}) {
          /call pull_hardstop ${maDebug.Find[hardstop].Value}
          /if (${Macro.Return}) {
            /return FALSE
          }
        } else /if (${timer_Check_Pull_Hardstop}) {
          DEBUG ${submenu}pullstate (\a-rhardstop Timer${sep}${timer_Check_Pull_Hardstop}\ax)
          /delay 1
          /return FALSE
        }
      }

      /if (${maPull.Find[stPullMode].Value.Equal[base]}) {
        DEBUG ${submenu}pullstate (\a-rbase\ax)
        /call get_target_basepull ${maDebug.Find[pull].Value}
      } else {
        DEBUG ${submenu}pullstate (\a-radvanced: prep\ax)
        /call pull_prep_advanced ${maDebug.Find[pull].Value}
        DEBUG ${submenu}pullstate (\a-radvanced: ${maPull.Find[stPullMode].Value}\ax)
        /call get_target_${maPull.Find[stPullMode].Value}pull ${maDebug.Find[pull].Value}
      }
    }
    
    /delay 1
  }

/return TRUE








|***
 * note: pulling a mob with an intervention pull
 * use:
 ***|
sub pull_mob_intervention(bool _debug, int _tmpID)
  DEBUG \atpull_mob_intervention\ax(\a-w${_tmpID}\ax)




    /call cast_cycle_heal ${_debug} ${_tmpID} interventionpull




/return TRUE





|***
 * note: returns a valid resident toon name if there is one from a list of dannet groups
 * use: /call get_resident DEBUG [DANNET GROUPs] || ${get_resident[DEBUG, |CLS|CLS|CLS, distance, [notme]]}
 ***|
sub get_resident(bool _debug, string _groupList, int _distance, string _verbage)
  DEBUG \atget_resident\ax(\a-w${_groupList}, ${_distance}, ${_verbage}\ax)

  /declare _countGroup int local 0
  /declare _countName int local 0
  /declare _group string local FALSE
  /declare _name string local FALSE
  
  /for _countGroup 1 to ${_groupList.Count[|]}
    /varset _group ${_groupList.Arg[${_countGroup},|]}

    | group empty / does not exist
    /if (!${DanNet.PeerCount[${_group}]}) {
      /continue
    }

    | check each name in the group. bail on FIRST good match
    /for _countName 1 to ${DanNet.Peers[${_group}].Count[|]}
      /varset _name ${DanNet.Peers[${_group}].Arg[${_countName},|]}

      | never use the MA
      /if (${Group.MainAssist.Name.Equal[${_name}]} || ${_verbage.Equal[notme]}) {
        DEBUG ${break}dont use me, use someone else
        /continue
      }

      | no name.. wtf?
      /if (!${Bool[pc ${_name}]}) {
        DEBUG ${break}bad name?
        /continue
      }

      | pc not here?
      /if (!${Spawn[pc ${_name}].ID}) {
        DEBUG ${break}pc NOT here
        /continue
      }

      | bitches are dead
      /if (${Spawn[pc ${_name}].Dead}) {
        DEBUG ${break}dead
        /continue
      }

      | too far away to be of any damn use
      /if (${Spawn[pc ${_name}].Distance} > ${_distance}) {
        DEBUG ${break}pc.distance > ${_distance}
        /continue
      }

      | we made it this far give em a name
      DEBUG ${sep}get_name_multipull ${sep} ${_name}
      /return ${_name}

    /next _countName
  /next _countGroup

/return FALSE

