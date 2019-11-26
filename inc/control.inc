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
sub Bind_control_debug(string _type, bool _debug)
	DEBUG \atBind_control_debug\ax(\a-w${_type}\ax)
	
  | output control map
	/if (${_type.Equal[see]}) {
			/call Bind_command_see maDebug
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux debug
		/return		

	} else /if (${maDebug.Contains[${_type}]}) {
		/varset dump ${maDebug.Add[${_type},${If[${maDebug.Find[${_type}].Value},FALSE,TRUE]}]}
	}
  
  /declare _out string local
  /vardata mi maDebug.First.Clone
  /while (!${mi.IsEnd}) {
		/varset _out ${_out} ${If[${mi.Key.Equal[${maDebug.First}]},,${dot}]} ${If[${mi.Value},\ag${mi.Key}\ax,${info}${mi.Key}\ax]}
		/varset dump ${mi.Advance}
  }
	/varset dump ${out[12, "/debug ${_out}"]}

/return



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

	| delete aliases
	} else /if (${_type.Equal[whitewash]}) {

		/vardata cli liAlias.First.Clone
		/while (!${cli.IsEnd}) {
			/varset dump ${out[12, "Deleting Alias${sep}\a-w${cli.Value}\ax"]}
			/squelch /alias /${cli.Value} delete
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "Current Build ${info}${maEnv.Find[build].Value}\ax ${dot} ${bad}Ended\ax"]}
		/endm
		/return 

	| parse SPA data
	} else /if (${_type.Equal[spa]}) {
		
		/if (!${Bool[${_verbage}]}) {
			/varset dump ${out[12, "/admin spa \atEXACT SPELLNAME\ax"]}
			/return
		}
		
		
		/declare _spell "${_verbage}"

		/declare list string local
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
			OUT ${dot} .Trigger \a-w${Spell[${_spell}].Trigger[${_count}].Name}\ax ${dot} ID:\a-w${Spell[${_spell}].Base2[${_count}]}\ax\ax
			OUT ${dot} .Base \a-w${Spell[${_spell}].Base[${_count}]}\ax 
			OUT ${dot} .Max \a-w${Spell[${_spell}].Max[${_count}]} \ax
			OUT ${dot} .Calc \a-w${Spell[${_spell}].Calc[${_count}]} \ax
			OUT ${dot} .Restrictions \a-w${Spell[${_spell}].Restrictions[${_count}]}\ax
			OUT ${dot} .Extra \a-w${Spell[${_spell}].Extra}\ax

			/varset list ${list} ${Spell[${_spell}].Attrib[${_count}]}
		/next _count
		OUT ${dot} ${dot} ${dot} ${dot}
		OUT \awSPA list\ax ${sep} \a-w${list}\ax
		
		OUT \aw/spellslotinfo\ax
		/spellslotinfo ${Spell[${_spell}].ID}

		/return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/declare liout list local
	/varset dump ${liout.Append[checkvar,spa,whitewash]}
  /declare _out string local
	/vardata cli liout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${liout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/admin (PID:\a-w${EverQuest.PID}\ax) ${_out}"]}

/return



|***
 * note: environment control
 * use: /env
 ***|
#bind control_env /env
sub Bind_control_env(string _type, string _verbage, string _silent, bool _debug)
	DEBUG CURSUB(\a-w${_type}, ${_verbage}, ${_silent}\ax)
	
	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maEnv ${_verbage}
		/return

	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux env ${_verbage}
		/return		

	| macro auto/manual
	} else /if (${_type.Equal[auto]}) {
		/varset dump ${set_switch_env[FALSE, swAuto, FALSE, ${_verbage}]}
		/varset dump ${out[12, "/env ${If[${maEnv.Find[swAuto].Value},${good}auto\ax,${bad}auto\ax]}"]}
		
		/if (!${maEnv.Find[swAuto].Value}) {
			/if (HOME) /call Bind_control_home clear
			/call clear_combat FALSE
		} else /if (${_type.Equal[auto]}) {
			/if (${maCC.Find[swPullSetHome].Value} && PULL) {
				/call Bind_control_home set
			}
		}
		/return

	| enable wasiting fireworks for those bastards with too much AA on their hands
	} else /if (${_type.Equal[fireworks]}) {
		/varset dump ${set_switch_env[FALSE, swAAFireworks, TRUE, ${_verbage}]}
		| /if (!${swAAFireworks}) /call set_timer timer_Waste_Fireworks 1
		
	| enable bagging from mages for rods and pet gear
	} else /if (${_type.Equal[begmage]}) {
		/varset dump ${set_switch_env[FALSE, swBegMAG, TRUE, ${_verbage}]}

	| take raid invites
	} else /if (${_type.Equal[takeri]}) {
		/varset dump ${set_switch_env[FALSE, swTakeRaidInvite, TRUE, ${_verbage}]}

	| take group invites
	} else /if (${_type.Equal[takegi]}) {
		/varset dump ${set_switch_env[FALSE, swTakeGroupInvite, TRUE, ${_verbage}]}

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
			/varset dump ${set_switch_env[FALSE, swCorrectEXPAA, TRUE, ${_silent}]}
		}

		/declare _lsout list local
		/varset dump ${_lsout.Append[adjust,level,maintain]}
	  /declare _out string local
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[adjust]}) /varset _out ${_out}:${If[${maEnv.Find[swCorrectEXPAA].Value},${on},${off}]}
			/if (${cli.Value.Equal[level]}) /varset _out ${_out}:${If[${maEnv.Find[stLvlMax].Value},${num}${maEnv.Find[stLvlMax].Value}\ax,${r0}]}
			/if (${cli.Value.Equal[maintain]}) /varset _out ${_out}:${num}${maEnv.Find[stLvlPct].Value}\ax
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/env exp ${_out}"]}
	/return

	| food/drink
	} else /if (${_type.Equal[meal]}) {
		/if (${_verbage.Equal[food]}) {
			/varset dump ${do_raw_edit[FALSE, SILENT, stFood, "${_silent}"]}
		} else /if (${_verbage.Equal[drink]}) {
			/varset dump ${do_raw_edit[FALSE, SILENT, stDrink, "${_silent}"]}
		}
		OUT /env meal${sep}
		OUT ${dot} ${info}food\ax:[${If[${Bool[${maEnv.Find[stFood].Value}]},${num}${maEnv.Find[stFood].Value}\ax,${off}]}]
		OUT ${dot} ${info}drink\ax:[${If[${Bool[${maEnv.Find[stDrink].Value}]},${num}${maEnv.Find[stDrink].Value}\ax,${off}]}]
		/return

	| rese settings
	} else /if (${_type.Equal[rest]}) {

		| set rest %		
		/if (${_verbage.Equal[pct]}) {
			/if (!${set_control_num_range[${_debug}, stPctRest, "${_silent}", 0, 99]}) /return

		| rest to full
		} else /if (${_verbage.Equal[full]}) {
			/varset dump ${set_switch_env[FALSE, swRestFull, TRUE, ${_silent}]}

		| rest through combat
		} else /if (${_verbage.Equal[combat]}) {
			/varset dump ${set_switch_env[FALSE, swRestCombat, TRUE, ${_silent}]}

		| mod rods in rest
		} else /if (${_verbage.Equal[modrod]}) {
			/varset dump ${set_switch_env[FALSE, swRestModRod, TRUE, ${_silent}]}

		| check buffs in rest
		} else /if (${_verbage.Equal[buff]}) {
			/varset dump ${set_switch_env[FALSE, swRestBuff, TRUE, ${_silent}]}

		}
		/declare _lsout list local
		/varset dump ${_lsout.Append[pct,full,combat,modrod,buff]}
	  /declare _out string local
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[pct]}) /varset _out ${_out}:${If[${maEnv.Find[stPctRest].Value},${num}${maEnv.Find[stPctRest].Value}\ax,${r0}]}
			/if (${cli.Value.Equal[full]}) /varset _out ${_out}:${If[${maEnv.Find[swRestFull].Value},${on},${off}]}
			/if (${cli.Value.Equal[combat]}) /varset _out ${_out}:${If[${maEnv.Find[swRestCombat].Value},${on},${off}]}
			/if (${cli.Value.Equal[modrod]}) /varset _out ${_out}:${If[${maEnv.Find[swRestModRod].Value},${on},${off}]}
			/if (${cli.Value.Equal[buff]}) /varset _out ${_out}:${If[${maEnv.Find[swRestBuff].Value},${on},${off}]}
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/env rest ${_out}"]}
	/return

	} else /if (${_type.Equal[moblvlmin]}) {
		/varset dump ${set_switch_env[FALSE, stMobLvlMin, TRUE, ${_verbage}]}

	| set mob maximum level for attacking
	} else /if (${_type.Equal[moblvmax]}) {
		/varset dump ${set_switch_env[FALSE, stMobLvlMax, TRUE, ${_verbage}]}

	| force a nuke loop till all are casted before exiting
	} else /if (${_type.Equal[nukeloop]}) {
		/varset dump ${set_switch_env[FALSE, swNukeLoop, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[advloot]}) {
		/varset dump ${set_switch_env[FALSE, swADVLoot, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[autoinv]}) {
		/varset dump ${set_switch_env[FALSE, swAutoInv, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[tribute]}) {
		/varset dump ${set_switch_env[FALSE, swCheckTribute, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[glyphoutsideraid]}) {

	} else /if (${_type.Equal[safelistguild]}) {
		/varset dump ${set_switch_env[FALSE, swSafeListGuild, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[buffgemhold]}) {
		/varset dump ${set_switch_env[FALSE, swHoldBuffGem, TRUE, ${_verbage}]}

	| set buffgem
	} else /if (${_type.Equal[buffgem]}) {
		/if (!${lsClassCast.CountOf[${Me.Class.ShortName}]}) {
			/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE environment_character SET stBuffGem='0' WHERE env_var='stBuffGem'
			/varset dump ${get_shade[FALSE, MQWindow]}
			/return
		}
		/if (${Range.Between[1,${Me.NumGems}:${Int[${_verbage}]}]}) {
			/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE environment_character SET stBuffGem='${_verbage}' WHERE env_var='stBuffGem'
			/varset dump ${maEnv.Add[stBuffGem,${_verbage}]}
		} else {
			/varset dump ${out[12, "/env buffgem(${num}${maEnv.Find[stBuffGem].Value}\ax) range ${info}1 - ${Me.NumGems}\ax"]}
			/return
		}		

	| set fake safe radius
	} else /if (${_type.Equal[saferadius]}) {
		/if (!${set_control_num_range[${_debug}, stEnvSafeRadius, "${_verbage}", 0, 60]}) /return

	| set operation radius
	} else /if (${_type.Equal[rad]}) {
		/if (!${set_control_num_range[${_debug}, stEnvRadius, "${_verbage}", 0, 999]}) /return



	} else /if (${_type.Equal[incharge]}) {

		/declare _count int local 0
		/declare _name string local FALSE
		
		| cycle all the toons you are boxing
		/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
			/varset _name ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
			
			| if i'm issuing the command. turn off tie and move along
			/if (${_name.Equal[${Me.DisplayName}]}) {
				/varset dump ${do_raw_edit[FALSE, SILENT, stTieToon, FALSE]}

				/continue
			}
			
			| if the toon is not in this zone, move along
			/if (!${Spawn[pc ${_name}].ID}) /continue
			
			| update the toons DB setting
			/dexecute ${_name} /squelch /target clear
			/dexecute ${_name} /tc toon ${Me.DisplayName}
			
		/next _count
		/varset dump ${out[12, "Gator's bitches better be using jimmies!"]}
		/return

	}

	/if (${_silent.Equal[SILENT]}) /return
	/declare _lsout list local
	/varset dump ${_lsout.Append[auto,rad,rest,meal,exp,advloot,autoinv,buffgem,buffgemhold,nukeloop,safelistguild,moblvmax]}
	/varset dump ${_lsout.Append[moblvlmin,begmage,fireworks,takeri,takegi,saferadius,tribute]}
  /declare _out string local
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[auto]}) /varset _out ${_out}:(${If[${maEnv.Find[swAuto].Value},${on},${off}]})
		/if (${cli.Value.Equal[rad]}) /varset _out ${_out}:${If[${maEnv.Find[stEnvRadius].Value},${num}${maEnv.Find[stEnvRadius].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[advloot]}) /varset _out ${_out}:${If[${maEnv.Find[swADVLoot].Value},${on},${off}]}
		/if (${cli.Value.Equal[autoinv]}) /varset _out ${_out}:${If[${maEnv.Find[swAutoInv].Value},${on},${off}]}
		/if (${cli.Value.Equal[buffgemhold]}) /varset _out ${_out}:${If[${maEnv.Find[swHoldBuffGem].Value},${on},${off}]}
		/if (${cli.Value.Equal[buffgem]}) /varset _out ${_out}:${num}${maEnv.Find[stBuffGem].Value}\ax
		/if (${cli.Value.Equal[nukeloop]}) /varset _out ${_out}:${If[${maEnv.Find[swNukeLoop].Value},${on},${off}]}
		/if (${cli.Value.Equal[safelistguild]}) /varset _out ${_out}:${If[${maEnv.Find[swSafelistGuild].Value},${on},${off}]}
		/if (${cli.Value.Equal[rest]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[moblvlmin]}) /varset _out ${_out}:${If[${maEnv.Find[stMobLvlMin].Value},${num}${maEnv.Find[stMobLvlMin].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[moblvmax]}) /varset _out ${_out}:${If[${maEnv.Find[stMobLvlMax].Value},${num}${maEnv.Find[stMobLvlMax].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[begmage]}) /varset _out ${_out}:${If[${maEnv.Find[swBegMAG].Value},${on},${off}]}
		/if (${cli.Value.Equal[dpsmeter]}) /varset _out ${_out}:${If[${maEnv.Find[swDPSMeter].Value},${on},${off}]}
		/if (${cli.Value.Equal[exp]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[fireworks]}) /varset _out ${_out}:${If[${maEnv.Find[swAAFireworks].Value},${on},${off}]}
		/if (${cli.Value.Equal[hook]}) /varset _out ${_out}:${If[${maEnv.Find[swHookSub].Value},${on},${off}]}
		/if (${cli.Value.Equal[takegi]}) /varset _out ${_out}:${If[${maEnv.Find[swTakeGroupInvite].Value},${on},${off}]}
		/if (${cli.Value.Equal[takeri]}) /varset _out ${_out}:${If[${maEnv.Find[swTakeRaidInvite].Value},${on},${off}]}
		/if (${cli.Value.Equal[saferadius]}) /varset _out ${_out}:${If[${maEnv.Find[stEnvSafeRadius].Value},${num}${maEnv.Find[stEnvSafeRadius].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[tribute]}) /varset _out ${_out}:${If[${maEnv.Find[swCheckTribute].Value},${on},${off}]}
		/if (${cli.Value.Equal[meal]}) /varset _out ${_out}:${submenu}
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/env ${_out}"]}

/return



|***
 * note: class shortname bind conversion to /cls
 * use: 
 ***|
#bind to_chr /chr
sub Bind_to_chr(string _type, string _verbage, string _silent, bool _debug)
	/call set_control_${Me.Class.ShortName} ${_type} "${_verbage}" ${_silent} ${_debug}

/return



|***
 * note: force buff check routine
 * use: /cb
 ***|
#bind command_cb /cb
sub Bind_command_cb(string _verbage)

	/if (!${Bool[${_verbage}]}) {
		/call check_buff_cycle ${maDebug.Find[buff].Value} TRUE
	} 

/return



|***
 * note: force minion check routine
 * use: /cm
 ***|
#bind command_cm /cm
sub Bind_command_cm(string _verbage)

	/if (!${Bool[${_verbage}]}) {
		/call check_Minion_cycle ${maDebug.Find[buff].Value} TRUE
	} 

/return



|***
 * note: watches for a spawn in the zone
 * use: /watchspawn [name]
 ***|
#bind command_watchspawn /watchspawn
sub Bind_command_watchspawn(string _variable, bool _debug)
	DEBUG \atBind_command_watchspawn\ax(\a-w${_spawn}\ax)
	/return

	/if (!${_variable.Length} || ${_variable.Equal[NULL]} || ${_variable.Equal[help]}) {
		/call Bind_command_aux watchspawn
		/mapshow reset
		/return		
	} else /if (${_variable.Equal[see]}) {
		/call Bind_command_see maWatchspawn
		/return
	} else /if (${Bool[${_variable}]}) {

	} else /if (!${Bool[${_variable}]} && ${Target.ID}) {

	} 
	
	/if (${_variable.Equal[FALSE]}) /mapshow reset
	
/return



|***
 * note:control for /home
 * use: 
 ***|
#Bind control_home /home
/varset _debug true
sub Bind_control_home(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_home\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	/declare _lsout list local
  /declare _out string local
  /declare _count int local 0

	| no mesh for zone
	/if (!${Navigation.MeshLoaded}) {
		/varset dump ${get_shade[FALSE, MQWindow]}
		/varset dump ${maHome.Add[swHomeMarker,FALSE]}
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
		/varset dump ${out[12, "Sorting Campfire"]}
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
			/declare _clist string local ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}]}
		} else {
			| /sqlite ${dbCore} dummy SELECT lstCrew${_verbage} FROM environment WHERE currentBuild="${currentBuild}"
			| /declare _clist string local ${sqlite.Result[dummy 1 lstCrew${_verbage}]}	
		}

		/for _count 1 to ${_clist.Count[|]}
			/if (${_clist.Arg[${_count},|].Equal[${Me.DisplayName}]}) /continue
			/if (${Bool[${Spawn[pc ${_clist.Arg[${_count},|]}]}]}) {
				/varset dump ${out[12, "Gathering${sep}\a-w${_clist.Arg[${_count},|]}\ax"]}
				/dexecute ${_clist.Arg[${_count},|]} /navto ${Me.ID}
			}
		
		/next _count		
		/return
		
		

	} else /if (${_type.Equal[switch]}) {
		/varset dump ${maHome.Add[swSetTie,FALSE]}
		/if (!${maHome.Find[swHomeMarker].Value}) {
			/varset dump ${out[19, "Home${sep}Set ${dot} Y:${num}${Me.Y}\ax X:${num}${Me.X}\ax Z:${num}${Me.Z}\ax "]}
			/varset dump ${maHome.Add[swHomeMarker,TRUE]}
			/varset dump ${maData.Add[stCampX,${Me.X}]}
			/varset dump ${maData.Add[stCampY,${Me.Y}]}
			/varset dump ${maData.Add[stCampZ,${Me.Z}]}
			/varset dump ${set_data_timer[FALSE, Home_Check, ${maHome.Find[stHomeVariance].Value}s, temp]}
			/return
			
		} else /if (${maHome.Find[swHomeMarker].Value}) {
			/if (${maHome.Find[swHomeMarker].Value}) ${out[19, "Home${sep}Clear"]} 
			/varset dump ${maHome.Add[swHomeMarker,FALSE]}
			/varset dump ${maData.Add[stCampX,999999999]}
			/varset dump ${maData.Add[stCampY,999999999]}
			/varset dump ${maData.Add[stCampZ,999999999]}
			/varset dump ${set_data_timer[FALSE, Home_Check, FALSE, delete]}
			/return
		}

	} else /if (${_type.Equal[set]}) {
		/varset dump ${out[19, "Home${sep}Set ${dot} Y:${num}${Me.Y}\ax X:${num}${Me.X}\ax Z:${num}${Me.Z}\ax"]}
		/varset dump ${maHome.Add[swSetTie,FALSE]}
		/varset dump ${maHome.Add[swHomeMarker,TRUE]}
		/varset dump ${maData.Add[stCampX,${Me.X}]}
		/varset dump ${maData.Add[stCampY,${Me.Y}]}
		/varset dump ${maData.Add[stCampZ,${Me.Z}]}
		/varset dump ${set_data_timer[FALSE, Home_Check, ${maHome.Find[stHomeVariance].Value}s, temp]}
		/return

	} else /if (${_type.Equal[clear]}) {
		/if (${maHome.Find[swHomeMarker].Value}) ${out[19, "Home${sep}Clear"]} 
		/varset dump ${maHome.Add[swHomeMarker,FALSE]}
		/varset dump ${maData.Add[stCampX,999999999]}
		/varset dump ${maData.Add[stCampY,999999999]}
		/varset dump ${maData.Add[stCampZ,999999999]}
		/varset dump ${set_data_timer[FALSE, Home_Check, FALSE, delete]}
		/return

	} else /if (${_type.Equal[return]} && AUTO) {
		/if (${maHome.Find[stCampZ].Value} > 999999991) {
			/return FALSE
		} else /if (!${Navigation.PathExists[locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value}]}) {
			/varset dump ${out[19, "${notice}${sep}no path home"]}
			/return FALSE
		}		

		| check variance for home position
		/if (${Math.Distance[${Me.Y},${Me.X},${Me.Z}:${maData.Find[stCampY].Value},${maData.Find[stCampX].Value},${maData.Find[stCampZ].Value}]} < ${maHome.Find[stHomeVariance].Value}) {
			/return
		}
		
		SETHUD "'goin home"
		
		/delay 2
		/doevents flush
		/nav locyxz ${maData.Find[stCampY].Value} ${maData.Find[stCampX].Value} ${maData.Find[stCampZ].Value} dist=${maHome.Find[stHomeVariance].Value} log=off
		/delay 2 ${Navigation.Active}
		/while (${Navigation.Active}) {
			/delay 2	
		}
		SETHUD FALSE
		| /if (${swActivePull}) /varset swActivePull FALSE
		/return

	} else /if (${_type.Equal[arc]}) {
		/if (!${set_control_num_range[FALSE, stFaceArc, "${_verbage}", 0, 45]}) /return

	} else /if (${_type.Equal[variance]}) {
		/if (!${set_control_num_range[FALSE, stHomeVariance, "${_verbage}", 0, 15]}) /return

	} else /if (${_type.Equal[movebehind]}) {
		/varset dump ${set_switch_env[FALSE, swMoveBehind, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[facefast]}) {
		/varset dump ${set_switch_env[FALSE, swFaceFast, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[mount]}) {
		/varset dump ${do_raw_edit[FALSE, SILENT, stMount, "${_verbage}"]}

	} else /if (${_type.Equal[deathclear]}) {
		/varset dump ${set_switch_env[FALSE, swMoveDeathClear, TRUE, ${_verbage}]}

	} 

	/if (${_silent.Equal[SILENT]}) /return
	/if (${maHome.Find[swHomeMarker].Value}) {
		/varset dump ${out[12, "Home${sep}Set ${dot} Y:${num}${maData.Find[stCampY].Value}\ax X:${num}${maData.Find[stCampX].Value}\ax Z:${num}${maData.Find[stCampZ].Value}\ax"]}
	} else /if (!${maHome.Find[swHomeMarker].Value}) {
		/varset dump ${out[12, "Home${sep}Clear"]}
	}

	/varset dump ${_lsout.Append[set,variance,movebehind,facefast,arc,mount,deathclear,gather]}
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[set]}) /varset _out ${_out}:(${If[${maHome.Find[swHomeMarker].Value},${on},${off}]})
		/if (${cli.Value.Equal[variance]}) /varset _out ${_out}:${If[${maHome.Find[stHomeVariance].Value},${num}${maHome.Find[stHomeVariance].Value}\ax,${off}]}
		/if (${cli.Value.Equal[movebehind]}) /varset _out ${_out}:${If[${maHome.Find[swMoveBehind].Value},${on},${off}]}
		/if (${cli.Value.Equal[facefast]}) /varset _out ${_out}:${If[${maHome.Find[swFaceFast].Value},${on},${off}]}
		/if (${cli.Value.Equal[arc]}) /varset _out ${_out}:${If[${maHome.Find[stFaceArc].Value},${num}${maHome.Find[stFaceArc].Value}\ax,${off}]}
		/if (${cli.Value.Equal[mount]}) /varset _out ${_out}:[${If[${Bool[${maHome.Find[stMount].Value}]},${num}${maHome.Find[stMount].Value}\ax,${off}]}]
		/if (${cli.Value.Equal[deathclear]}) /varset _out ${_out}:${If[${maHome.Find[swMoveDeathClear].Value},${on},${off}]}
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/home ${_out}"]}

/return 



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
		/varset dump ${_lsout.Append[nav,stick]}
		/if (${_lsout.Contains[${_verbage}]}) {
			/if (!${Navigation.MeshLoaded}) {
				/varset dump ${do_raw_edit[FALSE, SILENT, stTieMode, stick]}
			} else /if (${Navigation.MeshLoaded}) {
				/varset dump ${do_raw_edit[FALSE, SILENT, stTieMode, "${_verbage}"]}
			}
		} 
		
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${If[${cli.Value.Equal[${maTie.Find[stTieMode].Value}]},${good}${cli.Value}\ax,${info}${cli.Value}\ax]}
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/tc mode ${_out}"]}
		/return

	} else /if (${_type.Equal[toon]}) {
		/varset dump ${do_raw_edit[FALSE, SILENT, stTieToon, "${_verbage}"]}

	} else /if (${_type.Equal[variance]}) {
		/if (!${set_control_num_range[TRUE, stTieVariance, "${_verbage}", 0, 15]}) /return

	} 

	/if (${_silent.Equal[SILENT]}) /return
	/varset dump ${_lsout.Append[toon,variance,mode]}
	/vardata cli _lsout.First.Clone
	/varset _out ${_out}tie:(${If[${maTie.Find[swSetTie].Value},${on},${off}]})
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[toon]}) /varset _out ${_out}:[${If[${Bool[${maTie.Find[stTieToon].Value}]},${num}${maTie.Find[stTieToon].Value}\ax,${off}]}]
		/if (${cli.Value.Equal[variance]}) /varset _out ${_out}:${If[${maTie.Find[stTieVariance].Value},${num}${maTie.Find[stTieVariance].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[mode]}) /varset _out ${_out}:${submenu}
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/tc ${_out}"]}

/return



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
		/varset dump ${maTie.Add[swSetTie, FALSE]}
		/return FALSE
	}

	/if (${_type.Equal[on]}) {
		/if (${maTie.Find[swSetTie].Value}) /return
		/if (${Bool[${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}]}) /varset dump ${maTie.Add[swSetTie, TRUE]}
	} else /if (${_type.Equal[off]}) {
		/if (!${maTie.Find[swSetTie].Value}) /return
		/if (${Bool[${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}]}) /varset dump ${maTie.Add[swSetTie, FALSE]}
	} else {
		/if (${Bool[${Spawn[pc ${maTie.Find[stTieToon].Value}].ID}]}) /varset dump ${maTie.Add[swSetTie,${If[${maTie.Find[swSetTie].Value},FALSE,TRUE]}]}  
	}

	/varset dump ${out[12, "/tie ${maTie.Find[stTieToon].Value}${sep}${If[${maTie.Find[swSetTie].Value},${on},${off}]}"]}
	
