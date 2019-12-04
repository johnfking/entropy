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
	/if (${_debug}) /echo \atset_data\ax(\a-w${_container}, ${_name}, ${_type}, ${_scope}, "${_value}"\ax)
	/declare _dump bool local FALSE

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
		/varset _dump ${${_container}.Add[${_name},"${_value}"]}
		/if (${_debug}) /echo \a-p::\axmap contents\a-p::\ax${${_container}.Find[${_name}]}
		/if (${_debug}) /echo \a-p::\axmap Success\a-p::\ax\a-w${_dump}\ax count\a-p::\ax${${_container}.Count}
	}
	
	| sets
	/if (${Select[${_type},set]}) {
		| make sure set container exists
		/if (!${Defined[${_container}]}) {
			/declare ${_container} set ${_scope}
		}
		
		| add to set
		/varset _dump ${${_container}.Add[${_value}]}
		/if (${_debug}) /echo \a-p::\axset Success\a-p::\ax\a-w${_dump}\ax count\a-p::\ax${${_container}.Count}
	}
 
	| lists
	/if (${Select[${_type},col,list]}) {
		| make sure list container exists
		/if (!${Defined[${_name}]}) {
			/declare ${_name} list ${_scope}
			/varset dump ${${_name}.Delimiter["|"]}
		}

		| add to list
		/if (${Bool[${_value}]}) {
			/varset dump ${${_name}.Append["${_value}"]}
			/if (${_debug}) /echo \a-p::\axlist Success\a-p::\ax\a-w${dump}\ax
		}
	}

/return



|***
 * note: generate/restart timers
 * use: /call set_data_timer DEBUG [timer variable] [value|RESTART] [Temp|delete] || ${set_data_timer[DEBUG, Variable, value|RESTART, temp|delete]}
 ***|
sub set_data_timer(string _debug, string _env_timer, string _value, string _verbage)
	DEBUG \atset_data_timer\ax(\a-w${_env_timer}, ${_value}, ${_verbage}\ax)

	| timer restart
	/if (${_value.Equal[RESTART]}) {
		DEBUG RESTART for \aw${_env_timer}\ax${sep}${maTimer.Find[ti${_env_timer}].Value}\ax
		/varset timer_${_env_timer} ${maTimer.Find[ti${_env_timer}].Value}
		/return	
	}
	
	| temporary timer
	/if (${_verbage.Equal[temp]}) {
		/if (!${Defined[timer_${_env_timer}]}) {
			/declare timer_${_env_timer} timer outer
		}
		/varset timer_${_env_timer} ${_value}	
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
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_timer WHERE env_timer='ti${_env_timer}'
	DEBUG ${sep}dummy SELECT * FROM environment_timer WHERE env_timer='${_env_timer}' count:\ap${sqlite.Rows[dummy]}\ax
	
	| data exists. get from DB and call good
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}Timer does exists. map it
		/varset dump ${maTimer.Add[ti${_env_timer},${sqlite.Result[dummy 1 value]}]}
	
	} else /if (!${sqlite.Rows[dummy]}) {
		DEBUG ${sep}Timer does NOT exists. add to DB and map it
		/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT INTO environment_timer(env_timer, value) VALUES('ti${_env_timer}', '${_value}')
		DEBUG ${sep} /sqlite ${maData.Find[DBcharacter].Value} dummy INSERT INTO environment_timer(env_timer, value) VALUES('ti${_env_timer}', '${_value}') \ay${sqlite.Status[dummy]}\ax
		/varset dump ${maTimer.Add[ti${_env_timer},${_value}]}
	}
	
	| make timer
	/if (!${Defined[timer_${_env_timer}]}) {
		/declare timer_${_env_timer} timer outer
	}
	
	| all timers start at 1 so the code will trigger the associated sub on the first cycle. from there we get into a normal routine
	/varset timer_${_env_timer} 1

/return



|***
 * note: generate initial environment sql and map
 * use: /call set_data_map DEBUG [map name] [variable] [value] [aux]
 ***|
