|*********************************************************************
 * entropy
 * initialize.inc
 * all shared/static data initilization
 *
 ***|





|***
 * note: the start of the end, all visual output formatting, debugs trash variables, verbosity
 * use: 
 ***|
sub set_initialize(bool _debug)
	DEBUG \atset_initialize\ax()

	| set inital trash variables	
	/call set_data FALSE FALSE dump 																		string outer FALSE
	/call set_data FALSE FALSE bindswap																	bool outer FALSE
	/call set_data FALSE FALSE swap 																		string outer FALSE
	/call set_data FALSE FALSE DNReturn																	string outer FALSE
	/call set_data FALSE FALSE swDBlocked																bool outer FALSE
	/call set_data FALSE FALSE DNQReturn																string outer FALSE

	/declare mi	mapiterator outer
	/declare li listiterator outer
	
	
	| initalize base verbosity (do not allow boot to be "hidden")
	| /call set_data FALSE FALSE stVerbLvl																int outer 5
	| /var dump ${maEnv.Add[verbosity,0]}
	
	| hook map
	/declare maHook map outer
	
	| buff beg map
	/declare maBuffRequest map outer
	
	| watchspawn map
	/declare maWatchspawn map outer
	
	/declare lsBlockedBuffsMe 																					string outer

	| list of all maps used with persistant data
	/declare lsMaps list outer
	/varset dump ${lsMaps.Append[maChr,maEnv,maHome,maCC,maHeal,maRez,maItems,maBuffs,maBuff,maBurn]}
	/varset dump ${lsMaps.Append[maEntropy,maOver,maDebug,maTimer,maCondition,madeBuff,maTie,maMinion,maCrew]}

	
	| set all output shortcuts
	/call set_data FALSE FALSE num																			string outer "\a-t"
	/call set_data FALSE FALSE submenu																	string outer "\a-t->\ax"
	/call set_data FALSE FALSE r0																				string outer "\ar0\ax"
	/call set_data FALSE FALSE error																		string outer "\arERROR\ax"
	/call set_data FALSE FALSE tar																			string outer "\a-w"
	/call set_data FALSE FALSE pc																				string outer "\a-w"
	/call set_data FALSE FALSE sp																				string outer "\a-r"
	/call set_data FALSE FALSE bad																			string outer "\ar"
	/call set_data FALSE FALSE good																			string outer "\ag"
	/call set_data FALSE FALSE bug																			string outer "\at"
	/call set_data FALSE FALSE info																			string outer "\aw"
	/call set_data FALSE FALSE sep																			string outer "\a-p::\ax"
	/call set_data FALSE FALSE dot																			string outer "\at.\ax"
	/call set_data FALSE FALSE on																				string outer "\agon\ax"
	/call set_data FALSE FALSE off																			string outer "\aroff\ax"
	/call set_data FALSE FALSE warning																	string outer "\arWARNING\ax"
	/call set_data FALSE FALSE fatal																		string outer "\arFATAL ERROR\ax \at..\ax \arENDING MACRO\ax"
	/call set_data FALSE FALSE caution																	string outer "\arCAUTION\ax"
	/call set_data FALSE FALSE true																			string outer "\arTRUE\ax"
	/call set_data FALSE FALSE false																		string outer "\arFALSE\ax"
	/call set_data FALSE FALSE destroy																	string outer "\arDESTROY\ax"
	/call set_data FALSE FALSE leave																		string outer "\awLeave\ax"
	/call set_data FALSE FALSE add																			string outer "\agAdd\ax"
	/call set_data FALSE FALSE keep																			string outer "\agKeep\ax"
	/call set_data FALSE FALSE notice																		string outer "\ayNotice\ax"
 	/call set_data FALSE FALSE combat																		string outer "\amCombat\ax"
	/call set_data FALSE FALSE remove																		string outer "\arRemoving\ax"
	/call set_data FALSE FALSE broadcast																string outer "Broadcast"	
	/call set_data FALSE FALSE auxnum																		string outer "[\a-w#\ax]"	
	/call set_data FALSE FALSE auxsw																		string outer "[\a-wswitch\ax]"	
	/call set_data FALSE FALSE auxna																		string outer "[\a-wname\ax]"	
	/call set_data FALSE FALSE auxli																		string outer "[\a-wlist\ax]"	
	/call set_data FALSE FALSE eol																			string outer "\at........\ax \a-gEND OF LINE\ax"	
	
	
	| generate DEBUGS
	/declare maDebug map outer
	/declare _lsout list local
	/varset dump ${_lsout.Delimiter["|"]}
  /varset dump ${_lsout.Append[build|buff|burn|mezz|cast|melee|pull|cure|heal|rest|rez]}

	/declare _li listiterator local
	/vardata _li _lsout.First
	/while (!${_li.IsEnd}) {
		/varset dump ${maDebug.Add[${_li.Value},FALSE]}
		/varset dump ${_li.Advance}
	}	

/return



|***
 * note: sets all list data. all of it. did I say all of it..
 * use: /call set_initial_environment [DEBUG]
 ***|