/return TRUE



|***
 * note: control for /area
 * use: 
 ***| 
#bind control_area /area
sub Bind_control_area(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_area\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| make sure were on/in the right zone
	| /call validate_zone FALSE FALSE

	| get the zone data
	/sqlite ${maData.Find[DBzone].Value} curZoneData SELECT * FROM zone WHERE ShortName="${Zone.ShortName.Lower}"

	| output control map
	/if (${_type.Equal[map]}) {
		| /call Bind_control_admin iterate map Area
		/varset dump ${out[12, "no map yet sorry"]}

	| safezone toggle
	} else /if (${_type.Equal[Safe]}) {
		/if (${maEnv.Find[swZoneSafe].Value} || ${_verbage.Equal[FALSE]}) {
			/sqlite ${maData.Find[DBzone].Value} dummy UPDATE zone_control SET value='FALSE' WHERE zone_shortname='${Zone.ShortName}' and element='swZoneSafe'
			/varset dump ${maEnv.Add[swZoneSafe,FALSE]}
		} else /if (!${maEnv.Find[swZoneSafe].Value} || ${_verbage.Equal[TRUE]}) {
			/sqlite ${maData.Find[DBzone].Value} dummy UPDATE zone_control SET value='TRUE' WHERE zone_shortname='${Zone.ShortName}' and element='swZoneSafe'
			/varset dump ${maEnv.Add[swZoneSafe,TRUE]}
		}
	
	| remove a mob from the current zone
	} else /if (${_type.Equal[delete]}) {
		/sqlite ${maData.Find[DBzone].Value} dummy DELETE FROM zone_control WHERE zone_shortname='${Zone.ShortName}' and value='${Target.DisplayName}'
		/varset dump ${out[12, "/area delete ${dot} ${num}${Zone.ShortName}\ax${sep}\a-w${Target.DisplayName}"]}
		/call set_zone_lists FALSE
		/return

	| remaining list adjustments
	} else /if (${Select[${_type},exclude,named,ranged,hunt,nomez,nocast,nomelee,nomagic,nodisease,nofire,nopoision,nocold,nocorruption]}) {
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
			
		| Add a property to a mob
		/sqlite ${maData.Find[DBzone].Value} dummy INSERT INTO zone_control VALUES ("${Zone.ShortName}", "${_element}", "${Target.DisplayName}")
		/varset dump ${out[12, "/area ${_type} ${dot} ${num}${_element}\ax${sep}\a-w${Target.DisplayName}\ax"]}
		/call set_zone_lists FALSE
		/return
		
	}

	/if (${_silent.Equal[SILENT]}) /return
  /declare _out string local
	/vardata cli lsZoneProperty.First.Clone
	/varset _out ${info}safe\ax:${If[${maEnv.Find[swZoneSafe].Value},${on},${off}]}
	/while (!${cli.IsEnd}) {		
		/varset _out ${_out} ${If[${cli.Value.Equal[${lsZoneProperty.First}]},,${dot}]} ${info}${cli.Value.Right[-6].Lower}\ax
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/area ${_out}"]}

/return 

	
	
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

	| /varset dump ${maBuffRequest.${_operate}[${Me.Name},"${_buff}"]}
	/dgzexecute /requestbuff ${Me.Name} Add "${_buff}"

/return



|***
 * note:sends the buff beg to all toons
 * use: 
 ***|	
#Bind control_requestbuff /requestbuff
sub Bind_control_requestbuff(string _toon, string _operate, string _buff)

	/if (!${Bool[${_toon}]}) /varset _toon ${Me.Name}
	/if (!${Bool[${_operate}]}) /varset _operate Add
	/varset dump ${maBuffRequest.${_operate}[${_toon},"${_buff}"]}

/return



|***
 * note:control for /heal
 * use: 
 ***|
#Bind control_heal /heal
sub Bind_control_heal(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atcontrol_heal\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maHeal ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux heal ${_verbage}
		/return		
	
	| set heal point	
	} else /if (${_type.Equal[point]}) {
		/call set_control_heal_point FALSE ${_verbage}
		/return	

	| heal self
	} else /if (${_type.Equal[self]}) {
		/varset dump ${set_switch_env[FALSE, swHealSelf, TRUE, ${_verbage}]}

	| heal group
	} else /if (${_type.Equal[group]}) {
		/varset dump ${set_switch_env[FALSE, swHealGroup, TRUE, ${_verbage}]}

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

		/varset dump ${set_switch_env[FALSE, swHealXTarget, TRUE, ${_verbage}]}
		
		/if (!${maHeal.Find[swHealXTarget].Value}) {
			/call set_xtarget_clear
		}

	| heal pets
	} else /if (${_type.Equal[pet]}) {
		/varset dump ${set_switch_env[FALSE, swHealPet, TRUE, ${_verbage}]}

	| heal cures
	} else /if (${_type.Equal[cure]}) {
		/varset dump ${set_switch_env[FALSE, swHealCure, TRUE, ${_verbage}]}
		/if (${maHeal.Find[swHealCure].Value}) { 
			/call set_observers FALSE maCure add FALSE
		} else /if (!${maHeal.Find[swHealCure].Value}) { 
			/call set_observers FALSE maCure drop FALSE
		}			
	
	} else /if (${_type.Equal[adj]}) {
		/if (!${set_control_num_range[${_debug}, stHealAdjust, "${_verbage}", 0, 99]}) /return

	} else /if (${_type.Equal[mode]}) {
		
		/if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
			/varset dump ${get_shade[TRUE, MQWindow]}
			/return
		}
		
		/if (${lsHealMode.Contains[${_verbage}]}) {
			/call do_raw_edit ${_debug} SILENT stHealMode "${_verbage}"

		}
		/if (${_silent.Equal[SILENT]}) /return 
		/declare _outm string local
		/vardata cli lsHealMode.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _outm ${_outm} ${If[${cli.Value.Equal[${lsHealMode.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${maHeal.Find[stHealMode].Value.Equal[${cli.Value}]}) /varset _outm ${_outm}:${on}	
			/if (${maHeal.Find[stHealMode].Value.NotEqual[${cli.Value}]}) /varset _outm ${_outm}:${off}	
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/heal mode ${_outm}"]}
		/return

	| set the XTarget class to heal
	} else /if (${_type.Equal[xtclass]}) {
		/call set_control_chr_xtclass FALSE ${_verbage}
		/return	

	| set rampage tank toon
	} else /if (${_type.Equal[rampage]}) {
		/if (${Bool[${_verbage}]}) {
			/call do_raw_edit ${_debug} SILENT stRampageTank "${_verbage.Left[1].Upper}${_verbage.Right[-1].Lower}"
		} else /if (!${Bool[${_verbage}]}) {
			/varset dump ${set_switch_env[${_debug}, stRampageTank, TRUE, FALSE]}
		}
	
	}

	/if (${_silent.Equal[SILENT]}) /return 
	/declare _out string local
	/declare _lsout list 
	/varset dump ${_lsout.Append[point,mode,self,group,xt,xtclass,pet,cure,adj,rampage]}
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[point]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[mode]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[xtclass]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[self]}) /varset _out ${_out}:${If[${maHeal.Find[swHealSelf].Value},${on},${off}]}
		/if (${cli.Value.Equal[group]}) /varset _out ${_out}:${If[${maHeal.Find[swHealGroup].Value},${on},${off}]}
		/if (${cli.Value.Equal[xt]}) /varset _out ${_out}:${If[${maHeal.Find[swHealXTarget].Value},${on},${off}]}
		/if (${cli.Value.Equal[pet]}) /varset _out ${_out}:${If[${maHeal.Find[swHealPet].Value},${on},${off}]}
		/if (${cli.Value.Equal[cure]}) /varset _out ${_out}:${If[${maHeal.Find[swHealCure].Value},${on},${off}]}
		/if (${cli.Value.Equal[adj]}) /varset _out ${_out}:${If[${maHeal.Find[stHealAdjust].Value},${num}${maHeal.Find[stHealAdjust].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[rampage]}) /varset _out ${_out}:${If[${Bool[${maChr.Find[stRampageTank].Value}]},[${num}${maChr.Find[stRampageTank].Value}\ax],[${off}]]}
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/heal ${_out}"]}

/return



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
		/varset dump ${set_switch_env[FALSE, swMaintenance, TRUE, ${_verbage}]}
		/varset dump ${out[12, "/entropy ${If[${maEntropy.Find[swMaintenance].Value},${good}maintenance\ax,${bad}maintenance\ax]}"]}
		/varset dump ${out[12, "${warning} ${dot} ENDING MACRO${dot}${dot}${dot}please restart"]}
		/endmacro

	| enable the DPS meter
	} else /if (${_type.Equal[dpsmeter]}) {
		/varset dump ${set_switch_env[FALSE, swDPSMeter, TRUE, ${_verbage}]}
		
	| enable/disable using events.inc file
	} else /if (${_type.Equal[events]}) {
		/varset dump ${set_switch_env[FALSE, swEventsInc, TRUE, ${_verbage}]}

	| autoinvnetory while in manual
	} else /if (${_type.Equal[manautoinv]}) {
		/varset dump ${set_switch_env[FALSE, swAutoInvManual, TRUE, ${_verbage}]}

	| enable sub hook checking
	} else /if (${_type.Equal[hook]}) {
		/varset dump ${set_switch_env[FALSE, swHookSub, TRUE, ${_verbage}]}
		
		/if (!${maHook.Find[swHookSub].Value}) {
			/varset dump ${maHook.Clear}
		}

	} else /if (${_type.Equal[nettimeout]}) {
		/if (!${set_control_num_range[${_debug}, stNetworkTimeout, "${_verbage}", 0, 30]}) /return
		
	} else /if (${_type.Equal[buffrefresh]}) {
		/if (!${set_control_num_range[${_debug}, stBuffRefresh, "${_verbage}", 0, 36]}) /return

	| verbosity level		
	} else /if (${_type.Equal[verb]}) {
		/if (!${set_control_num_range[${_debug}, stVerb, "${_verbage}", 1, 10]}) /return


	| enable auto adding mobs to lists
	} else /if (${_type.Equal[autolist]}) {
		/varset dump ${set_switch_env[FALSE, swAutoList, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[hud]}) {

		| adjust hud clock
		/if (${_verbage.Equal[time]}) {
			/if (${maData.Find[swHUDTime].Value}) {
				/varset dump ${maData.ADd[swHUDTime,FALSE]}
				/varset dump ${out[12, "HUD Time${sep}\a-wGame\ax"]}
				
			} else /if (!${maData.Find[swHUDTime].Value}) {
				/varset dump ${maData.ADd[swHUDTime,TRUE]}
				/varset dump ${out[12, "HUD Time${sep}\a-wIRL\ax"]}
			}
			
						 

		| reset the MQhud ini file
		} else /if (${_verbage.Equal[update]}) {
			/call set_initialize_hud FALSE ${_silent}
			/loadhud ${_silent}

		| delete the hud
		} else /if (${_verbage.Equal[delete]}) {
			/ini ".\..\MQ2HUD.ini" "${_silent}" NULL NULL
			/varset dump ${out[12, "Deleting MQ2HUD.INI${sep}\a-w${_silent}\ax"]}
			
		} 
		
		/varset dump ${_lsout.Append[time,update,delete]}
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/entropy hud ${_out}"]}
		/return
		
		
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
			
			/varset dump ${out[12, "\awImporting Loot File from\ax\at...\ax\a-w${_lootini}\ax"]}
			/varset dump ${out[12, "On Large loot files this will take a min to build."]}
			
				/for _countS 0 to ${Ini[${_lootini}].Count[|]}

					/varset _section ${Ini[${_lootini}].Arg[${_countS},|]}

					/if (!${Ini[${_lootini},${_section}].Count[|]}) {
						/ini "${_lootini}" "${_section}" NULL NULL
						/continue
					}

					/varset dump ${out[12, "\at..\ax\aw${_section}"]}
	
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
							| /varcalc _loopKey ${_loopKey}+1
							/continue
						}

						| add key->value to DB and delete
						/sqlite ${maData.Find[DBloot].Value} dummy INSERT INTO loot_control VALUES ("${_key}", "${_value}", 0, 0)
						/ini "${_lootini}" "${_section}" "${_key}" NULL

					}
					/next _countS 
				
			}
			
			/varset dump ${out[18, "${eol}"]}
			/return					
			
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
				/varset dump ${out[12, "\at..\ax\aw${_section}\ax"]}
				/for _countK 1 to ${Ini[${_ini},${_section}].Count[|]}
					/varset _key ${Ini[${_ini},${_section}].Arg[${_countK},|]}
					/if (${Select[${_key},NULL]}) /continue
			
					/for _countV 1 to ${Ini[${_ini},${_section},${_key}].Count[|]}
						/varset _value ${Ini[${_ini},${_section},${_key}].Arg[${_countV},|]}
						/varset dump ${out[12, "\at...\ax\a-w${_value}\ax"]}
						/sqlite ${maData.Find[DBzone].Value} dummy INSERT OR IGNORE INTO zone_control VALUES ("${_section}", "${_key}", "${_value}")
					/next _countV
				/next _countK
			/next _countS 
			/varset dump ${out[18, "${eol}"]}
			/echo
			/return								
		}
		

	}
	
	/if (${_silent.Equal[SILENT]}) /return
	/varset dump ${_lsout.Append[maintenance,events,hud,dpsmeter,hook,nettimeout,autolist,buffrefresh,manautoinv]}
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[maintenance]}) /varset _out ${_out}:${If[${maEntropy.Find[swMaintenance].Value},${on},${off}]}
		/if (${cli.Value.Equal[events]}) /varset _out ${_out}:${If[${maEntropy.Find[swEventsInc].Value},${on},${off}]}
		/if (${cli.Value.Equal[hud]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[dpsmeter]}) /varset _out ${_out}:${If[${maEntropy.Find[swDPSMeter].Value},${on},${off}]}
		/if (${cli.Value.Equal[hook]}) /varset _out ${_out}:${If[${maEntropy.Find[swHookSub].Value},${on},${off}]}
		/if (${cli.Value.Equal[nettimeout]}) /varset _out ${_out}:${If[${maEntropy.Find[stNetworkTimeout].Value},${num}${maEntropy.Find[stNetworkTimeout].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[buffrefresh]}) /varset _out ${_out}:${num}${maEntropy.Find[stBuffRefresh].Value}s\ax
		/if (${cli.Value.Equal[autolist]}) /varset _out ${_out}:${If[${maEntropy.Find[swAutoList].Value},${on},${off}]}
		/if (${cli.Value.Equal[manautoinv]}) /varset _out ${_out}:${If[${maEntropy.Find[swAutoInvManual].Value},${on},${off}]}
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/entropy ${_out}"]}
	
/return



|***
 * note: control for rez
 * use: 
 ***|
#Bind control_rez /rez
sub Bind_control_rez(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atcontrol_rez\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

  /declare _out string local
	/declare _lsout list local

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maRez ${_verbage}
		/return

	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux rez ${_verbage}
		/return		

	| give rez
	} else /if (${_type.Equal[give]}) {
		/varset dump ${set_switch_env[FALSE, swRezGive, TRUE, ${_verbage}]}

	| take rez
	} else /if (${_type.Equal[take]}) {
		/varset dump ${set_switch_env[FALSE, swRezTake, TRUE, ${_verbage}]}

	| rez in combat
	} else /if (${_type.Equal[combat]}) {
		/varset dump ${set_switch_env[FALSE, swRezCombat, TRUE, ${_verbage}]}

	| rez everyone
	} else /if (${_type.Equal[everyone]}) {
		/varset dump ${set_switch_env[FALSE, swRezEveryone, TRUE, ${_verbage}]}

	| use rez tokens
	} else /if (${_type.Equal[token]}) {
		/varset dump ${set_switch_env[FALSE, swRezToken, TRUE, ${_verbage}]}
		
		
	| rez radius
	} else /if (${_type.Equal[rad]}) {
		/if (!${set_control_num_range[FALSE, stMaxRezRange, "${_verbage}", 0, 100]}) /return

	

	| set rez accept percent	
	} else /if (${_type.Equal[pct]}) {
		/if (${Range.Between[10,96:${Int[${_verbage}]}]}) {
			/if (${lsRezPct.Contains[${_verbage}]}) /call do_raw_edit ${_debug} SILENT stPctMinRez "${_verbage}"
		}
	
		/vardata cli lsRezPct.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${lsRezPct.First}]},,${dot}]} ${If[${maRez.Find[stPctMinRez].Value} == ${cli.Value},${good}${cli.Value}\ax,${info}${cli.Value}\ax]}  
			/varset dump ${cli.Advance}
		}
		/varset dump ${out[12, "/rez pct ${_out}"]}
		/return

	}
	
	/if (${_silent.Equal[SILENT]}) /return
	/varset dump ${_lsout.Append[give,take,combat,everyone,token,pct,rad]}
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[give]}) /varset _out ${_out}:${If[${maRez.Find[swRezGive].Value},${on},${off}]}
		/if (${cli.Value.Equal[take]}) /varset _out ${_out}:${If[${maRez.Find[swRezTake].Value},${on},${off}]}
		/if (${cli.Value.Equal[combat]}) /varset _out ${_out}:${If[${maRez.Find[swRezCombat].Value},${on},${off}]}
		/if (${cli.Value.Equal[everyone]}) /varset _out ${_out}:${If[${maRez.Find[swRezEveryone].Value},${on},${off}]}
		/if (${cli.Value.Equal[token]}) /varset _out ${_out}:${If[${maRez.Find[swRezToken].Value},${on},${off}]}
		/if (${cli.Value.Equal[pct]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[rad]}) /varset _out ${_out}:${If[${maRez.Find[stMaxRezRange].Value},${num}${maRez.Find[stMaxRezRange].Value}\ax,${r0}]}
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[12, "/rez ${_out}"]}
	