sub set_data_map (bool _debug, string _map, string _env_var, string _value, _aux)
	DEBUG \atset_data_map\ax(\a-w${_map}, ${_env_var}, "${_value}", "${_aux}"\ax)
	
	| make sure map container exists
	/if (!${Defined[${_map}]}) {
		/declare ${_map} map outer
	}
	
	| record pregen data
	/if (${_value.Arg[1,|].Equal[PREGEN]}) {
		/varset dump ${maPREGEN.Add[${_env_var},${_value.Arg[2,|]}]}
	}

	| is the data there already?
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_character WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_character WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax
		
	| data exists. get from DB and call good
	/if (${sqlite.Rows[dummy]}) { 
		/if (${maEnv.Find[build].Value.Equal[${maData.Find[stPreviousBuild].Value}]}) {
			DEBUG ${sep}DATA \agdoes exists VERSIONS MATCH\ax
			/varset dump ${${_map}.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
			DEBUG ${sep}${_map}${${_map}.Find[${sqlite.Result[dummy 1 env_var]}]}
			/return TRUE

		| data exists but versions are different
		} else /if (${maEnv.Find[build].Value.NotEqual[${maData.Find[stPreviousBuild].Value}]}) {
			DEBUG ${sep}DATA \agdoes exists VERSIONS \arNO NOT\ax MATCH\ax
			/varset dump ${${_map}.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
			DEBUG ${sep}${_map}${${_map}.Find[${sqlite.Result[dummy 1 env_var]}]}
			/declare _DBLOC string local ${get_key_DBLOC[${_debug}, ${_env_var}]}	
			/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET 'aux'="${Parse[1,${_aux}]}"  WHERE env_var="${_env_var}"
			DEBUG ${sep}dummy UPDATE ${_DBLOC.Arg[1,|]} SET 'aux'='${Parse[1,${_aux}]}' WHERE env_var='${_env_var}' ${dot} \ay${sqlite.Status[dummy]}\ax
			/return TRUE
	
		}
	
	| data does not exist	
	} else /if (!${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \ardoes NOT exists\ax
		
		/if (${_value.Find[PREGEN]}) {
			DEBUG \agPREGEN DATA\ax
			/declare _count int local 1
			/declare _tmpValue string local

			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM env_${Me.Class.ShortName.Lower} WHERE env_var="${_env_var}"
			DEBUG ${sep}dummy SELECT * From env_${Me.Class.ShortName.Lower} WHERE env_var='${_env_var}' ${dot} \ay${sqlite.Status[dummy]}\ax
			DEBUG ${sep}${sep}${sqlite.Result[dummy 1 env_var]}${sep}\a-w${sqlite.Result[dummy 1 value]}\ax
			
			| how many do we want to find?
			/while (${_count} <= ${sqlite.Result[dummy 1 value].Count[|]}) {
				DEBUG ${sep}_count list${sep}\a-w${_count}/${sqlite.Result[dummy 1 value].Count[|]}\ax ${dot} ${sqlite.Result[dummy 1 value].Arg[${_count},|]}
				/if (${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]}) {
					/varset _tmpValue ${sqlite.Result[dummy 1 value].Arg[${_count},|]}
					/break
				}
				/varcalc _count ${_count}+1
			}
		
			| adjust our running value for the database entry
			/varset _value ${_tmpValue}
			DEBUG ${sep} _value ${_tmpValue}
		}		
	}
			
	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_character(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${Parse[1,${_aux}]}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_character(env_var,value,aux) VALUES('${_env_var}', '${_value}', '${_aux}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/varset dump ${${_map}.Add[${_env_var},"${_value}"]}

/return



|***
 * note: generate initial environment sql and list
 * use: /call set_data_list DEBUG [list name] [variable] [value] [aux]
 ***|
sub set_data_list (bool _debug, string _map, string _env_var, string _value, string _aux)
	DEBUG \atset_data_list\ax(\a-w${_map}, ${_env_var}, "${_value}", "${_aux}"\ax)
	
	| make sure list container exists
	/if (!${Defined[${_env_var}]}) {
		/declare ${_env_var} list outer
		/varset dump ${${_env_var}.Delimiter["|"]}
	}
	
	| make sure map container exists
	/if (!${Defined[${_map}]}) {
		/declare ${_map} map outer
	}	
	
	| record pregen data
	/if (${_value.Arg[1,|].Equal[PREGEN]}) {
		/varset dump ${maPREGEN.Add[${_env_var},${_value.Arg[2,|]}]}
	}

	| is the data there already?
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_character WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_character WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax
		
	| data exists. get from DB and call it good
	/if (${sqlite.Rows[dummy]}) {
		/if (${maEnv.Find[build].Value.Equal[${maData.Find[stPreviousBuild].Value}]}) {
			DEBUG ${sep}DATA \agdoes exists VERSIONS MATCH\ax

			/if (${Bool[${sqlite.Result[dummy 1 value]}]}) {
				/declare _tmpValue string local ${sqlite.Result[dummy 1 value]}
				DEBUG _tmpValue${sep}${_tmpValue}

				/varset dump ${${_env_var}.Append["${_tmpValue}"]}
				DEBUG /varset dump ${${_map}.Add[${_env_var},"${_tmpValue}"]}
				/varset dump ${${_map}.Add[${_env_var},"${_tmpValue}"]}
			}
			/return TRUE


		| data exists but versions are different
		} else /if (${maEnv.Find[build].Value.NotEqual[${maData.Find[stPreviousBuild].Value}]}) {
			DEBUG ${sep}DATA \agdoes exists VERSIONS \arNO NOT\ax MATCH\ax

			/if (${Bool[${sqlite.Result[dummy 1 value]}]}) {
				/declare _tmpValue string local ${sqlite.Result[dummy 1 value]}
				DEBUG _tmpValue${sep}${_tmpValue}

				/varset dump ${${_env_var}.Append["${_tmpValue}"]}
				DEBUG /varset dump ${${_map}.Add[${_env_var},"${_tmpValue}"]}
				/varset dump ${${_map}.Add[${_env_var},"${_tmpValue}"]}
			}			
			/declare _DBLOC string local ${get_key_DBLOC[${_debug}, ${_env_var}]}	
			/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET 'aux'='${Parse[1,${_aux}]}' WHERE env_var="${_env_var}"
			DEBUG ${sep}dummy UPDATE ${_DBLOC.Arg[1,|]} SET 'aux'='${Parse[1,${_aux}]}' WHERE env_var='${_env_var}' ${dot} \ay${sqlite.Status[dummy]}\ax
			/return TRUE
	
		}

	| if data doesnt exist??	
	} else /if (!${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \ardoes NOT exists\ax
		/declare _count int local 1
		/declare _tmpList string local
	
		/if (${_value.Find[PREGEN]}) {
			DEBUG PREGEN DATA
			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM env_${Me.Class.ShortName.Lower} WHERE env_var="${_env_var}"
			DEBUG ${sep}dummy SELECT * From env_${Me.Class.ShortName.Lower} WHERE env_var='${_env_var}' ${dot} \ay${sqlite.Status[dummy]}\ax
			DEBUG ${sep}${sep}${sqlite.Result[dummy 1 env_var]}${sep}\a-w${sqlite.Result[dummy 1 value]}\ax
					
			| how many do we want to find?
			
			/while (${_count} <= ${_value.Arg[2,|]}) {
				DEBUG +++++++ ${sqlite.Result[dummy 1 value].Arg[${_count},|]}
				| is it in the book?
				/if (${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]}) {
					/if (${Bool[${_tmpList}]}) /varset _tmpList ${_tmpList}|
					/varset _tmpList ${_tmpList}${sqlite.Result[dummy 1 value].Arg[${_count},|]}
					/varcalc _count ${_count}+1
				} else /if (!${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} && !${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]}) {
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
		/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_character(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${Parse[1,${_aux}]}")
		DEBUG ${sep}  dummy INSERT OR IGNORE INTO environment_character(env_var,value,aux) VALUES('${_env_var}', "${_value}", '${Parse[1,${_aux}]}') ${dot} \ay${sqlite.Status[dummy]}\ax
		
	}		
		
	| create our list for the class to use	
	/if (!${_value.Find[PREGEN]}) {
		DEBUG ${sep} _map${sep}\a-w${_map}\ax ${dot} _env_var${sep}\a-w${_env_var}\ax ${dot} _value${sep}\a-w${_value}\ax
		/varset dump ${${_env_var}.Append[${_value}]}
		/varset dump ${${_map}.Add[${_env_var},"${_value}"]}
	}

/return



|***
 * note: /creates / sets conditions
 * use: /call set_data_condition DEBUG [variable] [value]
 ***|
sub set_data_condition(bool _debug, string _env_var, string _value)

	/if (!${Defined[maCondition]}) {
		/declare maCondition map outer
	}	

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_condition WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax
		

	| data exists. get from DB and call good
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/varset dump ${maCondition.Add[${sqlite.Result[dummy 1 env_var]},${Parse[1,"${sqlite.Result[dummy 1 value]}"]}]}]}
		/return TRUE
	
	} 
			
	| no data. add it.
	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_condition(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_condition(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/varset dump ${maCondition.Add[${_env_var},"${_value}"]}

/return



|***
 * note: /creates / sets buffs
 * use: /call set_data_buffs DEBUG [variable] [value]
 ***|
sub set_data_buffs(bool _debug, string _env_var, string _value)
	DEBUG \atset_data_buffs\ax(\a-w${_env_var}, ${_value}\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/varset dump ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maBuffs]}) {
		/declare maBuffs map outer
	}	

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_buffs WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_buffs WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax
		
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/varset dump ${maBuffs.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maBuffs${sep}${maBuffs.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/varset dump ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	} 
			
	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_buffs(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_buffs(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/varset dump ${maBuffs.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/varset dump ${${_env_var}.Append["${_value}"]}
	}

/return



|***
 * note: /creates / sets crew
 * use: /call set_data_crew DEBUG [variable] [value]
 ***|
sub set_data_crew(bool _debug, string _env_var, string _value)
	DEBUG \atset_data_crew\ax(\a-w${_env_var}, ${_value}\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/varset dump ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maCrew]}) {
		/declare maCrew map outer
	}	

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_crew WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_crew WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax
		
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/varset dump ${maCrew.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maCrew${sep}${maBuffs.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/varset dump ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	} 
			
	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_crew(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_crew(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/varset dump ${maCrew.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/varset dump ${${_env_var}.Append["${_value}"]}
	}

/return




|***
 * note: /creates / sets items
 * use: /call set_data_items DEBUG [variable] [value]
 ***|
sub set_data_items(bool _debug, string _env_var, string _value)
	DEBUG \atset_data_items\ax(\a-w${_env_var}, ${_value}\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/varset dump ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maItems]}) {
		/declare maItems map outer
	}	

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_items WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_items WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax
		
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/varset dump ${maItems.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maItems${sep}${maItems.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/varset dump ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	} 
			
	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_items(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_items(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/varset dump ${maItems.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/varset dump ${${_env_var}.Append["${_value}"]}
	}

/return



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
		/varset dump ${get_shade[FALSE, MQWindow]}
		/if (${_low} == 0) {
			/varset dump ${out[0, "${notice}${sep}Control Range ${dot} ${info}${_low}\ax(\aroff\ax)${info} - ${_high}\ax"]}
			/return FALSE
		} else {
			/varset dump ${out[0, "${notice}${sep}Control Range ${dot} ${info}${_low} - ${_high}\ax"]}
			/return FALSE
		}
	}

/return FALSE



|***
 * note: DEFINE call to forde event and bind queue checking
 * use: GETINPUT [SUBTOCREATE] [VARIABLETOMAKEACCESSABLE]
 ***|
sub get_input(string _sub, string _variable)
	| set a non-use variable to force bind checks
	/varset bindswap ${If[${bindswap},FALSE,TRUE]}
		
	| check the event queue
	/doevents
	
	/if (${maEntropy.Find[swHookSub].Value} && ${Bool[${_sub}]}) {
		/call register_observer ${_sub} ${_variable}
	}
	
/return



|***
 * note: check that a plugin is loaded
 * use: 
 ***|
sub check_plugin(bool _debug, bool _error, string _plugin)
	DEBUG \atcheck_plugin\ax(\a-w${_error}, ${_plugin}\ax)
	
	/if (!${Bool[${_plugin}]}) {
		/varset dump ${out[0, "${bad}ERROR\ax${sep}validate_plugin()"]}
		/endmacro
	}
	/declare _plugcount int local 0	
	/declare _count int local 0

	/for _count 1 to ${_plugin.Count[|]}
		/if (${Bool[${Plugin[${_plugin.Arg[${_count},|]}]}]}) {
			/varcalc _plugcount ${_plugcount}+1
		} else /if (!${Bool[${Plugin[${_plugin.Arg[${_count},|]}]}]} && ${_error}) {
			 /echo \arWARNING\ax\a-p::\ax\a-w${_plugin.Arg[${_count},|]}\ax is not loaded
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
	
	/if (!${Defined[lsBlockedBuffsMe]}) /declare lsBlockedBuffsMe string outer

	| cycle the blocked buffs and add them to a string
	/for _count 1 to 40
		| skip empty ones
		/if (!${Me.BlockedBuff[${_count}].ID}) /continue
		/varset lsBlockedBuffsMe ${lsBlockedBuffsMe}|${Me.BlockedBuff[${_count}].ID}
		| /varset dump ${lsBlockedBuffsMe.Append["${Me.BlockedBuff[${_count}]}"]}
	/next _count
	

	/if (${_silent.Equal[SILENT]}) /return TRUE
	/varset dump ${out[0, "Marking Blocked Buffs"]}
	
/return



|***
 * note: sarcasim. because, in general, people suck..
 * use: ${get_shade[DEBUG, [mqwindow|say]}
 ***|
sub get_shade(bool _debug, string _out)

	/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM shade ORDER BY RANDOM() LIMIT 1

	/if (${_out.Equal[MQWindow]}) {
		/varset dump ${out[1, "\a-g${sqlite.Result[dummy 1 value]}\ax"]}
	} else /if (${_out.Equal[say]}) {
		/say ${sqlite.Result[dummy 1 value]}
	}

/return TRUE



|***
 * note: get free ingame cash
 * use: /call check_free_ingame_cash DEBUG
 ***|
sub check_free_ingame_cash(bool _debug)
	DEBUG \atcheck_free_ingame_cash\ax()
	GETINPUT check_free_ingame_cash FALSE|FALSE
	
	/if (${Window[MKPW_ClaimWindow].Child[MKPW_ClaimClickHereBtn].Text.EqualCS[Claimed]}) /return
	/declare _current_funds string local 0

	/if (${Window[MKPW_ClaimWindow].Child[MKPW_ClaimClickHereBtn].Text.EqualCS[Claim]}) {
		/if (!${Window[MarketplaceWnd]}) /market
		/varset _current_funds ${Window[MarketplaceWnd].Child[MKPW_AvailableFundsUpper].Text}
		/notify MKPW_ClaimWindow MKPW_ClaimClickHereBtn leftmouseup
		/delay 1s
		/delay 10s ${Window[MarketplaceWnd].Child[MKPW_AvailableFundsUpper].Text.NotEqual[${_current_funds}]}
		/varset _current_funds ${Window[MarketplaceWnd].Child[MKPW_AvailableFundsUpper].Text}
		/varset dump ${out[0, "In Game Cash${sep}${num}${_current_funds}\ax"]}

		/if (${Window[MarketplaceWnd]}) /market

	}

/return



|***
 * note: clear a sql query from memory
 * use: /call sql_clear DEBUG [QUERYNAME]
 ***| 
sub sql_clear(bool _debug, string _verbage)

	/if (!${sqlite.Clear[${_verbage}]}) {
		/varset debug ${out[16, "${warning} ${dot} Couldn't clear memory for${sep}\a-w${_verbage}\ax"]}
	}
	
/return



|***
 * note: build the sql database and tables.
 * use: NA
 ***|
sub sql_schema(bool _debug)
	DEBUG \atsql_schema\ax()
		
	| ok, something needs building...
	/declare _count int local 0
	/declare _db string local ${dbCore}

	/varset debug ${out[16, "\awDB\ax${sep}\awcurrentBuild\ax${sep}\a-w${currentBuild}\ax"]}

	| cycle each table in the list
	/for _count 1 to ${sqlTables.Count[|]}
		DEBUG ${sep}_count${sep}\aw${_count}\ax ${sep} Table${sep}\a-w${sqlTables.Arg[${_count},|]}\ax

		| adjust db for character
		/varset _db ${If[${sqlTables.Arg[${_count},|].Equal[${chrID}]},${chrDBID},${dbCore}]}
		DEBUG ${sep}using _db${sep}\a-p${_db}\ax
		
		| get the build from current iteration table. if it matches, we will continue on. if not???
		
		/while (TRUE) {
			/sqlite ${_db} prevBuild SELECT currentBuild FROM ${sqlTables.Arg[${_count},|]}
			DEBUG ${sep}SQL SELECT \a-wcurrentBuild\ax FROM \a-w${sqlTables.Arg[${_count},|]}\ax ${sep} \a-y${sqlite.Status[prevBuild]}\ax
			/if (${sqlite.Status[prevBuild].Find[Failed:]}) /break
			/if (${sqlite.Status[prevBuild].Equal[SUCCESS]}) /break
			/delay 2

		}	
			
		DEBUG ${sep}prevBuild SELECT currentBuild FROM \a-w${sqlTables.Arg[${_count},|]}\ax == \aw${sqlite.Result[prevBuild 1 currentBuild]}\ax || ${If[${swForceUpdate},\arFORCED\ax,\a-rNOT_FORCED\ax]}
		DEBUG ${sep}prevBuild returned rows:\aw${sqlite.Rows[prevBuild]}\ax${sep}\aw${sqlite.Result[prevBuild 1 currentBuild]}\ax
		DEBUG ${sep}STATUS${sep}\ay${sqlite.Status[prevBuild]}\ax
		
		| query the build from database and store it as prevBuild
		/if (${sqlite.Rows[prevBuild]} || ${swForceUpdate}) {

			| if the previos build exists does equal the current build from core.mac
			/if (${sqlite.Result[prevBuild 1 currentBuild].Equal[${currentBuild}]} && !${swForceUpdate}) {
				/if (${Select[${sqlTables.Arg[${_count},|]},environment,${chrID}]}) VSQL \awDB\ax${sep}\aw${sqlTables.Arg[${_count},|]}\ax${sep}\a-wCurrent\ax
				/varset bld${sqlTables.Arg[${_count},|]}current TRUE
				DEBUG ${sep}marking${sep}bld${sqlTables.Arg[${_count},|]}current\ax${sep}\a-w${bld${sqlTables.Arg[${_count},|]}current}\ax
				/goto :loadTableData

			| if the previous build does not match the current build from core.mac
			} else /if (${sqlite.Result[prevBuild 1 currentBuild].NotEqual[${currentBuild}]} || ${swForceUpdate}) {
				/if (${Select[${sqlTables.Arg[${_count},|]},environment,${chrID}]}) VSQL \awDB\ax\a-t::\ax\aw${sqlTables.Arg[${_count},|]}\ax\a-t::\ax\a-gUpdating\ax
				/while (TRUE) {
					/sqlite ${_db} dummy UPDATE or REPLACE ${sqlTables.Arg[${_count},|]} SET currentBuild="${currentBuild}"
					DEBUG ${sep}SQL UPDATE or REPLACE \a-w${sqlTables.Arg[${_count},|]}\ax SET \a-wcurrentBuild=${currentBuild}\ax ${sep} \a-y${sqlite.Status[dummy]}\ax
					/if (${sqlite.Status[dummy].Equal[SUCCESS]}) /break
					/delay 2
				}	
			}
		
		| if no rows returned, then there is no database. we got work to do
		} else {
			/varset debug ${out[16, "\awDB\ax${sep}\aw${sqlTables.Arg[${_count},|]}\ax${sep}\a-yBuilding\ax"]}
			DEBUG ${sep}build${sep}${sqlTables.Arg[${_count},|]}${sep}\arMissing\ax
			/while (TRUE) {
				/sqlite ${_db} dummy CREATE TABLE ${sqlTables.Arg[${_count},|]} (currentBuild TEXT UNIQUE)
				DEBUG ${sep}SQL CREATE TABLE \a-w${sqlTables.Arg[${_count},|]}\ax ${sep} \a-y${sqlite.Status[dummy]}\ax
				/if (${sqlite.Status[dummy].Equal[SUCCESS]}) /break
				/delay 2
			}				

			/while (TRUE) {
				/sqlite ${_db} dummy INSERT INTO ${sqlTables.Arg[${_count},|]} (currentBuild) VALUES ("${currentBuild}")
				DEBUG ${sep}SQL INSERT INTO \a-w${sqlTables.Arg[${_count},|]}\ax (currentBuild) VALUES (\a-w${currentBuild}\ax) ${sep} \a-y${sqlite.Status[dummy]}\ax
				/if (${sqlite.Status[dummy].Equal[SUCCESS]}) /break
				/delay 2
			}		
	
		}

		| load table data	
		:loadTableData
		DEBUG gotoloop \a-w:loadTableData\ax
		/if (${sqlTables.Arg[${_count},|].Equal[${chrID}]} || ${swForceUpdate}) {
			DEBUG PREGEN \a-wpregen_character\ax and \a-wpregen_${Me.Class.ShortName}\ax
			/call pregen_character ${_debug}
			/call pregen_${Me.Class.ShortName} ${_debug}
		} else {
			DEBUG PREGEN "other" \a-wpregen_${sqlTables.Arg[${_count},|]}\ax
			/call pregen_${sqlTables.Arg[${_count},|]} ${_debug}
		}
	/next _count
	
	| get pragma write mode and hang on to it
	/sqlite ${dbCore} query_journal_core PRAGMA journal_mode=WAL
	/sqlite ${chrDBID} query_journal_character PRAGMA journal_mode=WAL
			
/return TRUE



|***
 * note: checks whos hurt, returns a count. sets most hurt: ID, Class, pct hurt
 * use: /call get_hurt DEBUG [group|pet|mypet|xtarget|] || ${get_hurt[DEBUG, group|pet|mypet|xtarget]}
 ***|
sub get_hurt(bool _debug, string _type)
	DEBUG \atget_hurt\ax(\a-w${_type}\ax)

	/declare _count int local 0
	/varset hurtID ${Me.ID}
	/varset hurtPctHP 100
	/varset hurtCLS ${Me.Class.ShortName}
	/varset hurtCount 0
	/declare _hurtWeight int local 0
	/declare _tmpWeight int local 0

	| check group hurt
	/if (${_type.Equal[group]}) {
		DEBUG \a-w{_type.Equal[group]}\ax
	
		/for _count 0 to ${Group.Members}
			DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}${Group.Member[${_count}].Name}${sep}${Group.Member[${_count}].ID}\ax
			/if (${Group.Member[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}) /continue
			/if (${Group.Member[${_count}].Distance} > ENV) /continue
			/if (!${Group.Member[${_count}].Present} || ${Group.Member[${_count}].Dead}) /continue
			/if (${Group.Member[${_count}].DisplayName.Equal[${maChr.Find[stRampageTank].Value}]}) /continue
			/if (${lsNoHealToon.Contains[${Group.Member[${_count}].DisplayName}]}) /continue
	
			DEBUG ${sep}_tmpWeight${sep}\a-w${_tmpWeight}\ax
			/varcalc _tmpWeight (${Group.Member[${_count}].PctHPs} / ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}) + .${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}
			DEBUG ${sep}_tmpWeight${sep}\a-w${_tmpWeight}\ax

			/if (${_tmpWeight} > ${_hurtWeight}) { 
				/varset _hurtWeight ${_tmpWeight}
				/varset hurtPctHP ${Group.Member[${_count}].PctHPs}
				/varset hurtID ${Group.Member[${_count}].ID}
				/varset hurtCLS ${Group.Member[${_count}].Class.ShortName}
				/varcalc hurtCount ${hurtCount} + 1
				/if (${maChr.Find[stCountHealGroup].Value}) {
					DEBUG ${sep}break on${sep}\a-w${hurtCount}\ax \aw>=\ax \a-w${maChr.Find[stCountHealGroup].Value}\ax
					/if (${hurtCount} >= ${maChr.Find[stCountHealGroup].Value}) /break
				}
			}
		
		/next _count
		
		DEBUG ${sep}worst hurt${sep}\a-w${hurtID} ${hurtCLS}@${hurtPctHP}\ax
		DEBUG ${sep}hurt count${sep}\a-w${hurtCount}\ax
		/return ${hurtCount}


	| check xtarget hurt
	} else /if (${_type.Equal[xtarget]}) {
		DEBUG \a-w{_type.Equal[xtarget]}\ax
		/for _count 1 to ${Me.XTargetSlots}
	  	/if (!${Me.XTarget[${_count}].ID}) /continue
	  	/if (${Me.XTarget[${_count}].Dead}) /continue
	  	/if (${Me.XTarget[${_count}].PctHPs} > ${hurtPctHP}) /continue
	  	/if (${Me.XTarget[${_count}].Distance} > ENV) /continue
			/if (${Me.XTarget[${_count}].DisplayName.Equal[${maChr.Find[stRampageTank].Value}]}) /continue
			/if (${lsNoHealToon.Contains[${Me.XTarget[${_count}].DisplayName}]}) /continue
	  	/if (!${Me.XTarget[${_count}].Type.Equal[PC]} && !${lsProtectNPCList.Contains[${Me.XTarget[${_count}].DisplayName}]}) /continue
			/if (${Me.XTarget[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}*.PCTHO) /continue
			/varcalc _tmpWeight (${Group.Member[${_count}].PctHPs} / ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}) + .${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}

			/if (${_tmpWeight} > ${_hurtWeight}) { 
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
	    /if (${Group.Member[${_count}].Pet.PctHPs} > ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}) /continue
	    /if (${Group.Member[${_count}].Pet.Distance} > ENV) /continue
	    /if (${Group.Member[${_count}].Pet.PctHPs} >= ${hurtPctHP}) /continue

			/varset hurtCount 1
			/varset hurtID ${Group.Member[${_count}].Pet.ID}
			/varset hurtPctHP ${Group.Member[${_count}].Pet.PctHPs}
			/varset hurtCLS Pet
	  /next _count
	  /return ${hurtCount}
	  

	| self only pet hurt
	} else /if (${_type.Equal[mypet]}) {
		/if (!${Me.Pet.ID}) /return FALSE
    /if (${Me.Pet.PctHPs} >= ${PetPoint}) /return FALSE
    /if (${Me.Pet.Distance} > ENV) /return FALSE

		/varset hurtID ${Me.Pet.ID}
		/varset hurtPctHP ${Me.Pet.PctHPs}
		/varset hurtCLS Pet
	  /if (${hurtPctHP} >= ${${hurtCLS}Point}*.PCTHO) /return FALSE
		/varset hurtCount 1
		/return ${hurtCount}

	| set self hurt
	} else /if (${_type.Equal[self]}) {
		/varset hurtID ${Me.ID}
		/varset hurtPctHP ${Me.PctHPs}
		/varset hurtCLS ${Me.Class.ShortName}

		/if (${hurtPctHP} >= ${maHeal.Find[stHealPoint${hurtCLS}].Value}*.PCTHO) /return FALSE
		/varset hurtCount 1
		/return ${hurtCount}
	  
	}	
/return FALSE







|***
 * note: checks facing of a target and immediate distance
 * use: /call check_facing DEBUG [TARGET ID] [adjust position FORCE/FALSE]
 ***|
sub check_facing(bool _debug, int _tmpID, string _adjust)
	DEBUG \atcheck_facing\ax(\a-w${_tmpID}, ${_adjust}\ax)
	/if (!AUTO) /return FALSE
	/if (${Stick.Active}) /return FALSE
	
	/if (!${Range.Between[0,${maHome.Find[stFaceArc].Value}:${Math.Abs[${Math.Calc[${Target.HeadingTo.Degrees}-${Me.Heading.Degrees}]}]}]}) {
		/squelch /face id ${_tmpID} nolook ${If[${maHome.Find[swFaceFast].Value},fast,]}
	}

	/if (${_adjust.Equal[FORCE]}) {
		/declare _mobdist float local ${Spawn[id ${_tmpID}].Distance}
		
		/if ((${_mobdist} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.9 && ${_mobdist} < ${Spawn[id ${_tmpID}].MaxRangeTo}+1) || ${_mobdist} < ${Spawn[id ${_tmpID}].MaxRangeTo}*.2) {
			/squelch /stick moveback ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.9]}
		}
	}

/return TRUE



|***
 * note: During combat, keeps one defensive up at all times if possible from a user defined list of ????,
 * note: starting from the top and moving progressively trhough the list until it finds a useable ability only if no other ability is up 
 * use: /call cast_defensive_cycle DEBUG
 ***|
sub cast_defensive_cycle(bool _debug)
	DEBUG \atcast_defensive_cycle\ax()

	/declare _li listiterator string local
	/vardata _li lsMitigationCycle.First.Clone
	/while (!${_li.IsEnd}) {
		ISMEDEAD
		/if (${is_ready[FALSE, "${_li.Value}"]}) {
			/call voodoo FALSE "${_li.Value}" 0 FALSE
			/return TRUE
		}
		/varset dump ${_li.Advance}
	}

/return FALSE



|***
 * note: check if we can run a definsive mitigation 
 * use: /if (${is_defensive_active[DEBUG, [burn|???], [range]]})
 ***|
sub is_defensive_active(bool _debug, string _abort, int _range)
	DEBUG \atis_defensive_active\ax(\a-w${_abort}, ${_range}\ax)
	
	/if (!AUTO || !${maChr.Find[stCountDefensive].Value}) /return TRUE

	/if (!${maChr.Count[lsMitigationCycle]} && !${maChr.Count[lsMitigationCyclecheck]}) /return TRUE

	/declare _li listiterator string local
	/vardata _li lsMitigationCyclecheck.First.Clone
	/while (!${_li.IsEnd}) {
		/if (${Me.Buff[${_li.Value}].ID}) /return TRUE
		/if (${Me.Song[${_li.Value}].ID}) /return TRUE
		/if (${Me.ActiveDisc[${_li.Value}].ID}) /return TRUE
		/varset dump ${_li.Advance}
	}

	/if (${SpawnCount[npc radius ${_range} PS4 targetable]} < ${maChr.Find[stCountDefensive].Value}) /return TRUE

	/if (${_abort.Equal[burn]} && BURN) /return FALSE

/return FALSE



|***
 * note: is something ready to be used? i dont GAF what it is, is it ready?
 * use: ${is_ready[DEBUG, "Thingtocheck"]}
 ***|
sub is_ready(bool _debug, string _verbage)
	DEBUG \atis_ready\ax(\a-w${_verbage}\ax)
	
	/if (!${Bool[${_verbage}]}) /return FALSE
	
	/if (${swIsMeDead}) /return FALSE
	
	/if (${_verbage.Find[+]}) {
		/varset _verbage ${_verbage.Arg[1,+]}
	}
	
	| alt ability
	/if (${Me.AltAbilityReady[${_verbage}]}) {	
		DEBUG ${sep}\a-yAA READY\ax${sep}\a-w${_verbage}\ax
		/return TRUE
	
	| spell
	} else /if (${Me.Book[${Spell[${_verbage}].RankName}]}) {
		
		| dont fail buffs so they can be memed when needed
		/if (!${Me.SpellReady[${Spell[${_verbage}].RankName}]} && !${Spell[${Spell[${_verbage}].RankName}].Category.Equal[Heals]}) {
			/if (!${lsFreePassSpells.Contains[${Spell[${Spell[${_verbage}].RankName}].SpellType}]}) /return FALSE			
		}		
		
		/if (!${Me.SpellReady[${Spell[${_verbage}].RankName}]} && !${Spell[${Spell[${_verbage}].RankName}].Category.Equal[Auras]}) /return FALSE
		DEBUG ${sep}\a-ySPELL READY\ax${sep}\a-w${Spell[${_verbage}].RankName}\ax
		/return TRUE
	
	| disc
	} else /if (${Me.CombatAbilityReady[${Me.CombatAbility[${Spell[${_verbage}].RankName}]}]}) {
		DEBUG ${sep}\a-yDISC READY\ax${sep}\a-w${Spell[${_verbage}].RankName}\ax
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
 * use: ${get_assist_info[DEBUG, name|id|both}
 ***|
sub get_assist_info(bool _debug, string _type)
	DEBUG \atget_assist_info\ax(\a-w${_type}\ax)
	
	/declare _assist local string FALSE
	
	/if (${Group.MainAssist.Name.Equal[${Me.DisplayName}]}) {
		/varset _assist ${Me.DisplayName}
	} else /if (${maCC.Find[stAssistMode].Value.Equal[G]} && ${Spawn[${Group.MainAssist.ID}].ID}) {
		/varset _assist ${Spawn[pc ${Group.MainAssist}].DisplayName}
	} else /if (${maCC.Find[stAssistMode].Value.Equal[1]}) {
		/varset _assist ${Spawn[pc ${Raid.MainAssist}].DisplayName}
	} else /if (${maCC.Find[stAssistMode].Value.Equal[2]}) {
	} else /if (${maCC.Find[stAssistMode].Value.Equal[3]}) {
	} else /if (${Bool[${maCC.Find[stManualAssist1].Value}]} && ${Spawn[pc ${maCC.Find[stManualAssist1].Value}].ID} && !${Spawn[pc ${maCC.Find[stManualAssist1].Value}].Dead}) {
		/varset _assist ${Spawn[pc ${maCC.Find[stManualAssist1].Value}].DisplayName}
	} else /if (${Bool[${maCC.Find[stManualAssist2].Value}]} && ${Spawn[pc ${maCC.Find[stManualAssist2].Value}].ID} && !${Spawn[pc ${maCC.Find[stManualAssist2].Value}].Dead}) {
		/varset _assist ${Spawn[pc ${stManualAssist2}].DisplayName}
	}
	
	DEBUG get_assist_name(\at${_assist}\ax)
	
	/if (${_type.Equal[name]}) {
		/return ${_assist}
	} else /if (${_type.Equal[id]}) {
		/return ${Spawn[PC ${_assist}].ID}
	} else /if (${_type.Equal[both]}) {
		/return |${_assist}|${Spawn[PC ${_assist}].ID}
	} else {
		/varset dump ${out[0, "${bad}ERROR\ax${sep}get_assist_name"]}
	}

/return FALSE




|***
 * note: creates observers
 * use: /call set_observers DEBUG [map of things to observe] [add|drop]
 ***|
sub set_observers(bool _debug, string _map, string _action)
	DEBUG \atset_observers\ax(\a-w${_map}, ${_action}\ax)

	/declare _count						int 		local 0
	/declare _actionO					string	local FALSE
	/declare _toon		 				string 	local FALSE
	/declare _timestart				int			local 0
	/declare _timeend					int			local 0
	/declare _tlo							string  local ${${_map}.Find[tlo].Value}

	/varset dump ${set_data_timer[FALSE, Check_Observers, RESTART]}
	
	| correct/check the action variable
	/if (${_action.Equal[drop]}) {
		/varset _actionO -drop
	} else /if (${_action.Equal[add]}) {
		/varset _actionO
	} else {
		/varset dump ${out[0, "_action \arMISSING\ax for set_observer(${_map}) call"]}
		/return FALSE
	}	
	 
	| do all the peoples!
	/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
		/varset _timestart ${MacroQuest.Running}
		| were going to use the dannet default all group to get names from. can be hatever you like tho
		/varset _toon ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
	
		| start the list of things we will be observing
		/if (!${Defined[lsObserved_${_toon}]}) {
			/declare lsObserved_${_toon} list outer
			/varset dump ${lsObserved_${_toon}.Delimiter["|"]}
		}
		DEBUG ${sep}_toon\aw ${_count}\ax::\at${_toon}\ax

		| start our trip throught the map
		/vardata mi ${_map}.First.Clone
		/while (!${mi.IsEnd}) {
			| we're skipping the TLO for monitoring
			/if (${mi.Key.Equal[tlo]}) {
				/varset dump ${mi.Advance}
				/continue
			}
			
			DEBUG ${sep}map(key,value)${sep}${_map}(\a-w${mi.Key},${mi.Value}\ax)
			
			| define or delete the variables as we need
			/if (${_action.Equal[drop]}) {
				DEBUG /varset dump lsObserved_${_toon}.Remove[${mi.Value}]
				/if (${Defined[${_toon}_${mi.Value}]}) /deletevar ${_toon}_${mi.Value}
				/varset dump ${lsObserved_${_toon}.Remove[${mi.Value}]}
				
			} else /if (${_action.Equal[add]}) {
				| if what we are observing isnt in the charcters list, add it
				/if (!${lsObserved_${_toon}.Contains[${mi.Value}]}) {
					/varset dump ${lsObserved_${_toon}.Append[${mi.Value}]}
				}
				| if the observer variable does not exist create it
				/if (!${Defined[${_toon}_${mi.Value}]}) /declare ${_toon}_${mi.Value} string outer
			}

			| set observer or drop it. depending on your choice
			DEBUG ${sep}\a-t/dobserve\ax ${_toon} -q \a-w"${_tlo}.${mi.Value}"\ax -o ${_toon}_${mi.Value} ${_actionO}
			/dobserve ${_toon} -q "${_tlo}.${mi.Value}" -o ${_toon}_${mi.Value} ${_actionO}

			| increment to the next item in the map and start loop over if theres more to do
			/varset dump ${mi.Advance}

		}	
		| set an end time for our test for that characer
		/varset _timeend ${MacroQuest.Running}
		| give us some response so we know that something is happening
		/varset dump ${out[17, "Observed(${If[${_action.Equal[add]},\agadd\ax,\ardrop\ax]})${sep}${_map}${sep}\aw${_toon}\ax${sep}\a-w${Math.Calc[((${_timeend}-${_timestart}) / 10) / 60]}s\ax"]}
		
	| moving to the next toon in the dannet all default channel
	/next _count

/return
	


|***
 * note: basic 3-way switch control for environment settings
 * use: /call set_switch_env DEBUG [switchname] [SQL] [value] || ${set_switch_env[DEBUG, switchname, SQL T/F, value]}
 ***|
sub set_switch_env(bool _debug, string _switch, bool _sql, string _verbage)
	DEBUG \atset_switch\ax(\a-w${_switch}, ${_sql}, ${_verbage}\ax) 
	
	/declare _map string local ${get_key_map[${_debug}, ${_switch}]}
	DEBUG _map${sep}${_map}
	
	| swap the switch status
	/if (!${_verbage.Length} || (!${Bool[${_verbage}]} && ${_verbage.NotEqual[NULL]})) {
		DEBUG ${sep}swapping
		/varset dump ${${_map}.Add[${_switch},${If[${${_map}.Find[${_switch}].Value},FALSE,TRUE]}]}
	
	| set switch TRUE
	} else /if (${_verbage.Equal[on]} || ${_verbage.Equal[TRUE]}) {
		DEBUG ${sep}on
		/varset dump ${${_map}.Add[${_switch},TRUE]}

	| set switch FALSE		
	} else /if (${_verbage.Equal[off]} || ${_verbage.Equal[FALSE]}) {
		DEBUG ${sep}off
		/varset dump ${${_map}.Add[${_switch},FALSE]}
	}
	
	| update database?
	/if (${_sql}) {
		/declare _DBLOC string local ${get_key_DBLOC[${_debug}, ${_switch}]}	
		DEBUG ${sep}dummy UPDATE ${_DBLOC.Arg[1,|]} SET value='${${_map}.Find[${_switch}].Value}' WHERE ${_DBLOC.Arg[2,|]}='${_switch}'
		/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET value='${${_map}.Find[${_switch}].Value}' WHERE ${_DBLOC.Arg[2,|]}='${_switch}'
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

	/varset dump ${set_data_timer[FALSE, Waste_Fireworks, RESTART]}
	
	/if (!${maEnv.Find[swAAFireworks].Value} || ${Raid.Members}) /return
	/if (${Me.AAPoints} < 20) /return

	/if (${Me.AAPoints} >= 120 && !${Me.AltAbilityReady[Glyph of Fireworks II]}) {
		/alt buy 897
		/varset dump ${set_data_timer[FALSE, Waste_Fireworks, 1m]}
		/return
	} else /if (${Me.AltAbilityReady[Glyph of Fireworks II]}) {
		/varset dump ${out[1, "'Yo, Adrian. I DID IT."]}
		/call voodoo ${maDebug.Find[cast].Value} "Glyph of Fireworks II" 0 FALSE
	}

/return



|***
 * note: watches for the rez box if we have bben marked as dead or called.
 * use: /call check_for_rezbox()
 ***|
sub check_for_rezbox(bool _debug)
	DEBUG \atcheck_for_rezbox\ax()

	GETINPUT rez_box_found swIsMeDead|${swIsMeDead}
	
	| do I have corpses around ?
	/if (!${SpawnCount[pccorpse name ${Me.DisplayName}]}) {

		| dont rezz me
		/if (!${maRez.Find[swRezTake].Value}) /return
		
		| not dead? GO AWAY
		/if (!${swIsMeDead}) /return
			
		| bail if no rez box to check
		/if (!${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["Do you wish this?"]}]}) /return
	}


	| is there a rez window?
	/if (${Window[ConfirmationDialogBox].Open}) {

		/declare _stRezWindowPct string local FALSE

		| get rez window % to something we can use			
		/if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["percent)"]}]}) {
			
			/if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(10 percent)"]}]}) { 
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
			}
		}
		
		DEBUG  _stRezWindowPct ${_stRezWindowPct}
		
		/declare _swValidRezFound bool local FALSE
			
		| we do not accept any 0% rezes
		/if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(0 percent)"]}]}) {
			/varset _swValidRezFound FALSE

		| we take call to corpse as well
		} else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["you will still be able to get a resurrection later"]}]}) {
			/echo pct call
			| /varset swNeedRez TRUE
			/varset _swValidRezFound TRUE

		} else /if (${maRez.Find[stPctMinRez].Value} <= ${_stRezWindowPct}) {
			/varset _swValidRezFound TRUE

		} 

		DEBUG _swValidRezFound ${_swValidRezFound}
		| more waiting
		/if (${_swValidRezFound}) {
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
			/delay 2s !${Me.State.Equal[HOVER]}
			/delay 2s !${Me.Hovering}
			/delay 2s !${Me.Dead}
			/varset stAssTarID 0
			/call set_stop FALSE SILENT
			/varset swIsMeDead FALSE
			/varset _swValidRezFound FALSE

			/if (${maHome.Find[swHomeMarker].Value}) {
				/call Bind_control_home clear
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
 * note: watches to see if you are deaded. if so.. theres a couple things we need to do.
 * use: ${is_me_deadDEBUG}
 ***|
sub is_me_dead(bool _debug)
	DEBUG \atis_me_dead\ax()
	
	GETINPUT is_me_dead swIsMeDead|${swIsMeDead}

	/if (${swIsMeDead}) /return TRUE

	| check death states
	/if (${Me.State.Equal[HOVER]} || ${Me.Hovering}) /varset swIsMeDead TRUE
	
	
	| did I just die and get sent to bind ?
	/if ((${Me.Buff[Resurrection Sickness].ID} || ${Me.Buff[Revival Sickness].ID}) && ${Zone.ID} == ${Me.ZoneBound.ID}) {
		/if (${Me.BoundLocation[0].Y} == ${Me.Y} && ${Me.BoundLocation[0].X} == ${Me.X} && ${Me.BoundLocation[0].Z} == ${Me.Z}) /varset swIsMeDead TRUE
	}

	| if we died, shut shit down
	/if (${swIsMeDead}) {
		/varset stAssTarID 0
		/doevents flush

		| stop all the things(s)
		/if (${maHome.Find[swMoveDeathClear].Value}) {
			/call Bind_control_home clear FALSE silent FALSE
		}

		/call set_stop FALSE SILENT
		/call clear_combat FALSE
		/if (${Me.Casting.ID}) /stopcast

	}

/return ${swIsMeDead}



|***
 * note: what we have here is one large ass
 * use: /call check_buff_cycle_shrink DEBUG
 ***|
sub check_buff_cycle_shrink(bool _debug)
	DEBUG \atcheck_buff_cycle_shrink\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/varset dump ${set_data_timer[FALSE, Check_Buff_Shrink, RESTART]}
	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE


	/if (!${is_ready[${_debug}, "${maBuff.Find[stShrinkItem].Value}"]}) /return FALSE
	
	/declare _count int local 0

	| me shrinks
	/while (${Me.Height} > 2.04 && ${_count} < 2) {
		/call voodoo ${_debug} "${maBuff.Find[stShrinkItem].Value.Arg[1,+]}" ${Me.ID} FALSE
		/varcalc _count ${_count}+1
	}
	
/return
	



|***
 * note: conditions 
 * use:
 ***|
sub check_condition(bool _debug, str _condition)
	DEBUG \atcheck_condition\ax(\a-w${_condition}\ax)

	DEBUG ${sep}Condition${sep}\aw${_condition}\ax${sep}\a-w${Parse[2,${maCondition.Find[${_condition}].Value}]}\ax

	/if (${maCondition.Find[${_condition}].Value.Equal[FALSE]}) {
		DEBUG ${sep}${_condition}{sep}\a-w${maCondition.Find[${_condition}].Value.Equal[FALSE]}\ax (false??)
		/return TRUE

	} else {
		DEBUG ${sep}evaluate${sep}\a-y${Parse[999,${maCondition.Find[${_condition}].Value}]}\ax
		/if (${Parse[999,${maCondition.Find[${_condition}].Value}]}) {
			DEBUG ${sep}Returning \agTRUE\ax
			/return TRUE
		}

	} 
	DEBUG ${sep}Returning \arFALSE\ax
/return FALSE


 
|***
 * note: this casts things. it doesn't care what it casts, but it will cast it.
 * note: ORDER of selections: ALT, SPELL, DISC, ITEM, SONG
 * note: 
 * note: best to wrap or prep the cast in an is_ready[]
 * note:
 * use: /call voodoo DEBUG ["name of voodoo to do"] [TARGET ID] [FALSE|sub to check on the way]
 ***| 
sub voodoo(bool _debug, string _verbage, int _tmpID, string _sub)
	DEBUG \atvoodoo\ax(\a-w${_verbage}, ${_tmpID}, ${_sub}\ax)
	ISMEDEAD

	| critical errors
	/if (!${Bool[${_verbage}]}) {
		/varset dump ${out[0, "${error}${sep}voodoo${sep}\ar_verbage\ax${sep}\a-w${_verbage}, ${_tmpID}, ${_sub}\ax)"]}
		/delay 5
		/return FALSE
	}
	
	| generic validations
	/if (${Me.Invis}) /return FALSE
	/if (${swIsMeDead}) /return FALSE
	
	
	
	| redirect the current cast to a dichotomic if GoM rocs
	/if (${_tmpID} != 0 && ${Target.ID} && ${Me.Song[Gift of Mana].ID} && !${Select[${Me.Class.ShortName},CLR]} && ${Spawn[${Target.ID}].Type.NotEqual[pc]}) {
	
		| nuke 1
		/if (${is_ready[${_debug}, "${maChr.Find[stDichotomic].Value}"]}) {
			/varset _verbage ${maChr.Find[stDichotomic].Value}
		}
	}	
	
	
	
	
	| are we moving? and not a bard?
	/if (!${Select[${Me.Class.ShortName},BRD]}) {
		/if (${Me.Moving}) /return FALSE
		/if (${Stick.Active}) /return FALSE
		/if (${Navigation.Active}) /return FALSE
	}
	
	| is the issued target still a thing???
	/if (${_tmpID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
		/if (!${Spawn[${_tmpID}].ID} || ${Spawn[${_tmpID}].Dead}) /return FALSE
		/if (${Spawn[${_tmpID}].Distance} > ENV) /return FALSE
	}	
	
	| condition attched?
	/if (${_verbage.Find[+c]}) {
		/declare _condition string local ${_verbage.Arg[2,+]}
		DEBUG ${sep}condition${sep}\aw${_condition}\ax
		/varset _verbage ${_verbage.Arg[1,+]}
		/if (!${check_condition[FALSE, ${_condition}]}) /return FALSE
	}
	
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
	
	| make sure we have our target if one is passed 0 is assumed no target required
	/if (${_tmpID} != 0) {
		/varset dump ${set_target[${_tmpID}]}
	}

	/declare _castLoop int local 1
	/varset dump ${maData.Find[stCastReturn].Value} NOTREADY


	| alt abilities
	/if (${_type.Equal[ALT]}) {
		DEBUG ${sep}alt${sep}${_verbage}
	
		/if (${Me.CurrentMana} < ${Spell[${_verbage}].Mana}) /return FALSE

		| /if (${Spell[${_verbage}].SpellType.Equal[Beneficial]} && ${_tmpID} == 0) {
		| 	/if (!${Spell[${AltAbility[${_verbage}].Spell.ID}].Stacks}) /return FALSE
		| }

		/if (${Me.Buff[${_verbage}].ID}) /return FALSE
		/if (${Me.Song[${_verbage}].ID}) /return FALSE		
		
		
		
		/delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}
		/if (${_tmpID} == 0) {
			/varset dump ${out[2, "AA${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/varset dump ${out[2, "AA${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[id ${_tmpID}].DisplayName}\ax"]}
		}

		/while (${_castLoop}) {
			DEBUG ${sep}altCastLoop${sep}${_verbage}
			/alt act ${Me.AltAbility[${_verbage}].ID}
			/delay 8
			/delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}
			/while (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
				/delay 3
			}
			GETINPUT
			/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
				/if (${Select[${Me.Class.ShortName},BRD]}) /call pulse_stop
				/break
			}
			/if (${Select[${maData.Find[stCastReturn].Value},NOTREADY,DISTRACTED]}) {
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
		/if (${Me.Casting.ID}) /return FALSE
		
		| global cooldown
		/if (${Me.SpellInCooldown}) /return FALSE
				
		| correct rank name
		/varset _verbage ${Spell[${_verbage}].RankName}

		| have it?
		/if (!${Bool[${Me.Book[${_verbage}]}]}) /return FALSE		

		| gem in cooldown?
		/if (${Me.GemTimer[${_verbage}]}) /return FALSE
		
		| enough manna to cast?
		/if (${Me.CurrentMana} < ${Spell[${_verbage}].Mana}) /return FALSE
		
		| dont fail buffs so they can be memed when needed
		/if (!${Me.SpellReady[${_verbage}]} && !${Spell[${_verbage}].Category.Equal[Heals]}) {
			/declare _pass bool local FALSE
			/if (${Spell[${_verbage}].SpellType.Equal[Beneficial]}) /varset _pass TRUE
			/if (${Spell[${_verbage}].SpellType.Equal[Pet]}) /varset _pass TRUE
			/if (${Spell[${_verbage}].SpellType.Equal[aura]}) /varset _pass TRUE
			
			/if (!${_pass}) /return FALSE
		}
		
		| check cast range
		/if (${Spell[${_verbage}].Range}) {
			/if (${Spawn[${_tmpID}].Distance} > ${Spell[${_verbage}].MyRange}) /return FALSE
		}
		| check AE range
		/if (${Spell[${_verbage}].TargetType.Equal[Group V2]}) {
			/if (${Spawn[${_tmpID}].Distance} > ${Spell[${_verbage}].AERange}) /return FALSE
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
		
		| check for stacking issues
		/if (${_tmpID} == ${Me.ID}) {
			/if (${Spell[${_verbage}].SpellType.Equal[Beneficial]} && !${Spell[${_verbage}].Category.Equal[Heals]}) {
				/if (!${Spell[${_verbage}].Stacks}) /return FALSE
				/if (${Me.Buff[${_verbage}].ID}) /return FALSE
				/if (${Me.Song[${_verbage}].ID}) /return FALSE		
			}
		}
		
		

		|** check any subs? yes.. no.. maybe??
		/if (${SubDefined[${_sub}]} && ${_sub.NotEqual[skiptargetcheck]}) {
			DEBUG ${sep}/call ${_sub}
			/call ${_sub}
			/if (${castBreak}) {
				/varset ${maData.Find[stCastReturn].Value} BREAK
				/return BREAK
			}
		}
		**|

		| if a gom procs and a spell is ready, we hijack the existing cast and use the GOM spell in its place
		/if (${_tmpID} != 0 && ${Target.ID} && ${Me.Song[Gift of Mana].ID} && !${Select[${Me.Class.ShortName},CLR]}) {
	
			/declare _ligom listiterator local
			/vardata _ligom lsGoM.First.Clone
				/while (!${_ligom.IsEnd}) {
					/if (${_ligom.Value.Equal[empty]}) /break
					/if (${is_ready[FALSE, "${_ligom.Value}"]}) {
						/varset _verbage ${_ligom.Value}
						/break					
					}
				/varset dump ${_ligom.Advance}
			}
		}

		/if (${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoClose}
				
		| mem spell if needed??
		/if (!${Me.Gem[${_verbage}]}) {
			/call set_spell_to ${_debug} TRUE "${_verbage}" ${maEnv.Find[stBuffGem].Value} TRUE
		}
		
		| is the spell memed
		/if (${Me.Gem[${_verbage}]}) {

			| echos
			/if (${_tmpID} == 0) {
				/varset dump ${out[3, "Cast${sep}${sp}${_verbage}\ax"]}
			} else /if (${_tmpID} != 0 && ${Target.ID}) {
				/varset dump ${out[3, "Cast${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[id ${_tmpID}].DisplayName}\ax"]}
			}
		
			/while (${_castLoop}) {
			
				/if (${Me.Invis}) /return FALSE
			
				| can we say no MQ2Cast ???
				/cast ${Me.Gem[${_verbage}]}
	
				| set delays for casting. GTFO out of this as fast as possible
				/delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}

				| delay for longer casts if there is any, cooldowns or open spell cast windows
				/while (${Me.Casting.ID} || ${Window[CastingWindow].Open} || ${Me.SpellInCooldown}) {
					/delay 3
				}
 				
 				GETINPUT
						
				| good cast
				/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
					/if (${Select[${Me.Class.ShortName},BRD]}) /call pulse_stop
					/break
				}
				| spell is not ready
				/if (${Select[${maData.Find[stCastReturn].Value},NOTREADY,DISTRACTED]}) {
					/varcalc _castLoop ${_castLoop}-1
					/delay 2
				}

			}

		}
		/return


	| discs
	} else /if (${_type.Equal[DISC]}) {
		DEBUG ${sep}\agDISC\ax${sep}${_verbage}

		/if (${Me.CurrentEndurance} < ${Spell[${_verbage}].EnduranceCost}) /return FALSE
		/if (${Spawn[id ${_tmpID}].Distance} > ${Spell[${_verbage}].MyRange}) /return FALSE
		/if (${Bool[${Me.Song[${_verbage}].Duration}]}) /return FALSE
		/if (${Bool[${Me.Buff[${_verbage}].Duration}]}) /return FALSE		
					
		| stacking
	 	/if (!${Spell[${_verbage}].NewStacks}) /return FALSE

		| CHECK DISK DURATION VS ACTIVE DISC FOR STACKING ISSUES/ BLOCKED DISCS
		/if (${Me.ActiveDisc.Name.Equal[${_verbage}]}) /return FALSE

		/if (${_tmpID} == 0) {
			/varset dump ${out[4, "Disc${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/varset dump ${out[4, "Disc${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[id ${_tmpID}].DisplayName}\ax"]}
		}
		/disc ${Spell[${_verbage}].ID}
		
		/delay 5
		/while (${Me.Casting.ID} && ${Window[CastingWindow].Open}) {
			/delay 5
		}		
		
		/if (${Select[${Me.Class.ShortName},BRD]}) /call pulse_stop
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
		
		/if (${_tmpID} == 0 || ${_tmpID} == ${Me.ID}) {
			/varset dump ${out[7, "Item${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/varset dump ${out[7, "Item${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax"]}
		}

		/while (${_castLoop}) {
			DEBUG While
			/useitem "${_verbage}" 
			/delay 2
			
			| get out if its a no cast click item			
			/if (!${FindItem[=${_verbage}].CastTime}) {
				DEBUG NO CAST TIME
				/if (${Select[${Me.Class.ShortName},BRD]}) /call pulse_stop
				/varset dump ${maData.Add[stCastReturn,SUCCESS]}
				/return
				
				
			} else {
				DEBUG CAST TIME
				/delay 8
				/delay ${Math.Calc[${FindItem[${_verbage}].CastTime.Float}+5]}s !${Me.Casting.ID}
				/while (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
					/delay 3
				}
				GETINPUT
				/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
					/if (${Select[${Me.Class.ShortName},BRD]}) /call pulse_stop
					/break
				}
				/if (${Select[${maData.Find[stCastReturn].Value},NOTREADY,DISTRACTED]}) {
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
		/if (${Me.Buff[${_verbage}].Duration.Seconds} > ${maChr.Find[stSongRefresh].Value}*10) /return FALSE
		/if (${Me.Song[${_verbage}].Duration.Seconds} > ${maChr.Find[stSongRefresh].Value}*10) /return FALSE
		/if (!${Spell[${_verbage}].Stacks}) /return FALSE
		/if (${Me.BardSongPlaying}) /call pulse_stop core_cast_pulsestop

		/if (${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoClose}
				
		| mem spell if needed??
		/if (!${Me.Gem[${_verbage}]}) {
			/call set_spell_to ${_debug} TRUE "${_verbage}" ${maEnv.Find[stBuffGem].Value} TRUE
		}

		/if (${_tmpID} == 0) {
			/varset dump ${out[5, "Play${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/varset dump ${out[5, "Play${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax"]}
		}

		/while (${_castLoop}) {
				/cast ${Me.Gem[${_verbage}]}
			/delay 5 
			| adjust delay for Metronome AA (fuck bards)
			/if (${Me.AltAbility[Metronome]}) {
				/delay ${Spell[${_verbage}].CastTime.Seconds}s
			} else {
				/delay ${Math.Calc[${Spell[${_verbage}].CastTime.Seconds}+${Math.Calc[${Spell[${_verbage}].CastTime.Seconds}*.05]}]}s
			}

			/while (${Window[CastingWindow].Open}) {
				/delay 3
			}
			GETINPUT
			/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
				/call pulse_stop
				/break
			}
			/if (${Select[${maData.Find[stCastReturn].Value},NOTREADY,DISTRACTED]}) {
			 	/varcalc _castLoop ${_castLoop}-1
			 	/delay 2
			}
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


	/if (${Select[${Me.Class.ShortName},BRD]} && (${Me.Casting.ID} || ${Me.BardSongPlaying})) {

		/while (${Me.Casting.ID} || ${Me.BardSongPlaying}) {
			/varset dump ${maData.Add[stCastReturn,NOTREADY]}
			/stopsong
			/stopcast
			/delay 10 !${Me.Casting.ID}
		}

	}	else {
		/if (${Me.Casting.ID}) /stopcast
	}
	
	| /varset combat FALSE
	/if (${_silent.NotEqual[SILENT]}) ${out[0, "\a-rResetting\ax"]} 
	
	/if (${Stick.Active}) /squelch /stick off
	/if (${Navigation.Active}) /nav stop |log=off
	/if (${maHome.Find[swSnapStuck].Value}) {
		/varset dump ${maHome.Add[swSnapStuck,FALSE]}
	}
	
	/if (TIE) {
		/varset dump ${maTie.Add[swSetTie,FALSE]}
	}
	
	| stop moving
	/if (${Me.Moving}) {
		/keypress forward
		/keypress back
	}
 	 	
	/if (${Me.Pet.ID} && ${Me.Pet.Target.ID}) {
		/if (!${Me.Pet.ReGroup}) /pet regroup
		/if (!${Me.Pet.Hold}) /pet hold on
		/pet back off		
	}

	SETHUD FALSE

/return




|***
 * note: moves the toon to a valid/LoS location of a target id with a given distance
 * use : /call moveto_range [ID] [DISTANCE]
 ***|
sub moveto_range(int _tmpID, int _range, bool _debug)
	DEBUG \atmoveto_range\ax(\a-w${_tmpID}, ${_range}\ax)
	
	/declare _i int local 0
	/declare _locX float local 0
	/declare _locY float local 0
	/declare _locZ float local 0
	/declare _tmpX float local 0
	/declare _tmpY float local 0
	/declare _dist float local ${Math.Calc[${If[${_range}==0,1,${_range}]} / ${If[${Spawn[${_tmpID}].Distance}==0,1,${Spawn[${_tmpID}].Distance}]}]}
	
	/varcalc _locX ${_dist}*${Me.X} + (1-${_dist})*${Spawn[${_tmpID}].X}	
	/varcalc _locY ${_dist}*${Me.Y} + (1-${_dist})*${Spawn[${_tmpID}].Y}	
	/if (${Navigation.PathExists[locyxz ${_locY} ${_locX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_locX} ${_locY} ${Me.Z}]} && ${LineOfSight[${_locY},${_locX},${Me.Z}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X},${Spawn[${_tmpID}].Z}]}) {
		/nav locyxz ${_locY} ${_locX} ${Me.Z} |log=off
		/delay 5 ${Navigation.Active}
		/delay 5s !${Navigation.Active}
		/return
	}

	/for _i 1 to 10
		/varcalc _tmpX ${_locX}*${cos[${_i}]} - ${_locY}*${sin[${_i}]}
		/varcalc _tmpY ${_locX}*${cos[${_i}]} + ${_locY}*${sin[${_i}]}
		/if (${Navigation.PathExists[locyxz ${_tmpY} ${_tmpX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_tmpX} ${_tmpY} ${Me.Z}]} && ${LineOfSight[${_tmpY},${_tmpX},${Me.Z}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X},${Spawn[${_tmpID}].Z}]}) {
			/nav locyxz ${_tmpY} ${_tmpX} ${Me.Z} |log=off
			/delay 5 ${Navigation.Active}
			/delay 5s !${Navigation.Active}
			/return
		}

		/varcalc _tmpX ${_locX}*${cos[${_i}]} + ${_locY}*${sin[${_i}]}
		/varcalc _tmpY ${_locX}*${cos[${_i}]} - ${_locY}*${sin[${_i}]}
		/if (${Navigation.PathExists[locyxz ${_tmpY} ${_tmpX} ${Me.Z}]} && ${EverQuest.ValidLoc[${_tmpX} ${_tmpY} ${Me.Z}]} && ${LineOfSight[${_tmpY},${_tmpX},${Me.Z}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X},${Spawn[${_tmpID}].Z}]}) {
			/nav locyxz ${_tmpY} ${_tmpX} ${Me.Z} |log=off
			/delay 5 ${Navigation.Active}
			/delay 5s !${Navigation.Active}
			/return
		}
	    
	/next _i

/return



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
 * use: /call check_xtarget_build DEBUG
 ***|
sub check_xtarget_build(bool _debug, string _forced)
	DEBUG \atcheck_xtarget_build\ax()
	
	/varset dump ${set_data_timer[FALSE, Check_Xtarget_Build, RESTART]}
	
	/if (!${Me.XTargetSlots}) /return	
	/if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
		/varset dump ${get_shade[FALSE, MQWindow]}
		/return
	}
	
	/varset dump ${out[0, "Adjusting XTargets${sep}${If[${Bool[${_forced}]},\a-wForced\ax,Timer \a-w${maTimer.Find[tiCheck_Xtarget_Build].Value}\ax]}"]}
	/declare _x 				int 		local
	/declare _b 				int 		local
	/declare _count 		int			local 0
	/declare _name			string	local

	| reset extended target slots	
	/call set_xtarget_clear FALSE
			
	| observe any assists for xtarget 1	
	/if (${Select[${maCC.Find[stAssistMode].Value},G,1,2,3]} || ${Bool[${maCC.Find[stManualAssist1].Value}]} || ${Bool[${maCC.Find[stManualAssist2].Value}]}) {
		/varset _b 1
	}

	| set protection for NPCs
	|**
	/if (${lsProtectNPCList.Count}]}) {
		/for _count 1 to ${ProtectNPCList.Count[|]}
			/varset _name ${Spawn[=${ProtectNPCList.Arg[${_x},|]} npc].DisplayName}

			/if (${_b} > ${Me.XTargetSlots} || !${Bool[${Spawn[${_name}]}]}) {
			} else /if (${Bool[${Me.XTarget[${_name}]}]}) {
				/varcalc _b ${_b}+1
			} else /if (${Bool[${Spawn[${_name}]}]}) {
				/varcalc _b ${_b}+1
				/xtarget set ${_b} ${Spawn[${_name}].Name}
			}
		/next _count
	}	

	| protect PCs when outside group
	/if (!${Raid.Members} && ${lsProtectOutsideGroup.Count}]}) {
		/for _x 1 to ${ProtectOutsideGroup.Count[|]}
			/varset _name ${Spawn[=${ProtectOutsideGroup.Arg[${_x},|]} pc].DisplayName}
			/if (${Spawn[pc ${_name}].Distance} > ENV) /continue
			/if (${Group.Member[${_name}].ID}) /continue
			/if (${_b} > ${Me.XTargetSlots} || !${Bool[${Spawn[pc ${_name}]}]}) /continue
			/if (${Bool[${Me.XTarget[${_name}]}]}) {
				/varcalc _b ${_b}+1
			} else /if (${Bool[${Spawn[pc ${_name}]}]}) {
				/varcalc _b ${_b}+1
				/xtarget set ${_b} ${Spawn[pc ${_name}].Name}
			}
		/next _x
	}			
	**|	

	| set raid member classes to protect
	/if (${Raid.Members}) {
		|***
		/if (${lsProtectInsideRaid.Count}) {
			/for _x 1 to ${ProtectInsideRaid.Count[|]}
			/varset _name ${Spawn[=${ProtectInsideRaid.Arg[${_x},|]} pc].DisplayName}

				/if (${Spawn[pc ${_name}].Distance} > ENV) /continue
				/if (${Bool[${Group.Member[${_name}]}]}) /continue
				/if (${_b} > ${Me.XTargetSlots} || !${Bool[${Spawn[pc ${_name}]}]}) {
				} else /if (${Bool[${Me.XTarget[${_name}]}]}) {
					/varcalc _b ${_b}+1
				} else /if (${Bool[${Spawn[pc ${_name}]}]}) {
					/varcalc _b ${_b}+1
					/xtarget set ${_b} ${Spawn[pc ${_name}].Name}
				}
			/next _x
		}
		***|


		/declare _lixt listiterator local
		/vardata _lixt lsHealXTClass.First.Clone
		/while (!${_lixt.IsEnd}) {

			| no class in range
			/if (!${SpawnCount[pc ${_lixt.Value} raid radius ENV]}) {
				/varset dump ${_lixt.Advance}
				/continue
			}

			| cycle the class and add to slots
			/for _x 1 to ${SpawnCount[pc ${_lixt.Value} raid radius ENV]}

				/if (${_b} > ${Me.XTargetSlots}) /break
				
				| are they a group member?
				/if (${Group.Member[${NearestSpawn[${_x}, pc ${_lixt.Value} raid]}].ID}) /continue
				
				| on the xtarget
				/if (${Bool[${Me.XTarget[${NearestSpawn[${_x}, pc ${_lixt.Value} raid].Name}]}]}) /continue

				| me?
				/if (${NearestSpawn[${_x}, pc ${_lixt.Value} raid radius ENV].DisplayName.Equal[${Me.DisplayName}]}) /continue

				/varcalc _b ${_b}+1
				/xtarget set ${_b} ${NearestSpawn[${_x}, pc ${_lixt.Value} raid radius ENV]}
			/next _x
			
			/varset dump ${_lixt.Advance}
		}

	}
	
/return




|***
 * note: Sticks you to the target
 * note: for the love of whatever you find important, don't touch this ever again...
 * use: /call stick_to_target DEBUG [target ID]
 ***|
sub stick_to_target(bool _debug, int _tmpID)
	DEBUG \atstick_to_target\ax(\a-w${_tmpID}\ax)
	
	/if (!AUTO) /return

	ISMEDEAD	
	GETINPUT

	| if the stick behind switch is on. we do that over ranged decision combat
	/if (${maHome.Find[swMoveBehind].Value} && !${Stick.Behind} && ${Target.Distance} <= ENV) {
		DEBUG ${sep}maHome.Find[swMoveBehind].Value${sep}\a-w${maHome.Find[swMoveBehind].Value}\ax
		| /call get_assist_target
		/call check_facing FALSE TARGET FALSE
			
		/if (!${Stick.Active} && ${Target.ID} && VALID && !${Stick.Behind} && !${timer_Check_Behind}) {
			DEBUG ${sep}snaproll
			/squelch /stick id ${_tmpID} ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.9]} snaproll
		} 

		:checkstuck
		ISMEDEAD	
		GETINPUT
		/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE
		/if (${MoveUtils.Stuck}) {
			/delay 1
			/squelch /stick off
			/varset dump ${set_data_timer[FALSE, Check_Behind, RESTART]}
			/return
		}	
		/if (!${Stick.Behind} && !${timer_Check_Behind}) /goto :checkstuck

	} else /if (!${maHome.Find[swMoveBehind].Value} && ${Target.Distance} <= ENV) {
		| /if (${swModeHunt}) /varset swValidAssTarID TRUE
		:movetomob
		ISMEDEAD	
		GETINPUT
		/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE
		/if (!${Stick.Active} && ${Target.ID} && VALID && ${Spawn[id ${_tmpID}].Distance} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.9) {
			/call check_facing FALSE ${_tmpID} FALSE
			/squelch /stick id ${_tmpID} ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.9]}
		}
		/delay 1
		/if (${Spawn[id ${_tmpID}].Distance} > ${Spawn[id ${_tmpID}].MaxRangeTo}*.9) /goto :movetomob
		/if (${Spawn[id ${_tmpID}].Distance} < ${Spawn[id ${_tmpID}].MaxRangeTo}*.9) /squelch /stick off
		/varset dump ${set_data_timer[FALSE, Check_Behind, RESTART]}

		/return
	}
	
	/squelch /stick off
/return



|***
 * note: Seriously, do you need to ask?
 * use: ${is_target_dead[DEBUG, TARGETID]}
 ***|
sub is_target_dead(bool _debug, int _tmpID)
	DEBUG \atis_target_dead\ax(\a-w${_tmpID}\ax)
	
	ISMEDEAD

	/declare _dead bool local FALSE
	
	/if (!AUTO) {
		/if (!${Bool[${_tmpID}]} && ${Target.ID}) /varset _tmpID ${Target.ID}
		/if (!${Bool[${_tmpID}]}) {
			/call clear_combat FALSE
			/return TRUE
		}
	}
	
	| no target ID? shits dead 'till we get one to check..
	/if (!${Target.ID}) {
		/varset _dead TRUE
	} else /if (${_tmpID} == 0 || !${Bool[${_tmpID}]}) {
		/varset _dead TRUE
	}

	| targets dead? were done here
	/if (${Target.Dead}) {
		/varset _dead TRUE
	} else /if (${Target.Type.Equal[Corpse]} || ${Spawn[${Target.ID}].Type.Equal[CORPSE]}) {
		/varset _dead TRUE
	} else /if (${Spawn[${_tmpID}]].Dead} || ${Spawn[${Target.ID}].Dead}) {
		/varset _dead TRUE
	} else /if (!${Spawn[${_tmpID}].ID}) {
		/varset _dead TRUE
	} else /if (${Spawn[${Target.ID}].Type.Equal[PCCORPSE]}) {
		/varset _dead TRUE
	}

	/if (${_dead}) {
		/call clear_combat FALSE
		| /call set_stop FALSE
	}
	
/return ${_dead}



|***
 * note: Clear all combat variables
 * use: /call clear_combat DEBUG
 ***|
sub clear_combat(bool _debug)
	DEBUG \atclear_combat\ax()
	
	GETINPUT
	
	/if (${SubDefined[DPSMeter_Reset]}) { 
		/call DPSMeter_Reset
	}
	
	/varset dump ${maCC.Add[swValidAssTarID,FALSE]}
	/varset dump ${maCC.Add[swSetCombat,FALSE]}

	/if (${Me.AutoFire}) /squelch /autofire
	/if (${Me.Combat}) /squelch /attack off
	/if (!${Select[${Me.Class.ShortName},BRD]}) {
		/if (AUTO && ${Me.Casting.ID} && (!${Spell[${Me.Casting.ID}].Category.Equal[Transport]} || !${Spell[${Me.Casting.ID}].Category.Equal[Heals]})) /stopcast
	}
	/if (TARGET) {
		/call clear_timer FALSE TARGET
	}

	/varset stAssTarID 0
	
	/varset dump ${maBurn.Add[swBurnConditionMet,FALSE]}
	/if (${Stick.Active}) /squelch /stick off
	/if (AUTO && ${Target.ID}) /squelch /target clear
	/if (!AUTO && !${Spawn[${Target.ID}].Type.Equal[CORPSE]}) /squelch /target clear
	
/return TRUE



|***
 * note: Clears all defined timers for a specific ID
 * use: /call clear_timer DEBUG [TARGETID]
 ***|
sub clear_timer(bool _debug, int _name)
	DEBUG \atclear_timer\ax(\a-w${_name}\ax)

	/if (${Defined[timer_${_name}]}) /deletevar timer_${_name}

/return



|***
 * note: returns a valid resident toon name .. if there is one
 * use : ${get_resident_class[DEBUG, DANNET GROUP(class), max distance to search]}
 ***|
sub get_resident_class(bool _debug, string _class, int _distance)
	DEBUG \atget_resident_class\ax(\a-w${_class}, ${_distance}\ax)
	
	| none of that class here
	/if (!${DanNet.PeerCount[${_class}]}) /return FALSE
	
	/varset _class ${_class.Upper}
	/declare _name string local FALSE
	/declare _count int local 0
	
	/for _count 1 to ${DanNet.PeerCount[${_class}]}		
		/varset _name ${DanNet.Peers[${_class}].Arg[${_count},|]}
		
		/if (!${Bool[pc ${_name}]}) /continue
		/if (!${Spawn[pc ${_name}].ID}) /continue
		/if (${Spawn[pc ${_name}].Dead}) /continue
		/if (${Spawn[pc ${_name}].Distance} > ${_distance}) /continue

		/if (${Bool[${_name}]}) /break
	/next _count	

/return ${_name}



|***
 * note: Make sure that what ever we are targeting is something we want to kill
 * note: in manual mode, anything is a valid target.
 * use: ${is_target_valid[DEBUG, TARGETID]}
**|
sub is_target_valid(bool _debug, int _tmpID)
	DEBUG \atis_target_valid\ax(\a-w${_tmpID}\ax)
	
	/if (!AUTO) /return TRUE
	
	ISMEDEAD
	GETINPUT
		
	/if (!${Target.ID}) /return FALSE
	/if (!${Bool[${_tmpID}]}) /return FALSE
	/if (SAFEZONE) /return FALSE

	| check smart assists
	/if (${Target.ID}) {
		/if (${Select[${maCC.Find[stAssistMode].Value},1,2,3]} && ${Spawn[${_tmpID}].ID} != ${Me.XTarget[1].ID}) {
			/call clear_combat FALSE
			/return FALSE
		} else /if (${Select[${maCC.Find[stAssistMode].Value},g]} && ${Group.MainAssist.ID} && !${Bool[${Spawn[${Group.MainAssist}].Dead}]} && ${Spawn[${_tmpID}].ID} != ${Me.XTarget[1].ID}) {
			/call clear_combat FALSE
			/return FALSE
		}
	}
	
	/if (!${maOver.Find[swOverPctEngage].Value} && !PULL) {
		/if (${Spawn[${_tmpID}].PctHPs} > ${maCC.Find[stPctEngage].Value} && !${Select[${maCC.Find[stAssistMode].Value},1,2,3,g]}) {
			/delay 1
			/return FALSE
		} 
	}
	
	| shit we really shouldnt't be attacking
	/if (${lsNoAttackBodyType.Contains[${Spawn[${_tmpID}].Type}]}) /return FALSE
	/if (${lsZoneExclude.Contains[${Spawn[${_tmpID}].DisplayName}]}) /return FALSE
	/if (${Target.ID} && ${Bool[${Target.Mezzed.Duration}]} && !PULL) /return FALSE
	/if (!PULL) {
		/if ((!${Spawn[${_tmpID}].LineOfSight} && !${maOver.Find[swOverLOS].Value}) || ${Spawn[${_tmpID}].Distance} > ENV) /return FALSE
	} else /if (PULL) {
		/if (${Spawn[${_tmpID}].Distance} > PULLENV) /return FALSE
	}
	/if (${Spawn[${_tmpID}].Type.Equal[PC]}) /return FALSE
	/if (${Spawn[${_tmpID}].Master.Type.Equal[PC]}) /return FALSE
	/if (${Target.Type.Equal[PC]}) /varset dump ${maCC.Add[swValidAssTarID,FALSE]}
	/if (${_tmpID} == ${Me.ID} || ${_tmpID} == ${Me.Pet.ID}) /return FALSE

	/if (${Select[${Me.Class.ShortName},ROG]}) {
		/if (${maEnv.Find[swSoS].Value}) /makemevisible
	}

/return TRUE



|***
 * note: Check if we need to engage burn routines. /combat heal/dual are treated differently
 * use: ${is_burn_validDEBUG}
 ***|
sub is_burn_valid(bool _debug)
	DEBUG \atis_burn_valid\ax()

	| dont do shit in safe zone..
	/if (${maEnv.Find[swZoneSafe].Value}) {
		/varset dump ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}

	| no raid burning if turned off
	/if (!${maBurn.Find[swBurnRaid].Value} && ${Raid.Members}) {
		/varset dump ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}
	
	| if always set to burn, always burn while in combat
	/if (${maBurn.Find[swBurnForce].Value} && ${Me.Combat}) {
		/varset dump ${maBurn.Add[swBurnConditionMet,TRUE]}
		/return TRUE
	}
	
	| remainder requires automated modes
	/if (!AUTO) /return FALSE

	| no mobs around
	/if (!${SpawnCount[npc radius ENV PS4]}) {
		/varset dump ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}
	
	| if the current target is a namer
	/if (${maBurn.Find[swBurnAuto].Value} && TARGET && ${Spawn[TARGET].PctHPs} <= ${maBurn.Find[stPctBurnEngage].Value} && (${lsZoneNamed.Contains[${Target.DisplayName}]} && !${maOver.Find[swOverName].Value})) {
		/varset dump ${maBurn.Add[swBurnConditionMet,TRUE]}
		/return TRUE
	}
	

	| if number of mobs in the local area reaches the /burn count ## threashold
	/if (${maBurn.Find[stBurnCount].Value}) {
		/if (${SpawnCount[npc radius ENV targetable PS4]} >= ${maBurn.Find[stBurnCount].Value}) {
			/varset dump ${maBurn.Add[swBurnConditionMet,TRUE]}
			/return TRUE
		}
	}
	
	| healers and off combat routines simply check for a named in the local area to start a burn. no direct target of a named is required. just be in the area.
	/if (!${maCC.Find[swCombatHeal].Value}) {
		/varset dump ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}

	/if (COMBAT) {
		/declare _count int local 0
		/declare _mobID int local 0

		/for _count 1 to ${SpawnCount[npc radius ENV PS4]}
			/varset _mobID ${NearestSpawn[${_count}, npc radius ENV].ID}
			/if (${maBurn.Find[swBurnAuto].Value} && ${lsZoneNamed.Contains[${Spawn[ID ${_mobID} radius ENV].DisplayName}]} && !${maOver.Find[swOverName].Value}) {
				/varset dump ${maBurn.Add[swBurnConditionMet,TRUE]}
				/return TRUE
			}
		/next _count 	
	}
	
/return FALSE



|***
 * note: do we want to exit the class while loop. assume no exit (FALSE)
 * use: ${exit_class_loopDEBUG}
 ***|
sub check_class_loop(bool _debug)
	DEBUG \atcheck_class_loop\ax()
	
	/if (AUTO) {	
		| no target.. were done
		/if (!${Spawn[TARGET].ID}) /return TRUE
		| targets dead. why bother
		/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return TRUE
		
	| manual mode	
	} else /if (!AUTO) {
		| no target. go away
		/if (!${Target.ID}) /return TRUE
		| forced combat in manual
		/if (COMBAT) {
			| target dead. end
			/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return TRUE
		}

	}
	
/return FALSE



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
			/varset dump ${maCC.Add[swSetCombat,FALSE]}
		}
		/if (!COMBAT) {
			/return FALSE
		}
		/if (${Target.ID}) {
			/if (${Target.ID} == ${Me.ID}) /return FALSE
			/if (COMBAT) {
				/varset dump ${is_burn_valid[${maDebug.Find[debugBurn].Value}]}
			}
			/return TRUE
		} else /if (!${Target.ID}) {
			/if (COMBAT || VALID) {
				/call clear_combat
			}
			/return FALSE
		}
	
	| combat healing/crowd control
	} else /if (AUTO && (HEAL || (${madeBuff.Find[swMez].Value} && ${maCC.Find[swCombatControl].Value}))) {
		DEBUG ${sep}auto-heal/CC combat check

		| /if (!SAFEZONE) /call set_base_target_timers TARGET
		/if (${is_target_valid[FALSE, TARGET]}) {
			/varset dump ${maCC.Add[swValidAssTarID,TRUE]}
		}
		/varset dump ${maCC.Add[swSetCombat,TRUE]}
		/varset dump ${is_burn_valid[${maDebug.Find[debugBurn].Value}]}
		
		/return TRUE
	
	
	| automatic mode
	} else /if (AUTO) {
		DEBUG ${sep}auto combat check

		
		/if (${Me.Invis} && ${Me.Class.ShortName.NotEqual[ROG]}) /return FALSE
		
		/if (${Me.Class.ShortName.Equal[BRD]}) {
			/if (${Me.Song[Rallying Solo].ID}) /return FALSE
		}
		
		/if (${Me.Invis} && ${Me.Class.ShortName.Equal[ROG]} && !${maEnv.Find[swSoS].Value}) /return FALSE
			
		| give mezers a free pass with more then 1 mob
		| /if (${swdeBuffMezz} && !${mobList.Find[|]}) {
		| 	/if (${SpawnCount[npc los radius ENV zradius ${setzradius} targetable]} > 1) {
		| 		/delay 1s
		| 		/return TRUE
		| 	}
		| }
		
		/if (${is_target_valid[FALSE, TARGET]}) {
			/varset dump ${maCC.Add[swValidAssTarID,TRUE]}
		}
		
		/if (!VALID || !${Spawn[TARGET].ID}) {
			/call clear_combat FALSE
			/return FALSE

		} else /if (VALID && ${Spawn[TARGET].ID}) {
			DEBUG ${sep} \ayelse /if (VALID && ${Spawn[TARGET].ID})\ax
			| /if (!SAFEZONE) /call set_base_target_timers TARGET
			| /if (${Me.Class.ShortName.Equal[BRD]}) {
				| /if (${Me.Song[${spellRestSong}].ID}) /stopsong
			| }
			/varset dump ${maCC.Add[swSetCombat,TRUE]}
			/varset dump ${is_burn_valid[${maDebug.Find[debugBurn].Value}]}
			/return TRUE
			
		}
	}

/return FALSE



|***
 *	DES: preps toon for combat modes
 *	USE: /call set_combat DEBUG
 *	NOTE: 
 ***|
sub set_combat(bool _debug)
	DEBUG \atset_combat\ax()

	/if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return
	
	CHECKCURSOR
	
	| out of range?
	/if (${Target.Distance} > ENV) /return
	
	/if (${Me.State.NotEqual[STAND]}) /stand
	/if (AUTO && ${Me.State.Equal[FEIGN]}) /squelch /stand
	/if (!AUTO && ${Me.Invis}) /makemevisible
		
	| not within ${combatradius} and far enought to range?
	/if (${Target.Distance} > 30 && ${Target.Distance} > ${Spawn[id TARGET].MaxRangeTo}*.9) {

		| range mobs if enabled
		/if (RANGE) {
			
			/call check_facing FALSE TARGET FORCE
			
			| no need to move were are at a good distance
			/if (${Stick.Active}) /squelch /stick off		
		
			/if (${lsZoneRange.Contains[${Target.DisplayName}]}) {
				/if (${Me.AutoFire}) /squelch /autofire
			} else {
				/if (${Spawn[TARGET].LineOfSight} && !${Me.AutoFire}) /squelch /autofire
			}

		| cannot range, so melee if in agro range?
		} else /if (!RANGE && MELEE) {
			/if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && !AGRO)) {
				/squelch /attack off
				/return
			} else {
				/if (${Me.AutoFire}) /squelch /autofire
				/call check_facing FALSE TARGET FORCE
				/squelch /attack on
				/if (AUTO) /call stick_to_target FALSE TARGET
			}

		}
	}

	| mob too close for range
	/if (${Target.Distance} <= 31) {

		| adjust range if melee is diabled to keep close to target. give them some range to.. range again
		/if (${Target.Distance} < 31 && RANGE && !MELEE) {
			/if (AUTO && !${Stick.Active} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) /call moveto_range TARGET 35
			
		| can we get to the melee already?
		} else /if (MELEE) {
			| /if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || ${check_no_melee_buff[FALSE]} || (AUTO && ${Target.DSed.ID} && ${stoponDS} && !${swAgro})) {
			/if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && !AGRO)) {
				/squelch /attack off
				/return FALSE
			} else {
				/if (${Me.AutoFire}) /squelch /autofire
				/call check_facing FALSE TARGET FORCE
				/squelch /attack on
				/if (AUTO) /call stick_to_target FALSE TARGET
			}
		}
	}

