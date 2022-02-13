|*********************************************************************
 * entropy
 * initialize.inc
 * all shared/static data initilization
 *
 ***|



|***
 * note: builds all the DEBUG variables and lists for the map
 * use:
 ***|
sub set_debug(string _verbage)

  | generate DEBUGS
  /declare lsDebuglist list outer
  /invoke ${lsDebuglist.Append[buff,heal,home,debuff,cast,loot,mode,combat,other,minion,class,admin,schema,zone]}

  | build the variables for the DEBUG lists
  /declare _licdeb listiterator local
  /vardata _licdeb lsDebuglist.First.Clone
  /while (!${_licdeb.IsEnd}) {
    /declare lsDebug${_licdeb.Value} list outer
    /invoke ${_licdeb.Advance}
  }

  | buff
  /invoke ${lsDebugbuff.Append[cb,ammo,beg,unity,aura,crew,buffself,shrink,poison,class,now,temp,bocked,buffgroup,buffraid,drop]}
  | heal
  /invoke ${lsDebugheal.Append[cure,xt,group,pet,self,hurt,tot,dannet,rez,chain,ch,twin]}
  | home
  /invoke ${lsDebughome.Append[face,tie,stick,stop,home,gtfo]}
  | debuff
  /invoke ${lsDebugdebuff.Append[tash,malo,mez,slow,eradicate,snare]}
  | minion
  /invoke ${lsDebugminion.Append[petbuild,lost,sic,temp,heal]}
  | cast
  /invoke ${lsDebugcast.Append[item,nuke,dot]}
  | loot
  /invoke ${lsDebugloot.Append[loot]}
  | mode
  /invoke ${lsDebugmode.Append[forage,drag,summon,petfarm,harvest,hunt,fish,farm,lush,trainskill,trainspell,temppet,scribe]}
  | combat
  /invoke ${lsDebugcombat.Append[def,target,melee,burn,agro,pull,misc,assist,decision,proc,ttl,sash,status,wait,hardstop,prediction]}
  | other
  /invoke ${lsDebugother.Append[rest,meal,spire,glyph,fireworks,manarecover,fade,event,watch,tag,safe,intensity,chrwatch,staminarecover,inventoryscan,invis,gather]}
  | admin
  /invoke ${lsDebugadmin.Append[build,observe,house,exit,sql,edit,e3]}
  | schema (database creation/modification)
  /invoke ${lsDebugschema.Append[init,s_zone,s_character,s_loot,s_nav,s_shared,s_stale]}
  | area
  /invoke ${lsDebugzone.Append[area]}


  | class specific debugs
  /if (${Select[${Me.Class.ShortName},BER]}) {
  } else /if (${Select[${Me.Class.ShortName},BRD]}) {
    /invoke ${lsDebugclass.Append[pulse,scan]}
  } else /if (${Select[${Me.Class.ShortName},BST]}) {
    /invoke ${lsDebugclass.Append[para]}
  } else /if (${Select[${Me.Class.ShortName},CLR]}) {
    /invoke ${lsDebugclass.Append[arb,splash,regen,yaulp,checkmana,chain]}
  } else /if (${Select[${Me.Class.ShortName},DRU]}) {
  } else /if (${Select[${Me.Class.ShortName},ENC]}) {
    /invoke ${lsDebugclass.Append[crystal,gather,charm]}
  } else /if (${Select[${Me.Class.ShortName},MAG]}) {
    /invoke ${lsDebugclass.Append[coh,gather,cauldron]}
  } else /if (${Select[${Me.Class.ShortName},MNK]}) {
    /invoke ${lsDebugclass.Append[fury]}
  } else /if (${Select[${Me.Class.ShortName},NEC]}) {
  } else /if (${Select[${Me.Class.ShortName},PAL]}) {
    /invoke ${lsDebugclass.Append[splash,harmonious,stun]}
  } else /if (${Select[${Me.Class.ShortName},RNG]}) {
    /invoke ${lsDebugclass.Append[summer]}
  } else /if (${Select[${Me.Class.ShortName},ROG]}) {
  } else /if (${Select[${Me.Class.ShortName},SHD]}) {
    /invoke ${lsDebugclass.Append[harmonious]}
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
  /call set_data ${_debug} FALSE bindswap bool outer FALSE
  /call set_data ${_debug} FALSE DNReturn string outer FALSE
  /call set_data ${_debug} FALSE swDBlocked bool outer FALSE
  /call set_data ${_debug} FALSE DNQReturn string outer FALSE
  /call set_data ${_debug} FALSE DNQReturn2 string outer FALSE
  /call set_data ${_debug} FALSE DNQHardStop string outer FALSE

  | list iterator dump variable
  | /declare li listiterator outer

  | control maps
  /declare maControl map outer
  /declare maChrControl map outer
  /declare maMobControl map outer
  
  | build the debuf control maps
  /declare lsdebuffType list outer 
  /invoke ${lsdebuffType.Append[Mez,Malo,Tash,Share,Charm]}
  /declare _lsdbt listiterator local
  /vardata _lsdbt lsdebuffType.First.Clone
  /while (!${_lsdbt.IsEnd}) {
    /declare maMobControl${_lsdbt.Value} map outer
    /invoke ${_lsdbt.Advance}
  }

  | dumpster map
  /declare maData map outer
  /invoke ${maData.Add[stFacingPositionCheck,FALSE]}
  /invoke ${maData.Add[stPctEngageSet,FALSE]}
  /invoke ${maData.Add[stPctEngagePetSet,FALSE]}
  /invoke ${maData.Add[stPctEngageSwarmSet,FALSE]}
  /invoke ${maData.Add[stPctEngageBurnSet,FALSE]}


  | hook map
  /declare maHook map outer
  
  | buff beg map
  /declare maBuffRequest map outer

  | watchspawn map
  /declare maWatchspawn map outer

  | rez control map
  /declare maControlRez map outer

  | PREGEN recorder
  /declare maPREGEN map outer
  
  | cast data map
  /declare maCD map outer
  
  | spawn data map
  /declare maSD map outer
  
  | assist data
  /declare _assist spawn outer
  /declare assistData spawn outer
  
  | assists target spawn data
  /declare targetData spawn outer
  /vardata targetData Me
  
  | spell/cast data
  /declare spellData spell outer
  
  | current worst hurt data
  /declare hurtData spawn outer
  /vardata hurtData Me

  | TTL stuff
  /declare maTTL map outer
  /call set_data ${_debug} maTTL stTTLTimeStart                           map outer 0

  | predictive combat
  /declare maPred map outer
  
  | hud output reference
  /declare HUDOutput string outer

  | list of all maps used with usable data
  /declare lsMaps list outer
  /invoke ${lsMaps.Append[maChr,maEnv,maHome,maCC,maHeal,maRez,maBuff,maBurn,maPred]}
  /invoke ${lsMaps.Append[maEntropy,maTimer,madeBuff,maTie,maMinion,maRest]}
  /invoke ${lsMaps.Append[maData,maDebug,maOver,maPREGEN,maMode,maHard]}
  /invoke ${lsMaps.Append[maBuffRequest,maHook,maManaRecovery,maTTL,maWatch,maAgro,maMercenary]}
  /invoke ${lsMaps.Append[maNuke,maDoT,maItem,maDefense,maMiscDPS,maSong,maSash,maControlRez]}
  /invoke ${lsMaps.Append[maGTFO,maPull,maMelee,maCure]}
  /invoke ${lsMaps.Append[maMobControlMez]}
  /invoke ${lsMaps.Append[maHud]}

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
  /call set_data ${_debug} FALSE eol                                      string outer "\a#4183D7................\ax"
  /call set_data ${_debug} FALSE break                                    string outer "${sep}\a#FF06BBbreak\ax${sep}"
  /call set_data ${_debug} FALSE skip                                     string outer "${sep}\a#FF06BBskip\ax${sep}"
  /call set_data ${_debug} FALSE out_list                                 string outer "\a#1E90FF[\ax\awlist\ax\a#1E90FF]\ax"

  | lua
  /call set_data ${_debug} FALSE lsep                                    string outer "\a#00FFFF :: \a#C0C0C0"
  


  /call set_data ${_debug} FALSE enabled                                  string outer "\agenabled\ax"
  /call set_data ${_debug} FALSE disabled                                 string outer "\ardisabled\ax"

  | all character db tables
  /declare maDBTables map outer
  /invoke ${maDBTables.Add[environment_buff,env_var]}
  /invoke ${maDBTables.Add[environment_character,env_var]}
  /invoke ${maDBTables.Add[environment_timer,env_timer]}
  /invoke ${maDBTables.Add[environment_item,env_var]}
  /invoke ${maDBTables.Add[environment_defense,env_var]}
  /invoke ${maDBTables.Add[environment_heal,env_var]}
  /invoke ${maDBTables.Add[environment_song,env_var]}
  /invoke ${maDBTables.Add[environment_nuke,env_var]}
  /invoke ${maDBTables.Add[environment_dot,env_var]}
  /invoke ${maDBTables.Add[environment_agro,env_var]}
  /invoke ${maDBTables.Add[environment_miscdps,env_var]}
  /invoke ${maDBTables.Add[environment_burn,env_var]}
  /invoke ${maDBTables.Add[environment_sash,env_var]}
  /invoke ${maDBTables.Add[environment_hard,env_var]}
  /invoke ${maDBTables.Add[environment_mercenary,env_var]}
  /invoke ${maDBTables.Add[environment_minion,env_var]}
  /invoke ${maDBTables.Add[environment_rez,env_var]}
  /invoke ${maDBTables.Add[environment_gtfo,env_var]}
  /invoke ${maDBTables.Add[environment_pull,env_var]}
  /invoke ${maDBTables.Add[environment_melee,env_var]}
  /invoke ${maDBTables.Add[environment_debuff,env_var]}
  /invoke ${maDBTables.Add[environment_cure,env_var]}
  /invoke ${maDBTables.Add[environment_rest,env_var]}
  /invoke ${maDBTables.Add[environment_hud,env_var]}
 
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
  /call set_data ${_debug} FALSE lsClassMercenary                         list outer WAR|CLR|WIZ|ROG
  /call set_data ${_debug} FALSE lsClassMelee                             list outer WAR|ROG|BST|MNK|SHD|PAL|BER|BRD|RNG
  /call set_data ${_debug} FALSE lsClassCast                              list outer PAL|SHD|BRD|RNG|BST|WIZ|MAG|ENC|NEC|CLR|DRU|SHM
  /call set_data ${_debug} FALSE lsClassStamina                           list outer WAR|PAL|SHD|BRD|RNG|BST|BER|MNK|ROG
  /call set_data ${_debug} FALSE lsClassPet                               list outer ENC|DRU|SHM|SHD|WIZ|BRD|BST|MAG|NEC
  /call set_data ${_debug} FALSE lsClassHeal                              list outer CLR|DRU|SHM|PAL|RNG|BST
  /call set_data ${_debug} FALSE lsClassRez                               list outer CLR|DRU|SHM|PAL|NEC
  /call set_data ${_debug} FALSE lsClassAASnare                           list outer RNG|DRU|SHD|NEC|ROG|WAR|WIZ|MNK|PAL|BER
  /call set_data ${_debug} FALSE lsClassAAEradicate                       list outer RNG|DRU|NEC|WIZ|ENC|MAG
  /call set_data ${_debug} FALSE lsClassCastCure                          list outer CLR|DRU|SHM|BST|PAL|RNG
  /call set_data ${_debug} FALSE lsClassHealToT                           list outer BST|PAL|RNG
  /call set_data ${_debug} FALSE lsClassHealSelf                          list outer WAR|PAL|SHD|BRD|RNG|BST|BER|MNK|ROG|WIZ|MAG|ENC|NEC|CLR|DRU|SHM
  /call set_data ${_debug} FALSE lsClassHealGroup                         list outer CLR|SHM|PAL|DRU
  /call set_data ${_debug} FALSE lsClassHealXT                            list outer CLR|SHM|PAL|DRU
  /call set_data ${_debug} FALSE lsClassHealPet                           list outer CLR|SHM|PAL|DRU|NEC|MAG|BST|SHM|RNG
  /call set_data ${_debug} FALSE lsClassHealCure                          list outer WAR|PAL|SHD|BRD|RNG|BST|BER|MNK|ROG|WIZ|MAG|ENC|NEC|CLR|DRU|SHM

  /call set_data ${_debug} FALSE lsIVU                                    list outer "Group Perfected Invisibility to Undead"|"Perfected Invisibility to Undead"|"Invisibility to Undead"|"Invisibility versus Undead"|"Shauri's Sonorous Clouding"
  /call set_data ${_debug} FALSE lsIV                                     list outer "Shared Perfected Camouflage"|"Shared Camouflage"|"Camouflage"|"Group Perfected Invisibility"|"Invisibility"|"Perfected Invisibility"|"Phase March"|"Nethervoid March"|"Perfected Silent Presence"|"Cloak of Shadows"|"Gather Umbra"|"Invisibility Cloak"|"Perfected Natural Invisibility"|"Perfected Spirit Walk Invisibility"|"Perfected Group Spirit Walk Invisibility"|"Shauri's Sonorous Clouding"|"Selo's Song of Travel"
  /call set_data ${_debug} FALSE lsPullTypes                              list outer base|pet|int|nav|multi|watch
  | /call set_data ${_debug} FALSE lsRezBuffTriggers                        list outer "Revival Sickness"|"Resurrection Sickness"
  /call set_data ${_debug} FALSE lsBroadcastTypes                         list outer group|raid|zone|all

  | multi proc spells
  /call set_data ${_debug} FALSE lsHasSPA                                 list outer 470|374|340

  | things we dont want pullers attacking
  /call set_data ${_debug} FALSE lsNoAttackBodyType                       list outer aura|banner|campfire|corpse|chest|item|trigger|trap|timer|mount|mercenary|object
  /call set_data ${_debug} FALSE lsNoTargetNPC                            list outer "a campfire"|"a corpse"|"controller"|"Steel Mass"|"Sphere of Fire"|"an ant"|"a barrel of food"|"a crate of food"|"Agent of Change"
  /call set_data ${_debug} FALSE lsNoTargetPC                             list outer "eye of|a barrel of food"
  /call set_data ${_debug} FALSE lsNoTargetMagic                          list outer "eye of"
  /call set_data ${_debug} FALSE lsAnimationNoTarget                      list outer 16|43|44
  | /call set_data ${_debug} FALSE lsAnimationNoTarget                      list outer 16|43|44|146
  /call set_data ${_debug} FALSE lsAnimationMezed                         list outer 13|49|58

  | cardinal directions
  /call set_data ${_debug} FALSE lsCardinal                               list outer n|nne|ne|ene|e|ese|se|sse|s|ssw|sw|wsw|w|wnw|nw|nnw


  | playerstates
  /call set_data ${_debug} FALSE lsPlayerstate                            list outer 1|2|4|8|16|32|64|128|0


  | custom /build set variables
  /call set_data ${_debug} FALSE lsBuildSet                               list outer primary|secondary|range|spellset

  | spell types we let through the checking/validation methods
  /call set_data ${_debug} FALSE lsFreePassSpells                         list outer Beneficial|Beneficial(Group)|Pet|Aura

  | control lines
  /call set_data ${_debug} FALSE lsControlLine                            list outer nuke|song|dot|heal|item|miscdps|agro|defense|burn|rez|gtfo|pull|melee|debuff|cure|sash

  | increment lists
  /call set_data ${_debug} FALSE stCount6                                 string outer "0|1|2|3|4|5"
  /call set_data ${_debug} FALSE stCount10                                string outer "1|2|3|4|5|6|7|8|9|10"
  /call set_data ${_debug} FALSE stCount12                                string outer "1|2|3|4|5|6|7|8|9|10|11|12"
  /call set_data ${_debug} FALSE stCount20                                string outer "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20"
  /call set_data ${_debug} FALSE stCount50                                string outer "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|40|41|42|43|44|45|46|47|48|49|50"

  /call set_data ${_debug} FALSE lsCount6                                 list outer "${stCount6}"
  /call set_data ${_debug} FALSE lsCount10                                list outer "${stCount10}"
  /call set_data ${_debug} FALSE lsCount12                                list outer "${stCount12}"
  /call set_data ${_debug} FALSE lsCount20                                list outer "${stCount20}"
  /call set_data ${_debug} FALSE lsCount50                                list outer "${stCount50}"

  | cleric group regens
  /call set_data ${_debug} FALSE lsGroupRegens                            list outer "Elixir of Transcendence|Elixir of Wulthan|Elixir of the Seas|Elixir of Expiation|Elixir of Atonement|Elixir of Redemption|Elixir of Divinity|Ethereal Elixir"
  | shaman group regens
  /call set_data ${_debug} FALSE lsGroupRegens                            list outer "Spectre of Renewal|Talisman of the Enduring|Cloud of Renewal|Shear of Renewal|Wisp of Renewal|Phantom of Renewal|Penumbra of Renewal|Shadow of Renewal|Shade of Renewal|Specter of Renewal|Ghost of Renewal"

  | timers  for everyone
  /invoke ${set_data_timer[${_debug}, Try_Door, 4s]}
  /invoke ${set_data_timer[${_debug}, Check_Ammo, 5s]}
  /invoke ${set_data_timer[${_debug}, Nuke_Delay, 1]}
  /invoke ${set_data_timer[${_debug}, Check_Cursor, 1s]}
  /invoke ${set_data_timer[${_debug}, Check_Zone_ID, 10s]}
  /invoke ${set_data_timer[${_debug}, Check_Observers, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Poison, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_deBuffs, 6s]}
  /invoke ${set_data_timer[${_debug}, Check_Behind, 15s]}
  /invoke ${set_data_timer[${_debug}, Check_Tribute, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Loot, 1s]}
  /invoke ${set_data_timer[${_debug}, Check_Mode, 5]}
  /invoke ${set_data_timer[${_debug}, Check_LoS_Timeout, 10s]}
  /invoke ${set_data_timer[${_debug}, Check_Housekeeping, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Home, 1]}
  /invoke ${set_data_timer[${_debug}, Check_Target, 5s]}
  /invoke ${set_data_timer[${_debug}, Check_Alliance, 6s]}
  /invoke ${set_data_timer[${_debug}, Check_Mercenary, 1s]}
  /invoke ${set_data_timer[${_debug}, Check_Pull_Hardstop, 1]}
  /invoke ${set_data_timer[${_debug}, Check_Pull_Hardstop_Delay, 1]}
  /invoke ${set_data_timer[${_debug}, Check_Inventory_ManaClick, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Build_Spawn_Map_Data, 5s]}

  | wait time for pulling
  /invoke ${set_timer[${_debug}, wait_for_mob, 0]}

  | get the stick timer started
  /invoke ${set_timer[${_debug}, timer_check_stick, 1s]}

  | controls for all classes
  /invoke ${maChrControl.Add[ae,stCountAE]}
  /invoke ${maChrControl.Add[fade,stPctFade]}
  /invoke ${maChrControl.Add[spire,submenu]}
  /invoke ${maChrControl.Add[intensity,submenu]}

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
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[resolve,swResolve]}

  } else /if (${Select[${Me.Class.ShortName},BRD]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[rest,swSongRest]}
    /invoke ${maChrControl.Add[refresh,stSongRefresh]}
    /invoke ${maChrControl.Add[bellow,swAABellow]}
    /invoke ${maChrControl.Add[ooc,swPulseOOC]}
    /invoke ${maChrControl.Add[swap,submenu]}
    /invoke ${maChrControl.Add[scan,option]}
    /invoke ${maChrControl.Add[vainglorious,swVainglorious]}
    /invoke ${maChrControl.Add[remez,streMezTime|br]}

  } else /if (${Select[${Me.Class.ShortName},BST]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[consume,stPctConsume]}
    /invoke ${maChrControl.Add[para,submenu]}
    /invoke ${maChrControl.Add[mpara,mass]}

  } else /if (${Select[${Me.Class.ShortName},CLR]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[arb,submenu]}
    /invoke ${maChrControl.Add[beacon,stCountBeaconofLife]}
    /invoke ${maChrControl.Add[issuance,stCountIssuance]}
    /invoke ${maChrControl.Add[primary,submenu]}
    /invoke ${maChrControl.Add[vp,stPctVP]}
    /invoke ${maChrControl.Add[yaulp,swYaulp]}
    /invoke ${maChrControl.Add[qp,submenu]}
    /invoke ${maChrControl.Add[mcr,mass]}

  } else /if (${Select[${Me.Class.ShortName},DRU]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[wolf,swWolfSpirit]}
    /invoke ${maChrControl.Add[mwood,mass]}

  } else /if (${Select[${Me.Class.ShortName},ENC]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[gather,stPctGather]}
    /invoke ${maChrControl.Add[bos,swAABoS]}
    /invoke ${maChrControl.Add[crystal,swAACrystals]}
    /invoke ${maChrControl.Add[remez,streMezTime|br]}

  } else /if (${Select[${Me.Class.ShortName},MAG]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[gather,stPctGather]}
    /invoke ${maChrControl.Add[cauldron,submenu]}
    /invoke ${maChrControl.Add[pettype,stMinionElementType|br]}

  } else /if (${Select[${Me.Class.ShortName},MNK]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[train,swTrainMelee]}
    /invoke ${maChrControl.Add[fdhp,stPctFDHP]}
    /invoke ${maChrControl.Add[fdaggro,stPctFDAggro]}
    /invoke ${maChrControl.Add[fist,submenu]}

  } else /if (${Select[${Me.Class.ShortName},NEC]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[blood,submenu]}
    /invoke ${maChrControl.Add[gather,stPctGather]}

  } else /if (${Select[${Me.Class.ShortName},PAL]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[yaulp,swYaulp]}
    /invoke ${maChrControl.Add[attraction,swAttraction]}
    /invoke ${maChrControl.Add[harmonious,swHarmonious]}
    /invoke ${maChrControl.Add[loh,stPctLoH]}
    /invoke ${maChrControl.Add[mg,stPctMG]}
    /invoke ${maChrControl.Add[stun,submenu]}

  } else /if (${Select[${Me.Class.ShortName},RNG]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[summer,stCountSummer]}
    /invoke ${maChrControl.Add[mspice,mass]}

  } else /if (${Select[${Me.Class.ShortName},ROG]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[blind,swBlinding]}
    /invoke ${maChrControl.Add[poison,stLegPoison]}
    /invoke ${maChrControl.Add[nosneaktie,swNoSneakinTie]}

  } else /if (${Select[${Me.Class.ShortName},SHD]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[attraction,swAttraction]}
    /invoke ${maChrControl.Add[harmonious,swHarmonious]}
    /invoke ${maChrControl.Add[vot,swVoT]}

  } else /if (${Select[${Me.Class.ShortName},SHM]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[talisman,swTalisman]}
    /invoke ${maChrControl.Add[canni,submenu]}
    /invoke ${maChrControl.Add[maid,mass]}

  } else /if (${Select[${Me.Class.ShortName},WAR]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[attraction,swAttraction]}

  } else /if (${Select[${Me.Class.ShortName},WIZ]}) {
    /invoke ${maChrControl.Add[list,list]}
    /invoke ${maChrControl.Add[mb,swManaBurn]}
    /invoke ${maChrControl.Add[gather,stPctGather]}
    /invoke ${maChrControl.Add[concussion,stPctConcussion]}

  }

  | all classes
  /if (${lsClassAll.Contains[${Me.Class.ShortName}]}) {
    /invoke ${maChrControl.Add[invis,submenu]}
  }

  | set current zone
  /call set_data ${_debug} maEnv stCurrentZoneID                            map outer FALSE

  | generate modes
  /declare maMode map outer
  /declare lsMode list outer
  /invoke ${lsMode.Append[forage,drag,summon,petfarm,harvest,hunt,fish,farm,lush,trainskill,trainspell,temppet,scribe]}

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
sub sql_schema_zone(bool _debug)
  DEBUG \atsql_schema_zone\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  | zone fields
  /declare lsZoneProperty list outer
  /invoke ${lsZoneProperty.Delimiter["|"]}
  /invoke ${lsZoneProperty.Append[lsZoneNamed|lsZoneExclude|lsZoneCharm|lsZoneRanged|lsZoneNoCast|lsZoneNoMelee]}
  /invoke ${lsZoneProperty.Append[lsZoneNoFire|lsZoneNoPoison|lsZoneNoMagic|lsZoneNoCorruption|lsZoneNoCold|lsZoneNoDisease]}
  /invoke ${lsZoneProperty.Append[lsZoneNoSnare|lsZoneNoTash|lsZoneNoMalo|lsZoneNoEradicate|lsZoneNoCripple|lsZoneNoMez|lsZoneNoSlow]}

  /declare _lisszs listiterator local
  /vardata _lisszs lsZoneProperty.First
  /while (!${_lisszs.IsEnd}) {
    /declare ${_lisszs.Value} list outer
    /invoke ${${_lisszs.Value}.Delimiter["|"]}
    /invoke ${_lisszs.Advance}
  }

  /declare _schemaMismatch bool local FALSE
  /declare _buildMismatch bool local FALSE

  | get the data from the table
  /sqlite query "${DBzone}" buildnumber SELECT version FROM metadata WHERE name='build_number';
  /if (!${sql_check[${_debug}, buildnumber, sql_schema_zone,  "SELECT (version)"]}) /endmacro
  /sqlite query "${DBzone}" schemanumber SELECT version FROM metadata WHERE name='schema_number';
  /if (!${sql_check[${_debug}, schemanumber, sql_schema_zone, "SELECT (version)"]}) /endmacro

  | macro mismatch flag
  /if (${sqlite.Result[buildnumber 1 version].NotEqual[${maEnv.Find[build].Value}]}) {
    /varset _buildMismatch TRUE
  }

  | schema mismatch flag
  /if (${sqlite.Result[schemanumber 1 version].NotEqual[${maEnv.Find[schema].Value}]}) {
    /varset _schemaMismatch TRUE
  }

  | if the numbers match, we consider it valid and go away to dosoemthing else.
  /if (!${_schemaMismatch} && !${_buildMismatch}) {
    DEBUG ${sep}Macro and Schema builds match for zone database, bailing on this routine
    /sqlite clear schemanumber
    /sqlite clear buildnumber
    /return TRUE
  }

  | if there is no schema number? no database??
  /if (!${sqlite.Rows[schemanumber]}) {
    OUT \a-tSchema Build\ax${sep}\a-wzone\ax (\a-r${maEnv.Find[schema].Value}\ax)
    /while (TRUE) {
      | create shared build table
      /sqlite query "${DBzone}" build CREATE TABLE IF NOT EXISTS metadata(name, version);
      /if (!${sql_check[${_debug}, build, sql_schema_zone, "CREATE TABLE IF NOT EXISTS (metadata)"]}) /endmacro
      /sqlite query "${DBzone}" build INSERT OR IGNORE INTO metadata VALUES('build_number', '${maEnv.Find[build].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_zone, "INSERT INTO (metadata)"]}) /endmacro
      /sqlite query "${DBzone}" build INSERT OR IGNORE INTO metadata VALUES('schema_number', '${maEnv.Find[schema].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_zone, "INSERT INTO (metadata)"]}) /endmacro

      /sqlite query "${DBzone}" build CREATE TABLE IF NOT EXISTS zone_control('zone_shortname' TEXT NOT NULL,'element' TEXT NOT NULL,'value' TEXT NOT NULL, PRIMARY KEY (zone_shortname,element,value))
      /if (!${sql_check[${_debug}, build, sql_schema_zone, "CREATE TABLE"]}) /endmacro

      | if not locked, were good
      /if (${sqlite.Resultcode[build]} == 0) /break
    }
    /sqlite clear schemanumber
    /sqlite clear build
    /return TRUE
  }

  | correct build mismatch
  /if (${_buildMismatch}) {
    /while (TRUE) {
      /sqlite query "${DBzone}" updatebuildnum UPDATE metadata SET version='${maEnv.Find[build].Value}' WHERE name='build_number';
      /if (!${sql_check[${_debug}, updatebuildnum, sql_schema_zone, "UPDATE (metadata)"]})  /endmacro

      | db locked?
      /if (${sqlite.Resultcode[updatebuildnum]} == 0) /break
    }
    /sqlite clear updatebuildnum
  }

  | corect schema mismatch
  /if (${_schemaMismatch}) {
    /if (${sqlite.Status[schemanumber].NotEqual[success]}) {
      OUT \a-tSchema Update\ax${sep}\a-wzone\ax (\a-r${sqlite.Result[schemanumber 1 version]}\ax -> \a-r${maEnv.Find[schema].Value}\ax)
    } else {
      OUT \a-tSchema Update\ax${sep}\a-wzone\ax (\a-r${maEnv.Find[schema].Value}\ax)
    }
    /while (TRUE) {
      /sqlite query "${DBzone}" updateschemanum UPDATE metadata SET version='${maEnv.Find[schema].Value}' WHERE name='schema_number';
      /if (!${sql_check[${_debug}, updateschemanum, sql_schema_zone, "UPDATE (metadata)"]}) /endmacro

      /call sql_schema_update ${_debug} TRUE

      /if (${sqlite.Resultcode[updateschemanum]} == 0) /break
    }
    /sqlite clear updateschemanum

  }

  | set pragma
  /sqlite query "${DBzone}" query_journal_DBzone PRAGMA journal_mode=WAL;

  /if (${maDebug.Find[init].Value}) {
    OUT \agsql_schema_zone\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE




|***
 * note:
 * use: generate loot.sqlite
 ***|
sub sql_schema_loot(bool _debug)
  DEBUG \atsql_schema_loot\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  | check the current schema/build versions
  /sqlite query "${DBloot}" buildnumber SELECT version FROM metadata WHERE name='build_number';
  /if (!${sql_check[${_debug}, buildnumber, sql_schema_loot, "SELECT (version)"]}) /endmacro
  /sqlite query "${DBloot}" schemanumber SELECT version FROM metadata WHERE name='schema_number';
  /if (!${sql_check[${_debug}, schemanumber, sql_schema_loot, "SELECT (version)"]}) /endmacro

  /declare _schemaMismatch bool local FALSE
  /declare _buildMismatch bool local FALSE

  | macro mismatch flag
  /if (${sqlite.Result[buildnumber 1 version].NotEqual[${maEnv.Find[build].Value}]}) {
    /varset _buildMismatch TRUE
  }

  | schema mismatch flag
  /if (${sqlite.Result[schemanumber 1 version].NotEqual[${maEnv.Find[schema].Value}]}) {
    /varset _schemaMismatch TRUE
  }

  | if the numbers match, we consider it valid and go away to dosoemthing else.
  /if (!${_schemaMismatch} && !${_buildMismatch}) {
    DEBUG ${sep}Macro and Schema builds match for loot database, bailing on this routine
    /sqlite clear schemanumber
    /sqlite clear buildnumber
    /return TRUE
  }


  /if (!${sqlite.Rows[schemanumber]}) {
    DEBUG ${sep}FOUND${sep}\ar${sqlite.Rows[schemanumber]}\ax
    OUT \a-tSchema Build\ax${sep}\a-wloot\ax (\a-r${maEnv.Find[schema].Value}\ax)

    /while (TRUE) {
      | create shared build table
      /sqlite query "${DBloot}" build CREATE TABLE IF NOT EXISTS metadata(name, version);
      /if (!${sql_check[${_debug}, build, sql_schema_loot, "CREATE TABLE IF NOT EXISTS (metadata)"]}) /endmacro
      /sqlite query "${DBloot}" build INSERT OR IGNORE INTO metadata VALUES('build_number', '${maEnv.Find[build].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_loot, "INSERT INTO (metadata)"]}) /endmacro
      /sqlite query "${DBloot}" build INSERT OR IGNORE INTO metadata VALUES('schema_number', '${maEnv.Find[schema].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_loot, "INSERT INTO (metadata)"]}) /endmacro
      /sqlite query "${DBloot}" build CREATE TABLE IF NOT EXISTS loot_control(item TEXT NOT NULL PRIMARY KEY, action TEXT, pass TEXT, count TEXT);
      /if (!${sql_check[${_debug}, build, sql_schema_loot, "CREATE TABLE IF (loot_control)"]}) /endmacro

      /if (${sqlite.Resultcode[build]} != 0) /delay 1
      /if (${sqlite.Resultcode[build]} == 0) /break
    }
    /sqlite clear schemanumber
    /sqlite clear build
    /return TRUE
  }

  /if (${_buildMismatch}) {
    DEBUG ${sep}build mismatch
    /while (TRUE) {
      /sqlite query "${DBloot}" updatebuildnum UPDATE metadata SET version='${maEnv.Find[build].Value}' WHERE name='build_number';
      /if (!${sql_check[${_debug}, updatebuildnum, sql_schema_loot, "UPDATE (metadata)"]})  /endmacro
      /if (${sqlite.Resultcode[updatebuildnum]} != 0) /delay 1
      /if (${sqlite.Resultcode[updatebuildnum]} == 0) /break
    }
    /sqlite clear updatebuildnum
  }

  /if (${_schemaMismatch}) {
    /if (${sqlite.Status[schemanumber].NotEqual[success]}) {
      OUT \a-tSchema Update\ax${sep}\a-wloot\ax (\a-r${sqlite.Result[schemanumber 1 version]}\ax -> \a-r${maEnv.Find[schema].Value}\ax)
    } else {
      OUT \a-tSchema Update\ax${sep}\a-wloot\ax (\a-r${maEnv.Find[schema].Value}\ax)
    }

    /while (TRUE) {
      /sqlite query "${DBloot}" updateschemanum UPDATE metadata SET version='${maEnv.Find[schema].Value}' WHERE name='schema_number';
      /if (!${sql_check[${_debug}, updateschemanum, sql_schema_loot, "UPDATE (metadata)"]}) /endmacro

      | call schema update by version here!
      /call sql_schema_update ${_debug} loot

      /if (${sqlite.Resultcode[updateschemanum]} != 0) /delay 1
      /if (${sqlite.Resultcode[updateschemanum]} == 0) /break
    }
    /sqlite clear schemanumber
    /sqlite clear updateschemanum
  }

  | set pragma
  /sqlite query "${DBloot}" query_journal_DBloot PRAGMA journal_mode=WAL;

  /if (${maDebug.Find[init].Value}) {
    OUT \agsql_schema_loot\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: generate character file DB
 * use:
 ***|
sub sql_schema_character(bool _debug)
  DEBUG \atsql_schema_character\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  /declare _metadataFail bool local FALSE

  | get character build version and maintain previous number
  /sqlite query "${DBcharacter}" buildnumber SELECT version FROM metadata WHERE name='build_number';
  /if (!${sql_check[${_debug}, buildnumber, sql_schema_character, "SELECT (buildnumber)"]}) /endmacro
  /if (${sqlite.Rows[buildnumber]}) {
    /invoke ${maData.Add[stPreviousBuild,${sqlite.Result[buildnumber 1 version]}]}
  } else {
    /invoke ${maData.Add[stPreviousBuild,0]}
  }

  | get character schema version and maintain previous number
  /sqlite query "${DBcharacter}" schemanumber SELECT version FROM metadata WHERE name='schema_number';
  /if (!${sql_check[${_debug}, schemanumber, sql_schema_character, "SELECT (schemanumber)"]}) /endmacro
  /if (${sqlite.Rows[schemanumber]}) {
    /invoke ${maData.Add[stPreviousSchema,${sqlite.Result[schemanumber 1 version]}]}
  } else {
    /invoke ${maData.Add[stPreviousSchema,0]}
    /varset _metadataFail TRUE
  }

  /declare _schemaMismatch bool local FALSE
  /declare _buildMismatch bool local FALSE

  | macro mismatch flag
  /if (${sqlite.Result[buildnumber 1 version].NotEqual[${maEnv.Find[build].Value}]}) {
    /varset _buildMismatch TRUE
  }

  | schema mismatch flag
  /if (${sqlite.Result[schemanumber 1 version].NotEqual[${maEnv.Find[schema].Value}]}) {
    /varset _schemaMismatch TRUE
  }

  | if the numbers match, we consider it valid and go away to dosoemthing else.
  /if (!${_schemaMismatch} && !${_buildMismatch}) {
    DEBUG ${sep}Macro and Schema builds match for character database, bailing on this routine
    /sqlite clear schemanumber
    /sqlite clear buildnumber
    /return TRUE
  }

  | schema and macro builds missing? (fresh database)
  /if (!${sqlite.Rows[schemanumber]}) {
    OUT \a-tSchema Build\ax${sep}\a-wcharacter\ax (\a-r${maEnv.Find[schema].Value}\ax)
    /sqlite query "${DBcharacter}" build CREATE TABLE IF NOT EXISTS metadata(name, version);
    /if (!${sql_check[${_debug}, build, sql_schema_character, "CREATE TABLE IF NOT EXISTS (metadata)"]}) /endmacro
    /sqlite query "${DBcharacter}" build INSERT OR IGNORE INTO metadata VALUES('build_number', '${maEnv.Find[build].Value}');
    /if (!${sql_check[${_debug}, build, sql_schema_character, "INSERT INTO (metadata)"]}) /endmacro
    /sqlite query "${DBcharacter}" build INSERT OR IGNORE INTO metadata VALUES('schema_number', '${maEnv.Find[schema].Value}');
    /if (!${sql_check[${_debug}, build, sql_schema_character, "INSERT INTO (metadata)"]}) /endmacro
    /sqlite query "${DBcharacter}" build INSERT INTO metadata VALUES('repo_number', '${maEnv.Find[repo].Value}');
    /if (!${sql_check[${_debug}, build, sql_schema_character, "INSERT INTO (metadata)"]}) /endmacro

    /sqlite clear build
    /if (!${_metadataFail}) {
      /sqlite clear schemanumber
      /return TRUE
    }
    /sqlite query "${DBcharacter}" schemanumber SELECT version FROM metadata WHERE name='schema_number'
  }

  | updates to macro on a version mismatch
  /if (${_buildMismatch} || ${_metadataFail}) {
    DEBUG ${sep}correct build mismatch
    /sqlite query "${DBcharacter}" updatebuildnum  UPDATE metadata SET version='${maEnv.Find[build].Value}' WHERE name='build_number'
    /if (!${sql_check[${_debug}, updatebuildnum,  sql_schema_character, "UPDATE (metadata)"]}) /endmacro
    /sqlite clear updatebuildnum
  }

 | updates to schema on a version mismatch
  /if (${_schemaMismatch} || ${_metadataFail}) {
    DEBUG ${sep}\arcorrect schema mismatch\ax

    /if (${sqlite.Status[schemanumber].NotEqual[success]}) {
      OUT \a-tSchema Update\ax${sep}\a-wcharacter\ax (\a-r${sqlite.Result[schemanumber 1 version]}\ax -> \a-r${maEnv.Find[schema].Value}\ax)
    } else {
      OUT \a-tSchema Update\ax${sep}\a-wcharacter\ax (\a-r${maEnv.Find[schema].Value}\ax)
    }

    /sqlite query "${DBcharacter}" updateschemanum UPDATE metadata SET version='${maEnv.Find[schema].Value}' WHERE name='schema_number';
    /if (!${sql_check[${_debug}, updateschemanum, sql_schema_shared, "UPDATE (metadata)"]}) /endmacro

    /declare _mi mapiterator local
    /vardata _mi maDBTables.First.Clone
    /while (!${_mi.IsEnd}) {
      /sqlite query "${DBcharacter}" build CREATE TABLE IF NOT EXISTS '${_mi.Key}' ('${_mi.Value}' TEXT NOT NULL PRIMARY KEY,'value' TEXT NOT NULL, 'aux' TEXT);
      /if (!${sql_check[${_debug}, build, sql_schema_character, "CREATE TABLE IF NOT EXISTS (${_mi.Key})"]}) /endmacro
      /invoke ${_mi.Advance}
    }

    /call sql_schema_update ${_debug} character
    /sqlite clear updateschemanum
    /sqlite clear schemanumber

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



|** testing variables (no ingame use) **|
  /declare test1 int outer 0



|** data **|
  /call set_data ${_debug} maData stCastReturn                            map outer FALSE



|** cast type search order **|
  /call set_data_list ${_debug} maChr lsOrderCastType                    alt|pulse|spell|disc|item|ability     "${auxli} cast type order search"


|** HUD settings **|
  /call set_data_map ${_debug} maHud swTabBuff                            TRUE                         "${auxsw} buff tab"
  /call set_data_map ${_debug} maHud swTabHeal                            TRUE                         "${auxsw} heal tab"
  /call set_data_map ${_debug} maHud swTabCombat                          TRUE                         "${auxsw} combat tab"
  /call set_data_map ${_debug} maHud swTabPull                            TRUE                         "${auxsw} pull tab"
  /call set_data_map ${_debug} maHud swTabRez                             TRUE                         "${auxsw} rez tab"
  /call set_data_map ${_debug} maHud swTabTie                             TRUE                         "${auxsw} tie tab"
  /call set_data_map ${_debug} maHud swTabRest                            TRUE                         "${auxsw} rest tab"
  /call set_data_map ${_debug} maHud swTabTarget                          TRUE                         "${auxsw} target tab"
  /call set_data_map ${_debug} maHud swTabAbout                           FALSE                        "${auxsw} about tab"




|** control: grain **|
  /call set_data_list ${_debug} maEnv lsGrain                             1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30        "${auxnum} visual outputs you wish to see. \aw/verb aux\ax for full list"



|** control: predictive **|
  /invoke ${set_data_timer[${_debug}, Check_Prediction, 1]}
  /call set_data_map ${_debug} maPred swPred                              FALSE                         "${auxsw} use predictive combat logic"
  /call set_data_map ${_debug} maPred stPredRad                           40                            "${auxnum} radius around predicted pull target"
  /call set_data_map ${_debug} maPred stPredCountPrepBurn                 0                             "${auxnum} prep burn routines on number of mobs"
  /call set_data_map ${_debug} maPred stPredCountPrepClass                0                             "${auxnum} prep class specific routine on number of mobs"
  /call set_data_map ${_debug} maPred stPredPlayerstate                   4                             "${auxnum} playerstate of the type of mobs you are looking for"
  /call set_data_map ${_debug} maPred stPredDelay                         60                            "${auxnum} seconds to delay next check once activated"
  /call set_data ${_debug} maPred stPredID                                map outer FALSE
  /call set_data ${_debug} FALSE lsPredPullers                            list outer EMPTY




|** control: mercenary **|
  /call set_data_map ${_debug} maMercenary swMercenary                    FALSE                         "${auxsw} use your mercenary" mercenary
  /call set_data_map ${_debug} maMercenary stMercenaryClass               FALSE                         "${auxna} class type of the mercenary you want to use" mercenary




|** control: hard **|
| these variables are not for users to change
| they are here in case adjustments need to be made so i dont have to go through k's of lines of code to find them all
  /call set_data_map ${_debug} maHard stMaxMeleeAdj                       9                             "${auxna} % variance for max meele range adjustment"                                hard
  /call set_data_map ${_debug} maHard stRadGrab                           20                            "${auxna} pick up allowable 3d radius for EQ"                                       hard
  /call set_data_map ${_debug} maHard stMaxRadPet                         200                           "${auxna} max usable range for pets"                                                hard
  /call set_data_map ${_debug} maHard stMinBowDist                        31                            "${auxna} minimum distance to be able to bow or throw"                              hard
  /call set_data_map ${_debug} maHard stMaxBowDist                        250                           "${auxna} maximum distance to bow or throw"                                         hard
  /call set_data_map ${_debug} maHard stMinMovetoBow                      35                            "${auxna} minimum distance to move to get bow/throw range"                          hard
  /call set_data_map ${_debug} maHard stRezSuccess                        30s                           "${auxnum} timer reset to rez the same person on a success"                         hard
  /call set_data_map ${_debug} maHard stRezFail                           15s                           "${auxnum} timer reset to rez the same person on a fail"                            hard
  /call set_data_map ${_debug} maHard stRezFailToken                      1m                            "${auxnum} timer reset to rez the same person on a fail with a token rez"           hard
  /call set_data_map ${_debug} maHard stRezSummonRange                    100                           "${auxnum} /corpse summon range when rezing"                                        hard
  /call set_data_map ${_debug} maHard stRezIgnoreFucktards                30m                           "${auxnum} suration to mark corpses on the no rez list"                             hard
  /call set_data_map ${_debug} maHard stMaxCureRad                        200                           "${auxnum} max radius to check for curing"                                          hard
  /call set_data_map ${_debug} maHard stPullMobTagTry                     5                             "${auxnum} max trys to tag a mob while pulling"                                     hard
  /call set_data_map ${_debug} maHard stBardSwapRemain                    1500                          "${auxnum} ms left on cast time to swap instrument in"                              hard
  /call set_data_map ${_debug} maHard stSpellBookSize                     960                           "${auxnum} maximum # of spell book slots"                                           hard
  /call set_data_map ${_debug} maHard stFoodMaintain                      3000                          "${auxnum} level of, i'm full, to maintain for food"                                hard
  /call set_data_map ${_debug} maHard stDrinkMaintain                     3000                          "${auxnum} level of, i'm not thirsty, to maintain for drink"                        hard
  | /call set_data_map ${_debug} maHard stPullMobFindStep                   5                             "${auxnum} number of increments your pull radius is broken into to look for mobs"   hard
  /call set_data_map ${_debug} maHard stModeDragTargetCheck               5                             "${auxnum} speed that you grab another corpse to drag. lower = faster"              hard




|** control: chr (shared) **|
  | invs
  /call set_data_map ${_debug} maChr stInvisSingle                        FALSE                         "${auxsw} self invisibility"
  /call set_data_map ${_debug} maChr stInvisGroup                         FALSE                         "${auxsw} group invisibility"
  /call set_data_map ${_debug} maChr stInvisSingleIVU                     FALSE                         "${auxsw} self invis vs undead"
  /call set_data_map ${_debug} maChr stInvisGroupIVU                      FALSE                         "${auxsw} group invis vs undead"
  /call set_data_map ${_debug} maChr stInvisDelay                         0                             "${auxnum} delay to cast (10ths of a second)"
  /call set_data_map ${_debug} maChr stOrnamentRobe                       FALSE                         "${auxna} name of the robe you want to swap out for the plaee/ornament visable bug"




|** control: env **|
  /call set_data ${_debug} FALSE lsLootMode                               list outer off|manual|advanced
  /call set_data_map ${_debug} maEnv  stLootMode                          off                           "${auxna} [\a-woff-manual-advanced\ax] how or if you want to loot things"
  /call set_data ${_debug} maEnv swAuto                                   map outer FALSE
  /call set_data_map ${_debug} maEnv swSafeListGuild                      FALSE                         "${auxsw} let guild in your safe list"
  /invoke ${set_data_timer[${_debug}, Check_EXP_Adjust, 5m]}
  /call set_data_map ${_debug} maEnv stMobLvlMin                          1                             "${auxnum} lowest level mob you will attack"
  /call set_data_map ${_debug} maEnv stMobLvlMax                          500                           "${auxnum} highest level mob you will attack"

  | /call set_data_map ${_debug} maEnv swADVLoot                            FALSE                         "${auxsw} use advanced loot functions"
  /call set_data_map ${_debug} maEnv swAutoInv                            FALSE                         "${auxsw} use autoinventory loot control on cursor items"
  /call set_data_list ${_debug} maEnv lsAutoInventory                     EMPTY                         "${auxli} list of shit to auto inventory"
  /call set_data_map ${_debug} maEnv stBuffGem                            ${Me.NumGems}                 "${auxnum} gem slot to use as a swap for buffing"
  /call set_data_map ${_debug} maEnv swHoldBuffGem                        FALSE                         "${auxsw} remember and return the spell used in that slot before buffing"
  /call set_data_map ${_debug} maEnv stEnvRadius                          31                            "${auxnum} radius you are willing to operate in"
  /call set_data_map ${_debug} maEnv stEnvSafeRadius                      30                            "${auxnum} radius you consider safe even with mobs in it"
  /call set_data_map ${_debug} maEnv stLvlPct                             99                            "${auxnum} % of exp in a level you want to maintain"
  /call set_data_map ${_debug} maEnv stLvlMax                             110                           "${auxnum} level you want to maintain"
  /call set_data_map ${_debug} maEnv swCorrectEXPAA                       FALSE                         "${auxsw} use exp/aa adjust functions"
  /call set_data_map ${_debug} maEnv stBuildSetCurrent                    FALSE                         "${auxna} name of current \a-w/build set\ax"

  /invoke ${set_data_timer[${_debug}, Waste_Fireworks, 15m]}
  /call set_data_map ${_debug} maEnv swAAFireworks                        FALSE                         "${auxsw} waste aa on fireworks"

  /call set_data_map ${_debug} maEnv swTakeGroupInvite                    FALSE                         "${auxsw} take group invites from people you trust"
  /call set_data_map ${_debug} maEnv swTakeRaidInvite                     FALSE                         "${auxsw} take raid invites from people you trust"
  /call set_data_map ${_debug} maEnv swSoS                                FALSE                         "${auxsw} rogue sos switch"

  | food and drink
  /invoke ${set_data_timer[${_debug}, Check_FoodDrink, 10m]}
  /call set_data_map ${_debug} maEnv stFood                               FALSE                         "${auxna} food item you want to eat"
  /call set_data_map ${_debug} maEnv stDrink                              FALSE                         "${auxna} drink item you want to drink"

  | tribute switch
  /call set_data_map ${_debug} maEnv swCheckTribute                       FALSE                         "${auxsw} check and corect tribute for raids"

  | drop buffs list
  /invoke ${set_data_timer[${_debug}, Check_Drop_Buffs, 6s]}
  /call set_data_map ${_debug} maBuff swDropBuffs                         FALSE                         "${auxsw} enable buff removal"
  /call set_data_list ${_debug} maEnv lsDropBuffs                         EMPTY                         "${auxli} list of buffs to drop that are smelly"
  /call set_data ${_debug} maEnv swAlwaysDropInvisCauseThePeopleIGroupWithAreAssHats     map outer FALSE

  | no cast buff list??
  /call set_data_list ${_debug} maChr lsNoCastBuff                        EMPTY                         "${auxli} buffs on you to stop casting"

  /call set_data_map ${_debug} maEnv swBegMAG                             FALSE                         "${auxsw} beg mages for pet gear or rods" 

  | ignore invis checks
  /call set_data_map ${_debug} maEnv swIgnoreInvis                        FALSE                         "${auxsw} ignore all invis checks" 
  


|** control: rest **|
  /call set_data_map ${_debug} maRest stPctRest                           0                             "${auxnum} % health/mana to rest at"                  rest
  /call set_data_map ${_debug} maRest swRestFull                          FALSE                         "${auxsw} rest to full health if possible"            rest
  /call set_data_map ${_debug} maRest swRestModRod                        FALSE                         "${auxsw} use mod rods while resting"                 rest
  /call set_data_map ${_debug} maRest swRestBuff                          FALSE                         "${auxsw} do buffing while resting"                   rest
  /call set_data_map ${_debug} maRest swRestCombat                        FALSE                         "${auxsw} continue resting while in combat"           rest
  /call set_data_map ${_debug} maRest swRestCheckHealth                   FALSE                         "${auxsw} check health as well when resting"          rest
  /call set_data_map ${_debug} maRest stRestRadius                        30                            "${auxnum} no mob in radius for resting"              rest
  /call set_data_map ${_debug} maRest swRestInRaid                        FALSE                         "${auxsw} enable resting while in raids"              rest
  /call set_data_map ${_debug} maRest stRestSpawnSearch                   "npc radius ${Parse[0,${maRest.Find[stRestRadius].Value}]} playerstate 4"      "${auxna} spawn search string for resting"            rest




|** control: heal **|
  /call set_data_list ${_debug} maHeal lsOrderHeal                        cure|tot|self|dannet|ch|group|xtarget|pet         "${auxli} heal cycle order"                               heal

  /call set_data_map ${_debug} maHeal stHealAdjust                        0                             "${auxnum} % to overide max health to"                                        heal
  /call set_data_map ${_debug} maHeal swHealSelf                          FALSE                         "${auxsw} enable self healing"                                                heal
  /call set_data_map ${_debug} maHeal swHealToT                           FALSE                         "${auxsw} enable healing your targets target"                                 heal
  /call set_data_map ${_debug} maHeal swHealXTarget                       FALSE                         "${auxsw} enable xtarget healing"                                             heal
  /call set_data_map ${_debug} maHeal swHealPet                           FALSE                         "${auxsw} enable pet healing"                                                 heal
  /call set_data_map ${_debug} maHeal swHealGroup                         FALSE                         "${auxsw} enable group healing"                                               heal
  /call set_data_map ${_debug} maHeal swHealDanNet                        FALSE                         "${auxsw} enable use of DanNet for healing"                                   heal

  /call set_data_map ${_debug} maHeal stRampageTank                       FALSE                         "${auxna} rampage tank to not heal (\a-wcompletely ignore\ax)"                heal
  /call set_data_map ${_debug} maHeal swHealOverTime                      FALSE                         "${auxsw} enable to use heal over time"                                       heal
  /call set_data_map ${_debug} maHeal stCountHealGroup                    0                             "${auxnum} number in group hurt to use a group heal"                          heal

  /call set_data_map ${_debug} maHeal swBreakHealPCT                      FALSE                         "${auxsw} enable a stop heal when target is > its heal point"                 heal

  /call set_data_list ${_debug} maHeal lsHealXTClass                      EMPTY                         "${auxli} classes to populate xtarget for healing use: \a-t/heal xtarget\ax"  heal
  /call set_data_list ${_debug} maHeal lsNoHealToon                       EMPTY                         "${auxli} fuck these people.. no heals for them.."                            heal
  /call set_data_map ${_debug} maHeal swHealWeighted                      FALSE                         "${auxsw} enable weighted HP check healing"                                   heal
  /call set_data_map ${_debug} maHeal swHealPromise                       FALSE                         "${auxsw} enable use of promised heal spells healing"                         heal

  /call set_data ${_debug} FALSE tmpHealAdjust                            float outer 99.99
  /call set_data ${_debug} FALSE hurtID                                   int outer 0
  /call set_data ${_debug} FALSE hurtPctHP                                int outer 100
  /call set_data ${_debug} FALSE hurtCLS                                  string outer ${Me.Class.ShortName}
  /call set_data ${_debug} FALSE hurtCount                                int outer 0
  /call set_data ${_debug} FALSE hurtDead                                 int outer 0

  | points for healing
  /call set_data_map ${_debug} maHeal stHealPointHoT                    80                            "${auxnum} % of health to use single heal over time"                          heal
  /declare _lishp listiterator local
  /vardata _lishp lsClassAll.First
  /while (!${_lishp.IsEnd}) {
    /call set_data_map ${_debug} maHeal stHealPoint${_lishp.Value}      80                            "${auxnum} % of health to start healing:\a-w${_lishp.Value}\ax"               heal
    /invoke ${_lishp.Advance}
  }

  /call set_data_list ${_debug} maHeal lsProtectOutsideGroup              EMPTY                         "${auxli} names outside of your group to add to xtarget for healing" heal
  /call set_data_list ${_debug} maHeal lsLineHeal                         EMPTY                         "${auxli} alternate cast order for heals"                            heal

  | xtarget sets heal classes
  /if (${lsClassHeal.Contains[${Me.Class.ShortName}]}) {
    /declare lsXTargetClass list outer
    /invoke ${set_data_timer[${_debug}, Check_Xtarget_Build, 10m]}
  }




|** control: cure  **|
  /invoke ${set_data_timer[FALSE, Check_Cure, 6s]}
  /call set_data_list ${_debug} maCure lsLineCure                         EMPTY                         "${auxli} alternate cast order for cures"                 cure
  /call set_data_list ${_debug} maCure lsNoCure                           EMPTY                         "${auxli} list of debuffs you DO NOT want to cure"        cure
  /call set_data_map ${_debug} maCure swCureSelf                          FALSE                         "${auxsw} enable self curing"                             cure
  /call set_data_map ${_debug} maCure swCureGroup                         FALSE                         "${auxsw} enable group curing"                            cure
  /call set_data_map ${_debug} maCure swCureCrew                          FALSE                         "${auxsw} enable crew curing (everyone)"                  cure




|** control: rez  **|
  /call set_data ${_debug} FALSE swIsMeDead                               bool outer FALSE
  /call set_data ${_debug} FALSE lsRezPct                                 list outer "10|20|35|50|60|75|85|90|93|96"
  /call set_data_list ${_debug} maRez lsRezIC                             PREGEN|1                      "${auxli} names of in combat rezes"                                           rez
  /call set_data_list ${_debug} maRez lsRezOOC                            PREGEN|1                      "${auxli} names of  out of combat rezes"                                      rez
  /call set_data_list ${_debug} maRez lsNoRezToon                         EMPTY                         "${auxli} names of people you never want to rez"                              rez
  /call set_data_list ${_debug} maRez lsRezTokenToon                      EMPTY                         "${auxli} list of class shortnames you want to use a rez token on"            rez
  /call set_data_map ${_debug} maRez swRezIC                              FALSE                         "${auxsw} rez people while in combat"                                         rez
  /call set_data_map ${_debug} maRez swRezOOC                             FALSE                         "${auxsw} rez people while out of combat"                                     rez

  /call set_data_map ${_debug} maRez swRezTake                            FALSE                         "${auxsw} take rezes"                                                         rez
  /call set_data_map ${_debug} maRez swRezTakeCall                        FALSE                         "${auxsw} take call to corpses"                                               rez
  /call set_data_map ${_debug} maRez swRezEveryone                        FALSE                         "${auxsw} rez anyone.. we just don't care do we?"                             rez
  /call set_data_map ${_debug} maRez swRezDanNet                          FALSE                         "${auxsw} rez your DanNet peoples"                                            rez
  /call set_data_map ${_debug} maRez swRezToken                           FALSE                         "${auxsw} use rez tokens (\a-wseparate from stResurrection\ax)"               rez
  /call set_data_map ${_debug} maRez stPctMinRez                          90                            "${auxnum} minimum % rez you are willing to accept"                           rez
  /call set_data_map ${_debug} maRez stMaxRezRange                        60                            "${auxnum} how far away to look for corpses to rez"                           rez
  /call set_data_map ${_debug} maRez stRez1                               "pccorpse cleric raid"        "rez priority 1 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez1                               TRUE                          "rez priority 1 switch"                                                       rez
  /call set_data_map ${_debug} maRez stRez2                               "pccorpse cleric group"       "rez priority 2 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez2                               TRUE                          "rez priority 2 switch"                                                       rez
  /call set_data_map ${_debug} maRez stRez3                               "pccorpse group"              "rez priority 3 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez3                               TRUE                          "rez priority 3 switch"                                                       rez
  /call set_data_map ${_debug} maRez stRez4                               "pccorpse raid tank"          "rez priority 4 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez4                               TRUE                          "rez priority 4 switch"                                                       rez
  /call set_data_map ${_debug} maRez stRez5                               "pccorpse raid"               "rez priority 5 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez5                               TRUE                          "rez priority 5 switch"                                                       rez
  /call set_data_map ${_debug} maRez stRez6                               "pccorpse guild"              "rez priority 6 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez6                               TRUE                          "rez priority 6 switch"                                                       rez
  /call set_data_map ${_debug} maRez stRez7                               FALSE                         "rez priority 7 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez7                               FALSE                         "rez priority 7 switch"                                                       rez
  /call set_data_map ${_debug} maRez stRez8                               FALSE                         "rez priority 8 spawn string search"                                          rez
  /call set_data_map ${_debug} maRez swRez8                               FALSE                         "rez priority 8 switch"                                                       rez
  /call set_data_list ${_debug} maRez lsLineRez                           "1|2|3|4|5|6"                 "${auxli} rez order priorities"                                               rez




|** control: buff **|
  /invoke ${set_data_timer[${_debug}, Check_Buff_Cycle, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Unity, 5m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Shrink, 10m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Self, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Crew, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Raid, 2m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Group, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Beg, 1m]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Temp, 30s]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Now, 5s]}
  /invoke ${set_data_timer[${_debug}, Check_Buff_Powersource, 10m]}
  /call set_data ${_debug} maBuff swBuffMaster                            map outer TRUE
  /call set_data_map ${_debug} maBuff swBuffSelf                          FALSE                         "${auxsw} buff yourself"
  /call set_data_map ${_debug} maBuff swBuffCrew                          FALSE                         "${auxsw} buff your crew (\aynot you\ax)"
  /call set_data_map ${_debug} maBuff swBuffRaid                          FALSE                         "${auxsw} buff raid members (not crew)"
  /call set_data_map ${_debug} maBuff swBuffGroup                         FALSE                         "${auxsw} buff group members (not crew)"
  /call set_data_map ${_debug} maBuff swBuffBeg                           FALSE                         "${auxsw} answer buff beg requests"
  /call set_data_map ${_debug} maBuff swBuffUnity                         FALSE                         "${auxsw} buff aa unity"
  /call set_data_map ${_debug} maBuff swBuffPowersource                   FALSE                         "${auxsw} use powersources (\a-wmust have buff set with tag\ax)"
  /call set_data_map ${_debug} maBuff swBuffTemp                          FALSE                         "${auxsw} buff short term combat buffs"
  /call set_data_map ${_debug} maBuff swBuffShrink                        FALSE                         "${auxsw} buff shring"
  /call set_data_map ${_debug} maBuff stShrinkItem                        FALSE                         "${auxna} buff shrink item/spell"
  /call set_data_map ${_debug} maBuff stShrinkHeight                      2.04                          "${auxna} shrink min height"
  /call set_data_map ${_debug} maBuff swBuffNow                           FALSE                         "${auxsw} buff the now things"
  /call set_data_map ${_debug} maBuff swBuffAmmo                          FALSE                         "${auxsw} summons ammo"

  | auras
  /invoke ${set_data_timer[${_debug}, Check_Buff_Aura, 3m]}
  /call set_data_map ${_debug} maBuff swBuffAura                          FALSE                         "${auxsw} use auras or not. i dont care"

  | tlp specific
  /call set_data_map ${_debug} maBuff swBuffKeepMount                     FALSE                         "${auxsw} allows TLPs to keep the mount buff and the mount stat buff"
  


|** control: melee **|
  /call set_data_list ${_debug} maMelee lsLineMelee                       EMPTY                         "${auxli} alternate defense activation order" melee




|** control: defense **|
  /call set_data_map ${_debug} maDefense stCountDef                       0                             "${auxna} number of imediate area mobs to activate defensive" defense
  /call set_data_map ${_debug} maDefense stPctDef                         0                             "${auxna} pct of health to activate a defensive" defense
  /call set_data_list ${_debug} maDefense lsLineDefense                   EMPTY                         "${auxli} alternate defense activation order" defense




|** control: home **|
  | control list for facing
  /call set_data ${_debug} FALSE lsCombatPosition                         list outer free|left|right|rear|face|random
  | user list for preferences on facing (what they can edit)
  /call set_data_list ${_debug} maHome lsPosition                         left|right|rear|rear|rear
  /call set_data_map ${_debug} maHome stCombatPosition                    free                          "${auxnum} where you will stick to the mob"
  /call set_data ${_debug} maHome swHomeMarker                            map outer FALSE
  /call set_data_map ${_debug} maHome stHomeVariance                      6                             "${auxnum} acceptable buffer range for being home"
  /call set_data ${_debug} maData stCampX                                 map outer 999999999
  /call set_data ${_debug} maData stCampY                                 map outer 999999999
  /call set_data ${_debug} maData stCampZ                                 map outer 999999999
  /call set_data_map ${_debug} maHome stMount                             FALSE                         "${auxna} name of mount you wish to sit your fat ass on"
  /call set_data_map ${_debug} maHome swMoveDeathClear                    FALSE                         "${auxsw} on = clear home marker and movements on death"
  /call set_data_map ${_debug} maHome swMoveGatherClear                   FALSE                         "${auxsw} on = clear home marker when a gather is issued"
  /call set_data_map ${_debug} maHome swFaceFast                          FALSE                         "${auxsw} use fast facing movements"
  /call set_data_map ${_debug} maHome stFaceArc                           15                            "${auxnum} [\a-w1-45\az] acceptable range of facing and still be on target"
  /call set_data ${_debug} maHome swSnapStuck                             map outer FALSE
  /call set_data_map ${_debug} maHome swonAuto                            FALSE                         "${auxsw} set home camp on auto"
  /call set_data_map ${_debug} maHome swinCombat                          FALSE                         "${auxsw} keep home camp while in combat"




|** control: tie **|
  /invoke ${set_data_timer[${_debug}, Check_Tie, 5]}
  /call set_data ${_debug} maTie swSetTie                                 map outer FALSE
  /call set_data_map ${_debug} maTie stTieToon                            FALSE                         "${auxna} name of toon you want to tie too when commanded"
  /call set_data_map ${_debug} maTie stTieMode                            nav                           "${auxna} [\a-wstick-nav\ax] the mode you want to use for tie movements"
  /call set_data_map ${_debug} maTie stTieVariance                        5                             "${auxnum} acceptabe distances to be away from the tie toon before moving to it"
  /call set_data_map ${_debug} maTie swTieCombat                          FALSE                         "${auxsw} enable tie while in active combat"

  | force break movement command
  /call set_data_map ${_debug} maTie swTieBreak                           FALSE                         "${auxsw} breaks the fucking world to tie"
  /call set_data ${_debug} maData swGetOverHere                           map outer FALSE




|** control: mode **|
  /call set_data ${_debug} FALSE swModeToken                              bool outer FALSE
  /call set_data ${_debug} FALSE stModeData                               string outer FALSE
  | /call set_data_map ${_debug} maMode swTrainAlteration                   FALSE                         "${auxsw} train alteration type spells"
  | /call set_data_map ${_debug} maMode swTrainAbjuration                   FALSE                         "${auxsw} train abjuration type spells"
  | /call set_data_map ${_debug} maMode swTrainConjuration                  FALSE                         "${auxsw} train conjuration type spells"
  | /call set_data_map ${_debug} maMode swTrainDivination                   FALSE                         "${auxsw} train divination type spells"
  | /call set_data_map ${_debug} maMode swTrainEvocation                    FALSE                         "${auxsw} train evocation type spells"
  | /call set_data ${_debug} FALSE swTrainSpecialize                        FALSE                         "${auxsw} train specialize field type spells"




|** control: watch **|
  /invoke ${set_data_timer[${_debug}, Check_Watch, 5s]}
  /call set_data_map ${_debug} maWatch swWatch                            FALSE                         "${auxsw} watch for spawns"   
  /call set_data_map ${_debug} maWatch swWatchBeep                        FALSE                         "${auxsw} beep when a watched mob spawns"   
  /call set_data ${_debug} FALSE stWatchCount                             int outer 0
  /call set_data ${_debug} FALSE stWatchCountChange                       int outer 0
  /call set_data_list ${_debug} maWatch lsWatch                           EMPTY                         "${auxli} list of mobs to watch for"




|** control: pull **|
  /call set_data_list ${_debug} maPull lsLinePull                         EMPTY                         "${auxli} alternate order for pulling"                                                                            pull
  /call set_data_map ${_debug} maPull swPull                              FALSE                         "${auxsw} enable pulling (\a-wselect mobs to kill\ax)"                                                            pull
  /call set_data_map ${_debug} maPull stPullRadius                        0                             "${auxnum} radius to pull mobs from"                                                                              pull
  /call set_data_map ${_debug} maPull stPullPetWatch                      0                             "${auxnum} radius to pull dumb ass NPC pets"                                                                      pull
  /call set_data_map ${_debug} maPull swPullSetHome                       FALSE                         "${auxsw} suto set home marker when pulling is enabled"                                                           pull
  /call set_data_map ${_debug} maPull stPullNavStopDistance               60                            "${auxnum} how close you will nav pull to a mob before trying too tag it"                                         pull
  /call set_data_map ${_debug} maPull stPullNavVariance                   0                             "${auxnum} % variance added to pull radisu to exceed nav pull lengths to get to a mob"                            pull
  /call set_data ${_debug} FALSE lsPullMode                               list outer base|pet|int|nav|multi|watch
  /call set_data_map ${_debug} maPull stPullMode                          base                          "${auxna} [\a-wbase-nav-int-pet-multi\ax] the mode of pulling you wish to use while looking for said mobs"        pull
  /call set_data ${_debug} FALSE lsPullWith                               list outer melee|range|other
  /call set_data_map ${_debug} maPull stPullWith                          FALSE                         "${auxna} [\a-wmelee-range-other\ax] what you want to pull with???"             pull
  /call set_data_map ${_debug} maPull stRangeBandolier                    FALSE                         "${auxna} name of bandolier for ranged pulling?"                                pull
  /call set_data_map ${_debug} maPull stPctHardStop                       0                             "${auxnum} % of crew sta/manna to stop pulling at to give them rest time?"      pull
  /call set_data_map ${_debug} maPull stHardStopDuration                  0                             "${auxnum} duration to stop pulling for crew to recover"                        pull
  /call set_data_map ${_debug} maPull swHardStopRez                       FALSE                         "${auxsw} stop pulling when someone has rez effects"                            pull
  /call set_data_map ${_debug} maPull stPullZRadius                       60                            "${auxnum} the z distance of your pulling environment"                          pull
  /call set_data_map ${_debug} maPull swNavPathLogic                      TRUE                          "${auxsw} use nav path lenght logic for finding targets to pull"                pull
  /call set_data_map ${_debug} maPull stCountChainPull                    0                             "${auxnum} # of mobs left in area to go pull more"                              pull
  /call set_data_map ${_debug} maPull stPullOutrunRange                   0                             "${auxnum} slowdown range when pulling a mob back to camp so you dont outrun them" pull
  /call set_data_map ${_debug} maPull swPullNavLoS                        TRUE                          "${auxsw} is Line of Sight required for nav pulling"                                pull
  /call set_data_map ${_debug} maPull swPullHealCheck                     FALSE                         "${auxsw} self healing check while pulling"                                         pull
  /call set_data_map ${_debug} maPull stPullStep                          5                             "${auxnum} # of steps to increment mob finding. 0 to disable"                       pull

  | /call set_data_map ${_debug} maPull swPullNoStep                        FALSE                         "${auxsw} do not use steps to find mobs. use entire pull radius"                    pull


  | arc controls
  | /call set_data_map ${_debug} maPull stPullArcCardinal                   FALSE                         "${auxna} cardinal direction to pull from camp"                                 pull
  /call set_data ${_debug} FALSE stPullArcLeft                            float outer 0
  /call set_data ${_debug} FALSE stPullArcRight                           float outer 0
  /call set_data ${_debug} FALSE stPullArcCenter                          float outer 0
  /call set_data ${_debug} maPull stPullArcWidth                          map outer 0




|** control: cc **|
  /call set_data ${_debug} maCC swSetForced                               map outer FALSE
  /call set_data ${_debug} maCC swSetCombat                               map outer FALSE
  /call set_data ${_debug} FALSE stAssTarID                               int outer 0
  /call set_data ${_debug} maCC swValidAssTarID                           map outer FALSE
  /call set_data_map ${_debug} maCC swCombatMelee                         FALSE                         "${auxsw} do melee things while in combat"
  /call set_data_map ${_debug} maCC swCombatRange                         FALSE                         "${auxsw} do range things while in combat"
  /call set_data_map ${_debug} maCC swCombatNuke                          FALSE                         "${auxsw} nuke things while in combat"
  /call set_data_map ${_debug} maCC swCombatDoT                           FALSE                         "${auxsw} dot all the things!!"
  /call set_data_map ${_debug} maCC swCombatHeal                          FALSE                         "${auxsw} enable heal modes in or OUT of combat"
  /call set_data_map ${_debug} maCC swCombatControl                       FALSE                         "${auxsw} enable crowd control modes in or out of combat"
  /call set_data_map ${_debug} maCC swAlliance                            FALSE                         "${auxsw} enable class alliance"
  /call set_data_map ${_debug} maCC swStoponDS                            FALSE                         "${auxsw} Stop combat when the target has a DS. Does not apply agro enabled."

  | glyph
  /call set_data_map ${_debug} maCC swGlyphGroup                          FALSE                         "${auxsw} use glyph in a group"
  /call set_data_map ${_debug} maCC swGlyphRaid                           FALSE                         "${auxsw} use glyph in a raid"
  /call set_data_map ${_debug} maCC stGlyph                               FALSE                         "${auxna} name of the glyph you want to use"

  /invoke ${set_data_timer[${_debug}, Check_Assist_Delay, 5]}
  /call set_data_map ${_debug} maCC stAssistMode                          FALSE                         "${auxna} [\axg,1,2,3,m1,m2,m3\ax] your current smart assist mode"
  /call set_data_map ${_debug} maCC swManualAssist                        FALSE                         "${auxna} use manual assisting"
  /call set_data_map ${_debug} maCC stPctEngage                           98                            "${auxnum} mobs % health to engage it"
  /call set_data_list ${_debug} maCC lsAss                                EMPTY                         "${auxli} list of names to assist"

  /call set_data_map ${_debug} maCC stZRadius                             60                            "${auxnum} the z distance of your environment"

  /call set_data_list ${_debug} maCC lsNoCastWith                         EMPTY                         "${auxli} list of target buffs you dont want to cast with"
  /call set_data_list ${_debug} maCC lsNoMeleeWith                        EMPTY                         "${auxli} list of target buffs you dont want to melee with"

  | combat melee skills by class
  /declare lsMeleeSkill list outer

  | specific lists by class
  /if (${Select[${Me.Class.ShortName},BER]}) {

  } else /if (${Select[${Me.Class.ShortName},BRD]}) {

  } else /if (${Select[${Me.Class.ShortName},BST]}) {

  } else /if (${Select[${Me.Class.ShortName},CLR]}) {

  } else /if (${Select[${Me.Class.ShortName},DRU]}) {

  } else /if (${Select[${Me.Class.ShortName},ENC]}) {

  } else /if (${Select[${Me.Class.ShortName},MAG]}) {

  } else /if (${Select[${Me.Class.ShortName},MNK]}) {

  } else /if (${Select[${Me.Class.ShortName},NEC]}) {

  } else /if (${Select[${Me.Class.ShortName},PAL]}) {
    /invoke ${lsMeleeSkill.Append[bash,taunt,disarm]}
    /call set_data_map ${_debug} maAgro stPctTaunt                        FALSE                         "${auxnum} Agro Pct to spam the taunt key like a noob!"           agro
    /call set_data_map ${_debug} maAgro stPctAgroHold                     FALSE                         "${auxnum} Minimum Agro Pct you want to maintain"                 agro

  } else /if (${Select[${Me.Class.ShortName},RNG]}) {
    /invoke ${lsMeleeSkill.Append[taunt,kick,disarm]}
    /call set_data_map ${_debug} maAgro stPctTaunt                        FALSE                         "${auxnum} Agro Pct to spam the taunt key like a noob!"           agro
    /call set_data_map ${_debug} maAgro stPctAgroHold                     FALSE                         "${auxnum} Minimum Agro Pct you want to maintain"                 agro

  } else /if (${Select[${Me.Class.ShortName},ROG]}) {

  } else /if (${Select[${Me.Class.ShortName},SHD]}) {
    /invoke ${lsMeleeSkill.Append[taunt,disarm,bash]}
    /call set_data_map ${_debug} maAgro stPctTaunt                        FALSE                         "${auxnum} Agro Pct to spam the taunt key like a noob!"           agro
    /call set_data_map ${_debug} maAgro stPctAgroHold                     FALSE                         "${auxnum} Minimum Agro Pct you want to maintain"                 agro

  } else /if (${Select[${Me.Class.ShortName},SHM]}) {

  } else /if (${Select[${Me.Class.ShortName},WAR]}) {
    /invoke ${lsMeleeSkill.Append[bash,taunt,kick,disarm]}
    /call set_data_map ${_debug} maAgro stPctTaunt                        FALSE                         "${auxnum} Agro Pct to spam the taunt key like a noob!"           agro
    /call set_data_map ${_debug} maAgro stPctAgroHold                     FALSE                         "${auxnum} Minimum Agro Pct you want to maintain"                 agro

  } else /if (${Select[${Me.Class.ShortName},WIZ]}) {

  }




|** control: dot **|
  /call set_data_map ${_debug} maDoT stDotRefresh                         3                             "${auxnum} time left on a dot to refresh it, in seconds"          dot
  /call set_data_map ${_debug} maDoT stPctStopDoT                         0                             "${auxnum} % of mob health to stop dotting it at"                 dot
  /call set_data_list ${_debug} maDoT lsLineDoT                           EMPTY                         "${auxli} alternate cast order for dots"                          dot




|** control: nuke **|
  /call set_data_map ${_debug} maNuke stPctStopNuke                       0                             "${auxnum} % of mob health to stop nuking it at"                  nuke
  /call set_data_map ${_debug} maNuke stNukeDelay                         0                             "${auxnum} time to force between using nukes (1/10 second)"       nuke
  /call set_data_list ${_debug} maNuke lsLineNuke                         EMPTY                         "${auxli} alternate cast order for nukes"                         nuke
  /call set_data_map ${_debug} maNuke swNukeLoop                          FALSE                         "${auxsw} force a nuke loop to complete before releasing"         nuke
  /call set_data_map ${_debug} maNuke swNukeWhackAMole                    FALSE                         "${auxsw} sit between nukes. REQUIRES /nuke delay"                nuke




|** control: burn **|
  /call set_data ${_debug} maBurn swBurnConditionMet                      map outer FALSE
  /call set_data ${_debug} maBurn swBurnSpinUp                            map outer FALSE
  /call set_data_map ${_debug} maBurn swBurnForce                         FALSE                         "${auxsw} force burn routines"                                    burn
  /call set_data_map ${_debug} maBurn swBurnAuto                          FALSE                         "${auxsw} auto burn on named encounters"                          burn
  /call set_data_map ${_debug} maBurn stPctBurnEngage                     99                            "${auxnum} % of mobs health to start burns"                       burn
  /call set_data_map ${_debug} maBurn stBurnCount                         0                             "${auxnum} number of mobs in your area to auto engage burns"      burn
  /call set_data_map ${_debug} maBurn swBurnRaid                          FALSE                         "${auxsw} use burns on raids"                                     burn
  /call set_data_list ${_debug} maBurn lsLineBurn                         EMPTY                         "${auxli} alternate burn line order (engine 2 only)"              burn




|** control: debuff **|
  /call set_data_list ${_debug} madeBuff lsdeBuffOrder                    tash|malo|slow|cripple|eradicate|snare      "${auxli} debuff type order"                              debuff
  /call set_data_list ${_debug} madeBuff lsLinedeBuff                     EMPTY                               "${auxli} alternate order for debuffing"                          debuff
  /call set_data_map ${_debug} madeBuff swMez                             FALSE                               "${auxsw} use mez"                                                debuff
  /call set_data_map ${_debug} madeBuff stCountMezResist                  3                                   "${auxnum} number of times to retry single mez with resists"      debuff
  /call set_data_map ${_debug} madeBuff swMalo                            FALSE                               "${auxsw} use malo"                                               debuff
  /call set_data_map ${_debug} madeBuff swTash                            FALSE                               "${auxsw} use tash"                                               debuff
  /call set_data_map ${_debug} madeBuff swSlow                            FALSE                               "${auxsw} use slow"                                               debuff
  /call set_data_map ${_debug} madeBuff swCripple                         FALSE                               "${auxsw} use cripple"                                            debuff
  /call set_data_map ${_debug} madeBuff swEradicate                       FALSE                               "${auxsw} use eradicate"                                          debuff
  /call set_data_map ${_debug} madeBuff stTimerEradicate                  30s                                 "${auxnum} time to retry an eradicate on a target"                debuff
  /call set_data_map ${_debug} madeBuff swSnare                           FALSE                               "${auxsw} use snare"                                              debuff
  /call set_data ${_debug} FALSE lsSlowIgnore                             list outer "Tendon Slice"|"Helix of the Undying"|"Crippling Snare"





|** control: minion **|
  /invoke ${set_data_timer[FALSE, Check_Minion_Cycle, 1m]}
  /call set_data_map ${_debug} maMinion swPet                             FALSE                         "${auxsw} use pets"                                                         minion
  /if (${Select[${Me.Class.ShortName},ENC,DRU,MAG,NEC]}) {
    /call set_data_map ${_debug} maMinion swPetCharm                      FALSE                         "${auxsw} use charm pets"                                                   minion
  }
  /call set_data_map ${_debug} maMinion stPetIllusion                     FALSE                         "${auxna} name of pet illusion whatever spell, clickie, left over dinner"   minion
  /call set_data_map ${_debug} maMinion stPctPetEngage                    95                            "${auxnum} % of mob health to engage pet at"                                minion
  /call set_data_map ${_debug} maMinion swSwarm                           FALSE                         "${auxsw} use swarm pets/items/aas/left shoe"                               minion
  /call set_data_map ${_debug} maMinion stPctSwarmEngage                  95                            "${auxnum} % of mob health to engage swarm pets at"                         minion
  /call set_data_map ${_debug} maMinion swNoSwarmonDS                     FALSE                         "${auxsw} use swarms on mobs woth a damage shield"                          minion
  /call set_data_map ${_debug} maMinion swonForce                         FALSE                         "${auxsw} send pets on \a-w/cc force\ax command"                            minion
  /call set_data_map ${_debug} maMinion swUsePetGear                      FALSE                         "${auxsw} use pet weapons"                                                  minion
  /call set_data_map ${_debug} maMinion stPetWeapon                       FALSE                         "${auxna} weapon to give your pet"                                          minion
  /call set_data_map ${_debug} maMinion stBegAlias                        FALSE                         "${auxna} the alias your mage uses for /beg"                                minion

  /invoke ${set_data_timer[FALSE, Check_Minion_Shrink, 10m]}
  /call set_data_map ${_debug} maMinion swPetShrink                       FALSE                         "${auxsw} use pet shrink item/spell/drugs?"                                 minion
  /call set_data_map ${_debug} maMinion stPetShrink                       FALSE                         "${auxna} name of pet shrink thingie"                                       minion

  /invoke ${set_data_timer[FALSE, Check_Minion_Buff, 1m]}
  /call set_data_map ${_debug} maMinion swPetBuff                         FALSE                         "${auxsw} buff minion (\ayyours\ax)"                                        minion




|** control: over **|
  /call set_data_map ${_debug} maOver swOverLOS                           FALSE                         "${auxsw} override line of sight validations"
  /call set_data_map ${_debug} maOver swOverPctEngage                     FALSE                         "${auxsw} ocerride engage % for all types, instant engage of a mob"
  /call set_data_map ${_debug} maOver swOverLoot                          FALSE                         "${auxsw} attempt to loot regardless of master looter"
  /call set_data_map ${_debug} maOver swOverSafeNames                     FALSE                         "${auxsw} ignore safename checks, trust everyone. pm me your account info please"
  /call set_data_map ${_debug} maOver swOverTrainSpell                    FALSE                         "${auxsw} ignore spell training caps. cast forever"
  /call set_data_map ${_debug} maOver swOverEnvAuto                       FALSE                         "${auxsw} ignore non-persistent auto and start with \a-w/env auto on\ax"
  /call set_data_map ${_debug} maOver swOverDSCheck                       FALSE                         "${auxsw} ignore damage shield checks while melee"
  /call set_data_map ${_debug} maOver swOverNavCheck                      FALSE                         "${auxsw} bypass Nav PathExists checks"
  /call set_data_map ${_debug} maOver swOverGroupAss                      FALSE                         "${auxsw} override group assist rules and force manual"
  /call set_data_map ${_debug} maOver swOverSplashHurt                    FALSE                         "${auxsw} override hurt requirements for splashing (try to keep it in cooldown no matter what)"
  /call set_data_map ${_debug} maOver swTargetClear                       FALSE                         "${auxsw} override clearing targets"




|** control: entropy  **|
  /call set_data_map ${_debug} maEntropy swMaintenance                    FALSE                         "${auxsw} place entropy in a limited maintenance mode for operations"
  /call set_data_map ${_debug} maEntropy swEventsInc                      FALSE                         "${auxsw} use events from events.inc file"
  /call set_data_map ${_debug} maEntropy swDPSMeter                       FALSE                         "${auxsw} use dps meeter"
  /call set_data_map ${_debug} maEntropy swHookSub                        FALSE                         "${auxsw} use hook functions"
  /call set_data_list ${_debug} maEntropy lsSafeNames                     EMPTY                         "${auxli} list of people you trust (\a-wDanNet connections trusted by default\ax)"
  /call set_data_map ${_debug} maEntropy swAutoList                       FALSE                         "${auxsw} autopopulate zone listsnames for some mob types"
  /call set_data_map ${_debug} maEntropy stBuffRefresh                    0                             "${auxnum} time in seconds to refrsh a buff before it fades. 0=wait till it fades"
  /call set_data_map ${_debug} maEntropy swAutoInvManual                  FALSE                         "${auxsw} obey autoinventory fast list even in manual mode"
  /call set_data_map ${_debug} maEntropy swBuildSetRemember               FALSE                         "${auxsw} remember and refresh a set on macro boot"
  /call set_data_map ${_debug} maEntropy stCastLoop                       1                             "${auxna} number of castloop attempts, event based"
  /call set_data_map ${_debug} maEntropy stEngine                         1                             "${auxna} code engine"
  /call set_data_map ${_debug} maEntropy swBuffCooldownWait               FALSE                         "${auxsw} force a wait between buffs for cooldown to do complete cycle"
  /call set_data_map ${_debug} maEntropy swRelayTell                      FALSE                         "${auxsw} relay tells to the tie toon"
  /call set_data_map ${_debug} maEntropy swRelayTellBeep                  FALSE                         "${auxsw} beep on a tell relay"
  /call set_data_map ${_debug} maEntropy swUseConColor                    FALSE                         "${auxsw} Use ConColors for targets"
  /call set_data_map ${_debug} maEntropy swNavLogSpew                     off                           "${auxsw} (on/off) Show mq2nav log spew in mq2 chat window"
  /call set_data_map ${_debug} maEntropy stMainLoopDelay                  1                             "${auxnum} this is the main delay for the master macro loops. USE CAUTION when changing!"
  /call set_data_map ${_debug} maEntropy swLua                            FALSE                         "${auxsw} use mq2lua plugin"
  /call set_data_map ${_debug} maEntropy swTLP                            FALSE                         "${auxsw} TLP player"
  /call set_data_map ${_debug} maEntropy stCountFizzleRetry               0                             "${auxnum} number of cast retrys on a fizzle"

  | hud
  /call set_data_map ${_debug} maEntropy swHUDAuto                        FALSE                         "${auxsw} Auto start HUD on mac start"

  | randomize
  /call set_data_map ${_debug} maEntropy stRNDEngage                      0                             "${auxsw} randomize engage % by ##"
  /call set_data_map ${_debug} maEntropy stRNDEngagePet                   0                             "${auxsw} randomize engage pet % by ##"
  /call set_data_map ${_debug} maEntropy stRNDEngageSwarm                 0                             "${auxsw} randomize engage swarm % by ##"
  /call set_data_map ${_debug} maEntropy stRNDEngageBurn                  0                             "${auxsw} randomize engage burn % by ##"
  /call set_data_map ${_debug} maEntropy stRNDRezTake                     0                             "${auxsw} randomize random delay to accept rez (10th/sec)"
  /call set_data_map ${_debug} maEntropy stRNDRepeat                      40                            "${auxsw} randomize delay time to echo /repeat (10th/sec)"


  | /call set_data_map ${_debug} maEntropy stRNDVariance                    3                             "${auxsw} +/- variance for random %s"
  | /call set_data_map ${_debug} maEntropy swRNDEngage                      FALSE                         "${auxsw} randomize engage %"
  | /call set_data_map ${_debug} maEntropy swRNDEngagePet                   FALSE                         "${auxsw} randomize engage pet %"
  | /call set_data_map ${_debug} maEntropy swRNDEngageSwarm                 FALSE                         "${auxsw} randomize engage swarm %"
  | /call set_data_map ${_debug} maEntropy swRNDEngageBurn                  FALSE                         "${auxsw} randomize engage burn %"
  | /call set_data_map ${_debug} maEntropy stRNDRezTake                     0                            "${auxsw} randomize random delay to accept rez (10th/sec)"
  | /call set_data_map ${_debug} maEntropy swRNDRezTake                     FALSE                         "${auxsw} randomize random delay to accept rez"


  

  | dannet options
  /call set_data_map ${_debug} maEntropy stNetworkTimeout                 1                             "${auxnum} dannet timeout for network lag. ignore this if at all possible"
  /call set_data_map ${_debug} maEntropy stNetworkEvasive                 5000                          "${auxnum} dannet timeout client loss. set higher for laggy systems."
  /call set_data_map ${_debug} maEntropy stNetworkExpired                 30000                         "${auxnum} dannet timeout to delete a peer"
  /call set_data_map ${_debug} maEntropy stDanNetQueryDelay               10                            "${auxnum} delays for dannet query returns"
  /call set_data_map ${_debug} maEntropy swLocalEcho                      FALSE                         "${auxsw} dannet local echo"
  /call set_data_map ${_debug} maEntropy swCommandEcho                    FALSE                         "${auxsw} dannet local command echo"
  /call set_data_map ${_debug} maEntropy swFrontDelim                     TRUE                          "${auxsw} dannet lead from delimiters"
  /call set_data_map ${_debug} maEntropy swFullNames                      FALSE                         "${auxsw} dannet fullnames"
  /call set_data_map ${_debug} maEntropy stEntropyGroup_all               entropy                       "${auxna} dannet main entropy group"
  /call set_data_map ${_debug} maEntropy swEvasiveRefresh                 FALSE                         "${auxsw} refreshes observers if a client goes evasive"

  | join the main channel
  /djoin ${maEntropy.Find[stEntropyGroup_all].Value}
  
  | broadcast defaults
  /call set_data_map ${_debug} maEntropy stBroadcastHere                 group                         "${auxna} change the default broadcast for /here [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastEnter                group                         "${auxna} change the default broadcast for /enter [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastOn                   group                         "${auxna} change the default broadcast for /on [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastOff                  group                         "${auxna} change the default broadcast for /off [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastTake                 group                         "${auxna} change the default broadcast for /take [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastState                group                         "${auxna} change the default broadcast for /state [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastInvis                group                         "${auxna} change the default broadcast for /invis [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastCls                  group                         "${auxna} change the default broadcast for /cls [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastGetoverhere          group                         "${auxna} change the default broadcast for /getoverhere [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastGather               group                         "${auxna} change the default broadcast for /gather [group|raid|zone|all]."
  /call set_data_map ${_debug} maEntropy stBroadcastRepeat               group                         "${auxna} change the default broadcast for /repeat [group|raid|zone|all]."



  | GoM list
  /call set_data_list ${_debug} maChr lsGoM                               EMPTY                         "${auxli} things to use on a gift of mana aa proc"

  | ae count you act on
  /call set_data_map ${_debug} maChr stCountAE                            0                             "${auxnum} number of mobs in your area to even consider using ae skills"

  | fade
  /call set_data_map ${_debug} maChr stPctFade                            0                             "${auxnum} % of your healt to use class aa fade skill"

  | spire AA
  /call set_data_map ${_debug} maChr swSpireRaid                          FALSE                         "${auxsw} use while in group for burns"
  /call set_data_map ${_debug} maChr swSpireGroup                         FALSE                         "${auxsw} use while in a raid for burns"

  | intensity of the resolute AA
  /call set_data_map ${_debug} maChr swIntensityRaid                      FALSE                         "${auxsw} use while in a raid for burns"
  /call set_data_map ${_debug} maChr swIntensityGroup                     FALSE                         "${auxsw} use while in group for burns"
  /call set_data_map ${_debug} maChr stAAIntensity                        "Intensity of the Resolute"   "${auxna} Intensity of the Resolute AA"

  | can cast?
  /if (${lsClassCast.Contains[${Me.Class.ShortName}]}) {
    /call set_data_map ${_debug} maChr stPctManaRecovery                   0                            "${auxna} % to use whatever manna recovery things entropy can find on you"
  }




|** control: gtfo **|
  /invoke ${set_data_timer[FALSE, Check_GTFO, 6s]}
  /call set_data_list ${_debug} maGTFO lsLineGTFO                         EMPTY                         "${auxli} order line for GTFO stuff" gtfo
  /call set_data_map  ${_debug} maGTFO swGTFOActive                       FALSE                         "${auxsw} use GTFO logic" gtfo
  /call set_data_map  ${_debug} maGTFO stCountGTFODead                    0                             "${auxnum} number of group dead to activate" gtfo
  /call set_data_map  ${_debug} maGTFO stCountGTFOSpawns                  0                             "${auxnum} number of spawns in your env range to bail on your healer" gtfo




|** control: clickitem  **|
  /call set_data_list ${_debug} maItem lsLineItem                         EMPTY                         "${auxli} alternate cast order for clickitems" item




|** control: TTL  **|
  /call set_data_map ${_debug} maTTL swTTL                                FALSE                         "${auxsw} use ttl logic"




|** control: MiscDPS  **|
  /call set_data_list ${_debug} maMiscDPS lsLineMiscdps                   EMPTY                         "${auxli} alternate activation order for miscdps routine (engine 2)"




|** control: Agro  **|
  /call set_data_list ${_debug} maAgro lsLineAgro                         EMPTY                         "${auxli} alternate activation order for agro routine (engine 2/3)" agro
  /call set_data_map ${_debug} maAgro swAgro                              FALSE                         "${auxsw} use any agro skills you can (\a-wtaunt not included separate control under /agro\ax)" agro




|** control: sash  **|
  /invoke ${set_data_timer[${_debug}, Check_Sash, 6s]}
  /invoke ${set_data_timer[FALSE, Check_Proc_Sash, 5s]}
  /call set_data_map ${_debug} maSash swSashProc                          FALSE                         "${auxsw} enable sash for a procing weapon"               sash
  /call set_data_map ${_debug} maSash stSashProc                          FALSE                         "${auxna} name of sash with procing  weapon"              sash
  /call set_data_map ${_debug} maSash swSash                              FALSE                         "${auxsw} enable auto sash swapping based off conditions" sash
  /call set_data_map ${_debug} maSash stSashStart                         FALSE                         "${auxna} name of sash to start with"                     sash
  /call set_data_list ${_debug} maSash lsLineSash                         EMPTY                         "${auxli} alternate sash activation order"                sash

  | tank shield flash
  /if (${Select[${Me.Class.ShortName},PAL,WAR,SHD]}) {
    /call set_data_map ${_debug} maSash stPctShieldFlash                  0                             "${auxnum} % of HP to try to get shield flash running"    sash
    /call set_data_map ${_debug} maSash stCountShieldFlash                0                             "${auxnum} # of mobs to hit shield flash"                 sash
    | /call set_data_map ${_debug} maSash stSashShieldFlash                 FALSE                         "${auxna} name of sash to be used for Slash skills"       sash
  }


  /if (${maDebug.Find[init].Value}) {
    OUT \agset_initialize_map_character_environment\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE


|***
 * note: loads class epics
 * use: do i need to ssay it again?
 ***|
sub set_initialize_class_epic(bool _debug)
  DEBUG \atset_initialize_class_epic\ax()

  /sqlite query "${DBstatic}" epic${Me.Class.ShortName} SELECT * FROM epic WHERE env_var='${Me.Class.ShortName}';

  /declare epic1 string outer ${sqlite.Result[epic${Me.Class.ShortName} 1 name]}
  /declare epic15 string outer ${sqlite.Result[epic${Me.Class.ShortName} 2 name]}
  /declare epic2 string outer ${sqlite.Result[epic${Me.Class.ShortName} 3 name]}
  
/return



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
    /call set_data_map ${_debug} maBuff swBuff${_count} FALSE "${auxsw} buff ${_count} switch"          buff
    /call set_data_map ${_debug} maBuff stBuff${_count} FALSE "${auxna} buff ${_count} name"            buff
    /call set_data_map ${_debug} maBuff stBuff${_count}Alias FALSE "${auxna} buff ${_count} alias"      buff
    /call set_data_list ${_debug} maBuff lsBuff${_count}Tag EMPTY "${auxli} buff ${_count} tag list"    buff
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
  /invoke ${_tmpMap.Add[GTFO,gtfo]}
  /invoke ${_tmpMap.Add[Pull,pull]}
  /invoke ${_tmpMap.Add[Melee,melee]}
  /invoke ${_tmpMap.Add[deBuff,debuff]}
  /invoke ${_tmpMap.Add[Cure,cure]}

  /vardata _tmi _tmpMap.First.Clone
  /while (!${_tmi.IsEnd}) {
    /vardata _liC12 lsCount12.First.Clone
    /while (!${_liC12.IsEnd}) {
      /call set_data_map ${_debug} ma${_tmi.Key} sw${_tmi.Key}${_liC12.Value} FALSE "${auxsw} ${_tmi.Value} ${_liC12.Value} switch" ${_tmi.Value}
      /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC12.Value} FALSE "${auxna} ${_tmi.Value} ${_liC12.Value} name" ${_tmi.Value}
      /call set_data_list ${_debug} ma${_tmi.Key} ls${_tmi.Key}${_liC12.Value}Tag EMPTY "${auxli} ${_tmi.Value} ${_liC12.Value} tag list" ${_tmi.Value}
      /invoke ${_liC12.Advance}
    }

    /if (${maDebug.Find[init].Value}) {
      OUT \agset_initialize_map_12\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
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
  /invoke ${_tmpMap.Add[Defense,defense]}
  /invoke ${_tmpMap.Add[Item,item]}
  /invoke ${_tmpMap.Add[Agro,agro]}
  /invoke ${_tmpMap.Add[Song,song]}
  /invoke ${_tmpMap.Add[Burn,burn]}

  /vardata _tmi _tmpMap.First.Clone
  /while (!${_tmi.IsEnd}) {
    /vardata _liC20 lsCount20.First.Clone
    /while (!${_liC20.IsEnd}) {
      /call set_data_map ${_debug} ma${_tmi.Key} sw${_tmi.Key}${_liC20.Value} FALSE "${auxsw} ${_tmi.Value} ${_liC20.Value} switch" ${_tmi.Value}
      /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC20.Value} FALSE "${auxna} ${_tmi.Value} ${_liC20.Value} name" ${_tmi.Value}
      /call set_data_list ${_debug} ma${_tmi.Key} ls${_tmi.Key}${_liC20.Value}Tag EMPTY "${auxli} ${_tmi.Value} ${_liC20.Value} tag list" ${_tmi.Value}
      /invoke ${_liC20.Advance}
    }

    /if (${maDebug.Find[init].Value}) {
      OUT \agset_initialize_map_20\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
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
  /invoke ${_tmpMap.Add[Heal,heal]}

  /vardata _tmi _tmpMap.First.Clone
  /while (!${_tmi.IsEnd}) {
    /vardata _liC50 lsCount50.First.Clone
    /while (!${_liC50.IsEnd}) {
      /call set_data_map ${_debug} ma${_tmi.Key} sw${_tmi.Key}${_liC50.Value} FALSE "${auxsw} ${_tmi.Value} ${_liC50.Value} switch" ${_tmi.Value}
      /call set_data_map ${_debug} ma${_tmi.Key} st${_tmi.Key}${_liC50.Value} FALSE "${auxna} ${_tmi.Value} ${_liC50.Value} name" ${_tmi.Value}
      /call set_data_list ${_debug} ma${_tmi.Key} ls${_tmi.Key}${_liC50.Value}Tag EMPTY "${auxli} ${_tmi.Value} ${_liC50.Value} tag list" ${_tmi.Value}
      /invoke ${_liC50.Advance}
    }

    /if (${maDebug.Find[init].Value}) {
      OUT \agset_initialize_map_50\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
      /mqp
    }

    /invoke ${_tmi.Advance}
  }

/return TRUE



|***
 * note: maps for data that users will not change
 * use: don't fucking use it..
 ***|
sub set_initialize_static_map(bool _debug)
   DEBUG\atset_initialize_static_map\ax()

  /declare maStaticMaps map local
  /invoke ${maStaticMaps.Add[maShade,shade]}
  /invoke ${maStaticMaps.Add[maSQLError,sql_error]}
  /invoke ${maStaticMaps.Add[maManaRecovery,mana_recovery]}
  /invoke ${maStaticMaps.Add[maExpansion,env_xpac]}
  /invoke ${maStaticMaps.Add[maCure,cure]}
  /invoke ${maStaticMaps.Add[maHud,hud]}
  /invoke ${maStaticMaps.Add[maSash,sash]}
  /invoke ${maStaticMaps.Add[maChrWatch,chrwatch]}
  | /invoke ${maStaticMaps.Add[maRezID,rezid]}

  | leave this out. only for design
  | /invoke ${maStaticMaps.Add[maSPA,eq_spa]}

  /declare _count int local 0
  /declare _mi mapiterator local
  /vardata _mi maStaticMaps.First.Clone
  /while (!${_mi.IsEnd}) {
    /sqlite query "${DBstatic}" dummycount SELECT Count(*) as data FROM ${_mi.Value};
    /for _count 1 to ${sqlite.Result[dummycount 1 data]}
      /sqlite query "${DBstatic}" dummy SELECT * FROM ${_mi.Value} WHERE ROWID=${_count};
      /if (!${sql_check[${_debug}, dummy, set_initialize_static_map, "SELECT (${_mi.Value})"]}) /endmacro
      /call set_data ${_debug} "${_mi.Key}" "${sqlite.Result[dummy 1 env_var]}" map outer "${sqlite.Result[dummy 1 value]}"
      | DEBUG set_data ${_debug} "${_mi.Key}" "${sqlite.Result[dummy 1 env_var]}" map outer "${sqlite.Result[dummy 1 value]}"
    /next _count
    /invoke ${_mi.Advance}
  }

/return TRUE



|***
 * note: sets any aliases
 * use: /call set_alias [add|delete] DEBUG 
 ***|
sub set_alias(string _action, bool _debug)
  DEBUG \atset_alias\ax(\a-w${_action}\ax)

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
      /squelch /alias ${_maal.Key} ${_maal.Value}
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
 * note:
 * use: generate nav.sqlite
 ***|
sub sql_schema_nav(bool _debug)
  DEBUG \atsql_schema_nav\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  | get the data from the table
  /sqlite query "${DBnav}" buildnumber SELECT version FROM metadata WHERE name='build_number';
  /if (!${sql_check[${_debug}, buildnumber, sql_schema_nav, "SELECT (version)"]}) /endmacro
  /sqlite query "${DBnav}" schemanumber SELECT version FROM metadata WHERE name='schema_number';
  /if (!${sql_check[${_debug}, schemanumber, sql_schema_nav, "SELECT (version)"]}) /endmacro

  /declare _schemaMismatch bool local FALSE
  /declare _buildMismatch bool local FALSE

  | macro mismatch flag
  /if (${sqlite.Result[buildnumber 1 version].NotEqual[${maEnv.Find[build].Value}]}) {
    /varset _buildMismatch TRUE
  }

  | schema mismatch flag
  /if (${sqlite.Result[schemanumber 1 version].NotEqual[${maEnv.Find[schema].Value}]}) {
    /varset _schemaMismatch TRUE
  }

  | if the numbers match, we consider it valid and go away to dosoemthing else.
  /if (!${_schemaMismatch} && !${_buildMismatch}) {
    DEBUG ${sep}Macro and Schema builds match for nav database, bailing on this routine
    /sqlite clear schemanumber
    /sqlite clear buildnumber
    /return TRUE
  }

  /if (!${sqlite.Rows[schemanumber]}) {
    OUT \a-tSchema Build\ax${sep}\a-wnav\ax (\a-r${maEnv.Find[schema].Value}\ax)
    /while (TRUE) {
      /sqlite query "${DBnav}" build CREATE TABLE IF NOT EXISTS metadata(name, version);
      /if (!${sql_check[${_debug}, build, sql_schema_nav, "CREATE TABLE IF NOT EXISTS (metadata)"]}) /endmacro
      /sqlite query "${DBnav}" build INSERT OR IGNORE INTO metadata VALUES('build_number', '${maEnv.Find[build].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_nav, "INSERT INTO (metadata)"]}) /endmacro
      /sqlite query "${DBnav}" build INSERT OR IGNORE INTO metadata VALUES('schema_number', '${maEnv.Find[schema].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_nav, "INSERT INTO (metadata)"]}) /endmacro
      /sqlite query "${DBnav}" build CREATE TABLE IF NOT EXISTS waypoint(zone_shortname, nickname, locY, locX, locZ);
      /if (!${sql_check[${_debug}, build, sql_schema_nav, "CREATE TABLE IF (waypoint)"]}) /endmacro

      /if (${sqlite.Resultcode[build]} != 0) /delay 1
      /if (${sqlite.Resultcode[build]} == 0) /break
    }
    /sqlite clear schemanumber
    /sqlite clear build
    /return TRUE
  }

  | correct build mismatch
  /if (${_buildMismatch}) {
    /while (TRUE) {

      /sqlite query "${DBnav}" updatebuildnum UPDATE metadata SET version='${maEnv.Find[build].Value}' WHERE name='build_number';
      /if (!${sql_check[${_debug}, updatebuildnum, sql_schema_nav, "UPDATE (metadata)"]}) /endmacro
      /if (${sqlite.Resultcode[updatebuildnum]} == 0) /break
    }
    /sqlite clear updatebuildnum
  }

  | corect schema mismatch
  /if (${_schemaMismatch}) {
    /if (${sqlite.Status[schemanumber].NotEqual[success]}) {
      OUT \a-tSchema Update\ax${sep}\a-wnav\ax (\a-r${sqlite.Result[schemanumber 1 version]}\ax -> \a-r${maEnv.Find[schema].Value}\ax)
    } else {
      OUT \a-tSchema Update\ax${sep}\a-wnav\ax (\a-r${maEnv.Find[schema].Value}\ax)
    }
    /while (TRUE) {

      /sqlite query "${DBnav}" updateschemanum UPDATE metadata SET version='${maEnv.Find[schema].Value}' WHERE name='schema_number';
      /if (!${sql_check[${_debug}, updateschemanum, sql_schema_nav, "UPDATE (metadata)"]}) /endmacro

      | call schema update by version here!
      /call sql_schema_update ${_debug} nav

      /if (${sqlite.Resultcode[updateschemanum]} == 0) /break
    }
    /sqlite clear updateschemanum
  }

  | set pragma
  /sqlite query "${DBnav}" query_journal_DBnav PRAGMA journal_mode=WAL;

  /if (${maDebug.Find[init].Value}) {
    OUT \agsql_schema_nav\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note: drop a sql field
 * use: /call sql_drop TABLE FIELD [echo T/F]
 ***|
sub sql_drop(string _table, string _field, bool _echo)

  /declare _db string local FALSE
  /declare _dbtable string local FALSE

  | character DB
  /if (${_table.Equal[character]}) {
    /varset _db DBcharacter
    /varset _dbtable environment_${_table}

  | loot DB
  } else /if (${_table.Equal[loot]}) {
    /varset _db DBloot
    /varset _dbtable loot_control

  | zone DB
  } else /if (${_table.Equal[zone]}) {
    /varset _db DBzone
    /varset _dbtable zone_control

  | nav DB
  } else /if (${_table.Equal[nav]}) {
    /varset _db DBnav
    /varset _dbtable waypoint
    
  }

  | is the data there?
  | /echo --  sqlite query "${${_db}}" dummy SELECT * FROM ${_dbtable} WHERE env_var='${_field}';
  /sqlite query "${${_db}}" dummy SELECT * FROM ${_dbtable} WHERE env_var='${_field}';

  /if (${sqlite.Rows[dummy]}) {
    /sqlite query "${${_db}}" dummy DELETE FROM ${_dbtable} WHERE env_var='${_field}';
    /if (${_echo}) {
      OUT SQL DROP${sep}\a-w${_dbtable}${submenu}${_field}\ax ${dot} \a-w${sqlite.Status[dummy]}\ax
    }

  } else /if (!${sqlite.Rows[dummy]}) {
    /if (${_echo}) {
     OUT Not Found${sep}\a-w\a-w${_dbtable}${submenu}${_field}\ax\ax
    }
    
  }

/return TRUE



|***
 * note: sqlite query result validation
 * use: ${sql_check[DEBUG, QUERYNAME, DATABASECHECKINGNAME]}
 ***|
sub sql_check(bool _debug, string _query, string _subname, string _action)

  | results we accept at face value and move on
  /if (!${Select[${sqlite.Resultcode[${_query}]},0,1,6,5]} || ${_debug}) {
    /sqlite query "${DBstatic}" queryresult SELECT * FROM sql_error WHERE env_var='${sqlite.Resultcode[${_query}]}';
    /if (!${_debug}) {
      OUT \ap${_action}\ax${sep}${_subname} (\a-r${_query}\ax)${sep}\ay${sqlite.Resultcode[${_query}]}\ax${sep}\aw${sqlite.Result[queryresult 1 value]}\ax${sep}\a-w${sqlite.Result[queryresult 1 aux]}\ax
    } else {
      DEBUG ${sep}\ap${_action}\ax${sep}${_subname} (\a-r${_query}\ax)${sep}${sqlite.Rows[${_query}]}${sep}\ay${sqlite.Resultcode[${_query}]}\ax${sep}\aw${sqlite.Result[queryresult  1 value]}\ax${sep}\a-w${sqlite.Result[queryresult 1 aux]}\ax
      /if (${Select[${sqlite.Resultcode[${_query}]},0,1,6,5]}) {
        /return TRUE
      }
    }
  } else /if (${Select[${sqlite.Resultcode[${_query}]},0,1,6,5]}) {
    /return TRUE
  }

  OUT ${warning} Ending Entropy on SQLite error${sep}${sqlite.Resultcode[${_query}]}

/return FALSE



|***
 * note:
 * use: generate shared.sqlite
 ***|
sub sql_schema_shared(bool _debug, bool _force)
  DEBUG \atsql_schema_shared\ax()

  /if (${maDebug.Find[init].Value}) {
    /declare _boottimestart int local ${MacroQuest.Running}
  }

  | check the current schema/build versions
  /sqlite query "${DBshared}" buildnumber SELECT version FROM metadata WHERE name='build_number';
  /if (!${sql_check[${_debug}, buildnumber, sql_schema_shared, "SELECT (version)"]}) /endmacro
  /sqlite query "${DBshared}" schemanumber SELECT version FROM metadata WHERE name='schema_number';
  /if (!${sql_check[${_debug}, schemanumber, sql_schema_shared, "SELECT (version)"]}) /endmacro

  /declare _schemaMismatch bool local FALSE
  /declare _buildMismatch bool local FALSE

  | macro mismatch flag
  /if (${sqlite.Result[buildnumber 1 version].NotEqual[${maEnv.Find[build].Value}]}) {
    /varset _buildMismatch TRUE
  }

  | schema mismatch flag
  /if (${sqlite.Result[schemanumber 1 version].NotEqual[${maEnv.Find[schema].Value}]}) {
    /varset _schemaMismatch TRUE
  }

  | if the numbers match, we consider it valid and go away to dosoemthing else.
  /if (!${_schemaMismatch} && !${_buildMismatch} && !${_force}) {
    DEBUG ${sep}Macro and Schema builds match for shared database, bailing on this routine
    /sqlite clear schemanumber
    /sqlite clear buildnumber
    /return TRUE
  }

  /if (!${sqlite.Rows[schemanumber]} || ${_force}) {
    OUT \a-tSchema Build\ax${sep}\a-wshared\ax (\a-r${maEnv.Find[schema].Value}\ax)
    /while (TRUE) {

      | -------------------- metadata build
      | create shared build table
      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS metadata(name, version);
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (metadata)"]}) /endmacro
      /sqlite query "${DBshared}" build INSERT OR IGNORE INTO metadata VALUES('build_number', '${maEnv.Find[build].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "INSERT INTO (metadata)"]}) /endmacro
      /sqlite query "${DBshared}" build INSERT OR IGNORE INTO metadata VALUES('schema_number', '${maEnv.Find[schema].Value}');
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "INSERT INTO (metadata)"]}) /endmacro

      | -------------------- crews
      | create the characters
      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS characters(id INTEGER PRIMARY KEY autoincrement, charactername TEXT NOT NULL, server TEXT NOT NULL);
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (characters)"]}) /endmacro

      | create the rolls.. move to entropy.sqlite???
      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS roles(name TEXT NOT NULL, UNIQUE (name));
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (roles)"]}) /endmacro
      /call data_schema_roles

      | create the crews table
      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS crews(id INTEGER PRIMARY KEY, crewname TEXT NOT NULL);
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (crews)"]}) /endmacro
      | create the crewmembers table
      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS crewmembers(id INTEGER PRIMARY KEY autoincrement, crewid INTEGER, characterid INTEGER, FOREIGN KEY (crewid) REFERENCES crews(id), FOREIGN KEY (characterid) REFERENCES characters(id), UNIQUE (crewid,characterid));
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (crewmembers)"]}) /endmacro
      | create crewmemberroles
      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS crewmemberroles(id INTEGER PRIMARY KEY autoincrement, crewmemberid INTEGER, role TEXT NOT NULL, FOREIGN KEY (crewmemberid) REFERENCES character(id), FOREIGN KEY (role) REFERENCES roles(name), UNIQUE (crewmemberid, role));
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (crewmemberroles)"]}) /endmacro


      | -------------------- tags n'stuff
      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS tag(id INTEGER PRIMARY KEY, name TEXT, created_by INTEGER, UNIQUE (name));
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (tag)"]}) /endmacro
      /call data_schema_tag

      /sqlite query "${DBshared}" build  CREATE TABLE IF NOT EXISTS tag_group(id INTEGER PRIMARY KEY, grouping TEXT, UNIQUE (grouping));
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (tag_group)"]}) /endmacro
      /call data_schema_tag_group

      /sqlite query "${DBshared}" build CREATE TABLE IF NOT EXISTS tag_validate(grouping INTEGER, tag INTEGER, created_by INTEGER, eval TEXT, aux TEXT, UNIQUE(tag, grouping), FOREIGN KEY (tag) REFERENCES tag(id), FOREIGN KEY (grouping) REFERENCES tag_group(id));
      /if (!${sql_check[${_debug}, build, sql_schema_shared, "CREATE TABLE IF NOT EXISTS (tag_validate)"]}) /endmacro
      /call data_schema_tag_validation

      /if (${sqlite.Resultcode[build]} != 0) /delay 1
      /if (${sqlite.Resultcode[build]} == 0) /break
    }
    /sqlite clear schemanumber
    /sqlite clear build
    /return TRUE
  }

  /if (${_buildMismatch}) {
    DEBUG ${sep}build mismatch
    /while (TRUE) {
      /sqlite query "${DBshared}" updatebuildnum UPDATE metadata SET version='${maEnv.Find[build].Value}' WHERE name='build_number';
      /if (!${sql_check[${_debug}, updatebuildnum, sql_schema_shared, "UPDATE (value)"]}) /endmacro
      /if (${sqlite.Resultcode[updatebuildnum]} != 0) /delay 1
      /if (${sqlite.Resultcode[updatebuildnum]} == 0) /break
    }
    /sqlite clear updatebuildnum
  }

  /if (${_schemaMismatch} || ${_force}) {
    /if (${sqlite.Status[schemanumber].NotEqual[success]}) {
      OUT \a-tSchema Update\ax${sep}\a-wshared\ax (\a-r${sqlite.Result[schemanumber 1 version]}\ax -> \a-r${maEnv.Find[schema].Value}\ax)
    } else {
      OUT \a-tSchema Update\ax${sep}\a-wshared\ax (\a-r${maEnv.Find[schema].Value}\ax)
    }
    /while (TRUE) {
      /sqlite query "${DBshared}" updateschemanum UPDATE metadata SET version='${maEnv.Find[schema].Value}' WHERE name='schema_number';
      /if (!${sql_check[${_debug}, updateschemanum, sql_schema_shared, "UPDATE (value)"]}) /endmacro

      | call schema update by version here!
      | /call data_schema_bind_name
      /call data_schema_tag
      /call data_schema_tag_group
      /call data_schema_tag_validation
      /call sql_schema_update ${_debug} shared
      

      /if (${sqlite.Resultcode[updateschemanum]} != 0) /delay 1
      /if (${sqlite.Resultcode[updateschemanum]} == 0) /break
    }
    /sqlite clear updateschemanum
  }

  | set pragma
  /sqlite query "${DBshared}" query_journal_DBshared PRAGMA journal_mode=WAL;

  /if (${maDebug.Find[init].Value}) {
    OUT \agsql_schema_shared\ax${sep}\a-w${Math.Calc[((${MacroQuest.Running}-${_boottimestart}) / 10) / 60]}s\ax
    /mqp
  }

/return TRUE



|***
 * note:
 * use: generate data for schema_roles
 ***|
sub data_schema_roles()
  /sqlite query "${DBshared}" build INSERT OR IGNORE INTO roles(name) VALUES('Leader');
  /sqlite query "${DBshared}" build INSERT OR IGNORE INTO roles(name) VALUES('MainTank');
  /sqlite query "${DBshared}" build INSERT OR IGNORE INTO roles(name) VALUES('MainAssist');
  /sqlite query "${DBshared}" build INSERT OR IGNORE INTO roles(name) VALUES('MasterLooter');
  /sqlite query "${DBshared}" build INSERT OR IGNORE INTO roles(name) VALUES('MarkNpc');
  /sqlite query "${DBshared}" build INSERT OR IGNORE INTO roles(name) VALUES('Puller');
/return TRUE



|***
 * note:
 * use: generate data for schema_tag_group
 ***|
sub data_schema_tag_group()
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('1', 'heal');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('2', 'nuke');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('3', 'dot');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('4', 'burn');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('5', 'song');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('6', 'defense');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('7', 'buff');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('8', 'miscdps');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('9', 'item');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('10', 'pull');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('11', 'melee');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('12', 'debuff');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('13', 'cure');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('14', 'sash');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag_group(id, grouping) VALUES('15', 'agro');

/return TRUE



|***
 * note:
 * use: generate data for schema_tag
 ***|
sub data_schema_tag()
  | /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('X', '', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('1', 'WAR', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('2', 'PAL', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('3', 'SHD', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('4', 'BRD', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('5', 'RNG', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('6', 'BST', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('7', 'BER', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('8', 'MNK', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('9', 'ROG', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('10', 'WIZ', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('11', 'MAG', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('12', 'ENC', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('13', 'NEC', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('14', 'CLR', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('15', 'DRU', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('16', 'SHM', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('17', 'assist', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('18', 'nocombat', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('19', 'incombat', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('20', 'noactivedisc', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('21', 'outdoor', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('22', 'drop', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('23', 'puller', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('24', 'agro', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('25', 'noagro', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('26', 'noraid', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('27', 'inraid', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('28', 'burn', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('29', 'swarm', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('30', 'ingroup', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('31', 'nosafezone', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('32', 'autoinventory', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('33', 'epic', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('34', 'aliasonly', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('35', 'target', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('36', 'targetundead', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('37', 'targethumanoid', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('38', 'targetplant', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('39', 'delay1s', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('40', 'nogroup', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('41', 'dropdisc', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('42', 'dropinvis', '1');

  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('43', 'single', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('44', 'group', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('45', 'hot', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('46', 'grouphot', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('47', 'tot', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('48', 'minion', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('49', 'self', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('50', 'splash', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('51', 'complete', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('52', 'promise', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('53', 'surge', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('56', 'crew', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('58', 'now', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('59', 'ps', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('60', 'aura', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('61', 'raid', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('62', 'summonrange', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('63', 'summonpoison', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('64', 'summonweapons', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('65', 'summonitem', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('66', 'summonaxe', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('67', 'outofwater', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('1005', 'pulseooc', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('1004', 'pulserest', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('1003', 'pulseincombat', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('1001', 'totem', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('1000', 'twincast', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('999', 'pbaenuke', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('998', 'temp', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('997', 'interventionpull', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('996', 'mez', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('995', 'eradicate', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('994', 'tash', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('993', 'malo', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('992', 'snare', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('991', 'slow', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('990', 'mezae', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('989', 'cripple', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('987', 'delaycooldown', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('986', 'inmygroup', '1');


  | cure stuff
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('979', 'corruption', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('978', 'curse', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('977', 'disease', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('976', 'poison', '1');

  | sash
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('969', 'mainsash', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('968', 'defensive', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('967', 'proconme', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('966', 'procontarget', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('965', 'macstart', '1');

  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('960', 'ignoremovement', '1');

  | agro
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('959', 'ae2', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('958', 'ae4', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('957', 'ae6', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('956', 'ae8', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('955', 'badtot', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('954', 'nochanter', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('953', 'aeon', '1');
  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('952', 'agrohold', '1');


  /sqlite query "${DBshared}" dummy INSERT OR IGNORE INTO tag(id, name, created_by) VALUES('951', 'memspell', '1');


/return TRUE



|***
 * note:
 * use: pregeerated validations
 ***|
sub data_schema_tag_validation

  | heals
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '43', '1', 'CONTROL::MARKER', 'marker single target heals');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '44', '1', 'CONTROL::MARKER', 'marker no target group heals');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '45', '1', 'CONTROL::MARKER', 'marker single target heal over time');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '46', '1', 'CONTROL::MARKER', 'marker no target group heal over time');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '47', '1', 'CONTROL::MARKER', 'marker /heal tot single target of target heal');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '48', '1', 'CONTROL::MARKER', 'marker single target minion heal');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '49', '1', 'CONTROL::MARKER', 'marker /heal self only single heal');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '50', '1', 'CONTROL::MARKER', 'marker splash heal [PAL|CLR]');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '51', '1', 'CONTROL::MARKER', 'marker complete heal [CLR]');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '52', '1', 'CONTROL::MARKER', 'marker promised heal (stupid things)');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '53', '1', 'CONTROL::MARKER', 'marker surge heals [SHM]');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '1', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[WAR]}]}', 'heal is for a warrior');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '2', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[PAL]}]}', 'heal is for a paladin');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '3', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[SHD]}]}', 'heal is for a snowflake');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '4', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[BRD]}]}', 'heal is for a bard');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '5', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[RNG]}]}', 'heal is for a ranger');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '6', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[BST]}]}', 'heal is for a beastlord');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '7', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[BER]}]}', 'heal is for a berserker');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '8', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[MNK]}]}', 'heal is for a monk');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '9', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[ROG]}]}', 'heal is for a rogue');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '10', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[WIZ]}]}', 'heal is for a wizard');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '11', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[MAG]}]}', 'heal is for a magician');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '12', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[ENC]}]}', 'heal is for a enchanter');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '13', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[NEC]}]}', 'heal is for a necromancer');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '14', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[CLR]}]}', 'heal is for a cleric');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '15', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[DRU]}]}', 'heal is for a druid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '16', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[SHM]}]}', 'heal is for a shaman');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '18', '1', '${Parse[0,!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'cannot be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '19', '1', '${Parse[0,${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'must be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '20', '1', '${Parse[0,!${Me.ActiveDisc.ID}]}', 'cannot have an active disc');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '21', '1', '${Parse[0,!${Zone.Indoor}]}', 'must be outdoor');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '23', '1', '${Parse[0,${maPull.Find[swPull].Value}]}', 'must be the puller');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '24', '1', '${Parse[0,${maAgro.Find[swAgro].Value}]}', 'must have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '25', '1', '${Parse[0,!${maAgro.Find[swAgro].Value}]}', 'cannot have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '26', '1', '${Parse[0,!${Raid.Members}]}', 'not in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '27', '1', '${Parse[0,${Raid.Members}]}', 'in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '28', '1', '${Parse[0,${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '29', '1', '${Parse[0,${maMinion.Find[swSwarm].Value}]}', 'use with swarm pets');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '30', '1', '${Parse[0,${Group.Members}]}', 'must be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '31', '1', '${Parse[0,!${maEnv.Find[swZoneSafe].Value}]}', 'cannot be used in a safe zone');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '35', '1', '${Parse[0,${Target.ID}]}', 'you must have a target');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '36', '1', '${Parse[0,${Target.Body.Name.Equal[undead]}]}', 'target must be undead');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '37', '1', '${Parse[0,${Target.Body.Name.Equal[humanoid]}]}', 'target must be humanoid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '38', '1', '${Parse[0,${Target.Body.Name.Equal[plant]}]}', 'target must be a plant');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '34', '1', 'ACTION::aliasonly', 'skip instances, use this for alias only');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '39', '1', 'ACTION::delay1s', 'causes a 1 second pause');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '40', '1', '${Parse[0,!${Group.Members}]}', 'cannot be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '41', '1', 'ACTION::dropdisc', 'drops any running active discs');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '42', '1', 'ACTION::dropinvis', 'drops invis and sos');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '67', '1', '${Parse[0,!${Me.FeetWet}]}', 'must be out of water');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '17', '1', '${Parse[0,${Select[${_name},${Group.MainAssist.DisplayName},${maCC.Find[stManualAssist1].Value},${maCC.Find[stManualAssist2].Value}]}]}', 'my assist');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '997', '1', 'CONTROL::MARKER', 'intervention pulling for clerics and shaman');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '987', '1', 'ACTION::delaycooldown', 'delays until gem cooldown is clear');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '986', '1', '${Parse[0,${Group.Member[${_name}].Index}]}', 'target must be in my group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '960', '1', 'CONTROL::MARKER', 'marker force ignore movement checks');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('1', '951', '1', 'ACTION::memspell', 'force meming of spell into buff slot if not on bar');

  | nuke
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '18', '1', '${Parse[0,!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'cannot be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '19', '1', '${Parse[0,${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'must be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '20', '1', '${Parse[0,!${Me.ActiveDisc.ID}]}', 'cannot have an active disc');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '21', '1', '${Parse[0,!${Zone.Indoor}]}', 'must be outdoor');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '23', '1', '${Parse[0,${maPull.Find[swPull].Value}]}', 'must be the puller');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '24', '1', '${Parse[0,${maAgro.Find[swAgro].Value}]}', 'must have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '25', '1', '${Parse[0,!${maAgro.Find[swAgro].Value}]}', 'cannot have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '26', '1', '${Parse[0,!${Raid.Members}]}', 'not in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '27', '1', '${Parse[0,${Raid.Members}]}', 'in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '28', '1', '${Parse[0,${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '29', '1', '${Parse[0,${maMinion.Find[swSwarm].Value}]}', 'use with swarm pets');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '30', '1', '${Parse[0,${Group.Members}]}', 'must be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '31', '1', '${Parse[0,!${maEnv.Find[swZoneSafe].Value}]}', 'cannot be used in a safe zone');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '35', '1', '${Parse[0,${Target.ID}]}', 'you must have a target');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '36', '1', '${Parse[0,${Target.Body.Name.Equal[undead]}]}', 'target must be undead');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '37', '1', '${Parse[0,${Target.Body.Name.Equal[humanoid]}]}', 'target must be humanoid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '38', '1', '${Parse[0,${Target.Body.Name.Equal[plant]}]}', 'target must be a plant');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '34', '1', 'ACTION::aliasonly', 'skip instances, use this for alias only');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '39', '1', 'ACTION::delay1s', 'causes a 1 second pause');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '40', '1', '${Parse[0,!${Group.Members}]}', 'cannot be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '41', '1', 'ACTION::dropdisc', 'drops any running active discs');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '42', '1', 'ACTION::dropinvis', 'drops invis and sos');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '67', '1', '${Parse[0,!${Me.FeetWet}]}', 'must be out of water');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '1000', '1', 'CONTROL::MARKER', 'marker twincast nukes');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '1001', '1', 'CONTROL::MARKER', 'marker for totem type cast results');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '999', '1', 'CONTROL::MARKER', 'marker for point blank ae nukes');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '987', '1', 'ACTION::delaycooldown', 'delays until gem cooldown is clear');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('2', '951', '1', 'ACTION::memspell', 'force meming of spell into buff slot if not on bar');

  | dot
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '18', '1', '${Parse[0,!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'cannot be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '19', '1', '${Parse[0,${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'must be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '20', '1', '${Parse[0,!${Me.ActiveDisc.ID}]}', 'cannot have an active disc');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '21', '1', '${Parse[0,!${Zone.Indoor}]}', 'must be outdoor');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '23', '1', '${Parse[0,${maPull.Find[swPull].Value}]}', 'must be the puller');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '24', '1', '${Parse[0,${maAgro.Find[swAgro].Value}]}', 'must have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '25', '1', '${Parse[0,!${maAgro.Find[swAgro].Value}]}', 'cannot have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '26', '1', '${Parse[0,!${Raid.Members}]}', 'not in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '27', '1', '${Parse[0,${Raid.Members}]}', 'in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '28', '1', '${Parse[0,!${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '29', '1', '${Parse[0,${maMinion.Find[swSwarm].Value}]}', 'use with swarm pets');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '30', '1', '${Parse[0,${Group.Members}]}', 'must be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '31', '1', '${Parse[0,!${maEnv.Find[swZoneSafe].Value}]}', 'cannot be used in a safe zone');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '35', '1', '${Parse[0,${Target.ID}]}', 'you must have a target');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '36', '1', '${Parse[0,${Target.Body.Name.Equal[undead]}]}', 'target must be undead');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '37', '1', '${Parse[0,${Target.Body.Name.Equal[humanoid]}]}', 'target must be humanoid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '38', '1', '${Parse[0,${Target.Body.Name.Equal[plant]}]}', 'target must be a plant');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '34', '1', 'ACTION::aliasonly', 'skip instances, use this for alias only');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '39', '1', 'ACTION::delay1s', 'causes a 1 second pause');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '40', '1', '${Parse[0,!${Group.Members}]}', 'cannot be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '41', '1', 'ACTION::dropdisc', 'drops any running active discs');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '42', '1', 'ACTION::dropinvis', 'drops invis and sos');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '67', '1', '${Parse[0,!${Me.FeetWet}]}', 'must be out of water');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '987', '1', 'ACTION::delaycooldown', 'delays until gem cooldown is clear');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('3', '951', '1', 'ACTION::memspell', 'force meming of spell into buff slot if not on bar');

  | song
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '18', '1', '${Parse[0,!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'cannot be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '19', '1', '${Parse[0,${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'must be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '20', '1', '${Parse[0,!${Me.ActiveDisc.ID}]}', 'cannot have an active disc');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '21', '1', '${Parse[0,!${Zone.Indoor}]}', 'must be outdoor');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '23', '1', '${Parse[0,${maPull.Find[swPull].Value}]}', 'must be the puller');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '24', '1', '${Parse[0,${maAgro.Find[swAgro].Value}]}', 'must have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '25', '1', '${Parse[0,!${maAgro.Find[swAgro].Value}]}', 'cannot have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '26', '1', '${Parse[0,!${Raid.Members}]}', 'not in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '27', '1', '${Parse[0,${Raid.Members}]}', 'in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '28', '1', '${Parse[0,${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '29', '1', '${Parse[0,${maMinion.Find[swSwarm].Value}]}', 'use with swarm pets');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '30', '1', '${Parse[0,${Group.Members}]}', 'must be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '31', '1', '${Parse[0,!${maEnv.Find[swZoneSafe].Value}]}', 'cannot be used in a safe zone');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '35', '1', '${Parse[0,${Target.ID}]}', 'you must have a target');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '36', '1', '${Parse[0,${Target.Body.Name.Equal[undead]}]}', 'target must be undead');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '37', '1', '${Parse[0,${Target.Body.Name.Equal[humanoid]}]}', 'target must be humanoid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '38', '1', '${Parse[0,${Target.Body.Name.Equal[plant]}]}', 'target must be a plant');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '67', '1', '${Parse[0,!${Me.FeetWet}]}', 'must be out of water');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '34', '1', 'ACTION::aliasonly', 'skip instances, use this for alias only');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '39', '1', 'ACTION::delay1s', 'causes a 1 second pause');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '40', '1', '${Parse[0,!${Group.Members}]}', 'cannot be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '41', '1', 'ACTION::dropdisc', 'drops any running active discs');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '42', '1', 'ACTION::dropinvis', 'drops invis and sos');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '1005', '1', 'CONTROL::MARKER', 'marker for songs to pulse out of combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '1003', '1', 'CONTROL::MARKER', 'marker for songs to pulse while in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '1004', '1', 'CONTROL::MARKER', 'marker for songs to pulse while resting');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('5', '951', '1', 'ACTION::memspell', 'force meming of spell into buff slot if not on bar');

  | defense
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '18', '1', '${Parse[0,!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'cannot be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '19', '1', '${Parse[0,${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'must be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '20', '1', '${Parse[0,!${Me.ActiveDisc.ID}]}', 'cannot have an active disc');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '21', '1', '${Parse[0,!${Zone.Indoor}]}', 'must be outdoor');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '23', '1', '${Parse[0,${maPull.Find[swPull].Value}]}', 'must be the puller');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '24', '1', '${Parse[0,${maAgro.Find[swAgro].Value}]}', 'must have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '25', '1', '${Parse[0,!${maAgro.Find[swAgro].Value}]}', 'cannot have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '26', '1', '${Parse[0,!${Raid.Members}]}', 'not in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '27', '1', '${Parse[0,${Raid.Members}]}', 'in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '28', '1', '${Parse[0,${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '29', '1', '${Parse[0,${maMinion.Find[swSwarm].Value}]}', 'use with swarm pets');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '30', '1', '${Parse[0,${Group.Members}]}', 'must be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '31', '1', '${Parse[0,!${maEnv.Find[swZoneSafe].Value}]}', 'cannot be used in a safe zone');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '35', '1', '${Parse[0,${Target.ID}]}', 'you must have a target');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '36', '1', '${Parse[0,${Target.Body.Name.Equal[undead]}]}', 'target must be undead');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '37', '1', '${Parse[0,${Target.Body.Name.Equal[humanoid]}]}', 'target must be humanoid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '38', '1', '${Parse[0,${Target.Body.Name.Equal[plant]}]}', 'target must be a plant');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '34', '1', 'ACTION::aliasonly', 'skip instances, use this for alias only');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '39', '1', 'ACTION::delay1s', 'causes a 1 second pause');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '40', '1', '${Parse[0,!${Group.Members}]}', 'cannot be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '41', '1', 'ACTION::dropdisc', 'drops any running active discs');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '42', '1', 'ACTION::dropinvis', 'drops invis and sos');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '67', '1', '${Parse[0,!${Me.FeetWet}]}', 'must be out of water');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('6', '987', '1', 'ACTION::delaycooldown', 'delays until gem cooldown is clear');

  | buffs
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '49', '1', 'CONTROL::MARKER', 'marker /buff self');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '48', '1', 'CONTROL::MARKER', 'marker /buff minion');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '56', '1', 'CONTROL::MARKER', 'marker /buff crew (dannet peoples)');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '44', '1', 'CONTROL::MARKER', 'marker /buff group (not dannet, outsiders and mercs)');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '58', '1', 'CONTROL::MARKER', 'marker /buff now 24/7 buffs, always have them');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '59', '1', 'CONTROL::MARKER', 'marker /buff ps (power source)');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '60', '1', 'CONTROL::MARKER', 'marker /buff aura');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '61', '1', 'CONTROL::MARKER', 'marker /buff raid (not dannet)');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '62', '1', 'CONTROL::MARKER', 'marker ranged summoning, like arrows, or daggers');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '63', '1', 'CONTROL::MARKER', 'marker pants poison summoning for rogues');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '64', '1', 'CONTROL::MARKER', 'marker weapon summoning for mages');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '65', '1', 'CONTROL::MARKER', 'marker item summoning from spells');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '66', '1', 'CONTROL::MARKER', 'marker axe summoning for zerkers');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '1', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[WAR]}]}', 'buff is for a warrior');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '2', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[PAL]}]}', 'buff is for a paladin');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '3', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[SHD]}]}', 'buff is for a snowflake');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '4', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[BRD]}]}', 'buff is for a bard');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '5', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[RNG]}]}', 'buff is for a ranger');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '6', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[BST]}]}', 'buff is for a beastlord');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '7', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[BER]}]}', 'buff is for a berserker');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '8', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[MNK]}]}', 'buff is for a monk');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '9', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[ROG]}]}', 'buff is for a rogue');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '10', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[WIZ]}]}', 'buff is for a wizard');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '11', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[MAG]}]}', 'buff is for a magician');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '12', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[ENC]}]}', 'buff is for a enchanter');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '13', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[NEC]}]}', 'buff is for a necromancer');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '14', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[CLR]}]}', 'buff is for a cleric');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '15', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[DRU]}]}', 'buff is for a druid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '16', '1', '${Parse[0,${Spawn[${_name}].Class.ShortName.Equal[SHM]}]}', 'buff is for a shaman');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '18', '1', '${Parse[0,!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'cannot be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '19', '1', '${Parse[0,${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'must be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '20', '1', '${Parse[0,!${Me.ActiveDisc.ID}]}', 'cannot have an active disc');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '21', '1', '${Parse[0,!${Zone.Indoor}]}', 'must be outdoor');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '23', '1', '${Parse[0,${maPull.Find[swPull].Value}]}', 'must be the puller');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '24', '1', '${Parse[0,${maAgro.Find[swAgro].Value}]}', 'must have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '25', '1', '${Parse[0,!${maAgro.Find[swAgro].Value}]}', 'cannot have agro enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '26', '1', '${Parse[0,!${Raid.Members}]}', 'not in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '27', '1', '${Parse[0,${Raid.Members}]}', 'in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '28', '1', '${Parse[0,${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '29', '1', '${Parse[0,${maMinion.Find[swSwarm].Value}]}', 'use with swarm pets');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '30', '1', '${Parse[0,${Group.Members}]}', 'must be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '31', '1', '${Parse[0,!${maEnv.Find[swZoneSafe].Value}]}', 'cannot be used in a safe zone');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '35', '1', '${Parse[0,${Target.ID}]}', 'you must have a target');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '36', '1', '${Parse[0,${Target.Body.Name.Equal[undead]}]}', 'target must be undead');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '37', '1', '${Parse[0,${Target.Body.Name.Equal[humanoid]}]}', 'target must be humanoid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '38', '1', '${Parse[0,${Target.Body.Name.Equal[plant]}]}', 'target must be a plant');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '22', '1', 'ACTION::drop', 'removes illusion and mount buffs');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '32', '1', 'ACTION::autoinventory', 'forces your hands emptied');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '33', '1', 'SUB::tag_epic', 'fires your epic');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '34', '1', 'ACTION::aliasonly', 'skip instances, use this for alias only');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '39', '1', 'ACTION::delay1s', 'causes a 1 second pause');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '40', '1', '${Parse[0,!${Group.Members}]}', 'cannot be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '41', '1', 'ACTION::dropdisc', 'drops any running active discs');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '42', '1', 'ACTION::dropinvis', 'drops invis and sos');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '67', '1', '${Parse[0,!${Me.FeetWet}]}', 'must be out of water');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '17', '1', '${Parse[0,${Select[${_name},${Group.MainAssist.DisplayName},${maCC.Find[stManualAssist1].Value},${maCC.Find[stManualAssist2].Value}]}]}', 'my assist');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '998', '1', 'CONTROL::MARKER', 'marker /buff temp');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '987', '1', 'ACTION::delaycooldown', 'delays until gem cooldown is clear');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('7', '986', '1', '${Parse[0,${Group.Member[${_name}].Index}]}', 'target must be in my group');

  | pull
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('10', '960', '1', 'CONTROL::MARKER', 'marker force ignore movement checks');

  | debuff
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '996', '1', 'CONTROL::MARKER', 'marker /debuff mez');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '995', '1', 'CONTROL::MARKER', 'marker /debuff eradicate');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '994', '1', 'CONTROL::MARKER', 'marker /debuff tash');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '993', '1', 'CONTROL::MARKER', 'marker /debuff malo');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '992', '1', 'CONTROL::MARKER', 'marker /debuff snare');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '991', '1', 'CONTROL::MARKER', 'marker /debuff slow');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '990', '1', 'CONTROL::MARKER', 'marker /debuff mez ae');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '989', '1', 'CONTROL::MARKER', 'marker /debuff cripple');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('12', '987', '1', 'ACTION::delaycooldown', 'delays until gem cooldown is clear');

  | cure
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('13', '979', '1', 'CONTROL::MARKER', 'marker /cure corruption');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('13', '978', '1', 'CONTROL::MARKER', 'marker /cure curse');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('13', '977', '1', 'CONTROL::MARKER', 'marker /cure disease');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('13', '976', '1', 'CONTROL::MARKER', 'marker /cure poison');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('13', '987', '1', 'ACTION::delaycooldown', 'delays until gem cooldown is clear');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('13', '986', '1', '${Parse[0,${Group.Member[${_name}].Index}]}', 'target must be in my group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('13', '951', '1', 'ACTION::memspell', 'force meming of spell into buff slot if not on bar');

  | sash  
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('14', '969', '1', 'CONTROL::MARKER', 'marker main sash');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('14', '968', '1', 'CONTROL::MARKER', 'marker defensive sash');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('14', '967', '1', 'CONTROL::MARKER', 'marker proc check on me?');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('14', '966', '1', 'CONTROL::MARKER', 'marker proc check on the target?');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('14', '28', '1', '${Parse[0,${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met'); 
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('14', '965', '1', 'CONTROL::MARKER', 'marker load this sash on mac start');
 
  | agro
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '23', '1', '${Parse[0,${maPull.Find[swPull].Value}]}', 'must be the puller');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '26', '1', '${Parse[0,!${Raid.Members}]}', 'not in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '27', '1', '${Parse[0,${Raid.Members}]}', 'in a raid');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '28', '1', '${Parse[0,${maBurn.Find[swBurnConditionMet].Value}]}', 'burn conditions must be met');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '29', '1', '${Parse[0,${maMinion.Find[swSwarm].Value}]}', 'use with swarm pets');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '30', '1', '${Parse[0,${Group.Members}]}', 'must be in a group');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '960', '1', 'CONTROL::MARKER', 'marker force ignore movement checks');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '18', '1', '${Parse[0,!${Select[${Me.CombatState},combat]} && !${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'cannot be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '19', '1', '${Parse[0,${Select[${Me.CombatState},combat]} || ${SpawnCount[npc radius ${maEnv.Find[stEnvSafeRadius].Value} playerstate 4]}]}', 'must be in combat');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '959', '1', '${Parse[0,${SpawnCount[npc radius ${maEnv.Find[stEnvRadius].Value} playerstate 4]}]} >= 2', 'spawncount >=2');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '958', '1', '${Parse[0,${SpawnCount[npc radius ${maEnv.Find[stEnvRadius].Value} playerstate 4]}]} >= 4', 'spawncount >=4');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '957', '1', '${Parse[0,${SpawnCount[npc radius ${maEnv.Find[stEnvRadius].Value} playerstate 4]}]} >= 6', 'spawncount >=6');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '956', '1', '${Parse[0,${SpawnCount[npc radius ${maEnv.Find[stEnvRadius].Value} playerstate 4]}]} >= 8', 'spawncount >=8');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '955', '1', '${Parse[0,${Me.TargetOfTarget.ID} != ${Me.ID}]}', 'target of target is not me');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '954', '1', '${Parse[0,!${SpawnCount[pc enchanter radius ${maEnv.Find[stEnvRadius].Value}]}]}', 'no  chanters around');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '953', '1', '${Parse[0,${maChr.Find[stCountAE].Value}]} && ${Parse[0,${maChr.Find[stCountAE].Value} > 0]}', 'ae is enabled');
  /sqlite query "${DBshared}" dummy INSERT OR REPLACE INTO tag_validate(grouping, tag, created_by, eval, aux) VALUES('15', '952', '1', '${Parse[0,${Me.PctAggro} < ${maAgro.Find[stPctAgroHold].Value}]}', 'target agro below pctagrohold setting');
 

/return TRUE












|***
 * note: schema data updates
 * use: called on every schema increment
 ***|
sub sql_schema_update(bool _debug, string _type)

  /if (${sqlite.Status[schemanumber].NotEqual[success]}) {
    /return FALSE
  }

  | character database
  /if (${_type.Equal[character]}) {

    /if (${sqlite.Result[schemanumber 1 version]} >= 1.01) {
      | drop build table character
      /sqlite query "${DBcharacter}" dummy DROP TABLE IF EXISTS build
      | drop old comm table
      
    }


  | loot database
  } else /if (${_type.Equal[loot]}) {

    /if (${sqlite.Result[schemanumber 1 version]} >= 1.01) {
      | drop build table
      /sqlite query "${DBloot}" dummy DROP TABLE IF EXISTS build
    }



  | nav database
  } else /if (${_type.Equal[nav]}) {

    /if (${sqlite.Result[schemanumber 1 version]} >= 1.01) {
      | drop build table
      /sqlite query "${DBnav}" dummy DROP TABLE IF EXISTS build
    }


  | shared database
  } else /if (${_type.Equal[shared]}) {


    | agro map change
    /if (${sqlite.Result[schemanumber 1 version]} >= 1.06) {
      | 7
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,${maAgro.Find[swAgro].Value}]}' WHERE grouping='7' AND tag='24';
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,!${maAgro.Find[swAgro].Value}]}' WHERE grouping='7' AND tag='25';
      | 6
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,${maAgro.Find[swAgro].Value}]}' WHERE grouping='6' AND tag='24';
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,!${maAgro.Find[swAgro].Value}]}' WHERE grouping='6' AND tag='25';
      | 5
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,${maAgro.Find[swAgro].Value}]}' WHERE grouping='5' AND tag='24';
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,!${maAgro.Find[swAgro].Value}]}' WHERE grouping='5' AND tag='25';
      | 3
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,${maAgro.Find[swAgro].Value}]}' WHERE grouping='3' AND tag='24';
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,!${maAgro.Find[swAgro].Value}]}' WHERE grouping='3' AND tag='25';
      | 2
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,${maAgro.Find[swAgro].Value}]}' WHERE grouping='2' AND tag='24';
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,!${maAgro.Find[swAgro].Value}]}' WHERE grouping='2' AND tag='25';
      | 1
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,${maAgro.Find[swAgro].Value}]}' WHERE grouping='1' AND tag='24';
      /sqlite query "${DBshared}" dummy UPDATE tag_validate set eval='${Parse[0,!${maAgro.Find[swAgro].Value}]}' WHERE grouping='1' AND tag='25';

    }

    /if (${sqlite.Result[schemanumber 1 version]} >= 1.01) {
      | drop build table
      /sqlite query "${DBshared}" dummy DROP TABLE IF EXISTS build;
    }

  | zone database
  } else /if (${_type.Equal[zone]}) {

    /if (${sqlite.Result[schemanumber 1 version]} >= 1.01) {
      | drop build table
      /sqlite query "${DBzone}" dummy DROP TABLE IF EXISTS build;
    }

 }

