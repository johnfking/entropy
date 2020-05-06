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
		/if (${_debug}) /echo \a-p::\axmap contents\a-p::\ax${${_container}.Find[${_name}]}
		/if (${_debug}) /echo \a-p::\axmap Success\a-p::\ax count\a-p::\ax${${_container}.Count}
	}

	| sets
	/if (${Select[${_type},set]}) {
		| make sure set container exists
		/if (!${Defined[${_container}]}) {
			/declare ${_container} set ${_scope}
		}

		| add to set
		/invoke ${${_container}.Add[${_value}]}
		/if (${_debug}) /echo \a-p::\axset Success\a-p:: count\a-p::\ax${${_container}.Count}
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
			/if (${_debug}) /echo \a-p::\axlist Success\a-p::\ax
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
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_timer WHERE env_timer='ti${_env_timer}'
	DEBUG ${sep}dummy SELECT * FROM environment_timer WHERE env_timer='${_env_timer}' count:\ap${sqlite.Rows[dummy]}\ax

	| data exists. get from DB and call good
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}Timer \agdoes\ax exists. map it
		/invoke ${maTimer.Add[ti${_env_timer},${sqlite.Result[dummy 1 value]}]}

	} else /if (!${sqlite.Rows[dummy]}) {
		DEBUG ${sep}Timer does \arNOT\ax exists. add to DB and map it
		/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT INTO environment_timer(env_timer, value) VALUES('ti${_env_timer}', '${_value}')
		DEBUG ${sep} /sqlite ${maData.Find[DBcharacter].Value} dummy INSERT INTO environment_timer(env_timer, value) VALUES('ti${_env_timer}', '${_value}') \ay${sqlite.Status[dummy]}\ax
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
sub set_data_map (bool _debug, string _map, string _env_var, string _value, _aux, _table)
	DEBUG \atset_data_map\ax(\a-w${_map}, ${_env_var}, "${_value}", "${_aux}", ${_table}\ax)

	| make sure map container exists
	/if (!${Defined[${_map}]}) {
		/declare ${_map} map outer
	}

	| set the default database suffix
	/if (!${Bool[${_table}]}) {
		/varset _table character
	}

	| record pregen data
	/if (${_value.Arg[1,|].Equal[PREGEN]}) {
		/invoke ${maPREGEN.Add[${_env_var},${_value.Arg[2,|]}]}
	}

	| is the data there already?
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_${_table} WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_${_table} WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	| data exists. get from DB and call good
	/if (${sqlite.Rows[dummy]}) {
		/if (${maEnv.Find[build].Value.Equal[${maData.Find[stPreviousBuild].Value}]}) {
			DEBUG ${sep}DATA \agdoes exists VERSIONS MATCH\ax
			/invoke ${${_map}.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
			DEBUG ${sep}${_map}${${_map}.Find[${sqlite.Result[dummy 1 env_var]}]}
			/return TRUE

		| data exists but versions are different
		} else /if (${maEnv.Find[build].Value.NotEqual[${maData.Find[stPreviousBuild].Value}]}) {
			DEBUG ${sep}DATA \agdoes exists VERSIONS \arDO NOT\ax MATCH\ax
			/invoke ${${_map}.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
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

	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_${_table}(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${_aux}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_${_table}(env_var,value,aux) VALUES('${_env_var}', '${_value}', '${_aux}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/invoke ${${_map}.Add[${_env_var},"${_value}"]}

/return TRUE



|***
 * note: generate initial environment sql and list
 * use: /call set_data_list DEBUG [list name] [variable] [value] [aux]
 ***|
sub set_data_list (bool _debug, string _map, string _env_var, string _value, string _aux, string _table)
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

	| record pregen data
	/if (${_value.Arg[1,|].Equal[PREGEN]}) {
		/invoke ${maPREGEN.Add[${_env_var},${_value.Arg[2,|]}]}
	}

	| is the data there already?
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_${_table} WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_${_table} WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	| data exists. get from DB and call it good
	/if (${sqlite.Rows[dummy]}) {
		/if (${maEnv.Find[build].Value.Equal[${maData.Find[stPreviousBuild].Value}]}) {
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
		} else /if (${maEnv.Find[build].Value.NotEqual[${maData.Find[stPreviousBuild].Value}]}) {
			DEBUG ${sep}DATA \agdoes exists VERSIONS \arNO NOT\ax MATCH\ax

			/if (${Bool[${sqlite.Result[dummy 1 value]}]}) {
				/declare _tmpValue string local ${sqlite.Result[dummy 1 value]}
				DEBUG ${sep}_tmpValue${sep}${_tmpValue}

				/invoke ${${_env_var}.Append["${_tmpValue}"]}
				DEBUG ${sep}${_map}.Add[${_env_var},"${_tmpValue}"]
				/invoke ${${_map}.Add[${_env_var},"${_tmpValue}"]}
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
				DEBUG ++ ${sqlite.Result[dummy 1 value].Arg[${_count},|]}
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
		/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_${_table}(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${Parse[1,${_aux}]}")
		DEBUG ${sep}  dummy INSERT OR IGNORE INTO environment_${_table}(env_var,value,aux) VALUES('${_env_var}', "${_value}", '${Parse[1,${_aux}]}') ${dot} \ay${sqlite.Status[dummy]}\ax

	}

	| create our list for the class to use
	/if (!${_value.Find[PREGEN]}) {
		DEBUG ${sep} _map${sep}\a-w${_map}\ax ${dot} _env_var${sep}\a-w${_env_var}\ax ${dot} _value${sep}\a-w${_value}\ax
		/invoke ${${_env_var}.Append[${_value}]}
		/invoke ${${_map}.Add[${_env_var},"${_value}"]}
	}

/return TRUE



|***
 * note: /creates / sets conditions
 * use: /call set_data_condition DEBUG [variable] [value]

sub set_data_condition(bool _debug, string _env_var, string _value)

	/if (!${Defined[maCondition]}) {
		/declare maCondition map outer
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_condition WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax


	| data exists. get from DB and call good
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/invoke ${maCondition.Add[${sqlite.Result[dummy 1 env_var]},${Parse[1,"${sqlite.Result[dummy 1 value]}"]}]}
		/return TRUE

	}

	| no data. add it.
	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_condition(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_condition(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/invoke ${maCondition.Add[${_env_var},"${_value}"]}

/return TRUE
 ***|


|***
 * note: /creates / sets mis variables
 * use: /call set_data_var DEBUG [variable] [bool,string,int] [local|outer|global] [value]

sub set_data_var(bool _debug, string _env_var, string _type, string _scope, string _value)
	/if (${_debug}) /echo \atset_data\ax(\a-w${_env_var}, ${_type}, ${_scope}, "${_value}"\ax)

	/if (!${Defined[${_env_var}]}) {
		/declare ${_env_var} ${_type} ${_scope}
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_var WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_var WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	| data exists. get from DB and call good
	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		DEBUG /varset ${_env_var} ${sqlite.Result[dummy 1 value]}
		/varset ${_env_var} ${sqlite.Result[dummy 1 value]}
		/return TRUE

	}

	| no data. add it.
	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_var(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_var(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/varset ${_env_var} ${_value}

/return TRUE
 ***|


|***
 * note: /creates / sets buff
 * use: /call set_data_buff DEBUG [variable] [value]
 ***|
sub set_data_buff(bool _debug, string _env_var, string _value)
	DEBUG \atset_data_buff\ax(\a-w${_env_var}, ${_value}\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/invoke ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maBuff]}) {
		/declare maBuff map outer
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_buff WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_buff WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/invoke ${maBuff.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maBuff${sep}${maBuff.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/invoke ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_buff(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_buff(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/invoke ${maBuff.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/invoke ${${_env_var}.Append["${_value}"]}
	}

/return TRUE



|***
 * note: /creates / sets crew
 * use: /call set_data_crew DEBUG [variable] [value]

sub set_data_crew(bool _debug, string _env_var, string _value)
	DEBUG \atset_data_crew\ax(\a-w${_env_var}, ${_value}\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/invoke ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maCrew]}) {
		/declare maCrew map outer
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_crew WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_crew WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/invoke ${maCrew.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maCrew${sep}${maBuff.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/invoke ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_crew(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_crew(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/invoke ${maCrew.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/invoke ${${_env_var}.Append["${_value}"]}
	}

/return TRUE
 ***|


|***
 * note: /creates / sets items
 * use: /call set_data_items DEBUG [variable] [value]

sub set_data_items(bool _debug, string _env_var, string _value)
	DEBUG \atset_data_items\ax(\a-w${_env_var}, ${_value}\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/invoke ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maItem]}) {
		/declare maItem map outer
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_items WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_items WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/invoke ${maItem.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maItem${sep}${maItem.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/invoke ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_items(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_items(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/invoke ${maItem.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/invoke ${${_env_var}.Append["${_value}"]}
	}

/return TRUE
 ***|


|***
 * note: creates / sets defense
 * use: /call set_data_defense DEBUG [variable] [value]

sub set_data_defense(bool _debug, string _env_var, string _value)
	DEBUG \atset_data_defense\ax(\a-w${_env_var}, ${_value}\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/invoke ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maDefense]}) {
		/declare maDefense map outer
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_defense WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_defense WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/invoke ${maDefense.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maDefense${sep}${maDefense.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/invoke ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_defense(env_var, value) VALUES("${_env_var}", "${_value}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_defense(env_var, value) VALUES('${_env_var}', '${_value}') ${dot} \ay${sqlite.Status[dummy]}\ax
	/invoke ${maDefense.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/invoke ${${_env_var}.Append["${_value}"]}
	}

/return TRUE
 ***|


|***
 * note: creates / sets healing
 * use: /call set_data_heal DEBUG [variable] [value]

sub set_data_heal(bool _debug, string _env_var, string _value, string _aux)
	DEBUG \atset_data_heal\ax(\a-w${_env_var}, ${_value}, "${_aux}"\ax)

	/if (!${Defined[${_env_var}]} && ${_env_var.Left[2].Equal[ls]}) {
		/declare ${_env_var} list outer
		/invoke ${${_env_var}.Delimiter["|"]}
	}

	/if (!${Defined[maHeal]}) {
		/declare maHeal map outer
	}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_heal WHERE env_var="${_env_var}"
	DEBUG ${sep}dummy SELECT * FROM environment_heal WHERE env_var='${_env_var}' ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

	/if (${sqlite.Rows[dummy]}) {
		DEBUG ${sep}DATA \agdoes exists\ax
		/invoke ${maHeal.Add[${sqlite.Result[dummy 1 env_var]},${sqlite.Result[dummy 1 value]}]}
		DEBUG maHeal${sep}${maHeal.Find[${sqlite.Result[dummy 1 value]}]}
		/if (${_env_var.Left[2].Equal[ls]}) {
			/invoke ${${_env_var}.Append["${sqlite.Result[dummy 1 value]}"]}
		}
		/return TRUE
	}

	| /sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_heal(env_var, value, aux) VALUES("${_env_var}", "${_value}", "${_aux}")
	| DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_heal(env_var, value) VALUES('${_env_var}', '${_value}', ) ${dot} \ay${sqlite.Status[dummy]}\ax

	/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT OR IGNORE INTO environment_heal(env_var,value,aux) VALUES("${_env_var}", "${_value}", "${_aux}")
	DEBUG ${sep}dummy INSERT OR IGNORE INTO environment_heal(env_var,value,aux) VALUES('${_env_var}', '${_value}', '${_aux}') ${dot} \ay${sqlite.Status[dummy]}\ax


	/invoke ${maHeal.Add[${_env_var},"${_value}"]}
	/if (${_env_var.Left[2].Equal[ls]}) {
		/invoke ${${_env_var}.Append["${_value}"]}
	}

/return TRUE
 ***|







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
		/invoke ${get_shade[FALSE, MQWindow]}
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
	| set a non-use variable to force bind checks
	| /varset bindswap ${If[${bindswap},FALSE,TRUE]}

	| check the event queue
	/doevents

	/if (${maEntropy.Find[swHookSub].Value} && ${Bool[${_sub}]}) {
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
		/invoke ${out[0, "\arERROR\ax\a-p::\axvalidate_plugin()"]}
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
 * note: sarcasim. because, in general, people suck..
 * use: ${get_shade[DEBUG, [mqwindow|say]}
 ***|
sub get_shade(bool _debug, string _out)
	DEBUG \atget_shade\ax(\a-w${_out}\ax)
	FLOW get_shade ${_out}

	/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM shade ORDER BY RANDOM() LIMIT 1

	/if (${_out.Equal[MQWindow]}) {
		/echo \a-g${sqlite.Result[dummy 1 value]}\ax
		| /invoke ${out[1, "\a-g${sqlite.Result[dummy 1 value]}\ax"]}
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
	FLOW check_free_ingame_cash

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
		/invoke ${out[0, "In Game Cash${sep}${num}${_current_funds}\ax"]}

		/if (${Window[MarketplaceWnd]}) /market

	}

/return TRUE



|***
 * note: clear a sql query from memory
 * use: /call sql_clear DEBUG [QUERYNAME]
 ***|
sub sql_clear(bool _debug, string _verbage)

	/if (!${sqlite.Clear[${_verbage}]}) {
		/varset debug ${out[16, "${warning} ${dot} Couldn't clear memory for${sep}\a-w${_verbage}\ax"]}
	}

/return TRUE



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
 * use: /call get_hurt DEBUG [group|pet|mypet|xtarget|tot] || ${get_hurt[DEBUG, group|pet|mypet|xtarget|tot]}
 ***|
sub get_hurt(bool _debug, string _type)
	DEBUG \atget_hurt\ax(\a-w${_type}\ax)
	DEBUG ${sep}using weighted HP check${sep}\a-w${maHeal.Find[swHealWeighted].Value}\ax

	/declare _count int local 0
	/varset hurtID ${Me.ID}
	/varset hurtPctHP 100
	/varset hurtCLS ${Me.Class.ShortName}
	/varset hurtCount 0
	/declare _hurtWeight float local 98
	/declare _tmpWeight float local 99

	| we just take defaults of no one hurt and exit.
	/if (${_type.Equal[reset]}) {
		/return 0

	| check group hurt
	} else /if (${_type.Equal[group]}) {
		DEBUG \a-w{_type.Equal[group]}\ax

		/for _count 0 to ${Group.Members}
			DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}${Group.Member[${_count}].Name}${sep}${Group.Member[${_count}].ID}\ax
			/if (${Group.Member[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}) /continue
			/if (${Group.Member[${_count}].Distance} > ENV) /continue
			/if (!${Group.Member[${_count}].Present} || ${Group.Member[${_count}].Dead}) /continue
			/if (${Group.Member[${_count}].DisplayName.Equal[${maHeal.Find[stRampageTank].Value}]}) /continue
			/if (${lsNoHealToon.Contains[${Group.Member[${_count}].DisplayName}]}) /continue

			| group hurt running count
			/if (${Group.Member[${_count}].PctHPs} <= ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}) {
				/varcalc hurtCount ${hurtCount} + 1
				/if (${maHeal.Find[stCountHealGroup].Value}) {
					DEBUG ${sep}break on${sep}\a-w${hurtCount}\ax \aw>=\ax \a-w${maHeal.Find[stCountHealGroup].Value}\ax
					/if (${hurtCount} >= ${maHeal.Find[stCountHealGroup].Value}) /break
				}
			}

			/if (${maHeal.Find[swHealWeighted].Value}) {
				/varset _tmpWeight ${Math.Calc[(${Group.Member[${_count}].PctHPs} / ${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}) - .${maHeal.Find[stHealPoint${Group.Member[${_count}].Class.ShortName}].Value}]}
				DEBUG ${sep}_count${sep}\a-w${_count}\ax${sep}_tmpWeight${sep}\a-w${_tmpWeight}\ax
				/if (${_tmpWeight} < ${_hurtWeight}) {
					/varset _hurtWeight ${_tmpWeight}
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
	  	/if (!${Me.XTarget[${_count}].Type.Equal[PC]} && !${lsProtectNPCList.Contains[${Me.XTarget[${_count}].DisplayName}]}) /continue
			/if (${Me.XTarget[${_count}].PctHPs} > ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}*.PCTHO) /continue
			/varset _tmpWeight ${Math.Calc[(${Me.XTarget[${_count}].PctHPs} / ${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}) - .${maHeal.Find[stHealPoint${Me.XTarget[${_count}].Class.ShortName}].Value}]}

			/if (${_tmpWeight} < ${_hurtWeight}) {
				/varset _hurtWeight ${_tmpWeight}
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

	}

/return 0



|***
 * note: checks facing of a target and immediate distance
 * use: /call check_facing DEBUG [TARGET ID] [adjust position FORCE/FALSE]
 ***|
sub check_facing(bool _debug, int _tmpID, string _adjust)
	DEBUG \atcheck_facing\ax(\a-w${_tmpID}, ${_adjust}\ax)
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
	DEBUG \atis_ready\ax(\a-w${_verbage}\ax)
	FLOW is_ready::${_verbage} (1)

	/if (!${Bool[${_verbage}]} || ${_verbage.Equal[NULL]}) {
		FLOW is_ready::${_verbage} (2)
		/return FALSE
	}

	| /if (${swIsMeDead}) {
	| 	/return FALSE
	| }

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

		| /if (!${Me.SpellReady[${Spell[${_verbage}].RankName}]} && !${Spell[${Spell[${_verbage}].RankName}].Category.Equal[Auras]}) /return FALSE
		/if (!${Me.SpellReady[${Spell[${_verbage}].RankName}]}) /return FALSE
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
	FLOW get_assist_info ${_type} (1)

	/declare _assist local string FALSE

	| healers or CC that may not have an assst set
	/if (${maCC.Find[stAssistMode].Value.Equal[FALSE]} && (${maCC.Find[swCombatHeal].Value} || ${maCC.Find[swCombatControl].Value})) {
		FLOW get_assist_info ${_type} (2)

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
		FLOW get_assist_info ${_type} (3)

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
		} else /if (${Bool[${maCC.Find[stManualAssist1].Value}]} && ${Spawn[pc ${maCC.Find[stManualAssist1].Value}].ID} && !${Spawn[pc ${maCC.Find[stManualAssist1].Value}].Dead}) {
			DEBUG ${sep}assist found${sep}\a-w${Spawn[pc ${maCC.Find[stManualAssist1].Value}].DisplayName}\ax
			/varset _assist ${Spawn[pc ${maCC.Find[stManualAssist1].Value}].DisplayName}
		} else /if (${Bool[${maCC.Find[stManualAssist2].Value}]} && ${Spawn[pc ${maCC.Find[stManualAssist2].Value}].ID} && !${Spawn[pc ${maCC.Find[stManualAssist2].Value}].Dead}) {
			DEBUG ${sep}assist found${sep}\a-w${Spawn[pc ${stManualAssist2}].DisplayName}\ax
			/varset _assist ${Spawn[pc ${stManualAssist2}].DisplayName}
		}
	}

	FLOW get_assist_info ${_type} (4)

	| return the assist info requested
	/if (${_type.Equal[name]}) {
		DEBUG ${sep}returning assist${sep}\a-w${_assist}\ax
		/return ${_assist}
	} else /if (${_type.Equal[id]}) {
		DEBUG ${sep}returning assist${sep}\a-wSpawn[PC ${_assist}].ID}\ax
		/return ${Spawn[PC ${_assist}].ID}
	} else /if (${_type.Equal[both]}) {
		DEBUG ${sep}returning assist${sep}\a-w|${_assist}|${Spawn[PC ${_assist}].ID}\ax
		/return |${_assist}|${Spawn[PC ${_assist}].ID}
	} else {
		DEBUG ${sep}returning assist${sep}\arFALSE\ax
		/invoke ${out[0, "${notice}${sep}an assist was not found"]}
		/return FALSE
	}

/return FALSE



|***
 * note: creates observers
 * use: /call set_observers DEBUG [map of things to observe] [add|drop]
 ***|
sub set_observers(bool _debug, string _map, string _action)
	DEBUG \atset_observers\ax(\a-w${_map}, ${_action}\ax)
	FLOW set_observers

	/declare _count						int 		local 0
	/declare _actionO					string	local FALSE
	/declare _toon		 				string 	local FALSE
	/declare _timestart				int			local 0
	/declare _timeend					int			local 0
	/declare _tlo							string  local ${${_map}.Find[tlo].Value}
	/declare _miso 						mapiterator local

	/invoke ${set_data_timer[FALSE, Check_Observers, RESTART]}

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
	/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
		/varset _timestart ${MacroQuest.Running}

		| skip toons that are not here
		/if (!${Spawn[${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}].ID}) /continue

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

			| DEBUG ${sep}map(key,value)${sep}${_map}(\a-w${_miso.Key},${_miso.Value}\ax)

			| define or delete the variables as we need
			/if (${_action.Equal[drop]}) {
				DEBUG ${sep}/invoke lsObserved_${_toon}.Remove[${_miso.Value}]
				/if (${Defined[${_toon}_${_miso.Value}]}) /deletevar ${_toon}_${_miso.Value}
				/invoke ${lsObserved_${_toon}.Remove[${_miso.Value}]}

			} else /if (${_action.Equal[add]}) {
				| if what we are observing isnt in the charcters list, add it
				/if (!${lsObserved_${_toon}.Contains[${_miso.Value}]}) {
					/invoke ${lsObserved_${_toon}.Append[${_miso.Value}]}
				}
				| if the observer variable does not exist create it
				/if (!${Defined[${_toon}_${_miso.Value}]}) /declare ${_toon}_${_miso.Value} string outer
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
		/invoke ${out[17, "Observed(${If[${_action.Equal[add]},\agadd\ax,\ardrop\ax]})${sep}${_map}${sep}\aw${_toon}\ax${sep}\a-w${Math.Calc[((${_timeend}-${_timestart}) / 10) / 60]}s\ax"]}

	| moving to the next toon in the dannet all default channel
	/next _count

/return



|***
 * note: basic 3-way switch control for environment settings
 * use: /call set_switch_env DEBUG [switchname] [SQL] [value] || ${set_switch_env[DEBUG, switchname, SQL T/F, value]}
 ***|
sub set_switch_env(bool _debug, string _switch, bool _sql, string _verbage)
	DEBUG\atset_switch_env\ax(\a-w${_switch}, ${_sql}, ${_verbage}\ax)
	FLOW set_switch_env

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
	FLOW cast_AA_fireworks

	/invoke ${set_data_timer[FALSE, Waste_Fireworks, RESTART]}

	/if (!${maEnv.Find[swAAFireworks].Value} || ${Raid.Members}) /return
	/if (${Me.AAPoints} < 20) /return

	/if (${Me.AAPoints} >= 120 && !${Me.AltAbilityReady[Glyph of Fireworks II]}) {
		/alt buy 897
		/invoke ${set_data_timer[FALSE, Waste_Fireworks, 1m]}
		/return
	} else /if (${Me.AltAbilityReady[Glyph of Fireworks II]}) {
		/invoke ${out[1, "'Yo, Adrian. I DID IT."]}
		/call voodoo ${_debug} "Glyph of Fireworks II" 0 FALSE
	}

/return



|***
 * note: watches for the rez box if we have been marked as dead or called.
 * use: /call check_for_rezbox()
 ***|
sub check_for_rezbox(bool _debug)
	DEBUG \atcheck_for_rezbox\ax()
	FLOW check_for_rezbox

	GETINPUT rez_box_found swIsMeDead|${swIsMeDead}

	| dont rezz me
	/if (!${maRez.Find[swRezTake].Value}) /return

	| do I have corpses around ?
	/if (!${SpawnCount[pccorpse name ${Me.DisplayName}]}) {

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
			/invoke ${maCC.Add[swValidAssTarID,FALSE]}
			/invoke ${maCC.Add[swSetCombat,FALSE]}
			/invoke ${maCC.Add[swSetForced,FALSE]}
			/invoke ${set_data_timer[FALSE, Check_Behind, 0]}

			/varset swIsMeDead FALSE
			/varset _swValidRezFound FALSE

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
	FLOW check_buff_cycle_shrink (1)

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
	FLOW check_buff_cycle_shrink (2)

	GETINPUT check_buff_cycle_shrink NA|NA

	/if (!${is_ready[${_debug}, "${maBuff.Find[stShrinkItem].Value}"]}) {
		/return FALSE
	}

	/declare _count int local 0

	| me shrinks
	/while (${Me.Height} > 2.04 && ${_count} < 2) {
		/call voodoo ${_debug} "${maBuff.Find[stShrinkItem].Value}" ${Me.ID} FALSE
		/varcalc _count ${_count}+1
	}

	SETHUD EMPTY

/return TRUE



|***
 * note: conditions: default retrun value is TRUE. evaluate for FALSE to use
 * use: ${check_condition[DEBUG, condition#]}
 ***|
sub check_condition(bool _debug, str _condition)
	DEBUG \atcheck_condition\ax(\a-w${_condition}\ax)
	FLOW check_condition|${_condition}

	| bail on no condition checks
	/if (${_condition} == 0) {
		/return TRUE
	}

	/sqlite ${maData.Find[DBcharacter].Value} data SELECT * FROM environment_condition WHERE env_var="c${_condition}"

	DEBUG ${sep}rows${sep}\a-w${sqlite.Rows[data]}\ax
	/if (${sqlite.Rows[data]}) {
		DEBUG ${sep}${_condition}${sep}\a-w${Parse[1,${sqlite.Result[data 1 value]}]}\ax\ag==\ax\a-w${Parse[9999,${sqlite.Result[data 1 value]}]}\ax
		DEBUG ${sep}(!${Math.Calc[${Parse[9999,${sqlite.Result[data 1 value]}]}]})

		/if (!${Math.Calc[${Parse[9999,${sqlite.Result[data 1 value]}]}]}) {
			DEBUG ${sep}Returning${sep}\a-wFALSE\ax
			/return FALSE
		}
	}

	DEBUG returning default TRUE

/return TRUE



|***
 * note: this casts things. it doesn't care what it casts, but it will cast it.
 * note: ORDER of selections: ALT, SPELL, DISC, ITEM, SONG
 * note:
 * note: best to wrap or prep the cast in an ${is_ready[]}
 * note:
 * use: /call voodoo DEBUG ["name of voodoo to do"] [TARGETID] [sub to check on the way] [ENV override]
 ***|
sub voodoo(bool _debug, string _verbage, int _tmpID, string _sub, int _rangeOverride)
	DEBUG \atvoodoo\ax(\a-w${_verbage}, ${_tmpID}, ${_sub}, ${_rangeOverride}\ax)
	FLOW voodoo ${_verbage}|${_tmpID}|${_sub}|${_rangeOverride} (1)
	ISMEDEAD

	| critical errors
	/if (!${Bool[${_verbage}]}) {
		/invoke ${out[0, "${error}${sep}voodoo${sep}\ar_verbage\ax${sep}\a-w${_verbage}, ${_tmpID}, ${_sub}\ax)"]}
		/delay 5
		/return FALSE
	}

	| generic validations
	/if (${Me.Invis}) {
		/return FALSE
	}
	/if (${swIsMeDead}) {
		/return FALSE
	}

	| are we moving? and not a bard?
	/if (!${Select[${Me.Class.ShortName},BRD]}) {
		/if (${Me.Moving}) /return FALSE
		/if (${Stick.Active}) /return FALSE
		/if (${Navigation.Active}) /return FALSE
	}

	| correct the range if needed
	/declare _range int local
	/if (${Range.Between[1,9999999:${_rangeOverride}]}) {
		/varset _range ${_rangeOverride}
	} else {
		/varset _range ENV
	}

	| is the issued target still a thing???
	/if (${_tmpID} != 0 && ${_sub.NotEqual[skiptargetcheck]}) {
		/if (!${Spawn[${_tmpID}].ID} || ${Spawn[${_tmpID}].Dead}) /return FALSE
		/if (${Spawn[${_tmpID}].Distance} > ${_range}) /return FALSE
	}

	| condition attched?
	/if (${_verbage.Find[+c]}) {
		/declare _condition string local ${_verbage.Arg[2,+]}
		DEBUG ${sep}condition${sep}\aw${_condition}\ax
		/varset _verbage ${_verbage.Arg[1,+]}
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${_condition}]}) {
			/return FALSE
		}
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


	/declare _castLoop int local ${maEntropy.Find[stCastLoop].Value}

	| alt abilities
	/if (${_type.Equal[ALT]}) {
		DEBUG ${sep}alt${sep}${_verbage}

		/if (${Me.CurrentMana} < ${Spell[${_verbage}].Mana}) {
			/return FALSE
		}

		| /if (${Spell[${_verbage}].SpellType.Equal[Beneficial]} && ${_tmpID} == 0) {
		| 	/if (!${Spell[${AltAbility[${_verbage}].Spell.ID}].Stacks}) /return FALSE
		| }

		/if (${Me.Buff[${_verbage}].ID}) {
			/return FALSE
		}
		/if (${Me.Song[${_verbage}].ID}) {
			/return FALSE
		}

		/delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}

		| make sure we have our target if one is passed. 0 is assumed no target required
		/if (${_tmpID}) {
			/invoke ${target[set, ${_tmpID}]}
			FLOW voodoo ${_verbage}|${_tmpID}|${_sub}|${_rangeOverride} (2)
		}

		/if (${_tmpID} == 0) {
			/invoke ${out[2, "AA${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/invoke ${out[2, "AA${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[id ${_tmpID}].DisplayName}\ax"]}
		}

		/while (${_castLoop} > 0) {
			DEBUG ${sep}altCastLoop${sep}${_verbage}

			/if (${_tmpID} && ${Target.ID} != ${_tmpID}) {
				/break
			}

			/alt act ${Me.AltAbility[${_verbage}].ID}


			/if (${Spell[${_verbage}].MyCastTime}) {
				DEBUG ${sep}alt cast delay loop start
				/delay 1s ${Me.Casting.ID}
				/delay ${Math.Calc[${Spell[${_verbage}].MyCastTime.Float}+5]}s !${Me.Casting.ID}
				/while (${Me.Casting.ID}) {
					/delay 3
				}
			}

			/doevents
			DEBUG ${sep}cast return${sep}\ay${maData.Find[stCastReturn].Value}\ax

			| /echo maData.Find[stCastReturn].Value == ${maData.Find[stCastReturn].Value}


			/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,TAKEHOLD,DISTRACTED,RESISTED,FALSE]}) {
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
			FLOW voodoo ${_verbage}|${_tmpID}|${_sub}|${_rangeOverride} (3)
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



		|**
		| check any subs? yes.. no.. maybe??
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

		/if (${Spawn[npc ${_tmpID}].Dead}) {
			/return FALSE
		}

		/if (!${Defined[_castTime]}) {
			/declare _castTime int local 0
		}


		| is the spell memed
		/if (${Me.Gem[${_verbage}]}) {

			| echos
			/if (${_tmpID} == 0) {
				/invoke ${out[3, "Cast${sep}${sp}${_verbage}\ax"]}
			} else /if (${_tmpID} != 0 && ${Target.ID} && ${Target.ID} == ${_tmpID}) {
				/invoke ${out[3, "Cast${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[id ${_tmpID}].DisplayName}\ax"]}
			}

			/while (${_castLoop} > 0) {

				/if (${Me.Invis}) {
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
					/if (${_tmpID} != 0) {
						/if (${Spawn[id ${_tmpID}].Dead}) {
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
				DEBUG ${sep}castloop.casting,window,cooldown
				/while (${Me.Casting.ID} || ${Window[CastingWindow].Open} || ${Me.SpellInCooldown}) {
					/if (!${Me.Casting.ID}) /break
					/if (!${Window[CastingWindow].Open}) /break
					/if (!${Me.SpellInCooldown}) /break
					/delay 3
				}

				/doevents
				DEBUG ${sep}cast return${sep}\ay${maData.Find[stCastReturn].Value}\ax

				| notready/ cant cast
				/if (${Select[${maData.Find[stCastReturn].Value},DISTRACTED,NOTREADY]}) {
					DEBUG ${sep}spell.cr:DISTRACTED,NOTREADY./arbreak/ax
					/varset _castLoop 0
					/if (${Select[${Me.Class.ShortName},BRD]}) {
						/call pulse_stop
					}
					/break

				| good
				} else /if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,BLOCKED,RESISTED,FALSE]}) {
					DEBUG ${sep}spell.cr:SUCCESS,BLOCKED,RESISTED./agbreak/ax
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
			FLOW voodoo ${_verbage}|${_tmpID}|${_sub}|${_rangeOverride} (4)
		}

		/if (${_tmpID} == 0) {
			/invoke ${out[4, "Disc${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/invoke ${out[4, "Disc${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[id ${_tmpID}].DisplayName}\ax"]}
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
			FLOW voodoo ${_verbage}|${_tmpID}|${_sub}|${_rangeOverride} (5)
		}

		/if (${_tmpID} == 0 || ${_tmpID} == ${Me.ID}) {
			/invoke ${out[7, "Item${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/invoke ${out[7, "Item${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax"]}
		}

		/while (${_castLoop}) {
			| DEBUG While
			/useitem "${_verbage}"
			/delay 2

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
		/if (${Me.Buff[${_verbage}].Duration.Seconds} > ${maChr.Find[stSongRefresh].Value}*10) /return FALSE
		/if (${Me.Song[${_verbage}].Duration.Seconds} > ${maChr.Find[stSongRefresh].Value}*10) /return FALSE
		/if (!${Spell[${_verbage}].Stacks}) /return FALSE
		| /if (${Me.BardSongPlaying}) /call pulse_stop core_cast_pulsestop

		/if (${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoClose}

		| mem spell if needed??
		/if (!${Me.Gem[${_verbage}]}) {
			/call set_spell_to ${_debug} TRUE "${_verbage}" ${maEnv.Find[stBuffGem].Value} TRUE
		}

		| make sure we have our target if one is passed. 0 is assumed no target required
		/if (${_tmpID}) {
			/invoke ${target[set, ${_tmpID}]}
			FLOW voodoo ${_verbage}|${_tmpID}|${_sub}|${_rangeOverride} (6)
		}

		/if (${_tmpID} == 0) {
			/invoke ${out[5, "Play${sep}${sp}${_verbage}\ax"]}
		} else /if (${_tmpID} != 0 && ${Target.ID}) {
			/invoke ${out[5, "Play${sep}${sp}${_verbage}\ax ${dot} ${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax"]}
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
			GETINPUT voodoo_pulse NA|NA
			/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,RESISTED,IMMUNE]}) {
				/varset _castLoop 0
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
		DEBUG ${sep}stop${sep}\a-wbard bs\ax
		/while (${Me.Casting.ID} || ${Me.BardSongPlaying}) {
			/invoke ${maData.Add[stCastReturn,NOTREADY]}
			/stopsong
			/stopcast
			/delay 10 !${Me.Casting.ID}
		}

	}	else {
		/if (${Me.Casting.ID}) {
			DEBUG ${sep}stop${sep}\a-wcasting\ax
			/stopcast
		}
	}


	| resets
	/invoke ${maPred.Add[stPredID,0]}

	/if (${_silent.NotEqual[SILENT]}) {
		/invoke ${out[0, "\a-rResetting\ax"]}
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
	/while (${Me.Moving}) {

		/if (${Stick.Active}) {
			DEBUG ${sep}stop${sep}\a-wstick\ax
			/squelch /stick off
		}

		/if (${Navigation.Active}) {
			DEBUG ${sep}stop${sep}\a-wnavigation\ax
			/nav stop |log=off
		}

		/varcalc _breakmove ${_breakmove}+1
		/if (${Me.Moving} || ${_breakmove} > 4) /break

	}

	SETHUD EMPTY

/return TRUE



|***
 * note: moves the toon to a valid/LoS location of a target id with a given distance
 * use : /call moveto_range [ID] [DISTANCE]
 ***|
sub moveto_range(bool _debug), int _tmpID, int _range)
	DEBUG \atmoveto_range\ax(\a-w${_tmpID}, ${_range}\ax)

	/declare _i int local 0
	/declare _locX float local 0
	/declare _locY float local 0
	/declare _locZ float local 0
	/declare _tmpX float local 0
	/declare _tmpY float local 0
	/declare _dist float local ${Math.Calc[${If[${_range}==0,1,${_range}]} / ${If[${Spawn[${_tmpID}].Distance}==0,1,${Spawn[${_tmpID}].Distance}]}]}

	DEBUG ${sep}${Math.Calc[${If[${_range}==0,1,${_range}]} / ${If[${Spawn[${_tmpID}].Distance}==0,1,${Spawn[${_tmpID}].Distance}]}]}
	DEBUG ${sep}_dist${sep}\a-w${_dist}\ax

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

	/invoke ${set_data_timer[FALSE, Check_Xtarget_Build, RESTART]}

	/if (!${Me.XTargetSlots}) /return
	/if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
		/invoke ${get_shade[FALSE, MQWindow]}
		/return
	}

	/invoke ${out[0, "Adjusting XTargets${sep}${If[${Bool[${_forced}]},\a-wForced\ax,Timer \a-w${maTimer.Find[tiCheck_Xtarget_Build].Value}\ax]}"]}
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

		**|

	| protect PCs when outside group
	/if (!${Raid.Members} && ${lsProtectOutsideGroup.Count}) {

		/declare _lipcxt listiterator local
		/vardata _lipcxt lsProtectOutsideGroup.First.Clone
		/while (!${_lipcxt.IsEnd}) {

			/varset _name ${Spawn[${_lipcxt.Value} pc].DisplayName}

			| i hate people
			/if (${lsNoHealToon.Contains[${_name}]}) {
				/invoke ${_lipcxt.Advance}
				/continue
			}

			| in our range of operaions?
			/if (${Spawn[pc ${_name}].Distance} > ENV || !${Bool[${Spawn[pc ${_name}]}]}) {
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
			} else /if (${Bool[${Spawn[pc ${_name}]}]}) {
				/varcalc _b ${_b}+1
				/xtarget set ${_b} ${Spawn[pc ${_name}].Name}
			}

			/invoke ${_lipcxt.Advance}
		}
	}


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
				/invoke ${_lixt.Advance}
				/continue
			}

			| cycle the class and add to slots
			/for _x 1 to ${SpawnCount[pc ${_lixt.Value} raid radius ENV]}

				/if (${_b} > ${Me.XTargetSlots}) /break


				| yes.. i still hate people
				/if (${lsNoHealToon.Contains[${NearestSpawn[${_x}, pc ${_lixt.Value} raid].DisplayName}]}) /continue

				| are they a group member?
				/if (${Group.Member[${NearestSpawn[${_x}, pc ${_lixt.Value} raid]}].ID}) /continue

				| on the xtarget
				/if (${Bool[${Me.XTarget[${NearestSpawn[${_x}, pc ${_lixt.Value} raid].Name}]}]}) /continue

				| me?
				/if (${NearestSpawn[${_x}, pc ${_lixt.Value} raid radius ENV].DisplayName.Equal[${Me.DisplayName}]}) /continue

				/varcalc _b ${_b}+1
				/xtarget set ${_b} ${NearestSpawn[${_x}, pc ${_lixt.Value} raid radius ENV]}
				/delay 1
			/next _x

			/invoke ${_lixt.Advance}
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
	FLOW stick_to_target ${_tmpID}

	/if (${timer_check_stick}) /return FALSE

	/invoke ${set_timer[${_debug}, timer_check_stick, 1s]}

	/if (!AUTO && !${swModeToken}) /return FALSE

	| /if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE

	/if (${target[isdead, ${_tmpID}]}) {
		FLOW stick_to_target ${_tmpID}
		/return FALSE
	}

	ISMEDEAD
	GETINPUT stick_to_target tmpID|${_tmpID}

	/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FALSE

	| if the stick behind switch is on. we do that over ranged decision combat
	/if (${maHome.Find[swMoveBehind].Value} && !${Stick.Behind} && ${Target.Distance} <= ENV && !${timer_Check_Behind} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
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
	FLOW is_target_dead ${_tmpID}

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
			FLOW is_target_dead ${_tmpID}
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
		| SDEBUG ${sep}dead clear_combat
		/invoke ${target[clear]}
		FLOW is_target_dead ${_tmpID}
	}

/return ${_dead}



|***
 * note: Clear all combat variables
 * use: /call clear_combat DEBUG
 ***|
sub clear_combat(bool _debug)
	DEBUG \atclear_combat\ax()
	FLOW clear_combat

	GETINPUT clear_combat NA|NA

	/if (${SubDefined[DPSMeter_Reset]}) {
		/call DPSMeter_Reset
	}

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

	/if (${Defined[timer_${_tmpID}]}) /deletevar timer_${_tmpID}

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
	FLOW is_target_valid ${_tmpID}

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
			FLOW is_target_valid ${_tmpID}
			/return FALSE
		} else /if (${Select[${maCC.Find[stAssistMode].Value},g]} && ${Group.MainAssist.ID} && !${Bool[${Spawn[${Group.MainAssist}].Dead}]} && ${Spawn[${_tmpID}].ID} != ${Me.XTarget[1].ID}) {
			/invoke ${target[clear]}
			FLOW is_target_valid ${_tmpID}
			/return FALSE
		}
	}

	/if (!${maOver.Find[swOverPctEngage].Value} && !PULL) {
		/if (${Spawn[${_tmpID}].PctHPs} > ${maCC.Find[stPctEngage].Value} && !${Select[${maCC.Find[stAssistMode].Value},1,2,3,g]}) {
			/delay 1
			/return FALSE
		}
	}

	| shit we really shouldn't be attacking
	/if (${lsNoAttackBodyType.Contains[${Spawn[${_tmpID}].Type}]}) /return FALSE
	/if (${lsNoTargetNPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /return FALSE
	/if (${lsNoTargetPC.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /return FALSE
	/if (${lsNoTargetMagic.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) /return FALSE
	/if (${lsNoTargetAnimation.Contains[${Spawn[id ${_tmpID}].DisplayName}]}	) /return FALSE
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
 * use: ${is_burn_validDEBUG}
 ***|
sub is_burn_valid(bool _debug)
	DEBUG \atis_burn_valid\ax()
	FLOW is_burn_valid

	| dont do shit in safe zone..
	/if (${maEnv.Find[swZoneSafe].Value}) {
		/invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}

	| no raid burning if turned off
	/if (!${maBurn.Find[swBurnRaid].Value} && ${Raid.Members}) {
		/invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}

	| if always set to burn, always burn while in combat
	/if (${maBurn.Find[swBurnForce].Value} &&  ${Me.CombatState.Equal[combat]}) {
		/invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
		/return TRUE
	}

	| remainder requires automated modes
	/if (!AUTO) /return FALSE

	| no mobs around
	/if (!${SpawnCount[npc radius ENV PS4]}) {
		/invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}

	| if the current target is a namer
	/if (${maBurn.Find[swBurnAuto].Value} && TARGET && ${Spawn[TARGET].PctHPs} <= ${maBurn.Find[stPctBurnEngage].Value} && (${lsZoneNamed.Contains[${Target.DisplayName}]} && !${maOver.Find[swOverName].Value})) {
		/invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
		/return TRUE
	}


	| if number of mobs in the local area reaches the /burn count ## threashold
	/if (${maBurn.Find[stBurnCount].Value}) {
		/if (${SpawnCount[npc radius ENV targetable PS4]} >= ${maBurn.Find[stBurnCount].Value}) {
			/invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
			/return TRUE
		}
	}

	| healers and off combat routines simply check for a named in the local area to start a burn. no direct target of a named is required. just be in the area.
	/if (!${maCC.Find[swCombatHeal].Value}) {
		/invoke ${maBurn.Add[swBurnConditionMet,FALSE]}
		/return FALSE
	}

	/if (COMBAT) {
		/declare _count int local 0
		/declare _mobID int local 0

		/for _count 1 to ${SpawnCount[npc radius ENV PS4]}
			/varset _mobID ${NearestSpawn[${_count}, npc radius ENV].ID}
			/if (${maBurn.Find[swBurnAuto].Value} && ${lsZoneNamed.Contains[${Spawn[ID ${_mobID} radius ENV].DisplayName}]} && !${maOver.Find[swOverName].Value}) {
				/invoke ${maBurn.Add[swBurnConditionMet,TRUE]}
				/return TRUE
			}
		/next _count
	}

/return FALSE



|***
 * note: do we want to exit the class while loop. assume no exit (FALSE)
 * use: ${exit_class_loop[DEBUG]}
 ***|
sub check_class_loop(bool _debug)
	DEBUG \atcheck_class_loop\ax()
	FLOW check_class_loop

	/if (AUTO) {
		| no target.. were done
		/if (!${Spawn[TARGET].ID}) {
			DEBUG ${sep}no target
			/return TRUE
		}

		| targets dead. why bother
		| /if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return TRUE

		/if (${target[isdead, TARGET]}) {
			FLOW check_class_loop
			/return TRUE
		}

	| manual mode
	} else /if (!AUTO) {
		| no target. go away
		/if (!${Target.ID}) /return TRUE
		| forced combat in manual
		/if (COMBAT) {
			| target dead. end
			| /if (${is_target_dead[${maDebug.Find[target].Value}, TARGET]}) /return TRUE

			/if (${target[isdead, TARGET]}) {
				FLOW check_class_loop
				/return TRUE
			}

		}

	}

/return FALSE



|***
 * note: prep sub for all calsses to decide if they are going to be in combat and if its time to burn
 * use: /call check_combat_status DEBUG || ${check_combat_status[DEBUG]}
 **|
sub check_combat_status(bool _debug)
	DEBUG \atcheck_combat_status\ax()
	FLOW check_combat_status

	CHECKCURSOR

	/delay 1
 	| manual mode
	/if (!AUTO) {
		DEBUG ${sep}Manual combat check
		/if (HEAL) {
			/invoke ${maCC.Add[swSetCombat,FALSE]}
		}
		/if (!COMBAT) {
			/return FALSE
		}
		/if (${Target.ID}) {
			/if (${Target.ID} == ${Me.ID}) {
				/return FALSE
			}
			/if (COMBAT) {
				/invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}
			}

			| /call control_TTL ${maDebug.Find[ttl].Value} start

			/return TRUE
		} else /if (!${Target.ID}) {
			/if (COMBAT || VALID) {
				/invoke ${target[clear]}
				FLOW check_combat_status
			}
			/return FALSE
		}

	| combat healing/crowd control
	} else /if (AUTO && (HEAL || (${madeBuff.Find[swMez].Value} && ${maCC.Find[swCombatControl].Value}))) {
		DEBUG ${sep}auto-heal/CC combat check

		| /if (!SAFEZONE) /call set_base_target_timers TARGET
		/if (${is_target_valid[${_debug}, TARGET]}) {
			/invoke ${maCC.Add[swValidAssTarID,TRUE]}
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
			FLOW check_combat_status
			/return FALSE

		} else /if (VALID && ${Spawn[TARGET].ID}) {
			DEBUG ${sep}\ayelse /if (VALID && ${Spawn[TARGET].ID})\ax
			| /if (!SAFEZONE) /call set_base_target_timers TARGET
			| /if (${Me.Class.ShortName.Equal[BRD]}) {
				| /if (${Me.Song[${spellRestSong}].ID}) /stopsong
			| }
			/invoke ${maCC.Add[swSetCombat,TRUE]}
			/invoke ${is_burn_valid[${maDebug.Find[burn].Value}]}
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
	FLOW set_combat

	/if (${target[isdead, TARGET]}) {
		/return FALSE
		FLOW set_combat
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

	/if (${Me.State.NotEqual[STAND]}) /stand
	/if (AUTO && ${Me.State.Equal[FEIGN]}) /squelch /stand
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
			/if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && !AGRO)) {
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
			/if (AUTO && !${Stick.Active} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
				/call moveto_range TARGET ${maHard.Find[stMinMovetoBow].Value}
			}

		| can we get to the melee already?
		} else /if (MELEE) {
			| /if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || ${check_no_melee_buff[FALSE]} || (AUTO && ${Target.DSed.ID} && ${stoponDS} && !${swAgro})) {
			/if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && !AGRO)) {
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

				/squelch /attack on
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
	FLOW set_combat_control ${_tmpID}

	ISMEDEAD

	| /if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE

	| validate the target
	/if (${target[isdead, ${_tmpID}]}) {
		FLOW set_combat_control ${_tmpID}
		/return FALSE
	}


	| out of range?
	/if (${Target.Distance} > ENV && !${maCC.Find[swSetForced].Value}) {
		/return FALSE
	}

	/if (${Me.State.NotEqual[STAND]}) {
		/squelch /stand
	}
	/if (AUTO && ${Me.State.Equal[FEIGN]}) {
		/squelch /stand
	}
	/if (!AUTO && ${Me.Invis}) {
		/makemevisible
	}

	/call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE

	| set the max range so we dont have to keep calculating it over and over
	| /declare _maxrange float local ${Math.Calc[${Spawn[id ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}

	| range or melee, priority is range!
	/if (MELEE && RANGE) {
		DEBUG ${sep}\agmelee\ax${sep}\ayor\ax${sep}\agrange\ax

		/if (${lsZoneRange.Contains[${Target.DisplayName}]} || ${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && !${maOver.Find[swOverDSCheck].Value} && !AGRO)) {
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

		/if (${lsZoneNoMelee.Contains[${Target.DisplayName}]} || (AUTO && ${Target.DSed.ID} && !${maOver.Find[swOverDSCheck].Value} && !AGRO)) {
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
	FLOW set_combat_range

	/if (${Stick.Active}) {
		/squelch /stick off
	}

	/if (AUTO && !${Stick.Active} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
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
	FLOW set_combat_melee ${_tmpID}

	/if (${Me.AutoFire}) {
		/squelch /autofire
	}

	/if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
		/call check_sash_proc ${_debug}
	}

	/squelch /attack on

	/if (${timer_check_stick}) {
		/return FALSE
	}

	/invoke ${set_timer[${_debug}, timer_check_stick, 1s]}

	/if (!AUTO && !${swModeToken}) {
		/return FALSE
	}

	| /if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE

	/if (${target[isdead, ${_tmpID}]}) {
		FLOW set_combat_melee ${_tmpID}
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
	FLOW get_target_buff_time_left ${_buff}

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
	FLOW get_target_basepull ${_tmpID}

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
	}	else {
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
			FLOW get_target_basepull TARGET
		}
	}


	| Screws fall out all the time, the world is an imperfect place.
	/call pull_mob_prep ${_debug} TARGET
	/if (!${Macro.Return}) /return FALSE

	/call pull_mob_tag ${_debug} TARGET

	| /if (!${pull_mob_prep[${_debug}, TARGET]}) {
	| 	SETHUD EMPTY
	| 	/return FALSE
	| }

	| /call pull_mob_tag ${_debug} TARGET

	/if (MELEE) /attack on

	| wait for mob to get to camp
	DEBUG ${sep}loop\a-wwaitformob\ax

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
	DEBUG \atget_target_wait\ax(\a-w${_tmpID}, ${_waittime}\ax)
	FLOW get_target_wait ${_tmpID} (1)

	| wait for mob to get to camp
	DEBUG ${sep}loop\a-wwait for mob\ax${sep}\a-w${_tmpID}\ax
	| :waitformob
	/if (!${SpawnCount[npc radius ENV PS4 targetable]}) {
		| DEBUG ${sep}_wait${sep}\ay${_waittime}\ax
		/invoke ${set_timer[${_debug}, wait_for_mob, ${_waittime}]}

		/if (!${Spawn[${_tmpID}].Dead}) {
			/invoke ${out[9, "Waiting on${sep}${tar}${Spawn[${_tmpID}].DisplayName}\ax${sep}\a-w${_waittime}\ax"]}
		}

		/while (${wait_for_mob}) {
			FLOW get_target_wait ${_tmpID} (2)
			DEBUG ${sep}\awTARGET\ax.distance${sep}\a-w${Spawn[${_tmpID}].Distance}\ax${sep}\a-w${wait_for_mob}\ax
			GETINPUT get_target_basepull_wait NA|NA
			/delay 5

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
			/if (!${_tmpID} || ${Spawn[${_tmpID}].Dead}) {
				DEBUG ${sep}/while \a-gbreak_NO_target\ax
				/invoke ${target[clear]}
				FLOW get_target_basepull ${_tmpID}
				/break
			}

			| mobs in our personal space. fuck those things. no respect
			| /if (${SpawnCount[npc radius SAFEENV PS4 targetable]} || ${NearestSpawn[1, npc radius ENV].ID} != ${_tmpID}) {
			/if (${SpawnCount[npc radius ENV PS4 targetable]}) {
				DEBUG ${sep}break.mob in environment space
				/invoke ${out[9, "Resetting target to${sep}\a-w${NearestSpawn[1, npc radius ENV].DisplayName}\ax"]}
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
			/if (${maCC.Find[stPullMode].Value.Equal[pet]} && !${Pet.ID} && PET) {
				/call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
			}

			| timer ran out
			/if (!${wait_for_mob}) {
				/invoke ${out[9, "Sick of waiting on${sep}\a-w${Spawn[${_tmpID}].DisplayName}\ax"]}
				/invoke ${target[clear]}
				FLOW get_target_wait ${_tmpID} (3)
				/break
			}
		}

	}

/return TRUE



|***
 * note: various conditions for the target in question
 * use: /call pull_mob_prep DEBUG [TARGETID]
 ***|
sub pull_mob_prep(bool _debug, int _tmpID)
	DEBUG \atpull_mob_prep\ax(\a-w${_tmpID}\ax)
	FLOW pull_mob_prep ${_tmpID}

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
		/return FALSE
	}

	| get the mob to us for non pet classes..
	DEBUG ${sep}pull_mob_prep${sep}\awLoop.Start\ax${sep}${_tmpID}
	/if (!${target[isdead, ${_tmpID}]} && !${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) {
		FLOW pull_mob_prep ${_tmpID}

		| keep tabs on targets range if they are the puller. advanced pullers fetching we do not care about yet
		/if (${Spawn[${_tmpID}].Distance} > PULLENV) {
			/invoke ${target[clear]}
			FLOW pull_mob_prep ${_tmpID}
			/return FALSE
		}

		| CHECKCURSOR TRUE
		GETINPUT pull_mob_prep _tmpID|${_tmpID}
		ISMEDEAD

		| stand the fuck up. seriously...
		/if (${SpawnCount[npc ${_los} radius PULLENV targetable]} && ${Me.State.NotEqual[STAND]}) /stand
		| ain't got no time for that
		/if (${Me.Stunned}) /return FALSE

		| keep us facing the target just enough...
		/if (${Target.ID} && (MELEE || RANGE) && ${Spawn[ID ${_tmpID}].LineOfSight}) {
			/call check_facing ${_debug} ${_tmpID} FORCE
		}

		| /delay 2
		/if (${target[isdead, ${_tmpID}]}) {
			FLOW pull_mob_prep ${_tmpID}
			/return FALSE
		}

		| get out if mob is even remotely close
		/if (${Spawn[ID ${_tmpID}].Distance} <= ENV && ${Spawn[ID ${_tmpID}].LineOfSight}) {
			DEBUG ${sep}prep${sep}in sight < ENV
			/return TRUE
		}
	}

	| pet auto send to kill
	/if (${Spawn[ID ${_tmpID}].Distance} <= PULLENV && ${Select[${Me.Class.ShortName},MAG,NEC,BST,ENC]}) {
		DEBUG ${sep}Pet Range${sep}${_tmpID}
		/if (${Target.ID} != ${_tmpID}) {
			/invoke ${target[set, ${_tmpID}]}
			FLOW pull_mob_prep ${_tmpID}
		}
		/if (${Target.ID} && !${Bool[${Me.Pet.Target}]}) {
			/squelch /pet attack
		}
		/return TRUE
	}

/return TRUE



|***
 * note: tea anyone?
 * use: /call pull_mob_tag DEBUG [TARGETID]
 ***|
sub pull_mob_tag(bool _debug, int _tmpID)
	DEBUG \atpull_mob_tag\ax(\a-w${_tmpID}\ax)
	FLOW pull_mob_tag ${_tmpID}

	/declare _count int local 0

	/if (!${Select[${Me.Class.ShortName},RNG]} && !${Me.Inventory[ammo].ID} && ${maBuff.Find[swBuffAmmo].Value}) {
		/call check_buff_cycle_ammo ${_debug} TRUE
	}

	/if (${Spawn[ID ${_tmpID}].LineOfSight}) {
		DEBUG ${sep}Target${sep}\agLoS\ax

		| get our target
		/if (${Target.ID} != ${_tmpID}) {
			/invoke ${target[set, ${_tmpID}]}
			FLOW pull_mob_tag ${_tmpID}
		}

		| aa/spell/disc/item tag
		/if (${Bool[${maCC.Find[stRangeOther].Value}]}) {
			/if (${is_ready[${_debug}, "${maCC.Find[stRangeOther].Value}"]}) {
				DEBUG ${sep}Tag${sep}Other${sep}${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax${sep}\a-w${maCC.Find[stRangeOther].Value}\ax


				|**
				/while (${Me.Moving}) {
					/delay 2
					/if (${Spawn[ID ${_tmpID}].Distance} < 100) {
						DEBUG ${sep}pull tag${sep} < 100 breaking
						/call set_stop ${maDebug.Find[stop].Value} SILENT
						/delay 2s !${Me.Moving}
						/break
					}
				}
				**|

				| get the range for whatever it is we are using
				/declare _range int local

				/if (${Me.AltAbilityReady[${maCC.Find[stRangeOther].Value}]}) {
					DEBUG ${sep}\a-wALT\ax
					/varset _range ${Spell[${maCC.Find[stRangeOther].Value}].MyRange}

				| spell/song
				} else /if (${Me.Book[${Spell[${maCC.Find[stRangeOther].Value}].RankName}]}) {
					DEBUG ${sep}SPELL
					/varset _range ${Spell[${maCC.Find[stRangeOther].Value}].MyRange}

				| disc
				} else /if (${Me.CombatAbilityReady[${Me.CombatAbility[${Spell[${maCC.Find[stRangeOther].Value}].RankName}]}]}) {
					DEBUG ${sep}DISC
					/varset _range ${Spell[${maCC.Find[stRangeOther].Value}].MyRange}

				| item
				} else /if (${Me.ItemReady[=${maCC.Find[stRangeOther].Value}]}) {
					DEBUG ${sep}ITEM
					/varset _range ${Spell[${FindItem[=${maCC.Find[stRangeOther].Value}].Spell}].MyRange}

				| we have no idea what this is!
				} else {
					/invoke ${out[0, "No idea wtf you put for stRangeOther but you 'dun fucked up."]}
					/invoke ${out[0, "Turning pulling off. Go check your settings."]}

					/varset _range 0
				}

				/call voodoo ${_debug} "${maCC.Find[stRangeOther].Value}" ${_tmpID} FALSE ${_range}
				/delay 5
				/return TRUE
			}
		}

		| range tag
		| there are some base limitations to combat range tagging. we are going to limit to 250 for range/thrown items
		| yes i know some classes can go a lot farther..
		/if (${Spawn[ID ${_tmpID}].Distance} > ${maHard.Find[stMinBowDist].Value} && ${Spawn[ID ${_tmpID}].Distance} > ENV && ${Spawn[ID ${_tmpID}].Distance} < ${maHard.Find[stMaxBowDist].Value} && RANGE) {
			DEBUG ${sep}Tag${sep}Range${sep}${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax${sep}\a-w${Spawn[ID ${_tmpID}].Distance}\ax
			GETINPUT pull_mob_tag_range NA|NA
			/nav stop |log=off
			/delay 1s !${Me.Moving}
			/if (${Target.ID}) {
				/range
				/range
			}

			/while (${Target.AggroHolder.Name.NotEqual[${Me.DisplayName}]} || ${_count} < 4) {
				/if (${Target.AggroHolder.Name.Equal[${Me.DisplayName}]}) /break
				/if (${Target.ID} && ${Spawn[ID ${_tmpID}].Distance} > ${maHard.Find[stMinBowDist].Value}) {
			 		/range
			 		/delay 5
					/varcalc _count ${_count}+1
				} else {
					/return FALSE
			 	}
			}

			| /if (${Target.ID}) /range
			/delay 5
			/return TRUE
		}

		| melee tag
		| we are using 90% of the targets max range to melee just to make sure we get close enough.
		| dont argue with me on this. i'm not changing my mind
		| if we are not in this range, we try and get closer.
		/if (MELEE && ${Spawn[ID ${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
			DEBUG ${sep}Tag${sep}Melee${sep}${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax
			GETINPUT pull_mob_tag_melee NA|NA
			/attack on
			/return TRUE
		}

		| inbetween melee and range distance.. you know that fucked up mo mans land spot...
		/if (MELEE && ${Spawn[ID ${_tmpID}].Distance} > ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} && ${Spawn[ID ${_tmpID}].Distance} <= ENV) {
			DEBUG ${sep}Tag${sep}Melee${sep}${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax
			GETINPUT pull_mob_tag_both NA|NA
			/attack on

			DEBUG ${sep}${Spawn[ID ${_tmpID}].Distance} > ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}
			DEBUG ${sep}Tag${sep}Melee${sep}${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax \agNAVTO\ax
			/nav ID ${_tmpID} |dist=${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} log=off
			/delay 5 ${Me.Moving}
			/return TRUE
		}

	} else /if (!${Spawn[${_tmpID}].LineOfSight} && !${maOver.Find[swOverLOS].Value}) {
		DEBUG ${sep}Target${sep}\arLOS\ax\a-w${_tmpID}\ax
		/invoke ${set_data_timer[FALSE, Check_LoS_Timeout, RESTART]}
		/if (${maCC.Find[stPullMode].Value.NotEqual[pet]}) {
			/invoke ${out[9, "Waiting for pull (\a-wLOS\ax)${sep}\a-w${Spawn[ID ${_tmpID}].DisplayName}\ax"]}
		}
		/while (${timer_Check_LoS_Timeout}) {
			/delay 2
			/if (${Spawn[ID ${_tmpID}].LineOfSight}) /break
			/if (${Spawn[ID ${_tmpID}].Distance} > PULLENV) /break
			/if (!${Spawn[ID ${_tmpID}].ID}) /break
		}
		| /invoke ${target[clear]}
		FLOW pull_mob_tag ${_tmpID}
		/return FALSE
	}

	/if (${Spawn[ID ${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} && MELEE && !RANGE) {
		DEBUG ${sep}Tag${sep}Melee NO RANGE${sep}${tar}${Spawn[ID ${_tmpID}].DisplayName}\ax
		GETINPUT pull_mob_tag_fail NA|NA
		/attack on
		/return TRUE

	}

/return FALSE



|***
 * note: Finds and sets stAssTarID
 * use: /call pull_mob_find DEBUG [# to force SpawnCount increment]
 ***|
sub pull_mob_find(bool _debug, int _spCount)
	DEBUG \atpull_mob_find\ax(\a-w${_spCount}\ax)
	FLOW pull_mob_find ${_spCount}

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
	/declare _tmpAgroRange int local ${Int[${Math.Calc[PULLENV/5]}]}
	/declare _count int local 0
	/declare _mobHP int local 999
	/declare _tmpHPAdj int local 0
	/declare _los string local los
	/declare _closestNAV int local PULLENV
	/declare _closestID int local 0
	/declare _navRange int local 0

	| /declare _tmpCombatRange int local ${maHard.Find[stMinBowDist].Value}

	| adjust LOS?
	/if (${maOver.Find[swOverLOS].Value} || ${Select[${maCC.Find[stPullMode].Value},nav,int,pet,calm,multi]}) {
		DEBUG ${sep} LOS overridden
		/varset _los
	}

	| time to step the agro range out so we get mobs closer first
	DEBUG ${sep}range${sep}\aw${maEnv.Find[stMobLvlMin].Value}\ax-\aw${maEnv.Find[stMobLvlMax].Value}\ax zradius${sep}\a-w${maCC.Find[stZRadius].Value}\ax

	/while (${_tmpAgroRange} <= PULLENV && !TARGET) {
		/delay 1

		| search local ENV range in steps of 10 for mob acquisition
		DEBUG ${sep}count:\aw${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}\ax radius${sep}\ay${_tmpAgroRange}\ax
		/if (${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}) {
			/for _count 1 to ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}
				ISMEDEAD

				/varset _tmpID ${NearestSpawn[${If[${_spCount},${Math.Calc[${_count}+${_spCount}]},${_count}]}, npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value}].ID}
				DEBUG ${sep}_tmpID${sep}\a-w${_tmpID}\ax

				| excluded mobs
				/if (${lsZoneExclude.Contains[${Spawn[id ${_tmpID}].DisplayName}]}) {
					DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsZoneExclude\ax (\a-r${Spawn[${_tmpID}].DisplayName}\ax)
					/continue
				}

				| animation checks
				/if (${lsNoTargetAnimation.Contains[${Spawn[id ${_tmpID}].Animation}]}) {
					DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetAnimation\ax (\a-r${Spawn[id ${_tmpID}].Animation}\ax)
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

				/if (${_tmpID} == ${Me.Pet.ID}) /continue

				| CYA for shit MQ will pick up in the environment that does not have a name
				/if (${Spawn[ID ${_tmpID}].DisplayName.Length} < 1) {
					DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wDisplayName.Length < 1\ax
					/varset _spCount 1
					/continue
				}



				| check navigation path validity for all but base pulling
				/if (!${Navigation.PathExists[id ${_tmpID}]} && !${maOver.Find[swOverNavCheck].Value} && !${Select[${maCC.Find[stPullMode].Value},base]}) {
					SDEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wNavigation.PathExists[\a-yid ${_tmpID}\ax]\ax (\a-r${Spawn[id ${_tmpID}].DisplayName}\ax)
					/varset _spCount 1
					/continue
				}

				/varset _navRange ${Navigation.PathLength[id ${_tmpID}]}

				| sort by closest nav path length
				/if (${_navRange} <= ${_closestNAV}) {
					/varset _closestNAV ${_navRange}
				} else /if (${_navRange} > ${_closestNAV}) {
					/continue
				}



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



|***
 * note: memorize a spell/song to a specific gem if not memed
 * use: /call set_spell_to DEBUG [echo memming T/F] ["spell name"] [gem to mem it in] [wait for recycle to cast T/F]
 ***|
sub set_spell_to(bool _debug, bool _echo, string _spellname, string _spellgem, bool _wait)
	DEBUG \atset_spell_to\ax(\a-w${_echo}, ${_spellname}, ${_spellgem}, ${_wait}\ax)
	FLOW set_spell_to

 	/if (${Me.Gem[${_spellname}]}) /return TRUE
	ISMEDEAD
	GETINPUT set_spell_to NA|NA

	/declare _ready bool FALSE

	| cant open a book with shit in hands
	/if (${Cursor.ID}) {
		/call check_cursor ${_debug} CLEAR
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
 * note: corrects the Gem## and returns the # only #
 * use: /call get_gem_id DEBUG [gem##] | ${get_gem_id[DEBUG, gem##]}
 ***|
sub get_gem_id(bool _debug, string _spellGem)
	DEBUG \atget_gem_id\ax(\a-w${_spellGem}\ax)
	FLOW get_gem_id

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
	DEBUG \atcheck_exit\ax()
	FLOW check_exit::${_call}

	| invis
	/if (${Me.Invis}) {
		/return TRUE
	}

	| movement
	/if (${Me.Moving} && !${Select[${Me.Class.ShortName},BRD]}) {
		/return TRUE
	}

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

	FLOW check_exit::${_call} (exit)
/return FALSE



|***
 * note: Check to see if toon is engaged in any way.
 * use: /call check_engaged DEBUG || ${check_engagedDEBUG}
 ***|
sub check_engaged(bool _debug)
	DEBUG \atcheck_engaged\ax()
	FLOW check_engaged

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
	FLOW do_rest

	| the dead dont need to rest
	/if (!AUTO || ${swIsMeDead}) /return FALSE

	| dont rest while in tie
	/if (TIE) /return FALSE

	| clerics and pallys wait 'till yaulp gone
	/if (${Me.Buff[Yaulp].ID}) /return
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
		/return
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
			/invoke ${out[5, "Lullaby${sep}\a-w${Spell[${maChr.Find[stSongRest].Value}].RankName}\ax"]}
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
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
				/if (${maChr.Find[swHarmonious].Value} && !${timer_Check_Harmonious}) /call check_harmonious

			} else /if (${Select[${Me.Class.ShortName},RNG]}) {
			} else /if (${Select[${Me.Class.ShortName},BST]}) {
				/if (!${timer_Check_Para}) /call check_mana_BST ${maDebug.Find[para].Value}
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
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
			} else /if (${Select[${Me.Class.ShortName},ENC]}) {
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
			} else /if (${Select[${Me.Class.ShortName},WIZ]}) {
			} else /if (${Select[${Me.Class.ShortName},NEC]}) {
				/if (PET && !${timer_Check_Minion_Cycle}) /call check_minion_cycle ${maDebug.Find[petbuild].Value}
			} else /if (${Select[${Me.Class.ShortName},MAG]}) {
				/if (${maChr.Find[stPctManaRecovery].Value}) /call do_mana_recovery ${maDebug.Find[manarecover].Value}
				/if (${maChr.Find[stPctGather].Value} && !${timer_Check_Gather}) /call check_gather_${Me.Class.ShortName} ${maDebug.Find[gather].Value}
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
 * note: Acquire target, get bitches. This will establish ${stAssTarID} for everyone.
 * use: /call get_assist_target DEBUG
 ***|
sub get_assist_target(bool _debug)
	DEBUG \atget_assist_target\ax()
	FLOW get_assist_target

	/if (!AUTO) /return

	ISMEDEAD
	GETINPUT get_assist_target NA|NA

	| if were not assisting.. don't assist
	/if (!${Bool[MA1]} && !${Bool[MA2]} && !${Select[${maCC.Find[stAssistMode].Value},g,1,2,3]}) {
		DEBUG ${sep}\arNO\ax assist
		/delay 1
		/return FALSE
	}

	| cannnot assist yourself
	/if (${Group.MainAssist.Name.Equal[${Me.DisplayName}]}) {
		DEBUG ${sep}cannot assist self
		/return FALSE
	}

	/if (${Spawn[MA1].ID} == ${Me.ID} || ${Spawn[=${Raid.MainAssist}].ID} == ${Me.ID}) {
		/invoke ${do_raw_edit[FALSE, TRUE, stManualAssist1, FALSE]}
	}
	/if (${Spawn[MA2].ID} == ${Me.ID} || ${Spawn[=${Raid.MainAssist}].ID} == ${Me.ID}) {
		/invoke ${do_raw_edit[FALSE, TRUE, stManualAssist2, FALSE]}
	}

	| if we dont have xtargetsd, skip ALL smartassist go directly to jail.
	/if (!${Me.XTargetSlots}) {
		/goto :skippingtoMAtarget
	}

	| tar acquisition when using smart assist. set XTarget positions.
	/if (${Select[${maCC.Find[stAssistMode].Value},g,1,2,3]} && ${Bool[${Group.MainAssist}]}) {
		DEBUG ${sep}Smart Assist${sep}\a-w${maCC.Find[stAssistMode].Value}\ax

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
		/if (${Spawn[ID ${stAssTarID}].Distance} <= ENV && ${stAssTarID}) {
			/if (MELEE || RANGE || NUKE || DOT) {
				/if (${Me.XTarget[1].PctHPs} <= ${maCC.Find[stPctEngage].Value}) {
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

	| target acquisition when not using manual assist.
	:skippingtoMAtarget
	/if ((${Bool[MA1]} || ${Bool[MA2]}) || !${Bool[${Group.MainAssist}]}) {
	DEBUG ${sep}Manual Assist${sep}\a-w${maCC.Find[stAssistMode].Value}\ax

		| assist ma1
		/if (${Spawn[MA1].ID} && !${Spawn[MA1].Dead}) {
			DEBUG ${sep}assisting${sep}\a-wMA1\ax
			/if (MELEE || RANGE || NUKE || DOT) {
				/squelch /assist MA1
			}

		| assist ma2
		} else /if (${Spawn[MA2].ID} && !${Spawn[MA2].Dead} && (!${Spawn[MA1].ID} || ${Spawn[MA1].Dead})) {
			DEBUG ${sep}assisting${sep}\a-wMA2\ax
			/if (MELEE || RANGE || NUKE || DOT) {
				/squelch /assist MA2
			}
		}

		/delay 2s ${Me.AssistComplete}
		/if (!TARGET && ${Target.ID}) {
			/varset stAssTarID ${Target.ID}
		} else /if (TARGET != ${Target.ID} && TARGET != 0 && ${Target.ID} && ${Spawn[${Target}].Type.Equal[npc]}) {
			/delay 1
			/varset stAssTarID 0
			/return TRUE
		}

	}

	ISMEDEAD
	GETINPUT
	/if (TARGET == ${Target.ID}) {
		/if (${Target.PctHPs} > ${maCC.Find[stPctEngage].Value} && !${maOver.Find[swOverPctEngage].Value}) {
			/delay 5
			/goto :skippingtoMAtarget
		}
	} else {
		/if (${Target.ID}) {
			/squelch /target clear
		}
		/return TRUE
	}


/return



|***
 * note: creates hooks for data\custom.inc
 * use: GETINPUT [sub] [variable]
 ***|
sub register_hook(string _sub, string _variable)
	FLOW register_hook

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
	FLOW set_zone_lists

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
		/invoke ${maEnv.Add[swZoneSafe,FALSE]}
	} else {
		/sqlite ${maData.Find[DBzone].Value} dummy SELECT value from zone_control where zone_shortname='${Zone.ShortName}' and element='swZoneSafe'
		/invoke ${maEnv.Add[swZoneSafe,${sqlite.Result[dummy 1 value]}]}
	}

	| load zone properties
  /declare _li listiterator local
	/vardata _li lsZoneProperty.First.Clone
	/while (!${_li.IsEnd}) {
		/invoke ${${_li.Value}.Clear}

		/sqlite ${maData.Find[DBzone].Value} dummy SELECT count(*) as data from zone_control where zone_shortname='${Zone.ShortName}' and element='${_li.Value}'
		| /echo ${sqlite.ResultCode[dummy]}

		| /if (${sqlite.ResultCode[dummy]} != 0) {
		| 	/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM sql_error WHERE env_var="${sqlite.ResultCode[dummy]}"

		| 	/invoke ${_li.Advance}
		| 	/continue
		| }

		/if (${sqlite.Result[dummy 1 data]} > 0) {
			/sqlite ${maData.Find[DBzone].Value} dummy SELECT group_concat(value) as data FROM zone_control WHERE zone_shortname = '${Zone.ShortName}' and element = '${_li.Value}' group by element
			/invoke ${${_li.Value}.Append[${sqlite.Result[dummy 1 data]}]}
		}
	/invoke ${_li.Advance}
	}

/return TRUE



|***
 * note: find a variables mapping
 * use: ${get_key_map[DEBUG, variable, validate t/f]} returns the mapname
 ***|
sub get_key_map(bool _debug, string _key, bool _validate)
	DEBUG\atget_key_map\ax(\a-w${_key}\ax)
	FLOW get_key_map

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
	DEBUG\atget_key_DBLOC\ax(\a-w${_key}\ax)

	/declare _mikey mapiterator local
	/vardata _mikey maDBTables.First.Clone
	/while (!${_mikey.IsEnd}) {
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM '${_mikey.Key}' WHERE ${_mikey.Value}='${_key}'
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
	DEBUG \atdo_raw_edit\ax(\aw${_silent}, ${_variable}, ${Parse[1,${_value}]}, ${_add_delete}, ${_verbage}\ax)
	FLOW do_raw_edit

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
		/invoke ${out[16, "${notice}${sep}\ar/edit\ax${sep}invalid list option${sep}\a-w[add|delete|clear]\ax"]}
		/return FALSE
	}

	| errors for values
	/if (!${_value.Length}) {
		/invoke ${out[16, "${notice}${sep}\ar/edit\ax${sep}invalid value${sep}[\a-wname of thing|gem#|cursor|PREGEN\ax]"]}
		/return FALSE
	}

	| ** end of error control .. start of actually getting something fucking done. ** |

	| does it have a ?
	/declare _map string local ${get_key_map[${_debug}, ${_variable}]}
	/declare _list bool local FALSE
	/declare _found bool local FALSE


	| environment_var edits. not for users...
	/if (${_DBLOC.Arg[1,|].Equal[environment_var]}) {

		/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}"
		DEBUG ${sep}dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}" \ay${sqlite.Status[dummy]}\ax
		/varset ${_variable} ${_value}
		/return TRUE

	| set value as a gem
	} else /if (${_value.Find[gem]} && ${_value.Length} < 6) {

		/declare _gemN int local ${get_gem_id[FALSE, ${_value}]}

		/if (!${Range.Between[1,${Me.NumGems}:${_gemN}]}) {
			/invoke ${out[16, "1-${Me.NumGems} only for the gem option"]}
			/invoke ${out[16. "ex: \aw/edit\ax [${num}variable\ax] [gem${num}#\ax] \a-w[add|delete]\ax"]}
			/return FALSE
		}

		| adjust to the loaded spell
		/varset _value ${Me.Gem[${_gemN}].BaseName}

	| set value as cursor
	} else /if (${_value.Equal[cursor]}) {
		/if (!${Cursor.ID}) {
			/invoke ${out[16, "\a-wcursor\ax requires an object on your cursor"]}
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

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_variable}'
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

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var="${_variable}"
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

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_variable}'
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
		/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}"
		DEBUG ${sep}dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}" \ay${sqlite.Status[dummy]}\ax
		| /sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE ${_DBLOC.Arg[1,|]} SET value=${Parse[1,"${_value}"]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}"

		| reload data
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}"
		DEBUG ${sep}dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE ${_DBLOC.Arg[2,|]}="${_variable}" \ay${sqlite.Status[dummy]}\ax
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
				OUT /edit ${_variable}${sep}\a-w${Parse[1, ${sqlite.Result[dummy 1 value]}]}\ax
			}

		} else /if (${_found}) {
			DEBUG FOUND output
			/if (${lsGrain.Contains[24]}) {
				OUT /edit ${_variable}\ax${sep}${num}${Parse[0,"${_value}"]}\ax
			}
		} else {

		}

	} else /if (${sqlite.Status[dummy].NotEqual[Success]}) {
		| /invoke ${out[16, "${warning} \arERROR\ax${sep}\atdo_raw_edit\ax(\aw${_silent}, "${_variable}"", ${Parse[1,${_value}]}, ${_add_delete}, ${_verbage}\ax)"]}
		/echo ${warning} \arERROR\ax${sep}\atdo_raw_edit\ax(\aw${_silent}, ${_variable}, ${Parse[1,${_value}]}, ${_add_delete}, ${_verbage}\ax)
	}

/return	TRUE



|***
 * note: sets the spires a character will use
 * use: /call set_control_chr_spire [debug] [group|raid] [NOTUSED]
 ***|
sub set_control_chr_spire(bool _debug, string _type, string _verbage)
	DEBUG \atset_control_chr_spire\ax(\a-w${_type}\ax)
	FLOW set_control_chr_spire

  /declare _out string local

	/if (${_type.Equal[group]}) {
		/invoke ${set_switch_env[${_debug}, swSpireGroup, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[raid]}) {
		/invoke ${set_switch_env[${_debug}, swSpireRaid, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[con]}) {
		/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSpireCon" "${_verbage}"


	}

	/invoke ${out[12, "/chr spire${sep}"]}
	/invoke ${out[12, "${dot} ${info}group\ax:${If[${maChr.Find[swSpireGroup].Value},${on},${off}]}"]}
	/invoke ${out[12, "${dot} ${info}raid\ax:${If[${maChr.Find[swSpireRaid].Value},${on},${off}]}"]}

	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maChr.Find[stSpireCon].Value}"
	/if (${sqlite.Rows[dummy]}) {
		OUT ${dot} ${info}con\ax${sep}\a-w${maChr.Find[stSpireCon].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
	} else {
		OUT ${dot} ${info}con\ax${sep}\a-w${maChr.Find[stSpireCon].Value}\ax
	}


/return TRUE



|***
 * note: sets healpoints
 * use: /call set_control_heal_point [debug] [%## heal at]
 ***|
sub set_control_heal_point(bool _debug, int _verbage)
	DEBUG \atset_control_heal_point\ax(\a-w${_verbage}\ax)
	FLOW set_control_heal_point

	/declare _out string local

	| non healers (self only)
	/if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
		/if (${set_control_num_range[${_debug}, stHealPoint${Me.Class.ShortName}, "${_verbage}", 1, 99]}) {
			/invoke ${out[12, "/healpoint ${dot} ${num}${_verbage}\ax"]}
		}
		/return

	| Healer Classes
	} else /if (${lsClassHeal.Contains[${Me.Class.ShortName}]} && ${Bool[${_verbage}]}) {
		/invoke ${set_control_num_range[${_debug}, stHealPoint${Target.Class.ShortName}, "${_verbage}", 1, 99]}
	}

	/declare _lichp listiterator local
	/vardata _lichp lsClassAll.First.Clone
	/while (!${_lichp.IsEnd}) {
		/varset _out ${_out} ${If[${_lichp.Value.Equal[${lsClassAll.First}]},,${dot}]} ${info}${_lichp.Value}\ax:${num}${maHeal.Find[stHealPoint${_lichp.Value}].Value}\ax
		/invoke ${_lichp.Advance}
	}

	| /varset _out ${_out} ${dot} ${info}HOT\ax:${num}${maHeal.Find[stHealPointHoT].Value}\ax

	/invoke ${out[12, "/heal point ${_out}"]}


/return FALSE



|***
 * note: sets the XTarget Heal classes a character will use
 * use: /call set_character_XTHealClass [debug] [class short name|TARGET]
 ***|
sub set_control_chr_xtclass(bool _debug, string _verbage)
	DEBUG \atset_control_chr_xtclass\ax(\a-w${_verbage}\ax)
	FLOW set_control_chr_xtclass

	| fix user entry
	/if (!${Bool[${_verbage}]} && ${lsClassAll.Contains[${Target.Class.ShortName}]}) {
		/varset _verbage ${Target.Class.ShortName}
	} else /if (${Bool[${_verbage}]} && ${lsClassAll.Contains[${_verbage.Upper}]}) {
		/varset _verbage ${_verbage.Upper}
	} else {
		/invoke ${out[0, "usage /heal xtclass [\a-wTarget\ax|\a-wClass ShortName\ax]"]}
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
		/invoke ${_lisccx.Advance}
	}
	/invoke ${out[12, "/chr xtclass ${_out}"]}

/return



|***
 * note: make sure you have the correct zone INI info loaded
 * use: /call check_zone_id DEBUG [force T/F]
 ***|
sub check_zone_id(bool _debug, bool _force)
	DEBUG \atcheck_zone_id\ax(\a-w${_force}\ax)
	FLOW check_zone_id

	GETINPUT check_zone_id stCurrentZoneID|${maEnv.Find[stCurrentZoneID].Value}

	/if (${timer_Check_Zone_ID} && !${_force}) {
		/return FALSE
	}

	/invoke ${set_data_timer[${_debug}, Check_Zone_ID, RESTART]}
	/declare _zoned bool local FALSE

	| zone matches. nothing to see here. move on
	/if (${maEnv.Find[stCurrentZoneID].Value} == ${Zone.ID} && !${_force}) /return TRUE

	| zone does not match. wtf..
	/if (${maEnv.Find[stCurrentZoneID].Value} != ${Zone.ID} || ${_force}) {
		/if (${lsGrain.Contains[1]}) {
			OUT ${num}${Zone.Name}\ax${sep}\a-w${Zone.ShortName}\ax
		}

		/call set_zone_lists FALSE
		/varset swIsMeDead FALSE
		/varset _zoned TRUE
		/doevents flush

		| check for mesh
		/if (!${Navigation.MeshLoaded}) {
			/invoke ${out[1, "${warning} ${dot} NO Mesh for \a-w${Zone.ShortName}\ax"]}
		}

		| make it right with the world
		/invoke ${maEnv.Add[stCurrentZoneID,${Zone.ID}]}

	}

	/return
	| yah. we zoned and stuff
	/if (${_zoned}) {

		/call set_stop ${maDebug.Find[stop].Value} SILENT
		/call clear_combat
		/invoke ${target[clear]}
		FLOW check_zone_id

		/if (${maBurn.Find[swBurnForce].Value}) {
			/invoke ${maBurn.Add[swBurnForce,FALSE]}
		}
		/if (HOME) /call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
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
	FLOW check_expaa_adjust

	/invoke ${set_data_timer[${_debug}, Check_EXP_Adjust, RESTART]}

	| set EXP cause we deaded too much
	/if (${Me.Level} <= ${maEnv.Find[stLvlMax].Value} && ${Me.PctExp} <= ${maEnv.Find[stLvlPct].Value}) {
		/invoke ${out[13, "EXP below maintain level${sep}\a-w${maEnv.Find[stLvlMax].Value}\ax${sep}\a-w${maEnv.Find[stLvlPct].Value}%\ax"]}
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
	FLOW check_last_supper

	/invoke ${set_data_timer[FALSE, Check_FoodDrink, RESTART]}

	| for a frame of reference hunger and thirst seem to go to ~10k
	| one bite of black bread from a cleric is 500
	| one drink of a kadim constitution is almost 6k

	| eat, drink and be merry
	/if (${FindItemCount[=${maEnv.Find[stFood].Value}]} && ${Me.Hunger} < 3000) {
		/invoke ${out[0, "Hungry${sep}\a-w${maEnv.Find[stFood].Value}\ax"]}
		/useitem "${maEnv.Find[stFood].Value}"
	}
	/if (${FindItemCount[=${maEnv.Find[stDrink].Value}]} && ${Me.Thirst} < 3000) {
		/invoke ${out[0, "Thirsty${sep}\a-w${maEnv.Find[stDrink].Value}\ax"]}
		/useitem "${maEnv.Find[stDrink].Value}"
	}

/return TRUE



 |***
 * note: will loosly tie to a specific toon. will moove close to the toon when it moves. set to timer [Timers] -> CheckTieTime
 * use: /tie [on/off toggle] requires /tc name [NAME] and /tc variance ##
 **|
sub set_tie(bool _debug)
	DEBUG \atset_tie\ax()
	FLOW set_tie

	/if (AUTO) {
		/invoke ${set_data_timer[FALSE, Check_Tie, RESTART]}
	}

	/if (!${Bool[${maTie.Find[stTieToon].Value}]}) {
		/return FALSE
	}

	/declare _ID int local
	/varset _ID ${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}

	DEBUG _ID${sep}\a-w${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}\ax

	/if (HOME) {
		/call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
	}

	ISMEDEAD
	GETINPUT set_tie NA|NA

	| reasons to not tie to a toon
	/if (${maTie.Find[swSetTie].Value} && ${_ID} && ${Spawn[ID ${_ID}].Dead}) {
		/return
	}

	/if (${Me.Combat}) {
		/return
	}

	/if (${Spawn[pc ${maTie.Find[stTieToon].Value}].Distance} <= ${maTie.Find[stTieVariance].Value}) {
		/return TRUE
	}

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
			/invoke ${out[0, "NO Nav Mesh for this zone. Returning to Stick."]}
			/call Bind_control_tc mode stick
		}

	| MQ2MoveUtils mode
	} else /if (${maTie.Find[stTieMode].Value.Equal[stick]}) {
		DEBUG ${sep}mode${sep}\a-wstick\ax
		| all present and or accounted for SIR!
		/if (${Spawn[ID ${_ID}].Distance} > ${maTie.Find[stTieVariance].Value}) {
			/if (${Target.ID}) {
				/squelch /target clear
			}

			/echo here
			/squelch /face ID ${_ID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
			/squelch /stick ID ${_ID} ${maTie.Find[stTieVariance].Value} UW
		}

		:okweresticking
		ISMEDEAD
		GETINPUT set_tie_stick NA|NA

		/if (${Spawn[ID ${_ID}].Distance} <= ${maTie.Find[stTieVariance].Value}) {
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

/return TRUE



|***
 * note: cast classes AA fade
 * use: /call cast_AA_fade
 ***|
sub cast_AA_fade(bool _debug)
	DEBUG \a-tcast_AA_fade\as()
	FLOW cast_AA_fade

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
	FLOW check_buff_cycle_now (1)

	/invoke ${set_data_timer[${_debug}, Check_Buff_Now, RESTART]}

	/if (AUTO) {
		/if (!${maBuff.Find[swBuffMaster].Value}) {
			/return FALSE
		}
	}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT check_buff_cycle_now
	FLOW check_buff_cycle_now (2)
	GETINPUT check_buff_cycle_now NA|NA

	/declare _spell string local
	/declare _targetID int local 0
	/declare _targetIDorg string local NULL
	/declare _licountself listiterator local
  /vardata _licountself lsCount50.First.Clone
  /while (!${_licountself.IsEnd}) {

		| no name set?
		/if (!${Bool[${maBuff.Find[stBuff${_licountself.Value}].Value}]}) {
			| DEBUG ${sep}break.no name set:${_licountself.Value}
			/invoke ${_licountself.Advance}
			/continue
		}

		| buff on/off switch
		/if (!${maBuff.Find[swBuff${_licountself.Value}].Value}) {
			| DEBUG ${sep}break.switched off:${_licountself.Value}
			/invoke ${_licountself.Advance}
			/continue
		}

		| alwaysup tag needed
		/if (!${lsBuff${_licountself.Value}Tag.Contains[alwaysup]}) {
			| DEBUG ${sep}break. no alwaysup tag
			/invoke ${_licountself.Advance}
			/continue
		}

		| is it ready
		/if (!${is_ready[${_debug}, "${maBuff.Find[stBuff${_licountself.Value}].Value}"]}) {
			DEBUG ${sep}break. not ready
			/invoke ${_licountself.Advance}
			/continue
		}

		| condition check
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maBuff.Find[stBuff${_licountself.Value}Con].Value}]}) {
			DEBUG ${sep}break.condition
			/invoke ${_licountself.Advance}
			/continue
		}

		| tag check
		/call check_basic_tags ${maDebug.Find[tag].Value} Buff${_licountself.Value} ${Me.DisplayName} "${maBuff.Find[stBuff${_licountself.Value}].Value}"
		/if (!${Macro.Return}) {
			DEBUG ${sep}break.tag
			/invoke ${_licountself.Advance}
			/continue
		}

		DEBUG ${sep}Buff#: ${_licountself.Value}${sep}\a-w${maBuff.Find[stBuff${_licountself.Value}].Value}\ax
		/varset _spell ${maBuff.Find[stBuff${_licountself.Value}].Value}

		/if (${Me.AltAbilityReady[${_spell}]} || ${Me.Book[${Spell[${_spell}].RankName}]} || ${Me.CombatAbilityReady[${Me.CombatAbility[${Spell[${_spell}].RankName}]}]}) {

			/if (${Spell[${_spell}].NumEffects} == 1)  {
				/varset _spell ${Spell[${Spell[${_spell}].Trigger[1]}].RankName}
				DEBUG ${sep}_spell${sep}\a-w${Spell[${Spell[${_spell}].Trigger[1]}].RankName}\ax
			}

		} else /if (${Me.ItemReady[=${_spell}]}) {
			/varset _spell ${FindItem[${_spell}].Clicky}

		}

		| DEBUG ${sep}${_spell} :: ${Me.Buff[${_spell}].ID} || ${Me.Song[${_spell}].ID}

		| do we have it, now that we know what it is?
		/if (${Me.Buff[${_spell}].ID} || ${Me.Song[${_spell}].ID}) {
			DEBUG ${sep}break.have buff/song
			/invoke ${_licountself.Advance}
			/continue
		}

		| remember target
		| /if (!${Select[${Target.ID},NULL]}) {
		| 	/varset _targetIDorg ${Target.ID}
		| }

		/if (${Spell[${_spell}].TargetType.Equal[single]}) {
			/varset _targetID ${Me.ID}
		} else {
			/varset _targetID 0
		}

		/call voodoo ${_debug} "${maBuff.Find[stBuff${_licountself.Value}].Value}" ${_targetID} FALSE

		/if (${lsBuff${_licountself.Value}Tag.Contains[autoinventory]}) {
			/call check_cursor ${_debug} CLEAR
		}

		| return target
		| /if (${Select[${_targetIDorg},NULL]}) {
		| 	/squelch /target clear
		| } else /if (!${Select[${_targetIDorg},NULL]}) {
		|
		| }

		DEBUG ${sep}next iteration
		/invoke ${_licountself.Advance}
	}

/return TRUE



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
	FLOW check_buff_cycle_unity (1)

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
	FLOW check_buff_cycle_unity (2)
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
 * note: Check for running auras, If they are duped, remove the second one
 * use: /call check_aura

sub check_buff_cycle_aura2(bool _debug)
	FLOW check_buff_cycle_aura (1)

	SETHUD "buff aura"

	/if (AUTO) {
		/invoke ${set_data_timer[${_debug}, Check_Buff_Aura, RESTART]}
		/if (!${maBuff.Find[swBuffMaster].Value}) {
			/return FALSE
		}
	}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT check_buff_cycle_aura
	FLOW check_buff_cycle_aura (2)
	GETINPUT check_buff_cycle_aura NA|NA

	| set aura 1
	/declare _a1 		string local ${set_rank[${_debug}, ${maBuff.Find[stAura1].Value}]}
	/declare _a1c 	string local ${maBuff.Find[stAura1Check].Value}

	| set aura 2
	/declare _a2 		string local ${set_rank[${_debug}, ${maBuff.Find[stAura2].Value}]}
	/declare _a2c 	string local ${maBuff.Find[stAura2Check].Value}

	| Aura: berzerker, monk, warrior
	/if (${check_class[FALSE, |BER|MNK|WAR]}) {
		DEBUG ${sep}(\a-rBER|MNK|WAR\ax)
		/if (${Bool[${_a1c}]} && !${Bool[${Me.Aura[1].Name.Equal[${_a1c}]}]}) {
			/call check_basic_tags ${maDebug.Find[tag].Value} Aura1 ${Me.DisplayName} "${_a1c}"
			/if (!${Macro.Return}) /return FALSE
			/call voodoo FALSE "${_a1}" 0 FALSE
		}
		/return TRUE
	}

	| Aura: Bard
	/if (${check_class[FALSE, |BRD]}) {
		DEBUG ${sep}(\a-rBRD\ax)
		/if (${Bool[${_a1c}]} && !${Bool[${Me.Aura[1].Name.Equal[${_a1c}]}]}) {
			/call check_basic_tags ${maDebug.Find[tag].Value} Aura1 ${Me.DisplayName} "${_a1c}"
			/if (!${Macro.Return}) /return FALSE
			/if (${is_ready[${_debug}, "${_a1}"]}) {
				/call voodoo ${_debug} "${_a1}" 0 FALSE
			}
		}
		/return TRUE
	}

	| everyone else...
	/if (!${check_class[FALSE, |BRD|BER|MNK|WAR]}) {
		DEBUG ${sep}(\a-rEVERYONE ELSE\ax)
		/declare _count int local 0
		/declare _aura int local 0
		/declare _found1 bool FALSE
		/declare _found2 bool FALSE

		| check auras entered in database 1/2
		/for _aura 1 to 2
			DEBUG ${sep}\a-y------\axLOOP#\aw${_aura}\ax
			DEBUG ${sep}Aura ${_aura}${sep}\a-w${_a1}\ax
			DEBUG ${sep}Aura ${_aura} check${sep}\a-w${_a1c}\ax

			| check each arua for each slot
			/if (${Bool[${_a${_aura}}]}) {
				/for _count 1 to 2
					/if (${Bool[${_a${_aura}c}]}) {
						/call check_basic_tags ${maDebug.Find[tag].Value} Aura${_count} ${Me.DisplayName} "${_a${_aura}}"
						/if (!${Macro.Return}) {
							/varset _found${_aura} TRUE
							/continue
						}

						DEBUG ${sep}/if Me.Aura[${_count}].Name.Equal[${_a${_aura}c}]
						/if (${Me.Aura[${_count}].Name.Equal[${_a${_aura}c}]}) {
							DEBUG ${sep} _found${_aura} TRUE
							/varset _found${_aura} TRUE

							DEBUG ${sep}/if Me.Aura[${_count}].Name.Equal[${_a${_aura}}]
						} else /if (${Me.Aura[${_count}].Name.Equal[${_a${_aura}}]}) {
							DEBUG ${sep} _found${_aura} TRUE
							/varset _found${_aura} TRUE
						}

					}
					/if (${_found${_aura}}) /break
				/next _count

				DEBUG ${sep}_found${_aura}${sep}\a-w${_found${_aura}}\ax
				/if (!${_found${_aura}}) {
					/call voodoo ${_debug} "${_a${_aura}}" 0 FALSE
					DEBUG ${sep}cR${sep}\a-w${maData.Find[stCastReturn].Value}\ax
					/if (${maEntropy.Find[swBuffCooldownWait].Value}) {
						DEBUG ${sep}buff cooldown recycle
						/while (${Me.SpellInCooldown}) {
							CHECKEXIT
							ISMEDEAD
							/if (!${Me.SpellInCooldown}) /break
							/delay 5
						}
					}

				} else /if (${_found${_aura}}) {
					/varset _found${_aura} FALSE
				}

			}
		/next _aura
	}

	SETHUD EMPTY

/return TRUE
 ***|




|***
 * note: do all the damn buffs
 * use: /call check_buff_cycle DEBUG <FORCE>
 ***|
sub check_buff_cycle(bool _debug, bool _force, string _verbage)
	DEBUG \atcheck_buff_cycle\ax(\a-wforce:${_force}, verb:${_verbage}\ax)
	FLOW check_buff_cycle

	/if (AUTO) {
		/invoke ${set_data_timer[${_debug}, Check_Buff_Cycle, RESTART]}
		/if (!${maBuff.Find[swBuffMaster].Value}) {
			/return FALSE
		}
	}

	/if (${_force}) {
		DEBUG ${sep}forced /cb${sep}\ao${_verbage}\ax
		/declare _buffcycletimestart int local ${MacroQuest.Running}
		/if (AUTO && !${maBuff.Find[swBuffMaster].Value}) {
			/invoke ${out[0, "Master Buff Switch${sep}${off}"]}
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

	| beg
	/if ((${maBuff.Find[swBuffBeg].Value} && AUTO && !${timer_Check_Buff_Beg}) || (${_verbage.Equal[b]} && ${_force})) {
		/call check_buff_cycle_beg ${If[${_force},${_debug},${maDebug.Find[beg].Value}]}
	}

	| powersource
	/if (${maBuff.Find[swBuffPowersource].Value} && ((AUTO && !${timer_Check_Buff_Powersource}) || ${_force})) {
		/call check_buff_cycle_powersource ${If[${_force},${_debug},${maDebug.Find[power].Value}]}
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
	FLOW check_buff_cycle_powersource

	/invoke ${set_data_timer[${_debug}, Check_Buff_Powersource, RESTART]}

	| /if (${Me.Inventory[powersource].Power}) /return FALSE

	/declare _psName string local FALSE

	/declare _count int local 0
	/for _count 1 to 50
		/if (!${lsBuff${_count}Tag.Contains[powersource]}) {
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
			/call voodoo FALSE "${_psName}" 0 FALSE
			/delay 5
			/autoinventory
		} else /if (!${FindItem[${_psName}].ID}) {
			/return FALSE
		}

	}	else {
		/return FALSE
	}

/return TRUE



|***
 * note: Mana recovery items; rods, shards, crack pipes
 * use: /call do_mana_recovery DEBUG
 ***|
sub do_mana_recovery(bool _debug)
	DEBUG \atdo_mana_recovery\ax()
	FLOW do_mana_recovery (1)

	/if (!${maChr.Find[stPctManaRecovery].Value}) {
		/return FALSE
	}

	/if (SAFEZONE) {
		/return FALSE
	}


	/if (${Me.PctMana} > ${maChr.Find[stPctManaRecovery].Value}*.PCTHO) {
		/return FALSE
	}

	/if (!${lsClassCast.Contains[${Me.Class.ShortName}]}) {
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT do_mana_recovery
	FLOW do_mana_recovery (2)
	GETINPUT do_mana_recovery NA|NA

	| you can take that rod.. and shove it right up you...
	/declare _tmpID int local 0
	/declare _midmc mapiterator local
  /vardata _midmc maManaRecovery.First.Clone
  /while (!${_midmc.IsEnd}) {

		/if (${Me.PctHPs} < ${_midmc.Value}) {
			/if (${is_ready[${_debug}, "${_midmc.Key}"]}) {
		  	DEBUG ${_midmc.Key}${sep}\a-w${_midmc.Value}\ax

		  	/if (${Select[${Spell[${_midmc.Key}].RankName.TargetType},Self,AE PC v2]}) {
		  		/varset _tmpID 0
		  	} else /if (${Select[${Spell[${_midmc.Key}].RankName.TargetType},Single]}) {
		  		/varset _tmpID ${Me.ID}
		  	}

				/call voodoo ${_debug} "${_midmc.Key}" ${_tmpID} FALSE

				/if (!${Select[${Me.Class.ShortName},MAG]} && !${FindItemCount[=${_midmc.Key}]}) {
					| /call tell_resident_mage shard
					| ${get_resident_class[FALSE, MAG, 100]}
				}
				/break
			}
		}

		/invoke ${_midmc.Advance}
  }
	FLOW do_mana_recovery (exit)

/return TRUE



|***
 * note: Cleric/Paladin AA Yaulp
 * use: /call cast_yaulp
 ***|
sub cast_yaulp(bool _debug)
	DEBUG \atcast_yaulp\ax()
	FLOW cast_yaulp (1)

	/invoke ${set_data_timer[FALSE, Check_Yaulp, RESTART]}

	/if (!${maBuff.Find[swBuffMaster].Value}) /return FALSE

	/if (${Me.PctMana} > 98*.PCTHO) /return FALSE
	/if (${Me.Sitting} || ${Me.Mount.ID}) /return FALSE
	/if (${Me.Buff[Lesser Yaulp].ID} || ${Me.Buff[Yaulp].ID}) /return FALSE
	/if (!${is_ready[${_debug}, "${maChr.Find[stYaulp].Value}"]}) /return FALSE
	/if (!${Select[${Me.CombatState},combat]}) /return FALSE
	ISMEDEAD
	CHECKEXIT cast_yaulp
	FLOW cast_yaulp (2)
	/call voodoo ${_debug} "${maChr.Find[stYaulp].Value}" 0 FALSE

/return TRUE



|***
 * note: sets your target.. nothing more.. nothing less..
 * use: ${set_target[ID]} | /call set_target [ID]
 ***|
sub set_target(int _tmpID)
	FLOW set_target

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
	/delay 1s ${Target.ID} == ${_tmpID}

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
	FLOW check_buff_cycle_ammo (1)

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
	FLOW check_buff_cycle_ammo (2)
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
			/if (!${FindItemCount[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}]}) {
				/if (${is_ready[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
					/call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" 0 FALSE
				}
				/delay 1s
				/call Bind_command_swap "${FindItem[=${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}" 22 ${_debug}
			}
		}


		| /if (${is_ready[${_debug}, "${maBuff.Find[stBuff${_count}].Value}"]}) {
		| 	/call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" 0 FALSE
		| }
		| /delay 1s ${Me.Casting.ID}
		| /delay 15s !${Me.Casting.ID}

		| autoinventory?
		/if (${lsBuff${_count}Tag.Contains[autoinventory]}) {
			/call check_cursor ${_debug} CLEAR
		}

	/next _count

/return TRUE



${Spell[${maBuff.Find[stBuff3].Value}].Base[1]}


${Spell[${maBuff.Find[stBuff3].Value}].Trigger}





${FindItemCount[=${Spell[${maBuff.Find[stBuff3].Value}].Base[1]}]}






${Spell[${maBuff.Find[stBuff2].Value}].Trigger[1]}



/while (${Select[${Me.Animation},42,43,27]}) {
	/echo current animation is ${Me.Animation}
	/keypress left hold
	/keypress left
	/echo pressed left
	/delay 5
}





	/for _count 1 to ${Spell[${maBuff.Find[stBuff3].Value}].NumEffects}
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




































|***
 * note: poison clickie things
 * use: /call check_buff_cycle_poison DEBUG
 ***|
sub check_buff_cycle_leg_poison(bool _debug)
	DEBUG \atcheck_buff_cycle_leg_poison\ax()
	FLOW check_buff_cycle_leg_poison

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
			/autoinventory
		}

		/if (${Cursor.Name.Equal[${FindItem[${Spell[${FindItem[=${maBuff.Find[stBuff${_count}].Value}].Spell}].Base[1]}].Name}]}) {
			/autoinventory
		}

	/next _count

	SETHUD EMPTY

/return TRUE



|***
 * note: check self buffs
 * use: /call check_buff_cycle_self DEBUG
 ***|
sub check_buff_cycle_self(bool _debug)
	DEBUG \atcheck_buff_cycle_self\ax()
	FLOW check_buff_cycle_self (1)

	SETHUD "buff self"

	/if (AUTO) {
		/invoke ${set_data_timer[FALSE, Check_Buff_Self, RESTART]}
		/if (!${maBuff.Find[swBuffMaster].Value}) {
			/return FALSE
		}
	}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT check_buff_cycle_self
	FLOW check_buff_cycle_self (2)
	GETINPUT check_buff_cycle_self NA|NA

	/declare _count int local 0
	/declare _countD int local 0
	/declare _i int local 0
	/declare _found bool local
	/declare _spell string local FALSE

	/for _count 1 to 50
		DEBUG _count ${_count}

		/if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) {
			/continue
		}

		/if (!${maBuff.Find[swBuff${_count}].Value}) {
			/continue
		}

		/if (!${lsBuff${_count}Tag.Contains[self]}) {
			/continue
		}

		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maBuff.Find[stBuff${_count}Con].Value}]}) {
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Buff${_count} ${Me.DisplayName} "${maBuff.Find[stBuff${_count}].Value}"
		/if (!${Macro.Return}) /continue

		/varset _spell "${maBuff.Find[stBuff${_count}].Value}"

		/varset _found TRUE

		| trigger swap AAs... fuck DBG and their shit..
		/if (${Spell[${_spell}].NumEffects} == 1)  {
			DEBUG ${sep}\arSWAP\ax to trigger _spell\aw==\ax\a-w${Spell[${_spell}].Trigger[1]}\ax
			/varset _spell ${Spell[${Spell[${_spell}].Trigger[1]}].RankName}
		}

		| stacking / exist ?
		/if (${Spell[${_spell}].HasSPA[374]} || ${Spell[${_spell}].HasSPA[340]} || ${Spell[${_spell}].HasSPA[470]}) {
			DEBUG ${sep}\aw470\ax${sep}\a-w${Spell[${_spell}].HasSPA[470]}\ax ${dot} \aw374\ax${sep}\a-w${Spell[${_spell}].HasSPA[374]}\ax ${dot} \aw340\ax${sep}\a-w${Spell[${_spell}].HasSPA[340]}\ax
			DEBUG ${sep}\aweffects\ax${sep}\a-w${Spell[${_spell}].NumEffects}\ax

			/for _i 1 to ${Spell[${_spell}].NumEffects}
				DEBUG ${sep}\a-t${_i}/${Spell[${_spell}].NumEffects}\ax \aw::\ax ${Spell[${Spell[${_spell}].Base2[${_i}]}].Name} \aw::\ax ${Me.Buff[${Spell[${Spell[${_spell}].Base2[${_i}]}].Name}].ID} \aw::\ax D(\a-w${Spell[${Spell[${_spell}].Base2[${_i}]}].Duration}\ax)

				| SPA has no duration, skip it
				/if (!${Spell[${Spell[${_spell}].Base2[${_i}]}].Duration}) {
					DEBUG ${sep}\aw.Duration\ax break (${Spell[${Spell[${_spell}].Base2[${_i}]}].Name})
					/continue
				}

				| Does the SPA stack? if not, move on
				/if (!${Spell[${Spell[${_spell}].Base2[${_i}]}].Stacks}) {
					DEBUG ${sep}\aw.Stacks\ax break (${Spell[${Spell[${_spell}].Base2[${_i}]}].Name})
					/continue
				}

				| blocked?
				/if (${lsBlockedBuffsMe.Find[${Spell[${Spell[${_spell}].Base2[${_i}]}].ID}]}) {
					DEBUG ${sep}\aw.Blocked\ax break (${Spell[${Spell[${_spell}].Base2[${_i}]}].Name})
					/continue
				}

				| do we have the SPA
				/if (!${Me.Buff[${Spell[${Spell[${_spell}].Base2[${_i}]}].Name}].ID}) {
					DEBUG ${sep}have name${sep}\a-w${Me.Buff[${Spell[${Spell[${_spell}].Base2[${_i}]}].Name}].ID}\ax
					/varset _found FALSE
				}

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

		/if (${_found}) {
			/continue
		}

		/call voodoo FALSE "${maBuff.Find[stBuff${_count}].Value}" ${Me.ID} FALSE
		/if (${maEntropy.Find[swBuffCooldownWait].Value}) {
			DEBUG ${sep}buff cooldown recycle
			/while (${Me.SpellInCooldown}) {
				CHECKEXIT
				ISMEDEAD
				/if (!${Me.SpellInCooldown}) /break
				/delay 5
			}
		}

		/if (${lsBuff${_count}Tag.Contains[autoinventory]}) {
			/autoinventory
		}

		| drop illusions/mounts
		/if (${lsBuff${_count}Tag.Contains[drop]}) {
			DEBUG ${sep}tag-\a-ydrop\ax
			/delay 5s !${Me.Casting.ID}
			/for _countD 1 to ${FindItem[${_spell}].Spell.NumEffects}
				/if (${FindItem[${_spell}].Spell.Trigger[${_countD}].Name.Find[Illusion:]} || ${Me.Buff[${FindItem[${_spell}].Spell}].ID}) {
					| mounts
					/if (${Me.Buff[${FindItem[${_spell}].Spell}].ID}) {
						/invoke ${out[0, "Removing${sep}\a-w${FindItem[${_spell}].Spell}\ax"]}
						/removebuff ${FindItem[${_spell}].Spell}
						/delay 5
					}
					| illusions
					/if (${Me.Buff[${FindItem[${_spell}].Spell.Trigger[${_countD}]}].ID}) {
						/invoke ${out[0, "Removing${sep}\a-w${FindItem[${_spell}].Spell}\ax"]}
						/removebuff ${FindItem[${_spell}].Spell.Trigger[${_countD}]}
						/delay 5
					}
				}
			/next _countD
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
	FLOW check_buff_cycle_beg (1)

	SETHUD "buff beg"

	/if (AUTO) {
		/invoke ${set_data_timer[FALSE, Check_Buff_Beg, RESTART]}
		/if (!${maBuff.Find[swBuffMaster].Value}) {
			/return FALSE
		}
	}

	/declare _remove bool local FALSE

  /declare _mibb mapiterator local
  /vardata _mibb maBuffRequest.First.Clone
  /while (!${_mibb.IsEnd}) {

		CHECKTIE
		ISMEDEAD
		CHECKEXIT check_buff_cycle_beg
		FLOW check_buff_cycle_beg (2)
		GETINPUT check_buff_cycle_beg NA|NA

		/if (!${is_lsSafeNames[${maDebug.Find[safe].Value}, ${_mibb.Key}]}) {
			/invoke ${maBuffRequest.Remove[${_mibb.Key}]}
			/return FALSE
		}

		/declare _licountbeg listiterator local
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
				/call voodoo ${_debug} "${maBuff.Find[stBuff${_licountbeg.Value}].Value}" ${Spawn[${_mibb.Key}].ID} FALSE
			}

			/invoke ${_licountbeg.Advance}
		}


		/if (${_remove}) {
			/invoke ${maBuffRequest.Remove[${_mibb.Key}]}
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
	FLOW check_buff_cycle_crew (1)

	SETHUD "buff crew"

	/if (AUTO) {
		/invoke ${set_data_timer[FALSE, Check_Buff_Crew, RESTART]}
		/if (!${maBuff.Find[swBuffMaster].Value}) {
			/return FALSE
		}
	}

	CHECKTIE
	ISMEDEAD
	CHECKEXIT check_buff_cycle_crew
	FLOW check_buff_cycle_crew (2)
	GETINPUT check_buff_cycle_crew NA|NA

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
			| SDEBUG ${_count}

			/if (!${maBuff.Find[swBuff${_count}].Value}) /continue
			/if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) /continue

			| skip self only
			/if (${lsBuff${_count}Tag.Count} == 1 && (${lsBuff${_count}Tag.Contains[self]} || ${Spell[${_spell}].TargetType.Equal[Self]} || ${lsBuff${_count}Tag.Contains[minion]})) /continue

			/if (!${check_condition[${maDebug.Find[condition].Value}, ${maBuff.Find[stBuff${_count}Con].Value}]}) {
				/continue
			}

			| check basic tags
			/call check_basic_tags ${maDebug.Find[tag].Value} Buff${_count} ${_toonName} "${maBuff.Find[stBuff${_count}].Value}"
			/if (!${Macro.Return}) /continue

			DEBUG ${sep}\agBUFF\ax#${sep}\a-w${_count}\ax \a-p---------------------\ax

			| class
			| /if (${lsBuff${_count}Tag.Contains[${Spawn[${_toonName}].Class.ShortName}]}) /varset _target TRUE

			| one of your crew, pretend they are not a target until they qualify
			/varset _target FALSE

			/if (${lsBuff${_count}Tag.Contains[crew]} && ${Spawn[${_toonName}].ID}) {
				/if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_toonName}]}) /varset _target TRUE
			}

			| main assist
			| /if (${lsBuff${_count}Tag.Contains[ma]}) {
			| 	/if (${Group.MainAssist.Name.Equal[${_toonName}]} || ${maCC.Find[stManualAssist1].Value.Equal[${_toonName}]} || ${maCC.Find[stManualAssist2].Value.Equal[${_toonName}]}) /varset _target TRUE
			| }

			/varset _spell ${set_rank[FALSE, "${maBuff.Find[stBuff${_count}].Value}"]}
			DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax

			| distance check
			/if (${Select[${Spell[${_spell}].TargetType},Group v2]}) {
				/if (${Spawn[${_toonName}].Distance} > ${Spell[${_spell}].AERange}) /continue
			} else {
				/if (${Spawn[${_toonName}].Distance} > ${Spell[${_spell}].MyRange} && !${Select[${Spell[${_spell}].TargetType},self]}) /continue
			}

			DEBUG ${sep}\aw470\ax${sep}\a-w${Spell[${_spell}].HasSPA[470]}\ax
			DEBUG ${sep}\aw374\ax${sep}\a-w${Spell[${_spell}].HasSPA[374]}\ax
			DEBUG ${sep}\aw340\ax${sep}\a-w${Spell[${_spell}].HasSPA[340]}\ax
			/if (${Spell[${_spell}].HasSPA[470]} || ${Spell[${_spell}].HasSPA[374]} || ${Spell[${_spell}].HasSPA[340]}) {

				/for _i 1 to ${Spell[${_spell}].NumEffects}
					/varset _found TRUE

					| skip any secondary procs with no duration
					/if (!${Bool[${Spell[${Spell[${_spell}].Base2[${_i}]}].Duration}]}) /continue

					/varset _spellTrigger ${Spell[${_spell}].Trigger[${_i}]}
					DEBUG ${sep}${sep}\awTrigger\ax${sep}\a-w${_spellTrigger}\ax ID:\aw${Spell[${_spellTrigger}].ID}\ax

					| is it buff blocked
					/dquery ${_toonName} -q "lsBlockedBuffsMe.Find[${Spell[${_spellTrigger}].ID}]" -o DNQReturn -t 5
					DEBUG ${sep}${sep}BLOCKED .Trigger.Blocked Buff \at${Bool[${DNQReturn}]}\ax ${dot} ${_spellTrigger}
					/if (${Bool[${DNQReturn}]}) {
						/continue
					}

					| does it stack
					/dquery ${_toonName} -q "Spell[${_spellTrigger}].Stacks" -o DNQReturn -t 5
					DEBUG ${sep}${sep}STACKS: .Trigger.Stacks \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					/if (!${Bool[${DNQReturn}]}) {
						/continue
					}

					| do we have the triggered buff?
					/dquery ${_toonName} -q "Me.Buff[${_spellTrigger}].ID" -o DNQReturn -t 5
					DEBUG trigger:\ay${_spellTrigger}\ax
					DEBUG ${sep}${sep}HAVE: .Trigger.ID \at${DNQReturn}\ax ${dot} ${_spellTrigger}
					/if (!${Bool[${DNQReturn}]}) {
						/varset _found FALSE
						/break
					}

				/next _i

				DEBUG ${sep}${sep}\ay****\ax Found:${_found} .. Target:${_target}
				/if (!${_found} && ${_target}) {
					/call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Spawn[${_toonName}].ID} FALSE
					/if (${maEntropy.Find[swBuffCooldownWait].Value}) {
						DEBUG ${sep}buff cooldown recycle
						/while (${Me.SpellInCooldown}) {
							CHECKEXIT
							ISMEDEAD
							/if (!${Me.SpellInCooldown}) /break
							/delay 5
						}
					}
					/continue
				}

			}	else {

				/varset _spellTrigger ${Spell[${_spell}].RankName}
				DEBUG ${sep}\awTrigger\ax${sep}\a-w${_spellTrigger}\ax ID:${Spell[${_spellTrigger}].ID}

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
					/call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Spawn[${_toonName}].ID} FALSE
					/if (${maEntropy.Find[swBuffCooldownWait].Value}) {
						DEBUG ${sep}buff cooldown recycle
						/while (${Me.SpellInCooldown}) {
							CHECKEXIT
							ISMEDEAD
							/if (!${Me.SpellInCooldown}) /break
							/delay 5
						}
					}
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

			/if (!${maBuff.Find[swBuff${_count}].Value}) /continue
			/if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) /continue

			/call check_basic_tags ${maDebug.Find[tag].Value} Buff${_count} ${_toonName} "${maBuff.Find[stBuff${_count}].Value}"
			/if (!${Macro.Return}) /continue

			| pretend the target does not have the spell
			/varset _target FALSE
			| class
			| /if (!${lsBuff${_count}Tag.Contains[${Spawn[${_toonName}].Class.ShortName}]}) /continue
			| main assist
			/if (${lsBuff${_count}Tag.Contains[ma]} && !${Group.MainAssist.Name.Equal[${_toonName}]} && !${maCC.Find[stManualAssist1].Value.Equal[${_toonName}]} && !${maCC.Find[stManualAssist2].Value.Equal[${_toonName}]}) /continue
			| skip self only
			/if (${lsBuff${_count}Tag.Count} == 1 && (${lsBuff${_count}Tag.Contains[self]} || ${Spell[${_spell}].TargetType.Equal[Self]} || ${lsBuff${_count}Tag.Contains[minion]})) /continue

			/varset _spell ${set_rank[FALSE, "${maBuff.Find[stBuff${_count}].Value}"]}
			DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax

			| distance check
			/if (${Spawn[${_toonName}].Distance} > ${Spell[${_spell}].MyRange}) /continue

			| target the group member
			/invoke ${target[set, ${Group.Member[${_toonName}].ID}]}
			FLOW check_buff_cycle_crew
			| /invoke ${set_target[${Group.Member[${_toonName}].ID}]}

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
					/call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Group.Member[${_toonName}].ID} FALSE
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
					/call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Group.Member[${_toonName}].ID} FALSE
					/continue
				}
			}

		/next _count

		/if (${Target.ID}) {
			/squelch /target clear
		}

	}

	SETHUD EMPTY

/return TRUE



|***
 * note: we dont trust anyone
 * use: ${is_lsSafeNames[DEBUG, NAME]}
 ***|
sub is_lsSafeNames(bool _debug, string _name)
	DEBUG \atis_lsSafeNames\ax(\a-w${_name}\ax)
	FLOW is_lsSafeNames (${_name.Lower})

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
	| /if (${maEntropy.Find[lsSafeNames].Value.Find[${_name}]}) {
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
	FLOW check_buff_cycle_temp_self ${_spell} (1)

	/if (${target[isdead, TARGET]}) {
		FLOW check_buff_cycle_temp_self
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT check_buff_cycle_temp_self
	FLOW check_buff_cycle_temp_self ${_spell} (2)
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
	FLOW check_buff_cycle_temp_list ${_spell}|${_toonList} (1)

	SETHUD "buff temp"

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
	FLOW check_buff_cycle_temp_list ${_spell}|${_toonList} (2)
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

			/dquery ${_libt.Value} -q "Spell[${maChr.Find[${_spell}].Value}].Stacks" -o DNQReturn -t 5
			DEBUG ${DNQReturn}
			/if (${Bool[${DNQReturn}]}) /varset _castSpell TRUE

			/dquery ${_libt.Value} -q "Me.Buff[${maChr.Find[${_spell}].Value}].ID" -o DNQReturn -t 5
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
	FLOW cast_item_summon (1)

	CHECKTIE
	ISMEDEAD
	CHECKEXIT cast_item_summon
	FLOW cast_item_summon (2)
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
	FLOW cast_challenge ${_tmpID} (1)

	/if (${Target.Buff[${maChr.Find[stChallenge].Value}].ID}) {
		/return TRUE
	}

	ISMEDEAD
	CHECKEXIT cast_challenge
	FLOW cast_challenge ${_tmpID} (2)
	GETINPUT cast_challenge NA|NA

	/if (${Target.BuffDuration[${maChr.Find[stChallenge].Value}].TotalSeconds}) {
		DEBUG ${sep}Target.BuffDuration${sep}\a-w${Target.BuffDuration[${maChr.Find[stChallenge].Value}].TotalSeconds}\ax
		/return TRUE
	}

	/if (${is_Ready[${_debug}, "${maChr.Find[stChallenge].Value}"]}) {
		/call voodoo ${_debug} "${maChr.Find[stChallenge].Value}" ${_tmpID} FALSE
	}

/return TRUE



|***
 * note: cast nukes lists
 * use: /call cast_nuke DEBUG [line] [targetid]
 ***|
 sub cast_nuke(bool _debug, string _line, int _tmpID)
	DEBUG \atcast_nuke\ax(\a-w${_line}, ${_tmpID}\ax)
	FLOW cast_nuke ${_line}|${_tmpID}

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

		| /if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_nuke ${_line}|${_tmpID}
			/return FALSE
		}

		/invoke ${target[set, ${_tmpID}]}
		FLOW cast_nuke ${_line}|${_tmpID}

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


|***
 * note: cast dot lists
 * use: /call cast_dot DEBUG [listname] [targetid]

sub cast_dot(bool _debug, string _line, int _tmpID)
	DEBUG \atcast_dot\ax(\a-w${_line}, ${_tmpID}\ax)
	FLOW cast_dot ${_line}|${_tmpID}


	/declare _lidot listiterator local
	/vardata _lidot ${_line}.First.Clone
	/while (!${_lidot.IsEnd}) {
		DEBUG ${sep}_lidot.Value${sep}\a-w${_lidot.Value}\ax
		/delay 1

		ISMEDEAD
		CHECKEXIT cast_dot
		GETINPUT cast_dot NA|NA

		/if (${Me.PctMana} < 2) {
			/return FALSE
		}

		| TTL evaluations
		/if (${maTTL.Find[swTTL].Value}) {
			/if (${maTTL.Contains[${Spawn[${_tmpID}].DisplayName}]}) {
				| check overall durations of the average kill +2 tics over the spell duration time
				/if (${Spell[${Spell[${_lidot.Value}].RankName}].Duration} > ${Math.Calc[${maTTL.Find[${Spawn[${_tmpID}].DisplayName}].Value.Arg[2,|]} + (${maTTL.Find[${Spawn[${_tmpID}].DisplayName}].Value.Arg[2,|]}*.5)]}) {
					/invoke ${_lidot.Advance}
					/continue
				}
				| if mob is less then 50% hp and spell duration is > 1/2 your average kill time
				/if (${Spawn[${_tmpID}].PctHPs} < 50 && ${Spell[${Spell[${_lidot.Value}].RankName}].Duration} > ${Math.Calc[${maTTL.Find[${Spawn[${_tmpID}].DisplayName}].Value.Arg[2,|]} / 2]}) {
					/invoke ${_lidot.Advance}
					/continue
				}
			}
		}

		/if (!${is_ready[${_debug}, "${_lidot.Value}"]}) {
			/invoke ${_lidot.Advance}
			/continue
		}

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_dot ${_line}|${_tmpID}
			/return FALSE
		}


		| /if (${get_target_buff_time_left[${_debug}, "${_lidot.Value}"]} >= ${maCC.Find[stDotRefresh].Value}) {
		/if (${Target.BuffDuration[${_lidot.Value}].TotalSeconds} >= ${maCC.Find[stDotRefresh].Value}) {
			/invoke ${_lidot.Advance}
			/continue
		}

		/call voodoo ${_debug} "${_lidot.Value}" ${_tmpID} FALSE

		| forced delay
		/if (${maCC.Find[stNukeDelay].Value}) {
			/invoke ${set_data_timer[${_debug}, Nuke_Delay, RESTART]}
		}

		| maintain the loop of nukes till all are cycled.
		/if (${maEnv.Find[swNukeLoop].Value}) {
			DEBUG ${sep}swNukeLoop delay${sep}\a-w${timer_Nuke_Delay}\ax\ay+\ax
			/delay 10s !${Me.Casting.ID}
			/delay 2.5s
			/delay ${timer_Nuke_Delay}
		}

		/invoke ${_lidot.Advance}
	}

/return TRUE
 ***|


|***
 * nore: use base melee discs
 * use: /call do_melee DEBUG [TARGETID]
 ***|
sub do_melee(bool _debug, int _tmpID)
	DEBUG \atdo_melee\ax(\a-w${_tmpID}\ax)
	FLOW do_melee ${_tmpID} (1)

	ISMEDEAD
	CHECKEXIT do_melee
	FLOW do_melee ${_tmpID} (2)
	GETINPUT do_melee NA|NA

	| /if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return

	/if (${target[isdead, ${_tmpID}]}) {
		FLOW do_melee ${_tmpID}
		/return FALSE
	}

	CHECKDEFENSE

	/invoke ${target[set, ${_tmpID}]}
	FLOW do_melee ${_tmpID}

	/if (AUTO || ${swModeToken}) {
		| /call check_facing ${maDebug.Find[face].Value} ${_tmpID} FORCE
		/if (!${timer_check_stick}) {
			/call set_combat_control ${maDebug.Find[decision].Value} ${_tmpID}
			| /call stick_to_target ${maDebug.Find[stick].Value} ${_tmpID}
		}
	}

	| melee bandolier swap for proc weapons
	/if (${maSash.Find[swSashProc].Value} && !${timer_Check_Proc_Sash}) {
		/call check_sash_proc ${_debug}
	}

	| taunt /onoff agro
	/if (${maCC.Find[swTaunt].Value} && AGRO) {
		/if (${Me.AbilityReady[Taunt]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]} && ${Me.TargetOfTarget.Name.NotEqual[${Me.DisplayName}]}) {
			/invoke ${out[8, "Melee${sep}${sp}Taunt\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/doability Taunt
		}
	}

	| disarm
	/if (${maCC.Find[swDisarm].Value} && !${Skill[Disarm].Auto} && AGRO) {
		/if (${Me.AbilityReady[Disarm]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
			/invoke ${out[8, "Melee${sep}${sp}Disarm\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/doability Disarm
		}
	}

	| kick
	/if (${maCC.Find[swKick].Value} && !${Skill[Kick].Auto} && !${Select[${Me.Class.ShortName},MNK]}) {
	 	/if (${Me.AbilityReady[Kick]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
			/invoke ${out[8, "Melee${sep}${sp}Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/doability Kick
		}
	}

	| bash
	/if (${maCC.Find[swBash].Value} && !${Skill[Bash].Auto}) {
		/if (${Me.AbilityReady[Bash]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
			/invoke ${out[8, "Melee${sep}${sp}Bash\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
			/doability Bash
			|**
			/if (${maCC.Find[swBandolier].Value} && ${Bool[${bandolierBash}]} && ${Bool[${bandolierNormal}]}) {
				/if (${Target.ID}) {
					/call bandolier_swap Bash
					/doability Bash
					/call bandolier_swap Normal
				}
			} else /if (!${maCC.Find[swBandolier].Value}) {
				/doability Bash
			}

			**|

		}
	}

	| Intimidation
	/if (${check_class[FALSE, |BRD|ROG|BER]}) {
		/if (${maCC.Find[swIntimidation].Value}) {
			/if (${Me.AbilityReady[Intimidation]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
				/invoke ${out[8, "Melee${sep}${sp}Intimidation\ax ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax"]}
				/doability Intimidation
			}
		}
	}

	| Zerker only skill
	/if (${check_class[FALSE, |BER]}) {
		/if (${maCC.Find[swFrenzy].Value} && && !${Skill[Frenzy].Auto}) {
			/if (${Me.AbilityReady[Frenzy]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
			/invoke ${out[8, "Melee${sep}${sp}Frenzy\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability Frenzy
			}
		}
	}

	| rogue only skill
	/if (${check_class[FALSE, |ROG]}) {
		/if (${maCC.Find[swBackstab].Value} && && !${Skill[Backstab].Auto}) {
			/if (${Me.AbilityReady[Backstab]} && ${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
			/invoke ${out[8, "Melee${sep}${sp}Backstab\ax ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax"]}
				/doability Backstab
			}
		}
	}

	| monk/bst only skill
	/if (${check_class[FALSE, |MNK|BST]}) {
			/if (${Spawn[${_tmpID}].Distance} <= ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) {
			| fist
			/if (${maCC.Find[swDragonpunch].Value} && !${Skill[Dragon Punch].Auto} && ${Me.AbilityReady[Dragon Punch]}) {
				/invoke ${out[8, "Melee${sep}${sp}Dragon Punch\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Dragon Punch"
			} else /if (${maCC.Find[swEaglestrike].Value} && !${Skill[Eagle Strike].Auto} && ${Me.AbilityReady[Eagle Strike]}) {
				/invoke ${out[8, "Melee${sep}${sp}Eagle Strike\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Eagle Strike"
			} else /if (${maCC.Find[swTigerclaw].Value} && !${Skill[Tiger Claw].Auto} && ${Me.AbilityReady[Tiger Claw]}) {
			/invoke ${out[8, "Melee${sep}${sp}Tiger Claw\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Tiger Claw"
			}
			| kick
		 	/if (${maCC.Find[swKick].Value} && !${Skill[Kick].Auto} && ${Me.AbilityReady[Kick]}) {
				/invoke ${out[8, "Melee${sep}${sp}Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability Kick
			} else /if (${maCC.Find[swFlyingkick].Value} && !${Skill[Flying Kick].Auto} && ${Me.AbilityReady[Flying Kick]}) {
				/invoke ${out[8, "Melee${sep}${sp}Flying Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
				/doability "Flying Kick"
			} else /if (${maCC.Find[swRoundkick].Value} && !${Skill[Round Kick].Auto} && ${Me.AbilityReady[Round Kick]}) {
				/invoke ${out[8, "Melee${sep}${sp}Round Kick\ax ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
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
	FLOW cast_stamina_recovery

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
	FLOW check_harmonious (1)

	ISMEDEAD
	CHECKEXIT check_harmonious
	FLOW check_harmonious (2)
	GETINPUT check_harmonious NA|NA

	/invoke ${set_data_timer[${_debug}, Check_Harmonious, RESTART]}

	/if (!${Me.Buff[${maChr.Find[stHarmonious].Value}].ID} && ${Me.PctMana} > 2) {
		/if (${is_ready[${_debug}, "${maChr.Find[stHarmonious].Value}"]}) {
			/call voodoo ${_debug} "${maChr.Find[stHarmonious].Value}" 0 FALSE
		}
	}

/return TRUE



|***
 * note: checks tags for buffs..
 * use: ${check_basic_tags[DEBUG, type, toonname, spellname]}
 ***|
sub check_basic_tags(bool _debug, string _type, string _name, string _spell)
	DEBUG \atcheck_basic_tags\ax(\a-w${_type}, ${_name}, ${_spell}\ax)
	FLOW check_basic_tags

	| is the master switch off?
	/if (!${maBuff.Find[swBuffMaster].Value}) {
		DEBUG ${sep}tag${sep}\a-wswBuffMaster\ax
		/return FALSE
	}

	| obey no tags. only use the aliase for this .. thing..
	/if (${ls${_type}Tag.Contains[aliasonly]}) {
		DEBUG ${sep}tag${sep}\a-waliasonly\ax
		/return FALSE
	}

	| ** ** ** do a thing tags ** ** ** |

	| force a 1 second delay
	/if (${ls${_type}Tag.Contains[delay1s]}) {
		DEBUG ${sep}tag${sep}\a-wdelay1s\ax
		/delay 1s
	}

  | drop invis, let the fun ensue
  /if (${ls${_type}Tag.Contains[dropinvis]}) {
  	DEBUG ${sep}tag${sep}\a-wdropinvis\ax
   	/makemevisible
  }

	| force drop current running disc
	/if (${ls${_type}Tag.Contains[dropdisc]}) {
		/if (${Me.ActiveDisc.ID} && ${Me.CombatAbilityReady[${Spell[_spell].RankName}]}) {
			DEBUG ${sep}tag${sep}\a-wdropdisc\ax (\a-r${Me.ActiveDisc.ID}\ax)
			/stopdisk
			/delay 5
		}
	}

	| mem to a specific gem?
	/declare _count int local 0
	| /echo here${sep}\a-w${_spell}\ax
	/for _count 1 to 13

		/if (${ls${_type}Tag.Contains[g${_count}]}) {
			DEBUG ${sep}tag${sep}\a-rgem#\ax${sep}\a-wg${_count}\ax .. (${_spell}) -- (!${Me.Gem[${_count}].Name.Equal[${Spell[${_spell}].RankName}]})
			/call set_spell_to ${_debug} TRUE "${Spell[${_spell}].RankName}" ${_count} TRUE
			/break
		}
	/next _count


	| ** ** ** fail tags ** ** ** |

	| no buffing inside an entrtopy combat cycle
	/if (${ls${_type}Tag.Contains[nocombat]}) {
		/if (AUTO && COMBAT) {
			DEBUG ${sep}tag${sep}\a-wnocombat\ax
			/if (HEAL && ${SpawnCount[npc radius SAFEENV PS4]}) {
				/return FALSE
			} else {
				/return FALSE
			}
		}
	}

	| no active eq combat
	/if (${ls${_type}Tag.Contains[noactivecombat]}) {
		/if (${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius SAFEENV PS4]}) {
			DEBUG ${sep}tag${sep}\a-wnoactivecombat\ax
			/return FALSE
		}
	}

	| no active disc running
	/if (${ls${_type}Tag.Contains[noactivedisc]}) {
		/if (${Me.ActiveDisc.ID}) {
			DEBUG ${sep}tag${sep}\a-wnoactivedisc\ax
			/return FALSE
		}
	}

	| my group members only
	/if (${ls${_type}Tag.Contains[groupmember]}) {
		/if (!${Group.Member[${_name}].ID} || !${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_name}]}) {
			DEBUG ${sep}tag${sep}\a-wgroupmember\ax
			/return FALSE
		}
	}

	| must be in a raid
	/if (${ls${_type}Tag.Contains[raid]}) {
		/if (!${Raid.Members}) {
			DEBUG ${sep}tag${sep}\a-wraid\ax
			/return FALSE
		}
	}

	| must be in a group
	/if (${ls${_type}Tag.Contains[group]}) {
		/if (!${Group.Members}) {
			DEBUG ${sep}tag${sep}\a-wgroup\ax
			/return FALSE
		}
	}

	| must not be in a raid
	/if (${ls${_type}Tag.Contains[noraid]}) {
		/if (${Raid.Members}) {
			DEBUG ${sep}tag${sep}\a-wnoraid\ax
			/return FALSE
		}
	}

	| must be in an entropy combat cycle
	/if (${ls${_type}Tag.Contains[combat]}) {
		/if (!COMBAT) {
			DEBUG ${sep}tag${sep}\a-wcombat\ax
			/return FALSE
		}
	}

	| no active eq combat
	/if (${ls${_type}Tag.Contains[activecombat]}) {
		/if (!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius SAFEENV PS4]}) {
			DEBUG ${sep}tag${sep}\a-wactivecombat\ax
			/return FALSE
		}
	}


	| must be outdoors
	/if (${ls${_type}Tag.Contains[outdoor]}) {
		/if (${Me.FeetWet} || ${Zone.Indoor}) {
			DEBUG ${sep}tag${sep}\a-woutdoor\ax
			/return FALSE
		}
	}

	| must have pull enabled
	/if (${ls${_type}Tag.Contains[puller]}) {
		/if (PULL) {
			DEBUG ${sep}tag${sep}\a-wpuller\ax
			/return FALSE
		}
	}

	| must have agro enabled
	/if (${ls${_type}Tag.Contains[agro]}) {
		/if (!AGRO) {
			DEBUG ${sep}tag${sep}\a-wagro\ax
			/return FALSE
		}
	}

	| cannot have agro enabled
	/if (${ls${_type}Tag.Contains[noagro]}) {
		/if (AGRO) {
			DEBUG ${sep}tag${sep}\a-wnoagro\ax
			/return FALSE
		}
	}

	| must be burning
	/if (${ls${_type}Tag.Contains[burn]}) {
		/if (!BURN) {
			DEBUG ${sep}tag${sep}\a-wburn\ax
			/return FALSE
		}

	}

	| cannot be in a safe zone
	/if (${ls${_type}Tag.Contains[nosafezone]}) {
		/if (SAFEZONE) {
			DEBUG ${sep}tag${sep}\a-wnosafezone\ax
			/return FALSE
		}
	}

	| current target must be the MAs target
	/if (${ls${_type}Tag.Contains[matarget]}) {
		/if (${Target.ID} != TARGET) {
			DEBUG ${sep}tag${sep}\a-wmatarget\ax
			/return FALSE
		}
	}

	| must have a pet (static)
	/if (${ls${_type}Tag.Contains[minion]}) {
		/if (!${Pet.ID}) {
			DEBUG ${sep}tag${sep}\a-wminion\ax
			/return FALSE
		}
	}

	| must have a target
	/if (${ls${_type}Tag.Contains[target]}) {
		/if (!${Target.ID}) {
			DEBUG ${sep}tag${sep}\a-wtarget\ax
			/return FALSE
		}
	}

	| must have a target and iut must be undead
	/if (${ls${_type}Tag.Contains[targetundead]}) {
		/if (${Target.Body.Name.NotEqual[undead]}) {
			DEBUG ${sep}tag${sep}\a-wtargetundead\ax
			/return FALSE
		}
	}

	| must have a target and it must be humanoid
	/if (${ls${_type}Tag.Contains[targethumanoid]}) {
		/if (${Target.Body.Name.NotEqual[humanoid]}) {
			DEBUG ${sep}tag${sep}\a-wtargethumanoid\ax
			/return FALSE
		}
	}

	| must have a target and it must be plant
	/if (${ls${_type}Tag.Contains[targetplant]}) {
		/if (${Target.Body.Name.NotEqual[plant]}) {
			DEBUG ${sep}tag${sep}\a-wtargetplant\ax
			/return FALSE
		}
	}

	| must have swarm pets enable
	/if (${ls${_type}Tag.Contains[swarm]}) {
		/if (!SWARM) {
			DEBUG ${sep}tag${sep}\a-wswarm\ax
			/return FALSE
		}
	}

	| target must be the main assist
	/if (${ls${_type}Tag.Contains[ma]}) {
		/if (${Target.ID} != ${get_assist_info[FALSE, ID]}) {
			DEBUG ${sep}tag${sep}\a-wma\ax
			/return FALSE
		}
	}

	| used for items that summon something
	/if (${ls${_type}Tag.Contains[summonitem]}) {
		/if (${check_basic_tags_summonitem[${maDebug.Find[tag].Value}, ${_type}]}) {
			DEBUG ${sep}tag${sep}\a-wsummonitem\ax
			/return FALSE
		}
	}

	| main assist
	| /if (${ls${_type}Tag.Contains[ma]}) {
	| 	/if (${Group.MainAssist.Name.Equal[${_name}]} && ${maCC.Find[stManualAssist1].Value.Equal[${_name}]} && ${maCC.Find[stManualAssist2].Value.Equal[${_name}]})) /return FALSE
	| }

	| /if (!${Bool[${_name}]} || ${_name.Equal[FALSE]}) /return TRUE
	| /if (!${Spawn[${_name} ].ID}) /return FALSE


	| checks to match a class shortname
	/declare _marker bool FALSE
	/declare _contains FALSE
	/declare _li listiterator local
	/vardata _li lsClassAll.First.Clone
	| cycle all 16 classes
	/while (!${_li.IsEnd}) {
		| if the tag has a class in it st containts true
		/if (${ls${_type}Tag.Contains[${_li.Value}]}) {
			/varset _contains TRUE
			| if we get a match for the class and the tag, set marker true and break the check
			/if (${Spawn[${_name}].Class.ShortName.Equal[${_li.Value}]}) {
				DEBUG ${sep}\aytag\ax${sep}\a-w${_li.Value}\ax
				/varset _marker TRUE
				/break
			}
		}
		/varset _contains FALSE
		/invoke ${_li.Advance}
	}
	| if contains==TRUE and _marker != TRUE we return a fail for that class that is the target
	/if (${_contains} && !${_marker}) {
		/return FALSE
	}


	| one of your crew
	/if (${ls${_type}Tag.Contains[crew]}) {
		/if (!${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_name}]}) {
			DEBUG ${sep}tag${sep}\a-wcrew\ax
			/return FALSE
		}
	}

/return TRUE



|***
 * note: checks the tag "summonitem" for a count of the item to be summoned on the toon. returns the count
 * use: ${check_basic_tags_summonitem[DEBUG, BUFF##]}
 ***|
sub check_basic_tags_summonitem(bool _debug, string _buff)
	DEBUG \atcheck_basic_tags_summonitem\ax(\a-w${_buff}\ax)
	FLOW check_basic_tags_summonitem

	/declare _count int local
	/varset _count ${FindItemCount[${Spell[${Spell[${maBuff.Find[st${_buff}].Value}].RankName}].Base[1]}]}

/return ${_count}



|***
 * note: converts a click item
 * use: /call get_item_convert DEBUG [item to find] [full name]
 ***|
sub get_item_convert(bool _debug, string _item, string _fullname)
	DEBUG \atget_item_convert\ax(\a-w${_item}, ${_fullname}\ax)
	FLOW get_item_convert

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
 *	DES: advanced loot routine(s)
 *	USE: auto
 *	NOTE: set a person in the group as master looter. Or, as the raid master looter.
 *	NOTE2: The event in this sectction is designed to handle lore items the toon already posseses. as of this, MQ has no property .Member value for LORE in ${AdvLoot
 ***|
| #Event forcedloreleave "#*#already has #1# and it is lore#*#"
| #Event forcedloreleave "#*#does not want #1#. It is either on their never list or they have selected No#*#"
sub Event_forcedloreleave(string line, string _itemName, bool _debug)
	| /if (!${maEnv.Find[swADVLoot].Value}) /return
	| /varset _itemName ${AdvLoot.SList[1].Name}
	| /invoke ${out[14, "Cursor ${sep} ${cinfo}${_itemName}\ax -> ${oleave}"]}
	| /ini "${INILoot}" "${_itemName.Left[1]}" "${_itemName}" Leave
	| /doevents flush
/return

sub do_loot(bool _debug)
	DEBUG \atdo_loot\ax()
	FLOW do_loot

	ISMEDEAD
	GETINPUT do_loot NA|NA

	/invoke ${set_data_timer[FALSE, Check_Loot, RESTART]}

	/if (${Stick.Active} || !${Me.FreeInventory} || !${Me.UseAdvancedLooting}) /return FALSE

	| I am not the raid loot master /sigh
	/if (${Raid.Members} && ${Raid.MasterLooter.ID} != ${Me.ID} && !${maOver.Find[swOverLoot].Value}) /return FALSE
	/if (!${Raid.Members} && ${Group.Members} && ${Group.MasterLooter.ID} != ${Me.ID} && !${maOver.Find[swOverLoot].Value}) /return FALSE

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
					/invoke ${out[14, "${add} ${dot} ${_advlItem} = ${leave} [no drop/lore]"]}
					/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave")
					DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave") \ay${sqlite.Status[lootquery]}\ax
				} else {
					/if (${lsGrain.Contains[14]}) {
					 	OUT ${add} ${dot} ${_advlItem} = ${keep} [default]
					}
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
				/if (${AdvLoot.SList[${_scount}].Nodrop} || ${FindItem[=${_advlItem}].Lore} || ${FindItemBank[${_advlItem}].Lore}) {
					/invoke ${out[0, "${add} ${dot} ${_advlItem} = ${leave} [no drop/lore]"]}
					/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave")
					DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_advlItem}", "leave") \ay${sqlite.Status[lootquery]}\ax
				} else {
					/if (${lsGrain.Contains[14]}) {
						OUT ${add} ${dot} ${_advlItem} = ${keep} [default]
					}
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
					/invoke ${out[14, "Leaving${sep}\a-w${_advlItem}\ax"]}
					/delay 1s !${AdvLoot.LootInProgress}
					/if (!${AdvLoot.LootInProgress}) /advloot personal ${_pcount} leave
				} else /if (${Select[${_advlChoice},keep,sell]}) {
					DEBUG ${sep}${_advlChoice}
					/delay 2s !${AdvLoot.LootInProgress}
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

							/invoke ${out[14, "Destroying${sep}\a-w${_advlItem}\ax"]}
							/nomodkey /destroy

							/call check_cursor ${_debug} CLEAR
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

	SETHUD EMPTY

/return TRUE



|***
 * note: Cures/removes debuffs, we hope...
 * use:
 ***|
sub check_cure(bool _debug)
	DEBUG \atcheck_cure\ax()
	FLOW check_cure

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
			DEBUG ${sep}\aw${_name}\ax is not being observerd.. \a-wshit coding inc\ax...
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

			| AA Radient Cure
			| 100' 1 min
			/if (${Select[${Me.Class.ShortName},PAL,CLR,SHM,DRU]} && ${Me.AltAbilityReady[Radiant Cure]} && ${Spawn[pc ${_name}].Distance} < 100 && ${Group.Member[${_name}].ID}) {
				/call voodoo FALSE "Radiant Cure" alt 0 FALSE
				/invoke ${set_data_timer[FALSE, Check_Cure, 2s]}
				/return
			}

			| AA Purify Soul
			| 200' 5 min
			/if (${Select[${Me.Class.ShortName},PAL,CLR,DRU]} && ${Me.AltAbilityReady[Purify Soul]} && ${Spawn[pc ${_name}].Distance} < 200) {
				/call voodoo FALSE "Purify Soul" ${Spawn[pc ${_name}].ID} FALSE
				/invoke ${set_data_timer[FALSE, Check_Cure, 2s]}
				/return
			}

			| AA Group Purify Soul . were in deep shit if we've gotten this far.
			| 0/100' 15 min
			/if (${Select[${Me.Class.ShortName},CLR,DRU]} && ${Me.AltAbilityReady[Group Purify Soul]} && ${Spawn[pc ${_name}].Distance} < 100 && ${Group.Member[${_name}].ID}) {
				/call voodoo FALSE "Group Purify Soul" 0 FALSE
				/invoke ${set_data_timer[FALSE, Check_Cure, 2s]}
				/return
			}

			| paladin splash 50' range
			/if (${Select[${Me.Class.ShortName},PAL]} && ${Spawn[pc ${_name}].Distance} < 50) {
				/call Bind_command_splash FALSE
				/invoke ${set_data_timer[FALSE, Check_Cure, 2s]}
				/return
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

	SETHUD EMPTY

/return



|***
 * note: dumb fucks always gotta put that red bouncy ball on my cursor
 * use: /call check_cursor DEBUG [clear]
 ***|
sub check_cursor(bool _debug, string _verbage)
	DEBUG \atcheck_cursor\ax(\a-w${_verbage}\ax)
	FLOW check_cursor

	SETHUD "cursor"

	/if (!${Me.FreeInventory}) /return FALSE

	| force clearing the cursor and GTFO
	/if (${_verbage.Equal[CLEAR]}) {
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

	/if (!${Cursor.ID}) /return

	| check DB for the item
	/call set_loot_item ${_debug} "${Cursor.Name}"

	| reload data
	/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}" LIMIT 1
	DEBUG lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}" LIMIT 1 \ay${sqlite.Status[dummy]}\ax

	| correct the ignore vs. leave spam issue for lore items in the db, then reset
	/if (${Cursor.Lore} && ${Select[${sqlite.Result[lootquery 1 action]},leave]}) {
		/sqlite ${maData.Find[DBloot].Value} lootquery UPDATE loot_control SET 'action'="ignore"  WHERE item="${Cursor.Name}"
		/invoke ${set_data_timer[${_debug}, Check_Cursor, 5]}
		/return TRUE
	}

	| wtf we gotta do with the item?
	| do the things that we do best
	/if (${Select[${sqlite.Result[lootquery 1 action]},keep]}) {
		/invoke ${out[14, "Cursor${sep}\agkeep\ax${sep}\a-w${Cursor.Name}\ax${sep}\a-w${FindItemCount[=${Cursor.Name}]}\ax"]}
		/nomodkey /autoinventory
	} else /if (${Select[${sqlite.Result[lootquery 1 action]},sell]}) {
		/invoke ${out[14, "Cursor${sep}\atsell\ax${sep}\a-w${Cursor.Name}\ax${sep}\a-w${FindItemCount[=${Cursor.Name}]}\ax"]}
		/nomodkey /autoinventory
	} else /if (${Select[${sqlite.Result[lootquery 1 action]},destroy]}) {
		/invoke ${out[14, "Cursor${sep}\ardestroy\ax${sep}\a-w${Cursor.Name}\ax"]}
		/nomodkey /destroy
	} else /if (${Select[${sqlite.Result[lootquery 1 action]},leave]}) {
		/invoke ${out[14, "Cursor${sep}\ayleave\ax${sep}\a-w${Cursor.Name}\ax"]}
		/nomodkey /drop
	} else /if (${Select[${sqlite.Result[lootquery 1 action]},ignore]}) {
		| do nothing.. seriously.. what do you think ignore means?
	}

	/delay 1

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
	FLOW cast_temp_pet ${_tmpID}|${_tmpSpell} (1)

	| these classes can only have one pet at a time (not counting  AA and spell combo. but i'n not gonna sort that shit)
	/if (${Select[${Me.Class.ShortName},CLR,WIZ]}) {
		/if (${Me.Pet.ID}) /return FALSE
	}

	ISMEDEAD
	CHECKEXIT cast_temp_pet
	FLOW cast_temp_pet ${_tmpID}|${_tmpSpell} (2)
	GETINPUT cast_temp_pet NA|NA

	| /if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE

	/if (${target[isdead, ${_tmpID}]}) {
		FLOW cast_temp_pet ${_tmpID}|${_tmpSpell}
		/return FALSE
	}

	/if (${Target.PctHPs} > ${maMinion.Find[stPctSwarmEngage].Value}) /return FALSE

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
	FLOW send_pet ${_tmpID} (1)

	| no target == bail
	/if (!${_tmpID}) {
		DEBUG ${sep}break._tmpID
		/return FALSE
	}

	| out of env range
	/if (${Spawn[id ${_tmpID}].Distance} > ENV) {
		DEBUG ${sep}break.>env
		/return FALSE
	}

	| pet has the passed target
	/if (${Me.Pet.Target.ID} == ${_tmpID}) {
		DEBUG ${sep}break.ID${sep}\a-w${Me.Pet.Target.ID} == ${_tmpID}\ax
		/return TRUE
	}

	| pets target does not match the passed id
	/if (${Me.Pet.Target.ID} != ${_tmpID}) {
		DEBUG ${sep}break.ID${sep}\a-w${Me.Pet.Target.ID} != ${_tmpID}\ax
		/pet back off
	}

	| manual peoples...
	/if (!AUTO && !${maMinion.Find[swonForce].Value}) {
		DEBUG ${sep}break.manual NO swonForce
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT send_pet
	FLOW send_pet ${_tmpID} (2)
	GETINPUT send_pet NA|NA

	/if (PET) {
		/if (${Spawn[id ${_tmpID}].PctHPs} < ${maMinion.Find[stPctPetEngage].Value}) {
			/invoke ${target[set, ${_tmpID}]}
			FLOW send_pet ${_tmpID}
			/pet attack
		}
	}

	| send in the swarm
	/if (SWARM) {
		/if (${Spawn[id ${_tmpID}].PctHPs}  < ${maMinion.Find[stPctSwarmEngage].Value}) {
			/invoke ${target[set, ${_tmpID}]}
			FLOW send_pet ${_tmpID}
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
	FLOW check_deBuffs ${_tmpID} (1)

	| check distance or bail early
	/if (${Spawn[${_tmpID}].Distance} > ENV) {
		/invoke ${set_data_timer[FALSE, Check_deBuffs, RESTART]}
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT check_deBuffs
	FLOW check_deBuffs ${_tmpID} (2)
	CHECKTIE
	GETINPUT check_deBuffs _tmpID|${_tmpID}

	/invoke ${target[set, ${_tmpID}]}
	FLOW check_deBuffs ${_tmpID}

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

	| make sure we have a target and the buff list is current
	/invoke ${target[set, ${_tmpID}]}
	FLOW check_deBuffs ${_tmpID}
	/delay 1s ${Target.BuffsPopulated}

	| start the debuff cycle
	/while (!${_lideb.IsEnd}) {
		DEBUG ${sep}checking${sep}\a-w${_lideb.Value}\ax (${If[${madeBuff.Find[sw${_lideb.Value}].Value},${on},${off}]})

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
				}
				| }

				/if (AE# && ${_mobCount} >= AE#) {
					/here 2
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
	FLOW check_minion_cycle

	/if (!PET) {
		/invoke ${out[0, "Master Minion Switch${sep}${off}"]}
		/return FALSE
	}

	/if (AUTO && !${_force}) {
		DEBUG ${sep}minion cycle${sep}\a-wAURO, not forced\ax
		/invoke ${set_data_timer[${_debug}, Check_Minion_Cycle, RESTART]}

	| reset timers to force shit
	} else /if (${_force}) {
		DEBUG ${sep}minion cycle${sep}\arFORCED\ax
		/declare _buffcycletimestart int local ${MacroQuest.Running}
		| /invoke ${set_data_timer[${_debug}, Check_Minion_Cycle, 0]}
		/invoke ${set_data_timer[${_debug}, Check_Minion_Buff, 0]}
		/invoke ${set_data_timer[${_debug}, Check_Minion_Shrink, 0]}
		/invoke ${out[0, "Minion Cycle${sep}\a-wForced\ax"]}
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

	| get weapons?
	DEBUG ${sep}minion cycle${sep}\a-wget weapons?\ax
	/if (${maMinion.Find[swUsePetGear].Value}) {
		/call check_minion_cycle_weapons ${_debug}
	}

	| illusions
	DEBUG ${sep}minion cycle${sep}\a-willusions\ax
	/if (${Pet.ID} && ${Bool[${maMinion.Find[stPetIllusion].Value}]}) {
		/call check_minion_cycle_Illusion ${_debug}
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
	FLOW check_minion_cycle_shrink (1)

	ISMEDEAD
	CHECKEXIT check_minion_cycle_shrink
	FLOW check_minion_cycle_shrink (2)
	GETINPUT check_minion_cycle_shrink NA|NA

	/invoke ${set_data_timer[FALSE, Check_Minion_Shrink, RESTART]}

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
	FLOW check_minion_cycle_buff

	/if (AUTO) {
		/invoke ${set_data_timer[${_debug}, Check_Minion_Buff, RESTART]}
	}

	/if (!${Pet.ID}) /return FALSE

	/declare _spell string local FALSE
	/declare _count int local 0

	DEBUG ${sep}Buff${sep}\a-wMinion\ax

	| maybe 50 buffs was a bit much.
	/for _count 1 to 50

		/if (!${maBuff.Find[swBuff${_count}].Value}) /continue
		/if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) /continue

		| minion buffs?
		/if (!${lsBuff${_count}Tag.Contains[minion]}) /continue

		/call check_basic_tags ${maDebug.Find[tag].Value} Buff${_count} FALSE "${maBuff.Find[stBuff${_count}].Value}"
		/if (!${Macro.Return}) /continue

		DEBUG _count${sep}\a-w${_count}

		/varset _spell ${set_rank[FALSE, "${maBuff.Find[stBuff${_count}].Value}"]}
		DEBUG ${sep}\aw${_count}\ax${sep}\a-w${_spell}\ax${sep}\at${Spell[${_spell}].NumEffects}\ax

		| distance check
		/if (${Pet.Distance} > ${Spell[${_spell}].MyRange}) /continue

		| target the group member
		/invoke ${target[set, ${Pet.ID}]}
		FLOW check_minion_cycle_buff

		| have the buff?
		/if (${Pet.Buff[${maBuff.Find[stBuff${_count}].Value}]}) /continue

		| give the game a second to populate the targets buffs
		/delay 1s ${Target.BuffsPopulated}

		| stacks
		/if (!${Spell[${_spell}].StacksTarget}) /continue

		/call voodoo ${_debug} "${maBuff.Find[stBuff${_count}].Value}" ${Pet.ID} FALSE
		/if (${maEntropy.Find[swBuffCooldownWait].Value}) {
			DEBUG ${sep}buff cooldown recycle
			/while (${Me.SpellInCooldown}) {
				CHECKEXIT
				ISMEDEAD
				/if (!${Me.SpellInCooldown}) /break
				/delay 5
			}
		}


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
	FLOW check_minion_cycle_summon (1)

	ISMEDEAD
	CHECKEXIT check_minion_cycle_summon
	FLOW check_minion_cycle_summon (2)
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
			/invoke ${out[0, "Missing pet component."]}
			/invoke ${out[0, "Shutting pets off 'till you go buy some."]}
			/invoke ${out[0, "Don't forget to turn back on."]}
			/invoke ${set_switch_env[${_debug}, swPet, TRUE, FALSE]}
			/return FALSE
		}
	}

	/invoke ${out[0, "Summoning${sep}\a-w${maMinion.Find[stPetType].Value}\ax"]}
	/call voodoo ${_debug} "${maMinion.Find[stPetType].Value}" 0 FALSE
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
	FLOW check_minion_cycle_weapons (1)

	ISMEDEAD
	CHECKEXIT check_minion_cycle_weapons
	FLOW check_minion_cycle_weapons (2)
	GETINPUT check_minion_cycle_weapons NA|NA

	| no pets!!
	/if (!PET || !${Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) {
		/return FALSE
	}


	| pet has weapons
	/if ((${Pet.Equipment[primary]} && ${Me.Pet.Equipment[primary]} != 10855) && ${Pet.Equipment[offhand]}) {
		/return TRUE
	}

	| if you are not a mage, you should not be here
	/if (${Select[${Me.Class.ShortName},MAG]} && !${FindItemCount[=${get_MAG_folded_pack[]}]}) {

		/delay 4s !${Me.SpellInCooldown}

		/declare _count int local 0
		/for _count 1 to 50
			/if (!${maBuff.Find[swBuff${_count}].Value}) /continue
			/if (!${Bool[${maBuff.Find[stBuff${_count}].Value}]}) /continue
			/if (!${lsBuff${_count}Tag.Contains[summonweapons]}) /continue
			DEBUG _count${sep}${_count}

			/if (${FindItem[=${Spell[${maBuff.Find[stBuff${_count}].Value}].Base[1]}].ID}) /return TRUE

			| /if (${is_ready[FALSE, "${maBuff.Find[stBuff${_count}].Value}"]}) {
				/call voodoo FALSE "${maBuff.Find[stBuff${_count}].Value}" ${Me.ID} FALSE
			| }
			/delay 10s !${Me.Casting.ID}

			/if (${Cursor.Name.Equal[${FindItem[${Spell[${maBuff.Find[stBuff${_count}].Value}].Base[1]}].Name}]}) {
				/autoinventory
				/break
			}

		/next _count
	}

	/if (${Pet.ID}) {
		/call get_minion_summon_weapons FALSE
	}

/return TRUE



|***
 * note: lipstick on a pig is still a fucking pig
 * use: /call check_minion_cycle_Illusion DEBUG
 ***|
sub check_minion_cycle_Illusion(bool _debug)
	DEBUG \atcheck_minion_cycle_Illusion\ax()
	FLOW check_minion_cycle_Illusion (1)

	ISMEDEAD
	CHECKEXIT check_minion_cycle_Illusion
	FLOW check_minion_cycle_Illusion (2)
	GETINPUT check_minion_cycle_Illusion NA|NA

	| pet has illusion
	/if (${Pet.Buff[${Spell[${FindItem[${maMinion.Find[stPetIllusion].Value}].Clicky.SpellID}].Name}]}) {
		/return TRUE
	}


	| chekck for pet illusion
	/if (${is_ready[FALSE, "${maMinion.Find[stPetIllusion].Value}"]}) {
		/call voodoo FALSE "${maMinion.Find[stPetIllusion].Value}" ${Pet.ID} FALSE
	}

/return TRUE



|***
 * note: returns a non container inventory slot ##, 0 on full bag inventory, you can pass an optional name to find an item in a top slot
 * use: ${get_openPackSlot[DEBUG, [itemname]]}
 ***|
sub get_openPackSlot(bool _debug, string _orFind)
	DEBUG \atget_openPackSlot\ax(\a-w${_orFind}\ax)
	FLOW get_openPackSlot

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
	DEBUG \atgive_to_pet\ax(\a-w${_item}\ax)
	FLOW give_to_pet ${_item}

	/invoke ${target[set, ${Pet.ID}]}
	FLOW give_to_pet ${_item}

	ISMEDEAD
	GETINPUT give_to_pet NA|NA
	/if (${Cursor.ID} && ${Me.Pet.ID}) {
		/if (${Cursor.Name.Equal[${_item}]}) {
			/click left target
			/delay 5s !${Bool[${Cursor.ID}]}
		} else {
			/call check_cursor ${_debug} CLEAR
		}
	}

/return TRUE



|***
 * note: equip pet weapons
 * use: /call get_minion_summon_weapons DEBUG
 ***|
sub get_minion_summon_weapons(bool _debug)
	DEBUG \atget_minion_summon_weapons\ax()
	FLOW get_minion_summon_weapons

	/if (!PET || !${Pet.ID} || !${maMinion.Find[swUsePetGear].Value}) /return FALSE

	/if (!${Me.FreeInventory}) {
		/invoke ${out[0, "${notice} ${sep} You have no free inventory. Damn hoarders...."]}
		/return FALSE
	}

	CHECKTIE
	ISMEDEAD
	GETINPUT get_minion_summon_weapons NA|NA

	/declare _count int local 0
	/declare _openPackSlot int local 0

	| get everything out of your hands, including your dick
	/call check_cursor ${_debug} CLEAR

	| need a non bag inventory slot or ur fuckled!
	/varset _openPackSlot ${get_openPackSlot[FALSE, "Pouch of Quellious"]}
	DEBUG _openPackSlot ${_openPackSlot}
	/if (!${_openPackSlot} && !${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]}) {
		/invoke ${out[0, "${notice} ${sep} You have no TOP inventory slots that are not bags."]}
		/invoke ${out[0, "Clean up your damn inventory then you can give your pet some weapons."]}
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
	/call check_cursor ${_debug} CLEAR

	/if (${FindItemCount[=${get_MAG_folded_pack[]}]}) {
		DEBUG :havemageclosedbag
		/if (${Select[${FindItem[=${get_MAG_folded_pack[]}].InvSlot},23,24,25,26,27,28,29,30,31,32]} != 0) {
			DEBUG \agSWAP\ax ${get_MAG_folded_pack[]} to slot \aw${_openPackSlot}\ax
			/call Bind_command_swap "${get_MAG_folded_pack[]}" ${_openPackSlot} FALSE
		}

		/call check_cursor ${_debug} CLEAR

		| convert the folded pack
		/if (${is_ready[FALSE, "${get_MAG_folded_pack[]}"]}) {
			/call voodoo FALSE "${get_MAG_folded_pack[]}" 0 FALSE
		}

		/call check_cursor ${_debug} CLEAR

	}

	/call check_cursor ${_debug} CLEAR

	| have 2 blades
	:havepetweapons
	DEBUG :havepetweapons
	/if (${Cursor.Name.Equal[Pouch of Quellious]}) /autoinventory
	/for _count 1 to 2
		/call check_cursor ${_debug} CLEAR
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

	/call check_cursor ${_debug} CLEAR

	:havepouchandnoweapons
	DEBUG :havepouchandnoweapons
	/if (${FindItemCount[=Pouch of Quellious]} && ${FindItemCount[=${maMinion.Find[stPetWeapon].Value}]} < 2) {
		/nomodkey /itemnotify ${FindItem[Pouch of Quellious].InvSlot} leftmouseup
		/delay 1
		/call item_delete FALSE "Pouch of Quellious"

	}

	/if (${Cursor.ID}) /call check_cursor ${_debug} CLEAR

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
	FLOW item_delete

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
	FLOW item_find

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
	FLOW get_MAG_folded_pack

	/if (!${Defined[stMAGFoldedPack]}) {
		/declare stMAGFoldedPack string outer
	}

	/declare _lsout list local
	/invoke ${_lsout.Append[Wirn's,Thassis',Frightforged,Manaforged,Spectral]}
	/devlare _ligmfp listiterator local
	/vardata _ligmfp _lsout.First.Clone
	/while (!${_ligmfp.IsEnd}) {
		/if (${FindItemCount[Folded Pack of ${_ligmfp.Value} Armaments]}) {
			/varset stMAGFoldedPack Folded Pack of ${_ligmfp.Value} Armaments
		}
		/invoke ${_ligmfp.Advance}
	}

/return ${stMAGFoldedPack}



|***
 * note: active Shield Flash when HP reach a specific % or a number of mobs in immediate range
 * use: /call cast_shield_flash DEBUG [aa|disc]
 ***|
sub cast_shield_flash(bool _debug, string _type)
	DEBUG \atcast_shield_flash\ax(\a-w${_type}\ax)
	FLOW cast_shield_flash

	/if (!AUTO) {
		/return FALSE
	}

	| health low enough || enough mobs in range
	/if (${Me.PctHPs} > ${maChr.Find[stPctShieldFlash].Value} && ${SpawnCount[npc radius SAFEENV PS4 targetable]} < ${maChr.Find[stCountShieldFlash].Value}) {
		/return FALSE
	}

	| is the skill available
	/if (${_type.Equal[aa]}) {
		/if (!${is_ready[FALSE, "Shield Flash"]}) {
			/return FALSE
		}
	} else /if (${_type.Equal[disc]}) {
		/if (!${is_ready[FALSE, "${maChr.Find[stFlash].Value}"]}) {
			/return FALSE
		}
	}

	| mark current bandolier and swap to flash
	/declare _Bandolier string local FALSE

	| take note of current Bandolier
	/if (!${Me.Bandolier[Flash].Active}) {
		/declare _count int local 0

		/for _count 1 to 12
			/if (${Me.Bandolier[${_count}].Active}) {
				/varset _Bandolier ${Me.Bandolier[${_count}]}
				/break
			}
		/next _count
	}

	| pally / shadowknight
	/if (${_type.Equal[aa]}) {
		/call check_facing ${maDebug.Find[face].Value} TARGET FORCE
		/invoke ${Me.Bandolier[Flash].Activate}
		/call voodoo FALSE "Shield Flash" 0 FALSE

	| warior
	} else /if (${_type.Equal[disc]}) {
		/call check_facing ${maDebug.Find[face].Value} TARGET FORCE
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
 * use: /call set_bandolier DEBUG [1h|2h|range|no|avatar]
 ***|
sub set_bandolier(bool _debug)
	DEBUG \atset_bandolier\ax()
	FLOW set_bandolier

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
	FLOW cast_glyph

	/if (${maCC.Find[stGlyph].Value.Equal[FALSE]}) /return FALSE

	/if (${Raid.Members}) {
		/if (!${maCC.Find[swGlyphRaid].Value}) {
			/return FALSE
		}
	} else /if (${Group.Members}) {
		/if (!${maCC.Find[swGlyphGroup].Value}) {
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

${AltAbility[${maCC.Find[stGlyph].Value}].CanTrain}

|***
 * note: uses the aa Spire you have set
 * use: /call cast_AA_spire DEBUG
 ***|
sub cast_AA_spire(bool _debug)
	DEBUG \atcast_AA_spire\aw()
	FLOW cast_AA_spire (1)

	ISMEDEAD
	CHECKEXIT cast_AA_spire
	FLOW cast_AA_spire (2)
	GETINPUT cast_AA_spire NA|NA


	| apply any conditions
	/if (!${check_condition[${maDebug.Find[condition].Value}, ${maChr.Find[stSpireCon].Value}]}) {
		/return FALSE
	}

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

/return




|***
 * note: set hud varaibles
 * use: not for your use
 ***|
sub set_hud_data(string _mode)
	| SDEBUG \atset_hud_data\ax(\a-w${_mode}\ax)

	/varset h_build ${maEnv.Find[build].Value}

	/if (${Bool[${_mode}]}) {
		/varset h_mode ${_mode}
	} else {
		/varset h_mode
	}

	/varset h_time 					${maData.Find[swHUDTime].Value}

	| engages
	/varset h_engage				${maCC.Find[stPctEngage].Value}
	/varset h_engagepet			${maMinion.Find[stPctPetEngage].Value}
	/varset h_engageswarm	 	${maMinion.Find[stPctSwarmEngage].Value}
	/varset h_swpet					${maMinion.Find[swPet].Value}
	/varset h_swswarm				${maMinion.Find[swSwarm].Value}

	| cc modes
	/varset h_auto					${maEnv.Find[swAuto].Value}
	/varset h_melee					${maCC.Find[swCombatMelee].Value}
	/varset h_range					${maCC.Find[swCombatRange].Value}
	/varset h_nuke					${maCC.Find[swCombatNuke].Value}
	/varset h_dot						${maCC.Find[swCombatDoT].Value}
	/varset h_heal					${maCC.Find[swCombatHeal].Value}
	/varset h_control				${maCC.Find[swCombatControl].Value}
	/varset h_env						${maEnv.Find[stEnvRadius].Value}

	| healing
	| /varset h_healmode			${maHeal.Find[stHealMode].Value}
	/varset h_override			${maHeal.Find[stHealAdjust].Value}
	/varset h_self					${maHeal.Find[swHealSelf].Value}
	/varset h_group					${maHeal.Find[swHealGroup].Value}
	/varset h_xt						${maHeal.Find[swHealXTarget].Value}
	/varset h_pet						${maHeal.Find[swHealPet].Value}
	/varset h_cure					${maHeal.Find[swHealCure].Value}
	/varset h_tot						${maHeal.Find[swHealToT].Value}

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
	/varset h_agro					${maCC.Find[swAgro].Value}
	/varset h_pull					${maCC.Find[swPull].Value}
	/varset h_pullarea			${maCC.Find[stPullRadius].Value}
	/varset h_pullmode			${maCC.Find[stPullMode].Value}

	| tie
	/varset h_tie						${maTie.Find[swSetTie].Value}
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

	| character build
	/varset h_eset					${maEnv.Find[stBuildSetCurrent].Value}

	| build dev variables
	/varset h_dcombat					${maCC.Find[swSetCombat].Value}
	/varset h_dburn						${maBurn.Find[swBurnConditionMet].Value}
	/varset h_denv						${maEnv.Find[stEnvRadius].Value}
	/varset h_dsafe						${maEnv.Find[stEnvSafeRadius].Value}
	/varset h_dpull						${maCC.Find[stPullRadius].Value}
	/varset h_dcaststatus			${maData.Find[stCastReturn].Value}


/return TRUE




|***
 * note: passive toggle AA Agro switches
 * use: auto based on /cc agro switch
 ***|
sub set_AA_agro(bool _debug)
	DEBUG \atset_AA_agro\ax()
	FLOW set_AA_agro

	ISMEDEAD
	GETINPUT set_AA_agro NA|NA

	| turn agro ON
	/if (AGRO) {

		| adjust bold attacks AA
		/if (${Me.AltAbilityReady[Bold Attacks: Disabled]}) {
			/alt act ${Me.AltAbility[Bold Attacks: Disabled].ID}
			/invoke ${out[2, "AA${sep}${enabled}${sep}\a-wBold Attacks\ax"]}
		}

		| SHD Soul Flay
		/if (${Select[${Me.Class.ShortName},SHD]}) {
			/if (${Me.AltAbilityReady[Soul Flay: Disabled]}) {
				/alt act ${Me.AltAbility[Soul Flay: Disabled].ID}
				/invoke ${out[2, "AA${sep}${enabled}${sep}\a-wSoul Flay\ax"]}
			}
		}

		| BST Combat Subtlety
		/if (${Select[${Me.Class.ShortName},BST]}) {
			/if (${Me.AltAbilityReady[Combat Subtlety: Enabled]}) {
				/alt act ${Me.AltAbility[Combat Subtlety: Enabled].ID}
				/invoke ${out[2, "AA${sep}${disabled}${sep}\a-wCombat Subtlety\ax"]}
			}
		}

		| ranger ferocious kicks
		/if (${Select[${Me.Class.ShortName},RNG]}) {
			/if (${Me.AltAbilityReady[Ferocious Kick Subtlety: Enabled]}) {
				/alt act ${Me.AltAbility[Ferocious Kick Subtlety: Enabled].ID}
				/invoke ${out[2, "AA${sep}${disabled}${sep}\a-wFerocious Kick Subtlety\ax"]}
			}
		}

	| turn agro OFF
	} else /if (!AGRO) {

		| adjust bold attacks AA
		/if (${Me.AltAbilityReady[Bold Attacks: Enabled]}) {
			/alt act ${Me.AltAbility[Bold Attacks: Enabled].ID}
				/invoke ${out[2, "AA${sep}${disabled}${sep}\a-wBold Attacks\ax"]}
		}

		| SHD Soul Flay
		/if (${Select[${Me.Class.ShortName},SHD]}) {
			/if (${Me.AltAbilityReady[Soul Flay: Enabled]}) {
				/alt act ${Me.AltAbility[Soul Flay: Enabled].ID}
				/invoke ${out[2, "AA${sep}${disabled}${sep}\a-wSoul Flay\ax"]}
			}
		}

		| BST Combat Subtlety
		/if (${Select[${Me.Class.ShortName},BST]}) {
			/if (${Me.AltAbilityReady[Combat Subtlety: Disabled]}) {
				/alt act ${Me.AltAbility[Combat Subtlety: Disabled].ID}
				/invoke ${out[2, "AA${sep}${enabled}${sep}\a-wCombat Subtlety\ax"]}
			}
		}

		| ranger ferocious kicks
		/if (${Select[${Me.Class.ShortName},RNG]}) {
			/if (${Me.AltAbilityReady[Ferocious Kick Subtlety: Disabled]}) {
				/alt act ${Me.AltAbility[Ferocious Kick Subtlety: Disabled].ID}
				/invoke ${out[2, "AA${sep}${enabled}${sep}\a-wFerocious Kick Subtlety\ax"]}
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
	FLOW get_target_intpull ${_tmpID}

	/if (!AUTO || !PULL) {
		/return FALSE
	}

	ISMEDEAD
	GETINPUT get_target_intpull NA|NA
	CHECKTIE

	/if (${_tmpID}) {
		/varset stAssTarID ${_tmpID}
	}	else {
		/call pull_mob_find FALSE
	}

	/if (!TARGET) {
		SETHUD EMPTY
		/return FALSE
	}

	/invoke ${target[set, TARGET]}
	FLOW get_target_intpull ${_tmpID}

	/delay 1s
	/dex ${get_resident_class[${_debug}, CLR, 60]} /advancedpullint ${Me.ID} ${_debug}
	/delay 1s

	/call pull_mob_prep ${_debug} TARGET

	SETHUD EMPTY

/return TRUE



|***
 * note: moves to an established stAssTarID
 * use: /call pull_mob_move_to DEBUG [TARGETID] [distance to stop at] [Line Of Sight on|off]
 ***|
sub pull_mob_move_to(bool _debug, int _tmpID, int _dist, string _LoS)
	DEBUG \atpull_mob_move_to\ax(\a-w${_tmpID}, ${_dist}, ${_LoS}\ax)
	FLOW pull_mob_move_to

	/nav ID ${_tmpID} |distance=${_dist} lineofsight=${_LoS} log=off
	/delay 1s ${Me.Moving}

	/while (${Navigation.Active}) {
		DEBUG ${sep}moving to${sep}\a-w${_tmpID}\ax

		| am I dead on the move
		/if (${is_me_dead[${maDebug.Find[rez].Value}]}) {
			DEBUG ${sep}/while \a-gis_me_dead\ax
			/invoke ${target[clear]}
			/return FALSE
		}

		| are we there yet???
		/if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]} > ${_dist} && !${Spawn[ID TARGET].LineOfSight}) {
			DEBUG ${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${Spawn[TARGET].Y} ${Spawn[TARGET].X} ${Spawn[TARGET].Z}]} > ${_dist} && !${Spawn[ID TARGET].LineOfSight}
			/delay 5
			| /continue
		}

		| range from camp check
		/if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} > PULLENV && !${swModeToken}) {
			DEBUG Mob exceeded setMobAGro range
			/varset _tmpID 0
			/return
		}
		/delay 1

		GETINPUT pull_mob_move_to _tmpID|${_tmpID}
		/if (!${_tmpID}) {
			/call set_stop ${maDebug.Find[stop].Value} SILENT
			/return FALSE
		}
		/if (${Macro.Return}) {
			/break
		}
	}

	/call pull_mob_tag ${_debug} ${_tmpID}

/return TRUE



|***
 * note: uses a pet class to get mobs to camp
 * use: /call get_target_petpull DEBUG [targetID]
 ***|
sub get_target_petpull(bool _debug, int _tmpID)
	DEBUG \atget_target_petpull\ax(\a-w${_tmpID}\ax)
	FLOW get_target_petpull ${_tmpID}

	ISMEDEAD
	GETINPUT get_target_petpull _tmpID|${_tmpID}
	CHECKTIE

	/if (!${Pet.ID} && PET) {
		/if (${Me.SpellInCooldown}) {
			/delay 5s !${Me.SpellInCooldown}
		}
		/call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
	}


	/declare _count int local 0

	/if (!PULL) {
		/call set_pull_observer ${_debug}
	}	else {
		/call pull_mob_find ${_debug}
	}

	/if (PULL && !TARGET) {
		DEBUG ${sep}\arNO\ax stAssTarID
		SETHUD EMPTY
		/return FALSE
	} else {
		/if (!${_tmpID}) {
			/varset _tmpID TARGET
		}
	}

	/invoke ${target[set, ${_tmpID}]}
	FLOW get_target_petpull ${_tmpID}

	/invoke ${set_timer[${_debug}, timer_giveUpPull, 30s]}

	DEBUG ${sep}target${sep}\ay${_tmpID}\ax${sep}\aw${Spawn[${_tmpID}].DisplayName}\ax ${sep} dist:\a-w${Spawn[${_tmpID}].Distance}\ax

	/if (${Me.Pet.Taunt}) /pet taunt
	/if (${Me.Pet.Stance.NotEqual[follow]}) /pet follow
	/if (!HOME) /call Bind_control_home set FALSE silent ${maDebug.Find[home].Value}

	/invoke ${out[9, "Fetching${sep}${tar}${Spawn[id ${_tmpID}].DisplayName}\ax (\a-r${Spawn[${_tmpID}].Distance}\ax)"]}

	/if (!${Pet.ID} && PET) {
		/call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
	}


	/declare _melee float local ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}

	| get closer if we need too
	/if (${Spawn[${_tmpID}].Distance} > ${maHard.Find[stMaxRadPet].Value}) {
		/call pull_mob_move_to ${_debug} ${_tmpID} ${maCC.Find[stPullNavStopDistance].Value} off
		/if (!${timer_giveUpPull}) /goto :timerfail
	}

	/while (${Me.Moving}) {
		/delay 2
	}

	/delay 10s ${Me.Pet.Distance} < ENV
	/pet attack

	/while (${Math.Distance[${Me.Pet.Y},${Me.Pet.X}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X}]} > ${_melee}) {
		/if (!${Spawn[id ${_tmpID}].ID} || ${Spawn[id ${_tmpID}].Dead}) /break
		/pet attack
		| /delay 5
		DEBUG ${sep}waiting on pet to get to target${sep}\a-w${Math.Distance[${Me.Pet.Y},${Me.Pet.X}:${Spawn[${_tmpID}].Y},${Spawn[${_tmpID}].X}]}\ax
		/delay 1s ${Math.Distance[${Me.Pet.Y}, ${Me.Pet.X}, ${Me.Pet.Z} : ${Spawn[${_tmpID}].Y}, ${Spawn[${_tmpID}].X}, ${Spawn[${_tmpID}].Z}]} > ${_melee}
		| /delay 5
		/if (${Target.AggroHolder.Name.Equal[${Me.Pet.Name}]}) /break
		/if (!${Me.Pet.ID} || !${_tmpID}) /break
		GETINPUT get_target_petpull_range _melee|${_melee}
		/if (!${timer_giveUpPull}) /goto :timerfail
	}

	/call where_the_fuck_is_my_pet ${maDebug.Find[lost].Value} FALSE

	/if (HOME) {
		/call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
	}
	/if (!${timer_giveUpPull}) /goto :timerfail

	| wait for mob to get to camp
	:waitformob

	/declare _waittime int local ${Math.Calc[(PULLENV + ${Spawn[${_tmpID}].Distance})*.08]}
	/call get_target_wait ${maDebug.Find[wait].Value} ${_tmpID} ${_waittime}s

	:timerfail
	/if (!${timer_giveUpPull}) {
		| /invoke ${out[9, "${notice} ${dot} Timer to get mob failed!"]}
		/call where_the_fuck_is_my_pet ${maDebug.Find[lost].Value} TRUE
		/if (HOME) {
			/call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
		}
	}

	SETHUD EMPTY

/return



|***
 * note: mechanics for nav pulling
 * note: /call get_target_navpull DEBUG [targetID]
 ***|
sub get_target_navpull(bool _debug, int _tmpID)
	DEBUG \atget_target_navpull\ax(\a-w${_tmpID}\ax)
	FLOW get_target_navpull ${_tmpID}

	ISMEDEAD
	GETINPUT get_target_navpull _tmpID|${_tmpID}
	CHECKTIE

	/declare _count int local 0

	/if (!PULL) {
		/call set_pull_observer ${_debug}
	}	else {
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
	/if	(!${Navigation.PathExists[id ${_tmpID}]} && !${maOver.Find[swOverNavCheck].Value}) {
		GETINPUT get_target_navpull_wait _tmpID|${_tmpID}
		/invoke ${out[9, "${notice}${sep}No Nav Path ${dot} ${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
		/varcalc _count ${_count}+1
		/if (${_count} > 10) /return FALSE
		/delay 1s
		/call pull_mob_find FALSE ${_count}
	}


	| /declare _movetorange float local ${Math.Calc[${Spawn[${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}

	DEBUG ${sep}Target ${sep} ${_tmpID} ${sep} \aw${Spawn[${_tmpID}].DisplayName}\ax

	| force home even if not using it
	/if (${maHome.Find[swinCombat].Value} && ${maCC.Find[swPullSetHome].Value} && !HOME) {
		/call Bind_control_home set FALSE silent ${maDebug.Find[home].Value}
	}

	| set the target
	/invoke ${out[9, "Fetching${sep}${tar}${Spawn[${_tmpID}].DisplayName}\ax"]}
	/invoke ${target[set, ${_tmpID}]}
	FLOW get_target_navpull ${_tmpID}

	| if the mob is kinda close??
	/if (${Spawn[${_tmpID}].LineOfSight} && ${Spawn[${_tmpID}].Distance} < ENV) {
		DEBUG ${sep}navpull${sep}in sight < ENV
		/call pull_mob_prep ${_debug} ${_tmpID}
		/return

	| get closer if we need too
	} else /if (!${Spawn[${_tmpID}].LineOfSight} || ${Spawn[${_tmpID}].Distance} > ${maCC.Find[stPullNavStopDistance].Value}) {
		DEBUG ${sep}Sight:${Spawn[${_tmpID}].LineOfSight} || Distance:${Spawn[${_tmpID}].Distance}
		/call pull_mob_move_to ${_debug} ${_tmpID} ${maCC.Find[stPullNavStopDistance].Value} on
		/while (${Me.Moving}) {
			/if (${Spawn[${_tmpID}].LineOfSight}) /break
			/delay 2
		}
		/call pull_mob_prep ${_debug} ${_tmpID}
	}

	/call pull_mob_tag ${_debug} ${_tmpID}

	| move home
	/if (${maCC.Find[swPullSetHom].Value} && ${maCC.Find[swPullSetHome].Value} && HOME) {
		/if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} > SAFEENV && PULL) {
			/call Bind_control_home return FALSE silent ${maDebug.Find[home].Value}
		}


	}

	/while (${Me.Moving}) {
		GETINPUT get_target_navpull_moving _tmpID|${_tmpID}
		ISMEDEAD
		/delay 5
	}

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
	FLOW get_target_multipull

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
	/if (!HOME) /call Bind_control_home set NULL NULL ${maDebug.Find[home].Value}
	| how the hell do we want to kill it.?

	| pull with a normal range if the mob is los and within 100. multi puller keeps local camp under control.
	/if (${Spawn[TARGET].LineOfSight} && ${Spawn[TARGET].Distance} < 100) {
		DEBUG ${sep}Spawn is LOS & < \ag100\ax
		/invoke ${out[9, "Pulling ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax"]}
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
		/if	(${Navigation.PathExists[id TARGET]}) {
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
		} else C
			DEBUG ${sep}NAV Path \arbad\ax
			:intpull
			/call get_target_intpull ${_debug} TARGET
			/goto :waitformob

		}

	}

	| wait for mob to get to camp
	:waitformob
	/if (!${SpawnCount[npc radius ${maHard.Find[stMinBowDist].Value} PS4 targetable]}) {

		/invoke ${set_timer[${_debug}, wait_for_mob, 30s]}
		/invoke ${out[9, "Waiting on ${dot} ${tar}${Spawn[TARGET].DisplayName}\ax${sep}\a-w30s\ax"]}
		/while (TARGET && ${Spawn[TARGET].Distance} > ${maHard.Find[stMinBowDist].Value}) {
			GETINPUT get_target_multipull_wait NA|NA
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
	FLOW find_priest

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
	FLOW where_the_fuck_is_my_pet ${_force}

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
	FLOW set_pull_observer

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
	FLOW get_name_multipull

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
		/echo ${If[${Bool[${Plugin[MQ2Discord]}]},\at-\ax,\at.\ax]} ${_verbage}
	}

/return



|***
 * note: checks and corrects tribute and trophy usafe during a raid
 * use:
 ***|
sub check_tribute(bool _debug)
	DEBUG \atcheck_tribute\aw()
	FLOW check_tribute

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

/return



|***
 * note: checks if a mode needs to be executed
 * use: /call check_mode DEBUG [RESET]
 ***|
sub check_mode(bool _debug, string _verbage)
	DEBUG \atcheck_mode\ax()
	FLOW check_mode

	/invoke ${set_data_timer[FALSE, Check_Mode, RESTART]}

  /declare _out string local
  /declare _mimode mapiterator local
  /vardata _mimode maMode.First.Clone

	| reset used for zoning, and other stuffs
  /if (${_verbage.Equal[reset]}) {
	  /while (!${_mimode.IsEnd}) {
	  	/if (${_mimode.Value}) {
				/invoke ${maMode.Add[${_mimode.Key},FALSE]}
	  	}
			/invoke ${_mimode.Advance}
	  }
  	/return TRUE
  }


  | normal check routine
  /while (!${_mimode.IsEnd}) {
  	/if (${_mimode.Value}) {
  		DEBUG mode_${_mimode.Key}
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
	FLOW set_loot_item

	| check for the item in the DB
	/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${_item}"
	DEBUG ${sep}lootquery SELECT * FROM loot_control WHERE item="${_item}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax

	| we did not find it?
	/if (${sqlite.Status[lootquery].NotEqual[Success]} || !${sqlite.Rows[lootquery]}) {
		DEBUG ${sep}${_item}${sep}\a-w not in database\ax

		| item not in database or lore and have
		/if (${AdvLoot.SList[${_item}].NoDrop} || ${AdvLoot.SList[${_item}].NoDrop} || ${FindItem[=${_item}].Lore} || ${FindItemBank[=${_item}].Lore}) {
			/invoke ${out[0, "${add} ${dot} ${_item} = ${leave} [no drop/lore]"]}
			/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "leave")
			DEBUG ${sep}lootquery INSERT INTO loot_control(item,action) VALUES("${_item}", "leave") \ay${sqlite.Status[lootquery]}\ax

		| by default add everything as keep.
		} else {
			/invoke ${out[0, "${add} ${dot} ${_item} = ${keep} [default]"]}
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
	FLOW vendor_buy

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
				/echo Buying ${_itemtoBuy} ${_count} of ${_qty}
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
	FLOW control_routine (1)

	/if (!AUTO) /return FALSE
	/if (!${madeBuff.Find[swMez].Value}) /return FALSE
	/if (!${maCC.Find[swCombatControl].Value}) /return FALSE

	GETINPUT control_routine NA|NA

	| slow this shit down so we can read it...
	/if (${_debug}) {
		/delay 5
	}

	| ID for unmezzed/short mob
	/declare _mezzID int local 0
	| count of uncontrolled mobs in area
	/declare _countArea int local 0

	/declare _count int local 0

	/if (!${Defined[maMobControl]}) {
		/declare maMobControl map outer
	}

	| check control map for any mobs dead or gone
	/invoke ${control_map[${_debug}, dead]}

	| find unaccounted mobs in my area
	| returns the # of mobs that do not have a current mez timer associated
	/varset _countArea ${control_area[${_debug}]}

	| if ${check_control_area returns more then 1
	| hit the AE routines
	/if (${_countArea}) {
		/call control_mezae ${_debug}
	} else /if (!${_countArea}) {
		/return TRUE
	}

	|** check control map for any mobs marked as mezed with low or no timer
	/varset _mezzID ${control_map[${_debug}, mez]}


	| mez something for christs sake...
	/if (${_mezzID} && ${_countArea} && ${Bool[${madeBuff.Find[stMez].Value}]}) {
		/for _count 1 to ${_countArea}

			| /varset _mezzID ${NearestSpawn[${_count}, npc los range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ENV zradius ${maCC.Find[stZRadius].Value} PS4].ID}
			/varset _mezzID ${control_map[${_debug}, mez]}
			SDEBUG  ${_count}--_mezzID--${_mezzID}
			/call control_mez ${_debug} ${_mezzID}
		/next _count
	}


	**|

	/declare _mzcheck mapiterator local
  /vardata _mzcheck maMobControl.First.Clone
  /while (!${_mzcheck.IsEnd}) {
		/call control_mez ${_debug} ${_mzcheck.Key}
		/invoke ${_mzcheck.Advance}
  }

	/delay 1

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
	FLOW control_area (1)

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
					DEBUG ${sep} deleting ttimer for${sep}\a-wtimer_mez_ID${_tmpID}\ax
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
 * note: cheeck the IDs in the passed list. remove them from the list if they are dead/gone
 * use: ${control_map[DEBUG, THINGIETOCHECK??]}
 *
 * THINGIETOCHECK:
 * dead = check for dead or no more spawns. removes them from the control map
 * mez = find mobs that are not mezed. returns ID of nonmezed or short duration remaining mezed mob
 ***|
sub control_map(bool _debug, string _type)
	DEBUG \atcontrol_map\ax(\a-w${_type}\ax)
	FLOW control_map

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
			/if (${Bool[${maCC.Find[stAssistMode].Value}]} && ${_mimez.Key} == ${Me.XTarget[1].ID}) {
				/invoke ${_mimez.Advance}
				/continue
			}

			| /if (${_mimez.Value.Equal[MEZ]} && ${Defined[timer_mez_ID${_mimez.Key}]}) {

			/if (${_mimez.Value.Equal[MEZ]} && ${Defined[timer_mez_ID${_mimez.Key}]}) {

				/if (!${Defined[timer_mez_ID${_mimez.Key}]}) {
					/invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, 1]}
				}

				| if the timer is < 6 seconds. remezz
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
	FLOW control_mezae

	ISMEDEAD
	GETINPUT control_mezae NA|NA
	/declare _tmpID int local

	/if (!${is_ready[${_debug}, "${madeBuff.Find[stAEMez].Value}"]}) {
		/return FALSE
	}

	/if (${maMobControl.Count} >= 1) {
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
			/if (${Spawn[${_tmpID}].NearestSpawn[3,npc radius 30].ID} && ${Spawn[${_tmpID}].Distance3D} <= 200 && ${Spawn[${_tmpID}].Distance3D} >= ENV*.5) {
				/if (${is_ready[${_debug}, "Beam of Slumber"]}) {
					/if (${Me.Casting.ID}) /stopcast
					/face ID ${_tmpID} ${If[${maHome.Find[swFaceFast].Value},fast,]}
					/call voodoo ${_debug} "Beam of Slumber" 0 FALSE
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
	FLOW control_mez ${_tmpID}

	/if (!${_tmpID}) /return FALSE

	/if (!${Defined[timer_mez_ID${_tmpID}]}) {
		/invoke ${set_timer[${_debug}, timer_mez_ID${_tmpID}, 1]}
	}

	/if (${timer_mez_ID${_tmpID}} > 60) {
		/return FALSE
	}

	/declare _resistCount int local 0
	GETINPUT control_mez NA|NA

	| try to mez more then once if there is a fail
	/while (${_resistCount} < ${madeBuff.Find[stCountMezResist].Value}) {

		/invoke ${target[set, ${_tmpID}]}
		FLOW control_mez ${_tmpID}
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
					/sqlite ${maData.Find[DBzone].Value} dummy INSERT INTO zone_control VALUES ("${Zone.ShortName}", "lsZoneNoMez", "${Target.DisplayName}")
					/invoke ${out[15, "/area nomez ${dot} ${num}lsZoneNoMez\ax${sep}\a-w${Target.DisplayName}\ax"]}
					/call set_zone_lists FALSE
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
 ***|
sub set_control_output(bool _debug, string _map, string _outname, string _control)
	DEBUG \atset_control_output\ax(\a-w${_map}, "${_outname}", ${_control}\ax)
	FLOW set_control_output

	/declare _out string local
	/declare _value string local
  /vardata cmi ${_control}.First.Clone
	/while (!${cmi.IsEnd}) {
		DEBUG ${sep}left 2${sep}\a-w${cmi.Value.Left[2]}\ax
		DEBUG ${sep}${cmi.Key}${sep}\a-w${cmi.Value}\ax${sep}\a-y${${_map}.Find[${cmi.Value}].Value}\ax

		/if (${cmi.Value.Count[|]}) {
			/varset _value ${cmi.Value.Arg[1,|]}
		} else {
			/varset _value ${cmi.Value}
		}

		| submenu only
		/if (${cmi.Value.Equal[submenu]}) {
			/varset _out ${_out} ${info}${cmi.Key}\ax:${submenu}

		| string in brackets
		} else /if (${cmi.Value.Left[2].Equal[st]} && ${cmi.Value.Arg[2,|].Equal[br]}) {
			/varset _out ${_out} ${info}${cmi.Key}\ax:[${If[${Bool[${${_map}.Find[${_value}].Value}]},${num}${${_map}.Find[${_value}].Value}\ax,${off}]}]

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
 * note: returns current priority dannet group
 * use: ${set_dnet_group[DEBUG]}
 ***|
sub set_dnet_group(bool _debug)
	FLOW set_dnet_group

	/if (${stCurrentCrew.Equal[empty]} || !${Group.Members}) {
		/return ${maComm.Find[stEntropyGroup_all].Value}
	} else /if (${stCurrentCrew.NotEqual[empty]}) {
		/return ${stCurrentCrew}
	}

/return



|***
 * note: checks heal routines by class
 * use: /call check_heal_routines
 ***|
sub check_heal_routines()
	FLOW check_heal_routines

	| check for any defensives first
	CHECKDEFENSE

	| heal: target of target
	/if (${lsClassHealToT.Contains[${Me.Class.ShortName}]} && ${maHeal.Find[swHealToT].Value}) {
		/call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} ToT
	}

	| heal: self
	/if (${lsClassHealSelf.Contains[${Me.Class.ShortName}]} && ${maHeal.Find[swHealSelf].Value}) {
		/call check_heal_${Me.Class.ShortName} ${maDebug.Find[self].Value} Self
	}

	| heal: group
	/if (${lsClassHealGroup.Contains[${Me.Class.ShortName}]} && ${maHeal.Find[swHealGroup].Value} && ${Group.Members}) {
		/call check_heal_${Me.Class.ShortName} ${maDebug.Find[group].Value} Group
	}

	| heal: extended target lists
	/if (${lsClassHealXT.Contains[${Me.Class.ShortName}]} && ${maHeal.Find[swHealXTarget].Value}) {
		/call check_heal_${Me.Class.ShortName} ${maDebug.Find[xt].Value} XTarget
	}

	| heal: pets
	/if (${lsClassHealPet.Contains[${Me.Class.ShortName}]} && ${maHeal.Find[swHealPet].Value}) {
		/call check_heal_${Me.Class.ShortName} ${maDebug.Find[pet].Value} Pet
	}

	| heal: curing
	/if (!${timer_Check_Cure} && (${maHeal.Find[swHealCureSelf].Value} || ${maHeal.Find[swHealCureGroup].Value} || ${maHeal.Find[swHealCureCrew].Value})) {
		/call check_cure ${maDebug.Find[cure].Value}
	}

	GETINPUT check_heal_routines_end NA|NA
	CHECKREZ

/return TRUE



|***
 * note: checks/corrects a/the a target
 * note: returns FALSE if a target is dead or does not exist, returns TRUE otherwise
 * use: /call target [assist|isdead|clear|set] [TARGETID]
 * use: ${target[assist|isdead|clear|set, [TARGETID]]}
 ***|
sub target(string _type, int _tmpID)
	FLOW target ${_type}|${_tmpID}

	/declare _debug bool local FALSE

	/if (${maDebug.Find[target].Value}) {
		/varset _debug TRUE
		DEBUG \attarget\ax(\a-w${_type}, ${_tmpID}\ax)
	}

	| check assist (XTarget[1] vs. _tmpID
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

		/if (${SubDefined[DPSMeter_Reset]}) {
			/call DPSMeter_Reset
		}

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
	FLOW pull_mob_attraction ${_tmpID}|${_force}

	| /if (!${maChr.Find[swAttraction].Value} && !${_force}) /return FALSE
	/if (${Spawn[ID ${_tmpID}].Distance} > PULLENV) /return FALSE
	| /if (${Spawn[ID ${_tmpID}].Distance} < ${Math.Calc[${Spawn[ID ${_tmpID}].MaxRangeTo}*.${maHard.Find[stMaxMeleeAdj].Value}]}) /return FALSE
	/if (${Spawn[ID ${_tmpID}].Distance} < ${maHard.Find[stMinBowDist].Value}) /return FALSE
	/if (${Me.TargetOfTarget.ID} == ${Me.ID}) /return FALSE

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
	FLOW check_sash_proc

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
		| /echo ${_mob},${_mobcount}
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
	FLOW cast_cycle_defense ${_tmpID}

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
		/invoke ${lsLineDefense.Append[1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20]}
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

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maDefense.Find[stDefense${_liC20.Value}Con].Value}]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		| check any tags
		/call check_basic_tags ${maDebug.Find[tag].Value} Defense${_liC20.Value} ${Me.DisplayName} "${maDefense.Find[stDefense${_liC20.Value}].Value}"
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


		| targetable item with a range?
		| /if (${FindItem[${maDefense.Find[stDefense${_liC20.Value}].Value}].Spell.Duration.TotalSeconds} && ${FindItem[${maDefense.Find[stDefense${_liC20.Value}].Value}].Spell.Range}) {
		| 	/invoke ${target[set, ${_tmpID}]}
		| 	FLOW check_cycle_defense ${_tmpID} (item)
		| 	/if (${Target.BuffDuration[${maDefense.Find[stDefense${_liC20.Value}].Value}].TotalSeconds}) {
		| 		/invoke ${_liC20.Advance}
		| 		/continue
		| 	}
		| }

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
	FLOW check_item_cycle ${_tmpID}|${_tag}

	| if the user has not set the line order, set it to a default for now
	/if (${lsLineItem.Contains[EMPTY]} && ${lsLineItem.Count} == 1) {
		DEBUG ${sep}resetting the empty clickitem line
		/invoke ${lsLineItem.Clear}
		/invoke ${lsLineItem.Append[1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20]}
	}

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

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maItem.Find[stItem${_liC20.Value}Con].Value}]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		| check for specific tags, like epics
		/if (${Bool[${_tag}]}) {
			/if (!${lsItem${_liC20.Value}Tag.Contains[${_tag}]}) {
				/invoke ${_liC20.Advance}
				/continue
			}
		}

		/if (!${is_ready[${_debug}, "${maItem.Find[stItem${_liC20.Value}].Value}"]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Item${_liC20.Value} ${Me.DisplayName} "${maItem.Find[stItem${_liC20.Value}].Value}"
		/if (!${Macro.Return}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		DEBUG ${sep}_liC20.Value${sep}\a-w${_liC20.Value}\ax${sep}passed tags
		/delay 1
		/if (${lsItem${_liC20.Value}Tag.Contains[self]}) {
			/if (!${Me.Buff[${maItem.Find[stItem${_liC20.Value}].ID}]}) {
				/varset _tmpID ${Me.ID}
			}
		}

		/if (${FindItem[${maItem.Find[stItem${_liC20.Value}].Value}].Spell.Duration.TotalSeconds} && ${FindItem[${maItem.Find[stItem${_liC20.Value}].Value}].Spell.Range}) {
			/invoke ${target[set, ${_tmpID}]}
			FLOW check_item_cycle ${_tmpID}|${_tag}
			/if (${Target.BuffDuration[${maItem.Find[stItem${_liC20.Value}].Value}].TotalSeconds}) {
				/invoke ${_liC20.Advance}
				/continue
			}
		}

		| /if (${is_target_dead[${maDebug.Find[target].Value}, ${_tmpID}]}) /return FALSE

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW check_item_cycle ${_tmpID}|${_tag}
			/return FALSE
		}

		/call voodoo ${_debug} "${maItem.Find[stItem${_liC20.Value}].Value}" ${_tmpID} FALSE

		/invoke ${_liC20.Advance}
	}

/return TRUE



|***
 * note: casts nukes from maNuke
 * use: /call cast_cycle_nuke DEBUG [TARGETID] [TAG]
 ***|
sub cast_cycle_nuke(bool _debug, int _tmpID, string _tag)
	DEBUG \atcast_cycle_nuke\ax(\a-w${_tmpID}, ${_tag}\ax)
	FLOW cast_cycle_nuke ${_tmpID}|${_tag} (1)

	| if the user has not set the line order, set it to a default for now
	/if (${lsLineNuke.Contains[EMPTY]} && ${lsLineNuke.Count} == 1) {
		DEBUG ${sep}resetting the empty nuke line
		/invoke ${lsLineNuke.Clear}
		/invoke ${lsLineNuke.Append[${stCount20}]}
	}

	/declare _liNuke listiterator local
	/vardata _liNuke lsLineNuke.First.Clone
	/while (!${_liNuke.IsEnd}) {

		/if (${Me.PctMana} < 2) {
			/return FALSE
		}

		GETINPUT cast_cycle_nuke NA|NA
		ISMEDEAD
		CHECKEXIT cast_cycle_nuke
		FLOW cast_cycle_nuke ${_tmpID}|${_tag}  (2)

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_cycle_nuke ${_tmpID}
			/return FALSE
		}

		| stopnuke %
		/if (${maNuke.Find[stPctStopNuke].Value}) {
			/if (${Spawn[${_tmpID}].PctHPs} <= ${maNuke.Find[stPctStopNuke].Value}) {
				/return FALSE
			}
		}

		/invoke ${target[set, ${_tmpID}]}
		FLOW cast_cycle_nuke ${_tmpID}

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

		| apply any conditions
		/if (!${check_condition[TRUE, ${maNuke.Find[stNuke${_liNuke.Value}Con].Value}]}) {
			/invoke ${_liNuke.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Nuke${_liNuke.Value} ${Me.DisplayName} "${maNuke.Find[stNuke${_liNuke.Value}].Value}"
		/if (!${Macro.Return}) {
			/invoke ${_liNuke.Advance}
			/continue
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
	FLOW cast_cycle_dot ${_tmpID} (1)

	| if the user has not set the line order, set it to a default for now
	/if (${lsLineDoT.Contains[EMPTY]} && ${lsLineDoT.Count} == 1) {
		DEBUG ${sep}resetting the empty dot line
		/invoke ${lsLineDoT.Clear}
		/invoke ${lsLineDoT.Append[1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20]}
	}

	/declare _liC20 listiterator local
	/vardata _liC20 lsLineDoT.First.Clone
	/while (!${_liC20.IsEnd}) {

		/if (${Me.PctMana} < 2) {
			DEBUG ${sep}<2%manna
			/return FALSE
		}

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_cycle_dot ${_tmpID}
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
		FLOW cast_cycle_dot ${_tmpID} (2)

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_cycle_dot ${_tmpID}
			DEBUG ${sep}dead2
			/return FALSE
		}

		/invoke ${target[set, ${_tmpID}]}
		FLOW cast_cycle_dot ${_tmpID}

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

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maDoT.Find[stDoT${_liC20.Value}Con].Value}]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} DoT${_liC20.Value} ${Me.DisplayName} "${maDoT.Find[stDoT${_liC20.Value}].Value}"
		/if (!${Macro.Return}) {
			DEBUG ${sep}tag fail${sep}\a-w${maDoT.Find[stDoT${_liC20.Value}].Value}\ax
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
	FLOW cast_cycle_agro ${_tmpID} (1)

	| if the user has not set the line order, set it to a default for now
	/if (${lsLineAgro.Contains[EMPTY]} && ${lsLineAgro.Count} == 1) {
		DEBUG ${sep}resetting the empty agro line
		/invoke ${lsLineAgro.Clear}
		/invoke ${lsLineAgro.Append[1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20]}
	}

	/declare _liC20 listiterator local
	/vardata _liC20 lsLineAgro.First.Clone
	/while (!${_liC20.IsEnd}) {

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_cycle_agro ${_tmpID}
			DEBUG ${sep}target dead
			/return FALSE
		}

		GETINPUT cast_cycle_agro NA|NA
		ISMEDEAD
		CHECKEXIT cast_cycle_agro
		FLOW cast_cycle_agro ${_tmpID} (2)

		/invoke ${target[set, ${_tmpID}]}
		FLOW cast_cycle_agro ${_tmpID}

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

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maAgro.Find[stAgro${_liC20.Value}Con].Value}]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Agro${_liC20.Value} ${Me.DisplayName} "${maAgro.Find[stAgro${_liC20.Value}].Value}"
		/if (!${Macro.Return}) {
			DEBUG ${sep}tag fail${sep}\a-w${maAgro.Find[stAgro${_liC20.Value}].Value}\ax
			/invoke ${_liC20.Advance}
			/continue
		}

		/call voodoo ${_debug} "${maAgro.Find[stAgro${_liC20.Value}].Value}" ${_tmpID} FALSE

		/invoke ${_liC20.Advance}
	}

/return TRUE



|***
 * note: mash dps alternative
 * use: /call cast_cycle_miscdps DEBUG [TARGETID]
 ***|
sub cast_cycle_miscdps(bool _debug, int _tmpID)
	DEBUG \atcast_cycle_miscdps\ax(\a-w${_tmpID}\ax)
	FLOW cast_cycle_miscdps ${_tmpID} (1)


	| if the user has not set the line order, set it to a default for now
	/if (${lsLineMiscdps.Contains[EMPTY]} && ${lsLineMiscdps.Count} == 1) {
		DEBUG ${sep}resetting the empty MiscDPS line
		/invoke ${lsLineMiscdps.Clear}
		/invoke ${lsLineMiscdps.Append[1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20]}
		/invoke ${lsLineMiscdps.Append[21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|40]}
		/invoke ${lsLineMiscdps.Append[41|42|43|44|45|46|47|48|49|50]}
	}


	/declare _liC50 listiterator local
	/vardata _liC50 lsLineMiscdps.First.Clone
	/while (!${_liC50.IsEnd}) {

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_cycle_miscdps ${_tmpID}
			DEBUG ${sep}target dead
			/return FALSE
		}

		GETINPUT cast_cycle_miscdps NA|NA
		ISMEDEAD
		CHECKEXIT cast_cycle_miscdps
		FLOW cast_cycle_miscdps ${_tmpID} (2)

		/invoke ${target[set, ${_tmpID}]}
		FLOW cast_cycle_miscdps ${_tmpID}

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

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maMiscDPS.Find[stMiscDPS${_liC50.Value}Con].Value}]}) {
			/invoke ${_liC50.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} MiscDPS${_liC50.Value} ${Me.DisplayName} "${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}"
		/if (!${Macro.Return}) {
			DEBUG ${sep}tag fail${sep}\a-w${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}\ax
			/invoke ${_liC50.Advance}
			/continue
		}

		/call voodoo ${_debug} "${maMiscDPS.Find[stMiscDPS${_liC50.Value}].Value}" ${_tmpID} FALSE

		/invoke ${_liC50.Advance}
	}

/return TRUE



|***
 * note: casts burns from maBurn
 * use: /call cast_cycle_burn DEBUG [TARGETID]
 ***|
sub cast_cycle_burn(bool _debug, int _tmpID)
	DEBUG \atcast_cycle_burn\ax(\a-w${_tmpID}\ax)
	FLOW cast_cycle_burn ${_tmpID} (1)

	| if the user has not set the line order, set it to a default for now
	/if (${lsLineBurn.Contains[EMPTY]} && ${lsLineBurn.Count} == 1) {
		DEBUG ${sep}resetting the empty burn line
		/invoke ${lsLineBurn.Clear}
		/invoke ${lsLineBurn.Append[1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20]}
	}

	/declare _liC20 listiterator local
	/vardata _liC20 lsLineBurn.First.Clone
	/while (!${_liC20.IsEnd}) {


		GETINPUT cast_cycle_burn NA|NA
		ISMEDEAD
		CHECKEXIT cast_cycle_burn
		FLOW cast_cycle_burn ${_tmpID} (2)

		/if (${target[isdead, ${_tmpID}]}) {
			FLOW cast_cycle_burn ${_tmpID}
			/return FALSE
		}

		/invoke ${target[set, ${_tmpID}]}
		FLOW cast_cycle_burn ${_tmpID}

		| check the switch
		/if (!${Bool[${maBurn.Find[swBurn${_liC20.Value}].Value}]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		| check the name
		/if (!${Bool[${maBurn.Find[stBurn${_liC20.Value}].Value}]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		/if (!${is_ready[${_debug}, "${maBurn.Find[stBurn${_liC20.Value}].Value}"]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maBurn.Find[stBurn${_liC20.Value}Con].Value}]}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Burn${_liC20.Value} ${Me.DisplayName} "${maBurn.Find[stBurn${_liC20.Value}].Value}"
		/if (!${Macro.Return}) {
			/invoke ${_liC20.Advance}
			/continue
		}

		/call voodoo ${_debug} "${maBurn.Find[stBurn${_liC20.Value}].Value}" ${_tmpID} FALSE


		/if (${Select[${maData.Find[stCastReturn].Value},OUTOFMANA,STUNNED]} && !${maBurn.Find[stBurnDelay].Value}) {
			/break
		}

		/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS,DISTRACTED]} && !${maBurn.Find[stBurnDelay].Value}) {
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
	FLOW cycle_sash (1)

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
		FLOW cast_cycle_bandolier (2)

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

		| apply any conditions
		/if (${check_condition[${_debug}, ${maSash.Find[stSash${_liC12.Value}Con].Value}]}) {
			/varset _condition TRUE
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Sash${_liC12.Value} ${Me.DisplayName} "${maSash.Find[stSash${_liC12.Value}].Value}"
		/if (${Macro.Return}) {
			/varset _tag TRUE
		}

		DEBUG ${sep}_condition${sep}\a-w${_condition}\ax
		DEBUG ${sep}_tag${sep}\a-w${_tag}\ax
		DEBUG ${sep}Me.Bandolier[${maSash.Find[stSash${_liC12.Value}].Value}].Active${sep}\a-w${Me.Bandolier[${maSash.Find[stSash${_liC12.Value}].Value}].Active}\ax

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
	| DEBUG \atis_me_dead\ax()
	FLOW is_me_dead

	| dont check in manual
	/if (!AUTO) {
		/return FALSE
	}

	GETINPUT is_me_dead swIsMeDead|${swIsMeDead}


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
		/invoke ${set_data_timer[FALSE, Check_Behind, 0]}

		/doevents flush

		| stop all the things(s)
		/if (${maHome.Find[swMoveDeathClear].Value}) {
			/call Bind_control_home clear FALSE silent ${maDebug.Find[home].Value}
		}

		| /call set_stop ${maDebug.Find[stop].Value} SILENT
		/invoke ${target[clear]}
		FLOW is_me_dead
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
	FLOW check_for_corpse

	/if (${Me.State.Equal[HOVER]} || ${Me.Hovering} || ${Me.Dead}) {
		/return FALSE
	}
	| fuck your corpse
	/if (!${maRez.Find[swRezGive].Value}) {
		/return	FALSE
	}
	| no one dead
	/if (!${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}) {
		/return FALSE
	}
	| no combat rez
	/if (!${maRez.Find[swRezCombat].Value} && COMBAT) {
		/return FALSE
	}

	| not enough manna for a rez
	/if (${Me.PctMana} < 2 && !${maRez.Find[swRezToken].Value}) {
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
			/invoke ${out[0, "${Me.DisplayName} needs Rez Tokens...."]}
		}
	}

	| set rez name/type
	| necro stuff
	/if (${Select[${Me.Class.ShortName},NEC]}) {
		/if (!${FindItem[=Essence Emerald].ID}) {
			/return FALSE
		}
		/declare _rezNEC												string	local "Convergence"

	| cleric stuff
	} else /if (${Select[${Me.Class.ShortName},CLR]}) {
		/if (!${is_ready[${_debug}, "${maRez.Find[stResurrection].Value}"]}) {
			DEBUG ${sep}in combat rez${sep}\a-wcleric aas\ax
			/declare _divinerezCLR									string	local "Divine Resurrection"
			/declare _rezCLR												string	local "Blessing of Resurrection"

			/if (!${Me.AltAbility[Blessing of Resurrection]} && ${FindItemCount[=Water Sprinkler of Nem Ankh]}) {
				DEBUG ${sep}in combat rez${sep}\a-wcleric epic\ax
				/varset _divinerezCLR					"Water Sprinkler of Nem Ankh"
				/varset _rezCLR 							"Water Sprinkler of Nem Ankh"
			}

		} else /if (${is_ready[${_debug}, "${maRez.Find[stResurrection].Value}"]}) {
			DEBUG ${sep}in combat rez${sep}\a-w${maRez.Find[stResurrection].Value}\ax
			/varset _divinerezCLR					"${maRez.Find[stResurrection].Value}"
			/varset _rezCLR 							"${maRez.Find[stResurrection].Value}"
		}


	| pally, shaman, druid stuff
	} else /if (${Select[${Me.Class.ShortName},PAL,SHM,DRU]}) {

		|  in combat
		/if (${Select[${Me.CombatState},combat]}) {
			/if (!${is_ready[${_debug}, "${maRez.Find[stResurrection].Value}"]}) {
				DEBUG ${sep}in combat rez${sep}\a-w${Me.Class.ShortName} aas\ax
				/declare _rezDRU											string	local "Call of the Wild"
				/declare _rezSHM											string	local "Call of the Wild"
				/declare _rezPAL											string	local "Gift of Resurrection"
			} else /if (${is_ready[${_debug}, "${maRez.Find[stResurrection].Value}"]}) {
				DEBUG ${sep}in combat rez${sep}\a-w${Me.Class.ShortName} aas\ax
				/declare _rezDRU											string	local "${maRez.Find[stResurrection].Value}"
				/declare _rezSHM											string	local "${maRez.Find[stResurrection].Value}"
				/declare _rezPAL											string	local "${maRez.Find[stResurrection].Value}"
			}

		| out of combat
		} else /if (!${Select[${Me.CombatState},combat]}) {
			/if (!${is_ready[${_debug}, "${maRez.Find[stResurrection].Value}"]}) {
				DEBUG ${sep}out combat rez${sep}\a-w${Me.Class.ShortName} aas\ax
				/declare _rezDRU											string	local "Rejuvenation of Spirit"
				/declare _rezSHM											string	local "Rejuvenation of Spirit"
				/declare _rezPAL											string	local "Gift of Resurrection"
			} else /if (${is_ready[${_debug}, "${maRez.Find[stResurrection].Value}"]}) {
				DEBUG ${sep}out combat rez${sep}\a-w${Me.Class.ShortName} ${maRez.Find[stResurrection].Value}\ax
				/declare _rezDRU											string	local "${maRez.Find[stResurrection].Value}"
				/declare _rezSHM											string	local "${maRez.Find[stResurrection].Value}"
				/declare _rezPAL											string	local "${maRez.Find[stResurrection].Value}"
			}
		}
	}

	/if (${maRez.Find[swRezToken].Value}) {
		DEBUG ${sep}Token rez
		/declare _rezEmergency									string	local "Token of Resurrection"
	}

	/declare _count 													int 		local 0
	/declare _tmpID 													int 		local 0

	| make the damn variables
	/for _count 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value}]}
		/varset _tmpID ${NearestSpawn[${_count},pccorpse radius ${maRez.Find[stMaxRezRange].Value}].ID}

		/if (!${Defined[timer_corpse_${_tmpID}]}) {
			/declare timer_corpse_${_tmpID} timer outer 1
			/delay 1
		}

		| for the people that suck ass (not in a good way)
		/if (${lsNoRezToon.Contains[${Spawn[${_tmpID}].DisplayName}]}) {
			/varset timer_corpse_${_tmpID} ${maHard.Find[stRezIgnoreFucktards].Value}
		}

	/next _count

	SETHUD "resurrect"

	| clerics first group or raid
	/if (${SpawnCount[pccorpse cleric raid radius ${maRez.Find[stMaxRezRange].Value}]} && ${Raid.Members} || ${SpawnCount[pccorpse cleric group radius ${maRez.Find[stMaxRezRange].Value}]}) {
		/for _count 1 to ${SpawnCount[pccorpse cleric radius ${maRez.Find[stMaxRezRange].Value}]}
			/varset _tmpID ${NearestSpawn[${_count},pccorpse cleric radius ${maRez.Find[stMaxRezRange].Value}].ID}
			/if (!${Defined[timer_corpse_${_tmpID}]}) /declare timer_corpse_${_tmpID} timer outer 0
			/if (!${timer_corpse_${_tmpID}}) {
				/if (${Select[${Me.Class.ShortName},CLR]}) {
					/if (${maRez.Find[swDivineCleric].Value} && (${Me.AltAbilityReady[${_divinerezCLR}]} || ${Me.ItemReady[${_divinerezCLR}]})) {
						/call cast_rez ${maDebug.Find[rez].Value} ${_tmpID} ${_divinerezCLR}
					}
				} else /if (${Select[${Me.Class.ShortName},CLR,DRU,PAL,SHM,NEC]} && ${Me.AltAbilityReady[${_rez${Me.Class.ShortName}}]}) {
					/call cast_rez ${_debug} ${_tmpID} ${_rez${Me.Class.ShortName}}
				} else /if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
					/call cast_rez ${_debug} ${_tmpID} ${_rezEmergency}
				}
				/return
			}
		/next _count
	}

	| group members
	/if (${SpawnCount[pccorpse group radius ${maRez.Find[stMaxRezRange].Value}]}) {
		/for _count 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value} group]}
		/if (!${Defined[timer_corpse_${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} group].ID}]}) /continue
		/varset _tmpID ${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} group].ID}
		/if (!${timer_corpse_${_tmpID}}) {

			/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
				/call cast_rez ${_debug} ${_tmpID} ${_rezEmergency}
			} else /if (${Defined[_rez${Me.Class.ShortName}]}) {
				/call cast_rez ${_debug} ${_tmpID} ${_rez${Me.Class.ShortName}}
			}
			/return
		}
		/next _count
	}

	| tanks
	/if (${Raid.Members}) {
		/if (${SpawnCount[pccorpse raid tank radius ${maRez.Find[stMaxRezRange].Value}]}) {
			/for _count 1 to ${SpawnCount[pccorpse raid radius ${maRez.Find[stMaxRezRange].Value} tank]}
			/if (!${Defined[timer_corpse_${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value} tank].ID}]}) /continue
			/varset _tmpID ${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value} tank].ID}
			/if (!${timer_corpse_${_tmpID}}) {
				/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]} && (!${lsRezTokenToon.CountOf[EMPTY]} && ${lsRezTokenToon.Count} != 1)) {
					/call cast_rez ${_debug} ${_tmpID} ${_rezEmergency}
				} else /if (${Defined[_rez${Me.Class.ShortName}]}) {
					/call cast_rez ${_debug} ${_tmpID} ${_rez${Me.Class.ShortName}}
				}
				/return
			}
			/next _count
		}

		| remaining raid members
		/if (${SpawnCount[pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}]}) {
			/for _count 1 to ${SpawnCount[pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}]}
			/if (!${Defined[timer_corpse_${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}].ID}]}) /continue
			/varset _tmpID ${NearestSpawn[${_count},pccorpse raid radius ${maRez.Find[stMaxRezRange].Value}].ID}
			/if (!${timer_corpse_${_tmpID}}) {
				/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
					/call cast_rez ${_debug} ${_tmpID} ${_rezEmergency}
				} else /if (${Defined[_rez${Me.Class.ShortName}]}) {
					/call cast_rez ${_debug} ${_tmpID} ${_rez${Me.Class.ShortName}}
				}
				/return
			}
			/next _count
		}
	}

	| guildies - all the time
	/if (${SpawnCount[pccorpse guild radius ${maRez.Find[stMaxRezRange].Value}]}) {
		/for _count 1 to ${SpawnCount[pccorpse radius ${maRez.Find[stMaxRezRange].Value} GUILD]}
		/if (!${Defined[timer_corpse_${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} guild].ID}]}) /continue
		/varset _tmpID ${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value} guild].ID}
		/if (!${timer_corpse_${_tmpID}}) {
			/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
				/call cast_rez ${_debug} ${_tmpID} ${_rezEmergency}
			} else /if (${Defined[_rez${Me.Class.ShortName}]}) {
				/call cast_rez ${_debug} ${_tmpID} ${_rez${Me.Class.ShortName}}
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
			/if (!${Defined[timer_corpse_${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value}].ID}]}) /continue
			/varset _tmpID ${NearestSpawn[${_count}, pccorpse radius ${maRez.Find[stMaxRezRange].Value}].ID}
			/if (!${timer_corpse_${_tmpID}}) {
				/if (${maRez.Find[swRezToken].Value} && ${lsRezTokenToon.Contains[${Spawn[${_tmpID}].Class.ShortName}]}) {
					/call cast_rez ${_debug} ${_tmpID} ${_rezEmergency}
				} else /if (${Defined[_rez${Me.Class.ShortName}]}) {
					/call cast_rez ${_debug} ${_tmpID} ${_rez${Me.Class.ShortName}}
				}
				/return
			}
			/next _count
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
	FLOW cast_rez ${_tmpID}|${_rez} (1)


	/if (${Me.Dead}) {
		/return FALSE
	}
	/if (!${Defined[timer_corpse_${_tmpID}]}) {
		/return FALSE
	}
	/if (${timer_corpse_${_tmpID}}) {
		/return FALSE
	}

	ISMEDEAD
	CHECKEXIT cast_rez ${_tmpID}|${_rez}
	FLOW cast_rez ${_tmpID}|${_rez} (2)

	SETHUD "resurrect"

	| ignore the people we dont like
	/if (${lsNoRezToon.Contains[${Spawn[pccorpse ${_tmpID}].DisplayName}]}) {
		/invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezIgnoreFucktards].Value}]}
		/return FALSE
	}

	| get the corpse close to us
	/if (${Spawn[pccorpse ${_tmpID}].Distance} <= ${maHard.Find[stRezSummonRange].Value}) {
		/squelch /corpse
	}

	| can we AA res or token, maybe spell?
	/if (${Me.AltAbilityReady[${_rez}]} || (${Me.ItemReady[Token of Resurrection]} && ${maRez.Find[swRezToken].Value}) || ${Me.SpellReady[${_rez}]} || ${Me.ItemReady[${_rez}]}) {

		| not token rez rez
		/if (!${maRez.Find[swRezToken].Value}) {
			DEBUG ${sep}casting${sep}\a-w${_rez}\ax
			/delay 5s !${Me.Casting.ID}

			/squelch /target id ${_tmpID}
			/delay 1s ${Target.ID} == ${_tmpID}

			FLOW check_for_corpse ${_tmpID}|${_rez}

			/call voodoo ${_debug} "${_rez}" ${_tmpID} skiptargetcheck

		| token rez
		} else /if (${maRez.Find[swRezToken].Value}) {
			DEBUG ${sep}casting${sep}\a-wclicking token\ax
			/delay 5s !${Me.Casting.ID}

			/squelch /target id ${_tmpID}
			/delay 1s ${Target.ID} == ${_tmpID}

			FLOW check_for_corpse ${_tmpID}|${_rez}
			/call voodoo ${_debug} "Token of Resurrection" ${_tmpID} skiptargetcheck
			/varset _rez "Token of Resurrection"
		}

		/doevents

		| if cast was good, set the timers.
		/if (${Select[${maData.Find[stCastReturn].Value},SUCCESS]}) {
			/invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezSuccess].Value}]}
			/invoke ${out[0, "Rez Success${sep}\a-w${Spawn[${_tmpID}].DisplayName}\ax (\a-r${_rez}\ax)"]}
		} else {
			/if (${Select[${Me.Class.ShortName},CLR,DRU,SHM,PAL,NEC]}) {
				/invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezFail].Value}]}
			} else {
				/invoke ${set_timer[${_debug}, timer_corpse_${_tmpID}, ${maHard.Find[stRezFailTolken].Value}]}
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
	FLOW cast_cycle_heal_regen ${_tmpID}|${_tag}

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
	/if (${_tag.Equal[healgrouphot]}) {
		DEBUG ${sep}tag.Found

		| break on no mobs in area
		/if (AE# && ${SpawnCount[npc radius ENV PS4 targetable]} < AE#) {
			DEBUG ${sep}break.not enough mobs in ae rad
			/return FALSE
		}

		| find my hot
		/while (!${_liHoT.IsEnd}) {

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
			/if (!${lsHeal${_liHoT.Value}Tag.Contains[healgrouphot]}) {
				DEBUG ${sep}break.wrongtag
				/invoke ${_liHoT.Advance}
				/continue
			}

			| check matching tag
			/if (${lsHeal${_liHoT.Value}Tag.Contains[healgrouphot]}) {
				DEBUG ${sep}setting _myHoT${sep}\a-w${maHeal.Find[stHeal${_liHoT.Value}].Value}\ax
				/varset _myHoT ${maHeal.Find[stHeal${_liHoT.Value}].Value}
			}

			| ready?.. or continue
			/if (!${is_ready[${_debug}, "${_myHoT}"]}) {
				DEBUG ${sep}break.notready
				/varset _myHoT FALSE
				/invoke ${_liHoT.Advance}
				/continue
			}

			| apply any conditions
			/if (!${check_condition[${maDebug.Find[condition].Value}, ${maHeal.Find[stHeal${_liHoT.Value}Con].Value}]}) {
				DEBUG ${sep}break.condition
				/varset _myHoT FALSE
				/invoke ${_liHoT.Advance}
				/continue
			}

			/call check_basic_tags ${maDebug.Find[tag].Value} Heal${_liHoT.Value} ${Spawn[${hurtID}].DisplayName} "${maHeal.Find[stHeal${_liHoT.Value}Con].Value}"
			/if (!${Macro.Return}) {
				/varset _myHoT FALSE
				/invoke ${_liHoT.Advance}
				/continue
			}


			/invoke ${_liHoT.Advance}
		}

		| bail if user doesnt have a healgrouphot tag set.. no one reads the damn changeloggs...
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
	} else /if (${_tag.Equal[healhot]}) {

		| find my hot
		/while (!${_liHoT.IsEnd}) {
			| check the switch
			/if (!${Bool[${maHeal.Find[swHeal${_liHoT.Value}].Value}]}) {
				/invoke ${_liHoT.Advance}
				/continue
			}
			| check the item name
			/if (!${Bool[${maHeal.Find[stHeal${_liHoT.Value}].Value}]}) {
				/invoke ${_liHoT.Advance}
				/continue
			}
			| check matching tag
			/if (!${lsHeal${_liHoT.Value}Tag.Contains[healhot]}) {
				/invoke ${_liHoT.Advance}
				/continue
			}
			| cehck matching tag
			/if (${lsHeal${_liHoT.Value}Tag.Contains[healhot]}) {
				/varset _myHoT ${maHeal.Find[stHeal${_liHoT.Value}].Value}
				/break
			}
			/invoke ${_liHoT.Advance}
		}

		| could not find an available spell/item/whatever
		/if (!${Bool[${_myHoT}]}) {
			DEBUG ${sep}fail, no regen set
			/return FALSE
		}

		/if (${maChr.Find[swHealOverTime].Value}) {
			/if (${hurtPctHP} <= ${maHeal.Find[stHealPointHoT].Value}*.PCTHO) {
				/dquery ${Spawn[${hurtID}].DisplayName} -q "Spell[${_myHoT}].Stacks" -o DNQReturn
				/dquery ${Spawn[${hurtID}].DisplayName} -q "Me.Song[${_myHoT}].Duration" -o DNQReturn2
				/if (${DNQReturn} && !${DNQReturn2}) {
					/call voodoo ${_debug} "${_myHoT}" ${hurtID} FALSE
				}
			}
		}
		/return TRUE

	}

/return FALSE



|***
 * note: casts heals from maHeal
 * use: /call cast_cycle_heal DEBUG [TARGETID]
 ***|
sub cast_cycle_heal(bool _debug, int _tmpID, string _tag)
	DEBUG \atcast_cycle_heal\ax(\a-w${_tmpID}, ${_tag}\ax)
	FLOW cast_cycle_heal ${_tmpID}|${_tag} (1)

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
		GETINPUT cast_cycle_heal NA|NA
		ISMEDEAD
		CHECKEXIT cast_cycle_heal
		FLOW cast_cycle_heal ${_tmpID}|${_tag} (2)

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

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maHeal.Find[stHeal${_liHeal.Value}Con].Value}]}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Heal${_liHeal.Value} ${Spawn[${hurtID}].DisplayName} "${maHeal.Find[stHeal${_liHeal.Value}Con].Value}"
		/if (!${Macro.Return}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		/call voodoo ${_debug} "${maHeal.Find[stHeal${_liHeal.Value}].Value}" ${_tmpID} FALSE

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
	FLOW cast_cycle_heal_promise ${_tmpID} (1)

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
		FLOW cast_cycle_heal_promise ${_tmpID} (2)

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
		/if (!${lsHeal${_liHeal.Value}Tag.Contains[healpromise]}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		| check matching tag pass
		/if (${lsHeal${_liHeal.Value}Tag.Contains[healpromise]}) {
			/varset _myHeal ${maHeal.Find[stHeal${_liHeal.Value}].Value}
		}

		| ready?.. or continue
		/if (!${is_ready[${_debug}, "${_myHeal}"]}) {
			/varset _myHeal FALSE
			/invoke ${_liHeal.Advance}
			/continue
		}

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maHeal.Find[stHeal${_liHeal.Value}Con].Value}]}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Heal${_liHeal.Value} FALSE "${maHeal.Find[stHeal${_liHeal.Value}Con].Value}"
		/if (!${Macro.Return}) {
			/invoke ${_liHeal.Advance}
			/continue
		}

		/declare _tmpName string local ${get_assist_info[${_debug}, ID]}

		| set the spell
		/varset _spell ${Spell[${maHeal.Find[stHeal${_liHeal.Value}Con].Value}].RankName}
		DEBUG ${sep}\awSpell\ax${sep}\a-w${_spell}\ax ID:${Spell[${_spell}].ID}

		| does it stack
		/dquery ${_tmpName} -q "Spell[${_spell}].Stacks" -o DNQReturn -t 5
		DEBUG ${sep}${sep}STACKS: .Stacks \at${DNQReturn}\ax ${dot} ${_spell}
		/if (${DNQReturn.Equal[FALSE]}) /continue

		| is it buff blocked
		/dquery ${_tmpName} -q "lsBlockedBuffsMe.Find[${Spell[${_spell}].ID}]" -o DNQReturn -t 1s
		DEBUG ${sep}${sep}BLOCKED .Buff.Blocked \at${DNQReturn}\ax ${dot} ${_spell}
		/if (${Bool[${DNQReturn}]}) {
			/continue
		}

		| do we have the triggered buff?
		/dquery ${_tmpName} -q "Me.Buff[${_spell}].ID" -o DNQReturn -t 5
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
		/nav locyxz ${_tmpY} ${_tmpX} ${_tmpZ} |log=off dist=${_dist}
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
	FLOW check_buff_cycle_aura ${_tmpID}|${_tag} (1)

	/declare _myAura string local FALSE
	/declare _myAuraRK string local FALSE
	/declare _liBuff listiterator local
	/vardata _liBuff lsCount50.First.Clone
	/while (!${_liBuff.IsEnd}) {
		DEBUG ${sep}Buff${_liBuff.Value}${sep}\a-w${maBuff.Find[stBuff${_liBuff.Value}].Value}${sep}${maBuff.Find[stBuff${_liBuff}].Value}\ax
		GETINPUT check_buff_cycle_aura NA|NA
		ISMEDEAD
		CHECKEXIT check_buff_cycle_aura
		FLOW check_buff_cycle_aura ${_tmpID}|${_tag} (2)

		| check the switch
		/if (!${maBuff.Find[swBuff${_liBuff.Value}].Value}) {
			DEBUG ${sep}break.OFF
			/invoke ${_liBuff.Advance}
			/continue
		}

		| check the item name
		/if (!${Bool[${maBuff.Find[stBuff${_liBuff.Value}].Value}]}) {
			DEBUG ${sep}break.NO NAME
			/invoke ${_liBuff.Advance}
			/continue
		}

		| check matching tag for failure
		/if (!${lsBuff${_liBuff.Value}Tag.Contains[buffaura]}) {
			DEBUG ${sep}break.no buffaura tag
			/invoke ${_liBuff.Advance}
			/continue
		}

		| check matching tag pass
		/if (${lsBuff${_liBuff.Value}Tag.Contains[buffaura]}) {
			/varset _myAura ${maBuff.Find[stBuff${_liBuff.Value}].Value}
			/varset _myAuraRK ${Spell[${maBuff.Find[stBuff${_liBuff.Value}].Value}].RankName}
		}

		| have the auras?
		/if (${Me.Aura[1].Name.Equal[${Spell[${maBuff.Find[stBuff${_liBuff}].Value}].RankName}]} || ${Me.Aura[1].Name.Find[${maBuff.Find[stBuff${_liBuff}].Value}]}) {
			DEBUG ${sep}break.have aura1
			/varset _myAura FALSE
			/invoke ${_liBuff.Advance}
			/continue
		}

		| have the aura continued....
		/if (${Me.Aura[2].Name.Equal[${Spell[${maBuff.Find[stBuff${_liBuff}].Value}].RankName}]} || ${Me.Aura[2].Name.Find[${maBuff.Find[stBuff${_liBuff}].Value}]}) {
			DEBUG ${sep}break.have aura2
			/varset _myAura FALSE
			/invoke ${_liBuff.Advance}
			/continue
		}

		| apply any conditions
		/if (!${check_condition[${maDebug.Find[condition].Value}, ${maBuff.Find[stBuff${_liBuff.Value}Con].Value}]}) {
			DEBUG ${sep}break.condition
			/varset _myAura FALSE
			/invoke ${_liBuff.Advance}
			/continue
		}

		/call check_basic_tags ${maDebug.Find[tag].Value} Heal${_liBuff.Value} ${Spawn[${hurtID}].DisplayName} "${maBuff.Find[stBuff${_liBuff.Value}Con].Value}"
		/if (!${Macro.Return}) {
			/invoke ${_liBuff.Advance}
			/continue
		}

		/call voodoo ${_debug} "${maBuff.Find[stBuff${_liBuff.Value}].Value}" 0 FALSE

		/if (${maEntropy.Find[swBuffCooldownWait].Value}) {
			DEBUG ${sep}buff cooldown recycle
			/while (${Me.SpellInCooldown}) {
				CHECKEXIT
				ISMEDEAD
				/if (!${Me.SpellInCooldown}) /break
				/delay 5
			}
		}

		/invoke ${_liBuff.Advance}
	}

/return TRUE


${Me.Aura[2].Name.Find[${maBuff.Find[stBuff50].Value}]}







|***
 * note: drop a sql field
 * use: /call sql_drop TABLE FIELD
 ***|
sub sql_drop(string _table, string _field, bool _echo)

	| is the data there?
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_${_table} WHERE env_var="${_field}"

	/if (${sqlite.Rows[dummy]}) {
		/sqlite ${maData.Find[DBcharacter].Value} dummy DELETE FROM environment_${_table} WHERE env_var="${_field}"
		/if (${_echo}) OUT SQL DROP${sep}\a-w${_table}${submenu}${_field}\ax ${dot} \a-w${sqlite.Status[dummy]}\ax

	} else /if (!${sqlite.Rows[dummy]}) {
		/if (${_echo}) OUT Not Found${sep}\a-w\a-w${_table}${submenu}${_field}\ax\ax
	}

/return TRUE