/return



|***
 * note: finds a buff you cast on the current target
 * note: response is in duration (in seconds) left on the buff
 * use: ${get_target_buff_time_left[DEBUG, [BUFF NAME]]}
 ***|
sub get_target_buff_time_left(bool _debug, string _buff)
	DEBUG \atget_target_buff_time_left\ax(\a-w${_buff}\ax)
	
	/if (!${Target.ID} || ${Target.Dead}) /return FALSE
	
	/declare _count int local 0
	/for _count 1 to ${Target.BuffCount}
		/if (!${Target.Buff[${_count}].Name.Find[${_buff}]}) /continue
		/if (${Target.Buff[${_count}].Caster.Equal[${Me.DisplayName}]}) /break
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
	GETINPUT
	CHECKTIE

	SETHUD "pull base"		
	 
	| A naked blonde walks into a bar with a poodle under one arm and a two-foot salami under the other. 
	| She lays the poodle on the table. Bartender says,'I suppose you won't be needing a drink.' 
	| Naked lady says...
	/if (${_tmpID}) {
		/varset stAssTarID ${_tmpID}
	}	else {
		/call pull_mob_find ${_debug}
	}
	
	DEBUG ${sep}target${sep}\a-wTARGET
	
	| When you grow up, your heart dies.
	/if (!TARGET) {
		SETHUD FALSE
		/if (HOME && !${SpawnCount[npc radius 15 PS4 targetable]}) {
			/call Bind_control_home return
		}
		/return
	}	

	| Could you describe the ruckus, sir?
	/if (TARGET) {
		/if (${Target.ID} != TARGET) {
			/varset dump ${set_target[TARGET]}
		}
	} 

	
	| Screws fall out all the time, the world is an imperfect place.
	/call pull_mob_prep ${_debug} TARGET

	/if (MELEE) /attack on

	| wait for mob to get to camp
	DEBUG ${sep}loop \a-w:waitformob\ax
	:waitformob
	/if (!${SpawnCount[npc radius 31 PS4 targetable]}) {
		/declare _wait int local ${Math.Calc[(PULLENV + ${Spawn[TARGET].Distance})*.08]}
		/call set_timer FALSE _wait_for_mob ${_wait}s
	
		/if (!${Spawn[TARGET].Dead}) {
			/varset dump ${out[9, "Waiting on${sep}${tar}${Spawn[TARGET].DisplayName}\ax${sep}\a-w${_wait}s\ax"]}
		}
	
		/while (TRUE) {
			DEBUG ${sep}\awTARGET\ax.distance${sep}\a-w${Spawn[TARGET].Distance}\ax${sep}\a-w${_wait_for_mob}\ax
			GETINPUT
			/delay 5
			
			/if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID}) {
				/call check_buff_cycle_ammo FALSE TRUE
			}			
			
			/if (!TARGET || ${Spawn[TARGET].Dead}) {
				/call clear_combat FALSE
				DEBUG ${sep}/while \a-gbreak_NO_target\ax
				/break			
			}

			| mobs in our personal space. fuck those things. no respect
			/if (${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
				DEBUG ${sep}/while \a-gbreak_MOB IN SAFE SPACE\ax
				/break
			}

			| target LOS & within tag range
			/if (${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} < ENV && ${Spawn[TARGET].Distance} < 100 && RANGE) {
				DEBUG ${sep}/while \a-gbreak_tag\ax
				/break
			}
			
			| target within melee
			/if (${Spawn[TARGET].Distance} <= ${Spawn[TARGET].MaxRangeTo}*.9) {
				DEBUG ${sep}/while \a-gbreak_MaxRangeTo\ax
				/break
			}
			
			| LoS time reduction
			/if (!${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} < ENV) {
				| /varcalc _wait_for_mob ${_wait_for_mob} - 10
				| /call set_timer FALSE _wait_for_mob ${_wait_for_mob}s
				| /varset dump ${out[9, "Target \arLoS\ax${sep}\a-w${_wait_for_mob}s\ax"]}
				/delay 1s
			}
			
			
			| timer ran out
			/if (!${_wait_for_mob}) {
				/varset dump ${out[9, "Sick of waiting on${sep}\a-w${Spawn[TARGET].DisplayName}\ax"]}
				/call clear_combat FALSE
				/break
			}
		}
	}

	SETHUD FALSE

/return

${Spawn[${Target.ID}].MaxRangeTo}


|***
 * note: 
 * use: /call pull_mob_prep DEBUG [TARGETID]
 ***|
sub pull_mob_prep(bool _debug, int _tmpID)
	DEBUG \a-tpull_mob_prep\ax(\a-w${_tmpID}\ax)
	
	/declare _count int local 0
	/declare _los string local los

	| advanced pulling does not require LoS, clear the reqs for anything except base pulling
	/if (${maOver.Find[swOverLOS].Value} || ${Select[${maCC.Find[stPullMode].Value},int,pet,calm,multi]}) {
		/varset _los
	} 

	| exclude list pulls. clear these targets.
	/if (${lsZoneExclude.Contains[${Spawn[ID ${_tmpID}].DisplayName}]}) {
		DEBUG ${sep}Pull Excludelist${sep}\a-w${lsZoneExclude.Contains[${Spawn[ID ${_tmpID}].DisplayName}]}\ax \arreset\ax
		/varset stAssTarID 0
		/varset _tmpID 0
		/return
	}

	| get the mob to us for non pet classes..
	/while (!${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]} && !${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) { 
		DEBUG ${sep}pull_mob_prep${sep}\awLoop.Start\ax${sep}${_tmpID}
		
		| keep tabs on targets range if they are the puller. advanced pullers fetching we do not care about yet
		/if (${Spawn[ID ${_tmpID}].Distance} > PULLENV && PULL) {
			/call clear_combat FALSE
			/return
		}
		
		| CHECKCURSOR TRUE
		GETINPUT
		ISMEDEAD

		| stand the fuck up. seriously...
		/if (${SpawnCount[npc ${_los} radius PULLENV targetable]} && ${Me.State.NotEqual[STAND]}) /stand
		| ain't got no time for that
		/if (${Me.Stunned}) /return

		| keep us facing the target just enough...
		/if (${Target.ID} && (MELEE || RANGE) && ${Spawn[ID ${_tmpID}].LineOfSight}) {
			DEBUG ${sep}Facing${sep}${_tmpID}
			/call check_facing FALSE ${_tmpID} FORCE
		}
			
		| check target is LoS. if stays out of sight too long. reset.
		| again only the puller cares about this, not fetchers
		/if (!${Spawn[${_tmpID}].LineOfSight} && !${maOver.Find[swOverLOS].Value} && PULL) {
			/varset dump ${set_timer[FALSE, timer_wait_nolos, 30s]}
			/varset dump ${out[9, "Waiting for pull (\a-wLOS\ax)"]}
			/while (${timer_wait_nolos}) {
				/delay 2 
				/if (${Spawn[ID ${_tmpID}].LineOfSight}) /break
				/if (${Spawn[ID ${_tmpID}].Distance} > PULLENV*2) /break
				/if (${Spawn[ID ${_tmpID}].ID}) /break 
			}
			| /call clear_combat FALSE
			/return
		}
		

		| time to tag the T2K. onced tagged release
		DEBUG ${sep}pull_mob_prep${sep}\awLoop.Tag\ax${sep}${_tmpID}
		/call pull_mob_tag ${_debug} ${_tmpID}
		/if (${Macro.Return}) {
			/nav stop log=off
			/break
		}


		/delay 1
		/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /break
	}
	
	| pet auto send to kill
	/if (${Spawn[ID ${_tmpID}].Distance} <= PULLENV && ${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) {
		DEBUG ${sep}Pet Range${sep}${_tmpID}
		/if (${Target.ID} != ${_tmpID}) {
			/varset dump ${set_target[${_tmpID}]}
			}
		/if (${Target.ID} && !${Bool[${Me.Pet.Target}]}) {
			/squelch /pet attack		
		}
		/return
	}

/return



|***
 * note: 
 * use: /call pull_mob_tag DEBUG [TARGETID]
 ***|
sub pull_mob_tag(bool _debug, int _tmpID)
	DEBUG \atpull_mob_tag\ax(\a-w${_tmpID}\ax)

	/declare _count int local 0
	
	/if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID}) {
		/call check_buff_cycle_ammo FALSE TRUE
	}
	
	/if (${Spawn[ID ${_tmpID}].LineOfSight}) {
		DEBUG ${sep}TargetLOS

		| attraction pulls/agro
		/if (${Select[${Me.Class.ShortName},SHD,WAR,PAL]} && ${maChr.Find[swAttraction].Value}) {
			/if (${Spawn[ID ${_tmpID}].Distance} < PULLENV && ${Spawn[ID ${_tmpID}].Distance} > 40) {
				/declare _attraction string local FALSE
				/if (${Select[${Me.Class.ShortName},SHD]}) /varset _attraction Hate's Attraction
				/if (${Select[${Me.Class.ShortName},WAR]}) /varset _attraction Warlord's Grasp
				/if (${Select[${Me.Class.ShortName},PAL]}) /varset _attraction Divine Call
				/if (${Select[${Me.Class.ShortName},MON]}) /varset _attraction Moving Mountains
				
				/if (${is_ready[FALSE, "${_attraction}"]}) {
					DEBUG ${sep}${Me.Class.ShortName} _attraction${sep}\aw${_attraction}\ax
					/call voodoo FALSE "${_attraction}" ${_tmpID} FALSE
				}
			}
		}
					
		| aa/spell/disc/item tag
		/if (${is_ready[FALSE, "${maCC.Find[stRangeOther].Value}"]}) {			
			DEBUG ${sep}Tag${sep}Other${sep}[${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax]${sep}\a-w${maCC.Find[stRangeOther].Value}\ax
			/if (${Spawn[ID ${_tmpID}].Distance} < 80) {
				/call set_stop FALSE				
			}
			/delay 1		
			/call voodoo FALSE "${maCC.Find[stRangeOther].Value}" ${_tmpID} FALSE
			/delay 5
			/return TRUE
				
					
		| combat tag	
		} else {
			| get our target
			/if (${Target.ID} != ${_tmpID}) {
				/varset dump ${set_target[${_tmpID}]}
			}
			
			| range tag
			| there are some base limitations to combat range tagging. we are going to limit to 274 for range/thrown items
			| yes i know some classes can go a lot farther..
			/if (${Spawn[ID ${_tmpID}].Distance} > 31 && ${Spawn[ID ${_tmpID}].Distance} < 275) {
				DEBUG ${sep}Tag${sep}Range${sep}[${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax]
				GETINPUT
				| /if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID}) {
				| 	/call check_buff_cycle_ammo FALSE TRUE
				| }
				/nav stop log=off
				/delay 1s !${Me.Moving}
				/if (${Target.ID}) /range
				/while (${Target.AggroHolder.Name.NotEqual[${Me.DisplayName}]} || ${_count} < 4) {
					/if (${Target.ID} && ${Spawn[ID ${_tmpID}].Distance} > 31) {
						/range
						/delay 5
						/varcalc _count ${_count}+1
					} else {
						/return FALSE
					}				
				}
				
				/if (${Target.ID}) /range
				/delay 1
				/return TRUE
			
			| melee tag
			| anything less then what we can range at. magic number is 31 paces
			} else /if (${Spawn[ID ${_tmpID}].Distance} < 31) {
				DEBUG ${sep}Tag${sep}Melee${sep}[${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax]
				GETINPUT
				/attack on
				
				| we are using 90% of the targets max range to melee just to make sure we get close enough.
				| dont argue with me on this. i'm not changing my mind
				| if we are not in this range, we try and get closer.
				/if (${Spawn[ID ${_tmpID}].Distance} > ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]}) {
					/if (MELEE) /nav ID ${_tmpID} |dist=${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]} log=off
					/delay 5 ${Me.Moving}
					| /delay 3s !${Me.Moving}
				}
				/delay 2
				| make sure we have the targets attention.
				/delay 2s ${Target.AggroHolder.Name.Equal[${Me.DisplayName}]}
				/return TRUE
			
			}
				
		}
	} else /if (!${Spawn[${_tmpID}].LineOfSight} && !${maOver.Find[swOverLOS].Value}) {
		DEBUG Target\arLOS\ax
			/varset dump ${set_timer[FALSE, timer_wait_nolos, 30s]}
			/varset dump ${out[9, "Waiting for pull (\a-wLOS\ax)"]}
			/while (${timer_wait_nolos}) {
				/delay 2 
				/if (${Spawn[ID ${_tmpID}].LineOfSight}) /break
				/if (${Spawn[ID ${_tmpID}].Distance} > PULLENV*2) /break
				/if (!${Spawn[ID ${_tmpID}].ID}) /break 
			}
			/call clear_combat FALSE
			/return FALSE
		}	
	
	/return FALSE