/return





|***
 * note: testing control. not for in game use...
 * use: 
 ***|
#Bind deez /deez
sub Bind_deez(string _type, bool _debug)
	/if (${_type.Equal[nutz]}) {
		/varset dump ${out[0, "dannuic said i should put this in to powerlevel people"]}
		/varset dump ${out[0, "well. i'm not going to code in powerleveling. but you can"]}
		/varset dump ${out[0, "have the command anyways"]}

	}

	| fakes a hash	
	/if (${_type.Equal[hash]}) {
		
		/declare alpha list local
		/varset dump ${alpha.Append[a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z]}
		/declare numeric list local
		/varset dump ${numeric.Append[0,1,2,3,4,5,6,7,8,9]}	
		/declare _hash string local
		/declare _list string local
		
		/declare _count int local 0
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
		
		/echo hash:\a-w${_hash}\ax
	
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

		/varset dump ${out[18, "${eol}"]} 
	}
	
/return




|***
 * note: your mama so fat, each ass cheek has a different zip code
 * use: /buff
 ***|
#Bind control_buff /buff
sub Bind_control_buff(string _type, string _verbage, string _silent, bool _debug)
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
		/varset dump ${set_switch_env[${_debug}, swBuff${_type}, TRUE, ${_verbage}]}
		/varset dump ${out[12, "\awBuff${_type}\ax${sep}${If[${maBuffs.Find[swBuff${_type}].Value},${on},${off}]}${sep}\a-w${maBuffs.Find[stBuff${_type}].Value}\ax"]}
		/return
		
	| master buff switch
	} else /if (${_type.Equal[master]}) {
		/varset dump ${set_switch_env[${_debug}, swBuffMaster, FALSE, ${_verbage}]}

	} else /if (${_type.Equal[ps]}) {
		/varset dump ${set_switch_env[FALSE, swBuffPowersource, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[self]}) {
		/varset dump ${set_switch_env[${_debug}, swBuffSelf, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[crew]}) {
		/varset dump ${set_switch_env[${_debug}, swBuffCrew, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[now]}) {
		/varset dump ${set_switch_env[${_debug}, swBuffNow, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[raid]}) {
		/varset dump ${set_switch_env[${_debug}, swBuffRaid, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[beg]}) {

		| output control map
		/if (${_verbage.Equal[see]}) {
			/call Bind_command_see maBuffRequest ${_silent}
			/return		
		}
		
		/varset dump ${set_switch_env[${_debug}, swBuffBeg, TRUE, ${_verbage}]}
	
	} else /if (${_type.Equal[unity]}) {
		/varset dump ${set_switch_env[${_debug}, swBuffUnity, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[aura]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swBuffAura, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[aura1]}) {
			/varset dump ${do_raw_edit[${_debug}, SILENT, stAura1, "${_silent}"]}
		} else /if (${_verbage.Equal[aura1check]}) {
			/varset dump ${do_raw_edit[${_debug}, SILENT, stAura1Check, "${_silent}"]}
		} else /if (${_verbage.Equal[aura2]}) {
			/varset dump ${do_raw_edit[${_debug}, SILENT, stAura2, "${_silent}"]}
		} else /if (${_verbage.Equal[aura2check]}) {
			/varset dump ${do_raw_edit[${_debug}, SILENT, stAura2Check, "${_silent}"]}
		}
		
		OUT /buff aura${sep}
		OUT ${dot} ${info}use\ax:${If[${maBuff.Find[swBuffAura].Value},${on},${off}]}

		OUT ${dot} ${info}aura1\ax:[${If[${Bool[${maBuff.Find[stAura1].Value}]},${num}${maBuff.Find[stAura1].Value}\ax,${off}]}]
		OUT ${dot} ${info}aura1check\ax:[${If[${Bool[${maBuff.Find[stAura1Check].Value}]},${num}${maBuff.Find[stAura1Check].Value}\ax,${off}]}]
		OUT ${dot} lsAura\aw1\axTag
		/vardata cli lsAura1Tag.First.Clone
		/while (!${cli.IsEnd}) {
			OUT ${dot}${dot} \a-w${cli.Value}\ax
			/varset dump ${cli.Advance}
		}	
		/if (!${Select[${Me.Class.ShortName},CLR,ENC]}) /return
		
		/echo
		OUT ${dot} ${info}aura2\ax:[${If[${Bool[${maBuff.Find[stAura2].Value}]},${num}${maBuff.Find[stAura2].Value}\ax,${off}]}]
		OUT ${dot} ${info}aura2check\ax:[${If[${Bool[${maBuff.Find[stAura2Check].Value}]},${num}${maBuff.Find[stAura2Check].Value}\ax,${off}]}]
		OUT ${dot} lsAura\aw2\axTag
		/vardata cli lsAura2Tag.First.Clone
		/while (!${cli.IsEnd}) {
			OUT ${dot}${dot} \a-w${cli.Value}\ax
			/varset dump ${cli.Advance}
		}	

		/return

	} else /if (${_type.Equal[temp]}) {
		/varset dump ${set_switch_env[${_debug}, swBuffTemp, TRUE, ${_verbage}]}

	} else /if (${_type.Equal[shrink]}) {
		/if (${_verbage.Equal[item]}) {
			/call do_raw_edit ${_debug} SILENT stShrinkItem "${_silent}"
		} else /if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swBuffShrink, TRUE, ${_silent}]}
		}
		
		/declare _lsout list local
	  /declare _out string local
		/varset dump ${_lsout.Append[use,item]}
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffShrink].Value},${on},${off}]}
			/if (${cli.Value.Equal[item]}) /varset _out ${_out}:[${If[${Bool[${maBuff.Find[stShrinkItem].Value}]},${num}${maBuff.Find[stShrinkItem].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/buff shrink ${_out}"]}
		/return

	}
	
	/if (${_silent.Equal[SILENT]}) /return
	/declare _lsout list local
	/varset dump ${_lsout.Append[master,self,crew,raid,beg,unity,shrink,aura,temp,now,ps]}
  /declare _out string local
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[master]}) /varset _out ${_out}:(${If[${maBuff.Find[swBuffMaster].Value},${on},${off}]})
		/if (${cli.Value.Equal[self]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffSelf].Value},${on},${off}]}
		/if (${cli.Value.Equal[crew]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffCrew].Value},${on},${off}]}
		/if (${cli.Value.Equal[raid]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffRaid].Value},${on},${off}]}
		/if (${cli.Value.Equal[beg]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffBeg].Value},${on},${off}]}
		/if (${cli.Value.Equal[temp]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffTemp].Value},${on},${off}]}
		/if (${cli.Value.Equal[unity]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffUnity].Value},${on},${off}]}
		/if (${cli.Value.Equal[aura]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffAura].Value},${on},${off}]} ${submenu}
		/if (${cli.Value.Equal[shrink]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[now]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffNow].Value},${on},${off}]}
		/if (${cli.Value.Equal[ps]}) /varset _out ${_out}:${If[${maBuff.Find[swBuffPowersource].Value},${on},${off}]}

		/varset dump ${cli.Advance}
	}		
	/varset dump ${out[12, "/buff ${_out}"]}

