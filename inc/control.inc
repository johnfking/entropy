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

  | output control map
	/if (${_type.Equal[see]}) {
			/call Bind_command_see maDebug
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux debug
		/return

	} else /if (${_type.Equal[stop]}) {
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
		/invoke ${out[12, "/debug ${_type.Lower} ${_out}"]}

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
	/invoke ${out[12, "/debug ${_out}"]}

/return TRUE



|***
 * note: mode control
 * use: /mode []
 ***|
#bind control_mode /mode
sub Bind_control_mode(string _type, string _verbage, bool _debug)
	DEBUG \atBind_control_mode\ax(\a-w${_type}\ax)

  | output control map
	/if (${_type.Equal[see]}) {
			/call Bind_command_see maMode
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux mode
		/return

	} else /if (${_type.Equal[stop]}) {
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
			/call ${_verbage}
			/return TRUE
		}

	} else /if (${maMode.Contains[${_type}]}) {
		/invoke ${maMode.Add[${_type},${If[${maMode.Find[${_type}].Value},FALSE,TRUE]}]}
	}

  /declare _out string local
  /declare _mimode mapiterator local
  /vardata _mimode maMode.First.Clone
  /while (!${_mimode.IsEnd}) {
		/varset _out ${_out} ${If[${_mimode.Key.Equal[${maMode.First}]},,${dot}]} ${If[${_mimode.Value},\ag${_mimode.Key}\ax,${info}${_mimode.Key}\ax]}
		/invoke ${_mimode.Advance}
  }
  /varset _out ${_out} ${dot} \awdata\ax:(${num}${If[${Bool[${stModeData}]},${stModeData},${off}]}\ax)
  /varset _out ${_out} ${dot} \ar<\ax${info}stop\ax\ar>\ax
	/invoke ${out[12, "/mode ${_out}"]}

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
		/echo
		/invoke ${Macro.Undeclared}
		/echo
		/return

	} else /if (${_type.Equal[sqlcheck]}) {
		/echo
		/invoke ${out[0, "Checking database for stale data${sep}"]}
		/call sql_check ${maDebug.Find[sql].Value}
		/return TRUE

	| delete aliases
	} else /if (${_type.Equal[whitewash]}) {

		/declare _lidel listiterator local
		/vardata _lidel liAlias.First.Clone
		/while (!${_lidel.IsEnd}) {
			/invoke ${out[12, "Deleting Alias${sep}\a-w${_lidel.Value}\ax"]}
			/squelch /alias /${_lidel.Value} delete
			/invoke ${_lidel.Advance}
		}
		/invoke ${out[12, "Current Build ${info}${maEnv.Find[build].Value}\ax ${dot} ${bad}Ended\ax"]}
		/endm
		/return

	| parse SPA data
	} else /if (${_type.Equal[spa]}) {

		/if (!${Bool[${_verbage}]}) {
			/invoke ${out[12, "/admin spa \atEXACT SPELLNAME with ranks\ax"]}
			/return
		}

		/declare _spell string local "${_verbage}"

		/declare _list string local
		/declare _count int local 0
		/varset _spell ${Spell[${_spell}].RankName}
		/declare _spellID ${Spell[${_spell}].ID}

		/mqclear
		OUT \aw${_spell}\ax ${sep} ID\a-w ${_spellID}\ax ${sep} Group \a-w${Spell[${_spellID}].SpellGroup}\ax
		OUT \a-w${Spell[${Int[${Math.Calc[${_spellID} + ${If[${Spell[${_spellID}].SpellGroup},3,1]}]}]}].Name}\ax ${sep} ID \a-w${Spell[${Int[${Math.Calc[${_spellID} + ${If[${Spell[${_spellID}].SpellGroup},3,1]}]}]}].ID}\ax
		OUT \awSPA Count\ax ${sep} \a-w${Spell[${_spell}].NumEffects}\ax

		/for _count 1 to ${Spell[${_spell}].NumEffects}
			OUT #\ay${_count}\ax
			/if (${Select[${Spell[${_spell}].Attrib[${_count}]},10]}) /continue
			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM eq_spa WHERE env_var='${Spell[${_spell}].Attrib[${_count}]}'
			OUT ${dot} .Attrib \a-w${Spell[${_spell}].Attrib[${_count}]}\ax ${dot} \a-g${sqlite.Result[dummy 1 value]}\ax
			OUT ${dot} .Trigger \a-w${Spell[${_spell}].Trigger[${_count}].Name}\ax ${dot} ID:\a-w${Spell[${_spell}].Base2[${_count}]}\ax\ax ()
			OUT ${dot} .Base \a-w${Spell[${_spell}].Base[${_count}]}\ax
			OUT ${dot} .Max \a-w${Spell[${_spell}].Max[${_count}]} \ax
			OUT ${dot} .Calc \a-w${Spell[${_spell}].Calc[${_count}]} \ax
			OUT ${dot} .Restrictions \a-w${Spell[${_spell}].Restrictions[${_count}]}\ax
			OUT ${dot} .Extra \a-w${Spell[${_spell}].Extra}\ax

			/varset _list ${_list} ${Spell[${_spell}].Attrib[${_count}]}
		/next _count
		OUT ${dot} ${dot} ${dot} ${dot}
		OUT \awSPA list\ax ${sep} \a-w${_list}\ax

		| OUT \aw/spellslotinfo\ax
		| /spellslotinfo ${Spell[${_spell}].ID}

		/return TRUE

	} else /if (${_type.Equal[import]}) {

		/if (${_verbage.Equal[loot]}) {
			/mqclear
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
						/sqlite ${maData.Find[DBloot].Value} dummy INSERT INTO loot_control VALUES ("${_key}", "${_value}", 0, 0)
						/ini "${_lootini}" "${_section}" "${_key}" NULL

					}
					/next _countS

			}

			/invoke ${out[18, "${eol}"]}
			/return

		} else /if (${_type.Equal[see]}) {

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
					/endm
				}
			}

		} else /if (${_type.Equal[keymap]}) {
			/call get_key_map TRUE ${_verbage} TRUE

		}

		/if (${_verbage.Equal[zone]}) {
			/mqclear
			| current core loot file
			/declare _ini string local exspes\\zonedata.ini
			/declare _section string local
			/declare _key string local
			/declare _value string local
			/declare _pass string local
			/declare _countS int local 0
			/declare _countK int local 0
			/declare _countV int local 0

			/echo \awImporting Named File from\ax\at...\ax\a-w${_ini}\ax
			/echo \a-yINI sections\ax::\a-w${Ini[${_ini}].Count[|]}\ax

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
						/sqlite ${maData.Find[DBzone].Value} dummy INSERT OR IGNORE INTO zone_control VALUES ("${_section}", "${_key}", "${_value}")
					/next _countV
				/next _countK
			/next _countS
			/invoke ${out[18, "${eol}"]}
			/echo
			/return
		}

	| }

	/if (${_silent.Equal[SILENT]}) /return
	/declare liout list local
	/invoke ${liout.Append[checkvar,spa,whitewash,import,keymap,see,sqlcheck]}
  /declare _out string local
  /declare _liadout listiterator local
	/vardata _liadout liout.First.Clone
	/while (!${_liadout.IsEnd}) {
		/varset _out ${_out} ${If[${_liadout.Value.Equal[${liout.First}]},,${dot}]} ${info}${_liadout.Value}\ax
		/invoke ${_liadout.Advance}
	}
	/invoke ${out[12, "/admin (PID:\a-w${EverQuest.PID}\ax) ${_out}"]}

/return TRUE



|***
 * note: environment control
 * use: /env
 ***|
#bind control_env /env
sub Bind_control_env(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_env\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maEnv ${_verbage}
		/return

	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux env ${_verbage}
		/return

	| macro auto/manual
	} else /if (${_type.Equal[auto]}) {
		/invoke ${set_switch_env[FALSE, swAuto, FALSE, ${_verbage}]}
		/invoke ${out[12, "/env ${If[${maEnv.Find[swAuto].Value},${good}auto\ax,${bad}auto\ax]}"]}

		/if (!${maEnv.Find[swAuto].Value}) {
			/if (HOME) {
				/call Bind_control_home clear
			}
			/invoke ${target[clear]}
			| /call clear_combat FALSE
		} else /if (${_type.Equal[auto]}) {
			/if ((${maCC.Find[swPullSetHome].Value} && PULL) || ${maHome.Find[swonAuto].Value}) {
				/if (!${swModeToken}) {
					/call Bind_control_home set
				}
			}
		}
		/return TRUE

	| enable wasiting fireworks for those bastards with too much AA on their hands
	} else /if (${_type.Equal[fireworks]}) {
		/invoke ${set_switch_env[FALSE, swAAFireworks, TRUE, ${_verbage}]}
		| /if (!${swAAFireworks}) /call set_timer timer_Waste_Fireworks 1

	| enable bagging from mages for rods and pet gear
	} else /if (${_type.Equal[begmage]}) {
		/invoke ${set_switch_env[FALSE, swBegMAG, TRUE, ${_verbage}]}

	| take raid invites
	} else /if (${_type.Equal[takeri]}) {
		/invoke ${set_switch_env[FALSE, swTakeRaidInvite, TRUE, ${_verbage}]}

	| take group invites
	} else /if (${_type.Equal[takegi]}) {
		/invoke ${set_switch_env[FALSE, swTakeGroupInvite, TRUE, ${_verbage}]}

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

	| rese settings
	} else /if (${_type.Equal[rest]}) {

		| set rest %
		/if (${_verbage.Equal[pct]}) {
			/if (!${set_control_num_range[${_debug}, stPctRest, "${_silent}", 0, 99]}) /return

		| rest to full
		} else /if (${_verbage.Equal[full]}) {
			/invoke ${set_switch_env[FALSE, swRestFull, TRUE, ${_silent}]}

		| rest through combat
		} else /if (${_verbage.Equal[combat]}) {
			/invoke ${set_switch_env[FALSE, swRestCombat, TRUE, ${_silent}]}

		| mod rods in rest
		} else /if (${_verbage.Equal[modrod]}) {
			/invoke ${set_switch_env[FALSE, swRestModRod, TRUE, ${_silent}]}

		| check buffs in rest
		} else /if (${_verbage.Equal[buff]}) {
			/invoke ${set_switch_env[FALSE, swRestBuff, TRUE, ${_silent}]}

		}

		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[pct,stPctRest]}
		/invoke ${maControl.Add[full,swRestFull]}
		/invoke ${maControl.Add[combat,swRestCombat]}
		/invoke ${maControl.Add[modrod,swRestModRod]}
		/invoke ${maControl.Add[buff,swRestBuff]}
		/invoke ${set_control_output[${_debug}, maEnv, "env rest", maControl]}
		/return

	} else /if (${_type.Equal[moblvlmin]}) {
		/invoke ${set_switch_env[FALSE, stMobLvlMin, TRUE, ${_verbage}]}

	| set mob maximum level for attacking
	} else /if (${_type.Equal[moblvmax]}) {
		/invoke ${set_switch_env[FALSE, stMobLvlMax, TRUE, ${_verbage}]}

	| force a nuke loop till all are casted before exiting
	} else /if (${_type.Equal[nukeloop]}) {
		/invoke ${set_switch_env[FALSE, swNukeLoop, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[advloot]}) {
		/invoke ${set_switch_env[FALSE, swADVLoot, TRUE, ${_verbage}]}

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
			/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE environment_character SET stBuffGem='0' WHERE env_var='stBuffGem'
			/invoke ${get_shade[FALSE, MQWindow]}
			/return
		}
		/if (${Range.Between[1,${Me.NumGems}:${Int[${_verbage}]}]}) {
			/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE environment_character SET stBuffGem='${_verbage}' WHERE env_var='stBuffGem'
			/invoke ${maEnv.Add[stBuffGem,${_verbage}]}
		} else {
			/invoke ${out[12, "/env buffgem(${num}${maEnv.Find[stBuffGem].Value}\ax) range ${info}1 - ${Me.NumGems}\ax"]}
			/return
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
		/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
			/varset _name ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}

			| if i'm issuing the command. turn off tie and move along
			/if (${_name.Equal[${Me.DisplayName}]}) {
				/invoke ${do_raw_edit[FALSE, SILENT, stTieToon, FALSE]}

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
			/invoke ${get_shade[FALSE, MQWindow]}
			/return FALSE
		}

	| shortcut for the build set routine
	} else /if (${_type.Equal[SET]}) {
		/if (${Select[${_verbage},FALSE,off]} || !${Ini[${maData.Find[INIset].Value}].Find[${_verbage}]}) {
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
	/invoke ${maControl.Add[rest,submenu]}
	/invoke ${maControl.Add[meal,submenu]}
	/invoke ${maControl.Add[exp,submenu]}
	/invoke ${maControl.Add[advloot,swADVLoot]}
	/invoke ${maControl.Add[autoinv,swAutoInv]}
	/invoke ${maControl.Add[buffgem,stBuffGem]}
	/invoke ${maControl.Add[buffgemhold,swHoldBuffGem]}
	/invoke ${maControl.Add[nukeloop,swNukeLoop]}
	/invoke ${maControl.Add[safelistguild,swSafeListGuild]}
	/invoke ${maControl.Add[moblvmax,stMobLvlMax]}
	/invoke ${maControl.Add[moblvlmin,stMobLvlMin]}
	/invoke ${maControl.Add[begmage,swBegMAG]}
	/invoke ${maControl.Add[fireworks,swAAFireworks]}
	/invoke ${maControl.Add[takeri,swTakeRaidInvite]}
	/invoke ${maControl.Add[takegi,swTakeGroupInvite]}
	/invoke ${maControl.Add[saferadius,stEnvSafeRadius]}
	/invoke ${maControl.Add[tribute,swCheckTribute]}
	| /invoke ${maControl.Add[set,stCurrentSet|br]}

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
	| DEBUG /call set_control_${Me.Class.ShortName} _type:${_type} _verbage:${_verbage} _silent:${_silent} _debug:${_debug}

	/call set_control_${Me.Class.ShortName} ${_type} "${_verbage}" ${_silent} ${_debug}

/return TRUE



|***
 * note: force buff check routine
 * use: /cb
 ***|
#bind command_cb /cb
sub Bind_command_cb(string _verbage)

	/call check_buff_cycle ${maDebug.Find[cb].Value} TRUE ${_verbage}

/return TRUE



|***
 * note: force minion check routine
 * use: /cm
 ***|
#bind command_cm /cm
sub Bind_command_cm(string _verbage)

	/if (!${Bool[${_verbage}]}) {
		/call check_Minion_cycle ${maDebug.Find[minion].Value} TRUE
	}

/return TRUE



|***
 * note: watches for a spawn in the zone
 * use: /watch [add|delete|reset] [NAME]
 ***|
#bind control_watch /watch
sub Bind_control_watch(string _verbage, string _verbage2, bool _debug)
	DEBUG \atBind_control_watch\ax(\a-w${_verbage}, ${_verbage2}\ax)

	/if (${_verbage.Equal[aux]}) {
		/call Bind_command_aux watch
		/mapshow reset
		/return TRUE

	} else /if (${_verbage.Equal[see]}) {
		/call Bind_command_see maWatch
		| /call Bind_command_see lsWatch
		/return TRUE

	} else /if (${_verbage.Equal[use]}) {
		/invoke ${set_switch_env[${_debug}, swWatch, FALSE, ${_verbage2}]}

	| } else /if (${_verbage.Equal[beep]}) {
	| 	/invoke ${set_switch_env[${_debug}, swWatchBeep, TRUE, ${_verbage2}]}

	} else /if (${_verbage.Equal[add]}) {
		/if (${Target.ID}) {
			/call do_raw_edit ${_debug} FALSE lsWatch "${Target.DisplayName}" add
		} else /if (${Bool[${_verbage2}]}) {
			/call do_raw_edit ${_debug} FALSE lsWatch "${_verbage2}" add
		}

	} else /if (${_verbage.Equal[delete]}) {
		/if (${Target.ID}) {
			/call do_raw_edit ${_debug} FALSE lsWatch "${Target.DisplayName}" delete
		} else /if (${Bool[${_verbage2}]}) {
			/call do_raw_edit ${_debug} FALSE lsWatch "${_verbage2}" delete
		}

	} else /if (${_verbage.Equal[reset]}) {
		/invoke ${set_switch_env[${_debug}, swWatch, FALSE, FALSE]}
		| /invoke ${set_switch_env[${_debug}, swWatchBeep, TRUE, FALSE]}
		/call do_raw_edit ${_debug} TRUE lsWatch clear

	}

	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[use,swWatch|pr]}
	| /invoke ${maControl.Add[beep,swWatchBeep]}
	/invoke ${set_control_output[${_debug}, maWatch, watch, maControl]}

/return TRUE



|***
 * note:control for /home
 * use:
 ***|
#Bind control_home /home
sub Bind_control_home(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_home\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	/declare _lsout list local
  /declare _out string local
  /declare _count int local 0

	| no mesh for zone
	/if (!${Navigation.MeshLoaded}) {
		/invoke ${get_shade[FALSE, MQWindow]}
		/invoke ${maHome.Add[swHomeMarker,FALSE]}
		/return
	}

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maHome ${_verbage}
		/return
	}	else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux home ${_verbage}
		/return
	} else /if (${_type.Equal[campfire]}) {
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
		/return

	| gather your crew
	} else /if (${_type.Equal[gather]}) {

		/if (${_verbage.Equal[all]}) {
			/declare _clist string local all
		} else {
			/declare _clist string local ${set_dnet_group[FALSE]}
		}

		/invoke ${out[12, "Gathering${sep}\a-w${_clist}\ax"]}
		/dgexecute  ${_clist} /navto ${Me.ID}
		/return

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
			/if (${maHome.Find[swHomeMarker].Value}) ${out[19, "Home${sep}Clear"]}
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
		/if (${maHome.Find[swHomeMarker].Value}) ${out[19, "Home${sep}Clear"]}
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

		| check variance for home position
		/if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} < ${maHome.Find[stHomeVariance].Value}) {
			/return TRUE
		}

		SETHUD "'goin home"

		/doevents flush
		/delay 2

		/if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} < SAFEENV && PULL) {
			DEBUG move backwords
			/nav locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value} |dist=${maHome.Find[stHomeVariance].Value} log=off facing=backward
		} else {
			/nav locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value} |dist=${maHome.Find[stHomeVariance].Value} log=off
		}

		/delay 2 ${Navigation.Active}
		/while (${Navigation.Active}) {
			/delay 2
		}
		SETHUD EMPTY
		| /if (${swActivePull}) /varset swActivePull FALSE
		/return

	} else /if (${_type.Equal[arc]}) {
		/if (!${set_control_num_range[FALSE, stFaceArc, "${_verbage}", 0, 45]}) /return

	} else /if (${_type.Equal[variance]}) {
		/if (!${set_control_num_range[FALSE, stHomeVariance, "${_verbage}", 0, 15]}) /return

	} else /if (${_type.Equal[movebehind]}) {
		/invoke ${set_switch_env[FALSE, swMoveBehind, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[facefast]}) {
		/invoke ${set_switch_env[FALSE, swFaceFast, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[mount]}) {
		/invoke ${do_raw_edit[FALSE, SILENT, stMount, "${_verbage}"]}

	} else /if (${_type.Equal[deathclear]}) {
		/invoke ${set_switch_env[FALSE, swMoveDeathClear, TRUE, ${_verbage}]}

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
	/invoke ${maControl.Add[movebehind,swMoveBehind]}
	/invoke ${maControl.Add[facefast,swFaceFast]}
	/invoke ${maControl.Add[arc,stFaceArc]}
	/invoke ${maControl.Add[mount,stMount]}
	/invoke ${maControl.Add[deathclear,swMoveDeathClear]}
	/invoke ${maControl.Add[onauto,swonAuto]}
	/invoke ${maControl.Add[incombat,swinCombat]}
	/invoke ${set_control_output[${_debug}, maHome, home, maControl]}

/return TRUE



|***
 * note: control for /tc
 * use:
 ***|
#bind control_tc /tc
sub Bind_control_tc(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_tc\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)
	/declare _lsout list local
  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maTie ${_verbage}
		/return

	}	else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux tc ${_verbage}
		/return

	} else /if (${_type.Equal[mode]}) {
		/invoke ${_lsout.Append[nav,stick]}
		/if (${_lsout.Contains[${_verbage}]}) {
			/if (!${Navigation.MeshLoaded}) {
				/invoke ${do_raw_edit[FALSE, SILENT, stTieMode, stick]}
			} else /if (${Navigation.MeshLoaded}) {
				/invoke ${do_raw_edit[FALSE, SILENT, stTieMode, "${_verbage}"]}
			}
		}

		/declare _litc listiterator local
		/vardata _litc _lsout.First.Clone
		/while (!${_litc.IsEnd}) {
			/varset _out ${_out} ${If[${_litc.Value.Equal[${_lsout.First}]},,${dot}]} ${If[${_litc.Value.Equal[${maTie.Find[stTieMode].Value}]},${good}${_litc.Value}\ax,${info}${_litc.Value}\ax]}
			/invoke ${_litc.Advance}
		}
		/invoke ${out[12, "/tc mode ${_out}"]}
		/return

	} else /if (${_type.Equal[toon]}) {
		/invoke ${do_raw_edit[FALSE, SILENT, stTieToon, "${_verbage}"]}

	} else /if (${_type.Equal[variance]}) {
		/if (!${set_control_num_range[TRUE, stTieVariance, "${_verbage}", 0, 15]}) /return

	}

	/if (${_silent.Equal[SILENT]}) /return

	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[toon,stTieToon|br]}
	/invoke ${maControl.Add[variance,stTieVariance]}
	/invoke ${maControl.Add[mode,submenu]}
	/invoke ${set_control_output[${_debug}, maTie, tc, maControl]}

/return TRUE



|***
 * note: control for /tie
 * use: /tie [switch|on|off]
 ***|