|***
 * note: Finds and sets stAssTarID
 * use: /call pull_mob_find DEBUG [# to force SpawnCount increment]
 ***|
sub pull_mob_find(bool _debug, int _spCount)
	DEBUG \atpull_mob_find\ax(\a-w${_spCount}\ax)
	
	/if (!${Bool[${_spCount}]}) /varset _spCount 0

	/if (HOME && !${SpawnCount[npc radius SAFEENV PS4 targetable]}) {
		/call Bind_control_home return
	}
	
	/if (${_spCount}) {
		/varset stAssTarID 0
	}
		
	| we have an existign target
	/if (TARGET && !${_spCount}) /return TARGET
	
	/declare _tmpID int local
	/declare _tmpAgroRange int local ${Int[${Math.Calc[ENV/10]}]}
	/declare _count int local 0
	/declare _mobHP int local 999
	/declare _tmpHPAdj int local 0
	/declare _los string local los
	/declare _tmpCombatRange int local 31

	| adjust LOS?
	/if (${maOver.Find[swOverLOS].Value} || ${Select[${maCC.Find[stPullMode].Value},int,pet,calm,multi,nav]}) {
		/varset _los
	} 
	
	| time to step the agro range out so we get mobs closer first	
	DEBUG ${sep}range${sep}\aw${maEnv.Find[stMobLvlMin].Value}\ax-\aw${maEnv.Find[stMobLvlMax].Value}\ax zradius${sep}\a-w${maCC.Find[stZRadius].Value}\ax
	
	/while (${_tmpAgroRange} <= PULLENV && !TARGET) {

		| search local ENV range in steps of 10 for mob acquisition
		DEBUG ${sep}count:\aw${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}\ax radius${sep}\ay${_tmpAgroRange}\ax
		/if (${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}) {
			/for _count 1 to ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}
				| /if ((!PULL || !AUTO) && !${swModeToken}) /return -1
				ISMEDEAD
				
				/varset _tmpID ${NearestSpawn[${If[${_spCount},${Math.Calc[${_count}+${_spCount}]},${_count}]}, npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value}].ID}
				
				| fish are friends, not food
				/if (${lsNoAttackBodyType.Contains[${Spawn[id ${_tmpID}].Body}]}) /continue
				
				| NPCs to not consider
				/if (${Spawn[id ${_tmpID}].Type.Equal[NPC]}) {
					/if (${lsNoTargetNPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /continue
				}
				
				| PCs to not consider
				/if (${Spawn[id ${_tmpID}].Type.Equal[PC]}) {
					/if (${lsNoTargetPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /continue
				}
				
				| magic creatures to not consider
				/if (${Spawn[id ${_tmpID}].Type.Equal[Magic]}) {
					/if (${lsNoTargetMagic.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /continue
				}

				/if (${lsZoneExclude.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /continue
				/if (${_tmpID} == ${Me.Pet.ID}) /continue
				
				| CYA for shit MQ will pick up in the environment that does not have a name
				/if (${Spawn[ID ${_tmpID}].DisplayName.Length} < 1) /continue
				
				| added because MQ sucks ass for getting accurate HP values returned over a mouse dick in length.
				/if (${Spawn[ID ${_tmpID}].PctHPs} > 100 && ${_tmpAgroRange} > 200) {
					/varset _tmpHPAdj 100
				} else {
					/varset _tmpHPAdj ${Spawn[id ${_tmpID}].PctHPs}
				}

				| get the lowest HP mob if we can
				/if (${_tmpHPAdj} <= ${_mobHP}) {
					/varset _mobHP ${Spawn[id ${_tmpID}].PctHPs}
					/varset stAssTarID ${_tmpID}
					/varset dump ${maCC.Add[swSetCombat,TRUE]}
					DEBUG ${sep}T2K${sep}\awTARGET\aw${sep}\ay${Spawn[TARGET].DisplayName}\ay ${sep} ${If[${Spawn[TARGET].LineOfSight},\ag${Spawn[TARGET].Distance}\ax,\ar${Spawn[TARGET].Distance}\ax]}
				}

				| is this a priority mob?
				/if (${lsZoneNoMez.Contains[${Spawn[ID ${_tmpID}].DisplayName}]} || ${lsZoneNamed.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
					DEBUG ${sep}Priority Mob${sep}\a-w${Spawn[id ${_tmpID}].DisplayName}\ax
					| off tanks have to sort by a different position
					/if (TARGET && ${Me.XTarget[1].ID} == ${_tmpID}) /continue 
					/return TARGET	
				}

			/next _count
		
		} 
		
		/if (TARGET) /return TARGET	
		/varcalc _tmpAgroRange ${_tmpAgroRange}+${Math.Calc[ENV/10]}
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
	
 	/if (${Me.Gem[${_spellname}]}) /return TRUE
	ISMEDEAD
	GETINPUT
	
	/declare _ready bool FALSE
	
	| cant open a book with shit in hands
	/if (${Cursor.ID}) {
		/call check_cursor FALSE TRUE CLEAR
	}

	| set timer for.. whatever
	/if (${_wait}) {
		/declare _breakTimer timer local 30s
	} else {
		/declare _breakTimer timer local 5s
	}
	
	DEBUG _breakTimer${sep}\a-w${_breakTimer}\ax

	| do we even have the spell?
	/if (!${Me.Book[${_spellname}]}) {
		/varset dump ${out[0, "${notice}${sep}Bad Spell Name ${dot} ${_spellname}"]}
		/return FALSE
	}
	
	| just the facts ma'am
	/call get_gem_id FALSE "${_spellgem}"
	/declare _gem int local ${Macro.Return}
	
	/while (${Me.State.NotEqual[SIT]} && !${Me.Mount.ID}) {
		/sit
		/delay 1s ${Me.State.Equal[SIT]}
	}
	
	/if (${_echo}) ${out[11, "Gem${_gem}${sep}\a-w${_spellname}\ax"]} 
	
	/while (!${_ready}) {
		/memspell ${_gem} "${_spellname}"
		/delay 5
		/delay 6s ${Me.Gem[${_gem}].Name.Equal[${_spellname}]}
		
		/if (${Me.Gem[${_gem}].Name.Equal[${_spellname}]}) /varset _ready TRUE
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
 *	DES: mem lines
 *	USE: /call set_line_to DEBUG [linename]
 ***|
sub set_line_to(bool _debug, string _line)


	ISMEDEAD
	CHECKEXIT
	GETINPUT
	
	/declare _currentSong 	string NULL
	
	/for _count 1 to ${_list.Size}


		/for _index 1 to ${_list[${_count}].Count[|]}

			/varset _currentSong ${Spell[${li.Value}].RankName}
			/if (${Me.Gem[${_index}].Name.Equal[${_currentSong}]}) /continue
			/call mem_spell FALSE TRUE "${_currentSong}" "${_index}" FALSE


			| /call set_spell_to DEBUG [echo memming T/F] ["spell name"] [gem to mem it in] [wait for recycle to cast T/F]


		/next _index	

	/next _count	
	
	
	/varset dump ${out[11, "Line ${sep} ${info}${lineCurrent}\ax memorization completed."]}
	
	
/return TRUE




		/vardata li _line.First.Clone
		/while (!${li.IsEnd}) {



			/varset dump ${li.Advance}
		}	









|***
 * note: corrects the Gem## and returns the # only #
 * use: /call get_gem_id DEBUG [gem##] | ${get_gem_id[DEBUG, gem##]}
 *	NOTE:
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
 * use: ${check_exitDEBUG}
 ***|
sub check_exit(bool _debug)
	DEBUG \atcheck_exit\ax()
	
	| invis 
	/if (${Me.Invis}) /return TRUE
	
	| movement
	/if (${Me.Moving} && !${Select[${Me.Class.ShortName},BRD]}) /return TRUE

	| SELF STATES
	/if (${Me.Invulnerable.ID}) /return TRUE	
	/if (${Me.State.Equal[FEIGN]}) /return TRUE
	/if (${Me.Ducking}) /return TRUE
	/if (${Me.Stunned}) /return TRUE
	/if (${Me.Charmed.ID}) /return TRUE
	/if (${Me.Mezzed.ID}) /return TRUE
	
	| check no cast list and mezz?
	/if (AUTO && ${Target.ID}) {
		/if (${lsZoneNoCast.Contains[${Target.DisplayName}]}) /return TRUE
		/if (${Target.Mezzed.ID} && !${maCC.Find[swPull].Value}) /return TRUE
	}

/return FALSE



|***
 * note: Check to see if toon is engaged in any way.
 * use: /call check_engaged DEBUG || ${check_engagedDEBUG}
 ***|
sub check_engaged(bool _debug)
	DEBUG \atcheck_engaged\ax()
	
	GETINPUT
	/if (${Select[${Me.Class.ShortName},ROG]}) {
		/if (AUTO && ${maEnv.Find[swSoS].Value}) /return FALSE
	}
	
	/if (${Me.Invis}) /return TRUE
	
	| /if (!${swBuffCombat}) {
	| 	/if (${SpawnCount[npc OVERLOS radius ENV PS4]}) /return TRUE
	| }

	/if (${Me.Casting.ID}) /return TRUE

	| movement
	/if (${Me.Moving} && !${Select[${Me.Class.ShortName},BRD]}) /return TRUE
	
	| combat
	/if (${Me.Combat}) /return TRUE
	/if (${Me.AutoFire}) /return TRUE
	
	| am i the puller or main agro with mobs in range?
	/if (PULL && AGRO) {
		/if (${SpawnCount[npc OVERLOS radius 15*2 PS4]}) /return TRUE
	}
	/if (${Group.MainAssist.Name.Equal[${Me.DisplayName}]}) {
		/if (${SpawnCount[npc OVERLOS radius 15*2 PS4]}) /return TRUE
	}

/return FALSE



|***
 * note: rest/not rest routines.
 * use: /call do_rest
 ***|
sub do_rest(bool _debug)
	DEBUG \atdo_rest\ax()
	
	| the dead dont need to rest
	/if (!AUTO || ${swIsMeDead}) /return FALSE
	
	| dont rest while in tie
	/if (TIE) /return FALSE
	
	| clerics and pallys wait 'till yaulp gone
	/if (${Me.Buff[Yaulp].ID}) /return
	GETINPUT

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
			/call set_stop FALSE SILENT
		} else /if (${Me.Casting.ID} || ${Window[CastingWindow].Open}) {
			/stopcast
		}
		DEBUG ${sep}dont need rest
		/return
	}

	ISMEDEAD
	CHECKCURSOR
	CHECKTIE
	GETINPUT
	| check self click buffs
	/if (!${Me.Invis} && ${maEnv.Find[swRestBuff].Value}) {
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
			/varset dump ${out[5, "Lullaby${sep}\a-w${Spell[${maChr.Find[stSongRest].Value}].RankName}\ax"]}
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
			
	/if (PCTREST && !${Me.Moving} && ((${maEnv.Find[swRestCombat].Value} && ${Me.CombatState.NotEqual[combat]}) || !${SpawnCount[npc OVERLOS radius ENV PS4]})) {
			
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
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
				/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) /call check_harmonious

			} else /if (${Select[${Me.Class.ShortName},RNG]}) {
			} else /if (${Select[${Me.Class.ShortName},BST]}) {
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
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
					/if (${maHeal.Find[swHealGroup].Value} && ${Group.Members}) /call check_heal_${Me.Class.ShortName} FALSE Group
					/if (${maHeal.Find[swHealXTarget].Value}) /call check_heal_${Me.Class.ShortName} FALSE XTarget
				}				
			} else /if (${Select[${Me.Class.ShortName},DRU]}) {
				| /if (${maCC.Find[swCombatHeal].Value} && ${SpawnCount[npc radius ENV PS4]}) {
				| 	/if (${swHealGroup}) /call check_heal_${Me.Class.ShortName} FALSE Group
				| 	/if (${swHealXTarget}) /call check_heal_${Me.Class.ShortName} FALSE XTarget
				| }				
			} else /if (${Select[${Me.Class.ShortName},SHM]}) {
				/call check_cani ${maDebug.Find[canni].Value}
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
			} else /if (${Select[${Me.Class.ShortName},ENC]}) {
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
			} else /if (${Select[${Me.Class.ShortName},WIZ]}) {
			} else /if (${Select[${Me.Class.ShortName},NEC]}) {
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
			} else /if (${Select[${Me.Class.ShortName},MAG]}) {
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[minion].Value} TRUE
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
		GETINPUT
		/goto :loopneedmorerest
	} 
	
	SETHUD FALSE
/return



|***
 * note: Acquire target, get bitches. This will establish ${stAssTarID} for everyone.
 * use: /call get_assist_target DEBUG
 ***|
sub get_assist_target(bool _debug)
	DEBUG \atget_assist_target\ax()
	
	ISMEDEAD
	GETINPUT

	/if (!AUTO) /return
	
	| if no MA present. move along..	
	/if (!${Spawn[${maCC.Find[stManualAssist1].Value}].ID} && !${Spawn[${maCC.Find[stManualAssist2].Value}].ID} && !${Select[${maCC.Find[stAssistMode].Value},g,1,2,3]}) {
		/delay 1
		/return false
	}
	
	| cannnot assist yourself
	DEBUG ${sep}cannot assist self
	/if (${Group.MainAssist.Name.Equal[${Me.DisplayName}]}) /return
	/if (${Spawn[pc ${maCC.Find[stManualAssist1].Value}].ID} == ${Me.ID} || ${Spawn[=${Raid.MainAssist} pc].ID} == ${Me.ID}) {
		/varset dump ${do_raw_edit[FALSE, TRUE, stManualAssist1, FALSE]}
	}
	/if (${Spawn[pc ${maCC.Find[stManualAssist2].Value}].ID} == ${Me.ID} || ${Spawn[=${Raid.MainAssist} pc].ID} == ${Me.ID}) {
		/varset dump ${do_raw_edit[FALSE, TRUE, stManualAssist2, FALSE]}
	}
	
	| if we dont have xtargetsd, skip ALL smartassist go directly to jail. 
	/if (!${Me.XTargetSlots}) /goto :skippingtoMAtarget

	| tar acquisition when using smart assist. set XTarget positions.
	/if (${Select[${maCC.Find[stAssistMode].Value},g,1,2,3]}) {
		DEBUG ${sep}SmartAssist${sep}\a-w${maCC.Find[stAssistMode].Value}\ax [1,2,3,g]
		
		| skip corpses and me
		/if (${Spawn[id ${stAssTarID}].Dead} || ${stAssTarID} == ${Me.ID}) {
			DEBUG ${sep} bad target${sep}\a-wcorpse or self\ax
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
			/if (${Me.XTarget[1].TargetType.NotEqual[Group Assist Target]} && ${Group.Members}) /xtarget set 1 groupassisttarget
		} else /if (${Select[${maCC.Find[stAssistMode].Value},1]}) {
			DEBUG ${sep}set Xtarget${sep}\a-w1\ax
			/if (${Me.XTarget[1].TargetType.NotEqual[Raid Assist 1 Target]} && ${Raid.Members}) /xtarget set 1 raidassist1target
		} else /if (${Select[${maCC.Find[stAssistMode].Value},2]}) {
			DEBUG ${sep}set Xtarget${sep}\a-w2\ax
			/if (${Me.XTarget[1].TargetType.NotEqual[Raid Assist 2 Target]} && ${Raid.Members}) /xtarget set 1 raidassist2target
		} else /if (${Select[${maCC.Find[stAssistMode].Value},3]}) {
			DEBUG ${sep}set Xtarget${sep}\a-w3\ax
			/if (${Me.XTarget[1].TargetType.NotEqual[Raid Assist 3 Target]} && ${Raid.Members}) /xtarget set 1 raidassist3target
		}
		
		
		| /delay 2s ${Me.XTarget[1].ID}
		
		| smart assist group
		DEBUG ${sep}GET Target
		
		| set main target variable
		/if (${Me.XTarget[1].ID} && TARGET != ${Me.XTarget[1].ID}) {
			DEBUG ${sep}setting${sep}stAssTarID ${Me.XTarget[1].ID}
			/varset stAssTarID ${Me.XTarget[1].ID}
		}
				
		| set actual pc target
		/if (${Spawn[ID ${stAssTarID}].Distance} <= ENV && ${stAssTarID}) {
			/if (MELEE || RANGE || NUKE || DOT) {
				/if (${Me.XTarget[1].PctHPs} <= ${maCC.Find[stPctEngage].Value}) {
					DEBUG targeting${sep}stAssTarID${sep}${maCC.Find[stPctEngage].Value}
					/if (${Target.ID} != ${stAssTarID}) {
						/squelch /target id ${stAssTarID}
						/delay 1s ${Target.ID} == ${stAssTarID}
					}
				}
			}
		}
		
		/return TRUE
	}

	| target acquisition when not using manual assist.
	:skippingtoMAtarget
	/if (${SpawnCount[npc radius ENV]} && (${Bool[MA1]} || ${Bool[MA2]})) {

		| stManualAssist1
		/if (${Bool[MA1]} && ${Spawn[MA1].ID} && !${Spawn[MA1].Dead}) {
			/if (MELEE || RANGE || NUKE || DOT) /squelch /assist MA1
			/delay 2s ${Me.AssistComplete}
			/if (!TARGET && ${Target.ID}) {
				/varset stAssTarID ${Target.ID}
			} else /if (TARGET != ${Target.ID} && TARGET != 0 && ${Target.ID} && ${Spawn[${Target}].Type.Equal[npc]}) {
				/delay 1
				/varset stAssTarID 0
				/return
			}
	
		| stManualAssist2
		} else /if (${Bool[MA2]} && ${Spawn[MA2].ID} && !${Spawn[MA2].Dead} && (!${Spawn[MA1].ID} || ${Spawn[MA1].Dead})) {
			/if (MELEE || RANGE || NUKE || DOT) /squelch /assist MA2
			/delay 2s ${Me.AssistComplete}
			/if (!TARGET && ${Target.ID}) {
				/varset stAssTarID ${Target.ID}
			} else /if (TARGET != ${Target.ID} && TARGET != 0 && ${Target.ID} && ${Spawn[${Target}].Type.Equal[npc]}) {
				/delay 1
				/varset stAssTarID 0
				/return
			}	
			
		} 		
		ISMEDEAD
		GETINPUT
		/if (TARGET == ${Target.ID}) {
			/if (${Target.PctHPs} > ${maCC.Find[stPctEngage].Value} && !${swOverrideEngagePct}) {
				/delay 3
				/goto :skippingtoMAtarget
			} 
		} else {
			/if (${Target.ID}) /squelch /target clear
			/return
		}
	}
	
/return



|***
 * note: creates hoos for custom.inc
 * use: GETINPUT [sub] [variable]
 ***|
sub register_observer(string _sub, string _variable)
	/if (!${maEntropy.Find[swHookSub].Value}) /return FALSE

	/if (${maHook.Contains[hook_${_sub}]}) {
		/if (${maHook.Find[hook_${_sub}].Value.Arg[2,|].Equal[${_variable.Arg[2,|]}]}) {
			/return FALSE
		}
		/varset dump ${maHook.Add[hook_${_sub},${_variable}]}
		/if (${SubDefined[hook_${_sub}]}) {
			/call hook_${_sub} ${_variable}
			/return ${Macro.Return}
		}

	} else {
		/varset dump ${maHook.Add[hook_${_sub},${_variable}]}
		/return FALSE
	}
	
/return FALSE



|***
 * note: updates working zone lists
 * use: /call set_zone_lists DEBUG
 ***|
sub set_zone_lists(bool _debug)
	DEBUG \atset_zone_lists\ax()
	
	| check zone exists
	/sqlite ${maData.Find[DBzone].Value} dummy SELECT count(*) as data from zone_control where zone_shortname='${Zone.ShortName}'
	DEBUG ${sep}dummy SELECT count(*) as data from zone_control where zone_shortname  \ay${sqlite.Status[dummy]}\ax resutl:${sqlite.Result[dummy 1 data]}
	/if (${sqlite.Status[dummy].Find[no such table:]}) {
		/sqlite ${maData.Find[DBzone].Value} dummy INSERT INTO zone_control values ('${Zone.ShortName}', 'swZoneSafe', 'FALSE')
	}

	| get safe zone
	/sqlite ${maData.Find[DBzone].Value} dummy SELECT count(*) as data from zone_control where zone_shortname='${Zone.ShortName}'
	|  and element='swZoneSafe'
	DEBUG ${sep}SELECT count(*) as data from zone_control where zone_shortname \ay${sqlite.Status[dummy]}\ax resutl:${sqlite.Result[dummy 1 data]}
	/if (!${sqlite.Result[dummy 1 data]}) {
		/sqlite ${maData.Find[DBzone].Value} dummy INSERT INTO zone_control values ('${Zone.ShortName}', 'swZoneSafe', 'FALSE')
		/varset dump ${maEnv.Add[swZoneSafe,FALSE]}
	} else {
		/sqlite ${maData.Find[DBzone].Value} dummy SELECT value from zone_control where zone_shortname='${Zone.ShortName}' and element='swZoneSafe'
		/varset dump ${maEnv.Add[swZoneSafe,${sqlite.Result[dummy 1 value]}]}
	}

  /declare _li listiterator local
	/vardata _li lsZoneProperty.First.Clone
	/while (!${_li.IsEnd}) {
		/varset dump ${${_li.Value}.Clear}
		/sqlite ${maData.Find[DBzone].Value} dummy SELECT count(*) as data from zone_control where zone_shortname='${Zone.ShortName}' and element='${_li.Value}';
		/if (${sqlite.Result[dummy 1 data]}) {
			/sqlite ${maData.Find[DBzone].Value} dummy SELECT group_concat(value) as data FROM zone_control WHERE zone_shortname = '${Zone.ShortName}' and element = '${_li.Value}' group by element
			/varset dump ${${_li.Value}.Append[${sqlite.Result[dummy 1 data]}]}
		}		
	/varset dump ${_li.Advance}
	}
	
/return



|***
 * note: find a variables mapping
 * use: ${get_key_map[DEBUG, variable, validate t/f]} returns the mapname
 ***|	
sub get_key_map(bool _debug, string _key, bool _validate)
	DEBUG\atget_key_map\ax(\a-w${_key}\ax)	

	/declare _liglm listiterator local
	/vardata _liglm lsMaps.First.Clone
	/while (!${_liglm.IsEnd}) {
		DEBUG ${sep}lsMaps${sep}\a-w${_liglm.Value}\ax${sep}contains(\aw${_key}\ax)${If[${${_liglm.Value}.Contains[${_key}]}, ${dot} \agFound\ax,]}
		/if (${${_liglm.Value}.Contains[${_key}]}) {
			DEBUG ${sep}lsMaps${sep}\agReturning\ax${sep}\a-w${_liglm.Value}\ax
			/return ${_liglm.Value}
		}
		/varset dump ${_liglm.Advance}
	}	
	DEBUG ${sep}Map Returning${sep}\arNO MAP FOUND\ax

/return FALSE



|***
 * note: find a variables DB table
 * use: ${get_key_table[DEBUG, variable]} returns the table name
 ***|	
sub get_key_DBLOC(bool _debug, string _key)
	DEBUG\atget_key_DBLOC\ax(\a-w${_key}\ax)	

	/vardata mi maDBTables.First.Clone
	/while (!${mi.IsEnd}) {
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM '${mi.Key}' WHERE ${mi.Value}='${_key}'
		/if (${sqlite.Rows[dummy]}) {
			DEBUG ${sep}DBLOC Returning${sep}\a-w|${mi.Key}|${mi.Value}\ax
			/return |${mi.Key}|${mi.Value}
		}
		/varset dump ${mi.Advance}
	}	
	DEBUG ${sep}DBLOC Returning${sep}\a-wNOT FOUND IN DB\ax
/return FALSE



|***
 * note: user controls
 * use: hey look squirrels...
 * use: /call do_raw_edit DEBUG [silent T/F] [variable to change] [value to change it too|gem#|cursor] [add|delete|clear (for lists)] [not used yet]
 ***|
sub do_raw_edit(bool _debug, string _silent, string _variable, string _value, string _add_delete, string _verbage)
	DEBUG \atdo_raw_edit\ax(\aw${_silent}, ${_variable}, ${Parse[1,${_value}]}, ${_add_delete}, ${_verbage}\ax)

	| ** error control ** |

	| is it really a DB variable?
	/declare _DBLOC string local ${get_key_DBLOC[${_debug}, ${_variable}]}
	/declare _originalValue string local ${Parse[1,${_value}]}
	
	| variable error instructions
	/if ((!${_variable.Length} || ${_DBLOC.Equal[FALSE]}) && ${_value.NotEqual[clear]}) {
		/varset dump ${out[16, "${notice}${sep}\ar/edit\ax${sep}invalid DB variable${sep}\a-w${_variable}\ax"]}
		/return FALSE
	} 
	
	| error for add/delete
	/if (${_variable.Left[2].Equal[ls]} && (!${Select[${_add_delete},add,delete,override]} && ${_value.NotEqual[clear]})) {
		/varset dump ${out[16, "${notice}${sep}\ar/edit\ax${sep}invalid list option${sep}\a-w[add|delete|clear]\ax"]}
		/return FALSE
	}

	| errors for values
	/if (!${_value.Length}) {
		/varset dump ${out[16, "${notice}${sep}\ar/edit\ax${sep}invalid value${sep}[\a-wname of thing|gem#|cursor|PREGEN\ax]"]}
		/return FALSE
	} 
	
	| ** end of error control .. start of actually getting something fucking done. ** |
	
	
	| does it have a ?
	/declare _map string local ${get_key_map[${_debug}, ${_variable}]}
	/declare _list bool local FALSE
	/declare _found bool local FALSE

	| set value as a gem
	/if (${_value.Find[gem]} && ${_value.Length} < 6) {

		/declare _gemN int local ${get_gem_id[FALSE, ${_value}]}

		/if (!${Range.Between[1,${Me.NumGems}:${_gemN}]}) {
			/varset dump ${out[16, "1-${Me.NumGems} only for the gem option"]}
			/varset dump ${out[16. "ex: \aw/edit\ax [${num}variable\ax] [gem${num}#\ax] \a-w[add|delete]\ax"]}
			/return FALSE
		}
		
		| adjust to the loaded spell
		/varset _value ${Me.Gem[${_gemN}].BaseName}	
		
	| set value as cursor
	} else /if (${_value.Equal[cursor]}) {
		/if (!${Cursor.ID}) {
			/varset dump ${out[16, "\a-wcursor\ax requires an object on your cursor"]}
			/return FALSE
		}
		/varset _value ${Cursor.Name}

	| return to default settings
	} else /if (${_value.Equal[pregen]}) {

		DEBUG ${sep}DATA \agdoes exists\ax \arWE ARE RESETTING IT\ax
		/declare _count int local 1
		/declare _tmpList string local
	
		DEBUG GET PREGEN DATA
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM env_${Me.Class.ShortName.Lower} WHERE env_var="${_env_var}"
		DEBUG ${sep}dummy SELECT * From env_${Me.Class.ShortName.Lower} WHERE env_var='${_env_var}' ${dot} \ay${sqlite.Status[dummy]}\ax
		DEBUG ${sep}${sep}${sqlite.Result[dummy 1 env_var]}${sep}\a-w${sqlite.Result[dummy 1 value]}\ax
				
		| how many do we want to find?
		/if (${maPREGEN.Find[${_value}].Value}) {
		/while (${_count} <= ${maPREGEN.Find[${_value}].Value}) { 
			DEBUG ${sep}${sqlite.Result[dummy 1 value].Arg[${_count},|]}
			| is it in the book?
			/if (${Me.Book[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]} || ${Me.CombatAbility[${Spell[${sqlite.Result[dummy 1 value].Arg[${_count},|]}].RankName}]}) {
				/if (${Bool[${_tmpList}]}) /varset _tmpList ${_tmpList}|
				/varset _tmpList ${_tmpList}${sqlite.Result[dummy 1 value].Arg[${_count},|]}
				/varcalc _count ${_count}+1
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
		/varset dump ${_tmpList.Delimiter["|"]}
	
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_variable}'
		/varset dump ${_tmpList.Append[${sqlite.Result[dummy 1 value]}]}
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
		/varset dump ${_tmpList.Delimiter["|"]}
	
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var="${_variable}"
		/varset dump ${_tmpList.Append[${sqlite.Result[dummy 1 value]}]}
		DEBUG ${sep}DB current count${sep}\aw${_tmpList.Count}\ax
		
		/if (!${_tmpList.Contains["${Parse[2,${_value}]}"]}) {
			DEBUG ${sep}String add does not exists${sep}\a-w"${Parse[1,${_value}]}"\ax
			/if (${_tmpList.Contains[EMPTY]} && ${_tmpList.Count} == 1) {
				DEBUG clearing \arEMPTY\ax list
				/varset dump ${_tmpList.Clear}
			}
			/varset dump ${_tmpList.Append["${Parse[1,${_value}]}"]}
			DEBUG ${sep}DB New count${sep}\aw${_tmpList.Count}\ax
		 	/declare _tmpString string local
			/declare _li listiterator local
			/vardata _li _tmpList.First.Clone
			/while (!${_li.IsEnd}) {
				DEBUG ${sep}${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}\a-w${_li.Value}\ax
				/varset _tmpString ${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}${_li.Value}
				/varset dump ${_li.Advance}
			}	
			DEBUG ${sep}_tmpString${sep}\ay${_tmpString}\ax
			/varset _value ${_tmpString}
			/varset _list TRUE

		}

	| delete an existing value
	} else /if (${_add_delete.Equal[delete]}) {
		DEBUG ${sep}\aydelete\ax
		/declare _tmpList list local
		/varset dump ${_tmpList.Delimiter["|"]}
	
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_variable}'
		/varset dump ${_tmpList.Append[${sqlite.Result[dummy 1 value]}]}
		DEBUG ${sep}DB current count${sep}\aw${_tmpList.Count}\ax
		
		/if (${_tmpList.Contains["${Parse[1,${_value}]}"]}) {
			DEBUG ${sep}String add exists${sep}\a-w"${Parse[1,${_value}]}"\ax
			/varset dump ${_tmpList.Remove["${Parse[1,${_value}]}"]}
			/if (${_tmpList.Count} == 0) {
				DEBUG populating \arEMPTY\ax list
				/varset dump ${_tmpList.Append[EMPTY]}
			}
			DEBUG ${sep}DB New count${sep}\aw${_tmpList.Count}\ax
		 	/declare _tmpString string local
			/declare _li listiterator local
			/vardata _li _tmpList.First.Clone
			/while (!${_li.IsEnd}) {
				DEBUG ${sep}${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}\a-w${_li.Value}\ax
				/varset _tmpString ${_tmpString}${If[${_li.Value.Equal[${_tmpList.First}]},,|]}${_li.Value}
				/varset dump ${_li.Advance}
			}	
			DEBUG ${sep}_tmpString${sep}\ay${_tmpString}\ax
			/varset _value ${_tmpString}
			/varset _list TRUE

		} 

	| everything else, if its a real variable, edit it..
	} 

	| update the _value into the DB			
	/if (${Bool[${_map}]} || ${_list} || ${_found}) {
		/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}"
		DEBUG ${sep}dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}" \ay${sqlite.Status[dummy]}\ax
		/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}"
		
		| reload data
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var="${_variable}"
		
		DEBUG valuerow:${sqlite.Result[dummy 1 value]}
	}
	| repopulate the containers
	/if (${Bool[${_map}]}) {
		DEBUG MAP ADD

		DEBUG /varset dump ${${_map}.Add[${_variable},"${sqlite.Result[dummy 1 value]}"]}
		/varset dump ${${_map}.Add[${_variable},"${sqlite.Result[dummy 1 value]}"]}
	} 
	
	/if (${_list}) {
		DEBUG LIST Append
		/varset dump ${${_variable}.Clear}
		/varset dump ${${_variable}.Append["${Parse[1,${_value}]}"]}
	}
	

	| outputs ??? 
	/if (${sqlite.Status[dummy].Equal[Success]} && ${_silent.NotEqual[SILENT]}) {
		/if (${_list}) {
			DEBUG LIST output
			/varset dump ${out[16, "DB${sep}${_variable}\ax${sep}\a-w${Parse[1,${_originalValue}]}\ax ${dot} \a-y${_add_delete}\ax"]}
			/varset dump ${out[16, "List Updated${sep}\a-w${_variable}\ax Count${sep}\a-w${${_variable}.Count}\ax"]}
		} else /if (${Bool[${_map}]}) {
			DEBUG MAP output
			OUT /edit ${_variable}${sep}\a-w${sqlite.Result[dummy 1 value]}\ax
			
		} else /if (${_found}) {
			DEBUG FOUND output
			OUT /edit ${_variable}\ax${sep}${num}${Parse[0,"${_value}"]}\ax
		} else {
			
		}

	} else /if (${sqlite.Status[dummy].NotEqual[Success]}) {
		/varset dump ${out[16, "${warning} \arERROR\ax${sep}\atdo_raw_edit\ax(\aw${_silent}, ${_variable}, ${Parse[1,${_value}]}, ${_add_delete}, ${_verbage}\ax)"]}
	} 
	
/return	TRUE



|***
 * note: sets the spires a character will use
 * use: /call set_control_chr_spire [debug] [group|raid] [NOTUSED]
 ***|
sub set_control_chr_spire(bool _debug, string _type, string _verbage)
	DEBUG \atset_control_chr_spire\ax(\a-w${_type}\ax)
	
  /declare _out string local

	/if (${_type.Equal[group]}) {
		/varset dump ${set_switch_env[${_debug}, swSpireGroup, TRUE, ${_verbage}]}
		
	} else /if (${_type.Equal[raid]}) {
		/varset dump ${set_switch_env[${_debug}, swSpireRaid, TRUE, ${_verbage}]}

	}

	/varset _out ${info}group\ax:${If[${maChr.Find[swSpireGroup].Value},${on},${off}]}
	/varset _out ${_out} ${dot} ${info}raid\ax:${If[${maChr.Find[swSpireRaid].Value},${on},${off}]}
	/varset dump ${out[12, "/chr spire ${_out}"]}

/return



|***
 * note: sets healpoints
 * use: /call set_control_heal_point [debug] [%## heal at]
 ***|
sub set_control_heal_point(bool _debug, int _verbage)
	DEBUG \atset_control_heal_point\ax(\a-w${_verbage}\ax)

	/declare _out string local
	
	| non healers (self only)
	/if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
		/if (${set_control_num_range[${_debug}, stHealPoint${Me.Class.ShortName}, "${_verbage}", 1, 99]}) {
			/varset dump ${out[12, "/healpoint ${dot} ${num}${_verbage}\ax"]}
		}
		/return 
		
	| Healer Classes
	} else /if (${lsClassHeal.Contains[${Me.Class.ShortName}]} && ${Bool[${_verbage}]}) {
		/varset dump ${set_control_num_range[${_debug}, stHealPoint${Target.Class.ShortName}, "${_verbage}", 1, 99]}
	}		

	/declare _lichp listiterator local
	/vardata _lichp lsClassAll.First.Clone
	/while (!${_lichp.IsEnd}) {
		/varset _out ${_out} ${If[${_lichp.Value.Equal[${lsClassAll.First}]},,${dot}]} ${info}${_lichp.Value}\ax:${num}${maHeal.Find[stHealPoint${_lichp.Value}].Value}\ax
		/varset dump ${_lichp.Advance}
	}
	/varset _out ${_out} ${dot}${info}PANIC\ax:${num}${maHeal.Find[stHealPointPanic].Value}\ax
	/varset _out ${_out} ${dot}${info}HOT\ax:${num}${maHeal.Find[stHealPointHoT].Value}\ax

	/varset dump ${out[12, "/heal point ${_out}"]}

		
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
		/varset dump ${out[0, "usage /heal xtclass [\a-wTarget\ax|\a-wClass ShortName\ax]"]}
	}

	/if (${lsClassAll.Contains[${_verbage}]}) {
		| if its there, delete
		/if (${lsHealXTClass.Contains[${_verbage}]}) {
			/call do_raw_edit ${_debug} SILENT lsHealXTClass "${_verbage}" delete

		| if its not there, add	
		}	else /if (!${lsHealXTClass.Contains[${_verbage}]}) {
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
		/varset dump ${_lisccx.Advance}
	}	
	/varset dump ${out[12, "/chr xtclass ${_out}"]}

/return
	
	
	
|***
 * note: make sure you have the correct zone INI info loaded
 * use: /call check_zone_id DEBUG [force T/F]
 ***|
sub check_zone_id(bool _debug, bool _force)
	DEBUG \atcheck_zone_id\ax(\a-w${_force}\ax)
	
	GETINPUT check_zone_id stCurrentZoneID|${maEnv.Find[stCurrentZoneID].Value}
	
	/if (${timer_Check_Zone_ID} && !${_force}) /return FALSE

	/varset dump ${set_data_timer[${_debug}, Check_Zone_ID, RESTART]}
	/declare _zoned bool local FALSE

	| zone matches. nothing to see here. move on
	/if (${maEnv.Find[stCurrentZoneID].Value} == ${Zone.ID} && !${_force}) /return TRUE
		
	| zone does not match. wtf..
	/if (${maEnv.Find[stCurrentZoneID].Value} != ${Zone.ID} || ${_force}) {
		/varset dump ${out[1, "${num}${Zone.Name}\ax${sep}\a-w${Zone.ShortName}\ax"]}
		/call set_zone_lists FALSE
		/varset swIsMeDead FALSE
		/varset _zoned TRUE
		/doevents flush
	
		| check for mesh
		/if (!${Navigation.MeshLoaded}) {
			/varset dump ${out[1, "${warning} ${dot} NO Mesh for \a-w${Zone.ShortName}\ax"]}
		}

		| make it right with the world
		/varset dump ${maEnv.Add[stCurrentZoneID,${Zone.ID}]}
		
	}
	
	/return
	| yah. we zoned and stuff
	/if (${_zoned}) {
		/if (${maBurn.Find[swBurnForce].Value}) /varset dump ${maBurn.Add[swBurnForce,FALSE]}
		/if (HOME) /call Bind_control_home clear FALSE silent FALSE
		| /if (!${swModePersistent}) /call switch_clear_mode
		/if (AUTO && ${maChr.Find[stShrink].Value}) /call check_shrink
	}

/return



|***
 * note: keep exp at a specified level then max aa. set lvl to 0 to disable
 * use: /call check_expaa_adjust DEBUG
 ***|
sub check_expaa_adjust(bool _debug)
	DEBUG \atcheck_expaa_adjust\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT check_expaa_adjust FALSE|FALSE

	/varset dump ${set_data_timer[${_debug}, Check_EXP_Adjust, RESTART]}

	| set EXP cause we deaded too much
	/if (${Me.Level} <= ${maEnv.Find[stLvlMax].Value} && ${Me.PctExp} <= ${maEnv.Find[stLvlPct].Value}) {
		/varset dump ${out[13, "EXP below maintain level${sep}\a-w${maEnv.Find[stLvlMax].Value}\ax${sep}\a-w${maEnv.Find[stLvlPct].Value}%\ax"]}
		/alternateadv off

	| set to AA cause the group healer is awesom	
	} else /if (${Me.Level} == ${maEnv.Find[stLvlMax].Value} && ${Me.PctExp} >= ${maEnv.Find[stLvlPct].Value} && ${Me.AAPoints} < ${Float[${Math.Calc[${Me.Level}*2]}].Int}) {
		/alternateadv on 100

	| set back to exp cause nothing else to buy
	} else /if (${Me.Level} == ${maEnv.Find[stLvlMax].Value} && ${Me.PctExp} >= ${maEnv.Find[stLvlPct].Value} && (${Me.AAPoints} == ${Int[${Math.Calc[${Me.Level}*2]}]})) {
		/alternateadv off
	}
	
/return



|***
 * note: food coma time
 * use: /call last_supper 
 ***|
sub check_last_supper(bool _debug)
	DEBUG \atcheck_last_supper\ax()
	
	/varset dump ${set_data_timer[FALSE, Check_FoodDrink, RESTART]}		

	| for a frame of reference hunger and thirst seem to go to ~10k
	| one bite of black bread from a cleric is 500
	| one drink of a kadim constitution is almost 6k

	| eat, drink and be merry
	/if (${FindItemCount[=${maEnv.Find[stFood].Value}]} && ${Me.Hunger} < 3000) {
		/varset dump ${out[0, "Hungry${sep}\a-w${maEnv.Find[stFood].Value}\ax"]}
		/useitem "${maEnv.Find[stFood].Value}"
	}
	/if (${FindItemCount[=${maEnv.Find[stDrink].Value}]} && ${Me.Thirst} < 3000) {
		/varset dump ${out[0, "Thirsty${sep}\a-w${maEnv.Find[stDrink].Value}\ax"]}
		/useitem "${maEnv.Find[stDrink].Value}"
	}
			
/return



 |***
 * note: will loosly tie to a specific toon. will moove close to the toon when it moves. set to timer [Timers] -> CheckTieTime
 * use: /tie [on/off toggle] requires /tc name [NAME] and /tc variance ##
 **|
sub set_tie(bool _debug)
	DEBUG \atset_tie\ax()

	/if (AUTO) /varset dump ${set_data_timer[FALSE, Check_Tie, RESTART]}
	
	/if (!${Bool[${maTie.Find[stTieToon].Value}]}) /return
	
	/declare _ID int local
	/varset _ID ${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}
	DEBUG _ID${sep}\a-w${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}\ax
	/if (HOME) /call Bind_control_home clear FALSE silent FALSE
	
	ISMEDEAD
	GETINPUT

	| reasons to not tie to a toon
	/if (${maTie.Find[swSetTie].Value} && ${_ID} && ${Spawn[ID ${_ID}].Dead}) /return
	/if (${Me.Combat}) /return
	/if (${Spawn[pc ${maTie.Find[stTieToon].Value}].Distance} <= ${maTie.Find[stTieVariance].Value}) /return
	
	| MQ2Nav mode
	/if (${maTie.Find[stTieMode].Value.Equal[nav]}) {
		DEBUG ${sep}mode${sep}\a-wnav\ax
		/if (${Navigation.MeshLoaded}) {
			/if (!${Navigation.Active} && ${Spawn[ID ${_ID}].Distance} > ${maTie.Find[stTieVariance].Value}) {
				DEBUG get closer
				/if (${Target.ID}) /squelch /target clear
				/squelch /face ID ${_ID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
				/nav ID ${_ID} |dist=${maTie.Find[stTieVariance].Value} log=off

			} else /if (${Spawn[ID ${_ID}].Distance} <= ${maTie.Find[stTieVariance].Value}) {
				/if (${Navigation.Active}) {
					/nav stop log=off
				}
			}

		} else /if (!${Navigation.MeshLoaded}) {
			/varset dump ${out[0, "NO Nav Mesh for this zone. Returning to Stick."]}
			/call Bind_control_tc mode stick
		}
		
	| MQ2MoveUtils mode
	} else /if (${maTie.Find[stTieMode].Value.Equal[stick]}) {
		DEBUG ${sep}mode${sep}\a-wstick\ax
		| all present and or accounted for SIR!
		/if (${Spawn[pc ${_ID}].Distance} > ${maTie.Find[stTieVariance].Value}) {
			/if (${Target.ID}) /squelch /target clear
			/squelch /face ID ${_ID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
			/squelch /stick ID ${_ID} ${maTie.Find[stTieVariance].Value} UW
		}

		:okweresticking
		ISMEDEAD
		GETINPUT
		/if (${Spawn[pc ${_ID}].Distance} <= ${maTie.Find[stTieVariance].Value}) {
			/squelch /stick off
			/return
		}
		
		/if (!${Stick.Stopped} && ${_ID}) {
			/goto :okweresticking
		} else {
			/if (${Stick.Active}) /squelch /stick off
			/if (${Me.Moving}) {
				/keypress forward
				/keypress back
			}
		}
	
	}

/return



|***
 * note: cast classes AA fade
 * use: /call cast_AA_fade
 ***|
sub cast_AA_fade(bool _debug)
	DEBUG \a-tcast_AA_fade\as()

	| does the class even have a fade?
	/if (!${maChr.Find[stPctFade].Value}) /return FALSE
	ISMEDEAD
		
	| mobs in safe range.?
	/if (!${SpawnCount[npc radius SAFEENV]}) /return FALSE
	

	| made it this far. may as well see if the shit works
	/if (${is_ready[${_debug}, "${maData.Find[stFade].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stFade].Value}" 0 FALSE
	}

/return TRUE



|***
 * note: checks and casts whats int lsBuffNow
 * use: /call check_buff_cycle_now DEBUG
 ***|
sub check_buff_cycle_now(bool _debug)
	DEBUG \atcheck_buff_cycle_now\ax()

	/declare _count int local 0

	/varset dump ${set_data_timer[${_debug}, Check_Buff_Now, RESTART]}
		| check now buffs
		/for _count 1 to 50
			/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
			/if (!${lsBuff${_count}Tag.Contains[alwaysup]}) /continue
			/if (!${check_basic_tags[${_debug}, Buff${_count}, ${Me.DisplayName}]}) /continue
	
			/call voodoo ${_debug} "${maBuffs.Find[stBuff${_count}].Value}" 0 FALSE
	
			/if (${lsBuff${_count}Tag.Contains[autoinventory]}) {
				/call check_cursor ${_debug} TRUE CLEAR
			}
		
		/next _count

/return



|***
 * note: returns a rank if there is one
 * use: ${set_rank[DEBUG, NAME]}
 ***|
sub set_rank(bool _debug, string _spell)
	DEBUG \atset_rank\ac(\a-w${_spell}\ax)
	
	/if (${_spell.Find[+c]}) {
		/varset _spell ${_spell.Arg[1,+c]}
	}

	/if (${Me.Book[${Spell[${_spell}].RankName}]}) {
		/return ${Spell[${_spell}].RankName}
	}
/return ${_spell}



|***
 * note: casts an in order value from a list at passed target ID
 * use: /call cast_list DEBUG [lsNAME] [TARGETID] 
 ***|
sub cast_list(bool _debug, string _lsList, int _tmpID)
	DEBUG \atcast_list\ax(\a-w${_lsList}\ax)

	/declare _liCL listiterator local
	/vardata _liCL ${_lsList}.First.Clone
	/while (!${_liCL.IsEnd}) {
		/if (!AUTO) /return
		
		DEBUG ${sep}_liCL.Value${sep}\a-w${_liCL.Value}\ax
		/if (${is_ready[${_debug}, "${_liCL.Value}"]}) {
			/call voodoo ${_debug} "${_liCL.Value}" ${_tmpID} FALSE
			/return
		}
		/varset dump ${_liCL.Advance}
	}	

/return



|***
 * note: check characters unity
 * use: /call check_buff_unity
 ***|
sub check_buff_cycle_unity(bool _debug)
	DEBUG \atcheck_buff_cycle_unity\ax()

	/varset dump ${set_data_timer[${_debug}, Check_Buff_Unity, RESTART]}
	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE

	/if (!${is_ready[${_debug}, "${maChr.Find[stAAUnity].Value}"]}) /return FALSE
	
	/declare _count int local 0
	/declare _found bool local TRUE
	/declare _spell string local ${maChr.Find[stAAUnity].Value}
	DEBUG ${sep}_spell${sep}\a-w${_spell}\ax 
	DEBUG ${sep}effects${sep}\a-w${Spell[${_spell}].NumEffects}\ax

	/for _count 1 to ${Spell[${_spell}].NumEffects}
		DEBUG ${sep}_count${sep}\aw${_count}\ax${sep}\a-w${Spell[${_spell}].Trigger[${_count}]}\ax
		
		| duration
		
		| trigger
		/if (${Me.Buff[${Spell[${_spell}].Trigger[${_count}].BaseName}].ID}) /continue
		
		| stacks
		/if (!${Spell[${_spell}].Trigger[${_count}].Stacks}) /continue
		
		| do not have
		/if (!${Me.Buff[${Spell[${_spell}].Trigger[${_count}]}].ID}) /varset _found FALSE
		DEBUG ${sep}_found${sep}\a-w${_found}\ax
		
		/if (!${_found}) /break
	/next _count

	/if (${_found}) /return

	/call voodoo ${_debug} "${_spell}" 0 FALSE

/return TRUE



|***
 * note: Check for running auras, If they are duped, remove the second one
 * use: /call check_aura
 ***|
sub check_buff_cycle_aura(bool _debug)
	DEBUG \atcheck_buff_cycle_aura\ax()
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/varset dump ${set_data_timer[${_debug}, Check_Buff_Aura, RESTART]}
	
	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE
	
	/declare _a1 		string local ${set_rank[${_debug}, ${maBuff.Find[stAura1].Value.Arg[1,+]}]}
	/declare _a1c 	string local ${maBuff.Find[stAura1Check].Value}
	/declare _a2 		string local ${set_rank[${_debug}, ${maBuff.Find[stAura2].Value.Arg[1,+]}]}
	/declare _a2c 	string local ${maBuff.Find[stAura2Check].Value}
	
	| Aura: berzerker, monk, warrior
	/if (${check_class[FALSE, |BER|MNK|WAR]}) {
		/if (${Bool[${_a1c}]} && !${Bool[${Me.Aura[1].Name.Equal[${_a1c}]}]}) {
			/if (!${check_basic_tags[FALSE, Aura1, ${Me.DisplayName}]}) /return FALSE
			/call voodoo FALSE "${_a1}" 0 FALSE
		}
		/return
	}
		
	| Aura: Bard
	/if (${check_class[FALSE, |BRD]}) {
		/if (${Bool[${_a1c}]} && !${Bool[${Me.Aura[1].Name.Equal[${_a1c}]}]}) {
			/if (!${check_basic_tags[FALSE, Aura1, ${Me.DisplayName}]}) /return FALSE
			/if (${is_ready[${_debug}, "${_a1}"]}) {
				/call voodoo ${_debug} "${_a1}" 0 FALSE
			}
		}
		/return
	}

	| everyone else...
	/declare _count int local 0
	/declare _aura int local 0
	/declare _found1 bool FALSE
	/declare _found2 bool FALSE

	/for _aura 1 to 2
		DEBUG Aura ${_aura}${sep}\a-w${_a1}\ax
		DEBUG Aura ${_aura} check${sep}\a-w${_a1c}\ax
		/if (${Bool[${_a${_aura}}]}) {
			/for _count 1 to 2
				/if (${Bool[${_a${_aura}c}]}) {
					/if (!${check_basic_tags[FALSE, Aura${_count}, ${Me.DisplayName}]}) /varset _found${_aura} TRUE
					DEBUG /if Me.Aura[${_count}].Name.Equal[${_a${_aura}c}]
					/if (${Me.Aura[${_count}].Name.Equal[${_a${_aura}c}]}) {
						/varset _found${_aura} TRUE
					
					DEBUG /if Me.Aura[${_count}].Name.Equal[${_a${_aura}}] 
					} else /if (${Me.Aura[${_count}].Name.Equal[${_a${_aura}}]}) {
						/varset _found${_aura} TRUE
					}

				}
				/if (${_found${_aura}}) /break
			/next _count
			
			DEBUG _found${_aura}${sep}\a-w${_found${_aura}}\ax
			
			/if (!${_found${_aura}}) {
				
				/call voodoo ${_debug} "${_a${_aura}}" ${Me.ID} FALSE
			} else /if (${_found${_aura}}) {
				/varset _found${_aura} FALSE
			}
		
		}
	/next _aura
		
/return



|***
 * note: do all the damn buffs
 * use: /call check_buff_cycle DEBUG <FORCE>
 ***|
sub check_buff_cycle(bool _debug, bool _force)
	DEBUG \atcheck_buff_cycle\ax(\a-w${_force}\ax)

	/if (${_force}) {
		/declare _buffcycletimestart int local ${MacroQuest.Running}
		/if (!${maBuff.Find[swBuffMaster].Value}) {
			/varset dump ${out[0, "Master Buff Switch${sep}${off}"]}
		} else {
			/varset dump ${out[0, "Buff Cycle${sep}\a-wForced\ax"]}
		}
	}

	| unity
	/if (${maBuff.Find[swBuffUnity].Value} && ((AUTO && !${timer_Check_Buff_Unity}) || ${_force})) /call check_buff_cycle_unity ${maDebug.Find[buffunity].Value} 

	| aura
	/if (${maBuff.Find[swBuffAura].Value} && ((AUTO && !${timer_Check_Buff_Aura}) || ${_force})) /call check_buff_cycle_aura ${maDebug.Find[buffaura].Value} 
	
	| shrink
	/if (${maBuff.Find[swBuffShrink].Value} && ((AUTO && !${timer_Check_Buff_Shrink}) || ${_force})) /call check_buff_cycle_shrink ${maDebug.Find[buffshrink].Value} 
	
	| self
	/if (${maBuff.Find[swBuffSelf].Value} && ((AUTO && !${timer_Check_Buff_Self}) || ${_force})) /call check_buff_cycle_self ${maDebug.Find[buffself].Value}

	| crew
	/if (${maBuff.Find[swBuffCrew].Value} && ((AUTO && !${timer_Check_Buff_Crew}) || ${_force})) /call check_buff_cycle_crew ${maDebug.Find[buffcrew].Value}
	
	| others
	
	| beg
	/if (${maBuff.Find[swBuffBeg].Value} && ((AUTO && !${timer_Check_Buff_Beg}) || ${_force})) /call check_buff_cycle_beg ${maDebug.Find[buffbeg].Value}
	
	| powersource
	/if (${maBuff.Find[swBuffPowersource].Value} && ((AUTO && !${timer_Check_Buff_Powersource}) || ${_force})) /call check_buff_cycle_powersource ${_debug}
	
	| ranged ammo
	/if (${maBuff.Find[swBuffAmmo].Value} && ((AUTO && !${timer_Check_Ammo}) && (!${Me.Inventory[ammo].ID} || ${_force}))) /call check_buff_cycle_ammo ${maDebug.Find[buffammo].Value}	

	| summon poison
	/if ((AUTO && !${timer_Check_Poison}) && (${maCC.Find[swPoison].Value} || ${_force})) /call check_buff_cycle_poison ${maDebug.Find[buffpoison].Value}
	
	| class specific buffs. buff self required
	/if (${maBuff.Find[swBuffSelf].Value} && (!${timer_Check_Buff_Self} || ${_force})) {
		/if (${Select[${Me.Class.ShortName},BER]}) {
		} else /if (${Select[${Me.Class.ShortName},BRD]}) {
		} else /if (${Select[${Me.Class.ShortName},BST]}) {
		} else /if (${Select[${Me.Class.ShortName},CLR]}) {
		} else /if (${Select[${Me.Class.ShortName},DRU]}) {
		} else /if (${Select[${Me.Class.ShortName},ENC]}) {
			/if (${maChr.Find[swAACrystals].Value}) /call check_crystals ${maDebug.Find[buffclass].Value}
		} else /if (${Select[${Me.Class.ShortName},MAG]}) {
		} else /if (${Select[${Me.Class.ShortName},MNK]}) {
		} else /if (${Select[${Me.Class.ShortName},NEC]}) {
		} else /if (${Select[${Me.Class.ShortName},PAL]}) {
		} else /if (${Select[${Me.Class.ShortName},RNG]}) {
		} else /if (${Select[${Me.Class.ShortName},ROG]}) {
		} else /if (${Select[${Me.Class.ShortName},SHD]}) {
		} else /if (${Select[${Me.Class.ShortName},SHM]}) {
		} else /if (${Select[${Me.Class.ShortName},WAR]}) {
		} else /if (${Select[${Me.Class.ShortName},WIZ]}) {
		}
	}

	/if (AUTO) /varset dump ${set_data_timer[${_debug}, Check_Buff_Cycle, RESTART]}

	/if (${_force}) {
		/if (${maBuff.Find[swBuffMaster].Value}) {
			/varset dump ${out[0, "Buff Cycle${sep}\a-wCompleted\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_buffcycletimestart}) / 10) / 60]}s\ax"]}
		}
	}

/return



|***
 * note: power sources
 * use: /call check_buff_cycle_powersource DEBUG 
 ***|
sub check_buff_cycle_powersource(bool _debug)
	DEBUG \atcheck_buff_cycle_powersource\ax()

	/varset dump ${set_data_timer[${_debug}, Check_Buff_Powersource, RESTART]}

	| /if (${Me.Inventory[powersource].Power}) /return FALSE

	/declare _psName string local FALSE

	/declare _count int local 0
	/for _count 1 to 50
		/if (!${lsBuff${_count}Tag.Contains[powersource]}) {
			/continue
		} else /if (!${lsBuff${_count}Tag.Contains[powersource]}) {
			DEBUG buff${_count}${sep}\a-w${maBuffs.Find[stBuff${_count}].Value}\ax
			/varset _psName ${maBuffs.Find[stBuff${_count}].Value}
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
			/call voodoo FALSE "${_psName}" 0 FALSE
			/delay 5
			/autoinventory
		} else /if (!${FindItem[${_psName}].ID}) {
			/return FALSE
		}

	}	else {
		/return FALSE
	}

/return







${Me.Inventory[powersource].PctPower.Equal[NULL]} 
${Me.Inventory[powersource].Power} 
${Me.Inventory[powersource].ID} 

InvSlot21



${Me.Inventory[powersource].Name}

${FindItem[Sealed Translucent Energeian Light Orb].Effect}


Sealed Translucent Energeian Light Orb

Translucent Energeian Light Orb

Scorpikis Worry Stone



${Spell[${FindItem[Sealed Translucent Energeian Light Orb].Clicky.SpellID}].Name}
Translucent
${Spell[${FindItem[Sealed  Energeian Light Orb].Clicky.SpellID}].ID}

${FindItem[Sealed Translucent Energeian Light Orb].Clicky.Name}

			/if (${Me.Buff[${Spell[${FindItem[=${_verbage}].Clicky.SpellID}].Name}].ID}) /return FALSE





${FindItemCount[${Spell[${FindItem[=${maBuffs.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}







|***
 * note: clicks stuff
 * use: /call check_item_cycle DEBUG [TARGETID]
 ***|
sub check_item_cycle(bool _debug, int _tmpID)
	DEBUG \atcheck_item_cycle\ax(\a-w${_tmpID}\ax)
	
	/declare _count int local 0
	/declare _spell string local
	/for _count 1 to 20
		/if (!COMBAT) {
			/return
		}
		/if (${maItems.Find[stItem${_count}].Value.Equal[FALSE]}) /continue
		/if (!${is_ready[${_debug}, "${maItems.Find[stItem${_count}].Value}"]}) /continue
		
		/if (!${check_basic_tags[${_debug}, Item${_count}, FALSE]}) /continue

		/varset _spell ${FindItem[${maItems.Find[stItem${_count}].Value.Arg[1,+]}].Spell}
		/if (${lsItem${_count}Tag.Contains[self]}) {
			/if (!${Me.BuffDuration[${_spell}]}) /varset _tmpID ${Me.ID}
		}

		/if (${FindItem[${_spell}].Spell.Duration.TotalSeconds} && ${FindItem[${_spell}].Spell.Range}) {
			/varset dump ${set_target[${_tmpID}]}
			/if (${Target.BuffDuration[${_spell}].TotalSeconds}) /continue			
		}

		/call voodoo ${_debug} "${maItems.Find[stItem${_count}].Value}" ${_tmpID} FALSE

	/next _count
/return



|***
 * note: Mana recovery items; rods, shards, crack pipes
 * use: /call do_mana_recovery DEBUG
 ***|
sub do_mana_recovery(bool _debug)
	DEBUG \atdo_mana_recovery\ax()
	
	/if (${Me.PctMana} > ${maChr.Find[stPctManaRecovery].Value}*.PCTHO) /return FALSE
	/if (!${lsClassCast.Contains[${Me.Class.ShortName}]}) /return FALSE
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
		
	| you can take that rod.. and shove it right up you...
  /vardata mi maManaRecovery.First.Clone
  /while (!${mi.IsEnd}) {
		/if (${is_ready[FALSE, "${mi.Key}"]} && ${Me.PctHPs} < ${mi.Value}) {
	  	DEBUG ${mi.Key}${sep}\a-w${mi.Value}\ax

			/call voodoo FALSE "${mi.Key}" 0 FALSE

			/if (!${Select[${Me.Class.ShortName},MAG]} && !${FindItemCount[=${mi.Key}]}) {
				| /call tell_resident_mage shard
				| ${get_resident_class[FALSE, MAG, 100]}
			}			
			/break
		}
		/varset dump ${mi.Advance}
  }

/return



|***
 * note: Cleric/Paladin AA Yaulp
 * use: /call cast_yaulp
 ***|
sub cast_yaulp(bool _debug)
	DEBUG \atcast_yaulp\ax()
	
	/varset dump ${set_data_timer[FALSE, Check_Yaulp, RESTART]}
	
	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE

	/if (${Me.PctMana} > 98*.PCTHO) /return FALSE
	/if (${Me.Sitting} || ${Me.Mount.ID}) /return FALSE
	/if (${Me.Buff[Lesser Yaulp].ID} || ${Me.Buff[Yaulp].ID}) /return FALSE
	/if (!${is_ready[${_debug}, "${maChr.Find[stYaulp].Value}"]}) /return FALSE
	/if (!${Select[${Me.CombatState},combat]}) /return FALSE
	ISMEDEAD 
	CHECKEXIT
	/call voodoo ${_debug} "${maChr.Find[stYaulp].Value}" 0 FALSE

/return



|***
 * note: sets your target.. nothing more.. nothing less..
 * use: ${set_target[ID]} | /call set_target [ID]
 ***|
sub set_target(int _tmpID)

	/if (!${Bool[${_tmpID}]}) /return TRUE

	/if (${Spawn[npc corpse ${_tmpID}].ID}) {
		/return FALSE
	}

	/if (${Target.ID} == ${_tmpID}) /return TRUE

	/squelch /target id ${_tmpID}
	/delay 1s ${Target.ID} == ${_tmpID}
	
	/if (${Target.ID} != ${_tmpID}) /return FALSE

/return TRUE



|***
 * note: used to refresh ammo for ranged attacks
 * use: /call check_buff_cycle_ammo DEBUG
 ***|
sub check_buff_cycle_ammo(bool _debug)
	DEBUG \atcheck_buff_cycle_ammo\ax()

	/varset dump ${set_data_timer[${_debug}, Check_Ammo, RESTART]}

	/declare _count int local 0
	/for _count 1 to 50
		/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
		/if (!${Bool[${maBuffs.Find[stBuff${_count}].Value}]}) /continue		
		/if (!${lsBuff${_count}Tag.Contains[summonrange]}) /continue
		
			DEBUG ${sep}_count${sep}\a-wBuff${_count}${sep}${maBuffs.Find[stBuff${_count}].Value}
		
		/if (${Select[${Me.Class.ShortName},BER]}) {
			/if (${FindItemCount[=${Spell[${maBuffs.Find[stBuff${_count}].Value}].Base[1]}]} && !${Me.Inventory[ammo].ID}) {
				/call Bind_command_swap "${FindItem[=${Spell[${maBuffs.Find[stBuff${_count}].Value}].Base[1]}].Name}" 22 ${_debug}
				/delay 5
				/return TRUE
			}				
		} else {
			/if (${FindItemCount[${Spell[${FindItem[=${maBuffs.Find[stBuff${_count}].Value}].Spell}].Base[1]}]} && !${Me.Inventory[ammo].ID}) {
				/call Bind_command_swap "${FindItem[=${Spell[${FindItem[=${maBuffs.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}" 22 ${_debug}
				/delay 5
				/return TRUE
			}		
		}
	
		/if (${is_ready[${_debug}, "${maBuffs.Find[stBuff${_count}].Value}"]}) {
			/call voodoo ${_debug} "${maBuffs.Find[stBuff${_count}].Value}" 0 FALSE
		}
		/delay 10s !${Me.Casting.ID}

		/call check_cursor FALSE TRUE CLEAR

	/next _count	

/return TRUE



| zerker
${maBuffs.Find[stBuff2].Value}
| count
${FindItemCount[=${Spell[${maBuffs.Find[stBuff${_count}].Value}].Base[1]}]}
| name
${FindItem[=${Spell[${maBuffs.Find[stBuff${_count}].Value}].Base[1]}].Name}




${FindItemCount[${Spell[${FindItem[=${maBuffs.Find[stBuff2].Value}].Spell}].Base[1]}]}
${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff2].Value}].Spell}].Base[1]}].Name}
${FindItem[${Spell[${FindItem[${maBuffs.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}

43
/itemnotify "${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff30].Value}].Spell}].Base[1]}].Name}" leftmouseup

${FindItem[${Spell[${FindItem[${maBuffs.Find[stBuff30].Value}].Spell}].Base[1]}].Name}

${FindItemCount[${Spell[${FindItem[=${maBuffs.Find[stBuff43].Value}].Spell}].Base[1]}]}

${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff43].Value}].Spell}].Base[1]}].Name}

${Spell[${FindItem[=${maBuffs.Find[stBuff30].Value}].Spell}].Base[1]}

/nomodkey /shiftkey /itemnotify "${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff43].Value}].Spell}].Base[1]}].Name}" 22 FALSE

/swap "${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff43].Value}].Spell}].Base[1]}].Name}" 22 FALSE


${FindItemCount[${Spell[${maBuffs.Find[stBuff30].Value}].Base[1]}]}

51573
${FindItemCount[79498]}

79498

${Me.AltAbilityReady[${maBuffs.Find[stBuff30].Value}]}
${Spell[${maBuffs.Find[stBuff30].Value}].NumEffects}







|***
 * note: poison clickie things
 * use: /call check_buff_cycle_poison DEBUG
 ***|
sub check_buff_cycle_poison(bool _debug)

	/varset dump ${set_data_timer[FALSE, Check_Poison, RESTART]}

	/declare _count int local 0
	/for _count 1 to 50
		/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
		/if (!${Bool[${maBuffs.Find[stBuff${_count}].Value}]}) /continue		
		/if (!${lsBuff${_count}Tag.Contains[summonpoison]}) /continue

		/if (${is_ready[FALSE, "${maBuffs.Find[stBuff${_count}].Value}"]}) {
			/call voodoo FALSE "${maBuffs.Find[stBuff${_count}].Value}" 0 FALSE
		}
		/delay 10s !${Me.Casting.ID}

		/if (${Cursor.Name.Equal[${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}]}) {
			/autoinventory
		}

	/next _count	

/return TRUE



${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].NumEffects}



${maBuffs.Find[stBuff5].Value}
${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}

${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].Trigger}

${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].Base[1]}].Name}

${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].Base[1]}].Name}
${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].Trigger}
${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].Base[1]}
${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].Trigger}