sub set_initial_environment(bool _debug)
	DEBUG \atset_initial_environment\ax()

	/declare _count int local 0

	| sin/cos generation for ranged movement/adjustment combat
	/declare cos[20] float outer
	/declare sin[20] float outer
	/for _count 1 to 20 
		/varcalc cos[${_count}] ${Math.Cos[9 * ${_count}]}
		/varcalc sin[${_count}] ${Math.Sin[9 * ${_count}]}
	/next _count	

	| yo' dawg, i heard you like lists. so heres a list of lists for your lists
	/call set_data FALSE FALSE lsClassAll																list outer WAR|PAL|SHD|BRD|RNG|BST|BER|MNK|ROG|WIZ|MAG|ENC|NEC|CLR|DRU|SHM
	/call set_data FALSE FALSE lsClassMelee    		  										list outer WAR|ROG|BST|MNK|SHD|PAL|BER|BRD|RNG
	/call set_data FALSE FALSE lsClassCast															list outer PAL|SHD|BRD|RNG|BST|WIZ|MAG|ENC|NEC|CLR|DRU|SHM
	/call set_data FALSE FALSE lsClassPet																list outer ENC|DRU|SHM|SHD|WIZ|BRD|BST|MAG|NEC
	/call set_data FALSE FALSE lsClassHeal															list outer CLR|DRU|SHM|PAL|RNG|BST
	/call set_data FALSE FALSE lsClassRez																list outer CLR|DRU|SHM|PAL|NEC
	/call set_data FALSE FALSE lsClassAASnare														list outer RNG|DRU|SHD|NEC|ROG|WAR|WIZ|MNK|PAL|BER
	/call set_data FALSE FALSE lsClassAAEradicate												list outer RNG|DRU|NEC|WIZ|ENC|MAG
	/call set_data FALSE FALSE lsClassCastCure													list outer CLR|DRU|SHM|BST|PAL|RNG
	/call set_data FALSE FALSE lsIVU 																		list outer "Group Perfected Invisibility to Undead"|"Perfected Invisibility to Undead"|"Invisibility to Undead"|"Invisibility versus Undead"|"Shauri's Sonorous Clouding"
	/call set_data FALSE FALSE lsIV 																		list outer "Shared Perfected Camouflage"|"Shared Camouflage"|"Camouflage"|"Group Perfected Invisibility"|"Invisibility"|"Perfected Invisibility"|"Phase March"|"Nethervoid March"|"Perfected Silent Presence"|"Cloak of Shadows"|"Gather Umbra"|"Invisibility Cloak"|"Perfected Natural Invisibility"|"Perfected Spirit Walk Invisibility"|"Perfected Group Spirit Walk Invisibility"|"Shauri's Sonorous Clouding"|"Selo's Song of Travel"
	/call set_data FALSE FALSE lsAssistTypes														list outer g|ma1|ma2|1|2|3
	/call set_data FALSE FALSE lsPullTypes															list outer base|pet|int|nav|multi
	/call set_data FALSE FALSE lsRezBuffTriggers												list outer "Revival Sickness"|"Resurrection Sickness"

	/call set_data FALSE FALSE lsNoAttackBodyType												list outer AURA|BANNER|CAMPFIRE|CORPSE|CHEST|ITEM|TRIGGER|TRAP|TIMER|MOUNT|MERCENARY|OBJECT
	/call set_data FALSE FALSE lsNoTargetNPC														list outer "a campfire"|"a corpse"|"controller"
	/call set_data FALSE FALSE lsNoTargetPC															list outer "eye of"
	/call set_data FALSE FALSE lsNoTargetMagic													list outer "eye of"

	| spell types we let through the checking/validation methods
	/call set_data FALSE FALSE lsFreePassSpells													list outer Beneficial|Pet

	| bandolier setting
	/call set_data FALSE FALSE lsBandolier															list outer "1h|2h|range|no"


	| timers  for everyone
	/varset dump ${set_data_timer[FALSE, Try_Door, 4s]}
	/varset dump ${set_data_timer[FALSE, Check_Ammo, 5s]}
	/varset dump ${set_data_timer[FALSE, Nuke_Delay, 1]}
	/varset dump ${set_data_timer[FALSE, Check_Cursor, 5s]}
	/varset dump ${set_data_timer[FALSE, Check_Zone_ID, 10s]}
	/varset dump ${set_data_timer[FALSE, Check_Observers, 1m]}
	/varset dump ${set_data_timer[FALSE, Check_Poison, 1m]}
	/varset dump ${set_data_timer[FALSE, Check_deBuffs, 6s]}
	/varset dump ${set_data_timer[FALSE, Check_Behind, 10s]}


	| /varset dump ${set_data_timer[FALSE, Check_Buff_List_Population, 2s]}
	


	| /call timer_set FALSE tCheck_Runes																	string		outer	10s		timer_check_runes
	| /call timer_set FALSE tScorchedEarth_ignore													string		outer	30s		timer_scorched_earth_ignore
	| /call timer_set FALSE tScorchedEarth_fail														string		outer	1m 		timer_auto_cursor_pause
	| /call timer_set FALSE tCheck_Runes																	string		outer	10s		timer_check_runes
	| /call timer_set FALSE tScorchedEarth_fail														string		outer	1m 		timer_scorched_earth_fail
	| /call timer_set FALSE tScorchedEarth_delay													string		outer	5s 		timer_ScorchedEarth_delay
	| /call timer_set FALSE tCheck_Familiar																string		outer	30s		timer_Check_Familiar
	| /call timer_set FALSE tHarvest_delay																string		outer	1s 		timer_Harvest_delay
	| /call timer_set FALSE tWatch_Spawn																	string		outer	1s 		timer_check_watch_spawn
	| /call timer_set FALSE tPetfarm_delay																string		outer	1s 		timer_Petfarm_delay	
	| /call timer_set FALSE tMGB_Buff																			string		outer	5m 		timer_check_buffs_mgb                                   										
	| /call timer_set FALSE tHunt_delay																		string		outer	30s 	timer_wait_for_mob_to_kill
	| /call timer_set FALSE tHUD_Death_Check															string		outer	10s		timer_HUD_death_check
	| /call timer_set FALSE tMode_Open																		string		outer 1s		timer_mode_open
	| /call timer_set FALSE tCheck_status																	string		outer	2s 		timer_check_status

	
	| controls for all classes
	/call set_data FALSE FALSE lsChr																		list outer ae|def|fade|spire
 	
	| controls for heal classes
	/if (${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
	}

	| controls for mana users
	/if (${lsClassCast.Contains[${Me.Class.ShortName}]}) {
		| /call set_data FALSE FALSE lsChr																	list outer manaclick
	  /varset dump ${lsChr.Append[manaclick]}
	}

	| controls by class
	/if (${Select[${Me.Class.ShortName},BER]}) {
	} else /if (${Select[${Me.Class.ShortName},BRD]}) {
	  /varset dump ${lsChr.Append[line|rest|refresh]}
	} else /if (${Select[${Me.Class.ShortName},BST]}) {
	  /varset dump ${lsChr.Append[consume|parafocus|paragroup]}
	} else /if (${Select[${Me.Class.ShortName},CLR]}) {
	  /varset dump ${lsChr.Append[arbitrate|divinerez|groupheal|grouphot|hot|issuance|primary|splash|vp|yaulp]}
	} else /if (${Select[${Me.Class.ShortName},DRU]}) {
	  /varset dump ${lsChr.Append[wolf|groupheal]}
	} else /if (${Select[${Me.Class.ShortName},ENC]}) {
	  /varset dump ${lsChr.Append[charm|gather|bos|crystal|charm]}
	} else /if (${Select[${Me.Class.ShortName},MAG]}) {
		/varset dump ${lsChr.Append[line|gather|forcetc]}
	} else /if (${Select[${Me.Class.ShortName},MNK]}) {
	} else /if (${Select[${Me.Class.ShortName},NEC]}) {
	} else /if (${Select[${Me.Class.ShortName},PAL]}) {
	  /varset dump ${lsChr.Append[attraction|harmonious|loh|mg|flash|stun|yaulp]}
	} else /if (${Select[${Me.Class.ShortName},RNG]}) {
	  /varset dump ${lsChr.Append[summer]}
	} else /if (${Select[${Me.Class.ShortName},ROG]}) {
	  /varset dump ${lsChr.Append[sos|blind]}
	} else /if (${Select[${Me.Class.ShortName},SHD]}) {
	  /varset dump ${lsChr.Append[attraction|harmonious|vot|flash|skin|stance]}
	} else /if (${Select[${Me.Class.ShortName},SHM]}) {
	} else /if (${Select[${Me.Class.ShortName},WAR]}) {
	  /varset dump ${lsChr.Append[attraction|flash]}
	} else /if (${Select[${Me.Class.ShortName},WIZ]}) {
	  /varset dump ${lsChr.Append[line|mb|gather|concussion]}
	}

	
	| set current zone
	/call set_data FALSE maEnv stCurrentZoneID													map outer FALSE

	| Comm	
	/if (${swDanNet}) {
		/call set_data FALSE maComm stLocalEcho															map outer FALSE							"dannet local echo"
		/call set_data FALSE maComm stCommandEcho														map outer FALSE							"dannet local command echo"
		/call set_data FALSE maComm stFrontDelim														map outer ON								"dannet lead from delimiters"
		/call set_data FALSE maComm stFullNames															map outer FALSE							"dannet fullnames"


		| set DanNet groups
		/declare lsEntropyGroup list outer
		/varset dump ${lsEntropyGroup.Delimiter["|"]}

	  /varset dump ${lsEntropyGroup.Append[all|1|2|3|4|5|6|7|8|9|10|11]}

		/declare _li listiterator local
		/vardata _li lsEntropyGroup.First
		/while (!${_li.IsEnd}) {
			/varset dump ${maComm.Add[stEntropyGroup_${_li.Value},entropy${_li.Value}]}
			/varset dump ${_li.Advance}
		}	

		| join the group
		/djoin ${maComm.Find[stEntropyGroup_all].Value}
	}
	


/return



|***
 * note: generate the zone file DB
 * use: 
 ***|
sub sql_set_zone_schema(bool _debug)
	DEBUG \atsql_set_zone_schema\ax()
	
	/varset swDBlocked TRUE

	| zone fields
	/declare lsZoneProperty list outer
	/varset dump ${lsZoneProperty.Delimiter["|"]}
	/varset dump ${lsZoneProperty.Append[lsZoneNamed|lsZoneExclude|lsZoneHunt|lsZoneCharm|lsZoneHarvest|lsZoneRange|lsZoneNoMez|lsZoneNoCast]}
	/varset dump ${lsZoneProperty.Append[lsZoneNoMelee|lsZoneNoFire|lsZoneNoPoison|lsZoneNoMagic|lsZoneNoCorruption|lsZoneNoCold|lsZoneNoDisease]}

  /declare _li listiterator local
	/vardata _li lsZoneProperty.First
	/while (!${_li.IsEnd}) {
		/declare ${_li.Value} list outer
		/varset dump ${_li.Advance}
	}

	/sqlite ${maData.Find[DBzone].Value} dummy SELECT build_number FROM build
	DEBUG ${sep}SELECT build_number FROM build \ay${sqlite.Status[dummy]}\ax

	/if (${sqlite.Result[dummy 1 build_number].Equal[${maEnv.Find[build].Value}]}) {
		DEBUG Builds match Bailing
		/return TRUE

	} else /if (${sqlite.Status[dummy].Find[no such table:]}) {
		/while (${swDBlocked}) {

			/sqlite ${maData.Find[DBzone].Value} dummy CREATE TABLE IF NOT EXISTS 'build' ('build_number' TEXT NOT NULL PRIMARY KEY)
			DEBUG ${sep}CREATE TABLE IF NOT EXISTS 'build' \ay${sqlite.Status[dummy]}\ax

			/sqlite ${maData.Find[DBzone].Value} dummy INSERT INTO 'build' ('build_number') VALUES ("${maEnv.Find[build].Value}")
			DEBUG ${sep}INSERT INTO 'build' \ay${sqlite.Status[dummy]}\ax

			/sqlite ${maData.Find[DBzone].Value} dummy CREATE TABLE 'zone_control' ('zone_shortname' TEXT NOT NULL,'element' TEXT NOT NULL,'value' TEXT NOT NULL,PRIMARY KEY (zone_shortname,element,value))
			DEBUG ${sep}CREATE TABLE 'zone_control' \ay${sqlite.Status[dummy]}\ax

			/delay 1
			/if (!${sqlite.Status[dummy].Find[locked]}) {
				/delay 1
				/varset swDBlocked FALSE
			}
		}			

	} else 	/if (${sqlite.Result[dummy 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {
		/while (${swDBlocked}) {
			/sqlite ${maData.Find[DBzone].Value} dummy UPDATE build SET build_number='${maEnv.Find[build].Value}'
			/delay 1
			/if (!${sqlite.Status[dummy].Find[locked]}) {
				/delay 1
				/varset swDBlocked FALSE
			}
		}			
	}

	/return



|***
 * note: generate the zone file DB
 * use: 
 ***|
sub sql_set_loot_schema(bool _debug)
	DEBUG \atsql_set_loot_schema\ax()
	
	/varset swDBlocked TRUE

	/sqlite ${maData.Find[DBloot].Value} dummy SELECT build_number FROM build
	DEBUG ${sep}SELECT build_number FROM build \ay${sqlite.Status[dummy]}\ax

	/if (${sqlite.Result[dummy 1 build_number].Equal[${maEnv.Find[build].Value}]}) {
		DEBUG Builds match Bailing
		/return TRUE

	} else /if (${sqlite.Status[dummy].Find[no such table:]}) {
		/while (${swDBlocked}) {

			/sqlite ${maData.Find[DBloot].Value} dummy CREATE TABLE IF NOT EXISTS 'build' ('build_number' TEXT NOT NULL PRIMARY KEY)
			DEBUG ${sep}CREATE TABLE IF NOT EXISTS 'build' \ay${sqlite.Status[dummy]}\ax

			/sqlite ${maData.Find[DBloot].Value} dummy INSERT INTO 'build' ('build_number') VALUES ("${maEnv.Find[build].Value}")
			DEBUG ${sep}INSERT INTO 'build' \ay${sqlite.Status[dummy]}\ax

			/sqlite ${maData.Find[DBloot].Value} dummy CREATE TABLE 'loot_control' ('item' TEXT NOT NULL,'action' TEXT NOT NULL,'pass' TEXT NOT NULL,'count' TEXT NOT NULL)
			DEBUG ${sep}CREATE TABLE 'loot_control' \ay${sqlite.Status[dummy]}\ax

			/delay 1
			/if (!${sqlite.Status[dummy].Find[locked]}) {
				/delay 1
				/varset swDBlocked FALSE
			}
		}			

	} else 	/if (${sqlite.Result[dummy 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {
		/while (${swDBlocked}) {
			/sqlite ${maData.Find[DBloot].Value} dummy UPDATE build SET build_number='${maEnv.Find[build].Value}'
			/delay 1
			/if (!${sqlite.Status[dummy].Find[locked]}) {
				/delay 1
				/varset swDBlocked FALSE
			}
		}			
	}

/return



|***
 * note: generate character file DB
 * use: 
 ***|
sub sql_set_character_schema(bool _debug)
	DEBUG \atsql_set_character_schema\ax()
	
	/declare maDBTables map outer
	/varset dump ${maDBTables.Add[environment_buffs,env_var]}
	/varset dump ${maDBTables.Add[environment_character,env_var]}
	/varset dump ${maDBTables.Add[environment_condition,env_var]}
	/varset dump ${maDBTables.Add[environment_timer,env_timer]}
	/varset dump ${maDBTables.Add[environment_items,env_var]}
	/varset dump ${maDBTables.Add[environment_crew,env_var]}

	/sqlite ${maData.Find[DBcharacter].Value} SQL_ENV_BUILD SELECT build_number FROM build
	DEBUG ${sep}SELECT build_number FROM build \ay${sqlite.Status[dummy]}\ax
	| keep a running copy of the previous build number
	/varset dump ${maData.Add[stPreviousBuild,${sqlite.Result[SQL_ENV_BUILD 1 build_number]}]}
	DEBUG ${sep}BUID${sep}\a-w${sqlite.Result[SQL_ENV_BUILD 1 build_number]}\ax
	
	/if (${sqlite.Result[SQL_ENV_BUILD 1 build_number].Equal[${maEnv.Find[build].Value}]}) {
		| /return TRUE
	} else /if (${sqlite.Status[SQL_ENV_BUILD].Find[no such table:]} || ${sqlite.Result[SQL_ENV_BUILD 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {

		/if (${sqlite.Status[SQL_ENV_BUILD].Find[no such table:]}) {
			| character build match
			/sqlite ${maData.Find[DBcharacter].Value} dummy CREATE TABLE IF NOT EXISTS 'build' ('build_number' TEXT NOT NULL PRIMARY KEY)
			DEBUG ${sep}CREATE TABLE IF NOT EXISTS 'build' \ay${sqlite.Status[dummy]}\ax
			/sqlite ${maData.Find[DBcharacter].Value} dummy INSERT INTO 'build' ('build_number') VALUES ("${maEnv.Find[build].Value}") 
			DEBUG ${sep}INSERT INTO 'build' \ay${sqlite.Status[dummy]}\ax

		} else 	/if (${sqlite.Result[SQL_ENV_BUILD 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {
			/sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE build SET build_number='${maEnv.Find[build].Value}'
			DEBUG ${sep}CREATE TABLE IF NOT EXISTS 'build' \ay${sqlite.Status[dummy]}\ax
		}

		/declare _mi mapiterator local
		/vardata _mi maDBTables.First
		/while (!${_mi.IsEnd}) {
			/sqlite ${maData.Find[DBcharacter].Value} dummy CREATE TABLE IF NOT EXISTS '${_mi.Key}' ('${_mi.Value}' TEXT NOT NULL PRIMARY KEY,'value' TEXT NOT NULL, 'aux' TEXT)
			DEBUG ${sep}CREATE TABLE IF NOT EXISTS '${_mi.Key}' \ay${sqlite.Status[dummy]}\ax
			/varset dump ${_mi.Advance}
		}	
	}
		
/return



|***
 * note: 
 * use: 
 ***|
sub set_initialize_map_character_environment(bool _debug)

	/declare _li listiterator local
	/declare _count int local 0
	
|** data **|
	/call set_data FALSE maData stCastReturn 														map outer FALSE

| output granularity
	/call set_data_list FALSE maEnv lsGrain														1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30				"${auxnum} visual outputs you wish to see. \aw/verb aux\ax for full list"

|** control: env **|
	/call set_data FALSE maEnv swAuto 																	map outer FALSE								
	/call set_data_map FALSE maEnv swSafeListGuild 											FALSE													"${auxsw} let guild in your safe list"
	/varset dump ${set_data_timer[FALSE, Check_EXP_Adjust, 5m]}
	/call set_data_map FALSE maEnv stMobLvlMin 													1															"${auxnum} lowest level mob you will attack"
	/call set_data_map FALSE maEnv stMobLvlMax 													500														"${auxnum} highest level mob you will attack"
	/call set_data_map FALSE maEnv swNukeLoop 													FALSE													"${auxsw} force a nuke loop to complete before releasing"

	/call set_data_map FALSE maEnv swADVLoot 														FALSE													"${auxsw} use advanced loot functions"
	/call set_data_map FALSE maEnv swAutoInv 														FALSE													"${auxsw} use autoinventory loot control on cursor items"
	/call set_data_list FALSE maEnv lsAutoInventory											EMPTY													"${auxli} list of shit to auto inventory"
	/call set_data_map FALSE maEnv stBuffGem 														${Me.NumGems}									"${auxnum} gem slot to use as a swap for buffing"
	/call set_data_map FALSE maEnv swHoldBuffGem 												FALSE													"${auxsw} remember and return the spell used in that slot before buffing"
	/call set_data_map FALSE maEnv stEnvRadius													60														"${auxnum} radius you are willing to operate in"
	/call set_data_map FALSE maEnv stEnvSafeRadius											30														"${auxnum} radius you consider safe even with mobs in it"
	/call set_data_map FALSE maEnv stLvlPct   													99														"${auxnum} % of exp in a level you want to maintain"
	/call set_data_map FALSE maEnv stLvlMax 		   											110														"${auxnum} level you want to maintain"
	/call set_data_map FALSE maEnv swCorrectEXPAA												FALSE													"${auxsw} use exp/aa adjust functions"

	/varset dump ${set_data_timer[FALSE, Waste_Fireworks, 15m]}
	/call set_data_map FALSE maEnv swAAFireworks												FALSE													"${auxsw} waste aa on fireworks"

	/call set_data_map FALSE maEnv swTakeGroupInvite										FALSE													"${auxsw} take group invites from people you trust"
	/call set_data_map FALSE maEnv swTakeRaidInvite											FALSE													"${auxsw} take raid invites from people you trust"
	/call set_data_map FALSE maEnv swSoS																FALSE													"${auxsw} rogue sos switch"
	/call set_data_list FALSE maEnv lsSafeListToon											EMPTY													"${auxli} list of people you trust"

	| rest
	/call set_data_map FALSE maEnv stPctRest	 													0															"${auxnum} % health/mana to rest at"
	/call set_data_map FALSE maEnv swRestFull														FALSE													"${auxsw} rest to full health if possible"
	/call set_data_map FALSE maEnv swRestModRod													FALSE													"${auxsw} use mod rods while resting"
	/call set_data_map FALSE maEnv swRestBuff														FALSE													"${auxsw} do buffing while resting"
	/call set_data_map FALSE maEnv swRestCombat													FALSE													"${auxsw} continue resting whilke in combat"
	/call set_data_map FALSE maEnv swBegMAG															FALSE													"${auxsw} beg mages for pet gear or rods"

	| food and drink
	/varset dump ${set_data_timer[FALSE, Check_FoodDrink, 10m]}
	/call set_data_map FALSE maEnv stFood																FALSE													"${auxna} food item you want to eat"
	/call set_data_map FALSE maEnv stDrink															FALSE													"${auxna} drink item you want to drink"


|** control: heal **|
	/varset dump ${set_data_timer[FALSE, Check_Cure, 6s]}
	/call set_data_map FALSE maHeal stHealAdjust												0															"${auxnum} % to overide max health to"
	/call set_data FALSE FALSE tmpHealAdjust														float outer 99.99	
	/call set_data_map FALSE maHeal swHealSelf													FALSE													"${auxsw} enable self healing"
	/call set_data_map FALSE maHeal swHealGroup													FALSE													"${auxsw} enable group healing"
	/call set_data_map FALSE maHeal swHealXTarget												FALSE													"${auxsw} enable xtarget healing"
	/call set_data_map FALSE maHeal swHealPet														FALSE													"${auxsw} enable pet healing"
	/call set_data_map FALSE maHeal swHealCure													FALSE													"${auxsw} enable curing"
	/call set_data_map FALSE maHeal stCureCurseItem											FALSE													"${auxna} name of item to cure self curses"
	/call set_data_map FALSE maHeal stCurePoisonItem 										FALSE													"${auxna} name of item to cure self poisons"
	/call set_data_map FALSE maHeal stCureDiseaseItem										FALSE													"${auxna} name of item to cure self diseases"
	/call set_data_map FALSE maHeal stCureCorruptionItem								FALSE													"${auxna} name of item to cure self corruption"
	/call set_data_list FALSE maHeal lsNoCureDeBuff											EMPTY													"${auxli} list of debuffs you DO NOT want to cure"
	/call set_data_list FALSE maHeal lsHealItem													EMPTY													"${auxli} click heal items you want to waste time using"
	/call set_data_list FALSE maHeal lsHealXTClass											EMPTY													"${auxli} classes to populate xtarget for healing use: \a-t/heal xtarget\ax"
	/call set_data_list FALSE maHeal lsNoHealToon												EMPTY													"${auxli} fuck these people.. no heals for them.."

	/call set_data FALSE FALSE hurtID 																	int outer 0
	/call set_data FALSE FALSE hurtPctHP 																int outer 100
	/call set_data FALSE FALSE hurtCLS 																	string outer FALSE
	/call set_data FALSE FALSE hurtCount 																int outer 0
	/if (${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
		/call set_data_map FALSE maHeal stHealPointPanic									80														"${auxnum} % of health to use panic heal"
		/call set_data_map FALSE maHeal stHealPointHoT										80														"${auxnum} % of health to use single heal over time"
		/vardata _li lsClassAll.First
		/while (!${_li.IsEnd}) {
			/call set_data_map FALSE maHeal stHealPoint${_li.Value}					80														"${auxnum} % of health to start healing:\a-w${_li.Value}\ax"
			/varset dump ${_li.Advance}
		}
	/call set_data_map FALSE maHeal stHealMode													base													"[\a-wbase\ax|\a-wpassive\ax|\a-waggressive\ax] heal mode to use"
	

	/call set_data_list FALSE maHeal lsProtectOutsideGroup							EMPTY													"${auxli} names outside of your group to add to xtarget for healing"
	/call set_data_list FALSE maHeal lsProtectInsideRaid								EMPTY													"${auxli} names in a raid to add to your xtarget healing (\a-wbeyond lsHealXTClass\ax)"
	/call set_data_list FALSE maHeal lsProtectNPCList										EMPTY													"${auxli} npcs to add to xtarget list for healing"
		
	} else /if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
		/call set_data_map FALSE maHeal stHealPoint${Me.Class.ShortName} 	60
	}

	/if (${lsClassCastCure.Contains[${Me.Class.ShortName}]}) {
 		/call set_data_map FALSE maHeal stCureCurse 											FALSE													"${auxna} name of cure curse spell"
		/call set_data_map FALSE maHeal stCurePoison 											FALSE													"${auxna} name of cure poison spell"
		/call set_data_map FALSE maHeal stCureDisease 										FALSE													"${auxna} name of cure disease spell"
		/call set_data_map FALSE maHeal stCureCorruption									FALSE													"${auxna} name of corruption cure spell"
	} 


|** control: buff **|
	/varset dump ${set_data_timer[FALSE, Check_Buff_Cycle, 1m]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Unity, 5m]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Shrink, 10m]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Self, 1m]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Crew, 1m]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Beg, 1m]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Temp, 30s]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Now, 5s]}
	/varset dump ${set_data_timer[FALSE, Check_Buff_Powersource, 10m]}
	/call set_data_list FALSE maBuff lsBuffRemove												EMPTY													"${auxli} buffs you want auto removed from you"
	/call set_data FALSE maBuff swBuffMaster														map outer TRUE
	/call set_data_map FALSE maBuff swBuffSelf													FALSE													"${auxsw} buff yourself"
	/call set_data_map FALSE maBuff swBuffCrew													FALSE													"${auxsw} buff your crew (\aynot you\ax)"
	/call set_data_map FALSE maBuff swBuffRaid													FALSE													"${auxsw} buff raid members"
	/call set_data_map FALSE maBuff swBuffBeg														FALSE													"${auxsw} answer buff beg requests"
	/call set_data_map FALSE maBuff swBuffUnity													FALSE													"${auxsw} buff aa unity"
	/call set_data_map FALSE maBuff swBuffPowersource										FALSE													"${auxsw} use powersources (\a-wmust have buff set with tag\ax)"
	/call set_data_map FALSE maBuff swBuffTemp													FALSE													"${auxsw} buff short term combat buffs"
	/call set_data_map FALSE maBuff swBuffShrink												FALSE													"${auxsw} buff shring"
	/call set_data_map FALSE maBuff stShrinkItem												FALSE													"${auxna} buff shrink item/spell"
	/call set_data_map FALSE maBuff swBuffNow														FALSE													"${auxsw} buff the now things"

	| auras
	/varset dump ${set_data_timer[FALSE, Check_Buff_Aura, 3m]}
	/call set_data_map FALSE maBuff swBuffAura													FALSE													"${auxsw} use auras or not. i dont care"
	/for _count 1 to 2
		/call set_data_map FALSE maBuff stAura${_count} 									FALSE													"${auxna} name of aura"
		/call set_data_map FALSE maBuff stAura${_count}Check							FALSE													"${auxna} name of aura that appears in aura buff window"
		/call set_data_list FALSE maBuff lsAura${_count}Tag								EMPTY													"${auxna} tags for your aura"
	/next _count

	
|** control: rez **|
	/call set_data FALSE FALSE swIsMeDead																bool outer FALSE
	/call set_data FALSE FALSE lsRezPct 																list outer "10|20|35|50|60|75|90|93|96"
	/call set_data_list FALSE maRez lsNoRezToon													EMPTY													"${auxli} names of people you never want to rez"
	/call set_data_list FALSE maRez lsRezTokenToon											EMPTY													"${auxli} list of class shortnames you want to use a rez token on"
	/call set_data_map FALSE maRez swRezGive	 													FALSE													"${auxsw} rez people"
	/call set_data_map FALSE maRez swRezTake 														FALSE													"${auxsw} take rezes"
	/call set_data_map FALSE maRez swRezCombat 													FALSE													"${auxsw} rez duting combat (inc using tokens if enabled)"
	/call set_data_map FALSE maRez swRezEveryone 												FALSE													"${auxsw} rez people not in your group/raid/guild"  
	/call set_data_map FALSE maRez swRezToken														FALSE													"${auxsw} use rez tokens (\a-wseparate from stResurrection\ax)"
	/call set_data_map FALSE maRez stPctMinRez													90														"${auxnum} minimum % rez you are willing to accept"
	/call set_data_map FALSE maRez stMaxRezRange				 								60 														"${auxnum} how far away to look for corpses to rez"
	/call set_data_map FALSE maRez stResurrection												FALSE													"${auxna} spell/item/aa to rez with"


|** control: home **|
	/call set_data FALSE maHome swHomeMarker 														map outer FALSE
	/call set_data_map FALSE maHome stHomeVariance 											6															"${auxnum} acceptable buffer range for being home"
	/call set_data FALSE maData stCampX 																map outer	999999999
	/call set_data FALSE maData stCampY																	map outer	999999999
	/call set_data FALSE maData stCampZ 																map outer	999999999
	/call set_data_map FALSE maHome stMount 														FALSE													"${auxna} name of mount you wish to sit your fat ass on"
	/call set_data_map FALSE maHome swMoveDeathClear										FALSE													"${auxsw} clear home marker and movements on death"
	/call set_data_map FALSE maHome swMoveBehind												FALSE													"${auxsw} move behind the target during combat"
	/call set_data_map FALSE maHome swFaceFast 													FALSE													"${auxsw} use fast facing movements"
	/call set_data_map FALSE maHome stFaceArc 													15														"${auxnum} [\a-w1-45\az] acceptable range of facing and still be on target"
	/call set_data FALSE maHome swSnapStuck 														map outer	FALSE


|** control: tie **|
	/varset dump ${set_data_timer[FALSE, Check_Tie, 5]}
	/call set_data FALSE maTie swSetTie 																map outer FALSE
	/call set_data_map FALSE maTie stTieToon 														FALSE													"${auxna} name of toon you want to tie too when commanded"
	/call set_data_map FALSE maTie stTieMode 														nav														"${auxna} [\a-wstick-nav\ax] the mode you want to use for tie movements"
	/call set_data_map FALSE maTie stTieVariance 												5															"${auxnum} acceptabe distances to be away from the tiem toon before moving to it"
	

|** control: cc **|
	/call set_data FALSE maCC swSetCombat																map outer FALSE
	/call set_data FALSE FALSE stAssTarID 															int outer 0
	/call set_data FALSE maCC swValidAssTarID 													map outer FALSE
	/call set_data_map FALSE maCC swCombatMelee													FALSE													"${auxsw} do melee things while in combat"
	/call set_data_map FALSE maCC swCombatRange													FALSE													"${auxsw} do range things while in combat"
	/call set_data_map FALSE maCC swCombatNuke													FALSE													"${auxsw} nuke things while in combat"
	/call set_data_map FALSE maCC swCombatDoT														FALSE													"${auxsw} dot all the things!!"
	/call set_data_map FALSE maCC swCombatHeal													FALSE													"${auxsw} enable heal modes in or out of combat"
	/call set_data_map FALSE maCC swCombatControl												FALSE													"${auxsw} enable crowd control modes in or out of combat"
	
	/call set_data_map FALSE maCC swPull																FALSE													"${auxsw} enable pulling (\a-wselect mobs to kill\ax)"
	/call set_data_map FALSE maCC stPullMode														base													"${auxna} [\a-wbase-nav-int-pet-multi\ax] the mode of pulling you wish to use while looking for said mobs"
	/call set_data_map FALSE maCC stPullRadius													0															"${auxnum} radius to pull mobs from"
	/call set_data_map FALSE maCC swPullSetHome													FALSE													"${auxsw} suto set home marker when pulling is enabled"

	/call set_data_map FALSE maCC swPoison															FALSE													"${auxsw} use poisons (\a-wsetup in buffs for summoning/clicking\ax)"
	/call set_data_map FALSE maCC swBandolier														FALSE													"${auxsw} (\a-w1/aux bandolier\ax for help" 

	| glyph
	/call set_data_map FALSE maCC swGlyphGroup													FALSE													"${auxsw} use glyph in a group"
	/call set_data_map FALSE maCC swGlyphRaid														FALSE													"${auxsw} use glyph in a raid"
	/call set_data_map FALSE maCC stGlyph																FALSE													"${auxna} name of the glyph you want to use"


	| agro on or off and current agro mode (g,1,2,3,ma1,ma2)
	/call set_data_map FALSE maCC swAgro																FALSE													"${auxsw} use any agro skills you can (\a-wtaunt not included separate control under /cc\ax)"
	/call set_data_map FALSE maCC stAssistMode													FALSE													"${auxna} [\axg,1,2,3,ma1,ma2\ax] your current assist mode"
	/call set_data_map FALSE maCC stManualAssist1												FALSE													"${auxna} name of manual assist 1 toon"
	/call set_data_map FALSE maCC stManualAssist2			 									FALSE													"${auxna} name of manual assist 2 toon"
	/call set_data_map FALSE maCC stPctEngage	 													98														"${auxnum} mobs % health to engage it"

	| combat melee skills by class
	/declare lsMeleeSkill list outer 
	/if (${Select[${Me.Class.ShortName},CLR,PAL,WAR]}) {
		/varset dump ${lsMeleeSkill.Append[bash]}
		/call set_data_map FALSE maCC swBash															FALSE													"${auxsw} switch for bashing the shit out of things"
	}
	/if (${Select[${Me.Class.ShortName},WAR,BRD,RNG,BST,MNK]}) {
		/varset dump ${lsMeleeSkill.Append[kick]}
		/call set_data_map FALSE maCC swKick															FALSE													"${auxsw} raise foot, impact nuts.."
	}
	/if (${Select[${Me.Class.ShortName},BRD,ROG,BER]}) {
		/varset dump ${lsMeleeSkill.Append[intimidation]}
		/call set_data_map FALSE maCC swIntimidation											FALSE													"${auxsw} you think you're all that.. you aint nothing.."
	}
	/if (${Select[${Me.Class.ShortName},BER]}) {
		/varset dump ${lsMeleeSkill.Append[frenzy]}
		/call set_data_map FALSE maCC swFrenzy														FALSE													"${auxsw} some odd form of ocd for zerkers.. who knows."
	}
	/if (${Select[${Me.Class.ShortName},BRD,BER,PAL,SHD,WAR,RNG]}) {
		/varset dump ${lsMeleeSkill.Append[disarm]}
		/call set_data_map FALSE maCC swDisarm														FALSE													"${auxsw} dodge, parry, thrust.."
	}
	/if (${Select[${Me.Class.ShortName},ROG]}) {
		/varset dump ${lsMeleeSkill.Append[backstab]}
		/call set_data_map FALSE maCC swBackstab													FALSE													"${auxsw} surprise butt sekx!"
	}
	/if (${Select[${Me.Class.ShortName},BST,MNK]}) {
		/varset dump ${lsMeleeSkill.Append[dragonpunch,eaglestrike,tigerclaw,flyingkick,roundkick]}
		/call set_data_map FALSE maCC swDragonpunch												FALSE													"${auxsw} use dragon punch"
		/call set_data_map FALSE maCC swEaglestrike												FALSE													"${auxsw} use eagle strike"
		/call set_data_map FALSE maCC swTigerclaw													FALSE													"${auxsw} use tiger claw"
		/call set_data_map FALSE maCC swFlyingkick												FALSE													"${auxsw} use flying kick"
		/call set_data_map FALSE maCC swRoundkick													FALSE													"${auxsw} use round kick"
	}	
	/if (${Select[${Me.Class.ShortName},PAL,WAR,SHD,RNG]}) {
		/varset dump ${lsMeleeSkill.Append[taunt]}
		/call set_data_map FALSE maCC swTaunt															FALSE													"${auxsw} you mama is sooo fat!"
	}

	/call set_data_map FALSE maCC stZRadius															60														"${auxnum} the z distance of your universe"
	/call set_data_map FALSE maCC swBandolier														FALSE													"${auxsw} use bandolier swaps"
	/call set_data_map FALSE maCC stPctStopNuke	 												0															"${auxnum} % of mob health to stop nuking/dotting it at"
	/call set_data_map FALSE maCC stNukeDelay	 													0															"${auxnum} time to forse between using nukes or dots"
	
	
	
	/call set_data_list FALSE maCC lsNoCastWith													EMPTY													"${auxli} list of target buffs you dont want to cast with"
	/call set_data_list FALSE maCC lsNoMeleeWith												EMPTY													"${auxli} list of target buffs you dont want to melee with"
	| /call set_data_list FALSE maCC lsRangeItem													EMPTY													"${auxli}"
	| /call set_data_list FALSE maCC lsRangeItemSummon										EMPTY													"${auxli}"
	| /call set_data_map FALSE maCC stRangeItemCount											0															"${auxnum}"
	/call set_data_map FALSE maCC stRangeOther													FALSE													"${auxna} something else you would like to use for ranged pulling?"
	/call set_data_map FALSE maCC stDotRefresh													3															"${auxnum} time left on a dot to refresh it, in seconds"
	
	

	| control: burn
	/call set_data FALSE FALSE swBurnConditionMet												bool outer FALSE
	/call set_data_map FALSE maBurn swBurnForce													FALSE													"${auxsw} force burn routines"
	/call set_data_map FALSE maBurn swBurnAuto													FALSE													"${auxsw} auto burn on named encounters"
	/call set_data_map FALSE maBurn stPctBurnEngage											99														"${auxnum} % of mobs health to start burns"
	/call set_data_map FALSE maBurn stBurnCount													0															"${auxnum} number of mobs in your area to auto engage burns"
	/call set_data_map FALSE maBurn swBurnRaid													FALSE													"${auxsw} use burns on raids"


| /call sql_control FALSE ${chrID} stoponDS														bool			outer FALSE		


	| specific lists by class
	/if (${Select[${Me.Class.ShortName},BER]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"	
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},BRD]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
	  
	  
	  /call set_data_list FALSE maChr lsLineTank												EMPTY													"${auxli} songs to play for tanks"	
	  /call set_data_list FALSE maChr lsLineCaster											EMPTY													"${auxli} songs to play for casters"	
	  /call set_data_list FALSE maChr lsLineMelee												EMPTY													"${auxli} songs to play for dps whores"	
	  /call set_data_list FALSE maChr lsLineMix													EMPTY													"${auxli} songs for the group you just cant keep happy"	
	  /call set_data_list FALSE maChr lsLineOther												EMPTY													"${auxli} B side songs"	
	  
  
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},BST]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
	 	/call set_data_list FALSE maChr lsHealPetLine											EMPTY													"${auxli} list of healing spells/aas/items for pets"
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},CLR]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
	  /call set_data FALSE FALSE lsHealMode 														list outer "base|passive|aggressive"
	} else /if (${Select[${Me.Class.ShortName},DRU]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"	
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
	  /call set_data FALSE FALSE lsHealMode 														list outer "base|passive|aggressive"
	} else /if (${Select[${Me.Class.ShortName},ENC]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},MAG]}) {
	  /call set_data_list FALSE maChr lsNukeFire												EMPTY													"${auxli} fire nukes"	
	  /call set_data_list FALSE maChr lsNukeMagic												EMPTY													"${auxli} magic nukes"	
	  /call set_data_list FALSE maChr lsNukeBase												EMPTY													"${auxli} whatver you want nukes"	
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"
	 	/call set_data_list FALSE maChr lsHealPetLine											EMPTY													"${auxli} list of healing spells/aas/items for pets"
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},MNK]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},NEC]}) {
	 	/call set_data_list FALSE maChr lsHealPetLine											EMPTY													"${auxli} list of healing spells/aas/items for pets"
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},PAL]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},RNG]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},ROG]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},SHD]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},SHM]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
	 	/call set_data_list FALSE maChr lsHealPetLine											EMPTY													"${auxli} list of healing spells/aas/items for pets"
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},WAR]}) {
	  /call set_data_list FALSE maChr lsNuke														EMPTY													"${auxli} list of nukes to use"		
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"	
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	} else /if (${Select[${Me.Class.ShortName},WIZ]}) {
	  /call set_data_list FALSE maChr lsNukeFire												EMPTY													"${auxli} fire nukes"	
	  /call set_data_list FALSE maChr lsNukeIce													EMPTY													"${auxli} ice nukes"	
	  /call set_data_list FALSE maChr lsNukeMagic												EMPTY													"${auxli} magic nukes"	
	  /call set_data_list FALSE maChr lsNukeBase												EMPTY													"${auxli} whatver you want nukes"	
	  /call set_data_list FALSE maChr lsDoT															EMPTY													"${auxli} list of dots to keep on a target"
		/call set_data FALSE FALSE lsHealMode 														list outer "base"
	}