#bind command_tie /tie
sub Bind_command_tie(string _type, bool _debug)
	DEBUG \atBind_command_tie\ax(\a-w${_type}\ax)

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux tie
		/return
	}

	| tie toon not in zone?
	/if (!${Bool[${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}]}) {
		DEBUG ${sep}tietoon is \arNOT\ax in zone
		/invoke ${maTie.Add[swSetTie, FALSE]}
		/return FALSE
	}

	/if (${_type.Equal[on]}) {
		DEBUG ${sep}setting tie${sep}${on}
		/if (${maTie.Find[swSetTie].Value}) /return
		/if (${Bool[${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}]}) /invoke ${maTie.Add[swSetTie, TRUE]}
	} else /if (${_type.Equal[off]}) {
		DEBUG ${sep}setting tie${sep}${off}
		/if (!${maTie.Find[swSetTie].Value}) /return
		/if (${Bool[${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}]}) /invoke ${maTie.Add[swSetTie, FALSE]}
	} else {
		DEBUG ${sep}setting tie${sep}[\a-yswap\ax]
		/if (${Bool[${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}]}) /invoke ${maTie.Add[swSetTie,${If[${maTie.Find[swSetTie].Value},FALSE,TRUE]}]}
	}

	/invoke ${out[12, "/tie ${maTie.Find[stTieToon].Value}${sep}${If[${maTie.Find[swSetTie].Value},${on},${off}]}"]}

/return TRUE



|***
 * note: control for /area
 * use:
 ***|
#bind control_area /area
sub Bind_control_area(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_area\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| get the zone data
	/sqlite ${maData.Find[DBzone].Value} curZoneData SELECT * FROM zone WHERE ShortName="${Zone.ShortName.Lower}"

	/declare _element string local FALSE
	/if (${_type.Equal[exclude]}) {
		/varset _element lsZoneExclude
	} else /if (${_type.Equal[named]}) {
		/varset _element lsZoneNamed
	} else /if (${_type.Equal[ranged]}) {
		/varset _element lsZoneRanged
	} else /if (${_type.Equal[hunt]}) {
		/varset _element lsZoneHunt
	} else /if (${_type.Equal[nomez]}) {
		/varset _element lsZoneNoMez
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
	} else /if (${_type.Equal[nopoision]}) {
		/varset _element lsZoneNoPoison
	} else /if (${_type.Equal[nocold]}) {
		/varset _element lsZoneNoCold
	} else /if (${_type.Equal[nocorruption]}) {
		/varset _element lsZoneNoCorruption
	}


	| check whats in the lists
	/if (!${Target.ID} && ${_verbage.Equal[list]}) {
		/call Bind_command_see ${_element}
		/return TRUE
	} else /if (${Target.ID} && ${_verbage.Equal[list]}) {
		/if (${${_element}.CountOf[${Target.DisplayName}]}) {
			/invoke ${out[0, "${_type} list contains${sep}\a-w${Target.DisplayName}\ax"]}
			/return TRUE
		}	else {
			/invoke ${out[0, "${_type} list does not contain${sep}\a-w${Target.DisplayName}\ax"]}
			/return TRUE
		}
	}


	| safezone toggle
	/if (${_type.Equal[Safe]}) {
		/if (${maEnv.Find[swZoneSafe].Value} || ${_verbage.Equal[FALSE]}) {
			/sqlite ${maData.Find[DBzone].Value} dummy UPDATE zone_control SET value='FALSE' WHERE zone_shortname='${Zone.ShortName}' and element='swZoneSafe'
			/invoke ${maEnv.Add[swZoneSafe,FALSE]}
		} else /if (!${maEnv.Find[swZoneSafe].Value} || ${_verbage.Equal[TRUE]}) {
			/sqlite ${maData.Find[DBzone].Value} dummy UPDATE zone_control SET value='TRUE' WHERE zone_shortname='${Zone.ShortName}' and element='swZoneSafe'
			/invoke ${maEnv.Add[swZoneSafe,TRUE]}
		}

	| remove a mob from the current zone
	} else /if (${_type.Equal[delete]}) {
		/sqlite ${maData.Find[DBzone].Value} dummy DELETE FROM zone_control WHERE zone_shortname='${Zone.ShortName}' and value='${Target.DisplayName}'
		/invoke ${out[12, "/area delete ${dot} ${num}${Zone.ShortName}\ax${sep}\a-w${Target.DisplayName}"]}
		/call set_zone_lists FALSE
		/return

	| remaining list adjustments
	} else /if (${Select[${_type},exclude,named,ranged,hunt,nomez,nocast,nomelee,nomagic,nodisease,nofire,nopoision,nocold,nocorruption]} && ${Target.ID}) {

		| Add a property to a mob
		/sqlite ${maData.Find[DBzone].Value} dummy INSERT INTO zone_control VALUES ("${Zone.ShortName}", "${_element}", "${Target.DisplayName}")
		/invoke ${out[12, "/area ${_type} ${dot} ${num}${_element}\ax${sep}\a-w${Target.DisplayName}\ax"]}
		/call set_zone_lists FALSE
		/return TRUE

	}

	/if (${_silent.Equal[SILENT]}) /return
  /declare _out string local
  /declare _lizone listiterator local
	/vardata _lizone lsZoneProperty.First.Clone
	/varset _out ${info}safe\ax:${If[${maEnv.Find[swZoneSafe].Value},${on},${off}]}
	/while (!${_lizone.IsEnd}) {
		/varset _out ${_out} ${If[${_lizone.Value.Equal[${lsZoneProperty.First}]},,${dot}]} ${info}${_lizone.Value.Right[-6].Lower}\ax
		/invoke ${_lizone.Advance}
	}
	/invoke ${out[12, "/area ${_out}"]}

/return TRUE



|***
 * note:/beg command
 * use: starts the buff begging process
 ***|
#Bind command_beg /beg
sub Bind_command_beg(string _buff, string _operate)

	/if (${_buff.Equal[see]}) {
		/call Bind_command_see maBuffRequest
		/return
	} else /if (${_buff.Equal[aux]}) {
			/call Bind_command_aux beg
		/return
	}

	/if (!${Bool[${_operate}]}) /varset _operate Add

	| /invoke ${maBuffRequest.${_operate}[${Me.Name},"${_buff}"]}
	/dgzexecute /requestbuff ${Me.Name} Add "${_buff}"

/return TRUE



|***
 * note:sends the buff beg to all toons
 * use:
 ***|
#Bind control_requestbuff /requestbuff
sub Bind_control_requestbuff(string _toon, string _operate, string _buff)

	/if (!${Bool[${_toon}]}) /varset _toon ${Me.Name}
	/if (!${Bool[${_operate}]}) /varset _operate Add
	/invoke ${maBuffRequest.${_operate}[${_toon},"${_buff}"]}

/return TRUE



|***
 * note:control for /heal
 * use:
 ***|
#Bind control_heal /heal
sub Bind_control_heal(string _type, string _verbage, string _silent, string _verbage2, bool _debug)
	DEBUG \atcontrol_heal\ax(\a-w${_type}, ${_verbage}, ${_silent}, ${_verbage2}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maHeal ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux heal ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see heal list
		/return	TRUE

	| heal number entered
	} else /if (${Range.Between[1,50:${Int[${_type}]}]}) {

		| see the heal #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see heal ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swHeal${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awHeal${_type}\ax${sep}${If[${maHeal.Find[swHeal${_type}].Value},${on},${off}]}${sep}\a-w${maHeal.Find[stHeal${_type}].Value}\ax"]}
			/return TRUE
		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swHeal${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stHeal${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stHeal${_type}Alias" clear
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stHeal${_type}" FALSE
		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stHeal${_type}Con" "${_silent}"
		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsHeal${_type}Tag" ${_silent} ${_verbage2}
		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stHeal${_type}" "${_verbage}"
		}

		/call Bind_command_see heal ${_type}

		/return TRUE

	| set heal point
	} else /if (${_type.Equal[point]}) {
		/call set_control_heal_point FALSE ${_verbage}
		/return

	| heal self
	} else /if (${_type.Equal[self]}) {
		/invoke ${set_switch_env[FALSE, swHealSelf, TRUE, ${_verbage}]}

	| heal targets target
	} else /if (${_type.Equal[tot]}) {
		/if (${lsClassHealToT.Contains[${Me.Class.ShortName}]}) {
			/invoke ${set_switch_env[FALSE, swHealToT, TRUE, ${_verbage}]}
		} else {
			/invoke ${get_shade[FALSE, MQWindow]}
		}

	| heal group
	} else /if (${_type.Equal[group]}) {
		/invoke ${set_switch_env[FALSE, swHealGroup, TRUE, ${_verbage}]}

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

		/invoke ${set_switch_env[FALSE, swHealXTarget, TRUE, ${_verbage}]}

		/if (!${maHeal.Find[swHealXTarget].Value}) {
			/call set_xtarget_clear
		}

	| heal pets
	} else /if (${_type.Equal[pet]}) {
		/invoke ${set_switch_env[${_debug}, swHealPet, TRUE, ${_verbage}]}

	| heal cures
	} else /if (${_type.Equal[cure]}) {
		/invoke ${set_switch_env[${_debug}, swHealCure, TRUE, ${_verbage}]}
		/if (${maHeal.Find[swHealCure].Value}) {
			/call set_observers FALSE maCure add FALSE
		} else /if (!${maHeal.Find[swHealCure].Value}) {
			/call set_observers FALSE maCure drop FALSE
		}

	} else /if (${_type.Equal[adj]}) {
		/if (!${set_control_num_range[${_debug}, stHealAdjust, "${_verbage}", 0, 99]}) /return


	| set the XTarget class to heal
	} else /if (${_type.Equal[xtclass]}) {
		/call set_control_chr_xtclass FALSE ${_verbage}
		/return

	| set rampage tank toon
	} else /if (${_type.Equal[rampage]}) {
		/if (${Bool[${_verbage}]} && ${_verbage.NotEqual[TRUE]} && ${_verbage.NotEqual[ON]} && ${_verbage.NotEqual[OFF]}) {
			/call do_raw_edit ${_debug} SILENT stRampageTank "${_verbage.Left[1].Upper}${_verbage.Right[-1].Lower}"
		} else /if (!${Bool[${_verbage}]} || ${_verbage.Equal[OFF]}) {
			/invoke ${set_switch_env[${_debug}, stRampageTank, TRUE, FALSE]}
		}

	| use group Heal over Time
	} else /if (${_type.Equal[grouphot]}) {
		/invoke ${set_switch_env[${_debug}, swHealGroupOverTime, TRUE, ${_verbage}]}

	| use splashes
	} else /if (${_type.Equal[splash]} && ${Select[${Me.Class.ShortName},CLR,PAL]}) {
		/if (!${set_control_num_range[${_debug}, stCountSplash, "${_verbage}", 0, 6]}) /return

	| set group Heals
	} else /if (${_type.Equal[groupheal]}) {
		/if (!${set_control_num_range[${_debug}, stCountHealGroup, "${_verbage}", 0, 6]}) /return

	| set single hot
	} else /if (${_type.Equal[hot]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swHealOverTime, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[name]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stHealOverTime, "${_silent}"]}
		} else /if (${_verbage.Equal[pct]}) {
			/if (!${set_control_num_range[${_debug}, stHealPointHoT, "${_silent}", 0, 99]}) /return
		}
		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[use,swHealOverTime]}
		/invoke ${maControl.Add[name,stHealOverTime|br]}
		/invoke ${maControl.Add[pct,stHealPointHoT]}
		/invoke ${set_control_output[${_debug}, maHeal, "heal hot", maControl]}
		/return TRUE

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[point,submenu]}
	| /invoke ${maControl.Add[mode,submenu]}
	/invoke ${maControl.Add[self,swHealSelf]}
	/if (${lsClassHealToT.Contains[${Me.Class.ShortName}]}) {
		/invoke ${maControl.Add[tot,swHealToT]}
	}
	/if (${Select[${Me.Class.ShortName},CLR,SHM,DRU]}) {
		/invoke ${maControl.Add[hot,submenu]}
	}

  /invoke ${maControl.Add[groupheal,submenu]}
  /invoke ${maControl.Add[grouphot,swHealGroupOverTime]}
	/invoke ${maControl.Add[groupheal,stCountHealGroup]}
	/invoke ${maControl.Add[xt,swHealXTarget]}
	/invoke ${maControl.Add[xtclass,submenu]}
	/invoke ${maControl.Add[pet,swHealPet]}
	/invoke ${maControl.Add[cure,swHealCure]}
	/invoke ${maControl.Add[adj,stHealAdjust]}

	/if (${Select[${Me.Class.ShortName},CLR,PAL]}) {
		/invoke ${maControl.Add[splash,stCountSplash]}
	}

	/invoke ${maControl.Add[rampage,stRampageTank|br]}
	/invoke ${set_control_output[${_debug}, maHeal, heal, maControl]}

/return	TRUE



|***
 * note: control for /entropy
 * use:
 ***|