/return



|***
 * note: check for stale data base of character DB versioning
 * use:
 ***|
sub sql_schema_stale_check(bool _debug, bool _force)
  DEBUG \atsql_schema_stale_check\ax()

  /declare _metadataFail bool local FALSE

  | get character build version and maintain previous number
  /sqlite query "${DBcharacter}" buildnumber SELECT version FROM metadata WHERE name='build_number';
  /if (!${sql_check[${_debug}, buildnumber, sql_schema_character, "SELECT (buildnumber)"]}) /endmacro
  
  /if (!${sqlite.Rows[buildnumber]}) {
    /return FALSE
  }
  /if (${sqlite.Rows[buildnumber]}) {
    /invoke ${maData.Add[stPreviousBuild,${sqlite.Result[buildnumber 1 version]}]}
  } else {
    /invoke ${maData.Add[stPreviousBuild,0]}
  }

  | get character schema version and maintain previous number
  /sqlite query "${DBcharacter}" schemanumber SELECT version FROM metadata WHERE name='schema_number';
  /if (!${sql_check[${_debug}, schemanumber, sql_schema_character, "SELECT (schemanumber)"]}) /endmacro
  /if (${sqlite.Rows[schemanumber]}) {
    /invoke ${maData.Add[stPreviousSchema,${sqlite.Result[schemanumber 1 version]}]}
  } else {
    /invoke ${maData.Add[stPreviousSchema,0]}
    /varset _metadataFail TRUE
  }

  /declare _schemaMismatch bool local FALSE
  /declare _buildMismatch bool local FALSE

  | macro mismatch flag
  /if (${sqlite.Result[buildnumber 1 version].NotEqual[${maEnv.Find[build].Value}]}) {
    /varset _buildMismatch TRUE
  }

  | schema mismatch flag
  /if (${sqlite.Result[schemanumber 1 version].NotEqual[${maEnv.Find[schema].Value}]}) {
    /varset _schemaMismatch TRUE
  }

  | run stale data deletion on tables
  /if (${_buildMismatch} || ${_schemaMismatch} || ${_force}) {

    OUT \a-tSchema\ax${sep}\a-wstale data check\ax (\a-r${maEnv.Find[schema].Value}\ax)
    /declare _count int local 0
    /declare _countSection int loca 0
    /declare _section string local
    /declare _key string local

    /for _count 1 to ${Math.Calc[${Ini[${INIsqlupdate}].Count[|]} - 1].Int} 
      /varset _section ${Ini["${INIsqlupdate}"].Arg[${_count},|]}
      
      /if (${_force}) {
        OUT \aw[\ax\a-w${_section}\ax\aw]\ax
      }
      
      DEBUG ${sep}\ay${_section}\ax
      /for _countSection 1 to ${Math.Calc[${Ini["${INIsqlupdate}",${_section}].Count[|]} - 1].Int}
        /varset _key ${Ini["${INIsqlupdate}",${_section}].Arg[${_countSection},|]}        
        /call sql_drop ${Ini["${INIsqlupdate}",${_section},${_key}]} ${_key} ${_force}
      /next _countSection
    /next _count

  }

  /sqlite clear schemanumber
  /sqlite clear buildnumber

/return TRUE