/return



|***
 * note: control for overrides
 * use: 
 ***|
#Bind control_over /over
sub Bind_control_over(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_over\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maOver ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux over ${_verbage}
		/return		
		
	} else /if (${_type.Equal[los]}) {
		/varset dump ${set_switch_env[FALSE, swOverLOS, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[engage]}) {
		/varset dump ${set_switch_env[FALSE, swOverPctEngage, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[loot]}) {
		/varset dump ${set_switch_env[FALSE, swOverLoot, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[safenames]}) {
		/varset dump ${set_switch_env[FALSE, swOverSafeNames, TRUE, ${_verbage}]}
	} else /if (${_type.Equal[trainspell]}) {
		/varset dump ${set_switch_env[FALSE, swOverTrainSpell, TRUE, ${_verbage}]}
	
	}
	
	/if (${_silent.Equal[SILENT]}) /return
	/declare _lsout list local
	/varset dump ${_lsout.Append[los,engage,loot,safenames,trainspell]}
  /declare _out string local
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[los]}) /varset _out ${_out}:${If[${maOver.Find[swOverLOS].Value},${on},${off}]}
		/if (${cli.Value.Equal[engage]}) /varset _out ${_out}:${If[${maOver.Find[swOverPctEngage].Value},${on},${off}]}
		/if (${cli.Value.Equal[loot]}) /varset _out ${_out}:${If[${maOver.Find[swOverLoot].Value},${on},${off}]}
		/if (${cli.Value.Equal[safenames]}) /varset _out ${_out}:${If[${maOver.Find[swOverSafeNames].Value},${on},${off}]}
		/if (${cli.Value.Equal[trainspell]}) /varset _out ${_out}:${If[${maOver.Find[swOverTrainSpell].Value},${on},${off}]}
		/varset dump ${cli.Advance}
	}	

	/varset dump ${out[12, "/over ${_out}"]}
	
/return	

	
	
|***
 * note: shorthand for raw db editing
 * use: mostly for breaking shit
 ***|
#bind_noparse raw_edit /edit
sub Bind_noparse_raw_edit(string _variable, string _value, string _add_delete, _verbage)

	/call do_raw_edit FALSE FALSE "${_variable}" "${Parse[1,${_value}]}" ${_add_delete} ${_verbage}
	
/return	TRUE



|***
 * note: lets see what we can see
 * use: /see [a thing]
 ***|