#Bind control_entropy /entropy
sub Bind_control_entropy(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atcontrol_entropy\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	/declare _lsout list local
  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maEntropy ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux entropy ${_verbage}
		/return

	} else /if (${_type.Equal[maintenance]}) {
		/invoke ${set_switch_env[FALSE, swMaintenance, TRUE, ${_verbage}]}
		/invoke ${out[12, "/entropy ${If[${maEntropy.Find[swMaintenance].Value},${good}maintenance\ax,${bad}maintenance\ax]}"]}
		/invoke ${out[12, "${warning} ${dot} ENDING MACRO${dot}${dot}${dot}please restart"]}
		/endmacro

	| enable the DPS meter
	} else /if (${_type.Equal[dpsmeter]}) {
		/invoke ${set_switch_env[FALSE, swDPSMeter, TRUE, ${_verbage}]}

	| enable/disable using events.inc file
	} else /if (${_type.Equal[events]}) {
		/invoke ${set_switch_env[FALSE, swEventsInc, TRUE, ${_verbage}]}

	| autoinvnetory while in manual
	} else /if (${_type.Equal[manautoinv]}) {
		/invoke ${set_switch_env[FALSE, swAutoInvManual, TRUE, ${_verbage}]}

	| enable buff cooldown waits
	} else /if (${_type.Equal[buffcooldownwait]}) {
		/invoke ${set_switch_env[FALSE, swBuffCooldownWait, TRUE, ${_verbage}]}

	| enable sub hook checking
	} else /if (${_type.Equal[hook]}) {
		/invoke ${set_switch_env[FALSE, swHookSub, TRUE, ${_verbage}]}

		/if (!${maHook.Find[swHookSub].Value}) {
			/invoke ${maHook.Clear}
		}

	} else /if (${_type.Equal[nettimeout]}) {
		/if (!${set_control_num_range[${_debug}, stNetworkTimeout, "${_verbage}", 0, 30]}) /return

	} else /if (${_type.Equal[buffrefresh]}) {
		/if (!${set_control_num_range[${_debug}, stBuffRefresh, "${_verbage}", 0, 36]}) /return

	| verbosity level
	} else /if (${_type.Equal[verb]}) {
		/if (!${set_control_num_range[${_debug}, stVerb, "${_verbage}", 1, 10]}) /return

	| remember the set build at mac start
	} else /if (${_type.Equal[rememberset]}) {
		/invoke ${set_switch_env[FALSE, swBuildSetRemember, TRUE, ${_verbage}]}

	| enable auto adding mobs to lists
	} else /if (${_type.Equal[autolist]}) {
		/invoke ${set_switch_env[FALSE, swAutoList, TRUE, ${_verbage}]}

	| count of cast loop attempts
	} else /if (${_type.Equal[castloops]}) {
		/invoke ${set_switch_env[FALSE, stCastLoop, TRUE, ${_verbage}]}

	| code engine
	} else /if (${_type.Equal[engine]}) {
		/if (!${set_control_num_range[${_debug}, stEngine, "${_verbage}", 1, 2]}) /return

	| hud adjustments
	} else /if (${_type.Equal[hud]}) {

		| adjust hud clock
		/if (${_verbage.Equal[time]}) {
			/if (${maData.Find[swHUDTime].Value}) {
				/invoke ${maData.Add[swHUDTime,FALSE]}
				/invoke ${out[12, "HUD Time${sep}\a-wGame\ax"]}

			} else /if (!${maData.Find[swHUDTime].Value}) {
				/invoke ${maData.ADd[swHUDTime,TRUE]}
				/invoke ${out[12, "HUD Time${sep}\a-wIRL\ax"]}
			}

		| reset the MQhud ini file
		} else /if (${_verbage.Equal[update]}) {
			/call set_initialize_hud FALSE ${_silent}
			/squelch /loadhud ${_silent}
			/return TRUE

		| delete the hud
		} else /if (${_verbage.Equal[delete]}) {
			/ini ".\..\MQ2HUD.ini" "${_silent}" NULL NULL
			/invoke ${out[12, "Deleting MQ2HUD.INI${sep}\a-w${_silent}\ax"]}
			/squelch /unloadhud ${_silent}
			/return TRUE

		}

		/invoke ${_lsout.Append[time,update,delete]}
		/declare _lihud listiterator local
		/vardata _lihud _lsout.First.Clone
		/while (!${_lihud.IsEnd}) {
			/varset _out ${_out} ${If[${_lihud.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${_lihud.Value}\ax
			/invoke ${_lihud.Advance}
		}
		/invoke ${out[12, "/entropy hud ${_out}"]}
		/return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[autolist,swAutoList]}
	/invoke ${maControl.Add[autolist,swAutoList]}
	/invoke ${maControl.Add[engine,stEngine]}
	/invoke ${maControl.Add[castloops,stCastLoop]}
	/invoke ${maControl.Add[dpsmeter,swDPSMeter]}
	/invoke ${maControl.Add[events,swEventsInc]}
	/invoke ${maControl.Add[hook,swHookSub]}
	/invoke ${maControl.Add[hud,submenu]}
	/invoke ${maControl.Add[manautoinv,swAutoInvManual]}
	/invoke ${maControl.Add[maintenance,swMaintenance]}
	/invoke ${maControl.Add[nettimeout,stNetworkTimeout]}
	/invoke ${maControl.Add[rememberset,swBuildSetRemember]}
	/invoke ${maControl.Add[buffcooldownwait,swBuffCooldownWait]}
	/invoke ${set_control_output[${_debug}, maEntropy, entropy, maControl]}

/return TRUE



|***
 * note: control for rez
 * use:
 ***|
#Bind control_rez /rez
sub Bind_control_rez(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atcontrol_rez\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maRez ${_verbage}
		/return TRUE

	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux rez ${_verbage}
		/return TRUE

	| give rez
	} else /if (${_type.Equal[give]}) {
		/invoke ${set_switch_env[FALSE, swRezGive, TRUE, ${_verbage}]}

	| take rez
	} else /if (${_type.Equal[take]}) {
		/invoke ${set_switch_env[FALSE, swRezTake, TRUE, ${_verbage}]}

	| rez in combat
	} else /if (${_type.Equal[combat]}) {
		/invoke ${set_switch_env[FALSE, swRezCombat, TRUE, ${_verbage}]}

	| other thing to rez with
	} else /if (${_type.Equal[other]}) {
		/call do_raw_edit ${_debug} SILENT stResurrection "${_verbage}"

	| rez everyone
	} else /if (${_type.Equal[everyone]}) {
		/invoke ${set_switch_env[FALSE, swRezEveryone, TRUE, ${_verbage}]}

	| use rez tokens
	} else /if (${_type.Equal[token]}) {
		/invoke ${set_switch_env[FALSE, swRezToken, TRUE, ${_verbage}]}

	| rez radius
	} else /if (${_type.Equal[rad]}) {
		/if (!${set_control_num_range[FALSE, stMaxRezRange, "${_verbage}", 0, 200]}) /return

	| set rez accept percent
	} else /if (${_type.Equal[pct]}) {
		/if (${Range.Between[10,96:${Int[${_verbage}]}]}) {
			/if (${lsRezPct.Contains[${_verbage}]}) /call do_raw_edit ${_debug} SILENT stPctMinRez "${_verbage}"
		}

		/declare _lirezpct listiterator local
		/vardata _lirezpct lsRezPct.First.Clone
		/while (!${_lirezpct.IsEnd}) {
			/varset _out ${_out} ${If[${_lirezpct.Value.Equal[${lsRezPct.First}]},,${dot}]} ${If[${maRez.Find[stPctMinRez].Value} == ${_lirezpct.Value},${good}${_lirezpct.Value}\ax,${info}${_lirezpct.Value}\ax]}
			/invoke ${_lirezpct.Advance}
		}
		/invoke ${out[12, "/rez pct ${_out}"]}
		/return TRUE

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[give,swRezGive]}
	/invoke ${maControl.Add[pct,stPctMinRez]}
	/invoke ${maControl.Add[rad,stMaxRezRange]}
	/invoke ${maControl.Add[token,swRezToken]}
	/invoke ${maControl.Add[everyone,swRezEveryone]}
	/invoke ${maControl.Add[take,swRezTake]}
	/invoke ${maControl.Add[combat,swRezCombat]}
	/invoke ${maControl.Add[other,stResurrection|br]}
	/invoke ${set_control_output[${_debug}, maRez, rez, maControl, maControl]}

/return TRUE



|***
 * note: testing control. not for in game use...
 * use:  please see note: and stop asking stupid questions
 ***|
#Bind deez /deez
sub Bind_deez(string _type, string _verbage)

	/declare _count int local 0

	/if (${_type.Equal[nutz]}) {
		/invoke ${out[0, "dannuic said i should put this in to powerlevel people"]}
		/invoke ${out[0, "well. i'm not going to code in powerleveling. but you can"]}
		/invoke ${out[0, "have the command anyways"]}

	}

	/if (!${Defined[maDeez]}) {
		/declare maDeez map outer
	}

	/if (!${Defined[lsDeez]}) {
		/declare lsDeez list outer
	}


	| fakes a hash
	/if (${_type.Equal[hash]}) {
		/declare alpha list local
		/invoke ${alpha.Append[a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z]}
		/declare numeric list local
		/invoke ${numeric.Append[0,1,2,3,4,5,6,7,8,9]}
		/declare _hash string local
		/declare _list string local
		/declare _an int local 0
		/for _count 1 to 32
			/varset _an ${Math.Rand[1,2]}
			/if (${_an} == 1) {
				/varset _list alpha
			} else /if (${_an} == 2) {
				/varset _list numeric
			}
			/varset _hash ${_hash}${${_list}.Item[${Math.Rand[0,${Math.Calc[${${_list}.Count}-1]}]}]}
		/next _count
		/echo hash: \a-w${_hash}\ax
	}

	| var echos for debugging
	/if (${_type.Equal[debug]}) {
		/echo
		OUT \awmode\ax ${dot} melee${sep}\a-wMELEE\ax  nuke${sep}\a-wNUKE\ax  range${sep}\a-wRANGE\ax  dot${sep}\a-wDOT\ax  heal${sep}\a-wHEAL\ax
		OUT {maEnv.Find[swAuto].Value}${sep}\a-w${maEnv.Find[swAuto].Value}\ax .. #auto:\a-wAUTO\ax
		OUT {maCC.Find[swSetCombat].Value}${sep}\a-w${maCC.Find[swSetCombat].Value}
		OUT {maCC.Find[swValidAssTarID].Value}${sep}\a-w${maCC.Find[swValidAssTarID].Value}
		OUT stAssTarID${sep}\a-w${stAssTarID}
		OUT {maBurn.Find[swBurnConditionMet].Value}${sep}\a-w${maBurn.Find[swBurnConditionMet].Value}
		OUT swIsMeDead${sep}\a-w${swIsMeDead}
		OUT {maEnv.Find[stEnvRadius].Value}${sep}\a-w${maEnv.Find[stEnvRadius].Value}\ax .. #env:\a-wENV\ax
		/invoke ${out[18, "${eol}"]}
	}

	| map comma test
	/if (${_type.Equal[map]}) {
		/invoke ${maDeez.Add[001,1]}
		/invoke ${maDeez.Add[007,"Bond, James"]}
		/invoke ${maDeez.Add[007,"Bond-James"]}
		/call Bind_command_see maDeez
	}

	| list comma test
	/if (${_type.Equal[list]}) {
		/invoke ${lsDeez.Append[001]}
		/invoke ${lsDeez.Append["bond, james"]}
		/invoke ${lsDeez.Append["Bond, James"]}
		/call Bind_command_see lsDeez
	}

	| item actors test
	/if (${_type.Equal[actors]}) {
		/declare MyGroundItem ground local
		/vardata MyGroundItem Ground.First
		/for _count 1 to ${GroundItemCount[Glowing Globe]}
			OUT ${MyGroundItem.Name}${sep}ID-\a-w${MyGroundItem.ID}\ax${sep}Path-\a-w${Navigation.PathExists[${MyGroundItem.ID}]}\ax${sep}dis-\a-w${MyGroundItem.Distance}\ax
			/vardata MyGroundItem MyGroundItem.Next
		/next _count
	}

	| ground item loop test
	/if (${_type.Equal[ground]}) {
		/declare _item ground local
		/vardata _item Ground.First
		/varset _count 1
		/while (${_count} <= ${GroundItemCount}) {
			/if (${_item.DisplayName.Find[Glowing Globe]}) {
				/echo ${_count}/${GroundItemCount} -- ${_item.DisplayName}\ax${sep}Path-\a-w${Navigation.PathExists[locyxz ${_item.Y} ${_item.X} ${_item.Z}]}\ax${sep}dis-\a-w${_item.Distance}\ax
			}
			/if (${_count} == ${GroundItemCount}) /break
			/varcalc _count ${_count} + 1
			/vardata _item _item.Next
		}
	}

	| database pull test for a mana click item
	/if (${_type.Equal[dbtest]}) {
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM mana_recovery WHERE env_var="Wand of Arcronite Modulation"
		OUT ${sqlite.Result[dummy 1 value]}
	}


	| pathing crawl test
	/if (${_type.Equal[crawl]}) {

		/declare _tmpID int local 0
		/declare _closest int local 9999999
		/declare _closestID int local 0


		/for _count 1 to ${SpawnCount[npc range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius PULLENV zradius ${maCC.Find[stZRadius].Value} targetable]}
			/varset _tmpID ${NearestSpawn[${_count}, npc range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius PULLENV zradius ${maCC.Find[stZRadius].Value}].ID}
			| /echo ${_tmpID}
			/if (!${Navigation.PathExists[locyxz ${Spawn[${_tmpID}].Y} ${Spawn[${_tmpID}].X} ${Spawn[${_tmpID}].Z}]}) /continue

			OUT  path length to \aw${_tmpID}\ax ${dot} NavDistance${sep}\a-w${Navigation.PathLength[locyxz ${Spawn[${_tmpID}].Y} ${Spawn[${_tmpID}].X} ${Spawn[${_tmpID}].Z}]}\ax (\aySpawnDistance${sep}${Spawn[${_tmpID}].Distance})

			/if (${Navigation.PathLength[locyxz ${Spawn[${_tmpID}].Y} ${Spawn[${_tmpID}].X} ${Spawn[${_tmpID}].Z}]} < ${_closest}) {
				/varset _closest ${Navigation.PathLength[locyxz ${Spawn[${_tmpID}].Y} ${Spawn[${_tmpID}].X} ${Spawn[${_tmpID}].Z}]}
				/varset _closestID ${_tmpID}
			}

		/next _count
		OUT closest mob${sep}\ag${_closestID}\ax

	}

	| testing for finding closest mob
	/if (${_type.Equal[findmob]}) {

		/call pull_mob_find ${_debug}

		OUT Closest mob${sep}\awTARGET\ax${sep}\a-w${Spawn[id TARGET].DisplayName}\ax
		OUT Setting Target
		/invoke ${set_target[TARGET]}
		/varset stAssTarID 0

	}

	/if (${_type.Equal[legs]})  {
		/echo Test 1: ${FindItem[=Battleworn Stalwart Moon Shadowscale Leggings].Spell}
		/echo Test 2: ${FindItem[=Battleworn Stalwart Moon Shadowscale Leggings].SpellID}
		/echo Test 3: ${FindItem[=Battleworn Stalwart Moon Shadowscale Leggings].Spell.SpellID}
		/echo Test 4: ${FindItem[=Battleworn Stalwart Moon Shadowscale Leggings].Clicky}
		/echo Test 5: ${FindItem[=Battleworn Stalwart Moon Shadowscale Leggings].Clicky.Spell}
		/echo Test 6: ${FindItem[=Battleworn Stalwart Moon Shadowscale Leggings].Clicky.SpellID}
	}



	| get spawn info.  in a radius
	/if (${_type.Equal[spawn]}) {
		OUT SpawnCount${sep}\a-w${SpawnCount[radius ${_verbage}]}
		/declare _tmpID int local

		/for _count 1 to ${SpawnCount[radius ${_verbage}]}
			/varset _tmpID ${NearestSpawn[${_count}, radius ${_verbage}].ID}

			/if (${_tmpID} == ${Me.ID}) {
				OUT #\ag${_count}\ax${sep}name:\a-wMe\ax
				/echo
				/continue
			}

			OUT #\ag${_count}\ax${sep}name:\a-w${Spawn[id ${_tmpID}].Name}\ax
			OUT ${sep}Dis(\a-r${Spawn[${_tmpID}].Distance}\ax) ${dot} Y(\a-r${Spawn[id ${_tmpID}].Y}\ax)X(\a-r${Spawn[id ${_tmpID}].X}\ax)
			OUT ${sep}Animation(\a-r${Spawn[id ${_tmpID}].Animation}\ax) ${dot} Body(\a-r${Spawn[id ${_tmpID}].Body}\ax)
			/echo
		/next _count

	}
	| get spawn info.  in a radius
	/if (${_type.Equal[spawn2]}) {
		OUT SpawnCount${sep}\a-w${SpawnCount[radius ${_verbage}]}
		/declare _tmpID int local

		/for _count 1 to ${_verbage}
			/varset _tmpID ${NearestSpawn[${_count}].ID}

			/if (${_tmpID} == ${Me.ID}) {
				OUT #\ag${_count}\ax${sep}name:\a-wMe\ax
				/echo
				/continue
			}

			OUT #\ag${_count}\ax${sep}name:\a-w${Spawn[id ${_tmpID}].Name}\ax ${dot} Animation(\a-r${Spawn[id ${_tmpID}].Animation}\ax) ${dot} Body(\a-r${Spawn[id ${_tmpID}].Body}\ax)
		/next _count

	}


	/if (${_type.Equal[dataset]}) {

    /declare i int local 0
    /if (!${Defined[spawnList]}) {
    	/declare spawnList list outer
    }
    /invoke ${spawnList.Clear}
    /declare testme spawn local 0

    /for i 1 to 4
        /vardata testme NearestSpawn[${i}, npc targetable radius 100]
        /invoke ${spawnList.Append[${NearestSpawn[${i}, npc targetable radius 100}]}
    /next i

    /declare lis listiterator local
    /vardata lis spawnList.First.Clone
    /while (!${lis.IsEnd}) {
    	/echo LOS on: ${lis.Value} = ${Spawn[${lis.Value}].LineOfSight}
    	/invoke ${lis.Advance}
    }
	}



	/if (${_type.Equal[nametest]}) {

		/invoke ${maTTL.Add["bob was here",5]}
		/invoke ${maTTL.Add[bob,5]}

		/echo ${maTTL.Find["bob was here"].Value}
		/echo ${maTTL.Find[bob].Value}

	}


	| get the hurt variables
	/if (${_type.Equal[gethurt]}) {
		/call get_hurt TRUE ${_verbage}
		/mqclear
		OUT Hirt variables${sep}\a-w${_verbage}\ax
		OUT hurtID${sep}\a-w${hurtID}\ax
		OUT hurtPctHP${sep}\a-w${hurtPctHP}\ax
		OUT hurtCLS${sep}\a-w${hurtCLS}\ax
		OUT hurtCount${sep}\a-w${hurtCount}\ax
	}


/return TRUE



|*** TEST ***|
#Bind control_item /item
sub Bind_control_item(string _type, string _verbage, string _silent, bool _debug)
	SDEBUG \atBind_control_item\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)
/return





|***
 * note: your mama so fat, each ass cheek has a different zip code
 * use: /buff
 ***|
#Bind control_buff /buff
sub Bind_control_buff(string _type, string _verbage, string _silent, string _add_delete, bool _debug)
	DEBUG \atBind_control_buff\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maBuff ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux buff ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see buff list
		/return

	} else /if (${Range.Between[1,50:${Int[${_type}]}]}) {

		| see the buff
		/if (!${Bool[${_verbage}]}) {
			/invoke ${set_switch_env[${_debug}, swBuff${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awBuff${_type}\ax${sep}${If[${maBuff.Find[swBuff${_type}].Value},${on},${off}]}${sep}\a-w${maBuff.Find[stBuff${_type}].Value}\ax"]}
			/return TRUE
		} else /if (${_verbage.Equal[see]}) {
			/call Bind_command_see buff ${_type}
			/return TRUE
		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swBuff${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBuff${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBuff${_type}Alias" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBuff${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsBuff${_type}Tag" clear
		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBuff${_type}Con" "${_silent}"
		} else /if (${Select[${_verbage},alias]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBuff${_type}Alias" "${_silent}"
		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsBuff${_type}Tag" "${_silent}" ${_add_delete} ${_verbage}
		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,alias,clear,see]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBuff${_type}" "${_verbage}"
		}

		/call Bind_command_see buff ${_type}
		/return TRUE


	| master buff switch
	} else /if (${_type.Equal[master]}) {
		/invoke ${set_switch_env[${_debug}, swBuffMaster, FALSE, ${_verbage}]}

	} else /if (${_type.Equal[ps]}) {
		/invoke ${set_switch_env[FALSE, swBuffPowersource, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[self]}) {
		/invoke ${set_switch_env[${_debug}, swBuffSelf, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[crew]}) {
		/invoke ${set_switch_env[${_debug}, swBuffCrew, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[now]}) {
		/invoke ${set_switch_env[${_debug}, swBuffNow, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[raid]}) {
		/invoke ${set_switch_env[${_debug}, swBuffRaid, TRUE, ${_verbage}]}

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
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swBuffAura, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[aura1]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stAura1, "${_silent}"]}
		} else /if (${_verbage.Equal[aura1check]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stAura1Check, "${_silent}"]}
		} else /if (${_verbage.Equal[aura2]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stAura2, "${_silent}"]}
		} else /if (${_verbage.Equal[aura2check]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stAura2Check, "${_silent}"]}
		}

		| aura 1
		/invoke ${out[12, "/buff aura${sep}"]}
		/invoke ${out[12, "${dot} ${info}use\ax:${If[${maBuff.Find[swBuffAura].Value},${on},${off}]}"]}
		/invoke ${out[12, "${dot} ${info}aura1\ax:[${If[${Bool[${maBuff.Find[stAura1].Value}]},${num}${maBuff.Find[stAura1].Value}\ax,${off}]}]"]}
		/invoke ${out[12, "${dot} ${info}aura1check\ax:[${If[${Bool[${maBuff.Find[stAura1Check].Value}]},${num}${maBuff.Find[stAura1Check].Value}\ax,${off}]}]"]}
		/invoke ${out[12, "${dot} lsAura\aw1\axTag"]}
		/declare _lia1 listiterator local
		/vardata _lia1 lsAura1Tag.First.Clone
		/while (!${_lia1.IsEnd}) {
			/invoke ${out[12, "${dot}${dot} \a-w${_lia1.Value}\ax"]}
			/invoke ${_lia1.Advance}
		}
		/if (!${Select[${Me.Class.ShortName},CLR,ENC]}) /return

		| aura 2
		/echo
		/invoke ${out[12, "${dot} ${info}aura2\ax:[${If[${Bool[${maBuff.Find[stAura2].Value}]},${num}${maBuff.Find[stAura2].Value}\ax,${off}]}]"]}
		/invoke ${out[12, "${dot} ${info}aura2check\ax:[${If[${Bool[${maBuff.Find[stAura2Check].Value}]},${num}${maBuff.Find[stAura2Check].Value}\ax,${off}]}]"]}
		/invoke ${out[12, "${dot} lsAura\aw2\axTag"]}
		/declare _lia2 listiterator local
		/vardata _lia2 lsAura2Tag.First.Clone
		/while (!${_lia2.IsEnd}) {
			/invoke ${out[12, "${dot}${dot} \a-w${_lia2.Value}\ax"]}
			/invoke ${_lia2.Advance}
		}
		/return

	} else /if (${_type.Equal[temp]}) {
		/invoke ${set_switch_env[${_debug}, swBuffTemp, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[shrink]}) {
		/if (${_verbage.Equal[item]}) {
			/call do_raw_edit ${_debug} SILENT stShrinkItem "${_silent}"
		} else /if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swBuffShrink, TRUE, ${_silent}]}
		}

		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[use,swBuffShrink]}
		/invoke ${maControl.Add[item,stShrinkItem|br]}
		/invoke ${set_control_output[${_debug}, maBuff, "buff shrink", maControl]}
		/return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[ammo,swBuffAmmo]}
	/invoke ${maControl.Add[aura,submenu]}
	/invoke ${maControl.Add[beg,swBuffBeg]}
	/invoke ${maControl.Add[crew,swBuffCrew]}
	/invoke ${maControl.Add[master,swBuffMaster|pr]}
	/invoke ${maControl.Add[now,swBuffNow]}
	/invoke ${maControl.Add[ps,swBuffPowersource]}
	/invoke ${maControl.Add[raid,swBuffRaid]}
	/invoke ${maControl.Add[self,swBuffSelf]}
	/invoke ${maControl.Add[shrink,submenu]}
	/invoke ${maControl.Add[temp,swBuffTemp]}
	/invoke ${maControl.Add[unity,swBuffUnity]}
	/invoke ${set_control_output[${_debug}, maBuff, buff, maControl]}

/return TRUE



|***
 * note: control for overrides
 * use:
 ***|
#Bind control_over /override
sub Bind_control_over(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_over\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maOver ${_verbage}
		/return TRUE
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux override ${_verbage}
		/return TRUE

	} else /if (${_type.Equal[los]}) {
		/invoke ${set_switch_env[FALSE, swOverLOS, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[engage]}) {
		/invoke ${set_switch_env[FALSE, swOverPctEngage, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[loot]}) {
		/invoke ${set_switch_env[FALSE, swOverLoot, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[safenames]}) {
		/invoke ${set_switch_env[FALSE, swOverSafeNames, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[trainspell]}) {
		/invoke ${set_switch_env[FALSE, swOverTrainSpell, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[envauto]}) {
		/invoke ${set_switch_env[FALSE, swOverEnvAuto, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[dscheck]}) {
		/invoke ${set_switch_env[FALSE, swOverDSCheck, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[navcheck]}) {
		/invoke ${set_switch_env[FALSE, swOverNavCheck, TRUE, ${_verbage}]}



	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[los,swOverLOS]}
	/invoke ${maControl.Add[engage,swOverPctEngage]}
	/invoke ${maControl.Add[loot,swOverLoot]}
	/invoke ${maControl.Add[safenames,swOverSafeNames]}
	/invoke ${maControl.Add[trainspell,swOverTrainSpell]}
	/invoke ${maControl.Add[dscheck,swOverDSCheck]}
	/invoke ${maControl.Add[envauto,swOverEnvAuto]}
	/invoke ${maControl.Add[navcheck,swOverNavCheck]}
	/invoke ${set_control_output[${_debug}, maOver, override, maControl]}

/return	TRUE



|***
 * note: shorthand for raw db editing
 * use: mostly for breaking shit
 ***|
#bind_noparse raw_edit /edit
sub Bind_noparse_raw_edit(string _variable, string _value, string _add_delete, string _verbage)

	/call do_raw_edit ${maDebug.Find[edit].Value} FALSE "${_variable}" "${Parse[1,${_value}]}" ${_add_delete} ${_verbage}

/return	TRUE


|***
 * note: lets see what we can see
 * use: /see [a thing]
 ***|
#bind command_see /see
sub Bind_command_see(string _variable, string _verbage, string _silent)
	/declare _debug bool local FALSE

	DEBUG \atBind_control_see\ax(\a-w${_variable}, ${_verbage}, ${_silent}\ax)
	DEBUG .Left[2]${sep}\a-w${_variable.Left[2]}\ax

	| input validations
	/if (!${_variable.Length} || ${_variable.Equal[NULL]} || ${_variable.Equal[aux]}) {
		/call Bind_command_aux see
		/return	FALSE
	} else /if (!${Defined[${_variable}]} && !${Select[${_variable},buff,list,condition,aux,item,target,defense,heal,dot,nuke,agro,miscdps,song,burn,sash]}) {
		/invoke ${out[0, "Crack says your input sucks${sep}\a-w${_variable}\ax"]}
		/return FALSE
	}

	/declare _count int local 0
	/declare _DBLOC string local
	/declare _tmpAux string local

	| shows us detailed target info
	/if (${_variable.Equal[target]}) {
		OUT name:\aw${Target.DisplayName}\ax (\a-r${Target.Name}\ax) ${sep} \ay${Target.ID}\ax
		OUT dis(\a-r${Target.Distance}\ax) ${dot} Y(\a-r${Target.Y}\ax)X(\a-r${Target.X}\ax)
		OUT animation(\a-r${Target.Animation}\ax) ${dot} body(\a-r${Target.Body}\ax)



	| see sash info
	} else /if (${_variable.Equal[sash]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awSash List\ax${sep}"]}
			/declare _licountsash listiterator local
		  /vardata _licountsash lsCount12.First.Clone
		  /while (!${_licountsash.IsEnd}) {
				/if (${Bool[${maSash.Find[stSash${_licountsash.Value}].Value}]}) {
					/invoke ${out[12, "\awstSash${_licountsash.Value}\ax${sep}${If[${maSash.Find[swSash${_licountsash.Value}].Value},${on},${off}]}${sep}\a-w${maSash.Find[stSash${_licountsash.Value}].Value}\ax"]}
				}
		    /invoke ${_licountsash.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,12:${Int[${_verbage}]}]}) {
			/call Bind_command_aux sash
			/return
		}
		/invoke ${out[12, "\awSash #${_verbage}\ax${sep}${If[${maSash.Find[swSash${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stSash\aw${_verbage}\ax${sep}\a-w${maSash.Find[stSash${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maSash.Find[stSash${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stSash\aw${_verbage}\axCon${sep}\a-w${maSash.Find[stSash${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stSash\aw${_verbage}\axCon${sep}\a-w${maSash.Find[stSash${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsSash\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsSash${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}



	| see song info
	} else /if (${_variable.Equal[song]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awSong List\ax${sep}"]}
			/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT value FROM environment_song WHERE env_var="lsLineSong"
			/invoke ${out[12, "Line${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}

			/declare _licountsong listiterator local
		  /vardata _licountsong lsCount20.First.Clone
		  /while (!${_licountsong.IsEnd}) {
				/if (${Bool[${maSong.Find[stSong${_licountsong.Value}].Value}]}) {
					/invoke ${out[12, "\awstSong${_licountsong.Value}\ax${sep}${If[${maSong.Find[swSong${_licountsong.Value}].Value},${on},${off}]}${sep}\a-w${maSong.Find[stSong${_licountsong.Value}].Value}\ax"]}
				}
		    /invoke ${_licountsong.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux song
			/return
		}
		/invoke ${out[12, "\awSong #${_verbage}\ax${sep}${If[${maSong.Find[swSong${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stSong\aw${_verbage}\ax${sep}\a-w${maSong.Find[stSong${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maSong.Find[stSong${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stSong\aw${_verbage}\axCon${sep}\a-w${maSong.Find[stSong${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stSong\aw${_verbage}\axCon${sep}\a-w${maSong.Find[stSong${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsSong\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsSong${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}


	| see miscDPS info
	} else /if (${_variable.Equal[miscdps]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awMiscDPS List\ax${sep}"]}

			/declare _licountmiscdps listiterator local
		  /vardata _licountmiscdps lsCount50.First.Clone
		  /while (!${_licountmiscdps.IsEnd}) {
				/if (${Bool[${maMiscDPS.Find[stMiscDPS${_licountmiscdps.Value}].Value}]}) {
					/invoke ${out[12, "\awstMiscDPS${_licountmiscdps.Value}\ax${sep}${If[${maMiscDPS.Find[swMiscDPS${_licountmiscdps.Value}].Value},${on},${off}]}${sep}\a-w${maMiscDPS.Find[stMiscDPS${_licountmiscdps.Value}].Value}\ax"]}
				}
		    /invoke ${_licountmiscdps.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,50:${Int[${_verbage}]}]}) {
			/call Bind_command_aux miscdps
			/return
		}
		/invoke ${out[12, "\awMiscDPS #${_verbage}\ax${sep}${If[${maMiscDPS.Find[swMiscDPS${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stMiscDPS\aw${_verbage}\ax${sep}\a-w${maMiscDPS.Find[stMiscDPS${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maMiscDPS.Find[stMiscDPS${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stMiscDPS\aw${_verbage}\axCon${sep}\a-w${maMiscDPS.Find[stMiscDPS${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stMiscDPS\aw${_verbage}\axCon${sep}\a-w${maMiscDPS.Find[stMiscDPS${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsMiscDPS\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsMiscDPS${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}

	| see agro info
	} else /if (${_variable.Equal[agro]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awAgro List\ax${sep}"]}

			/declare _licountdot listiterator local
		  /vardata _licountdot lsCount20.First.Clone
		  /while (!${_licountdot.IsEnd}) {
				/if (${Bool[${maAgro.Find[stAgro${_licountdot.Value}].Value}]}) {
					/invoke ${out[12, "\awstAgro${_licountdot.Value}\ax${sep}${If[${maAgro.Find[swAgro${_licountdot.Value}].Value},${on},${off}]}${sep}\a-w${maAgro.Find[stAgro${_licountdot.Value}].Value}\ax"]}
				}
		    /invoke ${_licountdot.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux agro
			/return
		}
		/invoke ${out[12, "\awAgro #${_verbage}\ax${sep}${If[${maAgro.Find[swAgro${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stAgro\aw${_verbage}\ax${sep}\a-w${maAgro.Find[stAgro${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maAgro.Find[stAgro${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stAgro\aw${_verbage}\axCon${sep}\a-w${maAgro.Find[stAgro${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stAgro\aw${_verbage}\axCon${sep}\a-w${maAgro.Find[stAgro${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsAgro\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsAgro${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}



	| see dot info
	} else /if (${_variable.Equal[dot]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awDoT List\ax${sep}"]}
			/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT value FROM environment_dot WHERE env_var="lsLineDoT"
			/invoke ${out[12, "Line${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}

			/declare _licountdot listiterator local
		  /vardata _licountdot lsCount20.First.Clone
		  /while (!${_licountdot.IsEnd}) {
				/if (${Bool[${maDoT.Find[stDoT${_licountdot.Value}].Value}]}) {
					/invoke ${out[12, "\awstDoT${_licountdot.Value}\ax${sep}${If[${maDoT.Find[swDoT${_licountdot.Value}].Value},${on},${off}]}${sep}\a-w${maDoT.Find[stDoT${_licountdot.Value}].Value}\ax"]}
				}
		    /invoke ${_licountdot.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux dot
			/return
		}
		/invoke ${out[12, "\awDoT #${_verbage}\ax${sep}${If[${maDoT.Find[swDoT${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stDoT\aw${_verbage}\ax${sep}\a-w${maDoT.Find[stDoT${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maDoT.Find[stDoT${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stDoT\aw${_verbage}\axCon${sep}\a-w${maDoT.Find[stDoT${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stDoT\aw${_verbage}\axCon${sep}\a-w${maDoT.Find[stDoT${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsDoT\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsDoT${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}

	| see nuke info
	} else /if (${_variable.Equal[nuke]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awNuke List\ax${sep}"]}
			/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT value FROM environment_nuke WHERE env_var="lsLineNuke"
			/invoke ${out[12, "Line${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}

			/declare _licountnuke listiterator local
		  /vardata _licountnuke lsCount20.First.Clone
		  /while (!${_licountnuke.IsEnd}) {
				/if (${Bool[${maNuke.Find[stNuke${_licountnuke.Value}].Value}]}) {
					/invoke ${out[12, "\awstNuke${_licountnuke.Value}\ax${sep}${If[${maNuke.Find[swNuke${_licountnuke.Value}].Value},${on},${off}]}${sep}\a-w${maNuke.Find[stNuke${_licountnuke.Value}].Value}\ax"]}
				}
		    /invoke ${_licountnuke.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux nuke
			/return
		}
		/invoke ${out[12, "\awNuke #${_verbage}\ax${sep}${If[${maNuke.Find[swNuke${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stNuke\aw${_verbage}\ax${sep}\a-w${maNuke.Find[stNuke${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maNuke.Find[stNuke${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stNuke\aw${_verbage}\axCon${sep}\a-w${maNuke.Find[stNuke${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stNuke\aw${_verbage}\axCon${sep}\a-w${maNuke.Find[stNuke${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsNuke\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsNuke${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}


	| see heal info
	} else /if (${_variable.Equal[heal]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awHeal List\ax${sep}"]}
			/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT value FROM environment_heal WHERE env_var="lsLineHeal"
			/invoke ${out[12, "Line${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}

			/declare _licountheal listiterator local
		  /vardata _licountheal lsCount50.First.Clone
		  /while (!${_licountheal.IsEnd}) {
				/if (${Bool[${maHeal.Find[stHeal${_licountheal.Value}].Value}]}) {
					/invoke ${out[12, "\awstHeal${_licountheal.Value}\ax${sep}${If[${maHeal.Find[swHeal${_licountheal.Value}].Value},${on},${off}]}${sep}\a-w${maHeal.Find[stHeal${_licountheal.Value}].Value}\ax"]}
				}
		    /invoke ${_licountheal.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,50:${Int[${_verbage}]}]}) {
			/call Bind_command_aux heal
			/return
		}
		/invoke ${out[12, "\awHeal #${_verbage}\ax${sep}${If[${maHeal.Find[swHeal${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stHeal\aw${_verbage}\ax${sep}\a-w${maHeal.Find[stHeal${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maHeal.Find[stHeal${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stHeal\aw${_verbage}\axCon${sep}\a-w${maHeal.Find[stHeal${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stHeal\aw${_verbage}\axCon${sep}\a-w${maHeal.Find[stHeal${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsHeal\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsHeal${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}


	| see burn info
	} else /if (${_variable.Equal[burn]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awBurn List\ax${sep}"]}
			/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT value FROM environment_burn WHERE env_var="lsLineBurn"
			/invoke ${out[12, "Line${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}

			/declare _licountburn listiterator local
		  /vardata _licountburn lsCount20.First.Clone
		  /while (!${_licountburn.IsEnd}) {
				/if (${Bool[${maBurn.Find[stBurn${_licountburn.Value}].Value}]}) {
					/invoke ${out[12, "\awstBurn${_licountburn.Value}\ax${sep}${If[${maBurn.Find[swBurn${_licountburn.Value}].Value},${on},${off}]}${sep}\a-w${maBurn.Find[stBurn${_licountburn.Value}].Value}\ax"]}
				}
		    /invoke ${_licountburn.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux burn
			/return
		}
		/invoke ${out[12, "\awBurn #${_verbage}\ax${sep}${If[${maBurn.Find[swBurn${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stBurn\aw${_verbage}\ax${sep}\a-w${maBurn.Find[stBurn${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maBurn.Find[stBurn${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stBurn\aw${_verbage}\axCon${sep}\a-w${maBurn.Find[stBurn${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stBurn\aw${_verbage}\axCon${sep}\a-w${maBurn.Find[stBurn${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsBurn\aw${_verbage}\axTag"]}

		/declare hli listiterator local
		/vardata hli lsBurn${_verbage}Tag.First.Clone
		/while (!${hli.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${hli.Value}\ax"]}
			/invoke ${hli.Advance}
		}


	| see defense info
	} else /if (${_variable.Equal[defense]}) {

		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awDefense List\ax${sep}"]}
			/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT value FROM environment_defense WHERE env_var="lsLineDefense"
			/invoke ${out[12, "Line${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}

			/declare _licountdef listiterator local
		  /vardata _licountdef lsCount20.First.Clone
		  /while (!${_licountdef.IsEnd}) {
				/if (${Bool[${maDefense.Find[stDefense${_licountdef.Value}].Value}]}) {
					/invoke ${out[12, "\awstDefense${_licountdef.Value}\ax${sep}${If[${maDefense.Find[swDefense${_licountdef.Value}].Value},${on},${off}]}${sep}\a-w${maDefense.Find[stDefense${_licountdef.Value}].Value}\ax"]}
				}
		    /invoke ${_licountdef.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux defense
			/return
		}
		/invoke ${out[12, "\awDefense #${_verbage}\ax${sep}${If[${maDefense.Find[swDefense${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stDefense\aw${_verbage}\ax${sep}\a-w${maDefense.Find[stDefense${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maDefense.Find[stDefense${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stDefense\aw${_verbage}\axCon${sep}\a-w${maDefense.Find[stDefense${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stDefense\aw${_verbage}\axCon${sep}\a-w${maDefense.Find[stDefense${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsDefense\aw${_verbage}\axTag"]}

		/declare _lidef listiterator local
		/vardata _lidef lsDefense${_verbage}Tag.First.Clone
		/while (!${_lidef.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${_lidef.Value}\ax"]}
			/invoke ${_lidef.Advance}
		}


	| see the buff info
	} else /if (${_variable.Equal[buff]}) {

		| list buffs
		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awBuff List\ax${sep}"]}
			/for _count 1 to 50
				/if (${maBuff.Find[stBuff${_count}].Value.NotEqual[FALSE]}) {
					/invoke ${out[12, "\awstBuff${_count}\ax${sep}${If[${maBuff.Find[swBuff${_count}].Value},${on},${off}]}${sep}\a-w${maBuff.Find[stBuff${_count}].Value}\ax"]}
				}
			/next _count
			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return TRUE

		| buff spa
		} else /if (${_verbage.Equal[spa]}) {
			DEBUG /call Bind_control_admin spa "${maBuff.Find[stBuff${_silent}].Value}"
			/call Bind_control_admin spa "${maBuff.Find[stBuff${_silent}].Value}"
			/return TRUE
		}

		/if (!${Range.Between[1,50:${Int[${_verbage}]}]}) {
			/call Bind_command_aux buff
			/return
		}

		/declare _tmpC string local ${maBuff.Find[stBuff${_verbage}].Value.Arg[2,+c]}
		/invoke ${out[12, "\awBuff #${_verbage}\ax${sep}${If[${maBuff.Find[swBuff${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stBuff\aw${_verbage}\ax${sep}\a-w${maBuff.Find[stBuff${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maBuff.Find[stBuff${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stBuff\aw${_verbage}\axCon${sep}\a-w${maBuff.Find[stBuff${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stBuff\aw${_verbage}\axCon${sep}\a-w${maBuff.Find[stBuff${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "stBuff\aw${_verbage}\axAlias${sep}\a-w${maBuff.Find[stBuff${_verbage}Alias].Value}\ax"]}
		/invoke ${out[12, "lsBuff\aw${_verbage}\axTag"]}

		/declare _liseedeb listiterator local
		/vardata _liseedeb lsBuff${_verbage}Tag.First.Clone
		/while (!${_liseedeb.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${_liseedeb.Value}\ax"]}
			/invoke ${_liseedeb.Advance}
		}

	} else /if (${_variable.Equal[condition]}) {
		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awCondition List\ax${sep}"]}
			/for _count 1 to 50

				/sqlite ${maData.Find[DBcharacter].Value} condition SELECT value FROM environment_condition WHERE env_var="c${_count}"

				/if (${Parse[1,${sqlite.Result[condition 1 value].Equal[FALSE]}]}) /continue

				OUT \awc${_count}\ax${sep}\a-w${Parse[1,${sqlite.Result[condition 1 value]}]}\ax
			/next _count
			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/sqlite ${maData.Find[DBcharacter].Value} condition SELECT value FROM environment_condition WHERE env_var="c${_verbage}"
		OUT c${_verbage}${sep}\a-w${Parse[1,${sqlite.Result[condition 1 value]}]}\ax
		/return TRUE


	} else /if (${_variable.Equal[item]}) {
		/if (${_verbage.Equal[list]}) {
			/echo
			/invoke ${out[12, "\awItem List\ax${sep}"]}


			/declare _licountitem listiterator local
		  /vardata _licountitem lsCount20.First.Clone
		  /while (!${_licountitem.IsEnd}) {
				/if (${Bool[${maItem.Find[stItem${_licountitem.Value}].Value}]}) {
					/invoke ${out[12, "\awstItem${_licountitem.Value}\ax${sep}${If[${maItem.Find[swItem${_licountitem.Value}].Value},${on},${off}]}${sep}\a-w${maItem.Find[stItem${_licountitem.Value}].Value}\ax"]}
				}
		    /invoke ${_licountitem.Advance}
		  }

			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
			/echo
			/return
		}

		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux item
			/return
		}
		/invoke ${out[12, "\awItem #${_verbage}\ax${sep}${If[${maItem.Find[swItem${_verbage}].Value},${on},${off}]}"]}
		/invoke ${out[12, "stItem\aw${_verbage}\ax${sep}\a-w${maItem.Find[stItem${_verbage}].Value}\ax"]}

		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition WHERE env_var="c${maItem.Find[stItem${_verbage}Con].Value}"
		/if (${sqlite.Rows[dummy]}) {
			OUT stItem\aw${_verbage}\axCon${sep}\a-w${maItem.Find[stItem${_verbage}Con].Value}\ax${sep}\a-w${Parse[1,${sqlite.Result[dummy 1 value]}]}\ax
		} else {
			OUT stItem\aw${_verbage}\axCon${sep}\a-w${maItem.Find[stItem${_verbage}Con].Value}\ax
		}

		/invoke ${out[12, "lsItem\aw${_verbage}\axTag"]}

		/declare _liseeit listiterator local
		/vardata _liseeit lsItem${_verbage}Tag.First.Clone
		/while (!${_liseeit.IsEnd}) {
			/invoke ${out[12, "${dot}\a-w${_liseeit.Value}\ax"]}
			/invoke ${_liseeit.Advance}
		}

	| see what we are observing
	} else /if (${_variable.Equal[observed]}) {
		/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
			/if (!${Defined[lsObserved_${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}]}) /continue
			/invoke ${out[12, "Observed on${sep}\at${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|].Upper}\ax"]}
			/declare _liob listiterator local
			/vardata _liob lsObserved_${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}.First
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
		| /if (!${lsMaps.Contains[${_variable}]} && ${_verbage.NotEqual[force]}) /return
		/invoke ${out[12, "\aw${_variable}\ax ${dot} \a-g${${_variable}.Count}\ax ${If[${${_variable}} > 1 || ${${_variable}} == 0,items,item]}"]}
		/if (!${${_variable}.Count}) /return
		/declare _misee mapiterator local
		/vardata _misee ${_variable}.First.Clone
		/while (!${_misee.IsEnd}) {
			/if (${_verbage.Equal[aux]}) {
				/varset _DBLOC ${get_key_DBLOC[${_debug}, ${_misee.Key}]}
				/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${_misee.Key}'
				/if (${sqlite.Status[dummy].Equal[SUCCESS]} && ${sqlite.Result[dummy 1 aux].Length} > 6) {
					/varset _tmpAux ${dot} ${sqlite.Result[dummy 1 aux]}
				}	else {
					/varset _tmpAux
				}
			}
			OUT ${_misee.Key}${sep}\aw${Parse[1,${_misee.Value}]}\ax
			/if (${_verbage.Equal[aux]}) {
				/invoke ${out[12, "\at..\ax\a-g${_tmpAux}\ax"]}
			}
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
	/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}
	/echo

/return	TRUE



|***
 * note: control for burn
 * use: /burn []
 ***|
#bind control_burn /burn
sub Bind_control_burn(string _type, string _verbage, string _silent, string _verbage2, bool _debug)
	DEBUG \atBind_control_burn\ax(\a-w${_type}, ${_verbage}, ${_silent}, ${_verbage2}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maBurn ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux burn ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/if (!ENGINE2) {
			/invoke ${out[0, "Engine 2 is currently disabled."]}
			/return FALSE
		}

		/call Bind_command_see burn list
		/return	TRUE

	| burn number entered
	} else /if (${Range.Between[1,20:${Int[${_type}]}]}) {
		/if (!ENGINE2) {
			/invoke ${out[0, "Engine 2 is currently disabled."]}
			/return FALSE
		}

		| see the defensive #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see burn ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swBurn${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awBurn${_type}\ax${sep}${If[${maBurn.Find[swBurn${_type}].Value},${on},${off}]}${sep}\a-w${maBurn.Find[stBurn${_type}].Value}\ax"]}
			/return TRUE
		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swBurn${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBurn${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBurn${_type}Alias" clear
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBurn${_type}" FALSE
		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBurn${_type}Con" "${_silent}"
		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsBurn${_type}Tag" ${_silent} ${_verbage2}
		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stBurn${_type}" "${_verbage}"
		}

		/call Bind_command_see burn ${_type}

		/return TRUE


	| Burn Automatic on/off
	} else /if (${_type.Equal[auto]}) {
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

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[auto,swBurnAuto]}
	/invoke ${maControl.Add[count,stBurnCount]}
	/invoke ${maControl.Add[engage,stPctBurnEngage]}
	/invoke ${maControl.Add[force,swBurnForce]}
	/invoke ${maControl.Add[raid,swBurnRaid]}
	/invoke ${set_control_output[${_debug}, maBurn, burn, maControl]}

/return TRUE



|***
 * note: set various debuff things
 * use: /debuff []
 ***|
#bind control_debuff /debuff
sub Bind_control_debuff(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_debuff\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	/declare _lsoutS list local
	/invoke ${_lsoutS.Append[use,single,ae]}
  /declare _out string local
  /declare _lideb listiterator local
	/vardata _lideb _lsoutS.First.Clone

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see madeBuff ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux debuff ${_verbage}
		/return

	} else /if (${_type.Equal[mez]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swMez, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stMez, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stAEMez, "${_silent}"]}
		} else /if (${_verbage.Equal[resistcount]}) {
			/if (!${set_control_num_range[${_debug}, stCountMezResist, "${_silent}", 0, 10]}) /return
		}

		/varset _out
		/invoke ${_lsoutS.Append[resistcount]}
		/vardata _lideb _lsoutS.First.Clone
		/while (!${_lideb.IsEnd}) {
			/varset _out ${_out} ${If[${_lideb.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${_lideb.Value}\ax
			/if (${_lideb.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swMez].Value},${on},${off}]}
			/if (${_lideb.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stMez].Value}]},${num}${madeBuff.Find[stMez].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAEMez].Value}]},${num}${madeBuff.Find[stAEMez].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[resistcount]}) /varset _out ${_out}:${num}${madeBuff.Find[stCountMezResist].Value}\ax
			/invoke ${_lideb.Advance}
		}
		/invoke ${out[12, "/debuff mez ${_out}"]}
		/return

	} else /if (${_type.Equal[malo]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swMalo, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stMalo, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stAEMalo, "${_silent}"]}
		}
		/varset _out
		/vardata _lideb _lsoutS.First.Clone
		/while (!${_lideb.IsEnd}) {
			/varset _out ${_out} ${If[${_lideb.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${_lideb.Value}\ax
			/if (${_lideb.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swMalo].Value},${on},${off}]}
			/if (${_lideb.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stMalo].Value}]},${num}${madeBuff.Find[stMalo].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAEMalo].Value}]},${num}${madeBuff.Find[stAEMalo].Value}\ax,${off}]}]
			/invoke ${_lideb.Advance}
		}
		/invoke ${out[12, "/debuff malo ${_out}"]}
		/return TRUE

	} else /if (${_type.Equal[tash]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swTash, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stTash, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stAETash, "${_silent}"]}
		}
		/while (!${_lideb.IsEnd}) {
			/varset _out ${_out} ${If[${_lideb.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${_lideb.Value}\ax
			/if (${_lideb.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swTash].Value},${on},${off}]}
			/if (${_lideb.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stTash].Value}]},${num}${madeBuff.Find[stTash].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAETash].Value}]},${num}${madeBuff.Find[stAETash].Value}\ax,${off}]}]
			/invoke ${_lideb.Advance}
		}
		/invoke ${out[12, "/debuff tash ${_out}"]}
		/return TRUE

	} else /if (${_type.Equal[slow]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swSlow, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stSlow, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stAESlow, "${_silent}"]}
		}
		/varset _out
		/while (!${_lideb.IsEnd}) {
			/varset _out ${_out} ${If[${_lideb.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${_lideb.Value}\ax
			/if (${_lideb.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swSlow].Value},${on},${off}]}
			/if (${_lideb.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stSlow].Value}]},${num}${madeBuff.Find[stSlow].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAESlow].Value}]},${num}${madeBuff.Find[stAESlow].Value}\ax,${off}]}]
			/invoke ${_lideb.Advance}
		}
		/invoke ${out[12, "/debuff slow ${_out}"]}
		/return TRUE


	} else /if (${_type.Equal[snare]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swSnare, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stSnare, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stAESnare, "${_silent}"]}
		}

		/while (!${_lideb.IsEnd}) {
			/varset _out ${_out} ${If[${_lideb.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${_lideb.Value}\ax
			/if (${_lideb.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swSnare].Value},${on},${off}]}
			/if (${_lideb.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stSnare].Value}]},${num}${madeBuff.Find[stSnare].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAESnare].Value}]},${num}${madeBuff.Find[stAESnare].Value}\ax,${off}]}]
			/invoke ${_lideb.Advance}
		}
		/invoke ${out[12, "/debuff snare ${_out}"]}
		/return TRUE

	} else /if (${_type.Equal[eradicate]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swEradicate, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stEradicate, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stAEEradicate, "${_silent}"]}
		}
		/while (!${_lideb.IsEnd}) {
			/varset _out ${_out} ${If[${_lideb.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${_lideb.Value}\ax
			/if (${_lideb.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swEradicate].Value},${on},${off}]}
			/if (${_lideb.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stEradicate].Value}]},${num}${madeBuff.Find[stEradicate].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAEEradicate].Value}]},${num}${madeBuff.Find[stAEEradicate].Value}\ax,${off}]}]
			/invoke ${_lideb.Advance}
		}
		/invoke ${out[12, "/debuff eradicate ${_out}"]}
		/return TRUE

	} else /if (${_type.Equal[cripple]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swCripple, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stCripple, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/invoke ${do_raw_edit[${_debug}, TRUE, stAECripple, "${_silent}"]}
		}
		/while (!${_lideb.IsEnd}) {
			/varset _out ${_out} ${If[${_lideb.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${_lideb.Value}\ax
			/if (${_lideb.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swCripple].Value},${on},${off}]}
			/if (${_lideb.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stCripple].Value}]},${num}${madeBuff.Find[stCripple].Value}\ax,${off}]}]
			/if (${_lideb.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAECripple].Value}]},${num}${madeBuff.Find[stAECripple].Value}\ax,${off}]}]

			/invoke ${_lideb.Advance}
		}
		/invoke ${out[12, "/debuff cripple ${_out}"]}
		/return TRUE

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[cripple,swCripple|sm]}
	/invoke ${maControl.Add[eradicate,swEradicate|sm]}
	/invoke ${maControl.Add[malo,swMalo|sm]}
	/invoke ${maControl.Add[mez,swMez|sm]}
	/invoke ${maControl.Add[slow,swSlow|sm]}
	/invoke ${maControl.Add[snare,swSnare|sm]}
	/invoke ${maControl.Add[tash,swTash|sm]}
	/invoke ${set_control_output[${_debug}, madeBuff, debuff, maControl]}