|** control: debuff	**|
	/call set_data_map FALSE madeBuff swCripple													FALSE													"${auxsw} use cripples"
	/call set_data_map FALSE madeBuff stCripple 												FALSE													"${auxna} single target cripple"
	/call set_data_map FALSE madeBuff stAECripple 											FALSE													"${auxna} ae cripple to use"
	/declare lsdeBuffCripple list outer

	/call set_data_map FALSE madeBuff swMez															FALSE													"${auxsw} use mez"
	/call set_data_map FALSE madeBuff stMez 														FALSE													"${auxna} single target mez"
	/call set_data_map FALSE madeBuff stAEMez 													FALSE													"${auxna} ae mez for when shit gets deep"

	/call set_data_map FALSE madeBuff swMalo														FALSE													"${auxsw} use malo"
	/call set_data_map FALSE madeBuff stMalo			 											FALSE													"${auxna} single target malo"
	/call set_data_map FALSE madeBuff stAEMalo			 										FALSE													"${auxna} ae malo"
	/declare lsdeBuffMalo list outer

	/call set_data_map FALSE madeBuff swTash														FALSE													"${auxsw} use tash"
	/call set_data_map FALSE madeBuff stTash			 											FALSE													"${auxna} single target tash"
	/call set_data_map FALSE madeBuff stAETash			 										FALSE													"${auxna} ae tash cause fauck all the mobs"
	/declare lsdeBuffTash list outer

	/call set_data_map FALSE madeBuff swSlow														FALSE													"${auxsw} use slow"
	/call set_data_map FALSE madeBuff stSlow			 											FALSE													"${auxna} single target slow"
	/call set_data_map FALSE madeBuff stAESlow			 										FALSE													"${auxna} ae slow"
	/declare lsdeBuffSlow list outer

	/call set_data_map FALSE madeBuff swEradicate												FALSE													"${auxsw} use eradicate"
	/call set_data_map FALSE madeBuff stEradicate 											FALSE													"${auxna} single target eradicate"
	/call set_data_map FALSE madeBuff stAEEradicate 										FALSE													"${auxna} ae eradicate"
	/call set_data_map FALSE madeBuff stTimerEradicate									30s														"${auxnum} time to retry an eradicate on a target"
	/declare lsdeBuffEradicate list outer

	/call set_data_map FALSE madeBuff swSnare														FALSE													"${auxsw} use snare"
	/call set_data_map FALSE madeBuff stSnare			 											FALSE													"${auxna} single target snare"
	/call set_data_map FALSE madeBuff stAESnare			 										FALSE													"${auxna} ae snare"
	/declare lsdeBuffSnare list outer