${Spell[${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Spell}].Base[2]}].Name}

${Me.Buff[${Spell[${FindItem[=${_verbage}].Clicky.SpellID}].Name}].ID}

${FindItem[${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Clicky.SpellID}].Trigger}].Name}

${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Clicky.SpellID}].Trigger}

| spell ID
${FindItem[=${maBuffs.Find[stBuff5].Value}].Clicky.SpellID}

| spell name
${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Clicky.SpellID}].Trigger}

| summoned Item ID
${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Clicky.SpellID}].Base[1]}


${FindItemCount[${Spell[${FindItem[=${maBuffs.Find[stBuff5].Value}].Clicky.SpellID}].Base[1]}]}

-------------------------------------------------




























|***
 * note: check self buffs
 * use: /call check_buff_cycle_self DEBUG
 ***|
sub check_buff_cycle_self(bool _debug)
	DEBUG \atcheck_buff_cycle_self\ax()
	
	/if (AUTO) /varset dump ${set_data_timer[FALSE, Check_Buff_Self, RESTART]}

	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE

	| /if (${watch_for_death[]}) /return 
	
	SETHUD "buff self"

	/declare _count int local 0
	/declare _countD int local 0
	/declare _i int local 0
	/declare _found bool local
	/declare _spell string local FALSE


	/for _count 1 to 50
		/if (!${Me.FreeBuffSlots}) /return
		/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
		/if (!${Bool[${maBuffs.Find[stBuff${_count}].Value}]}) /continue
		/if (!${lsBuff${_count}Tag.Contains[self]}) /continue
		/if (!${check_basic_tags[FALSE, Buff${_count}, ${Me.DisplayName}]}) /continue

		/varset _spell ${set_rank[FALSE, "${maBuffs.Find[stBuff${_count}].Value.Arg[1,+]}"]}
		/varset _found TRUE


		| stacking / exist ?
		/if (${Spell[${_spell}].HasSPA[374]} || ${Spell[${_spell}].HasSPA[340]} || ${Spell[${_spell}].HasSPA[470]}) {
			DEBUG ${sep}\aw470\ax${sep}\a-w${Spell[${_spell}].HasSPA[470]}\ax 
			DEBUG ${sep}\aw374\ax${sep}\a-w${Spell[${_spell}].HasSPA[374]}\ax 
			DEBUG ${sep}\aw340\ax${sep}\a-w${Spell[${_spell}].HasSPA[340]}\ax
			
			/for _i 1 to ${Spell[${_spell}].NumEffects}
				DEBUG ${sep}\a-t${_i}/${Spell[${_spell}].NumEffects}\ax \aw::\ax ${Spell[${Spell[${_spell}].Base2[${_i}]}].Name} \aw::\ax ${Me.Buff[${Spell[${Spell[${_spell}].Base2[${_i}]}].Name}].ID} \aw::\ax D(\a-w${Spell[${Spell[${_spell}].Base2[${_i}]}].Duration}\ax)

				| SPA has no duration, skip it
				/if (!${Spell[${Spell[${_spell}].Base2[${_i}]}].Duration}) /continue
				
				| Does the SPA stack? if not, move on
				/if (!${Spell[${Spell[${_spell}].Base2[${_i}]}].Stacks}) /continue
				
				| do we have the SPA 
				/if (!${Me.Buff[${Spell[${Spell[${_spell}].Base2[${_i}]}].Name}].ID}) /varset _found FALSE

			/next _i


		} else /if (${FindItem[${_spell}].ID}) {
			DEBUG ${sep}Found Item ID${sep}\a-w${_spell}\ax${sep}${Spell[${FindItem[=${_spell}].Clicky.SpellID}].Name}
			/if (!${Me.Buff[${Spell[${FindItem[=${_spell}].Clicky.SpellID}].Name}].ID}) /varset _found FALSE 
	
		} else {
			DEBUG ${sep}\a-t${_count}\ax${sep}${_spell}${sep}${Me.Buff[${_spell}].ID}

			| stacks?
			/if (!${Spell[${_spell}].NewStacks}) /continue

			| do we have the buff?
			/if (!${Me.Buff[${_spell}].ID}) /varset _found FALSE
			
		}
		
		/if (${_found}) /continue
		/call voodoo FALSE "${maBuffs.Find[stBuff${_count}].Value}" ${Me.ID} FALSE
		
		/if (${lsBuff${_count}Tag.Contains[autoinventory]}) /autoinventory		
	
		| drop illusions/mounts
		/if (${lsBuff${_count}Tag.Contains[drop]}) {
			DEBUG ${sep}tag-\a-ydrop\ax
			/delay 5s !${Me.Casting.ID}
			/for _countD 1 to ${FindItem[${_spell}].Spell.NumEffects}
				/if (${FindItem[${_spell}].Spell.Trigger[${_countD}].Name.Find[Illusion:]} || ${Me.Buff[${FindItem[${_spell}].Spell}].ID}) {
					| mounts
					/if (${Me.Buff[${FindItem[${_spell}].Spell}].ID}) {
						/varset dump ${out[0, "Removing${sep}\a-w${FindItem[${_spell}].Spell}\ax"]}
						/removebuff ${FindItem[${_spell}].Spell}
						/delay 2
					}
					| illusions
					/if (${Me.Buff[${FindItem[${_spell}].Spell.Trigger[${_countD}]}].ID}) {
						/varset dump ${out[0, "Removing${sep}\a-w${FindItem[${_spell}].Spell}\ax"]}
						/removebuff ${FindItem[${_spell}].Spell.Trigger[${_countD}]}
						/delay 2
					}
				}	
			/next _countD
		}
		
	/next _count	
	
	SETHUD FALSE

/return



|***
 * note: buffs all your toons
 * use: automatic any peers
 ***|

/sub check_buff_cycle_crew(bool _debug)
	DEBUG \atcheck_buff_cycle_crew\ax()
	
	| CHECKCOMBATCAST

	/if (AUTO) /varset dump ${set_data_timer[FALSE, Check_Buff_Crew, RESTART]}
	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE

	SETHUD "buff crew"	

	/declare _spell string local FALSE
	/declare _spellTrigger string local FALSE
	/declare _count int local 0
	/declare _countToon int local 1
	/declare _target bool local FALSE
	/declare _toonName string local FALSE
	/declare _found bool TRUE
	/declare _i int local 0
	
	| each toon
	/while (${_countToon} <= ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}) {
		
		| get the name to something usable
		/varset _toonName ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_countToon},|]}
		DEBUG ${sep}Current Character${sep}\ay${_toonName}\ax
	
		| skip this person?
		/if (${_toonName.Equal[${Me.DisplayName}]} || !${Spawn[pc ${_toonName}].ID} || ${Spawn[pc ${_toonName}].Dead}) {
			/varcalc _countToon ${_countToon}+1
			/continue
		}
	
		| any buff slots available?
		/dquery ${_toonName} -q "Me.FreeBuffSlots" -o DNQReturn -t 5
		/if (${DNQReturn.Equal[0]}) {
			/varcalc _countToon ${_countToon}+1
			/continue
		}

		| each buff
		/for _count 1 to 50
			| set the spell
			/varset _spell ${set_rank[FALSE, "${maBuffs.Find[stBuff${_count}].Value.Arg[1,+]}"]}
			DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax
			
			| buff turned off
			/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
			
			| skip self only
			/if (${lsBuff${_count}Tag.Count} == 1 && (${lsBuff${_count}Tag.Contains[self]} || ${Spell[${_spell}].TargetType.Equal[Self]} || ${lsBuff${_count}Tag.Contains[pet]})) /continue

			| distance check
			/if (${Spawn[${_toonName}].Distance} > ${Spell[${_spell}].MyRange}) /continue
			
			/varset _target FALSE
	
			| class		
			/if (${lsBuff${_count}Tag.Contains[${Spawn[${_toonName}].Class.ShortName}]}) /varset _target TRUE

			| one of your crew
			| /if (${lsBuff${_count}Tag.Contains[crew]} && ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_toonName}]} && ${Spawn[${_toonName}].ID}) /varset _target TRUE

			| main assist
			/if (${lsBuff${_count}Tag.Contains[ma]} && (${Group.MainAssist.Name.Equal[${_toonName}]} || ${maCC.Find[stManualAssist1].Value.Equal[${_toonName}]} || ${maCC.Find[stManualAssist2].Value.Equal[${_toonName}]})) /varset _target TRUE

			| group only
			/if (${lsBuff${_count}Tag.Contains[group]} && ${Group.Member[[${_toonName}].ID} && ${Group.Member[[${_toonName}].Present}) /varset _target TRUE

			/if (${Spell[${_spell}].HasSPA[470]} || ${Spell[${_spell}].HasSPA[374]} || ${Spell[${_spell}].HasSPA[340]}) {
	
				/for _i 1 to ${Spell[${_spell}].NumEffects}
					/varset _found TRUE

					/varset _spellTrigger ${Spell[${_spell}].Trigger[${_i}]}
					DEBUG ${sep}Trigger${sep}\a-w${_spellTrigger}\ax

					| does it stack
					/dquery ${_toonName} -q "Spell[${_spellTrigger}].Stacks" -o DNQReturn -t 5
					DEBUG ${sep}${sep}STACKS: .Trigger.Stacks \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					/if (!${Bool[${DNQReturn}]}) /continue

					| is it buff blocked 
					/dquery ${_toonName} -q "lsBlockedBuffsMe.Find[${Spell[${_spellTrigger}].ID}]" -o DNQReturn -t 5
					DEBUG ${sep}${sep}BLOCKED .Trigger.Blocked Buff \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					/if (${Bool[${DNQReturn}]}) /continue

					| do we have the triggered buff?
					/dquery ${_toonName} -q "Me.Buff[${_spellTrigger}].ID" -o DNQReturn -t 5
					DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					/if (!${Bool[${DNQReturn}]}) {
						/varset _found FALSE
						/break
					} else /if (${Bool[${DNQReturn}]}) {
						/continue
					}
			
				/next _i

				/if (!${_found} && ${_target}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maBuffs.Find[stBuff${_count}].Value}" ${Spawn[${_toonName}].ID} FALSE		
					/continue
				}

			}	else {
				
				/varset _spellTrigger ${Spell[${_spell}].Trigger}
				DEBUG ${sep}Trigger${sep}\a-w${_spellTrigger}\ax
	
				| does it stack
				/dquery ${_toonName} -q "Spell[${_spellTrigger}].Stacks" -o DNQReturn -t 5
				DEBUG ${sep}${sep}STACKS: .Trigger.Stacks \at${DNQReturn}\ax ${dot} ${_spellTrigger}
				/if (!${Bool[${DNQReturn}]}) /continue


				| is it buff blocked 
				/dquery ${_toonName} -q "lsBlockedBuffsMe.Find[${Spell[${_spellTrigger}].ID}]" -o DNQReturn -t 5
				DEBUG ${sep}${sep}BLOCKED .Trigger.Blocked Buff \at${DNQReturn}\ax ${dot} ${_spellTrigger}
				/if (${Bool[${DNQReturn}]}) /continue


				| do we have the triggered buff?
				/dquery ${_toonName} -q "Me.Buff[${_spellTrigger}].ID" -o DNQReturn -t 5
				DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}
				/if (${Bool[${DNQReturn}]}) {
					/continue
				}	
	
				/if (${_target}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maBuffs.Find[stBuff${_count}].Value}" ${Spawn[${_toonName}].ID} FALSE		
					/continue
				}	
					
			}

		/next _count	
		/varcalc _countToon ${_countToon}+1
		
	}
	
	SETHUD FALSE
		
/return



|***
 * note: watches buff beg map for aliases it can cast
 * use: /call check_buff_cycle_beg DEBUG
 ***|
sub check_buff_cycle_beg(bool _debug)
	DEBUG \atcheck_buff_cycle_beg\ax()

	GETINPUT
	SETHUD "buff beg"
	/declare _count int local
	/declare _remove bool local FALSE

	/if (AUTO) /varset dump ${set_data_timer[FALSE, Check_Buff_Beg, RESTART]}

	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE


  /declare _mibb mapiterator local
  /vardata _mibb maBuffRequest.First.Clone
  /while (!${_mibb.IsEnd}) {
		
		/if (!${is_lsSafeNames[FALSE, ${_mibb.Key}]}) {
			/varset dump ${maBuffRequest.Remove[${_mibb.Key}]}
			/return FALSE
		}

		/for _count 1 to 50
			/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
			/if (!${Bool[${maBuffs.Find[stBuff${_count}Alias].Value}]}) /continue
			/if (${Spawn[${_mibb.Key}].Distance} > ENV) /continue
			
			
			/if (${maBuffs.Find[stBuff${_count}Alias].Value.Find[${_mibb.Value}]}) {
				DEBUG ${sep}BEG \ap${_mibb.Value}\ax found${sep}\aw${_count}\ax${sep}\a-w${maBuffs.Find[stBuff${_count}].Value}\ax for \a-y${_mibb.Key}\ax
				/call voodoo ${maDebug.Find[cast].Value} "${maBuffs.Find[stBuff${_count}].Value}" ${Spawn[${_mibb.Key}].ID} FALSE
			}
		/next _count
		/if (${_remove}) /varset dump ${maBuffRequest.Remove[${_mibb.Key}]}
		/varset dump ${_mibb.Advance}
  }

	SETHUD FALSE
	
/return



|***
 * note: buffs all your toons
 * use: part of standard buff cycle if enabled
 ***|
sub check_buff_cycle_crew(bool _debug)
	DEBUG \atcheck_buff_cycle_crew\ax()
	
	/if (AUTO) /varset dump ${set_data_timer[FALSE, Check_Buff_Crew, RESTART]}

	SETHUD "buff crew"
	
	/declare _spell string local FALSE
	/declare _spellTrigger string local FALSE
	/declare _count int local 0
	/declare _countToon int local 1
	/declare _target bool local FALSE
	/declare _toonName string local FALSE
	/declare _found bool TRUE
	/declare _i int local 0
	/declare _base2 string local

	
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
		/dquery ${_toonName} -q "Me.FreeBuffSlots" -o DNQReturn -t 5
		/if (${DNQReturn.Equal[0]}) {
			/varcalc _countToon ${_countToon}+1
			/continue
		}

		| each buff
		/for _count 1 to 50
			DEBUG ${sep}\agBUFF\ax#${sep}\a-w${_count}\ax ---------------------
			
			/if (!${check_basic_tags[FALSE, Buff${_count}, ${_toonName}]}) /continue
			
			| pretend the target does not have the spell
			/varset _target FALSE
			| class		
			/if (${lsBuff${_count}Tag.Contains[${Spawn[${_toonName}].Class.ShortName}]}) /varset _target TRUE
			| one of your crew
			/if (${lsBuff${_count}Tag.Contains[crew]} && ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_toonName}]} && ${Spawn[${_toonName}].ID}) /varset _target TRUE
			| main assist
			/if (${lsBuff${_count}Tag.Contains[ma]} && (${Group.MainAssist.Name.Equal[${_toonName}]} || ${maCC.Find[stManualAssist1].Value.Equal[${_toonName}]} || ${maCC.Find[stManualAssist2].Value.Equal[${_toonName}]})) /varset _target TRUE
			
			/varset _spell ${set_rank[FALSE, "${maBuffs.Find[stBuff${_count}].Value.Arg[1,+]}"]}
			DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax
			
			| skip self only
			/if (${lsBuff${_count}Tag.Count} == 1 && (${lsBuff${_count}Tag.Contains[self]} || ${Spell[${_spell}].TargetType.Equal[Self]} || ${lsBuff${_count}Tag.Contains[pet]})) /continue

			| distance check
			/if (${Spawn[${_toonName}].Distance} > ${Spell[${_spell}].MyRange}) /continue

			DEBUG ${sep}\aw470\ax${sep}\a-w${Spell[${_spell}].HasSPA[470]}\ax 
			DEBUG ${sep}\aw374\ax${sep}\a-w${Spell[${_spell}].HasSPA[374]}\ax 
			DEBUG ${sep}\aw340\ax${sep}\a-w${Spell[${_spell}].HasSPA[340]}\ax
			/if (${Spell[${_spell}].HasSPA[470]} || ${Spell[${_spell}].HasSPA[374]} || ${Spell[${_spell}].HasSPA[340]}) {
	
					/varset _found FALSE
	
				/for _i 1 to ${Spell[${_spell}].NumEffects}
					| skip any secondary procs with no duration
					/if (!${Bool[${Spell[${Spell[${_spell}].Base2[${_i}]}].Duration}]}) /continue


					/varset _spellTrigger ${Spell[${_spell}].Trigger[${_i}]}
					DEBUG ${sep}Trigger${sep}\a-w${_spellTrigger}\ax

					| does it stack
					/dquery ${_toonName} -q "Spell[${_spellTrigger}].Stacks" -o DNQReturn -t 5
					DEBUG ${sep}${sep}STACKS: .Trigger.Stacks \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					| /if (!${Bool[${DNQReturn}]}) /continue
					/if (${DNQReturn.Equal[FALSE]}) /continue

					| is it buff blocked 
					/dquery ${_toonName} -q "lsBlockedBuffsMe.Find[${Spell[${_spellTrigger}].ID}]" -o DNQReturn -t 5
					DEBUG ${sep}${sep}BLOCKED .Trigger.Blocked Buff \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					/if (${Bool[${DNQReturn}]}) /continue

					| do we have the triggered buff?
					/dquery ${_toonName} -q "Me.Buff[${_spellTrigger}].ID" -o DNQReturn -t 5
					DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					/if (!${Bool[${DNQReturn}]}) {
						/varset _found FALSE
						/break
					} else /if (${Bool[${DNQReturn}]}) {
						/varset _found TRUE
						/continue
					}
			
				/next _i

				DEBUG Found:${_found} .. Target:${_target}
				/if (!${_found} && ${_target}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maBuffs.Find[stBuff${_count}].Value}" ${Spawn[${_toonName}].ID} FALSE		
					/continue
				}

			}	else {
				
				/varset _spellTrigger ${Spell[${_spell}].RankName}
				DEBUG ${sep}Trigger${sep}\a-w${_spellTrigger}\ax ID:${Spell[${_spellTrigger}].ID}
	
				| does it stack
				/dquery ${_toonName} -q "Spell[${_spellTrigger}].Stacks" -o DNQReturn -t 5
				DEBUG ${sep}${sep}STACKS: .Buff.Stacks \at${DNQReturn}\ax ${dot} ${_spellTrigger}
				/if (${DNQReturn.Equal[FALSE]}) /continue

				| is it buff blocked 
				/dquery ${_toonName} -q "lsBlockedBuffsMe.Find[${Spell[${_spellTrigger}].ID}]" -o DNQReturn -t 1s
				DEBUG ${sep}${sep}BLOCKED .Buff.Blocked \at${DNQReturn}\ax ${dot} ${_spellTrigger}
				/if (${Bool[${DNQReturn}]}) /continue

				| do we have the triggered buff?
				/dquery ${_toonName} -q "Me.Buff[${_spellTrigger}].ID" -o DNQReturn -t 5
				DEBUG ${sep}${sep}HAVE: .Buff.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}
				/if (${Bool[${DNQReturn}]}) {
					/continue
				}	
	
				/if (${_target}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maBuffs.Find[stBuff${_count}].Value}" ${Spawn[${_toonName}].ID} FALSE		
					/continue
				}	
					
			}

		/next _count	
		/varcalc _countToon ${_countToon}+1
		
	}
	
	| if we are in a raid, we are done here
	/if (${Raid.Members} || !${Group.Members}) /return

	| i'd like to go on the recored and let everyone know i fucking hate mercs. more then bards
	| they are a bitch to code around and they kill the group content of this game
	| ok, i feel better now. moving on..

	DEBUG ${sep}Buff${sep}\a-wGroup\ax

	| non boxed people/mercs (fuck your pets)
	/for _count 1 to ${Group.Members}
	
		| "A girl has no name."
		/varset _toonName ${Group.Member[${_count}].DisplayName}
		DEBUG ${sep}_toonName${sep}\ay${_toonName}\ax
		
		| skip this person?
		/if (${_toonName.Equal[${Me.DisplayName}]} || !${Spawn[${_toonName}].ID} || ${Spawn[pc ${_toonName}].Dead}) {
			DEBUG ${sep}skipping${sep}\a-w${_toonName}\ax
			/continue
		}
		
		| one of your crew
		/if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_toonName}]}) /continue

		| maybe 50 buffs was a bit much.
		/for _count 1 to 50
			
			/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
			/if (!${Bool[${maBuffs.Find[stBuff${_count}].Value}]}) /continue
			
			/if (!${check_basic_tags[FALSE, Buff${_count}, ${_toonName}]}) /continue			
			
			| pretend the target does not have the spell
			/varset _target FALSE
			| class		
			/if (!${lsBuff${_count}Tag.Contains[${Spawn[${_toonName}].Class.ShortName}]}) /continue
			| main assist
			/if (${lsBuff${_count}Tag.Contains[ma]} && !${Group.MainAssist.Name.Equal[${_toonName}]} && !${maCC.Find[stManualAssist1].Value.Equal[${_toonName}]} && !${maCC.Find[stManualAssist2].Value.Equal[${_toonName}]}) /continue
			| skip self only
			/if (${lsBuff${_count}Tag.Count} == 1 && (${lsBuff${_count}Tag.Contains[self]} || ${Spell[${_spell}].TargetType.Equal[Self]} || ${lsBuff${_count}Tag.Contains[pet]})) /continue

			/varset _spell ${set_rank[FALSE, "${maBuffs.Find[stBuff${_count}].Value.Arg[1,+]}"]}
			DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax
			
			| distance check
			/if (${Spawn[${_toonName}].Distance} > ${Spell[${_spell}].MyRange}) /continue

			| target the group member
			/varset dump ${set_target[${Group.Member[${_toonName}].ID}]}

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

				/if (!${_found} && ${Target.ID} == ${Group.Member[${_toonName}].ID}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maBuffs.Find[stBuff${_count}].Value}" ${Group.Member[${_toonName}].ID} FALSE		
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

				/if (!${_found} && ${Target.ID} == ${Group.Member[${_toonName}].ID}) {
					/call voodoo ${maDebug.Find[cast].Value} "${maBuffs.Find[stBuff${_count}].Value}" ${Group.Member[${_toonName}].ID} FALSE		
					/continue
				}	
			}

		/next _count

		/if (${Target.ID}) /squelch /target clear
		
	}
	
/return



|***
 * note: we dont trust anyone
 * use: ${is_lsSafeNames[DEBUG, NAME]}
 ***|
sub is_lsSafeNames(bool _debug, string _name)
	DEBUG \atis_lsSafeNames\ax(\a-w${_name}\ax)
	
	| override and accept everyone
	/if (${maOver.Find[swOverSafeNames].Value}) /return TRUE

	| auto accept ${DanNetMacroChannel} channel toons
	/if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_name.Lower}]}) /return TRUE

	| basic safe list check
	/if (${maEntropy.Find[lsSafeNames].Value.Find[${_name}]}) /return TRUE

	| guild safe list check
	/if (${maEnv.Find[swSafelistGuild].Value} && ${Bool[${Me.Guild}]}) {
		/if (${Spawn[PC ${_name}].Guild.Equal[${Me.Guild}]}) /return TRUE
	}

/return FALSE



|***
 * note: incombat self buffs
 * use: /call check_buff_cycle_temp_self DEBUG [spell]
 ***|