/return TRUE



|***
 * note: control for /cc
 * use:
 ***|
#Bind control_cc /cc
sub Bind_control_cc(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_cc\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /declare _out string local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maCC ${_verbage}
		/return TRUE
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux cc ${_verbage}
		/return TRUE

	| set assist
	} else /if (${_type.Equal[ass]}) {
		/if (${_verbage.Equal[g]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[g]},g,FALSE]}
		} else /if (${_verbage.Equal[ma1]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode "ma1"
			| /if (${Bool[${_silent}]} && ${_silent.NotEqual[SILENT]}) {
				/call do_raw_edit ${_debug} SILENT stManualAssist1 "${_silent}"
			| }
		} else /if (${_verbage.Equal[ma2]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode "ma2"
			| /if (${Bool[${_silent}]} && ${_silent.NotEqual[SILENT]}) {
				/call do_raw_edit ${_debug} SILENT stManualAssist2 "${_silent}"
			| }
		} else /if (${_verbage.Equal[1]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[1]},1,FALSE]}
		} else /if (${_verbage.Equal[2]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[2]},2,FALSE]}
		} else /if (${_verbage.Equal[3]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[3]},3,FALSE]}
		}

		/if (${_silent.Equal[SILENT]}) /return
		/declare _lsAssistTypes list local
		/invoke ${_lsAssistTypes.Append[g,1,2,3,ma1,ma2]}
		/declare _liass listiterator local
		/vardata _liass _lsAssistTypes.First.Clone
		/while (!${_liass.IsEnd}) {
			/varset _out ${_out} ${If[${_liass.Value.Equal[${lsAssistTypes.First}]},,${dot}]} ${info}${_liass.Value}\ax
			/if (${_liass.Value.Equal[g]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[g]},${on},${off}]}
			/if (${_liass.Value.Equal[1]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[1]},${on},${off}]}
			/if (${_liass.Value.Equal[2]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[2]},${on},${off}]}
			/if (${_liass.Value.Equal[3]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[3]},${on},${off}]}
			/if (${_liass.Value.Equal[ma1]}) /varset _out ${_out}:[${If[${Bool[${maCC.Find[stManualAssist1].Value}]},${num}${maCC.Find[stManualAssist1].Value}\ax,${off}]}]
			/if (${_liass.Value.Equal[ma2]}) /varset _out ${_out}:[${If[${Bool[${maCC.Find[stManualAssist2].Value}]},${num}${maCC.Find[stManualAssist2].Value}\ax,${off}]}]
			/invoke ${_liass.Advance}
		}
		/invoke ${out[12, "/cc ass ${_out}"]}
		/return TRUE

	| glyphs
	} else /if (${_type.Equal[glyph]}) {
		/if (${_verbage.Equal[group]}) {
			/invoke ${set_switch_env[${_debug}, swGlyphGroup, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[raid]}) {
			/invoke ${set_switch_env[${_debug}, swGlyphRaid, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[name]}) {
			/invoke ${do_raw_edit[FALSE, SILENT, stGlyph, "${_silent}"]}
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
			/invoke ${set_switch_env[${_debug}, swCombatMelee, TRUE, ${_silent}]}

		| enable combat: range
		} else /if (${_verbage.Equal[range]}) {
			/invoke ${set_switch_env[${_debug}, swCombatRange, TRUE, ${_silent}]}

		| enable combat: nuke
		} else /if (${_verbage.Equal[nuke]}) {
			/invoke ${set_switch_env[${_debug}, swCombatNuke, TRUE, ${_silent}]}

		| enable combat: dot
		} else /if (${_verbage.Equal[dot]}) {
			/invoke ${set_switch_env[${_debug}, swCombatDoT, TRUE, ${_silent}]}

		| enable combat: Heal
		} else /if (${_verbage.Equal[heal]}) {
			/if (!${Select[${Me.Class.ShortName},CLR,SHM,DRU]}) {
				/return FALSE
			}
			/invoke ${set_switch_env[${_debug}, swCombatHeal, TRUE, ${_silent}]}

		| enable combat: Heal
		} else /if (${_verbage.Equal[cc]}) {
			/if (!${Select[${Me.Class.ShortName},ENC,BRD]}) {
				/return FALSE
			}
			/invoke ${set_switch_env[${_debug}, swCombatControl, TRUE, ${_silent}]}

		}

		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[range,swCombatRange]}
		/invoke ${maControl.Add[dot,swCombatDoT]}
		/invoke ${maControl.Add[melee,swCombatMelee]}
		/invoke ${maControl.Add[nuke,swCombatNuke]}

		/if (${Select[${Me.Class.ShortName},CLR,SHM,DRU]}) {
			/invoke ${maControl.Add[heal,swCombatHeal]}
		}
		/if (${Select[${Me.Class.ShortName},ENC,BRD]}) {
			/invoke ${maControl.Add[cc,swCombatControl]}
		}

		/invoke ${set_control_output[${_debug}, maCC, "cc mode", maControl]}
		/return TRUE

	| set melee things to do
	} else /if (${_type.Equal[melee]}) {

		/if (${_verbage.Equal[bash]} && ${lsMeleeSkill.Contains[bash]}) {
			/invoke ${set_switch_env[${_debug}, swBash, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[disarm]} && ${lsMeleeSkill.Contains[disarm]}) {
			/invoke ${set_switch_env[${_debug}, swDisarm, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[backstab]} && ${lsMeleeSkill.Contains[backstab]}) {
			/invoke ${set_switch_env[${_debug}, swBackstab, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[dragonpunch]} && ${lsMeleeSkill.Contains[dragonpunch]}) {
			/invoke ${set_switch_env[${_debug}, swDragonpunch, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[eaglestrike]} && ${lsMeleeSkill.Contains[eaglestrike]}) {
			/invoke ${set_switch_env[${_debug}, swEaglestrike, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[tigerclaw]} && ${lsMeleeSkill.Contains[tigerclaw]}) {
			/invoke ${set_switch_env[${_debug}, swTigerclaw, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[kick]} && ${lsMeleeSkill.Contains[kick]}) {
			/invoke ${set_switch_env[${_debug}, swKick, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[flyingkick]} && ${lsMeleeSkill.Contains[flyingkick]}) {
			/invoke ${set_switch_env[${_debug}, swFlyingkick, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[roundkick]} && ${lsMeleeSkill.Contains[roundkick]}) {
			/invoke ${set_switch_env[${_debug}, swRoundkick, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[taunt]} && ${lsMeleeSkill.Contains[taunt]}) {
			/invoke ${set_switch_env[${_debug}, swTaunt, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[frenzy]} && ${lsMeleeSkill.Contains[frenzy]}) {
			/invoke ${set_switch_env[${_debug}, swFrenzy, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[intimidation]} && ${lsMeleeSkill.Contains[intimidation]}) {
			/invoke ${set_switch_env[${_debug}, swIntimidation, TRUE, ${_silent}]}

		}

		/declare _limelee listiterator local
		/vardata _limelee lsMeleeSkill.First.Clone
		/while (!${_limelee.IsEnd}) {
			/varset _out ${_out} ${If[${_limelee.Value.Equal[${lsMeleeSkill.First}]},,${dot}]} ${info}${_limelee.Value}\ax
			/if (${_limelee.Value.Equal[backstab]}) /varset _out ${_out}:${If[${Skill[backstab].Auto},\apauto\ax,${If[${maCC.Find[swBackstab].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[bash]}) /varset _out ${_out}:${If[${Skill[bash].Auto},\apauto\ax,${If[${maCC.Find[swBash].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[disarm]}) /varset _out ${_out}:${If[${Skill[disarm].Auto},\apauto\ax,${If[${maCC.Find[swDisarm].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[dragonpunch]}) /varset _out ${_out}:${If[${Skill[dragon punch].Auto},\apauto\ax,${If[${maCC.Find[swDragonpunch].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[eaglestrike]}) /varset _out ${_out}:${If[${Skill[eagle strike].Auto},\apauto\ax,${If[${maCC.Find[swEaglestrike].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[flyingkick]}) /varset _out ${_out}:${If[${Skill[flying kick].Auto},\apauto\ax,${If[${maCC.Find[swFlyingkick].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[frenzy]}) /varset _out ${_out}:${If[${Skill[frenzy].Auto},\apauto\ax,${If[${maCC.Find[swFrenzy].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[intimidation]}) /varset _out ${_out}:${If[${Skill[intimidation].Auto},\apauto\ax,${If[${maCC.Find[swIntimidation].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[kick]}) /varset _out ${_out}:${If[${Skill[kick].Auto},\apauto\ax,${If[${maCC.Find[swKick].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[roundkick]}) /varset _out ${_out}:${If[${Skill[round kick].Auto},\apauto\ax,${If[${maCC.Find[swRoundkick].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[taunt]}) /varset _out ${_out}:${If[${Skill[taunt].Auto},\apauto\ax,${If[${maCC.Find[swTaunt].Value},${on},${off}]}]}
			/if (${_limelee.Value.Equal[tigerclaw]}) /varset _out ${_out}:${If[${Skill[tiger claw].Auto},\apauto\ax,${If[${maCC.Find[swTigerclaw].Value},${on},${off}]}]}
			/invoke ${_limelee.Advance}
		}
		/invoke ${out[12, "/cc melee ${_out}"]}
		/return TRUE

	} else /if (${_type.Equal[ma1]}) {
		/if (${Bool[${_verbage}]} || ${_verbage.Equal[FALSE]}) {
			/invoke ${do_raw_edit[FALSE, SILENT, stManualAssist1, "${_verbage}"]}
		} else {
			/invoke ${do_raw_edit[FALSE, SILENT, stManualAssist1, FALSE]}
		}


	} else /if (${_type.Equal[ma2]}) {
		/if (${Bool[${_verbage}]} || ${_verbage.Equal[FALSE]}) {
			/invoke ${do_raw_edit[FALSE, SILENT, stManualAssist2, "${_verbage}"]}
		} else {
			/invoke ${do_raw_edit[FALSE, SILENT, stManualAssist2, FALSE]}
		}

	| set pull mode
	} else /if (${_type.Equal[pull]}) {
		| turn pull on or off
		/if (${_verbage.Equal[active]}) {
			/invoke ${set_switch_env[FALSE, swPull, TRUE, ${_silent}]}

		| pull radius
		} else /if (${_verbage.Equal[rad]}) {
			/invoke ${do_raw_edit[FALSE, SILENT, stPullRadius, "${_silent}"]}

		| nav stop distance
		} else /if (${_verbage.Equal[navstop]}) {
			/if (!${set_control_num_range[${_debug}, stPullNavStopDistance, "${_silent}", 31, 250]}) /return

		| enable/disable setting home marker when pulling
		} else /if (${_verbage.Equal[sethome]}) {
			/invoke ${set_switch_env[FALSE, swPullSetHome, TRUE, ${_silent}]}

		| base pulling
		} else /if (${_verbage.Equal[base]}) {
			/call do_raw_edit ${_debug} SILENT stPullMode "base"

		| intervention pulling
		} else /if (${_verbage.Equal[int]}) {
			/if (${Select[${Me.Class.ShortName},CLR,SHM]}) {
				/invoke ${get_shade[FALSE, MQWindow]}
				/return FALSE
			}
			/call do_raw_edit ${_debug} SILENT stPullMode "int"

		| pet pulling
		} else /if (${_verbage.Equal[pet]}) {
			/if (!${Select[${Me.Class.ShortName},MAG,ENC,DRU,SHM,SHD,NEC,BST]}) {
				/invoke ${get_shade[FALSE, MQWindow]}
				/return TRUE
			}
			/call do_raw_edit ${_debug} SILENT stPullMode "pet"
		| nav pulling
		} else /if (${_verbage.Equal[nav]}) {
			/call do_raw_edit ${_debug} SILENT stPullMode "nav"
		| decision pulling
		} else /if (${_verbage.Equal[multi]}) {
			/call do_raw_edit ${_debug} SILENT stPullMode "multi"
		| calm pulling
		} else /if (${_verbage.Equal[calm]}) {
			/call do_raw_edit ${_debug} SILENT stPullMode "calm"
		}
		/declare _liP listiterator local
		/declare _outP string local
		/varset _outP \awactive\ax:${If[${maCC.Find[swPull].Value},${on},${off}]} ${dot}
		/varset _outP ${_outP} \awrad\ax:${If[${maCC.Find[stPullRadius].Value},${num}${maCC.Find[stPullRadius].Value}\ax,${r0}]} ${dot}
		/varset _outP ${_outP} \awsethome\ax:${If[${maCC.Find[swPullSetHome].Value},${on},${off}]} ${dot}
		/varset _outP ${_outP} \awnavstop\ax:${num}${maCC.Find[stPullNavStopDistance].Value}\ax ${dot}

		/vardata _liP lsPullTypes.First.Clone
		/while (!${_liP.IsEnd}) {
			/varset _outP ${_outP} ${If[${_liP.Value.Equal[${lsPullTypes.First}]},,${dot}]} ${If[${_liP.Value.Equal[${maCC.Find[stPullMode].Value}]},${num}${_liP.Value}\ax,${info}${_liP.Value}\ax]}
			/invoke ${_liP.Advance}
		}
		/invoke ${out[12, "/cc pull ${_outP}"]}
		/return TRUE

	| mob engage %
	} else /if (${_type.Equal[engage]}) {
		/if (!${set_control_num_range[${_debug}, stPctEngage, "${_verbage}", 0, 100]}) /return

	| set Z radius
	} else /if (${_type.Equal[zradius]}) {
		/if (!${set_control_num_range[${_debug}, stZRadius, "${_verbage}", 0, 300]}) /return

	| use sash or not
	| } else /if (${_type.Equal[bandolier]}) {
	| 	/invoke ${set_switch_env[${_debug}, swBandolier, TRUE, ${_verbage}]}

	| use alliance
	} else /if (${_type.Equal[alliance]}) {
		/invoke ${set_switch_env[${_debug}, swAlliance, TRUE, ${_verbage}]}

	| agro
	} else /if (${_type.Equal[agro]}) {
		/invoke ${set_switch_env[${_debug}, swAgro, TRUE, ${_verbage}]}

	| force manual combat routines
	} else /if (${_type.Equal[force]}) {
		/if (${Target.ID} && (${Target.ID} != ${Me.ID}) && (${Target.ID} != ${Me.Pet.ID})) {

			/varset stAssTarID ${Target.ID}

			/if (${Me.Invis}) /makemevisible
			/invoke ${maCC.Add[swValidAssTarID,TRUE]}
			/invoke ${maCC.Add[swSetCombat,TRUE]}
			/invoke ${maCC.Add[swSetForced,TRUE]}
			/if (${swModeToken}) {
				/invoke ${out[12, "Hunting ${dot} ${tar}${Target.DisplayName}\ax"]}
			} else {
				/invoke ${out[12, "/cc force ${dot} ${tar}${Target.DisplayName}\ax"]}
			}
			/if (${maMinion.Find[swonForce].Value}) {
				/pet attack
			}
		}
		/return TRUE

	| shortcut to broadcast opverride engage pct to force instant engage
	} else /if (${_type.Equal[kill]}) {

		/declare _count int local 0
		/declare _toon string local FALSE

		/if (${_type.Equal[all]}) {
			/declare _group string local all
		} else {
			/declare _group string local ${set_dnet_group[FALSE]}
		}

	| /if (!${Bool[${_verbage}]} && ${DanNet.PeerCount[${_group}]} > 1) {




	| /if (!${Bool[${_verbage}]} && ${DanNet.PeerCount[${_group}]} > 1) {
	| 	/invoke ${out[21, "${broadcast}${sep}\a-w/cc kill\ax"]}
	| 	/dgexecute ${_group} /here TRUE
	| 	/return
	| }



		/for _count 1 to ${DanNet.PeerCount[${_group}]}
			/varset _toon ${DanNet.Peers[${_group}].Arg[${_count},|]}
			/if (${_toon.Equal[${Me.DisplayName}]}) /continue
			/if (!${Spawn[pc ${_toon}].ID}) /continue

			/if (${_verbage.Equal[off]}) {
				/dexecute ${_toon} /invoke ${maOver.Add[swOverPctEngage,FALSE]}
			} else /if (${_verbage.Equal[on]}) {
				/dexecute ${_toon} /invoke ${maOver.Add[swOverPctEngage,TRUE]}
			}
		/next _count

	}

	/if (${_silent.Equal[SILENT]}) /return TRUE

	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[ass,submenu]}
	/invoke ${maControl.Add[mode,submenu]}
	/invoke ${maControl.Add[pull,submenu]}
	/invoke ${maControl.Add[melee,submenu]}
	/invoke ${maControl.Add[glyph,submenu]}
	| /invoke ${maControl.Add[proc,submenu]}
	/invoke ${maControl.Add[engage,stPctEngage]}
	/invoke ${maControl.Add[zradius,stZRadius]}
	/invoke ${maControl.Add[agro,swAgro]}
	| /invoke ${maControl.Add[bandolier,swBandolier]}
	| /invoke ${maControl.Add[poison,swPoison]}
	/invoke ${maControl.Add[alliance,swAlliance]}
	/invoke ${set_control_output[${_debug}, maCC, cc, maControl]}

/return TRUE



|***
 * note: help text
 * use:
 ***|
#Bind command_aux /aux
sub Bind_command_aux(string _type, bool _debug)

	| /varset _debug TRUE
	/declare _lsout list local
	/invoke ${_lsout.Append[alto,beg,buff,build,burn,cc,crew,condition,cursor,debuff,debug,edit,enter]}
	/invoke ${_lsout.Append[entropy,env,grab,grain,heal,home,invis,mode,minion,navto,off,on,override,repeat,see,splash,take,tag,tc,tie]}
	/invoke ${_lsout.Append[coh,farm,ttl,defense,dot,nuke,clickitem,agro,miscdps,song,line,area,sash,pred]}

	/if (${_lsout.Contains[${_type}]}) {
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM help WHERE help_cat='${_type}' ORDER BY ROWID
		DEBUG dummy SELECT * FROM help WHERE help_cat='${_type}' ORDER BY help_var
		/declare _count int local 0
		/mqclear
		/echo
		/invoke ${out[0, "\aw/aux \ax\a-w${_type.Lower}\ax"]}
		/for _count 1 to ${sqlite.Rows[dummy]}
			/invoke ${out[0, "${If[${sqlite.Result[dummy ${_count} help_var].Find[${_type}]},,${sqlite.Result[dummy ${_count} help_var]}${sep}]}\a-g${Parse[2,${sqlite.Result[dummy ${_count} value]}]}\ax"]}
		/next _count
		/invoke ${out[18, "${eol}"]}
		/return TRUE
	}

	/echo
	/sqlite ${maData.Find[DBstatic].Value} dummy SELECT value FROM online WHERE env_var='discord'
	/invoke ${out[0, "Discord${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}
	/invoke ${out[0, "Available Control Information${sep}"]}
	/declare _out string local
	/declare _liaux listiterator local
	/vardata _liaux _lsout.First.Clone
	/while (!${_liaux.IsEnd}) {
		/varset _out ${_out} ${If[${_liaux.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${_liaux.Value}\ax
		/invoke ${_liaux.Advance}
	}
	/invoke ${out[0, "/aux ${_out}"]}
	/return TRUE

/return TRUE



|***
 * note: swap items. cause fuck MQ2Exchange
 * use: /swap "Item Name" slot [DEBUG] || /call Bind_command_swap "Item Name" slot [DEBUG]
 ***|
#Bind command_swap /swap
sub Bind_command_swap(string _item, int _toslot, bool _debug)
	DEBUG \atBind_command_swap\ax(\aw${_item}, ${_toslot}\ax)

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
		/invoke ${out[0, "Cannot Find${sep}\a-w${_item}\ax"]}
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
		/nomodkey /shiftkey /itemnotify ${Int[${Math.Calc[${_toslot}]}]} leftmouseup
	}

	/delay 1s ${Cursor.ID}

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
sub Bind_command_alto(string _debug)

	/if (${_debug.Equal[aux]}) {
		/call Bind_command_aux alto
		/return TRUE
	}

	/call set_stop ${maDebug.Find[stop].Value}
	/invoke ${target[clear]}

/return TRUE



|***
 * note: navigates to the name/target/locyxz
 * use: /navto [NAME|TARGET|locyxz]
 ***|
#bind command_navto /navto
sub Bind_command_navto(string _verbage, string _loc, bool _debug)
	DEBUG \atBind_command_navto\ax(\a-w${_verbage}, "${_loc}"\ax)

	/if (!${Target.ID} && !${Bool[${_verbage}]}) {
		OUT ${notice}${sep}need a name, target or locxyz.
		/return FALSE
	}

	/declare _tmpHomeMarker bool local FALSE
	/declare _tmpPullMarker bool local FALSE

	/if (${maHome.Find[swHomeMarker].Value}) {
		/varset _tmpHomeMarker TRUE
		/call Bind_control_home clear FALSE silent
	}

	/if (${maCC.Find[swPull].Value}) {
		/varset _tmpPullMarker TRUE
		/invoke ${maCC.Add[swPull,FALSE]}
	}

	| location given
	/if (${_verbage.Equal[loc]}) {
		DEBUG navto${sep}\a-wloc\ax
		/squelch /target clear
		/delay 1
		/invoke ${out[22, "/navto \a-w${_loc}\ax"]}
		/nav locyxz ${_loc} |dist=${maHome.Find[stHomeVariance].Value} log=off

	| nav to current target
	} else /if (${Target.ID} && !${Bool[${_verbage}]} && ${_verbage.NotEqual[loc]}) {
		DEBUG navto${sep}\a-wtarget\ax
		/invoke ${out[22, "/navto \a-w${Target.DisplayName}\ax"]}
		/nav ID ${Target.ID} |dist=${maHome.Find[stHomeVariance].Value} log=off

	| nav to closest spawn matching name
	}	else /if (!${Target.ID} && ${Bool[${_verbage}]} && ${_verbage.NotEqual[loc]}) {
		/if (!${Spawn[${_verbage}].ID}) {
			/invoke ${out[22, "not in zone${sep}\a-w${_verbage}\ax"]}
			/return FALSE
		}
		DEBUG navto${sep}\a-wspawn\ax
		/invoke ${out[22, "/navto \a-w${Spawn[${_verbage}].DisplayName}\ax"]}
		/nav ID ${Spawn[${_verbage}].ID} |dist=${maHome.Find[stHomeVariance].Value} log=off

	}

	/delay 5 ${Me.Moving}

	DEBUG ${sep}ok we're moving. right this way now..
	/while (${Me.Moving}) {
		GETINPUT
		/delay 2
	}

	| reset home and pull if they were being used
	/if (${_tmpHomeMarker}) {
		/call Bind_control_home set
	}
	/if (${_tmpPullMarker}) {
		/invoke ${maCC.Add[swPull,TRUE]}
	}

/return TRUE



|***
 * note: have everyone get your target and repeate what you say.
 * use: /repeat "things to say"
 ***|
#bind command_repeat /repeat
sub Bind_command_repeat(string _verbage, string _sender, int _tmpID, bool _debug)
	DEBUG \atBind_command_repeat\ax(\a-w${_verbage}, ${_sender}, ${_tmpID}\ax)

	/if (${_verbage.Equal[aux]}) {
		/call Bind_command_aux repeat
		/return TRUE
	}

	/makemevisible
	/invoke ${out[21, "${broadcast}${sep}\a-w/repeate ${_verbage}\ax"]}
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
		/if (${Bool[${Spawn[pc ${_sender} radius 60].ID}]}) {
			/invoke ${set_Target[${_tmpID}]}
			/if (${Target.ID} == ${_tmpID}) {
				/delay ${Int[${Math.Calc[${Math.Rand[20]}+10]}]}
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

	/if (!${check_class[TRUE, |PAL|CLR]}) {
		/return FALSE
	}

	| paladin
	/if (${Me.Class.ShortName.Equal[PAL]}) {
		DEBUG ${sep}paladin splash
		/call cast_cycle_heal ${_debug} 0 healsplash
	}

	| cleric
	/if (${Me.Class.ShortName.Equal[CLR]}) {
		DEBUG ${sep}cleric splash
		/call cast_cycle_heal ${_debug} ${Me.ID} healsplash
	}

/return TRUE



|***
 * note: clicks a door or lever to open or zone
 * use: /enter
 ***|
#bind command_enter /enter
sub Bind_command_enter(string _type, bool _debug)
	DEBUG \atBind_command_enter\ax()

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux enter
		/return TRUE
	}

	/if (${_type.Equal[all]}) {
		/declare _group string local all
	} else {
		/declare _group string local ${set_dnet_group[FALSE]}
	}

	/if (!${Bool[${_type}]} && ${DanNet.PeerCount[${_group}]} > 1) {
		/dgexecute ${_group} /enter TRUE
		/invoke ${out[21, "${broadcast}${sep}\a-w/enter\ax (\a-r${_group}\ax)"]}
	}

	/invoke ${set_data_timer[FALSE, Try_Door, RESTART]}

	:retrydoortarget
	/if (!${timer_Try_Door}) /return
	/squelch /doortarget
	/delay 1
	/if (!${Switch.ID}) /goto :retrydoortarget

	/if (!${timer_Try_Door}) /return

	/if (!${Switch.Open}) {
		/squelch /click left door
		/delay 1s
	}

/return TRUE



|***
 * note: stack invis
 * use: /invis
 ***|
#bind command_invis /invis
sub Bind_command_invis(string _type, bool _debug)

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux invis
		/return TRUE
	}

	/if (${Me.Invis}) {
		/return TRUE
	}

	/if (${_type.Equal[all]}) {
		/declare _group string local ${maComm.Find[stEntropyGroup_all].Value}
	} else {
		/declare _group string local ${set_dnet_group[FALSE]}
	}

	/if (!${Bool[${_type}]} || ${_type.Equal[all]}) {
		DEBUG echo here
		/if (${DanNet.PeerCount[${_group}]} > 1) {
			/dgexecute ${_group} /invis TRUE
			/invoke ${out[21, "${broadcast}${sep}\a-w/invis\ax (\a-r${_group}\ax)"]}
		}
	}

	| bards can blow me
	/if (${Me.AltAbilityReady[Shauri's Sonorous Clouding]} && ${Select[${Me.Class.ShortName},BRD]}) {
		/call pulse_stop
		/call voodoo FALSE "Shauri's Sonorous Clouding" 0 FALSE

	| Invis vs Normies  casters
	} else /if (${Me.AltAbilityReady[Group Perfected Invisibility]} && ${Select[${Me.Class.ShortName},WIZ,MAG,ENC]}) {
		/call voodoo FALSE "Group Perfected Invisibility" 0 FALSE

	| Invis Vs undead casters
	} else /if (${Me.AltAbilityReady[Group Perfected Invisibility to Undead]} && ${Select[${Me.Class.ShortName},CLR,SHD,NEC,PAL]}) {
		/delay 3
		/call voodoo FALSE "Group Perfected Invisibility to Undead" 0 FALSE

	| shaman
	} else /if (${Me.AltAbilityReady[Group Silent Presence]} && ${Select[${Me.Class.ShortName},SHM]}) {
		/delay 5
		/call voodoo FALSE "Group Silent Presence" ${Me.ID} FALSE

	| rangers and druids
	}	else /if (${Me.AltAbilityReady[Shared Camouflage]} && ${Select[${Me.Class.ShortName},RNG,DRU]}) {
		/delay 15
		/call voodoo FALSE "Shared Camouflage" 0 FALSE
	}

/return TRUE



|***
 * note: displays the port and builds the command to the porting class
 * use: /port
 ***|
#bind command_port /port
sub Bind_command_port(string _type, string _nick1, string _nick2, bool _debug)
	DEBUG \atBind_command_port\ax(\a-w${_type}, ${_nick1}, ${_nick2}\ax)

	/declare _count int local 0
	/declare _portName string local FALSE
	/declare _list list local


	/if (${_type.Equal[see]}) {

		/if (${Select[${_nick1},wiz,dru,guild,item,vendor,aa]}) {
			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='${_nick1}' ${If[${Bool[${_nick1}]},AND nick1='${_nick2}',]}

			/echo \awList\ax${sep}\a-w${sqlite.Rows[dummy]}\ax
			/for _count 1 to ${sqlite.Rows[dummy]}
				OUT ${dot}\a-w${sqlite.Result[dummy ${_count} nick1]} ${If[${Bool[${sqlite.Result[dummy ${_count} nick2]}]},${dot}${dot} ${sqlite.Result[dummy ${_count} nick2]},]}\ax
			/next _count
			/invoke ${out[18, "${eol}"]}
			/return
		}
		/return
	} else /if (${_type.Equal[aux]}) {


		/return
	}






	| /if (${Select[${_type},wiz,dru,guild,item,vendor,aa]}) {
	| 	SDEBUG SELECT * FROM port WHERE type='${_type}' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',AND nick2='FALSE']} LIMIT 1
	| 	/sqlite ${maData.Find[DBstatic].Value} portreturn SELECT * FROM port WHERE type='${_type}' AND nick1='${_nick1}' ${If[${Bool[${_nick2}]},AND nick2='${_nick2}',AND nick2='FALSE']} LIMIT 1
	| 	/varset _portName ${sqlite.Result[portreturn 1 name1]}
	| }

	| wizard ports
	/if (${Select[${_type},wiz]}) {

	| druid ports
	} else /if (${Select[${_type},dru]}) {

	| guild placable ports
	} else /if (${Select[${_type},guild]}) {
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='guild'
		/for _count 1 to ${sqlite.Rows[dummy]}
			/invoke ${_list.Append[${sqlite.Result[dummy ${_count} nick1]}]}
		/next _count

		/see ${_list}

	| item ports
	} else /if (${Select[${_type},item]}) {
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='item'
		/for _count 1 to ${sqlite.Rows[dummy]}
			/invoke ${_list.Append[${sqlite.Result[dummy ${_count} nick1]}]}
		/next _count

		| TBL port lamp
		/if (${_nick1.Equal[lamp]}) {
			/if (!${FindItem[Wishing Lamp:].ID}) {
				OUT ${notice}${sep}no wishing lamp. sorry, not sorry..
				/return FALSE
			}
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
			/if (!${FindItem[Zueria Slide:].ID}) {
				OUT ${notice}${sep}no slide. sorry, not sorry..
				/return FALSE
			}

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
			DEBUG list
			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='item' AND nick1="${_nick1}"
			/varset _portName ${sqlite.Result[dummy 1 name1]}

		} else {
			/invoke ${out[0, "not a valid item port"]}
		}

		/if (${Bool[${_portName}]}) {
			/makemevisible
			/if (${is_ready[FALSE, "${_portName}"]}) {
				/call voodoo FALSE "${_portName}" 0 FALSE
			}
		}


	| vendor ports
	} else /if (${Select[${_type},vendor]}) {




	| aa ports
	} else /if (${Select[${_type},aa]}) {
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='aa'
		/for _count 1 to ${sqlite.Rows[dummy]}
			/invoke ${_list.Append[${sqlite.Result[dummy ${_count} nick1]}]}
		/next _count

		/if (${_list.Contains[${_nick1}]}) {
			DEBUG ${sep}AA
			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='aa' AND nick1="${_nick1}"
			/varset _portName ${sqlite.Result[dummy 1 name1]}

		} else {
			/invoke ${out[0, "not a valid AA port"]}
		}


	}





/return


			/if (${_myDiety.Equal[Bertoxxulous]}) /varset _tmpItem Faithstone of Decay
			/if (${_myDiety.Equal[Brell Serilis]}) /varset _tmpItem Faithstone of Underfoot
			/if (${_myDiety.Equal[Bristlebane]}) /varset _tmpItem Faithstone of Mischief
			/if (${_myDiety.Equal[Cazic-Thule]}) /varset _tmpItem Faithstone of Fear
			/if (${_myDiety.Equal[Innoruuk]} && ${_myRace.Equal[Human]}) /varset _tmpItem Faithstone of Spite
			/if (${_myDiety.Equal[Innoruuk]} && ${_myRace.Equal[Dark Elf]}) /varset _tmpItem Faithstone of Hate
			/if (${_myDiety.Equal[Karana]}) /varset _tmpItem Faithstone of Thunder
			/if (${_myDiety.Equal[Mithaniel Marr]}) /varset _tmpItem Faithstone of Courage
			/if (${_myDiety.Equal[Prexus]}) /varset _tmpItem Faithstone of the Learned
			/if (${_myDiety.Equal[Rallos Zek]}) /varset _tmpItem Spiritstone of the Feerott
			/if (${_myDiety.Equal[Rodcet Nife]}) /varset _tmpItem Faithstone of Life
			/if (${_myDiety.Equal[The Tribunal]}) /varset _tmpItem Faithstone of Deep Musing
			/if (${_myDiety.Equal[Tunaree]}) /varset _tmpItem Faithstone of Nature
			/if (${_myDiety.Equal[Veeshan]}) /varset _tmpItem Faithstone of the Dedicated











|***
 * note: turns everyone off, get your tricks somewhere else...
 * use: /off
 ***|
#bind command_off /command_off
sub Bind_command_off(string _type, bool _debug)

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux off
		/return TRUE
	}

	DEBUG \atBind_command_off\ax()

	/declare _count int local 0
	/declare _toon string local FALSE

	/if (${_type.Equal[all]}) {
		/declare _group string local all
	} else {
		/declare _group string local ${set_dnet_group[FALSE]}
	}

	/for _count 1 to ${DanNet.PeerCount[${_group}]}
		/varset _toon ${DanNet.Peers[${_group}].Arg[${_count},|]}
		/if (${_toon.Equal[${Me.DisplayName}]}) /continue
		/if (!${Spawn[pc ${_toon}].ID}) /continue
		/dexecute ${_toon} /env auto off
		/dexecute ${_toon} /squelch /target clear
	/next _count

/return TRUE



|***
 * note: turns everyone on.. who doesn't like a little foreplay
 * use: /on
 ***|
#bind command_on /command_on
sub Bind_command_on(string _type, bool _debug)

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux on
		/return TRUE
	}

	DEBUG \atBind_command_on\ax()

	/declare _count int local 0
	/declare _toon string local FALSE

	/if (${_type.Equal[all]}) {
		/declare _group string local all
	} else {
		/declare _group string local ${set_dnet_group[FALSE]}
	}

	/for _count 1 to ${DanNet.PeerCount[${_group}]}
		/varset _toon ${DanNet.Peers[${_group}].Arg[${_count},|]}
		/if (${_toon.Equal[${Me.DisplayName}]}) /continue
		/if (!${Spawn[pc ${_toon}].ID}) /continue
		/dexecute ${_toon} /env auto on
	/next _count

/return TRUE



|***
 * note: there is shit everywhere....
 * use: /cls
 ***|
#bind command_cls /cls
sub Bind_command_cls(bool _debug, string _sender)

	/if (!${Bool[${_sender}]}) {
		/dgexecute /cls FALSE ${Me.DisplayName}
	}

	/hidecorpse NPC
	/hidecorpse LOOTED

/return TRUE



|***
 * note: fuck, what did the bard do this time?
 * use: /here [all]
 ***|
#bind command_here /here
sub Bind_command_here(string _type)

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux here
		/return TRUE
	}

	/if (${_type.Equal[all]}) {
		/declare _group string local all
	} else {
		/declare _group string local ${set_dnet_group[FALSE]}
	}

	/if (!${Bool[${_type}]} && ${DanNet.PeerCount[${_group}]} > 1) {
		/invoke ${out[21, "${broadcast}${sep}\a-w/here\ax (\a-r${_group}\ax)"]}
		/dgexecute ${_group} /here TRUE
		/return
	}

	ISMEDEAD

	| disable Tie
	/call Bind_command_tie off ${maDebug.Find[tie].Value}
	| sort movements
	/if (${Stick.Active}) /squelch /stick off
	| stop mq2nav
	/if (${Navigation.Active}) /squelch /nav stop |log=off

	| enable auto
	/invoke ${set_switch_env[FALSE, swAuto, FALSE, TRUE]}

	/makemevisible
	/stopcast

	| reduce pull range if in a dense area
	/if (${maCC.Find[swPull].Value}  && ${SpawnCount[NPC radius PULLENV PS4 targetable]} >= AE#) {
		/if (PULLENV > 60) {
			/call Bind_control_cc pull rad 60
		}
	}

	| set home?
	/if (${maHome.Find[swonAuto].Value}) {
		/call Bind_control_home set
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
	/if (${Target.ID}) /squelch /target clear

	/if (PET && ${Pet.ID} && ${Me.Pet.Target.ID}) /pet back off

/return TRUE



|***
 * note: clicks yes on a window option for some form of travel
 * use: /take [debug] <SENDER>
 ***|
#bind command_take /take
sub Bind_command_take(string _type, string _debug)

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux take
		/return
	}

	/if (${_type.Equal[all]}) {
		/declare _group string local all
	} else {
		/declare _group string local ${set_dnet_group[FALSE]}
	}

	/if (!${Bool[${_type}]} && ${DanNet.PeerCount[${_group}]} > 1) {
		/invoke ${out[21, "${broadcast}${sep}\a-w/take\ax (\a-r${_group}\ax)"]}
		/dgexecute ${_group} /take ${Me.DisplayName} FALSE
	}

	| take ports
	/if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find[${Spawn[${Me}].Guild}]} || ${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["Guild Hall"]}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
	/if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["be translocated by"]}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
	/if (${Window[LargeDialogWindow].Child[LDW_TextBox].Text.Find["Would you like to travel"]}) /nomodkey /notify LargeDialogWindow LDW_YesButton leftmouseup

	| take taskadds/missions/HA
	/if (${Window[LargeDialogWindow].Child[LDW_TextBox].Text.Find["You have a heroic adventure"]}) /nomodkey /notify LargeDialogWindow LDW_YesButton leftmouseup
	/if (${Window[LargeDialogWindow].Child[LDW_TextBox].Text.Find["Would you like to take on the challenge"]}) /nomodkey /notify LargeDialogWindow LDW_YesButton leftmouseup
	/if (${Window[ConfirmationDialogBox].Child[CD_TextOutPut].Text.Find["Join the expedition"]}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
	/if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["has asked you to join the shared task"]}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup

	| accept highlighted task
	/if (${Window[TaskSelectWnd]}) /nomodkey /notify TaskSelectWnd TSEL_AcceptButton leftmouseup


	| rez
	/if (${Window[ConfirmationDialogbox].Child[CD_TextOutPut].Text.Find["Blessing of Resurrection"]}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup

	| trade window
	/if (${Window[Tradewnd]}) /nomodkey /notify Tradewnd TRDW_Trade_Button leftmouseup

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

	/if (${_name.Equal[aux]}) {
		/call Bind_command_aux grab
		/return TRUE
	}
	| typed something wrong
	/if (${_name.Equal[NULL]} || !${Bool[${_name}]} || ${_name.Equal[info]} ) {
		OUT /grab ${info}NAME\ax of mapfilter spawn name you want to pick up.
		/return
	}

	| trash pick up.
	/if (${Ground[${_name}].Distance3D} > ${maHard.Find[stRadGrab].Value}) {
		/if (${_silent.NotEqual[silent]}) {
			OUT ${info}${_name}\ax is ${Ground[${_name}].Distance3D}. Needs to be within${sep}\a-w${maHard.Find[stRadGrab].Value}\ax
		}
		/return
	} else /if (${Ground[${_name}].Distance3D} <= ${maHard.Find[stRadGrab].Value}) {
		/invoke ${Ground[${_name}].Grab}
	}

/return TRUE



|***
 * note: lesson of the devoted
 * use: /lesson
 ***|
#bind command_lesson /lesson
sub Bind_command_lesson(bool _debug)

  /if (${is_ready[${_debug}, "Lesson of the Devoted"]}) /return FALSE

  /call voodoo FALSE "Lesson of the Devoted" 0 FALSE

/return TRUE



|***
 * note: Request Call of the Hero [aa|group]
 * use: /coh [group]
 ***|
#bind command_coh /coh
sub Bind_command_coh(string _verbage, bool _debug)
	DEBUG Bind_command_coh(${_verbage})

	/if (${_verbage.Equal[group]}) {
		/dex ${get_resident_class[FALSE, MAG, 99999]} /call_of_the_hero "${Me.DisplayName}" "${_verbage}"
		/return
	} else {
		/dex ${get_resident_class[FALSE, MAG, 99999]} /call_of_the_hero "${Me.DisplayName}"
		/return
	}

/return TRUE



|***
 * note: allows for custom builds
 * use: /build [save|load] [nickname] [DEBUG]
 ***|
#bind control_build /build
sub Bind_control_build(string _build, string _nickname, string _verbage, bool _debug)

	/declare _count int local 0

	| output control map
	/if (${_build.Equal[aux]}) {
		/call Bind_command_aux build ${_nickname}
		/return

	} else /if (!${Bool[${_build}]} || !${Bool[${_nickname}]}) {
		OUT Usage /build [\a-wsave\ax|\a-wload\ax|\a-wset\ax] "\a-wnick name\ax"
		/return
	}

	| set file name
	/declare _fn string local build_${Me.Class.ShortName.Lower}_${_nickname}.ini

	| set fully qualified MQ name
	/declare _fqn string local exspes\\data\\${_fn}

	| set stuff to not export/import
	/declare _skipMap list local
	/invoke ${_skipMap.Append[maDebug,maTimer,Gems,maCrew,maMode,maDeez,maPREGEN,maRezControl,maWatchspawn]}
	/invoke ${_skipMap.Append[maBuffRequest,maHook,maData,maManaRecovery,maDeez,maTTL]}
	/declare _skipKey list local
	/invoke ${_skipKey.Append[swBuffMaster,swIsMeDead,swSetCombat,swValidAssTarID,swSnapStuck,swHomeMarker,verbosity,swZoneSafe,swAuto]}
	/invoke ${_skipKey.Append[stCurrentZoneID,stCastReturn,build,swSetTie,swBurnConditionMet]}

	| obfuscation
	/invoke ${_skipKey.Append[lsProtectInsideRaid,lsProtectOutsideGroup,stTieToon,lsRetortToon,lsWardToon,stManualAssist1,stManualAssist2]}
	/invoke ${_skipKey.Append[lsGrowthToon]}

	| export to INI routine
	/if (${_build.Equal[save]}) {
		OUT /build save${sep}\a-w${_fn}\ax
		OUT \awSaving Maps\ax${sep}

		/ini "${_fqn}" "Header" "Macro" "Entropy"
		/ini "${_fqn}" "Header" "Build" "${maEnv.Find[build].Value}"
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

			/echo \at...\ax\a-w${_limaps.Value}\ax
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

					| OUTPUT TO THE INI
					/if (${_limaps.Value.Equal[maCondition]}) {
						/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_condition ORDER BY ROWID
						/for _count 1 to ${sqlite.Rows[dummy]}
							/ini "${_fqn}" ${_limaps.Value} "${sqlite.Result[dummy ${_count} env_var]}" "${Parse[2,${sqlite.Result[dummy ${_count} value]}]}"
						/next _count
					} else {
						/ini "${_fqn}" ${_limaps.Value} "${_mi.Key}" "${Parse[1,${_mi.Value}]}"
					}
					/invoke ${_mi.Advance}
				}
			/invoke ${_limaps.Advance}
		}

		/if (${Me.NumGems} && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
			OUT \awSaving Gems\ax${sep}\a-w${Me.NumGems}\ax
			/for _count 1 to ${Me.NumGems}
				/ini "${_fqn}" Gems "Gem${_count}" "${Spell[${Me.Gem[${_count}]}].BaseName}"
			/next _count
		}

		OUT \awUpdating\ax${sep}\a-wMD5\ax
		/squelch /exec powershell "(get-content ${MacroQuest.Path}\macros\exspes\data\${_fn}).Replace('MD5',(Get-FileHash ${MacroQuest.Path}\macros\exspes\data\${_fn} -Algorithm MD5).Hash) | Set-Content ${MacroQuest.Path}\macros\exspes\data\${_fn}" bg
		/invoke ${out[18, "${eol}"]}
		/echo
		/return TRUE

	| import INI routine
	} else /if (${_build.Equal[load]}) {
		/declare _section string local
		/declare _key string local
		/declare _value string local
		/declare _countS int local 0
		/declare _countK int local 0

		OUT /build load${sep}\a-w${_fn}\ax
		OUT \awLoading Maps\ax${sep}

		/for _countS 1 to ${Ini[${_fqn}].Count[|]}
			/varset _section ${Ini[${_fqn}].Arg[${_countS},|]}
			/if (${_skipMap.Contains[${_section}]}) /continue
			/if (${Select[${_section},Header,NULL]}) /continue
			/echo \at...\ax\a-w${_section}\ax

			/if (${_section.Equal[maCondition]}) {
				/for _countK 1 to ${Ini[${_fqn},${_section}].Count[|]}
					/varset _key ${Ini[${_fqn},${_section} ].Arg[${_countK},|]}
					/if (${Select[${_key},NULL]}) /continue
						/varset _value ${Ini[${_fqn},${_section},${Parse[1,${_key}]},noparse]}
						DEBUG ${sep}dummy UPDATE environment_condition SET value="${Parse[1,${_value}]}" WHERE env_var="${_key}"
						/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE environment_condition SET value="${Parse[1,${_value}]}" WHERE env_var="${_key}"
					/next _countK
			} else {
				/for _countK 1 to ${Ini[${_fqn},${_section}].Count[|]}
					/varset _key ${Ini[${_fqn},${_section} ].Arg[${_countK},|]}
					/if (${Select[${_key},NULL]}) /continue
					DEBUG ${_key}=${Ini[${_fqn},${_section},${Parse[1,${_key}]},noparse]}
					/call do_raw_edit FALSE SILENT ${_key} "${Ini[${_fqn},${_section},${Parse[1,${_key}]},noparse]}" OVERRIDE
				/next _countK
			}

		/next _countS

		| load spells
		/if (${Me.NumGems} && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
			OUT \awLoading Gems\ax${sep}\a-w${Me.NumGems}\ax
			/if (!${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoOpen}
			/for _count 1 to ${Me.NumGems}
				/call set_spell_to ${_debug} TRUE "${Spell[${Ini[${_fqn},Gems,Gem${_count}]}].RankName}" ${_count} FALSE
			/next _count
			/if (${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoClose}
		}

		/invoke ${out[18, "${eol}"]}
		/echo
		/return TRUE


	} else /if (${_build.Equal[set]}) {
		DEBUG ${sep}set${sep}\a-w${_nickname}\ax

		/declare _loadtimestart int local ${MacroQuest.Running}

		/declare _lsset list local
		/invoke ${_lsset.Delimiter["|"]}
		/invoke ${_lsset.Append[${Ini[${maData.Find[INIset].Value},${_nickname}]}]}
		/declare _liset listiterator local
		/vardata _liset _lsset.First.Clone
		| OUT setting${sep}\a-w${_nickname}\ax
		| /call do_raw_edit ${_debug} SILENT stBuildSetCurrent ${_nickname}
		/invoke ${do_raw_edit[ ${_debug}, SILENT, stBuildSetCurrent, "${_nickname}"]}
		/while (!${_liset.IsEnd}) {
			/if (${Bool[${_liset.Value}]}) {
				/if (${lsBuildSet.Contains[${_liset.Value}]}) {
					/if (${_liset.Value.Equal[primary]}) {
						/call Bind_command_swap "${Ini[${maData.Find[INIset].Value},${_nickname},${_liset.Value}]}" 13 FALSE
					} else /if (${_liset.Value.Equal[secondary]}) {
						/call Bind_command_swap "${Ini[${maData.Find[INIset].Value},${_nickname},${_liset.Value}]}" 14 FALSE
					} else /if (${_liset.Value.Equal[range]}) {
						/call Bind_command_swap "${Ini[${maData.Find[INIset].Value},${_nickname},${_liset.Value}]}" 11 FALSE
					} else /if (${_liset.Value.Equal[spellset]}) {
						/squelch /memspellset ${Ini[${maData.Find[INIset].Value},${_nickname},${_liset.Value}]}
					}



				} else {
					/call do_raw_edit ${_debug} SILENT ${_liset.Value} ${Ini[${maData.Find[INIset].Value},${_nickname},${_liset.Value}]}
				}
			}
			/invoke ${_liset.Advance}
		}
		OUT setting${sep}\a-w${_nickname}\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_loadtimestart}) / 10) / 60]}s\ax
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

	| output control map
	/if (${_type.Equal[see]}) {
		/if (!${Cursor.ID} && !${Bool[${_verbage}]}) {
			/invoke ${get_shade[FALSE, MQWindow]}
		} else {
			/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}"
			/if (!${sqlite.Rows[lootquery]}) {
				/invoke ${out[0, "DB settings for item${sep}\a-w${Cursor.Name}\ax"]}
				/invoke ${out[0, "${notice}${sep}\a-wnot in loot database\ax"]}
				/invoke ${out[18, "${eol}"]}
				/return FALSE
			}
			/invoke ${out[0, "DB settings for item${sep}\a-w${sqlite.Result[lootquery 1 item]}\ax"]}
			/invoke ${out[0, "action${sep}\a-w${sqlite.Result[lootquery 1 action]}\ax"]}
			/invoke ${out[0, "pass${sep}\a-w${sqlite.Result[lootquery 1 pass]}\ax"]}
			/invoke ${out[0, "count${sep}\a-w${sqlite.Result[lootquery 1 count]}\ax"]}
			/invoke ${out[18, "${eol}"]}
		}
		/return TRUE

	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux cursor ${_verbage}
		/return TRUE
	}


	| see if item is in the DB first?
	/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}"
	/if (${sqlite.Status[lootquery].NotEqual[Success]} || !${sqlite.Rows[lootquery]}) {
		/if (${FindItem[=${Cursor.Name}].NoDrop} || ${FindItem[=${Cursor.Name}].Lore} || ${FindItemBank[${Cursor.Name}].Lore}) {
			/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action)  VALUES("${Cursor.Name}", "leave")
		} else {
			/sqlite ${maData.Find[DBloot].Value} lootquery INSERT INTO loot_control(item,action) VALUES("${Cursor.Name}", "keep")
		}
	}

	/if (${Select[${_type},destroy,leave,keep,ignore]}) {
		/declare _item string local

		/if (${Cursor.ID}) {
			/varset _item ${Cursor.Name}
		} else /if (!${Cursor.ID} && ${Bool[${_verbage}]}) {
			/varset _item ${_verbage}
		} else /if (!${Cursor.ID} && !${Bool[${_verbage}]}) {
			/invoke ${get_shade[FALSE, MQWindow]}
		}

		/sqlite ${maData.Find[DBloot].Value} lootquery UPDATE loot_control SET action="${_type.Lower}" WHERE item="${_item}"
		DEBUG ${sep}lootquery UPDATE loot_control SET action="${_type.Lower}" WHERE item="${_item}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
		OUT /cursor ${_item}${sep}\a-w${_type.Lower}\ax
		/return TRUE

	} else /if (${_type.Equal[pass]}) {
		/sqlite ${maData.Find[DBloot].Value} lootquery UPDATE loot_control SET pass="${_verbage}" WHERE item="${Cursor.Name}"
		DEBUG ${sep}lootquery UPDATE loot_control SET pass="${_verbage}" WHERE item="${Cursor.Name}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
		OUT /cursor pass${sep}\a-w${_verbage}\ax
		/return TRUE
	} else /if (${_type.Equal[count]}) {


	}

	/declare _out string local
	/declare _lsout list local
	/invoke ${_lsout.Append[destroy,ignore,keep,leave,pass,count]}
  /varset _out
  /declare _licur listiterator local
	/vardata _licur _lsout.First.Clone
	/while (!${_licur.IsEnd}) {
		/varset _out ${_out} ${If[${_licur.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${_licur.Value}\ax
		/invoke ${_licur.Advance}
	}
	/invoke ${out[12, "/cursor ${_out}"]}

/return TRUE



|***
 * note: control for minion (pets)
 * use: /minion []
 ***|
#bind control_minion /minion
sub Bind_control_minion(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_minion\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /declare _out string local
	/declare _lsout list local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maMinion ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux minion ${_verbage}
		/return

	| force a minion build/check
	} else /if (${_type.Equal[build]}) {
		/call check_minion_cycle ${maDebug.Find[petbuild].Value} TRUE
		/return

	} else /if (${_type.Equal[engage]}) {
		/if (!${set_control_num_range[${_debug}, stPctPetEngage, "${_verbage}", 0, 99]}) /return

	| spell for pet
	} else /if (${_type.Equal[type]}) {
		/invoke ${do_raw_edit[FALSE, SILENT, stPetType, "${_verbage}"]}

	| illusion for pet
	} else /if (${_type.Equal[illusion]}) {
		/invoke ${do_raw_edit[FALSE, SILENT, stPetIllusion, "${_verbage}"]}

	| enable pet use
	} else /if (${_type.Equal[pet]}) {
		/invoke ${set_switch_env[${_debug}, swPet, TRUE, ${_verbage}]}

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
		} else /if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swUsePetGear, TRUE, ${_silent}]}
		}

		OUT /minion gear${sep}
		OUT ${dot} ${info}use\ax:${If[${maMinion.Find[swUsePetGear].Value},${on},${off}]}
		OUT ${dot} ${info}weapon\ax:[${If[${Bool[${maMinion.Find[stPetWeapon].Value}]},${num}${maMinion.Find[stPetWeapon].Value}\ax,${off}]}]
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
		} else /if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swPetShrink, TRUE, ${_silent}]}
		}

		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[use,swPetShrink]}
		/invoke ${maControl.Add[item,stPetShrink|br]}
		/invoke ${set_control_output[${_debug}, maMinion, "minion shrink", maControl]}
		/return

	}

	/if (${_silent.Equal[SILENT]}) /return

	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[type,stPetType|br]}
	/invoke ${maControl.Add[illusion,stPetIllusion|br]}
	/invoke ${maControl.Add[pet,swPet]}
	/invoke ${maControl.Add[buff,swPetBuff]}
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
 * note: shortcut to force sub voodoo in a manual instance
 * use: /entcast [NAME] [TARGET] DEBUG
 ***|
#bind command_entcast /entcast
sub Bind_command_entcast(string _name, string _target, bool _debug)
	DEBUG \atBind_command_entcast\ax(\a-w${_name}, ${_target}\ax)

	/call voodoo ${_debug} "${_name}" ${_target} FALSE

/return TRUE



|***
 * note: builds your group
 * use: /crew [1-12|kill]
 *
 *	/grouproles set [NAME] [#]
 *	 1=MainTank
 *	 2=MainAssist						${maCrew.Find[stCrew${_crew}Assist].Value}
 *	 3=Puller
 *	 4=MarkNpc
 *	 5=MasterLooter					${maCrew.Find[stCrew${_crew}Looter].Value}
 *	/makeleader [NAME] .. 	${maCrew.Find[stCrew${_crew}Leader].Value}
 ***|
#bind command_crew /crew
sub Bind_command_crew(string _type, bool _debug)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maCrew
		/return TRUE
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux crew
		/return TRUE
	}

	| kill the crew
	/if (${stCurrentCrew.NotEqual[empty]} && ${_type.Equal[kill]}) {
		/squelch /dgaexecute ${stCurrentCrew} /disband
		/delay 2
		/dgaexecute ${stCurrentCrew} /dleave ${stCurrentCrew}
		/delay 2
		/dgaexecute ${stCurrentCrew} /varset stCurrentCrew EMPTY
		/return TRUE
	}


	| bail if no crew type listed
	/if (!${_type.Length} || !${Range.Between[1,12:${Int[${_type}]}]}) {
		/invoke ${out[12, "/crew [\a-w1\ax-\a-w12\ax]"]}
		/return FALSE
	}

	| crew # empty?
	/if (${lsCrew${_type}.Count} == 1 && ${lsCrew${_type}.First.Value.Equal[EMPTY]}) {
		/invoke ${out[12, "${notice}${sep}lsCrew\a-w${_type}\ax is empty"]}
		/return FALSE
	}

	/invoke ${out[12, "\agForming\ax${sep}Crew\aw${_type}\ax"]}
	/declare _licrew listiterator local
	/vardata _licrew lsCrew${_type}.First.Clone
	/while (!${_licrew.IsEnd}) {

		| skip me
		/if (${_licrew.Value.Equal[${Me.DisplayName}]}) {
			/djoin crew${_type}
			/varset stCurrentCrew crew${_type}
			/invoke ${_licrew.Advance}
			/continue
		}

		| are they part of our comm group?
		/if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${_licrew.Value}]}) {

			| do they exist?
			/dquery ${_licrew.Value} -q "Me.Height" -o DNQReturn -t 5

			/if (!${Bool[${DNQReturn}]}) {
				/invoke ${_licrew.Advance}
				/continue
			}
			/if (!${Group.Leader.Name.Equal[${Me.DisplayName}]}) {
				/disband
				/delay 1s
				/raiddisband
				/delay 1s
			}
			| drop what they are doing!
			/dexecute ${_licrew.Value} /disband
			/delay 1s
			/dexecute ${_licrew.Value} /raiddisband
			/delay 1s

			| are they in the group already?
			/if (${Group.Member[${_licrew.Value}].Index}) {
				/dexecute ${_licrew.Value} /varset stCurrentCrew crew${_type}
				/invoke ${_licrew.Advance}
				/continue
			}

			| have the  invitee clear their target
			/dexecute ${_licrew.Value} /squelch /target clear

			| send invite
			/invoke ${out[12, "Inviting${sep}\a-w${_licrew.Value}\ax"]}

			| invite the member
			/invite ${_licrew.Value}

			| execute invite accept command
			/dexecute ${_licrew.Value} /invite

			| join needed crew channel
			/dexecute ${_licrew.Value} /djoin crew${_type}

			| set crew marker
			/dexecute ${_licrew.Value} /varset stCurrentCrew crew${_type}
		}

		/invoke ${_licrew.Advance}
	}

	| lag control
	/delay 1s

	| dont try to make changes if you're not the boss
	/if (${Group.Leader.Name.NotEqual[${Me.DisplayName}]}) {
		/goto :notmyresponsibilitybitch
	}

	| set group looter
	/delay 1s
	/if (${Bool[${maCrew.Find[stCrew${_type}Looter].Value}]} && ${Group.Member[${maCrew.Find[stCrew${_type}Looter].Value}].Index}) {
		/grouproles set ${maCrew.Find[stCrew${_type}Looter].Value} 5
	}

	| set group assist
	/delay 1s
	DEBUG ${sep}${maCrew.Find[stCrew${_type}Assist].Value}
	DEBUG ${sep}${Bool[${maCrew.Find[stCrew${_type}Assist].Value}]} && (${Group.Member[${maCrew.Find[stCrew${_type}Assist].Value}].Index} || ${Group.Member[${maCrew.Find[stCrew${_type}Assist].Value}].Index} == 0)
	/if (${Bool[${maCrew.Find[stCrew${_type}Assist].Value}]} && (${Group.Member[${maCrew.Find[stCrew${_type}Assist].Value}].Index} || ${Group.Member[${maCrew.Find[stCrew${_type}Assist].Value}].Index} == 0)) {
		/grouproles set ${maCrew.Find[stCrew${_type}Assist].Value} 2
	}

	| set group leader
	/delay 1s
	DEBUG ${sep}${Bool[${maCrew.Find[stCrew${_type}Leader].Value}]} && ${Group.Member[${Group.Leader}].Index} != ${Group.Member[${maCrew.Find[stCrew${_type}Leader].Value}].Index}
	/if (${Bool[${maCrew.Find[stCrew${_type}Leader].Value}]} && ${Group.Member[${Group.Leader}].Index} != ${Group.Member[${maCrew.Find[stCrew${_type}Leader].Value}].Index}) {
		/makeleader ${maCrew.Find[stCrew${_type}Leader].Value}
	}
	/delay 1s
	:notmyresponsibilitybitch
	/invoke ${out[12, "\awLeader\ax${sep}\a-w${Group.Leader}\ax ${dot} \awMA\ax${sep}\a-w${Group.MainAssist}\ax ${dot} \awLooter\ax${sep}\a-w${Group.MasterLooter}\ax"]}
	/invoke ${out[12, "\agCompleted\ax${sep}Crew\aw${_type}\ax"]}

/return TRUE



|***
 * note: lets you initiate a single advanced pull routine
 * use: /pull.. i mean wtf do you want?
 ***|
#bind command_pull /pull
sub Bind_command_pull(string _type, int _tmpID, bool _debug)
	DEBUG \atBind_advancedPulling\ax(\a-w${_type}, ${_tmpID}\ax)

	| get us a target
	/if (${_tmpID}) {
		/varset stAssTarID ${_tmpID}
	} else /if (${Target.ID}) {
		/varset stAssTarID ${Target.ID}
	}	else {
		/call pull_mob_find FALSE
		/if (TARGET) {
			/invoke ${out[9, "No Targets in${sep}\a-wPULLENV\ax"]}
			/return
		}
	}

	DEBUG ${sep}stAssTarID${sep}\a-wTARGET\ax
	DEBUG ${sep}Bind_advancedPulling${sep}\a-w${_type}\ax

	| intervention pulling
	/if (${_type.Equal[int]}) {
		/call get_name_multipull FALSE |CLR|SHM
		/if (${Bool[${Macro.Return}]}) {
			/dex ${Macro.Return} /advancedpullint ${Me.ID}
		} else {
			/invoke ${out[9, "NO Intervention pullers in range."]}
		}

	| pet pulling
	} else /if (${_type.Equal[pet]}) {
		/call get_name_multipull FALSE |MAG|BST|NEC|ENC|SHM|DRU
		/if (${Bool[${Macro.Return}]}) {
			/dex ${Macro.Return} /advancedpullnpet TARGET
		} else {
			/invoke ${out[9, "NO pet pullers in range."]}
		}

	| nav pulling
	} else /if (${_type.Equal[nav]}) {
		/call get_name_multipull FALSE |tank|melee
		/if (${Bool[${Macro.Return}]}) {
			/dex ${Macro.Return} /advancedpullnav TARGET
		} else {
			/invoke ${out[9, "NO melee nav pullers in range."]}
		}

	| lul pulling
	} else /if (${_type.Equal[lul]}) {
		| to be built still
	}

/return TRUE



|***
 * note: forces an intervention spell on that target
 * use: /advancedpullint [TargetID] DEBUG
 ***|
#bind command_pull_int /advancedpullint
sub Bind_command_pull_int(int _tmpID, bool _debug)
	DEBUG \atBind_command_pull_int\ax(\a-w${_tmpID}\ax)

	/if (!AUTO) /return FALSE

	/if (!${Bool[${_tmpID}]}) {
		/invoke ${out[9, "${notice}${sep}target required"]}
		/return FALSE
	}

	/call cast_list ${_debug} "lsHealIntervention" ${_tmpID}

/return FALSE



|***
 * nav: forces a nav pull on that target
 * use: /advancedpullnav [targetID] DEBUG
 ***|
#bind command_pull_nav /advancedpullnav
sub Bind_command_pull_nav(int _tmpID, bool _debug)
	DEBUG \atBind_command_pull_nav\ax(\a-w${_tmpID}\ax)

	/call get_target_navpull FALSE ${_tmpID}

/return FALSE



|***
 * note: forces a pet pull on that target
 * use: /advancedpullpet [TargetID] DEBUG
 ***|
#bind command_pull_pet /advancedpullpet
sub Bind_command_pull_pet(int _tmpID, bool _debug)
	DEBUG \atBind_command_pull_pet\ax(\a-w${_tmpID}\ax)

	/call get_target_petpull FALSE ${_tmpID}

/return FALSE



|***
 * note: control for output granulatiry
 * use: /grain #
 ***|
#bind control_grain /grain
sub Bind_control_grain(string _num, bool _debug)

	| output control map
	/if (${_num.Equal[see]}) {
		/call Bind_command_see lsGrain
		/return
	} else /if (${_num.Equal[aux]}) {
		/call Bind_command_aux grain
		/return
	}

	| remove a grain if it exists
	/if (${lsGrain.Contains[${_num}]} && ${Range.Between[1,30:${_num}]}) {
		/call do_raw_edit ${_debug} silent lsGrain ${_num} delete

	| add a grain if it oes not exist
	} else /if (!${lsGrain.Contains[${_num}]} && ${Range.Between[1,30:${_num}]}) {
		/call do_raw_edit ${_debug} silent lsGrain ${_num} add

	| idiot proofing
	} else /if (${Bool[${_num}]} && !${Range.Between[1,30:${_num}]}) {
		/invoke ${get_shade[FALSE, MQWindow]}
	}

	/declare _count int local 0
	/declare _out string local

	/for _count 1 to 30
		/varset _out ${_out} ${dot} \aw${_count}\ax:${If[${lsGrain.Contains[${_count}]},${on},${off}]}
	/next _count

	/invoke ${out[12, "/grain ${_out}"]}

/return FALSE



|***
 * note: condition editing shortcut
 * use: /condition # "code"
 ***|
#bind_noparse command_condition /condition
sub Bind_noparse_command_condition(string _type, string _verbage, bool _debug)
	DEBUG \atBind_noparse_command_condition\ax(\a-w${_type}, ${Parse[1,${_verbage}]}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maCondition
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux condition
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see condition list
		/return	TRUE
	}

	/if (${Range.Between[1,50:${Int[${_type}]}]}) {

		/if (${_verbage.Equal[clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "c${_type}" "FALSE"

		} else /if (${Bool[${_verbage}]}) {

			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "c${_type}" "${Parse[1, ${_verbage}]}"
			DEBUG ${sep}do_raw_edit ${maDebug.Find[edit].Value} SILENT c${_type} "${Parse[1,${_verbage}]}"
		}

		/echo
		/call Bind_command_see condition ${_type}

	}

/return TRUE



|***
 * note: clickitem editing
 * use: /clickitem
 ***|
#bind control_clickitem /clickitem
sub Bind_control_clickitem(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maItem ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux item ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see item list
		/return	TRUE

	| item number entered
	} else /if (${Range.Between[1,20:${Int[${_type}]}]}) {

		| see the item
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see item ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swItem${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awItem${_type}\ax${sep}${If[${maItem.Find[swItem${_type}].Value},${on},${off}]}${sep}\a-w${maItem.Find[stItem${_type}].Value}\ax"]}
			/return TRUE
		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swItem${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stItem${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stItem${_type}Alias" clear
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stItem${_type}" FALSE
		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stItem${_type}Con" "${_verbage2}"
		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsItem${_type}Tag" "${_verbage2}" ${_add_delete}
		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stItem${_type}" "${_verbage}"
		}

		/call Bind_command_see item ${_type}

	}

/return TRUE



|***
 * note: defense editing / control
 * use: /defense
 ***|
#bind control_defense /defense
sub Bind_control_defense(string _type, string _verbage, string _verbage2, string _add_delete, bool _debug)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maDefense
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux defense
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see defense list
		/return	TRUE

	| defense number entered
	} else /if (${Range.Between[1,20:${Int[${_type}]}]}) {

		| see the defensive #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see defense ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swDefense${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awDefense${_type}\ax${sep}${If[${maDefense.Find[swDefense${_type}].Value},${on},${off}]}${sep}\a-w${maDefense.Find[stDefense${_type}].Value}\ax"]}
			/return TRUE
		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swDefense${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDefense${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDefense${_type}Alias" clear
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDefense${_type}" FALSE
		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDefense${_type}Con" "${_verbage2}"
		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsDefense${_type}Tag" "${_verbage2}" ${_add_delete}
		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDefense${_type}" "${_verbage}"
		}

		/call Bind_command_see defense ${_type}

		/return TRUE

	| count of mobs to activate defensive
	} else /if (${_type.Equal[count]}) {
		/if (!${set_control_num_range[${_debug}, stCountDef, "${_verbage}", 0, 99]}) /return

	| pct of health to activate defensive
	} else /if (${_type.Equal[pct]}) {
		/if (!${set_control_num_range[${_debug}, stPctDef, "${_verbage}", 0, 99]}) /return

	}

	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[count,stCountDef]}
	/invoke ${maControl.Add[pct,stPctDef]}
	/invoke ${set_control_output[${_debug}, maDefense, defense, maControl]}

/return TRUE




|***
 * note: control for /ttl
 * use:
 ***|
#bind control_TTL /ttl
sub Bind_control_TTL(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_TTL\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maTTL
		/return TRUE
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux ttl
		/return TRUE

	| set the switch to use
	} else /if (${_type.Equal[logic]}) {
		/invoke ${set_switch_env[${_debug}, swTTL, TRUE, ${_verbage}]}

	| clear the TTL statiestics
	} else /if (${_type.Equal[clear]}) {
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT swTTL FROM environment_character
		/invoke ${maTTL.Clear}
		/invoke ${maTTL.Add[swTTL,${sqlite.Result[dummy 1 swTTL]}]}
		/invoke ${maTTL.Add[stTTLTimeStart,0]}
	}


	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[logic,swTTL]}
	/invoke ${set_control_output[${_debug}, maTTL, ttl, maControl]}

/return TRUE




|***
 * note:control for /nuke
 * use:
 ***|
#Bind control_nuke /nuke
sub Bind_control_nuke(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
	DEBUG \atcontrol_nuke\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maNuke ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux nuke ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see nuke list
		/return	TRUE

	| heal number entered
	} else /if (${Range.Between[1,20:${Int[${_type}]}]}) {

		| see the nuke #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see nuke ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swNuke${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awNuke${_type}\ax${sep}${If[${maNuke.Find[swNuke${_type}].Value},${on},${off}]}${sep}\a-w${maNuke.Find[stNuke${_type}].Value}\ax"]}
			/return TRUE

		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swNuke${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stNuke${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stNuke${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsNuke${_type}Tag" clear

		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stNuke${_type}Con" "${_verbage2}"

		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsNuke${_type}Tag" ${_verbage2} ${_silent}

		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stNuke${_type}" "${_verbage}"
		}

		/call Bind_command_see nuke ${_type}

		/return TRUE

	| set stop nuke %
	} else /if (${_type.Equal[pctstop]}) {
		/if (!${set_control_num_range[${_debug}, stPctStopNuke, "${_verbage}", 0, 99]}) /return

	| delay in nuking
	} else /if (${_type.Equal[delay]}) {
		/if (!${set_control_num_range[${_debug}, stNukeDelay, "${_verbage}", 0, 100]}) /return
	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[pctstop,stPctStopNuke]}
	/invoke ${maControl.Add[delay,stNukeDelay]}
	/invoke ${set_control_output[${_debug}, maNuke, nuke, maControl]}

/return	TRUE



|***
 * note:control for /dot
 * use:
 ***|
#Bind control_dot /dot
sub Bind_control_dot(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
	DEBUG \atcontrol_dor\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maDoT ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux dot ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see dot list
		/return	TRUE

	| heal number entered
	} else /if (${Range.Between[1,20:${Int[${_type}]}]}) {

		| see the nuke #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see dot ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swDoT${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awDoT${_type}\ax${sep}${If[${maDoT.Find[swDoT${_type}].Value},${on},${off}]}${sep}\a-w${maDoT.Find[stDoT${_type}].Value}\ax"]}
			/return TRUE

		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swDoT${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDoT${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDoT${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsDoT${_type}Tag" clear

		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDoT${_type}Con" "${_verbage2}"

		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsDoT${_type}Tag" ${_verbage2} ${_silent}

		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stDoT${_type}" "${_verbage}"
		}

		/call Bind_command_see dot ${_type}

		/return TRUE

	| set stop nuke %
	} else /if (${_type.Equal[pctstop]}) {
		/if (!${set_control_num_range[${_debug}, stPctStopDoT, "${_verbage}", 0, 99]}) /return

	| set dot refresh time in seconds
	} else /if (${_type.Equal[refresh]}) {
		/if (!${set_control_num_range[${_debug}, stDotRefresh, "${_verbage}", 0, 99]}) /return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[pctstop,stPctStopDoT]}
	/invoke ${maControl.Add[refresh,stDotRefresh]}
	/invoke ${set_control_output[${_debug}, maDoT, dot, maControl]}

/return	TRUE



|***
 * note:control for /agro
 * use:
 ***|
#Bind control_agro /agro
sub Bind_control_agro(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
	DEBUG \atcontrol_agro\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

	/if (ENGINE1) {
		/invoke ${out[0, "/agro is only available with engine 2 enabled."]}
		/return FALSE
	}

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maAgro ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux agro ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see agro list
		/return	TRUE

	| heal number entered
	} else /if (${Range.Between[1,20:${Int[${_type}]}]}) {

		| see the agro #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see agro ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swAgro${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awAgro${_type}\ax${sep}${If[${maAgro.Find[swAgro${_type}].Value},${on},${off}]}${sep}\a-w${maAgro.Find[stAgro${_type}].Value}\ax"]}
			/return TRUE

		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swAgro${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stAgro${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stAgro${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsAgro${_type}Tag" clear

		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stAgro${_type}Con" "${_verbage2}"

		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsAgro${_type}Tag" ${_verbage2} ${_silent}

		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stAgro${_type}" "${_verbage}"
		}

		/call Bind_command_see agro ${_type}

		/return TRUE

	}

/return	TRUE



|***
 * note:control for /miscdps
 * use:
 ***|
#Bind control_miscdps /miscdps
sub Bind_control_miscdps(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
	DEBUG \atcontrol_miscdps\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

	/if (ENGINE1) {
		/invoke ${out[0, "/miscdps is only available with engine 2 enabled."]}
		/return FALSE
	}

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maMiscDPS ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux miscdps ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see miscdps list
		/return	TRUE

	| heal number entered
	} else /if (${Range.Between[1,50:${Int[${_type}]}]}) {

		| see the miscdps #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see miscdps ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swMiscDPS${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awMiscDPS${_type}\ax${sep}${If[${maMiscDPS.Find[swMiscDPS${_type}].Value},${on},${off}]}${sep}\a-w${maMiscDPS.Find[stMiscDPS${_type}].Value}\ax"]}
			/return TRUE

		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swMiscDPS${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stMiscDPS${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stMiscDPS${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsMiscDPS${_type}Tag" clear

		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stMiscDPS${_type}Con" "${_verbage2}"

		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsMiscDPS${_type}Tag" ${_verbage2} ${_silent}

		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stMiscDPS${_type}" "${_verbage}"
		}

		/call Bind_command_see miscdps ${_type}

		/return TRUE

	}

/return	TRUE



|***
 * note:control for /miscdps
 * use:
 ***|
#Bind control_song /song
sub Bind_control_song(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
	DEBUG \atcontrol_song\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

	/if (!${Select[${Me.Class.ShortName},BRD]}) {
		/invoke ${out[0, "You are not a bard.. Might think you are.. but.."]}
		/return FALSE
	}

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maSong ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux song ${_verbage}
		/return

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see song list
		/return	TRUE

	| heal number entered
	} else /if (${Range.Between[1,50:${Int[${_type}]}]}) {

		| see the #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see song ${_type}
			/return
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swSong${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awSong${_type}\ax${sep}${If[${maSong.Find[swSong${_type}].Value},${on},${off}]}${sep}\a-w${maSong.Find[stSong${_type}].Value}\ax"]}
			/return TRUE

		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swSong${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSong${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSong${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsSong${_type}Tag" clear

		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSong${_type}Con" "${_verbage2}"

		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsSong${_type}Tag" ${_verbage2} ${_silent}

		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSong${_type}" "${_verbage}"
		}

		/call Bind_command_see song ${_type}

		/return TRUE

	}

/return	TRUE



|***
 * note: simple line adjustment command
 * use: /line [type] #|#|#|#|#|#|# and so on
 ***|
#bind command_line /line
sub Bind_command_line(string _type, string _line)

	/declare lsControl list local
	/invoke ${lsControl.Append[nuke,song,dot,heal,item,miscdps,agro,defense,burn]}

	| error control
	/if (!${lsControl.Contains[${_type}]}) {
		/if (ENGINE1) {
			/invoke ${out[0, "/line \awnuke\ax|\awdot\ax|\awsong\ax|\awheal\ax|\awdefense\ax \aw#|#|#|#|#\ax"]}
		} else /if (ENGINE2) {
			/invoke ${out[0, "/line \awnuke\ax|\awdot\ax|\awsong\ax|\awheal\ax|\awdefense\ax|\awagro\ax|\awmiscdps\ax|\awburn\ax \aw#|#|#|#|#\ax"]}
		}
		/return FALSE
	}

	/declare _type_env string local ${_type}

	| adjust to proper case
	/if (${Select[${_type},nuke,song,heal,item,miscdps,defense]}) {
		/varset _type ${_type.Left[1].Upper}${_type.Right[-1].Lower}
	} else /if (${Select[${_type},dot]}) {
		/varset _type DoT
	}

	| output control map
	/if (${_line.Equal[see]}) {
		/call Bind_command_see lsLine${_type}
		/return
	} else /if (${_line.Equal[aux]}) {
		/call Bind_command_aux line
		/return
	}

	| clear the current line data
	/invoke ${lsLine${_type}.Clear}
	/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE environment_${_type_env} SET value="EMPTY" WHERE env_var="lsLine${_type}"

	| add the new data
	/call do_raw_edit FALSE TRUE lsLine${_type} ${_line} add

/return TRUE





|***
 * note:control for /sash
 * use:
 ***|
#Bind control_sash /sash
sub Bind_control_sash(string _type, string _verbage, string _verbage2, string _silent, bool _debug)
	DEBUG \atcontrol_sash\ax(\a-w${_type}, ${_verbage}, ${_verbage2}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maSash ${_verbage}
		/return TRUE
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux sash ${_verbage}
		/return TRUE

	} else /if (${_type.Equal[list]}) {
		/call Bind_command_see sash list
		/return	TRUE

	| heal number entered
	} else /if (${Range.Between[1,12:${Int[${_type}]}]}) {

		| see the sash #
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see sash ${_type}
			/return TRUE
		}

		/if (!${Bool[${_verbage}]} && ${_verbage.NotEqual[FALSE]}) {
			/invoke ${set_switch_env[${_debug}, swSash${_type}, TRUE, ${_verbage}]}
			/invoke ${out[12, "\awSash${_type}\ax${sep}${If[${maSash.Find[swSash${_type}].Value},${on},${off}]}${sep}\a-w${maSash.Find[stSash${_type}].Value}\ax"]}
			/return TRUE

		} else /if (${Select[${_verbage},clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "swSash${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSash${_type}" FALSE
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSash${_type}Con" "0"
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsSash${_type}Tag" clear

		} else /if (${Select[${_verbage},con]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSash${_type}Con" "${_verbage2}"

		} else /if (${Select[${_verbage},tag]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "lsSash${_type}Tag" ${_verbage2} ${_silent}

		} else /if (${Bool[${_verbage}]} && !${Select[${_verbage},con,tag,clear]}) {
			/call do_raw_edit ${maDebug.Find[edit].Value} SILENT "stSash${_type}" "${_verbage}"
		}

		/call Bind_command_see sash ${_type}

		/return TRUE

	| procing weapon swaps
	} else /if (${_type.Equal[proc]}) {
		/if (${_verbage.Equal[use]}) {
			/invoke ${set_switch_env[${_debug}, swSashProc, TRUE, ${_verbage2}]}
		} else /if (${_verbage.Equal[name]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stSashProc, "${_verbage2}"]}
		}
		/if (${_silent.Equal[SILENT]}) /return
		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[use,swSashProc]}
		/invoke ${maControl.Add[name,stSashProc|br]}
		/invoke ${set_control_output[${_debug}, maSash, "sash proc", maControl]}
		/return TRUE



	} else /if (${_type.Equal[use]}) {
		/invoke ${set_switch_env[${_debug}, swSash, TRUE, ${_verbage}]}

	| shield flash
	} else /if (${_type.Equal[flash]}) {
		/if (${_verbage.Equal[pct]}) {
			/if (!${set_control_num_range[${_debug}, stPctShieldFlash, "${_verbage2}", 0, 99]}) /return
		} else /if (${_verbage.Equal[count]}) {
			/if (!${set_control_num_range[${_debug}, stCountShieldFlash, "${_verbage2}", 0, 99]}) /return
		} else /if (${_verbage.Equal[name]}) {
			/invoke ${do_raw_edit[${_debug}, SILENT, stSashShieldFlash, "${_verbage2}"]}
		}

		/if (${_silent.Equal[SILENT]}) /return
		/invoke ${maControl.Clear}
		/invoke ${maControl.Add[pct,stPctShieldFlash]}
		/invoke ${maControl.Add[count,stCountShieldFlash]}
		/invoke ${maControl.Add[name,stSashShieldFlash|br]}
		/invoke ${set_control_output[${_debug}, maSash, "sash flash", maControl]}
		/return TRUE

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[use,swSash]}
	/invoke ${maControl.Add[proc,submenu]}
	/if (${Select[${Me.Class.ShortName},PAL,WAR,SHD]}) {
		/invoke ${maControl.Add[flash,submenu]}
	}

	/invoke ${set_control_output[${_debug}, maSash, sash, maControl]}

/return	TRUE





|***
 * note: control for /pred
 * use: predictive combat
 ***|
#bind control_pred /pred
sub Bind_control_pred(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_TTL\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maPred
		/return TRUE
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux pred
		/return TRUE

	| set the switch to use
	} else /if (${_type.Equal[logic]}) {
		/invoke ${set_switch_env[${_debug}, swPred, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[prepclass]}) {
		/invoke ${set_switch_env[${_debug}, swPredPrepClass, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[prepburn]}) {
		/invoke ${set_switch_env[${_debug}, swPredPrepBurn, TRUE, ${_verbage}]}


	| clear the TTL statiestics
	} else /if (${_type.Equal[clear]}) {
		/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT swTTL FROM environment_character
		/invoke ${maPred.Add[swPredInc,FALSE]}
		/invoke ${maPred.Add[stPredCountMob,0]}
		/invoke ${maPred.Add[swPredNamed,FALSE]}

	| predictive radius
	} else /if (${_type.Equal[rad]}) {
		/if (!${set_control_num_range[${_debug}, stPredRad, "${_verbage}", 1, 80]}) /return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/invoke ${maControl.Clear}
	/invoke ${maControl.Add[logic,swPred]}
	/invoke ${maControl.Add[rad,stPredRad]}
	/invoke ${maControl.Add[prepclass,swPredPrepClass]}
	/invoke ${maControl.Add[prepburn,swPredPrepBurn]}
	/invoke ${set_control_output[${_debug}, maPred, pred, maControl]}

/return TRUE







| predictive combat
sub control_pred(bool _debug)
	DEBUG \atcontrol_pred\ax()
	FLOW control_pred (1)


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
	/if (${maPred.Find[swPredPrepClass].Value} && ${Defined[prediction_${Me.Class.ShortName}]}) {
		/call prediction_${Me.Class.ShortName} ${maDebug.Find[pred].Value}
	}

/return TRUE














${Spawn[${maPred.Find[stPredID].Value}].SpawnCount[3,npc radius ${maPred.Find[stPredRad].Value}].ID}

${Spawn[${maPred.Find[stPredID].Value}].SpawnCount[3,npc radius ${maPred.Find[stPredRad].Value}].ID}


${Spawn[10089].NearestSpawn[radius 50]}






		DEBUG ${sep}count:\aw${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}\ax radius${sep}\ay${_tmpAgroRange}\ax
		/if (${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}) {
			/for _count 1 to ${SpawnCount[npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value} targetable]}
				ISMEDEAD

				/varset _tmpID ${NearestSpawn[${If[${_spCount},${Math.Calc[${_count}+${_spCount}]},${_count}]}, npc ${_los} range ${maEnv.Find[stMobLvlMin].Value} ${maEnv.Find[stMobLvlMax].Value} radius ${_tmpAgroRange} zradius ${maCC.Find[stZRadius].Value}].ID}
				DEBUG ${sep}_tmpID${sep}\a-w${_tmpID}\ax

				/if (!${Navigation.PathExists[id ${_tmpID}]}) {
					| SDEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wNavigation.PathExists[\a-yid ${_tmpID}\ax]\ax
					| /continue
				}

				/varset _navRange ${Navigation.PathLength[id ${_tmpID}]}

				| sort by closest nav path length
				/if (${_navRange} <= ${_closestNAV}) {
					/varset _closestNAV ${_navRange}
				} else /if (${_navRange} > ${_closestNAV}) {
					/continue
				}

				| animation checks
				/if (${lsNoTargetAnimation.Contains[${Spawn[id ${_tmpID}].Animation}]}) {
					DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsNoTargetAnimation\ax
					/varset _spCount 1
					/continue
				}


				| fish are friends, not food
				/if (${lsNoAttackBodyType.Contains[${Spawn[id ${_tmpID}].Body}]}) {
					DEBUG ${sep}#:${_count}${dot}${_tmpID}${sep}\arPASS\ax on${sep}\a-wlsNoAttackBodyType\ax
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