#bind command_see /see
sub Bind_command_see(string _variable, string _verbage, string _silent)
	/declare _debug bool local FALSE
	
	/if (!${_variable.Length} || ${_variable.Equal[NULL]} || ${_variable.Equal[aux]}) {
		/call Bind_command_aux see
		/return		
	}
	
	DEBUG \atBind_control_see\ax(\a-w${_variable}\ax)
	DEBUG .Left[2]${sep}\a-w${_variable.Left[2]}\ax

	/declare _count int local 0
	/declare _DBLOC string local
	/declare _tmpAux string local

	/if (${_variable.Equal[buff]}) {
		/if (${_verbage.Equal[list]}) {
			/echo
			/varset dump ${out[12, "\awBuff List\ax${sep}"]}
			/for _count 1 to 50
				/if (${maBuffs.Find[stBuff${_count}].Value.NotEqual[FALSE]}) {
					/varset dump ${out[12, "\awstBuff${_count}\ax${sep}${If[${maBuffs.Find[swBuff${_count}].Value},${on},${off}]}${sep}\a-w${maBuffs.Find[stBuff${_count}].Value}\ax"]}
				}
			/next _count
		/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}  
		/echo
		/return
		}
		
		/if (${_verbage.Equal[spa]}) {
			DEBUG /call Bind_control_admin spa "${maBuffs.Find[stBuff${_silent}].Value}"
			/call Bind_control_admin spa "${maBuffs.Find[stBuff${_silent}].Value}"
			
			/return
		}
		
		
		/if (!${Range.Between[1,50:${Int[${_verbage}]}]}) {
			/call Bind_command_aux buff
			/return
		}
		
		| ${Parse[2,${maCondition.Find[c${_tmpC}].Value}]}
		
		/declare _tmpC string local ${maBuffs.Find[stBuff${_verbage}].Value.Arg[2,+c]}
		/varset dump ${out[12, "\awBuff #${_verbage}\ax${sep}${If[${maBuffs.Find[swBuff${_verbage}].Value},${on},${off}]}"]}
		/varset dump ${out[12, "stBuff\aw${_verbage}\ax${sep}\a-w${maBuffs.Find[stBuff${_verbage}].Value}\ax"]}
		/if (${maBuffs.Find[stBuff${_verbage}].Value.Arg[2,+].Find[c]}) {
			OUT ${maBuffs.Find[stBuff${_verbage}].Value.Arg[2,+]}${sep}\a-w${Parse[2,${maCondition.Find[${_tmpC}].Value}]}\ax
		}

		/varset dump ${out[12, "stBuff\aw${_verbage}\axAlias${sep}\a-w${maBuffs.Find[stBuff${_verbage}Alias].Value}\ax"]}
		/varset dump ${out[12, "lsBuff\aw${_verbage}\axTag"]}

		/vardata cli lsBuff${_verbage}Tag.First.Clone
		/while (!${cli.IsEnd}) {
			/varset dump ${out[12, "${dot}\a-w${cli.Value}\ax"]}
			/varset dump ${cli.Advance}
		}
		
	} else /if (${_variable.Equal[condition]}) {
		/if (${_verbage.Equal[list]}) {
			/echo
			/varset dump ${out[12, "\awCondition List\ax${sep}"]}
			/for _count 1 to 50
				/if (${maCondition.Find[c${_count}].Value.Equal[FALSE]}) /continue
				OUT \awc${_count}\ax${sep}\a-w${Parse[2,${maCondition.Find[c${_count}].Value}]}\ax
			/next _count
			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}  
			/echo
			/return
		}
		OUT c${_verbage}${sep}\a-w${Parse[2,${maCondition.Find[c${_verbage}].Value}]}\ax

	} else /if (${_variable.Equal[item]}) {
		/if (${_verbage.Equal[list]}) {
			/echo
			/varset dump ${out[12, "\awItem List\ax${sep}"]}
			/for _count 1 to 20
				/if (${maItems.Find[stItem${_count}].Value.NotEqual[FALSE]}) {
					/varset dump ${out[12, "\awstItem${_count}\ax${sep}\a-w${maItems.Find[stItem${_count}].Value}\ax"]}
				}
			/next _count
			/if (${_silent.NotEqual[silent]}) ${out[18, "${eol}"]}  
			/echo
			/return
		}
		
		/if (!${Range.Between[1,20:${Int[${_verbage}]}]}) {
			/call Bind_command_aux item
			/return
		}
		/varset dump ${out[12, "\awItem #${_verbage}\ax"]}
		/varset dump ${out[12, "stItem\aw${_verbage}\ax${sep}\a-w${maItems.Find[stItem${_verbage}].Value}\ax"]}
		/if (${maItems.Find[stItem${_verbage}].Value.Arg[2,+].Find[c]}) {
			OUT ${maItems.Find[stItem${_verbage}].Value.Arg[2,+]}${sep}\a-w${Parse[3,${maCondition.Find[${maItems.Find[stItem${_verbage}].Value.Arg[2,+]}].Value}]}\ax
		}
		/varset dump ${out[12, "lsItem\aw${_verbage}\axTag"]}

		/vardata cli lsItem${_verbage}Tag.First.Clone
		/while (!${cli.IsEnd}) {
			/varset dump ${out[12, "${dot}\a-w${cli.Value}\ax"]}
			/varset dump ${cli.Advance}
		}		
		
	| see what we are observing	
	} else /if (${_variable.Equal[observed]}) {
		/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
			/if (!${Defined[lsObserved_${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}]}) /continue
			/varset dump ${out[12, "Observed on${sep}\at${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|].Upper}\ax"]}
			/vardata cli lsObserved_${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}.First
			/while (!${cli.IsEnd}) {
				/varset dump ${out[12, "${sep}\aw${cli.Value}\ax"]}
				/varset dump ${cli.Advance}
			}			
		/next _count		
		
			
	| wtf.. list...
	} else /if (${_variable.Left[2].Equal[ls]} && ${${_variable}.Count}) {
		/varset dump ${out[12, "\aw${_variable}\ax ${dot} \a-g${${_variable}.Count}\ax ${If[${${_variable}} > 1 || ${${_variable}} == 0,items,item]}"]}
		/vardata cli ${_variable}.First.Clone
		/while (!${cli.IsEnd}) {
			/varset dump ${out[12, "${dot}\a-w${cli.Value}\ax"]}
			/varset dump ${cli.Advance}
		}

	| map
	} else /if (${_variable.Left[2].Equal[ma]} && ${${_variable}.Count} && ${lsMaps.Contains[${_variable}]}) {
		/varset dump ${out[12, "\aw${_variable}\ax ${dot} \a-g${${_variable}.Count}\ax ${If[${${_variable}} > 1 || ${${_variable}} == 0,items,item]}"]}
		/vardata mi ${_variable}.First.Clone
		/while (!${mi.IsEnd}) {
			/if (${_verbage.Equal[aux]}) { 
				/varset _DBLOC ${get_key_DBLOC[${_debug}, ${mi.Key}]}	
				/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM ${_DBLOC.Arg[1,|]} WHERE env_var='${mi.Key}'
				/if (${sqlite.Status[dummy].Equal[SUCCESS]} && ${sqlite.Result[dummy 1 aux].Length} > 6) {
					/varset _tmpAux ${dot} ${sqlite.Result[dummy 1 aux]}
				}	else {
					/varset _tmpAux
				}
			}
			/varset dump ${out[12, "${mi.Key}${sep}\aw${Parse[1,${mi.Value}]}\ax"]}
			/if (${_verbage.Equal[aux]}) {
				/varset dump ${out[12, "\at..\ax\a-g${_tmpAux}\ax"]}
			}
			/varset dump ${mi.Advance}
		}			

	| set
	} else /if (${_variable.Left[2].Equal[se]} && ${${_variable}.Count}) {
		/varset dump ${out[12, "\aw${_variable}\ax ${dot} \a-g${${_variable}.Count}\ax ${If[${${_variable}} > 1 || ${${_variable}} == 0,items,item]}"]}
		/vardata si se${_variable}.First.Clone
		/while (!${si.IsEnd}) {
			/varset dump ${out[12, "${si.Value}"]}
			/varset dump ${si.Advance}
		}

	| does it exist
	} else /if (!${Defined[${_variable}]}) {
		/varset dump ${out[0, "${notice}${sep}\aw${_variable}\ax is not defined. Nothing to see."]}
	 	/return
		
	} else {
		
		/if (${${_variable}.Count} == 0) {
			/varset dump ${out[0, "\a-w${dot} ${_variable} is empty\ax"]}
			/return FALSE
		} else {
			/varset dump ${out[0, "\a-w${dot} you spelled something wrong\ax"]}
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
sub Bind_control_burn(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_burn\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maBurn ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux burn ${_verbage}
		/return		

	| Burn Automatic on/off
	} else /if (${_type.Equal[auto]}) {
		/varset dump ${set_switch_env[${_debug}, swBurnAuto, TRUE, ${_verbage}]}

	| Burn in Raids
	} else /if (${_type.Equal[raid]}) {
		/varset dump ${set_switch_env[${_debug}, swBurnRaid, TRUE, ${_verbage}]}

	| Mob count #
	} else /if (${_type.Equal[count]}) {
		/if (!${set_control_num_range[${_debug}, stBurnCount, "${_verbage}", 0, 99]}) /return

	| Burn Engage at %
	} else /if (${_type.Equal[engage]}) {
		/if (!${set_control_num_range[${_debug}, stPctBurnEngage, "${_verbage}", 0, 99]}) /return

	| force constant burning
	} else /if (${_type.Equal[force]}) {
		/varset dump ${set_switch_env[${_debug}, swBurnForce, TRUE, ${_verbage}]}
		/if (${maBurn.Find[swBurnForce].Value}) {		
			/varset dump ${maBurn.Add[swBurnConditionMet,TRUE]}
		} else /if (!${maBurn.Find[swBurnForce].Value}) {
			/varset dump ${maBurn.Add[swBurnConditionMet,FALSE]}
		}

	} 
	
	/if (${_silent.Equal[SILENT]}) /return
	/declare _lsout list local
	/varset dump ${_lsout.Append[auto,engage,count,force,raid]}
  /declare _out string local
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[auto]}) /varset _out ${_out}:${If[${maBurn.Find[swBurnAuto].Value},${on},${off}]}
		/if (${cli.Value.Equal[engage]}) /varset _out ${_out}:${If[${maBurn.Find[stPctBurnEngage].Value},${num}${maBurn.Find[stPctBurnEngage].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[count]}) /varset _out ${_out}:${If[${maBurn.Find[stBurnCount].Value},${num}${maBurn.Find[stBurnCount].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[force]}) /varset _out ${_out}:${If[${maBurn.Find[swBurnForce].Value},${on},${off}]}
		/if (${cli.Value.Equal[raid]}) /varset _out ${_out}:${If[${maBurn.Find[swBurnRaid].Value},${on},${off}]}
		/varset dump ${cli.Advance}
	}		
	/varset dump ${out[12, "/burn ${_out}"]}
	
/return



|***
 * note: set various debuff things
 * use: /debuff []
 ***| 	
#bind control_debuff /debuff
sub Bind_control_debuff(string _type, string _verbage, string _silent, bool _debug)
	DEBUG \atBind_control_debuff\ax(\a-w${_type}, ${_verbage}, ${_silent}\ax)

	/declare _lsoutS list local
	/varset dump ${_lsoutS.Append[use,single,ae]}
  /declare _out string local
	/vardata cli _lsoutS.First.Clone

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see madeBuff ${_verbage}
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux debuff ${_verbage}
		/return

	} else /if (${_type.Equal[mez]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swMez, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stMez, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stAEMez, "${_silent}"]}
		}
		
		/varset _out
		/vardata cli _lsoutS.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swMez].Value},${on},${off}]}
			/if (${cli.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stMez].Value}]},${num}${madeBuff.Find[stMez].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAEMez].Value}]},${num}${madeBuff.Find[stAEMez].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/debuff mez ${_out}"]}
		/return
		
	} else /if (${_type.Equal[malo]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swMalo, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stMalo, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stAEMalo, "${_silent}"]}
		}
		/varset _out
		/vardata cli _lsoutS.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swMalo].Value},${on},${off}]}
			/if (${cli.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stMalo].Value}]},${num}${madeBuff.Find[stMalo].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAEMalo].Value}]},${num}${madeBuff.Find[stAEMalo].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/debuff malo ${_out}"]}
		/return

	} else /if (${_type.Equal[tash]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swTash, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stTash, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stAETash, "${_silent}"]}
		}
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swTash].Value},${on},${off}]}
			/if (${cli.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stTash].Value}]},${num}${madeBuff.Find[stTash].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAETash].Value}]},${num}${madeBuff.Find[stAETash].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/debuff tash ${_out}"]}
		/return


	} else /if (${_type.Equal[slow]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swSlow, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stSlow, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stAESlow, "${_silent}"]}
		}
		/varset _out
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swSlow].Value},${on},${off}]}
			/if (${cli.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stSlow].Value}]},${num}${madeBuff.Find[stSlow].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAESlow].Value}]},${num}${madeBuff.Find[stAESlow].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/debuff slow ${_out}"]}
		/return


	} else /if (${_type.Equal[snare]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swSnare, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stSnare, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stAESnare, "${_silent}"]}
		}

		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swSnare].Value},${on},${off}]}
			/if (${cli.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stSnare].Value}]},${num}${madeBuff.Find[stSnare].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAESnare].Value}]},${num}${madeBuff.Find[stAESnare].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/debuff snare ${_out}"]}
		/return

	} else /if (${_type.Equal[eradicate]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swEradicate, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stEradicate, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stAEEradicate, "${_silent}"]}
		}
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swEradicate].Value},${on},${off}]}
			/if (${cli.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stEradicate].Value}]},${num}${madeBuff.Find[stEradicate].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAEEradicate].Value}]},${num}${madeBuff.Find[stAEEradicate].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/debuff eradicate ${_out}"]}
		/return

	} else /if (${_type.Equal[cripple]}) {
		/if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swCripple, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[single]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stCripple, "${_silent}"]}
		} else /if (${_verbage.Equal[ae]}) {
			/varset dump ${do_raw_edit[${_debug}, TRUE, stAECripple, "${_silent}"]}
		}
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${madeBuff.Find[swCripple].Value},${on},${off}]}
			/if (${cli.Value.Equal[single]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stCripple].Value}]},${num}${madeBuff.Find[stCripple].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ae]}) /varset _out ${_out}:[${If[${Bool[${madeBuff.Find[stAECripple].Value}]},${num}${madeBuff.Find[stAECripple].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/debuff cripple ${_out}"]}
		/return	

	}

	/if (${_silent.Equal[SILENT]}) /return
	/declare _lsout list local
	/varset dump ${_lsout.Append[mez,malo,tash,slow,snare,eradicate,cripple]}
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsoutS.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[mez]}) /varset _out ${_out}:${If[${madeBuff.Find[swMez].Value},${on},${off}]} ${submenu}
		/if (${cli.Value.Equal[malo]}) /varset _out ${_out}:${If[${madeBuff.Find[swMalo].Value},${on},${off}]} ${submenu}
		/if (${cli.Value.Equal[tash]}) /varset _out ${_out}:${If[${madeBuff.Find[swTash].Value},${on},${off}]} ${submenu}
		/if (${cli.Value.Equal[slow]}) /varset _out ${_out}:${If[${madeBuff.Find[swSlow].Value},${on},${off}]} ${submenu}
		/if (${cli.Value.Equal[snare]}) /varset _out ${_out}:${If[${madeBuff.Find[swSnare].Value},${on},${off}]} ${submenu}
		/if (${cli.Value.Equal[eradicate]}) /varset _out ${_out}:${If[${madeBuff.Find[swEradicate].Value},${on},${off}]} ${submenu}
		/if (${cli.Value.Equal[cripple]}) /varset _out ${_out}:${If[${madeBuff.Find[swCripple].Value},${on},${off}]} ${submenu}
		/varset dump ${cli.Advance}
	}		
	/varset dump ${out[12, "/debuff ${_out}"]}

