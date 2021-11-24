|*********************************************************************
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
      /varset _previousValue ${${_map}.Find[${_env_var}].Value}

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
    
    /if (${pregenUpdate}) {
      /sqlite query "${DBcharacter}" dummy UPDATE environment_${_table} SET value="${_value}" WHERE env_var="${_env_var}";
      /if (!${sql_check[${_debug}, dummy, set_data_map, "UPDATE (environment_${_table})"]}) /endmacro
    } else {
      /sqlite query "${DBcharacter}" dummy INSERT OR IGNORE INTO environment_${_table}(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${_aux}");
      /if (!${sql_check[${_debug}, dummy, set_data_map, "INSERT OR IGNORE INTO (environment_${_table})"]}) /endmacro
      /invoke ${${_map}.Add[${_env_var},"${_value}"]}
    }
    
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
sub set_control_num_range(bool _debug, string _variable, string _value, int _low, int _high)
  DEBUG \atset_control_num_range\ax(\a-w${_variable}, "${_value}", "${_low}", ${_high}\ax)
  
  /invoke ${maData.Add[outVar,${_variable}]}
  /invoke ${maData.Add[outVal,${_value}]}
  /declare _valid bool local FALSE
  
  | allow 0 and falses
  /if (${Select[${_value},"0","FALSE"]}) {
    DEBUG ${sep} "${_value}" == "0","FALSE"
    /varset _valid TRUE
  }

  | just another check
  /if (${_value.Length} > 0 && ${Math.Abs[${_value}]} > -1) {
    DEBUG ${sep}Math.Abs[${_value}] > -1
    /varset _valid TRUE
  }

  | in range
  /if (${_value.Length} > 0 && ${Range.Between[${_low},${_high}:${Int[${_value}]}]}) {
    DEBUG ${sep} Range.Between[${_low},${_high}:${Int[${_value}]}]
    /varset _valid TRUE
  }

  /if (!${_valid}) {
    DEBUG ${sep} _valid=FALSE
    /invoke ${out[1, shade, 0]}
    /if (${_low} == 0) {
      /invoke ${out[0, "${notice}${sep}Control Range ${dot} ${info}${_low}\ax(\aroff\ax)${info} - ${_high}\ax"]}
      /return FALSE
    } else {
      /invoke ${out[0, "${notice}${sep}Control Range ${dot} ${info}${_low} - ${_high}\ax"]}
      /return FALSE
    }
  } else {
    /call do_raw_edit ${_debug} SILENT "${_variable}" "${_value}"
    /return TRUE

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
    /declare _toon string local

    | each toon
    /for _count 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}

      | get the name to something usable
      /varset _toon ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
      DEBUG ${sep}_toon${sep}\ay${_toon}\ax

      | is the observer set
      /if (!${DanNet[${_toon}].ObserveSet["Me.PctHPs"]} || !${DanNet[${_toon}].ObserveSet["Me.Class.ShortName"]} || !${DanNet[${_toon}].ObserveSet["Me.ID"]}) {
        DEBUG ${break}defined${sep}!\a-w${_toon}_PctHPs\ax
        /continue
      }

      DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}${Spawn[pc ${_toon}].DisplayName}${sep}${Spawn[pc ${_toon}].ID}\ax

      /if (${DanNet[${_toon}].Observe["Me.PctHPs"]} > ${maHeal.Find[stHealPoint${DanNet[${_toon}].Observe["Me.Class.ShortName"]}].Value}*.PCTHO) {
        DEBUG ${break} > HealPoint
        /continue
      } else /if (${Spawn[pc ${_toon}].Distance} > ENV) {
        DEBUG ${break} > env radius
        /continue
      } else /if (${Spawn[pc ${_toon}].Dead}) {
        DEBUG ${break} dead toon
        /varcalc hurtDead ${hurtDead} + 1
        /continue
      } else /if (!${Spawn[pc ${_toon}].ID}) {
        DEBUG ${break} not in zone
        /continue
      } else /if (${Spawn[pc ${_toon}].DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) {
        DEBUG ${break} rampage tank
        /continue
      }

      | hurt running count
      /if (${DanNet[${_toon}].Observe["Me.PctHPs"]} <= ${maHeal.Find[stHealPoint${DanNet[${_toon}].Observe["Me.Class.ShortName"]}].Value}*.PCTHO) {
        DEBUG ${sep}hurtCount +1
        /varcalc hurtCount ${hurtCount} + 1
      }

      /if (${maHeal.Find[swHealWeighted].Value}) {
        DEBUG ${sep}_count${sep}\a-w${_toon}\ax${sep}\a-o(${DanNet[${_toon}].Observe["Me.PctHPs"]} - .${maHeal.Find[stHealPoint${DanNet[${_toon}].Observe["Me.Class.ShortName"]}].Value}) / ${maHeal.Find[stHealPoint${DanNet[${_toon}].Observe["Me.Class.ShortName"]}].Value}\ax
        /varset _tmpWeight ${Float[${Math.Calc[(${DanNet[${_toon}].Observe["Me.PctHPs"]} - .${maHeal.Find[stHealPoint${DanNet[${_toon}].Observe["Me.Class.ShortName"]}].Value}) / ${maHeal.Find[stHealPoint${DanNet[${_toon}].Observe["Me.Class.ShortName"]}].Value}].Precision[5]}]}
        DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}_tmpWeight${sep}\a-w${_tmpWeight}\ax
        /if (${Float[${_tmpWeight}].Precision[5]} < ${Float[${_hurtWeight}].Precision[5]}) {
          /varset _hurtWeight ${Float[${_tmpWeight}].Precision[5]}
          /varset hurtPctHP ${DanNet[${_toon}].Observe["Me.PctHPs"]}
          /varset hurtID ${DanNet[${_toon}].Observe["Me.ID"]}
          /varset hurtCLS ${DanNet[${_toon}].Observe["Me.Class.ShortName"]}
        }
      } else /if (!${maHeal.Find[swHealWeighted].Value}) {
        /varset hurtPctHP ${DanNet[${_toon}].Observe["Me.PctHPs"]}
        /varset hurtID ${DanNet[${_toon}].Observe["Me.ID"]}
        /varset hurtCLS ${DanNet[${_toon}].Observe["Me.Class.ShortName"]}
      }

    /next _count
    /return ${hurtCount}


  }

/return 0



|***
 * note: checks facing of a spawn
 * use: /invoke ${spawn_face[spawn]}
 ***|
sub spawn_face(spawn _spawn)

  /if (!${_spawn.ID}) {
    /return FALSE
  }

  /if (!AUTO && !${swModeToken}) {
    /return FALSE
  }

  /if (!${Range.Between[0,${maHome.Find[stFaceArc].Value}:${Math.Abs[${Math.Calc[${Target.HeadingTo.Degrees}-${Me.Heading.Degrees}]}]}]}) {
    /squelch /face id ${_spawn.ID} nolook ${If[${maHome.Find[swFaceFast].Value},fast,]}
  }

/return TRUE



|***
 * note: finds youre current assisst and returns their info Name, ID, or both
 * use: ${get_assist_info[DEBUG, name|id|both]}
 ***|
sub get_assist_info(bool _debug, string _type)
  DEBUG \atget_assist_info\ax(\a-w${_type}\ax)

  | healers that may not have an assst set
  /if (!${Select[${maCC.Find[stAssistMode].Value},1,2,3,g]} && ${lsAss.Contains[EMPTY]} && ${lsAss.Count} == 1 && ${maCC.Find[swCombatHeal].Value} && ${Raid} && ${Raid.MainAssist.ID} && !${maCC.Find[swManualAssist].Value}) {
    DEBUG ${sep}healer default raid main assist
    /vardata assistData Spawn[${Raid.MainAssist.ID}].ID

  } else /if (!${Select[${maCC.Find[stAssistMode].Value},1,2,3,g]} && ${lsAss.Contains[EMPTY]} && ${lsAss.Count} == 1 && ${maCC.Find[swCombatHeal].Value} && !${Raid.Members} && ${Group.Members} && ${Group.MainAssist.ID} && !${maCC.Find[swManualAssist].Value}) {
    DEBUG ${sep}healer default group assist
    /vardata assistData Spawn[${Group.MainAssist.ID}].ID

  | other group assist
  } else /if (${Select[${maCC.Find[stAssistMode].Value},g]} && ${Group.MainAssist.ID} && !${Group.MainAssist.Dead} && ${Group.Members} && !${maCC.Find[swManualAssist].Value}) {
    DEBUG ${sep}assist found${sep}\a-w${Group.MainAssist.DisplayName}\ax
    /vardata assistData Spawn[${Group.MainAssist.ID}].ID
    
  | raid assist 1
  } else /if (${Select[${maCC.Find[stAssistMode].Value},1]} && ${Raid.MainAssist[1].ID} && !${Raid.MainAssist[1].Dead} && ${Raid.Members} && !${maCC.Find[swManualAssist].Value}) {
    DEBUG ${sep}assist found${sep}\a-w${Raid.MainAssist[1].DisplayName}\ax
    /vardata assistData Spawn[${Raid.MainAssist[1].ID}].ID
    
  | raid assist 2
  } else /if (${Select[${maCC.Find[stAssistMode].Value},2]} && ${Raid.MainAssist[2].ID} && !${Raid.MainAssist[2].Dead} && ${Raid.Members} && !${maCC.Find[swManualAssist].Value}) {
    DEBUG ${sep}assist found${sep}\a-w${Raid.MainAssist[2].DisplayName}\ax
    /vardata assistData Spawn[${Raid.MainAssist[2].ID}]
    
  | raid assist 3
  } else /if (${Select[${maCC.Find[stAssistMode].Value},3]} && ${Raid.MainAssist[2].ID} && !${Raid.MainAssist[3].Dead} && ${Raid.Members} && !${maCC.Find[swManualAssist].Value}) {
    DEBUG ${sep}assist found${sep}\a-w${Raid.MainAssist[3].DisplayName}\ax
    /vardata assistData Spawn[${Raid.MainAssist[3].ID}]

  } else /if (!${lsAss.Contains[EMPTY]} && ${lsAss.Count} >= 1 || ${maCC.Find[swManualAssist].Value}) {
    DEBUG list
    /if (!${lsAss.Contains[EMPTY]} && ${lsAss.Count} >= 1) {
      /declare _ligai listiterator local
      /vardata _ligai lsAss.First.Clone
      /while (!${_ligai.IsEnd}) {
        
        /if (!${Spawn[${_ligai.Value}].ID} || ${Spawn[${_ligai.Value}].Dead}) {
          /invoke ${_ligai.Advance}
          /continue
        }
        /if (${Spawn[${_ligai.Value}].Distance} > ENV)  {
          /invoke ${_ligai.Advance}
          /continue
        }
        /vardata assistData Spawn[${_ligai.Value}]

        /invoke ${_ligai.Advance}
      } 
    }
  }

  
  | no assist found?!
  /if (!${assistData.ID})   {
    DEBUG ${sep}\arNO\ax ASSIST FOUND
    /return FALSE
  }

  | return the assist info requested
  /if (${_type.Equal[name]}) {
    DEBUG ${sep}returning assist${sep}\a-w${assistData}\ax
    /return ${assistData.DisplayName}
  } else /if (${_type.Equal[id]}) {
    DEBUG ${sep}returning assist${sep}\a-w${assistData.ID}\ax
    /return ${assistData.ID}
  } else /if (${_type.Equal[both]}) {
    DEBUG ${sep}returning assist${sep}\a-w|${assistData}|${assistData.ID}\ax
    /return |${assistData.DisplayName}|${assistData.ID}
  } else {
    DEBUG ${sep}returning assist${sep}\arFALSE\ax
    /invoke ${out[0, 0, 61]}
    /return FALSE
  }

/return FALSE



|***
 * note: basic 3-way switch control for environment settings for TRUE/FALSE
 * use: /call set_switch_env DEBUG [switchname] [SQL] [value] || ${set_switch_env[DEBUG, switchname, SQL T/F, value]}
 ***|
sub set_switch_env(bool _debug, string _switch, bool _sql, string _verbage)
  DEBUG \atset_switch_env\ax(\a-wsw:${_switch}, sql:${_sql}, v:${_verbage}\ax)

  /declare _map string local ${get_key_map[${_debug}, ${_switch}]}

  /invoke ${maData.Add[outVar,${_switch}]}
  /invoke ${maData.Add[outVal,${_verbage}]}

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
  
  /invoke ${maData.Add[outVar,${_switch}]}
  /invoke ${maData.Add[outVal,${${_map}.Find[${_switch}].Value}]}

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
  } else /if (${cast_data[${_debug}, "Glyph of Fireworks II"]}) {
    /invoke ${out[1, 0, 62]}
    /call cast ${_debug} 0 FALSE
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
      } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(85 percent)"]}]}) {
        /varset _stRezWindowPct 85
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
      /delay 1s
      /if (${maEntropy.Find[swRNDRezTake].Value}) {
        /delay ${maEntropy.Find[stRNDVariance].Value}
      }
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
      /squelch /target clear
      /varset stAssTarID 0
      /varset targetData 0
      /varset swIsMeDead FALSE
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

      /varset swIsMeDead FALSE
      /varset _swValidRezFound FALSE
      /doevents REZ_ACCEPT

      /call set_stop ${maDebug.Find[stop].Value} SILENT

      
      |**
      /if (${maHome.Find[swHomeMarker].Value}) {
        /call Bind_control_home clear NULL NULL ${maDebug.Find[home].Value}
      }
      **|
      
      /if (${maHome.Find[swMoveDeathClear].Value}) {
        /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
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

  /invoke ${set_data_timer[FALSE, Check_Buff_Shrink, RESTART]}

  /if (!${maBuff.Find[swBuffMaster].Value}) {
    /return FALSE
  }

  SETHUD "buff shrink"

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_buff_cycle_shrink

  GETINPUT check_buff_cycle_shrink NA|NA

  /if (!${cast_data[${_debug}, "${maBuff.Find[stShrinkItem].Value}"]}) {
    /return FALSE
  }

  /declare _count int local 0

  | me shrinks
  /while (${Me.Height} > ${maBuff.Find[stShrinkHeight].Value} && ${_count} < 3) {
    /call cast ${_debug} ${Me.ID} FALSE
    /varcalc _count ${_count}+1
    /delay 5
  }

  SETHUD

/return TRUE



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
  
  | clear panic move
  /invoke ${maData.Add[swGetOverHere,FALSE]}

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

  SETHUD

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
 * note: moves the toon to a valid/LoS location of a spawn id with a given distance
 * use : /call moveto_range [spawnID] [DISTANCE]
 **|
sub moveto_range(bool _debug, spawn _spawn, int _range)
  DEBUG \atmoveto_range\ax(\a-w${_spawn.ID}, ${_range}\ax)
  
  /declare _i int local 0
  /declare _locX float local 0
  /declare _locY float local 0
  /declare _locZ float local 0
  /declare _tmpX float local 0
  /declare _tmpY float local 0
  /declare _dist float local ${Math.Calc[${If[${_range}==0,1,${_range}]} / ${If[${_spawn.Distance}==0,1,${_spawn.Distance}]}]}

  DEBUG ${sep}{Math.Calc[${If[${_range}==0,1,${_range}]} / ${If[${_spawn.Distance}==0,1,${_spawn.Distance}]}]}
  DEBUG ${sep}_dist${sep}\a-w${_dist}\ax

  /varcalc _locX ${_dist}*${Me.X} + (1-${_dist})*${_spawn.X}
  /varcalc _locY ${_dist}*${Me.Y} + (1-${_dist})*${_spawn.Y}
  /if (${Navigation.PathExists[locyxz ${_locY} ${_locX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_locX} ${_locY} ${Me.Z}]} && ${LineOfSight[${_locY},${_locX},${Me.Z}:${_spawn.Y},${_spawn.X},${_spawn.Z}]}) {
    DEBUG {Navigation.PathExists[locyxz ${_locY} ${_locX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_locX} ${_locY} ${Me.Z}]} && ${LineOfSight[${_locY},${_locX},${Me.Z}:${_spawn.Y},${_spawn.X},${_spawn.Z}]
    DEBUG ${sep}(\a-r0\ax) locyxz ${_locY} ${_locX} ${Me.Z}
    /nav locyxz ${_locY} ${_locX} ${Me.Z} |log=${maEntropy.Find[swNavLogSpew].Value}
    /delay 5 ${Navigation.Active}
    /delay 5s !${Navigation.Active}
    /return TRUE
  }

  /for _i 1 to 20
    /varcalc _tmpX ${_locX}*${cos[${_i}]} - ${_locY}*${sin[${_i}]}
    /varcalc _tmpY ${_locX}*${cos[${_i}]} + ${_locY}*${sin[${_i}]}
    /if (${Navigation.PathExists[locyxz ${_tmpY} ${_tmpX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_tmpX} ${_tmpY} ${Me.Z}]} && ${LineOfSight[${_tmpY},${_tmpX},${Me.Z}:${_spawn.Y},${_spawn.X},${_spawn.Z}]}) {
      DEBUG ${sep}(\a-r-${_i}\ax) locyxz ${_locY} ${_locX} ${Me.Z}
      /nav locyxz ${_tmpY} ${_tmpX} ${Me.Z} |log=${maEntropy.Find[swNavLogSpew].Value}
      /delay 5 ${Navigation.Active}
      /delay 5s !${Navigation.Active}
      /return TRUE
    }

    /varcalc _tmpX ${_locX}*${cos[${_i}]} + ${_locY}*${sin[${_i}]}
    /varcalc _tmpY ${_locX}*${cos[${_i}]} - ${_locY}*${sin[${_i}]}
    /if (${Navigation.PathExists[locyxz ${_tmpY} ${_tmpX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_tmpX} ${_tmpY} ${Me.Z}]} && ${LineOfSight[${_tmpY},${_tmpX},${Me.Z}:${_spawn.Y},${_spawn.X},${_spawn.Z}]}) {
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

  /invoke ${set_data_timer[${_debug}, Check_Xtarget_Build, RESTART]}

  /if (!${Me.XTargetSlots}) {
    /return FALSE
  }
  
  /if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
    /invoke ${out[1, shade, 0]}
    /return FALSE
  }

  /invoke ${out[0, "Adjusting XTargets${sep}${If[${Bool[${_forced}]},\a-wForced\ax,Timer \a-w${maTimer.Find[tiCheck_Xtarget_Build].Value}\ax]}"]}
  /declare _x int local
  /declare _slot int local 0
  /declare _count int local 0
  /declare _spawn spawn local

  | reset extended target slots
  /call set_xtarget_clear ${_debug}

  | observe any assists for xtarget 1
  | /if (${Select[${maCC.Find[stAssistMode].Value},G,1,2,3]} || ${Bool[${maCC.Find[stManualAssist1].Value}]} || ${Bool[${maCC.Find[stManualAssist2].Value}]}) {
  |   /varset _slot 1
  | }

  | protect specific people outside your group
  /if (${lsProtectOutsideGroup.Count}) {
    DEBUG ${sep}setting${sep}\a-wlsProtectOutsideGroup\ax

    /declare _lipcxt listiterator local
    /vardata _lipcxt lsProtectOutsideGroup.First.Clone
    /while (!${_lipcxt.IsEnd}) {
      
      | out of xtarget slots?
      /if (${_slot} > ${Me.XTargetSlots}) {
        DEBUG ${break} out of slots
        /return TRUE
      }

      /if (!${Spawn[${_lipcxt.Value}].ID}) {
        /invoke ${_lipcxt.Advance}
        /continue
      }

      /vardata _spawn Spawn[${_lipcxt.Value}]
      
      /if (!${_spawn.ID}) {
        /invoke ${_lipcxt.Advance}
        /continue
      }

      | i hate people
      /if (${lsNoHealToon.Contains[${_spawn.DisplayName}]}) {
        DEBUG ${break}
        /invoke ${_lipcxt.Advance}
        /continue
      }

      | in our range of operaions?
      /if (${_spawn.Distance} > ENV) {
        DEBUG ${break}
        /invoke ${_lipcxt.Advance}
        /continue
      }

      | are they already in our group?
      /if (${Group.Member[${_spawn.DisplayName}].ID}) {
        DEBUG ${break}
        /invoke ${_lipcxt.Advance}
        /continue
      }

      /if (${Me.XTarget[${_spawn.DisplayName}].ID}) {
        /varcalc _slot ${_slot}+1
      } else /if (${_spawn.ID}) {
        /varcalc _slot ${_slot}+1
        DEBUG ${sep}lsProtectOutsideGroup${sep}\a-wxtarget set ${_slot} ${_spawn.Name}\ax
        /xtarget set ${_slot} ${_spawn.Name}
        /delay 1
      }

      /invoke ${_lipcxt.Advance}
    }
  }

  | set raid member classes to protect
  /if (${Raid.Members}) {
    DEBUG ${sep}setting${sep}\a-wRaid.Members\ax
    /declare _lixt listiterator local
    /vardata _lixt lsHealXTClass.First.Clone
    /while (!${_lixt.IsEnd}) {
      DEBUG ${sep}checking class${sep}\a-w${_lixt.Value}\ax (\a-r${SpawnCount[pc ${_lixt.Value} raid radius ENV]}\ax)

      | no class in range
      /if (!${SpawnCount[pc ${_lixt.Value} raid radius ENV]}) {
        DEBUG ${break}none in range for xtarget${sep}\a-w${_lixt.Value}\ax
        /invoke ${_lixt.Advance}
        /continue
      }

      | cycle the class and add to slots
      /for _x 1 to ${SpawnCount[pc ${_lixt.Value} raid radius ENV]}

        /if (${_slot} > ${Me.XTargetSlots}) {
          /return TRUE
        }
        
        /vardata _spawn NearestSpawn[${_x}, pc ${_lixt.Value} raid radius ENV]
        
        | yes.. i still hate people
        /if (${lsNoHealToon.Contains[${_spawn.DisplayName}]}) {
          DEBUG ${break}lsNoHealToon
          /continue
        }

        | are they a group member?
        /if (${Group.Member[${_spawn.DisplayName}].ID}) {
          DEBUG ${break}in group
          /continue
        }

        | on the xtarget
        /if (${Me.XTarget[${_spawn.DisplayName}].ID}) {
          DEBUG ${break}in xtarget
          /continue
        }
        
        | skip lsProtectOutsideGroup list. it is already covered
        /if (${lsProtectOutsideGroup.Contains[${_spawn.DisplayName}]}) {
          DEBUG ${break}lsProtectOutsideGroup
          /continue
        }

        | me?
        /if (${_spawn.ID} == ${Me.ID}) {
          DEBUG ${break}${_spawn.ID} == ${Me.ID}
          /continue
        }

        /varcalc _slot ${_slot}+1

        /if (${_slot} > 13) {
          /return TRUE
        }
        DEBUG ${sep}Raid.Members${sep}\a-wxtarget set ${_slot} ${_spawn.DisplayName}\ax
        /xtarget set ${_slot} ${_spawn.DisplayName}
        /delay 1
      /next _x

      /invoke ${_lixt.Advance}
    }

  }

/return TRUE



|***
 * note: Clear all combat variables
 * use: /call clear_combat DEBUG
 ***|
sub clear_combat(bool _debug)
  DEBUG \atclear_combat\ax()

  GETINPUT clear_combat NA|NA

  /varset stAssTarID 0
  /varset targetData 0
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

  /if (AUTO) {
    /if (!${maOver.Find[swTargetClear].Value}) {
      /squelch /target clear
    }
  }

/return TRUE



|***
 * note: Make sure that what ever we are targeting is something we want to kill
 * note: in manual mode, anything is a valid target.
 * use: ${is_target_valid[DEBUG, spawn]}
**|
sub is_target_valid(bool _debug, spawn _spawn)
  DEBUG \atis_target_valid\ax(\a-w${_spawn.ID}\ax)

  /if (!AUTO) {
    /return TRUE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT is_target_valid _spawn|${_spawn.ID}

  /if (!${Target.ID}) /return FALSE
  /if (!${Bool[${_spawn.ID}]}) /return FALSE
  /if (SAFEZONE) /return FALSE

  | check smart assists
  /if (${_spawn.ID} != ${Target.ID}) {
    /invoke ${target[clear]}
    /return FALSE
  }

  | shit we really shouldn't be attacking
  /if (${lsNoAttackBodyType.Contains[${_spawn.Type.Lower}]}) /return FALSE
  /if (${lsNoTargetNPC.Contains[${_spawn.DisplayName}]}) /return FALSE
  /if (${lsNoTargetPC.Contains[${_spawn.DisplayName}]}) /return FALSE
  /if (${lsNoTargetMagic.Contains[${_spawn.DisplayName}]}) /return FALSE
  /if (${lsAnimationNoTarget.Contains[${_spawn.DisplayName}]}) /return FALSE
  /if (${lsZoneExclude.Contains[${_spawn.DisplayName}]}) /return FALSE


  /if (!PULL) {
    /if (${Target.Mezzed.Duration}) /return FALSE
    /if ((!${_spawn.LineOfSight} && !${maOver.Find[swOverLOS].Value}) || ${_spawn.Distance} > ENV) /return FALSE
  } else /if (PULL) {
    /if (${_spawn.Distance} > PULLENV) /return FALSE
  }
  /if (${_spawn.Type.Equal[PC]}) /return FALSE
  /if (${_spawn.Master.Type.Equal[PC]}) /return FALSE
  /if (${Target.Type.Equal[PC]}) /invoke ${maCC.Add[swValidAssTarID,FALSE]}
  /if (${_spawn.ID} == ${Me.ID} || ${_spawn.ID} == ${Me.Pet.ID}) /return FALSE

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
  /if (${maBurn.Find[swBurnAuto].Value} && !HEAL) {
    | target is named ?
    /if (${lsZoneNamed.Contains[${Target.DisplayName}]} && !${maOver.Find[swOverName].Value}) {
      DEBUG ${break}target is named${sep}\a-wreturning TRUE\ax
      /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
      /return TRUE
    }
    

    | engage %
    /if (${maEntropy.Find[stRNDEngageBurn].Value} && !${maData.Find[stRNDEngageBurnSet].Value}) {
      /declare _high int local ${Math.Calc[${maBurn.Find[stPctBurnEngage].Value} + ${maEntropy.Find[stRNDEngageBurn].Value}]}
      /declare _low int local ${Math.Calc[${maBurn.Find[stPctBurnEngage].Value} - ${maEntropy.Find[stRNDEngageBurn].Value}]}

      /if (${_high} > 99) {
        /varset _high 99
      }
      /invoke ${maData.Add[stPctEngageBurnSet,${Math.Rand[${_low},${_high}]}]}

    } else /if (${maEntropy.Find[stRNDEngageBurn].Value} && ${maData.Find[stRNDEngageBurnSet].Value}) {

    } else {
      /invoke ${maData.Add[stPctEngageBurnSet,${maBurn.Find[stPctBurnEngage].Value}]} 
    }
      
    /if (${targetData.PctHPs} <= ${maData.Find[stPctEngageBurnSet].Value}) {
      DEBUG ${break}burn engage %${sep}\a-w${targetData.PctHPs}\ax
      /invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
      /return TRUE
    }  
  
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
    /if (!${maOver.Find[swOverPctEngage].Value} && !PULL && !${maPull.Find[stCountChainPull].Value}) {
      
      | randomize the engage % a little bit
      /if (${maEntropy.Find[stRNDEngage].Value} && !${maData.Find[stRNDEngageSet].Value}) {
        /declare _high int local ${Math.Calc[${maCC.Find[stPctEngage].Value} + ${maEntropy.Find[stRNDEngage].Value}]}
        /declare _low int local ${Math.Calc[${maCC.Find[stPctEngage].Value} - ${maEntropy.Find[stRNDEngage].Value}]}

        /if (${_high} > 99) {
          /varset _high 99
        }
        /invoke ${maData.Add[stPctEngageSet,${Math.Rand[${_low},${_high}]}]}

      } else /if (${maEntropy.Find[stRNDEngage].Value} && ${maData.Find[stRNDEngageSet].Value}) {

 
      | default to base engage %
      } else {
        /invoke ${maData.Add[stPctEngageSet,${maCC.Find[stPctEngage].Value}]} 
      }
      
      /if (${Spawn[TARGET].PctHPs} > ${maData.Find[stPctEngageSet].Value}) {
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
    /if (HEAL && !${maCC.Find[swSetForced].Value}) {
      DEBUG ${sep}heal
      /invoke ${maCC.Add[swSetCombat,FALSE]}
    }
    /if (!COMBAT) {
      DEBUG ${break}manual${sep}no combat
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

    /if (BARD) {
      /if (${Me.Song[Rallying Solo].ID}) /return FALSE
    }

    /if (${is_target_valid[${_debug}, TARGET]}) {
      /invoke ${maCC.Add[swValidAssTarID,TRUE]}
    }

    /if (!VALID || !${Spawn[TARGET].ID}) {
      | /invoke ${target[clear]}
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
  /if (!AUTO && ${Me.Invis}) {
    /call Bind_control_nomore invis ${_debug}
  }

  | not within ${combatradius} and far enought to range?
  /if (${Target.Distance} > 30 && ${Target.Distance} > ${Spawn[id TARGET].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {

    | range mobs if enabled
    /if (RANGE) {

      /invoke ${spawn_face[TARGET]}

      | no need to move were are at a good distance
      /if (${Stick.Active}) {
        /squelch /stick off
      }

      /if (${lsZoneRanged.Contains[${Target.DisplayName}]}) {
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
        /invoke ${spawn_face[TARGET]}
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
        /call moveto_range ${_debug} TARGET ${maHard.Find[stMinMovetoBow].Value}
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
        /invoke ${spawn_face[TARGET]}

        /if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
          /call check_sash_proc ${_debug}
        } else {
          /call cycle_sash ${maDebug.Find[sash].Value}
        }

        /if (MELEE) {
          /squelch /attack on
        }
        /if (AUTO && !${timer_Check_Behind}) {
          /call set_combat_control ${maDebug.Find[decision].Value} TARGET
        }
      }
    }
  }

/return



|***
 * note: decision tree for what kind of combat to do
 * use: /call set_combat_control DEBUG [spawnID]
 ***|
sub set_combat_control(bool _debug, spawn _spawn)
  DEBUG \atset_combat_control\ax(\a-w${_spawn.ID}\ax)

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

  /if (!AUTO && ${maCC.Find[swSetForced].Value} && ${Target.ID}) {
    /vardata _spawn Target.ID
  }

  SPAWNDEAD

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
    /call Bind_control_nomore invis ${_debug}
  }

  /if (${Target.ID} && (MELEE || RANGE) && ${_spawn.LineOfSight}) {
    /invoke ${spawn_face[${_spawn.ID}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}S

  | range or melee, priority is range!
  /if (MELEE && RANGE) {
    DEBUG ${sep}\agmelee\ax${sep}\ayor\ax${sep}\agrange\ax

    /if (${lsZoneRanged.Contains[${Target.DisplayName}]} || ${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && ${maCC.Find[swStoponDS].Value} && !AGRO)) {
      /call set_combat_range ${_debug} ${_spawn.ID}
      /return TRUE
    }

    /if (${Target.Distance} >= ${maHard.Find[stMinBowDist].Value}) {
      DEBUG ${sep}...range${sep}\a-w${Target.Distance}\ax
      /call set_combat_range ${_debug} ${_spawn.ID}
    } else /if (${Target.Distance} < ${maHard.Find[stMinBowDist].Value}) {
      DEBUG ${sep}...melee${sep}\a-w${Target.Distance}\ax
      /call set_combat_melee ${_debug} ${_spawn.ID}
    }
    /return TRUE

  | melee combat only
  } else /if (MELEE && !RANGE) {
    DEBUG ${sep}\agmelee\ax${sep}\arrange\ax

    /if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && ${maCC.Find[swStoponDS].Value} && !AGRO)) {
      /call set_combat_range ${_debug} ${_spawn.ID}
      /return TRUE
    }

    /call set_combat_melee ${_debug} ${_spawn.ID}
    /return TRUE

  | range combat only
  } else /if (RANGE && !MELEE) {
    DEBUG ${sep}\agrange\ax${sep}\armelee\ax
    /call set_combat_range ${_debug} ${_spawn.ID}
    /return TRUE

  }

/return TRUE



|***
 * note: sets range combat
 * use: /call set_combat_range DEBUG [spawn]
 ***|
sub set_combat_range(bool _debug, spawn _spawn)
  DEBUG \atset_combat_range\ax(\a-w${_spawn.ID}\ax)

  /if (${Stick.Active}) {
    /squelch /stick off
  }

  /if (AUTO && !${Stick.Active} && ${Me.TargetOfTarget.ID} != ${Me.ID}) {
    /if (${_spawn.Distance} < ${maHard.Find[stMinBowDist].Value} || !${_spawn.LineOfSight}) {
      /call moveto_range ${_debug} ${_spawn.ID} 35
    }
  }

  /invoke ${spawn_face[${_spawn.ID}]}

  /if (${_spawn.LineOfSight} && !${Me.AutoFire}) {
    /squelch /autofire
  }

/return TRUE



|***
 * note: sets melee combat
 * use: /call set_combat_melee DEBUG [spawnID]
 ***|
sub set_combat_melee(bool _debug, spawn _spawn)
  DEBUG \atset_combat_melee\ax(\a-w${_spawn.ID}\ax)

  /if (${Me.AutoFire}) {
    /squelch /autofire
  }

  /if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
    /call check_sash_proc ${_debug}
  }

  /if (MELEE) {
    /squelch /attack on
  }

  SPAWNDEAD
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT set_combat_melee _spawn|${_spawn.ID}
  
  /if (!${timer_Check_Behind}) {
    /call spawn_stick ${_debug} ${_spawn.ID}
  }

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
sub get_target_basepull(bool _debug, spawn _spawn)
  DEBUG \atget_target_basepull\ax(\a-w${_spawn.ID}\ax)

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
  /if (${_spawn.ID}) {
    /varset stAssTarID ${_spawn.ID}
  } else {
    /call pull_mob_find ${_debug}
  }

  DEBUG ${sep}target${sep}\a-wTARGET\ax

  | When you grow up, your heart dies.
  /if (!TARGET) {
    SETHUD
    /if (HOME && !${SpawnCount[npc radius 15 PS4 targetable]}) {
      /call Bind_control_home return NULL NULL ${maDebug.Find[home].Value}
    }
    /return FALSE
  }

  | Could you describe the ruckus, sir?
  /if (TARGET) {
    /if (${Target.ID} != TARGET) {
      /call target set TARGET
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

  /declare _waittime int local ${Math.Calc[(PULLENV + ${_spawn.Distance})*.08]}
  /call get_target_wait ${maDebug.Find[wait].Value} TARGET ${_waittime}s

  DEBUG ${sep}\arCLOSING\ax${sep}\atget_target_basepull\ax (\a-rTARGET\ax)
  SETHUD

/return



|***
 * note: wait for mobs to get to you from a pull
 * use: /call get_target_wait DEBUG [TARGETID] [WAITTIME]
 ***|
sub get_target_wait(bool _debug, spawn _spawn, string _waittime)
  DEBUG \atget_target_wait\ax(\a-w${_spawn.ID}, ${_waittime}\ax)

  | too close for comfort
  /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} && !${maPull.Find[stCountChainPull].Value}) {
    /return TRUE
  }

  | wait for mob to get to camp
  DEBUG ${sep}loop \a-wwait for mob\ax${sep}\a-w${_spawn.ID}\ax
  DEBUG ${sep}_wait${sep}\ay${_waittime}\ax
  /invoke ${set_timer[${_debug}, wait_for_mob, ${_waittime}]}

  /if (!${_spawn.Dead}) {
    /invoke ${out[9, "Waiting on${sep}${con_color[${_debug}, ${_spawn.ID}]}${sep}\a-w${_waittime}\ax"]}
  }

  /while (${wait_for_mob}) {

    /if (${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
      DEBUG ${break}/while \a-ghostile mob in safeenv raidus\ax
      /invoke ${set_timer[${_debug}, wait_for_mob, 0]}
      /invoke ${target[clear]}
      /break
    }
        
    DEBUG ${sep}\awTARGET\ax.distance${sep}\a-w${_spawn.Distance}\ax${sep}\a-w${wait_for_mob}\ax
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
    /if (!${_spawn.ID} || ${_spawn.Dead} || !${_spawn.ID}) {
      DEBUG ${sep}/while \a-gbreak_NO_target\ax
      /invoke ${target[clear]}
      /break
    }

    | mobs in our personal space. fuck those things. no respect
    /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} && ${NearestSpawn[1, npc radius SAFEENV].ID} != ${_spawn.ID}) {
    | /if (${SpawnCount[npc radius ENV PS4 targetable]}) {
      DEBUG ${break}mob in environment space
      /invoke ${out[9, 0, 26]}
      /invoke ${target[clear]}
      /break
    }

    | target LOS & within tag range
    /if (${_spawn.LineOfSight} && ${_spawn.Distance} < ENV) {
      DEBUG ${sep}/while \a-gbreak_tag\ax
      /break
    }

    | target within melee
    /if (${_spawn.Distance} <= ${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value} && MELEE) {
      DEBUG ${sep}/while \a-gbreak_MaxRangeTo\ax
      /break
    }

    | target in stupid range out of meele less then range.. hate this spot
    /if (${_spawn.Distance} > ${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value} && ${_spawn.Distance} < ${maHard.Find[stMinBowDist].Value}) {
      DEBUG ${sep}/while \a-gbreak_MaxRangeTo <> ${maHard.Find[stMinBowDist].Value}\ax
      /break
    }

    | LoS time reduction
    /if (!${_spawn.LineOfSight} && ${_spawn.Distance} < ENV) {
      /delay 1
    }

    | tank attraction skills
    /if (${maChr.Find[swAttraction].Value} && (${Me.TargetOfTarget.ID} != ${Me.ID}) && ${_spawn.LineOfSight}) {
      /call pull_mob_attraction ${_debug} ${_spawn.ID} FALSE
    }

    | pet loss?
    /if (${maPull.Find[stPullMode].Value.Equal[pet]} && !${Me.Pet.ID} && PET) {
      /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
    }

    | timer ran out
    /if (!${wait_for_mob} || !AUTO) {
      /invoke ${out[9, "Sick of waiting on${sep}${con_color[${_debug}, ${_spawn.ID}]}"]}
      /invoke ${target[clear]}
      /break
    }
  }

  /if (${maPull.Find[stCountChainPull].Value}) {
    DEBUG ${break}clearing target for chain puller
    /invoke ${target[clear]}
  }
  
/return TRUE



|***
 * note: various conditions for the target in question
 * use: /call pull_mob_prep DEBUG [TARGETID]
 ***|
sub pull_mob_prep(bool _debug, spawn _spawn)
  DEBUG \atpull_mob_prep\ax(\a-w${_spawn.ID}\ax)

  /declare _count int local 0
  /declare _los string local los

  | advanced pulling does not require LoS, clear the reqs for anything except base pulling
  /if (${maOver.Find[swOverLOS].Value} || ${Select[${maPull.Find[stPullMode].Value},int,pet,calm,multi]}) {
    /varset _los
  }

  | exclude list pulls. clear these targets.
  /if (${lsZoneExclude.Contains[${_spawn.DisplayName}]}) {
    DEBUG ${sep}Pull Excludelist${sep}\a-w${lsZoneExclude.Contains[${_spawn.DisplayName}]}\ax \arreset\ax
    /varset stAssTarID 0
    /varset targetData 0
    /varset _spawn 0
    /return FALSE
  }

  | get the mob to us for non pet classes..
  DEBUG ${sep}pull_mob_prep${sep}\awLoop.Start\ax${sep}${_spawn.ID}
  /if (!${target[isdead, ${_spawn.ID}]} && !${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) {

    /if (!AUTO) {
      /invoke ${target[clear]}
      /return FALSE
    }

    | keep tabs on targets range if they are the puller. advanced pullers fetching we do not care about yet
    /if (${_spawn.Distance} > PULLENV) {
      /invoke ${target[clear]}
      /return FALSE
    }

    | CHECKCURSOR TRUE
    GETINPUT pull_mob_prep _spawn|${_spawn.ID}
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

    | stand the fuck up. seriously...
    /if (${SpawnCount[npc ${_los} radius PULLENV targetable]} && ${Me.State.NotEqual[stand]}) {
      /stand
    }
    | ain't got no time for that
    /if (${Me.Stunned}) {
      /return FALSE
    }

    | keep us facing the target just enough...
    /if (${Target.ID} && (MELEE || RANGE) && ${_spawn.LineOfSight}) {
      /invoke ${spawn_face[${_spawn.ID}]}
    }

    SPAWNDEAD

    | get out if mob is even remotely close
    /if (${_spawn.Distance} <= ENV && ${_spawn.LineOfSight}) {
      DEBUG ${sep}prep${sep}in sight < ENV
      /return TRUE
    }
  }

  | pet auto send to kill
  /if (${_spawn.Distance} <= PULLENV && ${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) {
    DEBUG ${sep}Pet Range${sep}${_spawn.ID}
    /if (${Target.ID} != ${_spawn.ID}) {
      /call target set ${_spawn.ID}
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
    /invoke ${set_data_timer[${_debug}, Check_Pull_Hardstop_Delay, 6s]}
    /return FALSE
  }

  /declare _found bool local FALSE
  
  | check the puller
  /if (${Bool[${maPull.Find[stPctHardStop].Value}]}) {
    /if (${lsClassCast.Contains[${Me.Class.ShortName}]}) {
      /if (${Me.PctMana} < ${maPull.Find[stPctHardStop].Value}) {
        /varset _found TRUE
      }
    }
    /if (${lsClassStamina.Contains[${Me.Class.ShortName}]}) {
      /if (${Me.PctEndurance} < ${maPull.Find[stPctHardStop].Value}) {
        /varset _found TRUE
      }
    }
  }
    
  /if (${Bool[${maPull.Find[swHardStopRez].Value}]}) {
    /if (${Me.Buff[Revival Sickness].ID} || ${Me.Buff[Resurrection Sickness].ID}) {
      DEBUG ${sep}${sep}just rezed
      /varset _found TRUE
    }
  }

  /if (${_found} && !${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
    /invoke ${set_data_timer[FALSE, Check_Pull_Hardstop, ${maPull.Find[stHardStopDuration].Value}s]}
    /return TRUE
  }
  
  /if (${_found} && ${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
    /return FALSE
  }


  /declare _countToon int local 1
  /declare _toon string local FALSE

  | each toon
  /while (${_countToon} <= ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}) {
    
    /if (${SpawnCount[npc radius ENV PS4 targetable]}) {
      /return FALSE
    }

    | get the name to something usable
    /varset _toon ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}
    DEBUG ${sep}_toon${sep}\ay${_toon}\ax
    
    | set observer check
    /if (!${observer_validate[${_debug}, obs_chrwatch, ${_toon}]}) {
      /varcalc _countToon ${_countToon} + 1
      /continue
    }

    | skip this person?
    /if (!${Spawn[pc ${_toon}].ID} || ${DanNet[${_toon}].Observe["Me.Dead"]} || ${Spawn[pc ${_toon}].Distance} > ENV || ${_toon.Equal[${Me.DisplayName}]}) {
      DEBUG ${sep}skipping${sep}\a-w${_toon}\ax ${dot} me, dead or beyond ENV
      /varcalc _countToon ${_countToon} + 1
      /continue
    }

    | check mana and stamina
    /if (${Bool[${maPull.Find[stPctHardStop].Value}]}) {
  
      | mana
      /if (${lsClassCast.Contains[${DanNet[${_toon}].Observe["Me.Class.ShortName"]}]}) {
        /if (${DanNet[${_toon}].Observe["Me.PctMana"]} < ${maPull.Find[stPctHardStop].Value}) {
          /varset _found TRUE
          /break
        }
      }
      
      | stamina 
      /if (${lsClassStamina.Contains[${DanNet[${_toon}].Observe["Me.Class.ShortName"]}]}) {
        /if (${DanNet[${_toon}].Observe["Me.PctEndurance"]} < ${maPull.Find[stPctHardStop].Value}) {
          /varset _found TRUE
          /break
        }
      }
    }

    | rez efects
    /if (${Bool[${maPull.Find[swHardStopRez].Value}]}) {
      
      /if (${DanNet[${_toon}].Observe["Me.Buff[Revival Sickness].ID"]}) {
        DEBUG ${sep}${sep}Revival Sickness
        /varset _found TRUE
        /break
      }

      /if (${DanNet[${_toon}].Observe["Me.Buff[Resurrection Sickness].ID"]}) {
        DEBUG ${sep}${sep}Resurrection Sickness
        /varset _found TRUE
        /break
      }

    }

    /varcalc _countToon ${_countToon}+1
  }

  /if (${_found} && !${SpawnCount[npc radius ENV PS4 targetable]}) {
    /invoke ${set_data_timer[FALSE, Check_Pull_Hardstop, ${maPull.Find[stHardStopDuration].Value}s]}
    /return TRUE
  }

/return FALSE



|***
 * note: tea anyone?
 * use: /call pull_mob_tag DEBUG [spawn]
 ***|
sub pull_mob_tag(bool _debug, spawn _spawn)
  DEBUG \atpull_mob_tag\ax(\a-w${_spawn.ID}\ax)

  /declare _count int local 0

  /if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID} && ${maBuff.Find[swBuffAmmo].Value}) {
    /call check_buff_cycle_ammo ${_debug} TRUE
  }

  | loop cycle start
  /declare _loopbreak int local 1

  /if (${_spawn.LineOfSight} || !${maPull.Find[swPullNavLoS].Value}) {
    DEBUG ${sep}Target${sep}\agLoS\ax

    | get our target
    /if (${Target.ID} != ${_spawn.ID}) {
      /call target set ${_spawn.ID}
      /delay 5 ${Target.ID} == ${_spawn.ID}
    }


    | melee tag
    | we are using a % of the targets max range to melee just to make sure we get close enough.
    | if we are not in this range, we try and get closer.
    /if (${maPull.Find[stPullWith].Value.Equal[melee]} && ${_spawn.Distance} <= ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} || ${_spawn.Distance} <= ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      DEBUG ${sep}Tag${sep}Melee${sep}${tar}${_spawn.DisplayName}\ax
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
      /invoke ${spawn_face[${_spawn.ID}]}
      /varset _loopbreak 1

      DEBUG ${sep}Entering _loopbreak Melee
      /while (${_loopbreak} <= ${maHard.Find[stPullMobTagTry].Value}) {

        /delay 5
        GETINPUT
        /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

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
        /if (${Spawn[id ${_spawn.ID} playerstate 32].ID}) {
          DEBUG ${break}pull_mob_tag${sep}stunned
          /break
        }

        | break if mob is close enough to bash to a bloody pulp
        /if (${_spawn.Distance} <= ${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
          DEBUG ${break}pull_mob_tag${sep}target is in melee range
          /break
        }

        /if (${NearestSpawn[1, npc radius SAFEENV PS4].ID} != ${_spawn.ID}) {
          DEBUG ${break}/while \a-ghostile mob in safeenv rad, breaking (\a-rpull_mob_tag(${_spawn.ID})\ax) to alter target\ax
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
      DEBUG ${sep}Tag${sep}Other${sep}${tar}${_spawn.DisplayName}\ax

      DEBUG ${sep}Entering _loopbreak OTHER
      /while (${_loopbreak} <= ${maHard.Find[stPullMobTagTry].Value}) {

        | get its attention
        /call pull_get_other_type ${_debug} ${_spawn.ID}
        /delay 5
        GETINPUT

        | have the targets attention. can go back
        /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
          DEBUG ${break}pull_mob_tag${sep}tot\ag==\axme (\a-r${_loopbreak}\ax)
          /break
        }

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
    /if (${maPull.Find[stPullWith].Value.Equal[range]} && ${_spawn.Distance} > ${maHard.Find[stMinBowDist].Value} && ${_spawn.Distance} < ${maHard.Find[stMaxBowDist].Value}) {
      DEBUG ${sep}Tag${sep}Range${sep}${tar}${_spawn.DisplayName}\ax${sep}\a-w${_spawn.Distance}\ax
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

      | /while (${Target.AggroHolder.Name.NotEqual[${Me.DisplayName}]} || ${_count} < 4) {
      |   /if (${Target.AggroHolder.Name.Equal[${Me.DisplayName}]}) /break

      /while (${Me.TargetOfTarget.ID} != ${Me.ID} || ${_count} < 4) {
        /if (${Me.TargetOfTarget.ID} != ${Me.ID}) /break
        /if (${Target.ID} && ${_spawn.Distance} > ${maHard.Find[stMinBowDist].Value}) {
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
    /if (MELEE && ${_spawn.Distance} > ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} && ${_spawn.Distance} <= ENV) {
      DEBUG ${sep}Tag${sep}Melee${sep}${tar}${_spawn.DisplayName}\ax
      GETINPUT pull_mob_tag_both NA|NA
      /attack on

      DEBUG ${sep}${_spawn.Distance} > ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
      DEBUG ${sep}Tag${sep}Melee${sep}${tar}${_spawn.DisplayName}\ax \agNAVTO\ax
      /nav id ${_spawn.ID} |dist=${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} log=${maEntropy.Find[swNavLogSpew].Value}
      /delay 5 ${Me.Moving}
      /return TRUE
    }

  } else /if (!${_spawn.LineOfSight} && !${maOver.Find[swOverLOS].Value}) {
    DEBUG ${sep}Target${sep}\arLOS\ax\a-w${_spawn.ID}\ax
    /return tagfailure

  }

  /if (${_spawn.Distance} <= ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} && MELEE && !RANGE) {
    DEBUG ${sep}Tag${sep}Melee NO RANGE${sep}${tar}${_spawn.DisplayName}\ax
    GETINPUT pull_mob_tag_fail NA|NA
    /attack on
    /return TRUE

  }

/return FALSE



|***
 * note: Finds and sets stAssTarID / targetData
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
    /varset targetData 0
  }

  | we have an existing target and not using a mode
  /if (${targetData.ID} && !${_spCount} && !${swModeToken}) {
    /return ${targetData.ID}
  }

  /declare _spawn spawn local
  /declare _tmpAgroRange int local 0
  /declare _count int local 0
  /declare _mobHP int local 999
  /declare _tmpHPAdj int local 0
  /declare _los string local los
  /declare _closestNAV int local PULLENV
  /declare _closestID int local 0
  /declare _navRange int local 0
  /declare _mobcount int local 0


  | set the temp agro radius for steps
  /if (!${maPull.Find[stPullStep].Value}) {
    /varset _tmpAgroRange ${Int[${Math.Calc[PULLENV - 1]}]}
  } else {
    /varset _tmpAgroRange ${Int[${Math.Calc[PULLENV / ${maPull.Find[stPullStep].Value}]}]}
  }

  | adjust LOS?
  /if (${maOver.Find[swOverLOS].Value} || ${Select[${maPull.Find[stPullMode].Value},nav,int,pet,calm,multi]}) {
    DEBUG ${sep}LOS overridden
    /varset _los
  }

  | time to step the agro range out so we get mobs closer first
  DEBUG ${sep}rad${sep}\aw PULLENV\ax zradius${sep}\a-w${maPull.Find[stPullZRadius].Value}\ax

  /while (${_tmpAgroRange} <= PULLENV && !${targetData.ID}) {
    DEBUG start _tmpAgroRange${sep}${_tmpAgroRange} <= PULLENV
    /delay 1

    /if (${_tmpAgroRange} <= SAFEENV) {
      /varset _mobcount ${SpawnCount[npc ${_los} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
    } else {
      /varset _mobcount ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
    }

    | no mobs around us, no reason to be here
    /if (!${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}) {
      DEBUG ${break}NO SpawnCount to pull from${sep}\a-w${_tmpAgroRange}\ax
      /if (!${maPull.Find[swPullStep].Value}) {
        /break
      } else {
        /varcalc _tmpAgroRange ${_tmpAgroRange}+${Int[${Math.Calc[PULLENV / ${maPull.Find[stPullStep].Value}]}]}
      }
      /continue
    }

    /if (${maPull.Find[stCountChainPull].Value} && ${_spCount}) {
      /if (${_spCount} >= ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius PULLENV zradius ${maPull.Find[stPullZRadius].Value} targetable]}) {
        DEBUG ${sep}spawncount error: looking for more spawns then exist for chain pulling
        /invoke ${target[clear]}
        /return FALSE
      } 
    }
    

    | search local ENV range in steps for mob acquisition
    DEBUG ${sep}count:\aw${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}\ax radius${sep}\ay${_tmpAgroRange}\ax
    /for _count ${Math.Calc[${_spCount} +1]} to ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
      DEBUG ${sep}count${sep}${_count} of ${SpawnCount[${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      
      /if (${_count} > ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]}) {
        DEBUG ${break}spawncount increment invalid moving to next temp radius
        /continue
      }
          
      /vardata _spawn NearestSpawn[${_count}, npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maPull.Find[stPullZRadius].Value} targetable]

      /if (!${_spawn.ID}) {
        DEBUG ${break}something is seriously wrong with the spawn${sep}\ar${_spawn.ID}\ax
        /return FALSE
      }
      
      | if we dont specify PCs to ignore, we ignore all of them...
      /if (${_spawn.Type.Equal[pc]} && ${lsNoTargetPC.Contains[EMPTY]} && ${lsNoTargetPC.Count} == 1) {
        DEBUG ${break}use a diferent macro if oyu want to attack players${sep}\ar${_spawn.Type}\ax
        /continue
      }

      | skip pets, we deal with them last
      /if (${_spawn.Type.Equal[npcpet]}) {
        DEBUG ${break}do not engage pets in the main cycle{sep}\ar${_spawn.Type}\ax
        /continue
      }
  
      | excluded mobs
      /if (${lsZoneExclude.Contains[${_spawn.DisplayName}]}) {
        DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wlsZoneExclude\ax (\a-r${_spawn.DisplayName}\ax)
        /continue
      }

      | animation checks
      /if (${lsAnimationNoTarget.Contains[${_spawn.Animation}]}) {
        DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wlsAnimationNoTarget\ax (\a-r${_spawn.Animation}\ax)
        /continue
      }

      | fish are friends, not food
      /if (${lsNoAttackBodyType.Contains[${_spawn.Body}]}) {
        DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wlsNoAttackBodyType\ax (\a-r${_spawn.Body}\ax)
        /continue
      }

      | is this a priority mob?
      /if ((${lsZoneNoMez.Contains[${_spawn.DisplayName}]} || ${lsZoneNamed.Contains[${_spawn.DisplayName}]}) && ${_spawn.Distance} <= ENV) {
        DEBUG ${sep}Close Priority Mob${sep}\ag${_spawn.DisplayName}\ax
        /invoke ${maCC.Add[swSetCombat,TRUE]}
        /vardata targetData _spawn.ID
        /varset stAssTarID ${_spawn.ID}
        /return ${_spawn.ID}
      }

      | body types to consider.. is this fat shaming?

        | can i come into the out now?
        /if (${_spawn.Type.Equal[NPC]}) {
          /if (${lsNoTargetNPC.Contains[${_spawn.DisplayName}]}) {
            DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetNPC\ax (\a-r${_spawn.DisplayName}\ax)
            /continue
          }

        | PCs to not consider
        } else /if (${_spawn.Type.Equal[PC]}) {
          /if (${lsNoTargetPC.Contains[${_spawn.DisplayName}]}) {
            DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetPC\ax (\a-r${_spawn.DisplayName}\ax)
            /continue
          }

        | magic creatures to not consider
        } else /if (${_spawn.Type.Equal[Magic]}) {
          /if (${lsNoTargetMagic.Contains[${_spawn.DisplayName}]}) {
            DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetMagic\ax (\a-r${_spawn.DisplayName}\ax)
            /continue
          }
        }

      /if (${_spawn.ID} == ${Me.Pet.ID}) {
        DEBUG ${break}spawn is my pet
        /continue
      }

      | CYA for shit MQ will pick up in the environment that does not have a name
      /if (${_spawn.DisplayName.Length} < 1) {
        DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wDisplayName.Length < 1\ax

        /if (!${maPull.Find[swPullStep].Value}) {
          /varset _tmpAgroRange ${Int[${Math.Calc[PULLENV - 1]}]}
        } else {
          /varcalc _tmpAgroRange ${_tmpAgroRange}+${Int[${Math.Calc[PULLENV / ${maPull.Find[stPullStep].Value}]}]}
        }

        /continue
      }

      | bishes suffering from tunnel vision
      /if (${maPull.Find[stPullArcWidth].Value}) {
        |**
        /if (!${Defined[_tmpDegree]}) {
          /declare _tmpDegree float local 0
        }

        /varset _tmpDegree ${_spawn.HeadingTo[${maData.Find[stCampY].Value},${maData.Find[stCampX].Value}].Degrees}

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

      | skip this if we are the group main assist
      /if (${Group.MainAssist.Name.NotEqual[${Me.DisplayName}]}) {
        /if (${maPull.Find[stPullMode].Value.Equal[pet]} && ${lsAnimationMezed.Contains[${_spawn.Animation}]}) {
          /continue
        }
      }

      DEBUG ${sep}_spawn${sep}\a-w${_spawn.ID}\ax navlength${sep}\a-w${Navigation.PathLength[id ${_spawn.ID}]}\ax

      | check navigation path validity for all but base pulling
      /if (!${Select[${maPull.Find[stPullMode].Value},base]}) {
        /if (!${Navigation.PathExists[id ${_spawn.ID}]} && !${maOver.Find[swOverNavCheck].Value}) {
          DEBUG ${break}#:${_count}${dot}${_spawn.ID}${sep}\arPASS\ax on${sep}\a-wNavigation.PathExists[\a-y${_spawn.ID}\ax]\ax (\a-r${_spawn.DisplayName}\ax)
          /continue
        }
      }

      | check nav path length
      /if ((${maPull.Find[swNavPathLogic].Value} || ${Select[${maPull.Find[stPullMode].Value},nav]}) && !${maMode.Find[petfarm].Value}) {
        
        | break on nav if there is an error in length
        /if (${Navigation.PathLength[id ${_spawn.ID}]} < 0) {
          DEBUG ${break}Navigation.PathLength[id ${_spawn.ID}] < 0
          /continue
        }
        
        | nav pulling ignore radius limitation, focus on nav length only and the varriance
        /if (${Select[${maPull.Find[stPullMode].Value},nav]}) {
          /if (${Navigation.PathLength[id ${_spawn.ID}]} > ${Math.Calc[PULLENV + (PULLENV*.${maPull.Find[stPullNavVariance].Value})]}) {
            DEBUG ${break}Navigation.PathLength(nav)[${_spawn.ID}] > ${Math.Calc[PULLENV + (PULLENV*.${maPull.Find[stPullNavVariance].Value})]}
            /continue
          }

        | we DO NOT go out side of the max pull radius.. no matter what!
        } else {
          /if (${Navigation.PathLength[id ${_spawn.ID}]} > PULLENV && ${_spawn.Distance} > PULLENV) {
            DEBUG ${break}Navigation.PathLength[${_spawn.ID}] > PULLENV
            /continue
          }
        }
        
        /varset _navRange ${Navigation.PathLength[id ${_spawn.ID}]}

        | sort by closest nav path length
        /if (${_navRange} <= ${_closestNAV}) {
          /varset _closestNAV ${_navRange}
        } else /if (${_navRange} > ${_closestNAV}) {
          DEBUG ${break}nav check${sep}\a-w${_navRange} > ${_closestNAV}\ax
          /continue
        }
      }

      | added because MQ sucks ass for getting accurate HP values returned over a mouse dick in length.
      /if (${_spawn.PctHPs} > 100 && ${_tmpAgroRange} > 200) {
        /varset _tmpHPAdj 100
      } else {
        /varset _tmpHPAdj ${_spawn.PctHPs}
      }

      | get the lowest HP mob if we can
      /if (${_tmpHPAdj} <= ${_mobHP}) {
        /varset _mobHP ${_spawn.PctHPs}
        /varset stAssTarID ${_spawn.ID}
        /vardata targetData _spawn.ID
        /invoke ${maCC.Add[swSetCombat,TRUE]}
        DEBUG ${sep}targetData${sep}\aw${_spawn.DisplayName}\aw${sep}\ay${_spawn.ID}\ay ${sep} ${If[${_spawn.LineOfSight},\ag${_spawn.Distance}\ax,\ar${_spawn.Distance}\ax]}
        /break
      }

    /next _count



    /if (!AUTO && !${Select[${_tag},petfarm]}) {
      /return 0
    }
    
    | check for pets once everything else is dead    
    /if (!${_spawn.ID} && ${maPull.Find[stPullPetWatch].Value}) {
      /if (${SpawnCount[npcpet radius ${maPull.Find[stPullPetWatch].Value} targetable]}) {
        DEBUG ${sep}killing closest NPC pet
        /vardata _spawn NearestSpawn[npcpet radius ${maPull.Find[stPullPetWatch].Value} targetable]
        /invoke ${maCC.Add[swSetCombat,TRUE]}
        /varset stAssTarID ${_spawn.ID}
        /vardata targetData _spawn.ID
      }

    }

    | we have our target. back to whatever brought us here
    /if (${targetData.ID}) {
      /return ${targetData.ID}
    }  
    

    | no target, try next step
    /varset targetData 0
    /varset stAssTarID 0
    /if (!${maPull.Find[swPullStep].Value}) {
      /break
    } else {
      DEBUG ${sep}_tmpAgroRange ${_tmpAgroRange}+${Int[${Math.Calc[PULLENV / ${maPull.Find[stPullStep].Value}]}]}
      /varcalc _tmpAgroRange ${_tmpAgroRange}+${Int[${Math.Calc[PULLENV / ${maPull.Find[stPullStep].Value}]}]}
    }

  }

  /if (!${targetData.ID}) {
    /delay 2
  }

/return 0



|***
 * note: memorize a spell/song to a specific gem if not memed
 * use: /call set_spell_to DEBUG [echo memming T/F] ["spell name"] [gem to mem it in] [wait for recycle to cast T/F]
 ***|
sub set_spell_to(bool _debug, bool _echo, string _spellname, string _spellgem, bool _wait)
  DEBUG \atset_spell_to\ax(\a-w${_echo}, ${_spellname}, ${_spellgem}, ${_wait}\ax)

  /if (${Me.Gem[${_spellname}]}) {
    /return TRUE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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

  /declare _count int local 0
  /while (!${_ready}) {
    /memspell ${_gem} "${_spellname}"
    /delay 5
    /delay 15s ${Me.Gem[${_gem}].Name.Equal[${_spellname}]}

    /if (${Me.Gem[${_gem}].Name.Equal[${_spellname}]}) {
      /varset _ready TRUE
    }
    /varcalc _count ${_count} + 1
    /if (${_count} >= 4) {
      /return FALSE
    }
  }

  | wait for spell to be ready
  /while (${_wait}) {
    GETOVERHERE
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
  /if (CHECKINVIS && !${Select[${Me.Class.ShortName},ROG]}) /return TRUE

  | movement
  /if (${Me.Moving} && !BARD) /return TRUE

  | charm
  /if (${Me.SPA[22]}) /return TRUE

  | inhibit: 96=spell 191=skill
  /if (${Me.SPA[96]}) /return TRUE
  /if (${Me.SPA[191]} && ${Select[${Me.Class.ShortName},BER,ROG,MNK,RNG,WAR,BRD]}) /return TRUE
  
  
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
 * use: /call check_engaged DEBUG || ${check_engaged[DEBUG]}
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
  /if (${Me.Moving} && !BARD) {
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
    | /if (${SpawnCount[npc OVERLOS radius 15*2 PS4]}) {
    /if (${SpawnCount[npc OVERLOS radius ${maRest.Find[stRestRadius].value} PS4]}) {
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

  | no rest in manual
  /if (!AUTO) {
    DEBUG ${break}no rest in manual
    /return FALSE
  }
  
  | initial spawncount check
  /if (${SpawnCount[${maRest.Find[stRestSpawnSearch].Value}]}) {
    DEBUG ${break}mobs close
    /return FALSE
  }
  
  | rest in raids?
  /if (!${maRest.Find[swRestInRaid].Value} && ${Raid.Members}) {
    DEBUG ${break}no rest in raidsS
    /return FALSE
  }

  | the dead dont need to rest
  /if (${swIsMeDead}) {
    | DEBUG ${break}swIsMeDead${sep}\a-w${swIsMeDead}\ax
    | /return FALSE
  }

  | we DO NOT enter rest while in active combat
  /if (!${maRest.Find[swRestCombat].Value} && ${Me.CombatState.Equal[combat]}) {
    DEBUG ${break}active combat
    /return FALSE
  }

  | dont rest while in tie
  /if (TIE) {
    DEBUG ${break}tie active
    /return FALSE
  }

  | clerics and pallys wait 'till yaulp gone
  /if (${Me.Buff[Yaulp].ID}) {
    DEBUG ${break}buff${sep}\a-wyalup\ax
    /return FALSE
  }
  
  GETINPUT do_rest NA|NA

  /declare _needrest bool local FALSE
  /declare _resting bool local FALSE

  :loopneedmorerest
  DEBUG ${sep}loop${sep}\a-wloopneedmorerest\ax start
  /varset _needrest FALSE
  CHECKREZ
  
  /if ((!${maRest.Find[swRestCombat].Value} && ${Me.CombatState.Equal[combat]}) ||!AUTO || ${SpawnCount[${maRest.Find[stRestSpawnSearch].Value}]}) {
    /varset _needrest FALSE
    /if (${Me.State.NotEqual[STAND]}) /stand
    /if (BARD && ${Me.BardSongPlaying}) {
      /call set_stop ${maDebug.Find[stop].Value} SILENT
    } else /if (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
      /stopcast
    }
    DEBUG ${sep}dont need rest
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKCURSOR
  CHECKTIE
  GETINPUT
  | check self click buffs
  /if (!${Me.Invis} && !${Select[${Me.Class.ShortName},ROG]} && ${maRest.Find[swRestBuff].Value}) {
    /if (!${timer_Check_Buff_Cycle}) {
      /call check_buff_cycle ${maDebug.Find[buff].Value}
    }
  }

  /if (${Select[${maEnv.Find[stLootMode].Value},advanced]} && !${timer_Check_Loot} && AUTO) {
    /if (${Group.Members} && ${Group.MasterLooter.ID} == ${Me.ID} && ${Me.UseAdvancedLooting} && !${AdvLoot.LootInProgress} && !${Stick.Active}) {
      /call do_loot ${maDebug.Find[loot].Value}
    } else /if (!${Raid.Members} && !${Group.Members}) {
      /call do_loot ${maDebug.Find[loot].Value}      
    }
  }

  | bard rest song
  /if (BARD && !${Me.Invis}) {
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

  /if (PCTREST && !${Me.Moving} && ((${maRest.Find[swRestCombat].Value} && ${Me.CombatState.NotEqual[combat]}) || !${SpawnCount[${maRest.Find[stRestSpawnSearch].Value}]})) {

    | Hybrid: RNG,SHD,BST,PAL,BRD
    /if ((${lsClassMelee.CountOf[${Me.Class.ShortName}]} && ${lsClassCast.CountOf[${Me.Class.ShortName}]}) || BARD) {

      | rest to full requirement
      /if (${maRest.Find[swRestFull].Value} && ${_resting}) {
        /if (${Me.PctEndurance} < 99*.PCTHO) /varset _needrest TRUE
        /if (${Me.PctMana} < 99*.PCTHO) /varset _needrest TRUE
        /if (${maRest.Find[swRestCheckHealth].Value}) {
          /if (${Me.PctHPs} < 99*.PCTHO) /varset _needrest TRUE
        }
      } else {
        /if (${Me.PctEndurance} < PCTREST*.PCTHO) /varset _needrest TRUE
        /if (${Me.PctMana} < PCTREST*.PCTHO) /varset _needrest TRUE
        /if (${maRest.Find[swRestCheckHealth].Value}) {
          /if (${Me.PctHPs} < PCTREST*.PCTHO) /varset _needrest TRUE
        }
      }

      DEBUG ${sep}swRestFull${sep}\a-w${_needrest}\ax

      | bards suck...
      /if (BARD) {
        /call cast_AA_rallying FALSE
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
        /if (${Me.PctEndurance} < PCTREST*.PCTHO) /varset _needrest TRUE
        /if (${Me.PctMana} < PCTREST*.PCTHO) /varset _needrest TRUE
        /if (${maChr.Find[swSongRest].Value}) /varset _needrest TRUE
        /if (${maRest.Find[swRestCheckHealth].Value}) {
          /if (${Me.PctHPs} < PCTREST*.PCTHO) /varset _needrest TRUE
        }


      } else {
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
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
        /if (PET && !${Pet.Sitting} && ${maEntropy.Find[swTLP].Value}) /pet sit
      }

      DEBUG ${sep}hybrid _needrest${sep}\a-w${_needrest}\ax${sep}

    | Melee: WAR,MNK,ROG,BER
    } else /if (${lsClassMelee.CountOf[${Me.Class.ShortName}]} && !${lsClassCast.CountOf[${Me.Class.ShortName}]}) {
      /if (${Me.PctEndurance} < PCTREST*.PCTHO) /varset _needrest TRUE
      /if (${maRest.Find[swRestCheckHealth].Value}) {
        /if (${Me.PctHPs} < PCTREST*.PCTHO) /varset _needrest TRUE
      }

      | rest to full requirement
      /if (${maRest.Find[swRestFull].Value}) {
        /if (${Me.PctEndurance} < 99*.PCTHO) /varset _needrest TRUE
        /if (${maRest.Find[swRestCheckHealth].Value}) {
          /if (${Me.PctHPs} < 99*.PCTHO) /varset _needrest TRUE
        }
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
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
      }
      /if (${maRest.Find[swRestCheckHealth].Value}) {
        /if (${Me.PctHPs} < PCTREST*.PCTHO) /varset _needrest TRUE
      }

      | rest to full requirement
      /if (${maRest.Find[swRestFull].Value}) {
        /if (${Me.PctMana} < 99*.PCTHO) /varset _needrest TRUE
        /if (${maRest.Find[swRestCheckHealth].Value}) {
          /if (${Me.PctHPs} < 99*.PCTHO) /varset _needrest TRUE
        }
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
      }

      /if (${Select[${Me.Class.ShortName},CLR]}) {
        /if (${maCC.Find[swCombatHeal].Value}) {
          /if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
            /call check_heal FALSE Group
          }
          /if (${maHeal.Find[swHealXTarget].Value}) /call check_heal FALSE XTarget
        }
      } else /if (${Select[${Me.Class.ShortName},DRU]}) {
        /if (${maCC.Find[swCombatHeal].Value}) {
          /if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
            /call check_heal FALSE Group
          }
          /if (${maHeal.Find[swHealXTarget].Value}) /call check_heal FALSE XTarget
        }

      } else /if (${Select[${Me.Class.ShortName},SHM]}) {
        /call check_cani ${maDebug.Find[canni].Value}
        /if (${maCC.Find[swCombatHeal].Value}) {
          /if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
            /call check_heal FALSE Group
          }
          /if (${maHeal.Find[swHealXTarget].Value}) /call check_heal FALSE XTarget
        }
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
        /if (PET && !${Pet.Sitting} && ${maEntropy.Find[swTLP].Value}) /pet sit
      } else /if (${Select[${Me.Class.ShortName},ENC]}) {
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
        /if (PET && !${Pet.Sitting} && ${maEntropy.Find[swTLP].Value}) /pet sit
      } else /if (${Select[${Me.Class.ShortName},WIZ]}) {
      } else /if (${Select[${Me.Class.ShortName},NEC]}) {
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
        /if (PET && !${Pet.Sitting} && ${maEntropy.Find[swTLP].Value}) /pet sit
      } else /if (${Select[${Me.Class.ShortName},MAG]}) {
        /if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
        /if (${maChr.Find[stPctGather].Value} && !${timer_Check_Gather}) /call check_gather ${maDebug.Find[gather].Value}
        /if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
        /if (PET && !${Pet.Sitting} && ${maEntropy.Find[swTLP].Value}) /pet sit
      }

      DEBUG ${sep}caster _needrest${sep}\a-w${_needrest}\ax

    }
  }

  /if (${_needrest}) {
    SETHUD "rest"
    /if (${Me.State.NotEqual[sit]} && !${Me.Mount.ID}) {
      /sit
    }
    /varset _resting TRUE
    /delay 2
    CHECKTIE
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETINPUT do_rest_loop NA|NA
    /goto :loopneedmorerest
  } else /if (!${_needrest}) {
    /if (${Me.State.Equal[sit]} && !${Me.Mount.ID}) {
      | delay standing to prevent the client lag up and down for a full tick of mana
      /docommand /timed 120 /if (${Me.State.Equal[sit]}) /stand
    }
  }    

  SETHUD

/return TRUE



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
      /sqlite query "${DBzone}" QZone INSERT OR IGNORE INTO zone_control VALUES ('${Zone.ShortName}', 'swZoneSafe', 'FALSE');
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
    /sqlite query "${DBzone}" QZone INSERT OR IGNORE INTO zone_control VALUES ('${Zone.ShortName}', 'swZoneSafe', 'FALSE');
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
 * use: /call do_raw_edit DEBUG [silent T/F] [variable to change] [value to change it too|gem#|cursor] [add|delete|clear (for lists)|overwrite] [T/F noecho]
 ***|
sub do_raw_edit(bool _debug, string _silent, string _variable, string _value, string _add_delete, bool _noecho)
| /varset _debug TRUE

  /if (${maDebug.Find[edit].Value}) {
    DEBUG \atdo_raw_edit\ax
    DEBUG ${sep}_silent${sep}\a-w${_silent}\ax
    DEBUG ${sep}_variable${sep}\a-w${_variable}\ax
    DEBUG ${sep}_value${sep}\a-w${Parse[1,"${_value}"]}\ax
    DEBUG ${sep}_add_delete${sep}\a-w${_add_delete}\ax
    DEBUG ${sep}_noecho${sep}\a-w${_noecho}\ax
  }

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
    /if (${_variable.Left[2].Equal[ls]} && (!${Select[${_add_delete},add,delete,overwrite]} && ${_value.NotEqual[clear]})) {
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


  /invoke ${maData.Add[outVar,${_variable}]}
  /invoke ${maData.Add[outVal,${_value}]}

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
    /sqlite query "${DBstatic}" dummy SELECT * FROM env_${Me.Class.ShortName.Lower} WHERE env_var='${_variable}';
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
  } else /if (${_add_delete.Equal[overwrite]}) {
    /varset _value ${_value}
    /if (${_variable.Left[2].Equal[ls]}) {
      /varset _list TRUE
    } else /if (${_variable.Left[2].Equal[ma]}) {
      /varset _map TRUE
    } else {
      /varset _found TRUE
    }

  | add to an existig or default value
  } else /if (${Select[${_add_delete},add,overwrite]}) {
    DEBUG ${sep}\ayadd\ax
    
    | FORCE A CLEAR BEFORE AN overwrite 
    /if (${Select[${_add_delete},overwrite]}) {
      /declare _tmpLst list local
      /invoke ${_tmpLst.Delimiter["|"]}
      /sqlite query "${DBcharacter}" dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_variable}';
      /if (!${sql_check[${_debug}, dummy, do_raw_edit, SELECT]}) /endmacro
      /invoke ${_tmpLst.Append[${sqlite.Result[dummy 1 value]}]}
      DEBUG ${sep}DB current count${sep}\aw${_tmpLst.Count}\ax
      /varset _list TRUE
    }
    
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
  | /if (${sqlite.Status[dummy].Equal[Success]} && ${_silent.NotEqual[SILENT]} && !${_noecho}) {
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

  } else /if (${sqlite.Status[dummy].NotEqual[Success]} && !${_noecho}) {
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
    /invoke ${maData.Add[swGetOverHere,FALSE]}

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

  /declare _spawn spawn local
  /vardata _spawn Spawn[pc ${maTie.Find[stTieToon].Value}]
  
  /if (!${_spawn.ID}) {
    /return FALSE
  }

  /if (HOME) {
    /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT set_tie _spawn|${_spawn.ID}

  | reasons to not tie to a toon
  /if (${maTie.Find[swSetTie].Value} && ${_spawn.ID} && ${_spawn.Dead}) {
    /return
  }

  /if (${Me.Combat}) {
    /return
  }

  /if (${_spawn.Distance} <= ${maTie.Find[stTieVariance].Value}) {
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
      /if (!${Navigation.Active} && ${_spawn.Distance} > ${maTie.Find[stTieVariance].Value}) {
        DEBUG get closer
        /if (${Target.ID}) {
          /squelch /target clear
        }
        /invoke ${spawn_face[${_spawn.ID}]}
        /nav id ${_spawn.ID} |dist=${maTie.Find[stTieVariance].Value} log=${maEntropy.Find[swNavLogSpew].Value}

      } else /if (${_spawn.Distance} <= ${maTie.Find[stTieVariance].Value}) {
        /if (${Navigation.Active}) {
          /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
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
    /if (${_spawn.Distance} > ${maTie.Find[stTieVariance].Value}) {
      /if (${Target.ID}) {
        /squelch /target clear
      }

      /invoke ${spawn_face[${_spawn.ID}]}
      /squelch /stick id ${_spawn.ID} ${maTie.Find[stTieVariance].Value} UW
    }

    :okweresticking
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETINPUT set_tie_stick NA|NA

    /if (${_spawn.Distance} <= ${maTie.Find[stTieVariance].Value}) {
      /squelch /stick off
      /return
    }

    /if (!${Stick.Stopped} && ${_spawn.ID}) {
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
  DEBUG \atcast_AA_fade\as()

  /if (!${Bool[${maChr.Find[stPctFade].Value}]}) {
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT
  
  /if (${Me.PctHPs} > ${maChr.Find[stPctFade].Value}) {
    /return FALSE
  }
  /if (!${SpawnCount[npc radius ENV PS4]}) {
    /return FALSE
  }

  | made it this far. may as well see if the shit works
  /if (${cast_data[${_debug}, "${maData.Find[stFade].Value}"]}) {
    /call cast ${_debug} 0 FALSE
  }

/return TRUE



|***
 * note: checks and casts whats int lsBuffNow
 * use: /call check_buff_cycle_now DEBUG
 ***|
sub check_buff_cycle_now(bool _debug)
  DEBUG \atcheck_buff_cycle_now\ax()


  /invoke ${set_data_timer[${_debug}, Check_Buff_Now, RESTART]}

  /if (AUTO && !${maBuff.Find[swBuffMaster].Value}) {
    /return FALSE
  }

  SETHUD "buff now"
  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_buff_cycle_now
  GETINPUT check_buff_cycle_now NA|NA

  /declare _licountself listiterator local
  /vardata _licountself lsCount50.First.Clone
  /while (!${_licountself.IsEnd}) {
    
    GETOVERHERE

    | buff on/off switch
    /if (!${maBuff.Find[swBuff${_licountself.Value}].Value}) {
      /invoke ${_licountself.Advance}
      /continue
    }

    | now tag needed
    /if (!${lsBuff${_licountself.Value}Tag.Contains[now]}) {
      DEBUG ${break}${_licountself.Value}${sep}\a-wno now tag\ax
      /invoke ${_licountself.Advance}
      /continue
    }

    | no name set?
    /if (!${Bool[${maBuff.Find[stBuff${_licountself.Value}].Value}]}) {
      DEBUG ${break}no name set:${_licountself.Value}
      /invoke ${_licountself.Advance}
      /continue
    }

    /if (!${cast_data[${_debug}, "${maBuff.Find[stBuff${_licountself.Value}].Value}"]}) {
      DEBUG ${break}${_licountself.Value}${sep}\a-wnot ready\ax (\a-r${maBuff.Find[stBuff${_licountself.Value}].Value}\ax)
      /invoke ${_licountself.Advance}
      /continue
    }

    | do we have it, now that we know what it is?
    /if (${have_buff[${_debug}]}) {
      DEBUG ${break}have_buff/song (\a-r${spellData.Name}\ax)
      /invoke ${_licountself.Advance}
      /continue    
    }
    
    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_licountself.Value} ${Me.DisplayName} "${maBuff.Find[stBuff${_licountself.Value}].Value}" check_buff_cycle_now
    /if (!${Macro.Return}) {
      DEBUG ${break}${_licountself.Value}${sep}\a-wcondition fail\ax
      /invoke ${_licountself.Advance}
      /continue
    }

    /call cast ${_debug} 0 FALSE

    /if (${lsBuff${_licountself.Value}Tag.Contains[autoinventory]}) {
      /delay 2s ${Cursor.ID}
      /call check_cursor ${_debug} clear
    }

    DEBUG ${sep}next iteration
    /invoke ${_licountself.Advance}
  }
  
  SETHUD

/return TRUE



|***
 * note: casts an in order value from a list at passed target ID
 * note: this should not be used anywhere....
 * use: /call cast_list DEBUG [lsNAME] [TARGETID]
 ***|
sub cast_list(bool _debug, string _lsList, spawn _spawn)
  DEBUG \atcast_list\ax(\a-w${_lsList}, ${_spawn.ID}\ax)

  /declare _liCL listiterator local
  /vardata _liCL ${_lsList}.First.Clone
  /while (!${_liCL.IsEnd}) {
    /if (!${maCC.Find[swSetCombat].Value}) {
      /return FALSE
    }

    DEBUG ${sep}_liCL.Value${sep}\a-w${_liCL.Value}\ax
    /if (${cast_data[${_debug}, "${_liCL.Value}"]}) {
      /call cast ${_debug} ${_spawn.ID} FALSE
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
    /invoke ${set_data_timer[${_debug}, Check_Buff_Unity, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_buff_cycle_unity
  GETINPUT check_buff_cycle_unity NA|NA
  
  /if (!${cast_data[${_debug}, "${maChr.Find[stAAUnity].Value}"]}) {
    /return FALSE
  }
  
  /declare _count int local 0
  /declare _found bool local TRUE
  DEBUG ${sep}spellData${sep}\a-w${spellData}\ax
  DEBUG ${sep}effects${sep}\a-w${spellData.NumEffects}\ax

  /for _count 1 to ${spellData.NumEffects}
    DEBUG ${sep}_count${sep}\aw${_count}\ax${sep}\a-w${spellData.Trigger[${_count}]}\ax

    | trigger
    /if (${Me.Buff[${spellData.Trigger[${_count}].BaseName}].ID}) /continue

    | stacks
    /if (!${spellData.Trigger[${_count}].WillLand}) /continue

    | do not have
    /if (!${Me.Buff[${spellData.Trigger[${_count}]}].ID}) /varset _found FALSE
    DEBUG ${sep}_found${sep}\a-w${_found}\ax

    /if (!${_found}) /break
  /next _count

  /if (!${_found}) {
    /call cast ${_debug} 0 FALSE
  }

  SETHUD

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
  /if ((${maBuff.Find[swBuffUnity].Value} && AUTO && !${timer_Check_Buff_Unity}) || (${Select[${_verbage},u,xx]} && ${_force})) {
    /call check_buff_cycle_unity ${If[${_force},${_debug},${maDebug.Find[unity].Value}]}
  }

  | aura
  /if ((${maBuff.Find[swBuffAura].Value} && AUTO && !${timer_Check_Buff_Aura}) || (${Select[${_verbage},a,xx]} && ${_force})) {
    /call check_buff_cycle_aura ${If[${_force},${maDebug.Find[aura].Value},${maDebug.Find[aura].Value}]}
  }

  | shrink
  /if ((${maBuff.Find[swBuffShrink].Value} && AUTO && !${timer_Check_Buff_Shrink}) || (${Select[${_verbage},s,xx]} && ${_force})) {
    /call check_buff_cycle_shrink ${If[${_force},${_debug},${maDebug.Find[shrink].Value}]}
  }

  | self
  /if ((${maBuff.Find[swBuffSelf].Value} && AUTO && !${timer_Check_Buff_Self}) || (${Select[${_verbage},s,xx]} && ${_force})) {
    /call check_buff_cycle_self ${If[${_force},${_debug},${maDebug.Find[buffself].Value}]}
  }

  | crew
  /if ((${maBuff.Find[swBuffCrew].Value} && AUTO && !${timer_Check_Buff_Crew}) || (${Select[${_verbage},c]} && ${_force})) {
    /call check_buff_cycle_crew ${If[${_force},${_debug},${maDebug.Find[crew].Value}]}
  }

  | group
  /if ((${maBuff.Find[swBuffGroup].Value} && AUTO && !${timer_Check_Buff_Group}) || (${Select[${_verbage},g]} && ${_force})) {
    /call check_buff_cycle_others ${If[${_force},${_debug},${maDebug.Find[buffgroup].Value}]} Group
  }

  | raid
  /if ((${maBuff.Find[swBuffRaid].Value} && AUTO && !${timer_Check_Buff_Raid}) || (${Select[${_verbage},r]} && ${_force})) {
    /call check_buff_cycle_others ${If[${_force},${_debug},${maDebug.Find[buffraid].Value}]} Raid
  }

  | beg
  /if ((${maBuff.Find[swBuffBeg].Value} && AUTO && !${timer_Check_Buff_Beg}) || (${Select[${_verbage},b]} && ${_force})) {
    /call check_buff_cycle_beg ${If[${_force},${_debug},${maDebug.Find[beg].Value}]}
  }

  | powersource
  /if (${maBuff.Find[swBuffPowersource].Value} && ((AUTO && !${timer_Check_Buff_Powersource}) || ${_force})) {
    | /call check_buff_cycle_powersource ${If[${_force},${_debug},${maDebug.Find[power].Value}]}
  }

  | ranged ammo
  /if ((${maBuff.Find[swBuffAmmo].Value} && AUTO && !${timer_Check_Ammo}) || (${Select[${_verbage},s]} && ${_force})) {
    /call check_buff_cycle_ammo ${If[${_force},${_debug},${maDebug.Find[ammo].Value}]}
  }

  | plate robe ornament swap bug
  /if (${Select[${_verbage},robe]}) {
    /call check_ornament_robe
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
    DEBUG ${break}safezone
    /return FALSE
  }

  /if (!${lsClassCast.Contains[${Me.Class.ShortName}]}) {
    DEBUG ${break}not a caster
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT
  GETINPUT do_mana_recovery NA|NA

  | get mana recovery items
  /if (!${timer_Check_Inventory_ManaClick}) {
    /call scan_inventory_for ${maDebug.Find[inventoryscan].Value} ManaClick 2 15
    /invoke ${set_data_timer[${_debug}, Check_Inventory_ManaClick, RESTART]}
  }

  /declare _lidmc listiterator local
  /vardata _lidmc lsInventoryManaClick.First.Clone
  /while (!${_lidmc.IsEnd}) {

    DEBUG ${sep}\a-w${_lidmc.Value}\ax
    GETOVERHERE

    /if (!${cast_data[${_debug}, "${_lidmc.Value}"]}) {
      /invoke ${_lidmc.Advance}
      /continue
    }

    | no potions or scrolls (spells in inventory)
    /if (${Select[${FindItem[=${_lidmc.Value}].Type},Potion,Scroll]}) {
      DEBUG ${sep}${FindItem[=${_lidmc.Value}]} Is a ${FindItem[=${_lidmc.Value}].Type} so ignoring it!
      /invoke ${_lidmc.Advance}
      /continue 

    | worth the mana or gonna kill us?
    } else /if (${Me.CurrentHPs} < ${spellData.Base[1]}) {
      DEBUG ${sep}Come back when you have more HP slacker
      /invoke ${_lidmc.Advance}
      /continue

    | can we use it?
    } else /if (!${FindItem[=${_lidmc.Value}].CanUse}) {
      DEBUG ${sep}${FindItem[=${_lidmc.Value}]} Can't be used by you!
      /invoke ${_lidmc.Advance}
      /continue
      
    | only use instant duration things
    } else /if (${spellData.Duration} > 0) {
      DEBUG ${sep}${FindItem[=${_lidmc.Value}]} has a castable buff so not touching it....
      /invoke ${_lidmc.Advance}
      /continue
    } 

    | cast something already
    /call cast ${_debug} ${Me.ID} FALSE
    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
      /return TRUE
    }

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

  /if (!${cast_data[${_debug}, "${maChr.Find[stYaulp].Value}"]}) {
    /return FALSE
  }

  /if (!${Select[${Me.CombatState},combat]}) {
    DEBUG ${break}not in combat
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT cast_yaulp
  /call cast ${_debug} 0 FALSE

/return TRUE



|***
 * note: used to refresh ammo for ranged attacks
 * use: /call check_buff_cycle_ammo DEBUG
 ***|
sub check_buff_cycle_ammo(bool _debug)
  DEBUG \atcheck_buff_cycle_ammo\ax()

  SETHUD "buff ammo"

  /if (AUTO || MODE) {
    /invoke ${set_data_timer[${_debug}, Check_Ammo, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
        /if (${cast_data[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
          /call cast ${_debug} 0 FALSE
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
        /if (${cast_data[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
          /call cast ${_debug} 0 FALSE
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

  /if (AUTO || MODE) {
    /invoke ${set_data_timer[${_debug}, Check_Poison, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
      /call Bind_control_nomore invis ${_debug}
    }

    /if (${cast_data[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
      /call cast ${_debug} 0 FALSE
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

  SETHUD

/return TRUE



|***
 * note: watches buff beg map for aliases it can cast
 * use: /call check_buff_cycle_beg DEBUG
 ***|
sub check_buff_cycle_beg(bool _debug)
  DEBUG \atcheck_buff_cycle_beg\ax()

  SETHUD "buff beg"

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

  /declare _remove bool local FALSE

  /declare _mibb mapiterator local
  /vardata _mibb maBuffRequest.First.Clone
  /declare _libb listiterator local
  /while (!${_mibb.IsEnd}) {

    CHECKTIE
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    CHECKEXIT check_buff_cycle_beg
    GETINPUT check_buff_cycle_beg NA|NA

    /if (!${is_lsSafeNames[${maDebug.Find[safe].Value}, ${_mibb.Key}]}) {
      DEBUG${break}not in safe list
      /invoke ${maBuffRequest.Remove[${_mibb.Key}]}
      /return FALSE
    }

    /vardata _libb lsCount50.First.Clone
    /while (!${_libb.IsEnd}) {

      /if (!${maBuff.Find[swBuff${_libb.Value}].Value}) {
        DEBUG ${break}switch off${sep}swBuff${_libb.Value}
        /invoke ${_libb.Advance}
        /continue
      }
      /if (!${Bool[${maBuff.Find[stBuff${_libb.Value}].Value}]}) {
        DEBUG ${break}no name${sep}stBuff${_libb.Value}}
        /invoke ${_libb.Advance}
        /continue
      }
      /if (!${Bool[${maBuff.Find[stBuff${_libb.Value}Alias].Value}]}) {
        DEBUG ${break}no alias${sep}stBuff${_libb.Value}Alias
        /invoke ${_libb.Advance}
        /continue
      }
      
      | buffs with SDPA parsing to be done
      /if (${maBuff.Find[stBuff${_libb.Value}].Value.Count[|]} == 3) {
        /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_libb.Value}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_libb.Value}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_libb.Value}].Value.Arg[3,|]}"
        /invoke ${maData.Add[tmpName,${Macro.Return}]}

      | get item clickie spells
      } else /if (${FindItem[=${maBuff.Find[stBuff${_libb.Value}].Value}].ID}) {
        DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_libb.Value}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_libb.Value}].Value}].Clicky.SpellID}].Name}\ax
        /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_libb.Value}].Value}].Clicky.SpellID}].Name}]}
  
      | just the spell/aa/whatever
      } else {
        DEBUG ${sep}spell${sep}${maBuff.Find[stBuff${_libb.Value}].Value}
        /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_libb.Value}].Value}]}
      }

      /if (${maBuff.Find[stBuff${_libb.Value}Alias].Value.Find[${_mibb.Value}]}) {
        DEBUG ${sep}BEG \ap${_mibb.Value}\ax found${sep}\aw${_libb.Value}\ax${sep}\a-w${maBuff.Find[stBuff${_libb.Value}].Value}\ax for \a-y${_mibb.Key}\ax
        /call cast_data ${_debug} "${maData.Find[tmpName].Value}"
        /call cast ${_debug} ${Spawn[pc ${_mibb.Key}].ID} FALSE
        /delay 1s ${Me.Casting.ID}
        /delay 5s !${Me.Casting.ID}
        /varset _remove TRUE
        /break
      }

      /invoke ${_libb.Advance}
    }

    /if (${_remove}) {
      /invoke ${maBuffRequest.Remove[${_mibb.Key}]}
      /break
    }

    /invoke ${_mibb.Advance}
  }

  SETHUD

/return



|***
 * note: buffs all your toons
 * use: part of standard buff cycle if enabled
 ***|
sub check_buff_cycle_crew(bool _debug)
  DEBUG \atcheck_buff_cycle_crew\ax()

  SETHUD "buff crew"

  /if (AUTO) {
    /invoke ${set_data_timer[FALSE, Check_Buff_Crew, RESTART]}
    /if (!${maBuff.Find[swBuffMaster].Value}) {
      /return FALSE
    }
  }

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_buff_cycle_crew
  GETINPUT check_buff_cycle_crew NA|NA

  /declare _count int local 0
  /declare _countToon int local 1
  /declare _target bool local FALSE
  /declare _found bool TRUE
  /declare _i int local 0
  /declare _base2 string local
  /declare _tmpBuff string local FALSE
  /declare _spawn spawn local

  DEBUG ${sep}Buff${sep}\a-wCrew\ax

  | each toon
  /while (${_countToon} <= ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}) {

    /if (!${Spawn[pc ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}].ID}) {
      /varcalc _countToon ${_countToon}+1
      /continue
    }

    | get the name to something usable
    /vardata _spawn Spawn[pc ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}]
    
    DEBUG ${sep}_spawn${sep}\ay${_spawn.DisplayName}\ax

    | skip this person?
    /if (${_spawn.DisplayName.Equal[${Me.DisplayName}]} || !${_spawn.ID} || ${_spawn.Dead}) {
      DEBUG ${sep}skipping${sep}\a-w${_spawn.DisplayName}\ax
      /varcalc _countToon ${_countToon}+1
      /continue
    }

   | any buff slots available?
    /dquery ${_spawn.DisplayName} -q "Me.FreeBuffSlots" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    /if (${DNQReturn.Equal[0]}) {
      /varcalc _countToon ${_countToon}+1
      /continue
    }

    | each buff
    /for _count 1 to 50
      
      GETOVERHERE
      
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
        /invoke ${maCD.Add[tmpName,${Macro.Return}]}
        /invoke ${maCD.Add[tmpCastName,${Macro.Return}]}

      | get item clickie spells
      } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
        DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
        /invoke ${maCD.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
        /invoke ${maCD.Add[tmpCastName,${maBuff.Find[stBuff${_count}].Value}]}
  
      | just the spell/aa/whatever
      } else {
        DEBUG ${sep}spell${sep}${maBuff.Find[stBuff${_count}].Value}
        /invoke ${maCD.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
        /invoke ${maCD.Add[tmpCastName,${maBuff.Find[stBuff${_count}].Value}]}
      }
      
      | skip self only
      /if (${Select[${Spell[${maCD.Find[tmpName].Value}].RankName.TargetType},self]}) {
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
      /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_spawn.DisplayName} "${maCD.Find[tmpName].Value}" check_buff_cycle_crew
      /if (!${Macro.Return}) {
        DEBUG ${sep}skip${sep}failed tag cycle check
        /continue
      }

      DEBUG ${sep}\agBUFF\ax#${sep}\a-w${_count}\ax \a-p---------------------\ax

      | one of your crew, pretend they are not a target until they qualify
      /varset _target FALSE

      | is this an acceptable target for casting
      /if (${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Find[${_spawn.DisplayName.Lower}]}) {
        DEBUG ${sep}crew member${sep}\a-w${_spawn.DisplayName}\ax (\a-rvalid target\ax)
        /varset _target TRUE
      }
      
      /call cast_data ${_debug} "${maCD.Find[tmpCastName].Value}"

      | distance check
      /if (${Select["${spellData.TargetType}",Group v2]}) {
        DEBUG ${sep}distance check${sep}\a-wGroup v2\ax
        /if (${_spawn.Distance} > ${spellData.AERange}) /continue
      } else {
        DEBUG ${sep}distance check${sep}\a-wtargeted ?? ${spellData.MyRange}\ax
        /if (${spellData.MyRange}) {
          /if (${_spawn.Distance} > ${spellData.MyRange} && !${Select[${spellData.TargetType},self]}) /continue
        }
      }

     /if (${maCD.Find[hasspa].Value}) {
        DEBUG ${sep}multiple landing spell

        /for _i 1 to ${spellData.NumEffects}
          /varset _found TRUE

          | skip any secondary procs with no duration
          /if (!${Bool[${Spell[${Spell[${maCD.Find[tmpName].Value}].Base2[${_i}]}].Duration}]}) /continue

          | /varset spellDataTrigger ${Spell[${maCD.Find[tmpName].Value}].Trigger[${_i}]}
          | DEBUG ${sep}${sep}\awTrigger\ax${sep}\a-w${spellData.Trigger}\ax ID:\aw${Spell[${spellData.RankName}].ID}\ax

          | is it buff blocked
          /dquery ${_spawn.DisplayName} -q "lsBlockedBuffsMe.Find[${spellData.Trigger[${_i}].ID}]" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
          DEBUG ${sep}${sep}BLOCKED .Trigger.Blocked Buff \at${Bool[${DNQReturn}]}\ax ${dot} ${spellData.RankName}
          /if (${Bool[${DNQReturn}]}) {
            /continue
          }

          | does it stack
          /dquery ${_spawn.DisplayName} -q "Spell[${spellData.RankName}].Trigger[${_i}].WillLand" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
          DEBUG ${sep}${sep}WillLand: .Trigger.WillLand \at${DNQReturn}\ax ${dot} ${spellData.RankName}
          /if (!${DNQReturn}) {
            /continue
          }

          | do we have the triggered buff?
          /dquery ${_spawn.DisplayName} -q "Me.Buff[${spellData.Trigger[${_i}]}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
          DEBUG ${sep}trigger${sep}\ay${spellData.Trigger[${_i}]}\ax
          DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${spellData.Trigger[${_i}]}
          /if (!${Bool[${DNQReturn}]}) {
            /varset _found FALSE
            /break
          }

        /next _i

        DEBUG ${sep}${sep}\ay****\ax Found:${_found} .. Target:${_target}
        /if (!${_found} && ${_target}) {
          /call cast ${_debug} ${_spawn.ID} FALSE
          /call cast_cooldown_wait ${_debug}
          /continue
        }

      } else {
        DEBUG ${sep}single landing spell${sep}\a-w{maCD.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}\ax

        DEBUG ${sep}\awTrigger\ax${sep}\a-w${spellData.RankName}\ax ID:${spellData.ID}

        | does it stack
        /dquery ${_spawn.DisplayName} -q "Spell[${spellData.RankName}].WillLand" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}WillLand: .Buff.WillLand \at${DNQReturn}\ax ${dot} ${spellData.RankName}
  
        /if (!${DNQReturn}) {
          /continue
        }

        | is it buff blocked
        /dquery ${_spawn.DisplayName} -q "lsBlockedBuffsMe.Find[${spellData.ID}]" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}BLOCKED .Buff.Blocked \at${DNQReturn}\ax ${dot} ${spellData.ID}
        /if (${Bool[${DNQReturn}]}) /continue

        | do we have the triggered buff?
        /dquery ${_spawn.DisplayName} -q "Me.Buff[${spellData.RankName}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
        DEBUG ${sep}${sep}HAVE: .Buff.ID \atslot ${DNQReturn}\ax ${dot} ${spellData.ID}
        /if (${Bool[${DNQReturn}]}) {
          /continue
        }

        DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${spellData.ID} .. ${maCD.Find[tmpName].Value}

        /if (${_target}) {
          /call cast ${_debug} ${_spawn.ID} FALSE
          /call cast_cooldown_wait ${_debug}
          /continue
        }

      }

    /next _count
    /varcalc _countToon ${_countToon}+1

  }

  SETHUD

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

  SETHUD "buff ${_type.Lower}"

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

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_buff_cycle_others
  GETINPUT check_buff_cycle_others NA|NA

  | i'd like to go on the record and let everyone know i fucking hate mercs. more then bards
  | they are a bitch to code around and they kill the group content of this game
  | ok, i feel better now. moving on..

  DEBUG ${sep}Buff${sep}\a-w${_type}\ax

  /declare _countToon int 1
  /declare _count int 1
  /declare _toon string local FALSE
  /declare _spawn spawn local
  /declare _found bool local TRUE
  /declare _i int local 1

  | non boxed people/mercs (fuck your pets)
  /while (${_countToon} <= ${${_type}.Members}) {
    DEBUG ${sep}_countToon${sep}\a-w${_countToon}\ax / ${${_type}.Members}

    | "A girl has no name."
    /vardata _spawn Spawn[${${_type}.Member[${_countToon}].DisplayName}]
    DEBUG ${sep}_spawn${sep}\ay${_spawn.ID}\ax

    | one of your crew
    /if (${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Find[${_spawn.DisplayName}]}) {
      DEBUG ${skip}.Me${sep}\a-w${_spawn}\ax
      /varcalc _countToon ${_countToon} + 1
      /continue
    }
    
    | no pets
    /if (${_spawn.Type.Equal[pet]}) {
      DEBUG ${break}no pet for you
      /varcalc _countToon ${_countToon} + 1
      /continue
    }

    | skip this person?
    /if (${_spawn.Dead}) {
      DEBUG ${skip}.Dead${sep}\a-w${_spawn}\ax
      /varcalc _countToon ${_countToon} + 1
      /continue
    }

    | maybe 50 buffs was a bit much.
    /for _count 1 to 50
      | DEBUG ${sep}_count${sep}\a-w${_count}\ax

      /if (!${maBuff.Find[swBuff${_count}].Value}) {
        /continue
      }
      /if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
        /continue
      }

      | skip self only
      /if (${lsBuff${_count}Tag.Count} == 1 && (${lsBuff${_count}Tag.Contains[self]} || ${lsBuff${_count}Tag.Contains[minion]})) {
        DEBUG ${skip}self/minion buffs
        /continue
      }

      | make sure base tags are there?
      /if (${_type.Equal[group]}) {
        /if (!${lsBuff${_count}Tag.Contains[group]}) /continue
      } else /if (${_type.Equal[raid]}) {
        /if (!${lsBuff${_count}Tag.Contains[raid]}) /continue
      }

      DEBUG ${sep}_count${sep}\a-w${_count}\ax

      | cycle the tags
      /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_spawn.DisplayName} "${maBuff.Find[stBuff${_count}].Value}" check_buff_cycle_others
      /if (!${Macro.Return}) {
        /continue
      }
      
      | buffs with SDPA parsing to be done
      /if (${maBuff.Find[stBuff${_count}].Value.Count[|]} == 3) {
        /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_count}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[3,|]}"
        /invoke ${maCD.Add[tmpName,${Macro.Return}]}
        /invoke ${maCD.Add[tmpCastName,${Macro.Return}]}

      | get item clickie spells
      } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
        DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
        /invoke ${maCD.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
        /invoke ${maCD.Add[tmpCastName,${maBuff.Find[stBuff${_count}].Value}]}
  
      | just the spell/aa/whatever
      } else {
        DEBUG ${sep}spell${sep}${maBuff.Find[stBuff${_count}].Value}
        /invoke ${maCD.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
        /invoke ${maCD.Add[tmpCastName,${maBuff.Find[stBuff${_count}].Value}]}
      }
      
      /call cast_data ${_debug} "${maCD.Find[tmpName].Value}"
      
      | distance check
      /if (${Select["${spellData.TargetType}",Group v2]}) {
        /if (${_spawn.Distance} > ${spellData.AERange}) {
          DEBUG ${break}distance check${sep}\a-wGroup v2\ax
          /continue
        }
      } else {
        /if (${_spawn.Distance} > ${spellData.MyRange} && !${Select[${spellData.TargetType},self]}) {
          DEBUG ${break}distance check${sep}\a-wtargeted ?? ${spellData.MyRange}\ax
          /continue
        }
      }

      DEBUG ${sep}\awstBuff${_count}\ax${sep}\a-w${spellData.RankName}\ax (\a-r${spellData.NumEffects} triggers\ax)

      | target the toon
      /call target set ${_spawn.ID}

      | give the game a second to populate the targets buffs
      /delay 2s ${Target.BuffsPopulated}

      /if (${maCD.Find[hasspa].Value}) {
        DEBUG ${sep}multiple landing spell

        | start sorting the buff triggers
        /if (${spellData.NumEffects}) {
          /varset _found TRUE
          
          
          /for _i 1 to ${spellData.NumEffects}
            DEBUG ${sep}spellData${sep}\a-w${spellData.Trigger[${_i}]}\ax

            | Stacks/WilLand
            /if (!${spellData.Trigger[${_i}].StacksTarget}) {
              DEBUG ${break}.StacksTarget
              /continue
            }

            | target has the buff?
            /if (!${Target.Buff[${spellData.Trigger[${_i}]}].ID}) {
              DEBUG ${sep}does not have buff
              /varset _found FALSE
            }

            | cycle the tags
            /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_spawn.DisplayName} "${spellData.Trigger[${_i}]}" check_buff_cycle_others
            /if (!${Macro.Return}) {
              /continue
            }

            /if (!${_found}) /break

          /next _i

          /if (!${_found} && ${Target.ID} == ${_spawn.ID}) {
            /call cast ${_debug} ${_spawn.ID} FALSE
            /continue
          }
        }

      | buffs with only 1 trigger
      } else {
        DEBUG ${sep}single landing spell

        /varset _found TRUE

        | StacksTarget
        /if (!${spellData.StacksTarget}) {
          DEBUG ${break}.StacksTarget
          /continue          
        }

        | cycle the tags
        /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_spawn.DisplayName} "${spellData.RankName}" check_buff_cycle_others
        /if (!${Macro.Return}) {
          /continue
        }

        | target has the buff?
        /if (!${Target.Buff[${spellData.RankName}].ID}) {
          DEBUG ${sep}does not have buff
          /varset _found FALSE
        }

        /if (!${_found} && ${Target.ID} == ${_spawn.ID}) {
          /call cast ${_debug} ${_spawn.ID} FALSE
          /continue
        }
      }

    /next _count

    /varcalc _countToon ${_countToon} + 1
  }

  SETHUD

/return TRUE



|***
 * note: we dont trust anyone
 * use: ${is_lsSafeNames[DEBUG, spawn]}
 ***|
sub is_lsSafeNames(bool _debug, string _name)
  DEBUG \atis_lsSafeNames\ax(\a-w${_name}\ax)

  | override and accept everyone
  /if (${maOver.Find[swOverSafeNames].Value}) {
    DEBUG ${sep}safe${sep}\a-wswOverSafeNames\ax
    /return TRUE
  }

  | auto accept ${DanNetMacroChannel} channel toons
  /if (${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Find[${_name}]}) {
    DEBUG ${sep}safe.dannet${sep}\a-wstEntropyGroup_all\ax
    /return TRUE
  }

  | basic safe list check
  /if (${lsSafeNames.Contains[${_name}]}) {
    DEBUG ${sep}safe.safelist${sep}\a-wlsSafeNames\ax
    /return TRUE
  }

  | guild safe list check
  /if (${maEnv.Find[swSafelistGuild].Value} && ${Bool[${Me.Guild}]}) {
    /if (${_spawn.Guild.Equal[${Me.Guild}]}) {
      DEBUG ${sep}safe${sep}\a-wswSafelistGuild\ax
      /return TRUE
    }
  }

/return FALSE



|***
 * note: get an item from a spell
 * use:  /call cast_item_summon DEBUG [ITEMVARIABLE]
 ***|
sub cast_item_summon(bool _debug, string _var)
  DEBUG \atcast_item_summon\ax()

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
    /call cast_data ${_debug} "${_spellc}"
    /call cast ${_debug} ${Me.ID} FALSE
    /delay 5
    /delay 15s !${Me.Casting.ID}

    | need autoinventory stuff here!!

  }

/return TRUE



|***
 * note: challenge line for knights
 * use: /call cast_challenge DEBUG [TARGETID]
 ***|
sub cast_challenge(bool _debug, spawn _spawn)
  DEBUG \atcast_challenge\ax(\a-w${_spawn.ID}\ax)
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

  /if (${Target.BuffDuration[${maChr.Find[stChallenge].Value}].TotalSeconds}) {
    DEBUG ${sep}Target.BuffDuration${sep}\a-w${Target.BuffDuration[${maChr.Find[stChallenge].Value}].TotalSeconds}\ax
    /return TRUE
  }

  CHECKEXIT cast_challenge
  GETINPUT cast_challenge NA|NA

  /if (!${cast_data[${_debug}, "${maChr.Find[stChallenge].Value}"]}) {
    /return FALSE
  }

  /call cast ${_debug} ${_spawn.ID} FALSE

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
  
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT cast_stamina_recovery NA|NA

  /declare _endurancePoint int local 0

  | haitus switch
  /if (${maChr.Find[swHiatus].Value} && ${Me.CombatAbilityReady[${Spell[${maChr.Find[stHiatus].Value}].RankName}]} && ${Me.PctEndurance} < 20) {
    DEBUG rest${sep}\a-whiatus\ax
    /if (${Me.CombatAbility[${maChr.Find[stHiatus].Value} Rk. III]} && (${Me.CurrentEndurance} < 102500 && ${Me.PctEndurance} < 34)) /varset _endurancePoint 34
    /if (${Me.CombatAbility[${maChr.Find[stHiatus].Value} Rk. II]} && (${Me.CurrentEndurance} < 97500 && ${Me.PctEndurance} < 32)) /varset _endurancePoint 32
    /if (${Me.CombatAbility[${maChr.Find[stHiatus].Value}]} && (${Me.CurrentEndurance} < 92500 && ${Me.PctEndurance} < 29)) /varset _endurancePoint 29

    /if (${Me.PctEndurance} < ${_endurancePoint}) {
      /if (${cast_data[${_debug}, "${maChr.Find[stHiatus].Value}"]}) {
        /call cast ${_debug} 0 FALSE
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
      /if (${cast_data[${_debug}, "${maChr.Find[stRest].Value}"]}) {
        /call cast ${_debug} 0 FALSE
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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_harmonious
  GETINPUT check_harmonious NA|NA

  /invoke ${set_data_timer[${_debug}, Check_Harmonious, RESTART]}

  /if (!${Me.Buff[${maChr.Find[stHarmonious].Value}].ID} && ${Me.PctMana} > 2) {
    /if (${cast_data[${_debug}, "${maChr.Find[stHarmonious].Value}"]}) {
      /call cast ${_debug} 0 FALSE
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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT do_loot NA|NA

  /invoke ${set_data_timer[${_debug}, Check_Loot, RESTART]}

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
        GETOVERHERE
        /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

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
        GETOVERHERE
        /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

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
                /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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

  SETHUD

/return TRUE



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
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      GETOVERHERE
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
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

  | please bow your heads..  lsAutoInventory has entered the room
  /if (${lsAutoInventory.Contains[${Cursor.Name}]}) {
    DEBUG ${sep}lsAutoInventory${sep}\a-w${Cursor.Name}\ax
    /autoinventory
    /return TRUE
  }

  /invoke ${set_data_timer[${_debug}, Check_Cursor, 5s]}

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

  SETHUD

/return TRUE



|***
 * note: will cast ePen pet against all raid leader instructions. because my DPS parse matters, not yours!
 * use: /call cast_temp_pet DEBUG [spawn] [Temp Spell Name]
 ***|
sub cast_temp_pet(bool _debug, spawn _spawn, string _tmpSpell)
  DEBUG \atcast_temp_pet\ax(\a-w${_spawn.ID}, ${_tmpSpell}\ax)

  | these classes can only have one pet at a time (not counting  AA and spell combo. but i'n not gonna sort that shit)
  /if (${Select[${Me.Class.ShortName},CLR,WIZ]}) {
    /if (${Me.Pet.ID}) /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT cast_temp_pet
  GETINPUT cast_temp_pet NA|NA
  SPAWNDEAD

  /if (${Target.PctHPs} > ${maMinion.Find[stPctSwarmEngage].Value}) {
    /return FALSE
  }

  /if (${cast_data[${_debug}, "${_tmpSpell}"]}) {
    /call cast ${_debug} ${_spawn.ID} FALSE
  }

/return TRUE



|***
 * note: chopper.. sic balls!
 * use: /call send_pet ${maDebug.Find[sic].Value} [spawnID]
 ***|
sub send_pet(bool _debug, spawn _spawn)
  DEBUG \atsend_pet\ax(\a-w${_spawn.ID}\ax)

  | no target == bail
  /if (!${_spawn.ID}) {
    DEBUG ${break}._spawn
    /return FALSE
  }

  | out of env range
  /if (${_spawn.Distance} > ENV) {
    DEBUG ${break}>env
    /return FALSE
  }

  | pet has the passed target
  /if (${Me.Pet.Target.ID} == ${_spawn.ID}) {
    DEBUG ${break}ID${sep}\a-w${Me.Pet.Target.ID} == ${_spawn.ID}\ax
    /return TRUE
  }

  | pets target does not match the passed id
  /if (${Me.Pet.Target.ID} != ${_spawn.ID}) {
    DEBUG ${break}ID${sep}\a-w${Me.Pet.Target.ID} != ${_spawn.ID}\ax
    /pet back off
  }

  | manual peoples...
  /if (!AUTO && !${maMinion.Find[swonForce].Value}) {
    DEBUG ${break}manual NO swonForce
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT send_pet
  GETINPUT send_pet NA|NA
  
  /declare _high int local 0
  /declare _low int local 0


  /if (PET) {
    /if (${maEntropy.Find[stRNDEngagePet].Value} && !${maData.Find[stRNDEngageSetPet].Value} && !${Raid.Members}) {
      /varcalc _high ${maMinion.Find[stPctPetEngage].Value} + ${maEntropy.Find[stRNDEngagePet].Value}
      /varcalc _low ${maMinion.Find[stPctPetEngage].Value} - ${maEntropy.Find[stRNDEngagePet].Value}

      /if (${_high} > 99) {
        /varset _high 99
      }
      /invoke ${maData.Add[stPctEngageSetPet,${Math.Rand[${_low},${_high}]}]}

    } else /if (${maEntropy.Find[stRNDEngagePet].Value} && ${maData.Find[stRNDEngageSetPet].Value} && !${Raid.Members}) {

    | default to base engage %
    } else {
      /invoke ${maData.Add[stPctEngageSetPet,${maMinion.Find[stPctPetEngage].Value}]} 
    }
    
    /if (${_spawn.PctHPs} < ${maData.Find[stPctEngageSetPet].Value}) {
      /call target set ${_spawn.ID}
      /pet attack
    }
  }

  | send in the swarm
  /if (SWARM) {
    /if (${maEntropy.Find[stRNDEngageSwarm].Value} && !${maData.Find[stRNDEngageSetSwarm].Value} && !${Raid.Members}) {
      /varcalc _high ${maMinion.Find[stPctSwarmEngage].Value} + ${maEntropy.Find[stRNDEngageSwarm].Value}
      /varcalc _low ${maMinion.Find[stPctSwarmEngage].Value} - ${maEntropy.Find[stRNDEngageSwarm].Value}

      /if (${_high} > 99) {
        /varset _high 99
      }
      /invoke ${maData.Add[stPctEngageSetSwarm,${Math.Rand[${_low},${_high}]}]}

    | default to base engage %
    } else {
      /invoke ${maData.Add[stPctEngageSetPet,${maMinion.Find[stPctSwarmEngage].Value}]} 
    }    

    /if (${_spawn.PctHPs} < ${maData.Find[stPctEngageSetPet].Value}) {
      /call target set ${_spawn.ID}
      /pet swarm
    }
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
    | /invoke ${out[0, 0, 42]}
  }

  | summon minion
  DEBUG ${sep}minion cycle${sep}\a-wsummon minion\ax (\a-r\ax)
  /if (!${Me.Pet.ID}) {
    /call check_minion_cycle_summon ${_debug}
  }

  | buffs
  DEBUG ${sep}minion cycle${sep}\a-wbuffs\ax (\a-r${timer_Check_Minion_Buff}\ax)
  /if (${maMinion.Find[swPetBuff].Value} && (!${timer_Check_Minion_Buff} || ${_force})) {
    /call check_minion_cycle_buff ${_debug}
  }

  | illusions
  DEBUG ${sep}minion cycle${sep}\a-willusions\ax
  /if (${Me.Pet.ID} && ${Bool[${maMinion.Find[stPetIllusion].Value}]}) {
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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_minion_cycle_shrink
  GETINPUT check_minion_cycle_shrink NA|NA

  /invoke ${set_data_timer[FALSE, Check_Minion_Shrink, RESTART]}

  /if (${Me.Pet.Height} < 2) /return TRUE

  /if (!${cast_data[${_debug}, "${maMinion.Find[stPetShrink].Value}"]}) {
    /return FALSE
  }

  /declare _count int local 0

  | me shrinks
  /while (${Me.Pet.Height} > 2 && ${_count} < 2) {
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    /call cast ${_debug} ${Me.Pet.ID} FALSE
    /varcalc _count ${_count}+1
  }

/return TRUE



|***
 * note: buffs your pet
 * use: part of standard minion cycle if enabled
 ***|
sub check_minion_cycle_buff(bool _debug)
  DEBUG \atcheck_minion_cycle_buff\ax()

  /if (AUTO || MODE) {
    /invoke ${set_data_timer[${_debug}, Check_Minion_Buff, RESTART]}
  }

  /if (!${Me.Pet.ID}) {
    /return FALSE
  }

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
    /if (${Pet.Distance} > ${Spell[${maData.Find[tmpName].Value}].MyRange}) {
      /continue
    }

    | have the buff?
    /if (${Me.Pet.Buff[${maData.Find[tmpName].Value}]}) {
      /continue
    }
    
    /call cast_data ${_debug} "${maData.Find[tmpName].Value}"

    /call cast ${_debug} ${Me.Pet.ID} FALSE
    /call cast_cooldown_wait ${_debug}

  /next _count

/return TRUE



|***
 * note: summons your girl friend..
 * use: /call check_minion_cycle_summon DEBUG
 ***|
sub check_minion_cycle_summon(bool _debug)
  DEBUG \atcheck_minion_cycle_summon\ax()

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_minion_cycle_summon
  GETINPUT check_minion_cycle_summon NA|NA

  | component check. cause `yo ho needs an allowance
  /if (PET && !${Me.Pet.ID} && !${Select[${Me.Class.ShortName},BST]}) {
    DEBUG ${sep}checking pet components
    /if (${Select[${Me.Class.ShortName},ENC]} && !${FindItemCount[=Tiny Dagger]}) {
      /invoke ${maData.Add[peterror,TRUE]}
    } else /if (${Select[${Me.Class.ShortName},MAG]} && !${FindItemCount[=Malachite]}) {
      /invoke ${maData.Add[peterror,TRUE]}
    } else /if (${Select[${Me.Class.ShortName},NEC,SHD]} && !${FindItemCount[=Bone Chips]}) {
      /invoke ${maData.Add[peterror,TRUE]}
    }

    /if (${maData.Find[peterror].Value}) {
      DEBUG ${sep}error summon comonent not found${sep}
      /invoke ${out[0, 0, 43]}
      /invoke ${out[0, 0, 44]}
      /invoke ${out[0, 0, 45]}
      /invoke ${set_switch_env[${_debug}, swPet, TRUE, FALSE]}
      /return FALSE
    }
  }

  /declare _minion string local ${get_best_in_pet[${_debug}]}
  /call cast_data ${_debug} "${_minion}"
  
  /call set_spell_to ${_debug} TRUE "${spellData.RankName}" ${maEnv.Find[stBuffGem].Value} TRUE
  /call cast ${_debug} 0 FALSE
  /delay 15s !${Me.Casting.ID}
  /pet hold on
  /pet ghold on

  /if (AUTO || MODE) {
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
  /if (!PET || !${Me.Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) {
    DEBUG ${break}weapons are off
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_minion_cycle_weapons
  GETINPUT check_minion_cycle_weapons NA|NA

  | pet has weapons
  /if ((${Me.Pet.Equipment[primary]} && ${Me.Pet.Equipment[primary]} != 10855) && ${Me.Pet.Equipment[offhand]}) {
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

        /call cast_cooldown_wait ${_debug}
        
        /call cast_data ${_debug} "${maBuff.Find[stBuff${_count}].Value}"
        /call cast ${_debug} ${Me.ID} FALSE

        /delay 10s !${Me.Casting.ID}

        /if (${Cursor.Name.Equal[${FindItem[${Spell[${maBuff.Find[stBuff${_count}].Value}].Base[1]}].Name}]}) {
          /autoinventory
          /break
        }

      /next _count
    }

  } else /if (!${Select[${Me.Class.ShortName},MAG]} && ${SpawnCount[mag]} && ${DanNet.PeerCount[mag]}) {
    /if (!${FindItemCount[=${get_MAG_folded_pack[${_debug}]}]} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {
      /call Bind_command_beg ${maMinion.Find[stBegAlias].Value}
      /return TRUE
    }
  }

  /if (${Me.Pet.ID}) {
    /call get_minion_summon_weapons ${_debug}
  }

/return TRUE



|***
 * note: lipstick on a pig is still a fucking pig
 * use: /call check_minion_cycle_Illusion DEBUG
 ***|
sub check_minion_cycle_Illusion(bool _debug)
  DEBUG \atcheck_minion_cycle_Illusion\ax()

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_minion_cycle_Illusion
  GETINPUT check_minion_cycle_Illusion NA|NA

  | pet has illusion
  /if (${Me.Pet.Buff[${Spell[${FindItem[${maMinion.Find[stPetIllusion].Value}].Clicky.SpellID}].Name}]}) {
    /return TRUE
  }

  | chekck for pet illusion
  /if (${cast_data[${_debug}, "${maMinion.Find[stPetIllusion].Value}"]}) {
    /call cast ${_debug} ${Me.Pet.ID} FALSE
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

  /call target set ${Me.Pet.ID}

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
  DEBUG \atget_minion_summon_weapons\ax()

  /if (!PET || !${Me.Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) /return FALSE

  /if (!${Me.FreeInventory}) {
    /invoke ${out[0, 0, 34]}
    /return FALSE
  }

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
    /if (${cast_data[${_debug}, "${get_MAG_folded_pack[]}"]}) {
      /call cast ${_debug} 0 FALSE
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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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

  /if (!AUTO && !MODE) {
    /return FALSE
  }

  | health low enough || enough mobs in range
  /if (${Me.PctHPs} > ${maSash.Find[stPctShieldFlash].Value} && ${SpawnCount[npc radius SAFEENV PS4 targetable]} < ${maSash.Find[stCountShieldFlash].Value}) {
    /return FALSE
  }

  | is the skill available
  /if (${_type.Equal[aa]}) {
    /if (!${cast_data[${_debug}, "Shield Flash", ready]}) {
      /return FALSE
    }
  } else /if (${_type.Equal[disc]}) {
    /if (!${cast_data[${_debug}, "${maSash.Find[stFlash].Value}"]}) {
      /return FALSE
    }
  }

  CHECKEXIT

  /invoke ${spawn_face[TARGET]}
  /call check_sash_cycle ${maDebug.Find[sash].Value} defensive

  | pally / shadowknight
  /if (${_type.Equal[aa]}) {
    /call cast_data ${_debug} "Shield Flash" 
    /call cast ${_debug} 0 FALSE

  | warior
  } else /if (${_type.Equal[disc]}) {
    /call cast_data ${_debug} "${maChr.Find[stFlash].Value}" 
    /call cast ${_debug} 0 FALSE
  }

  | reset to previous bandolier
  /if (${Me.Bandolier[${maSash.Find[stSashShieldFlash].Value}].Name.NotEqual[${_Bandolier}]}) {
    /docommand /timed 70 /call check_sash_cycle ${maDebug.Find[sash].Value} mainsash
  }

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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT cast_glyph NA|NA

  | buy the AA
  /if (!${Me.AltAbility[${maCC.Find[stGlyph].Value}].ID} && ${Me.AAPoints} >= ${AltAbility[${maCC.Find[stGlyph].Value}].Cost} && ${AltAbility[${maCC.Find[stGlyph].Value}].CanTrain}) {
    DEBUG${sep}Buying${sep}\a-w${maCC.Find[stGlyph].Value}\ax
    /alt buy ${AltAbility[${maCC.Find[stGlyph].Value}].ID}
  }

  /if (${cast_data[${_debug}, "${maCC.Find[stGlyph].Value}"]}) {
    /call cast ${_debug} 0 FALSE
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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT cast_AA_spire
  GETINPUT cast_AA_spire NA|NA

  /if (${Raid.Members} && ${maChr.Find[swSpireRaid].Value}) {
    DEBUG spire${sep}Raid
    /if (!${Me.Buff[${maData.Find[stSpire].Value}].ID} && ${Spell[${maData.Find[stSpire].Value}].WillLand}) {
      /if (${cast_data[${_debug}, "${maData.Find[stSpire].Value}"]}) {
        /call cast ${_debug} 0 FALSE
        /return TRUE
      }
    }
  } else /if (!${Raid.Members} && ${maChr.Find[swSpireGroup].Value}) {
    DEBUG spire${sep}Group
    /if (!${Me.Buff[${maData.Find[stSpire].Value}].ID} && ${Spell[${maData.Find[stSpire].Value}].WillLand}) {
      /if (${cast_data[${_debug}, "${maData.Find[stSpire].Value}"]}) {
        /call cast ${_debug} 0 FALSE
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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT cast_AA_intensity
  GETINPUT cast_AA_intensity NA|NA
  
  | so not use with a glyph running
  /if (${Me.Buff[Glyph of Destruction].ID}) {
    /return FALSE
  }
  
  | have the buff
  /if (${Me.Song[${maChr.Find[stAAIntensity].Value}].ID}) { 
    DEBUG ${break}have buff
    /return FALSE
  } 

  | is it ready?
  /if (!${cast_data[${_debug}, "${maChr.Find[stAAIntensity].Value}"]}) {
    /return FALSE
  }

  | stacks?
  /if (!${spellData.WillLand}) {
    DEBUG ${break}does nnot stack
    /return FALSE
  }
  
  | well?
  /if (${Raid.Members} && ${maChr.Find[swIntensityRaid].Value}) {
    DEBUG intensity${sep}\a-wRaid\ax
    /call cast ${_debug} 0 FALSE
  } else /if (!${Raid.Members} && ${Group.Members} && ${maChr.Find[swIntensityGroup].Value}) {
    DEBUG intensity${sep}\a-wGroup\ax
    /call cast ${_debug} 0 FALSE
  }

/return FALSE



|***
 * note: set hud varaibles
 * use: not for your use
 ***|
sub set_hud_data(string _mode)

  /if (${Bool[${_mode}]}) {
    /invoke ${maData.Add[mode,${_mode}]}
  } else {
    /invoke ${maData.Add[mode,--]}
  }
  
/return TRUE


|***
 * note: passive toggle AA Agro switches
 * use: auto based on /cc agro switch
 ***|
sub set_AA_agro(bool _debug)
  DEBUG \atset_AA_agro\ax()

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
 * use: /call get_target_intpull DEBUG [spawnID]
 ***|
sub get_target_intpull(bool _debug, spawn _spawn)
  DEBUG \atget_target_intpull\ax(\a-w${_spawn.ID}\ax)

  /if (!AUTO || !PULL) {
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT get_target_intpull NA|NA
  CHECKTIE

  /if (${_spawn.ID}) {
    /varset stAssTarID ${_spawn.ID}
  } else {
    /call pull_mob_find FALSE
  }

  /if (!TARGET) {
    SETHUD
    /return FALSE
  }

  /call target set ${_spawn.ID}

  /delay 1s
  /dex ${get_resident[${_debug}, |clr, 60]} /advancedpullint ${Me.ID} ${_debug}
  /delay 1s

  /call pull_mob_prep ${_debug} ${_spawn.ID}
  /if (!${Macro.Return}) {
    DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
    /invoke ${target[clear]}
    /return FALSE
  }

  SETHUD

/return TRUE



|***
 * note: moves to an established spawn
 * use: /call pull_mob_move_to DEBUG [spawnID] [distance to stop at] [Line Of Sight on|off]
 ***|
sub pull_mob_move_to(bool _debug, spawn _spawn, int _dist, string _LoS)
  DEBUG \atpull_mob_move_to\ax(\a-wID:${_spawn.ID}, Dis:${_dist}, LoS:${_LoS}\ax)

  /if (!AUTO && !MODE) {
    /return FALSE
  }

  /nav ID ${_spawn.ID} |distance=${_dist} lineofsight=${_LoS} log=${maEntropy.Find[swNavLogSpew].Value}
  /delay 1s ${Navigation.Active}

  DEBUG ${sep}moving to${sep}\a-w${_spawn.ID}\ax
  /while (${Navigation.Active}) {
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE

    /if (!AUTO && !MODE) {
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
    /if (!${_spawn.ID} || ${_spawn.Dead}) {
      DEBUG ${sep}/while \a-gbreak_NO_target\ax
      /invoke ${target[clear]}
      /break
    }
    
    | self heal while returning to camp; heal needs tag ignoremovement
    /if (${maPull.Find[swPullHealCheck].Value}) {
      /call spawn_hurt ${maDebug.Find[hurt].Value} self
      /if (${hurtCount}) {
        /call cast_cycle_heal ${_debug} ${Me.ID} ignoremovement
      }
    }    

    | are we there yet??? (no los required)
    /if (!${Bool[${_LoS}]}) {
      /if (${_spawn.Distance} <= ${_dist} && !${_spawn.LineOfSight}) {
        DEBUG ${sep}dis check${sep}\a-w${_spawn.Distance} <= ${_dist} && !\arLOS\ax
        /delay 2
        /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
        /return TRUE
      }
      
    | LOS required
    } else /if (${Bool[${_LoS}]}) {
      /if (${_spawn.Distance} <= ${_dist} && ${_spawn.LineOfSight}) {
        DEBUG ${sep}dis \arHALT\ax${sep}\a-w${_spawn.Distance} <= \ag${_dist}\ax && \agLOS\a-w (\a-r${_spawn.ID}\ax)
        /nav stop |log=${maEntropy.Find[swNavLogSpew].Value}
        /return TRUE
      }
    }

    | abort nav if agro (non mode)
    /if (!${swModeToken}) {
      /if (${SpawnCount[npc radius SAFEENV PS4]} > 0  && ${NearestSpawn[npc radius SAFEENV PS4].ID} != ${_spawn.ID} && ${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} > SAFEENV) {
        DEBUG ${break}we have agro.. drop intended target and return to camp
        /varset _spawn 0
        /return TRUE    
      }
    }

    | abort pull if agro (mode)
    /if (${swModeToken}) {
      /if (${SpawnCount[npc radius SAFEENV PS4]} > 0 && ${NearestSpawn[npc radius SAFEENV PS4].ID} != ${_spawn.ID}) {
        DEBUG ${break}we have agro.. drop intended, clear and allow reacquisition
        /invoke ${target[clear]}
        /call set_stop_movement ${_debug}
        /return FALSE
      }
    }

    | range from camp check
    /if (${Select[${maPull.Find[stPullMode].Value},nav]}) {
      /if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} > ${Math.Calc[PULLENV + (PULLENV*.${maPull.Find[stPullNavVariance].Value})]} && !${swModeToken}) {
        DEBUG ${break}Mob exceeded nav path range range
        /varset _spawn 0
        /return TRUE
      }
    } else {
      /if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} > PULLENV && !${swModeToken}) {
        DEBUG ${break}Mob exceeded setMobAGro range
        /varset _spawn 0
        /return TRUE
      }
    }
    
    | alow move to mob to stop sooner on ranged combat for modes only
    /if (${swModeToken}) {
      /if (RANGE && ${_spawn.Distance} <= ENV) {
        /call target set ${_spawn.ID}
        /return TRUE
      }
    }

    /delay 1
  }

/return TRUE



|***
 * note: uses a pet class to get mobs to camp
 * use: /call get_target_petpull DEBUG [targetID] [adjusted find counter]
 ***|
sub get_target_petpull(bool _debug, spawn _spawn, int _spCount)
  DEBUG \atget_target_petpull\ax(\a-w${_spawn.ID}, ${_spCount}\ax)

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT get_target_petpull _spawn|${_spawn.ID}
  CHECKTIE

  | fix the pet
  /if (!${Me.Pet.ID} && PET) {
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
    SETHUD
    /return FALSE
  } else {
    /if (!${_spawn.ID}) {
      /varset _spawn TARGET
    }
  }

  | target the mob
  /call target set ${_spawn.ID}

  | if the target is mezed, we need to look again, increment the spawncount
  /if (${Target.Mezzed.ID}) {
    /invoke ${target[clear]}
    /call pull_mob_find ${_debug} ${Int[${Math.Calc[${_spCount} + 1]}]}
  }

  DEBUG ${sep}target${sep}\ay${_spawn.ID}\ax${sep}\aw${_spawn.DisplayName}\ax ${sep} dist:\a-w${_spawn.Distance}\ax

  /if (${Me.Pet.Taunt}) /pet taunt
  /if (${Me.Pet.Stance.NotEqual[follow]}) /pet follow
  /if (!HOME) /call Bind_control_home set FALSE silent ${maDebug.Find[home].Value}

  | check for ammo if needed
  /if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID}) {
    /call check_buff_cycle_ammo ${maDebug.Find[ammo].Value} TRUE
  }

  /invoke ${out[9, "Fetching${sep}${con_color[${_debug}, ${_spawn.ID}]}\ax (\a-r${_spawn.Distance}\ax)"]}

  /if (!${Me.Pet.ID} && PET) {
    /call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
  }

  /declare _targetmelee float local ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}

  | get closer if we need too
  /if (${_spawn.Distance} > ${maHard.Find[stMaxRadPet].Value}) {
    /call pull_mob_move_to ${_debug} ${_spawn.ID} ${maPull.Find[stPullNavStopDistance].Value} off
  }
  
  /while (${Me.Moving}) {
    /delay 1
  }

  | target the mob
  /call target set ${_spawn.ID}

  | we set the pet to mob timer based off total distance you have to run to the mob + a buffer good good measure.
  | pet pathing is shit and for longer pulls need the longer wait time
  /declare _waitforstupidpet int local ${Math.Calc[(${Math.Distance[${_spawn.Y}, ${_spawn.X}, ${_spawn.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} /10) + 10]}
  /invoke ${set_timer[${_debug}, timer_waitforstupidpet, ${_waitforstupidpet}s]}

  /pet attack

  /while (TRUE) {

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    GETINPUT get_target_petpull_range NA|NA

    /if (!${_spawn.ID} || ${_spawn.Dead}) {
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
    /delay 5 ${Math.Distance[${Me.Pet.Y}, ${Me.Pet.X}, ${Me.Pet.Z} : ${_spawn.Y}, ${_spawn.X}, ${_spawn.Z}]} < ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
    DEBUG ${sep}waiting on pet to get to target${sep}\a-w${Math.Distance[${Me.Pet.Y},${Me.Pet.X}:${_spawn.Y},${_spawn.X}]} range\ax
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
    SETHUD
    /if (HOME) {
      /call pull_home_move_to ${_debug} ${_spawn.ID}
    }
    /return FALSE
  }

  /call where_the_fuck_is_my_pet ${maDebug.Find[lost].Value} FALSE

  /if (HOME) {
    /call pull_home_move_to ${_debug} ${_spawn.ID}
  }

  | wait for mob to get to camp
  /call get_target_wait ${maDebug.Find[wait].Value} ${_spawn.ID} ${_waitforstupidpet}s
  SETHUD

/return



|***
 * note: mechanics for nav pulling
 * note: /call get_target_navpull DEBUG [spawn] [spawncount adjustment]
 ***|
sub get_target_navpull(bool _debug, spawn _spawn, int _spCount)
  DEBUG \atget_target_navpull\ax(\a-w${_spawn.ID}, ${_spCount}\ax)

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT get_target_navpull _spawn|${_spawn.ID}
  CHECKTIE

  /declare _count int local 0

  /if (!PULL) {
    /call set_pull_observer ${_debug}
  } else /if (PULL) {
    /if (!${_spawn.ID} && !${targetData.ID}) {
      
      /if (!${Bool[${_spCount}]}) {
        /varset _spCount 0
      }      
      
      /call pull_mob_find ${_debug} ${_spCount}
      /if (!${targetData.ID}) {
        DEBUG ${sep}\arNO\ax${sep}\a-wtargetData.ID\ax
        SETHUD
        /return FALSE
      }
      
    }
  }

  | correct the working spawn information
  /if (!${_spawn.ID} && ${targetData.ID}) {
    /vardata _spawn targetData
  }


  | we have a target, get us within range to pull
  /if (!${Navigation.PathExists[id ${_spawn.ID}]} && !${maOver.Find[swOverNavCheck].Value}) {
    GETINPUT get_target_navpull_wait _spawn|${_spawn.ID}
    /invoke ${out[9, "${notice}${sep}No Nav Path ${dot} ${con_color[${_debug}, ${_spawn.ID}]}\ax"]}
    /varcalc _count ${_count}+1
    /if (${_count} > 10) /return FALSE
    /delay 1s
    /call pull_mob_find ${_debug} ${_count}
  }

  DEBUG ${sep}Target ${sep} ${_spawn.ID} ${sep} \aw${_spawn.DisplayName}\ax

  | force home even if not using it
  /if (${maHome.Find[swinCombat].Value} && ${maPull.Find[swPullSetHome].Value} && !HOME) {
    /call Bind_control_home set FALSE silent ${maDebug.Find[home].Value}
  }

  | set the target
  /invoke ${out[9, "Fetching${sep}${con_color[${_debug}, ${_spawn.ID}]}\ax"]}
  | /call target set ${_spawn.ID} 

  | if the mob is kinda close??
  /if (${_spawn.LineOfSight} && ${_spawn.Distance} < ENV) {
    DEBUG ${sep}navpull${sep}in sight < ENV
    /call pull_mob_prep ${_debug} ${_spawn.ID}
    /if (!${Macro.Return}) {
      DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
      /invoke ${target[clear]}
      /return FALSE
    }

    /if (MELEE && ${_spawn.Distance} > ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /call pull_mob_move_to ${_debug} ${_spawn.ID} ${maPull.Find[stPullNavStopDistance].Value} ${maPull.Find[swPullNavLoS].Value}
    }

    /call pull_mob_tag ${_debug} ${_spawn.ID}
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
    DEBUG ${sep}Sight:${_spawn.LineOfSight} ${dot} Distance:${_spawn.Distance} ?? ${maPull.Find[stPullNavStopDistance].Value}

    | melee pulls get up the mobs ass
    | all other methods obey the stop distance settings
    /if (${maPull.Find[stPullWith].Value.Equal[melee]}) {
      /call pull_mob_move_to ${_debug} ${_spawn.ID} ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} ${maPull.Find[swPullNavLoS].Value}
    } else {
      /call pull_mob_move_to ${_debug} ${_spawn.ID} ${maPull.Find[stPullNavStopDistance].Value} ${maPull.Find[swPullNavLoS].Value}
    }

    /while (${Me.Moving}) {
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      GETOVERHERE
      /if (${_spawn.LineOfSight} && ${_spawn.Distance} < ${maPull.Find[stPullNavStopDistance].Value}) {
        /break
      }
      /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
        /break
      }
      /delay 5
    }
    /call pull_mob_prep ${_debug} ${_spawn.ID}
    /if (!${Macro.Return}) {
      DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
      /invoke ${target[clear]}
      /return FALSE
    }
  }

  /delay 1s !${Me.Moving}
  /call pull_mob_tag ${_debug} ${_spawn.ID}
  /delay 2

  | failure on tag
  /if (${Macro.Return.Equal[tagfailure]}) {
    DEBUG ${sep}\arNAVpull FAILURE\ax${sep}resetting/returning to camp
    /invoke ${target[clear]}
    /call pull_home_move_to ${_debug} ${Me.ID}    
    /call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
    /return FALSE
  }

  /if (HOME) {
    /call pull_home_move_to ${_debug} ${_spawn.ID}
  }
  
  /invoke ${spawn_face[${_spawn.ID}]}

  /declare _waittime int local ${Math.Calc[(PULLENV + ${_spawn.Distance})*.08]}
  /call get_target_wait ${maDebug.Find[wait].Value} ${_spawn.ID} ${_waittime}s

  SETHUD

/return TRUE



|***
 * note: still need to build this
 * use: /call get_target_calmpull DEBUG [spawnID]
 ***|
sub get_target_calmpull(bool _debug, spawn _spawn)
  DEBUG \atget_target_calmpull\ax(\a-w${_spawn.ID}\ax)
| find a target

| find out how many are around it.

| lul more then 2 targets

| tag extra

| bring it back to camp

/return



|***
 * note: pull mobs from the watch list first
 * use: /call get_target_watchpull DEBUG [spawn]
 ***|
sub get_target_watchpull(bool _debug, spawn _spawn)
  DEBUG \atget_target_watchpull\ax(\a-w${_spawn.ID}\ax)

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT get_target_watchpull NA|NA
  CHECKTIE

  /if (TARGET && ${Spawn[TARGET].ID}) {
    DEBUG ${sep}we already have a target${sep}\a-wTARGET\ax
    /return TRUE
  }

  /if (${maWatch.Find[swWatch].Value} && !${timer_Check_Watch}) {
    /if (${lsWatch.Count}) {
      /call check_watch_spawn ${maDebug.Find[watch].Value}
    }
  }

  SETHUD "pull watchlist"
  
  | no mobs passed, look for something on the watch list
  /declare _liwp listiterator local
  /vardata _liwp lsWatch.First.Clone
  /while (!${_liwp.IsEnd}) {

    | no spawns around?
    /if (!${Spawn[radius PULLENV zradius ${maPull.Find[stPullZRadius].Value} ${_liwp.Value}].ID}) {
      /delay 5
      /invoke ${_liwp.Advance}
      /continue      
    }
    
    | valid path?
    /if (!${Navigation.PathExists[id ${Spawn[radius PULLENV zradius ${maPull.Find[stPullZRadius].Value} ${_liwp.Value}].ID}]}) {
      /invoke ${_liwp.Advance}
      /continue      
    }

    /vardata _spawn Spawn[radius PULLENV zradius ${maPull.Find[stPullZRadius].Value} ${_liwp.Value}]

    DEBUG .. ${_spawn.DisplayName} .. ${_spawn.ID}
    /call get_target_navpull ${_debug} ${_spawn.ID}

    SETHUD
    /return FALSE
    /invoke ${_liwp.Advance}
  }

/return TRUE



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

  /if (!${targetData.ID}) {
    DEBUG ${sep}\arNO\ax targetData
    SETHUD
    /return FALSE
  }

  | maintain home marker
  /if (!HOME) {
    /call Bind_control_home set NULL NULL ${maDebug.Find[home].Value}
  }

  | how the hell do we want to kill it.?

  | pull with a normal range if the mob is los and within 100. multi puller keeps local camp under control.
  /if (${targetData.LineOfSight} && ${targetData.Distance} < 100) {
    DEBUG ${sep}Spawn is LOS & < \ag100\ax
    /invoke ${out[9, 0, 55]}
    /target id ${targetData.ID}
    /delay 1s ${Target.ID} == ${targetData.ID}

    | get us a mob
    /call pull_mob_prep ${_debug} ${targetData.ID}
    /if (!${Macro.Return}) {
      DEBUG ${sep}\arPrep FAILURE\ax${sep}resetting
      /invoke ${target[clear]}
      /return FALSE
    }

    /call pull_mob_tag ${_debug} ${targetData.ID}
    /if (${Macro.Return.Equal[tagfailure]}) {
      DEBUG ${sep}\arNAVpull FAILURE\ax${sep}resetting/returning to camp
      /invoke ${target[clear]}
      /call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
      /return FALSE
    }

  | the mob is not close enough to risk abandoning the camp or it is not in our direct line of sight. send some poor other gimp to get us some loots.
  } else /if (!${targetData.LineOfSight} || (${targetData.Distance} > 100 && ${targetData.Distance} < PULLENV)) {
    DEBUG Spawn is > 100  & < \ayPULLENV\ax

    | is there a usable nav path to this pos?
    /if (${Navigation.PathExists[id ${targetData.ID}]}) {
      DEBUG ${sep}NAV Path \aggood\ax

      | we try melee first for nav to target then we try pet pulling

      | see if we have any melee classes avaialble?
      /call get_name_multipull ${_debug} |tank|melee

      /if (${Bool[${Macro.Return}]}) {
        /dex ${Macro.Return} /advancedpullnav ${targetData.ID} ${_debug}
        /goto :waitformobmp
      }

      | no melee available get a pet puller?
      /call get_name_multipull FALSE |MAG|BST|NEC|ENC|SHM|DRU
      /if (${Bool[${Macro.Return}]}) {
        /dex ${Macro.Return} /advancedpullpet ${targetData.ID} FALSE
        /goto :waitformobmp
      }

    | no path exists to get to target, "God Save the Queen!"
    } else /if (!${Navigation.PathExists[id ${targetData.ID}]}) {
      DEBUG ${sep}NAV Path \arbad\ax
      :intpull
      /call get_target_intpull ${_debug} ${targetData.ID}
      /goto :waitformobmp

    }

  }

  | wait for mob to get to camp
  :waitformobmp
  /if (!${SpawnCount[npc radius ${maHard.Find[stMinBowDist].Value} PS4 targetable]}) {

    /invoke ${set_timer[${_debug}, wait_for_mob, 30s]}
    /invoke ${out[9, "Waiting on ${dot} ${con_color[${_debug}, TARGET]}${sep}\a-w30s\ax"]}
    /while (${targetData.ID} && ${targetData.Distance} > ${maHard.Find[stMinBowDist].Value}) {
      GETINPUT get_target_multipull_wait NA|NA
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      /delay 2

      | target gone.. wtf?
      /if (!${targetData.ID}) {
        DEBUG ${sep}/while \a-gbreak NO targetData.ID\ax
        /break
      }

      | mobs in range not our target
      /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} && ${targetData.Distance} > ${maHard.Find[stMinBowDist].Value}) {
        DEBUG ${sep}/while \a-gbreak_TARGET/RANGE\ax
        /break
      }
      | target LOS & within tag range
      /if (${targetData.LineOfSight} && ${targetData.Distance} < PULLENV &&  && ${targetData.Distance} < 100) {
        DEBUG ${sep}/while \a-gbreak_tag\ax
        /break
      }
      | target within melee
      /if (${targetData.Distance} <= ${targetData.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) {
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
  /declare _spawn spawn local

  /for _count 1 to ${DanNet.PeerCount[priest]}
    
    | /if (!${Spawn[pc ${DanNet.Peers[priest].Arg[${_count},|]}].ID}) {
    |   /continue
    | }
    
    /vardata _spawn Spawn[pc ${DanNet.Peers[priest].Arg[${_count},|]}]

    /if (!${_spawn.ID}) /continue
    /if (${_spawn.Dead}) /continue
    /if (${_spawn.Distance} > ${_distance}) /continue
    /break

  /next _count

/return ${_spawn.DisplayName}



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
    /if (${cast_data[${_debug}, "Summon Companion"]}) {
      /call cast ${_debug} 0 FALSE
      /delay 5s
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
    /varset HUDOutput ${sqlite.Result[getstring 1 value]}

  | shade output. grain validation
  } else /if (${_verbage.Equal[shade]} && (${lsGrain.Contains[${_lvl}]} || ${_lvl} == 0)) {
    /lua run list_shade
    /return TRUE

  | output grain validation
  } else /if (${Range.Between[1,99999:${Int[${_strNum}]}]} && (${lsGrain.Contains[${_lvl}]} || ${_lvl} == 0)) {
    /sqlite query "${DBstatic}" getstring SELECT value FROM string WHERE env_var='${_strNum}';
    /if (!${sql_check[${_debug}, getstring, out, SELECT]}) /endmacro
    /if (${sqlite.Rows[getstring]}) {
      ECHO ${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}${sqlite.Result[getstring 1 value]}
      /varset HUDOutput ${sqlite.Result[getstring 1 value]}
    }

  | output grain validation with passed verbage
  } else /if (!${Range.Between[1,99999:${Int[${_strNum}]}]} && (${lsGrain.Contains[${_lvl}]} || ${_lvl} == 0)) {
    ECHO ${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,]}${_verbage}
    /varset HUDOutput ${_verbage}

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
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      GETOVERHERE
      /if (${_mimode.Value}) {
        /invoke ${maMode.Add[${_mimode.Key},FALSE]}
      }
      /invoke ${_mimode.Advance}
    }
    /return TRUE
  }

  | normal check routine
  /while (!${_mimode.IsEnd}) {
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
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
 * note: this is used to cycle the control type maps and handle the IDs in it. final return will be an uncounted for mob ID, if any
 * use: /call check_control_routine DEBUG CONTROLTYPE
 ***|
sub control_routine(bool _debug, string _type)
  DEBUG \atcontrol_routine\ax(\a-w${_type}\ax)

  /if (!AUTO || SAFEZONE) {
    /return FALSE
  }

  GETINPUT control_routine NA|NA
  CHECKEXIT

  | slow this shit down so we can read it...
  /if (${_debug}) {
    /delay 1s
  }

  /declare _count int local 0

  | check control map for any mobs dead or gone
  /invoke ${control_map[${_debug}, dead]}
  | count of uncontrolled mobs in area
  /declare _countArea int local 0


  /if (${Select[${_type},Mez]} && ${madeBuff.Find[swMez].Value}) {
    /if (!${maCC.Find[swCombatControl].Value}) /return FALSE
    /delay 1

    | ID for unmezzed/short mob
    /declare _mezzID int local 0

    | ae mez marker
    /declare _aeMezDone bool local FALSE

    | find unaccounted mobs in my area
    | returns the # of mobs that do not have a current mez timer associated
    /varset _countArea ${control_area[${_debug}, Mez]}
    DEBUG _countArea${sep}\ay${_countArea}\ax

    | if no mobs are returned we bail
    /if (!${_countArea}) {
      DEBUG ${break}\arNo\ax mez targets${sep}\a-w${_countArea}\ax
      /delay 1
      /return TRUE
    }

    | if ${control_area returns more then the toons AE setting, hit the AE routines
    /if (AE#) {
      /if (${_countArea} >= AE#) {
        | do we have an ae mez spell
        /call cast_cycle_debuff ${maDebug.Find[mez].Value} 0 mezae TRUE          
        /if (${Bool[${maCD.Find[returnCast].Value}]}) {
           DEBUG ${sep}ae mez
          /call control_mezae ${_debug}
          /varset _aeMezDone TRUE
        }
      }
    }

    | if we have more then one mob in camp, Not including the assists target
    /if (${_countArea}) {
      DEBUG ${sep}single target mez

      | mez something for christs sake...
      /declare _mobcheck mapiterator local
      /vardata _mobcheck maMobControl${_type}.First.Clone
      /while (!${_mobcheck.IsEnd}) {
        
        | skip assists target
        /if (${_mobcheck.Key} == TARGET) {
          DEBUG ${sep}target${sep}\a-wcurrent assist target\ax
          /invoke ${_mobcheck.Advance}
          /continue
        }
        
        | skip mobs with timers
        /if (${Defined[timer_mez_ID${_mobcheck.Key}]}) {
          /if (${timer_mez_ID${_mobcheck.Key}}) {
            DEBUG ${sep}target timer${sep}\a-w${timer_mez_ID${_mobcheck.Key}}\ax
            /invoke ${_mobcheck.Advance}
            /continue
          }
        }      
        
        | skip exempt targets
        /if (${Select[${_mobcheck.Value},EXEMPT,CHARM]}) {
          DEBUG ${sep}target${sep}\a-w${_mobcheck.Value}\ax
          /invoke ${_mobcheck.Advance}
          /continue
        }

        /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
        GETOVERHERE
        CHECKEXIT
        /call control_mez ${_debug} ${Spawn[${_mobcheck.Key}].ID}
        /invoke ${_mobcheck.Advance}
      }
      /varset _aeMezDone FALSE
      /delay 1
    }
  } else /if (${Select[${_type},Slow]} && ${madeBuff.Find[swSlow].Value}) {
  } else /if (${Select[${_type},Charm]} && ${madeBuff.Find[swCharm].Value}) {
  } else /if (${Select[${_type},Malo]} && ${madeBuff.Find[swMalo].Value}) {
  } else /if (${Select[${_type},Tash]} && ${madeBuff.Find[swTash].Value}) {
  }
  
  DEBUG ${sep}control_routine${sep}\arexit\ax
  /delay 1
  
/return TRUE



|***
 * note: this is used to check our current ENV and find mobs that we do not know about that are pissed at us
 * use: ${control_area[DEBUG, type]}
 ***|
sub control_area(bool _debug, string _type)
  DEBUG \atcontrol_area\ax(\a-w${_type}\ax)

  /declare _agroCount int local 0
  /declare _count int local 0
  /declare _spcount int local 0
  /declare _spawn spawn local

  /varset _spcount ${SpawnCount[npc los range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ENV zradius ${maCC.Find[stZRadius].Value} PS4]}
  /if (!${_spcount}) {
    /return ${_agroCount}
  }
  /for _count 1 to ${_spcount}

    | get the first mob in our discovery
    /vardata _spawn NearestSpawn[${_count}, npc los range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ENV zradius ${maCC.Find[stZRadius].Value} PS4]
    DEBUG ${sep}control _spawn${sep}\a-w${_spawn.ID}\ax

    | if this mob is the current assists target, remove it from our need to control
    /if (${_spawn.ID} == TARGET) {
      /if (${maMobControl${_type}.Contains[${_spawn.ID}]}) {

        DEBUG ${sep}removing assist target from maMobControl${_type} map${sep}\a-w${_spawn.ID}\ax
        /invoke ${maMobControl${_type}.Remove[${_spawn.ID}]}

        | delete timers 
        /declare _lsdeb list local
        /invoke ${_lsdeb.Append[mez,malo,slow,charm,tash]}
        /declare _lscadeb listiterator local
        /vardata _lscadeb _lsdeb.First.Clone
        /while (!${_lscadeb.IsEnd}) {
          /if (${Defined[timer_${_ligmfp.Value}_ID${_spawn.ID}]}) {
            DEBUG ${sep}deleting timer for${sep}\a-wtimer_${_ligmfp.Value}_ID${_spawn.ID}\ax
            /deletevar timer_${_ligmfp.Value}_ID${_spawn.ID}
          }
          /invoke ${_ligmfp.Advance}
        }
      }
      /continue
    }
    
    /if (!${_spawn.ID}) {
      /continue
    }

    | build our counter to return
    DEBUG ${sep}increment aecounter${sep}\a-w${_agroCount} + 1\ax
    /varcalc _agroCount ${_agroCount} + 1

    | do we have the mob in the mez list ??
    /if (${maMobControl${_type}.Contains[${_spawn.ID}]}) {
      DEBUG ${sep}mob already in maMobControl${_type} map${sep}\a-w${_spawn.ID}\ax
      /continue
    }
    
    | mob is not in list and not in agro list. fresh meat...
    /if (!${maMobControl${_type}.Contains[${_spawn.ID}]}) {
      DEBUG ${sep}mob \arNOT\ax in maMobControl${_type} map${sep}\a-w${_spawn.ID}\ax

      | identify mobs we dont touch
      /if (${lsZoneExclude.Contains[${_spawn.DisplayName}]}) {
        DEBUG ${sep}lsZoneExclude${sep}\a-w${_spawn.DisplayName}\ax
        /invoke ${maMobControl${_type}.Add[${_spawn.ID},EXEMPT]}

      | charm
      } else /if (${_type.Equal[charm]} && ${lsZoneCharm.Contains[${_spawn.DisplayName}]} && ${madeBuff.Find[swCharm].Value}) {
        DEBUG ${sep}lsZoneCharm${sep}\a-w${_spawn.DisplayName}\ax
        /invoke ${maMobControl${_type}.Add[${_spawn.ID},CHARM]}

      | mez
      } else /if (${_type.Equal[mez]} && !${lsZoneNoMez.Contains[${_spawn.DisplayName}]} && ${madeBuff.Find[swMez].Value}) {
        DEBUG ${sep}adding to control map${sep}\a-w${_type}\ax
        /if (!${Defined[timer_mez_ID${_spawn.ID}]}) {
          /invoke ${set_timer[${_debug}, timer_mez_ID${_spawn.ID}, 1]}
        }
        /invoke ${maMobControl${_type}.Add[${_spawn.ID},MEZ]}

      | malo
      } else /if (${_type.Equal[malo]} && ${madeBuff.Find[swMalo].Value}) {
        DEBUG ${sep}adding to control map${sep}\a-w${_type}\ax
        /if (!${Defined[timer_malo_ID${_spawn.ID}]}) {
          /invoke ${set_timer[${_debug}, timer_malo_ID${_spawn.ID}, 1]}
        }
        /invoke ${maMobControl${_type}.Add[${_spawn.ID},MALO]}

      | slow
      } else /if (${_type.Equal[slow]} && ${madeBuff.Find[swSlow].Value}) {
        DEBUG ${sep}adding to control map${sep}\a-w${_type}\ax
        /if (!${Defined[timer_slow_ID${_spawn.ID}]}) {
          /invoke ${set_timer[${_debug}, timer_slow_ID${_spawn.ID}, 1]}
        }
        /invoke ${maMobControl${_type}.Add[${_spawn.ID},SLOW]}

      | tash
      } else /if (${_type.Equal[tash]} && ${madeBuff.Find[swTash].Value}) {
        DEBUG ${sep}adding to control map${sep}\a-w${_type}\ax
        /if (!${Defined[timer_tash_ID${_spawn.ID}]}) {
          /invoke ${set_timer[${_debug}, timer_tash_ID${_spawn.ID}, 1]}
        }
        /invoke ${maMobControl${_type}.Add[${_spawn.ID},TASH]}

      }

    }

  /next _count

/return ${_agroCount}



|***
 * note: check the IDs in the passed list. remove them from the list if they are dead/gone
 * use: ${control_map[DEBUG, THINGIETOCHECK??]}
 *
 * THINGIETOCHECK:
 * dead = check for dead or no more spawns. removes them from all control maps
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
    /declare _miremtype mapiterator local

    /declare _lsdbt listiterator local
    /vardata _lsdbt lsdebuffType.First.Clone
    /while (!${_lsdbt.IsEnd}) {

      /vardata _mirem maMobControl${_lsdbt.Value}.First.Clone
      /invoke ${maRemovals.Clear}
      /while (!${_mirem.IsEnd}) {
        GETINPUT
        DEBUG ${sep}_mirem.Key--.Value${sep}\a-w${_mirem.Key} -- ${_mirem.Value}\ax${sep}${If[!${Spawn[${_mirem.Key}].ID} || ${Spawn[${_mirem.Key}].Dead},\ardead\ax,\agalive\ax]}
        /if (!${Spawn[${_mirem.Key}].ID} || ${Spawn[${_mirem.Key}].Dead} || ${Spawn[${_mirem.Key}].ID} == TARGET) {
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
        /invoke ${maMobControl${_lsdbt.Value}.Remove[${maRemovals.Head}]}
      /next _count

      /invoke ${_lsdbt.Advance}
    }


  }

  | find a non mezed mob or mob with a short duration mez left. return its ID if any for cc
  /if (${_type.Equal[mez]}) {
    DEBUG ${sep}cc${sep}
    /declare _mimez mapiterator local
    /vardata _mimez maMobControlMez.First.Clone
    /while (!${_mimez.IsEnd}) {
      DEBUG ${sep}_mimez.Key--.Value${sep}\a-w${_mimez.Key} -- ${_mimez.Value}\ax
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      GETOVERHERE
      
      
      /if (${Bool[${maCC.Find[stAssistMode].Value}]} && ${_mimez.Key} == TARGET) {
        /invoke ${_mimez.Advance}
        /continue
      }

      /if (${_mimez.Value.Equal[MEZ]} && ${Defined[timer_mez_ID${_mimez.Key}]}) {

        /if (!${Defined[timer_mez_ID${_mimez.Key}]}) {
          /invoke ${set_timer[${_debug}, timer_mez_ID${_mimez.Key}, 1]}
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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT control_mezae NA|NA
  /declare _spawn spawn local

  /if (${maMobControlMez.Count} >= AE#) {
    DEBUG ${sep}entering AE mez
    /vardata _spawn Spawn[${maMobControlMez.First.Key}]

    /if (AE# && ${SpawnCount[npc los radius 30 targetable]} >= AE#) {
      DEBUG ${sep}AE# && ${SpawnCount[npc los radius 30 targetable]} >= AE#

      /if (${Me.Gem[${Spell[${maChr.Find[stAEStun].Value}].RankName}]}) {
        /if (${Me.Casting.ID}) /stopcast
        /delay 2.25s !${Me.SpellInCooldown}
        /call cast_data ${_debug} "${maChr.Find[stAEStun].Value}"
        /call cast ${_debug} 0 FALSE
      }

      | /if (${Me.Casting.ID}) /stopcast
      /delay 2.25s !${Me.SpellInCooldown}
      /call cast_cycle_debuff ${_debug} ${_spawn.ID} mezae

    }

    | use AA beam of slumber?
    /if (${maChr.Find[swAABoS].Value}) {
      /if (${_spawn.NearestSpawn[3,npc radius 30].ID} && ${_spawn.Distance3D} <= 200 && ${_spawn.Distance3D} >= ENV) {
        /if (${cast_data[${_debug}, "${maChr.Find[stAABeamofSlumber].Value}"]}) {
          /if (${Me.Casting.ID}) /stopcast
          /invoke ${spawn_face[${_spawn.ID}]}
          /call cast ${_debug} 0 FALSE
        }
      }
    }
  }

/return



|***
 * note: single mez the target
 * use: /call control_mez DEBUG [spawn]
 ***|
sub control_mez(bool _debug, spawn _spawn)
  DEBUG \atcontrol_mez\ax(${_spawn.DisplayName})

  /if (!${_spawn.ID}) {
    /return FALSE
  }

  | if theres no timer.. make it
  /if (!${Defined[timer_mez_ID${_spawn.ID}]}) {
    /invoke ${set_timer[${_debug}, timer_mez_ID${_spawn.ID}, 1]}
  }

  | if the current timer is greater then maChr streMezTime seconds, move on
  /if (${timer_mez_ID${_spawn.ID}} > ${maChr.Find[streMezTime].Value}) {
    /return FALSE
  }

  /declare _resistCount int local 0
  GETINPUT control_mez NA|NA

  | try to mez more then once if there is a fail
  /while (${_resistCount} < ${madeBuff.Find[stCountMezResist].Value}) {
    
    /delay 1

    | turning attak off. bards are stupid.
    /if (${Me.Combat}) {
      /squelch /attack off
      /delay 1
    }   

    /call get_assist_target ${maDebug.Find[assist].Value} return
    
    /if (${_spawn.ID} == ${targetData.ID}) {
      /invoke ${set_timer[${_debug}, timer_mez_ID${_spawn.ID}, 30s]}
      /return FALSE
    }

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    GETINPUT

    /if (!${_spawn.LineOfSight}) {
      /varcalc _resistCount ${_resistCount} + 1
      /continue
    }

    /call target set ${_spawn.ID}
    
    /if (${Target.Mezzed.Duration.TotalSeconds} > ${maChr.Find[streMezTime].Value}) {
      /invoke ${set_timer[${_debug}, timer_mez_ID${_spawn.ID}, ${Target.Mezzed.Duration.TotalSeconds}s]}
      /return TRUE
    }

    | mez ready?

    /delay 2.25s !${Me.SpellInCooldown}    
    /call cast_cycle_debuff ${maDebug.Find[mez].Value} ${_spawn.ID} mez

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
        /invoke ${maMobControlMez.Add[${_spawn.ID},EXEMPT]}
        /if (${Defined[timer_mez_ID${_spawn.ID}]}) {
          /deletevar timer_mez_ID${_spawn.ID}
          /return FALSE
        }

      }

      /invoke ${set_timer[${_debug}, timer_mez_ID${_spawn.ID}, 36s]}
      /return TRUE

    } else /if (${Select[${maData.Find[stCastReturn].Value},RESISTED,RESISTEDYOU]}) {
      /varcalc _resistCount ${_resistCount} + 1
      /if (${_resistCount} >= ${madeBuff.Find[stCountMezResist].Value}) {
        /call cast_cycle_debuff ${maDebug.Find[tash].Value} ${_spawn.ID} tash
      }
    }
  }

/return



|***
 * note: builds the outputs for various / commands
 * use:

 - option - the command is just an option no associated variables
 - out = simply display the variable with no off status
 - submenu = marker pointing to a deeper menu
 - br = square brackets surround the variable [variable]
 - sw - an ON/OFF green/red switch
 - pr - surround the on/off switch in a ( )
 - mass - mass group buff execution option (spice, aid, cr, so on)
 - br_no_c - bracketed variables w/o collections
 ***|
sub set_control_output(bool _debug, string _map, string _outname, string _control)
  DEBUG \atset_control_output\ax(\a-w${_map}, "${_outname}", ${_control}\ax)

  /declare _out string local
  /declare _value string local
  /declare _cmi mapiterator local
  /vardata _cmi ${_control}.First.Clone
  
  | start the control with the list option if there is one.
  /if (${${_control}.Contains[list]}) {
    /varset _out ${_out} ${out_list}
    /if (${${_control}.Count} > 1) {
      /varset _out ${_out} ${dot}
    }
  }
  
  | single popup outputs
  /if (${Bool[${maData.Find[outVar].Value}]}) {
    /invoke ${out_popup["${maData.Find[outVar].Value}: ${maData.Find[outVal].Value}"]}
    /invoke ${maData.Add[outVar,FALSE]}
    /invoke ${maData.Add[outVal,FALSE]}
  }
    
  /while (!${_cmi.IsEnd}) {
    DEBUG ${sep}left ${sep}\a-w${_cmi.Value.Left[2]}\ax
    DEBUG ${sep}${_cmi.Key}${sep}\a-w${_cmi.Value}\ax${sep}\a-y${${_map}.Find[${_cmi.Value}].Value}\ax

    /if (${_cmi.Value.Count[|]}) {
      /varset _value ${_cmi.Value.Arg[1,|]}
    } else {
      /varset _value ${_cmi.Value}
    }

    DEBUG ${sep}_cmi${sep}\a-w${_cmi.Value}\ax

    | submenu only
    /if (${_cmi.Value.Equal[submenu]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:${submenu}
      
    | no variables just one shot option
    } else /if (${_cmi.Value.Equal[list]}) {
      /invoke ${_cmi.Advance}
      /continue   
      
    | no variables just one shot option
    } else /if (${_cmi.Value.Equal[option]}) {
      /varset _out ${_out} \a#1E90FF${_cmi.Key}\ax

    | no variables just one shot mass group buff
    } else /if (${_cmi.Value.Equal[mass]}) {
      /varset _out ${_out} \a#FF00F3${_cmi.Key}\ax

    | no variables
    } else /if (${_cmi.Value.Equal[out]}) {
      /varset _out ${_out} \aw${_cmi.Key}\ax

    | no variables just an option
    } else /if (${_cmi.Value.Arg[2,|].Equal[show]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:${num}${${_map}.Find[${_value}].Value}\ax

    | string in brackets w/o collections
    } else /if (${_cmi.Value.Left[2].Equal[st]} && ${_cmi.Value.Arg[2,|].Equal[br_no_c]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:[${If[${Bool[${${_value}}]},${num}${${_value}}\ax,${off}]}]
    
    | string in brackets
    } else /if (${_cmi.Value.Left[2].Equal[st]} && ${_cmi.Value.Arg[2,|].Equal[br]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:[${If[${Bool[${${_map}.Find[${_value}].Value}]},${num}${${_map}.Find[${_value}].Value}\ax,${off}]}]

    | string in persentasies
    } else /if (${_cmi.Value.Left[2].Equal[st]} && ${_cmi.Value.Arg[2,|].Equal[pr]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:(${If[${Bool[${${_map}.Find[${_value}].Value}]},\ag${${_map}.Find[${_value}].Value}\ax,${r0}]})

    | straight string
    } else /if (${_cmi.Value.Left[2].Equal[st]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:${If[${Bool[${${_map}.Find[${_value}].Value}]},${num}${${_map}.Find[${_value}].Value}\ax,${r0}]}

    | switch in persentasies
    } else /if (${_cmi.Value.Left[2].Equal[sw]} && ${_cmi.Value.Arg[2,|].Equal[pr]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:(${If[${${_map}.Find[${_value}].Value},${on},${off}]})

    | straight switch (optional submenu)
    } else /if (${_cmi.Value.Left[2].Equal[sw]}) {
      /varset _out ${_out} ${info}${_cmi.Key}\ax:${If[${${_map}.Find[${_value}].Value},${on},${off}]}${If[${_cmi.Value.Arg[2,|].Equal[sm]},${submenu},]}
    }

    /invoke ${_cmi.Advance}
    /if (!${_cmi.IsEnd}) {
      /varset _out ${_out} ${dot}
    }
  }
  /invoke ${out[12, "/${_outname} ${dot} ${_out}"]}
  SETHUD

/return



|***
 * note: checks/corrects a/the target
 * note: returns FALSE if a target is dead or does not exist, returns TRUE otherwise
 * use: /call target [isdead|clear|set|release,reztarget] [spawnID]
 * use: ${target[isdead|clear|set|release|reztarget, spawnID]}
 ***|
sub target(string _type, spawn _spawn)

  /declare _debug bool local FALSE

  /if (${maDebug.Find[target].Value}) {
    DEBUG \attarget\ax(\a-w${_type}, ${_spawn.ID}\ax)
  }

  | set a default target id if none passed
  /if (!${_spawn.ID} && ${Bool[${maCC.Find[stAssistMode].Value}]} && !${Select[${_type},clear]} && !${maCC.Find[swSetForced].Value}) {
    /varset _spawn TARGET
  }

  | clear the current target and related info, nothing more
  /if (${Select[${_type},release]}) {
    DEBUG ${sep}\a-y${_type}\ax${sep}

    /varset stAssTarID 0
    /varset targetData 0
    /varset assistData 0
    /invoke ${maCC.Add[swValidAssTarID,FALSE]}
    /invoke ${maCC.Add[swSetCombat,FALSE]}
    /invoke ${maCC.Add[swSetForced,FALSE]}
    /invoke ${set_data_timer[FALSE, Check_Behind, 0]}
    /invoke ${maData.Add[stFacingPositionCheck,FALSE]}
    /invoke ${maData.Add[stPctEngageSet,FALSE]}
    /invoke ${maData.Add[stPctEngagePetSet,FALSE]}
    /invoke ${maData.Add[stPctEngageSwarmSet,FALSE]}
    /invoke ${maData.Add[stPctEngageBurnSet,FALSE]}
    /squelch /target clear
    SETHUD
    /return TRUE
  }

  /declare _isdead bool local FALSE

  | set a target for rezing
  /if (${Select[${_type},reztarget]}) {

    /if (${Target.ID} == ${_spawn.ID}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}good
      /return TRUE
    }

    DEBUG ${sep}\a-y${_type}\ax${sep}setting
    /squelch /target id ${_spawn.ID}
    /delay 1s ${Target.ID} == ${_spawn.ID}

    /if ((${Target.ID} == ${_spawn.ID})) {
      DEBUG ${sep}\a-y${_type}\ax${sep}isset
      /return TRUE
    }

  }

  | set a target
  /if (${Select[${_type},set]}) {

    /if (${_spawn.ID} && ${_spawn.Type.NotEqual[Corpse]}) {

      /if (${Target.ID} == ${_spawn.ID}) {
        DEBUG ${sep}\a-y${_type}\ax${sep}good
        /return TRUE
      }

      DEBUG ${sep}\a-y${_type}\ax${sep}setting
      /squelch /target id ${_spawn.ID}
      /delay 1s ${Target.ID} == ${_spawn.ID}

      /if ((${Target.ID} == ${_spawn.ID}) && !${_isdead}) {
        DEBUG ${sep}\a-y${_type}\ax${sep}isset
        /return TRUE
      }
    } else /if (!${_spawn.ID} || ${_spawn.Type.Equal[Corpse]}) {
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
      /if (!${_spawn.ID} && ${Target.ID}) {
        DEBUG ${sep}!auto !target.id clear_combat
        /varset _spawn ${Target.ID}
      }
      /if (!${_spawn.ID}) {
        DEBUG ${sep}!auto !_spawn clear_combat
        /varset _isdead TRUE
      }
    }

    | reasons to consider something dead
    /if (${_spawn.ID} == 0 || !${_spawn.ID}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}!_spawn.ID
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
    } else /if (${_spawn.Dead}) {
      DEBUG ${sep}\a-y${_type}\ax${sep}spawn-dead
      /varset _isdead TRUE
    } 

    /if (!${_isdead}) {
      /return FALSE
    }

  }

  | clear/reset all combat
  /if (${Select[${_type},clear]} || ${_isdead}) {
    DEBUG ${sep}\a-y${_type}\ax${sep}

    /varset stAssTarID 0
    /varset targetData 0
    /varset assistData 0
    /invoke ${maCC.Add[swValidAssTarID,FALSE]}
    /invoke ${maCC.Add[swSetCombat,FALSE]}
    /invoke ${maCC.Add[swSetForced,FALSE]}
    /invoke ${set_data_timer[FALSE, Check_Behind, 0]}
    /invoke ${maData.Add[stFacingPositionCheck,FALSE]}
    /invoke ${maData.Add[stPctEngageSet,FALSE]}
    /invoke ${maData.Add[stPctEngagePetSet,FALSE]}
    /invoke ${maData.Add[stPctEngageSwarmSet,FALSE]}
    /invoke ${maData.Add[stPctEngageBurnSet,FALSE]}
    SETHUD
    
    /if (${Me.AutoFire}) {
      /squelch /autofire
    }

    /if (${Me.Combat}) {
      /squelch /attack off
    }

    /if (!BARD) {
      /if (AUTO && ${Me.Casting.ID}) {
        /if (!${Select[${Spell[${Me.Casting.ID}].Category},Transport,Heals]}) {
          /stopcast
        }
      }
    }
    /if (!${maOver.Find[swTargetClear].Value}) {
      /squelch /target clear
    }

    /return TRUE
  }

/return NULL



|***
 * note: attraction pulls for tanks
 * use: /call pull_mob_attraction DEBUG [spawnID] [FORCE T/F]
 ***|
sub pull_mob_attraction(bool _debug, spawn _spawn, bool _force)
  DEBUG \atpull_mob_attraction\ax(\a-w${_spawn.ID}, ${_force}\ax)

  /if (${_spawn.Distance} > PULLENV) /return FALSE
  /if (${_spawn.Distance} < ${maHard.Find[stMinBowDist].Value}) /return FALSE
  /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
    /return FALSE
  }

  /declare _attraction string local FALSE
  /if (${Select[${Me.Class.ShortName},SHD]}) {
    /varset _attraction Hate's Attraction
  } else /if (${Select[${Me.Class.ShortName},WAR]}) {
    /varset _attraction Warlord's Grasp
  } else /if (${Select[${Me.Class.ShortName},PAL]}) {
    /varset _attraction Divine Call
  } else /if (${Select[${Me.Class.ShortName},MNK]}) {
    /varset _attraction Moving Mountains
  }

  DEBUG ${sep}${Me.Class.ShortName} _attraction${sep}\a-w${_attraction}\ax

  /if (!${cast_data[${_debug}, "${_attraction}"]}) {
    /return FALSE
  }

  /call cast ${_debug} ${_spawn.ID} FALSE PULLENV

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
sub cast_cycle_defense(bool _debug, spawn _spawn)
  DEBUG \atcast_cycle_defense\ax(\a-w${_spawn.ID}\ax)

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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE

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
    /if (!${cast_data[${_debug}, "${maDefense.Find[stDefense${_liC20.Value}].Value}"]}) {
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
      /vardata _spawn Me.ID
    } else {
      /varset _spawn 0
    }

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

    /call cast ${_debug} ${_spawn.ID} FALSE

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: clicks stuff
 * use: /call check_item_cycle DEBUG [spawn]
 ***|
sub check_item_cycle(bool _debug, spawn _spawn, string _tag)
  DEBUG \atcheck_item_cycle\ax(\a-w${_spawn.ID}, ${_tag}\ax)

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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT

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

    /call cast_data ${_debug} "${_tmpName}"
    /if (!${Macro.Return}) {
    | /if (!${cast_data[${_debug}, "${_tmpName}"]}) {
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
        /varset _spawn ${Me.ID}
      }
    }

    /if (${FindItem[${_tmpName}].Spell.Duration.TotalSeconds} && ${FindItem[${_tmpName}].Spell.Range}) {
      /call target set ${_spawn.ID}
      /if (${Target.BuffDuration[${_tmpName}].TotalSeconds}) {
        DEBUG ${break}target has buff
        /invoke ${_liC20.Advance}
        /continue
      }
    }

    /if (${target[isdead, ${_spawn.ID}]}) {
      /return FALSE
    }

    /call cast ${_debug} ${_spawn.ID} FALSE

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: casts nukes from maNuke
 * use: /call cast_cycle_nuke DEBUG [spawn] [TAG]
 ***|
sub cast_cycle_nuke(bool _debug, spawn _spawn, string _tag)
  DEBUG \atcast_cycle_nuke\ax(\a-w${_spawn.ID}, ${_tag}\ax)

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
      DEBUG ${sep}doing ae things
      /declare _liae listiterator local
      /vardata _liae lsLineNuke.First.Clone
      /while (!${_liae.IsEnd}) {
        GETOVERHERE

        | check for pbaenuke
        /if (!${Bool[${maNuke.Find[swNuke${_liae.Value}].Value}]} || !${Bool[${maNuke.Find[stNuke${_liae.Value}].Value}]} || !${lsNuke${_liae.Value}Tag.Contains[pbaenuke]}) {
          /invoke ${_liae.Advance}
          /continue
        }

        | if we find an available ae nuke, and it is ready, temp reset the line to just that spell to save the next loop cycle
        /if (${cast_data[${_debug}, "${maNuke.Find[stNuke${_liae.Value}].Value}"]}) {
          /invoke ${lsLineNuke.Clear}
          /invoke ${lsLineNuke.Append[${_liae.Value}]}
          /varset _tag pbaenuke
          /varset _spawn 0
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
      DEBUG ${sep}manna < 2%
      /return FALSE
    }
    
    GETINPUT cast_cycle_nuke NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_nuke
    
    DEBUG ${sep}base nuke checks${sep}\a-wstarted\ax

    /if (${target[isdead, ${_spawn.ID}]}) {
      DEBUG ${sep}Target dead
      /return FALSE
    }

    | stopnuke %
    /if (${maNuke.Find[stPctStopNuke].Value}) {
      /if (${_spawn.PctHPs} <= ${maNuke.Find[stPctStopNuke].Value}) {
        DEBUG ${sep}mob %HP below ${maNuke.Find[stPctStopNuke].Value}
        /return FALSE
      }
    }

    /call target set ${_spawn.ID}

    | check the switch
    /if (!${Bool[${maNuke.Find[swNuke${_liNuke.Value}].Value}]}) {
      DEBUG ${sep}switched off${sep}\a-w${maNuke.Find[stNuke${_liNuke.Value}].Value}\ax
      /invoke ${_liNuke.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maNuke.Find[stNuke${_liNuke.Value}].Value}]}) {
      DEBUG ${sep}no name
      /invoke ${_liNuke.Advance}
      /continue
    }

    | check matching tag for failure
    /if (${Bool[${_tag}]}) {
      /if (!${lsNuke${_liNuke.Value}Tag.Contains[${_tag}]}) {
        DEBUG ${sep}invalid passed _tag
        /invoke ${_liNuke.Advance}
        /continue
      }
    }

    /if (!${cast_data[${_debug}, "${maNuke.Find[stNuke${_liNuke.Value}].Value}"]}) {
      /invoke ${_liNuke.Advance}
      /continue
    }

    | skip twincast nukes (they are accessed a different way)
    /if (${lsNuke${_liNuke.Value}Tag.Contains[twincast]} && ${_tag.NotEqual[twincast]}) {
        DEBUG ${sep}invalid passed _tag (twincast check)
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

    DEBUG ${sep}base nuke checks${sep}\a-wpassed\ax

    /call cast ${_debug} ${_spawn.ID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},OUTOFMANA]}) {
      /break
    }

    | forced delay
    /if (${maNuke.Find[stNukeDelay].Value}) {
      DEBUG ${sep} nuke delay
      /delay 10s !${Me.Casting.ID}
      /delay 2.5s
      /if (${maNuke.Find[swNukeWhackAMole].Value} && !${Me.Sitting} && !${Me.Mount.ID}) {
        DEBUG ${sep}sitting
        /sit
      }
      /invoke ${set_data_timer[${_debug}, Nuke_Delay, ${maNuke.Find[stNukeDelay].Value}]}
      DEBUG ${sep}delaying${sep}\a-w${timer_Nuke_Delay}\ax
      /delay ${timer_Nuke_Delay}
    } else /if (!${maNuke.Find[stNukeDelay].Value}) {
      /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,STUNNED]}) {
        /break
      }
    }

    /invoke ${_liNuke.Advance}
  }

/return TRUE



|***
 * note: casts dots from maDoT
 * use: /call cast_cycle_dot DEBUG [spawnID]
 ***|
sub cast_cycle_dot(bool _debug, spawn _spawn)
  DEBUG \atcast_cycle_dot\ax(\a-wtID:${_spawn.ID}\ax)

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

    SPAWNDEAD

    | stopdot %
    /if (${maDoT.Find[stPctStopDoT].Value}) {
      /if (${_spawn.PctHPs} <= ${maDoT.Find[stPctStopDoT].Value}) {
        DEBUG ${sep}${_spawn.PctHPs} <= ${maDoT.Find[stPctStopDoT].Value}
        /return FALSE
      }
    }
    
    /call cast_data ${_debug} "${maDoT.Find[stDoT${_liC20.Value}].Value}"

    | TTL evaluations
    /if (${maTTL.Find[swTTL].Value}) {
      /if (${maTTL.Contains[${_spawn.DisplayName}]}) {

        DEBUG ${sep}ttl

        | check overall durations of the average kill +2 tics over the spell duration time
        /if (${spellData.Duration} > ${Math.Calc[${maTTL.Find[${_spawn.DisplayName}].Value.Arg[2,|]} + (${maTTL.Find[${_spawn.DisplayName}].Value.Arg[2,|]}*.5)]}) {
          /invoke ${_liC20.Advance}
          /continue
        }
        | if mob is less then 50% hp and spell duration is > 1/2 your average kill time
        /if (${_spawn.PctHPs} < 50 && ${spellData.Duration} > ${Math.Calc[${maTTL.Find[${_spawn.DisplayName}].Value.Arg[2,|]} / 2]}) {
          /invoke ${_liC20.Advance}
          /continue
        }

      }
    }

    GETINPUT cast_cycle_dot NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_dot

    SPAWNDEAD

    /call target set ${_spawn.ID}

    /if (${Target.ID} == ${_spawn.ID}) {
      /if (${Target.BuffDuration[${spellData}].TotalSeconds} >= ${maDoT.Find[stDotRefresh].Value}) {
        DEBUG ${sep}dot refresh fail. target has dot ${Target.BuffDuration[${spellData}].TotalSeconds} >= ${maDoT.Find[stDotRefresh].Value}
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

    /if (!${cast_data[${_debug}, "${maDoT.Find[stDoT${_liC20.Value}].Value}"]}) {
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

    /call cast ${_debug} ${_spawn.ID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
      /break
    }

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: casts skills from maAgro for engine2
 * use: /call cast_cycle_agro DEBUG [spawnIT]
 ***|
sub cast_cycle_agro(bool _debug, spawn _spawn)
  DEBUG \atcast_cycle_agro\ax(\a-w${_spawn.ID}\ax)

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

    SPAWNDEAD

    GETINPUT cast_cycle_agro NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_agro

    /call target set ${_spawn.ID}

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

    /if (!${cast_data[${_debug}, "${maAgro.Find[stAgro${_liC20.Value}].Value}"]}) {
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

    /call cast ${_debug} ${_spawn.ID} FALSE

    /invoke ${_liC20.Advance}
  }

/return TRUE



|***
 * note: mash dps alternative Engine 2/3
 * use: /call cast_cycle_miscdps DEBUG [spawn]
 ***|
sub cast_cycle_miscdps(bool _debug, spawn _spawn)
  DEBUG \atcast_cycle_miscdps\ax(\a-w${_spawn.ID}\ax)


  | if the user has not set the line order, set it to a default for now
  /if (${lsLineMiscdps.Contains[EMPTY]} && ${lsLineMiscdps.Count} == 1) {
    DEBUG ${sep}resetting the empty MiscDPS line
    /invoke ${lsLineMiscdps.Clear}
    /invoke ${lsLineMiscdps.Append[${stCount50}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /declare _liC50 listiterator local
  /vardata _liC50 lsLineMiscdps.First.Clone
  /while (!${_liC50.IsEnd}) {

    SPAWNDEAD

    GETINPUT cast_cycle_miscdps NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_miscdps

    /call target set ${_spawn.ID}

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

    /if (!${cast_data[${_debug}, "${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}"]}) {
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

    /call cast ${_debug} ${_spawn.ID} FALSE

    /invoke ${_liC50.Advance}
  }

/return TRUE



|***
 * note: casts burns from maBurn
 * use: /call cast_cycle_burn DEBUG [spawnID]
 ***|
sub cast_cycle_burn(bool _debug, spawn _spawn)
  DEBUG \atcast_cycle_burn\ax(\a-w${_spawn.ID}\ax)

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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_burn

    SPAWNDEAD

    /call target set ${_spawn.ID}

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

    /call cast_data ${_debug} "${_tmpName}"
    /if (!${Macro.Return}) {

   | /if (!${cast_data[${_debug}, "${_tmpName}"]}) {
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

    /call cast ${_debug} ${_spawn.ID} FALSE

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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
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

  /if (${swIsMeDead}) {
    /return TRUE
  }

  | check death states
  /if (${Me.State.Equal[HOVER]} || ${Me.Hovering} || ${Me.Dead}) {
    /varset swIsMeDead TRUE
  }

  | did I just die and get sent to bind ?
  /if ((${Me.Buff[Resurrection Sickness].ID} || ${Me.Buff[Revival Sickness].ID}) && ${Zone.ID} == ${Me.ZoneBound.ID}) {
    /if (${Me.BoundLocation[0].Y} == ${Me.Y} && ${Me.BoundLocation[0].X} == ${Me.X} && ${Me.BoundLocation[0].Z} == ${Me.Z}) {
      /varset swIsMeDead TRUE
    }
  }

  | GETINPUT is_me_dead swIsMeDead|${swIsMeDead}

  | if we died, shut shit down
  /if (${swIsMeDead}) {

    /doevents REZ_ACCEPT

    | map my death
    /call control_rez ${maDebug.Find[rez].Value} add ${Me.ID} dead
    | /dgzexecute /invoke ${maControlRez.Add[${Me.ID},dead]}

    /varset stAssTarID 0
    /invoke ${maCC.Add[swValidAssTarID,FALSE]}
    /invoke ${maCC.Add[swSetCombat,FALSE]}
    /invoke ${maCC.Add[swSetForced,FALSE]}
    /invoke ${set_data_timer[${_debug}, Check_Behind, 0]}
    /squelch /target clear

    /if (${Me.AutoFire}) {
      /squelch /autofire
    }

    /if (${Me.Combat}) {
      /squelch /attack off
    }    

    /doevents flush

    | stop all the things(s)
    /if (${maHome.Find[swMoveDeathClear].Value}) {
      /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
    }

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
      /if (${cast_data[${_debug}, "${_liic.Value}"]}) {
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
      /if (${cast_data[${_debug}, "${_liooc.Value}"]}) {
        /varset _rez ${_liooc.Value}
        /varset _rezReady TRUE
        /break
      }
      /invoke ${_liooc.Advance}
    }

  }

  /if (${maRez.Find[swRezToken].Value}) {
    /declare _rezEmergency string local "Token of Resurrection"
    /if (${cast_data[${_debug}, "${_rezEmergency}"]}) {
      /varset _tokenReady TRUE
    }
  }

  | still good???
  /if (!${_rezReady} && !${_tokenReady}) {
    DEBUG ${break}fail${sep}\a-wno rez type available\ax
    /return FALSE
  }

  /declare _tmpID int local 0
  /declare _spawn spawn local
  /declare _increment int local 1
  /declare _found bool local FALSE
  /declare _liC8 listiterator local
  /vardata _liC8 lsLineRez.First.Clone

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineRez.Contains[EMPTY]} && ${lsLineRez.Count} == 1) {
    DEBUG ${sep}resetting an empty rez priority list
    /invoke ${lsLineRez.Clear}
    /invoke ${lsLineRez.Append[1|2|3|4|5|6|7|8]}
  }

  | check priorities
  /while (!${_liC8.IsEnd}) {
    | DEBUG ${sep}_liC8.Value${sep}${_liC8.Value}${sep}\a-w${maRez.Find[stRez${_liC8.Value}].Value}\ax
    GETOVERHERE

    | check the switch
    /if (!${maRez.Find[swRez${_liC8.Value}].Value}) {
      DEBUG ${sep}switch off${sep}\a-w${maRez.Find[swRez${_liC8.Value}].Value}\ax
      /invoke ${_liC8.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maRez.Find[stRez${_liC8.Value}].Value}]}) {
      DEBUG ${sep}no name${sep}\a-w${maRez.Find[swRez${_liC8.Value}].Value}\ax
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
        | /vardata _spawn NearestSpawn[${_increment}, pccorpse ${maRez.Find[stRez${_liC8.Value}].Value} radius ${maRez.Find[stMaxRezRange].Value}]
        /varset _tmpID ${NearestSpawn[${_increment}, ${maRez.Find[stRez${_liC8.Value}].Value} radius ${maRez.Find[stMaxRezRange].Value}].ID}
        /vardata _spawn Spawn[${_tmpID}]
        
        DEBUG ${sep}_spawn${sep}\a-w${NearestSpawn[${_increment}, ${maRez.Find[stRez${_liC8.Value}].Value} radius ${maRez.Find[stMaxRezRange].Value}].ID}\ax (\a-r${maRez.Find[stRez${_liC8.Value}].Value}\ax)

        | theres an error, get out
        /if (!${_spawn.ID}) {
          /break
        }

        | fix any timer
        /if (!${Defined[timer_corpse_${_spawn.ID}]}) {
          /declare timer_corpse_${_spawn.ID} timer outer 1
          /delay 1
        }

        | for the people that suck ass (not in a good way)
        /if (${lsNoRezToon.Contains[${_spawn.DisplayName}]}) {
          /varset timer_corpse_${_spawn.ID} ${maHard.Find[stRezIgnoreFucktards].Value}
          /varset _spawn 0
        }

        | if there is no id, or the current corpse has a marker on it, move on
        /if (!${_spawn.ID} || ${timer_corpse_${_spawn.ID}}) {
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
      /if (${maRez.Find[swDivineCleric].Value} && ${Select[${_spawn.Class.ShortName},CLR]}) {
        /if (${Me.AltAbilityReady[${maRez.Find[stAADivineResurrection].Value}]} || ${Me.ItemReady[${maRez.Find[stAADivineResurrection].Value}]}) {
          /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${maRez.Find[stAADivineResurrection].Value}"
          /return TRUE
        }
      | normal rez
      } else {
        /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${_rez}"
        /return TRUE
      }

    | other rez classes
    } else /if (${Select[${Me.Class.ShortName},DRU,PAL,SHM,NEC]}) {
      /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${_rez}"
      /return TRUE

    | token rezing
    } else /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${_spawn.Class.ShortName}]}) {
      /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${_rezEmergency}"
      /return TRUE
    }

    /invoke ${_liC8.Advance}
  }

  | end of priority rezing

  | dannet members
  /if (${maRez.Find[swRezDanNet].Value}) {
    DEBUG ${sep}swRezDanNet rez
    /declare _countD int local 0
    /for _countD 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}

      /if (!${Spawn[${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}].Dead}) {
        DEBUG ${sep}${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} dead${sep}\a-w${Spawn[${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}].Dead}\ax
        /continue
      } else /if (${Spawn[${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}].Dead}) {
        /vardata _spawn Spawn[pccorpse ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countD},|]} radius ${maRez.Find[stMaxRezRange].Value}]
      }

      | mark the toon if it isnt already
      /if (!${Defined[timer_corpse_${_spawn.ID}]}) {
        DEBUG ${sep}timer_corpse_${_spawn.ID}
        /declare timer_corpse_${_spawn.ID} timer outer 0
        /delay 1
      } else /if (${timer_corpse_${_spawn.ID}}) {
      DEBUG ${sep}timer_corpse_${_spawn.ID} == ${timer_corpse_${_spawn.ID}}
        /continue
      }

      /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${_spawn.Class.ShortName}]}) {
        /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${_rezEmergency}"
        /break
      } else {
        /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${_rez}"
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
      /vardata _spawn NearestSpawn[${_countE}, pccorpse radius ${maRez.Find[stMaxRezRange].Value}]

      /if (!${Defined[timer_corpse_${_spawn.ID}]}) {
        DEBUG ${sep}timer_corpse_${_spawn.ID}
        /declare timer_corpse_${_spawn.ID} timer outer 0
        /delay 1
      } else /if (${timer_corpse_${_spawn.ID}}) {
        /continue
      }

      /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${_spawn.Class.ShortName}]}) {
        /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${_rezEmergency}"
        /break
      } else {
        /call cast_rez ${maDebug.Find[rez].Value} ${_spawn.ID} "${_rez}"
        /break
      }
      /next _countE
    }
  }

  SETHUD

/return TRUE



|***
 * note: casts trez thingies and sets any needed timers for dead fuckers
 * use: /call cast_rez DEBUG [ID] [rez aa|token]
 ***|
sub cast_rez(bool _debug, spawn _spawn, string _rez)
  DEBUG \atcast_rez\ax(\a-w${_spawn.ID}, ${_rez}\ax)

  /if (${Me.Dead}) {
    DEBUG ${break}i'm dead.. wtf..
    /return FALSE
  }
  /if (!${Defined[timer_corpse_${_spawn.ID}]}) {
    DEBUG ${break}no timer for${sep}\a-wtimer_corpse_${_spawn.ID}\ax
    /return FALSE
  }
  /if (${timer_corpse_${_spawn.ID}}) {
    DEBUG ${break}timer not empty${sep}\a-wtimer_corpse_${_spawn.ID}\ax
    /return FALSE
  }

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT cast_rez _rez|${_rez}

  SETHUD "resurrect"

  | ignore the people we dont like
  /if (${lsNoRezToon.Contains[${_spawn.DisplayName}]}) {
    DEBUG ${break}on go fuck yourself list${sep}\a-w${_spawn.DisplayName}]}\ax
    /invoke ${set_timer[${_debug}, timer_corpse_${_spawn.ID}, ${maHard.Find[stRezIgnoreFucktards].Value}]}
    /return FALSE
  }

  | get the corpse close to us
  /if (${_spawn.Distance} <= ${maHard.Find[stRezSummonRange].Value}) {
    DEBUG ${sep}/corpse${sep}\a-wid ${_spawn.ID}\ax
    /call target reztarget ${_spawn.ID}
    /delay 1
    /squelch /corpse
  }

  | can we AA res or token, maybe spell?
  /if (${Me.AltAbilityReady[${_rez}]} || (${Me.ItemReady[Token of Resurrection]} && ${maRez.Find[swRezToken].Value}) || ${Me.SpellReady[${_rez}]} || ${Me.ItemReady[${_rez}]}) {

    | not token rez rez
    /if (!${maRez.Find[swRezToken].Value}) {
      DEBUG ${sep}casting${sep}\a-w${_rez}\ax
      /delay 5s !${Me.Casting.ID}
      /call target reztarget ${_spawn.ID}
      /call cast_data ${_debug} "${_rez}"
      /call cast ${_debug} ${_spawn.ID} skiptargetcheck
      /delay 5
      /delay 5s !${Me.Casting.ID}

    | token rez
    } else /if (${maRez.Find[swRezToken].Value}) {
      DEBUG ${sep}casting${sep}\a-wclicking token\ax
      /delay 5s !${Me.Casting.ID}
      /call target reztarget ${_spawn.ID}
      /call cast_data ${_debug} "Token of Resurrection"
      /call cast ${_debug} ${_spawn.ID} skiptargetcheck
      /delay 5
      /delay 5s !${Me.Casting.ID}

    }

    /doevents

    | if cast was good, set the timers.
    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
      /invoke ${set_timer[${_debug}, timer_corpse_${_spawn.ID}, ${maHard.Find[stRezSuccess].Value}]}
      /invoke ${out[0, "Rez Success${sep}${con_color[${_debug}, ${_spawn.ID}]}"]}
    } else {
      /if (${Select[${Me.Class.ShortName},CLR,DRU,SHM,PAL,NEC]}) {
        /invoke ${set_timer[${_debug}, timer_corpse_${_spawn.ID}, ${maHard.Find[stRezFail].Value}]}
      } else {
        /invoke ${set_timer[${_debug}, timer_corpse_${_spawn.ID}, ${maHard.Find[stRezFailToken].Value}]}
      }
    }

  }

  SETHUD

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

/return TRUE



|***
 * note: watches for spawns in the zone
 * use:
 ***|
sub check_watch_spawn(bool _debug)
  DEBUG \atcheck_watch_spawn\ax()

  /declare _liwa listiterator local
  /vardata _liwa lsWatch.First.Clone
  /varset stWatchCount 0

  /while (!${_liwa.IsEnd}) {
    /squelch /mapshow ${_liwa.Value}
    /varcalc stWatchCount ${stWatchCount} + ${SpawnCount[${_liwa.Value}]}
    /invoke ${_liwa.Advance}
  }
  
  /if (${stWatchCount} <= ${stWatchCountChange}) {
    /varset stWatchCountChange ${stWatchCount}
  } else /if (${stWatchCount} > ${stWatchCountChange}) {
    /varset stWatchCountChange ${stWatchCount}
    /if (${maWatch.Find[swWatchBeep].Value}) {
      /beep
    }
  }

  /invoke ${set_data_timer[${_debug}, Check_Watch, RESTART]}

/return TRUE



|***
 * note: regeneration spells (not the AAs)
 * use: /call cast_cycle_heal_regen DEBUG [healgrouphot|healhot]
 ***|
sub cast_cycle_heal_regen(bool _debug, string _tag)
  DEBUG \atcast_cycle_heal_regen\ax(\a-w${_tag}\ax)

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
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      GETOVERHERE
      
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
      /if (!${cast_data[${_debug}, "${_myHoT}"]}) {
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
        /if (!${Spell[${Spell[${_myHoT}].Base2[${_count}]}].WillLand}) /varset _found TRUE
        /if (${_found}) {
          /break
        }
      /next _count
    } else {
      /if (!${Me.Song[${_myHoT}].ID}) /varset _found FALSE
      /if (${Spell[${_myHoT}].WillLand}) /varset _found FALSE
    }
    /call cast ${_debug} 0 FALSE
    /return TRUE

  | single heal over time
  } else /if (${_tag.Equal[hot]}) {

    | find my hot
    /while (!${_liHoT.IsEnd}) {
      
      GETOVERHERE
      
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

      | ready?.. or continue
      /if (!${cast_data[${_debug}, "${_myHoT}"]}) {
        DEBUG ${break}notready
        /varset _myHoT FALSE
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

    /dquery ${hurtData.DisplayName} -q "Spell[${Spell[${_myHoT}].RankName}].WillLand" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG DNQReturn.WillLand${sep}\a-w${DNQReturn}\ax (\a-r${hurtData.DisplayName}\ax)
    /dquery ${hurtData.DisplayName} -q "Me.Song[${Spell[${_myHoT}].RankName}].Duration" -o DNQReturn2 -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG DNQReturn2.Duration${sep}\a-w${DNQReturn2}\ax (\a-r${hurtData.DisplayName}\ax)
    /if (${DNQReturn} && !${DNQReturn2}) {
      /call cast ${_debug} ${hurtData.ID} FALSE
    }

    /return TRUE

  }

/return FALSE



|***
 * note: casts heals from maHeal
 * use: /call cast_cycle_heal DEBUG [spawnID] [tag]
 ***|
sub cast_cycle_heal(bool _debug, spawn _spawn, string _tag)
  DEBUG \atcast_cycle_heal\ax(\a-w${_spawn.ID}, ${_tag}\ax)

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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
    /if (!${lsHeal${_liHeal.Value}Tag.Contains[${_tag}]}) {
      DEBUG ${break}no tag${sep}\a-w${_tag}\ax
      /invoke ${_liHeal.Advance}
      /continue
    }

    | check matching tag pass
    /if (${lsHeal${_liHeal.Value}Tag.Contains[${_tag}]}) {
      /varset _myHeal ${maHeal.Find[stHeal${_liHeal.Value}].Value}
    }

    | ready?
    /if (!${cast_data[${_debug}, "${_myHeal}"]}) {
      /varset _myHeal FALSE
      /invoke ${_liHeal.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Heal ${_liHeal.Value} ${Me.DisplayName} "${_myHeal}" cast_cycle_heal
    /if (!${Macro.Return}) {
      /invoke ${_liHeal.Advance}
      /continue
    }

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    /if (${_tag.Equal[ignoremovement]}) {
      /call cast ${_debug} ${_spawn.ID} ignoremovement
    } else {
      /call cast ${_debug} ${_spawn.ID} breakonhealpct
    }

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
      /break
    }

    /invoke ${_liHeal.Advance}
  }

/return TRUE



|***
 * note: cast promise heals from maHeal on your assist
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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
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
    /if (!${cast_data[${_debug}, "${_myHeal}"]}) {
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
    /dquery ${_tmpName} -q "Spell[${_spell}].WillLand" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
    DEBUG ${sep}${sep}WillLand: .WillLand \at${DNQReturn}\ax ${dot} ${_spell}
    /if (!${DNQReturn}) /continue

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

    /delay 1
    /call cast ${_debug} ${assistData.ID} FALSE

    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,OUTOFMANA,STUNNED]}) {
      /break
    }

    /invoke ${_liHeal.Advance}
  }

/return TRUE



|***
 * note: moves you behind a target
 * use: /call moveto_behind DEBUG spawnID
 ***|
sub moveto_behind(bool _debug, spawn _spawn)
  DEBUG \atmoveto_behind\ax()
  /declare _dist float local ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
  /declare _tmpY float local ${Math.Calc[${_spawn.Y}-${Math.Calc[(${_dist}*${Math.Cos[${_spawn.Heading.DegreesCCW}]}]}]}
  /declare _tmpX float local ${Math.Calc[${_spawn.X}-${Math.Calc[(${_dist}*${Math.Sin[${_spawn.Heading.DegreesCCW}]}]}]}
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
 * use: /call check_buff_cycle_aura DEBUG [spawn]
 ***|
sub check_buff_cycle_aura(bool _debug)
  DEBUG \atcheck_buff_cycle_aura\ax()

  SETHUD "buff aura"
  
  /declare _liBuff listiterator local
  /vardata _liBuff lsCount50.First.Clone
  /while (!${_liBuff.IsEnd}) {
    GETINPUT check_buff_cycle_aura NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
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
      /invoke ${maData.Add[tmpCastName,${Macro.Return}]}
      
    | get item clickie name
    } else /if (${FindItem[=${maBuff.Find[stBuff${_liBuff}].Value}].ID}) {
      DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_liBuff}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_liBuff}].Value}].Clicky.SpellID}].Name}\ax
      /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_liBuff}].Value}].Clicky.SpellID}].Name}]}
      /invoke ${maData.Add[tmpCastName,${maBuff.Find[stBuff${_liBuff}].Value}]}

    | stick with buff name
    } else {
      DEBUG ${sep}defaultname${sep}${maBuff.Find[stBuff${_liBuff}].Value}\ax
      /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_liBuff}].Value}]}
      /invoke ${maData.Add[tmpCastName,${maBuff.Find[stBuff${_liBuff}].Value}]}
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
    
    /call cast_data ${_debug} "${maData.Find[tmpCastName].Value}"

    /delay 5
    /call cast ${_debug} 0 FALSE
    /call cast_cooldown_wait ${_debug}

    /invoke ${_liBuff.Advance}
  }
  
  SETHUD EMPTH

/return TRUE



|***
 * note: check user input for valid stuff
 * use: /call validate_input DEBUG [TYPE] [check]
 ***|
sub validate_input(bool _debug, string _type, string _verbage)
  DEBUG \at\ax(\a-w${_type}, ${_verbage}\ax)

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
  
  /if (${maEnv.Find[swAlwaysDropInvisCauseThePeopleIGroupWithAreAssHats].Value}) {
    /call Bind_control_nomore invis
  }

  /if (${lsDropBuffs.Contains[EMPTY]} && ${lsDropBuffs.Count} == 1) {
    /return FALSE
  }

  /declare _lidb listiterator local
  /vardata _lidb lsDropBuffs.First.Clone
  /while (!${_lidb.IsEnd}) {

    /if (${Me.Buff[${_lidb.Value}].ID} || ${Me.Song[${_lidb.Value}].ID})  {
      OUT Drop${sep}${info}${_lidb.Value}\ax
      /invoke ${Me.Buff[${_lidb.Value}].Remove}
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

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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
      /invoke ${maData.Add[tmpCastName,${Macro.Return}]}

    | get item clickie name
    } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
      DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
      /invoke ${maData.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
      /invoke ${maData.Add[tmpCastName,${maBuff.Find[stBuff${_count}].Value}]}

    | stick with buff name
    } else {
      /invoke ${maData.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
      /invoke ${maData.Add[tmpCastName,${maBuff.Find[stBuff${_count}].Value}]}
    }

    | cycle the tags
    | /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${Me.DisplayName} "${maData.Find[tmpName].Value}" check_buff_cycle_self
    /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${Me.DisplayName} "${maData.Find[tmpName].Value}" check_buff_cycle_self
    /if (!${Macro.Return}) {
      /continue
    }

    /varset _found TRUE
    DEBUG ${sep}_count${sep}\a-w${_count}\ax
    

    | trigger swap AAs... fuck DBG and their shit..
    /if (${Spell[${maData.Find[tmpName].Value}].NumEffects} == 1 && ${Me.AltAbility[${maData.Find[tmpName].Value}]})  {
      DEBUG ${sep}\arSWAP\ax to trigger spell\aw==\ax\a-w${Spell[${maData.Find[tmpName].Value}].Trigger[1]}\ax
      /invoke ${maData.Add[tmpName,${Spell[${Spell[${maData.Find[tmpName].Value}].Trigger[1]}].RankName}]}
    }

    | stacking / exist ?
    /if (${Spell[${maData.Find[tmpName].Value}].HasSPA[374]} || ${Spell[${maData.Find[tmpName].Value}].HasSPA[340]} || ${Spell[${maData.Find[tmpName].Value}].HasSPA[470]}) {
      DEBUG ${sep}\aw470\ax${sep}\a-w${Spell[${maData.Find[tmpName].Value}].HasSPA[470]}\ax ${dot} \aw374\ax${sep}\a-w${Spell[${maData.Find[tmpName].Value}].HasSPA[374]}\ax ${dot} \aw340\ax${sep}\a-w${Spell[${maData.Find[tmpName].Value}].HasSPA[340]}\ax

      /for _i 1 to ${Spell[${maData.Find[tmpName].Value}].NumEffects}
        DEBUG ${sep}\a-t${_i}/${Spell[${maData.Find[tmpName].Value}].NumEffects}\ax \aw::\ax ${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name} \aw::\ax ${Me.Buff[${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name}].ID} \aw::\ax Duration(\a-w${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Duration.TotalSeconds}s\ax)

        | SDEBUG ${sep}${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name} \aw::\ax slot-${Me.Buff[${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name}].ID}



        | SPA has no duration, skip it
        /if (!${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Duration}) {
          DEBUG ${break}\aw.Duration\ax break (${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name})
          /continue
        }

        | Does the SPA stack? if not, move on
        /if (!${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].WillLand}) {
          DEBUG ${break}\aw.WillLand\ax break (${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name})
          /continue
        }

        | blocked?
        /if (${lsBlockedBuffsMe.Find[${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].ID}]}) {
          DEBUG ${break}\aw.Blocked\ax break (${Spell[${Spell[${maData.Find[tmpName].Value}].Base2[${_i}]}].Name})
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
        /if (!${cast_data[${_debug}, "${maData.Find[tmpName].Value}"]}) {
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
      /call Bind_control_nomore invis ${_debug}
    }
    
    /call cast_data ${_debug} "${maData.Find[tmpCastName].Value}"
    
    /call cast ${_debug} ${Me.ID} FALSE
    /call cast_cooldown_wait ${_debug}

    /if (${lsBuff${_count}Tag.Contains[autoinventory]}) {
      /delay 2s ${Cursor.ID}
      /autoinventory
    }

    | drop illusions/mounts
    /if (${lsBuff${_count}Tag.Contains[drop]}) {
      DEBUG ${sep}tag-\a-ydrop\ax
      /delay 5s !${Me.Casting.ID}

      | mounts
      /if (${FindItem[${maBuff.Find[stBuff${_count}].Value}].Type.Equal[Mount]} && !${maBuff.Find[swBuffKeepMount].Value}) {
        /if (${Me.Buff[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].ID}) {
          /invoke ${out[0, "Removing${sep}\a-w${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}\ax"]}
          /invoke ${Me.Buff[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}].Remove}
          /delay 5
        }
      }

      | illusions
      /if (${FindItem[${maBuff.Find[stBuff${_count}].Value}].Type.Equal[Illusion]}) {
        DEBUG ${sep} removing illusion ${maBuff.Find[stBuff${_count}].Value}

        /if (${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.NumEffects} == 1) {
          /invoke ${out[0, "Removing${sep}\a-w${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell}\ax"]}
          /invoke ${Me.Buff[${FindItem[Fabled Ball of Golem Clay].Spell}].Remove}
          /delay 5
 
        } else /for _countD 1 to ${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.NumEffects}
          /if (${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.Trigger[${_countD}].Name.Find[Illusion:]}) {
              /if (${Me.Buff[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.Trigger[${_countD}]}].ID}) {
                /invoke ${out[0, "Removing${sep}\a-w${Me.Buff[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.Trigger[${_countD}]}]}\ax"]}
                /invoke ${Me.Buff[${FindItem[${maBuff.Find[stBuff${_count}].Value}].Spell.Trigger[${_countD}]}].Remove}
                /delay 5
              }
            }
        /next _countD
      }
        
    }

  /next _count

  SETHUD

/return TRUE




|***
 * note: checks tags for stuff and things..
 * use: /call tag_evaluation type, ##, toonname, spellname, calling routine
 ***|
sub tag_evaluation(bool _debug, string _type, int _num, string _name, spell _spell, string _sub)
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
        DEBUG ${skip}evaluation${sep} (\a-rq${_type}_${_litag.Value}\ax)
        | do jack all here... just ignore the shit

      | name marker
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Equal[marker name]}]}) {
        /if (${_name.NotEqual[${_litag.Value}]}) {
          /invoke ${_litag.Advance}
          /continue
        }

      | execute sub here
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Find[SUB::]}]}) {
        DEBUG ${sep}releasing to sub${sep} (\a-rq${_type}_${_litag.Value}\ax)
        
        /call ${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Arg[2,::]}]}
        /if (!${Macro.Return}) {
          /return FALSE
        } 

      | remove buff tag
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Find[DROPBUFF-]}]}) {
        /invoke ${Me.Buff[${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Arg[2,-]}].Remove}

      | action tags here
      } else /if (${Parse[99,${sqlite.Result[q${_type}_${_litag.Value} 1 eval].Find[ACTION::]}]}) {
        DEBUG ${sep}doing a thing for tag${sep} (\a-rq${_type}_${_litag.Value}\ax)

        | delay1s
        /if (${_litag.Value.Equal[delay1s]}) {
          DEBUG ${sep}tag${sep}\a-wdelay1s\ax
          /delay 1s
        }

        | delay until gems are not in cooldown
        /if (${_litag.Value.Equal[delaycooldown]}) {
          DEBUG ${sep}tag${sep}\a-wdelaycooldown\ax
          /delay 3s !${Me.SpellInCooldown} 
        }

        | dropdisc
        /if (${_litag.Value.Equal[dropdisc]}) {
          /if (${Me.ActiveDisc.ID} && ${Me.CombatAbilityReady[${_spell.RankName}]}) {
            DEBUG ${sep}tag${sep}\a-wdropdisc\ax (\a-r${Me.ActiveDisc.ID}\ax)
            /stopdisc
            /delay 5
          }
        }
        
        | dropinvis
        /if (${_litag.Value.Equal[dropinvis]} && ${Me.Invis}) {
          DEBUG ${sep}tag${sep}\a-wdropinvis\ax
          /call Bind_control_nomore invis
          /delay 5
        }

        | autoinventory
        /if (${_litag.Value.Equal[autoinventory]}) {
          /call check_cursor ${maDebug.Find[loot].Value} CLEAR
        }

 
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
            DEBUG ${break}evaluation${sep}\arFAIL\ax (\a-rq${_type}_${_litag.Value}\ax) 
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

  /if (${_type.Equal[see]}) {
    /call Bind_command_see ma${_chk}
    /return TRUE
  }

  | check specific #
  /if (${Range.Between[1,${_loopnum}:${Int[${_type}]}]}) {

    | on/off switch
    /if ((!${Bool[${_verbage}]} || ${Select[${_verbage},on,off]}) && ${_verbage.NotEqual[FALSE]}) {
      /invoke ${set_switch_env[${_debug}, sw${_chk}${_type}, TRUE, ${_verbage}]}
      /if (${ma${_chk}.Find[st${_chk}${_type}].Value.Count[|]}) {
        /call get_best_in_spell ${_debug} "${ma${_chk}.Find[st${_chk}${_type}].Value.Arg[1,|]}" "${ma${_chk}.Find[st${_chk}${_type}].Value.Arg[2,|]}" "${ma${_chk}.Find[st${_chk}${_type}].Value.Arg[3,|]}"
        /varset _tmpName "${Macro.Return}"
      }      
      /invoke ${out[12, "\aw${_chk}${_type}\ax${sep}${If[${ma${_chk}.Find[sw${_chk}${_type}].Value},${on},${off}]}${sep}\a-w${ma${_chk}.Find[st${_chk}${_type}].Value}\ax ${If[${Macro.Return.Length} >5,(\a-r${_tmpName}\ax),]}"]}
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

  /if (!${Me.SpellInCooldown}) {
    /return TRUE
  }

  /if (!${maEntropy.Find[swBuffCooldownWait].Value}) {
    /return TRUE
  }

  /while (${Me.SpellInCooldown}) {
    CHECKEXIT
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
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

  /if (!${cast_data[${_debug}, "${maChr.Find[stInvisSingle].Value}"]}) {
    /return FALSE
  }

  /if (!${Spell[${maChr.Find[stInvisSingle].Value}].MyCastTime}) {
    /call cast ${_debug} 0 ignoremovement
  } else {
    /call set_stop_movement ${_debug}
    /call cast ${_debug} 0 FALSE
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
  DEBUG \atset_control_shared\ax(\a-wt:${_type}, v:"${_verbage}", v2:"${_verbage2}"\ax)

  | output control map
  /if (${_type.Equal[see]}) {
    /call Bind_command_see maChr ${_verbage}
    /return SKIP

  } else /if (${_type.Equal[list]}) {
    OUT Base Order${sep}\a-wlsOrderClassBase\ax
    /call out_list lsOrderClassBase
    OUT Pre Order${sep}\a-wlsOrderClassPre\ax
    /call out_list lsOrderClassPre
    OUT Main Order${sep}\a-wlsOrderClass\ax
    /call out_list lsOrderClass
    OUT Post Order${sep}\a-wlsOrderClassPost\ax
    /call out_list lsOrderClassPost
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
sub con_color(bool _debug, spawn _spawn)
  DEBUG \atcon_color\ax(\a-w${_spawn.ID}\ax)

  /if (!${maEntropy.Find[swUseConColor].Value}) {
    /return ${tar}${_spawn.DisplayName}\ax
  } else /if (${_spawn.ConColor.Equal[RED]}) {
    /return \a#FF0000${_spawn.DisplayName}\ax
  } else /if (${_spawn.ConColor.Equal[YELLOW]}) {
    /return \a#FFFF00${_spawn.DisplayName}\ax
  } else /if (${_spawn.ConColor.Equal[WHITE]}) {
    /return \a#FFFFFF${_spawn.DisplayName}\ax
  } else /if (${_spawn.ConColor.Equal[BLUE]}) {
    /return \a#0000FF${_spawn.DisplayName}\ax
  } else /if (${_spawn.ConColor.Equal[LIGHT BLUE]}) {
    /return \a#4FC3F7${_spawn.DisplayName}\ax
  } else /if (${_spawn.ConColor.Equal[GREEN]}) {
    /return \a#008000${_spawn.DisplayName}\ax
  } else /if (${_spawn.ConColor.Equal[GREY]}) {
    /return \a#808080${_spawn.DisplayName}\ax
  }

/return \arError\ax${sep}con_colorID



|***
 * note: taunts a mob, but only if needed
 * use: ${do_taunt[DEBUG, ID]}
 ***|
sub do_taunt(bool _debug, spawn _spawn)
  DEBUG \atdo_taunt\ax(\a-w${_spawn.ID}\ax)

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
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

  /if (${_spawn.Distance} > ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
    /return FALSE
  }

  /invoke ${out[8, "Melee${sep}${sp}Taunt\ax ${dot} ${con_color[${_debug}, ${_spawn.ID}]}\ax"]}
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

  /if (!${sqlite.Rows[qBestSpell]}) {
    DEBUG ${break}
    /return FALSE
  }

  /for _count 1 to ${maHard.Find[stSpellBookSize].Value}
    
    /if (!${Me.Book[${_count}].ID}) {
      | DEBUG ${break}Me.Book[${_count}].ID
      /continue
    }
    /if (${Me.Book[${_count}].Subcategory.NotEqual[${_subcat}]}) {
      | DEBUG ${break}Me.Book[${_count}].Subcategory.NotEqual[${_subcat}]
      /continue
    }
    
    /if (${Me.Book[${_count}].TargetType.NotEqual[${_targettype}]}) {
      | DEBUG ${break}Me.Book[${_count}].TargetType.NotEqual[${_targettype}]
      /continue
    }
    
    
    /if (${Me.Book[${_count}].HasSPA[470]} || ${Me.Book[${_count}].HasSPA[374]} || ${Me.Book[${_count}].HasSPA[340]}) {
      DEBUG ${sep}${Me.Book[${_count}].HasSPA[470]} || ${Me.Book[${_count}].HasSPA[374]} || ${Me.Book[${_count}].HasSPA[340]}
      /for _countNumEff 1 to ${Me.Book[${_count}].NumEffects}
        /if (!${Me.Book[${_count}].Trigger[${_countNumEff}].HasSPA[${sqlite.Result[qBestSpell 1 env_var]}]}) {
          | DEBUG ${break}${Me.Book[${_count}].Name} does not contain${sep}\a-w${sqlite.Result[qBestSpell 1 env_var]}\ax
          /continue
        }
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
    /varset _subcat "Sum: ${maChr.Find[stMinionElementType].Value}"
  }

  /for _count 1 to ${maHard.Find[stSpellBookSize].Value}
    /if (!${Me.Book[${_count}].ID}) /continue
    /if (${Me.Book[${_count}].Category.NotEqual[Pet]}) /continue
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

  | no assist ??
  /if (!${assistData.ID} && !HEAL) {
    /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
    DEBUG ${break}NO ASSIST found/set
    /return FALSE
  } else /if (!${assistData.ID} && HEAL) {
    /if (${Group.MainAssist.ID}) {
      /varset assistData.ID Group.MainAssist.ID
    } else {
      /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
      DEBUG ${break}NO ASSIST found/set
      /return FALSE
    }
  }

  | get spawncount around the assist
  DEBUG SpawnCount[npc loc ${assistData.X} ${assistData.Y} ${assistData.Z} radius ${maPred.Find[stPredRad].Value} PS4]${sep}\a-w${SpawnCount[npc loc ${assistData.X} ${assistData.Y} ${assistData.Z} radius ${maPred.Find[stPredRad].Value} playerstate ${maPred.Find[stPredPlayerstate].Value}]}\ax
  /declare _countMob ${SpawnCount[npc loc ${assistData.X} ${assistData.Y} ${assistData.Z} radius ${maPred.Find[stPredRad].Value} playerstate ${maPred.Find[stPredPlayerstate].Value}]}

  | no mo bs
  /if (!${_countMob}) {
    /invoke ${set_data_timer[${_debug}, Check_Prediction, 12s]}
    DEBUG ${break}NO mob count
    /return FALSE
  }

  | more reasons to not do this??
  | assist is still relatively close
  /if (${assistData.Distance} < SAFEENV) {
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
  
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

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
    /if (!${cast_data[${_debug}, "${maGTFO.Find[stGTFO${_liC12.Value}].Value}"]}) {
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

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    
    /call cast ${_debug} 0 FALSE
    /return TRUE

    /invoke ${_liC12.Advance}
  }

/return TRUE



|***
 * note: casts skills from maPull for engine2
 * use: /call cast_cycle_agro DEBUG TEMPID
 ***|
sub pull_get_other_type(bool _debug, spawn _spawn)
  DEBUG \atpull_get_other_type\ax(\a-w${_spawn.ID}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLinePull.Contains[EMPTY]} && ${lsLinePull.Count} == 1) {
    DEBUG ${sep}resetting the empty pull line
    /invoke ${lsLinePull.Clear}
    /invoke ${lsLinePull.Append[${stCount12}]}
  }

  /declare _liC12 listiterator local
  /vardata _liC12 lsLinePull.First.Clone
  /while (!${_liC12.IsEnd}) {

    SPAWNDEAD

    GETINPUT cast_cycle_agro NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_agro

    /call target set ${_spawn.ID}

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

    /if (!${cast_data[${_debug}, "${maPull.Find[stPull${_liC12}].Value}"]}) {
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


    | make sure we are stopped if needed
    /if (!${Select[${Me.Class.ShortName},BRD]} || !${lsPull${_liC12.Value}Tag.Contains[ignoremovement]}) {
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

    /if (${lsPull${_liC12.Value}Tag.Contains[ignoremovement]}) {
      /call cast ${_debug} ${_spawn.ID} ignoremovement PULLENV
    } else { 
      /call cast ${_debug} ${_spawn.ID} FALSE PULLENV
    }
    
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

  CHECKTIE
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKEXIT check_buff_cycle_temp
  GETINPUT check_buff_cycle_temp NA|NA

  /declare _count int local 0
  /declare _countToon int local 1
  /declare _target bool local FALSE
  /declare _found bool TRUE
  /declare _i int local 0
  /declare _tmpBuff string local FALSE
  /declare _spawn spawn local

  DEBUG ${sep}Buff${sep}\a-wTemp\ax

  SETHUD "buff temp"

  | each toon
  /while (${_countToon} <= ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}) {
    DEBUG ${sep}${_countToon} <= ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}

    | is the spawn in zone?
    /if (!${Spawn[pc ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}].ID}) {
      /varcalc _countToon ${_countToon}+1
      /continue
    }
     
    /vardata _spawn Spawn[pc ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}]
    | skip this person?
    /if (!${_spawn.ID} || ${_spawn.Dead}) {
      DEBUG ${sep}skipping${sep}\a-w${_spawn.DisplayName}\ax
      /varcalc _countToon ${_countToon}+1
      /continue
    }
    
    DEBUG ${sep}_spawn${sep}\ay${_spawn.DisplayName}\ax


    | any buff slots available?
    /if (${_spawn.DisplayName.Equal[${Me.DisplayName}]}) {
      /if (!${Me.FreeBuffSlots}) {
        /varcalc _countToon ${_countToon}+1
        /continue
      }
    } else {
      /dquery ${_spawn.DisplayName} -q "Me.FreeBuffSlots" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      /if (${DNQReturn.Equal[0]}) {
        /varcalc _countToon ${_countToon}+1
        /continue
      }      
    }

    | each buff
    /for _count 1 to 50
      
      CHECKEXIT
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      GETOVERHERE
    
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
      } else /if (!${lsBuff${_count}Tag.Contains[${_spawn.DisplayName}]}) {
        /continue
      }
            
      | buffs with SDPA parsing to be done
      /if (${maBuff.Find[stBuff${_count}].Value.Count[|]} == 3) {
        /call get_best_in_spell ${_debug} "${maBuff.Find[stBuff${_count}].Value.Arg[1,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[2,|]}" "${maBuff.Find[stBuff${_count}].Value.Arg[3,|]}"
        /invoke ${maCD.Add[tmpName,${Macro.Return}]}

      | get item clickie spells
      } else /if (${FindItem[=${maBuff.Find[stBuff${_count}].Value}].ID}) {
        DEBUG ${sep}item${sep}${maBuff.Find[stBuff${_count}].Value}${sep}\a-w${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}\ax
        /invoke ${maCD.Add[tmpName,${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Clicky.SpellID}].Name}]}
    
      | just the spell/aa/whatever
      } else {
        DEBUG ${sep}spell${sep}${maBuff.Find[stBuff${_count}].Value}
        /invoke ${maCD.Add[tmpName,${maBuff.Find[stBuff${_count}].Value}]}
      }

      | cycle the tags
      /call tag_evaluation ${maDebug.Find[tag].Value} Buff ${_count} ${_spawn.DisplayName} "${maCD.Find[tmpName].Value}" check_buff_cycle_temp
      /if (!${Macro.Return}) {
        DEBUG ${sep}skip${sep}failed tag cycle check
        /continue
      }

      DEBUG ${sep}\agBUFF TEMP\ax#${sep}\a-w${_count}\ax \a-p---------------------\ax

      | pretend they are not a target until they qualify
      /varset _target FALSE

      | is this an acceptable target for casting
      /if (${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Find[${_spawn.DisplayName}]}) {
        DEBUG ${sep}target${sep}\a-w${_spawn.DisplayName}\ax (\a-rvalid\ax)
        /varset _target TRUE
      }

      /call cast_data ${_debug} "${maCD.Find[tmpName].Value}"

      | distance check
      /if (${Select["${spellData.TargetType}",Group v2]}) {
        DEBUG ${sep}distance check${sep}\a-wGroup v2\ax
        /if (${_spawn.Distance} > ${spellData.AERange}) /continue
      } else {
        DEBUG ${sep}distance check${sep}\a-wtargeted ?? ${spellData.MyRange}\ax
        /if (${spellData.MyRange}) {
          /if (${_spawn.Distance} > ${spellData.MyRange} && !${Select[${spellData.TargetType},self]}) /continue
        }
      }

      | does it stack
      /dquery ${_spawn.DisplayName} -q "Spell[${spellData.RankName}].WillLand" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${sep}${sep}WillLand: .Buff.WillLand \at${DNQReturn}\ax ${dot} ${spellData.RankName}
      /if (!${DNQReturn}) {
        /continue
      }

      | do they have the buff?
      /dquery ${_spawn.DisplayName} -q "Me.Buff[${maCD.Find[tmpName].Value}].ID" -o DNQReturn -t ${maEntropy.Find[stDanNetQueryDelay].Value}
      DEBUG ${sep}${sep}HAVE: .Buff.ID \at${DNQReturn}\ax ${dot} ${maCD.Find[tmpName].Value}
      /if (${Bool[${DNQReturn}]}) {
        /continue
      }

      DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${maCD.Find[tmpName].Value}

      /if (${_target}) {
        /call cast ${_debug} ${_spawn.ID} FALSE
        SETHUD
        /return TRUE
      }


    /next _count
    /varcalc _countToon ${_countToon}+1

  }

  SETHUD
  
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

  /if (${_option.Equal[-h]}) {
    OUT use${sep}\a-w/${_command.Lower} [self|group|zone|raid|all|broadcast]\ax
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
  
  /varset _command ${proper_case[name, ${_command}]}

  | default return to let original sub do the work
  /if (${Select[${_command},gather]}) {
    /return FALSE
  }

   
  | redirect command for solo use
  /if (${Select[${_command},cls,enter,lesson]}) {
    /if (${Select[${maEntropy.Find[stBroadcast${_command}].Value},group]} && !${Group.Members}) {
      /varset _option self
    } else /if (${Select[${maEntropy.Find[stBroadcast${_command}].Value},raid]} && !${Raid.Members}) {
      /varset _option self
    }
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
      /dggexecute /${_command.Lower} self
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
      /dgexecute ${maEntropy.Find[stEntropyGroup_all].Value} /command_${_command} self
    } else {
      /dgexecute ${maEntropy.Find[stEntropyGroup_all].Value} /${_command} self
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
 * use: /call cast_cycle_melee DEBUG [spawnID] [TAG]
 ***|
sub cast_cycle_melee(bool _debug, spawn _spawn, string _tag)
  DEBUG \atcast_cycle_melee\ax(\a-w${_spawn.ID}, ${_tag}\ax)

  /if (${target[isdead, ${_spawn.ID}]}) {
    /return FALSE
  }

  CHECKEXIT
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  CHECKDEFENSE

  /call target set ${_spawn.ID}

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /if (AUTO || ${swModeToken}) {
    /if (!${timer_check_stick}) {
      /call set_combat_control ${maDebug.Find[decision].Value} ${_spawn.ID}
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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_melee

    /if (${target[isdead, ${_spawn.ID}]}) {
      /return FALSE
    }

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

    /if (!${cast_data[${_debug}, "${maMelee.Find[stMelee${_liMelee.Value}].Value}"]}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    | minimum range
    /if (${_spawn.Distance} > ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Melee ${_liMelee.Value} ${Me.DisplayName} "${maMelee.Find[stMelee${_liMelee.Value}].Value}" cast_cycle_melee
    /if (!${Macro.Return}) {
      /invoke ${_liMelee.Advance}
      /continue
    }

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    
    /invoke ${out[8, "Melee${sep}${sp}${maMelee.Find[stMelee${_liMelee.Value}].Value}\ax ${dot} ${con_color[${_debug}, ${_spawn.ID}]}\ax"]}
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
sub pull_home_move_to(bool _debug, spawn _spawn)
  DEBUG \atpull_home_move_to\ax(\a-w${_spawn.ID}\ax)

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
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    
    | self heal while returning to camp; heal needs tag ignoremovement
    /if (${maPull.Find[swPullHealCheck].Value}) {
      /call spawn_hurt ${maDebug.Find[hurt].Value} self
      /if (${hurtCount}) {
        /call cast_cycle_heal ${_debug} ${Me.ID} ignoremovement
      }
    }

    | do we slow down if we start to outrun a mob?
    /if (${maPull.Find[stPullOutrunRange].Value} && !${Me.Mount.ID}) {
      DEBUG ${sep}stPullOutrunRange${sep}\a-w${maPull.Find[stPullOutrunRange].Value}\ax  ..  ${_spawn.Distance}
      | set walk
      /if (${_spawn.Distance} > ${maPull.Find[stPullOutrunRange].Value} && ${Me.Running}) {
        DEBUG ${sep}movement${sep}walk
        /keypress RUN_WALK
      }
      /delay 1 ${_spawn.Distance} <= ${maPull.Find[stPullOutrunRange].Value}
      | return to run
      /if (${_spawn.Distance} <= ${maPull.Find[stPullOutrunRange].Value} && !${Me.Running}) {
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
  
    /if (!TARGET || ${Spawn[TARGET].Distance} > ENV || HEAL) {
      /if (!${maOver.Find[swTargetClear].Value}) {
        /invoke ${target[clear]}
      }
    }
    
    | no assist set at all
    /if (!${Bool[${maCC.Find[stAssistMode].Value}]} && !${maCC.Find[swManualAssist].Value} && (${lsAss.Contains[EMPTY]} && ${lsAss.Count} == 1)) {
      DEBUG ${submenu}pullstate (\a-rno assist set, releasing\ax)
      /if (!${maOver.Find[swTargetClear].Value}) {
        /invoke ${target[clear]}
      }
      /return FALSE
    }     
    
    | prediction
    /if (${maPred.Find[swPred].Value} && !${timer_Check_Prediction}) {
      /call get_prediction ${maDebug.Find[prediction].Value}
    }

    DEBUG ${submenu}pullstate (\a-rget assist\ax)
    /call get_assist_target ${maDebug.Find[assist].Value}
    /if (!${Macro.Return}) {
      /delay 1
      /if (${maOver.Find[swTargetClear].Value}) {
        /invoke ${target[clear]}
      }
    }

  } else /if (PULL) {
    
    | alternate pullers
    /if (${Group.MainAssist.ID} != ${Me.ID} && ${Group.Members}) {
      DEBUG ${submenu}pullstate (\a-ralternate puller\ax)

      | chain pulling
      /if (${maPull.Find[stCountChainPull].Value} && ${maPull.Find[stPullMode].Value.NotEqual[base]} && ${SpawnCount[NPC los radius ENV PS4 targetable]} <= ${maPull.Find[stCountChainPull].Value}) {
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
          | /call get_target_${maPull.Find[stPullMode].Value}pull ${maDebug.Find[pull].Value} 0 ${Int[${Math.Calc[${maPull.Find[stCountChainPull].Value} + 1]}]}
          /call get_target_${maPull.Find[stPullMode].Value}pull ${maDebug.Find[pull].Value} 0 ${maPull.Find[stCountChainPull].Value}
          
   
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
  }

/return TRUE



|***
 * note: pulling a mob with an intervention pull
 * use:
 ***|
sub pull_mob_intervention(bool _debug, spawn _spawn)
  DEBUG \atpull_mob_intervention\ax(\a-w${_spawn.ID}\ax)
  /call cast_cycle_heal ${_debug} ${_spawn.ID} interventionpull
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
  /declare _spawn spawn local
  
  /for _countGroup 1 to ${_groupList.Count[|]}
    /varset _group ${_groupList.Arg[${_countGroup},|]}
    
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

    | group empty / does not exist
    /if (!${DanNet.PeerCount[${_group}]}) {
      DEBUG ${break}no counts int group${sep}\a-w${_groupList.Arg[${_countGroup},|]}\ax
      /continue
    }

    | check each name in the group. bail on FIRST good match
    /for _countName 1 to ${DanNet.Peers[${_group}].Count[|]}
      DEBUG ${sep}_countName:${_countName}  (${_groupList.Arg[${_countGroup},|]})
 
      /if (!${Spawn[pc ${DanNet.Peers[${_group}].Arg[${_countName},|]}].ID}) {
        /continue
      }
 
      /vardata _spawn Spawn[pc ${DanNet.Peers[${_group}].Arg[${_countName},|]}]
      
      DEBUG ${sep}spawn${sep}\a-w${_spawn.DisplayName}\ax
      
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

      | never use the MA
      /if (${Group.MainAssist.ID} == ${_spawn.ID} || ${_verbage.Equal[notme]} && ${_spawn.ID} == ${Me.ID}) {
        DEBUG ${break}dont use me, use someone else
        /continue
      }

      | no name/not here.. wtf?
      /if (!${_spawn.ID}) {
        DEBUG ${break}bad name/nothere?
        /continue
      }

      | bitches are dead
      /if (${_spawn.Dead}) {
        DEBUG ${break}dead
        /continue
      }

      | too far away to be of any damn use
      /if (${_spawn.Distance} > ${_distance}) {
        DEBUG ${break}_spawn.distance > ${_distance}
        /continue
      }

      | we made it this far give em a name
      DEBUG ${sep}get_resident${sep}\a-w${_spawn.DisplayName}\ax
      /return ${_spawn.DisplayName}

    /next _countName
  /next _countGroup

/return FALSE



|***
 * note: buff removal by SPA
 * use: /call remove_spa DEBUG SPA#
 ***|
sub remove_spa(bool _debug, int _spa)
  DEBUG \atremove_spa\ax(\a-w${_spa}\ax)
 
  /if (!${Me.SPA[${_spa}]}) {
    /return TRUE
  }

  /declare _count int local 0

  /for _count 1 to ${Me.MaxBuffSlots}
   
    | remove buffs
    /if (${Me.Buff[${_count}].HasSPA[${_spa}]}) {
      /if (${lsGrain.Contains[28]}) {
        OUT Removing (\a-r${_count}\ax)${sep}\a-w${Me.Buff[${_count}]}\ax
      }
      /invoke ${Me.Buff[${_count}].Remove}
      /delay 2
    }

    /if (!${Me.SPA[${_spa}]}) {
      /return TRUE
    }
    
    /if (${_count} > 30) {
      /continue
    }
    
    | remove buffs
    /if (${Me.Song[${_count}].HasSPA[${_spa}]}) {
      /if (${lsGrain.Contains[28]}) {
        OUT Removing (\a-r${_count}\ax)${sep}\a-w${Me.Song[${_count}]}\ax
      }
      /invoke ${Me.Song[${_count}].Remove}
      /delay 2
    }    
   
    /if (!${Me.SPA[${_spa}]}) {
      /return TRUE
    }

  /next _count

/return TRUE


  
|***
 * note: Cures/removes debuffs, we hope...
 * use:
 **|
sub check_cure(bool _debug)
  DEBUG \atcheck_cure\ax()

  /invoke ${set_data_timer[${_debug}, Check_Cure, RESTART]}

  /if (!AUTO) {
    /return FALSE
  }

  | dont take time from mobs attention
  /if ((PULL || AGRO) && ${Me.CombatState.Equal[combat]}) {
    /return FALSE
  }

  SETHUD "check cure"

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT check_cure NA|NA

  | My daughter said you have issues.. She's usually right.
  /if (${Me.TotalCounters} && ${maCure.Find[swCureSelf].Value}) {
    DEBUG ${sep}\aw${Me.DisplayName.Lower}\ax needs a medic!

  } else /if (!${Me.TotalCounters} && ${maCure.Find[swCureSelf].Value}) {
    DEBUG ${sep}\aw${Me.DisplayName.Lower}\ax is just fine
  }
  
  /if (${Me.CountersCurse}) {
    /call cast_cycle_cure ${_debug} ${Me.ID} curse  
    /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
    /goto :end_cure
  } else /if (${Me.CountersCorruption}) {
    /call cast_cycle_cure ${_debug} ${Me.ID} corruption  
    /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
    /goto :end_cure
  } else /if (${Me.CountersPoison}) {
    /call cast_cycle_cure ${_debug} ${Me.ID} poison  
    /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
    /goto :end_cure
  } else /if (${Me.CountersDisease}) {
    /call cast_cycle_cure ${_debug} ${Me.ID} disease  
    /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
    /goto :end_cure
  }


  | bail now if only self curing
  /if (!${maCure.Find[swCureGroup].Value} && !${maCure.Find[swCureCrew].Value}) {
    /return TRUE
  }

  | time to check the others
  /declare _count int local 0
  /declare _toon string local

  | each toon
  /for _count 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}

    | get the name to something usable
    /varset _toon ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
    DEBUG ${sep}${_count}${sep}checking${sep}\aw${_toon}\ax (\a-r${maEntropy.Find[stEntropyGroup_all].Value}\ax)
    
    /if (${_toon.Equal[${Me.DisplayName}]}) {
      DEBUG ${sep}{_toon.Equal[${Me.DisplayName}]}
      /continue
    }
      
    | are they here?
    /if (!${Spawn[pc ${_toon}].ID} || ${Spawn[pc ${_toon}].Dead}) {
      DEBUG ${sep} ! Spawn
      /continue
    }

    /if (!${observer_validate[${_debug}, obs_cure, ${_toon}]}) {
      DEBUG ${sep}observer validate fail
      /continue
    }   

    | group only
    /if (${maCure.Find[swCureGroup].Value} && !${maCure.Find[swCureCrew].Value}) {
      /if (!${Group.Member[${_toon}].Present}) {
      DEBUG ${sep}a switch is off
        /continue
      }
    }

    | are they close?. if not. fuck'em
    /if (${Spawn[pc ${_toon}].Distance} > ${maHard.Find[stMaxCureRad].Value}) {
      DEBUG ${sep}out of range
      /continue
    }
    
    | Put cure stuff here.... like the next line..

    /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
    | /goto :end_cure
    
    | are they debuffed at all?
    /if (!${DanNet[${_toon}].Observe["Me.TotalCounters"]} && !${Spell[${DanNet[${_toon}].Observe["Me.Snared"]}].ID} && !${Spell[${DanNet[${_toon}].Observe["Me.Mezzed"]}].ID}) {
      DEBUG ${sep}\aw${_toon}\ax is just fine
      /continue
    } else /if (${DanNet[${_toon}].Observe["Me.TotalCounters"]}) {
      OUT Medic${sep}\a-w${_toon}\ax (\a-r${DanNet[${_toon}].Observe["Me.TotalCounters"]}\ax)
    }

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    CHECKEXIT

    | cure with spell
    /if (${DanNet[${_toon}].Observe["Me.CountersCurse"]}) {
      DEBUG ${sep}${_toon} is CURSED
      /delay 3s !${Me.SpellInCooldown}
      /call cast_cycle_cure ${_debug} ${Spawn[pc ${_toon}].ID} curse
      /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
      /goto :end_cure
    }

    /if (${DanNet[${_toon}].Observe["Me.CountersCorruption"]}) {
      DEBUG ${sep}${_toon} is CORRUPTED
      /delay 3s !${Me.SpellInCooldown}
      /call cast_cycle_cure ${_debug} ${Spawn[pc ${_toon}].ID} corruption
      /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
      /goto :end_cure
    }

    /if (${DanNet[${_toon}].Observe["Me.CountersPoison"]}) {
      DEBUG ${sep}${_toon} is POISONED... danger noodle~
      /delay 3s !${Me.SpellInCooldown}
      /call cast_cycle_cure ${_debug} ${Spawn[pc ${_toon}].ID} poison
      /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
      /goto :end_cure
    }

    /if (${DanNet[${_toon}].Observe["Me.CountersDisease"]}) {
      DEBUG ${sep}${_toon} is DISEASED...Burn'em with FIRE!!!
      /delay 3s !${Me.SpellInCooldown}
      /call cast_cycle_cure ${_debug} ${Spawn[pc ${_toon}].ID} disease
      /invoke ${set_data_timer[${_debug}, Check_Cure, 1s]}
      /goto :end_cure
    }

  /next _count

  :end_cure
  SETHUD

/return TRUE



|***
 * note: cycles a map of tlos to make sure the observers have been set before usage
 * use: /call observer_validate [mapname] [character to check]
 **|
sub observer_validate(bool _debug, string _table, string _toon)
  DEBUG \atobserver_validate\ax(\a-w${_table}, ${_toon}\ax)

  /declare _tmpVar          string  local FALSE
  /declare _tmpObs          string  local FALSE
  /declare _countObs        int     local 1

  /while (${_countObs} <= ${sqlite.Rows[${_table}]}) {

    /varset _tmpVar ${_toon}_${sqlite.Result[${_table} ${_countObs} env_var]}
    /varset _tmpObs ${sqlite.Result[${_table} ${_countObs} value]}
    DEBUG ${sep}{DanNet[${_toon}].OSet[${_tmpObs}]}${sep}\a-w${DanNet[${_toon}].OSet["${_tmpObs}"]}\ax

    /if (!${DanNet[${_toon}].OSet["${_tmpObs}"]}) {
      /return FALSE      
    }

    /varcalc _countObs ${_countObs} + 1
  }

/return TRUE



|***
 * note: creates observers
 * use: /call observer_build DEBUG [db table of things to observe] [add|drop] [silent T/F]
 ***|
sub observer_build(bool _debug, string _table, string _action, bool _silent)
  DEBUG \atset_observers\ax(\a-w${_table}, ${_action}, ${_silent}\ax)

  /declare _count           int     local 1
  /declare _countObs        int     local 1
  /declare _actionO         string  local FALSE
  /declare _toon            string  local FALSE
  /declare _timestart       int     local 0
  /declare _timeend         int     local 0
  /declare _found           bool    local FALSE
  /declare _tmpVar          string  local FALSE
  /declare _tmpObs          string  local FALSE
  /declare _countFail       int     local 0

  /invoke ${set_data_timer[${_debug}, Check_Observers, RESTART]}

  | correct/check the action variable
  /if (${_action.Equal[drop]}) {
    /varset _actionO -drop
  } else /if (${_action.Equal[add]}) {
    /varset _actionO
  } 

  | only reload if not there
  /if (!${sqlite.Rows[${_table}]}) {
    /sqlite query "${DBstatic}" ${_table} SELECT * FROM ${_table};
  }
   
  | if we still dont have data, take the ball and go home
  /if (!${sqlite.Rows[${_table}]}) {
    /return FALSE
  }
  
  | do all the peoples!
  /while (${_count} <= ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}) {
    /varset _timestart ${MacroQuest.Running}
    /varset _toon ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
    /varset _countFail 0
    
    | skip me
    /if (${_toon.Equal[${Me.DisplayName}]}) {
      /varcalc _count ${_count} + 1
      /continue
    }
    
    DEBUG ${sep}_toon\aw ${_count}\ax::\at${_toon}\ax
    /varset _found FALSE
    /varset _countObs 1
    
    DEBUG ${sep}${sep}\a-w${sqlite.Rows[${_table}]}\ax

    | start our trip throught the sql data
    /while (${_countObs} <= ${sqlite.Rows[${_table}]}) {

      /varset _tmpVar ${_toon}_${sqlite.Result[${_table} ${_countObs} env_var]}
      /varset _tmpObs ${sqlite.Result[${_table} ${_countObs} value]}

      | define or delete the variables as we need
      /if (${_action.Equal[drop]}) {
        /if (!${DanNet[${_toon}].OSet["${_tmpObs}"]}) {
          /varcalc _countObs ${_countObs} + 1
          /continue
        }

      } else /if (${_action.Equal[add]}) {
        /if (${DanNet[${_toon}].OSet["${_tmpObs}"]}) {
          /varcalc _countObs ${_countObs} + 1
          /continue
        }
        
        | if the observer variable does not exist create it
        /if (!${Defined[${_tmpVar}]}) {
          /declare ${_tmpVar} string outer
        }
      }
       
      | set observer or drop it
      /dobserve ${_toon} -q "${_tmpObs}" -o ${_tmpVar} ${_actionO}
      /delay 5 ${DanNet[${_toon}].OSet["${_tmpObs}"]}
      /if (!${DanNet[${_toon}].OSet["${_tmpObs}"]}) {
        /varcalc _countFail ${_countFail} + 1
      }
      DEBUG ${sep}\a-t/dobserve\ax ${_toon} -q \a-w"${_tmpObs}"\ax -o ${_tmpVar} ${_actionO} (\a-w${DanNet[${_toon}].OSet["${_tmpObs}"]}\ax)
      
      /varset _found TRUE
      /varcalc _countObs ${_countObs} + 1
    }
    
    
    | set an end time for our test for that characer
    /varset _timeend ${MacroQuest.Running}

    | give us some response so we know that something is happening
    /if ((!${_silent} || ${_debug}) && ${_found}) {
      /invoke ${out[17, "Observe (${If[${_action.Equal[add]},\agadd\ax,\ardrop\ax]})${sep}${_table}(\a-r${_countFail}\ax)${sep}\aw${_toon}\ax${sep}\a-w${Math.Calc[((${_timeend}-${_timestart}) / 10) / 60]}s\ax"]}
    }

    | moving to the next toon in the dannet
    /varcalc _count ${_count} + 1
    /varset _found FALSE
  }

/return TRUE



|***
 * note: this really isnt much help
 * use: 
 ***|
sub get_help(string _type, string _verbage)

  |              subject   flag (-?)
  /lua get_help ${_type} ${_verbage}
  
/return TRUE



|***
 * note: cycles debuffs on a target
 * use: /call cast_cycle_debuff DEBUG [spawn] [TAG]
 ***|
sub cast_cycle_debuff(bool _debug, spawn _spawn, string _tag, bool _return)
  DEBUG \atcast_cycle_debuff\ax(\a-wtID:${_spawn.ID}, tag:${_tag}, return:${_return}\ax)

  CHECKDEFENSE
  
  /invoke ${set_data_timer[${_debug}, Check_deBuffs, RESTART]}

  | if the user has not set the line order, set it to a default for now
  /if (${lsLinedeBuff.Contains[EMPTY]} && ${lsLinedeBuff.Count} == 1) {
    DEBUG ${sep}resetting the empty melee line
    /invoke ${lsLinedeBuff.Clear}
    /invoke ${lsLinedeBuff.Append[${stCount12}]}
  }

  /declare _count int local 0
  /declare _found bool local TRUE
  /declare _type string local FALSE

  /declare _lidebuff listiterator local
  /vardata _lidebuff lsLinedeBuff.First.Clone
  /while (!${_lidebuff.IsEnd}) {

    GETINPUT cast_cycle_debuff NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT cast_cycle_debuff
    
    /delay 1
    
    DEBUG ${sep}checking ${_lidebuff.Value}${sep}\a-w${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}\ax

    | check the switch
    /if (!${madeBuff.Find[swdeBuff${_lidebuff.Value}].Value}) {
      DEBUG ${break}${_lidebuff.Value} switch off
      /invoke ${_lidebuff.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}]}) {
      DEBUG ${break}${_lidebuff.Value} no name
      /invoke ${_lidebuff.Advance}
      /continue
    }

    | check matching tag for failure
    /if (${Bool[${_tag}]}) {
      /if (!${lsdeBuff${_lidebuff.Value}Tag.Contains[${_tag}]}) {
        DEBUG ${break}${_lidebuff.Value} bad tag${sep}\a-w${_tag}\ax
        /invoke ${_lidebuff.Advance}
        /continue
      }
    }
   
    | get some workable data
    /if (!${cast_data[${_debug}, "${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}"]}) {
      DEBUG ${break}${_lidebuff.Value} not ready (\a-r${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}\ax)
      /invoke ${_lidebuff.Advance}
      /continue
    }
    
    | found us a spell, set it and return for calling routine
    /if (${_return}) {
      /invoke ${maCD.Add[returnCast,${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}]}
      /return TRUE
    }
 
    /call target set ${_spawn.ID}
    /delay 2s ${Target.BuffsPopulated}
    /varset _found TRUE

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} deBuff ${_lidebuff.Value} ${Me.DisplayName} "${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}" cast_cycle_debuff
    /if (!${Macro.Return}) {
      DEBUG ${break}tag fail
      /invoke ${_lidebuff.Advance}
      /continue
    }
    
    DEBUG ${sep}spellData ${dot}${dot} \aw${spellData.Name}\ax (\a-r${spellData.ID}\ax)
    
    /if (${maCD.Find[hasspa].Value}) {
      DEBUG ${sep}multiple landing ${dot}${dot} \aw${spellData}\ax

      /for _count 1 to ${spellData.NumEffects}
        DEBUG ${sep}_count ${dot}${dot} \aw${_count}\ax
        /varset _found TRUE

        | skip any secondary procs with no duration
        DEBUG ${sep}duration${sep}\a-w${spellData.Trigger[${_count}].Duration}\ax
        /if (${spellData.Trigger[${_count}].Duration} < 3) {
          DEBUG ${break} Duration < 3 ticks
          /continue
        }

        | stacksTarget
        DEBUG ${sep}stacks${sep}\a-w${spellData.Trigger[${_count}].StacksTarget}\ax
        /if (!${spellData.Trigger[${_count}].StacksTarget}) {
          DEBUG ${break}stack failure ${dot}${dot} \aw${spellData.StacksTarget}\ax
          /continue
        }

        | target has debuff
        DEBUG ${sep}Target.Buff${sep}\a-w${Target.Buff[${spellData.Trigger[${_count}]}].ID}\ax   ...... ${spellData.Trigger[${_count}]}
        /if (!${Target.Buff[${spellData.Trigger[${_count}]}].ID}) {
          DEBUG ${sep}NOT FOUND ${dot}${dot} \aw${spellData}\ax
          /varset _found FALSE
          /break
        }

      /next _count

      /if (!${_found}) {
        /call cast ${_debug} ${_spawn.ID} FALSE

        | good cast, get out...
        /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
          DEBUG ${break}good cast ${dot}${dot} \aw${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}\ax
          /return TRUE
        }

        /call cast_cooldown_wait ${_debug}
        /continue
      }

    } else {
      DEBUG ${sep}single landing ${dot}${dot} \aw${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}\ax      

      | stackstarget
      /if (!${spellData.StacksTarget}) {
        DEBUG ${break} spellData.StacksTarget
        /invoke ${_lidebuff.Advance}
        /continue
      }

      | target has debuff
      DEBUG ${sep}target deBuff ${Target.Buff[${spellData}].ID}
      /if (${Target.Buff[${spellData}].ID}) {
        DEBUG ${break} Target.Buff[${spellData}].ID
        /invoke ${_lidebuff.Advance}
        /continue
      }

      /call cast ${_debug} ${_spawn.ID} FALSE

      | good cast, get out...
      /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
        DEBUG ${break}good cast ${dot}${dot} \aw${madeBuff.Find[stdeBuff${_lidebuff.Value}].Value}\ax
        /return TRUE
      }

      /call cast_cooldown_wait ${_debug}
      /invoke ${_lidebuff.Advance}
      /continue

    }
    
    /invoke ${_lidebuff.Advance}

  }

/return TRUE



|***
 * note: see what debuffs to cast?
 * use: /call check_debuff_routines
 ***|
sub check_debuff_routines(spawn _spawn, bool _debug)
  DEBUG \atcheck_debuff_routines\ax(\aw${_spawn.ID}\ax)
 
  | kill this routine if there is no valid target
  /if (!${_spawn.ID}) {
    DEBUG ${break}no target passed
    /return FALSE
  }
 
  /declare _count int local 0
  /declare _found list local 
  /declare _variable string local FALSE
  /declare _member string local
  /declare _tmpTLO string local FALSE

  /declare _lideorder listiterator local
  /vardata _lideorder lsdeBuffOrder.First.Clone
  /while (!${_lideorder.IsEnd}) {

    GETOVERHERE
    CHECKEXIT
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}    
 
    /varset _debug ${If[${maDebug.Find[${_lideorder.Value}].Value},TRUE,FALSE]}
    
    /varset _variable ${proper_case[name, ${_lideorder.Value}]}
    
    | in an exclude list
    /if (${lsZoneNo${_variable}.Contains[${_spawn.DisplayName}]}) {
      DEBUG ${break} on exclude list${sep}\a-wlsZoneNo${_variable}\ax (\a-r${_spawn.DisplayName}\ax)
      /return FALSE
    }

    /delay 2s ${Target.BuffsPopulated}
    
    /if (${target[isdead, ${_spawn.ID}]}) {
      /return FALSE
    }      

    /if (!${Target.ID} || !${Target.BuffsPopulated} || ${Target.ID} != ${_spawn.ID}) {
      /return FALSE

    } else /if (${Target.ID} && ${Target.ID} == ${_spawn.ID} && ${Target.BuffsPopulated}) {
      DEBUG ${sep}.BuffsPopulated

      | tash / cripple / malo / slow
      /if (${Select[${_lideorder.Value},tash,cripple,malo,slow]}) {
        
        | set our tlo member name
        /if (${_lideorder.Value.Equal[tash]}) {
          /if (!${madeBuff.Find[swTash].Value}) {
            /invoke ${_lideorder.Advance}
            /continue                        
          }
          /varset _member Tashed
        } else /if (${_lideorder.Value.Equal[cripple]}) {
          /if (!${madeBuff.Find[swCripple].Value}) {
            /invoke ${_lideorder.Advance}                        
            /continue                        
          }
          /varset _member Crippled
        } else /if (${_lideorder.Value.Equal[malo]}) {
          /if (!${madeBuff.Find[swMalo].Value}) {
            /invoke ${_lideorder.Advance}                        
            /continue                        
          }
          /varset _member Maloed
        } else /if (${_lideorder.Value.Equal[slow]}) {
          /if (!${madeBuff.Find[swSlow].Value}) {
            /invoke ${_lideorder.Advance}                        
            /continue                        
          }
          /varset _member Slowed
        } 
        DEBUG ${sep}debuff${sep}\ay${_lideorder.Value}\ax
        
        | start our markers to show if the multi parts will land or not
        /invoke ${_found.Clear}
        
        |  does the target have that debuf? if not!
        /if (${Target.${_member}.ID}) {
          DEBUG ${sep}target is${sep}\a-w${_lideorder.Value}\ax (\a-r${Target.${_member}}\ax)
          | find the debuff we need to cast and hold it
          /call cast_cycle_debuff ${maDebug.Find[${_lideorder.Value}].Value} ${_spawn.ID} ${_lideorder.Value} TRUE          
          DEBUG ${sep}cast_cycle_debuff returned${sep}\a-r${maCD.Find[returnCast].Value}\ax
          
          | did we find something that we can cast?
          /if (${Bool[${maCD.Find[returnCast].Value}]} && ${_spawn.ID} && ${Target.ID}) {
            
            | is it a multi landing debuff?
            /if (${maCD.Find[hasspa].Value}) {
              DEBUG ${sep}multi landing debff${sep}\ay${spellData.Name}\ax
              
              | cycle each trigger for the debuff
              /for _count 1 to ${spellData.NumEffects}
                
                DEBUG ${break}compare${sep}${Target.${_member}.Name}${sep}\a-r${Target.${_member}.Base[2]}\ax <= ${spellData.Name}${sep}\a-r${spellData.Trigger[${_count}].Base[2]}}\ax
                
                | is the targets debuff better then the one we are casting      
                /if (${Target.${_member}.Base[2]} <= ${spellData.Trigger[${_count}].Base[2]} || !${spellData.StacksTarget}) {
                  | if it is better, add a found marker and continue
                  /invoke ${_found.Append[1]}
                } else {
                  | if its not better mark it missing and continue
                  /invoke ${_found.Append[0]}
                }
                
              /next _count

            } else {
              DEBUG ${sep}single landing debff${sep}\a-w${spellData.Name}\ax
              /if (${Spell[${Target.${_member}}].Base[2]} <= ${spellData.Base[2]}) {
                DEBUG ${break}compare${sep}${Target.${_member}.Name}${sep}\a-r${Target.${_member}.Base[2]}\ax <= ${spellData.Name}${sep}\a-r${spellData.Base[2]}\ax
                /invoke ${_found.Append[1]}
              } else {
                /invoke ${_found.Append[0]}
              }
            }
            
          | we found no available debuff to cast on the target
          } else /if (!${Bool[${maCD.Find[returnCast].Value}]} || !${_spawn.ID} || !${Target.ID}) {
            DEBUG ${sep}Something is wrong with our targeting or cast
            DEBUG ${sep}returnCast:${maCD.Find[returnCast].Value} / _spawn.ID:${_spawn.ID} / target.id:${Target.ID}
            /return FALSE
          }
        }
 
      | snare
      } else /if (${Select[${_lideorder.Value},snare]}) {

        /if (${_lideorder.Value.Equal[snare]}) {
          /varset _member Snared
        } 
        DEBUG ${sep}debuff${sep}\ay${_lideorder.Value}\ax
        
        /invoke ${_found.Clear}
        
       /if (${Target.${_member}.ID}) {

          DEBUG ${sep}target is${sep}\a-w${_lideorder.Value}\ax (\a-r${Target.${_member}}\ax)
          /call cast_cycle_debuff ${maDebug.Find[${_lideorder.Value}].Value} ${_spawn.ID} ${_lideorder.Value} TRUE
          DEBUG ${sep}returning spell${sep}\a-r${maCD.Find[returnCast].Value}\ax

          /if (${Bool[${maCD.Find[returnCast].Value}]} && ${_spawn.ID}) {

            /if (${maCD.Find[hasspa].Value}) {
              DEBUG ${sep}multi landing debff${sep}\ay${spellData.Name}\ax
              /for _count 1 to ${spellData.NumEffects}
                DEBUG Target.${_member}.Base[2] (\a-r${Target.${_member}.Base[2]}\ax) < spellData.Trigger[${_count}].Base[2] (\a-r${spellData.Trigger[${_count}].Base[2]}\ax) || !spellData.StacksTarget (\a-r!${spellData.StacksTarget}\ax)
                /if (${Target.${_member}.Base[2]} < ${spellData.Trigger[${_count}].Base[2]} || !${spellData.StacksTarget}) {
                  /invoke ${_found.Append[1]}
                } else {
                  /invoke ${_found.Append[0]}
                }
                
              /next _count

            } else {
              DEBUG ${sep}single landing debff${sep}\a-w${spellData.Name}\ax
              /if (${Target.${_member}.Base[2]} < ${spellData.Base[2]}) {
                DEBUG ${break}compare${sep}${Target.${_member}.Name}${sep}\a-r${Target.${_member}.Base[2]}\ax < ${spellData.Name}${sep}\a-r${spellData.Base[2]}\ax
                /invoke ${_found.Append[1]}
              } else {
                /invoke ${_found.Append[0]}
              }            
            }
          } else /if (!${Bool[${maCD.Find[returnCast].Value}]} || !${_spawn.ID} || !${Target.ID}) {
            DEBUG ${sep}Something is wrong with our targeting or cast
            DEBUG ${sep}returnCast:${maCD.Find[returnCast].Value} / _spawn.ID:${_spawn.ID} / target.id:${Target.ID}
            /return FALSE
          }
          
        }

      }
      
      | if we do not find any missing markers, we continue on our debuffing journey of: OMG can we just move on
      /if (!${_found.Contains[0]}) {
        DEBUG ${sep}debuffing....
        | debuff time
        /call cast_cycle_debuff ${maDebug.Find[${_lideorder.Value}].Value} ${_spawn.ID} ${_lideorder.Value} FALSE
      } else {
        DEBUG ${sep}targets debuffs are better or equal
      }

      /invoke ${maCD.Add[returnCast,FALSE]}
      /invoke ${_lideorder.Advance}
    }
                
  }
  
/return TRUE



|***
 * note: https://www.imdb.com/title/tt2490326
 * use: /call check_cycle_cure [DEBUG] [spawnID] [tag] [return t/f]
 ***|
sub cast_cycle_cure(bool _debug, spawn _spawn, string _tag, bool _return)
  DEBUG \atcast_cycle_cure\ax(\a-wID:${_spawn.ID}, tag:${_tag}, ${_return}\ax)

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineCure.Contains[EMPTY]} && ${lsLineCure.Count} == 1) {
    DEBUG ${sep}resetting the empty cure line
    /invoke ${lsLineCure.Clear}
    /invoke ${lsLineCure.Append[${stCount12}]}
  }

  /declare _lilc listiterator local
  /vardata _lilc lsLineCure.First.Clone
  /while (!${_lilc.IsEnd}) {

    GETINPUT cast_cycle_defense NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE

    | check the switch
    /if (!${maCure.Find[swCure${_lilc.Value}].Value}) {
      /invoke ${_lilc.Advance}
      /continue
    }

    | check the name
    /if (!${Bool[${maCure.Find[stCure${_lilc.Value}].Value}]}) {
      /invoke ${_lilc.Advance}
      /continue
    }

    | is ready?
    /if (!${cast_data[${_debug}, "${maCure.Find[stCure${_lilc.Value}].Value}"]}) {
      /invoke ${_lilc.Advance}
      /continue
    }
     
    | check matching passed tag for failure
    /if (${Bool[${_tag}]}) {
      /if (!${lsCure${_lilc.Value}Tag.Contains[${_tag}]}) {
        DEBUG ${break}${_lilc.Value} bad passed tag${sep}\a-w${_tag}\ax
        /invoke ${_lilc.Advance}
        /continue
      }
    }   

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Defense ${_lilc.Value} ${Me.DisplayName} "${maCure.Find[stCure${_lilc.Value}].Value}" cast_cycle_cure
    /if (!${Macro.Return}) {
      /invoke ${_lilc.Advance}
      /continue
    }

    /if (${_return}) {
      /invoke ${maCD.Add[returnCast,${maCure.Find[stCure${_lilc.Value}].Value}].Value}]}
      /return TRUE
    }

    DEBUG ${sep}_lilc.Value${sep}\a-w${_lilc.Value}\ax${sep}passed tags
    
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
        
    /call cast ${_debug} ${_spawn.ID} FALSE

    | good cast, get out...
    /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
      DEBUG ${break}good cast${sep}\a-w${maCure.Find[stCure${_lilc}].Value}\ax
      /return TRUE
    }

    /invoke ${_lilc.Advance}
  }

/return TRUE



|***
 * note: Acquire target, get bitches. This will establish ${stAssTarID} / targetData
 * use: /call get_assist_target DEBUG [call]
 * option: inline - is for inline calling of assisting only. cannot be used with the manual switch on
 * option: return - does no targeting and simply updates spawn targetData, cannot be used with the manual switch
 ***|
sub get_assist_target(bool _debug, string _call)
  DEBUG \atget_assist_target\ax(\a-w${_call}\ax)
  
  /if (!AUTO || ${maCC.Find[swSetForced].Value} || MODE) {
    DEBUG ${sep}assisting${sep}\a-wmanual target\ax
    /return ${Target.ID}

  | if were not assisting.. don't assist
  } else /if (!${Bool[${maCC.Find[stAssistMode].Value}]} && !${maCC.Find[swManualAssist].Value} && (${lsAss.Contains[EMPTY]} && ${lsAss.Count} == 1) && !PULL) {
    DEBUG ${break}\arNO\ax assist
    /return 0

  | normal puller bypass for assisting
  } else /if (PULL && AUTO) {
    /if (${Group.MainAssist.ID} == ${Me.ID} || ${Raid.MainAssist[1].ID} == ${Me.ID} || ${Raid.MainAssist[2].ID} == ${Me.ID} || ${Raid.MainAssist[3].ID} == ${Me.ID}) {
      DEBUG ${break}i'm the assist/puller
      /return ${targetData.ID}
    } else /if (${maPull.Find[stCountChainPull].Value} && ${targetData.ID}) {
      DEBUG ${sep}assisting${sep}\a-wpullers target\ax
      | do nothing. we want this to do an assist
    } 
  }
  
  | smart / DanNet Assisting
  /if (!${maCC.Find[swManualAssist].Value}) {
 
    | tar acquisition when using smart assist. set XTarget positions.
    /if (${Select[${maCC.Find[stAssistMode].Value},g,1,2,3,m1,m2,m3]}) {
      DEBUG ${sep}Returning${sep}\a-wsmart assist\ax

      | assist info for group assisting
      /if (${Select[${maCC.Find[stAssistMode].Value},g]} && ${Group.MainAssist.ID} && !${Group.MainAssist.Dead} && ${Me.GroupAssistTarget.ID}) {
        DEBUG ${sep}set smart${sep}\a-wg\ax
        /vardata assistData Spawn[${Group.MainAssist.ID}]
        /vardata targetData Spawn[${Me.GroupAssistTarget.ID}]
        /varset stAssTarID ${Me.GroupAssistTarget.ID}
        /if (${_call.Equal[return]}) {
          /return ${targetData.ID}
        }

      | raid assist 1
      } else /if (${Select[${maCC.Find[stAssistMode].Value},1]} && ${Raid.MainAssist[1].ID} && !${Raid.MainAssist[1].Dead}) {
        DEBUG ${sep}set smart${sep}\a-w1\ax
        /vardata assistData Spawn[${Raid.MainAssist[1].ID}]
        /vardata targetData Spawn[${Me.RaidAssistTarget[1].ID}]
        | /vardata assistData Raid.MainAssist[1].ID
        | /vardata targetData Me.RaidAssistTarget[1].ID
        /varset stAssTarID ${Me.RaidAssistTarget[1].ID}
        /if (${_call.Equal[return]}) {
          /return ${targetData.ID}
        }
       
      | raid mark 1  
      } else /if (${Select[${maCC.Find[stAssistMode].Value},m1]}) {
        DEBUG ${sep}set raid mark${sep}\a-wm1\ax
        /vardata targetData Spawn[${Me.RaidMarkNPC[1].ID}]
        /varset stAssTarID ${Me.RaidMarkNPC[1].ID}
        /if (${_call.Equal[return]}) {
          /return ${targetData.ID}
        }


      | raid assist 2
      } else /if (${Select[${maCC.Find[stAssistMode].Value},2]} && ${Raid.MainAssist[2].ID} && !${Raid.MainAssist[2].Dead}) {
        DEBUG ${sep}set smart${sep}\a-w2\ax
        /vardata assistData Spawn[${Raid.MainAssist[2].ID}]
        /vardata targetData Spawn[${Me.RaidAssistTarget[2].ID}]
        /varset stAssTarID ${Me.RaidAssistTarget[2].ID}
        /if (${_call.Equal[return]}) {
          /return ${targetData.ID}
        }

      | raid mark 2  
      } else /if (${Select[${maCC.Find[stAssistMode].Value},m2]}) {
        DEBUG ${sep}set raid mark${sep}\a-wm1\ax
        /vardata targetData Spawn[${Me.RaidMarkNPC[2].ID}]
        /varset stAssTarID ${Me.RaidMarkNPC[2].ID}
        /if (${_call.Equal[return]}) {
          /return ${targetData.ID}
        }


      | raisd assist 3
      } else /if (${Select[${maCC.Find[stAssistMode].Value},3]} && ${Raid.MainAssist[3].ID} && !${Raid.MainAssist[3].Dead}) {
        DEBUG ${sep}set smart${sep}\a-w3\ax
        /vardata assistData Spawn[${Raid.MainAssist[3].ID}]
        /vardata targetData Spawn[${Me.RaidAssistTarget[3].ID}]
        /varset stAssTarID ${Me.RaidAssistTarget[3].ID}
        /if (${_call.Equal[return]}) {
          /return ${targetData.ID}
        }
      
      | raid mark 3  
      } else /if (${Select[${maCC.Find[stAssistMode].Value},m3]}) {
        DEBUG ${sep}set raid mark${sep}\a-wm1\ax
        /vardata targetData Spawn[${Me.RaidMarkNPC[3].ID}]
        /varset stAssTarID ${Me.RaidMarkNPC[3].ID}
        /if (${_call.Equal[return]}) {
          /return ${targetData.ID}
        } 
      }     

      | skip corpses and me
      /if (${targetData.Dead} || ${targetData.ID} == ${Me.ID} || !${Select[${targetData.Type},npc]}) {
        DEBUG ${break} bad target${sep}\a-wcorpse or self\ax
        /varset assistData 0
        /varset targetData 0
        /varset stAssTarID 0
        /return ${targetData.ID}
      }

      | set actual pc target
      /if (${targetData.Distance} <= ENV && ${targetData.ID}) {
        /if (MELEE || RANGE || NUKE || DOT) {
          /if (${targetData.PctHPs} <= ${maCC.Find[stPctEngage].Value} || ${maOver.Find[swOverPctEngage].Value}) {
             DEBUG ${sep}targeting${sep}\a-wTARGET\ax
            /if (${Target.ID} != ${targetData.ID}) {
              /squelch /target id ${targetData.ID}
            }
          }
        }
      }
      /return ${targetData.ID}
    }
    

    | dannet assisting
    /if (!${lsAss.Contains[EMPTY]} && ${lsAss.Count} >= 1) {
      DEBUG ${sep}assisting${sep}\a-wdannet\ax
      
      /declare _liass listiterator local
      /vardata _liass lsAss.First.Clone
      /while (!${_liass.IsEnd}) {
        
        | dont assist yourself
        /if (${_liass.Value.Equal[${Me.DisplayName}]}) {
          /invoke ${_liass.Advance}
          /continue
        }

        | make sure target is being watched
        /if (!${DanNet[${_liass.Value}].OSet["Target.ID"]}) {
          DEBUG ${break}not observerd${sep}\a-w${_liass.Value} .. Target.ID\ax
          /invoke ${_liass.Advance}    
          /continue        
        }

        | is toon here and within range
        /if (!${Spawn[${_liass.Value}].ID} || ${Spawn[${_liass.Value}].Distance} >= ENV) {
          DEBUG ${break}not here/out of range${sep}\a-w${_liass.Value}\ax
          /invoke ${_liass.Advance}    
          /continue        
        }

        | no target or invalid target type
        /if (!${DanNet[${_liass.Value}].Observe["Target.ID"]} || !${Select[${Spawn[${DanNet[${_liass.Value}].Observe["Target.ID"]}].Type},npc]}) {
          DEBUG ${break}${_liass.Value}${sep}\a-wNo/bad Target.ID\ax
          /if (${targetData.ID}) {
            /invoke ${target[clear]}
          }
          /invoke ${_liass.Advance}    
          /continue        
        
        | set our data
        } else /if (${DanNet[${_liass.Value}].Observe["Target.ID"]}) {
          /vardata assistData Spawn[pc ${_liass.Value}]
          /vardata targetData Spawn[${DanNet[${_liass.Value}].Observe["Target.ID"]}]
          /varset stAssTarID ${DanNet[${_liass.Value}].Observe["Target.ID"]}

          /if (${targetData.ID} && ${targetData.Type.NotEqual[Corpse]} & ${Target.ID} != ${targetData.ID}) {
            /squelch /target id ${targetData.ID}
          }      

          /if (${targetData.PctHPs} > ${maCC.Find[stPctEngage].Value} && !${maOver.Find[swOverPctEngage].Value}) {
            DEBUG ${break}HP(${targetData.PctHPs}) > engage(${maCC.Find[stPctEngage].Value}) ${sep} ${targetData} ${targetData.ID}
            /invoke ${_liass.Advance}    
            /varset targetData 0
            /varset assistData 0
            /varset stAssTarID 0
            /return 0     
            /continue        
          }
                  
          DEBUG ${sep}targetData${sep}\a-w${targetData.ID}\ax
          /if (${_call.Equal[return]}) {
            /return ${targetData.ID}
          }
          
          /if ((MELEE || RANGE || NUKE || DOT) && ${targetData.Distance} <= ENV) {
            /if (${Target.ID} != ${targetData.ID} && !${targetData.Dead}) {
              /squelch /target id ${targetData.ID}
            } 
          }
          /return ${targetData.ID}
        }
        /invoke ${_liass.Advance}    
      }
      
    }


  | manual assisting
  } else /if (${maCC.Find[swManualAssist].Value} && ${_call.NotEqual[inline]}) {

    /if (!${lsAss.Contains[EMPTY]} && ${lsAss.Count} >= 1) {
      DEBUG ${sep}assisting${sep}\a-wmanual\ax
      
      | no mobs in env
      /if (!${SpawnCount[npc radius ENV PS4 targetable]} || SAFEZONE) {
        DEBUG ${break}no mobs in env rad || safezone
        /if (!${maOver.Find[swTargetClear].Value}) {
          /squelch /target clear
        }
        /varset stAssTarID 0
        /varset targetData 0
        /return 0      
      }
      
      /declare _liass listiterator local
      /vardata _liass lsAss.First.Clone
      /while (!${_liass.IsEnd}) {
        
        | dont assist yourself
        /if (${_liass.Value.Equal[${Me.DisplayName}]}) {
          /invoke ${_liass.Advance}
          /continue
        }

        | spawn dead or gone?
        /if (${Spawn[${_liass.Value}].Dead} || !${Spawn[${_liass.Value}].ID}) {
          DEBUG ${break}dead or not here${sep}\a-w${_liass.Value}\ax
          /invoke ${_liass.Advance}    
          /continue 
        }       
        
        | is toon here and within range
        /if (!${Spawn[${_liass.Value}].ID} || ${Spawn[${_liass.Value}].Distance} >= ENV) {
          DEBUG ${break}not here/out of range${sep}\a-w${_liass.Value}\ax
          /invoke ${_liass.Advance}    
          /continue        
        }
        
        /squelch /assist ${_liass.Value}
        /delay 5 ${Me.AssistComplete}
        
        /while (${Target.PctHPs} > ${maCC.Find[stPctEngage].Value} && !${maOver.Find[swOverPctEngage].Value}) {
          /if (!${Target.ID}) {
            /if (!${maOver.Find[swTargetClear].Value}) {
              /squelch /target clear
            }
            /varset targetData 0
            /varset assistData 0
            /varset stAssTarID 0
            /return 0             
          }
          GETINPUT
          /if (${Target.PctHPs} <= ${maCC.Find[stPctEngage].Value}) {
            /break
          }
          /delay 5
        }
        
        /if (${Target.ID} && ${Select[${Target.Type},npc]} || ${Target.ID} != TARGET) {
          DEBUG ${sep}Assisting${sep}\a-w${_liass.Value}\ax
          /vardata assistData Spawn[${_liass.Value}].ID
          /varset stAssTarID ${Target.ID}  
          /call target set TARGET
          /vardata targetData Target
          
          /return ${targetData.ID}
        
        } else /if (${Target.ID} && ${Select[${Target.Type},npc]}) {
          DEBUG ${break}bad target type
          /if (!${maOver.Find[swTargetClear].Value}) {
            /squelch /target clear
          }
          /varset targetData 0
          /varset assistData 0
          /varset stAssTarID 0
          /return 0      
        }
        
        /invoke ${_liass.Advance}    
      }

    }
  }

/return TARGET



|***
 * note: class routine order
 * use: /call class_cycle LIST
 *  
 *  melee - /melee
 *  heal - /heal
 *  burn - class burn() routine
 *  misc - class misc_combet() routine
 *  dot - /dot
 *  nuke - /nuke
 *  debuff - /debuff
 *  item - /clickitem
 *  pet - sends pets in and swarm for active combat
 *  swarm - class specific sparm pets like bst and mage
 *  manarecover - shared mana_recover() routine
 *  alliance - *borked atm*
 *  staminarecover - shared stamina_recover() routine
 *  checkminion - pet class pet checker thingie
 *  xtarget - sorts/builds xtargets for healers
 *  taunt - taunt stuff
 *  agro - agro routines
 *  harmonious - knight harmonious routine
 *  cc - CC (mez) routine
 *  gather - aa mana gather routine
 *  mode - cycle any modes that are enabled
 *  
 ***|
sub class_cycle(string _order)

  /call set_combat_control ${maDebug.Find[decision].Value} ${targetData.ID}
 
  /declare _lico listiterator local
  /vardata _lico ${_order}.First.Clone
  /while (!${_lico.IsEnd}) {

    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

    | /heal
    /if (${_lico.Value.Equal[heal]}) {
      /call check_heal_routines
    }
 
    | /melee
    /if (${_lico.Value.Equal[melee]} && MELEE) {
      /call cast_cycle_melee ${maDebug.Find[melee].Value} GETASSIST
    }
    
    BUFFNOW
    CHECKDEFENSE
    
    BUFFTEMP
    /invoke GETASSIST

    | class burn() routine
    /if (${_lico.Value.Equal[burn]} && BURN) {
      /call burn ${maDebug.Find[burn].Value} ${targetData.ID}

    | class misc_combat() routine
    } else /if (${_lico.Value.Equal[misc]}) {
      /call misc_combat ${maDebug.Find[misc].Value} ${targetData.ID}

    | /dot
    } else /if (${_lico.Value.Equal[dot]} && DOT) {
      /call cast_cycle_dot ${maDebug.Find[dot].Value} ${targetData.ID}

    | /nuke
    } else /if (${_lico.Value.Equal[nuke]} && NUKE) {
      /if (${maChr.Find[swForceTC].Value}) {
        /call force_twincast ${maDebug.Find[twincast].Value} ${targetData.ID}
      }      
      /call cast_cycle_nuke ${maDebug.Find[nuke].Value} ${targetData.ID}

    | /debuff
    } else /if (${_lico.Value.Equal[debuff]} && !${timer_Check_deBuffs}) {
      /call check_debuff_routines ${targetData.ID}

    | /clickitems
    } else /if (${_lico.Value.Equal[item]}) {
      /call check_item_cycle ${maDebug.Find[item].Value} ${targetData.ID}

    | pet and swarm (idiot) pets
    } else /if (${_lico.Value.Equal[pet]} && (PET || SWARM)) {
      /call send_pet ${maDebug.Find[sic].Value} ${targetData.ID}

    | class specific swarm pets like mage and beast
    } else /if (${_lico.Value.Equal[swarm]} && SWARM) {
      /call cast_temp_pet ${maDebug.Find[temp].Value} ${targetData.ID} "${maChr.Find[stNukeServant].Value}"
      /if (BARD) {
        /call cast_servant_swarm ${maDebug.Find[temp].Value} ${targetData.ID}
      }

    | alliance
    } else /if (${_lico.Value.Equal[alliance]} && ${maCC.Find[swAlliance].Value} && !${timer_Check_Alliance}) {
      /call alliance FALSE

    | shared mana_recovery routine
    } else /if (${_lico.Value.Equal[manarecover]} && ${maChr.Find[stPctManaRecovery].Value}) {
      /call do_mana_recovery ${maDebug.Find[manarecover].Value}
      
      | bst paragon checks
      /if (${Me.Class.ShortName.Equal[BST]}) {
        /if (!${timer_Check_Para}) /call check_mana ${maDebug.Find[para].Value}
      }
 
    | shares stamina_recovery routine
    } else /if (${_lico.Value.Equal[staminarecover]} && ${lsClassMelee.Contains[${Me.Class.ShortName}]}) {
      /call cast_stamina_recovery ${maDebug.Find[rest].Value}

    | minion checking for pet classes
    } else /if (${_lico.Value.Equal[checkminion]} && PET) {
      /if (!${check_engaged[FALSE]} && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
  
    | xtarget checking/building
    } else /if (${_lico.Value.Equal[xtarget]} && ${maHeal.Find[swHealXTarget].Value}) {
      /if (!${timer_Check_Xtarget_Build}) /call check_xtarget_build FALSE
      
    | taunt classes
    } else /if (${_lico.Value.Equal[taunt]}) {
      /invoke ${do_taunt[${maDebug.Find[taunt].Value}, ${targetData.ID}]}
      
    | agro routines
    } else /if (${_lico.Value.Equal[agro]} && AGRO) {
      /call get_agro ${maDebug.Find[agro].Value} ${targetData.ID}

    | knight harmonious routine
    } else /if (${_lico.Value.Equal[harmonious]} && ${maChr.Find[swHarmonious].Value}) {
      /if (!${timer_Check_Harmonious}) /call check_harmonious ${maDebug.Find[harmonious].Value}

    | cc routine
    } else /if (${_lico.Value.Equal[cc]} && ${maCC.Find[swCombatControl].Value}) {
      /call control_routine ${maDebug.Find[mez].Value} Mez
 
    | aa mana gather routine
    } else /if (${_lico.Value.Equal[gather]} && ${maChr.Find[stPctGather].Value} && ${Me.PctMana} <= ${maChr.Find[stPctGather].Value}) {
      /call check_gather ${maDebug.Find[gather].Value}

    | bard incombat singing
    } else /if (${_lico.Value.Equal[pulseic]}) {
      /call pulse_cycle ${maDebug.Find[pulse].Value} GETASSIST pulseincombat

    | rest routine
    } else /if (${_lico.Value.Equal[rest]} && PCTREST) {
      /if (!PULL || (PULL && !${SpawnCount[npc radius ${maEnv.Find[stRestRadius].Value} PS4]})) {
        /call do_rest ${maDebug.Find[rest].Value}
      }
      
    | modes
    } else /if (${_lico.Value.Equal[mode]} && !${timer_Check_Mode}) {
      /call check_mode ${maDebug.Find[mode].Value}
         
    }

    CHECKFADE
    CHECKREZ
    CHECKTIE
    GETOVERHERE
    BUFFTEMP

    /invoke ${_lico.Advance}
  }

/return TRUE



|***
 * note: checks heal routines by class
 * use: /call check_heal_routines
 ***|
sub check_heal_routines()

  /declare _liho listiterator local
  /vardata _liho lsOrderHeal.First.Clone
  /while (!${_liho.IsEnd}) {

    | heal: curing
    /if (${_liho.Value.Equal[cure]} && !${timer_Check_Cure} && (${maCure.Find[swCureSelf].Value} || ${maCure.Find[swCureGroup].Value} || ${maCure.Find[swCureCrew].Value})) { 
      /call check_cure ${maDebug.Find[cure].Value}

    | heal: target of target
    } else /if (${_liho.Value.Equal[tot]} && ${lsClassHealToT.Contains[${Me.Class.ShortName}]} && ${maHeal.Find[swHealToT].Value}) { 
      /call check_heal ${maDebug.Find[tot].Value} ToT

    | heal: self
    } else /if (${_liho.Value.Equal[self]} && ${maHeal.Find[swHealSelf].Value}) { 
      /call check_heal ${maDebug.Find[self].Value} Self

    | heal: DanNet
    } else /if (${_liho.Value.Equal[dannet]} && ${maHeal.Find[swHealDanNet].Value}) { 
      /call check_heal ${maDebug.Find[dannet].Value} DanNet

    | heal: ch (CLERIC ONLY)
    } else /if (${_liho.Value.Equal[ch]} && ${Select[${Me.Class.ShortName},CLR]} &&  ${Group.MainTank.PctHPs} <= ${maHeal.Find[stPctCH].Value}) { 
      /call check_heal ${maDebug.Find[ch].Value} ch

    | heal: group
    } else /if (${_liho.Value.Equal[group]} && ${maHeal.Find[swHealGroup].Value} && ${Group.Members}) { 
      /call check_heal ${maDebug.Find[group].Value} Group

    | heal: extended target lists
    } else /if (${_liho.Value.Equal[xtarget]} && ${maHeal.Find[swHealXTarget].Value}) { 
      /call check_heal ${maDebug.Find[xt].Value} XTarget

    | heal: pets
    } else /if (${_liho.Value.Equal[pet]} && ${maHeal.Find[swHealPet].Value}) { 
      /call check_heal ${maDebug.Find[pet].Value} Pet

    }
 
    /invoke ${_liho.Advance}
  }


  CHECKREZ

/return TRUE



|***
 * note: options/controls for plugin MQ2Grind
 * use: not for you
 ***|
sub plugin_grind(string _option)

  /if (${_option.Equal[combat]}) {
    /invoke ${maCC.Add[swValidAssTarID,TRUE]}
    /invoke ${maCC.Add[swSetCombat,TRUE]}
    /invoke ${maCC.Add[swSetForced,TRUE]}   
    /call class_main   
    /return TRUE 
  }

/return FALSE



|***
 * note: looks for a buff on you
 * use: thats it.. if buff is there, returns TRUE
 ***|
sub check_no_cast_buff(bool _debug, string _cast)
  DEBUG \atcheck_no_cast_buff\ax
  
  | check entire no cast buff list
  /if (${_cast.Equal[lsNoCastBuff]}) {
    /declare _linc listiterator local
    /vardata _linc lsNoCastBuff.First.Clone
    /while (!${_linc.IsEnd}) {
      /if (${Me.Buff[${_linc.Value}].ID}) {
        /return TRUE
      }
      /if (${Me.Song[${_linc.Value}].ID}) {
        /return TRUE
      }
      /invoke ${_linc.Advance}
    }
    /return FALSE

  | check single buff
  } else {
    /if (${Me.Buff[${_cast}].ID}) {
      /return TRUE
    }
    /if (${Me.Song[${_cast}].ID}) {
      /return TRUE
    }
  }

/return FALSE



|***
 * note: casting data
 * use: /call cast_data DEBUG [name of thing to figure out]
 * use: always return bool, cast is ready or not..
 ***|
sub cast_data(bool _debug, string _type)
  DEBUG \atcast_data\aw(\a-w"${_type}"\ax)

  /if (${Defined[spellData]}) {
    /deletevar spellData
  }
  
  /declare spellData spell outer
  /invoke ${maCD.Add[ready,FALSE]}
  /invoke ${maCD.Add[hasspa,FALSE]}
  
  /declare _lshs listiterator local
  /declare _ligt listiterator local
  /vardata _ligt lsOrderCastType.First.Clone
  /while (!${_ligt.IsEnd}) {

  | Alternate Abilities
    /if (${Select[${_ligt.Value},alt]}) {
      /if (${Me.AltAbility[${_type}]} && !${Me.AltAbility[${_type}].Passive}) {
        DEBUG ${sep}_type${sep}\agALT\ax
        /vardata spellData Spell[${Me.AltAbility[${_type}].Spell.ID}]
        /invoke ${maCD.Add[type,ALT]}
        /invoke ${maCD.Add[name,${_type}]}
        /invoke ${maCD.Add[id,${Me.AltAbility[${_type}].ID}]}
        /if (${Me.AltAbilityReady[${_type}]}) {
          /invoke ${maCD.Add[ready,TRUE]}
        }
        /break
      }
      
  | Spells
    } else /if (${Select[${_ligt.Value},spell]}) {
      /if (${Me.Book[${Spell[${_type}].RankName}]}) {
        DEBUG ${sep}_type${sep}\agSPELL\ax
        /invoke ${maCD.Add[type,SPELL]}
        /invoke ${maCD.Add[name,${_type}]}
        /invoke ${maCD.Add[id,${Me.Spell[${_type}].ID}]}
        /vardata spellData Spell[${_type}]
        /if (${Me.SpellReady[${spellData.RankName}]}) {
          /invoke ${maCD.Add[ready,TRUE]}
        }
        /break
      }

  | Songs
    } else /if (${Select[${_ligt.Value},pulse]} && BARD) {
      /if (${Me.Book[${Spell[${_type}].RankName}]}) {
        DEBUG ${sep}_type${sep}\agPULSE\ax
        /vardata spellData Spell[${_type}]
        /invoke ${maCD.Add[type,PULSE]}
        /invoke ${maCD.Add[name,${_type}]}
        /invoke ${maCD.Add[id,${Me.Spell[${_type}].ID}]}
        /if (${Me.SpellReady[${spellData.RankName}]}) {
          /invoke ${maCD.Add[ready,TRUE]}
        }
        /break
      }

  | Melee Discs
    } else /if (${Select[${_ligt.Value},disc]}) {
      /if (${Me.CombatAbility[${Spell[${_type}].RankName}]}) {
        DEBUG ${sep}_type${sep}\agDISC\ax
        /vardata spellData Me.CombatAbility[${Me.CombatAbility[${Spell[${_type}].RankName}]}]
        /invoke ${maCD.Add[type,DISC]}
        /invoke ${maCD.Add[name,${_type}]}
        /if (${Me.CombatAbilityReady[${spellData}]}) {
          /invoke ${maCD.Add[ready,TRUE]}
        }
        /break
      }

  | Items
    } else /if (${Select[${_ligt.Value},item]}) {
      /if (${FindItem[=${_type}].ID}) {
        DEBUG ${sep}_type${sep}\agITEM\ax
        /vardata spellData FindItem[=${_type}].Spell
        /invoke ${maCD.Add[type,ITEM]}
        /invoke ${maCD.Add[name,${_type}]}
        /invoke ${maCD.Add[id,${FindItem[=${_type}].ID}]}
        /invoke ${maCD.Add[clickid,${FindItem[=${_type}].Clicky.SpellID}]}
        /invoke ${maCD.Add[clickname,${Spell[${FindItem[=${_type}].Clicky.SpellID}].Name}]}
         /if (${Me.ItemReady[=${_type}]}) {
          /invoke ${maCD.Add[ready,TRUE]}
        }
        /break
      }
    
  | Abilities (bash, kick, so on)
    } else /if (${Select[${_ligt.Value},ability]}) {
      DEBUG ${sep}_type${sep}\a-wABILITY\ax
      /if (${Me.AbilityReady[${_type}]}) {
        /invoke ${maCD.Add[ready,TRUE]}
        /break
      }  
      
    }  

    /invoke ${_ligt.Advance}
  }

  /vardata _lshs lsHasSPA.First.Clone
  /while (!${_lshs.IsEnd}) {
    /if (${spellData.HasSPA[${_lshs.Value}]}) {
      /invoke ${maCD.Add[hasspa,TRUE]}
      /break
    }
    /invoke ${_lshs.Advance}
  }

  DEBUG ${sep}cast_data returning${sep}\a-w${maCD.Find[ready].Value}\ax
/return ${maCD.Find[ready].Value}



|***
 * note: this casts things. it doesn't care what it casts, but it will cast it.
 * note:
 * note: it is a requirement to prep the cast sub with /call cast_data || ${cast_data[]}
 * note: this is setting up the outer variable spellData and all the data that spell type needed
 * note:
 * use: /call cast DEBUG [spawnID] [sub to check on the way] [ENV override]
 ***|
sub cast(bool _debug, spawn _spawn, string _sub, int _rangeOverride)
  DEBUG \atcast\ax(\a-wtID:${_spawn.ID}, sub:${_sub}, rngOver:${_rangeOverride}\ax) (\a-r${spellData}\ax)
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETOVERHERE

  | critical errors
  /if (!${maCD.Find[id].Value} && !${Select[${maCD.Find[type].Value},DISC]}) {
    /invoke ${out[0, "${error}${sep}cast${sep}\a-w${maCD.Find[id].Value}, ${_spawn.ID}, ${_sub}\ax)"]}
    /delay 5
    /return FALSE
  }

  | generic validations
  /if (CHECKINVIS && ${_sub.NotEqual[ignoreall]}) {
    DEBUG ${break}invis
    /return FALSE
  }

  /if (!AUTO && ${swIsMeDead}) {
    /varset swIsMeDead FALSE
    DEBUG ${sep}passed swIsMeDead
  }

  | are we moving? and not a bard?
  /if (!${Select[${_sub},ignoremovement]}) {
    /if (!BARD) {
      /if (${Me.Moving}) {
        DEBUG ${sep}break cast.moving
        /return FALSE
      }
    }
  }
  
  | correct the range if needed
  /declare _range int local
  /if (${Range.Between[1,9999999:${_rangeOverride}]}) {
    DEBUG ${sep}setting max range to${sep}\a-w${_rangeOverride}\ax
    /varset _range ${_rangeOverride}
  } else {
    /varset _range ENV
  }
  
  | is the issued target still a thing???
  /if (${_spawn.ID} != 0 && !${Select[${_sub},skiptargetcheck]}) {
    /if (!${_spawn.ID} || ${_spawn.Dead}) /return FALSE
    /if (${_spawn.Distance} > ${_range}) /return FALSE
  }

  /declare _castLoop int local ${maEntropy.Find[stCastLoop].Value}
  /declare _tiCastTime timer local 0
  /declare _castTime int local 0
  /invoke ${maData.Add[stCastReturn,FALSE]}
  


  | alt abilities
  /if (${Select[${maCD.Find[type].Value},ALT]}) {
    DEBUG ${sep}alt${sep}${Me.AltAbility[${maCD.Find[id].Value}].Name} (\a-r${spellData}\ax)

    /if (${spellData.Mana}) {
      /if (${Me.CurrentMana} < ${spellData.Mana}) {
        /return FALSE
      }
    }

    /if (${_sub.NotEqual[ignoreall]}) {
      /if (${Me.Buff[${spellData}].ID} || ${Me.Song[${spellData}].ID}) {
        /return FALSE
      }
    }

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_spawn.ID}  && !${Select[${_sub},skiptargetcheck]}) {
      /call target set ${_spawn.ID}
    }

    /if (${_spawn.ID} == 0) {
      /invoke ${out[2, "AA${sep}${sp}${Me.AltAbility[${maCD.Find[id].Value}].Name}\ax"]}
    } else /if (${_spawn.ID} != 0 && ${Target.ID}) {
      /invoke ${out[2, "AA${sep}${sp}${Me.AltAbility[${maCD.Find[id].Value}].Name}\ax ${dot} ${con_color[FALSE, ${_spawn.ID}]}\ax"]}
    }

    /while (${_castLoop} > 0) {
      DEBUG ${sep}altCastLoop${sep}${spellData.Name}

      /if (${_spawn.ID} && ${Target.ID} != ${_spawn.ID}) {
        /break
      }
 
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      /alt act ${Me.AltAbility[${maCD.Find[id].Value}].ID}
      /delay 5 ${Me.Casting.ID}

      | set delays for casting. GTFO out of this as fast as possible
      /varset _tiCastTime ${spellData.MyCastTime}

      /while (${_tiCastTime} > 0) {
        /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
        /if (${_spawn.ID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
          /if (${_spawn.Dead} || !${_spawn.ID}) {
            DEBUG ${sep}spell.target dead.break
            /varset _tiCastTime 0
            /stopcast
            /return FALSE
          }
        }

        /if (!${Me.Casting.ID}) {
          /varset _tiCastTime 0
        }

        /delay 3

      }

      | delay for longer casts if there is any, cooldowns or open spell cast windows
      DEBUG ${sep}altloop.casting,window,cooldown
      /while (${Me.Casting.ID} && ${Window[CastingWindow].Open}) {
        /delay 3
      }

      /if (${Select[${_sub},ignoreall]}) {
        /return TRUE
      }
      
      DEBUG ${sep}cast return${sep}\ay${maData.Find[stCastReturn].Value}\ax

      /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,TAKEHOLD,DISTRACTED,RESISTED,FALSE]}) {
        /varset _castLoop 0
        /if (BARD) {
          /call pulse_stop
        }
        /break
      }

      /if (${Select[${maData.Find[stCastReturn].Value},OVERWRITTEN]}) {
        /varset _castLoop 0
        /if (BARD) {
          /call pulse_stop
        }
        /break
      }

      /if (${Select[${maData.Find[stCastReturn].Value},NOTREADY,OUTOFMANA]}) {
        /varcalc _castLoop ${_castLoop}-1
        /delay 5
      }

    }
    /return TRUE

  | spells
  } else /if (${Select[${maCD.Find[type].Value},SPELL]}) {
    DEBUG ${sep}\agSPELL\ax${sep}${spellData}

    | buff on me i shouldnt cast with?
    /if (${check_no_cast_buff[${_debug}, lsNoCastBuff]}) {
      | /return FALSE
    }
    
    /if (${lsClassCast.Contains[${Me.Class.ShortName}]}) {
      /if (${Me.PctMana} < 1) {
        /return FALSE
      }
    }
    
    | silence SPA
    /if (${Me.SPA[96]}) {
      /invoke ${maData.Add[stCastReturn,DISTRACTED]}
      /return FALSE
    }

    | am i casting a spell?
    /if (${Me.Casting.ID}) {
      DEBUG ${break}casting
      /return FALSE
    }

    | global cooldown
    /if (${Me.SpellInCooldown}) {
      DEBUG ${break}spells are in cooldown
      /return FALSE
    }

    | have it?
    /if (!${Me.Book[${spellData.RankName}]}) {
      DEBUG ${break}dont have spell (\a-r${spellData.RankName}\ax)
      /return FALSE
    }
    
    | gem in cooldown?
    /if (${Me.GemTimer[${spellData.RankName}]}) {
      DEBUG ${break}gem in cooldown
      /return FALSE
    }

    | enough manna to cast?
    /if (${Me.CurrentMana} < ${spellData.Mana}) {
      DEBUG ${break}not enough mana#{sep}${Me.CurrentMana} < ${spellData.Mana}
      /return FALSE
    }

    | dont fail buffs so they can be memed when needed
    /if (!${Me.SpellReady[${spellData.RankName}]} && !${Select[${spellData.Category},Heals]}) {
      /declare _pass bool local FALSE
      DEBUG ${sep}SpellType${sep}\a-w${spellData.SpellType}\ax
      /if (${lsFreePassSpells.Contains[${spellData.SpellType}]}) {
        /varset _pass TRUE
      }

      /if (!${_pass}) {
        DEBUG ${sep}FREE mem pass \arfailure\ax
        /return FALSE
      }
    }

    | check cast range
    /if (${spellData.Range}) {
      /if (${_spawn.Distance} > ${spellData.MyRange}) {
        DEBUG ${break}excceds range${sep}${_spawn.Distance} > ${spellData.MyRange}
        /return FALSE
      }
    }
    | check AE range
    /if (${Select["${spellData.TargetType}",Group V2]} && ${spellData.AERange}) {
      /if (${_spawn.Distance} > ${spellData.AERange}) {
        DEBUG ${break}exceeds AE range${sep}${_spawn.Distance} > ${spellData.AERange}
        /return FALSE
      }
    }
    
    | check for stacking issues
    /if (${_spawn.ID} == ${Me.ID} || ${_spawn.ID} == 0) {
      /if (${Select[${spellData.SpellType},Beneficial]} && !${Select[${spellData.Category},Heals,Combat Inates]}) {
        /if (!${spellData.WillLand}) {
          /return FALSE
        }
        /if (${Me.Buff[${spellData}].ID} || ${Me.Song[${spellData}].ID}) {
          /return FALSE
        }
      }
    }


    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_spawn.ID} && !${Select[${_sub},skiptargetcheck]}) {
      /call target set ${_spawn.ID}
    }

    | types vs zone control
    /if (!${Select[${spellData.SpellType},Beneficial]}) {
      /if (${lsZoneNoDisease.Contains[${Target.DisplayName}]} && ${Select[${spellData.ResistType},Disease]}) /return FALSE
      /if (${lsZoneNoMagic.Contains[${Target.DisplayName}]} && ${Select[${spellData.ResistType},Magic]}) /return FALSE
      /if (${lsZoneNoPoison.Contains[${Target.DisplayName}]} && ${Select[${spellData.ResistType},Poison]}) /return FALSE
      /if (${lsZoneNoFire.Contains[${Target.DisplayName}]} && ${Select[${spellData.ResistType},Fire]}) /return FALSE
      /if (${lsZoneNoCold.Contains[${Target.DisplayName}]} && ${Select[${spellData.ResistType},Cold]}) /return FALSE
      /if (${lsZoneNoCorruption.Contains[${Target.DisplayName}]} && ${Select[${spellData.ResistType},Corruption]}) /return FALSE
    }

    | check any subs? yes.. no.. maybe??
    /if (${Select[${_sub},breakonhealpct]} && ${maHeal.Find[swBreakHealPCT].Value}) {
      DEBUG ${sep}${_sub}${sep}\a-wbreakonhealpct\ax
      /if (${_spawn.PctHPs} >= ${maHeal.Find[stHealPoint${_spawn.Class.ShortName}].Value}*.PCTHO) {
        /stopcast
        /return TURE
      }
    }
    
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

    | if a gom procs and a spell is ready, we hijack the existing cast and use the GOM spell in its place
    /if (${_spawn.ID} != 0 && ${Target.ID} && ${Me.Song[Gift of Mana].ID} && !${Select[${Me.Class.ShortName},CLR]} && COMBAT) {

      | remember original spell just incase
      /declare _tmpspell spell
      /vardata _tmpspell spellData

      /declare _ligom listiterator local
      /vardata _ligom lsGoM.First.Clone
      /while (!${_ligom.IsEnd}) {
        
        | exit gom cast if not set
        /if (${Select[${_ligom.Value},empty]}) /break
        
        | get spell setup
        /call cast_data ${_debug} "${_ligom.Value}"
        
        | if not ready reset to current spell and tr again if another is available
        /if (!${Macro.Return}) {
          /vardata spellData _tmpspell
          /invoke ${_ligom.Advance}
          /continue
          
        | if it is ready, take the new set data and run with it
        } else /if (${Macro.Return}) {
          /break
        }
        
        /invoke ${_ligom.Advance}
      }
    }

    /if (${Window[SpellBookWnd].Open}) {
      /invoke ${Window[SpellBookWnd].DoClose}
    }

    | mem spell if needed??
    /if (!${Me.Gem[${spellData}]}) {
      /call set_spell_to ${_debug} TRUE "${spellData.RankName}" ${maEnv.Find[stBuffGem].Value} TRUE
    }

    /if (${_spawn.Dead} && ${_sub.NotEqual[skiptargetcheck]}) {
      /return FALSE
    }
    
    | is the spell memed
    /if (${Me.Gem[${spellData.RankName}]}) {
      
      /declare _countFizzle int local 0
      
      | echos
      /if (${_spawn.ID} == 0) {
        /invoke ${out[3, "Cast${sep}${sp}${spellData.RankName}\ax"]}
      } else /if (${_spawn.ID} != 0 && ${Target.ID} && ${Target.ID} == ${_spawn.ID}) {
        /invoke ${out[3, "Cast${sep}${sp}${spellData.RankName}\ax ${dot} ${con_color[FALSE, ${_spawn.ID}]}\ax"]}
      }

      /while (${_castLoop} > 0) {
        /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}

        /if (CHECKINVIS && ${_sub.NotEqual[ignoreall]}) {
          DEBUG ${sep}spell.cr:DISTRACTED(invis).break
          /varset _castLoop 0
          /invoke ${maData.Add[stCastReturn,DISTRACTED]}
          /break
        }

        /cast ${Me.Gem[${spellData.RankName}]}
        /delay 1
        /delay 1s ${Me.Casting.ID}

        | set delays for casting. GTFO out of this as fast as possible
        /varset _castTime ${Math.Calc[${spellData.MyCastTime}+60]}
        DEBUG ${sep}_castTime${sep}\a-w${_castTime}\ax

        | /invoke ${set_timer[${_debug}, _castTime, ${_castTime}]}
        /varset _tiCastTime ${_castTime}

        /while (${_castTime} > 0) {
          /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
          /if (${_spawn.ID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
            /if (${_spawn.Dead} || !${_spawn.ID}) {
              DEBUG ${sep}spell.target dead.break
              /stopcast
              /return FALSE
            }

            | break on heal % check
           /if (${Select[${_sub},breakonhealpct]} && ${maHeal.Find[swBreakHealPCT].Value}) {
              DEBUG ${sep}${_sub}${sep}\a-wbreakonhealpct\ax
              /if (${_spawn.PctHPs} >= ${maHeal.Find[stHealPoint${_spawn.Class.ShortName}].Value}*.PCTHO) {
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
          /if (!${Me.Moving}) /break
          /delay 3
        }

        /doevent
        DEBUG ${sep}cast return${sep}\ay${maData.Find[stCastReturn].Value}\ax

        /if (${Select[${_sub},ignoreall]}) {
          /return TRUE
        }

        | notready / cant cast
        /if (${Select[${maData.Find[stCastReturn].Value},DISTRACTED,NOTREADY,CANNOTSEE,OUTOFMANA,INTERRUPTED]}) {
          DEBUG ${sep}spell.cr:DISTRACTED,NOTREADY,CANNOTSEE,OUTOFMANA,INTERRUPTED.\arbreak\ax
          /varset _castLoop 0
          /if (BARD) {
            /call pulse_stop
          }
          /break

        | fizzle
        } else /if (${Select[${maData.Find[stCastReturn].Value},FIZZLE]}) {
          DEBUG ${sep}spell.cr:FIZZLE.\agretry\ax
          /if (${maEntropy.Find[stCountFizzleRetry].Value}) {
            /delay ${Math.Calc[${Me.Gem[${spellData.RankName}].RecastTime.Seconds} + 3]}s 
            /varcalc _castLoop ${_castLoop} + 1

            /if (${_castLoop} >= ${maEntropy.Find[stCountFizzleRetry].Value}) {
              /varset _castLoop 0
              /if (BARD) {
                /call pulse_stop
              }
              /break
            }
          }
          
        | good
        } else /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,BLOCKED,RESISTED,IMMUNE,FALSE]}) {
          DEBUG ${sep}spell.cr:SUCCESS,BLOCKED,RESISTED,IMMUNE.\agbreak\ax
          /varset _castLoop 0
          /if (BARD) {
            /call pulse_stop
          }
          /break
        }
      }
    }
    /return TRUE


  | discs
  } else /if (${Select[${maCD.Find[type].Value},DISC]}) {

    DEBUG ${sep}\agDISC\ax${sep}${spellData}${sep}${spellData.ID}
    /delay 1

    /if (${Me.CurrentEndurance} < ${spellData.EnduranceCost}) {
      DEBUG ${break}.CurrentEndurance
      /return FALSE
    }
    /if (${_spawn.Distance} > ${spellData.MyRange}) {
      DEBUG ${break}.MyRange
      /return FALSE
    }
    /if (${Me.Song[${spellData}].Duration}) {
      DEBUG ${break}Me.Song.Duration
      /return FALSE
    }
    /if (${Me.Buff[${spellData}].Duration}) {
      DEBUG ${break}Me.Song.Duration
      /return FALSE
    }

    | stacking
    /if (!${Spell[${spellData.ID}].WillLand}) {
      DEBUG ${break}Spell[${spellData.ID}].WillLand
      /return FALSE
    }

    | CHECK DISK DURATION VS ACTIVE DISC FOR STACKING ISSUES/ BLOCKED DISCS
    /if (${Me.ActiveDisc.ID} == ${spellData.ID}) {
      DEBUG ${break}ActiveDisc.ID == spellData.ID
      /return FALSE
    }

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_spawn.ID} && !${Select[${_sub},skiptargetcheck]}) {
      /call target set ${_spawn.ID}
    }

    /if (${_spawn.ID} == 0) {
      /invoke ${out[4, "Disc${sep}${sp}${Spell[${spellData}]}\ax"]}
    } else /if (${_spawn.ID} != 0 && ${Target.ID}) {
      /invoke ${out[4, "Disc${sep}${sp}${Spell[${spellData}]}\ax ${dot} ${con_color[FALSE, ${_spawn.ID}]}\ax"]}
    }
    
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    /disc ${spellData.ID}

    /delay 5
    /while (${Me.Casting.ID} && ${Window[CastingWindow].Open}) {
      /delay 5
    }

    /if (BARD) {
      /call pulse_stop
    }
    /return TRUE

  | items
  } else /if (${Select[${maCD.Find[type].Value},ITEM]}) {
    DEBUG ${sep}\agITEM\ax${sep}${FindItem[${maCD.Find[id].Value}].Name}

    /if (${_spawn.ID} == 0 || ${_spawn.ID} == ${Me.ID}) {
      DEBUG here

      | illusion check
      /if (${FindItem[${maCD.Find[id].Value}].Type.Equal[Illusion]} && !${Me.Mount.ID}) {
        /if (${Me.Buff[${FindItem[${maCD.Find[id].Value}].Illusion}].ID} || ${Me.Song[${FindItem[${maCD.Find[id].Value}].Illusion}].ID}) {
          DEBUG ${sep}I have the illusion
          /return FALSE
        }
      

      | mount check
      } else /if (${FindItem[${maCD.Find[id].Value}].Type.Equal[Mount]}) {
        DEBUG ${sep}mount check
        
        /declare _foundmount bool local FALSE
        /declare _foundstat bool local FALSE

        | mount buff
        /if (${Me.Mount.ID} && ${maBuff.Find[swBuffKeepMount].Value}) {
          DEBUG ${sep}i have the mount
          /varset _foundmount TRUE
        }        

        | mount stat buff
        /if (${Me.Buff[${FindItem[${maCD.Find[id].Value}].Illusion}].ID}) {
          DEBUG ${sep}i have the mount stats
          /varset _foundstat TRUE
        }
        
        DEBUG mount:${_foundmount} && stat:${_foundstat}
        /if (${maBuff.Find[swBuffKeepMount].Value}) {
          /if (${_foundmount} && ${_foundstat}) {
            /return FALSE
          }
        } else /if (!${maBuff.Find[swBuffKeepMount].Value}) {
          /if (${_foundstat}) {
            /return FALSE
          }
        }


      | familiar check
      } else /if (${FindItem[${maCD.Find[id].Value}].Type.Equal[Familiar]}) {
        /if (${Me.Buff[${FindItem[${maCD.Find[id].Value}].Illusion}].ID}) {
          /return FALSE
        }

      | catch clickitem check
      } else /if (${Me.Buff[${Spell[${FindItem[${maCD.Find[id].Value}].Clicky.SpellID}].Name}].ID} || ${Me.Song[${Spell[${FindItem[${maCD.Find[id].Value}].Clicky.SpellID}].Name}].ID}) {
        /return FALSE
      }

      | have the base buff/song      
      /if (${Me.Buff[${FindItem[${maCD.Find[id].Value}].Spell}].ID} || ${Me.Song[${FindItem[${maCD.Find[id].Value}].Spell}].ID}) /return FALSE
      
      | distance     
      /if ( ${Spell[${FindItem[${maCD.Find[id].Value}].Spell}].Range} != 0) {
        /if (${_spawn.Distance} > ${Spell[${FindItem[${maCD.Find[id].Value}].Spell}].MyRange}) /return FALSE
      }
    }

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_spawn.ID} && !${Select[${_sub},skiptargetcheck]}) {
      /call target set ${_spawn.ID}
    }

    /if (${_spawn.ID} == 0 || ${_spawn.ID} == ${Me.ID}) {
      /if (${lsGrain.Contains[7]}) {
        OUT Item${sep}${sp}${FindItem[${maCD.Find[id].Value}].Name}\ax
      }
    } else /if (${_spawn.ID} != 0 && ${Target.ID}) {
      /if (${lsGrain.Contains[7]}) {
        OUT Item${sep}${sp}${FindItem[${maCD.Find[id].Value}].Name}\ax ${dot} ${con_color[FALSE, ${_spawn.ID}]}
      }
    }

    /while (${_castLoop}) {
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      /useitem "${FindItem[${maCD.Find[id].Value}].Name}"
      /delay 2

      /if (${_spawn.ID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
        /if (${_spawn.Dead} || !${_spawn.ID}) {
          DEBUG ${sep}item.target dead.break
          /stopcast
          /return FALSE
        }

        | break on heal % check
        /if (${_sub.Equal[breakonhealpct]} && ${maHeal.Find[swBreakHealPCT].Value}) {
          DEBUG ${sep}${_sub}${sep}\a-wbreakonhealpct\ax
          /if (${_spawn.PctHPs} >= ${maHeal.Find[stHealPoint${_spawn.Class.ShortName}].Value}*.PCTHO) {
            /stopcast
            /return TURE
          }
        }
      }
      
      /if (!${Me.ItemReady[${maCD.Find[id].Value}]}) {
        /return TRUE
      }

      | get out if its a no cast click item
      /if (!${FindItem[${maCD.Find[id].Value}].CastTime}) {
        | DEBUG NO CAST TIME
        /if (BARD) /call pulse_stop
        /invoke ${maData.Add[stCastReturn,SUCCESS]}
        /return


      } else {
        | DEBUG CAST TIME
        /delay 8
        /delay ${Math.Calc[${FindItem[${maCD.Find[id].Value}].CastTime.Float}+5]}s !${Me.Casting.ID}
        
        /while (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
          /delay 3
        }       
        
        GETINPUT cast_item NA|NA
        /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED,RESISTED,OUTOFMANA]}) {
          /varset _castLoop 0
          /if (BARD) /call pulse_stop
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
  } else /if (${Select[${maCD.Find[type].Value},PULSE]}) {
    DEBUG ${sep}\agPULSE\ax${sep}

    /if (${Me.CurrentMana} < ${spellData.Mana}) {
      /return FALSE
    }

    /if (${Window[SpellBookWnd].Open}) {
      /invoke ${Window[SpellBookWnd].DoClose}
    }

    | mem spell if needed??
    /if (!${Me.Gem[${spellData.RankName}]}) {
      /call set_spell_to ${_debug} TRUE "${spellData.RankName}" ${maEnv.Find[stBuffGem].Value} TRUE
    }

    | make sure we have our target if one is passed. 0 is assumed no target required
    /if (${_spawn.ID} && !${Select[${_sub},skiptargetcheck]}) {
      /call target set ${_spawn.ID}
    }

    /if (${_spawn.ID} == 0) {
      /invoke ${out[5, "Play${sep}${sp}${spellData.RankName}\ax"]}
    } else /if (${_spawn.ID} != 0 && ${Target.ID}) {
      /invoke ${out[5, "Pulse${sep}${sp}${spellData.RankName}\ax ${dot} ${con_color[FALSE, ${_spawn.ID}]}\ax"]}
    }

    /while (${_castLoop}) {
      /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
      /cast ${Me.Gem[${spellData.RankName}]}
      /delay 1
      CHECKTIE

      | bard swap mechanics
      /while (${Window[CastingWindow].Open}) {
        /if (${maChr.Find[swInstrumentSwap].Value}) {
          /if (${Me.CastTimeLeft} < ${maHard.Find[stBardSwapRemain].Value}) {
            /if (${maBardFocus.Find[${Skill[${spellData.Skill}].ID}].Value.Arg[3,|]}) {
              /if (${FindItem[${maBardFocus.Find[${Skill[${spellData.Skill}].ID}].Value.Arg[1,|]}].WornSlot[offhand]}) {
                /call Bind_command_swap "${FindItem[${maBardFocus.Find[${Skill[${Spell[${spellData.ID}].Skill}].ID}].Value.Arg[1,|]}].Name}" 14 FALSE
              } else /if (${FindItem[${maBardFocus.Find[${Skill[${spellData.Skill}].ID}].Value.Arg[1,|]}].WornSlot[mainhand]}) {
                /call Bind_command_swap "${FindItem[${maBardFocus.Find[${Skill[${Spell[${spellData.ID}].Skill}].ID}].Value.Arg[1,|]}].Name}" 13 FALSE
              }
            }
          }
        }
        | /delay 1
      }
      | /delay 1
      /doevents

      /if (${Select[${_sub},ignoreall]}) {
        /return TRUE
      }

      /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,RESISTED,IMMUNE]}) {
        /varset _castLoop 0
        /call pulse_stop
        /break
      }
      /if (${Select[${maData.Find[stCastReturn].Value},NOTREADY,DISTRACTED]}) {
        /delay 1
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
 * note: this checks if you have a buff on you
 * note: it is a requirement to prep the have_buff sub with /call cast_data || ${cast_data[]}
 * use: ${have_buff[DEBUG]}
 ***|
sub have_buff(bool _debug)
  DEBUG \athave_buff\ax()

   
  /if (${Select[${maCD.Find[type].Value},alt]}) {
    /if (${Me.Buff[${maCD.Find[name].Value}].ID} || ${Me.Song[${maCD.Find[name].Value}].ID} || ${Me.Buff[${spellData.Name}].ID} || ${Me.Song[${spellData.Name}].ID}) {
      DEBUG ${break}have buff/song (\a-r${spellData.Name}\ax)
      /return TRUE
    }
    
   
  } else /if (${Select[${maCD.Find[type].Value},spell,song,item]}) {
    /if (${Me.Buff[${spellData.Name}].ID} || ${Me.Song[${spellData.Name}].ID}) {
      DEBUG ${break}have buff/song (\a-r${spellData.Name}\ax)
      /return TRUE
    }

  } else /if (${Select[${maCD.Find[type].Value},disc]}) {

  } else /if (${Select[${maCD.Find[type].Value},ability]}) {
    
  }
    
/return FALSE
  


|***
 * note: check a min/max range from a mob for hitbox adjustments
 * note: return bool TRUE if inside buffer else FALSE
 * use: ${spawn_range_buffer[spawnid]}
 ***|
sub spawn_range_buffer(spawn _spawn)

  /declare _min bool local FALSE
  /declare _max bool local FALSE
  /invoke ${maData.Add[spawnRangeSweetSpot,FALSE]}
  /invoke ${maData.Add[spawnRangeMin,${Math.Calc[(${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) - (${_spawn.MaxRangeTo}*.15)]}]}
  /invoke ${maData.Add[spawnRangeMax,${Math.Calc[(${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}) + (${_spawn.MaxRangeTo}*.09)]}]}

  /if (${_spawn.Distance} > ${maData.Find[spawnRangeMin].Value} && ${_spawn.Distance} < ${maData.Find[spawnRangeMax].Value}) {
    /invoke ${maData.Add[spawnRangeSweetSpot,TRUE]}
  }
  
/return ${maData.Find[spawnRangeSweetSpot].Value}

    

|***
 * note: Sticks you to the target
 * note: for the love of whatever you find important, don't touch this ever again...
 * use: /call spawn_stick DEBUG [target ID]
 ***|
sub spawn_stick(bool _debug, spawn _spawn)
  DEBUG \atspawn_stick\ax(\a-w${_spawn.ID}\ax)

  /if (!AUTO && !${swModeToken}) {
    DEBUG ${break}manual
    /return FALSE
  }
  
  SPAWNDEAD

  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT spawn_stick tmpID|${_spawn.ID}
  
  /invoke ${spawn_face[${_spawn.ID}]}
  /invoke ${spawn_range_buffer[${_spawn.ID}]}
  
  | position check
  /invoke ${set_data_timer[${_debug}, Check_Behind, RESTART]}
  /declare _position string local

  /if (${Select[${maHome.Find[stCombatPosition].Value},random]} && !${maData.Find[stFacingPositionCheck].Value}) {

    /if (${Me.TargetOfTarget.ID} == ${Me.ID}) {
      /return TRUE
    }
   
    /invoke ${maData.Add[stFacingPosition,${lsPosition.Item[${Math.Rand[0,${Math.Calc[${lsPosition.Count} -1]}]}]}]}
    
  } else /if (!${Select[${maHome.Find[stCombatPosition].Value},random]}) {
    /invoke ${maData.Add[stFacingPosition,${maHome.Find[stCombatPosition].Value}]}
  }

  /if (!${maData.Find[stFacingPositionCheck].Value}) {
    DEBUG ${sep}position adjustment${sep}\a-w${maData.Find[stFacingPosition].Value}\ax
    
    | adaptation to get to targets not line of sight before stick move
    /if (!${_spawn.LineOfSight}) {
      DEBUG ${sep}getting to mob..los is bad${sep}\a-w${_spawn.DisplayName}\ax
      OUT Getting to spawn${sep}\a-w${_spawn.DisplayName}\ax
      /nav ID ${_spawn.ID} |distance=${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} lineofsight=off log=${maEntropy.Find[swNavLogSpew].Value}
      /delay 1s ${Me.Moving}
      /delay 5s !${Me.Moving}
    }
    
    OUT Position${sep}\a-w${maData.Find[stFacingPosition].Value}\ax
    /stick id ${_spawn.ID} ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} snaproll ${maData.Find[stFacingPosition].Value}
    /invoke ${maData.Add[stFacingPositionCheck,TRUE]}
    /delay 1s ${Me.Moving}
    /delay 5s !${Me.Moving}
  }

  /if (!${spawn_range_buffer[${_spawn.ID}]}) {
    DEBUG ${sep}adjusting${sep}\a-wmoveback\ax
    /squelch /stick moveback ${Math.Calc[${_spawn.MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
    /delay 1s ${Me.Moving}
    /delay 5s !${Me.Moving}
  }

  /squelch /stick off

/return TRUE

   

|***
 * note: checks whos hurt, returns a count. sets most hurt: ID, Class, pct hurt
 * use: /call get_hurt DEBUG [group|pet|mypet|xtarget|tot|CH|reset] || ${get_hurt[DEBUG, group|pet|mypet|xtarget|tot|ch|reset]}
 ***|
sub spawn_hurt(bool _debug, string _type, bool _breakonGroup)
  DEBUG \atspawn_hurt\ax(\a-w${_type}\ax)
  DEBUG ${sep}using weighted HP check${sep}\a-w${maHeal.Find[swHealWeighted].Value}\ax

  /declare _count int local 0
  /vardata hurtData Me
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
      /if (${Group.Member[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}*.PCTHO) {
        /continue
      }
      /if (${Group.Member[${_count}].Distance} > ENV) {
        /continue
      }
      /if (!${Group.Member[${_count}].Present}) {
        /continue
      }
      /if (${Group.Member[${_count}].Dead}) {
        /varcalc hurtDead ${hurtDead} + 1
        /continue
      }
      /if (${Group.Member[${_count}].DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) {
        /continue
      }
      /if (${lsNoHealToon.Contains[${Group.Member[${_count}].DisplayName}]}) {
        /continue
      }

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
          /vardata hurtData Group.Member[${_count}]
        }
      } else /if (!${maHeal.Find[swHealWeighted].Value}) {
        /vardata hurtData Group.Member[${_count}]
      }

    /next _count
    /return ${hurtCount}


  | check xtarget hurt
  } else /if (${_type.Equal[xtarget]}) {
    DEBUG \a-w{_type.Equal[xtarget]}\ax
    /for _count 1 to ${Me.XTargetSlots}
      /if (!${Me.XTarget[${_count}].ID}) {
        /continue
      }
      /if (${Me.XTarget[${_count}].Dead}) {
        /continue
      }
      /if (${Me.XTarget[${_count}].PctHPs} > ${hurtData.PctHPs}) {
        /continue
      }
      /if (${Me.XTarget[${_count}].Distance} > ENV) {
        /continue
      }
      /if (${Me.XTarget[${_count}].DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) {
        /continue
      }
      /if (${lsNoHealToon.Contains[${Me.XTarget[${_count}].DisplayName}]}) {
        /continue
      }
      /if (!${Me.XTarget[${_count}].Type.Equal[PC]} && !${lsProtectOutsideGroup.Contains[${Me.XTarget[${_count}].DisplayName}]}) {
        /continue
      }
      /if (${Me.XTarget[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}*.PCTHO) {
        /continue
      }
      /varset _tmpWeight ${Float[${Math.Calc[(${Me.XTarget[${_count}].PctHPs} - .${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}) / ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}].Value}]}].Precision[5]}

      /if (${Float[${_tmpWeight}].Precision[5]} < ${Float[${_hurtWeight}].Precision[5]}) {
        /varset _hurtWeight ${Float[${_tmpWeight}].Precision[5]}
        /vardata hurtData Me.XTarget[${_count}]
        /varset hurtCount 1
      }

    /next _count
    /return ${hurtCount}


  | check pet hurt
  } else /if (${_type.Equal[pet]}) {
    /for _count 0 to ${Group}
      /if (!${Group.Member[${_count}].Pet.ID}) {
        /continue
      }
      /if (${Group.Member[${_count}].Pet.PctHPs} > ${maHeal.Find[stHealPoint${Group.Member[${_count}].Pet.Class.ShortName}].Value}) {
        /continue
      }
      /if (${Group.Member[${_count}].Pet.Distance} > ENV) {
        /continue
      }
      /if (${Group.Member[${_count}].Pet.PctHPs} >= ${hurtPctHP}) {
        /continue
      }

      /varset hurtCount 1
      /vardata hurtData Group.Member[${_count}].Pet
    /next _count
    /return ${hurtCount}


  | self only pet hurt
  } else /if (${_type.Equal[mypet]}) {
    /if (!${Me.Pet.ID}) {
      /return FALSE
    }
    
    /if (${Me.Pet.PctHPs} >= ${maHeal.Find[stHealPoint${Me.Pet.Class.ShortName}].Value}) {
      /return 0
    }
    /if (${Me.Pet.Distance} > ENV) {
      /return 0
    }

    /vardata hurtData Me.Pet
    /if (${hurtPctHP} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /return 0
    }
    /varset hurtCount 1
    /return 1

  | set self hurt
  } else /if (${_type.Equal[self]}) {
    /vardata hurtData Spawn[${Me.ID}]

    /if (${hurtPctHP} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /return 0
    }
    /varset hurtCount 1
    /return 1


  | target of target hurt
  } else /if (${_type.Equal[tot]}) {
    /vardata hurtData Spawn[${Me.TargetOfTarget.ID}]
    DEBUG ${sep}hurtData:\a-w${hurtData.ID}\ax

    DEBUG ${hurtData.PctHPs} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO
    /if (${hurtData.PctHPs} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) {
      /return 0
    }
    /vardata hurtData Spawn[${Target.ID}]
    /varset hurtCount 1
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
    /declare _spawn spawn local
    
    | each toon
    /for _count 1 to ${DanNet.PeerCount[${maEntropy.Find[stEntropyGroup_all].Value}]}

      | get the name to something usable
      /vardata _spawn Spawn[pc ${DanNet.Peers[${maEntropy.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}]
      
      /if (!${_spawn.ID} || ${_spawn.ID} == ${Me.ID}) {
        /continue
      }
      DEBUG ${sep}_spawn.DisplayName${sep}\ay${_spawn.DisplayName}\ax

      | is the observer set
      /if (!${DanNet[${_spawn.DisplayName}].ObserveSet["Me.PctHPs"]} || !${DanNet[${_spawn.DisplayName}].ObserveSet["Me.Class.ShortName"]} || !${DanNet[${_spawn.DisplayName}].ObserveSet["Me.ID"]}) {
        DEBUG ${break}observers are mission or catching up for ${_spawn.DisplayName}
        /continue
      }
      
      DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}${_spawn.DisplayName}${sep}${_spawn.ID}\ax

      /if (${DanNet[${_spawn.DisplayName}].Observe["Me.PctHPs"]} > ${maHeal.Find[stHealPoint${DanNet[${_spawn.DisplayName}].Observe["Me.Class.ShortName"]}].Value}*.PCTHO) {
        DEBUG ${break} > HealPoint ${sep} ${DanNet[${_spawn.DisplayName}].Observe["Me.PctHPs"]} > ${maHeal.Find[stHealPoint${DanNet[${_spawn.DisplayName}].Observe["Me.Class.ShortName"]}].Value}
        /continue
      } else /if (${_spawn.Distance} > ENV) {
        DEBUG ${break} > env radius ${sep} ${_spawn.Distance} > ENV
        /continue
      } else /if (${_spawn.Dead}) {
        DEBUG ${break} dead toon
        /varcalc hurtDead ${hurtDead} + 1
        /continue
      } else /if (!${_spawn.ID}) {
        DEBUG ${break} not in zone
        /continue
      } else /if (${_spawn.DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) {
        DEBUG ${break} rampage tank ${sep} ${_spawn.DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}
        /continue
      }

      | hurt running count
      /if (${DanNet[${_spawn.DisplayName}].Observe["Me.PctHPs"]} <= ${maHeal.Find[stHealPoint${DanNet[${_spawn.DisplayName}].Observe["Me.Class.ShortName"]}].Value}*.PCTHO) {
        /varcalc hurtCount ${hurtCount} + 1
        DEBUG ${sep}hurtCount +1 ${sep} ${hurtCount}
      }

      /if (${maHeal.Find[swHealWeighted].Value}) {
        DEBUG ${sep}_count${sep}\a-w${_spawn.DisplayName}\ax${sep}\a-o(${DanNet[${_spawn.DisplayName}].Observe["Me.PctHPs"]} - .${maHeal.Find[stHealPoint${DanNet[${_spawn.DisplayName}].Observe["Me.Class.ShortName"]}].Value}) / ${maHeal.Find[stHealPoint${DanNet[${_spawn.DisplayName}].Observe["Me.Class.ShortName"]}].Value}\ax
        /varset _tmpWeight ${Float[${Math.Calc[(${DanNet[${_spawn.DisplayName}].Observe["Me.PctHPs"]} - .${maHeal.Find[stHealPoint${DanNet[${_spawn.DisplayName}].Observe["Me.Class.ShortName"]}].Value}) / ${maHeal.Find[stHealPoint${DanNet[${_spawn.DisplayName}].Observe["Me.Class.ShortName"]}].Value}].Precision[5]}]}
        DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}_tmpWeight${sep}\a-w${_tmpWeight}\ax
        /if (${Float[${_tmpWeight}].Precision[5]} < ${Float[${_hurtWeight}].Precision[5]}) {
          /varset _hurtWeight ${Float[${_tmpWeight}].Precision[5]}
          | /vardata hurtData Spawn[pc ${DanNet[${_spawn.DisplayName}].Observe["Me.ID"]}]
          /vardata hurtData _spawn
        }
      } else /if (!${maHeal.Find[swHealWeighted].Value}) {
        | /vardata hurtData Spawn[pc ${DanNet[${_spawn.DisplayName}].Observe["Me.ID"]}]
        /vardata hurtData _spawn
      }

    /next _count
    /return ${hurtCount}

  }

/return 0



|***
 * note: this will swap out a usable robe with the chest slot and amulet of necro illusion 
 *       to bug out the users grapic showing a robe and not a breastplate
 * use: /cb robe
 ***|
sub check_ornament_robe(bool _debug)
  DEBUG \atcheck_ornament_robe\aw

  /if (!${cast_data[${_debug}, "Amulet of Necropotence"]}) {
    OUT get your shit together first
    /return FALSE
  }
  /invoke ${maData.Add[stChestItem,${InvSlot[17].Item.Name}]}
  /call Bind_command_swap "${maChr.Find[stOrnamentRobe].Value}" 18 ${_debug}
  /call cast ${_debug} 0 FALSE
  /delay 8
  /call Bind_command_swap "${maData.Find[stChestItem].Value}" 18 ${_debug}
  /delay 8
  /invoke ${Me.Buff["Illusion: Skeleton"].Remove}

/return TRUE



|***
 * note: get us a dannet spawn 
 * use: ${spawn_dannet[debug, channel, INT Position]}
 ***|
sub spawn_dannet(bool _debug, string _channel, int _count)
  DEBUG \atspawn_dannet\ax(\a-w${_channel}, ${_count}\aw)

  /declare _spawn spawn local
  /vardata _spawn DanNet.Peers[${_channel}].Arg[${_count},|]

/return _spawn



|***
 * note: find the best alliance you have with given conditions
 * use: ${get_best_in_alliance[DEBUG, TARGETTYPE, SUBCATEGORY, ATRIBUTENAME]}
 ***|
sub get_best_in_alliance(bool _debug, string _targettype, string _subcat, string _attrib)
  DEBUG \atget_best_in_alliance\ax(\a-w${_targettype}, ${_subcat}, ${_attrib}\ax)
  
  /declare _running         int local 0
  /declare _max             int local 0
  /declare _maxName         string local FALSE
  /declare _countNumEff     int local 0
  /declare _countSPA        int local 0

  /while (TRUE) {
    /sqlite query "${DBstatic}" qBestSpell SELECT * FROM eq_spa WHERE value="${_attrib}";
    /if (${sqlite.Resultcode[qBestSpell]} == 0) /break
  }

  /for _count 1 to ${maHard.Find[stSpellBookSize].Value}
    /if (!${Me.Book[${_count}].ID}) /continue
    /if (${Me.Book[${_count}].Subcategory.NotEqual[${_subcat}]}) /continue
    /if (${Me.Book[${_count}].TargetType.NotEqual[${_targettype}]}) /continue
    /if (!${Me.Book[${_count}].HasSPA[${sqlite.Result[qBestSpell 1 env_var]}]}) /continue
    /for _countNumEff 1 to ${Me.Book[${_count}].NumEffects}
      /for _countSPA 1 to ${Me.Book[${_count}].Trigger[${_countNumEff}].NumEffects}
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
  /next _count

/return ${_maxName}    

  

|***
 * note: proc twincast heal buff, target changes depending on heal class type
 * use: part of heal routine
 ***|
sub cast_cycle_proc_twinheal(bool _debug)
  DEBUG \atcast_cycle_proc_twinheal\ax()

  | break on active twinheal
  /if (${Me.Song[Healing Twincast].ID}) {
    DEBUG ${break} twincast heal active
    /return TRUE
  }

  | do checks by class type
  /if (${Select[${Me.Class.ShortName},CLR,DRU]}) {
    /if (${targetData.PctHPs} > ${maCC.Find[stPctEngage].Value}) {
      DEBUG ${break}.PctHPs${sep}${targetData.PctHPs} > ${maCC.Find[stPctEngage].Value}
      /return FALSE
    }  

    /if (!${targetData.LineOfSight}) {
      DEBUG ${break}.LineOfSight
      /return FALSE
    }

  } else /if (${Select[${Me.Class.ShortName},SHM]}) {

    /if (!${assistData.LineOfSight}) {
      DEBUG ${break}.LineOfSight
      /return FALSE
    }

  }
    
  CHECKEXIT
  /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
  GETINPUT

 
  | get our twincast proc
  /if (${lsLineNuke.Contains[EMPTY]} && ${lsLineNuke.Count} == 1) {
    DEBUG ${sep}resetting the empty nuke line
    /invoke ${lsLineNuke.Clear}
    /invoke ${lsLineNuke.Append[${stCount20}]}
  }

  /declare _liNukeTC listiterator local
  /vardata _liNukeTC lsLineNuke.First.Clone
  /while (!${_liNukeTC.IsEnd}) {

    | check the switch
    /if (!${Bool[${maNuke.Find[swNuke${_liNukeTC.Value}].Value}]}) {
      DEBUG ${break}switched off${sep}\a-w${maNuke.Find[stNuke${_liNukeTC.Value}].Value}\ax
      /invoke ${_liNukeTC.Advance}
      /continue
    }

    | skip non twincast nukes
    /if (!${lsNuke${_liNukeTC.Value}Tag.Contains[twincast]}) {
      DEBUG ${break}invalid twincast check (\a-r${maNuke.Find[stNuke${_liNukeTC.Value}].Value}\ax)
      /invoke ${_liNukeTC.Advance}
      /continue
    }
    
    /if (!${cast_data[${_debug}, "${maNuke.Find[stNuke${_liNukeTC.Value}].Value}"]}) {
      /invoke ${_liNukeTC.Advance}
      /continue
    }

    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Nuke ${_liNukeTC.Value} ${Me.DisplayName} "${maNuke.Find[stNuke${_liNukeTC.Value}].Value}" cast_cycle_proc_twinheal
    /if (!${Macro.Return}) {
      /invoke ${_liNukeTC.Advance}
      /continue
    }

    | nuke the mob or the tanks.. why not both?
    /if (${Select[${Me.Class.ShortName},CLR,DRU]}) {
      /call cast ${_debug} ${targetData.ID} FALSE
    } else /if (${Select[${Me.Class.ShortName},SHM]}) {
      /call cast ${_debug} ${assistData.ID} FALSE
    }
    
    | forced delay
    /if (${maNuke.Find[stNukeDelay].Value}) {
      DEBUG ${sep} nuke delay
      /delay 10s !${Me.Casting.ID}
      /delay 2.5s
      /if (${maNuke.Find[swNukeWhackAMole].Value} && !${Me.Sitting} && !${Me.Mount.ID}) {
        DEBUG ${sep}sitting
        /sit
      }
      /invoke ${set_data_timer[${_debug}, Nuke_Delay, ${maNuke.Find[stNukeDelay].Value}]}
      DEBUG ${sep}delaying${sep}\a-w${timer_Nuke_Delay}\ax
      /delay ${timer_Nuke_Delay}
    } 
        
    /if (${Me.Song[Healing Twincast].ID}) {
      DEBUG ${break} twincast heal active
      /return TRUE
    }
    
    /invoke ${_liNukeTC.Advance}
  }

/return FALSE




|***
 * note: check sash stuff
 * use: /call check_sash_cycle DEBUG [tag] [T/F]
 ***|
sub check_sash_cycle(bool _debug, string _tag, bool _return)
  DEBUG \atcheck_sash_cycle\ax(\a-w${_tag}\ax)
  
  
  /if (${timer_Check_Sash}) {
    /return FALSE
  }
  
  /invoke ${set_data_timer[${_debug}, Check_Sash, RESTART]}

  | if the user has not set the line order, set it to a default for now
  /if (${lsLineSash.Contains[EMPTY]} && ${lsLineSash.Count} == 1) {
    DEBUG ${sep}resetting the empty sash line
    /invoke ${lsLineSash.Clear}
    /invoke ${lsLineSash.Append[${stCount12}]}
  }

  /invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}

  /declare _mySash string local FALSE
  /declare _lisash listiterator local
  /vardata _lisash lsLineSash.First.Clone

  /while (!${_lisash.IsEnd}) {
    DEBUG ${sep}sash${sep}\a-w${maSash.Find[stSash${_lisash.Value}].Value}${sep}${maSash.Find[stSash${_lisash.Value}]}\ax
    GETINPUT check_sash_cycle NA|NA
    /if (${is_me_dead[${maDebug.Find[rez].Value}]}) /call check_for_rezbox ${maDebug.Find[rez].Value}
    GETOVERHERE
    CHECKEXIT check_sash_cycle

    | check the switch
    /if (!${maSash.Find[swSash${_lisash.Value}].Value}) {
      /invoke ${_lisash.Advance}
      /continue
    }

    | check matching tag for failure
    /if (!${lsSash${_lisash.Value}Tag.Contains[${_tag}]}) {
      DEBUG ${break}no tag${sep}\a-w${_tag}\ax
      /invoke ${_lisash.Advance}
      /continue
    }

    | check matching tag pass
    /if (${lsSash${_lisash.Value}Tag.Contains[${_tag}]}) {
      /varset _mySash ${maSash.Find[stSash${_lisash.Value}].Value}
    }

    | macro starting bandolier
    /if (${_tag.Equal[macstart]}) {
      /if (!${Me.Bandolier[${_mySash}].Active}) {
        /invoke ${Me.Bandolier[${_mySash}].Activate}
      }
      /return TRUE
    }
    
    | just after a sash name
    /if (${_return}) {
      /return ${maSash.Find[stSash${_lisash.Value}].Value}
    }
    
    | cycle the tags
    /call tag_evaluation ${maDebug.Find[tag].Value} Sash ${_lisash.Value} ${Me.DisplayName} "${_mySash}" check_sash_cycle
    /if (!${Macro.Return}) {
      /invoke ${_lisash.Advance}
      /continue
    } else {
      /invoke ${Me.Bandolier[${_mySash}].Activate}
      /return TRUE
    }


    /invoke ${_lisash.Advance}
  }

/return TRUE



|***
 * note: swap to a bandolier for a proc
 * use: check_sash_proc DEBUG [self|target]
 ***|
sub check_sash_proc(bool _debug, string _target)
  DEBUG \atcheck_sash_proc\ax()
  
  /return TRUE

  | make sure the settings we need are there
  | /if (!${maSash.Find[swSashProc].Value}) {
  |   /invoke ${set_data_timer[${_debug}, Check_Proc_Sash, 30s]}
  |   /return FALSE
  | }

  /invoke ${set_data_timer[${_debug}, Check_Proc_Sash, RESTART]}
  /declare _count int local 0
  /declare _spell spell local
  /declare _sash string local FALSE

  | set the spells we need to look for
  
  
  /for _count 1 to 2
    
    /varset _sash ${check_sash_cycle[FALSE, procontarget, TRUE]}
    /vardata _spell FindItem[${Me.Bandolier[${_sash}].Item[${_count}]}].Spell
    
    
    
    /if (${_target.Equal[self]}) {
 
      ${Me.Buff[${_buff}].ID} || ${Me.Song[${_buff}].ID}
 
      
    } else /if (${_target.Equal[target]}) {
 
 ${Target.Buff[${_buff}].ID}
      
    }
    
    
    

  /next _count
  
  
  
  



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

  /if (!${Me.Buff[${_buff}].ID} && !${Me.Song[${_buff}].ID} && !${Target.Buff[${_buff}].ID} && !${Me.Bandolier[${maSash.Find[stSashProc].Value}].Active} && ${Spell[${_buff}].WillLand}) {
    /invoke ${maData.Add[stMainhand,${Me.Inventory[mainhand].Name}]}
    /invoke ${maData.Add[stOffhand,${Me.Inventory[offhand].Name}]}
    /invoke ${maData.Add[stRanged,${Me.Inventory[ranged].Name}]}
    /invoke ${maData.Add[stAmmo,${Me.Inventory[ammo].Name}]}
    /delay 1
    /invoke ${Me.Bandolier[${maSash.Find[stSashProc].Value}].Activate}
  }

/return TRUE






Earthcaller



${FindItem[${Me.Bandolier[testband].Item[1]}].Spell}