|** control: minion	**|
	/varset dump ${set_data_timer[FALSE, Check_Minion_Cycle, 1m]}
	/call set_Data_map FALSE maMinion swPet															FALSE													"${auxsw} use pets"
	/call set_Data_map FALSE maMinion stPetType													FALSE													"${auxna} name of pet spell to summon pet with"
	/call set_Data_map FALSE maMinion stPetIllusion											FALSE													"${auxna} name of pet illusion whatever spell, clickie, left over dinner"
	/call set_Data_map FALSE maMinion stPctPetEngage										95														"${auxnum} % of mob health to engage pet at"
	/call set_Data_map FALSE maMinion swSwarm														FALSE													"${auxsw} use swarm pets/items/aas/left shoe"
	/call set_Data_map FALSE maMinion stPctSwarmEngage									95														"${auxnum} % of mob health to engage swarm pets at"
	/call set_Data_map FALSE maMinion swNoSwarmonDS											FALSE													"${auxsw} use swarms on mobs woth a damage shield"
	/call set_Data_map FALSE maMinion swonForce													FALSE													"${auxsw} send pets on \a-w/cc force\ax command"
	/call set_Data_map FALSE maMinion swUsePetGear											FALSE													"${auxsw} use pet weapons"
	/call set_Data_map FALSE maMinion stPetWeapon												FALSE													"${auxna} weapon to give your pet"


	
	/varset dump ${set_data_timer[FALSE, Check_Minion_Shrink, 10m]}
	/call set_Data_map FALSE maMinion swPetShrink												FALSE													"${auxsw} use pet shrink item/spell/drugs?"
	/call set_Data_map FALSE maMinion stPetShrink												FALSE													"${auxna} name of pet shrink thingie"

	/varset dump ${set_data_timer[FALSE, Check_Minion_Buff, 1m]}
	/call set_data_map FALSE maMinion swPetBuff													FALSE													"${auxsw} buff minion (\ayyours\ax)"


