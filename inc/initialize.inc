|*********************************************************************
 * entropy
 * initialize.inc
 * all shared/static data initilization
 *
 ***|



|***
 * note: builds all the debug variables and lists for the map
 * use:
 ***|
sub set_debug()

  | generate DEBUGS
  /declare flow string outer FALSE
  /declare lsDebuglist list outer
  /invoke ${lsDebuglist.Append[buff,heal,move,debuff,cast,loot,mode,combat,other,minion,class,admin]}

  | build the variables for the debug lists
  /declare _licdeb listiterator local
  /vardata _licdeb lsDebuglist.First.Clone
  /while (!${_licdeb.IsEnd}) {
    /declare lsDebug${_licdeb.Value} list outer
    /invoke ${_licdeb.Advance}
  }

  | buff
  /invoke ${lsDebugbuff.Append[cb,ammo,beg,unity,aura,crew,buffself,shrink,poison,class,now,temp,bocked]}
  | heal
  /invoke ${lsDebugheal.Append[cure,xt,group,pet,self,hurt,tot]}
  | move
  /invoke ${lsDebugmove.Append[face,tie,zone,stick]}
  | debuff
  /invoke ${lsDebugdebuff.Append[debuff,mez]}
  | minion
  /invoke ${lsDebugdebuff.Append[petbuid,lost,sic,temp,heal]}
  | cast
  /invoke ${lsDebugcast.Append[item,nuke,dot]}
	| loot
  /invoke ${lsDebugloot.Append[loot]}
	| mode
  /invoke ${lsDebugmode.Append[forage,drag,summon,petfarm,harvest,hunt,fish,farm,lush,trainskill]}
  | combat
  /invoke ${lsDebugcombat.Append[def,target,melee,burn,agro,pull,misc,assist,decision,proc,ttl,sash,status,wait]}
	| other
  /invoke ${lsDebugother.Append[edit,rest,rez,spire,glyph,fireworks,manarecover,fade,event,watch]}
	| admin
  /invoke ${lsDebugadmin.Append[build,init,observe,house,flow,exit,condition,sql]}
  | class specific debugs
  /if (${Select[${Me.Class.ShortName},BER]}) {
  } else /if (${Select[${Me.Class.ShortName},BRD]}) {
  	/invoke ${lsDebugclass.Append[pulse]}
  } else /if (${Select[${Me.Class.ShortName},BST]}) {
  	/invoke ${lsDebugclass.Append[para]}
  } else /if (${Select[${Me.Class.ShortName},CLR]}) {
  	/invoke ${lsDebugclass.Append[arb,splash,regen]}
  } else /if (${Select[${Me.Class.ShortName},DRU]}) {
  } else /if (${Select[${Me.Class.ShortName},ENC]}) {
  	/invoke ${lsDebugclass.Append[crystal,gather]}
  } else /if (${Select[${Me.Class.ShortName},MAG]}) {
  	/invoke ${lsDebugclass.Append[coh,gather,cauldron]}
  } else /if (${Select[${Me.Class.ShortName},MNK]}) {
  } else /if (${Select[${Me.Class.ShortName},NEC]}) {
  	/invoke ${lsDebugclass.Append[blood,bloom]}
  } else /if (${Select[${Me.Class.ShortName},PAL]}) {
  	/invoke ${lsDebugclass.Append[splash]}
  } else /if (${Select[${Me.Class.ShortName},RNG]}) {
  	/invoke ${lsDebugclass.Append[summer]}
  } else /if (${Select[${Me.Class.ShortName},ROG]}) {
  } else /if (${Select[${Me.Class.ShortName},SHD]}) {
  } else /if (${Select[${Me.Class.ShortName},SHM]}) {
  	/invoke ${lsDebugclass.Append[talisman,surge,canni]}
  } else /if (${Select[${Me.Class.ShortName},WAR]}) {
  } else /if (${Select[${Me.Class.ShortName},WIZ]}) {
  	/invoke ${lsDebugclass.Append[twincast]}
  }

  /declare maDebug map outer

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

/return TRUE



|***
 * note: the start of the end, all visual output formatting, debugs trash variables, verbosity
 * use:
 ***|
sub set_initialize(bool _debug)
  DEBUG \atset_initialize\ax()

  | set inital trash variables
  | /call set_data ${_debug} FALSE dump string outer FALSE
  /call set_data ${_debug} FALSE bindswap bool outer FALSE
  /call set_data ${_debug} FALSE swap string outer FALSE
  /call set_data ${_debug} FALSE DNReturn string outer FALSE
  /call set_data ${_debug} FALSE swDBlocked bool outer FALSE
  /call set_data ${_debug} FALSE DNQReturn string outer FALSE
  /call set_data ${_debug} FALSE DNQReturn2 string outer FALSE


	| map iterator dump variable
  | /declare mi mapiterator outer

  | list iterator dump variable
  /declare li listiterator outer

  | control list dump variable
  | /declare cli listiterator outer

  | control map dumps
 	/declare maControl map outer
 	/declare maChrControl map outer
	/declare cmi mapiterator outer

  | set iterator dump variable
  /declare si setiterator outer

  | hook map
  /declare maHook map outer

  | buff beg map
  /declare maBuffRequest map outer

  | watchspawn map
  | /declare maWatchspawn map outer

  | rez control map
 	/declare maControlRez map outer

	| self blocked buffs
  /declare lsBlockedBuffsMe string outer

  | PREGEN recorder
  /declare maPREGEN map outer

  | TTL stuff
  /declare maTTL map outer
  /call set_data ${_debug} maTTL stTTLTimeStart														map outer 0

  | predictive combat
  /declare maPred map outer

  | list of all maps used with usable data
  /declare lsMaps list outer
  /invoke ${lsMaps.Append[maChr,maEnv,maHome,maCC,maHeal,maRez,maBuff,maBurn,maPred]}
  /invoke ${lsMaps.Append[maEntropy,maTimer,maCondition,madeBuff,maTie,maMinion,maCrew]}
  /invoke ${lsMaps.Append[maData,maDebug,maOver,maPREGEN,maMode]}
  /invoke ${lsMaps.Append[maBuffRequest,maHook,maManaRecovery,maTTL,maWatch]}
  /invoke ${lsMaps.Append[maHeal,maNuke,maDoT,maItem,maDefense,maMiscDPS,maSong,maSash,maControlRez]}

  | set all output shortcuts
  /call set_data ${_debug} FALSE num                                      string outer "\a-t"
  /call set_data ${_debug} FALSE submenu                                  string outer "\a#1E90FF->\ax"
  /call set_data ${_debug} FALSE r0                                       string outer "\ar0\ax"
  /call set_data ${_debug} FALSE error                                    string outer "\arERROR\ax"
  /call set_data ${_debug} FALSE tar                                      string outer "\a-w"
  /call set_data ${_debug} FALSE pc                                       string outer "\a-w"
  /call set_data ${_debug} FALSE sp                                       string outer "\a-r"
  /call set_data ${_debug} FALSE bad                                      string outer "\ar"
  /call set_data ${_debug} FALSE good                                     string outer "\ag"
  /call set_data ${_debug} FALSE bug                                      string outer "\at"
  /call set_data ${_debug} FALSE info                                     string outer "\aw"
  /call set_data ${_debug} FALSE sep                                      string outer "\a#AA00AA::\ax"
  /call set_data ${_debug} FALSE dot                                      string outer "\at.\ax"
  /call set_data ${_debug} FALSE on                                       string outer "\agon\ax"
  /call set_data ${_debug} FALSE off                                      string outer "\aroff\ax"
  /call set_data ${_debug} FALSE warning                                  string outer "\arWARNING\ax"
  /call set_data ${_debug} FALSE fatal                                    string outer "\arFATAL ERROR\ax \at..\ax \arENDING MACRO\ax"
  /call set_data ${_debug} FALSE caution                                  string outer "\arCAUTION\ax"
  /call set_data ${_debug} FALSE true                                     string outer "\arTRUE\ax"
  /call set_data ${_debug} FALSE false                                    string outer "\arFALSE\ax"
  /call set_data ${_debug} FALSE destroy                                  string outer "\arDESTROY\ax"
  /call set_data ${_debug} FALSE leave                                    string outer "\awLeave\ax"
  /call set_data ${_debug} FALSE add                                      string outer "\agAdd\ax"
  /call set_data ${_debug} FALSE keep                                     string outer "\agKeep\ax"
  /call set_data ${_debug} FALSE notice                                   string outer "\ayNotice\ax"
  /call set_data ${_debug} FALSE combat                                   string outer "\amCombat\ax"
  /call set_data ${_debug} FALSE remove                                   string outer "\arRemoving\ax"
  /call set_data ${_debug} FALSE broadcast                                string outer "Broadcast"
  /call set_data ${_debug} FALSE auxnum                                   string outer "[\a-w#\ax]"
  /call set_data ${_debug} FALSE auxsw                                    string outer "[\a-wswitch\ax]"
  /call set_data ${_debug} FALSE auxna                                    string outer "[\a-wname\ax]"
  /call set_data ${_debug} FALSE auxli                                    string outer "[\a-wlist\ax]"
  /call set_data ${_debug} FALSE eol                                      string outer "\at........\ax \a#4183D7END OF LINE\ax"