sub check_buff_cycle_temp_self(bool _debug, string _spell)
	DEBUG \atcheck_buff_cycle_temp_self\ax(\a-w${_spell}\ax)

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE
	
	/varset dump ${set_data_timer[FALSE, Check_Buff_Temp, RESTART]}
	
	/if (${is_ready[${_debug}, "${maChr.Find[${_spell}].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[${_spell}].Value}" 0 FALSE
	}

/return



|***
 * node: checks and casts short term temp buffs that have a toon list
 * node: designed for fast cast in combat buffs
 * use: /call check_buff_cycle_temp_list DEBUG [spell var] [toon list var]
 ***|
sub check_buff_cycle_temp_list(bool _debug, string _spell, string _toonList)
	DEBUG \atcheck_buff_cycle_temp\ax(\a-w${_spell}, ${_toonList}\ax)

	ISMEDEAD
	CHECKEXIT
	GETINPUT
	CHECKTIE
	
	/varset dump ${set_data_timer[FALSE, Check_Buff_Temp, RESTART]}
	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE
	
	/declare _spellCHK string local
	DEBUG {maChr.Find[${_spell}].Value.Arg[1,+]}
	/varset _spellCHK ${maChr.Find[${_spell}].Value.Arg[1,+]}

	| ready?
	/if (!${is_ready[${_debug}, "${_spellCHK}"]}) /return FALSE

	| me?
	/if (${maBuff.Find[swBuffSelf].Value}) {
		/if (${${_toonList}.Contains[${Me.DisplayName}]} && !${Me.Buff[${_spellCHK}].ID}) {
			/call voodoo ${maDebug.Find[cast].Value} "${maChr.Find[${_spell}].Value}" ${Me.ID} FALSE
			/return TRUE
		}		
	}

	/if (!${maBuff.Find[swBuffTemp].Value}) /return FALSE
	
	/declare _castSpell bool local FALSE

	/declare _libt listiterator local
	/vardata _libt ${_toonList}.First.Clone
		/while (!${_libt.IsEnd}) {
			DEBUG ${sep}_libt.Value${sep}\a-w${_libt.Value}\ax
			/varset _castSpell FALSE

		/if (!${Spawn[pc ${_libt.Value}].ID} || ${Spawn[pc ${_libt.Value}].Dead}) {
			/varset dump ${_libt.Advance}
			/continue
		}

		/dquery ${_libt.Value} -q "Spell[${_spellCHK}].Stacks" -o DNQReturn -t 5
		DEBUG ${DNQReturn}
		/if (${Bool[${DNQReturn}]}) /varset _castSpell TRUE
			
		/dquery ${_libt.Value} -q "Me.Buff[${_spellCHK}].ID" -o DNQReturn -t 5
		DEBUG ${DNQReturn}
		/if (${Bool[${DNQReturn}]}) /varset _castSpell FALSE


		/if (${_castSpell}) /call voodoo ${maDebug.Find[cast].Value} "${maChr.Find[${_spell}].Value}" ${Spawn[=${_libt.Value} pc].ID} FALSE
		/varset dump ${_libt.Advance}
	}	

/return



|***
 * note: get an item from a spell
 * use:  /call cast_item_summon DEBUG [ITEMVARIABLE]
 ***|
sub cast_item_summon(bool _debug, string _var)
	DEBUG \atcast_item_summon\ax()
	
	CHECKTIE
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/if (${Bool[${_var}]}) {
		/declare _spellc string local ${${get_key_map[${_debug}, ${_var}]}.Find[${_var}].Value}
		/declare _spell string local ${_spellc.Arg[1,+]}
		/declare _condition string local ${_spellc.Arg[2,+]}
		/varset _spell ${set_rank[${_debug}, "${_spell}"]}
	} else {
		/varset dump ${out[0, "cast_item_summon${sep}\arERROR\ax"]}
	}
	
	/if (!${FindItem[${Spell[${_spell}].Base[1]}].ID}) {
		/call voodoo ${_debug} "${_spellc}" ${Me.ID} FALSE
		/delay 5
		/delay 15s !${Me.Casting.ID}
		
		| need autoinventory stuff here!!
		
	}
	
/return



|***
 * note: challenge line for knights
 * use: /call cast_challenge DEBUG [TARGETID]
 ***|
sub cast_challenge(bool _debug, int _tmpID)
	DEBUG \atcast_challenge\ax(\a-w${_tmpID}\ax)
	
	/if (${Target.Buff[${maChr.Find[stChallenge].Value}].ID}) /return
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	
	/if (${get_target_buff_time_left[${_debug}, "${maChr.Find[stChallenge].Value}"]} >= ${maCC.Find[stDotRefresh].Value}) /return TRUE
	
	/if (${is_Ready[${_debug}, "${maChr.Find[stChallenge].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stChallenge].Value}" ${_tmpID} FALSE
	}

/return



|***
 * note: cast nukes lists
 * use: /call cast_nuke DEBUG [line] [targetid]
 ***|
sub cast_nuke(bool _debug, string _line, int _tmpID)
	DEBUG \atcast_nuke\ax(\a-w${_line}, ${_tmpID}\ax)

	/declare _linu listiterator local
	/vardata _linu ${_line}.First.Clone
	
	/while (!${_linu.IsEnd}) {
		DEBUG _linu.Value${sep}\a-w${_linu.Value}\ax
	
		ISMEDEAD
		CHECKEXIT
		GETINPUT
	
		/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return
		/if (${Me.PctMana} < 2) /continue 
	
		/if (${is_ready[${_debug}, "${_linu.Value}"]}) {
			/call voodoo ${_debug} "${_linu.Value}" ${_tmpID} FALSE
		}
		/varset dump ${set_data_timer[${_debug}, Nuke_Delay, RESTART]}

		| maintain the loop of nukes till all are cycled.
		/if (${maCC.Find[stNukeDelay].Value}) {
			/delay 10s !${Me.Casting.ID}
			/delay 2.5s
			/delay ${timer_Nuke_Delay}
		}
		/varset dump ${_linu.Advance}
	}	

/return



|***
 * note: cast dot lists
 * use: /call cast_dot DEBUG [listname] [targetid]
 ***|
sub cast_dot(bool _debug, string _line, int _tmpID)
	DEBUG \atcast_dot\ax(\a-w${_line}, ${_tmpID}\ax)
	
	/declare _licu listiterator local
	/vardata _licu ${_line}.First.Clone
	/while (!${_licu.IsEnd}) {

		ISMEDEAD
		CHECKEXIT
		GETINPUT
		/if (${get_target_buff_time_left[${_debug}, "${_licu.Value}"]} >= ${maCC.Find[stDotRefresh].Value}) /return TRUE
		
		/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return
		
		/if (${Me.PctMana} < 2) /continue 
		
		/if (${is_ready[FALSE, "${_licu.Value}"]}) {
			/call voodoo FALSE "${_licu.Value}" ${_tmpID} FALSE
		}
		
		/varset dump ${set_data_timer[FALSE, Nuke_Delay, RESTART]}

		| maintain the loop of nukes till all are cycled.
		/if (${maEnv.Find[swNukeLoop].Value}) {
			/delay 10s !${Me.Casting.ID}
			/delay 2.5s
			/delay ${timer_Nuke_Delay}
		}

		/varset dump ${_licu.Advance}
	}	

/return



|***
 * nore: use base melee discs
 * use: /call do_melee DEBUG [TARGETID]
 ***|
sub do_melee(bool _debug, int _tmpID)
	DEBUG \atdo_melee\ax(\a-w${_tmpID}\ax)
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	
	/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return
	/varset dump ${set_target[${_tmpID}]}
	
	/if (AUTO) {
		/call check_facing FALSE ${_tmpID} FORCE
		/call stick_to_target ${_debug} TARGET
	}

	| taunt /onoff agro
	/if (${maCC.Find[swTaunt].Value} && AGRO) {
		/if (${Me.AbilityReady[Taunt]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
			/varset dump ${out[8, "Melee${sep}${sp}Taunt\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/doability Taunt
		}
	}
	
	| disarm
	/if (${maCC.Find[swDisarm].Value} && !${Skill[Disarm].Auto}) {
		/if (${Me.AbilityReady[Disarm]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]} && AGRO) {
			/varset dump ${out[8, "Melee${sep}${sp}Disarm\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/doability Disarm
		}
	}
		
	| kick
	/if (${maCC.Find[swKick].Value} && !${Skill[Kick].Auto} && !${Select[${Me.Class.ShortName},MNK]}) {
	 	/if (${Me.AbilityReady[Kick]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]}) {
			/varset dump ${out[8, "Melee${sep}${sp}Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/doability Kick
		}
	}
	
	| bash
	/if (${maCC.Find[swBash].Value} && !${Skill[Bash].Auto}) {
		/if (${Me.AbilityReady[Bash]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]}) {
			/varset dump ${out[8, "Melee${sep}${sp}Bash\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/if (${swBandolier} && ${Bool[${bandolierBash}]} && ${Bool[${bandolierNormal}]}) {
				/if (${Target.ID}) {
					/call bandolier_swap Bash
					/doability Bash
					/call bandolier_swap Normal
				}
			} else /if (!${swBandolier}) {
				/doability Bash
			}
		}
	}	
	
	| Intimidation
	/if (${check_class[FALSE, |BRD|ROG|BER]}) {
		/if (${maCC.Find[swIntimidation].Value}) {
			/if (${Me.AbilityReady[Intimidation]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]}) {
				/varset dump ${out[8, "Melee${sep}${sp}Intimidation\ax ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax"]}
				/doability Intimidation		
			}
		}
	}

	| Zerker only skill
	/if (${check_class[FALSE, |BER]}) {
		/if (${maCC.Find[swFrenzy].Value} && && !${Skill[Frenzy].Auto}) {
			/if (${Me.AbilityReady[Frenzy]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]}) {
			/varset dump ${out[8, "Melee${sep}${sp}Frenzy\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability Frenzy
			}
		}
	}

	| rogue only skill
	/if (${check_class[FALSE, |ROG]}) { 
		/if (${maCC.Find[swBackstab].Value} && && !${Skill[Backstab].Auto}) {
			/if (${Me.AbilityReady[Backstab]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]}) {
			/varset dump ${out[8, "Melee${sep}${sp}Backstab\ax ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax"]}
				/doability Backstab
			}
		}
	}
	
	| monk/bst only skill 
	/if (${check_class[FALSE, |MNK|BST]}) {
			/if (${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.9]}) {
			| fist
			/if (${maCC.Find[swDragonpunch].Value} && !${Skill[Dragon Punch].Auto} && ${Me.AbilityReady[Dragon Punch]}) {
				/varset dump ${out[8, "Melee${sep}${sp}Dragon Punch\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Dragon Punch"
			} else /if (${maCC.Find[swEaglestrike].Value} && !${Skill[Eagle Strike].Auto} && ${Me.AbilityReady[Eagle Strike]}) {
				/varset dump ${out[8, "Melee${sep}${sp}Eagle Strike\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Eagle Strike"
			} else /if (${maCC.Find[swTigerclaw].Value} && !${Skill[Tiger Claw].Auto} && ${Me.AbilityReady[Tiger Claw]}) {
			/varset dump ${out[8, "Melee${sep}${sp}Tiger Claw\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Tiger Claw"
			}
			| kick
		 	/if (${maCC.Find[swKick].Value} && !${Skill[Kick].Auto} && ${Me.AbilityReady[Kick]}) {
				/varset dump ${out[8, "Melee${sep}${sp}Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability Kick
			} else /if (${maCC.Find[swFlyingkick].Value} && !${Skill[Flying Kick].Auto} && ${Me.AbilityReady[Flying Kick]}) {
				/varset dump ${out[8, "Melee${sep}${sp}Flying Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Flying Kick"
			} else /if (${maCC.Find[swRoundkick].Value} && !${Skill[Round Kick].Auto} && ${Me.AbilityReady[Round Kick]}) {
				/varset dump ${out[8, "Melee${sep}${sp}Round Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Round Kick"
			}		
		}
	}

/return



|***
 * note: uses melee rest disc
 * use: /call cast_stamina_recovery DEBUG
 ***|
sub cast_stamina_recovery(bool _debug)
	DEBUG \atcast_stamina_recovery\ax()
	
	/if (${Me.PctEndurance} > 34) /return
	
 	/if (SAFEZONE) /return
	ISMEDEAD
	GETINPUT
	
	/declare _endurancePoint int local 0
	
	| haitus switch
	/if (${maChr.Find[swHiatus].Value} && ${Me.CombatAbilityReady[${Spell[Hiatus].RankName}]} && ${Me.PctEndurance} < 20) {
		DEBUG rest${sep}\a-whiatus\ax
		/if (${Me.CombatAbility[Hiatus Rk. III]} && (${Me.CurrentEndurance} < 102500 && ${Me.PctEndurance} < 34)) /varset _endurancePoint 34
		/if (${Me.CombatAbility[Hiatus Rk. II]} && (${Me.CurrentEndurance} < 97500 && ${Me.PctEndurance} < 32)) /varset _endurancePoint 32
		/if (${Me.CombatAbility[Hiatus]} && (${Me.CurrentEndurance} < 92500 && ${Me.PctEndurance} < 29)) /varset _endurancePoint 29
		
	 	/if (${Me.PctEndurance} < ${_endurancePoint}) {
	 		/if (${is_ready[${_debug}, "${Spell[Hiatus].RankName}"]}) {
	 			/call voodoo ${_debug} "${Spell[Hiatus].RankName}" 0 FALSE
	 		}
	 	}		
		/return TRUE
		
	| rest disc	
	} else /if (${Me.CombatAbilityReady[${Spell[stRest].RankName}]} && !${Select[${Me.CombatState},active]}) {
		DEBUG rest${sep}\a-w${maChr.Find[stRest].Value}\ax
		/if (${Me.CombatAbility[${maChr.Find[stRest].Value} Rk. III]}) /varset _endurancePoint 29
		/if (${Me.CombatAbility[${maChr.Find[stRest].Value} Rk. II]}) /varset _endurancePoint 25
		/if (${Me.CombatAbility[${maChr.Find[stRest].Value}]}) /varset _endurancePoint 21

	 	/if (${Me.PctEndurance} < ${_endurancePoint}) {
	 		/if (${is_ready[${_debug}, "${maData.Find[stRest].Value}"]}) {
	 			/call voodoo ${_debug} "${maData.Find[stRest].Value}" 0 FALSE
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
	CHECKEXIT
	GETINPUT

	/varset dump ${set_data_timer[FALSE, Check_Harmonious, RESTART]}
	
	/if (!${Me.Buff[${maChr.Find[stHarmonious].Value}].ID} && ${Me.PctMana} > 2) {
		/if (${is_ready[FALSE, "${maChr.Find[stHarmonious].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stHarmonious].Value}" 0 FALSE
		}
	}

/return



|***
 * note: checks tags for buffs..
 * use: ${check_basic_tags[DEBUG, type, name]}
 ***|
sub check_basic_tags(bool _debug, string _type, string _name)
	DEBUG \atcheck_basic_tags\ax(\a-w${_type}, ${_name}\ax)

	| no buffing in combat
	/if (${ls${_type}Tag.Contains[nocombat]} && AUTO) {
		/if (${Select[${Me.CombatState},combat]}) /return FALSE
		/if (${SpawnCount[npc radius SAFEENV PS4]}) /return FALSE
	}
	
	| no cast while not in a raid
	/if (${ls${_type}Tag.Contains[raid]}) /if (!${Raid.Members}) /return FALSE
	/if (${ls${_type}Tag.Contains[group]}) /if (!${Group.Members}) /return FALSE
	/if (${ls${_type}Tag.Contains[noraid]} && ${Raid.Members}) /return FALSE
	/if (${ls${_type}Tag.Contains[combat]} && ${Select[${Me.CombatState},cooldown,resting,active]}) /return FALSE
	/if (${ls${_type}Tag.Contains[outdoor]} && (${Me.FeetWet} || ${Zone.Indoor})) /return FALSE
	/if (${ls${_type}Tag.Contains[puller]} && PULL) /return FALSE
	/if (${ls${_type}Tag.Contains[agro]} && !${Bool[${maCC.Find[stAssistMode].Value}]}) /return FALSE
	/if (${ls${_type}Tag.Contains[burn]} && !BURN) /return FALSE
	/if (${ls${_type}Tag.Contains[nosafezone]} && SAFEZONE) /return FALSE
	/if (${ls${_type}Tag.Contains[matarget]} && (${Target.ID} != TARGET)) /return FALSE
	/if (${ls${_type}Tag.Contains[pet]} && !${Pet.ID}) /return FALSE
	/if (${ls${_type}Tag.Contains[swarm]} && !SWARM) /return FALSE

	/if (${ls${_type}Tag.Contains[ma]}) {
		/if (${Me.ID} != ${get_assist_info[FALSE, ID]}) /return FALSE
	}

	/if (${ls${_type}Tag.Contains[summonitem]}) {
		/if (${check_basic_tags_summonitem[FALSE, ${_type}]}) /return FALSE
	}
	
	
	| name/class specific tags
	/if (!${Bool[${_name}]} || ${_name.Equal[FALSE]}) /return TRUE
	/if (!${Spawn[${_name}].ID}) /return FALSE
	
	| class		
	/declare _li listiterator local
	/vardata _li lsClassAll.First.Clone
	/while (!${_li.IsEnd}) {
		/if (${ls${_type}Tag.Contains[${_li.Value}]}) {
			/if (${Spawn[${_name}].Class.ShortName.NotEqual[${_li.Value}]}) /return FALSE
		}
		/varset dump ${_li.Advance}
	}	
		
	| one of your crew
	/if (${ls${_type}Tag.Contains[crew]}) {
		/if (!${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_name}]}) /return FALSE
	}
	
	| main assist
	/if (${ls${_type}Tag.Contains[ma]}) {
		/if (${Group.MainAssist.Name.Equal[${_name}]} && ${maCC.Find[stManualAssist1].Value.Equal[${_name}]} && ${maCC.Find[stManualAssist2].Value.Equal[${_name}]})) /return FALSE
	}
	
/return TRUE



|***
 * note: checks the tag "summonitem" for a count of the item to be summoned on the toon. returns the count
 * use: ${check_basic_tags_summonitem[DEBUG, BUFF##]}
 ***|
sub check_basic_tags_summonitem(bool _debug, string _buff)

	/declare _count int local
	/varset _count ${FindItemCount[${Spell[${Spell[${maBuffs.Find[st${_buff}].Value}].RankName}].Base[1]}]}

/return ${_count}



|***
 * note: looks for dead fuckers. if we find dead fuckers, and they fall under dead fucker res status, we rez said dead fuckers
 * use: /call check_for_corpse DEBUG
 ***|
sub check_for_corpse(bool _debug)
	DEBUG \atcheck_for_corpse\ax()

	/if (${Me.State.Equal[HOVER]} || ${Me.Hovering} || ${Me.Dead}) /return FALSE
	
	| fuck your corpse
	/if (!${maRez.Find[swRezGive].Value}) /return	FALSE
	| no one dead
	/if (!${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}) /return FALSE
	| no combat rez
	/if (!${maRez.Find[swRezCombat].Value} && ${SpawnCount[npc radius ${maRez.Find[stMaxRezRange].Value} PS4]}) /return FALSE
	| no safe zone ressing
	/if (SAFEZONE) /return FALSE
	| not enough manna for AA rez
	/if (${Me.PctMana} < 2 && !${maRez.Find[swRezToken].Value}) /return FALSE

	| no emergency tokens AND dont use them when a cleric is around
	/if (${maRez.Find[swRezToken].Value}) {
		/if (!${Me.ItemReady[Token of Resurrection]}) /return FALSE
		/if (${Spawn[pc CLR radius ${maRez.Find[stMaxRezRange].Value}].ID}) /return FALSE

		| disable rezemergency if you dont have tokens or are on a TLP box
		/if (!${FindItemCount[=Token of Resurrection]}) {
			/varset dump ${do_raw_edit[FALSE, SILENT, swRezToken, FALSE]}
			/varset dump ${out[0, "${Me.DisplayName} needs Rez Tokens...."]}
		}
	}

		
	| set rez name/type	
	| necro stuff
	/if (${Select[${Me.Class.ShortName},NEC]}) {
		/if (!${FindItem[Essence Emerald].ID}) /return FALSE
		/declare _rezNEC												string	local "Convergence"

	| cleric stuff
	} else /if (${Select[${Me.Class.ShortName},CLR]}) {
		/declare _divinerezCLR									string	local "Divine Resurrection"
		/declare _rezCLR												string	local "Blessing of Resurrection"
		
		/if (!${Me.AltAbility[Blessing of Resurrection]} && ${FindItemCount[=Water Sprinkler of Nem Ankh]}) {
			/varset _divinerezCLR	Water Sprinkler of Nem Ankh
			/varset _rezCLR Water Sprinkler of Nem Ankh
		}
	
	| pally, shaman, druid stuff
	} else /if (${Select[${Me.Class.ShortName},PAL,SHM,DRU]}) {
		/if (${Select[${Me.CombatState},combat]}) {
			/declare _rezDRU											string	local "Call of the Wild"
			/declare _rezSHM											string	local "Call of the Wild"
			/declare _rezPAL											string	local "Gift of Resurrection"
		} else {                      					
			/declare _rezDRU											string	local "Rejuvenation of Spirit"
			/declare _rezSHM											string	local "Rejuvenation of Spirit"
			/declare _rezPAL											string	local "Restore Life"
		}
	}

	/if (${maRez.Find[swRezToken].Value}) {		
		/declare _rezEmergency									string	local "Token of Resurrection"
	}


	/declare _count 													int 		local 0
	/declare _tmpID 													int 		local 0

	| make the damn variables
	/for _count 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}
		/varset _tmpID ${NearestSpawn[${_count},pccorpse radius ${maRez.Find[stMaxRezRange].Value}].ID}
		
		| for the people that suck ass (not in a good way)
		/if (${lsNoRezToon.Contains[${Spawn[${_tmpID}].Name}]}) {
			/if (!${Defined[timer_corpse_${_tmpID}]}) /declare timer_corpse_${_tmpID} timer outer 7200
			/continue
		}	
		
		/if (!${Defined[timer_corpse_${_tmpID}]}) {
			/declare timer_corpse_${_tmpID} timer outer 1
			/delay 1
		}
	/next _count

	 SETHUD "resurrect"
	
	| clerics first group or raid
	/if (${SpawnCount[pccorpse cleric raid radius ${maRez.Find[stMaxRezRange].Value}]} && ${Raid.Members} || ${SpawnCount[pccorpse cleric group radius ${maRez.Find[stMaxRezRange].Value}]}) {
		/for _count 1 to ${SpawnCount[pccorpse cleric radius ${maRez.Find[stMaxRezRange].Value}]}
		ISMEDEAD
		/if (!${timer_corpse_${NearestSpawn[${_count},pccorpse cleric radius ${maRez.Find[stMaxRezRange].Value}].ID}}) {
			/varset _tmpID ${NearestSpawn[${_count},pccorpse cleric radius ${maRez.Find[stMaxRezRange].Value}].ID}
			/if (${Select[${Me.Class.ShortName},CLR]}) {
				/if (${maRez.Find[swDivineCleric].Value} && ${Me.AltAbilityReady[${_divinerezCLR}]}) /call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_divinerezCLR}
			} else /if (${Select[${Me.Class.ShortName},CLR,DRU,PAL,SHM,NEC]} && ${Me.AltAbilityReady[${_rez${Me.Class.ShortName}}]}) {
				/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rez${Me.Class.ShortName}}
			} else /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
				/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rezEmergency}
			}
			/return
		}
		/next _count
	} 
	
	| group members
	/if (${SpawnCount[pccorpse group radius ${maRez.Find[stMaxRezRange].Value}]}) {
		/for _count 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value} group]}
		/if (!${timer_corpse_${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} group].ID}}) {
			/varset _tmpID ${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} group].ID}
			/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
				/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rezEmergency}
			} else {
				/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rez${Me.Class.ShortName}}
			}
			/return
		}
		/next _count
	}		
	
	| tanks
	/if (${Raid.Members}) {
		/if (${SpawnCount[pccorpse raid tank radius ${maRez.Find[stMaxRezRange].Value}]}) {
			/for _count 1 to ${SpawnCount[pccorpse raid radius ${maRez.Find[stMaxRezRange].Value} tank]}
			/if (!${timer_corpse_${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value} tank].ID}}) {
				/varset _tmpID ${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value} tank].ID}
				/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
					/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rezEmergency}
				} else {
					/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rez${Me.Class.ShortName}}
				}
				/return
			}
			/next _count
		} 
	
		| remaining raid members
		/if (${SpawnCount[pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}]}) {
			/for _count 1 to ${SpawnCount[pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}]}
			/if (!${timer_corpse_${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}].ID}}) {
				/varset _tmpID ${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}].ID}
				/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
					/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rezEmergency}
				} else {
					/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rez${Me.Class.ShortName}}
				}
				/return
			}
			/next _count
		} 
	}
	
	| guildies - all the time
	/if (${SpawnCount[pccorpse guild radius ${maRez.Find[stMaxRezRange].Value}]}) {
		/for _count 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value} GUILD]}
		/if (!${timer_corpse_${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} guild].ID}}) {
			/varset _tmpID ${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} guild].ID}
			/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
				/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rezEmergency}
			} else {
				/call cast_rez ${maDebug.Find[rez].Value}${_tmpID} ${_rez${Me.Class.ShortName}}
			}
			/return
		}
		/next _count
	}
	
	| outside guild and raid
	| needs /rez everyone switch to be true
	/if (${maRez.Find[swRezEveryone].Value}) {
		/if (${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}) {
			/for _count 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}
			/if (!${timer_corpse_${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value}].ID}}) {
				/varset _tmpID ${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value}].ID}
				/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
					/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rezEmergency}
				} else {
					/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_rez${Me.Class.ShortName}}
				}
				/return
			}
			/next _count
		}	
	}

	SETHUD FALSE

/return



|***
 * note: casts trez thingies and sets any needed timers for dead fuckers
 * use: /call cast_rez DEBUG [ID] [rez aa|token]
 ***|
sub cast_rez(bool _debug, int _tmpID, string _rez)
	DEBUG \atcast_rez\ax(\a-w${_tmpID}, ${_rez}\ax)
	DEBUG _rez${sep}\a-w${_rez}\ax
	DEBUG _tmpID${sep}(${_tmpID})\a-w${Spawn[${_tmpID}].DisplayName}\ax
	
	/if (${Me.Dead}) /return FALSE
	/if (!${Defined[timer_corpse_${_tmpID}]}) /return FALSE
	/if (${timer_corpse_${_tmpID}}) /return FALSE
	ISMEDEAD
	CHECKEXIT

	| ignore the people we dont like
	/if (${lsNoRezToon.Contains[${Spawn[pccorpse ${_tmpID}].DisplayName}]}) {
		/call set_timer timer_corpse_${_tmpID}} 30m
		/return FALSE
	}
		
	| get the corpse close to us
	/if (${Spawn[pccorpse ${_tmpID}].Distance} > 10 && ${Spawn[pccorpse ${_tmpID}].Distance} <= ${maRez.Find[stMaxRezRange].Value}) {
		/squelch /corpse
	}

	| can we AA res or token, maybe spell?
	/if (${Me.AltAbilityReady[${_rez}]} || (${Me.ItemReady[Token of Resurrection]} && ${maRez.Find[swRezToken].Value}) || ${Me.SpellReady[${_rez}]}) {	

		| not token rez rez
		/if (!${maRez.Find[swRezToken].Value}) {
			DEBUG casting no token
			/delay 5s !${Me.Casting.ID}
			/varset dump ${set_target[${_tmpID}]}

			/if (${is_ready[${_debug}, "${_rez}"]}) {
				/call voodoo ${_debug} "${_rez}" ${_tmpID} skiptargetcheck
			}

		| token rez
		} else /if (${maRez.Find[swRezToken].Value}) {
			DEBUG clicking token
			/delay 5s !${Me.Casting.ID}
			/varset dump ${set_target[${_tmpID}]}
			/if (${is_ready[${_debug}, "Token of Resurrection"]}) {
				/call voodoo ${_debug} "Token of Resurrection" ${_tmpID} skiptargetcheck
			}
		}
	
		| if cast was good, set the timers.
		/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) { 
			/call set_timer timer_corpse_${_tmpID} 5m
			/varset dump ${out[0, "${Me.Class.ShortName} Rez: ${Spawn[${_tmpID}].DisplayName}"]}
		} else {
			/if (${Select[${Me.Class.ShortName},CLR]}) {
				/call set_timer timer_corpse_${_tmpID} 30s
			} else /if (${Select[${Me.Class.ShortName},DRU,SHM,PAL]}) {
				/call set_timer timer_corpse_${_tmpID} 45s
			} else {
				/call set_timer timer_corpse_${_tmpID} 1m
			}
		}
		
	}
/return



|***
 * note: converts a click item
 * use: /call get_item_convert DEBUG [item to find] [full name]  
 ***|
sub get_item_convert(bool _debug, string _item, string _fullname)
	DEBUG \atget_item_convert\ax(\a-w${_item}, ${_fullname}\ax)
	/while (!${FindItem[${_fullname}].ID}) {
		/convertitem ${_item}
		/delay 1s					
	}
 
/return



|***
 * note: sets/updates a non permanent timer
 * use: ${set_timer[DEBUG, [TIMER VARIABLE NAME], [DURATION]]}
 ***|
sub set_timer(bool _debug, string _my_timer, string _my_duration)
	DEBUG \atset_timer\ax(\a-w${_my_timer}, ${_my_duration}\ax)
	
	/if (!${Defined[${_my_timer}]}) /declare ${_my_timer} timer outer
	/varset ${_my_timer} ${_my_duration}
	
/return TRUE




|***
 *	DES: advanced loot routine(s)
 *	USE: auto
 *	NOTE: set a person in the group as master looter. Or, as the raid master looter.
 *	NOTE2: The event in this sectction is designed to handle lore items the toon already posseses. as of this, MQ has no property .Member value for LORE in ${AdvLoot
 ***|
| #Event forcedloreleave "#*#already has #1# and it is lore#*#"
| #Event forcedloreleave "#*#does not want #1#. It is either on their never list or they have selected No#*#"
sub Event_forcedloreleave(string line, string _itemName, bool _debug)
	/if (!${maEnv.Find[swADVLoot].Value}) /return
	/varset _itemName ${AdvLoot.SList[1].Name}
	/varset dump ${out[14, "Cursor ${sep} ${cinfo}${_itemName}\ax -> ${oleave}"]}
	/ini "${INILoot}" "${_itemName.Left[1]}" "${_itemName}" Leave
	/doevents flush
/return

sub do_loot(bool _debug)
	DEBUG \atdo_loot\ax()
	
	ISMEDEAD
	GETINPUT

	/varset dump ${set_data_timer[FALSE, Check_Loot, RESTART]}

	/if (${Stick.Active} || !${Me.FreeInventory} || !${Me.UseAdvancedLooting}) /return

	| I am not the raid loot master /sigh
	/if (${Raid.Members} && ${Raid.MasterLooter.ID} != ${Me.ID} && !${maOver.Find[swOverLoot].Value}) /return
	/if (!${Raid.Members} && ${Group.Members} && ${Group.MasterLooter.ID} != ${Me.ID} && !${maOver.Find[swOverLoot].Value}) /return

	SETHUD "loot whoring"

	/delay 2s !${AdvLoot.LootInProgress}
	
	/declare _scount int local 0
	/declare _pcount int local 0
	/declare _notfound bool local FALSE
	
	
	| Item To Process Name (the thingie in the list of advloot)
	/declare _advlItem string local
	
	| _advlChoice is what is listed to do with the items: Leave,Destroy,Keep,Sell,Announce
	/declare _advlChoice string local
	
	| if we want to give it to someone else
	/declare _advlPerson string local

	/if (${Raid.Members} || ${Group.Members}) {
		DEBUG ${sep}SHARED LOOT ${submenu} RAID${sep}\a-w${Raid.Members}\ax || GROUP${sep}\a-w${Group.Members}\ax
		| Shared List Sorting
		/for _scount ${AdvLoot.SCount} downto 1
			/varset _advlItem ${AdvLoot.SList[${_scount}].Name}
			
			/if (${_advlItem.Equal[NULL]}) /break
			
			/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${_advlItem}"
			DEBUG ${sep}lootquery SELECT * FROM loot_control WHERE item="${_advlItem}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
			
			/if (${sqlite.Status[lootquery].NotEqual[Success]} || !${sqlite.Rows[lootquery]}) {
				/varset _notfound TRUE
				DEBUG ${sep}${_advlItem}${sep}\a-w not in database\ax
				/if (${AdvLoot.SList[${_scount}].NoDrop} || ${FindItem[=${_advlItem}].Lore} || ${FindItemBank[${_advlItem}].Lore}) {
					/varset dump ${out[14, "${add} ${dot} ${_advlItem} = ${leave} [no drop/lore]"]}
					/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave")
					DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave") \ay${sqlite.Status[lootquery]}\ax
				} else {
					 OUT ${add} ${dot} ${_advlItem} = ${keep} [default]
					/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "keep")
					DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "keep") \ay${sqlite.Status[lootquery]}\ax
				}
			}
	
			| reload data if item was not found and we changed its status
			/if (${_notfound}) {
				/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${_advlItem}"
				DEBUG ${sep}RELOADING${sep}]lootquery SELECT * FROM loot_control WHERE item="${_advlItem}" \ay${sqlite.Status[lootquery]}\ax
				/varset _notfound FALSE
			}
			
			/varset _advlChoice ${sqlite.Result[lootquery 1 action]}
			DEBUG ${sep}_advlChoice${sep}\a-w${sqlite.Result[lootquery 1 action]}\ax
			/varset _advlPerson ${sqlite.Result[lootquery 1 pass]}
			DEBUG ${sep}_advlPerson${sep}\a-w${sqlite.Result[lootquery 1 pass]}\ax

			| now we do something with the item.
			
			| if we are going to leave the item or if it is lore AND we have one already. Conversly, we loot if we intend to keep or destroy the item
			| pass the item off if there is a person listed and in group
			/if (${Bool[${_advlPerson}]} && ${Group.Member[${_advlPerson}].ID}) {
				/delay 1s !${AdvLoot.LootInProgress}
				/if (!${AdvLoot.LootInProgress}) /advloot shared ${_scount} giveto ${_advlPerson}
				DEBUG ${sep}Passing to${sep}\a-w${_advlPerson}\ax
				/delay 1
			
			| ditch the item if we need too
			} else /if (${Select[${_advlChoice},leave]} || ${FindItem[=${_advlItem}].Lore} || ${FindItemBank[${_advlItem}].Lore}) {
				/if (${Select[${_advlChoice},Announce]}) ${out[14, "Loot Leaving${sep}\a-w${_advlItem}\ax"]} 
				/delay 1s !${AdvLoot.LootInProgress}
				/if (!${AdvLoot.LootInProgress}) /advloot shared ${_scount} leave
				DEBUG ${sep}Leaving${sep}\a-w${_advlItem}\ax
				/delay 1
				/continue

			| or we keep it.
			} else /if (${AdvLoot.SList[${_scount}].ID}) {
				/if (${Select[${_advlChoice},keep,destroy,sell]}) {
					/delay 1s !${AdvLoot.LootInProgress}
					/if (!${AdvLoot.LootInProgress}) /advloot shared ${_scount} giveto ${Me.Name}
					DEBUG ${sep}Keeping for next step${sep}\a-w${_advlItem}\ax
					/delay 1
				}
			}
		/next _scount
	}
	
	| Personal List sorting. ALSO for solo killing
	/if (${AdvLoot.PCount}) {
		DEBUG ${sep}PERSONAL LOOT ${submenu}
		/for _pcount ${AdvLoot.PCount} downto 1
			/varset _advlItem ${AdvLoot.PList[${_pcount}].Name}

			/if (${_advlItem.Equal[NULL]}) /break
			DEBUG ${sep}personal${sep}${_pcount}${sep}${AdvLoot.PList[${_pcount}].Name}

			/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${_advlItem}"
			DEBUG ${sep}lootquery SELECT * FROM loot_control WHERE item="${_advlItem}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
			
			/if (${sqlite.Status[lootquery].NotEqual[Success]} || !${sqlite.Rows[lootquery]}) {
				/varset _notfound TRUE
				DEBUG ${sep}${_advlItem}${sep}\a-wnot in database\ax
				/if (${Advloot.Slist[${_Scount}].Nodrop} || ${FindItem[=${_advlItem}].Lore} || ${FindItemBank[${_advlItem}].Lore}) {
					/varset dump ${out[0, "${add} ${dot} ${_advlItem} = ${leave} [no drop/lore]"]}
					/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave")
					DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave") \ay${sqlite.Status[lootquery]}\ax
				} else {
					 OUT ${add} ${dot} ${_advlItem} = ${keep} [default]
					/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "keep")
					DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "keep") \ay${sqlite.Status[lootquery]}\ax
				}
			}

			| reload data if item was not found and we changed its status
			/if (${_notfound}) {
				/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${_advlItem}"
				DEBUG ${sep}RELOADING${sep}]lootquery SELECT * FROM loot_control WHERE item="${_advlItem}" \ay${sqlite.Status[lootquery]}\ax
				/varset _notfound FALSE
			}

			/varset _advlChoice ${sqlite.Result[lootquery 1 action]}
			DEBUG ${sep}_advlChoice${sep}\a-w${sqlite.Result[lootquery 1 action]}\ax
		

			/if (${AdvLoot.PList[${_pcount}].ID}) {
				/if (${Select[${_advlChoice},ignore,leave]} || ${FindItem[=${_advlItem}].Lore} || ${FindItemBank[${_advlItem}].Lore}) {
					DEBUG ${sep}${_advlChoice}
					/varset dump ${out[14, "Leaving${sep}\a-w${_advlItem}\ax"]}
					/delay 1s !${AdvLoot.LootInProgress}
					/if (!${AdvLoot.LootInProgress}) /advloot personal ${_pcount} leave
				} else /if (${Select[${_advlChoice},keep,sell]}) {
					DEBUG ${sep}${_advlChoice}
					/delay 1s !${AdvLoot.LootInProgress}
					/advloot personal ${_pcount} loot
					/delay 1
					/if (${Window[ConfirmationDialogBox].Open}) {
						/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
						/delay 1
					}
				} else /if (${Select[${_advlChoice},destroy]}) {
					DEBUG ${sep}${_advlChoice}
					/delay 1s !${AdvLoot.LootInProgress}
					/advloot personal ${_pcount} loot
					/delay 1s !${AdvLoot.LootInProgress}
					/delay 5
					/if (${FindItem[=${_advlItem}].ID}) {
					| I'M SERIOUSLY SICK OF FIXING THIS delete code.
						/while (${FindItem[=${_advlItem}].ID}) {
							/delay 5
							/nomodkey /shift /itemnotify in Pack${Math.Calc[${FindItem[=${_advlItem}].ItemSlot}-22]} ${Math.Calc[${FindItem[=${_advlItem}].ItemSlot2}+1]} leftmouseup

							/varset dump ${out[14, "Destroying${sep}\a-w${_advlItem}\ax"]}
							/nomodkey /destroy
					
							| /call check_cursor FALSE TRUE CLEAR
							/if (${Window[ConfirmationDialogBox].Open}) {
								/delay 1
								/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
							}
							/delay 3 !${Cursor.ID}
						}
						| evil delete code end...
					}          
				} 
			/next _pcount
		}
	}

/return


						/nomodkey /shift /itemnotify in Pack${Int[${Math.Calc[${FindItem[=Scale Ore].ItemSlot}-22]}]} ${Math.Calc[${FindItem[=Scale Ore].ItemSlot2}+1]} leftmouseup





|***
 * note: Cures/removes debuffs, we hope...
 * use: 
 ***|
sub check_cure(bool _debug)
	DEBUG \atcheck_cure\ax()
	
	/if (AUTO) /varset dump ${set_data_timer[FALSE, Check_Cure, RESTART]}

	| dont take time from mobs attention
	/if ((PULL || AGRO) && ${Me.CombatState.Equal[combat]}) /return

	ISMEDEAD
	GETINPUT

	| My daughter said you have issues.. She's usually right.
	/if (${Me.TotalCounters}) {
		DEBUG ME totalcounters${sep}\a-w${Me.TotalCounters}\ax
		
		| Monk
		/if (${Select[${Me.Class.ShortName},MNK]} && ${Me.AltAbilityReady[Purify Body]}) {
			/call voodooo FALSE "Purify Body" 0 FALSE
			/delay 5
		}
			
		| Paladin, Cleric, Shaman, Druid
		} else /if (${Select[${Me.Class.ShortName},PAL,CLR,SHM,DRU]} && ${Me.AltAbilityReady[Radiant Cure]}) {
			/if (${Me.CountersCurse} || ${Me.CountersPoison} || ${Me.CountersDisease}) {
				/call voodoo FALSE "Radiant Cure" 0 FALSE
				/delay 5
				/return
			}
			
			| Paladin splash
			| /if (${Select[${Me.Class.ShortName},PAL]}) {
			| 	/call Bind_pal_splash
			| 	/delay 5
			| 	/return
			| }

		| Necromancer
		} else /if (${Select[${Me.Class.ShortName},NEC]} && ${Me.AltAbilityReady[Embrace the Decay]}) {
			/if (${Me.CountersCurse} || ${Me.CountersPoison} || ${Me.CountersDisease} || ${Me.CountersCorruption}) {
				/call voodoo FALSE "Embrace the Decay" 0 FALSE
				/delay 5
			}
		}
		
		| cure with items self only
		/if (${Me.CountersCurse} && ${Me.ItemReady[${maHeal.Find[stCureCurseItem].Value}]}) {
			/call voodoo FALSE "${maHeal.Find[stCureCurseItem].Value}" ${Me.ID} FALSE
			/delay 5
		}
		
		/if (${Me.CountersCorruption} && ${Me.ItemReady[${maHeal.Find[stCureCorruptionItem].Value}]}) {
			/call voodoo FALSE "${maHeal.Find[stCureCorruptionItem].Value}" ${Me.ID} FALSE
			/delay 5
		}
		
		/if (${Me.CountersPoison} && ${Me.ItemReady[${maHeal.Find[stCurePoisonItem].Value}]}) {
			/call voodoo FALSE "${maHeal.Find[stCurePoisonItem].Value}" ${Me.ID} FALSE
			/delay 5
		}
		
		/if (${Me.CountersDisease} && ${Me.ItemReady[${maHeal.Find[stCureDiseaseItem].Value}]}) {
			/call voodoo FALSE "${maHeal.Find[stCureDiseaseItem].Value}" ${Me.ID} FALSE
			/delay 5
		}
	}
	

	/declare _count int local 0
	/declare _name string local
	
	| each toon
	DEBUG Total group:${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
	/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}

		| get the name to something usable
		/varset _name ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}]}
		DEBUG ${sep}cast_cure()${sep}\a-weveryone else\ax
		DEBUG vChecking${sep}\aw${_name}\ax in ${maComm.Find[stEntropyGroup_all].Value}

		| are they here?
		/if (!${Spawn[pc ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}].ID}) /continue
			
		| if the observer hasnt "observerd" the toon yet. go no further!	
		/if (!${Defined[${_name}_TotalCounters]}) {
			DEBUG ${_name} is not being observerd.. \a-wshit coding inc\ax...
			/continue
		}
		
		| are they close?. if not. fuck'em
		/if (${Spawn[pc ${_name}].Distance} > 200) /continue

		| toon dead?
		/if (${Spawn[pc ${_name}].Dead}) /continue
	
		| are they debuffed at all?
		/if (!${Bool[${${_name}_TotalCounters}]} && !${Bool[${${_name}_Snared}]} && !${Bool[${${_name}_Mezzed}]}) {
			DEBUG ${_name} has NOTHING wrong with them.
			/continue
		}

		ISMEDEAD
				
		| healer class AA curing
		/if (${Select[${Me.Class.ShortName},PAL,CLR,SHM,DRU]}) {

			| AA Radient Cure 
			| 100' 1 min
			/if (${Select[${Me.Class.ShortName},PAL,CLR,SHM,DRU]} && ${Me.AltAbilityReady[Radiant Cure]} && ${Spawn[pc ${_name}].Distance} < 100 && ${Group.Member[${_name}].ID}) {
				/call voodoo FALSE "Radiant Cure" alt 0 FALSE
				/delay 5
				/return
			}

			| AA Purify Soul
			| 200' 5 min
			/if (${Select[${Me.Class.ShortName},PAL,CLR,DRU]} && ${Me.AltAbilityReady[Purify Soul]} && ${Spawn[pc ${_name}].Distance} < 200) {
				/call voodoo FALSE "Purify Soul" ${Spawn[pc ${_name}].ID} FALSE
				/delay 5
			}
			
			| AA Group Purify Soul . were in deep shit if we've gotten this far.
			| 0/100' 15 min
			/if (${Select[${Me.Class.ShortName},CLR,DRU]} && ${Me.AltAbilityReady[Group Purify Soul]} && ${Spawn[pc ${_name}].Distance} < 100 && ${Group.Member[${_name}].ID}) {
				/call voodoo FALSE "Group Purify Soul" alt 0 FALSE
				/delay 5
				/return
			}
			
			| paladin splash 50' range
			/if (${Select[${Me.Class.ShortName},PAL]} && ${Spawn[pc ${_name}].Distance} < 50) {
				/call Bind_pal_splash
				/return
			}
		}

		| are they debuffed?
		/if (!${Bool[${${_name}_TotalCounters}]}) /continue
		DEBUG ${_name}_TotalCounters .. ${${_name}_TotalCounters}

		
		| is the DB set with the following cures to continue?
		/if (!${Bool[${maHeal.Find[stCureCurse].Value}]} && !${Bool[${maHeal.Find[stCureCorruption].Value}]} && !${Bool[${maHeal.Find[stCurePoison].Value}]} && !${Bool[${maHeal.Find[stCureDisease].Value}]}) /continue
		
		| cure with spell
		/if (${Bool[${maHeal.Find[stCureCurse].Value}]} && ${${_name}_CountersCurse}) {
			DEBUG ${sep}${_name} is CURSED... oh fuck..
			/if (${is_ready[FALSE, "${maHeal.Find[stCureCurse].Value}"]}) {
				/call voodoo FALSE "${maHeal.Find[stCureCurse].Value}" ${Spawn[pc ${_name}].ID} FALSE
			}
		}
		
		/if (${Bool[${maHeal.Find[stCureCorruption].Value}]} && ${${_name}_CountersCorruption}) {
			DEBUG ${sep}${_name} is CORRUPTED... can't trust that bitch..
			/if (${is_ready[FALSE, "${maHeal.Find[stCureCorruption].Value}"]}) {
				/call voodoo FALSE "${maHeal.Find[stCureCorruption].Value}" ${Spawn[pc ${_name}].ID} FALSE
			}
		}
		
		/if (${Bool[${maHeal.Find[stCurePoison].Value}]} && ${${_name}_CountersPoison}) {
			DEBUG ${sep}${_name} is POISONED... teach you to play with snakes
			/if (${is_ready[FALSE, "${maHeal.Find[stCurePoison].Value}"]}) {
				/call voodoo FALSE "${maHeal.Find[stCurePoison].Value}" ${Spawn[pc ${_name}].ID} FALSE
			}
		}
		
		/if (${Bool[${maHeal.Find[stCureDisease].Value}]} && ${${_name}_CountersDisease}) {
			DEBUG ${sep}${_name} is DISEASED...Burn'em with FIRE!!!
			/if (${is_ready[FALSE, "${maHeal.Find[stCureDisease].Value}"]}) {
				/call voodoo FALSE "${maHeal.Find[stCureDisease].Value}" ${Spawn[pc ${_name}].ID} FALSE
			}
		}

	/next _count
	
	SETHUD FALSE