|** control: over	**|
  /call set_data_map FALSE maOver swOverLOS														FALSE													"${auxsw} override line of sight validations"
  /call set_data_map FALSE maOver swOverPctEngage											FALSE													"${auxsw} ocerride engage % for all types, instant engage of a mob"
  /call set_data_map FALSE maOver swOverLoot													FALSE													"${auxsw} attempt to loot regardless of master looter"
  /call set_data_map FALSE maOver swOverSafeNames											FALSE													"${auxsw} ignore safename checks, trust everyone. pm me your account info please"
  /call set_data_map FALSE maOver swOverTrainSpell										FALSE													"${auxsw} ignore spell training caps. cast forever"


|** control: entropy	**|
	/call set_data_map FALSE maEntropy swMaintenance 										FALSE													"${auxsw} place entropy in a limited maintenance mode for operations"
	/call set_data_map FALSE maEntropy swEventsInc  										FALSE													"${auxsw} use events from events.inc file"
	/call set_data_map FALSE maEntropy swDPSMeter												FALSE													"${auxsw} use dps meeter"
	/call set_data_map FALSE maEntropy swHookSub												FALSE													"${auxsw} use hook functions"
	/call set_data_map FALSE maEntropy stNetworkTimeout									1															"${auxnum} dannet timeout for network lag. ignore this if at all possible"
	/call set_data_list FALSE maEntropy lsSafeNames											EMPTY													"${auxli} list of people you trust (\a-wDanNet connections trusted by default\ax)"
	/call set_data_map FALSE maEntropy swAutoList												FALSE													"${auxsw} autopopulate zone listsnames for some mob types"
	/call set_data_map FALSE maEntropy stBuffRefresh										0															"${auxnum} time in seconds to refrsh a buff before it fades. 0=wait till it fades"
	/call set_data_map FALSE maEntropy swAutoInvManual									0															"${auxsw} obey autoinventory fast list even in manual mode"
	
	| GoM list
	/call set_data_list FALSE maChr lsGoM																EMPTY													"${auxli} things to use on a gift of mana aa proc"

	/call set_data_map FALSE maChr stCountAE						 								0															"${auxnum} number of mobs in your area to even consider using ae skills"
	/call set_data_map FALSE maChr stCountDefensive											0															"${auxnum} number of mobs in your area to start triggering mitigation cycle"
	/call set_data_list FALSE maChr lsMitigationCycle										EMPTY													"${auxli} list of things to use to mitigate damage"
	/call set_data_list FALSE maChr lsMitigationCyclecheck							EMPTY													"${auxli} list of effects for mitigation items to check against"

	| fade
	/call set_data_map FALSE maChr stPctFade														0															"${auxnum} % of your healt to use class aa fade skill"

	| spire
	/call set_data_map FALSE maChr stSpireRaid													0															"${auxnum} number of the spire you want to use while in group for burns"
	/call set_data_map FALSE maChr stSpireGroup													0															"${auxnum} number of the spire you want to use while in a raid for burns"

	| heal classes
	/if (${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
		/declare lsXTargetClass list outer
		/varset dump ${set_data_timer[FALSE, Check_Xtarget_Build, 10m]}
	}

	| everyone gets a base heal line
 	/call set_data_list FALSE maChr lsHealBaseLine											EMPTY													"${auxli} list of healing spells/aas/items you want to use (\a-wintended for non healers\ax)"

	| can cast?
	/if (${lsClassCast.Contains[${Me.Class.ShortName}]}) {
		/call set_data_map FALSE maChr stPctManaClick											0															"${auxna} % to use whatever manna click items entropy can find on you"
	}	


|** control: crew	**|

	| crew lists


/return	
	


|***
 * note: set/build crew lists/rolls
 * use: 
 ***|
sub set_initialize_crew(bool _debug)
	DEBUG \atset_initialize_crew\ax()

	/declare _count int local 0

	/for _count 1 to 12
		/call set_data_crew FALSE lsCrew${_count}									EMPTY													"${auxli} list for crew #${_count}"
		/call set_data_crew FALSE stCrew${_count}Leader						FALSE													"${auxli} crew #${_count} leader"
		/call set_data_crew FALSE stCrew${_count}Looter						FALSE													"${auxli} crew #${_count} looter"
		/call set_data_crew FALSE stCrew${_count}Assist						FALSE													"${auxli} crew #${_count} assist"
	/next _count

/return





|***
 * note: set/build conditions
 * use: 
 ***|
sub set_initialize_map_conditions(bool _debug)
	DEBUG \atset_initialize_map_conditions\ax()

	/declare _count int local 0
	/for _count 1 to 50
		/call set_data_condition FALSE c${_count}													FALSE
	/next _count

/return



|***
 * note: set/build buffs
 * use: 
 ***|
sub set_initialize_map_buffs(bool _debug)
	DEBUG \atset_initialize_map_buffs\ax()

	/declare _count int local 0
	/for _count 1 to 50
		/call set_data_buffs FALSE swBuff${_count}												FALSE
		/call set_data_buffs FALSE stBuff${_count}												FALSE
		/call set_data_buffs FALSE stBuff${_count}Alias										FALSE
		/call set_data_buffs FALSE lsBuff${_count}Tag											EMPTY	
	/next _count

/return



|***
 * note: set/build items
 * use: 
 ***|
sub set_initialize_map_items(bool _debug)
	DEBUG \atset_initialize_map_items\ax()

	/declare _count int local 0
	/for _count 1 to 20
		/call set_data_items FALSE stItem${_count}												FALSE
		/call set_data_items FALSE lsItem${_count}Tag											EMPTY
	/next _count

/return



|***
 * note: creats maps for data that users will not change
 * use: dont fucking use it..
 ***|
sub set_initialize_static_map(bool _debug)
	DEBUG \atset_initialize_static_map\ax()
	
	/declare maStaticMaps map local
	/varset dump ${maStaticMaps.Add[maShade, shade]}
	/varset dump ${maStaticMaps.Add[maSQLError, sql_error]}
	/varset dump ${maStaticMaps.Add[maManaClick, mana_click_item]}
	/varset dump ${maStaticMaps.Add[maExpansion, env_xpac]}
	/varset dump ${maStaticMaps.Add[maCure, cure]}
	
	| leave this out. only for design
	| /varset dump ${maStaticMaps.Add[maSPA, eq_spa]}

	/declare _count int local 0
	/declare _mi mapiterator local
	/vardata _mi maStaticMaps.First
	/while (!${_mi.IsEnd}) {
		/sqlite ${maData.Find[DBstatic].Value} dummycount SELECT Count(*) as data FROM ${_mi.Value}
		/for _count 1 to ${sqlite.Result[dummycount 1 data]}
			/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM ${_mi.Value} WHERE ROWID=${_count}
			DEBUG ${sep}${_mi.Key}${sep}${sqlite.Result[dummy 1 env_var]}${sep}\a-w${sqlite.Result[dummy 1 value]}\ax
			/call set_data ${_debug} "${_mi.Key}" "${sqlite.Result[dummy 1 env_var]}" map outer "${sqlite.Result[dummy 1 value]}"
		/next _count
		/varset dump ${_mi.Advance}
	}

/return


  
|***
 * note: sets any aliases
 * use: /call set_alias DEBUG [add|delete]
 ***|
sub set_alias(bool _debug, string _action)
	DEBUG \atset_alias\ax()

	/declare maAlias map outer
	/varset dump ${maAlias.Add[/on,/command_on]}
	/varset dump ${maAlias.Add[/off,/command_off]}

	| add aliases
	/if (${_action.Equal[add]}) {	
		/vardata mi maAlias.First.Clone
		/while (!${mi.IsEnd}) {
			/if (!${Alias[${mi.Key}]}) /squelch /alias ${mi.Key} ${mi.Value}
			/varset dump ${mi.Advance}
		}	
		
	| delete established aliases
	} else /if (${_action.Equal[delete]}) {	
		/vardata mi maAlias.First.Clone
		/while (!${mi.IsEnd}) {
			/squelch /alias ${mi.Key} delete
			/varset dump ${mi.Advance}
		}			
	}
	
/return




|***
 * note: build all the hud variables
 * use: 
 ***|

sub set_initialize_hud_variables()

	| running mode variable
	/varset dump ${maData.Add[mode,idle]}
	/varset dump ${maData.Add[swHUDTime,TRUE]}
	/declare stHUDNotice string outer

	/declare lsHUD list local
	/varset dump ${lsHUD.Append[build,mode,time]}
	/varset dump ${lsHUD.Append[engage,swpet,engagepet,swswarm,engageswarm,env]}
	/varset dump ${lsHUD.Append[auto,melee,range,nuke,dot,heal,control]}
	/varset dump ${lsHUD.Append[healmode,override,self,group,xt,pet,cure]}
	/varset dump ${lsHUD.Append[bmaster,bself,bcrew,braid,bbeg,btemp,bunity,baura,bnow,bpet]}
	/varset dump ${lsHUD.Append[agro,pull,pullarea,pullmode]}
	/varset dump ${lsHUD.Append[tie,tietoon,tiemode]}
	/varset dump ${lsHUD.Append[buauto,bupct,bucount,buraid]}
	/varset dump ${lsHUD.Append[rpct,rfull,rrod,rbuff,rcombat]}
	/varset dump ${lsHUD.Append[zgive,ztake,zcombat,zeveryone,ztoken]}
	/vardata li lsHUD.First.Clone
	
	/while (!${li.IsEnd}) {
		/declare h_${li.Value} string outer
		/varset dump ${li.Advance}
	}	
	
/return









|***
 * note: HUD pregeneration
 * use: 
 ***|
sub set_initialize_hud(bool _debug, string _verbage)

	OUT Updating MQ2HUD.INI${sep}\a-w${_verbage}\ax
	/delay 5

	| main HUD for Entropy
	/if (${_verbage.Equal[entropy]}) {

	
|**	
	
		
		home
		behind
		fast
		
		assist
		ma1
		ma2
			
**|



		| row 0
		
		HUDPATH entropy note0 	19,134,0,0,0,0,"${stHUDNotice}"

		HUDPATH entropy BG0 		3,131,0,0,0,0,██████████████████████████


		| conbat controls
	 	| HUDPATH entropy cc1 		3,133,14,100,149,237,cc
		HUDPATH entropy ccA1 		19,134,14,255,255,0,"${If[${h_auto},A,]}"
		HUDPATH entropy ccA2 		19,134,9,25,25,25,"${If[${h_auto},,_]}"
		HUDPATH entropy ccEnv1  19,146,14,140,140,140,"${h_env}"

		| pulling
		HUDPATH entropy ccPM1		19,170,14,0,255,0,"${If[${h_pull},${h_pullmode},]}"
		HUDPATH entropy ccPM2		19,170,14,255,0,0,"${If[${h_pull},,${h_pullmode}]}"		
		HUDPATH entropy ccPR1		19,200,14,255,255,255,"${If[${h_pullarea},${h_pullarea},]}"
		HUDPATH entropy ccPR2		19,200,9,25,25,25,"${If[${h_pullarea},,___]}"

		| agro
		HUDPATH entropy ccAg1		19,225,14,255,255,255,"${If[${h_agro},A,]}"
		HUDPATH entropy ccAg2		19,225,9,25,25,25,"${If[${h_agro},,_]}"
		
		| conbat modes
		HUDPATH entropy ccM1 		19,255,14,255,255,255,"${If[${h_melee},M,]}"
		HUDPATH entropy ccM2 		19,255,9,25,25,25,"${If[${h_melee},,_]}"
		HUDPATH entropy ccR1 		19,264,14,255,255,255,"${If[${h_range},R,]}"
		HUDPATH entropy ccR2 		19,264,9,25,25,25,"${If[${h_range},,_]}"
		HUDPATH entropy ccN1 		19,273,14,255,255,255,"${If[${h_nuke},N,]}"
		HUDPATH entropy ccN2 		19,273,9,25,25,25,"${If[${h_nuke},,_]}"
		HUDPATH entropy ccD1 		19,282,14,255,255,255,"${If[${h_dot},D,]}"
		HUDPATH entropy ccD2 		19,282,9,25,25,25,"${If[${h_dot},,_]}"
		HUDPATH entropy ccH1 		19,291,14,255,255,255,"${If[${h_heal},H,]}"
		HUDPATH entropy ccH2 		19,291,9,25,25,25,"${If[${h_heal},,_]}"
		HUDPATH entropy ccC1 		19,300,14,255,255,255,"${If[${h_control},C,]}"
		HUDPATH entropy ccC2 		19,300,9,25,25,25,"${If[${h_control},,_]}"


		

		| row 13
		HUDPATH entropy BG13 		3,131,13,0,0,0,██████████████████████████




		| row 26
		
		| build mode
		HUDPATH entropy build1 	3,5,26,100,149,237,build
		HUDPATH entropy build2 	19,45,26,140,140,140,"${h_build}"

		| healing
		| HUDPATH entropy h1		 	3,133,26,100,149,237,"[              ]"
		HUDPATH entropy h2		 	19,133,26,255,255,255,"${h_healmode}"
		HUDPATH entropy hS1 		19,174,26,255,255,255,"${If[${h_self},S,]}"
		HUDPATH entropy hS2 		19,174,21,25,25,25,"${If[${h_self},,_]}"
		HUDPATH entropy hG1 		19,183,26,255,255,255,"${If[${h_group},G,]}"
		HUDPATH entropy hG2 		19,183,21,25,25,25,"${If[${h_group},,_]}"
		HUDPATH entropy hX1 		19,192,26,255,255,255,"${If[${h_xt},X,]}"
		HUDPATH entropy hX2 		19,192,21,25,25,25,"${If[${h_xt},,_]}"
		HUDPATH entropy hP1 		19,201,26,255,255,255,"${If[${h_pet},P,]}"
		HUDPATH entropy hP2 		19,201,21,25,25,25,"${If[${h_pet},,_]}"
		HUDPATH entropy hC1 		19,210,26,255,255,255,"${If[${h_cure},C,]}"
		HUDPATH entropy hC2 		19,210,21,25,25,25,"${If[${h_cure},,_]}"
		HUDPATH entropy ho1 		19,219,26,255,255,255,"${If[${h_override},${h_override},]}"
		HUDPATH entropy ho2 		19,219,21,25,25,25,"${If[${h_override},,__]}"
		
		| tie
		HUDPATH entropy tie1 		19,255,26,100,149,237,"${h_tiemode}"
		HUDPATH entropy tie2 		19,280,26,0,255,0,"${If[${Bool[${h_tietoon}]},${If[${Bool[${Spawn[${h_tietoon}]}]},${h_tietoon},]},]}"
		HUDPATH entropy tie3 		19,280,26,255,0,0,"${If[${Bool[${h_tietoon}]},${If[!${Bool[${Spawn[${h_tietoon}]}]},${If[${Bool[${h_tietoon}]},${h_tietoon},]},]},]}"
		HUDPATH entropy tie4	 	19,280,22,25,25,25,"${If[${Bool[${h_tietoon}]},,____]}"

		HUDPATH entropy BG26 		3,3,26,0,0,0,██████████████████████████████████████████



		| row 39
		HUDPATH entropy mode1 	19,5,39,100,149,237,mode
		HUDPATH entropy mode2 	19,45,39,255,255,255,"${h_mode}"

		| buffing
		HUDPATH entropy b1		 	19,133,39,0,255,0,"${If[${h_bmaster},buff,]}"
		HUDPATH entropy b2		 	19,133,39,255,0,0,"${If[${h_bmaster},,buff]}"
		HUDPATH entropy bS1 		19,157,39,255,255,255,"${If[${h_bself},S,]}"
		HUDPATH entropy bS2 		19,157,34,25,25,25,"${If[${h_bself},,_]}"
		HUDPATH entropy bC1 		19,166,39,255,255,255,"${If[${h_bcrew},C,]}"
		HUDPATH entropy bC2 		19,166,34,25,25,25,"${If[${h_bcrew},,_]}"
		HUDPATH entropy bR1 		19,175,39,255,255,255,"${If[${h_braid},R,]}"
		HUDPATH entropy bR2 		19,175,34,25,25,25,"${If[${h_braid},,_]}"
		HUDPATH entropy bB1 		19,184,39,255,255,255,"${If[${h_bbeg},B,]}"
		HUDPATH entropy bB2 		19,184,34,25,25,25,"${If[${h_bbeg},,_]}"
		HUDPATH entropy bT1 		19,193,39,255,255,255,"${If[${h_btemp},T,]}"
		HUDPATH entropy bT2 		19,193,34,25,25,25,"${If[${h_btemp},,_]}"
		HUDPATH entropy bU1 		19,202,39,255,255,255,"${If[${h_bunity},U,]}"
		HUDPATH entropy bU2 		19,202,34,25,25,25,"${If[${h_bunity},,_]}"
		HUDPATH entropy bA1 		19,211,39,255,255,255,"${If[${h_baura},A,]}"
		HUDPATH entropy bA2 		19,211,34,25,25,25,"${If[${h_baura},,_]}"
		HUDPATH entropy bN1 		19,220,39,255,255,255,"${If[${h_bnow},N,]}"
		HUDPATH entropy bN2 		19,220,34,25,25,25,"${If[${h_bnow},,_]}"
		HUDPATH entropy bP1 		19,229,39,255,255,255,"${If[${h_bpet},P,]}"
		HUDPATH entropy bP2 		19,229,34,25,25,25,"${If[${h_bpet},,_]}"

		| burn
		HUDPATH entropy burn1 	19,255,39,0,255,0,"${If[${h_buauto},burn,]}"
		HUDPATH entropy burn2 	19,255,39,255,0,0,"${If[${h_buauto},,burn]}"
		HUDPATH entropy burn4		19,280,39,255,255,255,"${If[${h_bupct},${h_bupct},]}"
		HUDPATH entropy burn5 	19,280,34,25,25,25,"${If[${h_bupct},,__]}"
		HUDPATH entropy burn6		19,295,39,255,255,255,"${If[${h_bucount},${h_bucount},]}"
		HUDPATH entropy burn7 	19,295,34,25,25,25,"${If[${h_bucount},,__]}"
		HUDPATH entropy burn8		19,310,39,255,255,255,"${If[${h_buraid},R,]}"
		HUDPATH entropy burn9 	19,310,34,25,25,25,"${If[${h_buraid},,_]}"

		HUDPATH entropy BG39 		3,3,39,0,0,0,██████████████████████████████████████████




		| row 52
		
		| toons loc
		HUDPATH entropy loc1		3,5,52,100,149,237,"loc"
		HUDPATH entropy loc2		3,44,52,140,140,140,"${Int[${Me.Y}]}, ${Int[${Me.X}]}, ${Int[${Me.Z}]}"
		
		
		| rest
		HUDPATH entropy rest1 	19,255,52,0,255,0,${If[${Me.CombatState.Equal[resting]},rest,]}
		HUDPATH entropy rest2 	19,255,52,255,0,0,${If[${Me.CombatState.Equal[resting]},,rest]}
		HUDPATH entropy rest3		19,280,52,255,255,255,"${If[${h_rpct},${h_rpct},]}"
		HUDPATH entropy rest4 	19,280,47,25,25,25,"${If[${h_rpct},,__]}"		
		HUDPATH entropy rest5		19,295,52,255,255,255,"${If[${h_rfull},F,]}"
		HUDPATH entropy rest6 	19,295,47,25,25,25,"${If[${h_rfull},,_]}"
		HUDPATH entropy rest7		19,304,52,255,255,255,"${If[${h_rrod},R,]}"
		HUDPATH entropy rest8 	19,304,47,25,25,25,"${If[${h_rrod},,_]}"
		HUDPATH entropy rest9		19,313,52,255,255,255,"${If[${h_rbuff},B,]}"
		HUDPATH entropy rest10 	19,313,47,25,25,25,"${If[${h_rbuff},,_]}"
		HUDPATH entropy rest11	19,322,52,255,255,255,"${If[${h_rcombat},C,]}"
		HUDPATH entropy rest12	19,322,47,25,25,25,"${If[${h_rcombat},,_]}"

		
		HUDPATH entropy BG52 		3,3,52,0,0,0,██████████████████████████████████████████
		



		| row 65
		
		| rez
		HUDPATH entropy rez1 		19,255,65,0,255,0,${If[${h_ztake},rez,]}
		HUDPATH entropy rez2 		19,255,65,255,0,0,${If[${h_ztake},,rez]}
		HUDPATH entropy rez3 		19,275,65,255,255,255,${If[${h_zgive},G,]}
		HUDPATH entropy rez4 		19,275,60,25,25,25,${If[${h_zgive},,_]}
		HUDPATH entropy rez5 		19,284,65,255,255,255,${If[${h_zcombat},C,]}
		HUDPATH entropy rez6 		19,284,60,25,25,25,${If[${h_zcombat},,_]}
		HUDPATH entropy rez7 		19,293,65,255,255,255,${If[${h_zeveryone},E,]}
		HUDPATH entropy rez8 		19,293,60,25,25,25,${If[${h_zeveryone},,_]}
		HUDPATH entropy rez9 		19,302,65,255,255,255,${If[${h_ztoken},T,]}
		HUDPATH entropy rez10		19,302,60,25,25,25,${If[${h_ztoken},,_]}
	
		HUDPATH entropy BG65 		3,3,65,0,0,0,██████████████████████████████████████████
	
	
		
		
		| row 78
		HUDPATH entropy body1		3,73,78,0,255,0,"${If[${Target.LineOfSight},${Target.Body},]}"
		HUDPATH entropy body2		3,73,78,255,0,0,"${If[${Target.LineOfSight},,${Target.Body}]}"
		HUDPATH entropy body3		3,145,78,140,140,140,"${Target.Distance}"
		HUDPATH entropy eng0 		3,5,77,100,149,237,"[     ][     ][     ][                       ]"
		HUDPATH entropy eng1 		19,10,78,255,255,255,"${If[${h_engage},${h_engage},]}"
		HUDPATH entropy eng2 		19,12,73,25,25,25,"${If[(!${h_engage},__,]}"
		HUDPATH entropy eng5		19,31,78,255,255,255,"${If[${h_swpet},${h_engagepet},]}"
		HUDPATH entropy eng6		19,31,73,25,25,25,"${If[!${h_swpet},__,]}"
		HUDPATH entropy eng8 		19,52,78,255,255,255,"${If[${h_swswarm},${h_engageswarm},]}"
		HUDPATH entropy eng9		19,52,73,25,25,25,"${If[!${h_swswarm},__,]}"
		HUDPATH entropy BG78 		3,3,78,0,0,0,██████████████████████████████████████████
	
		
		
		| row 91
		HUDPATH entropy target	3,5,91,100,149,237,"${If[${Target.ID},[${Target.Class.ShortName} ${Target.Level}] ${Target.Name} ${If[${Bool[${Target.Guild}]},<${Target.Guild}>, ]}, ]}"
		HUDPATH entropy time1		19,290,91,146,149,180,"${If[${h_time},${Time.Time24},${GameTime}]}"
		HUDPATH entropy BG91 		3,3,91,0,0,0,██████████████████████████████████████████


	| dev hud
	} else /if (${_verbage.Equal[dev]}) {

		HUDPATH DEV over 				21,4,50,0,255,0,"${If[${EverQuest.LastMouseOver},${EverQuest.LastMouseOver.Name},]}"
		HUDPATH DEV overbg      21,2,50,0,0,0,"${If[${EverQuest.LastMouseOver},█████████████████████████,]}"

	}



/return







































































































	/call setDeclare hudwatchspawn																			string		outer FALSE
	/call setDeclare watchSpawnCount																		int				outer	FALSE

                                      

			
	/call setDeclare combat																							string		outer FALSE
	/call setDeclare currentLootCount																		int				outer 0
	




	/call sql_control FALSE environment swGlyphOutsideRaid							bool			outer FALSE
	/call sql_control FALSE environment swModePersistent								bool			outer FALSE
	/call sql_control FALSE environment lsNoInvite											string 		outer FALSE

	

	/call sql_control FALSE ${chrID} stoponDS														bool			outer FALSE		
	/call sql_control FALSE ${chrID} useGlyph														string		outer FALSE

	




	
	/call sql_control FALSE ${chrID} setBufftoWatch											string		outer FALSE




	/call sql_control FALSE ${chrID} swAlliance													bool			outer FALSE
	/call sql_control FALSE ${chrID} setAllianceTrigger									string		outer FALSE
	/call sql_control FALSE ${chrID} swForceFadeRest										bool			outer FALSE


	/call sql_control FALSE ${chrID} watchSpawn													string		outer FALSE





	| overrides


	| buffs


	| cursor                                                      			
	/call sql_control FALSE ${chrID} swAutoCursor												bool			outer FALSE					
                                                                			

	

	| mode control
	/call setDeclare swModeToken																				bool			outer FALSE
	/call setDeclare swModeFALSE																				bool			outer FALSE
	/call setDeclare swModeStop																					bool			outer FALSE
	/call setDeclare swModeAgro																					bool			outer FALSE
	/call setDeclare setModeType																				string		outer FALSE
	/call setDeclare setModeCount																				int				outer 0
	/call setDeclare swModeDrag																					bool			outer FALSE 
	/call setDeclare swModeDrink																				bool			outer FALSE 
	/call setDeclare swModeForage																				bool			outer FALSE
	/call setDeclare swModePetfarm																			bool			outer FALSE
	/call setDeclare swModeScorch																				bool			outer FALSE
	/call setDeclare swModeHunt																					bool			outer FALSE
	/call setDeclare swModeFish																					bool			outer FALSE
	/call setDeclare swModeSkill																				bool			outer FALSE
	/call setDeclare swModeHarvest																			bool			outer FALSE
	/call setDeclare swModeHarvestCount																	bool			outer FALSE
	/call setDeclare swModeTradeskill																		bool			outer FALSE
	/call setDeclare swModeLush																					bool			outer FALSE
	/call setDeclare swModeTrainspell																		bool			outer FALSE
	/call setDeclare swModeTrainspellSPC																bool			outer FALSE
	/call setDeclare swModeOpen																					bool			outer FALSE	
	/call setDeclare swModeCollect																			bool			outer FALSE	
	/call setDeclare swModeSummon																				bool			outer FALSE	
	
	/call sql_control FALSE ${chrID} setModeHarvestCount								int				outer 999



		

		                                                            			

	
 	
	
	                                                              			
	/call sql_control FALSE ${chrID} useCircle													string		outer FALSE
	/call sql_control FALSE ${chrID} CircleofPower											string		outer FALSE
	/call sql_control FALSE ${chrID} CircleofLife												string		outer FALSE
	/call sql_control FALSE ${chrID} CircleofMana												string		outer FALSE
                                                                			
	/for _count 1 to 6
		/call sql_control FALSE ${chrID} clickItemSwarm${_count}					string		outer FALSE
	/next _count

		



	| General class combat skills

	/call sql_control FALSE ${chrID} bandolierNormal										string  	outer FALSE
	/call setDeclare bandolierSwap																			string 		outer ${bandolierNormal}
	/call sql_control FALSE ${chrID} bandolierBash											string		outer FALSE
	/call sql_control FALSE ${chrID} bandolierBurn											string		outer FALSE
	/call sql_control FALSE ${chrID} bandolierRange											string		outer FALSE
	

	/if (${Select[${Me.Class.ShortName},MAG,ENC,CLR,NEC,WIZ,SHM,DRU]}) {
		/call sql_control FALSE ${chrID} TwincastSpell 										string 		outer FALSE
		/call sql_control FALSE ${chrID} GatherSpell 											string 		outer FALSE
		/call sql_control FALSE ${chrID} SilentCast	 											string		outer FALSE
	}
	

                                                    			

                                                                			
	                                                              			
	| Mode stuffs                                                 			
	| Train Spell spells                                          			
	/if (${Bool[${Me.Class.CanCast}]}) {                          			
		/call sql_control FALSE ${chrID} abjuration												string 		outer FALSE
		/call sql_control FALSE ${chrID} alteration												string 		outer FALSE
		/call sql_control FALSE ${chrID} conjuration											string 		outer FALSE
		/call sql_control FALSE ${chrID} divination												string 		outer FALSE
		/call sql_control FALSE ${chrID} evocation												string 		outer FALSE
	}

	| forced delay for mez target
	/if (${Select[${Me.Class.ShortName},ENC,BRD,NEC]}) {
 		/call sql_control FALSE ${chrID} setMezDelay											int 			outer 0
	}                                                             			