/return TRUE



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
  /call set_data ${_debug} FALSE lsClassAll                               list outer WAR|PAL|SHD|BRD|RNG|BST|BER|MNK|ROG|WIZ|MAG|ENC|NEC|CLR|DRU|SHM
  /call set_data ${_debug} FALSE lsClassMelee                             list outer WAR|ROG|BST|MNK|SHD|PAL|BER|BRD|RNG
  /call set_data ${_debug} FALSE lsClassCast                              list outer PAL|SHD|BRD|RNG|BST|WIZ|MAG|ENC|NEC|CLR|DRU|SHM
  /call set_data ${_debug} FALSE lsClassPet                               list outer ENC|DRU|SHM|SHD|WIZ|BRD|BST|MAG|NEC
  /call set_data ${_debug} FALSE lsClassHeal                              list outer CLR|DRU|SHM|PAL|RNG|BST
  /call set_data ${_debug} FALSE lsClassRez                               list outer CLR|DRU|SHM|PAL|NEC
  /call set_data ${_debug} FALSE lsClassAASnare                           list outer RNG|DRU|SHD|NEC|ROG|WAR|WIZ|MNK|PAL|BER
  /call set_data ${_debug} FALSE lsClassAAEradicate                       list outer RNG|DRU|NEC|WIZ|ENC|MAG
  /call set_data ${_debug} FALSE lsClassCastCure                          list outer CLR|DRU|SHM|BST|PAL|RNG
  /call set_data ${_debug} FALSE lsClassHealToT	                          list outer BST|PAL|RNG
  /call set_data ${_debug} FALSE lsIVU                                    list outer "Group Perfected Invisibility to Undead"|"Perfected Invisibility to Undead"|"Invisibility to Undead"|"Invisibility versus Undead"|"Shauri's Sonorous Clouding"
  /call set_data ${_debug} FALSE lsIV                                     list outer "Shared Perfected Camouflage"|"Shared Camouflage"|"Camouflage"|"Group Perfected Invisibility"|"Invisibility"|"Perfected Invisibility"|"Phase March"|"Nethervoid March"|"Perfected Silent Presence"|"Cloak of Shadows"|"Gather Umbra"|"Invisibility Cloak"|"Perfected Natural Invisibility"|"Perfected Spirit Walk Invisibility"|"Perfected Group Spirit Walk Invisibility"|"Shauri's Sonorous Clouding"|"Selo's Song of Travel"
  /call set_data ${_debug} FALSE lsAssistTypes                            list outer g|ma1|ma2|1|2|3
  /call set_data ${_debug} FALSE lsPullTypes                              list outer base|pet|int|nav|multi
  /call set_data ${_debug} FALSE lsRezBuffTriggers                        list outer "Revival Sickness"|"Resurrection Sickness"

	/call set_data ${_debug} FALSE lsClassHealToT														list outer PAL|RNG|BST
	/call set_data ${_debug} FALSE lsClassHealSelf													list outer WAR|PAL|SHD|BRD|RNG|BST|BER|MNK|ROG|WIZ|MAG|ENC|NEC|CLR|DRU|SHM
	/call set_data ${_debug} FALSE lsClassHealGroup													list outer CLR|SHM|PAL|DRU
	/call set_data ${_debug} FALSE lsClassHealXT														list outer CLR|SHM|PAL|DRU
	/call set_data ${_debug} FALSE lsClassHealPet														list outer CLR|SHM|PAL|DRU|NEC|MAG|BST|SHM|RNG
	/call set_data ${_debug} FALSE lsClassHealCure													list outer WAR|PAL|SHD|BRD|RNG|BST|BER|MNK|ROG|WIZ|MAG|ENC|NEC|CLR|DRU|SHM

	| things we ont want pullers attacking
  /call set_data ${_debug} FALSE lsNoAttackBodyType                       list outer AURA|BANNER|CAMPFIRE|CORPSE|CHEST|ITEM|TRIGGER|TRAP|TIMER|MOUNT|MERCENARY|OBJECT
  /call set_data ${_debug} FALSE lsNoTargetNPC                            list outer "a campfire"|"a corpse"|"controller"
  /call set_data ${_debug} FALSE lsNoTargetPC                             list outer "eye of"
  /call set_data ${_debug} FALSE lsNoTargetMagic                          list outer "eye of"
  /call set_data ${_debug} FALSE lsNoTargetAnimation	                    list outer 16|43|44|146

  | custom /build set variables
  /call set_data ${_debug} FALSE lsBuildSet							                  list outer primary|secondary|range|spellset

  | spell types we let through the checking/validation methods
  /call set_data ${_debug} FALSE lsFreePassSpells                         list outer Beneficial|Pet

	| increment lists
	/call set_data ${_debug} FALSE lsCount6			                            list outer "0|1|2|3|4|5"
	/call set_data ${_debug} FALSE lsCount10		                            list outer "1|2|3|4|5|6|7|8|9|10"
	/call set_data ${_debug} FALSE lsCount12		                            list outer "1|2|3|4|5|6|7|8|9|10|11|12"
	/call set_data ${_debug} FALSE lsCount20		                            list outer "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20"
	/call set_data ${_debug} FALSE lsCount50		                            list outer "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|40|41|42|43|44|45|46|47|48|49|50"
	| /call set_data ${_debug} FALSE lsCount			                            list outer ""
	| /call set_data ${_debug} FALSE lsCount			                            list outer ""


	| /call set_data ${_debug} FALSE lsGroupRegens			                      list outer ""

  | timers  for everyone
  /invoke ${set_data_timer[FALSE, Try_Door, 4s]}
  /invoke ${set_data_timer[FALSE, Check_Ammo, 5s]}
  /invoke ${set_data_timer[FALSE, Nuke_Delay, 1]}
  /invoke ${set_data_timer[FALSE, Check_Cursor, 1s]}
  /invoke ${set_data_timer[FALSE, Check_Zone_ID, 10s]}
  /invoke ${set_data_timer[FALSE, Check_Observers, 1m]}
  /invoke ${set_data_timer[FALSE, Check_Poison, 1m]}
  /invoke ${set_data_timer[FALSE, Check_deBuffs, 6s]}
  /invoke ${set_data_timer[FALSE, Check_Behind, 5s]}
  /invoke ${set_data_timer[FALSE, Check_Tribute, 1m]}
  /invoke ${set_data_timer[FALSE, Check_Loot, 1s]}
  /invoke ${set_data_timer[FALSE, Check_Mode, 5]}
  /invoke ${set_data_timer[FALSE, Check_LoS_Timeout, 10s]}
  /invoke ${set_data_timer[FALSE, Check_Housekeeping, 1m]}
  /invoke ${set_data_timer[FALSE, Check_Home, 1]}
  /invoke ${set_data_timer[FALSE, Check_Target, 5s]}
  /invoke ${set_data_timer[FALSE, Check_Watch, 5s]}

  | get the stick timer started
  /invoke ${set_timer[${_debug}, timer_check_stick, 1s]}

  | controls for all classes
  /invoke ${maChrControl.Add[ae,stCountAE]}
  | /invoke ${maChrControl.Add[def,stCountDefensive]}
  /invoke ${maChrControl.Add[fade,stPctFade]}
  /invoke ${maChrControl.Add[spire,submenu]}

  | controls for heal classes
  /if (${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
  }

  | controls for mana users
  /if (${lsClassCast.Contains[${Me.Class.ShortName}]}) {
	  /invoke ${maChrControl.Add[manarecover,stPctManaRecovery]}
  }

  | controls for melee users
  /if (${lsClassMelee.Contains[${Me.Class.ShortName}]}) {
	  /invoke ${maChrControl.Add[hiatus,swHiatus]}
  }

  | controls by class
  /if (${Select[${Me.Class.ShortName},BER]}) {

  } else /if (${Select[${Me.Class.ShortName},BRD]}) {
	  /invoke ${maChrControl.Add[rest,swSongRest]}
	  /invoke ${maChrControl.Add[refresh,stSongRefresh]}
	  /invoke ${maChrControl.Add[bellow,swAABellow]}
	  /invoke ${maChrControl.Add[ooc,swPulseOOC]}

  } else /if (${Select[${Me.Class.ShortName},BST]}) {
	  /invoke ${maChrControl.Add[consume,stPctConsume]}
	  /invoke ${maChrControl.Add[parafocus,stPctParagonF]}
	  /invoke ${maChrControl.Add[paragroup,stPctParagonG]}

  } else /if (${Select[${Me.Class.ShortName},CLR]}) {
	  /invoke ${maChrControl.Add[arb,submenu]}
	  /invoke ${maChrControl.Add[beacon,stCountBeaconofLife]}
	  /invoke ${maChrControl.Add[divinerez,swDivineCleric]}
	  /invoke ${maChrControl.Add[issuance,stCountIssuance]}
	  /invoke ${maChrControl.Add[primary,submenu]}
	  /invoke ${maChrControl.Add[vp,stPctVP]}
	  /invoke ${maChrControl.Add[yaulp,swYaulp]}

  } else /if (${Select[${Me.Class.ShortName},DRU]}) {
	  /invoke ${maChrControl.Add[groupheal,stCountHealGroup]}
	  /invoke ${maChrControl.Add[wolf,swWolfSpirit]}

  } else /if (${Select[${Me.Class.ShortName},ENC]}) {
		| /invoke ${maChrControl.Add[charm,]}
	  /invoke ${maChrControl.Add[gather,stPctGather]}
	  /invoke ${maChrControl.Add[bos,swAABoS]}
	  /invoke ${maChrControl.Add[crystal,swAACrystals]}

  } else /if (${Select[${Me.Class.ShortName},MAG]}) {
	  /invoke ${maChrControl.Add[gather,stPctGather]}
	  | /invoke ${maChrControl.Add[forcetc,swForceTC]}
	  /invoke ${maChrControl.Add[cauldron,submenu]}

  } else /if (${Select[${Me.Class.ShortName},MNK]}) {
	  /invoke ${maChrControl.Add[train,swTrainMelee]}
	  /invoke ${maChrControl.Add[fdhp,stPctFDHP]}
	  /invoke ${maChrControl.Add[fdaggro,stPctFDAggro]}

  } else /if (${Select[${Me.Class.ShortName},NEC]}) {
	  /invoke ${maChrControl.Add[blood,submenu]}
	  /invoke ${maChrControl.Add[bloom,stPctBloom]}

  } else /if (${Select[${Me.Class.ShortName},PAL]}) {
	  /invoke ${maChrControl.Add[yaulp,swYaulp]}
	  /invoke ${maChrControl.Add[attraction,swAttraction]}
	  /invoke ${maChrControl.Add[harmonious,swHarmonious]}
	  /invoke ${maChrControl.Add[loh,stPctLoH]}
	  /invoke ${maChrControl.Add[mg,stPctMG]}
	  | /invoke ${maChrControl.Add[flash,submenu]}
	  /invoke ${maChrControl.Add[stun,submenu]}

  } else /if (${Select[${Me.Class.ShortName},RNG]}) {
	  /invoke ${maChrControl.Add[summer,swSummer]}

  } else /if (${Select[${Me.Class.ShortName},ROG]}) {
	  | /invoke ${maChrControl.Add[sos,swSoS]}
	  /invoke ${maChrControl.Add[blind,swBlinding]}
	  /invoke ${maChrControl.Add[poison,stLegPoison]}

  } else /if (${Select[${Me.Class.ShortName},SHD]}) {
	  /invoke ${maChrControl.Add[attraction,swAttraction]}
	  /invoke ${maChrControl.Add[harmonious,swHarmonious]}
	  /invoke ${maChrControl.Add[vot,swVoT]}
	  | /invoke ${maChrControl.Add[flash,submenu]}
	  /invoke ${maChrControl.Add[skin,swSkin]}
	  /invoke ${maChrControl.Add[stance,swStance]}

  } else /if (${Select[${Me.Class.ShortName},SHM]}) {
	  | /invoke ${maChrControl.Add[groupheal,stCountHealGroup]}
	  | /invoke ${maChrControl.Add[hot,submenu]}
	  | /invoke ${maChrControl.Add[grouphot,swHealGroupOverTime]}
	  /invoke ${maChrControl.Add[cannihps,stPctCanniHP]}
	  /invoke ${maChrControl.Add[cannimanna,stPctCanniManna]}
	  /invoke ${maChrControl.Add[surge,swSurge]}
	  /invoke ${maChrControl.Add[talisman,swTalisman]}

  } else /if (${Select[${Me.Class.ShortName},WAR]}) {
	  /invoke ${maChrControl.Add[attraction,swAttraction]}
	  | /invoke ${maChrControl.Add[flash,submenu]}

  } else /if (${Select[${Me.Class.ShortName},WIZ]}) {
	  /invoke ${maChrControl.Add[mb,swManaBurn]}
	  /invoke ${maChrControl.Add[gather,stPctGather]}
	  /invoke ${maChrControl.Add[concussion,stPctConcussion]}

  }


  | set current zone
  /call set_data ${_debug} maEnv stCurrentZoneID                          	map outer FALSE

  | Comm
  /call set_data ${_debug} maComm stLocalEcho                     	        map outer FALSE             "dannet local echo"
  /call set_data ${_debug} maComm stCommandEcho                   	        map outer FALSE             "dannet local command echo"
  /call set_data ${_debug} maComm stFrontDelim                     	       	map outer ON                "dannet lead from delimiters"
  /call set_data ${_debug} maComm stFullNames                      	       	map outer FALSE             "dannet fullnames"

  | set DanNet groups
  /declare lsEntropyGroup list outer
  /invoke ${lsEntropyGroup.Delimiter["|"]}

  /invoke ${lsEntropyGroup.Append[all|1|2|3|4|5|6|7|8|9|10|11]}

	/declare _lidn listiterator local
  /vardata _lidn lsEntropyGroup.First
  /while (!${_lidn.IsEnd}) {
    /invoke ${maComm.Add[stEntropyGroup_${_lidn.Value},entropy${_lidn.Value}]}
    /invoke ${_lidn.Advance}
  }

  | join the group
  /djoin ${maComm.Find[stEntropyGroup_all].Value}


  | generate modes
  /declare maMode map outer
  /declare lsMode list outer
  /invoke ${lsMode.Append[forage,drag,summon,petfarm,harvest,hunt,fish,farm,lush,trainskill]}

	/declare _lisie listiterator local
  /vardata _lisie lsMode.First.Clone
  /while (!${_lisie.IsEnd}) {
    /invoke ${maMode.Add[${_lisie.Value},FALSE]}
    /invoke ${_lisie.Advance}
  }

/return TRUE



|***
 * note: generate the zone file DB
 * use:
 ***|
sub sql_set_zone_schema(bool _debug)
  DEBUG \atsql_set_zone_schema\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

  /varset swDBlocked TRUE

  | zone fields
  /declare lsZoneProperty list outer
  /invoke ${lsZoneProperty.Delimiter["|"]}
  /invoke ${lsZoneProperty.Append[lsZoneNamed|lsZoneExclude|lsZoneCharm|lsZoneRange|lsZoneNoMez|lsZoneNoCast|lsZoneNoMelee]}
  /invoke ${lsZoneProperty.Append[lsZoneNoFire|lsZoneNoPoison|lsZoneNoMagic|lsZoneNoCorruption|lsZoneNoCold|lsZoneNoDisease]}

	/declare _lisszs listiterator local
  /vardata _lisszs lsZoneProperty.First
  /while (!${_lisszs.IsEnd}) {
    /declare ${_lisszs.Value} list outer
    /invoke ${_lisszs.Advance}
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

  } else  /if (${sqlite.Result[dummy 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {
    /while (${swDBlocked}) {
      /sqlite ${maData.Find[DBzone].Value} dummy UPDATE build SET build_number='${maEnv.Find[build].Value}'
      /delay 1
      /if (!${sqlite.Status[dummy].Find[locked]}) {
        /delay 1
        /varset swDBlocked FALSE
      }
    }
  }

	/if (${maDebug.Find[init].Value}) {
		OUT \agsql_set_zone_schema\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: generate the zone file DB
 * use:
 ***|
sub sql_set_loot_schema(bool _debug)
	DEBUG \atsql_set_loot_schema\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

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

      /sqlite ${maData.Find[DBloot].Value} dummy CREATE TABLE 'loot_control' ('item' TEXT NOT NULL PRIMARY KEY, 'action' TEXT, 'pass' TEXT, 'count' TEXT)
      DEBUG ${sep}CREATE TABLE 'loot_control' \ay${sqlite.Status[dummy]}\ax

      /delay 1
      /if (!${sqlite.Status[dummy].Find[locked]}) {
        /delay 1
        /varset swDBlocked FALSE
      }
    }

  } else  /if (${sqlite.Result[dummy 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {
    /while (${swDBlocked}) {
      /sqlite ${maData.Find[DBloot].Value} dummy UPDATE build SET build_number='${maEnv.Find[build].Value}'
      /delay 1
      /if (!${sqlite.Status[dummy].Find[locked]}) {
        /delay 1
        /varset swDBlocked FALSE
      }
    }
  }

	/if (${maDebug.Find[init].Value}) {
		OUT \agsql_set_loot_schema\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: generate character file DB
 * use:
 ***|
sub sql_set_character_schema(bool _debug)
  DEBUG \atsql_set_character_schema\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

  /declare maDBTables map outer
  /invoke ${maDBTables.Add[environment_buff,env_var]}
  /invoke ${maDBTables.Add[environment_character,env_var]}
  /invoke ${maDBTables.Add[environment_condition,env_var]}
  /invoke ${maDBTables.Add[environment_timer,env_timer]}
  /invoke ${maDBTables.Add[environment_item,env_var]}
  /invoke ${maDBTables.Add[environment_crew,env_var]}
  /invoke ${maDBTables.Add[environment_defense,env_var]}
  /invoke ${maDBTables.Add[environment_heal,env_var]}
  /invoke ${maDBTables.Add[environment_song,env_var]}
  /invoke ${maDBTables.Add[environment_nuke,env_var]}
  /invoke ${maDBTables.Add[environment_dot,env_var]}
  /invoke ${maDBTables.Add[environment_agro,env_var]}
  /invoke ${maDBTables.Add[environment_miscdps,env_var]}
  /invoke ${maDBTables.Add[environment_burn,env_var]}
  /invoke ${maDBTables.Add[environment_sash,env_var]}

  /sqlite ${maData.Find[DBcharacter].Value} SQL_ENV_BUILD SELECT build_number FROM build
  DEBUG ${sep}SELECT build_number FROM build \ay${sqlite.Status[dummy]}\ax
  | keep a running copy of the previous build number
  /invoke ${maData.Add[stPreviousBuild,${sqlite.Result[SQL_ENV_BUILD 1 build_number]}]}
  DEBUG ${sep}BUID${sep}\a-w${sqlite.Result[SQL_ENV_BUILD 1 build_number]}\ax

  /if (${sqlite.Result[SQL_ENV_BUILD 1 build_number].Equal[${maEnv.Find[build].Value}]}) {
    | /return TRUE
  } else /if (${sqlite.Status[SQL_ENV_BUILD].Find[no such table:]} || ${sqlite.Result[SQL_ENV_BUILD 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {

		| fix any existing database variable issues.
	  /call sql_check ${maDebug.Find[sql].Value}

    /if (${sqlite.Status[SQL_ENV_BUILD].Find[no such table:]}) {
      | character build match
      /sqlite ${maData.Find[DBcharacter].Value} dummy CREATE TABLE IF NOT EXISTS 'build' ('build_number' TEXT NOT NULL PRIMARY KEY)
      DEBUG ${sep}CREATE TABLE IF NOT EXISTS 'build' \ay${sqlite.Status[dummy]}\ax
      /sqlite ${maData.Find[DBcharacter].Value} dummy INSERT INTO 'build' ('build_number') VALUES ("${maEnv.Find[build].Value}")
      DEBUG ${sep}INSERT INTO 'build' \ay${sqlite.Status[dummy]}\ax

    } else  /if (${sqlite.Result[SQL_ENV_BUILD 1 build_number].NotEqual[${maEnv.Find[build].Value}]}) {
      /sqlite ${maData.Find[DBcharacter].Value} dummy UPDATE build SET build_number='${maEnv.Find[build].Value}'
      DEBUG ${sep}CREATE TABLE IF NOT EXISTS 'build' \ay${sqlite.Status[dummy]}\ax
    }

    /declare _mi mapiterator local
    /vardata _mi maDBTables.First.Clone
    /while (!${_mi.IsEnd}) {
      /sqlite ${maData.Find[DBcharacter].Value} dummy CREATE TABLE IF NOT EXISTS '${_mi.Key}' ('${_mi.Value}' TEXT NOT NULL PRIMARY KEY,'value' TEXT NOT NULL, 'aux' TEXT)
      DEBUG ${sep}CREATE TABLE IF NOT EXISTS '${_mi.Key}' \ay${sqlite.Status[dummy]}\ax
      /invoke ${_mi.Advance}
    }
  }

	/if (${maDebug.Find[init].Value}) {
		OUT \agsql_set_character_schema\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: creat the base character environments
 * use: no touching!
 ***|
sub set_initialize_map_character_environment(bool _debug)

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

  /declare _count int local 0

	| hude mode cycle variable
  | /call set_data ${_debug} FALSE stCycleMode	                            string outer EMPTY

|** data **|
  /call set_data ${_debug} maData stCastReturn                            map outer FALSE

|** control: grain **|
  /call set_data_list ${_debug} maEnv lsGrain                           	1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30        "${auxnum} visual outputs you wish to see. \aw/verb aux\ax for full list"

|** control: predictive **|
  /call set_data_map ${_debug} maPred swPred															FALSE													 "${auxsw} use predictive combat logic"
  /call set_data_map ${_debug} maPred stPredRad														40														 "${auxnum} radius around predicted pull target"
  /call set_data_map ${_debug} maPred swPredPrepBurn											FALSE													 "${auxsw} prep burn routines"
  /call set_data_map ${_debug} maPred swPredPrepClass											FALSE													 "${auxsw} prep class specific routine"
  /call set_data ${_debug} maPred stPredID																map outer FALSE


|** control: env **|
  /call set_data ${_debug} maEnv swAuto                                   map outer FALSE
  /call set_data_map ${_debug} maEnv swSafeListGuild                      FALSE                         "${auxsw} let guild in your safe list"
  /invoke ${set_data_timer[${_debug}, Check_EXP_Adjust, 5m]}
  /call set_data_map ${_debug} maEnv stMobLvlMin                          1                             "${auxnum} lowest level mob you will attack"
  /call set_data_map ${_debug} maEnv stMobLvlMax                          500                           "${auxnum} highest level mob you will attack"
  /call set_data_map ${_debug} maEnv swNukeLoop                           FALSE                         "${auxsw} force a nuke loop to complete before releasing"

  /call set_data_map ${_debug} maEnv swADVLoot                            FALSE                         "${auxsw} use advanced loot functions"
  /call set_data_map ${_debug} maEnv swAutoInv                            FALSE                         "${auxsw} use autoinventory loot control on cursor items"
  /call set_data_list ${_debug} maEnv lsAutoInventory                     EMPTY                         "${auxli} list of shit to auto inventory"
  /call set_data_map ${_debug} maEnv stBuffGem                            ${Me.NumGems}                 "${auxnum} gem slot to use as a swap for buffing"
  /call set_data_map ${_debug} maEnv swHoldBuffGem                        FALSE                         "${auxsw} remember and return the spell used in that slot before buffing"
  /call set_data_map ${_debug} maEnv stEnvRadius                          31                            "${auxnum} radius you are willing to operate in"
  /call set_data_map ${_debug} maEnv stEnvSafeRadius                      30                            "${auxnum} radius you consider safe even with mobs in it"
  /call set_data_map ${_debug} maEnv stLvlPct                             99                            "${auxnum} % of exp in a level you want to maintain"
  /call set_data_map ${_debug} maEnv stLvlMax                             110                           "${auxnum} level you want to maintain"
  /call set_data_map ${_debug} maEnv swCorrectEXPAA                       FALSE                         "${auxsw} use exp/aa adjust functions"
  /call set_data_map ${_debug} maEnv stBuildSetCurrent										FALSE                         "${auxna} name of current \a-w/build set\ax"

  /invoke ${set_data_timer[${_debug}, Waste_Fireworks, 15m]}
  /call set_data_map ${_debug} maEnv swAAFireworks                        FALSE                         "${auxsw} waste aa on fireworks"

  /call set_data_map ${_debug} maEnv swTakeGroupInvite                    FALSE                         "${auxsw} take group invites from people you trust"
  /call set_data_map ${_debug} maEnv swTakeRaidInvite                     FALSE                         "${auxsw} take raid invites from people you trust"
  /call set_data_map ${_debug} maEnv swSoS                                FALSE                         "${auxsw} rogue sos switch"

  | rest
  /call set_data_map ${_debug} maEnv stPctRest                            0                             "${auxnum} % health/mana to rest at"
  /call set_data_map ${_debug} maEnv swRestFull                           FALSE                         "${auxsw} rest to full health if possible"
  /call set_data_map ${_debug} maEnv swRestModRod                         FALSE                         "${auxsw} use mod rods while resting"
  /call set_data_map ${_debug} maEnv swRestBuff                           FALSE                         "${auxsw} do buffing while resting"
  /call set_data_map ${_debug} maEnv swRestCombat                         FALSE                         "${auxsw} continue resting whilke in combat"
  /call set_data_map ${_debug} maEnv swBegMAG                             FALSE                         "${auxsw} beg mages for pet gear or rods"

  | food and drink
  /invoke ${set_data_timer[${_debug}, Check_FoodDrink, 10m]}
  /call set_data_map ${_debug} maEnv stFood                               FALSE                         "${auxna} food item you want to eat"
  /call set_data_map ${_debug} maEnv stDrink                              FALSE                         "${auxna} drink item you want to drink"

  | tribute switch
  /call set_data_map ${_debug} maEnv swCheckTribute                       FALSE                         "${auxsw} check and corect tribute for raids"


|** control: heal **|
  /invoke ${set_data_timer[FALSE, Check_Cure, 6s]}
  /call set_data_map ${_debug} maHeal stHealAdjust                        0                             "${auxnum} % to overide max health to" 																				heal
  /call set_data_map ${_debug} maHeal swHealSelf                          FALSE                         "${auxsw} enable self healing" 																								heal
  /call set_data_map ${_debug} maHeal swHealToT                          	FALSE                         "${auxsw} enable healing your targets target" 																heal
  /call set_data_map ${_debug} maHeal swHealGroup                         FALSE                         "${auxsw} enable group healing" 																							heal
  /call set_data_map ${_debug} maHeal swHealXTarget                       FALSE                         "${auxsw} enable xtarget healing" 																						heal
  /call set_data_map ${_debug} maHeal swHealPet                           FALSE                         "${auxsw} enable pet healing" 																								heal
  /call set_data_map ${_debug} maHeal swHealCure                          FALSE                         "${auxsw} enable curing" 																											heal
  /call set_data_map ${_debug} maHeal stCureCurseItem                     FALSE                         "${auxna} name of item to cure self curses" 																	heal
  /call set_data_map ${_debug} maHeal stCurePoisonItem                    FALSE                         "${auxna} name of item to cure self poisons" 																	heal
  /call set_data_map ${_debug} maHeal stCureDiseaseItem                   FALSE                         "${auxna} name of item to cure self diseases" 																heal
  /call set_data_map ${_debug} maHeal stCureCorruptionItem                FALSE                         "${auxna} name of item to cure self corruption" 															heal
  /call set_data_list ${_debug} maHeal lsNoCureDeBuff                     EMPTY                         "${auxli} list of debuffs you DO NOT want to cure" 														heal
  /call set_data_list ${_debug} maHeal lsHealXTClass                      EMPTY                         "${auxli} classes to populate xtarget for healing use: \a-t/heal xtarget\ax" 	heal
  /call set_data_list ${_debug} maHeal lsNoHealToon                       EMPTY                         "${auxli} fuck these people.. no heals for them.." 														heal

  /call set_data ${_debug} FALSE tmpHealAdjust                            float outer 99.99
  /call set_data ${_debug} FALSE hurtID                                   int outer 0
  /call set_data ${_debug} FALSE hurtPctHP                                int outer 100
  /call set_data ${_debug} FALSE hurtCLS                                  string outer ${Me.Class.ShortName}
  /call set_data ${_debug} FALSE hurtCount                                int outer 0

 	| points for healers
  /if (${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
    /call set_data_map ${_debug} maHeal stHealPointHoT                    80                            "${auxnum} % of health to use single heal over time"													heal
		/declare _lishp listiterator local
    /vardata _lishp lsClassAll.First
    /while (!${_lishp.IsEnd}) {
      /call set_data_map ${_debug} maHeal stHealPoint${_lishp.Value}      80                   					"${auxnum} % of health to start healing:\a-w${_lishp.Value}\ax"								heal
      /invoke ${_lishp.Advance}
    }
	/call set_data_list ${_debug} maHeal lsProtectOutsideGroup              EMPTY                         "${auxli} names outside of your group to add to xtarget for healing" heal
  /call set_data_list ${_debug} maHeal lsProtectInsideRaid                EMPTY                         "${auxli} names in a raid to add to your xtarget healing (\a-wbeyond lsHealXTClass\ax)" heal
  /call set_data_list ${_debug} maHeal lsProtectNPCList                   EMPTY                         "${auxli} npcs to add to xtarget list for healing" heal

  | points for everyone else
  } else /if (!${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
    /call set_data_map ${_debug} maHeal stHealPoint${Me.Class.ShortName}  60														"${auxnum} % of health to start healing:\a-w${Me.Class.ShortName}\ax" heal
  }

  /if (${lsClassCastCure.Contains[${Me.Class.ShortName}]}) {
    /call set_data_map ${_debug} maHeal stCureCurse                       FALSE                         "${auxna} name of cure curse spell"												heal
    /call set_data_map ${_debug} maHeal stCurePoison                      FALSE                         "${auxna} name of cure poison spell"											heal
    /call set_data_map ${_debug} maHeal stCureDisease                     FALSE                         "${auxna} name of cure disease spell"											heal
    /call set_data_map ${_debug} maHeal stCureCorruption                  FALSE                         "${auxna} name of corruption cure spell"									heal
  }

  /call set_data_list ${_debug} maHeal lsLineHeal	                       	EMPTY                        	"${auxli} alternate cast order for heals" 								heal

  | xtarget sets heal classes
  /if (${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
    /declare lsXTargetClass list outer
    /invoke ${set_data_timer[FALSE, Check_Xtarget_Build, 10m]}
  }


|** control: buff **|
  /invoke ${set_data_timer[${_debug}, Check_Buff_Cycle, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Unity, 5m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Shrink, 10m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Self, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Crew, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Beg, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Temp, 30s]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Now, 5s]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Powersource, 10m]}
  /call set_data ${_debug} maBuff swBuffMaster                            map outer TRUE
  /call set_data_list ${_debug} maBuff lsBuffRemove                       EMPTY                         "${auxli} buffs you want auto removed from you"
  /call set_data_map ${_debug} maBuff swBuffSelf                          FALSE                         "${auxsw} buff yourself"
  /call set_data_map ${_debug} maBuff swBuffCrew                          FALSE                         "${auxsw} buff your crew (\aynot you\ax)"
  /call set_data_map ${_debug} maBuff swBuffRaid                          FALSE                         "${auxsw} buff raid members"
  /call set_data_map ${_debug} maBuff swBuffBeg                           FALSE                         "${auxsw} answer buff beg requests"
  /call set_data_map ${_debug} maBuff swBuffUnity                         FALSE                         "${auxsw} buff aa unity"
  /call set_data_map ${_debug} maBuff swBuffPowersource                   FALSE                         "${auxsw} use powersources (\a-wmust have buff set with tag\ax)"
  /call set_data_map ${_debug} maBuff swBuffTemp                          FALSE                         "${auxsw} buff short term combat buffs"
  /call set_data_map ${_debug} maBuff swBuffShrink                        FALSE                         "${auxsw} buff shring"
  /call set_data_map ${_debug} maBuff stShrinkItem                        FALSE                         "${auxna} buff shrink item/spell"
  /call set_data_map ${_debug} maBuff swBuffNow                           FALSE                         "${auxsw} buff the now things"
  /call set_data_map ${_debug} maBuff swBuffAmmo                          FALSE                         "${auxsw} summons ammo"

  | auras
  /invoke ${set_data_timer[${_debug}, Check_Buff_Aura, 3m]}
  /call set_data_map ${_debug} maBuff swBuffAura                          FALSE                         "${auxsw} use auras or not. i dont care"
  /for _count 1 to 2
    /call set_data_map ${_debug} maBuff stAura${_count}                   FALSE                         "${auxna} name of aura"
    /call set_data_map ${_debug} maBuff stAura${_count}Check              FALSE                         "${auxna} name of aura that appears in aura buff window"
    /call set_data_list ${_debug} maBuff lsAura${_count}Tag               EMPTY                         "${auxna} tags for your aura"
  /next _count


|** control: defense **|
  /call set_data_map ${_debug} maDefense stCountDef                       FALSE                         "${auxna} number of imediate area mobs to activate defensive"
  /call set_data_map ${_debug} maDefense stPctDef	                        FALSE                         "${auxna} pct of health to activate a defensive"
  /call set_data_list ${_debug} maDefense lsLineDefense                   EMPTY                         "${auxli} alternate defense activation order"


|** control: rez **|
  /call set_data ${_debug} FALSE swIsMeDead                               bool outer FALSE
  /call set_data ${_debug} FALSE lsRezPct                                 list outer "10|20|35|50|60|75|90|93|96"
  /call set_data_list FA${_debug}LSE maRez lsNoRezToon                		EMPTY                         "${auxli} names of people you never want to rez"
  /call set_data_list ${_debug} maRez lsRezTokenToon                      EMPTY                         "${auxli} list of class shortnames you want to use a rez token on"
  /call set_data_map ${_debug} maRez swRezGive                            FALSE                         "${auxsw} rez people"
  /call set_data_map ${_debug} maRez swRezTake                            FALSE                         "${auxsw} take rezes"
  /call set_data_map ${_debug} maRez swRezCombat                          FALSE                         "${auxsw} rez duting combat (inc using tokens if enabled)"
  /call set_data_map ${_debug} maRez swRezEveryone                        FALSE                         "${auxsw} rez people not in your group/raid/guild"
  /call set_data_map ${_debug} maRez swRezToken                           FALSE                         "${auxsw} use rez tokens (\a-wseparate from stResurrection\ax)"
  /call set_data_map ${_debug} maRez stPctMinRez                          90                            "${auxnum} minimum % rez you are willing to accept"
  /call set_data_map ${_debug} maRez stMaxRezRange                        60                            "${auxnum} how far away to look for corpses to rez"
  /call set_data_map ${_debug} maRez stResurrection                       FALSE                         "${auxna} spell/item/aa to rez with"


|** control: home **|
  /call set_data ${_debug} maHome swHomeMarker                            map outer FALSE
  /call set_data_map ${_debug} maHome stHomeVariance                      6                             "${auxnum} acceptable buffer range for being home"
  /call set_data ${_debug} maData stCampX                                 map outer 999999999
  /call set_data ${_debug} maData stCampY                                 map outer 999999999
  /call set_data ${_debug} maData stCampZ                                 map outer 999999999
  /call set_data_map ${_debug} maHome stMount                             FALSE                         "${auxna} name of mount you wish to sit your fat ass on"
  /call set_data_map ${_debug} maHome swMoveDeathClear                    FALSE                         "${auxsw} on = clear home marker and movements on death"
  /call set_data_map ${_debug} maHome swMoveBehind                        FALSE                         "${auxsw} move behind the target during combat"
  /call set_data_map ${_debug} maHome swFaceFast                          FALSE                         "${auxsw} use fast facing movements"
  /call set_data_map ${_debug} maHome stFaceArc                           15                            "${auxnum} [\a-w1-45\az] acceptable range of facing and still be on target"
  /call set_data ${_debug} maHome swSnapStuck                             map outer FALSE
	/call set_data_map ${_debug} maHome swonAuto		                        FALSE                         "${auxsw} set home camp on auto"
	/call set_data_map ${_debug} maHome swinCombat	                        FALSE                         "${auxsw} keep home camp while in combat"


|** control: tie **|
  /invoke ${set_data_timer[${_debug}, Check_Tie, 5]}
  /call set_data ${_debug} maTie swSetTie                                 map outer FALSE
  /call set_data_map ${_debug} maTie stTieToon                            FALSE                         "${auxna} name of toon you want to tie too when commanded"
  /call set_data_map ${_debug} maTie stTieMode                            nav                           "${auxna} [\a-wstick-nav\ax] the mode you want to use for tie movements"
  /call set_data_map ${_debug} maTie stTieVariance                        5                             "${auxnum} acceptabe distances to be away from the tiem toon before moving to it"


|** control: mode **|
  /call set_data ${_debug} FALSE swModeToken                              bool outer FALSE
  /call set_data ${_debug} FALSE stModeData                            		string outer FALSE


|** control: watch **|
  /call set_data ${_debug} maWatch swWatch                                map outer FALSE
  /call set_data ${_debug} maWatch stWatchCount                           map outer 0
  /call set_data_list ${_debug} maWatch lsWatch		                        EMPTY                         "${auxli} list of mobs to watch for"


|** control: cc **|
  /call set_data ${_debug} maCC swSetForced                               map outer FALSE
  /call set_data ${_debug} maCC swSetCombat                               map outer FALSE
  /call set_data ${_debug} FALSE stAssTarID                               int outer 0
  /call set_data ${_debug} maCC swValidAssTarID                           map outer FALSE
  /call set_data_map ${_debug} maCC swCombatMelee                         FALSE                         "${auxsw} do melee things while in combat"
  /call set_data_map ${_debug} maCC swCombatRange                         FALSE                         "${auxsw} do range things while in combat"
  /call set_data_map ${_debug} maCC swCombatNuke                          FALSE                         "${auxsw} nuke things while in combat"
  /call set_data_map ${_debug} maCC swCombatDoT                           FALSE                         "${auxsw} dot all the things!!"
  /call set_data_map ${_debug} maCC swCombatHeal                          FALSE                         "${auxsw} enable heal modes in or out of combat"
  /call set_data_map ${_debug} maCC swCombatControl                       FALSE                         "${auxsw} enable crowd control modes in or out of combat"
  /call set_data_map ${_debug} maCC swAlliance			                      FALSE                         "${auxsw} enable class alliance"

	| pulling
  /call set_data_map ${_debug} maCC swPull                                FALSE                         "${auxsw} enable pulling (\a-wselect mobs to kill\ax)"
  /call set_data_map ${_debug} maCC stPullMode                            base                          "${auxna} [\a-wbase-nav-int-pet-multi\ax] the mode of pulling you wish to use while looking for said mobs"
  /call set_data_map ${_debug} maCC stPullRadius                          0                             "${auxnum} radius to pull mobs from"
  /call set_data_map ${_debug} maCC swPullSetHome                         FALSE                         "${auxsw} suto set home marker when pulling is enabled"
  /call set_data_map ${_debug} maCC stPullNavStopDistance                 60                            "${auxnum} how close you will nav pull to a mob before trying too tag it"

  | glyph
  /call set_data_map ${_debug} maCC swGlyphGroup                          FALSE                         "${auxsw} use glyph in a group"
  /call set_data_map ${_debug} maCC swGlyphRaid                           FALSE                         "${auxsw} use glyph in a raid"
  /call set_data_map ${_debug} maCC stGlyph                               FALSE                         "${auxna} name of the glyph you want to use"

  | agro on or off and current agro mode (g,1,2,3,ma1,ma2)
  /call set_data_map ${_debug} maCC swAgro                                FALSE                         "${auxsw} use any agro skills you can (\a-wtaunt not included separate control under /cc\ax)"
  /call set_data_map ${_debug} maCC stAssistMode                          FALSE                         "${auxna} [\axg,1,2,3,ma1,ma2\ax] your current assist mode"
  /call set_data_map ${_debug} maCC stManualAssist1                       FALSE                         "${auxna} name of manual assist 1 toon"
  /call set_data_map ${_debug} maCC stManualAssist2                       FALSE                         "${auxna} name of manual assist 2 toon"
  /call set_data_map ${_debug} maCC stPctEngage                           98                            "${auxnum} mobs % health to engage it"

  /call set_data_map ${_debug} maCC stZRadius                             60                            "${auxnum} the z distance of your universe"

  /call set_data_list ${_debug} maCC lsNoCastWith                         EMPTY                         "${auxli} list of target buffs you dont want to cast with"
  /call set_data_list ${_debug} maCC lsNoMeleeWith                        EMPTY                         "${auxli} list of target buffs you dont want to melee with"
  /call set_data_map ${_debug} maCC stRangeOther                          FALSE                         "${auxna} something else you would like to use for ranged pulling?"

  | combat melee skills by class
  /declare lsMeleeSkill list outer

  | specific lists by class
  /if (${Select[${Me.Class.ShortName},BER]}) {
    /invoke ${lsMeleeSkill.Append[frenzy,intimidation,disarm]}
    /call set_data_map ${_debug} maCC swFrenzy                            FALSE                         "${auxsw} some odd form of ocd for zerkers.. who knows."
    /call set_data_map ${_debug} maCC swIntimidation                      FALSE                         "${auxsw} you think you're all that.. you aint nothing.."
    /call set_data_map ${_debug} maCC swDisarm                            FALSE                         "${auxsw} dodge, parry, thrust.."

  } else /if (${Select[${Me.Class.ShortName},BRD]}) {
    /invoke ${lsMeleeSkill.Append[kick,intimidation,disarm]}
    /call set_data_map ${_debug} maCC swKick                              FALSE                         "${auxsw} raise foot, impact nuts.."
    /call set_data_map ${_debug} maCC swIntimidation                      FALSE                         "${auxsw} you think you're all that.. you aint nothing.."
    /call set_data_map ${_debug} maCC swDisarm                            FALSE                         "${auxsw} dodge, parry, thrust.."

  } else /if (${Select[${Me.Class.ShortName},BST]}) {
    /invoke ${lsMeleeSkill.Append[dragonpunch,eaglestrike,tigerclaw,flyingkick,roundkick,kick]}
    /call set_data_map ${_debug} maCC swKick                              FALSE                         "${auxsw} raise foot, impact nuts.."
    /call set_data_map ${_debug} maCC swDragonpunch                       FALSE                         "${auxsw} use dragon punch"
    /call set_data_map ${_debug} maCC swEaglestrike                       FALSE                         "${auxsw} use eagle strike"
    /call set_data_map ${_debug} maCC swTigerclaw                         FALSE                         "${auxsw} use tiger claw"
    /call set_data_map ${_debug} maCC swFlyingkick                        FALSE                         "${auxsw} use flying kick"
    /call set_data_map ${_debug} maCC swRoundkick                         FALSE                         "${auxsw} use round kick"

  } else /if (${Select[${Me.Class.ShortName},CLR]}) {
    /invoke ${lsMeleeSkill.Append[bash]}
    /call set_data_map ${_debug} maCC swBash                              FALSE                         "${auxsw} switch for bashing the shit out of things"

  } else /if (${Select[${Me.Class.ShortName},DRU]}) {

  } else /if (${Select[${Me.Class.ShortName},ENC]}) {

  } else /if (${Select[${Me.Class.ShortName},MAG]}) {

  } else /if (${Select[${Me.Class.ShortName},MNK]}) {
    /invoke ${lsMeleeSkill.Append[kick,dragonpunch,eaglestrike,tigerclaw,flyingkick,roundkick,intimidation]}
    /call set_data_map ${_debug} maCC swKick                              FALSE                         "${auxsw} raise foot, impact nuts.."
    /call set_data_map ${_debug} maCC swDragonpunch                       FALSE                         "${auxsw} use dragon punch"
    /call set_data_map ${_debug} maCC swEaglestrike                       FALSE                         "${auxsw} use eagle strike"
    /call set_data_map ${_debug} maCC swTigerclaw                         FALSE                         "${auxsw} use tiger claw"
    /call set_data_map ${_debug} maCC swFlyingkick                        FALSE                         "${auxsw} use flying kick"
    /call set_data_map ${_debug} maCC swRoundkick                         FALSE                         "${auxsw} use round kick"
    /call set_data_map ${_debug} maCC swIntimidation                      FALSE                         "${auxsw} you think you're all that.. you aint nothing.."

  } else /if (${Select[${Me.Class.ShortName},NEC]}) {
    | /call set_data ${_debug} FALSE lsHealMode                             list outer "base"

  } else /if (${Select[${Me.Class.ShortName},PAL]}) {
    /invoke ${lsMeleeSkill.Append[bash,taunt,disarm]}
    /call set_data_map ${_debug} maCC swBash                              FALSE                         "${auxsw} switch for bashing the shit out of things"
    /call set_data_map ${_debug} maCC swTaunt                             FALSE                         "${auxsw} you mama is sooo fat!"
    /call set_data_map ${_debug} maCC swDisarm                            FALSE                         "${auxsw} dodge, parry, thrust.."

  } else /if (${Select[${Me.Class.ShortName},RNG]}) {
    /invoke ${lsMeleeSkill.Append[taunt,kick,disarm]}
    /call set_data_map ${_debug} maCC swTaunt                             FALSE                         "${auxsw} you mama is sooo fat!"
    /call set_data_map ${_debug} maCC swKick                              FALSE                         "${auxsw} raise foot, impact nuts.."
    /call set_data_map ${_debug} maCC swDisarm                            FALSE                         "${auxsw} dodge, parry, thrust.."

  } else /if (${Select[${Me.Class.ShortName},ROG]}) {
 		/invoke ${lsMeleeSkill.Append[backstab,intimidation,disarm]}
    /call set_data_map ${_debug} maCC swBackstab                          FALSE                         "${auxsw} surprise butt sekx!"
    /call set_data_map ${_debug} maCC swIntimidation                      FALSE                         "${auxsw} you think you're all that.. you aint nothing.."
    /call set_data_map ${_debug} maCC swDisarm                            FALSE                         "${auxsw} dodge, parry, thrust.."

  } else /if (${Select[${Me.Class.ShortName},SHD]}) {
 		/invoke ${lsMeleeSkill.Append[taunt,disarm,bash]}
    /call set_data_map ${_debug} maCC swTaunt                             FALSE                         "${auxsw} you mama is sooo fat!"
    /call set_data_map ${_debug} maCC swDisarm                            FALSE                         "${auxsw} dodge, parry, thrust.."
    /call set_data_map ${_debug} maCC swBash                              FALSE                         "${auxsw} switch for bashing the shit out of things"

  } else /if (${Select[${Me.Class.ShortName},SHM]}) {

  } else /if (${Select[${Me.Class.ShortName},WAR]}) {
    /invoke ${lsMeleeSkill.Append[bash,taunt,kick,disarm]}
    /call set_data_map ${_debug} maCC swBash                              FALSE                         "${auxsw} switch for bashing the shit out of things"
    /call set_data_map ${_debug} maCC swTaunt                             FALSE                         "${auxsw} you mama is sooo fat!"
    /call set_data_map ${_debug} maCC swKick                              FALSE                         "${auxsw} raise foot, impact nuts.."
    /call set_data_map ${_debug} maCC swDisarm                            FALSE                         "${auxsw} dodge, parry, thrust.."

  } else /if (${Select[${Me.Class.ShortName},WIZ]}) {
  }


|** control: dot **|
  /call set_data_map ${_debug} maDoT stDotRefresh                         3                             "${auxnum} time left on a dot to refresh it, in seconds"
  /call set_data_map ${_debug} maDoT stPctStopDoT                       	0                            	"${auxnum} % of mob health to stop dotting it at"
  /call set_data_list ${_debug} maDoT lsLineDoT		                       	EMPTY                        	"${auxli} alternate cast order for dots" dot


|** control: nuke **|
  /call set_data_map ${_debug} maNuke stPctStopNuke                       0                             "${auxnum} % of mob health to stop nuking it at"
  /call set_data_map ${_debug} maNuke stNukeDelay                         0                             "${auxnum} time to force between using nukes or dots"
  /call set_data_list ${_debug} maNuke lsLineNuke	                       	EMPTY                        	"${auxli} alternate cast order for nukes" nuke


|** control: burn **|
  /call set_data ${_debug} maBurn swBurnConditionMet                      map outer FALSE
  /call set_data_map ${_debug} maBurn swBurnForce                         FALSE                         "${auxsw} force burn routines"																		burn
  /call set_data_map ${_debug} maBurn swBurnAuto                          FALSE                         "${auxsw} auto burn on named encounters"													burn
  /call set_data_map ${_debug} maBurn stPctBurnEngage                     99                            "${auxnum} % of mobs health to start burns"												burn
  /call set_data_map ${_debug} maBurn stBurnCount                         0                             "${auxnum} number of mobs in your area to auto engage burns"			burn
  /call set_data_map ${_debug} maBurn swBurnRaid                          FALSE                         "${auxsw} use burns on raids"																			burn
  /call set_data_list ${_debug} maBurn lsLineBurn                         FALSE                         "${auxli} alternate burn line order (engine 2 only)"							burn

	| /call sql_control FALSE ${chrID} stoponDS                           bool      outer FALSE


|** control: debuff **|
  /call set_data_map ${_debug} madeBuff swCripple                         FALSE                         "${auxsw} use cripples"
  /call set_data_map ${_debug} madeBuff stCripple                         FALSE                         "${auxna} single target cripple"
  /call set_data_map ${_debug} madeBuff stAECripple                       FALSE                         "${auxna} ae cripple to use"
  /declare lsdeBuffCripple list outer

  /call set_data_map ${_debug} madeBuff swMez                             FALSE                         "${auxsw} use mez"
  /call set_data_map ${_debug} madeBuff stMez                             FALSE                         "${auxna} single target mez"
  /call set_data_map ${_debug} madeBuff stCountMezResist                  3			                        "${auxnum} number of times to retry single mez with resists"
  /call set_data_map ${_debug} madeBuff stAEMez                           FALSE                         "${auxna} ae mez for when shit gets deep"

  /call set_data_map ${_debug} madeBuff swMalo                            FALSE                         "${auxsw} use malo"
  /call set_data_map ${_debug} madeBuff stMalo                            FALSE                         "${auxna} single target malo"
  /call set_data_map ${_debug} madeBuff stAEMalo                          FALSE                         "${auxna} ae malo"
  /declare lsdeBuffMalo list outer

  /call set_data_map ${_debug} madeBuff swTash                            FALSE                         "${auxsw} use tash"
  /call set_data_map ${_debug} madeBuff stTash                            FALSE                         "${auxna} single target tash"
  /call set_data_map ${_debug} madeBuff stAETash                          FALSE                         "${auxna} ae tash cause fuck all the mobs"
  /declare lsdeBuffTash list outer

  /call set_data_map ${_debug} madeBuff swSlow                            FALSE                         "${auxsw} use slow"
  /call set_data_map ${_debug} madeBuff stSlow                            FALSE                         "${auxna} single target slow"
  /call set_data_map ${_debug} madeBuff stAESlow                          FALSE                         "${auxna} ae slow"
  /declare lsdeBuffSlow list outer

  /call set_data_map ${_debug} madeBuff swEradicate                       FALSE                         "${auxsw} use eradicate"
  /call set_data_map ${_debug} madeBuff stEradicate                       FALSE                         "${auxna} single target eradicate"
  /call set_data_map ${_debug} madeBuff stAEEradicate                     FALSE                         "${auxna} ae eradicate"
  /call set_data_map ${_debug} madeBuff stTimerEradicate                  30s                           "${auxnum} time to retry an eradicate on a target"
  /declare lsdeBuffEradicate list outer

  /call set_data_map ${_debug} madeBuff swSnare                           FALSE                         "${auxsw} use snare"
  /call set_data_map ${_debug} madeBuff stSnare                           FALSE                         "${auxna} single target snare"
  /call set_data_map ${_debug} madeBuff stAESnare                         FALSE                         "${auxna} ae snare"
  /declare lsdeBuffSnare list outer


|** control: minion **|
  /invoke ${set_data_timer[FALSE, Check_Minion_Cycle, 1m]}
  /call set_data_map ${_debug} maMinion swPet                             FALSE                         "${auxsw} use pets"
  /call set_data_map ${_debug} maMinion stPetType                         FALSE                         "${auxna} name of pet spell to summon pet with"
  /call set_data_map ${_debug} maMinion stPetIllusion                     FALSE                         "${auxna} name of pet illusion whatever spell, clickie, left over dinner"
  /call set_data_map ${_debug} maMinion stPctPetEngage                    95                            "${auxnum} % of mob health to engage pet at"
  /call set_data_map ${_debug} maMinion swSwarm                           FALSE                         "${auxsw} use swarm pets/items/aas/left shoe"
  /call set_data_map ${_debug} maMinion stPctSwarmEngage                  95                            "${auxnum} % of mob health to engage swarm pets at"
  /call set_data_map ${_debug} maMinion swNoSwarmonDS                     FALSE                         "${auxsw} use swarms on mobs woth a damage shield"
  /call set_data_map ${_debug} maMinion swonForce                         FALSE                         "${auxsw} send pets on \a-w/cc force\ax command"
  /call set_data_map ${_debug} maMinion swUsePetGear                      FALSE                         "${auxsw} use pet weapons"
  /call set_data_map ${_debug} maMinion stPetWeapon                       FALSE                         "${auxna} weapon to give your pet"

  /invoke ${set_data_timer[FALSE, Check_Minion_Shrink, 10m]}
  /call set_data_map ${_debug} maMinion swPetShrink                       FALSE                         "${auxsw} use pet shrink item/spell/drugs?"
  /call set_data_map ${_debug} maMinion stPetShrink                       FALSE                         "${auxna} name of pet shrink thingie"

  /invoke ${set_data_timer[FALSE, Check_Minion_Buff, 1m]}
  /call set_data_map ${_debug} maMinion swPetBuff                         FALSE                         "${auxsw} buff minion (\ayyours\ax)"


|** control: over **|
  /call set_data_map ${_debug} maOver swOverLOS                           FALSE                         "${auxsw} override line of sight validations"
  /call set_data_map ${_debug} maOver swOverPctEngage                     FALSE                         "${auxsw} ocerride engage % for all types, instant engage of a mob"
  /call set_data_map ${_debug} maOver swOverLoot                          FALSE                         "${auxsw} attempt to loot regardless of master looter"
  /call set_data_map ${_debug} maOver swOverSafeNames                     FALSE                         "${auxsw} ignore safename checks, trust everyone. pm me your account info please"
  /call set_data_map ${_debug} maOver swOverTrainSpell                    FALSE                         "${auxsw} ignore spell training caps. cast forever"
  /call set_data_map ${_debug} maOver swOverEnvAuto		                    FALSE                         "${auxsw} ignore non-persistent auto and start with \a-w/env auto on\ax"
  /call set_data_map ${_debug} maOver swOverDSCheck		                    FALSE                         "${auxsw} ignore damage shield checks while melee"
  /call set_data_map ${_debug} maOver swOverNavCheck		                  FALSE                         "${auxsw} bypass Nav PathExists checks"


|** control: entropy  **|
  /call set_data_map ${_debug} maEntropy swMaintenance                    FALSE                         "${auxsw} place entropy in a limited maintenance mode for operations"
  /call set_data_map ${_debug} maEntropy swEventsInc                      FALSE                         "${auxsw} use events from events.inc file"
  /call set_data_map ${_debug} maEntropy swDPSMeter                       FALSE                         "${auxsw} use dps meeter"
  /call set_data_map ${_debug} maEntropy swHookSub                        FALSE                         "${auxsw} use hook functions"
  /call set_data_map ${_debug} maEntropy stNetworkTimeout                 1                             "${auxnum} dannet timeout for network lag. ignore this if at all possible"
  /call set_data_list ${_debug} maEntropy lsSafeNames                     EMPTY                         "${auxli} list of people you trust (\a-wDanNet connections trusted by default\ax)"
  /call set_data_map ${_debug} maEntropy swAutoList                       FALSE                         "${auxsw} autopopulate zone listsnames for some mob types"
  /call set_data_map ${_debug} maEntropy stBuffRefresh                    0                             "${auxnum} time in seconds to refrsh a buff before it fades. 0=wait till it fades"
  /call set_data_map ${_debug} maEntropy swAutoInvManual                  FALSE                         "${auxsw} obey autoinventory fast list even in manual mode"
  /call set_data_map ${_debug} maEntropy swBuildSetRemember								FALSE                         "${auxsw} remember and refresh a set on macro boot"
  /call set_data_map ${_debug} maEntropy stCastLoop												1			                   			"${auxna} number of castloop attempts, event based"
  /call set_data_map ${_debug} maEntropy stEngine													1			                   			"${auxna} code engine"

  | GoM list
  /call set_data_list ${_debug} maChr lsGoM                               EMPTY                         "${auxli} things to use on a gift of mana aa proc"

	| ae count you act on
  /call set_data_map ${_debug} maChr stCountAE                            0                             "${auxnum} number of mobs in your area to even consider using ae skills"

  | fade
  /call set_data_map ${_debug} maChr stPctFade                            0                             "${auxnum} % of your healt to use class aa fade skill"

  | spire
  /call set_data_map ${_debug} maChr swSpireRaid                          FALSE                         "${auxsw} use while in group for burns"
  /call set_data_map ${_debug} maChr swSpireGroup                         FALSE													"${auxsw} use while in a raid for burns"
  /call set_data_map ${_debug} maChr stSpireCon         	                0															"${auxnum} number of the condition to use for spires"

  | can cast?
  /if (${lsClassCast.Contains[${Me.Class.ShortName}]}) {
    /call set_data_map ${_debug} maChr stPctManaRecovery                   0                      	    "${auxna} % to use whatever manna recovery things entropy can find on you"
  }


|** control: clickitem  **|
  /call set_data_list ${_debug} maItem lsLineItem	                       	EMPTY                        	"${auxli} alternate cast order for clickitems" item


|** control: TTL  **|
  /call set_data_map ${_debug} maTTL swTTL				                        FALSE                         "${auxsw} use ttl logic"


|** control: MiscDPS  **|
  /call set_data_list ${_debug} maMiscDPS lsLineMiscdps                   EMPTY                         "${auxli} alternate activation order for miscdps routine (engine 2)"


|** control: Agro  **|
  /call set_data_list ${_debug} maAgro lsLineAgro              						EMPTY                         "${auxli} alternate activation order for agro routine (engine 2)"


|** control: burn  **|
  /call set_data_list ${_debug} maAgro lsLineBurn              						EMPTY                         "${auxli} alternate activation order for burn routine (engine 2)"


|** control: sash  **|
  /invoke ${set_data_timer[FALSE, Check_Proc_Sash, 5s]}
  /call set_data_map ${_debug} maSash swSashProc                     			FALSE                         "${auxsw} enable sash for a procing weapon"								sash
  /call set_data_map ${_debug} maSash stSashProc													FALSE                         "${auxna} name of sash with procing weapon"								sash

  /call set_data_map ${_debug} maSash swSash			                    		FALSE                         "${auxsw} enable auto sash swapping based off conditions" 									sash

	| tank shield flash
	/if (${Select[${Me.Class.ShortName},PAL,WAR,SHD]}) {
		/call set_data_map ${_debug} maSash stPctShieldFlash									0															"${auxnum} % of HP to try to get shield flash running"		sash
		/call set_data_map ${_debug} maSash stCountShieldFlash								0															"${auxnum} # of mobs to hit shield flash"									sash
 	 	/call set_data_map ${_debug} maSash stSashShieldFlash									FALSE                         "${auxna} name of sash to be used for Slash skills"				sash						sash
	}



	/if (${maDebug.Find[init].Value}) {
		OUT \agset_initialize_map_character_environment\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: set/build crew lists/rolls
 * use:
 ***|
sub set_initialize_crew(bool _debug)
  DEBUG \atset_initialize_crew\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

  /declare _count int local 0

  /for _count 1 to 12
    /call set_data_list ${_debug} maCrew lsCrew${_count}              EMPTY                         "${auxli} list for crew #${_count}"	crew
    /call set_data_map ${_debug} maCrew stCrew${_count}Leader         FALSE                         "${auxna} crew #${_count} leader"	crew
    /call set_data_map ${_debug} maCrew stCrew${_count}Looter         FALSE                         "${auxna} crew #${_count} looter"	crew
    /call set_data_map ${_debug} maCrew stCrew${_count}Assist         FALSE                         "${auxna} crew #${_count} assist"	crew
  /next _count

	/call set_data ${_debug} FALSE stCurrentCrew												string outer EMPTY

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_initialize_crew\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: set/build buffs
 * use:
 ***|
sub set_initialize_map_buffs(bool _debug)
  DEBUG \atset_initialize_map_buffs\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

  /declare _count int local 0
  /for _count 1 to 50
    /call set_data_map ${_debug} maBuff swBuff${_count}              FALSE													"${auxsw} buff ${_count} switch"	buff
    /call set_data_map ${_debug} maBuff stBuff${_count}              FALSE													"${auxna} buff ${_count} name"	buff
    /call set_data_map ${_debug} maBuff stBuff${_count}Alias         FALSE													"${auxna} buff ${_count} alias"	buff
    /call set_data_list ${_debug} maBuff lsBuff${_count}Tag          EMPTY													"${auxli} buff ${_count} tag list"	buff
    /call set_data_map ${_debug} maBuff stBuff${_count}Con           0															"${auxnum} buff ${_count} condition"	buff
  /next _count

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_initialize_map_buffs\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}


/return TRUE



|***
 * note: set/build the 12 count skill maps
 * use:
 ***|
sub set_initialize_map_12(bool _debug)
  DEBUG \atset_initialize_map_12\ax()

 	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/declare _tmpMap map local
	/declare _tmi mapiterator local
	/declare _liC12 listiterator local
	/invoke ${_tmpMap.Add[Sash,sash]}

  /vardata _tmi _tmpMap.First.Clone
  /while (!${_tmi.IsEnd}) {
		/vardata _liC12 lsCount12.First.Clone
		/while (!${_liC12.IsEnd}) {
	    /call set_data_map ${_debug} ma${_tmi.Key} sw${_tmi.Key}${_liC12.Value}				FALSE		"${auxsw} ${_tmi.Value} ${_liC12.Value} switch"	${_tmi.Value}
	    /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC12.Value}				FALSE		"${auxna} ${_tmi.Value} ${_liC12.Value} name"	${_tmi.Value}
	    /call set_data_list ${_debug} ma${_tmi.Key} ls${_tmi.Key}${_liC12.Value}Tag		EMPTY		"${auxli} ${_tmi.Value} ${_liC12.Value} tag list"	${_tmi.Value}
	    /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC12.Value}Con		0				"${auxnum} ${_tmi.Value} ${_liC12.Value} condition"	${_tmi.Value}
			/invoke ${_liC12.Advance}
		}

		/if (${maDebug.Find[init].Value}) {
			OUT \agset_initialize_map_heal\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
			/mqp
		}

    /invoke ${_tmi.Advance}
  }

/return TRUE



|***
 * note: set/build the 20 count skill maps
 * use:
 ***|
sub set_initialize_map_20(bool _debug)
  DEBUG \atset_initialize_map_20\ax()

 	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/declare _tmpMap map local
	/declare _tmi mapiterator local
	/declare _liC20 listiterator local
	/invoke ${_tmpMap.Add[DoT,dot]}
	/invoke ${_tmpMap.Add[Nuke,nuke]}
	/invoke ${_tmpMap.Add[Heal,heal]}
	/invoke ${_tmpMap.Add[Defense,defense]}
	/invoke ${_tmpMap.Add[Item,item]}
	/invoke ${_tmpMap.Add[Agro,agro]}
	/invoke ${_tmpMap.Add[Song,song]}
	/invoke ${_tmpMap.Add[Burn,burn]}

  /vardata _tmi _tmpMap.First.Clone
  /while (!${_tmi.IsEnd}) {
		/vardata _liC20 lsCount20.First.Clone
		/while (!${_liC20.IsEnd}) {
	    /call set_data_map ${_debug} ma${_tmi.Key} sw${_tmi.Key}${_liC20.Value}				FALSE		"${auxsw} ${_tmi.Value} ${_liC20.Value} switch"	${_tmi.Value}
	    /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC20.Value}				FALSE		"${auxna} ${_tmi.Value} ${_liC20.Value} name"	${_tmi.Value}
	    /call set_data_list ${_debug} ma${_tmi.Key} ls${_tmi.Key}${_liC20.Value}Tag		EMPTY		"${auxli} ${_tmi.Value} ${_liC20.Value} tag list"	${_tmi.Value}
	    /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC20.Value}Con		0				"${auxnum} ${_tmi.Value} ${_liC20.Value} condition"	${_tmi.Value}
			/invoke ${_liC20.Advance}
		}

		/if (${maDebug.Find[init].Value}) {
			OUT \agset_initialize_map_heal\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
			/mqp
		}

    /invoke ${_tmi.Advance}
  }

/return TRUE



|***
 * note: set/build the 50 count skill maps
 * use:
 ***|
sub set_initialize_map_50(bool _debug)
  DEBUG \atset_initialize_map_50\ax()

 	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

	/declare _tmpMap map local
	/declare _tmi mapiterator local
	/declare _liC50 listiterator local
	/invoke ${_tmpMap.Add[MiscDPS,miscdps]}

  /vardata _tmi _tmpMap.First.Clone
  /while (!${_tmi.IsEnd}) {
		/vardata _liC50 lsCount50.First.Clone
		/while (!${_liC50.IsEnd}) {
	    /call set_data_map ${_debug} ma${_tmi.Key} sw${_tmi.Key}${_liC50.Value}				FALSE		"${auxsw} ${_tmi.Value} ${_liC50.Value} switch"	${_tmi.Value}
	    /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC50.Value}				FALSE		"${auxna} ${_tmi.Value} ${_liC50.Value} name"	${_tmi.Value}
	    /call set_data_list ${_debug} ma${_tmi.Key} ls${_tmi.Key}${_liC50.Value}Tag		EMPTY		"${auxli} ${_tmi.Value} ${_liC50.Value} tag list"	${_tmi.Value}
	    /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC50.Value}Con		0				"${auxnum} ${_tmi.Value} ${_liC50.Value} condition"	${_tmi.Value}
			/invoke ${_liC50.Advance}
		}

		/if (${maDebug.Find[init].Value}) {
			OUT \agset_initialize_map_heal\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
			/mqp
		}

    /invoke ${_tmi.Advance}
  }

/return TRUE



|***
 * note: set/build conditions
 * use:
 ***|
sub set_initialize_map_conditions(bool _debug)
  DEBUG \atset_initialize_map_conditions\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

  /declare _count int local 0
  /for _count 1 to 50
    /call set_data_map ${_debug} maCondition c${_count}                      			FALSE													"${auxnum} condition ${_count}"	condition
  /next _count

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_initialize_map_conditions\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE



|***
 * note: maps for data that users will not change
 * use: don't fucking use it..
 ***|
sub set_initialize_static_map(bool _debug)
  DEBUG \atset_initialize_static_map\ax()

  /declare maStaticMaps map local
  /invoke ${maStaticMaps.Add[maShade, shade]}
  /invoke ${maStaticMaps.Add[maSQLError, sql_error]}
  /invoke ${maStaticMaps.Add[maManaRecovery, mana_recovery]}
  /invoke ${maStaticMaps.Add[maExpansion, env_xpac]}
  /invoke ${maStaticMaps.Add[maCure, cure]}

  | leave this out. only for design
  | /invoke ${maStaticMaps.Add[maSPA, eq_spa]}

  /declare _count int local 0
  /declare _mi mapiterator local
  /vardata _mi maStaticMaps.First.Clone
  /while (!${_mi.IsEnd}) {
    /sqlite ${maData.Find[DBstatic].Value} dummycount SELECT Count(*) as data FROM ${_mi.Value}
    /for _count 1 to ${sqlite.Result[dummycount 1 data]}
      /sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM ${_mi.Value} WHERE ROWID=${_count}
      DEBUG ${sep}${_mi.Key}${sep}${sqlite.Result[dummy 1 env_var]}${sep}\a-w${sqlite.Result[dummy 1 value]}\ax
      /call set_data ${_debug} "${_mi.Key}" "${sqlite.Result[dummy 1 env_var]}" map outer "${sqlite.Result[dummy 1 value]}"
    /next _count
    /invoke ${_mi.Advance}
  }

/return TRUE



|***
 * note: sets any aliases
 * use: /call set_alias DEBUG [add|delete]
 ***|
sub set_alias(bool _debug, string _action)
  DEBUG \atset_alias\ax()

	/if (${maDebug.Find[init].Value}) {
		/declare _boottimestart int local ${MacroQuest.Running}
	}

  /declare maAlias map outer
  /declare _maal mapiterator local
  /invoke ${maAlias.Add[/on,/command_on]}
  /invoke ${maAlias.Add[/off,/command_off]}

  | add aliases
  /if (${_action.Equal[add]}) {
    /vardata _maal maAlias.First.Clone
    /while (!${_maal.IsEnd}) {
      | /if (!${Alias[${mi.Key}]}) {
      	/squelch /alias ${_maal.Key} ${_maal.Value}
      | }
      /invoke ${_maal.Advance}
    }

  | delete established aliases
  } else /if (${_action.Equal[delete]}) {
    /vardata _maal maAlias.First.Clone
    /while (!${_maal.IsEnd}) {
      /squelch /alias ${_maal.Key} delete
      /invoke ${_maal.Advance}
    }
  }

	/if (${maDebug.Find[init].Value}) {
		OUT \agset_alias\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
		/mqp
	}

/return TRUE




|***
 * note: build all the hud variables
 * use:
 ***|
sub set_initialize_hud_variables()

  | running mode variable
  /invoke ${maData.Add[mode,idle]}
  /invoke ${maData.Add[swHUDTime,TRUE]}
  /declare stHUDNotice string outer

  /declare lsHUD list local
  /invoke ${lsHUD.Append[build,mode,time]}
  /invoke ${lsHUD.Append[engage,swpet,engagepet,swswarm,engageswarm,env]}
  /invoke ${lsHUD.Append[auto,melee,range,nuke,dot,heal,control]}
  /invoke ${lsHUD.Append[override,self,group,xt,pet,cure,tot]}
  /invoke ${lsHUD.Append[bmaster,bself,bcrew,braid,bbeg,btemp,bunity,baura,bnow,bpet,bammo]}
  /invoke ${lsHUD.Append[agro,pull,pullarea,pullmode]}
  /invoke ${lsHUD.Append[tie,tietoon,tiemode]}
  /invoke ${lsHUD.Append[buauto,bupct,bucount,buraid]}
  /invoke ${lsHUD.Append[rpct,rfull,rrod,rbuff,rcombat]}
  /invoke ${lsHUD.Append[zgive,ztake,zcombat,zeveryone,ztoken]}
  /invoke ${lsHUD.Append[eset]}

  | dev hud variables
  /invoke ${lsHUD.Append[dcombat,dburn,denv,dpull,dsafe,dcaststatus]}

	/declare _lisihv listiterator local
  /vardata _lisihv lsHUD.First.Clone
  /while (!${_lisihv.IsEnd}) {
    /declare h_${_lisihv.Value} string outer
    /invoke ${_lisihv.Advance}
  }

/return TRUE





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

    HUDPATH entropy IVU1   	${Parse[0, "3,134,0,0,255,0,${If[${Me.SPA[28]}||${Me.SPA[315]},IVU,]}"]}
    HUDPATH entropy IVU2   	${Parse[0, "3,134,0,255,0,0,${If[${Me.SPA[28]}||${Me.SPA[315]},,IVU]}"]}
    HUDPATH entropy INV1   	${Parse[0, "3,153,0,0,255,0,${If[${Me.SPA[12]}||${Me.SPA[314]},IV,]}"]}
    HUDPATH entropy INV2   	${Parse[0, "3,153,0,255,0,0,${If[${Me.SPA[12]}||${Me.SPA[314]},,IV]}"]}

    HUDPATH entropy BG0     3,131,0,0,0,0,██████████████████████████

    | conbat controls
    HUDPATH entropy ccA1    ${Parse[0, "19,134,14,255,255,0,${If[${h_auto},A,]}"]}
    HUDPATH entropy ccA2    ${Parse[0, "19,134,9,25,25,25,${If[${h_auto},,_]}"]}
    HUDPATH entropy ccEnv1  ${Parse[0, "19,146,14,140,140,140,${h_env}"]}

    | pulling
    HUDPATH entropy ccPM1   ${Parse[0, "19,170,14,0,255,0,${If[${h_pull},${h_pullmode},]}"]}
    HUDPATH entropy ccPM2   ${Parse[0, "19,170,14,255,0,0,${If[${h_pull},,${h_pullmode}]}"]}
    HUDPATH entropy ccPR1   ${Parse[0, "19,200,14,255,255,255,${If[${h_pullarea},${h_pullarea},]}"]}
    HUDPATH entropy ccPR2   ${Parse[0, "19,200,9,25,25,25,${If[${h_pullarea},,___]}"]}

    | agro
    HUDPATH entropy ccAg1   ${Parse[0, "19,225,14,255,255,255,${If[${h_agro},A,]}"]}
    HUDPATH entropy ccAg2   ${Parse[0, "19,225,9,25,25,25,${If[${h_agro},,_]}"]}

    | conbat modes
    HUDPATH entropy ccM1    ${Parse[0, "19,255,14,255,255,255,${If[${h_melee},M,]}"]}
    HUDPATH entropy ccM2    ${Parse[0, "19,255,9,25,25,25,${If[${h_melee},,_]}"]}
    HUDPATH entropy ccR1    ${Parse[0, "19,264,14,255,255,255,${If[${h_range},R,]}"]}
    HUDPATH entropy ccR2    ${Parse[0, "19,264,9,25,25,25,${If[${h_range},,_]}"]}
    HUDPATH entropy ccN1    ${Parse[0, "19,273,14,255,255,255,${If[${h_nuke},N,]}"]}
    HUDPATH entropy ccN2    ${Parse[0, "19,273,9,25,25,25,${If[${h_nuke},,_]}"]}
    HUDPATH entropy ccD1    ${Parse[0, "19,282,14,255,255,255,${If[${h_dot},D,]}"]}
    HUDPATH entropy ccD2    ${Parse[0, "19,282,9,25,25,25,${If[${h_dot},,_]}"]}
    HUDPATH entropy ccH1    ${Parse[0, "19,291,14,255,255,255,${If[${h_heal},H,]}"]}
    HUDPATH entropy ccH2    ${Parse[0, "19,291,9,25,25,25,${If[${h_heal},,_]}"]}
    HUDPATH entropy ccC1    ${Parse[0, "19,300,14,255,255,255,${If[${h_control},C,]}"]}
    HUDPATH entropy ccC2    ${Parse[0, "19,300,9,25,25,25,${If[${h_control},,_]}"]}


	| row 13
    HUDPATH entropy BG13    3,131,13,0,0,0,██████████████████████████


	| row 26

    | build mode
    HUDPATH entropy build1  3,5,26,100,149,237,build
    HUDPATH entropy build2  ${Parse[0, "19,40,26,140,140,140,${h_build}"]}

    | healing
    | HUDPATH entropy h2      19,133,26,255,255,255,${h_healmode}
    HUDPATH entropy hS1     ${Parse[0, "19,133,26,255,255,255,${If[${h_self},S,]}"]}
    HUDPATH entropy hS2     ${Parse[0, "19,133,21,25,25,25,${If[${h_self},,_]}"]}
    HUDPATH entropy hG1     ${Parse[0, "19,142,26,255,255,255,${If[${h_group},G,]}"]}
    HUDPATH entropy hG2     ${Parse[0, "19,142,21,25,25,25,${If[${h_group},,_]}"]}
    HUDPATH entropy hX1     ${Parse[0, "19,151,26,255,255,255,${If[${h_xt},X,]}"]}
    HUDPATH entropy hX2     ${Parse[0, "19,151,21,25,25,25,${If[${h_xt},,_]}"]}
    HUDPATH entropy hP1     ${Parse[0, "19,160,26,255,255,255,${If[${h_pet},P,]}"]}
    HUDPATH entropy hP2     ${Parse[0, "19,160,21,25,25,25,${If[${h_pet},,_]}"]}
    HUDPATH entropy hC1     ${Parse[0, "19,169,26,255,255,255,${If[${h_cure},C,]}"]}
    HUDPATH entropy hC2     ${Parse[0, "19,169,21,25,25,25,${If[${h_cure},,_]}"]}
   	HUDPATH entropy hT1     ${Parse[0, "19,178,26,255,255,255,${If[${h_tot},T,]}"]}
    HUDPATH entropy hT2     ${Parse[0, "19,178,21,25,25,25,${If[${h_tot},,_]}"]}
    HUDPATH entropy ho1     ${Parse[0, "19,187,26,255,255,255,${If[${h_override},${h_override},]}"]}
    HUDPATH entropy ho2     ${Parse[0, "19,187,21,25,25,25,${If[${h_override},,__]}"]}

    | tie
    HUDPATH entropy tie1    ${Parse[0, "19,255,26,100,149,237,${h_tiemode}"]}
    HUDPATH entropy tie2    ${Parse[0, "19,280,26,0,255,0,${If[${Bool[${h_tietoon}]},${If[${Bool[${Spawn[${h_tietoon}]}]},${h_tietoon},]},]}"]}
    HUDPATH entropy tie3    ${Parse[0, "19,280,26,255,0,0,${If[${Bool[${h_tietoon}]},${If[!${Bool[${Spawn[${h_tietoon}]}]},${If[${Bool[${h_tietoon}]},${h_tietoon},]},]},]}"]}
    HUDPATH entropy tie4    ${Parse[0, "19,280,22,25,25,25,${If[${Bool[${h_tietoon}]},,____]}"]}

    HUDPATH entropy BG26    3,3,26,0,0,0,██████████████████████████████████████████


	| row 39
    HUDPATH entropy mode1   19,5,39,100,149,237,mode
    HUDPATH entropy mode2   ${Parse[0, "19,40,39,255,255,255,${If[${h_mode.NotEqual[EMPTY]},${h_mode},]}"]}

	| buffing
    HUDPATH entropy b1      ${Parse[0, "19,133,39,0,255,0,${If[${h_bmaster},buff,]}"]}
    HUDPATH entropy b2      ${Parse[0, "19,133,39,255,0,0,${If[${h_bmaster},,buff]}"]}
    HUDPATH entropy bS1     ${Parse[0, "19,157,39,255,255,255,${If[${h_bself},S,]}"]}
    HUDPATH entropy bS2     ${Parse[0, "19,157,34,25,25,25,${If[${h_bself},,_]}"]}
    HUDPATH entropy bC1     ${Parse[0, "19,166,39,255,255,255,${If[${h_bcrew},C,]}"]}
    HUDPATH entropy bC2     ${Parse[0, "19,166,34,25,25,25,${If[${h_bcrew},,_]}"]}
    HUDPATH entropy bR1     ${Parse[0, "19,175,39,255,255,255,${If[${h_braid},R,]}"]}
    HUDPATH entropy bR2     ${Parse[0, "19,175,34,25,25,25,${If[${h_braid},,_]}"]}
    HUDPATH entropy bB1     ${Parse[0, "19,184,39,255,255,255,${If[${h_bbeg},B,]}"]}
    HUDPATH entropy bB2     ${Parse[0, "19,184,34,25,25,25,${If[${h_bbeg},,_]}"]}
    HUDPATH entropy bT1     ${Parse[0, "19,193,39,255,255,255,${If[${h_btemp},T,]}"]}
    HUDPATH entropy bT2     ${Parse[0, "19,193,34,25,25,25,${If[${h_btemp},,_]}"]}
    HUDPATH entropy bU1     ${Parse[0, "19,202,39,255,255,255,${If[${h_bunity},U,]}"]}
    HUDPATH entropy bU2     ${Parse[0, "19,202,34,25,25,25,${If[${h_bunity},,_]}"]}
    HUDPATH entropy bA1     ${Parse[0, "19,211,39,255,255,255,${If[${h_baura},A,]}"]}
    HUDPATH entropy bA2     ${Parse[0, "19,211,34,25,25,25,${If[${h_baura},,_]}"]}
    HUDPATH entropy bN1     ${Parse[0, "19,220,39,255,255,255,${If[${h_bnow},N,]}"]}
    HUDPATH entropy bN2     ${Parse[0, "19,220,34,25,25,25,${If[${h_bnow},,_]}"]}
    HUDPATH entropy bP1     ${Parse[0, "19,229,39,255,255,255,${If[${h_bpet},P,]}"]}
    HUDPATH entropy bP2     ${Parse[0, "19,229,34,25,25,25,${If[${h_bpet},,_]}"]}


    | burn
    HUDPATH entropy burn1   ${Parse[0, "19,255,39,0,255,0,${If[${h_buauto},burn,]}"]}
    HUDPATH entropy burn2   ${Parse[0, "19,255,39,255,0,0,${If[${h_buauto},,burn]}"]}
    HUDPATH entropy burn4   ${Parse[0, "19,280,39,255,255,255,${If[${h_bupct},${h_bupct},]}"]}
    HUDPATH entropy burn5   ${Parse[0, "19,280,34,25,25,25,${If[${h_bupct},,__]}"]}
    HUDPATH entropy burn6   ${Parse[0, "19,295,39,255,255,255,${If[${h_bucount},${h_bucount},]}"]}
    HUDPATH entropy burn7   ${Parse[0, "19,295,34,25,25,25,${If[${h_bucount},,__]}"]}
    HUDPATH entropy burn8   ${Parse[0, "19,310,39,255,255,255,${If[${h_buraid},R,]}"]}
    HUDPATH entropy burn9   ${Parse[0, "19,310,34,25,25,25,${If[${h_buraid},,_]}"]}

    HUDPATH entropy BG39    3,3,39,0,0,0,██████████████████████████████████████████




	| row 52

    | toons loc
    HUDPATH entropy yxz1    3,5,52,100,149,237,yxz
    HUDPATH entropy yxz2    ${Parse[0, "3,40,52,140,140,140,${Int[${Me.Y}]}, ${Int[${Me.X}]}, ${Int[${Me.Z}]}"]}


    | rest
    HUDPATH entropy rest1   ${Parse[0, "19,255,52,0,255,0,${If[${Me.CombatState.Equal[resting]},rest,]}"]}
    HUDPATH entropy rest2   ${Parse[0, "19,255,52,255,0,0,${If[${Me.CombatState.Equal[resting]},,rest]}"]}
    HUDPATH entropy rest3   ${Parse[0, "19,280,52,255,255,255,${If[${h_rpct},${h_rpct},]}"]}
    HUDPATH entropy rest4   ${Parse[0, "19,280,47,25,25,25,${If[${h_rpct},,__]}"]}
    HUDPATH entropy rest5   ${Parse[0, "19,295,52,255,255,255,${If[${h_rfull},F,]}"]}
    HUDPATH entropy rest6   ${Parse[0, "19,295,47,25,25,25,${If[${h_rfull},,_]}"]}
    HUDPATH entropy rest7   ${Parse[0, "19,304,52,255,255,255,${If[${h_rrod},R,]}"]}
    HUDPATH entropy rest8   ${Parse[0, "19,304,47,25,25,25,${If[${h_rrod},,_]}"]}
    HUDPATH entropy rest9   ${Parse[0, "19,313,52,255,255,255,${If[${h_rbuff},B,]}"]}
    HUDPATH entropy rest10  ${Parse[0, "19,313,47,25,25,25,${If[${h_rbuff},,_]}"]}
    HUDPATH entropy rest11  ${Parse[0, "19,322,52,255,255,255,${If[${h_rcombat},C,]}"]}
    HUDPATH entropy rest12  ${Parse[0, "19,322,47,25,25,25,${If[${h_rcombat},,_]}"]}


    HUDPATH entropy BG52    3,3,52,0,0,0,██████████████████████████████████████████

	| row 65

    HUDPATH entropy envset1    3,5,65,100,149,237,set
    HUDPATH entropy envset2    ${Parse[0, "19,40,65,140,140,140,${h_eset}"]}

    | rez
    HUDPATH entropy rez1    ${Parse[0, "19,255,65,0,255,0,${If[${h_ztake},rez,]}"]}
    HUDPATH entropy rez2    ${Parse[0, "19,255,65,255,0,0,${If[${h_ztake},,rez]}"]}
    HUDPATH entropy rez3    ${Parse[0, "19,275,65,255,255,255,${If[${h_zgive},G,]}"]}
    HUDPATH entropy rez4    ${Parse[0, "19,275,60,25,25,25,${If[${h_zgive},,_]}"]}
    HUDPATH entropy rez5    ${Parse[0, "19,284,65,255,255,255,${If[${h_zcombat},C,]}"]}
    HUDPATH entropy rez6    ${Parse[0, "19,284,60,25,25,25,${If[${h_zcombat},,_]}"]}
    HUDPATH entropy rez7    ${Parse[0, "19,293,65,255,255,255,${If[${h_zeveryone},E,]}"]}
    HUDPATH entropy rez8    ${Parse[0, "19,293,60,25,25,25,${If[${h_zeveryone},,_]}"]}
    HUDPATH entropy rez9    ${Parse[0, "19,302,65,255,255,255,${If[${h_ztoken},T,]}"]}
    HUDPATH entropy rez10   ${Parse[0, "19,302,60,25,25,25,${If[${h_ztoken},,_]}"]}

    HUDPATH entropy BG65    3,3,65,0,0,0,██████████████████████████████████████████

    | row 78
    HUDPATH entropy body1   ${Parse[0, "3,73,78,0,255,0,${If[${Target.LineOfSight},${Target.Body},]}"]}
    HUDPATH entropy body2   ${Parse[0, "3,73,78,255,0,0,${If[${Target.LineOfSight},,${Target.Body}]}"]}
    HUDPATH entropy body3   ${Parse[0, "3,145,78,140,140,140,${Target.Distance} (${Math.Calc[${Spawn[${Target.ID}].MaxRangeTo}*.9]})"]}
    HUDPATH entropy eng0    "3,5,77,100,149,237,[     ][     ][     ][                       ]"
    HUDPATH entropy eng1    ${Parse[0, "19,10,78,255,255,255,${If[${h_engage},${h_engage},]}"]}
    HUDPATH entropy eng2    ${Parse[0, "19,12,73,25,25,25,${If[(!${h_engage},__,]}"]}
    HUDPATH entropy eng5    ${Parse[0, "19,31,78,255,255,255,${If[${h_swpet},${h_engagepet},]}"]}
    HUDPATH entropy eng6    ${Parse[0, "19,31,73,25,25,25,${If[!${h_swpet},__,]}"]}
    HUDPATH entropy eng8    ${Parse[0, "19,52,78,255,255,255,${If[${h_swswarm},${h_engageswarm},]}"]}
    HUDPATH entropy eng9    ${Parse[0, "19,52,73,25,25,25,${If[!${h_swswarm},__,]}"]}
    HUDPATH entropy BG78    3,3,78,0,0,0,██████████████████████████████████████████



	| row 91
    HUDPATH entropy target		${Parse[0, "3,5,91,100,149,237,${If[${Target.ID},[${Target.Class.ShortName} ${Target.Level}] ${Target.Name} ${If[${Bool[${Target.Guild}]},<${Target.Guild}>, ]}, ]}"]}
    HUDPATH entropy time1			${Parse[0, "19,290,91,146,149,180,${If[${h_time},${Time.Time24},${GameTime}]}"]}
    HUDPATH entropy BG91    	3,3,91,0,0,0,██████████████████████████████████████████






  | cursor over hud
  } else /if (${_verbage.Equal[cursor]}) {

    HUDPATH cursor over				21,4,50,0,255,0,${If[${EverQuest.LastMouseOver},${EverQuest.LastMouseOver.Name},]}
    HUDPATH cursor overbg			21,2,50,0,0,0,${If[${EverQuest.LastMouseOver},█████████████████████████,]}



  | additional development hud
  } else /if (${_verbage.Equal[dev]}) {

    HUDPATH dev HL1    						"3,3,95,247,152,10,________________________________________________________"
    HUDPATH dev HL2    						"3,3,96,247,152,10,________________________________________________________"

	| row 110
  	HUDPATH dev devflow 					${Parse[0, "19,5,110,100,149,237,${flow}"]}
    HUDPATH dev BG110    					3,3,110,0,0,0,██████████████████████████████████████████


	| row 123
    HUDPATH dev devCombat1	 		  3,5,123,100,149,237,combat
    HUDPATH dev devCombat2	 		  ${Parse[0, "19,45,123,0,255,0,${If[${maCC.Find[swSetCombat].Value},T,]}"]}
    HUDPATH dev devCombat3	 		  ${Parse[0, "19,45,123,255,0,0,${If[${maCC.Find[swSetCombat].Value},,F]}"]}


    HUDPATH dev devBurn1	 		 		3,80,123,100,149,237,burn
    HUDPATH dev devBurn2	 			  ${Parse[0, "19,110,123,0,255,0,${If[${maBurn.Find[swBurnConditionMet].Value},T,]}"]}
    HUDPATH dev devBurn3	 		  	${Parse[0, "19,110,123,255,0,0,${If[${maBurn.Find[swBurnConditionMet].Value},,F]}"]}

    HUDPATH dev hurtID1						3,130,123,100,149,237,hID
    HUDPATH dev hurtID2		 			  ${Parse[0, "19,150,123,255,255,255,${hurtID}"]}


    HUDPATH dev BG123    					3,3,123,0,0,0,██████████████████████████████████████████

	| row 136
    HUDPATH dev devTargetID1 	  	3,5,136,100,149,237,targetID
    HUDPATH dev devTargetID2 		 	${Parse[0, "19,45,136,255,255,255,${Target.ID}"]}

    HUDPATH dev stEnvRadius1			3,80,136,100,149,237,env
    HUDPATH dev stEnvRadius2			${Parse[0, "19,110,136,255,255,255,${SpawnCount[NPC radius ${maEnv.Find[stEnvRadius].Value}]}"]}

    HUDPATH dev hurtPctHP1				3,130,136,100,149,237,h%
    HUDPATH dev hurtPctHP2		 		${Parse[0, "19,150,136,255,255,255,${hurtPctHP}"]}


    HUDPATH dev BG136    					3,3,136,0,0,0,██████████████████████████████████████████

	| row 149
    HUDPATH dev devASSTargetID1   3,5,149,100,149,237,AssID
    HUDPATH dev devASSTargetID2  	${Parse[0, "19,45,149,255,255,255,${stAssTarID}"]}

    HUDPATH dev stEnvSafeRadius1	,80,149,100,149,237,safe
    HUDPATH dev stEnvSafeRadius2	${Parse[0, "19,110,149,255,255,255,${SpawnCount[NPC radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}"]}

    HUDPATH dev hurtCLS1				3,130,149,100,149,237,hCL
    HUDPATH dev hurtCLS2		 		${Parse[0, "19,150,149,255,255,255,${hurtCLS}"]}


    HUDPATH dev BG149  			 		 	3,3,149,0,0,0,██████████████████████████████████████████


	| row 162
    HUDPATH dev devIsDead1  			3,5,162,100,149,237,IsDead
    HUDPATH dev devIsDead2	 		  ${Parse[0, "19,45,162,0,255,0,${If[${swIsMeDead},T,]}"]}
    HUDPATH dev devIsDead3	 		  ${Parse[0, "19,45,162,255,0,0,${If[${swIsMeDead},,F]}"]}

    HUDPATH dev stPullRadius1			3,80,162,100,149,237,pull
    HUDPATH dev stPullRadius2			${Parse[0, "19,110,162,255,255,255,${SpawnCount[NPC radius ${maCC.Find[stPullRadius].Value}]}"]}

    HUDPATH dev hurtCount1				3,130,162,100,149,237,h#
    HUDPATH dev hurtCount2		 		${Parse[0, "19,150,162,255,255,255,${hurtCount}"]}

    HUDPATH dev castStatus1				3,250,162,100,149,237,cR
    HUDPATH dev castStatus2		 		${Parse[0, "19,265,162,255,255,255,${maData.Find[stCastReturn].Value}"]}


    HUDPATH dev BG162    					3,3,162,0,0,0,██████████████████████████████████████████

    HUDPATH dev HL3    						3,3,166,247,152,10,________________________________________________________
    HUDPATH dev HL4    						3,3,167,247,152,10,________________________________________________________


	| row 181
		HUDPATH dev tTIE1							3,5,181,100,149,237,tie
		HUDPATH dev tTIE2							${Parse[0, "19,30,181,255,239,0,${timer_Check_Tie}"]}
		HUDPATH dev tb1								3,50,181,100,149,237,b
		HUDPATH dev tb2								${Parse[0, "19,70,181,255,239,0,${timer_Check_Buff_Cycle}"]}

		HUDPATH dev tbC1							3,105,181,100,149,237,bC
		HUDPATH dev tbC2							${Parse[0, "19,125,181,255,239,0,${timer_Check_Buff_Crew}"]}

		HUDPATH dev tbR1							3,160,181,100,149,237,bR
		| HUDPATH dev tbR2							19,185,181,255,239,0,${timer_Check_Buff_Raid}

		HUDPATH dev tMode1						3,215,181,100,149,237,tM
		HUDPATH dev tMode2						${Parse[0, "19,240,181,255,239,0,${timer_Check_Mode}"]}



    HUDPATH dev BG181    					3,3,181,0,0,0,██████████████████████████████████████████

	| row 194

		HUDPATH dev tCure1						3,5,194,100,149,237,cure
		HUDPATH dev tCure2						${Parse[0, "19,30,194,255,239,0,${timer_Check_Cure}"]}

		HUDPATH dev tbN1							3,50,194,100,149,237,bN
		HUDPATH dev tbN2							${Parse[0, "19,70,194,255,239,0,${timer_Check_Buff_Now}"]}
		HUDPATH dev tbA1							3,105,194,100,149,237,bA
		HUDPATH dev tbA2							${Parse[0, "19,125,194,255,239,0,${timer_Check_Buff_Aura}"]}

    HUDPATH dev BG194    					3,3,194,0,0,0,██████████████████████████████████████████

	| row207

		HUDPATH dev tbS1							3,50,207,100,149,237,bS
		HUDPATH dev tbS2							${Parse[0, "19,70,207,255,239,0,${timer_Check_Buff_Self}"]}
		HUDPATH dev tbB1							3,105,207,100,149,237,bB
		HUDPATH dev tbB2							${Parse[0, "19,125,207,255,239,0,${timer_Check_Buff_Beg}"]}

    HUDPATH dev BG207    					3,3,207,0,0,0,██████████████████████████████████████████

	| row 220

		HUDPATH dev tbU1							3,50,220,100,149,237,bU
		HUDPATH dev tbU2							${Parse[0, "19,70,220,255,239,0,${timer_Check_Buff_Unity}"]}
		HUDPATH dev tbT1							3,105,220,100,149,237,bT
		HUDPATH dev tbT2							${Parse[0, "19,125,220,255,239,0,${timer_Check_Buff_Temp}"]}

    HUDPATH dev BG220    					3,3,220,0,0,0,██████████████████████████████████████████

  }

/return



|***
 * note: find and remove stale entries from the character database
 * use:
 ***|
sub sql_check(bool _debug)
	DEBUG \atsql_check\ax()

	/declare _field string local FALSE
	/declare _table string local FALSE

	/declare _count int local 0
	/sqlite ${maData.Find[DBstatic].Value} dummycount SELECT Count(*) as data FROM stale_data

	/for _count 1 to ${sqlite.Result[dummycount 1 data]}

		/sqlite ${maData.Find[DBstatic].Value} dummy SELECT * FROM stale_data WHERE ROWID=${_count}
		/varset _table ${sqlite.Result[dummy 1 value]}
		/varset _field ${sqlite.Result[dummy 1 env_var]}
		DEBUG ${sep}${_field}${sep}\a-w${_table}\ax

		| table to remove
		/if (${_table.Equal[tabletodrop]}) {
			/sqlite ${maData.Find[DBcharacter].Value} dummy DROP TABLE IF EXISTS ${_field}

		| field to remove
		} else {
			| is the data there?
			/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_${_table} WHERE env_var="${_field}"
			DEBUG ${sep}dummy SELECT * FROM environment_${_table} WHERE env_var="${_field}" ${dot} count:\ap${sqlite.Rows[dummy]}\ax \ay${sqlite.Status[dummy]}\ax

			/if (${sqlite.Rows[dummy]}) {
				/sqlite ${maData.Find[DBcharacter].Value} dummy DELETE FROM environment_${_table} WHERE env_var="${sqlite.Result[dummy 1 env_var]}"
			}
		}

	/next _count

	| rename environment_buffs to singular
	/sqlite ${maData.Find[DBcharacter].Value} dummy SELECT * FROM environment_buffs WHERE env_var="stBuff1"
	/if (${sqlite.Rows[dummy]}) {
		/sqlite ${maData.Find[DBcharacter].Value} dummy ALTER TABLE "environment_buffs" RENAME TO "environment_buff"
	}

/return TRUE
