/return



|***
 * note: dumb fucks always gotta put that red bouncy ball on my cursor
 * use: /call check_cursor DEBUG [ignoretimer T/F] [clear]
 ***|
sub check_cursor(bool _debug, bool _ignoreTimer, string _verbage)
	DEBUG \atcheck_cursor\ax(\a-w${_ignoreTimer}, ${_verbage}\ax)
	
	/if (!${Cursor.ID}) /return

	| force clearing the cursor and GTFO
	/if (${_verbage.Equal[CLEAR]} && ${Me.FreeInventory}) {
		DEBUG Force Inventory
		/while (${Cursor.ID} && ${Me.FreeInventory}) {
			/autoinventory
			/delay 2
		}
		/return TRUE
	}

	/if (!${_ignoreTimer} && ${timer_Check_Cursor}) /return
	ISMEDEAD

	| please bow your heads..  lsAutoInventory has entered the room
	/if (${lsAutoInventory.Contains[${Cursor.Name}]}) {
		/autoinventory 
		/return TRUE
	}

	/if (${maEnv.Find[swADVLoot].Value}) {
		/varset dump ${set_data_timer[FALSE, Check_Cursor, 5s]}
	}
	
	| allow for absence of override for not in same zone as master  looter
	/if (!${maOver.Find[swOverLoot].Value} && !${_ignoreTimer}) /return FALSE
	
	/if (!${Cursor.ID}) /return
	/declare _cursorChoice string local 
	
	
	/while (${Cursor.ID}) {
		| see if the item is in the database
		| if its not there. add it accordingly

		/if (!${Cursor.ID}) /break
		
		| check DB for the item
		/call set_loot_item ${_debug} "${Cursor.Name}"

		| reload data
		/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}" LIMIT 1
		
		| wtf we gotta do with the item?
		| do the things that we do best
		/if (${Select[${sqlite.Result[lootquery 1 action]},keep,sell]}) {
			/varset dump ${out[14, "Cursor${sep}${keep}${sep}${info}${Cursor.Name}\ax${sep}\a-w${FindItemCount[=${Cursor.Name}]}\ax"]}
			/nomodkey /autoinventory
			/delay 1
		} else /if (${Select[${sqlite.Result[lootquery 1 action]},destroy]}) {
			/varset dump ${out[14, "Cursor${sep}${destroy}${sep}${info}${Cursor.Name}\ax"]}
			/if (${sqlite.Result[lootquery 1 action].Equal[destroy]}) /nomodkey /destroy
			/delay 1
		} else /if (${Select[${sqlite.Result[lootquery 1 action]},leave,ignore]}) {
			/varset dump ${out[14, "Cursor${sep}${leave}${sep}${info}${Cursor.Name}\ax"]}
			/if (${sqlite.Result[lootquery 1 action].Equal[leave]}) /nomodkey /drop
			/delay 1
		} 
	}

/return



|***
 * note: will cast ePen pet against all raid leader instructions. because my DPS parse matters, not yours!
 * use: /call cast_temp_pet DEBUG [MOBID] [Temp Spell Name]
 ***|