/return



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
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux cc ${_verbage}
		/return		

	| set assist
	} else /if (${_type.Equal[ass]}) {
		/if (${_verbage.Equal[g]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[g]},g,FALSE\ax]}
		} else /if (${_verbage.Equal[ma1]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode "ma1"
			/if (${Bool[${_silent}]} && ${_silent.NotEqual[SILENT]}) {
				/call do_raw_edit ${_debug} SILENT stManualAssist1 "${_silent}"
			}
		} else /if (${_verbage.Equal[ma2]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode "ma2"
			/if (${Bool[${_silent}]} && ${_silent.NotEqual[SILENT]}) {
				/call do_raw_edit ${_debug} SILENT stManualAssist2 "${_silent}"
			}
		} else /if (${_verbage.Equal[1]}) {
			/echo here
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[1]},1,FALSE\ax]}
		} else /if (${_verbage.Equal[2]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[2]},2,FALSE\ax]}
		} else /if (${_verbage.Equal[3]}) {
			/call do_raw_edit ${_debug} SILENT stAssistMode ${If[${maCC.Find[stAssistMode].Value.NotEqual[3]},3,FALSE\ax]}
		} 

		/if (${_silent.Equal[SILENT]}) /return
		/declare _lsAssistTypes list local
		/varset dump ${_lsAssistTypes.Append[g,1,2,3,ma1,ma2]}
		/vardata cli _lsAssistTypes.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${lsAssistTypes.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[g]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[g]},${on},${off}]}
			/if (${cli.Value.Equal[1]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[1]},${on},${off}]}
			/if (${cli.Value.Equal[2]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[2]},${on},${off}]}
			/if (${cli.Value.Equal[3]}) /varset _out ${_out}:${If[${maCC.Find[stAssistMode].Value.Equal[3]},${on},${off}]}
			/if (${cli.Value.Equal[ma1]}) /varset _out ${_out}:[${If[${Bool[${maCC.Find[stManualAssist1].Value}]},${num}${maCC.Find[stManualAssist1].Value}\ax,${off}]}]
			/if (${cli.Value.Equal[ma2]}) /varset _out ${_out}:[${If[${Bool[${maCC.Find[stManualAssist2].Value}]},${num}${maCC.Find[stManualAssist2].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/cc ass ${_out}"]}
		/return
	
	| glyphs
	} else /if (${_type.Equal[glyph]}) {
		/if (${_verbage.Equal[group]}) {
			/varset dump ${set_switch_env[${_debug}, swGlyphGroup, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[raid]}) {
			/varset dump ${set_switch_env[${_debug}, swGlyphRaid, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[name]}) {
			/varset dump ${do_raw_edit[FALSE, SILENT, stGlyph, "${_silent}"]}
		}
		
		/declare _lsout list local 
		/varset dump ${_lsout.Append[group,raid,name]}
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {	
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[group]}) /varset _out ${_out}:${If[${maCC.Find[swGlyphGroup].Value},${on},${off}]}
			/if (${cli.Value.Equal[raid]}) /varset _out ${_out}:${If[${maCC.Find[swGlyphRaid].Value},${on},${off}]}
			/if (${cli.Value.Equal[name]}) /varset _out ${_out}:[${If[${Bool[${maCC.Find[stGlyph].Value}]},${num}${maCC.Find[stGlyph].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/cc glyph ${_out}"]}
		/return
		
	| set combat mode	
	} else /if (${_type.Equal[mode]}) {

		| enable combat: melee
		/if (${_verbage.Equal[melee]}) {
			/varset dump ${set_switch_env[${_debug}, swCombatMelee, TRUE, ${_silent}]}

		| enable combat: range
		} else /if (${_verbage.Equal[range]}) {
			/varset dump ${set_switch_env[${_debug}, swCombatRange, TRUE, ${_silent}]}

		| enable combat: nuke
		} else /if (${_verbage.Equal[nuke]}) {
			/varset dump ${set_switch_env[${_debug}, swCombatNuke, TRUE, ${_silent}]}

		| enable combat: dot
		} else /if (${_verbage.Equal[dot]}) {
			/varset dump ${set_switch_env[${_debug}, swCombatDoT, TRUE, ${_silent}]}

		| enable combat: Heal
		} else /if (${_verbage.Equal[heal]}) {
			/varset dump ${set_switch_env[${_debug}, swCombatHeal, TRUE, ${_silent}]}

		| enable combat: Heal
		} else /if (${_verbage.Equal[cc]}) {
			/varset dump ${set_switch_env[${_debug}, swCombatControl, TRUE, ${_silent}]}

		}	
	
		/declare _lsout list local 
		/varset dump ${_lsout.Append[melee,range,dot,nuke,heal,cc]}
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {	
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[melee]}) /varset _out ${_out}:${If[${maCC.Find[swCombatMelee].Value},${on},${off}]}
			/if (${cli.Value.Equal[range]}) /varset _out ${_out}:${If[${maCC.Find[swCombatRange].Value},${on},${off}]}
			/if (${cli.Value.Equal[nuke]}) /varset _out ${_out}:${If[${maCC.Find[swCombatNuke].Value},${on},${off}]}
			/if (${cli.Value.Equal[dot]}) /varset _out ${_out}:${If[${maCC.Find[swCombatDoT].Value},${on},${off}]}
			/if (${cli.Value.Equal[heal]}) /varset _out ${_out}:${If[${maCC.Find[swCombatHeal].Value},${on},${off}]}
			/if (${cli.Value.Equal[cc]}) /varset _out ${_out}:${If[${maCC.Find[swCombatControl].Value},${on},${off}]}
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/cc mode ${_out}"]}
		/return
	
	| set melee things to do
	} else /if (${_type.Equal[melee]}) {

		/if (${_verbage.Equal[bash]} && ${lsMeleeSkill.Contains[bash]}) {
			/varset dump ${set_switch_env[${_debug}, swBash, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[disarm]} && ${lsMeleeSkill.Contains[disarm]}) {
			/varset dump ${set_switch_env[${_debug}, swDisarm, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[backstabb]} && ${lsMeleeSkill.Contains[backstabb]}) {
			/varset dump ${set_switch_env[${_debug}, swBackstab, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[dragonpunch]} && ${lsMeleeSkill.Contains[dragonpunch]}) {
			/varset dump ${set_switch_env[${_debug}, swDragonpunch, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[eaglestrike]} && ${lsMeleeSkill.Contains[eaglestrike]}) {
			/varset dump ${set_switch_env[${_debug}, swEaglestrike, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[tigerclaw]} && ${lsMeleeSkill.Contains[tigerclaw]}) {
			/varset dump ${set_switch_env[${_debug}, swTigerclaw, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[kick]} && ${lsMeleeSkill.Contains[kick]}) {
			/varset dump ${set_switch_env[${_debug}, swKick, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[flyingkick]} && ${lsMeleeSkill.Contains[flyingkick]}) {
			/varset dump ${set_switch_env[${_debug}, swFlyingkick, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[roundkick]} && ${lsMeleeSkill.Contains[roundkick]}) {
			/varset dump ${set_switch_env[${_debug}, swRoundkick, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[taunt]} && ${lsMeleeSkill.Contains[taunt]}) {
			/varset dump ${set_switch_env[${_debug}, swTaunt, TRUE, ${_silent}]}

		} else /if (${_verbage.Equal[frenzy]} && ${lsMeleeSkill.Contains[frenzy]}) {
			/varset dump ${set_switch_env[${_debug}, swFrenzy, TRUE, ${_silent}]}
		} else /if (${_verbage.Equal[intimidation]} && ${lsMeleeSkill.Contains[intimidation]}) {
			/varset dump ${set_switch_env[${_debug}, swIntimidation, TRUE, ${_silent}]}

		}	
		
		/vardata cli lsMeleeSkill.First.Clone
		/while (!${cli.IsEnd}) {	
			/varset _out ${_out} ${If[${cli.Value.Equal[${lsMeleeSkill.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[bash]}) /varset _out ${_out}:${If[${maCC.Find[swBash].Value},${on},${off}]}
			/if (${cli.Value.Equal[disarm]}) /varset _out ${_out}:${If[${maCC.Find[swDisarm].Value},${on},${off}]}
			/if (${cli.Value.Equal[backstabb]}) /varset _out ${_out}:${If[${maCC.Find[swBackstab].Value},${on},${off}]}
			/if (${cli.Value.Equal[dragonpunch]}) /varset _out ${_out}:${If[${maCC.Find[swDragonpunch].Value},${on},${off}]}
			/if (${cli.Value.Equal[eaglestrike]}) /varset _out ${_out}:${If[${maCC.Find[swEaglestrike].Value},${on},${off}]}
			/if (${cli.Value.Equal[tigerclaw]}) /varset _out ${_out}:${If[${maCC.Find[swTigerclaw].Value},${on},${off}]}
			/if (${cli.Value.Equal[kick]}) /varset _out ${_out}:${If[${maCC.Find[swKick].Value},${on},${off}]}
			/if (${cli.Value.Equal[flyingkick]}) /varset _out ${_out}:${If[${maCC.Find[swFlyingkick].Value},${on},${off}]}
			/if (${cli.Value.Equal[roundkick]}) /varset _out ${_out}:${If[${maCC.Find[swRoundkick].Value},${on},${off}]}
			/if (${cli.Value.Equal[taunt]}) /varset _out ${_out}:${If[${maCC.Find[swTaunt].Value},${on},${off}]}
			/if (${cli.Value.Equal[intimidation]}) /varset _out ${_out}:${If[${maCC.Find[swIntimidation].Value},${on},${off}]}
			/if (${cli.Value.Equal[frenzy]}) /varset _out ${_out}:${If[${maCC.Find[swFrenzy].Value},${on},${off}]}
			/varset dump ${cli.Advance}
		}	
		/varset dump ${out[12, "/cc melee ${_out}"]}
		/return
	
	| set dot refresh time in seconds
	} else /if (${_type.Equal[dotrefresh]}) {
		/if (!${set_control_num_range[${_debug}, stDotRefresh, "${_verbage}", 0, 99]}) /return


	} else /if (${_type.Equal[ma1]}) {
		/if (${Bool[${_verbage}]} || ${_verbage.Equal[FALSE]}) {
			/varset dump ${do_raw_edit[FALSE, SILENT, stManualAssist1, "${_verbage}"]}
		} else {
			/varset dump ${do_raw_edit[FALSE, SILENT, stManualAssist1, FALSE]}
		}
		

	} else /if (${_type.Equal[ma2]}) {
		/if (${Bool[${_verbage}]} || ${_verbage.Equal[FALSE]}) {
			/varset dump ${do_raw_edit[FALSE, SILENT, stManualAssist2, "${_verbage}"]}
		} else {
			/varset dump ${do_raw_edit[FALSE, SILENT, stManualAssist2, FALSE]}
		}
		
	| set pull mode
	} else /if (${_type.Equal[pull]}) {
		| turn pull on or off
		/if (${_verbage.Equal[active]}) {
			/varset dump ${set_switch_env[FALSE, swPull, TRUE, ${_silent}]}
			| /if (!PULL && HOME) /call Bind_home_control clear FALSE FALSE
		
		| pull radius
		} else /if (${_verbage.Equal[rad]}) {
			/varset dump ${do_raw_edit[FALSE, SILENT, stPullRadius, "${_silent}"]}

		| enable/disable setting home marker when pulling
		} else /if (${_verbage.Equal[sethome]}) {
			/varset dump ${set_switch_env[FALSE, swPullSetHome, TRUE, ${_silent}]}

		
		| base pulling	
		} else /if (${_verbage.Equal[base]}) {
			/call do_raw_edit ${_debug} SILENT stPullMode "base"
		| intervention pulling	
		} else /if (${_verbage.Equal[int]}) {
			/if (${Select[${Me.Class.ShortName},CLR,SHM]}) {
				/varset dump ${get_shade[FALSE, MQWindow]}
				/return
			}		
			/call do_raw_edit ${_debug} SILENT setPullMode "int"
		| pet pulling
		} else /if (${_verbage.Equal[pet]}) {
			/if (!${Select[${Me.Class.ShortName},MAG,ENC,DRU,SHM,SHD,NEC,BST]}) {
				/varset dump ${get_shade[FALSE, MQWindow]}
				/return
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
		/varset _outP ${_outP} \awrad\ax:${If[${maCC.Find[stPullRadius].Value},${num}${maCC.Find[stPullRadius].Value}\ax,${r0}]}
		/varset _outP  ${_outP} \awsethome\ax:${If[${maCC.Find[swPullSetHome].Value},${on},${off}]} ${dot}
		/vardata _liP lsPullTypes.First.Clone
		/while (!${_liP.IsEnd}) {		
			/varset _outP ${_outP} ${If[${_liP.Value.Equal[${lsPullTypes.First}]},,${dot}]} ${If[${_liP.Value.Equal[${maCC.Find[stPullMode].Value}]},${num}${_liP.Value}\ax,${info}${_liP.Value}\ax]}	
			/varset dump ${_liP.Advance}
		}	
		/varset dump ${out[12, "/cc pull ${_outP}"]}
		/return

	| set stop nuke %
	} else /if (${_type.Equal[stopnuke]}) {
		/if (!${set_control_num_range[${_debug}, stPctStopNuke, "${_verbage}", 0, 99]}) /return

	| delay in nuking	
	} else /if (${_type.Equal[nukedelay]}) {
		/if (!${set_control_num_range[${_debug}, stNukeDelay, "${_verbage}", 0, 100]}) /return

	| mob engage %
	} else /if (${_type.Equal[engage]}) {
		/if (!${set_control_num_range[${_debug}, stPctEngage, "${_verbage}", 0, 100]}) /return

	| set Z radius
	} else /if (${_type.Equal[zradius]}) {
		/if (!${set_control_num_range[${_debug}, stZRadius, "${_verbage}", 0, 300]}) /return

	| use bandolier or not
	} else /if (${_type.Equal[bandolier]}) {
		/varset dump ${set_switch_env[${_debug}, swBandolier, TRUE, ${_verbage}]}

	| agro
	} else /if (${_type.Equal[agro]}) {
		/varset dump ${set_switch_env[${_debug}, swAgro, TRUE, ${_verbage}]}

	| use poisons
	} else /if (${_type.Equal[poison]}) {
		/varset dump ${set_switch_env[${_debug}, swPoison, TRUE, ${_verbage}]}

	| force manual combat routines
	} else /if (${_type.Equal[force]}) {
		/if (${Target.ID} && (${Target.ID} != ${Me.ID}) && (${Target.ID} != ${Me.Pet.ID})) {

			/if (${Me.Invis}) /makemevisible
			/varset dump ${maCC.Add[swValidAssTarID,TRUE]}
			/varset dump ${maCC.Add[swSetCombat,TRUE]}
			/varset stAssTarID ${Target.ID}
			/varset dump ${out[12, "/cc force ${dot} ${tar}${Target.DisplayName}\ax"]}
			/if (${maMinion.Find[swonForce].Value}) {
				/pet attack
			}
		}
		/return

	| shortcut to broadcast opverride engage pct to force instant engage
	} else /if (${_type.Equal[kill]}) {
		
		/declare _count int local 0
		/declare _toon string local FALSE
		
		/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
			/varset _toon ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
			/if (${_toon.Equal[${Me.DisplayName}]}) /continue
			/if (!${Spawn[pc ${_toon}].ID}) /continue

			/if (${_verbage.Equal[off]}) {
				/dexecute ${_toon} /varset dump ${maOver.Add[swOverPctEngage,FALSE]}
			} else /if (${_verbage.Equal[on]}) {
				/dexecute ${_toon} /varset dump ${maOver.Add[swOverPctEngage,TRUE]}
			}
		/next _count
			
	}
	
	/if (${_silent.Equal[SILENT]}) /return
	/declare _lsout list local 
	/varset dump ${_lsout.Append[ass,agro,mode,pull,melee,glyph,dotrefresh,stopnuke,nukedelay,engage,poison,bandolier]}
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[ass]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[mode]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[dotrefresh]}) /varset _out ${_out}:${If[${maCC.Find[stDotRefresh].Value},${num}${maCC.Find[stDotRefresh].Value}s\ax,${r0}]}
		/if (${cli.Value.Equal[pull]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[melee]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[glyph]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[stopnuke]}) /varset _out ${_out}:${If[${maCC.Find[stPctStopNuke].Value},${num}${maCC.Find[stPctStopNuke].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[nukedelay]}) /varset _out ${_out}:${If[${maCC.Find[stNukeDelay].Value},${num}${maCC.Find[stNukeDelay].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[engage]}) /varset _out ${_out}:${If[${maCC.Find[stPctEngage].Value},${num}${maCC.Find[stPctEngage].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[zradius]}) /varset _out ${_out}:${If[${maCC.Find[stZRadius].Value},${num}${maCC.Find[stZRadius].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[agro]}) /varset _out ${_out}:${If[${maCC.Find[swAgro].Value},${on},${off}]}
		/if (${cli.Value.Equal[bandolier]}) /varset _out ${_out}:${If[${maCC.Find[swBandolier].Value},${on},${off}]}
		/if (${cli.Value.Equal[poison]}) /varset _out ${_out}:${If[${maCC.Find[swPoison].Value},${on},${off}]}
		/varset dump ${cli.Advance}
	}	
	
	
	/varset dump ${out[12, "/cc ${_out}"]}

/return



|***
 * note: help text
 * use: 
 ***|
#Bind command_aux /aux
sub Bind_command_aux(string _type, bool _debug)

	| /varset _debug TRUE
	/declare _lsout list local 
	/varset dump ${_lsout.Append[bandolier,beg,buff,build,burn,cc,crew,condition,cursor,debuff,debug,edit]}
	/varset dump ${_lsout.Append[entropy,env,grab,grain,heal,home,minion,navto,off,on,over,see,take,tag,tc,tie]}
	
	/if (${_lsout.Contains[${_type}]}) {
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM help WHERE help_cat='${_type}' ORDER BY help_var
		DEBUG dummy SELECT * FROM help WHERE help_cat='${_type}' ORDER BY help_var
		/declare _count int local 0
		/mqclear
		/echo
		/varset dump ${out[0, "\aw/aux \ax\a-w${_type.Lower}\ax"]}
		/for _count 1 to ${sqlite.Rows[dummy]}
			/varset dump ${out[0, "${If[${sqlite.Result[dummy ${_count} help_var].Find[${_type}]},,${sqlite.Result[dummy ${_count} help_var]}${sep}]}\a-g${Parse[2,${sqlite.Result[dummy ${_count} value]}]}\ax"]}
		/next _count
		/varset dump ${out[18, "${eol}"]} 
		/return
	}

	/echo
	/sqlite ${maData.Find[DBstatic].Value} dummy SELECT value FROM online WHERE env_var='discord'
	/varset dump ${out[0, "Discord${sep}\a-w${sqlite.Result[dummy 1 value]}\ax"]}
	/varset dump ${out[0, "Available Control Information${sep}"]}
	/declare _out string local
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/varset dump ${cli.Advance}
	}	
	/varset dump ${out[0, "/aux ${_out}"]}
	/return		

/return



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
	} else /if (!${FindItemCount[=${_item}]}) {
		OUT Cannot Find ${_item}
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
	
/return



|***
 * note: complete stop.. of all your dumb shit
 * use: /alto
 ***| 	
#bind command_alto /alto
sub Bind_command_alto(bool _debug)
	/call set_stop FALSE
	/call clear_combat FALSE
/return



|***
 * note: navigates to the name/target
 * use: /navto [NAME|TARGET]
 ***|
#bind command_navto /navto
sub Bind_command_navto(string _verbage, bool _debug)
	DEBUG \atBind_command_navto\ax(\a-w${_verbage}\ax)

	/if (!${Target.ID} && !${Bool[${_verbage}]}) {
		OUT ${notice}${sep}need name or target.
		/return
	}

	/if (${Target.ID} && !${Bool[${_verbage}]}) {
		/varset _verbage ${Target.DisplayName}
	}

	/declare _tmpHomeMarker bool local FALSE
	/declare _tmpPullMarker bool local FALSE

	/if (${maHome.Find[swHomeMarker].Value}) {
		/varset _tmpHomeMarker TRUE
		/call Bind_control_home clear FALSE silent
	}
	/if (${maCC.Find[swPull].Value}) {
		/varset _tmpPullMarker TRUE
		/varset dump ${maCC.Add[swPull,FALSE]}			
	}
	
	/declare _tmpID int local ${Spawn[${_verbage}].ID}
	OUT /navto \a-w${Spawn[${_tmpID}].DisplayName}\ax
	/nav ID ${_tmpID} dist=${maHome.Find[stHomeVariance].Value} log=off
	/delay 1s ${Me.Moving}

	DEBUG ${sep}moving
	/while (${Me.Moving}) {
		/delay 2
	}		
	
	| reset home and pull if they were being used
	/if (${_tmpHomeMarker}) {
		/call Bind_control_home set
	}
	/if (${_tmpPullMarker}) {
		/varset dump ${maCC.Add[swPull,TRUE]}	
	}
	
/return	



|***
 * note: have everyone get your target and repeate what you say.
 * use: /repeat "things to say"
 ***|
#bind command_repeat /repeat
sub Bind_command_repeat(string _verbage, string _sender, int _tmpID, bool _debug)
	DEBUG \atBind_command_repeat\ax(\a-w${_verbage}, ${_sender}, ${_tmpID}\ax)
	
	/makemevisible
	
	| sender
	/if (!${Bool[${_sender}]}) {
		| need a target
		/if (!${Target.ID}) /return FALSE
		
		| speak you peace
		/say ${_verbage}
		
		| tell everyone else to speak up
		/dgexecute /repeat "${_verbage}" ${Me.DisplayName} ${Target.ID}
		/return
	}

	| everyone else
	/if (${_sender.NotEqual[${Me.DisplayName}]}) {
		/if (${Bool[${Spawn[pc ${_sender} radius 60].ID}]}) {
			/varset dump ${set_Target[${_tmpID}]}
			/if (${Target.ID} == ${_tmpID}) {
				/delay ${Int[${Math.Calc[${Math.Rand[20]}+10]}]}
				/say ${_verbage}
				/delay 1s
			}
		}
	}
	
/return




|***
 * note: make the toon stop its shit and splash at its feet
 * use: /dex NAME /splash | /splash
 ***|
#bind command_splash /splash
sub Bind_command_splash(bool _debug)
	DEBUG \atBind_command_splash\ax()
	
	/if (!${check_class[TRUE, |PAL|CLR]}) /return FALSE
	
	| cleric
	/if (${Me.Class.ShortName.Equal[PAL]}) {
		/if (${is_ready[FALSE, "${maChr.Find[stSplashCure].Value}"]}) {
		/call voodoo FALSE "${maChr.Find[stSplashCure].Value}" 0 FALSE
		}
	}

	| cleric
	/if (${Me.Class.ShortName.Equal[CLR]}) {
		/if (${is_ready[FALSE, "${maChr.Find[stSplash].Value}"]}) {
			/call voodoo FALSE "${maChr.Find[stSplash].Value}" ${Me.ID} FALSE
		}
	}
	
/return TRUE



|***
 * note: clicks a door or lever to open or zone
 * use: /enter
 ***|
#bind command_enter /enter
sub Bind_command_enter(bool _other, bool _debug)

	/if (!${Bool[${_other}]} && ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]} > 1) {
		/dgexecute ${maComm.Find[stEntropyGroup_all].Value} /enter TRUE
		OUT ${broadcast}${sep}\a-w/enter\ax
	}

	/varset dump ${set_data_timer[FALSE, Try_Door, RESTART]}

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

/return



|***
 * note: stack invis
 * use: /invis
 ***|
#bind command_invis /invis
sub Bind_command_invis(bool _other, bool _debug)

	/if (!${Bool[${_other}]} && ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]} > 1) {
		/dgexecute ${maComm.Find[stEntropyGroup_all].Value} /invis TRUE
		OUT ${broadcast}${sep}\a-w/invis\ax
	}

	| bards can blow me
	/if (${Me.AltAbilityReady[Shauri's Sonorous Clouding]} && ${Select[${Me.Class.ShortName},BRD]}) {
		/call pulse_stop
		/call voodoo FALSE "Shauri's Sonorous Clouding" 0 FALSE
		/return
		
	| Invis vs Normies  casters	
	} else /if (${Me.AltAbilityReady[Group Perfected Invisibility]} && ${Select[${Me.Class.ShortName},WIZ,MAG,ENC]}) {
		/call voodoo FALSE "Group Perfected Invisibility" 0 FALSE
		/return
	
	| Invis Vs undead casters
	} else /if (${Me.AltAbilityReady[Group Perfected Invisibility to Undead]} && ${Select[${Me.Class.ShortName},CLR,SHD,NEC,PAL]}) {
		/delay 3
		/call voodoo FALSE "Group Perfected Invisibility to Undead" 0 FALSE
		/return
		
	| shaman	
	} else /if (${Me.AltAbilityReady[Group Silent Presence]} && ${Select[${Me.Class.ShortName},SHM]}) {
		/delay 5
		/call voodoo FALSE "Group Silent Presence" ${Me.ID} FALSE
		/return
	
	| rangers and druids
	}	else /if (${Me.AltAbilityReady[Shared Camouflage]} && ${Select[${Me.Class.ShortName},RNG,DRU]}) {
		/return
		/delay 15
		/call voodoo FALSE "Shared Camouflage" 0 FALSE
		/return
	}
	
/return








|***
 * note: displays the port and builds the command to the porting class
 * use: /port 
 ***| 
#bind command_port /port
sub Bind_command_port(string _type, string _nick1, string _nick2, bool _debug)
	| /varset _debug TRUE
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
			/varset dump ${out[18, "${eol}"]} 
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


	| item ports
	} else /if (${Select[${_type},item]}) {
		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='item'
		/for _count 1 to ${sqlite.Rows[dummy]}
			/varset dump ${_list.Append[${sqlite.Result[dummy ${_count} nick1]}]}
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
			OUT not a valid item port
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
			/varset dump ${_list.Append[${sqlite.Result[dummy ${_count} nick1]}]}
		/next _count
		
		/if (${_list.Contains[${_nick1}]}) {
			DEBUG AA
			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM port WHERE type='aa' AND nick1="${_nick1}"
			/varset _portName ${sqlite.Result[dummy 1 name1]}
			
		} else {
			OUT not a valid AA port
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
sub Bind_command_off(bool _debug)
	DEBUG \atBind_command_off\ax()
	
	/declare _count int local 0
	/declare _toon string local FALSE
	
	/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
		/varset _toon ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
		/if (${_toon.Equal[${Me.DisplayName}]}) /continue
		/if (!${Spawn[pc ${_toon}].ID}) /continue
		/dexecute ${_toon} /env auto off
		/dexecute ${_toon} /squelch /target clear
	/next _count
	
/return



|***
 * note: turns everyone on.. who doesn't like a little foreplay
 * use: /on  
 ***|
#bind command_on /command_on
sub Bind_command_on(bool _debug)
	DEBUG \atBind_command_on\ax()
	
	/declare _count int local 0
	/declare _toon string local FALSE
	
	/for _count 1 to ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]}
		/varset _toon ${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Arg[${_count},|]}
		/if (${_toon.Equal[${Me.DisplayName}]}) /continue
		/if (!${Spawn[pc ${_toon}].ID}) /continue
		/dexecute ${_toon} /env auto on
	/next _count
	
/return



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

/return



|***
 * note: fuck, what did the bard do this time?
 * use: /here
 ***|
#bind command_here /here
sub Bind_command_here(string _type)

	/if (${_type.Equal[aux]}) {
		/call Bind_command_aux here
		/return		
	}


	/if (!${Bool[${_type}]} && ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]} > 1) {
		OUT ${broadcast}${sep}\a-w/here\ax
		/dgexecute ${maComm.Find[stEntropyGroup_all].Value} /here TRUE
		/return
	}
	
	ISMEDEAD

	| disable Tie
	/varset dump ${maTie.Add[swSetTie,FALSE]}
	| sort movements
	/if (${Stick.Active}) /squelch /stick off
	| stop mq2nav
	/if (${Navigation.Active}) /squelch /nav stop log=off

	| enable auto
	/varset dump ${maEnv.Add[swAuto,TRUE]}
	
	/makemevisible
	/stopcast
	
	| reduce pull range if in a dense area
	/if (${maCC.Find[swPull].Value}  && ${SpawnCount[NPC radius PULLENV PS4 targetable]} >= AE#) {
		/if (PULLENV > 60) {
			/call Bind_control_cc pull rad 60
		}
	}
	
	
	| reset timers
	/declare lsTimers list local
	/varset dump ${lsTimers.Append[Check_Buff_Aura,Check_Buff_Beg,Check_Buff_Crew,Check_Buff_Self,Check_Buff_Shrink,Check_Buff_Unity]}
	/varset dump ${lsTimers.Append[Check_Minion_Cycle]}
	/vardata cli lsTimers.First.Clone
	/while (!${cli.IsEnd}) {
		/varset dump ${set_data_timer[FALSE, ${cli.Value}, 1m]}
		/varset dump ${cli.Advance}
	}	
	

	| stop moving
	/if (${Me.Moving}) {
		/keypress forward
		/keypress back
	}

	| clear the current target
	/if (${Target.ID}) /squelch /target clear

	/if (PET && ${Pet.ID} && ${Me.Pet.Target.ID}) /pet back off

/return



|***
 * note: clicks yes on a window option for some form of travel
 * use: /take [debug] <SENDER>
 ***|
#bind command_take /take
sub Bind_command_take(string _debug, string _sender)
	
	/if (${_debug.Equal[aux]}) {
		/call Bind_command_aux take
		/return		
	}
	
	/if (!${Bool[${_sender}]} && ${DanNet.PeerCount[${maComm.Find[stEntropyGroup_all].Value}]} > 1) {
		/dgexecute /take FALSE ${Me.DisplayName}
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
	
	| trade window
	/if (${Window[Tradewnd]}) /nomodkey /notify Tradewnd TRDW_Trade_Button leftmouseup

	| give trade (inc NPC)
	/if (${Window[GiveWnd]}) /squelch /notify GiveWnd GVW_Give_Button leftmouseup
	
/return



|***
 * note: grabs a ground spawn. max range of 20. parital names accepted
 * use: /grab NAME
 ***|
#bind command_grab /grab
sub Bind_command_grab(string _name, bool _debug)
	DEBUG Bind_command_grab(\a-w${_name}\ax)

	| typed something wrong
	/if (${_name.Equal[NULL]} || !${Bool[${_name}]} || ${_name.Equal[info]} ) {
		OUT /grab ${info}NAME\ax of mapfilter spawn name you want to pick up.
		/return
	}
	
	| trash pick up.
	/if (${Ground[${_name}].Distance3D} > 20) {
		OUT ${info}${_name}\ax is ${Ground[${_name}].Distance3D}. Needs to be within 20.
		/return
	} else /if (${Ground[${_name}].Distance3D} <= 20) {
		/invoke ${Ground[${_name}].Grab}
	}
	
/return



|***
 * note: lesson of the devoted
 * use: /lesson
 ***|
#bind command_lesson /lesson
sub Bind_command_lesson(bool _debug)
  /if (${is_ready[FALSE, "Lesson of the Devoted"]}) {
  	/call voodoo FALSE "Lesson of the Devoted" 0 FALSE
  }
  
/return



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
	
/return



|***
 * note: allows for custom builds
 * use: /build [save|load] [nickname] [DEBUG]
 ***|
#bind control_build /build
sub Bind_control_build(string _build, string _nickname, bool _debug)

	/declare _count int local 0

	| output control map
	/if (${_build.Equal[aux]}) {
		/call Bind_command_aux build ${_nickname}
		/return		

	} else /if (!${Bool[${_build}]} || !${Bool[${_nickname}]}) {
		OUT Usage /build [\a-wsave\ax|\a-wload\ax] "\a-wnick name\ax" 
		/return
	}
	
	| set file name
	/declare _fn string local build_${Me.Class.ShortName.Lower}_${_nickname}.ini
	
	| set fully qualified MQ name
	/declare _fqn string local exspes\\data\\${_fn}
	
	| set stuff to not export/import
	/declare _skipMap list local
	/varset dump ${_skipMap.Append[maDebug,maTimer,Gems,maCrew]}
	/declare _skipKey list local
	/varset dump ${_skipKey.Append[swBuffMaster,swIsMeDead,swSetCombat,swValidAssTarID,swSnapStuck,swHomeMarker,verbosity,swZoneSafe,swAuto]}
	/varset dump ${_skipKey.Append[stCurrentZoneID,stCastReturn,build,swSetTie,swBurnConditionMet]}

	| obfuscation
	/varset dump ${_skipKey.Append[lsProtectInsideRaid,lsProtectOutsideGroup,stTieToon,lsRetortToon,lsWardToon]}

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
		/vardata cli lsMaps.First.Clone
		/while (!${cli.IsEnd}) {

			| skip the maps we dont want to export
			/if (${_skipMap.Contains[${cli.Value}]}) {
				/varset dump ${cli.Advance}
				/continue
			}
		
			/echo \at...\ax\a-w${cli.Value}\ax
				/vardata _mi ${cli.Value}.First.Clone
				/while (!${_mi.IsEnd}) {
					/if (${_skipKey.Contains[${_mi.Key}]}) {
						/varset dump ${_mi.Advance}
						/continue
					}
					
					| skip empty/false values
					/if (${_mi.Value.Equal[FALSE]} || ${_mi.Value.Equal[EMPTY]}) {
						/varset dump ${_mi.Advance}
						/continue
					}
					
					/ini "${_fqn}" ${cli.Value} "${_mi.Key}" "${Parse[1,${_mi.Value}]}"
					/varset dump ${_mi.Advance}
				}
			/varset dump ${cli.Advance}
		}
		
		/if (${Me.NumGems} && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
			OUT \awSaving Gems\ax${sep}\a-w${Me.NumGems}\ax
			/for _count 1 to ${Me.NumGems}
				/ini "${_fqn}" Gems "Gem${_count}" "${Spell[${Me.Gem[${_count}]}].BaseName}"
			/next _count
		}
			
		OUT \awUpdating\ax${sep}\a-wMD5\ax
		/squelch /exec powershell "(get-content ${MacroQuest.Path}\macros\exspes\data\${_fn}).Replace('MD5',(Get-FileHash ${MacroQuest.Path}\macros\exspes\data\${_fn} -Algorithm MD5).Hash) | Set-Content ${MacroQuest.Path}\macros\exspes\data\${_fn}" bg
		/varset dump ${out[18, "${eol}"]}
		/echo
		/return

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
		
			/for _countK 1 to ${Ini[${_fqn},${_section}].Count[|]}
				/varset _key ${Ini[${_fqn},${_section}].Arg[${_countK},|]}
				/if (${Select[${_key},NULL]}) /continue
				DEBUG ${_key}=${Parse[2,${Ini[${_fqn},${_section},${_key}]}]}
				/call do_raw_edit FALSE SILENT ${_key} "${Parse[2,${Ini[${_fqn},${_section},${_key}]}]}" OVERRIDE
			/next _countK
		/next _countS
		
		| load spells
		/if (${Me.NumGems} && ${lsClassCast.Contains[${Me.Class.ShortName}]}) {
			OUT \awLoading Gems\ax${sep}\a-w${Me.NumGems}\ax
			/if (!${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoOpen}
			/for _count 1 to ${Me.NumGems}
				/call set_spell_to FALSE TRUE "${Spell[${Ini[${_fqn},Gems,Gem${_count}]}].RankName}" ${_count} FALSE
			/next _count
			/if (${Window[SpellBookWnd].Open}) /invoke ${Window[SpellBookWnd].DoClose}
		}		

		/varset dump ${out[18, "${eol}"]}  
		/echo
		/return		
	}

/return



|***
 * note: cursor control
 * use: /cursor
 ***| 
#bind control_cursor /cursor
sub Bind_control_cursor(string _type, string _verbage, bool _debug)
	DEBUG \atBind_control_cursor\ax(\a-w${_type}, ${_verbage}\ax)


	| output control map
	/if (${_type.Equal[see]}) {
		/if (!${Cursor.ID}) {
			/varset dump ${get_shade[FALSE, MQWindow]}
		} else {
			/sqlite ${maData.Find[DBloot].Value} lootquery SELECT * FROM loot_control WHERE item="${Cursor.Name}"
			/varset dump ${out[0, "DB settings for item${sep}\a-w${sqlite.Result[lootquery 1 item]}\ax"]}
			/varset dump ${out[0, "action${sep}\a-w${sqlite.Result[lootquery 1 action]}\ax"]}
			/varset dump ${out[0, "pass${sep}\a-w${sqlite.Result[lootquery 1 pass]}\ax"]}
			/varset dump ${out[0, "count${sep}\a-w${sqlite.Result[lootquery 1 count]}\ax"]}
			/varset dump ${out[18, "${eol}"]}
			/echo
		}
		/return

	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux cursor ${_verbage}
		/return		
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
		/sqlite ${maData.Find[DBloot].Value} lootquery UPDATE loot_control SET action="${_type.Lower}" WHERE item="${Cursor.Name}"
		DEBUG ${sep}lootquery UPDATE loot_control SET action="${_type.Lower}" WHERE item="${Cursor.Name}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
		/varset dump ${out[14, "Set ${dot}${Cursor.Name}${sep}\a-w${_type.Lower}\ax"]}

	} else /if (${_type.Equal[pass]}) {
		/sqlite ${maData.Find[DBloot].Value} lootquery UPDATE loot_control SET pass="${_verbage}" WHERE item="${Cursor.Name}"
		DEBUG ${sep}lootquery UPDATE loot_control SET pass="${_verbage}" WHERE item="${Cursor.Name}" \ay${sqlite.Status[lootquery]}${sep}${sqlite.Rows[lootquery]}\ax
		/varset dump ${out[14, "Set ${dot}pass${sep}\a-w${_verbage}\ax"]}

	} else /if (${_type.Equal[count]}) {

	
	} 
	
	/declare _out string local
	/declare _lsout list local
	/varset dump ${_lsout.Append[destroy,ignore,keep,leave,pass,count]}
  /varset _out
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/varset dump ${cli.Advance}
	}		
	/varset dump ${out[12, "/cursor ${_out}"]}
	
/return



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
		/call check_minion_cycle FALSE TRUE
		/return		

	} else /if (${_type.Equal[engage]}) {
		/if (!${set_control_num_range[${_debug}, stPctPetEngage, "${_verbage}", 0, 99]}) /return

	| spell for pet
	} else /if (${_type.Equal[type]}) {
		/varset dump ${do_raw_edit[FALSE, SILENT, stPetType, "${_verbage}"]}	
			
	| illusion for pet
	} else /if (${_type.Equal[illusion]}) {
		/varset dump ${do_raw_edit[FALSE, SILENT, stPetIllusion, "${_verbage}"]}	

	| enable pet use	
	} else /if (${_type.Equal[pet]}) {
		/varset dump ${set_switch_env[${_debug}, swPet, TRUE, ${_verbage}]}
	
	| enable swarm pets
	} else /if (${_type.Equal[swarm]}) {
		/varset dump ${set_switch_env[${_debug}, swSwarm, TRUE, ${_verbage}]}
		
	| set swarm pet engage
	} else /if (${_type.Equal[sengage]}) {
		/if (!${set_control_num_range[${_debug}, stPctSwarmEngage, "${_verbage}", 0, 99]}) /return

	| use swarm pets on mobs with a DS?
	} else /if (${_type.Equal[ds]}) {
		/varset dump ${set_switch_env[${_debug}, swNoSwarmonDS, TRUE, ${_verbage}]}
		
	| pet gear
	} else /if (${_type.Equal[gear]}) {
		/if (${_verbage.Equal[weapon]}) {
			/call do_raw_edit ${_debug} SILENT stPetWeapon "${_silent}"
		} else /if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swUsePetGear, TRUE, ${_silent}]}
		}
		
		OUT /minion gear${sep}
		OUT ${dot} ${info}use\ax:${If[${maMinion.Find[swUsePetGear].Value},${on},${off}]}
		OUT ${dot} ${info}weapon\ax:[${If[${Bool[${maMinion.Find[stPetWeapon].Value}]},${num}${maMinion.Find[stPetWeapon].Value}\ax,${off}]}]
		/return


	| send pets with a force command
	} else /if (${_type.Equal[onforce]}) {
		/varset dump ${set_switch_env[${_debug}, swonForce, TRUE, ${_verbage}]}
	
	| buff your pet?
	} else /if (${_type.Equal[buff]}) {
		/varset dump ${set_switch_env[${_debug}, swPetBuff, TRUE, ${_verbage}]}

	| pet shrinks
	} else /if (${_type.Equal[shrink]}) {
		/if (${_verbage.Equal[item]}) {
			/call do_raw_edit ${_debug} SILENT stPetShrink "${_silent}"
		} else /if (${_verbage.Equal[use]}) {
			/varset dump ${set_switch_env[${_debug}, swPetShrink, TRUE, ${_silent}]}
		}
		
	  /varset _out
		/varset dump ${_lsout.Append[use,item]}
		/vardata cli _lsout.First.Clone
		/while (!${cli.IsEnd}) {
			/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
			/if (${cli.Value.Equal[use]}) /varset _out ${_out}:${If[${maMinion.Find[swPetShrink].Value},${on},${off}]}
			/if (${cli.Value.Equal[item]}) /varset _out ${_out}:[${If[${Bool[${maMinion.Find[stPetShrink].Value}]},${num}${maMinion.Find[stPetShrink].Value}\ax,${off}]}]
			/varset dump ${cli.Advance}
		}	
		OUT /minion shrink ${_out}
		/varset dump ${out[12, ""]}
		/return

	} 
	
	/if (${_silent.Equal[SILENT]}) /return
	/varset dump ${_lsout.Append[pet,type,illusion,buff,shrink,engage,swarm,sengage,ds,gear,onforce]}
  /varset _out
	/vardata cli _lsout.First.Clone
	/while (!${cli.IsEnd}) {
		/varset _out ${_out} ${If[${cli.Value.Equal[${_lsout.First}]},,${dot}]} ${info}${cli.Value}\ax
		/if (${cli.Value.Equal[type]}) /varset _out ${_out}:[${If[${Bool[${maMinion.Find[stPetType].Value}]},${num}${maMinion.Find[stPetType].Value}\ax,${off}]}]
		/if (${cli.Value.Equal[illusion]}) /varset _out ${_out}:[${If[${Bool[${maMinion.Find[stPetIllusion].Value}]},${num}${maMinion.Find[stPetIllusion].Value}\ax,${off}]}]
		/if (${cli.Value.Equal[pet]}) /varset _out ${_out}:${If[${maMinion.Find[swPet].Value},${on},${off}]}
		/if (${cli.Value.Equal[buff]}) /varset _out ${_out}:${If[${maMinion.Find[swPetBuff].Value},${on},${off}]}
		/if (${cli.Value.Equal[shrink]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[engage]}) /varset _out ${_out}:${If[${maMinion.Find[stPctPetEngage].Value},${num}${maMinion.Find[stPctPetEngage].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[swarm]}) /varset _out ${_out}:${If[${maMinion.Find[swSwarm].Value},${on},${off}]}
		/if (${cli.Value.Equal[sengage]}) /varset _out ${_out}:${If[${maMinion.Find[stPctSwarmEngage].Value},${num}${maMinion.Find[stPctSwarmEngage].Value}\ax,${r0}]}
		/if (${cli.Value.Equal[ds]}) /varset _out ${_out}:${If[${maMinion.Find[swNoSwarmonDS].Value},${on},${off}]}
		/if (${cli.Value.Equal[gear]}) /varset _out ${_out}:${submenu}
		/if (${cli.Value.Equal[onforce]}) /varset _out ${_out}:${If[${maMinion.Find[swonForce].Value},${on},${off}]}
		/varset dump ${cli.Advance}
	}		
	OUT /minion ${_out}
	/varset dump ${out[12, ""]}

	
/return



|***
 * note: shortcut to force sub voodoo in a manual instance
 * use: /entcast [NAME] [TARGET] DEBUG
 ***|
#bind command_entcast /entcast
sub Bind_command_entcast(string _name, string _target, bool _debug)
	DEBUG \atBind_command_entcast\ax(\a-w${_name}, ${_target}\ax)

	/call voodoo ${_debug} "${_name}" ${_target} FALSE

/return









|***
 * note: builds your group
 * use: /crew [1-12]
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
sub Bind_command_crew(string _type)

	| output control map
	/if (${_type.Equal[see]}) {
		/call Bind_command_see maCrew
		/return
	} else /if (${_type.Equal[aux]}) {
		/call Bind_command_aux crew
		/return
	}

	| bail if no crew type listed
	/if (!${_type.Length} || !${Range.Between[1,12:${Int[${_type}]}]}) {
		OUT /crew [\a-w1\ax-\a-w12\ax]
		/varset dump ${out[12, ""]}
		/return
	}
	
	| crew # empty?
	/if (${lsCrew${_type}.Count} == 1 && ${lsCrew${_type}.First.Value.Equal[EMPTY]}) {
		OUT ${notice}${sep}lsCrew\a-w${_type}\ax is empty.
		/varset dump ${out[12, ""]}
		/return
	}
	
	/vardata cli lsCrew${_type}.First.Clone
	/while (!${cli.IsEnd}) {

		| skip me
		/if (${cli.Value.Equal[${Me.DisplayName}]}) {
			/varset dump ${cli.Advance}
			/continue
		}
	
		| are they part of our comm group?
		/if (${DanNet.Peers[${maComm.Find[stEntropyGroup_all].Value}].Find[${cli.Value}]}) {
	
			| do they exist?
			/dquery ${cli.Value} -q "Me.Height" -o DNQReturn -t 5
			
			/if (!${Bool[${DNQReturn}]}) {
				/varset dump ${cli.Advance}
				/continue
			}
			
			| send invite
			OUT Inviting${sep}\a-w${cli.Value}\ax
			/varset dump ${out[12, ""]}
			/invite ${cli.Value}
			/delay 1			
			/dexecute ${cli.Value} /invite
			/delay 1
		
			
			/varset dump ${cli.Advance}
		}	
	}
	
	| set group assist
	/if (${Bool[${maCrew.Find[stCrew${_crew}Assist].Value}]} && ${Group.Member[${maCrew.Find[stCrew${_crew}Assist].Value}].Index}) {
		/grouproles set ${maCrew.Find[stCrew${_crew}Assist].Value} 2
	}
	
	| set group looter
	/if (${Bool[${maCrew.Find[stCrew${_crew}Looter].Value}]} && ${Group.Member[${maCrew.Find[stCrew${_crew}Looter].Value}].Index}) {
		/grouproles set ${maCrew.Find[stCrew${_crew}Looter].Value} 5
	}
	
	| set group leader
	/if (${Bool[${maCrew.Find[stCrew${_crew}Leader].Value}]} && ${Group.Member[${Group.Leader}].Index} != ${Group.Member[${maCrew.Find[stCrew${_crew}Leader].Value}].Index}) {
		/makeleader ${maCrew.Find[stCrew${_crew}Leader].Value}
	}
	
	OUT Leader${sep}\a-w${Group.Leader}\ax ${dot} Assist:${sep}\a-w${Group.MainAssist}\ax ${dot} Looter${sep}\a-w${Group.MasterLooter}\ax
	/varset dump ${out[12, ""]}

/return
	


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
			OUT No Targets in${sep}\a-wPULLENV\ax
			/varset dump ${out[9, ""]}
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
			OUT NO Intervention pullers in range.
			/varset dump ${out[9, ""]}
		}	
	
	| pet pulling
	} else /if (${_type.Equal[pet]}) {
		/call get_name_multipull FALSE |MAG|BST|NEC|ENC|SHM|DRU
		/if (${Bool[${Macro.Return}]}) {
			/dex ${Macro.Return} /advancedpullnpet TARGET
		} else {
			OUT NO pet pullers in range.
			/varset dump ${out[9, ""]}
		}	
	
	| nav pulling
	} else /if (${_type.Equal[nav]}) {
		/call get_name_multipull FALSE |tank|melee
		/if (${Bool[${Macro.Return}]}) {
			/dex ${Macro.Return} /advancedpullnav TARGET
		} else {
			OUT NO melee nav pullers in range.
			/varset dump ${out[9, ""]}
		}	

	| lul pulling
	} else /if (${_type.Equal[lul]}) {
		| to be built still
	}

/return TRUE



|***
 * note: forces an intervention spell on that target
 * use: /intervention [TargetID] DEBUG
 ***|
#bind command_pull_int /advancedpullint
sub Bind_command_pull_int(int _tmpID, bool _debug)
	DEBUG \atBind_command_pull_int\ax(\a-w${_tmpID}\ax)
	
	/if (!AUTO) /return FALSE
	
	/if (!${Bool[${_tmpID}]}) {
		OUT ${notice}${sep}target required
		/varset dump ${out[9, ""]}
		/return FALSE
	}
	
	/declare _count int local 0
	/if (${Me.Class.ShortName.Equal[CLR]}) /varset _count 4
	| /if (${Me.Class.ShortName.Equal[DRU]}) /varset _count 4
	/if (${Me.Class.ShortName.Equal[SHM]}) /varset _count 1

	/while (${_count} > 0) {
		/if (${Me.SpellReady[${Spell[${healIntervention${_count}}].RankName}]}) {
			/if (${validate_cast[FALSE, spell, "${healIntervention${_count}}", ${_tmpID}]}) {
				/call core_cast2 "${healIntervention${_count}}" 0 ${_tmpID} FALSE
				/return TRUE
			}
		}
		/varcalc _count ${_count} - 1
	} 
	
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
sub Bind_control_grain(string _num)

	/varset dump ${out[12, "/grain aux for list"]}

	| output control map
	/if (${_num.Equal[see]}) {
		/call Bind_command_see lsGrain
		/return
	} else /if (${_num.Equal[aux]}) {
		/call Bind_command_aux grain
		/return
	}




/return FALSE