sub cast_temp_pet( bool _debug, int _tmpID, string _tmpSpell)
	DEBUG \atcast_temp_pet\ax(\a-w${_tmpID}, ${_tmpSpell}\ax)
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT
	/if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE
	/if (${Target.PctHPs} > ${maMinion.Find[stPctSwarmEngage].Value}) /return FALSE
	
	/if (${is_ready[FALSE, "${_tmpSpell}"]}) {
		/call voodoo FALSE "${_tmpSpell}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: chopper.. sick balls!
 * use: /call send_pet DEBUG [TARGETID]
 ***| 
sub send_pet(bool _debug, int _tmpID)
	DEBUG \atsend_pet\ax(\a-w${_tmpID}\ax)

	/if (!${_tmpID}) /return FALSE
	/if (!AUTO && !${maMinion.Find[swonForce].Value}) /return FALSE
	/if (${Spawn[id ${_tmpID}].Distance} > ENV) /return FALSE
	/if (${Me.Pet.Target.ID} == ${_tmpID}) /return TRUE
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/varset dump ${set_target[${_tmpID}]}
	
	/if (PET) {
		/if (!${Me.Pet.ID} && ${Target.PctHPs} <= ${maMinion.Find[stPctPetEngage].Value}) {
			/varset dump ${out[10, "Sending minion${sep}\a-w${Spawn[${_tmpID}].DisplayName}\ax"]}
			/pet attack
		}
	}
	
	| send in the swarm
	/if (SWARM) {
		/if (${Target.PctHPs} <= ${maMinion.Find[stPctSwarmEngage].Value}) {
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
	
	| check distance or bail early
	/if (${Spawn[${_tmpID}].Distance} > ENV) {
		/varset dump ${set_data_timer[FALSE, Check_deBuffs, RESTART]}
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT
	CHECKTIE
	GETINPUT

	/varset dump ${set_target[${_tmpID}]}
	
	/declare _count int local 0
	/declare _mobCount int local 0
	/declare _tmpMobID int local
	/declare _tmpSpell string local
	
	| eradicate (dispell thingies)
	/if (${madeBuff.Find[swEradicate].Value}) {
		DEBUG ${sep}{madeBuff.Find[swEradicate].Value}${sep}\a-w${madeBuff.Find[swEradicate].Value}\ax

		| does this mob have a timer associated with it?
		| if there is no timer, create one with 1/10 second
		/if (!${Defined[timer_eradicate_ID${_tmpID}]}) {
			/varset dump ${set_timer[${_debug}, timer_eradicate_ID${_tmpID}, 1]}
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
			/if ((!AE# || (AE# && !${is_ready[${_debug}, "${madeBuff.Find[stAEEradicate].Value}"]}))  && ${is_ready[${_debug}, "${madeBuff.Find[stEradicate].Value}"]}) {
				DEBUG ${sep}{madeBuff.Find[swEradicate].Value}${sep}\a-wSINGLE\ax

				| try to eradicate it
				/call voodoo ${_debug} "${madeBuff.Find[stEradicate].Value}" ${_tmpID} FALSE

				| was the debuff cast successful?
				/if (${maData.Find[stCastReturn].Value.Equal[SUCCESS]}) {
					| throw it in out list if not there
					/if (!${lsdeBuffEradicate.Contains[${_tmpID}]}) {
						/varset dump ${lsdeBuffEradicate.Append[${_tmpID}]}
					}
					
					| set the re-check timer
					/varset dump ${set_timer[${_debug}, timer_eradicate_ID${_tmpID}, ${madeBuff.Find[stTimerEradicate].Value}]}
					
				| cast failure we try again in one tick
				} else /if (${maData.Find[stCastReturn].Value.NotEqual[SUCCESS]}) {
					/varset dump ${set_timer[FALSE, timer_eradicate_ID${_tmpID}, 6s]}
				}
				
			}
			
			| multi mob debuff
			/if (AE# && ${is_ready[${_debug}, "${madeBuff.Find[stAEEradicate].Value}"]} && (${_mobCount} >= AE#)) {
				DEBUG ${sep}{madeBuff.Find[swEradicate].Value}${sep}\a-wMULTI\ax
				
				| try to eradicate it
				/call voodoo ${_debug} "${madeBuff.Find[stAEEradicate].Value}" ${_tmpID} FALSE

				| was the debuff cast successful?
				/if (${maData.Find[stCastReturn].Value.Equal[SUCCESS]}) {

					| throw it in out list if not there
					/if (!${lsdeBuffEradicate.Contains[${_tmpID}]}) {
						/varset dump ${lsdeBuffEradicate.Append[${_tmpID}]}
					}

					| set the re-check timer
					/varset dump ${set_timer[${_debug}, timer_eradicate_ID${_tmpID}, ${madeBuff.Find[stTimerEradicate].Value}]}
						
					| get all his fucking friends too			
					/for _count 1 to ${_mobCount}
						/varset _tmpMobID ${Spawn[${_tmpID}].NearestSpawn[${_count} ,npc radius ${Spell[${madeBuff.Find[stAEEradicate].Value}].AERange}].ID}
						/if (${_tmpMobID} == 0) /break
					
						/if (!${lsdeBuffEradicate.Contains[${_tmpMobID}]}) {
							/varset dump ${lsdeBuffEradicate.Append[${_tmpMobID}]}
						}
						
						| bitches need timers
						/varset dump ${set_timer[${_debug}, timer_eradicate_ID${_tmpMobID}, ${madeBuff.Find[stTimerEradicate].Value}]}
						
					/next _count
					
				| cast failure we try again in one tick
				} else /if (${maData.Find[stCastReturn].Value.NotEqual[SUCCESS]}) {
					/varset dump ${set_timer[FALSE, timer_eradicate_ID${_tmpID}, 6s]}
				}

			}
			
		}
	| end of eradicate debuff checks
	}
	
	/declare _lidb listiterator local
	/declare _lsout list local
	/varset dump ${_lsout.Append[Slow,Snare,Tash,Cripple,Malo]}
	/vardata _lidb _lsout.First.Clone
	/declare _tmpTLO string local
	/while (!${_lidb.IsEnd}) {
		
		/if (${_lidb.Value.Equal[Slow]}) {
			/varset _tmpTLO Slowed
		} else /if (${_lidb.Value.Equal[Snare]}) {
			/varset _tmpTLO Snared
		} else /if (${_lidb.Value.Equal[Tash]}) {
			/varset _tmpTLO Tashed
		} else /if (${_lidb.Value.Equal[Cripple]}) {
			/varset _tmpTLO Crippled
		} else /if (${_lidb.Value.Equal[Malo]}) {
			/varset _tmpTLO Maloed
		}
		
		/if (${madeBuff.Find[sw${_lidb.Value}].Value}) {
			DEBUG ${sep}{madeBuff.Find[sw${_lidb.Value}].Value}${sep}\a-w${madeBuff.Find[sw${_lidb.Value}].Value}\ax

			/if (!${Defined[timer_${_lidb.Value.Lower}_ID${_tmpID}]}) {
				/varset dump ${set_timer[${_debug}, timer_${_lidb.Value.Lower}_ID${_tmpID}, 1]}
			} 
					
			| add if already there?
			/if ((${Target.${_tmpTLO}.ID} || ${Target.Buff[${madeBuff.Find[st${_lidb.Value}].Value}].ID} || ${Target.Buff[${madeBuff.Find[stAE${_lidb.Value}].Value}].ID}) && (!${timer_${_lidb.Value.Lower}_ID${_tmpID}} || !${lsdeBuff${_lidb.Value}.Contains[${_tmpID}]})) {
				/if (!${lsdeBuff${_lidb.Value}.Contains[${_tmpID}]}) {
					/varset dump ${lsdeBuff${_lidb.Value}.Append[${_tmpID}]}
				}
				/if (${Target.${_tmpTLO}.Duration.TotalSeconds}) {
					/varset dump ${set_timer[${_debug}, timer_${_lidb.Value.Lower}_ID${_tmpID}, ${Target.${_tmpTLO}.Duration.TotalSeconds}]}			
				}
			}
				
			/if (!${timer_${_lidb.Value.Lower}_ID${_tmpID}} || !${lsdeBuff${_lidb.Value}.Contains[${_tmpID}]}) {
				DEBUG NO timer or not in list
				/if (${Bool[${madeBuff.Find[stAE${_lidb.Value}].Value}]} && AE#) {
					/varset _mobCount ${SpawnCount[npc loc ${Spawn[id ${_tmpID}].X} ${Spawn[id ${_tmpID}].Y} radius ${Spell[${madeBuff.Find[stAE${_lidb.Value}].Value}].AERange}]} 
				} else {
					/varset _mobCount 0
				}
				
				DEBUG _mobCount${sep}\a-w${_mobCount}\ax
				/if ((!AE# || (AE# && !${is_ready[${_debug}, "${madeBuff.Find[stAE${_lidb.Value}].Value}"]})) && ${is_ready[${_debug}, "${madeBuff.Find[st${_lidb.Value}].Value}"]}) {
					DEBUG ${sep}{madeBuff.Find[sw${_lidb.Value}].Value}${sep}\a-wSINGLE\ax
					/call voodoo ${_debug} "${madeBuff.Find[st${_lidb.Value}].Value}" ${_tmpID} FALSE
					/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
						DEBUG	lsdeBuff${_lidb.Value}.Contains[${_tmpID}]
						/if (!${lsdeBuff${_lidb.Value}.Contains[${_tmpID}]}) {
							/varset dump ${lsdeBuff${_lidb.Value}.Append[${_tmpID}]}
						}
						/varset dump ${set_timer[${_debug}, timer_${_lidb.Value.Lower}_ID${_tmpID}, ${Spell[${madeBuff.Find[st${_lidb.Value}].Value}].Duration.TotalSeconds}]}
					} else /if (${Select[${maData.Find[stCastReturn].Value},RESISTED,NOTREADY,FIZZLE,DISTRACTED]}) {
						/varset dump ${set_timer[FALSE, timer_${_lidb.Value.Lower}_ID${_tmpID}, 6s]}
					} else /if (${Select[${maData.Find[stCastReturn].Value},IMMUNE,TAKEHOLD]}) {
						/if (!${lsdeBuff${_lidb.Value}.Contains[${_tmpID}]}) {
							/varset dump ${lsdeBuff${_lidb.Value}.Append[${_tmpID}]}
						}
						/varset dump ${set_timer[${_debug}, timer_${_lidb.Value.Lower}_ID${_tmpID}, 10m]}
					}
					/varset dump ${_lidb.Advance}
					/continue
				}
				
				/if (AE# && ${is_ready[${_debug}, "${madeBuff.Find[stAE${_lidb.Value}].Value}"]} && (${_mobCount} >= AE#)) {
					DEBUG ${sep}{madeBuff.Find[sw${_lidb.Value}].Value}${sep}\a-wMULTI\ax
					/call voodoo ${_debug} "${madeBuff.Find[stAE${_lidb.Value}].Value}" ${_tmpID} FALSE
					/if (${maData.Find[stCastReturn].Value.Equal[SUCCESS]}) {
						/if (!${lsdeBuff${_lidb.Value}.Contains[${_tmpID}]}) {
							/varset dump ${lsdeBuff${_lidb.Value}.Append[${_tmpID}]}
						}
						/varset dump ${set_timer[${_debug}, timer_${_lidb.Value.Lower}_ID${_tmpID}, ${Spell[${madeBuff.Find[stAE${_lidb.Value}].Value}].Duration.TotalSeconds}]}
						/for _count 1 to ${_mobCount}
							/varset _tmpMobID ${Spawn[${_tmpID}].NearestSpawn[${_count} ,npc radius ${Spell[${madeBuff.Find[stAE${_lidb.Value}].Value}].AERange}].ID}
							/if (${_tmpMobID} == 0) /break
							/if (!${lsdeBuff${_lidb.Value}.Contains[${_tmpMobID}]}) {
								/varset dump ${lsdeBuff${_lidb.Value}.Append[${_tmpMobID}]}
							}
							/varset dump ${set_timer[${_debug}, timer_${_lidb.Value.Lower}_ID${_tmpMobID}, ${madeBuff.Find[stTimerSnare].Value}]}
						/next _count
					} else /if (${Select[${maData.Find[stCastReturn].Value},RESISTED,NOTREADY,FIZZLE,DISTRACTED]}) {
						/varset dump ${set_timer[FALSE, timer_${_lidb.Value.Lower}_ID${_tmpID}, 6s]}
					} else /if (${Select[${maData.Find[stCastReturn].Value},IMMUNE,TAKEHOLD]}) {
						/if (!${lsdeBuff${_lidb.Value}.Contains[${_tmpID}]}) {
							/varset dump ${lsdeBuff${_lidb.Value}.Append[${_tmpID}]}
						}
						/varset dump ${set_timer[${_debug}, timer_${_lidb.Value.Lower}_ID${_tmpID}, 10m]}
					}
					/varset dump ${_lidb.Advance}
					/continue
				}
			}
		}

		/varset dump ${_lidb.Advance}
	}		
	
/return
	
	

|***
 * note: pets all sorts of fucked up again..
 * use: /call check_minion_cycle DEBUG <FORCE>
 ***|
sub check_minion_cycle(bool _debug, bool _force)
	DEBUG \atcheck_minion_cycle\ax(\a-w${_force}\ax)
	
	/if (AUTO) /varset dump ${set_data_timer[${_debug}, Check_Minion_Cycle, RESTART]}

	/if (${_force}) {
		/declare _buffcycletimestart int local ${MacroQuest.Running}
		/if (!${maMinion.Find[swPet].Value}) {
			/varset dump ${out[0, "Master Minion Switch${sep}${off}"]}
		} else {
			/varset dump ${out[0, "Minion Cycle${sep}\a-wForced\ax"]}
		}
	}
		
	| summon minion
	/if (!${Pet.ID}) /call check_minion_cycle_summon TRUE 
	
	| buffs
	/if (${maMinion.Find[swPetBuff].Value} && (!${timer_Check_Minion_Buff} || ${_force})) /call check_minion_cycle_buff ${_debug} 

	| get weapons?
	/if (${maMinion.Find[swUsePetGear].Value}) /call check_minion_cycle_weapons ${_debug}

	/if (${Pet.ID} && ${Bool[${maMinion.Find[stPetIllusion].Value}]}) /call check_minion_cycle_Illusion ${_debug}
		
	| shrink
	/if (${maMinion.Find[swPetShrink].Value} && (!${timer_Check_Minion_Shrink} || ${_force})) /call check_minion_cycle_shrink ${_debug} 


	/if (AUTO) /varset dump ${set_data_timer[${_debug}, Check_Minion_Cycle, RESTART]}

	/if (${_force}) {
		/if (${maMinion.Find[swPet].Value}) {
			/varset dump ${out[0, "Minion Cycle${sep}\a-wCompleted\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_buffcycletimestart}) / 10) / 60]}s\ax"]}
		}
	}

/return
	
	
	
|***
 * note: minime??
 * use: /call check_minion_cycle_shrink DEBUG
 ***|
sub check_minion_cycle_shrink(bool _debug)
	DEBUG \atcheck_minion_cycle_shrink\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/varset dump ${set_data_timer[FALSE, Check_Minion_Shrink, RESTART]}

	/if (${Pet.Height} < 2) /return TRUE

	/if (!${is_ready[${_debug}, "${maMinion.Find[stPetShrink].Value}"]}) /return FALSE
	
	/declare _count int local 0

	| me shrinks
	/while (${Pet.Height} > 2 && ${_count} < 2) {
		/call voodoo ${_debug} "${maMinion.Find[stPetShrink].Value}" ${Pet.ID} FALSE
		/varcalc _count ${_count}+1
	}
	
/return
	
	
	
|***
 * note: buffs your pet
 * use: part of standard minion cycle if enabled
 ***|
sub check_minion_cycle_buff(bool _debug)
	DEBUG \atcheck_minion_cycle_buff\ax()
	
	/if (AUTO) /varset dump ${set_data_timer[${_debug}, Check_Minion_Buff, RESTART]}
	
	/if (!${Pet.ID}) /return FALSE

	/declare _spell string local FALSE
	/declare _count int local 0

	DEBUG ${sep}Buff${sep}\a-wMinion\ax

	| maybe 50 buffs was a bit much.
	/for _count 1 to 50
		
		/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
		/if (!${Bool[${maBuffs.Find[stBuff${_count}].Value}]}) /continue

		| minion buffs?		
		/if (!${lsBuff${_count}Tag.Contains[minion]}) /continue
		
		/if (!${check_basic_tags[FALSE, Buff${_count}, FALSE]}) /continue			

		DEBUG _count${sep}\a-w${_count}
		
		/varset _spell ${set_rank[FALSE, "${maBuffs.Find[stBuff${_count}].Value.Arg[1,+]}"]}
		DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax
		
		| distance check
		/if (${Pet.Distance} > ${Spell[${_spell}].MyRange}) /continue

		| target the group member
		/varset dump ${set_target[${Pet.ID}]}

		| have the buff?
		/if (${Pet.Buff[${maBuffs.Find[stBuff${_count}].Value}]}) /continue

		| give the game a second to populate the targets buffs
		/delay 1s ${Target.BuffsPopulated}
		
		| stacks
		/if (!${Spell[${_spell}].StacksTarget}) /continue
				
		| /if (${Target.ID} == ${Pet.ID}) {
			/call voodoo ${_debug} "${maBuffs.Find[stBuff${_count}].Value}" ${Pet.ID} FALSE		
			/continue
		| }	

	/next _count

	/if (${Target.ID}) /squelch /target clear
	
/return



|***
 * note: summons your girl friend..
 * use: /call check_minion_cycle_summon DEBUG
 ***|
sub check_minion_cycle_summon(bool _debug)
	DEBUG \atcheck_minion_cycle_summon\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/if (PET && AUTO && !${Pet.ID}) {
		/if (${is_ready[FALSE, "Suspended Minion"]}) {
			/call voodoo FALSE "Suspended Minion" 0 FALSE
		}
	}
	
	| component check. cause `yo ho needs an allowance
	/if (PET && !${Pet.ID}) {
		/declare _error bool local FALSE
		/if (${check_class[FALSE, |ENC]} && !${FindItemCount[=Tiny Dagger]}) /varset _error TRUE
		/if (${check_class[FALSE, |MAG]} && !${FindItemCount[=Malachite]}) /varset _error TRUE
		/if (${check_class[FALSE, |NEC|SHD]} && !${FindItemCount[=Bone Chips]}) /varset _error TRUE
		
		/if (${_error}) {
			/varset dump ${out[0, "Missing pet component."]}
			/varset dump ${out[0, "Shutting pets off 'till you go buy some."]}
			/varset dump ${out[0, "Don't forget to turn back on."]}
			/call set_switch pet swPet FALSE
			/return FALSE
		}
	}

	/varset dump ${out[0, "Summoning${sep}\a-w${maMinion.Find[stPetType].Value}\ax"]}
	/call voodoo FALSE "${maMinion.Find[stPetType].Value}" 0 FALSE
	/delay 15s !${Me.Casting.ID}
	/pet hold on
	/pet ghold on
	
	/if (AUTO) /varset dump ${set_data_timer[FALSE, Check_Minion_Buff, 1]}
	
/return



|***
 * note: pig stickers for your ho
 * use: /call check_minion_cycle_weapons DEBUG
 ***|
sub check_minion_cycle_weapons(bool _debug)
	DEBUG \atcheck_minion_cycle_weapons\ax()


	ISMEDEAD
	CHECKEXIT
	GETINPUT

	| no pets!!
	/if (!PET || !${Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) /return FALSE
	

	| pet has weapons
	/if ((${Pet.Equipment[primary]} && ${Me.Pet.Equipment[primary]} != 10855) && ${Pet.Equipment[offhand]}) /return TRUE

	| if you are not a mage, you should not be here
	/if (${Select[${Me.Class.ShortName},MAG]} && !${FindItemCount[=${get_MAG_folded_pack[]}]}) {

		/delay 4s !${Me.SpellInCooldown}

		/declare _count int local 0
		/for _count 1 to 50
			/if (!${maBuffs.Find[swBuff${_count}].Value}) /continue
			/if (!${Bool[${maBuffs.Find[stBuff${_count}].Value}]}) /continue
			/if (!${lsBuff${_count}Tag.Contains[summonweapons]}) /continue
			DEBUG _count${sep}${_count}
			
			/if (${FindItem[=${Spell[${maBuffs.Find[stBuff${_count}].Value}].Base[1]}].ID}) /return TRUE
			
			| /if (${is_ready[FALSE, "${maBuffs.Find[stBuff${_count}].Value}"]}) {
				/call voodoo FALSE "${maBuffs.Find[stBuff${_count}].Value}" ${Me.ID} FALSE
			| }
			/delay 10s !${Me.Casting.ID}

			/if (${Cursor.Name.Equal[${FindItem[${Spell[${maBuffs.Find[stBuff${_count}].Value}].Base[1]}].Name}]}) {
				/autoinventory
				/break
			}

		/next _count	
	}

	/if (${Pet.ID}) /call get_minion_summon_weapons FALSE

/return








${FindItem[=${Spell[${maBuffs.Find[stBuff11].Value}].Base[1]}].ID}
${lsBuff11Tag.Contains[summonweapons]}



${FindItemCount[=${Spell[${maBuffs.Find[stBuff11].Value}].Base[1]}]}
${FindItem[${Spell[${maBuffs.Find[stBuff11].Value}].Base[1]}].Name}




${Spell[${maBuffs.Find[stBuff11].Value}].Base[1]}



${Pet.Equipment[primary]}











|***
 * note: lipstick on a pig is still a fucking pig
 * use: /call check_minion_cycle_Illusion DEBUG
 ***|
sub check_minion_cycle_Illusion(bool _debug)
	DEBUG \atcheck_minion_cycle_Illusion\ax()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	| pet has illusion
	/if (${Pet.Buff[${Spell[${FindItem[${maMinion.Find[stPetIllusion].Value}].Clicky.SpellID}].Name}]}) /return TRUE
	
	
	| chekck for pet illusion
	/if (${is_ready[FALSE, "${maMinion.Find[stPetIllusion].Value}"]}) {
		/call voodoo FALSE "${maMinion.Find[stPetIllusion].Value}" ${Pet.ID} FALSE
	}

/return









|***
 * note: returns a non container inventory slot ##, 0 on full bag inventory, you can pass an optional name to find an item in a top slot
 * use: ${get_openPackSlot[DEBUG, [itemname]]}
 ***|
sub get_openPackSlot(bool _debug, string _orFind)
	DEBUG \atget_openPackSlot\ax(\a-w${_orFind}\ax)

	/declare _count int local 0

	/for _count 23 to 32
		/if (${InvSlot[${_count}].Item.Container} == 0 || ${InvSlot[${_count}].Item.Name.Equal[${_orFind}]}) {
			/return ${_count}
		}
	/next _count

/return 0



|***
 * note: give weapons to pet
 * use: /call give_to_pet "ITEMNAME"
 ***|
sub give_to_pet(string _item, bool _debug)
	DEBUG give_to_pet(${_item})

	/varset dump ${set_target[${Pet.ID}]}
	
	ISMEDEAD
	GETINPUT
	/if (${Cursor.ID} && ${Me.Pet.ID}) {
		/if (${Cursor.Name.Equal[${_item}]}) {
			/click left target
			/delay 5s !${Bool[${Cursor.ID}]}
		} else {
			/call check_cursor FALSE TRUE CLEAR
		}
	}
	
/return



|***
 * note: equip pet weapons
 * use: /call get_minion_summon_weapons DEBUG
 ***|
sub get_minion_summon_weapons(bool _debug)
	DEBUG \atget_minion_summon_weapons\ax()

	/if (!PET || !${Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) /return FALSE

	/if (!${Me.FreeInventory}) {
		/varset dump ${out[0, "${notice} ${sep} You have no free inventory. Damn hoarders...."]}
		/return FALSE
	}

	CHECKTIE
	ISMEDEAD
	GETINPUT

	/declare _count int local 0
	/declare _openPackSlot int local 0
	
	| get everything out of your hands, including your dick
	/call check_cursor FALSE TRUE CLEAR

	| need a non bag inventory slot or ur fuckled!
	/varset _openPackSlot ${get_openPackSlot[FALSE, "Pouch of Quellious"]}
	DEBUG _openPackSlot ${_openPackSlot}
	/if (!${_openPackSlot} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {
		/varset dump ${out[0, "${notice} ${sep} You have no TOP inventory slots that are not bags."]}
		/varset dump ${out[0, "Clean up your damn inventory then you can give your pet some weapons."]}
		/return FALSE
	}
	
	| we have the weapons we can rebuild her better then before
	/if (${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]} > 1) /goto :havepetweapons
	:havepouchandnoweapons
	| still have an old bag. it needs to be gone
	/if (${FindItemCount[=Pouch of Quellious]} && ${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]} < 2) {
		/call item_delete FALSE "Pouch of Quellious"
	}
	
	| how much for a dime?
	/if (!${FindItemCount[=${get_MAG_folded_pack[]}]} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {
		/return FALSE
	}
		
	| have bag/not opened
	/call check_cursor FALSE TRUE CLEAR
		
	/if (${FindItemCount[=${get_MAG_folded_pack[]}]}) {
		DEBUG :havemageclosedbag
		/if (${Select[${FindItem[=${get_MAG_folded_pack[]}].InvSlot},23,24,25,26,27,28,29,30,31,32]} != 0) {
			DEBUG \agSWAP\ax ${get_MAG_folded_pack[]} to slot \aw${_openPackSlot}\ax
			/call Bind_command_swap "${get_MAG_folded_pack[]}" ${_openPackSlot} FALSE
		}

		/call check_cursor FALSE TRUE CLEAR

		| convert the folded pack
		/if (${is_ready[FALSE, "${get_MAG_folded_pack[]}"]}) {
			/call voodoo FALSE "${get_MAG_folded_pack[]}" 0 FALSE
		}

		/call check_cursor FALSE TRUE CLEAR

	}	
	
	
	/call check_cursor FALSE TRUE CLEAR
	
	| have 2 blades
	:havepetweapons
	DEBUG :havepetweapons
	/if (${Cursor.Name.Equal[Pouch of Quellious]}) /autoinventory
	/for _count 1 to 2
		/call check_cursor FALSE TRUE CLEAR
		/delay 2
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

	/call check_cursor FALSE TRUE CLEAR

	:havepouchandnoweapons
	DEBUG :havepouchandnoweapons
	/if (${FindItemCount[=Pouch of Quellious]} && ${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]} < 2) {
		/nomodkey /itemnotify ${FindItem[Pouch of Quellious].InvSlot} leftmouseup
		/delay 1
		/call item_delete FALSE "Pouch of Quellious"

	}

	/if (${Cursor.ID}) /call check_cursor FALSE TRUE CLEAR

	/if (${Window[InventoryWindow].Open}) /invoke ${Window[InventoryWindow].DoClose}
	
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
		/varset dump ${out[14, "Destroying ${sep} ${tar}${_item}\ax"]}
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
 * use: ${get_MAG_folded_packDEBUG}
 ***|	
sub get_MAG_folded_pack(bool _debug)
	DEBUG \atget_MAG_folded_pack\ax()

	/if (!${Defined[stMAGFoldedPack]}) {
		/declare stMAGFoldedPack string outer
	}

	/declare _lsout list local
	/varset dump ${_lsout.Append[Wirn's,Thassis',Frightforged,Manaforged,Spectral]}
	/devlare _ligmfp listiterator local
	/vardata _ligmfp _lsout.First.Clone
	/while (!${_ligmfp.IsEnd}) {
		/if (${FindItemCount[Folded Pack of ${_ligmfp.Value} Armaments]}) {
			/varset stMAGFoldedPack Folded Pack of ${_ligmfp.Value} Armaments
		}
		/varset dump ${_ligmfp.Advance}
	}	

/return ${stMAGFoldedPack}



|***
 * note: this is used to cycle the control map and handle the IDs in it. final return will be an unmezzed mob ID, if any
 * use: /call check_control_routine DEBUG
 ***|
sub control_routine(bool _debug)
	| /varset _debug TRUE
	DEBUG \atcontrol_routine\ax()

	/if (!AUTO) /return FALSE
	/if (!${madeBuff.Find[swMez].Value}) /return FALSE
	/if (!${maCC.Find[swCombatControl].Value}) /return FALSE


	| ID for unmezzed/short mob
	/declare _mezzID int local 0
	| count of uncontrolled mobs in area
	/declare _countArea int local 0
	
	/declare _count int local 0
	
	/if (!${Defined[maMobControl]}) {
		/declare maMobControl map outer
	}

	| check control map for any mobs dead or gone
	/varset dump ${control_map[${_debug}, dead]}

	| find unaccounted mobs in my area
	| returns the # of mobs that do not have a current mez timer associated
	/varset _countArea ${control_area[${_debug}]}
	
	| if ${check_control_area returns more then 1
	| hit the AE routines
	| /if (${_countArea}) {
		/call control_mezae ${_debug}
	| }
	
	| check control map for any mobs marked as mezed with low or no timer
	/varset _mezzID ${control_map[${_debug}, mez]}

	
	| mez something for christs sake...
	/if (${_mezzID} && ${_countArea} && ${Bool[${madeBuff.Find[stMez].Value}]}) {
		/for _count 1 to ${_countArea}
			/varset _mezzID ${control_map[${_debug}, mez]}
			/call control_mez FALSE ${_mezzID}
		/next _count
	}
	
	/delay 1
	
/return TRUE



|***
 * note: this is used to check our current ENV and find mobs that we do not know about that are pissed at us
 * use: ${control_areaDEBUG}
 ***|
sub control_area(bool _debug)
	DEBUG \atcontrol_area\ax()

	/declare _agroCount int local 0
	/declare _count int local 0
	/declare _tmpID int local
	
	/for _count 1 to ${SpawnCount[npc los range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ENV zradius ${maCC.Find[stZRadius].Value} PS4]}
		/varset _tmpID ${NearestSpawn[${_count}, npc los range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ENV zradius ${maCC.Find[stZRadius].Value} PS4].ID}

		| do we have the mob in the mez list ??
		/if (${maMobControl.Contains[${_tmpID}]}) {
			/continue
		}
	
		| if this mob is the current assists target, remove it from our need to control
		/if (${_tmpID} == TARGET) {
			/if (${maMobControl.Contains[${_tmpID}]}) {
				/varset dump ${maMobControl.Remove[${_tmpID}]}
				
				| delete the existing mez timer if any?
				/if (${Defined[timer_mez_ID${mi.Key}]}) {
					/deletevar timer_mez_ID${mi.Key}
				}				
				
			}
			/continue
		}
		
		| mob is not in mez list and not in agro list. fresh meat...
		/if (!${maMobControl.Contains[${_tmpID}]}) {
			
			| identify mobs we dont touch
			/if (${lsZoneNoMez.Contains[${Spawn[${_tmpID}].DisplayName}]} || ${lsZoneExclude.Contains[${Spawn[${_tmpID}].DisplayName}]}) {
				/varset dump ${maMobControl.Add[${_tmpID},EXEMPT]}
			
			| mobs that we are to charm
			} else /if (${lsZoneCharm.Contains[${Spawn[${_tmpID}].DisplayName}]} && ${maChr.Find[swCharm].Value}) {
					/varset dump ${maMobControl.Add[${_tmpID},CHARM]}
			
			| mobs that we are to mez
			| create the mez timer with the ID
			} else { 
				/if (!${Defined[timer_mez_ID${_tmpID}]}) {
					/varset dump ${set_timer[FALSE, timer_mez_ID${_tmpID}, 1]}
					/varset dump ${maMobControl.Add[${_tmpID},MEZ]}
					/varcalc _agroCount ${_agroCount} + 1
				} 
					
			}
		}

	/next _count
	
/return ${_agroCount}



|***
 * note: cheeck the IDs in the passed list. remove them from the list if they are dead/gone
 * use: ${control_map[DEBUG, THINGIETOCHECK??]}
 * 
 * THINGIETOCHECK:
 * dead = check for dead or no more spawns. removes them from the control map
 * mez = find mobs that are not mezed. returns ID of nonmezed or short duration remaining mezed mob
 ***|
sub control_map(bool _debug, string _type)
	DEBUG \atcontrol_map\ax(\a-w${_type}\ax)
	
	/declare _returnCount int local 0
	
	| check list for dead mobs and get them gone
	/if (${_type.Equal[dead]}) {
	
		/declare maRemoveals list
		/declare _remcount int local 0
		/declare _count int local 0

		/vardata mi maMobControl.First.Clone
		/varset dump ${maRemoveals.Clear}
		/while (!${mi.IsEnd}) {
			DEBUG ${sep}mi.Key--.Value${sep}\a-w${mi.Key} -- ${mi.Value}\ax
			/if (!${Spawn[${mi.Key}].ID} || ${Spawn[${mi.Key}].Dead}) {
				DEBUG ${sep}removing${sep}\a-w${mi.Key}\ax

				| delete the existing mez timer if any?
				/if (${Defined[timer_mez_ID${mi.Key}]}) {
					/deletevar timer_mez_ID${mi.Key}
				}

				| Add the key to the list of items to remove.
				/varset dump ${maRemoveals.Append[${mi.Key}]}
			}
			/varset dump ${mi.Advance}
		}

		/varcalc _remcount ${maRemoveals.Count}

		/for _count 1 to ${_remcount}
			/varset dump ${maMobControl.Remove[${maRemoveals.Head}]}
		/next _count
		
	}
	
	| find a non mezed mob or mob with a short duration mez left. return its ID if any for cc
	/if (${_type.Equal[mez]}) {
		DEBUG ${sep}cc${sep}
		/vardata mi maMobControl.First.Clone
		/while (!${mi.IsEnd}) {
			DEBUG ${sep}mi.Key--.Value${sep}\a-w${mi.Key} -- ${mi.Value}\ax
			/if (${Bool[${maCC.Find[stAssistMode].Value}]} && ${mi.Key} == ${Me.XTarget[1].ID}) {
				/varset dump ${mi.Advance}
				/continue
			}
			
			/if (${mi.Value.Equal[MEZ]} && ${Defined[timer_mez_ID${mi.Key}]}) {
				
				| if the timer is < 6 seconds. remezz
				/if (${timer_mez_ID${mi.Key}} < 60) {
					/return ${mi.Key}
				}

			}
			/varset dump ${mi.Advance}
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
	GETINPUT
	/declare _tmpID int local

	/if (${maMobControl.Count} >= 1 && ${is_ready[FALSE, "${madeBuff.Find[stAEMez].Value}"]}) {
		/vardata _tmpID maMobControl.First

		/if (AE# && ${SpawnCount[npc los radius ${Spell[${madeBuff.Find[stAEMez].Value}].AERange} targetable]} >= AE#) {
	
			/if (${Me.Gem[${Spell[${maChr.Find[stAEStun].Value}].RankName}]}) {
				/if (${Me.Casting.Name.NotEqual[${Spell[${maChr.Find[stAEStun].Value}].RankName}]}) /stopcast
				/delay 2.25s ${Me.SpellReady[${Spell[${maChr.Find[stAEStun].Value}].RankName}]}
				/call voodoo FALSE "${maChr.Find[stAEStun].Value}" 0 FALSE
			}

			/if (${Me.Gem[${Spell[${madeBuff.Find[stAEMez].Value}].RankName}]}) {
				/if (${Me.Casting.Name.NotEqual[${Spell[${madeBuff.Find[stAEMez].Value}].RankName}]}) /stopcast
				/delay 2.25s ${Me.SpellReady[${Spell[${madeBuff.Find[stAEMez].Value}].RankName}]}
				/call voodoo FALSE "${madeBuff.Find[stAEMez].Value}" 0 FALSE
			}

		}
		
		| use AA beam of slumber?
		/if (${maChr.Find[swAABoS].Value}) {
			/if (${Spawn[${_tmpID}].NearestSpawn[3,npc radius 30].ID} && ${Spawn[${_tmpID}].Distance3D} <= 200 && ${Spawn[${_tmpID}].Distance3D} >= ENV*.5) {
				/if (${is_ready[FALSE, "Beam of Slumber"]}) {
					/if (${Me.Casting.ID}) /stopcast
					/face ID ${_tmpID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
					/call voodoo FALSE "Beam of Slumber" 0 FALSE
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
	
	/if (${timer_mez_ID${_tmpID}} > 60) /return
	
	/declare _resistCount int local 0

	| try to mez more then once if there is a fail
	/while (${_resistCount} < 4) {
	
		/varset dump ${set_target[${_tmpID}]}
		/if (${_tmpID} == TARGET) /return FALSE

		ISMEDEAD
		GETINPUT
		
		/if (!${Target.LineOfSight}) /return FALSE
		
		/if (${Target.Mezzed.Duration} && ${Spell[${Target.Mezzed}].Name.Find[${madeBuff.Find[stMez].Value}]}) {
			/call set_timer timer_mez_ID${_tmpID} ${Target.Mezzed.Duration.TotalSeconds}s
			/return TRUE
		} 
		
		| mez ready?
		/delay 2.25s ${Me.SpellReady[${Spell[${madeBuff.Find[stMez].Value}].RankName}]}
		/call voodoo FALSE "${madeBuff.Find[stMez].Value}" ${_tmpID} FALSE

		/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,IMMUNE,TAKEHOLD]}) {

			/if (${Select[${maData.Find[stCastReturn].Value},IMMUNE,TAKEHOLD]}) {
				| auto add to no mez list for zone
				/if (${maEntropy.Find[swAutoList].Value} && ${maData.Find[stCastReturn].Value.Equal[IMMUNE]}) {
					/sqlite ${maData.Find[DBzone].Value} dummy INSERT INTO zone_control VALUES ("${Zone.ShortName}", "lsZoneNoMez", "${Target.DisplayName}")
					/varset dump ${out[15, "/area nomez ${dot} ${num}lsZoneNoMez\ax${sep}\a-w${Target.DisplayName}\ax"]}
					/call set_zone_lists FALSE
				}
				
				| were not going to try this target again, clear its current control settings
			  /varset dump ${maMobControl.Add[${_tmpID},EXEMPT]}
			  /if (${Defined[timer_mez_ID${_tmpID}]}) {
			  	/deletevar timer_mez_ID${_tmpID}
			  	/return FALSE
			  }
			  
			}

			| /call set_timer timer_mez_ID${_tmpID} ${Spell[${madeBuff.Find[stMez].Value}].Duration.Seconds}s
			/varset dump ${set_timer[FALSE, timer_mez_ID${_tmpID}, ${Spell[${madeBuff.Find[stMez].Value}].Duration.Seconds}s]}

			/return TRUE
		
		} else /if (${Select[${maData.Find[stCastReturn].Value},RESISTED,RESISTEDYOU]}) {
			/varcalc _resistCount ${_resistCount} + 1
			/if (${_resistCount} >= 1) {
				/if (${is_ready[FALSE, "${madeBuff.Find[stTash].Value}"]}) {
					/call voodoo FALSE "${madeBuff.Find[stTash].Value}" ${_tmpID} FALSE
				}
			}
		}
	}

/return



|***
 * note: active Shield Flash when HP reach a specific % or a number of mobs in immediate range
 * use: /call cast_shield_flash DEBUG [aa|disc]
 ***|
sub cast_shield_flash(bool _debug, string _type)
	DEBUG \atcast_shield_flash\ax(\a-w${_type}\ax)
	/if (!AUTO) /return

	| health low enough || enough mobs in range
	/if (${Me.PctHPs} > ${maChr.Find[stPctShieldFlash].Value} && ${SpawnCount[npc radius SAFEENV PS4 targetable]} < ${maChr.Find[stCountShieldFlash].Value}) /return FALSE

	| is the skill available
	/if (${_type.Equal[aa]}) { 
		/if (!${is_ready[FALSE, "Shield Flash"]}) /return FALSE
	} else /if (${_type.Equal[disc]}) {
		/if (!${is_ready[FALSE, "${maChr.Find[stFlash].Value}"]}) /return FALSE
	}
	
	| do not use while another defensive line is running
	/if (${is_defensive_active[FALSE, FALSE, SAFEENV]}) /return FALSE
	
	
	| mark current bandolier and swap to flash
	/declare _Bandolier string local FALSE

	| take note of current Bandolier	
	/if (!${Me.Bandolier[Flash].Active}) {
		/declare _count int local 0

		/for _count 1 to 8
			/if (${Me.Bandolier[${_count}].Active}) {
				/varset _Bandolier ${Me.Bandolier[${_count}]}
				/break
			}
		/next _count
	}
	
	
	| pally / shadowknight
	/if (${_type.Equal[aa]}) { 
		/call check_facing FALSE TARGET FORCE
		/invoke ${Me.Bandolier[Flash].Activate}
		/call voodoo FALSE "Shield Flash" 0 FALSE
		
	| warior
	} else /if (${_type.Equal[disc]}) {
		/call check_facing FALSE TARGET FORCE
		/invoke ${Me.Bandolier[Flash].Activate}
		/call voodoo FALSE "${maChr.Find[stFlash].Value}" 0 FALSE

	}

	| reset to previous bandolier
	/if (${Me.Bandolier[Flash].Name.NotEqual[${_Bandolier}]}) {
		/docommand /timed 70 /bandolier Activate ${_Bandolier}
	}

/return





|***
 * note: swaps bandolier
 *	USE: /call set_bandolier DEBUG [1h|2h|range|no]
 ***|
sub set_bandolier(bool _debug)
	DEBUG \atset_bandolier\ax()
	
	/if (${bandolierSwap.Equal[${bandolier${_band}}]}) /return TRUE
	/if (!${swBandolier}) /return FALSE
	/if (!${Bool[${bandolierNormal}]}) /return FALSE
	/if (!${Bool[${bandolier${_band}}]}) /return FALSE
	/delay 1
	/bandolier Activate ${bandolier${_band}}
	/varset bandolierSwap ${bandolier${_band}}

/return TRUE


${Me.Bandolier.Count}


1h|2h|range|no


Sword Board (Defensive Proficiency II)

Dual-Wielding Proficiency II
Two-Handed Proficiency II

${Me.Bandolier.Name}
${Me.Bandolier}

${Me.Bandolier.Active}
${Me.Bandolier.ID}

${Me.Bandolier[]}

${Me.Bandolier[1H].Active}

${Me.Bandolier[1]}

/invoke ${Me.Bandolier[1H].Activate}



${Me.Bandolier[1].Active}


${Me.Bandolier[1].Name}






|***
 * note: will use a glyph and buy it if you have the aa
 * use: /call cast_glyph DEBUG
 ***|
sub cast_glyph(bool _debug)
	DEBUG \atcast_glyph\ax()
	
	/if (${maCC.Find[stGlyph].Value.Equal[FALSE]}) /return FALSE
	
	/if (${Raid.Members}) {
		/if (!${maCC.Find[swGlyphRaid].Value}) /return FALSE
	} else /if (${Group.Members}) {
		/if (!${maCC.Find[swGlyphGroup].Value}) /return FALSE
	}
	
	ISMEDEAD
	GETINPUT
	
	| buy the AA
	/if (!${Me.AltAbility[${maCC.Find[stGlyph].Value}].ID} && ${Me.AAPoints} >= ${AltAbility[${maCC.Find[stGlyph].Value}].Cost}) {
		/alt buy ${AltAbility[${maCC.Find[stGlyph].Value}].ID}
	}
	
	/if (${is_ready[FALSE, "${maCC.Find[stGlyph].Value}"]}) {
		/call core_cast2 "${maCC.Find[stGlyph].Value}" 0 FALSE
	}
	
	| buy it again if you can
	/if (!${Me.AltAbility[${maCC.Find[stGlyph].Value}].ID} && ${Me.AAPoints} >= ${AltAbility[${maCC.Find[stGlyph].Value}].Cost}) {
		/alt buy ${AltAbility[${maCC.Find[stGlyph].Value}].ID}
	}

/return



|***
 * note: uses the aa Spire you have set
 * use: /call cast_AA_spire DEBUG
 ***|
sub cast_AA_spire(bool _debug)
	DEBUG \atcast_AA_spire\aw()
	
	ISMEDEAD
	CHECKEXIT
	GETINPUT

	/if (${Raid.Members} && ${maChr.Find[swSpireRaid].Value}) {
		DEBUG spire${sep}Raid
		/if (!${Me.Buff[${maData.Find[stSpire].Value}].ID} && ${Spell[${maData.Find[stSpire].Value}].Stacks}) {
			/if (${is_ready[${_debug}, "${maData.Find[stSpire].Value}"]}) {
				/call voodoo ${_debug} "${maData.Find[stSpire].Value}" 0 FALSE
				/return
			}
		}
	} else /if (!${Raid.Members} && ${maChr.Find[swSpireGroup].Value}) {
		DEBUG spire${sep}Group
		/if (!${Me.Buff[${maData.Find[stSpire].Value}].ID} && ${Spell[${maData.Find[stSpire].Value}].Stacks}) {
			/if (${is_ready[${_debug}, "${maData.Find[stSpire].Value}"]}) {
				/call voodoo ${_debug} "${maData.Find[stSpire].Value}" 0 FALSE
				/return
			}
		}
	}

/return



|***
 * note: uses the aa Spire you have set
 * use: /call cast_AA_spire
 ***|
sub set_hud_data(string _mode)
	
	/varset h_build ${maEnv.Find[build].Value}
	
	/if (${Bool[${_mode}]}) {
		/varset h_mode ${_mode}
	} else {
		/varset h_mode
	}
		
	| check for Invis
	/declare _lishd listiterator local
	/vardata _lishd lsIV.First.Clone
	/while (!${_lishd.IsEnd}) {
		/if (${Me.Buff[${_lishd.Value}].ID}) {
			| /varset stHUDNotice ${stHUDNotice} IV
		}
		/varset dump ${_lishd.Advance}
	}		

	/vardata _lishd lsIVU.First.Clone
	/while (!${_lishd.IsEnd}) {
		/if (${Me.Buff[${_lishd.Value}].ID}) {
			| /varset stHUDNotice ${stHUDNotice} IVU
		}
		/varset dump ${_lishd.Advance}
	}		
	
	/varset h_time 					${maData.Find[swHUDTime].Value}

	| engages
	/varset h_engage				${maCC.Find[stPctEngage].Value}
	/varset h_engagepet			${maMinion.Find[stPctPetEngage].Value}
	/varset h_engageswarm	 	${maMinion.Find[stPctSwarmEngage].Value}
	/varset h_swpet					${maMinion.Find[swPet].Value}
	/varset h_swswarm				${maMinion.Find[swSwarm].Value}

	| cc modes
	/varset h_auto					AUTO
	/varset h_melee					MELEE
	/varset h_range					RANGE
	/varset h_nuke					NUKE
	/varset h_dot						DOT
	/varset h_heal					${maCC.Find[swCombatHeal].Value}
	/varset h_control				${maCC.Find[swCombatControl].Value}
	/varset h_env						ENV
	

	| healing
	/varset h_healmode			${maHeal.Find[stHealMode].Value}
	/varset h_override			${maHeal.Find[stHealAdjust].Value}
	/varset h_self					${maHeal.Find[swHealSelf].Value}
	/varset h_group					${maHeal.Find[swHealGroup].Value}
	/varset h_xt						${maHeal.Find[swHealXTarget].Value}
	/varset h_pet						${maHeal.Find[swHealPet].Value}
	/varset h_cure					${maHeal.Find[swHealCure].Value}

	| buff
	/varset h_bself					${maBuff.Find[swBuffSelf].Value}
	/varset h_bcrew					${maBuff.Find[swBuffCrew].Value}
	/varset h_braid					${maBuff.Find[swBuffRaid].Value}
	/varset h_bbeg					${maBuff.Find[swBuffBeg].Value}
	/varset h_bunity				${maBuff.Find[swBuffUnity].Value}
	/varset h_baura					${maBuff.Find[swBuffAura].Value}
	/varset h_btemp					${maBuff.Find[swBuffTemp].Value}
	/varset h_bnow					${maBuff.Find[swBuffNow].Value}
	/varset h_bmaster				${maBuff.Find[swBuffMaster].Value}
	/varset h_bpet					${maMinion.Find[swPetBuff].Value}
	
	| pull / agro
	/varset h_agro					AGRO
	/varset h_pull					PULL
	/varset h_pullarea			${maCC.Find[stPullRadius].Value}
	/varset h_pullmode			${maCC.Find[stPullMode].Value}
	
	| tie
	/varset h_tie						TIE
	/varset h_tietoon				${maTie.Find[stTieToon].Value}
	/varset h_tiemode				${maTie.Find[stTieMode].Value}

	| burn
	/varset h_buauto				${maBurn.Find[swBurnAuto].Value}
	/varset h_bupct					${maBurn.Find[stPctBurnEngage].Value}
	/varset h_bucount				${maBurn.Find[stBurnCount].Value}
	/varset h_buraid				${maBurn.Find[swBurnRaid].Value}

	| rest
	/varset h_rpct					${maEnv.Find[stPctRest].Value}
	/varset h_rfull					${maEnv.Find[swRestFull].Value}
	/varset h_rrod					${maEnv.Find[swRestModRod].Value}
	/varset h_rbuff					${maEnv.Find[swRestBuff].Value}
	/varset h_rcombat				${maEnv.Find[swRestCombat].Value}
	
	| rez
	/varset h_zgive					${maRez.Find[swRezGive].Value}
	/varset h_ztake					${maRez.Find[swRezTake].Value}
	/varset h_zcombat				${maRez.Find[swRezCombat].Value}
	/varset h_zeveryone			${maRez.Find[swRezEveryone].Value}
	/varset h_ztoken				${maRez.Find[swRezToken].Value}
	
/return



|***
 * note: passive toggle AA Agro switches
 * use: auto based on /cc agro switch
 ***|
sub set_AA_agro(bool _debug)
	DEBUG \atset_AA_agro\ax()
	ISMEDEAD
	GETINPUT
	
	| turn agro ON
	/if (AGRO) {

		| adjust bold attacks AA
		/if (${Me.AltAbilityReady[Bold Attacks: Disabled]}) {
			/alt act ${Me.AltAbility[Bold Attacks: Disabled].ID}
			/varset dump ${out[2, "AA${sep}${info}Bold Attacks\ax ${dot} ${enabled}"]}
		}

		| SHD Soul Flay
		/if (${Select[${Me.Class.ShortName},SHD]}) {
			/if (${Me.AltAbilityReady[Soul Flay: Disabled]}) {
				/alt act ${Me.AltAbility[Soul Flay: Disabled].ID}
				/varset dump ${out[2, "AA${sep}${info}Soul Flay\ax ${dot} ${enabled}"]}
			}			
		}

		| BST Combat Subtlety
		/if (${Select[${Me.Class.ShortName},BST]}) {
			/if (${Me.AltAbilityReady[Combat Subtlety: Enabled]}) {
				/alt act ${Me.AltAbility[Combat Subtlety: Enabled].ID}
				/varset dump ${out[2, "AA${sep}${info}Combat Subtlety\ax ${dot} ${disabled}"]}
			}			
		}		
		
		| ranger ferocious kicks
		/if (${Select[${Me.Class.ShortName},RNG]}) {
			/if (${Me.AltAbilityReady[Ferocious Kick Subtlety: Enabled]}) {
				/alt act ${Me.AltAbility[Ferocious Kick Subtlety: Enabled].ID}
				/varset dump ${out[2, "AA${sep}${info}Ferocious Kick Subtlety\ax ${dot} ${disabled}"]}
			}
		}

	| turn agro OFF
	} else /if (!AGRO) {

		| adjust bold attacks AA
		/if (${Me.AltAbilityReady[Bold Attacks: Enabled]}) {
			/alt act ${Me.AltAbility[Bold Attacks: Enabled].ID}
				/varset dump ${out[2, "AA${sep}${info}Bold Attacks\ax ${dot} ${disabled}"]}
		}

		| SHD Soul Flay
		/if (${Select[${Me.Class.ShortName},SHD]}) {
			/if (${Me.AltAbilityReady[Soul Flay: Enabled]}) {
				/alt act ${Me.AltAbility[Soul Flay: Enabled].ID}
				/varset dump ${out[2, "AA${sep}${info}Soul Flay\ax ${dot} ${disabled}"]}
			}			
		}

		| BST Combat Subtlety
		/if (${Select[${Me.Class.ShortName},BST]}) {
			/if (${Me.AltAbilityReady[Combat Subtlety: Disabled]}) {
				/alt act ${Me.AltAbility[Combat Subtlety: Disabled].ID}
				/varset dump ${out[2, "AA${sep}${info}Combat Subtlety\ax ${dot} ${enabled}"]}
			}			
		}
		
		| ranger ferocious kicks
		/if (${Select[${Me.Class.ShortName},RNG]}) {
			/if (${Me.AltAbilityReady[Ferocious Kick Subtlety: Disabled]}) {
				/alt act ${Me.AltAbility[Ferocious Kick Subtlety: Disabled].ID}
				/varset dump ${out[2, "AA${sep}${info}Ferocious Kick Subtlety\ax ${dot} ${enabled}"]}
			}
		}

	}

/return



|***
 * note: pulls NON los mobs to you using a healers [SHM/CLR] intervention spell
 * use: /call get_target_intpull DEBUG [targetID]
 ***|
sub get_target_intpull(bool _debug, int _tmpID)
	DEBUG \atget_target_intpull\ax(\a-w${_tmpID}\ax)
	/if (!AUTO || !PULL) /return
	ISMEDEAD
	GETINPUT
	CHECKTIE

	/declare _tmpCombatRange int local 31
	/declare _count int local 0
	
	/if (${_tmpID}) {
		/varset stAssTarID ${_tmpID}
	}	else {
		/call pull_mob_find FALSE
	}
	
	/if (!TARGET) {
		SETHUD FALSE
		/return FALSE
	}
	
	/if (${Target.ID} != TARGET) {
		/squelch /target ID TARGET
		/delay 1s ${Target.ID} == TARGET
	}	
	
	/if (!TARGET) {
		/delay 1s
		/return
	} else /if (${Spawn[id TARGET].Distance} < ${Spawn[ID TARGET].MaxRangeTo}) {
		/varset combat COMBAT
		/if (${swCombatMelee}) /squelch /stick ID TARGET SAFEENV
		/return
	}
	
	/delay 1s
	/dex ${get_resident[FALSE, CLR, 60]} /advancedpullint ${Me.ID}
	/delay 1s
	
	/call pull_mob_prep FALSE TARGET
				
	SETHUD FALSE

/return



|***
 * note: moves to an established stAssTarID
 * use: /call pull_mob_move_to DEBUG [TARGETID] [distance to stop at] [Line Of Sight on|off]
 ***|
sub pull_mob_move_to(bool _debug, int _tmpID, int _dist, string _LoS)
	DEBUG \atpull_mob_move_to\ax(\a-w${_tmpID}, ${_dist}, ${_LoS}\ax)
	
	/nav ID ${_tmpID} distance=${_dist} lineofsight=${_LoS} log=off		
	/delay 1s ${Me.Moving}	
	
	/while (${Navigation.Active}) {
		DEBUG ${sep}moving to${sep}\a-w${_tmpID}\ax
		
		| range from camp check
		/if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${setCampY},${setCampX},${setCampZ}]} > PULLENV) {
			DEBUG Mob exceeded setMobAGro range
			/varset _tmpID 0
			/return
		}
		/delay 1
		
		GETINPUT
		/if (!${_tmpID}) /return FALSE
		/call pull_mob_tag ${_debug} ${_tmpID}
		/if (${Macro.Return}) {
			/nav stop log=off
			/break
		}
	}

/return



|***
 * note: uses a pet class to get mobs to camp
 * use: /call get_target_petpull DEBUG [targetID]
 ***|
sub get_target_petpull(bool _debug, int _tmpID)
	DEBUG \atget_target_petpull(${_debug}, ${_tmpID})
	
	ISMEDEAD
	GETINPUT
	CHECKTIE
	
	/declare _count int local 0
	
	/if (!PULL) {
		/call set_pull_observer ${_debug}
	}	else {
		/call pull_mob_find ${_debug}
	}
	
	/if (PULL && !TARGET) {
		DEBUG ${sep}\arNO\ax stAssTarID
		SETHUD FALSE
		/return FALSE
	} else {
		/if (!${_tmpID}) /varset _tmpID TARGET
	}
	
	/varset dump ${set_target[${_tmpID}]}
	
	/call setDeclare giveUpPull timer outer 30s
	
	DEBUG ${sep}target${sep}\ay${_tmpID}\ax${sep}\aw${Spawn[${_tmpID}].DisplayName}\ax ${sep} dist:\a-w${Spawn[${_tmpID}].Distance}\ax
	
	/if (${Me.Pet.Taunt}) /pet taunt
	/if (${Me.Pet.Stance.NotEqual[follow]}) /pet follow
	/if (!${swHomeSet}) /call Bind_home_control set FALSE
	/varset dump ${out[9, "Fetching${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax"]}
	/declare _melee float local ${Spawn[${_tmpID}].MaxRangeTo}

	| get closer if we need too
	/if (${Spawn[${_tmpID}].Distance} >= 200) {
		/call pull_mob_move_to FALSE ${_tmpID} 60 off
		/if (!${giveUpPull}) /goto :timerfail
	}
		
	/while (${Me.Moving}) {
		/delay 2
	}
	
	/delay 8s ${Me.Pet.Distance} < 15
	/pet attack
	/delay 8s ${Me.Pet.Combat}
		
	/while (${Math.Distance[${Me.Pet.Y},${Me.Pet.X}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X}]} > ${_melee}) {
		/pet attack
		/delay 1s
		DEBUG ${sep}waiting on pet to get to target :: ${Math.Distance[${Me.Pet.Y},${Me.Pet.X}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X}]}
		/delay 6s ${Math.Distance[${Me.Pet.Y}, ${Me.Pet.X}, ${Me.Pet.Z} : ${Spawn[${_tmpID}].Y}, ${Spawn[${_tmpID}].X}, ${Spawn[${_tmpID}].Z}]} > ${_melee}
		/delay 5
		/if (${Target.AggroHolder.Name.Equal[${Me.Pet.Name}]}) /break
		/if (!${Me.Pet.ID} || !${_tmpID}) /break
		GETINPUT
		/if (!${giveUpPull}) /goto :timerfail
	}
		
	/call where_the_fuck_is_my_pet FALSE FALSE		
	
	/call Bind_home_control return FALSE
	/if (!${giveUpPull}) /goto :timerfail
	
	/while (${Me.Pet.Distance} > 20) {
		DEBUG ${sep}pet.distance ${sep} ${Me.Pet.Distance} > 20
		/delay 3
		/call where_the_fuck_is_my_pet FALSE FALSE
		/if (!${Me.Pet.ID} || !${_tmpID}) /return
		/if (${Me.Pet.Distance} < 20) /break
		/if (${Spawn[pc group raid ${Target.AggroHolder.Name}].ID}) /return
		GETINPUT
		/if (!${giveUpPull}) /goto :timerfail
	}		


|***
	/while (${Spawn[${_tmpID}].Distance} > ${MainAssistAgro}) {
		DEBUG Waiting for  ${sep} \ay${_tmpID}\ax ${sep} \aw${Spawn[${_tmpID}].DisplayName}\ax ${sep} dist:\a-w${Spawn[${_tmpID}].Distance}\ax
		/if (${Spawn[${_tmpID}].Dead}) /break
		/delay 3
		/if (!${Me.Pet.ID} || !${_tmpID}) /return
		/if (${Spawn[pc group raid ${Target.AggroHolder.Name}].ID}) /return
		GETINPUT
		/if (!${giveUpPull}) /goto :timerfail
	}
	
***|	
	
	
	| wait for mob to get to camp
	:waitformob
	/declare _wait_for_mob timer local 30s
	/varset dump ${out[9, "Waiting on ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
	/while (${_tmpID} && ${Spawn[${_tmpID}].Distance} > SAFEENV*2) {
		GETINPUT
		/delay 2

		/if (${Spawn[${_tmpID}].Dead}) /break
		/if (!${Me.Pet.ID} || !${_tmpID}) /break
		/if (${Spawn[pc group raid ${Target.AggroHolder.Name}].ID}) /break
		
		| target within melee
		/if (${Spawn[${_tmpID}].Distance} <= ${Spawn[${_tmpID}].MaxRangeTo}) {
			DEBUG ${sep}/while \a-gbreak_MaxRangeTo\ax
			/break
		}
		
		| timer ran out
		/if (!${_wait_for_mob}) {
			DEBUG /while \a-gbreak_Timer\ax
			/goto :timerfail
		}
	}	

	:timerfail
	/if (!${giveUpPull}) {
		/varset dump ${out[9, "${notice} ${dot} Timer to get mob failed!"]}
		/call where_the_fuck_is_my_pet FALSE TRUE
		/call Bind_home_control return FALSE
	}

	SETHUD FALSE	

/return



|***
 * note: mechanics for nav pulling
 * note: /call get_target_navpull DEBUG [targetID]
 ***|
sub get_target_navpull(bool _debug, int _tmpID)
	DEBUG \atget_target_navpull\ax(\a-w${_tmpID}\ax)
	
	ISMEDEAD
	GETINPUT
	CHECKTIE
	
	/declare _count int local 0
	
	/if (!PULL) {
		/call set_pull_observer ${_debug}
	}	else {
		/call pull_mob_find ${_debug}
	}
	
	/if (PULL && !TARGET) {
		DEBUG ${sep}\arNO\ax${sep}\a-wstAssTarID\ax
		SETHUD FALSE
		/return FALSE
	} else {
		/if (!${_tmpID}) /varset _tmpID TARGET
	}

	/if	(!${Navigation.PathExists[locyxz ${Spawn[${_tmpID}].Y} ${Spawn[${_tmpID}].X} ${Spawn[${_tmpID}].Z}]}) {
		GETINPUT
		/varset dump ${out[9, "${notice}${sep}No Nav Path ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
		/varcalc _count ${_count}+1
		/if (${_count} > 10) /return FALSE
		/delay 1s
		/call pull_mob_find FALSE ${_count}
		| /goto :checkpath
	}	

	/declare _movetorange float local ${Spawn[${_tmpID}].MaxRangeTo}
	
	DEBUG ${sep}Target ${sep} ${_tmpID} ${sep} \aw${Spawn[${_tmpID}].DisplayName}\ax

	/if (!${swHomeSet}) /call Bind_home_control set FALSE
	/varset dump ${out[9, "Fetching${sep}{tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
	/varset dump  ${set_target[${_tmpID}]}

	| go get the mob
	/if (${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < 100) {
		DEBUG ${sep}In Sight < 100
		/call pull_mob_prep ${_debug} ${_tmpID}
		/return	

	| get closer if we need too
	} else /if (!${Spawn[${_tmpID}].LineOfSight} || ${Spawn[${_tmpID}].Distance} > 100) {
		DEBUG ${sep}Sight:${Spawn[${_tmpID}].LineOfSight} || Distance:${Spawn[${_tmpID}].Distance} 
		/call pull_mob_move_to FALSE ${_tmpID} ${_movetorange} on
		/while (${Me.Moving}) {
			/if (${Spawn[${_tmpID}].LineOfSight}) /break
			/delay 2
		}		
		/call pull_mob_prep ${_debug} ${_tmpID}
	}

	| move home
	/call Bind_home_control return FALSE

	/while (${Me.Moving}) {
		GETINPUT
		/delay 5
	}
	
	| wait for mob to get to camp
	/declare _wait_for_mob timer local 30s
	/varset dump ${out[9, "Waiting on ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
	/while (${_tmpID} && ${Spawn[${_tmpID}].Distance} > SAFEENV*2 && ${_wait_for_mob}) {
		GETINPUT
		/delay 2
		/if (${SpawnCount[npc radius SAFEENV*2 PS4 targetable]}) {
			/varset _wait_for_mob 0
			/break
		}
		/if (${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < 100) /break
		/if (${Spawn[${_tmpID}].Distance} < SAFEENV*2) /break
	}	

	SETHUD FALSE

/return



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
		SETHUD FALSE
		/return FALSE
	}

	| maintain home marker
	/if (!HOME) /call Bind_home_control set FALSE
	| how the hell do we want to kill it.?
	
	| pull with a normal range if the mob is los and within 100. multi puller keeps local camp under control.
	/if (${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} < 100) {
		DEBUG ${sep}Spawn is LOS & < \ag100\ax
		varset dump ${out[9, "Pulling ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax"]}
		/target ID TARGET
		/delay 1s ${Target.ID} == TARGET	

		| get us a mob
		/call pull_mob_prep ${_debug} TARGET
		
		| now that we have preped and tagged the mob, wait for it to get to us.
		/goto :waitformob
		

	| the mob is not close enough to risk abandoning the camp or it is not in our direct line of sight. send some poor other gimp to get us some loots.
	} else /if (!${Spawn[TARGET].LineOfSight} || (${Spawn[TARGET].Distance} > 100 && ${Spawn[TARGET].Distance} < PULLENV)) {
		DEBUG Spawn is > 100  & < \ayPULLENV\ax
		
		| is there a usable nav path to this pos?		
		/if	(${Navigation.PathExists[locyxz ${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]}) {
			DEBUG ${sep}NAV Path \aggood\ax
			
			| we try melee first for nav to target then we try pet pulling

			| see if we have any melee classes avaialble?
			/call get_name_multipull FALSE |tank|melee
			/if (${Bool[${Macro.Return}]}) {
				/dex ${Macro.Return} /advancedpullnav TARGET FALSE
				/goto :waitformob
			} 
			
			| no melee available get a pet puller?
			/call get_name_multipull FALSE |MAG|BST|NEC|ENC|SHM|DRU
			/if (${Bool[${Macro.Return}]}) {
				/dex ${Macro.Return} /advancedpullpet TARGET FALSE
				/goto :waitformob
			}
			
		| no path exists to get to target, "God Save the Queen!"
		} else /if (!${Navigation.PathExists[locyxz ${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]}) {
			DEBUG ${sep}NAV Path \arbad\ax
			:intpull
			/call get_target_intpull ${_debug} TARGET
			/goto :waitformob

		}
		
	}
	
	| wait for mob to get to camp
	:waitformob
	/if (!${SpawnCount[npc radius 31 PS4 targetable]}) {

		/declare _wait_for_mob timer local 30s
		/varset dump ${out[9, "Waiting on ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax"]}
		/while (TARGET && ${Spawn[TARGET].Distance} > 31) {
			GETINPUT
			/delay 2
			
			| target gone.. wtf?
			/if (!TARGET) {
				DEBUG ${sep}/while \a-gbreak_NOTtarget\ax
				/break			
			}
			
			| mobs in range not our target
			/if (${SpawnCount[npc radius SAFEENV PS4 targetable]} && ${Spawn[TARGET].Distance} > 31) {
				DEBUG ${sep}/while \a-gbreak_TARGET/RANGE\ax
				/break
			}
			| target LOS & within tag range
			/if (${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} < PULLENV &&  && ${Spawn[TARGET].Distance} < 100) {
				DEBUG ${sep}/while \a-gbreak_tag\ax
				/break
			}
			| target within melee
			/if (${Spawn[TARGET].Distance} <= ${Spawn[TARGET].MaxRangeTo}) {
				DEBUG ${sep}/while \a-gbreak_MaxRangeTo\ax
				/break
			}
			
			| timer ran out
			/if (!${_wait_for_mob}) {
				DEBUG ${sep}/while \a-gbreak_Timer\ax
				/break
			}
		}
	}

/return



|***
 * note: preps all advanced pulling
 * note: entry point
 * use: /call prep_advanced_pulling DEBUG 
 ***|
sub prep_advanced_pulling(bool _debug)
	DEBUG \atprep_advanced_pulling\ax()
	SETHUD "pull ${maCC.Find[stPullMode].Value}"
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
		
		| /dex /echo $\{Me.SpellReady[$\{Spell[$\{healIntervention${_count}}].RankName}]}}
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
		/call core_cast2 "Summon Companion" alt 0 FALSE
	}
	
	/if (!${Me.Pet.ReGroup}) /pet regroup
	/if (!${Me.Pet.Hold}) /pet hold on
	/pet back off
/return



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
 * note: returns a valid resident toon name if there is one
 * note: use ${Macro.Return}
 * use: /call get_name_multipull DEBUG [DANNET GROUPs]
 ***|
sub get_name_multipull(bool _debug, string _groupList)
	DEBUG \atget_name_multipull\ax(\a-w${_groupList}\ax)

	/declare _countGroup int local 0
	/declare _countName	int local 0
	/declare _group string local FALSE
	/declare _name string local FALSE
		
	/for _countGroup 1 to ${_groupList.Count[|]}
		/varset _group ${_groupList.Arg[${_countGroup},|]}

		| group empty / does not exist
		/if (!${DanNet.PeerCount[${_group}]}) /continue

		| check each name in the group. bail on FIRST good match
		/for _countName 1 to ${DanNet.Peers[${_group}].Count[|]}
			/varset _name ${DanNet.Peers[${_group}].Arg[${_countName},|]}

			| never use the MA
			/if (${Group.MainAssist.Name.Equal[${_name}]}) /continue
			
			| no name.. wtf?
			/if (!${Bool[pc ${_name}]}) /continue
			| pc not here?
			/if (!${Spawn[pc ${_name}].ID}) /continue
			| bitches are dead
			/if (${Spawn[pc ${_name}].Dead}) /continue
			| too far away to be of any damn use
			/if (${Spawn[pc ${_name}].Distance} > 40) /continue			
			
			| we made it this far give em a name
			DEBUG ${sep}get_name_multipull ${sep} ${_name}
			/return ${_name}
			
		/next _countName
	/next _countGroup

/return



|***
 * note: outputs based by a verbosity lvl
 * use: ${out[reqlvl#, "string to output to MQ window"]}
 ***|
sub out(int _lvl, string _verbage)

	/if (${lsGrain.Contains[${_lvl}]} || ${_lvl} == 0) {
		/echo ${If[${Bool[${Plugin[MQ2Discord]}]},${maEnv.Find[stVerbLvl].Value},\at.\ax]} ${_verbage}
	}

/return



|***
 * note: checks and corrects tribute and trophy usafe during a raid
 * use: 
 ***|
sub check_tribute(bool _debug)
	DEBUG \atcheck_tribute\aw()
	
	/varset dump ${set_data_timer[FALSE, Check_Tribute, RESTART]}
	
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
     
/return






|***
 * note: rez control system. to ensure only 1 person trys to rez a corpse
 * use: 
 ***|
sub control_rez(string _action, int _ID, string _flag)
	| DEBUG\atcontrol_rez\ax(\a-w\ax)


	
	/if (${_action.Equal[add]}) {
		
		/dgzexecute /varset dump ${maRezControl.Add[${_ID},${_flag}]}
		
		
	} else /if (${_action.Equal[delete]}) {
		
	}
	
return

/dgzexecute /varset dump ${maRezControl.Add[55,rez]}





|***
 * note: checks if a mode needs to be executed
 * use: /call check_mode DEBUG [RESET]
 ***|
sub check_mode(bool _debug, string _verbage)
	DEBUG \atcheck_mode\ax()
	
	/varset dump ${set_data_timer[FALSE, Check_Mode, RESTART]}
	
  /declare _out string local
  /vardata mi maMode.First.Clone
  
	| reset used for zoning, and other stuffs
  /if (${_verbage.Equal[reset]}) {
	  /while (!${mi.IsEnd}) {
	  	/if (${mi.Value}) {
				/varset dump ${maMode.Add[${mi.Key},FALSE]}
	  	}
			/varset dump ${mi.Advance}
	  }  	
  	/return TRUE
  }
  
  
  | normal check routine
  /while (!${mi.IsEnd}) {
  	/if (${mi.Value}) {
  		DEBUG  mode_${mi.Key}
  		/call mode_${mi.Key} ${maDebug.Find[mode${mi.Key}].Value}
  	}
		/varset dump ${mi.Advance}
  }

/return TRUE




|***
 * note: checks the passed item name against the DB
 * use: /call set_loot_item DEBUG [NAME]
 ***|
sub set_loot_item(bool _debug, string _item)
	DEBUG \atset_loot_item\ax(\a-w${_item}\ax)

	| check for the item in the DB
	/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${_item}"
	DEBUG ${sep}lootquery SELECT * FROM loot_control WHERE item="${_item}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax

	| we did not find it?
	/if (${sqlite.Status[lootquery].NotEqual[Success]} || !${sqlite.Rows[lootquery]}) {
		DEBUG ${sep}${_item}${sep}\a-w not in database\ax

		| item not in database or lore and have
		/if (${AdvLoot.SList[${_item}].NoDrop} || ${AdvLoot.SList[${_item}].NoDrop} || ${FindItem[=${_item}].Lore} || ${FindItemBank[=${_item}].Lore}) {
			/varset dump ${out[0, "${add} ${dot} ${_item} = ${leave} [no drop/lore]"]}
			/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "leave")
			DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "leave") \ay${sqlite.Status[lootquery]}\ax

		| by default add everything as keep.
		} else {
			/varset dump ${out[0, "${add} ${dot} ${_item} = ${keep} [default]"]}
			/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "keep")
			DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "keep") \ay${sqlite.Status[lootquery]}\ax
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
	GETINPUT

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
				/echo Buying ${_itemtoBuy} ${_count} of ${_qty}
				/if (${_count} >= ${_qty}) /return
			/next _count
			/varcalc _qty ${_buyAmount}-${FindItemCount[=${_itemtoBuy}]}

		}
	}

/return TRUE








